/*
 * octree.h
 *
 *  Created on: 15/09/2014
 *      Author: remnanjona
 */

#ifndef OCTREE_H_
#define OCTREE_H_

#include "pbrt.h"
#include "primitive.h"

typedef vector<Reference<Primitive> > prim_array;

class OctreeNode {
public:
	bool Intersect(const Ray &ray, Intersection *isect) const;

	OctreeNode *c[8];
	BBox bounds;
	prim_array prims;
	bool isLeaf, isEmpty;
	unsigned int size;
};

class OctreeAccel : public Aggregate {
public:
	OctreeAccel(const vector<Reference<Primitive> > &);
	virtual ~OctreeAccel();

	BBox WorldBound() const {
		return root->bounds;
	}

	bool CanIntersect() const {
		return true;
	}

	bool Intersect(const Ray &ray, Intersection *isect) const;
	bool IntersectP(const Ray &ray) const;

private:
	prim_array oct_primitives;
    OctreeNode *root;
    OctreeNode *makeNode(const prim_array &, BBox, unsigned int);
};

OctreeAccel *CreateOctreeAccelerator(const vector<Reference<Primitive> > &prims,
        const ParamSet &ps);


#endif /* OCTREE_H_ */
