#include "Mesh.h"
#include <stdio.h>
#include "..\s3dlib\types.h"
#include "..\collision\collision.h"


MeshMaterial::MeshMaterial () { 
	Image = NULL; 

	AmbientRGBA.r = 1.0f;
	AmbientRGBA.g =	1.0f;
	AmbientRGBA.b =	1.0f;
	AmbientRGBA.a =	1;

	DiffuseRGBA.r = 1.0f;
	DiffuseRGBA.g =	1.0f;
	DiffuseRGBA.b =	1.0f;
	DiffuseRGBA.a =	1;

	SpecularRGBA.r = 1.0f;
	SpecularRGBA.g = 1.0f;
	SpecularRGBA.b = 1.0f;
	SpecularRGBA.a = 1;

	Shininess    = 0;
	Transparency = 0;
	Image = 0;
}

MeshGroup::MeshGroup(Screen3D& screen, cf_mesh& cfmesh, CollisionMesh* colmesh, AABB* bbox, FILE* file, bool isStatic)
{
	cf_vertex*   cfvertex;
	cf_normal*   cfnormal;
	cf_triangle* cftriangle;

	//init memory
	cfvertex = new cf_vertex[cfmesh.nVertices];
	cfnormal = new cf_normal[cfmesh.nNormals];
	cftriangle = new cf_triangle[cfmesh.nTriangles];

	//load in stuff

	// vertices
	fread(cfvertex, sizeof(cf_vertex), cfmesh.nVertices, file);

	// vertex normals
	fread(cfnormal, sizeof(cf_normal), cfmesh.nNormals, file);

	// triangles
	fread(cftriangle, sizeof(cf_triangle), cfmesh.nTriangles, file);			

	//create appropriate buffers for storing data
	if(isStatic)
		VB.Init(screen, D3DFVF_KEYFRAMEVERTEX, sizeof(KeyframeVertex), cfmesh.nVertices, false, false);
	else
		VB.Init(screen, NULL, sizeof(KeyframeVertex), cfmesh.nVertices, false, true);

	IB.Init(screen, cfmesh.nTriangles * 3, IB16Bit);

	IndexList = new unsigned int[cfmesh.nTriangles*3];
	nIndices  = cfmesh.nTriangles*3;
	VertexList= new dReal[cfmesh.nVertices*3];
	nVertices = cfmesh.nVertices;

	//temp variables
	KeyframeVertex* VBList;
	Index16* IList;
	int Index=0;

	//read the vertex data into a vertex buffer. Note
	//that we don't read in the normals quite yet --
	//we do that after this since normal indices are
	//stored with the triangles.
	VB.Lock((BYTE**)&VBList);


	//definitions for variables that are used to figure
	//out the bounding sphere for the vertex buffer
	Vector3D spherevector;
	Real cx=0, cy=0, cz=0;
	int vertindex = 0;
	for(int n=0; n<cfmesh.nVertices; n++)
	{
		// Initialize the vertex buffer with said vertex
		VBList[n].position.x = cfvertex[n].x;
		VBList[n].position.y = cfvertex[n].y;
		VBList[n].position.z = cfvertex[n].z;
		VBList[n].tu         = cfvertex[n].u;
		VBList[n].tv         = cfvertex[n].v;
		VertexList[vertindex+0] = cfvertex[n].x;
		VertexList[vertindex+1] = cfvertex[n].y;
		VertexList[vertindex+2] = cfvertex[n].z;

		//Add to the center value of the bounding sphere.
		//After we've added all the values we'll average
		//them out to find the center of the object
		cx += cfvertex[n].x;
		cy += cfvertex[n].y;
		cz += cfvertex[n].z;

		//Figure out how far out the vertex is,
		//and if it's the farthest out yet set the
		//radius of the bounding sphere equal to its distance out.
		spherevector.x = cfvertex[n].x;
		spherevector.y = cfvertex[n].y;
		spherevector.z = cfvertex[n].z;

		float mag = spherevector.Mag();
		if( mag > radius )
			radius = mag;

		// Check to see if we need to redefine boundaries to include
		// the vertex
		if(cfvertex[n].x < bbox->min.x )
			bbox->min.x = cfvertex[n].x;
		if(cfvertex[n].y < bbox->min.y )
			bbox->min.y = cfvertex[n].y;
		if(cfvertex[n].z < bbox->min.z )
			bbox->min.z = cfvertex[n].z;

		if(cfvertex[n].x > bbox->max.x )
			bbox->max.x = cfvertex[n].x;
		if(cfvertex[n].y > bbox->max.y )
			bbox->max.y = cfvertex[n].y;
		if(cfvertex[n].z > bbox->max.z )
			bbox->max.z = cfvertex[n].z;

		vertindex += 3;
	}


	//Set up the bounding sphere based off the values
	//found in the loop above.
	boundingSphere.x ( cx/cfmesh.nVertices );
	boundingSphere.y ( cy/cfmesh.nVertices );
	boundingSphere.z ( cz/cfmesh.nVertices );
	boundingSphere.Radius = radius;

	//read in triangle indices into an index buffer, as well as
	//also assigning normals to the vertices
	IB.Lock(&IList);

	Vector3D CollisionTriangle[3];
	for(int m=0; m<cfmesh.nTriangles; m++)
	{

		//steal the indices
		IList[Index+0] = (WORD)cftriangle[m].v1;
		IList[Index+1] = (WORD)cftriangle[m].v2;
		IList[Index+2] = (WORD)cftriangle[m].v3;

		CollisionTriangle[0].x = cfvertex[cftriangle[m].v1].x;
		CollisionTriangle[0].y = cfvertex[cftriangle[m].v1].y;
		CollisionTriangle[0].z = cfvertex[cftriangle[m].v1].z;
		CollisionTriangle[1].x = cfvertex[cftriangle[m].v2].x;
		CollisionTriangle[1].y = cfvertex[cftriangle[m].v2].y;
		CollisionTriangle[1].z = cfvertex[cftriangle[m].v2].z;
		CollisionTriangle[2].x = cfvertex[cftriangle[m].v3].x;
		CollisionTriangle[2].y = cfvertex[cftriangle[m].v3].y;
		CollisionTriangle[2].z = cfvertex[cftriangle[m].v3].z;
		colmesh->AddTriangles( CollisionTriangle, 1 );

		IndexList[Index+0] = cftriangle[m].v1;
		IndexList[Index+1] = cftriangle[m].v2;
		IndexList[Index+2] = cftriangle[m].v3;

		//read the normals and set the vertices with the appropriate normal values
		VBList[ cftriangle[m].v1 ].normal.x = cfnormal[ cftriangle[m].n1 ].x;
		VBList[ cftriangle[m].v1 ].normal.y = cfnormal[ cftriangle[m].n1 ].y;
		VBList[ cftriangle[m].v1 ].normal.z = cfnormal[ cftriangle[m].n1 ].z;

		VBList[ cftriangle[m].v2 ].normal.x = cfnormal[ cftriangle[m].n2 ].x;
		VBList[ cftriangle[m].v2 ].normal.y = cfnormal[ cftriangle[m].n2 ].y;
		VBList[ cftriangle[m].v2 ].normal.z = cfnormal[ cftriangle[m].n2 ].z;

		VBList[ cftriangle[m].v3 ].normal.x = cfnormal[ cftriangle[m].n3 ].x;
		VBList[ cftriangle[m].v3 ].normal.y = cfnormal[ cftriangle[m].n3 ].y;
		VBList[ cftriangle[m].v3 ].normal.z = cfnormal[ cftriangle[m].n3 ].z;

		//go to the next 3 indices
		Index+=3;
	}

	colmesh->NextGroup();

	//unlock both buffers
	IB.Unlock();
	VB.Unlock();

	//set the material
	materialIndex = cfmesh.material;
	
	//release them
	delete [] cfvertex;
	delete [] cfnormal;
	delete [] cftriangle;
}


