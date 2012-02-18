#include "sphere.h"
#include "frustrum.h"


VisSphere::VisSphere( float x, float y, float z, float rad, PyObject* ref):
	cx(x),
	cy(y),
	cz(z),
	radius(rad),
	userRef(ref)
{
	vis = SV_NONE;
	Py_XINCREF( ref );
}

VisSphere::~VisSphere()
{
	Py_XDECREF( userRef );
}

void VisSphere::updateLocation( float x, float y, float z)
{
	x = x;
	y = y;
	z = z;
}

SphereVisChecker::~SphereVisChecker()
{
	size_t sz = spheres.size();
	for(size_t i=0; i<sz; i++ )
	{
		VisSphere* sphere = spheres[i];
		delete sphere;
	}
}


VisSphere* SphereVisChecker::addSphere( float cx, float cy, float cz, float radius, PyObject* ref)
{
	VisSphere* sphere = new VisSphere( cx, cy, cz, radius, ref );
	spheres.push_back( sphere );
	return sphere;
}

void SphereVisChecker::markVisibleSpheres( Frustrum* frustrum )
{
	size_t sz = spheres.size();
	for(size_t i=0; i<sz; i++ )
	{
		VisSphere* sphere = spheres[i];
		if( frustrum->sphereInFrustrum( sphere->cx, sphere->cy, sphere->cz, sphere->radius ) )
			sphere->vis = SV_PARTIAL;
		else
			sphere->vis = SV_NONE;
		
	}
}

