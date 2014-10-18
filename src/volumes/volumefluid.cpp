
/*
    pbrt source code Copyright(c) 1998-2010 Matt Pharr and Greg Humphreys.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */


// volumes/volumefluid.cpp*
#include "stdafx.h"
#include "volumefluid.h"
#include "paramset.h"

// VolumeFluidDensity Method Definitions
VolumeFluid::VolumeFluid(const Transform &v2w, int x, int y, int z, const BBox &e, const float *d, const float *t,
            const float *in, int insz, float inb, const float *op, int opsz, float opb)
: WorldToVolume(Inverse(v2w)), resx(x), resy(y), resz(z), extent(e),
incan_ramp_num(insz), incan_bias(inb), opac_ramp_num(opsz), opac_bias(opb) 
{
    incan_ramp = new float[incan_ramp_num*3];
    memcpy(incan_ramp, in, incan_ramp_num*3*sizeof(float));
    opac_ramp = new float[opac_ramp_num];
    memcpy(opac_ramp, op, opac_ramp_num*sizeof(float));
    density = new float[resx*resy*resz];
    memcpy(density, d, resx*resy*resz*sizeof(float));
    temperature = new float[resx*resy*resz];
    memcpy(temperature, t, resx*resy*resz*sizeof(float));
    minx = maxx = extent.pMin.x;
    miny = maxy = extent.pMin.y;
    minz = maxz = extent.pMin.z;
}

VolumeFluid::~VolumeFluid() 
{ 
    delete[] incan_ramp; 
    delete[] opac_ramp; 
    delete[] density; 
    delete[] temperature;
}

BBox VolumeFluid::WorldBound() const 
{ 
    return Inverse(WorldToVolume)(extent); 
}

bool VolumeFluid::IntersectP(const Ray &r, float *t0, float *t1) const 
{
//printf("INT %g,%g,%g\n",r.o.x,r.o.y,r.o.z);
    Ray ray = WorldToVolume(r);
    return extent.IntersectP(ray, t0, t1);
}

float VolumeFluid::D(int x, int y, int z) const 
{
    x = Clamp(x, 0, resx-1);
    y = Clamp(y, 0, resy-1);
    z = Clamp(z, 0, resz-1);
    return density[z*resx*resy + y*resx + x];
}

float VolumeFluid::T(int x, int y, int z) const 
{
    x = Clamp(x, 0, resx-1);
    y = Clamp(y, 0, resy-1);
    z = Clamp(z, 0, resz-1);
    return temperature[z*resx*resy + y*resx + x];
}

float VolumeFluid::Density(const Point &Pobj) const {
    if (!extent.Inside(Pobj)) return 0;
    // Compute voxel coordinates and offsets for _Pobj_
    Vector vox = extent.Offset(Pobj);
    vox.x = vox.x * resx - .5f;
    vox.y = vox.y * resy - .5f;
    vox.z = vox.z * resz - .5f;
    int vx = Floor2Int(vox.x), vy = Floor2Int(vox.y), vz = Floor2Int(vox.z);
    float dx = vox.x - vx, dy = vox.y - vy, dz = vox.z - vz;

    // Trilinearly interpolate density values to compute local density
    float d00 = Lerp(dx, D(vx, vy, vz),     D(vx+1, vy, vz));
    float d10 = Lerp(dx, D(vx, vy+1, vz),   D(vx+1, vy+1, vz));
    float d01 = Lerp(dx, D(vx, vy, vz+1),   D(vx+1, vy, vz+1));
    float d11 = Lerp(dx, D(vx, vy+1, vz+1), D(vx+1, vy+1, vz+1));
    float d0 = Lerp(dy, d00, d10);
    float d1 = Lerp(dy, d01, d11);
    return Lerp(dz, d0, d1);
}

