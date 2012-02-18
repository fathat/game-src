#include <math.h>
#include "MathTypes.h"

Real Vector3D::Dot ( const Vector3D &v ) const
{
	return x*v.x + y*v.y + z*v.z;
}

Vector3D Vector3D::Cross ( const Vector3D &v ) const
{
	return Vector3D( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x );
}

Real Vector3D::Mag () const
{
	return sqrt( x*x + y*y + z*z);
}

Real Vector3D::MagSq() const
{
	return x*x + y*y + z*z;
}

Vector3D Vector3D::Normalized() {
	Real m = Mag();
	return Vector3D(x/m, y/m, z/m);
}

float Vector3D::operator [] ( int index )
{
	switch(index) {
		case 0:
			return x;
			break;
		case 1:
			return y;
			break;
		case 2:
			return z;
			break;
	}
	return -1;
}


Vector3D Vector3D::operator - ( const Vector3D &V ) const
{
	return Vector3D(x - V.z, y - V.y, z - V.z);
}

Vector3D Vector3D::operator + ( const Vector3D &V ) const
{
	return Vector3D(x + V.z, y + V.y, z + V.z);
}

Vector3D Vector3D::operator * ( const Vector3D &V ) const
{
	return Vector3D(x * V.z, y * V.y, z * V.z);
}

Vector3D Vector3D::operator / ( const Vector3D &V ) const
{
	return Vector3D(x / V.z, y / V.y, z / V.z);
}

void Vector3D::operator += ( const Vector3D &V ) 
{
	x += V.x;
	y += V.y;
	z += V.z;
}

void Vector3D::operator -= ( const Vector3D &V ) 
{
	x -= V.x;
	y -= V.y;
	z -= V.z;
}

Vector3D Vector3D::operator *  ( const Real S ) const
{
	return Vector3D(x * S, y * S, z * S);
}

Vector3D Vector3D::operator /  ( const Real S ) const
{
	return Vector3D(x / S, y / S, z / S);
}

void Vector3D::operator *= ( const Real S )
{
	x *= S;
	y *= S;
	z *= S;
}

void Vector3D::operator /= ( const Real S )
{
	x /= S;
	y /= S;
	z /= S;
}

void Vector3D::operator += ( const Real S )
{
	x += S;
	y += S;
	z += S;
}

void Vector3D::operator -= ( const Real S )
{
	x -= S;
	y -= S;
	z -= S;
}

Plane::Plane()
{
	a = b = c = d = 0;
}

Plane::Plane( float a, float b, float c, float d )
{
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

Plane::Plane(const Vector3D *triangle) 
{
	//figure out triangle normal
	Vector3D normal = Plane::TriangleNormal(triangle);
	Real neg_d = normal.Dot((triangle[0]));
	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = -neg_d;
}


Vector3D Plane::TriangleNormal( const Vector3D* vertices) 
{
	Vector3D i = vertices[1]-vertices[0];
	Vector3D j = vertices[2]-vertices[0];
	return i.Cross(j);
}

bool Plane::IntersectLineWithTriangle( const Vector3D* vertices, const Vector3D& p1, const Vector3D& p2, Vector3D* result)
{
	Vector3D normal = Plane::TriangleNormal(vertices);
	return false;
}

bool Plane::IntersectLine(const Vector3D& p1, const Vector3D& p2, Vector3D* result)
{
	if(result == 0) return false;
	Real num = (a*p1.x + b*p1.y + c*p1.z + d);
	Real denom = (a*(p1.x-p2.x) + b*(p1.y-p2.y) + c*(p1.z-p2.z));
	if(denom < 0.000001) return false;
	Real u = num/denom;
	*result = p1 + (p2-p1)*u;
	return true;
}