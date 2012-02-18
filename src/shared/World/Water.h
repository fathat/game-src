// I N C L U D E S /////////////////////////////
#pragma once
#include <windows.h>
#include <list>
#include "..\s3dlib\VertexBuffer.h"
#include "sceneobject.h"

// D E F I N E S ///////////////////////////////
#define WATERDIM 64
#define WATER_LEVEL 120
#define GRID_DIM 8

class WorldManager;

// C L A S S E S ///////////////////////////////

struct GridPoint
{
	int StartX;
	int StartY;
	int EndX;
	int EndY;
	int WaterPoints;
}
;

class WaterEffect
{
protected:
	Real* WaterBuffer;
	Real* WaterBuffer2;
	Real* NewWater;
	Real* OldWater;

	TextureHandle WaterTexture;
	TextureHandle RippleTexture;

	Real Width, Height, Spacing;

	GridPoint* JumpTable[GRID_DIM*GRID_DIM];


public:

	WaterEffect() { WaterBuffer = NULL; WaterBuffer2 = NULL; }
	~WaterEffect() { Cleanup(); }

	void Init( char* TextureName, Screen3D& Screen, WorldManager* wm );
	void Cleanup ();

	void MakeRipple( Real x, Real z, Real Force );
	void UpdateTexture( Real FrameTime, Screen3D& Screen );

	void Draw( Screen3D& Screen );

	
};