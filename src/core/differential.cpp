/*
 * differential.cpp
 *
 *  Created on: 7/10/2014
 *      Author: remnanjona
 */

#include <iostream>

#include "aggregatetest.h"
#include "differential.h"
#include "emission.h"
#include "image.h"
#include "imageio.h"
#include "adaptive.h"
#include "lowdiscrepancy.h"
#include "samplerrenderer.h"

using namespace std;

void Differential::DiffWriteImage(string fn, float *rgb, int width, int height) {
    // Write RGB image
    WriteImage(fn, rgb, NULL, width, height,
    		width, height, 0, 0);
}

Differential::Differential() {
	maskSamples = 16;
}

Differential::~Differential() {}

/*
 * s1 -- complete scene
 * s2 -- local only
 * s3 -- virtual only
 */
void Differential::process(string fn, Renderer *renderer, Camera *c, Scene *s1, Scene *s2, Scene *s3) {
	if ( !(renderer && s1 && s2) ) {
		return;
	}
	photoImage = ReadImage(fn, &width, &height);


	// create 3 films
	Film *f1 = c->film->clone(width, height);
	Film *f2 = c->film->clone(width, height);
	Film *f3 = c->film->clone(width, height);
	Film *f4 = c->film->clone(width, height);

	ParamSet pixel64;
	int value = maskSamples;
	pixel64.AddInt("pixelsamples", &value, 1);



    // Create mask renderer
    Sampler *sampler = CreateLowDiscrepancySampler(pixel64, f3, c);
    SurfaceIntegrator *surfaceIntegrator = new MaskSurfaceIntegrator();
    VolumeIntegrator *volumeIntegrator = CreateEmissionVolumeIntegrator(ParamSet());
    Renderer *masker1 = new SamplerRenderer(sampler, c, surfaceIntegrator,
                                   volumeIntegrator, false);

    sampler = CreateLowDiscrepancySampler(pixel64, f4, c);
    surfaceIntegrator = new MaskSurfaceIntegrator();
    volumeIntegrator = CreateEmissionVolumeIntegrator(ParamSet());
    Renderer *masker2 = new SamplerRenderer(sampler, c, surfaceIntegrator,
    								volumeIntegrator, false);



    // create outputs
	renderer->RenderToFilm(s1, *f1);
	renderer->RenderToFilm(s2, *f2);


	// rendering with no lighting
	s2->lights.clear();
    s3->lights.clear();

	masker1->RenderToFilm(s3, *f3);
	masker2->RenderToFilm(s2, *f4);
	findDifferential(f1, f2, f3, f4);
}

/*
 * film f1 -- complete scene
 * film f2 -- local only
 * film f3 -- virtual only
 */
void Differential::findDifferential(Film *f1, Film *f2, Film *f3, Film *f4) {
	float *rgb = new float[3 * width * height];

	// temp values
	float rgbF1[3], rgbF2[3], rgbF3[3], rgbF4[3], photoPixels[3];

	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			float *out = &rgb[3*(y * width + x)];
			f1->getPixRGB(x, y, rgbF1, true);
			f2->getPixRGB(x, y, rgbF2, true);
			f3->getPixRGB(x, y, rgbF3, false); // mask of virtual geom
			f4->getPixRGB(x, y, rgbF4, false); // mask of local geom
			photoImage[y * width + x].ToRGB(photoPixels);

			float geomWeight = rgbF3[0] / (float)maskSamples;
			float localWeight = rgbF4[0] / (float)maskSamples;
			if (geomWeight > 1.0f) geomWeight = 1.0f;
			if (localWeight > 1.0f) localWeight = 1.0f;
			float nongeomWeight = 1.0f - geomWeight;
			float nonlocalWeight = 1.0f - localWeight;

			for (int i = 0; i <3; ++i) {
				float d = (rgbF1[i] + 0.01) / (rgbF2[i] + 0.01);
				out[i] = 0.0f;
				out[i] += geomWeight * rgbF1[i];
				out[i] += nonlocalWeight * nongeomWeight * photoPixels[i];
				out[i] += localWeight * nongeomWeight * photoPixels[i] * d;
			}
		}
	}

	DiffWriteImage(f1->getFilename(), rgb, width, height);

    delete[] rgb;
}

RGBSpectrum *Differential::get(RGBSpectrum *s, int x, int y, int width) {
	return &s[y * width + x];
}