float VolumeFluid::Temperature(const Point &Pobj) const {
    if (!extent.Inside(Pobj)) return 0;
    // Compute voxel coordinates and offsets for _Pobj_
    Vector vox = extent.Offset(Pobj);
    vox.x = vox.x * resx - .5f;
    vox.y = vox.y * resy - .5f;
    vox.z = vox.z * resz - .5f;
    int vx = Floor2Int(vox.x), vy = Floor2Int(vox.y), vz = Floor2Int(vox.z);
    float tx = vox.x - vx, ty = vox.y - vy, tz = vox.z - vz;

    // Trilinearly interpolate density values to compute local temperature
    float t00 = Lerp(tx, T(vx, vy, vz),     T(vx+1, vy, vz));
    float t10 = Lerp(tx, T(vx, vy+1, vz),   T(vx+1, vy+1, vz));
    float t01 = Lerp(tx, T(vx, vy, vz+1),   T(vx+1, vy, vz+1));
    float t11 = Lerp(tx, T(vx, vy+1, vz+1), T(vx+1, vy+1, vz+1));
    float t0 = Lerp(ty, t00, t10);
    float t1 = Lerp(ty, t01, t11);
    return Lerp(tz, t0, t1);
}

Spectrum VolumeFluid::sigma_a(const Point &p, const Vector &, float) const 
{
    return Spectrum(0.0);
    Point vp = WorldToVolume(p);
    float d = Density(vp);
    float pos2 = getPos(d*20.0,opac_bias);
    float op = MapOpac(pos2);
    return Spectrum (op);
}

Spectrum VolumeFluid::sigma_s(const Point &p, const Vector &, float) const 
{
    return Spectrum(0.0);
}

//Spectrum VolumeFluid::sigma_t(const Point &p, const Vector &, float) const {
//    return Spectrum();
//}

Spectrum VolumeFluid::Lve(const Point &p, const Vector &, float) const {
    Point vp = WorldToVolume(p);
    float t = Temperature(vp);
//    float pos = (t<0.001)?0.0:Clamp(t+incan_bias,0.0,1.0);
    float pos = getPos(t,incan_bias);
    Spectrum incan = MapIncan(pos);
    float rgb[3];
    incan.ToRGB(rgb);
    float d = Density(vp);
//    float pos2 = (d<0.001)?0.0:Clamp(d+opac_bias,0.0,1.0);
    float pos2 = getPos(d*10.0,opac_bias);
    float op = MapOpac(pos2);
//if ((t>0.01)||(d>0.01))
//printf("%g,%g,%g %5.3f/%5.3f/%5.3f(%5.3f,%5.3f,%5.3f) %5.3f/%5.3f/%5.3f=%5.3f\n",p.x,p.y,p.z, incan_bias,t,pos,rgb[0],rgb[1],rgb[2], opac_bias,d,pos2,op);
    return op*incan;
}

float VolumeFluid::p(const Point &p, const Vector &w, const Vector &wp, float) const {
    return 0.0;
}

Spectrum VolumeFluid::tau(const Ray &r, float stepSize, float offset) const {
    float t0, t1;
    float length = r.d.Length();
    if (length == 0.f) return 0.f;
    Ray rn(r.o, r.d / length, r.mint * length, r.maxt * length, r.time);
    if (!IntersectP(rn, &t0, &t1)) return 0.f;
    Spectrum tau(0.);
    t0 += offset * stepSize;
    while (t0 < t1) {
        tau += sigma_t(rn(t0), -rn.d, r.time);
        t0 += stepSize;
    }
    return tau * stepSize;
}

Spectrum VolumeFluid::MapIncan(float pos) const
{
    if (pos==0.0) 
        return Spectrum::FromRGB(incan_ramp);
    else if (pos==1.0)
        return Spectrum::FromRGB(incan_ramp+(incan_ramp_num-1)*3);
    else {
        pos = pos * incan_ramp_num;
        int idx = int(pos);
        float adj = pos - (float)idx;
        float rgb[3];
        rgb[0] = incan_ramp[idx*3] + (incan_ramp[idx*3+3]-incan_ramp[idx*3])*adj;
        rgb[1] = incan_ramp[idx*3+1] + (incan_ramp[idx*3+4]-incan_ramp[idx*3+1])*adj;
        rgb[2] = incan_ramp[idx*3+2] + (incan_ramp[idx*3+5]-incan_ramp[idx*3+2])*adj;
        return Spectrum::FromRGB(rgb);
    }
}

