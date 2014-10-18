
/*
    pbrt source code Copyright(c) 1998-2012 Matt Pharr and Greg Humphreys.

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

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PBRT_VOLUMES_VOLUMEFLUID_H
#define PBRT_VOLUMES_VOLUMEFLUID_H

// volumes/volumefluid.h*
#include "pbrt.h"
#include "volume.h"
#include "geometry.h"
#include "transform.h"

// VolumeFluidDensity Declarations
class VolumeFluid : public VolumeRegion {
public:
    // VolumeFluid Public Methods
    VolumeFluid(const Transform &v2w, int x, int y, int z, const BBox &e, const float *d, const float *t,
            const float *in, int insz, float inb, const float *op, int opsz, float opb);
    ~VolumeFluid();
    BBox WorldBound() const;
    bool IntersectP(const Ray &r, float *t0, float *t1) const;
    Spectrum sigma_a(const Point &p, const Vector &, float) const; 
    Spectrum sigma_s(const Point &p, const Vector &, float) const; 
//    Spectrum sigma_t(const Point &p, const Vector &, float) const; 
    Spectrum Lve(const Point &p, const Vector &, float) const; 
    float p(const Point &p, const Vector &w, const Vector &wp, float) const; 
    Spectrum tau(const Ray &r, float stepSize, float offset) const;
    float Density(const Point &Pobj) const;
    float D(int x, int y, int z) const; 
    float Temperature(const Point &Pobj) const;
    float T(int x, int y, int z) const; 
    Spectrum MapIncan(float pos) const;
    float MapOpac(float pos) const;
    float getPos(float pos, float bias) const;
private:
    // VolumeFluid Private Data
    Transform WorldToVolume;
    const int resx, resy, resz;
    const BBox extent;
    float *density;
    float *temperature;
    float *incan_ramp;
    unsigned int incan_ramp_num;
    float incan_bias;
    float *opac_ramp;	
    unsigned int opac_ramp_num;
    float opac_bias;
    float maxx,maxy,maxz,minx,miny,minz;
};


VolumeFluid *CreateFluidVolumeRegion(const Transform &volume2world,
        const ParamSet &params);

#endif // PBRT_VOLUMES_VOLUMEFLUID_H
