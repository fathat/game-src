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
	/*int VertexCount = nTriangles*3;
	int meshindex = MeshListTemp.size()-1;


	for(int i=0; i<VertexCount; i++)
		TempVertexList.push_back( v[i] );
*/

	CMTriangle t;

	for( int i =0; i < nTriangles; i++) 
	{
		t.v1.x = v[i*3+0].x;
		t.v1.y = v[i*3+0].y;
		t.v1.z = v[i*3+0].z;
		t.v2.x = v[i*3+1].x;
		t.v2.y = v[i*3+1].y;
		t.v2.z = v[i*3+1].z;
		t.v3.x = v[i*3+2].x;
		t.v3.y = v[i*3+2].y;
		t.v3.z = v[i*3+2].z;
		TempVertexList.push_back( t );
	}
	

	return true;
}

void CollisionMesh::NextGroup()
{
	
}

void CollisionMesh::FindBoundingSphere( int meshindex)
{
	

}
void CollisionMesh::Finalize()
{
	//create mesh data structure
	meshData = dGeomTriMeshDataCreate();

	//create indices
	int indexlist = new int[TriangleList.size()*3];
	for( int i = 0; i< TriangleList.size()*3; i++)
	{
		indexlist[i] = i;
	}

	//copy over vertex data into buffer
	dReal* vertices = new dReal[TriangleList.size()*9];
	int vi=0;
	for ( int i=0; i<TriangleList.size(); i++ )
	{
		vertices[vi+0] = TriangleList[i].v1.x;
		vertices[vi+1] = TriangleList[i].v1.y;
		vertices[vi+2] = TriangleList[i].v1.z;
		vertices[vi+3] = TriangleList[i].v2.x;
		vertices[vi+4] = TriangleList[i].v2.y;
		vertices[vi+5] = TriangleList[i].v2.z;
		vertices[vi+6] = TriangleList[i].v3.x;
		vertices[vi+7] = TriangleList[i].v3.y;
		vertices[vi+8] = TriangleList[i].v3.z;
		vi += 9;

	}

	dGeomTriMeshDataBuildSingle(meshData, vertices, sizeof(dReal)*3,
		TriangleList.size()*3, 
		(const int*)indexlist, 
		TriangleList.size()*3, 3*sizeof( int ));

	Geom = dCreateTriMesh(solver->GetSpaceID(true, Location.x, Location.y, Location.z), meshData, 0, 0, 0);
	dGeomSetData( Geom, &SurfaceDesc );
	dGeomSetPosition( Geom, Location.x, Location.y, Location.z );
	dMatrix3 R;
	dRFromEulerAngles (R, pitch, -yaw, roll);
	dGeomSetRotation( Geom, R );
	Initialized = true;



}

bool CollisionMesh::SphereCollision( Position &MeshLocation, Vector3D& location, Real radius, CollisionInfo* Collision)
{

	return false;

}

bool CollisionMesh::Collision( Position &MeshLocation, Vector3D& p1, Vector3D& p2, CollisionInfo* Collision )
{


	return false;
}

bool CollisionMesh::Collision( Matrix& M, Vector3D& p1, Vector3D& p2, CollisionInfo* Collision )
{
	


	return false;
}


void CollisionMesh::CombineWith( CollisionMesh& CmbMesh, CollisionMesh* OutMesh, Real Percent)
{

		
}


