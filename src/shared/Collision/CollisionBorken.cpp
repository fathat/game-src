#include "collision.h"


bool CollisionMesh::AddTriangles( Vector3D* v, int nTriangles )
{
	int VertexCount = nTriangles*3;
	int meshindex = MeshList.size()-1;

	for(int i=0; i<VertexCount; i++)
		TempVertexList.push_back( v[i] );

	return true;
}

void CollisionMesh::NextGroup()
{
	if(MeshList.size())
	{
		//Copy the vertex data into the final buffer
		MeshList[MeshIndex].VertexList = new Vector3D[TempVertexList.size()];
		MeshList[MeshIndex].nVertices = TempVertexList.size();

		for(int i=0; i<MeshList[MeshIndex].nVertices; i++)
			MeshList[MeshIndex].VertexList[i] = TempVertexList[i];

		TempVertexList.clear();
	}
	

	//create a new group and place it in the list
	CollisionMeshGroup CMG;
	MeshList.push_back( CMG );

	MeshIndex++;

}

void CollisionMesh::FindBoundingSphere( int meshindex)
{
	//Make certain that we're using a valid index
	if(meshindex >= MeshList.size())
		return;

	//Find the center of the object.
	Real sumx=0, sumy=0, sumz=0;
	int i;
	for(i=0; i<MeshList[meshindex].nVertices; i++)
	{
		sumx += MeshList[meshindex].VertexList[i].x;
		sumy += MeshList[meshindex].VertexList[i].y;
		sumz += MeshList[meshindex].VertexList[i].z;		
	}

	MeshList[meshindex].x = sumx/(Real)i;
	MeshList[meshindex].y = sumy/(Real)i;
	MeshList[meshindex].z = sumz/(Real)i;


	//Next, find the radius
	Vector3D LengthVector;
	Vector3D CenterVector;

	CenterVector.x = sumx/i;
	CenterVector.y = sumy/i;
	CenterVector.z = sumz/i;

	Real Length=0;
	for(i=0; i<MeshList[meshindex].nVertices; i++)
	{
		LengthVector = MeshList[meshindex].VertexList[i]-CenterVector;

		if(LengthVector.Mag() > Length)
			Length = LengthVector.Mag();
	}

	MeshList[meshindex].Radius = Length;
	
	

}
void CollisionMesh::Finalize()
{
	for(int m=0;m<MeshList.size(); m++)
	{
		
		//calculate bounding sphere info for the group
		FindBoundingSphere( m );
	}

}

bool CollisionMesh::SphereCollision( Position &MeshLocation, Vector3D& location, Real radius, CollisionInfo* Collision)
{


	//Declarations

	//Make a buffer, and set its distance to something that's
	//greater than any other distance will be.
	CollisionInfo CollisionBuffer;
	CollisionBuffer.DistanceToPlane = 400000000;

	//Other variables for checking
	Vector3D CollisionVertex[3]; //Vertices of a triangle
	Vector3D Result, SphereLocation, SphereVector; //Used for checking the bounding sphere
												   //-- not the sphere for collisions
	Vector3D TestLocation = location; //location where sphere is assumed to be
	bool FoundCollision=false; //records if it's hit anything

	for(int m=0;m<MeshList.size(); m++)
	{


		//Test to see if the bounding spheres even meet, if 
		//they don't there's not even a possibility for 
		//a collision
		SphereLocation.x = MeshList[m].x;
		SphereLocation.y = MeshList[m].y;
		SphereLocation.z = MeshList[m].z;

		SphereLocation.Rotatey( -MeshLocation.Yaw);

		SphereVector.x = SphereLocation.x+MeshLocation.x;
		SphereVector.y = SphereLocation.y+MeshLocation.y;
		SphereVector.z = SphereLocation.z+MeshLocation.z;

		Result = SphereVector - location;

		if(Result.Mag() > MeshList[m].Radius)
			continue; //Not intersection, go to next mesh


		//If the bounding spheres met, test each triangle
		//against our sphere
		for(int i=0; i<MeshList[m].nVertices; i+=3)
		{

			//First find the triangle and translate it to where
			//we need it
			CollisionVertex[0] = MeshList[m].VertexList[i+0];
			CollisionVertex[1] = MeshList[m].VertexList[i+1];
			CollisionVertex[2] = MeshList[m].VertexList[i+2];

			CollisionVertex[0].Rotatey( -MeshLocation.Yaw );
			CollisionVertex[1].Rotatey( -MeshLocation.Yaw );
			CollisionVertex[2].Rotatey( -MeshLocation.Yaw );
			

			CollisionVertex[0].x += MeshLocation.x;
			CollisionVertex[0].y += MeshLocation.y;
			CollisionVertex[0].z += MeshLocation.z;

			CollisionVertex[1].x += MeshLocation.x;
			CollisionVertex[1].y += MeshLocation.y;
			CollisionVertex[1].z += MeshLocation.z;

			CollisionVertex[2].x += MeshLocation.x;
			CollisionVertex[2].y += MeshLocation.y;
			CollisionVertex[2].z += MeshLocation.z;


			//Set the first half of the collision vector to
			//the location of the center of the sphere.
			Vector3D Line[2];
			Line[0] = (Vector3D)TestLocation;

			//Find out the proper heading for the vector so that it's parallel
			//to the triangle's normal
			D3DXPLANE TrianglePlane;
			D3DXPlaneFromPoints( &TrianglePlane, (D3DXVECTOR3*)&CollisionVertex[0], 
				(D3DXVECTOR3*)&CollisionVertex[1], 
				(D3DXVECTOR3*)&CollisionVertex[2]);
			Line[1].x = (-TrianglePlane.a * radius);
			Line[1].y = (-TrianglePlane.b * radius);
			Line[1].z = (-TrianglePlane.c * radius);
			Line[1] = Line[0] + Line[1];
			

			// Check if the pick ray passes through this point
			if( LineIntersectsTriangle(Line, CollisionVertex,  Collision) )
			{
				Collision->Force = Line[1]-Line[0];
				FoundCollision = true;
				
			
				Collision->SuggestedNewLocation.x=Collision->CollisionPoint.x+(Collision->PlaneNormal.x*radius);
				Collision->SuggestedNewLocation.y=Collision->CollisionPoint.y+(Collision->PlaneNormal.y*radius);
				Collision->SuggestedNewLocation.z=Collision->CollisionPoint.z+(Collision->PlaneNormal.z*radius);
				
				
				if(Collision->DistanceToPlane < CollisionBuffer.DistanceToPlane )
					CollisionBuffer = *Collision;
			}
		}
	}
	
	if(FoundCollision)
	{
		*Collision = CollisionBuffer;
		return true;
	}

	return false;
	

}

