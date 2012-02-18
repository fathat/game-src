// I N C L U D E S //////////////////////////////////////
#pragma once

// We have to define these or VC6 will
// start telling us it's own headers have problems. >_<
#define _DBSETS_H_
#define _NETWORK_H_

// include all the stuff this file needs
#include <list>
#include <vector>
#include "..\..\shared\imageloader\image.h"
#include "..\s3dlib\VertexBuffer.h"
#include "..\s3dlib\combinationbuffer.h"
#include "..\s3dlib\audio.h"
#include "..\s3dlib\types.h"
#include "..\mesh\meshmanager.h"
#include "..\s3dlib\game timer.h"
#include "avatar.h"
#include "sceneobject.h"
#include "weather.h"
#include "water.h"
#include "lightmanager.h"


// D E F I N E S ////////////////////////////////////////

class Sky;
class NetworkManager;


#define SCALE 1 //how much to scale the map's scale by.
#define HEIGHT_SCALE 8 //how much to scale upwards


#define SceneHandle int

#define MAX_STEP_HEIGHT .6f //slope range that player can climb up.
#define GRAVITY         100
#define FRICTION        100
#define MIN_OBJ_DISTANCE 4


#define MAX_OBJECTS     1024
#define MAX_DRAWABLE_PARTICLES 512

class WorldManager;

//structure for loading objects from a file
#pragma pack(1)
struct Object
{
	BYTE Type;
	Real x, y;
	short altitude;
	short  yaw;
	char   name[4];
};
#pragma pack(8)


struct QuadTextureGroup
{
	VertexBuffer VB;
	IndexBuffer  IB;
	TextureHandle hTexture;
};



// C L A S S E S ///////////////////////////////////////////////


struct QuadNode
{
    // all the objects that are inside of this tree node
	std::vector<SceneHandle> SceneList;
	

	//map variables
	VertexBuffer* MapVB;
	Texture QuadTexture; //<-- Note that this is NOT a handle, because
						// each quad node had its own texture, so
						// storing its texture globablly would be useless
	bool Invisible;

	// New geometry stuff

	//Finalized results
	QuadTextureGroup CliffData;
	QuadTextureGroup GrassData;

	std::vector<int> GrassVertices;
	std::vector<int> GrassIndices;
	std::vector<int> CliffVertices;
	std::vector<int> CliffIndices;
		
	// Bounding box, used for tests
	AABB Box;
	AABB ObjectBox;

	Matrix FluidMatrix;
	
	// Pointers to children at one side or the other (if there are
	// any, otherwise it's NULL).
	QuadNode* TopLeft, *TopRight, *BotLeft, *BotRight;
    int iTL, iTR, iBL, iBR;

	
	bool GenerateTexture ( Screen3D& Screen,WorldManager* WM,  TextureHandle Cliff, TextureHandle Ground, 
							TextureHandle Beach, TextureHandle Underwater );
	void MakeDent(int vx, int vy, Real Amount, WorldManager* WM);
	void CalcFluidMatrix ( WorldManager* WM );

	bool InitializeGeometry( WorldManager* WM);

	
	//checks to see if this node is completly obscured by the
	//"obscuring map", and if it is we just call it invisible and do nothing
	//with it.
	void CalculateVisibility( WorldManager* WM, WorldManager* ObscuringWM );

	void Cleanup()
	{
		if(TopLeft) TopLeft->Cleanup(); 
		if(TopRight) TopRight->Cleanup();
		if(BotLeft) BotLeft->Cleanup();
		if(BotRight) BotRight->Cleanup();

		if(MapVB) delete MapVB;
		MapVB= NULL;
	}

	QuadNode() { TopLeft = NULL; TopRight = NULL; BotLeft = NULL; BotRight = NULL; MapVB = NULL; Invisible=false; }
	~QuadNode() { 
		Cleanup();
	}
};


struct QuadNodePtr
{
	QuadNode* Node;
};

//////// WorldManager /////////////////////////
//
// Description:
//
// Holds all the objects in a scene, as well as
// the map data.
//
// Known Problems:
//
// - [none]
//
// TODO:
//
// - [none]
//
///////////////////////////////////////////////

class WorldManager
{
public:
	
	//list of game objects. An STL Map would
	//probably be a better match, but this
	//works reasonably well
	SceneObject* ObjectList[MAX_OBJECTS];
	int nObjects;

	std::vector<std::string> ObjectTypeList;

	//This is the current active camera
	Camera Viewer;

	//dimensions
	int Width,//width of map, must be equal to height and a power of 2
		Height,//height of map, must be equal to width and a power of 2
		Spacing;//the x,z coords are evenly spaced by this member

	Real* Map; 

	int NodeWidth, NodeHeight;

	


	//Texture stuff //

	//Old texture stuff, semi-obsolete
	TextureHandle ColorMap;			//[OBSOLETE]
	TextureHandle DetailMap;
	TextureHandle DetailMapSigned;
	TextureHandle ObjectDetailMap;	//[TODO]
	
	//New texturing stuff, should be replaced soon however :O
	int QuadTexWidth, QuadTexHeight; //[OBSOLETE]
	int* TexBuffer;					 //[OBSOLETE]
	CGenImage CliffTexture;			 //[OBSOLETE]
	CGenImage GrassTexture;			 //[OBSOLETE]
	TextureHandle hGrassTexture;
	TextureHandle hCliffTexture;
	TextureHandle hGrassCliffTexture;

	char ObjectMap[255];
	
	//what alpha mode to use when drawing?
	_D3DBLEND SrcAlpha;
	_D3DBLEND DestAlpha;

	bool DrawSky;
	bool NoCulling;
	bool FluidMap;
	bool SolidGround;
	bool UseAvatar;
	IndexBuffer IB; //index buffer used for each node of the map. Obsolete!!``~~
	

