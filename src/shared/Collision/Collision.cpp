#include "collision.h"
#include "collisionUtils.h"


#include "..\defs\defs.h"


CollisionMesh::~CollisionMesh() 
{ 
	//Go through the entire mesh list and clean up allocated memory
	if(MeshList) 
	{
		for(int i=0; i< this->nMeshes; i++)
		{
			if(MeshList[i].VertexList )
				delete [] MeshList[i].VertexList;
			if(MeshList[i].SphereList )
				delete [] MeshList[i].SphereList;
		}

		delete [] MeshList;
	}
	
}


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

		//create bounding sphere list
		MeshListTemp[MeshIndex].SphereList = new CollisionBoundingSphere[MeshListTemp[MeshIndex].nVertices/3];
		MeshListTemp[MeshIndex].nSpheres   = MeshListTemp[MeshIndex].nVertices /3;

	
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

	//Find the center of the object by going through it
	//triangle by triangle
	Real sumx=0, sumy=0, sumz=0;
	Real d1, d2, d3;
	Real cx, cy, cz;
	int i;
	int nv = MeshListTemp[meshindex].nVertices;
	for(i=0; i<nv; i+=3)
	{
		//sum up the 3 vertices of the triangle
		sumx += MeshListTemp[meshindex].VertexList[i+0].x;
		sumy += MeshListTemp[meshindex].VertexList[i+0].y;
		sumz += MeshListTemp[meshindex].VertexList[i+0].z;		
		sumx += MeshListTemp[meshindex].VertexList[i+1].x;
		sumy += MeshListTemp[meshindex].VertexList[i+1].y;
		sumz += MeshListTemp[meshindex].VertexList[i+1].z;
		sumx += MeshListTemp[meshindex].VertexList[i+2].x;
		sumy += MeshListTemp[meshindex].VertexList[i+2].y;
		sumz += MeshListTemp[meshindex].VertexList[i+2].z;

		//Figure out the bounding sphere location for this triangle (average of 3 vertices)
		MeshListTemp[meshindex].SphereList[i/3].Location.x  = MeshListTemp[meshindex].VertexList[i+0].x;
		MeshListTemp[meshindex].SphereList[i/3].Location.y  = MeshListTemp[meshindex].VertexList[i+0].y;
		MeshListTemp[meshindex].SphereList[i/3].Location.z  = MeshListTemp[meshindex].VertexList[i+0].z;
		MeshListTemp[meshindex].SphereList[i/3].Location.x += MeshListTemp[meshindex].VertexList[i+1].x;
		MeshListTemp[meshindex].SphereList[i/3].Location.y += MeshListTemp[meshindex].VertexList[i+1].y;
		MeshListTemp[meshindex].SphereList[i/3].Location.z += MeshListTemp[meshindex].VertexList[i+1].z;
		MeshListTemp[meshindex].SphereList[i/3].Location.x += MeshListTemp[meshindex].VertexList[i+2].x;
		MeshListTemp[meshindex].SphereList[i/3].Location.y += MeshListTemp[meshindex].VertexList[i+2].y;
		MeshListTemp[meshindex].SphereList[i/3].Location.z += MeshListTemp[meshindex].VertexList[i+2].z;

		//average the points to get the triangle center
		MeshListTemp[meshindex].SphereList[i/3].Location.x /= 3.0f; 
		MeshListTemp[meshindex].SphereList[i/3].Location.y /= 3.0f;
		MeshListTemp[meshindex].SphereList[i/3].Location.z /= 3.0f;
		cx = MeshListTemp[meshindex].SphereList[i/3].Location.x;
		cy = MeshListTemp[meshindex].SphereList[i/3].Location.y;
		cz = MeshListTemp[meshindex].SphereList[i/3].Location.z;

		//now get the distance each vertex is from the triangle center
		d1 = sqrt( (MeshListTemp[meshindex].VertexList[i+0].x-cx) * (MeshListTemp[meshindex].VertexList[i+0].x-cx)
				   + (MeshListTemp[meshindex].VertexList[i+0].y-cy) * (MeshListTemp[meshindex].VertexList[i+0].y-cy) 
				   + (MeshListTemp[meshindex].VertexList[i+0].z-cz) * (MeshListTemp[meshindex].VertexList[i+0].z-cz) );
		d2 = sqrt( (MeshListTemp[meshindex].VertexList[i+1].x-cx) * (MeshListTemp[meshindex].VertexList[i+1].x-cx)
				   + (MeshListTemp[meshindex].VertexList[i+1].y-cy) * (MeshListTemp[meshindex].VertexList[i+1].y-cy) 
				   + (MeshListTemp[meshindex].VertexList[i+1].z-cz) * (MeshListTemp[meshindex].VertexList[i+1].z-cz) );
		d3 = sqrt( (MeshListTemp[meshindex].VertexList[i+2].x-cx) * (MeshListTemp[meshindex].VertexList[i+2].x-cx)
				   + (MeshListTemp[meshindex].VertexList[i+2].y-cy) * (MeshListTemp[meshindex].VertexList[i+2].y-cy) 
				   + (MeshListTemp[meshindex].VertexList[i+2].z-cz) * (MeshListTemp[meshindex].VertexList[i+2].z-cz) );

		//set the radius to the point farthest away
		if( d1 >= d2 && d1 >= d3)
			MeshListTemp[meshindex].SphereList[i/3].Radius = d1;
		else if( d2 >= d1 && d2 >= d3)
			MeshListTemp[meshindex].SphereList[i/3].Radius = d2;
		else 
			MeshListTemp[meshindex].SphereList[i/3].Radius = d3;
		
	}

	//set the mesh center
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

