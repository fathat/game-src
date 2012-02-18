#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

// D E F I N E S ///////////////////////////////////////////////////
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_NORMAL |D3DFVF_TEX2)
#define D3DFVF_KEYFRAMEVERTEX (D3DFVF_XYZ| D3DFVF_NORMAL |D3DFVF_TEX1)
#define D3DFVF_LANDSCAPEVERTEX (D3DFVF_XYZ| D3DFVF_NORMAL |D3DFVF_TEX4)
#define D3DFVF_PRELITVERTEX (D3DFVF_XYZ| D3DFVF_DIFFUSE |D3DFVF_TEX2)
#define D3DFVF_PARTICLEVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define D3DFVF_SCREENVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2)
#define D3DFVF_POINTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#define D3DFVF_TREEVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )
#define D3DFVF_WATERVERTEX (D3DFVF_XYZ |D3DFVF_DIFFUSE| D3DFVF_TEX1 )
#define D3DFVF_TERRAINLODVERTEX (D3DFVF_XYZ | D3DFVF_TEX3 )


// C L A S S E S ///////////////////////////////////////////////////


struct PointVertex
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;

	PointVertex( D3DXVECTOR3& _v, D3DCOLOR _c )
	{
		v	  = _v;
		color = _c;
	}

	PointVertex()
	{
		color = 0;
	}
};



// A structure for our custom vertex type. 
struct CustomVertex
{
    D3DXVECTOR3 position; // The position
	D3DXVECTOR3 normal;   // Normal
    FLOAT       tu, tv;   // The texture coordinates
	FLOAT       tu2, tv2; // Second set of texture coordinates

	CustomVertex() { 
		ZeroMemory(this, sizeof(CustomVertex));
	}
};

struct WaterVertex
{
    D3DXVECTOR3 position; // The position
	D3DCOLOR    color;
    FLOAT       tu, tv;   // The texture coordinates

	WaterVertex() { 
		ZeroMemory(this, sizeof(WaterVertex));
		color = D3DCOLOR_ARGB( 128, 255, 255, 255 );
	}
};

// A structure for our custom vertex type. 
struct LandscapeVertex
{
    D3DXVECTOR3 position; // The position
	D3DXVECTOR3 normal;   // Normal
    FLOAT       tu, tv;   // The texture coordinates
	FLOAT       tu2, tv2; // Second set of texture coordinates
	FLOAT       tu3, tv3; // Third set of texture coordinates
	FLOAT       tu4, tv4; // Third set of texture coordinates

	LandscapeVertex() { 
		ZeroMemory(this, sizeof(LandscapeVertex));
	}
};

struct PrelitVertex
{
    D3DXVECTOR3 position; // The position
	D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
	FLOAT       tu2, tv2; // Second set of texture coordinates

	PrelitVertex() { 
		ZeroMemory(this, sizeof(PrelitVertex));
	}
};

//struct MeshVertex
//{
	//D3DXVECTOR3 Position;
	//D3DXVECTOR3 Normal;
	//D3DXVECTOR2 Texture;
//};

struct TreeVertex
{
    D3DXVECTOR3 position; // The position
	D3DXVECTOR3 normal;    // The color
	FLOAT tu, tv;

	TreeVertex() { 
		ZeroMemory(this, sizeof(TreeVertex));
	}
};


struct ScreenVertex
{
    Real x, y, z; // The position
	Real RHW;
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
	FLOAT       tu2, tv2;   // The texture coordinates


	ScreenVertex() { 
		ZeroMemory(this, sizeof(ScreenVertex));
		RHW = 1;
		color = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	}

	ScreenVertex( float _x, float _y, float _z, D3DCOLOR _color, float _tu, float _tv )
	{
		x = _x;
		y = _y;
		z = _z;
		color = _color;
		tu = _tu;
		tv = _tv;
		tu2 = 0;
		tv2 = 0;
		RHW = 1;

	}
};



struct KeyframeVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	FLOAT tu, tv; 
};


// A structure for our particle vertex type. 
struct ParticleVertex
{
    D3DXVECTOR3 position; // The position
	D3DCOLOR    color;    // The color
	FLOAT tu, tv;

	ParticleVertex() { 
		ZeroMemory(this, sizeof(ParticleVertex));
	}
};

struct TerrainLODVertex
{
	D3DXVECTOR3 v;
	D3DXVECTOR2 tex0;
	D3DXVECTOR2 tex1;
	D3DXVECTOR2 tex2;

	TerrainLODVertex()
	{
		ZeroMemory( this, sizeof(TerrainLODVertex));
	}

	TerrainLODVertex( D3DXVECTOR3& _v)
	{
		ZeroMemory( this, sizeof(TerrainLODVertex));
		v = _v;
	}
};