	// Tree data
	QuadNode* NodeList;
	int nNodes;
	int CurrentIndex;
	QuadNode* Tree;
	int CurrentDepth;
	int MaxDepth;    

	//an array of pointers to the nodes at the very
	//farthest level down in the tree. You just access
	//these like any other 1d array facading as a
	//2d array ([x+y*Width])
	QuadNodePtr* NodeReferenceList;
	int nAcross, nDown;


	// Statistics 
	int ObjectsDrawn;
	int ObjectsTested;
	int RecursionCount;

	// Stored pointers to important objects so that we don't need to
	// request them in each function call.
	Screen3D* ScreenPtr;
	MeshManager* MMPtr;
	GameTimer* GTPtr;
	SoundManager* AMPtr; 
	NetworkManager* NMPtr;

	//Buffer for the particles that are going to be drawn and something something
	//and such.
	ParticleStream* ParticleBuffer[512]; //faceman pities the foo who uses more than
										 //500 particles in one frame.

	//Water stuff
	WaterEffect Water; //[OBSOLETE]
	WorldManager* ParentWM; //usually null, unless this is a water map
	WorldManager* WaterWM; //a map inside a map for watar!11

	//weather
	Weather WorldWeather;
	ParticleStream DustParticles;
	LightManager LM;

	//oh teh no! si teh playar???
	Avatar      LocalPlayer;

	//Movie stuff
	bool InMovie;
	std::string TopMovieText;
	std::string BottomMovieText;

	//Win condition state
	Real Minutes;
	Real Seconds;
	bool WinConditionsMet;

	//Various Object Lists
	SceneObject* DynamicObjectList[MAX_OBJECTS];
	std::vector<SceneObject*> NetworkObjectList;

	
public:

	//Constructor/destructor
	WorldManager() {
		for(int i=0; i<MAX_OBJECTS; i++)
		{
			ObjectList[i] = NULL;
			DynamicObjectList[i] = NULL;
		}
		nObjects = 0; 
		Map = NULL;
		SrcAlpha = D3DBLEND_SRCALPHA;
		DestAlpha = D3DBLEND_INVSRCALPHA;
		DrawSky= true;
		NoCulling = false;
		UseAvatar = true;
		FluidMap  = false;
		SolidGround = true;
		QuadTexWidth  = 128;
		QuadTexHeight = 128;
		TexBuffer     = NULL;
		WaterWM = NULL;
		ParentWM = NULL;
	}
	~WorldManager() { Cleanup(); }

	// Init/shutdown
	void Init( Screen3D& Screen, char* MapDesc, MeshManager* MM, SoundManager& AM, GameTimer* Timer, NetworkManager* NM, WorldManager* Parent );
	void Cleanup();

	// map functions
	bool LoadMap( char* MapDesc );
	bool LoadHeightmap( char* bitmap );
	bool LoadObjects( char* MapFile );
	bool LoadBaseTextures( char *MapDesc );
	void RestartLevel();

	void CreateVertexbuffer( QuadNode* Node );
	void CreateContourMap ( );
	
	void SetLandscapeBlend( _D3DBLEND srcAlpha, _D3DBLEND destAlpha ) { 
		SrcAlpha = srcAlpha; DestAlpha = destAlpha ;
	}

	void SetMusic ( char * MusicFile );

	// Landscape deformation!
	void MakeDent( int vx, int vy, Real Amount);
	void MakeBlast( Real WorldX, Real WorldY, Real Radius );
	void KickupDust( Real WorldX, Real WorldZ, Real Radius );

	// Object Management functions
	SceneHandle Add ( SceneObject* object );
	void Kill ( SceneHandle Who );	
	SceneObject* GetObjectPtr(SceneHandle h) { return ObjectList[h]; }

	// Dynamic Object Management
	SceneHandle AddDynamic( SceneObject* object );
	void KillDynamic( SceneHandle Who );
	SceneObject* GetDynamicObject( SceneHandle h ) { return DynamicObjectList[h]; }
	void DrawDynamicObjects( Screen3D& Screen );
	SceneObject* FindNearestObject( Position& Location, Real MaxDistance );

	// Network Object Management
	SceneHandle AddNetEnt( M3DAddEntPacket* NewEnt  );
	void KillNetEnt( SceneHandle Who );
	SceneObject* GetNetEnt( SceneHandle h );
	void DrawNetworkObjects( Screen3D& Screen );

	//Particle functions
	void AddStream( ParticleStream* stream );
	void ClearStreamList( );

	// Functions for the binary tree. 
	void BuildTree( QuadNode* Parent, QuadNode* NewNode, bool Left, bool Top );
	void _fastcall DrawTree ( QuadNode* Node, bool ParentFullyVisible );

	// Draw and update, both need to be called each frame
	void Update( Real FrameTime ); //call this every frame.
	void Draw( Screen3D& Screen );
	void DrawWater( Screen3D& Screen );
	void DrawFinal( Screen3D& Screen );
	

	bool CheckForCollision( Vector3D& p1, Vector3D& p2, CollisionInfo* Collision, bool CheckPlayer=true );
	bool CheckForSphereCollision( Vector3D& p1, Real radius, CollisionInfo* Collision, bool CheckPlayer=true );
	bool CheckNodeForCollision(  Vector3D& p1, Vector3D& p2,  CollisionInfo* Collision, QuadNode* Node );
	bool CheckNodeForSphereCollision(  Vector3D& p1, Real radius,  CollisionInfo* Collision, QuadNode* Node );
	Real GetHeightValue( Position &ObjectLocation, Real* Slope=NULL, Vector3D* SlopeNormal=NULL, D3DXPLANE* TPlane=NULL, Vector3D* RoundSlopeNormal=NULL  );

	
};

