#include "Structure.h"

std::string Structure::GetTypeString()
{
	return "Structure";
}


bool Structure::Init(MeshHandle mh, MeshManager* MM, DynamicsSolver* solver, Vector3D& Location, float yaw, float pitch, float roll )
{
	if(Initialized)
		Shutdown();

	//get a pointer to our mesh
	Mesh* mesh = MM->Get( mh );
    
	int nv=0;
	int ni=0;
	
	//figure out how many vertices and indices we have to store
	for(int i=0; i<mesh->GetSubmeshCount(); i++)
	{
		nv += mesh->GetSubmesh(i)->nVertices;
		ni += mesh->GetSubmesh(i)->nIndices;
	}

	SavedVertices = new dReal[nv*3];
	nSavedVertices = nv;
	SavedIndices = new dTriIndex[ni];
	nSavedIndices = ni;


	//copy data into our lists
	Vector3D MagCheck;
	Radius = 0;
	int vIndex=0, iIndex=0;
	for(int i=0; i<mesh->GetSubmeshCount(); i++)
	{
		
		//copy data into index buffer
		for(int j=0; j< mesh->GetSubmesh(i)->nIndices; j++)
		{
			SavedIndices[iIndex+j] = mesh->GetSubmesh(i)->IndexList[j] + vIndex/3;
			
		}
		iIndex  += mesh->GetSubmesh(i)->nIndices;

		//copy data into vertex buffer
		for(int j=0; j<3* mesh->GetSubmesh(i)->nVertices; j++)
		{
			SavedVertices[vIndex] = mesh->GetSubmesh(i)->VertexList[j];
			vIndex ++;

		}

		//figure out the radius
		for(int j=0; j<3*mesh->GetSubmesh(i)->nVertices; j+=3 )
		{
			MagCheck.x = SavedVertices[j+0];
			MagCheck.y = SavedVertices[j+1];
			MagCheck.z = SavedVertices[j+2];

			if( MagCheck.Mag() > Radius)
				Radius = MagCheck.Mag();
		}
		

	}

	char radstring[512];
	/*//intialize the mesh stuff
	meshData = dGeomTriMeshDataCreate();

	dGeomTriMeshDataBuildSimple(meshData, (dVector3*)mesh->ColliderList[0].VertexList, 
			mesh->ColliderList[0].nVertices, 
			(const int*)mesh->ColliderList[0].IndexList, 
			mesh->ColliderList[0].nIndices);
	Geom = dCreateTriMesh(solver->SpaceID, meshData, 0, 0, 0);
	dGeomSetPosition( Geom, Location.x, Location.y, Location.z );

	Initialized = true;*/
	//create the ODE geoms
	meshData = dGeomTriMeshDataCreate();
	
	/*dGeomTriMeshDataBuildSimple(meshData, SavedVertices, 
			nSavedVertices, 
			(const int*)SavedIndices, 
			nSavedIndices);*/

	dGeomTriMeshDataBuildSingle(meshData, SavedVertices, 3*sizeof(dReal),
		nSavedVertices, 
		(const int*)SavedIndices, 
		nSavedIndices, 3*sizeof( int ));
	
	Geom = dCreateTriMesh(solver->GetSpaceID(true, Location.x, Location.y, Location.z), meshData, 0, 0, 0);
	dGeomSetData( Geom, &SurfaceDesc );
	dGeomSetPosition( Geom, Location.x, Location.y, Location.z );
	dMatrix3 R;
	dRFromEulerAngles (R, pitch, -yaw, roll);
	dGeomSetRotation( Geom, R );
	Initialized = true;

	NoDisable = true;//false;
	//Geom = NULL;


	return true;

}


bool Structure::Init(CollisionMesh& ColMesh,DynamicsSolver* solver, Vector3D& Location, float yaw, float pitch, float roll )
{
	Shutdown();

	//determine how many triangles we actually have to deal with
	int totalTriangles = 0;

	for(int i=0; i<ColMesh.nMeshes; i++)
	{
		totalTriangles += ColMesh.MeshList[i].nVertices/3;
	}
	
	//create an array and copy all our triangles into it
	SavedVertices = new dReal[totalTriangles*3*3];
	nSavedVertices = totalTriangles*3;
	int index=0;
	Radius = 0;

	for(int m=0; m<ColMesh.nMeshes; m++)
	{		
		for(int n=0; n<ColMesh.MeshList[m].nVertices; n++)
		{
			SavedVertices[index+0] = ColMesh.MeshList[m].VertexList[n].x;
			SavedVertices[index+1] = ColMesh.MeshList[m].VertexList[n].y;
			SavedVertices[index+2] = ColMesh.MeshList[m].VertexList[n].z;
			index+=3;


			//determine the object's radius
			if( ColMesh.MeshList[m].VertexList[n].Mag() > Radius)
				Radius = ColMesh.MeshList[m].VertexList[n].Mag();
			
		}
	}


	//generate an index list
	SavedIndices = new dTriIndex[totalTriangles*3];
	nSavedIndices = totalTriangles*3;
	for(int j=0; j<totalTriangles*3; j+=3 )
	{
		SavedIndices[j+0] = j+0;
		SavedIndices[j+1] = j+1;
		SavedIndices[j+2] = j+2;
	}

	//create the ODE geom
	meshData = dGeomTriMeshDataCreate();

	dGeomTriMeshDataBuildSimple(meshData, (dReal*)SavedVertices, 
			nSavedVertices, 
			(const dTriIndex*)SavedIndices, 
			nSavedIndices);
	Geom = dCreateTriMesh(solver->GetSpaceID(true, Location.x, Location.y, Location.z), meshData, 0, 0, 0);
	dGeomSetData( Geom, &SurfaceDesc );
	dGeomSetPosition( Geom, Location.x, Location.y, Location.z );
	dMatrix3 R;
	dRFromEulerAngles (R, pitch, -yaw, roll);
	dGeomSetRotation( Geom, R );

	Initialized = true;
	NoDisable = false;


	return true;
}


