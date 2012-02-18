// I N C L U D E S //////////////////////////////////////
#pragma once

// include all the stuff this file needs
#include <list>
#include <vector>

#include "..\\imageloader\image.h"
#include "..\s3dlib\VertexBuffer.h"
#include "..\s3dlib\audio.h"
#include "..\s3dlib\types.h"
#include "avatar.h"
#include "vegetationmanager.h"
#include "weather.h"
#include "terrainlayer.h"
#include "..\terrainlod\terrainpatch.h"
#include "Grass.h"

// D E F I N E S ////////////////////////////////////////

class Terrain;

struct QuadNode
{

	//Geometry
	VertexBuffer* MapVB;

	TerrainPatch* Patch;
	
	//texture stuff
	Texture AlphaMask[4]; //<-- Note that this is NOT a handle, because
						// each quad node had its own texture, so
						// storing its texture globally would be useless
	bool Invisible;
		
	// Bounding box, used for tests
	AABB Box;
		
	// Pointers to children at one side or the other (if there are
	// any, otherwise it's NULL).
	QuadNode* TopLeft, *TopRight, *BotLeft, *BotRight;
    int iTL, iTR, iBL, iBR;

	Terrain* m_terrain;
	GrassPatch m_grass;


	QuadNode();
	~QuadNode() ;
	
	bool GenerateTexture ( Screen3D& Screen,WorldManager* WM,  TextureHandle Cliff, TextureHandle Ground, 
							TextureHandle Beach, TextureHandle Underwater );
	bool InitializeGeometry( Terrain* terrain);
	void InitGrass();
	void Cleanup();

	Vector3D Center() ;
	float DiagonalSize();
};


struct QuadNodePtr
{
	QuadNode* Node;
};
