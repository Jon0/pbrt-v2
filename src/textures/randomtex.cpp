/*
 * randomtex.cpp
 *
 *  Created on: 18/09/2014
 *      Author: remnanjona
 */

#include "randomtex.h"

RandomTexture *CreateRandomFloatTexture(const Transform &tex2world,
        const TextureParams &tp) {
	return NULL;
}

RandomTexture *CreateRandomSpectrumTexture(const Transform &tex2world,
        const TextureParams &tp) {

	float density = tp.FindFloat("density", 25.0f);
	float variance = tp.FindFloat("variance", 0.15f);
	float dark = tp.FindFloat("dark", 0.08f);

    // Initialize 3D texture mapping _map_ from _tp_
    TextureMapping3D *map = new IdentityMapping3D(tex2world);
	return new RandomTexture(map, density, variance, dark);
}
