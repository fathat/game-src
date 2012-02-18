#include "collisionutils.h"
#include <d3dx9.h>


#include "..\defs\defs.h"

Vector3D CollisionUtil::ClosestPointOnLine( Vector3D& l1, Vector3D& l2, Vector3D& p1 )
{
	Vector3D D = p1 - l1;
	Vector3D direction = l2 - l1;

	float e2 = direction.Dot(direction);
	float ed = direction.Dot(D);

	float t = ed / e2;

	if (t < 0.0f) t = 0.0f; else if (t > 1.0f) t = 1.0f;

	Vector3D ClosestPoint = l1 + (direction * t);

	return ClosestPoint;
}

bool CollisionUtil::LineIntersectSphere(Vector3D &l1, Vector3D& l2, Vector3D& center, float radius )
{
	Vector3D raydir = (l2-l1).Normalize();
	Vector3D invraydir = raydir * -1.0f;
	//Check if the ray of point1 to point2 intersects the circle
	bool RayIntersects = D3DXSphereBoundProbe( (D3DXVECTOR3*)&center, radius, (D3DXVECTOR3*)&l1, (D3DXVECTOR3*)&raydir);
	if(!RayIntersects)
	{
		return false;
	}

	//check if the opposite ray from point2 to point1 intsersects the circle
	RayIntersects = D3DXSphereBoundProbe( (D3DXVECTOR3*)&center, radius, (D3DXVECTOR3*)&l2, (D3DXVECTOR3*)&invraydir);
	if(!RayIntersects)
	{
		return false;
	}

	return true; 

}

