// I N C L U D E S ///////////////////////////////////
#pragma once

// D E F I N E S /////////////////////////////////////
#define IMPOSTER_DRAW_BUFFER_CAPACITY 20000

#include "..\s3dlib\screen.h"
#include "..\s3dlib\rendertarget.h"
#include "..\s3dlib\camera.h"
#include "..\mesh\meshmanager.h"
#include <vector>

// C L A S S E S /////////////////////////////////////



class ImposterType
{
protected:
	friend class ImposterManager;
	//the base mesh that this imposter is made from
	MeshHandle hBaseMesh;

	//A list of textures for various viewing angles of the imposter
	RenderTarget* TextureList;
	int nTextures;
	int TextureWidth, TextureHeight;

	RenderTarget OptimizedTexture;
	int nTexturesAcross;
	int nTexturesDown;

	Real WorldHeight;

	//The angle increment between said textures
	Real AngleIncrement; // this should be divisible 
	//out of 360 or else there'll be weird problems

	VertexBuffer SharedVB;
	IndexBuffer  SharedIB;

	//huge gigantic buffer thing
	ParticleVertex DrawBuffer[IMPOSTER_DRAW_BUFFER_CAPACITY];
	int nVertices;


public:
	ImposterType();
	~ImposterType();

	bool GenerateTextures( Screen3D& Screen, MeshManager& MM,  Real angleIncrement,int texWidth, int texHeight, MeshHandle hSourceMesh );
	void Cleanup();

	Texture* GetTexture(int n);
	int GetTextureCount() { return nTextures; }

	void Draw ( Screen3D& Screen, MeshManager& MM,  Camera* Viewer, Position* impLocation );
	void DrawOptimized( Screen3D& Screen, MeshManager& MM,  Camera* Viewer, Position* impLocation );
	bool OutputDrawVertices( Screen3D& Screen, MeshManager& MM, Camera* Viewer, Position* impLocation, ParticleVertex* v );

	int  TexWidth() { return TextureWidth; }
	int  TexHeight() { return TextureHeight; }


	//ultra superfast drawing stuff
	void WriteVertices(  Screen3D& Screen, MeshManager& MM,  Camera* Viewer, Position* impLocation );
	void DrawStoredVertices( Screen3D& Screen, MeshManager& MM );
};