float VolumeFluid::MapOpac(float pos) const
{
    if (pos==0.0) 
        return opac_ramp[0];
    else if (pos==1.0)
        return opac_ramp[opac_ramp_num-1];
    else {
        pos = pos * opac_ramp_num;
        int idx = int(pos);
        float adj = pos - (float)idx;
        return opac_ramp[idx] + (opac_ramp[idx+1]-opac_ramp[idx])*adj;
    }
}

float VolumeFluid::getPos(float pos, float bias) const
{
	if (bias==0.0)
		return pos;
	if ((bias>=1.0) || (pos>=1.0))
		return 1.0;
	if ((bias<=-1.0) || (pos<=0.0))
		return 0.0;

	float B = (bias+1.0)/2.0;
	if (pos<=B) {
		float exp = log(0.5)/log(B);
		return pow(pos,exp);
	} else {
		float exp = log(0.5)/log(1.0-B);
		return (1.0 - pow((1.0-pos),exp));
	}
}

VolumeFluid *CreateFluidVolumeRegion(const Transform &volume2world, const ParamSet &params) {
    // Initialize common volume region parameters

    int resx = params.FindOneInt("resx", 1);
    int resy = params.FindOneInt("resy", 1);
    int resz = params.FindOneInt("resz", 1);
    Point p0 = params.FindOnePoint("p0", Point(0,0,0));
    Point p1 = params.FindOnePoint("p1", Point(1,1,1));

    int incan_ramp_num = params.FindOneInt("incan_ramp_num", 1);
    float incan_bias = params.FindOneFloat("incan_bias", 0.5);
    int opac_ramp_num = params.FindOneInt("opac_ramp_num", 1);
    float opac_bias = params.FindOneFloat("opac_bias", 0.5);

    int nitems;
    const float *incan = params.FindFloat("incan_ramp", &nitems);
    if (!incan) {
        Error("No \"incan_ramp\" values provided for volume fluid?");
        return NULL;
    }
    if (nitems != incan_ramp_num*3) {
        Error("\"incan_ramp\" has %d values but differentfrom \"incan_ramp_num*3\" %d", nitems, incan_ramp_num*3);
        return NULL;
    }
    const float *opac = params.FindFloat("opac_ramp", &nitems);
    if (!opac) {
        Error("No \"opac_ramp\" values provided for volume fluid?");
        return NULL;
    }
    if (nitems != opac_ramp_num) {
        Error("\"opac_ramp\" has %d values but differentfrom \"opac_ramp_num\" %d", nitems, opac_ramp_num);
        return NULL;
    }
    const float *density = params.FindFloat("density", &nitems);
    if (!density) {
        Error("No \"density\" values provided for volume fluid?");
        return NULL;
    }
    if (nitems != (resx*resy*resz)) {
        Error("\"density\" has %d values but resX*Y*Z (%d)", nitems, resx*resy*resz);
        return NULL;
    }
    const float *temperature = params.FindFloat("temperature", &nitems);
    if (!temperature) {
        Error("No \"temperature\" values provided for volume fluid?");
        return NULL;
    }
    if (nitems != (resx*resy*resz)) {
        Error("\"temperature\" has %d values but differentfrom resX*Y*Z (%d)", nitems, resx*resy*resz);
        return NULL;
    }

    return new VolumeFluid(volume2world, resx, resy, resz, BBox(p0, p1), density, temperature, 
        incan, incan_ramp_num, incan_bias, opac, opac_ramp_num, opac_bias);
}


