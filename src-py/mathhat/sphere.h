#ifndef SPHERES_H
#define SPHERES_H

#include <Python.h>
#include <vector>

class Frustrum; //forward dec

enum SphereVisibility { 
	SV_TOTAL   = 2, 
	SV_PARTIAL = 1, 
	SV_NONE    = 0 
};

class VisSphere
{
public:
	float cx, cy, cz;
	float radius;

	SphereVisibility vis;

	PyObject* userRef;

public:
	VisSphere( float x, float y, float z, float rad, PyObject* ref);
	~VisSphere();

	void updateLocation( float x, float y, float z);

	bool visible() { return (vis != SV_NONE) ? true : false; }
};

class SphereVisChecker
{
public:
	std::vector< VisSphere* > spheres;
	
public:
	~SphereVisChecker();

	VisSphere* addSphere( float cx, float cy, float cz, float radius, PyObject* ref);
	void markVisibleSpheres( Frustrum* frustrum );

};


#endif