bool CollisionMesh::Collision( Position &MeshLocation, Vector3D& point1, Vector3D& point2, CollisionInfo* Collision )
{


	CollisionInfo CollisionBuffer;
	CollisionBuffer.DistanceToPlane = 400000000;
	Vector3D CollisionVertex[3];
	Vector3D Result, SphereLocation, SphereVector;
	Vector3D RayDir = point2-point1;

	bool FoundCollision=false;
	Vector3D TriangleCenter;

	for(int m=0;m<MeshList.size(); m++)
	{
		RayDir = point2-point1;

		SphereLocation.x = MeshList[m].x;
		SphereLocation.y = MeshList[m].y;
		SphereLocation.z = MeshList[m].z;

		SphereLocation.Rotatey( -MeshLocation.Yaw);

		SphereVector.x = SphereLocation.x+MeshLocation.x;
		SphereVector.y = SphereLocation.y+MeshLocation.y;
		SphereVector.z = SphereLocation.z+MeshLocation.z;

		Result = SphereVector - (point1+RayDir);

		
		bool RayIntersects = D3DXSphereBoundProbe( (D3DXVECTOR3*)&SphereVector, MeshList[m].Radius, (D3DXVECTOR3*)&point1, (D3DXVECTOR3*)&RayDir);

		if(RayIntersects)
		{
			if(Result.Mag() > MeshList[m].Radius)
			{
						

				continue;
			}
		}
		else
		{
			continue;
		}

		for(int i=0; i<MeshList[m].nVertices; i+=3)
		{

			CollisionVertex[0] = MeshList[m].VertexList[i+0];
			CollisionVertex[1] = MeshList[m].VertexList[i+1];
			CollisionVertex[2] = MeshList[m].VertexList[i+2];

			CollisionVertex[0].Rotatey( -MeshLocation.Yaw );
			CollisionVertex[1].Rotatey( -MeshLocation.Yaw );
			CollisionVertex[2].Rotatey( -MeshLocation.Yaw );
			

			CollisionVertex[0].x += MeshLocation.x;
			CollisionVertex[0].y += MeshLocation.y;
			CollisionVertex[0].z += MeshLocation.z;

			CollisionVertex[1].x += MeshLocation.x;
			CollisionVertex[1].y += MeshLocation.y;
			CollisionVertex[1].z += MeshLocation.z;

			CollisionVertex[2].x += MeshLocation.x;
			CollisionVertex[2].y += MeshLocation.y;
			CollisionVertex[2].z += MeshLocation.z;

			/*TriangleCenter = CollisionVertex[0] + CollisionVertex[1] + CollisionVertex[2];
			TriangleCenter *= 1.0f/3.0f;

			Vector3D DistanceVector;

			DistanceVector = CollisionVertex[0]-TriangleCenter;
			Real Distance1 = DistanceVector.Mag();
			DistanceVector = CollisionVertex[1]-TriangleCenter;
			Real Distance2 = DistanceVector.Mag();
			DistanceVector = CollisionVertex[2]-TriangleCenter;
			Real Distance3 = DistanceVector.Mag();

			Real Radius;

			if(Distance3>Distance1 && Distance3>Distance2)
				Radius=Distance3;
			else if(Distance2>Distance1 && Distance2>Distance3)
				Radius=Distance2;
			else
				Radius=Distance1;*/

			Vector3D Line[2];
			Line[0] = point1;
			Line[1] = point2;


			// Check if the pick ray passes through this point
			//if( D3DXSphereBoundProbe( (D3DXVECTOR3*)&TriangleCenter, Radius, (D3DXVECTOR3*)&Line[0], (D3DXVECTOR3*)&(Line[1]-Line[0])) )
			//{


			D3DXVECTOR3 v1, v2, v3;
			v1 = CollisionVertex[0];
			v2 = CollisionVertex[1];
			v3 = CollisionVertex[2];
			
			Real LineLength = RayDir.Mag();
			RayDir.Normalize();
			Real U, V, Distance;


			if( D3DXIntersectTri ( (D3DXVECTOR3*)&CollisionVertex[0], (D3DXVECTOR3*)&CollisionVertex[1], (D3DXVECTOR3*)&CollisionVertex[2],
				(D3DXVECTOR3*)&point1, (D3DXVECTOR3*)&RayDir, &U, &V, &Distance ) )
			{

				//Get force, collision point, and distance of collision
				Collision->Force = Line[1]-Line[0];
				Collision->CollisionPoint = v1 + (v2-v1)*U + (v3-v1)*V;
				Collision->DistanceToPlane = Distance;

				//Find plane normal here
				D3DXPLANE Plane;
				D3DXPlaneFromPoints( &Plane, &v1, &v2, &v3 );
				Collision->PlaneNormal.x=Plane.a;
				Collision->PlaneNormal.y=Plane.b;
				Collision->PlaneNormal.z=Plane.c;

				//If this is closest collision, set the buffer equal to it.
				if(Collision->DistanceToPlane < CollisionBuffer.DistanceToPlane)
					CollisionBuffer = *Collision;

				//Hooray! We found a collision!
				FoundCollision = true;

			}


			/*	if( LineIntersectsTriangle(Line, CollisionVertex,  Collision) )
				{
					Collision->Force = Line[1]-Line[0];
					FoundCollision = true;


					if(Collision->DistanceToPlane < CollisionBuffer.DistanceToPlane )
						CollisionBuffer = *Collision;
				}*/
			//}
		}
	}
	
	if(FoundCollision)
	{
		*Collision = CollisionBuffer;
		return true;
	}

	return false;
}



