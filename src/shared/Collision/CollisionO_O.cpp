#include "collision.h"
#include "sphere3dmath.h"

bool CollisionMesh::AddTriangles( Vector3D* v, int nTriangles )
{
	int VertexCount = nTriangles*3;
	int meshindex = MeshListTemp.size()-1;

	for(int i=0; i<VertexCount; i++)
		TempVertexList.push_back( v[i] );

	return true;
}

void CollisionMesh::NextGroup()
{
	if(MeshListTemp.size())
	{
		//Copy the vertex data into the final buffer
		MeshListTemp[MeshIndex].VertexList = new Vector3D[TempVertexList.size()];
		MeshListTemp[MeshIndex].nVertices = TempVertexList.size();

		for(int i=0; i<MeshListTemp[MeshIndex].nVertices; i++)
			MeshListTemp[MeshIndex].VertexList[i] = TempVertexList[i];

		TempVertexList.clear();
	}
	

	//create a new group and place it in the list
	CollisionMeshGroup CMG;
	MeshListTemp.push_back( CMG );

	MeshIndex++;

}

void CollisionMesh::FindBoundingSphere( int meshindex)
{
	//Make certain that we're using a valid index
	if(meshindex >= MeshListTemp.size())
		return;

	//Find the center of the object.
	Real sumx=0, sumy=0, sumz=0;
	int i;
	int nv = MeshListTemp[meshindex].nVertices;
	for(i=0; i<nv; i++)
	{
		sumx += MeshListTemp[meshindex].VertexList[i].x;
		sumy += MeshListTemp[meshindex].VertexList[i].y;
		sumz += MeshListTemp[meshindex].VertexList[i].z;		
	}

	MeshListTemp[meshindex].x = sumx/(Real)i;
	MeshListTemp[meshindex].y = sumy/(Real)i;
	MeshListTemp[meshindex].z = sumz/(Real)i;


	//Next, find the radius
	Vector3D LengthVector;
	Vector3D CenterVector;

	CenterVector.x = sumx/i;
	CenterVector.y = sumy/i;
	CenterVector.z = sumz/i;

	Real Length=0;
	for(i=0; i<MeshListTemp[meshindex].nVertices; i++)
	{
		LengthVector = MeshListTemp[meshindex].VertexList[i]-CenterVector;

		if(LengthVector.Mag() > Length)
			Length = LengthVector.Mag();
	}

	MeshListTemp[meshindex].Radius = Length;
	
	

}
void CollisionMesh::Finalize()
{
	for(int m=0;m<MeshListTemp.size(); m++)
	{
		
		//calculate bounding sphere info for the group
		FindBoundingSphere( m );
	}

	//Create the final buffer
	if(MeshList)
		delete [] MeshList;

	MeshList = new CollisionMeshGroup[MeshListTemp.size()];
	nMeshes = MeshListTemp.size();

	for(int i=0; i<nMeshes; i++)
	{
		MeshList[i] = MeshListTemp[i];
	}

	MeshListTemp.clear();

}

