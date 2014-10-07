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

void Differential::DiffWriteImage(string fn, RGBSpectrum *sp) {
    // Convert image to RGB and compute final pixel values
    int nPix = height * width;
    float *rgb = new float[3*nPix];
    int offset = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Convert pixel XYZ color to RGB
        	get(sp, x, y)->ToRGB(&rgb[3*offset]);
            ++offset;
        }
    }

    // Write RGB image
    WriteImage(fn, rgb, NULL, width, height,
    		width, height, 0, 0);

    // Release temporary image memory
    delete[] rgb;
}

Differential::Differential() {}

Differential::~Differential() {}

void Differential::process(Renderer *renderer, Camera *c, Scene *s1, Scene *s2) {
	if ( !(renderer && s1 && s2) ) {
		return;
	}
	string fn = c->film->getFilename();
	width = c->film->xResolution;
	height = c->film->yResolution;


	renderer->Render(s2);
	r2 = ReadImage(fn, &width, &height);
	renderer->Render(s1);
	r1 = ReadImage(fn, &width, &height);
	findDifferential();
}

void Differential::findDifferential() {
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			RGBSpectrum *n = get(r1, x, y);

			*n += *get(r2, x, y);

		}
	}

	DiffWriteImage("test.exr", r1);
}

RGBSpectrum *Differential::get(RGBSpectrum *s, int x, int y) {
	return &s[y * width + x];
}