bool CollisionMesh::SphereCollision( Position &MeshLocation, const Vector3D& location, Real radius, CollisionInfo* Collision)
{

	return false;

}

bool CollisionMesh::Collision( Position &MeshLocation, const Vector3D& p1, const Vector3D& p2, CollisionInfo* Collision )
{

	//Declarations
	CollisionInfo CollisionBuffer; //Buffer for storing the closest collision found so far
	CollisionBuffer.DistanceToPlane = 400000000; //
	D3DXVECTOR3 v1, v2, v3;		   //3 vertices of triangle being tested
	Vector3D* CollisionVertex;	   //pointer to vertices
	Vector3D SphereLocation;	   //Local copy version of where sphere is
	Vector3D point1=p1, point2=p2;   //Local copies of p1 and p2 of the line
	Vector3D RayDir = point2-point1; //Direction the line is facing
	Vector3D InvRayDir;				 //Inverse of that direction	
	D3DXVECTOR4 TransformedPoint[3]; //p1 and p2 transformed into local space
	D3DXMATRIX  InverseTransform;    //The inverse of the Mesh's world matrix
	Real Det;						 
	//Determinant, isn't used for anything but D3D wants it
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

		if( CollisionUtil::LineIntersectSphere( point1, point2, SphereLocation, MeshList[m].Radius ) == false )
		{
			continue;
		}
/*
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
		}*/

        //Hooray! So the line intersects the sphere! Check to see if there's a collision with
		//any vertices.
		for(int i=0; i<MeshList[m].nVertices; i+=3)
		{
            //Store vertex data pointer in something or other
			CollisionVertex = &MeshList[m].VertexList[i];	
				
			//Variables for finding collision point!		
			Real U, V, Distance;
			
			//check if the ray intersects the triangles bounding sphere, if it doesn't
			//don't even bother checking the triangle. 
			if(! D3DXSphereBoundProbe( (D3DXVECTOR3*)&MeshList[m].SphereList[i/3].Location, MeshList[m].SphereList[i/3].Radius, (D3DXVECTOR3*)&point1, (D3DXVECTOR3*)&RayDir) )
				continue;
			if(! D3DXSphereBoundProbe( (D3DXVECTOR3*)&MeshList[m].SphereList[i/3].Location, MeshList[m].SphereList[i/3].Radius, (D3DXVECTOR3*)&point2, (D3DXVECTOR3*)&InvRayDir) )
				continue;

			

			//Check for collision!
			if( D3DXIntersectTri ( (D3DXVECTOR3*) &CollisionVertex[0], (D3DXVECTOR3*)&CollisionVertex[1], (D3DXVECTOR3*)&CollisionVertex[2],
				(D3DXVECTOR3*)&point1, (D3DXVECTOR3*)&RayDir, &U, &V, &Distance ) )
			{

				if(LineLength < Distance )
					continue;

				//convert the Vector3D to D3DXVECTOR3
				v1 = CollisionVertex[0];
				v2 = CollisionVertex[1];
				v3 = CollisionVertex[2];

				//Get force, collision point, and distance of collision
				Collision->Force = point2-point1;
				Collision->CollisionPoint  = v1 + (v2-v1)*U + (v3-v1)*V;
				Collision->DistanceToPlane = Distance;

				//Transform the collision point into world space
				D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&Collision->CollisionPoint, &M.Mat );
				Collision->CollisionPoint = (D3DXVECTOR3)TransformedPoint[0];
		
				//Find plane normal here
				D3DXPLANE Plane;
				D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&v1, &M.Mat );
				D3DXVec3Transform( &TransformedPoint[1], (D3DXVECTOR3*)&v2, &M.Mat );
				D3DXVec3Transform( &TransformedPoint[2], (D3DXVECTOR3*)&v3, &M.Mat );
				v1 = (D3DXVECTOR3)TransformedPoint[0];
				v2 = (D3DXVECTOR3)TransformedPoint[1];
				v3 = (D3DXVECTOR3)TransformedPoint[2];
				D3DXPlaneFromPoints( &Plane, &v1, &v2, &v3 );
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

bool CollisionMesh::Collision( Matrix& M, const Vector3D& p1, const Vector3D& p2, CollisionInfo* Collision )
{
	
	//Declarations
	CollisionInfo CollisionBuffer; //Buffer for storing the closest collision found so far
	CollisionBuffer.DistanceToPlane = 400000000; //
	D3DXVECTOR3 v1, v2, v3;		   //3 vertices of triangle being tested
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
			CollisionVertex = &MeshList[m].VertexList[i];	
				
			//Variables for finding collision point!		
			Real U, V, Distance;
			
			//check if the ray intersects the triangles bounding sphere, if it doesn't
			//don't even bother checking the triangle. 
			if(! D3DXSphereBoundProbe( (D3DXVECTOR3*)&MeshList[m].SphereList[i/3].Location, MeshList[m].SphereList[i/3].Radius, (D3DXVECTOR3*)&point1, (D3DXVECTOR3*)&RayDir) )
				continue;
			if(! D3DXSphereBoundProbe( (D3DXVECTOR3*)&MeshList[m].SphereList[i/3].Location, MeshList[m].SphereList[i/3].Radius, (D3DXVECTOR3*)&point2, (D3DXVECTOR3*)&InvRayDir) )
				continue;

			

			//Check for collision!
			if( D3DXIntersectTri ( (D3DXVECTOR3*) &CollisionVertex[0], (D3DXVECTOR3*)&CollisionVertex[1], (D3DXVECTOR3*)&CollisionVertex[2],
				(D3DXVECTOR3*)&point1, (D3DXVECTOR3*)&RayDir, &U, &V, &Distance ) )
			{

				if(LineLength < Distance )
					continue;

				//convert the Vector3D to D3DXVECTOR3
				v1 = CollisionVertex[0];
				v2 = CollisionVertex[1];
				v3 = CollisionVertex[2];

				//Get force, collision point, and distance of collision
				Collision->Force = point2-point1;
				Collision->CollisionPoint  = v1 + (v2-v1)*U + (v3-v1)*V;
				Collision->DistanceToPlane = Distance;

				//Transform the collision point into world space
				D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&Collision->CollisionPoint, &M.Mat );
				Collision->CollisionPoint = (D3DXVECTOR3)TransformedPoint[0];
		
				//Find plane normal here
				D3DXPLANE Plane;
				D3DXVec3Transform( &TransformedPoint[0], (D3DXVECTOR3*)&v1, &M.Mat );
				D3DXVec3Transform( &TransformedPoint[1], (D3DXVECTOR3*)&v2, &M.Mat );
				D3DXVec3Transform( &TransformedPoint[2], (D3DXVECTOR3*)&v3, &M.Mat );
				v1 = (D3DXVECTOR3)TransformedPoint[0];
				v2 = (D3DXVECTOR3)TransformedPoint[1];
				v3 = (D3DXVECTOR3)TransformedPoint[2];
				D3DXPlaneFromPoints( &Plane, &v1, &v2, &v3 );
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
		//Interpolate the vertices
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