bool CollisionMesh::SphereCollision( Position &MeshLocation, Vector3D& location, Real radius, CollisionInfo* Collision)
{

	if(Collision->Stuck)
		return false;

	//Declarations

	D3DXVECTOR3 v1, v2, v3;		   //3 vertices of triangle being tested
	Vector3D PlaneNormal;
	D3DXVECTOR3* vTriangle;	   //pointer to vertices
	Vector3D SphereLocation;	   //Local copy version of where the *bounding* sphere is
	Vector3D vTestLocation;        //vector for colliding sphere location
	D3DXVECTOR4 TransformedPoint[3]; //p1 and p2 transformed into local space
	D3DXMATRIX  InverseTransform;    //The inverse of the Mesh's world matrix
	Real Det;						 //Determinant, isn't used for anything but D3D wants it
	Real LargerRadius; //Stores the larger radius between two spheres
	Real distance=0.0f;
	

	//Get the Mesh's matrix, and its inverse
	Matrix M = MeshLocation.GetMatrix();
	D3DXMatrixInverse( &InverseTransform, &Det, &M.Mat );

	//Translate test sphere location into.. local space!! O_O!
	vTestLocation = location;
	D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&vTestLocation, &InverseTransform );		
	vTestLocation = (D3DXVECTOR3)TransformedPoint[0];
	Vector3D vLocalLocation = vTestLocation;
	

	//Go through each sub-mesh and search for collisions
	for(int m=0;m<nMeshes; m++)
	{

		//Store the sphere's center in a vector for easier use
		SphereLocation.x = MeshList[m].x;
		SphereLocation.y = MeshList[m].y;
		SphereLocation.z = MeshList[m].z;


		//Check if sphere we're checking even intersects with the mesh's bounding sphere
		if( MeshList[m].Radius > radius)
			LargerRadius = MeshList[m].Radius;
		else
			LargerRadius = radius;
	
		if(LargerRadius < (SphereLocation - vTestLocation ).Mag() )
			continue;
		
	
        //Hooray! So the line intersects the sphere! Check to see if there's a collision with
		//any vertices.
		for(int i=0; i<MeshList[m].nVertices; i+=3)
		{
            //Store vertex data pointer in something or other
			vTriangle = (D3DXVECTOR3*)&MeshList[m].VertexList[i];	
			
			//convert the Vector3D to D3DXVECTOR3
			D3DXPLANE tplane;
			D3DXPlaneFromPoints( &tplane, &vTriangle[0], &vTriangle[1], &vTriangle[2] );
			PlaneNormal.x = tplane.a;
			PlaneNormal.y = tplane.b;
			PlaneNormal.z = tplane.c;
			PlaneNormal.Normalize();
			D3DXVECTOR3 vNormal;
			vNormal.x = PlaneNormal.x;
			vNormal.y = PlaneNormal.y;
			vNormal.z = PlaneNormal.z;
			

			distance = 0.0f;
			int classification = ClassifySphere((CVector3&)vTestLocation, (CVector3&)vNormal, (CVector3&)vTriangle[0], radius, distance);
			

			if(classification == INTERSECTS) 
			{
				// Now we want to project the sphere's center onto the triangle's plane
				D3DXVECTOR3 vOffset = vNormal * distance;

				// Once we have the offset to the plane, we just subtract it from the center
				// of the sphere.  "vIntersection" is now a point that lies on the plane of the triangle.
				Vector3D v3DTestLocation;
           		v3DTestLocation.x = vTestLocation.x;
				v3DTestLocation.y = vTestLocation.y;
				v3DTestLocation.z = vTestLocation.z;
				Vector3D v3DOffset;
				v3DOffset.x = vOffset.x;
				v3DOffset.y = vOffset.y;
				v3DOffset.z = vOffset.z;

				D3DXVECTOR3 vIntersection = v3DTestLocation - v3DOffset;

			
				// 3) STEP THREE - Check if the intersection point is inside the triangles perimeter

				// We first check if our intersection point is inside the triangle, if not,
				// the algorithm goes to step 4 where we check the sphere again the polygon's edges.

				// We do one thing different in the parameters for EdgeSphereCollision though.
				// Since we have a bulky sphere for our camera, it makes it so that we have to 
				// go an extra distance to pass around a corner. This is because the edges of 
				// the polygons are colliding with our peripheral view (the sides of the sphere).  
				// So it looks likes we should be able to go forward, but we are stuck and considered 
				// to be colliding.  To fix this, we just pass in the radius / 2.  Remember, this
				// is only for the check of the polygon's edges.  It just makes it look a bit more
				// realistic when colliding around corners.  Ideally, if we were using bounding box 
				// collision, cylinder or ellipses, this wouldn't really be a problem.

				if(InsidePolygon((CVector3&)vIntersection, (CVector3*)vTriangle, 3) ||
				EdgeSphereCollision((CVector3&)vTestLocation, (CVector3*)vTriangle, 3, radius/2 ))
				{

					
					//Check for a "stuck" condition
					/*Vector3D StuckTest = v3DTestLocation - (Vector3D&)vIntersection;
					if( StuckTest.Mag() < radius )
					{
						Collision->CollisionPoint = vIntersection;
						D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&Collision->CollisionPoint, &M.Mat );				
						Collision->CollisionPoint = (D3DXVECTOR3)TransformedPoint[0];
					
						D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&vNormal, &(MeshLocation.GetRotationMatrix()).Mat );		
						vNormal = (D3DXVECTOR3)TransformedPoint[0];


						Collision->SuggestedNewLocation = Collision->CollisionPoint;
						Collision->Force.x = 0;
						Collision->Force.y = 0;
						Collision->Force.z = 0;
						Collision->PlaneNormal = vNormal;
						Collision->DistanceToPlane = distance;
						Collision->Stuck = true;

						return false;
											
					}*/


					

					// If we get here, we have collided!  To handle the collision detection
					// all it takes is to find how far we need to push the sphere back.
					// GetCollisionOffset() returns us that offset according to the normal,
					// radius, and current distance the center of the sphere is from the plane.
					CVector3 cv3Offset;
					cv3Offset = GetCollisionOffset((CVector3&)vNormal, radius, distance);
					v3DOffset.x = cv3Offset.x;
					v3DOffset.y = cv3Offset.y;
					v3DOffset.z = cv3Offset.z;
					
					// Now that we have the offset, we want to ADD it to the position and
					// view vector in our camera.  This pushes us back off of the plane.  We
					// don't see this happening because we check collision before we render
					// the scene.
					D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&vNormal, &(MeshLocation.GetRotationMatrix()).Mat );		
					vNormal = (D3DXVECTOR3)TransformedPoint[0];
					
					Collision->CollisionPoint = vLocalLocation + v3DOffset;
					D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&Collision->CollisionPoint, &M.Mat );				
					Collision->CollisionPoint = (D3DXVECTOR3)TransformedPoint[0];
				


					Collision->SuggestedNewLocation = Collision->CollisionPoint;
					Collision->Force.x = 0;
					Collision->Force.y = 0;
					Collision->Force.z = 0;
					Collision->PlaneNormal = vNormal;
					Collision->DistanceToPlane = distance;
										

					return true;
					
									
				}
			}
		}
	}
	

	

	

	return false;

}

