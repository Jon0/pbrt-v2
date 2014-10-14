/*
 * differential.h
 *
 *  Created on: 7/10/2014
 *      Author: remnanjona
 */

#ifndef DIFFERENTIAL_H_
#define DIFFERENTIAL_H_

#include "camera.h"
#include "renderer.h"
#include "scene.h"

class Differential {
public:
	Differential();
	virtual ~Differential();

	void process(Renderer *, Camera *, Scene *, Scene *);

private:
	void DiffWriteImage(string, float *, int width, int height);
	void findDifferential(Film *, Film *);
	RGBSpectrum *get(RGBSpectrum *, int, int, int);
};

#endif /* DIFFERENTIAL_H_ */