MeshMaterial::~MeshMaterial () { Image = NULL; }


Mesh::Mesh () 
{
	Material = NULL;
	FarthestPoint = 0;
}

Mesh::~Mesh() 
{ 
	Destroy(); 
}

//////// Init /////////////////////////////////
//
// Description:
//
// Creates or recreates an index buffer.
//
///////////////////////////////////////////////

bool Mesh::Create(char* MeshFile, Screen3D& Screen, bool Static)
{

	Destroy();

	//create the file
    FILE *File = fopen (MeshFile, "rb");
    if (!File)
        return false;

	StaticMesh = Static;
	filename = MeshFile;

	//declarations
    int i;
	int vertexindex=0;

	cf_header_t  header;
	cf_mesh      cfmesh;
	cf_material  cfmat;

	Box.min.x = 0;
	Box.min.y = 0;
	Box.min.z = 0;
	Box.max.x = 0;
	Box.max.y = 0;
	Box.max.z = 0;

	FarthestPoint = 0;

	//read the header
	fread (&header, sizeof(cf_header_t), 1, File);

	//make sure the file format is compatible
	if(header.ID != ('S' + ('3' << 8) + ('D' << 16) + ('1' << 24)))
	{
		return false; //incompatible file format
	}

	if(header.MaterialCount > 0)
		Material = new MeshMaterial[header.MaterialCount];	

	nMaterials = header.MaterialCount;

    //read mesh data
    for (i = 0; i < header.MeshCount; i++)
    {
		//read in mesh data
		fread(&cfmesh, sizeof(cf_mesh), 1, File);
		if(cfmesh.nTriangles > 0) {
			MeshGroup* submesh = new MeshGroup(Screen, cfmesh, &ColMesh, &Box, File, Static);
			subMeshes.push_back(submesh);
			if(submesh->radius > FarthestPoint)
				FarthestPoint = submesh->radius;
		}

    }
 
	//read in the materials
	for(i=0; i<header.MaterialCount; i++)
	{
		fread(&cfmat, sizeof(cf_material), 1, File);

		memcpy(&Material[i].AmbientRGBA, cfmat.ambient_rgba, sizeof(Real)*4);
		memcpy(&Material[i].DiffuseRGBA, cfmat.diffuse_rgba, sizeof(Real)*4);
		memcpy(&Material[i].SpecularRGBA, cfmat.specular_rgba, sizeof(Real)*4);
		Material[i].Transparency  = cfmat.transparency;
		Material[i].Shininess     = cfmat.shininess;

		if(cfmat.Texture != NULL)
		{
			//figure out where the path ends and the texture name starts
			int LastSlash  = 0;
		
			for(int si=0; si<strlen(MeshFile); si++)
			{
				if(MeshFile[si] == '\\' || MeshFile[si] == '/')
				{
					LastSlash = si;
				}
			}

			if(LastSlash)
			{
				char Temp[255];
				ZeroMemory(Temp, 255);
				memcpy(Temp, MeshFile, LastSlash+1);
				strcat(Temp, cfmat.Texture);
				strcpy(cfmat.Texture, Temp);

				if( strlen( cfmat.AlphaMap ) > 0 )
				{
					ZeroMemory(Temp, 255);
					memcpy(Temp, MeshFile, LastSlash+1);
					strcat(Temp, cfmat.AlphaMap);
					strcpy(cfmat.AlphaMap, Temp);
				}

			}
			Material[i].Image = Screen.GetTextureHandle(cfmat.Texture, cfmat.AlphaMap);			
		}
		else
		{
			Material[i].Image = -1;
		}
	}

	//close the files
    fclose (File);
	ColMesh.Finalize();

	return true; 
}


