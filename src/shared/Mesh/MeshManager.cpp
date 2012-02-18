#include "MeshManager.h"
#include "..\s3dlib\screen.h"
#include <vector>


MeshManager::MeshManager () 
{ 
	ZeroMemory(MeshList, MAX_MESHES * sizeof(Mesh*)); 
	vertexShader = NULL;  
	ScreenPtr = NULL; 
	Clear();    
}

MeshManager::~MeshManager() 
{ 
	Destroy(); 
}


//////// Clear ////////////////////////////////
//
// Description:
//
// Deletes anything left in the list and sets
// it to null.
//
///////////////////////////////////////////////
void MeshManager::Clear()
{
	if(vertexShader != NULL && ScreenPtr )
	{
		vertexShader->Release();
		vertexShader = NULL;
	}
	

	for(int i=0; i<MAX_MESHES; i++)
	{
		if(MeshList[i] != NULL)
			delete MeshList[i];

		MeshList[i] = NULL;
	}
	
}


//////// InitShader ///////////////////////////
//
// Description:
//
// Initializes the shader something something!
//
///////////////////////////////////////////////
bool MeshManager::InitShader()
{
	
	D3DVERTEXELEMENT9 shaderDeclaration[] = 
	{
		//stream 1
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL  , 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		//stream 2
		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
		{ 1, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL  , 1 },
		{ 1, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		D3DDECL_END()
	};
	
	
	ScreenPtr->D3DDevice->CreateVertexDeclaration( shaderDeclaration, &m_shaderDeclaration);
	
	//Run the shader through the compiler
	LPD3DXBUFFER pCode;
	LPD3DXBUFFER pErrors;
	
	HRESULT HR = D3DXAssembleShaderFromFileA( "..\\scripts\\shaders\\keyframelinear.nvv", NULL, NULL, NULL, &pCode, &pErrors );

	if ( FAILED(HR) )
	{
		OutputDebugString( "Failed to assemble the vertex shader, errors:\n" );
		OutputDebugString( (char*)pErrors->GetBufferPointer() );
		OutputDebugString( "\n" );
	}
	else
	{


		//Create the shader!
		if(FAILED( ScreenPtr->D3DDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &vertexShader ) ) )
		{
			//OH NO!
			pCode->Release();
			return false;
		}
		
		pCode->Release();
	}

	
	

	//Hooray!
	return true;
}

//////// Destroy //////////////////////////////
//
// Description:
//
// Releases all allocated objects.
//
///////////////////////////////////////////////

void MeshManager::Destroy()
{
	Clear();
}


//////// Add //////////////////////////////////
//
// Description:
//
// Adds a mesh to the mesh manager.
//
///////////////////////////////////////////////

MeshHandle MeshManager::Add ( char* Filename, bool Static )
{

	//create the new Mesh
	Mesh* NewMesh = new Mesh;
	if(NewMesh->Create(  Filename,  *ScreenPtr, Static ) != true)
	{
		delete NewMesh;
		return -1;
	}
	

	//loop through and find an empty spot to place it
	for(int i=0; i<MAX_MESHES; i++)
	{
		//blank spot?
		if(MeshList[i] == NULL )
		{
			//assign the Mesh a spot
			MeshList[i] = NewMesh;
			
			//done, return the handle
			return i;
		}

	}

	return -1;
}



//////// Remove ///////////////////////////////
//
// Description:
//
// Deallocates a mesh.
//
///////////////////////////////////////////////

void MeshManager::Remove ( MeshHandle ID )
{
	//destroy the Mesh
	if(MeshList[ID] != NULL)
		delete MeshList[ID];

	MeshList[ID] = NULL;

}


//////// Exists ///////////////////////////////
//
// Description:
//
// Checks to see if a mesh exists, and returns
// its handle if it does. Otherwise it returns
// -1.
//
///////////////////////////////////////////////


MeshHandle MeshManager::Exists ( char* MeshName )
{
	//loop through all the Meshs to see if one
	//with the same name exists
	for(int i=0; i<MAX_MESHES; i++)
	{
		//if the Mesh exists in this spot..
		if(MeshList[i])
		{
			int RVal = strcmpi(MeshName, MeshList[i]->GetFilename());
			
			//did we find a Mesh with the same name?
			if(RVal == 0)
				return i;
		}
	}


	//nope, didn't find the Mesh
	return -1;

}