void CollisionMesh::CombineWith( CollisionMesh& CmbMesh, CollisionMesh* OutMesh, Real Percent)
{
	//return if they gave us null
	if(!OutMesh)
		return;

	//return if there is a different number of meshes in the output mesh
	if( CmbMesh.MeshList.size() != MeshList.size())
		return;

	if( OutMesh->MeshList.size() != MeshList.size() )
	{
		return;
	}
	
	Real StartX, StartY, StartZ;
	Real EndX, EndY, EndZ;
	Real ChangeX, ChangeY, ChangeZ;
	for(int m=0; m<MeshList.size(); m++)
	{
		//Interpolate the vertices O_O!
		for(int i=0; i<OutMesh->MeshList[m].nVertices; i++)
		{
			StartX = MeshList[m].VertexList[i].x;
			StartY = MeshList[m].VertexList[i].y;
			StartZ = MeshList[m].VertexList[i].z;

			EndX = CmbMesh.MeshList[m].VertexList[i].x;
			EndY = CmbMesh.MeshList[m].VertexList[i].y;
			EndZ = CmbMesh.MeshList[m].VertexList[i].z;

			ChangeX = (EndX-StartX) * Percent;
			ChangeY = (EndY-StartY) * Percent;
			ChangeZ = (EndZ-StartZ) * Percent;

					
			OutMesh->MeshList[m].VertexList[i].x = MeshList[m].VertexList[i].x + ChangeX;
			OutMesh->MeshList[m].VertexList[i].y = MeshList[m].VertexList[i].y + ChangeY;
			OutMesh->MeshList[m].VertexList[i].z = MeshList[m].VertexList[i].z + ChangeZ;
			
		}
	}
		
}



