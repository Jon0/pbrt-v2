/*
 * differential.cpp
 *
 *  Created on: 7/10/2014
 *      Author: remnanjona
 */

#include <iostream>

#include "differential.h"
#include "image.h"
#include "imageio.h"

using namespace std;

void Differential::DiffWriteImage(string fn, float *rgb, int width, int height) {
    // Write RGB image
    WriteImage(fn, rgb, NULL, width, height,
    		width, height, 0, 0);
}

Differential::Differential() {}

Differential::~Differential() {}

/*
 * s1 -- complete scene
 * s2 -- local only
 * s3 -- virtual only
 */
void Differential::process(Renderer *renderer, Camera *c, Scene *s1, Scene *s2, Scene *s3) {
	if ( !(renderer && s1 && s2) ) {
		return;
	}
	Film *f1 = c->film->clone();
	Film *f2 = c->film->clone();
	Film *f3 = c->film->clone();
	renderer->RenderToFilm(s1, *f1);
	renderer->RenderToFilm(s2, *f2);
	renderer->RenderToFilm(s3, *f3);
	findDifferential(f1, f2, f3);
}

/*
 * film f1 -- complete scene
 * film f2 -- local only
 * film f3 -- virtual only
 */
void Differential::findDifferential(Film *f1, Film *f2, Film *f3) {
	int width, height;
	RGBSpectrum *photoImage = ReadImage("in.exr", &width, &height);

	//int width = f1->xResolution;
	//int height = f1->yResolution;
	float *rgb = new float[3 * width * height];

	// temp values
	float rgbF1[3], rgbF2[3], rgbF3[3];

	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			float *out = &rgb[3*(y * width + x)];

			f1->getPixRGB(x, y, rgbF1);
			f2->getPixRGB(x, y, rgbF2);
			f3->getPixRGB(x, y, rgbF3);

			// depth values
			//float d1 = f1->getPixDepth(x, y);
			//float d2 = f1->getPixDepth(x, y);

			float photoPixels[3];
			photoImage[y * width + x].ToRGB(photoPixels);

			float geomWeight = 0.0f;
			float localWeight = 0.0f;
			if (rgbF3[0] > 0.0) {
				geomWeight = 1.0f;
			}
			else {
				localWeight = 1.0f;
			}
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

	DiffWriteImage("test.exr", rgb, width, height);

    delete[] rgb;
}

RGBSpectrum *Differential::get(RGBSpectrum *s, int x, int y, int width) {
	return &s[y * width + x];
}