void Mesh::Destroy()
{
	for(int i=0; i<subMeshes.size(); i++)
		delete subMeshes[i];

	if(Material)
		delete [] Material;
	Material  = NULL;

	

}



bool Mesh::CheckForCollision( Position& MeshLocation, Vector3D* Line, CollisionInfo* c)
{
	return ColMesh.Collision ( MeshLocation, Line[0], Line[1], c );
}

bool Mesh::CheckForCollision( Matrix& MeshLocation, Vector3D* Line, CollisionInfo* c)
{
	return ColMesh.Collision( MeshLocation, Line[0], Line [1],  c );
}

bool Mesh::CheckForSphereCollision( Position& MeshLocation, Vector3D& point, Real radius, CollisionInfo* c)
{

	return ColMesh.SphereCollision ( MeshLocation, point, radius, c );
	
}

BoundingSphere Mesh::MakeBoundingSphere( Matrix& transform )
{
	BoundingSphere newSphere;
	newSphere.location.Transform(  transform );
	newSphere.Radius = this->FarthestPoint;
	return newSphere;
}

bool Mesh::CheckVisibility( Screen3D& Screen, Position& MeshLocation)
{
	Matrix transform = MeshLocation.GetMatrix();
	//transform.Invert();
	
	BoundingSphere TestSphere;
	for(int i=0; i<GetSubmeshCount(); i++)
	{
		
		TestSphere = GetSubmesh(i)->boundingSphere;
		//TestSphere.location += MeshLocation;
		TestSphere.location.Transform( transform );
		

		if( Screen.SphereInFrustum( TestSphere ) )
			return true;
	}

	return false;

}

void Mesh::Draw(Screen3D& Screen, Position* camera, Position* Location, bool IgnoreAlpha )
{
	Screen.D3DDevice->SetTextureStageState (0,  D3DTSS_COLOROP, D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState (0,  D3DTSS_COLORARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState (0,  D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	Screen.D3DDevice->SetTextureStageState (0,  D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState (0,  D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState (0,  D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	Screen.D3DDevice->SetTextureStageState (1,  D3DTSS_COLOROP, D3DTOP_DISABLE );
	Screen.D3DDevice->SetTextureStageState (1,  D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	Screen.SetVertexShader(NULL);
	

	for(int i=0; i<GetSubmeshCount(); i++)
	{

		if(GetSubmesh(i)->VB.D3DBuffer() == 0 )
			continue;

		if(GetSubmesh(i)->materialIndex >= 0 && Material)
		{
			Screen.SetTexture( Material[GetSubmesh(i)->materialIndex].Image, 0 );
			Material[GetSubmesh(i)->materialIndex].SetOnScreen( &Screen );	
		}
	
		
		Screen.DrawPrimitiveVB(&subMeshes[i]->VB, &subMeshes[i]->IB, D3DPT_TRIANGLELIST, D3DFVF_KEYFRAMEVERTEX);
	}	

}