bool CollisionMesh::Collision( Position &MeshLocation, Vector3D& p1, Vector3D& p2, CollisionInfo* Collision )
{
	//Declarations
	CollisionInfo CollisionBuffer; //Buffer for storing the closest collision found so far
	CollisionBuffer.DistanceToPlane = 400000000; //
	D3DXVECTOR3* v;		   //3 vertices of triangle being tested
	Vector3D* CollisionVertex;	   //pointer to vertices
	Vector3D SphereLocation;	   //Local copy version of where sphere is
	Vector3D point1=p1, point2=p2;   //Local copies of p1 and p2 of the line
	Vector3D RayDir = point2-point1; //Direction the line is facing
	Vector3D InvRayDir;				 //Inverse of that direction	
	D3DXVECTOR4 TransformedPoint[3]; //p1 and p2 transformed into local space
	D3DXMATRIX  InverseTransform;    //The inverse of the Mesh's world matrix
	Real Det;						 //Determinant, isn't used for anything but D3D wants it
	bool FoundCollision=false;		 //If a collision was found, this is true


	//Get the Mesh's matrix, and its inverse
	Matrix M = MeshLocation.GetMatrix();
	D3DXMatrixInverse( &InverseTransform, &Det, &M.Mat );

	//Translate point1 and point2 into ..local space! Of the mesh!
	D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&point1, &InverseTransform );		
	D3DXVec3Transform( &TransformedPoint[1], (D3DXVECTOR3*)&point2, &InverseTransform );		
	point1 = (D3DXVECTOR3)TransformedPoint[0];
	point2 = (D3DXVECTOR3)TransformedPoint[1];

	
	//Find out the direction of the line and its inverse direction. 
	RayDir    = point2-point1;
	InvRayDir = RayDir * -1.0f;
	

	//Store the RayDir's magnitude, then normalize
	Real LineLength = RayDir.Mag();
	RayDir.Normalize();


	//Go through each sub-mesh and search for collisions
	for(int m=0;m<nMeshes; m++)
	{

		//Store the sphere's center in a vector for easier use
		SphereLocation.x = MeshList[m].x;
		SphereLocation.y = MeshList[m].y;
		SphereLocation.z = MeshList[m].z;


		//Check if the ray of point1 to point2 intersects the circle
		bool RayIntersects = D3DXSphereBoundProbe( (D3DXVECTOR3*)&SphereLocation, MeshList[m].Radius, (D3DXVECTOR3*)&point1, (D3DXVECTOR3*)&RayDir);
		if(!RayIntersects)
		{
			continue;
		}

		//check if the opposite ray from point2 to point1 intsersects the circle
		RayIntersects = D3DXSphereBoundProbe( (D3DXVECTOR3*)&SphereLocation, MeshList[m].Radius, (D3DXVECTOR3*)&point2, (D3DXVECTOR3*)&InvRayDir);
		if(!RayIntersects)
		{
			continue;
		}

        //Hooray! So the line intersects the sphere! Check to see if there's a collision with
		//any vertices.
		for(int i=0; i<MeshList[m].nVertices; i+=3)
		{
            //Store vertex data pointer in something or other
			v = (D3DXVECTOR3*)&MeshList[m].VertexList[i];	
								

			//Variables for finding collision point!		
			Real U, V, Distance;



			//Check for collision!
			if( D3DXIntersectTri ( (D3DXVECTOR3*)&v[0], (D3DXVECTOR3*)&v[1], (D3DXVECTOR3*)&v[2],
				(D3DXVECTOR3*)&point1, (D3DXVECTOR3*)&RayDir, &U, &V, &Distance ) )
			{

				if(LineLength < Distance )
					continue;

				//Get force, collision point, and distance of collision
				Collision->Force = point2-point1;
				Collision->CollisionPoint  = v[0] + (v[1]-v[0])*U + (v[2]-v[0])*V;
				Collision->DistanceToPlane = Distance;

				//Transform the collision point into world space
				D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&Collision->CollisionPoint, &M.Mat );
				Collision->CollisionPoint = (D3DXVECTOR3)TransformedPoint[0];
		
				//Find plane normal here
				D3DXPLANE Plane;
				D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&v[0], &M.Mat );
				D3DXVec3Transform( &TransformedPoint[1], (D3DXVECTOR3*)&v[1], &M.Mat );
				D3DXVec3Transform( &TransformedPoint[2], (D3DXVECTOR3*)&v[2], &M.Mat );
				v[0] = (D3DXVECTOR3)TransformedPoint[0];
				v[1] = (D3DXVECTOR3)TransformedPoint[1];
				v[2] = (D3DXVECTOR3)TransformedPoint[2];
				D3DXPlaneFromPoints( &Plane, &v[0], &v[1], &v[2] );
				Collision->PlaneNormal.x=Plane.a;
				Collision->PlaneNormal.y=Plane.b;
				Collision->PlaneNormal.z=Plane.c;
				Collision->PlaneNormal.Normalize();
				

				//If this is closest collision, set the buffer equal to it.
				if(Collision->DistanceToPlane < CollisionBuffer.DistanceToPlane)
					CollisionBuffer = *Collision;

				//Hooray! We found a collision!
				FoundCollision = true;

			}


		}
	}
	

	//YARRRR!!! RETURN THE COLLISION MATEY!!
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
	if( CmbMesh.nMeshes != nMeshes)
		return;

	if( OutMesh->nMeshes != nMeshes )
	{
		return;
	}
	
	Real StartX, StartY, StartZ;
	Real EndX, EndY, EndZ;
	Real ChangeX, ChangeY, ChangeZ;
	for(int m=0; m<nMeshes; m++)
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


