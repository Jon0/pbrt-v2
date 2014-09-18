/*
 * octree.cpp
 *
 *  Created on: 15/09/2014
 *      Author: remnanjona
 */

#include <iostream>

#include "intersection.h"
#include "octreeaccel.h"

/*
 * map integers 0-7 to 8 octree directions
 */
Point dirIndex(int i) {
	Point p;
	p.x = i % 2 == 0? 1 : 0;
	p.y = i/2 % 2 == 0? 1 : 0;
	p.z = i/4 % 2 == 0? 1 : 0;
	return p;
}

int dirIndex(Vector &v) {
	int i = 0;
	if (v.x > 0) i += 1;
	if (v.y > 0) i += 2;
	if (v.z > 0) i += 4;
	return i;
}

/*
 * create a divided bbox
 */
BBox subBoxIndex(BBox b, int i) {
	Vector halfsize = (b.pMax - b.pMin) / 2.0f;
	Point p = dirIndex(i);
	p.x *= halfsize.x;
	p.y *= halfsize.y;
	p.z *= halfsize.z;

	Point mid = 0.5 * b.pMin + 0.5 * b.pMax;
	BBox newb;
	newb.pMin = b.pMin + p;
	newb.pMax = mid + p;
	return newb;
}

bool OctreeNode::Intersect(const Ray &ray, Intersection *isect) const {
    float tmin, tmax;
    if (isEmpty || !bounds.IntersectP(ray, &tmin, &tmax)) {
        return false;
    }
	if (isLeaf) {
		for (const auto &p: prims) {
			if (p->Intersect(ray, isect)) {
				return true;
			}
		}
	}
	else {
		for (OctreeNode *child: c) {
			if (child->Intersect(ray, isect)) {
				return true;
			}
		}
	}
	return false;
}

OctreeAccel::OctreeAccel(const vector<Reference<Primitive> > &primitives) {
    for (uint32_t i = 0; i < primitives.size(); ++i)
    	primitives[i]->FullyRefine(oct_primitives);

    BBox bounds;
    for (uint32_t i = 0; i < oct_primitives.size(); ++i) {
        BBox b = oct_primitives[i]->WorldBound();
        bounds = Union(bounds, b);
    }
    root = makeNode(bounds, 8);
}

OctreeAccel::~OctreeAccel() {}

bool OctreeAccel::Intersect(const Ray &ray, Intersection *isect) const {
    return root->Intersect(ray, isect);
}

bool OctreeAccel::IntersectP(const Ray &ray) const {
	Intersection *in = new Intersection();
	return Intersect(ray, in);
}

OctreeNode *OctreeAccel::makeNode(BBox b, unsigned int maxDepth) {
	if (maxDepth >= 5) {
		std::cout << "depth = " << maxDepth << std::endl;
		//std::cout << b.pMin.x << ", " << b.pMin.y << ", " << b.pMin.z << std::endl;
		//std::cout << b.pMax.x << ", " << b.pMax.y << ", " << b.pMax.z << std::endl;
	}
	OctreeNode *node = new OctreeNode;
    node->size = 0;
	node->bounds = b;

	// find content in the node
	int content = 0;
    for (const auto &p: oct_primitives) {
        if (b.Overlaps( p->WorldBound() )) {
        	content++;
        }
    }

	if (maxDepth && content > 16) {
	    // create 8 children
		node->isLeaf = false;
		// does the node contain anything
		node->isEmpty = false;
		for (int i = 0; i < 8; ++i) {
			OctreeNode *child = makeNode(subBoxIndex(b, i), maxDepth - 1);
			node->c[i] = child;
			node->size += child->size;
		}
	}
	else {
		node->isLeaf = true;
	    for (const auto &p: oct_primitives) {
	        if (b.Overlaps(p->WorldBound())) {
	    		node->prims.push_back(p);
	        }
	    }
	    node->isEmpty = (node->prims.size() == 0);
	    node->size = 1;
	}


	if (maxDepth >= 5) {
		std::cout << "contains " << node->size << " nodes" << std::endl;
	}
	return node;
}

OctreeAccel *CreateOctreeAccelerator(const vector<Reference<Primitive> > &prims,
        const ParamSet &ps) {
    return new OctreeAccel(prims);
}
