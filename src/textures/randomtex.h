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
	RandomTexture(TextureMapping3D *m) {
		mapping = m;
	}
	virtual ~RandomTexture() {}

	Spectrum Evaluate(const DifferentialGeometry &dg) const {
        Vector dpdx, dpdy;
        Point P = mapping->Map(dg, &dpdx, &dpdy);

        float density = 25.0f;
        float ft = 1.15f + sin( density * FBm(P, dpdx, dpdy, 0.8, 7) ) * 0.15;
        float fu = (density + pow(ft, 9)) * FBm(P, dpdx, dpdy, 0.66, 9) * 1;
        float fv = (density + pow(ft, 9)) * FBm(P, dpdx, dpdy, 0.57, 16) * 3;
        float x = 0.08 + fabs(sin(fu)) * 0.92;

        Point R = Point(204.0/255.0, 98.0/255.0, 86.0/255.0);
        Point G = Point(95.0/255.0, 248.0/255.0, 122.0/255.0);
        Point B = Point(89.0/255.0, 102.0/255.0, 249.0/255.0);

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
		//return s.FromXYZ(rgb);
    }
private:
	TextureMapping3D *mapping;
};

RandomTexture *CreateRandomFloatTexture(const Transform &tex2world,
        const TextureParams &tp);
RandomTexture *CreateRandomSpectrumTexture(const Transform &tex2world,
        const TextureParams &tp);

#endif /* RANDOMTEX_H_ */
