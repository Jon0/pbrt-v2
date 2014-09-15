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

OctreeAccel::OctreeAccel(const vector<Reference<Primitive> > &primitives) {
    for (uint32_t i = 0; i < primitives.size(); ++i)
    	primitives[i]->FullyRefine(oct_primitives);

    BBox bounds;
    for (uint32_t i = 0; i < oct_primitives.size(); ++i) {
        BBox b = oct_primitives[i]->WorldBound();
        bounds = Union(bounds, b);
    }
    root = makeNode(bounds, 4);
}

OctreeAccel::~OctreeAccel() {}

bool OctreeAccel::Intersect(const Ray &ray, Intersection *isect) const {
    float tmin, tmax;
    if (!root->bounds.IntersectP(ray, &tmin, &tmax)) {
        return false;
    }

    vector<OctreeNode *> leaves;
    vector<OctreeNode *> search;
    search.push_back(root);
    while (!search.empty()) {
    	OctreeNode *nn = search.back();
    	search.pop_back();

    	if (nn->isEmpty) {
    		continue;
    	}
    	else if (nn->isLeaf) {
    		leaves.push_back(nn);
    	}
    	else {
    		for (int i = 0; i < 8; ++i) {
    			if (nn->c[i]->bounds.IntersectP(ray)) {
    				search.push_back(nn->c[i]);
    			}
    		}
    	}
    }

	for (uint32_t l = 0; l < leaves.size(); ++l) {
		for (uint32_t i = 0; i < leaves[l]->prim_index.size(); ++i) {
			if (oct_primitives[leaves[l]->prim_index[i]]->Intersect(ray, isect)) {
				return true;
			}
		}
	}
    return false;
}

bool OctreeAccel::IntersectP(const Ray &ray) const {
	Intersection *in = new Intersection();
	return Intersect(ray, in);
}

OctreeNode *OctreeAccel::makeNode(BBox b, unsigned int maxDepth) {
	std::cout << maxDepth << std::endl;
	OctreeNode *node = new OctreeNode;
	node->bounds = b;
	if (maxDepth) {
		node->isLeaf = false;
		node->isEmpty = true;
		for (int i = 0; i < 8; ++i) {
			node->c[i] = makeNode(subBoxIndex(b, i), maxDepth - 1);
			node->isEmpty &= node->c[i]->isEmpty;
		}
	}
	else {
		node->isLeaf = true;
	    for (uint32_t i = 0; i < oct_primitives.size(); ++i) {
	        BBox pbb = oct_primitives[i]->WorldBound();
	        if (pbb.Overlaps(b)) {
	    		node->prim_index.push_back(i);
	        }
	    }
	    node->isEmpty = (node->prim_index.size() == 0);
	}
	return node;
}

OctreeAccel *CreateOctreeAccelerator(const vector<Reference<Primitive> > &prims,
        const ParamSet &ps) {
    return new OctreeAccel(prims);
}
