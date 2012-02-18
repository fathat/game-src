#ifndef FRUSTRUM_H
#define FRUSTRUM_H

class Plane
{
public:
	float a, b, c, d;

public:
	Plane();

	void normalize();
	float distanceToPoint(float x, float y, float z );
};



class Frustrum
{
public:
	Plane planes[6];

public:
	Frustrum( );

	void update(  const float* clip_matrix );

	bool sphereInFrustrum( float cx, float cy, float cz, float radius ) const;

};

#endif

