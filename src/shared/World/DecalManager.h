// I N C L U D E S /////////////////////////////////////
#pragma once
#include "world.h"
#include "..\collision\collision.h"

// D E F I N E S ///////////////////////////////////////
#define MAX_DECALS 200
class WorldManager;

// C L A S S E S ///////////////////////////////////////


//Decal that goes in a decal manager list
struct Decal
{
    CustomVertex VertexList[4];

	TextureHandle texture;

	Real TimeAdded;
};


//Handles all the "decals" (bullet holes, blood splatters, etc.) in the level
class DecalManager
{
protected:
	Decal* DecalList[200];

public:

	DecalManager() { ZeroMemory(DecalList, 200*4); }
	~DecalManager() { Clear(); }

	void AddDecal(   CollisionInfo* collision, TextureHandle T, Real FrameTime );
	void Clear();

	void DrawAll( Screen3D& Screen );
};