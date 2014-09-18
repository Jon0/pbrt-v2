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

    // Initialize 3D texture mapping _map_ from _tp_
    TextureMapping3D *map = new IdentityMapping3D(tex2world);
	return new RandomTexture(map);
}