//Initializes a polygonal physics structure
bool Structure::Init(Vector3D* Vertices, int nVertices, int* Indices, int nIndices,DynamicsSolver* solver, Vector3D& Location, float yaw, float pitch, float roll )
{
	//clean out any old stuff
	Shutdown();
	
	//create an array and copy all our triangles into it
	SavedVertices = new dReal[nVertices*3];
	nSavedVertices = nVertices;
		

	Radius = 0;
	int vertindex=0;
	for(int n=0; n<nVertices; n++)
	{
		SavedVertices[vertindex+0] = Vertices[n].x;
		SavedVertices[vertindex+1] = Vertices[n].y;
		SavedVertices[vertindex+2] = Vertices[n].z;
		vertindex+=3;

		//determine the structure's radius
		if( Vertices[n].Mag() > Radius)
			Radius = Vertices[n].Mag();
		
		
	}

	//copy indices to new array
	SavedIndices = new dTriIndex[nIndices];
	nSavedIndices= nIndices;

	for(int j=0; j<nIndices; j++)
	{
		SavedIndices[j] = Indices[j];
	}

	//create the ODE geom
	meshData = dGeomTriMeshDataCreate();
	
	dGeomTriMeshDataBuildSimple(meshData, (dReal*)SavedVertices, 
			nVertices, 
			(const dTriIndex*)SavedIndices, 
			nIndices);
	Geom = dCreateTriMesh(solver->GetSpaceID( true, Location.x, Location.y, Location.z ), meshData, 0, 0, 0);
	dGeomSetData( Geom, &SurfaceDesc );
	dGeomSetPosition( Geom, Location.x, Location.y, Location.z );

	dMatrix3 R;
	dRFromEulerAngles (R, pitch, -yaw, roll);
	dGeomSetRotation( Geom, R );

	Initialized = true;
	NoDisable = false;


	return true;
}


void Structure::Shutdown()
{
	if(SavedVertices)
		delete [] SavedVertices;
	if(SavedIndices)
		delete [] SavedIndices;
	SavedVertices = NULL;
	SavedIndices  = NULL;
	Initialized = false;
}



/*static void staticObjectNearCallback (void *data, dGeomID o1, dGeomID o2)
{
	CollisionInfo* collisioninfo = (CollisionInfo*) data;
	dContact contact;
	int n = dCollide( o1, o2, 1,  &contact.geom, sizeof( dContact)  );

	if( n )
	{

		collisioninfo->CollisionPoint = Vector3D( contact.geom.pos[0], contact.geom.pos[1], contact.geom.pos[2] );
		collisioninfo->PlaneNormal    = Vector3D( contact.geom.normal[0], contact.geom.normal[1], contact.geom.normal[2]  );
	}	

}*/

bool Structure::CheckForCollision(const Vector3D& a, const Vector3D& b, CollisionInfo * info )
{
	//cache some data
	Vector3D c = (b-a);
	float length = c.Length();

	//create the ray
	dGeomID ray = dCreateRay( NULL , length );

	//set its position
	dGeomRaySet( ray, a.x, a.y, a.z, (b-a).x, (b-a).y, (b-a).z  );
	dGeomRaySetLength( ray, length );
	
	//collide the ray
	dContact contact[32];
	int n = dCollide( ray, Geom, 32,  &contact[0].geom, sizeof( dContact)  );

	if( n > 0 )
	{
		//find the closest point
		CollisionInfo cp ;
		cp.CollisionPoint = Vector3D( contact[0].geom.pos[0],contact[0].geom.pos[1],contact[0].geom.pos[2] );
		cp.PlaneNormal    = Vector3D( contact[0].geom.normal[0],contact[0].geom.normal[1],contact[0].geom.normal[2] );
		
		for( int i=0; i<n; i++ )
		{
			CollisionInfo inf;
			inf.CollisionPoint = Vector3D( contact[i].geom.pos[0],contact[i].geom.pos[1],contact[i].geom.pos[2] );
			inf.PlaneNormal    = Vector3D( contact[i].geom.normal[0],contact[i].geom.normal[1],contact[i].geom.normal[2] );

			Vector3D da, db;
			da = inf.CollisionPoint - a;
			db = cp.CollisionPoint - a;

			if( da.MagSq() < db.MagSq() )
			{
				cp = inf;
			}
		}

		*info = cp;
		dGeomDestroy( ray );
		return true;

	}
	
	dGeomDestroy( ray );
	return false;
}