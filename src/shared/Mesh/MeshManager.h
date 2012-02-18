#pragma once

// I N C L U D E S ////////////////////////////////////////////////////////
#include <vector>
#include "Mesh.h"



// D E F I N E S //////////////////////////////////////////////////////////
#ifndef MeshHandle
#define MeshHandle int
#endif

#define MAX_MESHES 256



// C L A S S E S //////////////////////////////////////////////////////////


//////// MeshManager //////////////////////////
//
// Description:
//
// Responsible for loading and keeping track
// of all mesh objects. That way there aren't
// problems with resource sharing and whatnot.
//
// Known Problems:
//
// - None
//
// TODO:
//
// - [None]
//
//
///////////////////////////////////////////////


class MeshManager
{
protected:
	//we're using a fixed size array since it's easier
	//to index into these with a handle (compared to a linked
	//list).
	Mesh* MeshList[MAX_MESHES];
	
	IDirect3DVertexShader9* vertexShader;
	IDirect3DVertexDeclaration9* m_shaderDeclaration ;

	//Local pointer to the screen.
	Screen3D* ScreenPtr;


		
public:

	//constructor/destructor
	MeshManager () ;
	~MeshManager() ;


	//Initializes the class for use
	void Init ( Screen3D* Screen ) { ScreenPtr = Screen; InitShader(); }
	bool InitShader();


	//Functions for cleanup
	void Clear ( ) ;
	void Destroy ( );

	IDirect3DVertexShader9* GetShader () { return vertexShader; }
	IDirect3DVertexDeclaration9 * GetShaderDeclaration() { return m_shaderDeclaration; }


	//Add a mesh to the list
	MeshHandle  Add ( char* Filename, bool Static=false ); //returns an ID for the Mesh
	void Remove ( MeshHandle ID ) ; //remove the Mesh with specified ID


	MeshHandle GetMeshHandle ( char* Filename, bool Static=true ) {
		MeshHandle ReturnValue = Exists(Filename);

		if(ReturnValue >= 0)
			return ReturnValue;

		return Add( Filename, Static );

	}

	//returns a pointer to the mesh with the specified handle, if
	//mesh doesn't exist, returns NULL.
	Mesh* Get ( MeshHandle ID ) {
		if(ID >= 0 && ID < MAX_MESHES)
			return MeshList[ID]; 
		else
			return NULL;
	}

	//checks to see if a mesh exists
	TextureHandle Exists(char* MeshName);


};