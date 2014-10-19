/*
 * differential.h
 *
 *  Created on: 7/10/2014
 *      Author: remnanjona
 */

#ifndef DIFFERENTIAL_H_
#define DIFFERENTIAL_H_

#include "camera.h"
#include "integrator.h"
#include "renderer.h"
#include "scene.h"
#include "volume.h"

class MaskSurfaceIntegrator: public SurfaceIntegrator {
public:
	MaskSurfaceIntegrator() {}
    Spectrum Li(const Scene *scene, const Renderer *renderer,
        const RayDifferential &ray, const Intersection &isect,
        const Sample *sample, RNG &rng, MemoryArena &arena) const {
    	return Spectrum(1.0f);
    }
};

class MaskVolumeIntegrator: public VolumeIntegrator {
public:
	MaskVolumeIntegrator() {}
    Spectrum Li(const Scene *scene, const Renderer *renderer,
        const RayDifferential &ray, const Sample *sample, RNG &rng,
        Spectrum *transmittance, MemoryArena &arena) const {
    	return Spectrum(0.0f);
    }

    Spectrum Transmittance(const Scene *scene,
        const Renderer *renderer, const RayDifferential &ray,
        const Sample *sample, RNG &rng, MemoryArena &arena) const {
    	return Spectrum(1.0f);
    }
};

class Differential {
public:
	Differential();
	virtual ~Differential();


	void saveWithMask(Renderer *renderer, Camera *c, Scene *s1);
	void process(string, string, string, Renderer *, Camera *, Scene *, Scene *, Scene *);


private:
	void DiffWriteImage(string, float *, int width, int height);
	void findDifferential(Film *, Film *, Film *, Film *f4);
	void findDifferential(RGBSpectrum *, RGBSpectrum *, Film *, Film *);
	void get(RGBSpectrum *s, int x, int y, int width, float rgb[3]) ;
	RGBSpectrum *get(RGBSpectrum *, int, int, int);
	RGBSpectrum *photoImage;
	int width, height;
	int maskSamples;
};

#endif /* DIFFERENTIAL_H_ */
