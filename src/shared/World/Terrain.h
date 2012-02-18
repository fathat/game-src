#pragma once

#include "terrainlayer.h"
#include "quadtree.h"
#include "..\TerrainLOD\TerrainMap.h"

class WorldManager;

class Terrain
{
protected:

	//pointer to parent world manager
	WorldManager* WMPtr;

	Screen3D* ScreenPtr;

	//dimensions
	int Width, //horizontal resolution of heightmap, must be equal to height and a power of 2
		Height,//vertical resolution of heightmap, must be equal to width and a power of 2
		Spacing;//the x,z coords are evenly spaced by this member

	std::vector< TerrainLayer* > LayerList;


	TerrainMap* terrainMap;
	HeightGrid* heightGrid;

	IndexBuffer IB; //index buffer used for each node of the map. 	

	//Heightmap array
	Real* Map; 

	//ODE geometric representation of the terrain
	dHeightfieldDataID heightid;
	dGeomID TerrainGeom;
	
	int NodeWidth, NodeHeight;
	Real LowestHeight;
	Real HighestHeight;

	//an array of pointers to the nodes at the very
	//farthest level down in the tree. You just access
	//these like any other 1d array facading as a
	//2d array ([x+y*Width])
	QuadNodePtr* NodeReferenceList;
	int nAcross, nDown;

	// Tree data
	QuadNode* NodeList;
	int nNodes;
	int CurrentIndex;
	QuadNode* Tree;
	int CurrentDepth;
	int MaxDepth;   

	//TODO: I should handle shaders better than this.
	
	IDirect3DPixelShader9* TerrainShader;

	VertexShader m_grassVertexShader;
	std::vector<QuadNode*> DrawQueue;


	bool DrawGrass;

	//New map loading functions
	bool LoadHeightmapDotBin( const char* binfile );
	bool LoadTextureLayers( const char* filename );

	void SetupDefaultValues();
	bool InitTerrainShader();
	void SetShaderValues();

	void CreateVertexbuffer( QuadNode* Node );

	// Functions for the quad tree. 
	void BuildTree( QuadNode* Parent, QuadNode* NewNode, bool Left, bool Top );
	void _fastcall DrawTree ( QuadNode* Node, bool ParentFullyVisible );


	


public:
	Terrain( WorldManager* wm, const char* heightmap, const char* layerfile) ;
	~Terrain(); 

	//accessors
	int GetWidth()  { return Width;  }
	int GetHeight() { return Height; }
	int GetSpacing(){ return Spacing;}

	int GetNodeWidth() { return NodeWidth; }
	int GetNodeHeight() { return NodeHeight; }

	int GetNodesAcross() { return nAcross; }
	int GetNodesDown() { return nDown; }

	float GetLightValue( float wx, float wz );

	Screen3D* GetScreen() { return ScreenPtr; }

	float* GetHeightmapArray() { return  Map;}

	void Update();
	void UpdateLOD( Vector3D* camera );

	void Draw();

	bool CheckTerrainCollision (Vector3D a, Vector3D b, Vector3D* oPos, Vector3D* oNormal);

	float GetLayerVisibility( int layerIndex, float wx, float wz ) ;
	
	Real GetHeightValue( Position &ObjectLocation, Real* Slope=NULL, Vector3D* SlopeNormal=NULL, D3DXPLANE* TPlane=NULL, Vector3D* RoundSlopeNormal=NULL );

	void InitializeGeom();

	void ShowGrass( bool b ) { DrawGrass = b; }
	bool ShowGrass() { return DrawGrass; } 

};