bool RayIntersectsTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,    
                                       FLOAT* t, FLOAT* u, FLOAT* v )
{
    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;
	
    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    if( det < 0.0001f )
        return false;

    // Calculate distance from vert0 to ray origin
    D3DXVECTOR3 tvec = orig - v0;

    // Calculate U parameter and test bounds
    *u = D3DXVec3Dot( &tvec, &pvec );
    if( *u < 0.0f || *u > det )
        return false;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    *v = D3DXVec3Dot( &dir, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return false;

    // Calculate t, scale parameters, ray intersects triangle
    *t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return true;
}

bool RayIntersectsTriangle ( Vector3D& orig, Vector3D& dir, Vector3D* vertex_list, Real* t, Real* u, Real* v)
{
	return RayIntersectsTriangle((D3DXVECTOR3&)orig, (D3DXVECTOR3&)dir, (D3DXVECTOR3&)vertex_list[0], (D3DXVECTOR3&)vertex_list[1], (D3DXVECTOR3&)vertex_list[2], t, u, v);

}

bool LineCrossesPlane(  D3DXPLANE& Plane, Vector3D& point1, Vector3D& point2 )
{
	Real Sign1, Sign2;


	Sign1 = (Plane.a * point1.x + Plane.b * point1.y + Plane.c * point1.z + Plane.d );
	Sign2 = ( Plane.a * point2.x + Plane.b * point2.y + Plane.c * point2.z + Plane.d );

	if(Sign1 * Sign2 >= 0.0f)
		return false;

	return true;

}



bool LineIntersectsTriangle ( Vector3D* Line, Vector3D* vertex_list, CollisionInfo* col_info )
{
	D3DXPLANE TrianglePlane;
	D3DXPlaneFromPoints( &TrianglePlane, (D3DXVECTOR3*)&vertex_list[0], (D3DXVECTOR3*)&vertex_list[1], (D3DXVECTOR3*)&vertex_list[2]);
	//TrianglePlane.d -= 3;

	if(LineCrossesPlane ( TrianglePlane, Line[0], Line[1]) )
	{
		Vector3D DirVector;
		DirVector = Line[1] - Line[0];
		DirVector.Normalize();

		
		Real t, u, v;

        if(RayIntersectsTriangle(Line[0], DirVector, vertex_list, &t, &u, &v) )
		{

			//Since it does intersect the triangle, find out *where*
			if(col_info)
			{
				D3DXVECTOR3 iPoint;
				D3DXPlaneIntersectLine(&iPoint, &TrianglePlane, (const D3DXVECTOR3*)&Line[0], (const D3DXVECTOR3*)&Line[1]);
				col_info->CollisionPoint = iPoint;
				col_info->PlaneNormal.x  = TrianglePlane.a;
				col_info->PlaneNormal.y  = TrianglePlane.b;
				col_info->PlaneNormal.z  = TrianglePlane.c;


				Vector3D InvPlaneNormal;
				InvPlaneNormal = col_info->PlaneNormal * -1.0f;

				Vector3D Lp1, Lp2;
				Lp1 = Line[0];
				Lp2 = Line[0] + InvPlaneNormal;
				Lp2 = Lp2* 100.0f;
				
				D3DXPlaneIntersectLine(&iPoint, &TrianglePlane, (const D3DXVECTOR3*)&Lp1, (const D3DXVECTOR3*)&Lp2);

				Lp2 = iPoint;
				Vector3D LengthVector = Lp2 - Lp1;

				col_info->DistanceToPlane =LengthVector.Mag();

			}
            
			return true;

		}
	}

	return false;
}


/*
bool LineIntersectsTriangle ( Vector3D* Line, Vector3D* vertex_list, Vector3D* IntersectionPoint )
{

	//Create a d3dx plane from our triangle
	D3DXPLANE D3dPlane;
	D3DXVECTOR3 t[3];

	t[0] = vertex_list[0];
	t[1] = vertex_list[1];
	t[2] = vertex_list[2];

	D3DXPlaneFromPoints(&D3dPlane, t, &t[1], &t[2]);

	if( LineCrossesPlane( D3dPlane, Line[1], Line[2]) == false)
		return false;

	if( LineIntersectsPlane( Line, &D3dPlane, vertex_list, IntersectionPoint))
	{
		if(PointInTriangle( IntersectionPoint, vertex_list ) )
			return true;
	}

	return false;
}

*/


bool CheckPointInTriangle(Vector3D& point, Vector3D& a, Vector3D& b, Vector3D& c) 
{
	float total_angles = 0.0f;

   // make the 3 vectors
	Vector3D v1 = (point-a);
	Vector3D v2 = (point-b);
	Vector3D v3 = (point-c);

	v1.Normalize();
	v2.Normalize();
	v3.Normalize();

	
	total_angles += (float)acos(v1.Dot( v2));   
	total_angles += (float)acos(v2.Dot( v3));
	total_angles += (float)acos(v3.Dot( v1));

	int u = _finite(total_angles); 

	if( (fabs(total_angles-2*PI) <= 0.005)&&(u==1) ) 
		return true;
	

	return false;
}
