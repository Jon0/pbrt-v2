/*
 * randomtex.h
 *
 *  Created on: 18/09/2014
 *      Author: remnanjona
 */

#ifndef RANDOMTEX_H_
#define RANDOMTEX_H_

#include "pbrt.h"
#include "texture.h"
#include "paramset.h"

class RandomTexture : public Texture<Spectrum> {
public:
	RandomTexture(TextureMapping3D *m, float dense, float var, float dr) {
		mapping = m;

		// settings
		density = dense;
		variance = var;
		dark = dr;

		// colours
        R = Point(204.0/255.0, 98.0/255.0, 86.0/255.0);
        G = Point(95.0/255.0, 248.0/255.0, 122.0/255.0);
        B = Point(89.0/255.0, 102.0/255.0, 249.0/255.0);
	}

	virtual ~RandomTexture() {}

	Spectrum Evaluate(const DifferentialGeometry &dg) const {
        Vector dpdx, dpdy;
        Point P = mapping->Map(dg, &dpdx, &dpdy);

        float ft = variance + 1.0f + sin( density * FBm(P, dpdx, dpdy, 0.8, 7) ) * variance;
        float fu = (density + pow(ft, 9)) * FBm(P, dpdx, dpdy, 0.66, 9) * 1;
        float fv = (density + pow(ft, 9)) * FBm(P, dpdx, dpdy, 0.57, 16) * 3;
        float x = dark + fabs(sin(fu)) * (1-dark);

        //  make out of phase sine waves
        float fr = 0.45 + cos(fv) * 0.44;
        float fg = (0.5 + cos(fv + 2.0f) * 0.48) * (1-fr);
        float fb = sqrt(1 - (fr*fr + fg*fg));
        float trate = 8.0f;
        Point Q = (pow(fr, trate)*R + pow(fg, trate)*G + pow(fb, trate)*B) * x * 2.5;

		float rgb[3];
		rgb[0] = Q.x;
		rgb[1] = Q.y;
		rgb[2] = Q.z;
		Spectrum s;
		return s.FromRGB(rgb);
    }
private:
	float density, variance, dark;
    Point R;
    Point G;
    Point B;


	TextureMapping3D *mapping;
};

RandomTexture *CreateRandomFloatTexture(const Transform &tex2world,
        const TextureParams &tp);
RandomTexture *CreateRandomSpectrumTexture(const Transform &tex2world,
        const TextureParams &tp);

#endif /* RANDOMTEX_H_ */
