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

		Spectrum s;

		float rgb[3];


		rgb[0] = P.x;
		rgb[1] = P.y;
		rgb[2] = P.z;

		return s.FromXYZ(rgb);

		//return s;
    }
private:
	TextureMapping3D *mapping;
};

RandomTexture *CreateRandomFloatTexture(const Transform &tex2world,
        const TextureParams &tp);
RandomTexture *CreateRandomSpectrumTexture(const Transform &tex2world,
        const TextureParams &tp);

#endif /* RANDOMTEX_H_ */
