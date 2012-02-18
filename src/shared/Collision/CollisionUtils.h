#pragma once
#include "..\s3dlib\types.h"

class CollisionUtil
{
public:
	static Vector3D ClosestPointOnLine( Vector3D& l1, Vector3D& l2, Vector3D& p1 );
	static bool LineIntersectSphere(Vector3D &l1, Vector3D& l2, Vector3D& center, float radius );
};
