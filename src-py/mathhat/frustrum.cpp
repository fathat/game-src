#include <math.h>
#include "frustrum.h"

Plane::Plane()
{
	a=0;
	b=0;
	c=0;
	d=0;
}

void Plane::normalize()
{
	float inv_length = 1.0f / sqrtf( a*a + b*b + c*c );
	a *= inv_length;
	b *= inv_length;
	c *= inv_length;
	d *= inv_length;
}

float Plane::distanceToPoint(float x, float y, float z )
{
	return 0.0;
}

Frustrum::Frustrum( )
{
}

void Frustrum::update(  const float* clip_matrix )
{
	const float* m = clip_matrix;
	//right
	planes[0].a = m[3]-m[0];
	planes[0].b = m[7]-m[4];
	planes[0].c = m[11]-m[8];
	planes[0].d = m[15]-m[12];
	planes[0].normalize();

	//left
	planes[1].a = m[3]+m[0];
	planes[1].b = m[7]+m[4];
	planes[1].c = m[11]+m[8];
	planes[1].d = m[15]+m[12];
	planes[1].normalize();

	//bottom
	planes[2].a = m[3]+m[1];
	planes[2].b = m[7]+m[5];
	planes[2].c = m[11]+m[9];
	planes[2].d = m[15]+m[13];
	planes[2].normalize();

	//top
	planes[3].a = m[3]-m[1];
	planes[3].b = m[7]-m[5];
	planes[3].c = m[11]-m[9];
	planes[3].d = m[15]-m[13];
	planes[3].normalize();

	//near
	planes[4].a = m[3]+m[2];
	planes[4].b = m[7]+m[6];
	planes[4].c = m[11]+m[10];
	planes[4].d = m[15]+m[14];
	planes[4].normalize();

	//far
	planes[5].a = m[3]-m[2];
	planes[5].b = m[7]-m[6];
	planes[5].c = m[11]-m[10];
	planes[5].d = m[15]-m[14];
	planes[5].normalize();
	
}

bool Frustrum::sphereInFrustrum( float cx, float cy, float cz, float radius ) const
{
	for( int i=0; i<6; i++)
	{
		if( planes[i].a * cx +  planes[i].b * cy + planes[i].c * cz + planes[i].d <= -radius )
			return false;

	}
	return true;
}

