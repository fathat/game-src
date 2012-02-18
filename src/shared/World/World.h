// I N C L U D E S //////////////////////////////////////
#pragma once

// include all the stuff this file needs
#include <list>
#include <vector>

#include "..\\imageloader\image.h"
#include "..\s3dlib\VertexBuffer.h"
#include "..\s3dlib\audio.h"
#include "..\s3dlib\types.h"
#include "..\gamegui\progresswidget.h"
#include "avatar.h"
#include "vegetationmanager.h"
#include "weather.h"
#include "terrainlayer.h"
#include "quadtree.h"
#include "terrain.h"
#include "ImposterManager.h"
#include "Rain.h"

// D E F I N E S ////////////////////////////////////////

//forward declarations
struct CollisionInfo;
class NetworkManager;
class Sky;
class SceneManager;
class SoundManager;
class VegetationManager;


#define SCALE 1 //how much to scale the map's scale by.
#define HEIGHT_SCALE 4 //how much to scale upwards


#define SceneHandle int

#define MAX_STEP_HEIGHT .6f //slope range that player can climb up.
#define GRAVITY         100
#define FRICTION        200
#define MIN_OBJ_DISTANCE 4


#define MAX_OBJECTS     1024
#define MAX_DRAWABLE_PARTICLES 512

class WorldManager;

// C L A S S E S ///////////////////////////////////////////////


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
//
///////////////////////////////////////////////

class WorldManager
{

public:

	// Stored pointers to important objects so that we don't need to
	// request them in each function call.
	Screen3D*       ScreenPtr;
	MeshManager*    MMPtr;
	GameTimer*      GTPtr;
	SoundManager*   AMPtr; 
	NetworkManager* NMPtr;

	ImposterManager m_imposterManager;


	//The scene manager manages objects in the game (characters, items, etc.)
	SceneManager* sceneManager;

	//This is the current active camera
	Camera Viewer;
	Camera SavedCamera; // used to restore old camera when we're doing a
						// lot of camera work (ie, in movies and such)

	DynamicsSolver Solver;

		//TODO: Maybe have a weather manager for this stuff?
	Weather WorldWeather;
	bool DrawSky;
	ParticleStream DustParticles;
	Rain* rain;

	//Lightmap generating stuff
	LightManager LM;
	
	//yarrr
	Avatar      LocalPlayer;
	SceneObject* InputFocus;

	DWORD TextureFilterMode;
	

	//Map name, and mod name
	std::string MapName;
	std::string ModName;


	//Vegetation manager, handles all the vegetation in the level.
	VegetationManager VegMan;

	
	Terrain* terrain;
	float Spacing;

	//A listing of the filenames of the various object types.
	std::vector<std::string> ObjectTypeList;

	

	//water
	LPDIRECT3DTEXTURE9      WaterBumpMap;
	D3DXMATRIXA16           WaterBumpMatrix;
	Texture* waterProjectionTexture;
	bool AllowWaterDraw;
	Real SeaLevel;

	

	//heightmap for the water
	Real* WaterMap;
	
	bool TimeDilated;
	Real DilationPercent;

	//bool StartedDeathSequence;
	//float RespawnTime;
	float TimeLeft;

	//TODO: Figure out what the hell this is for

	int TexWeightWidth;
	int TexWeightHeight;

	TextureHandle hWaterTexture;

	bool UseAvatar;
	
	

	bool DrawingWater;
	bool DrawingReflection;
	int WaterDamage;

	// Statistics 
	int ObjectsDrawn;
	int ObjectsTested;
	int RecursionCount;


	//Loading screen stuff (I think)
	std::string LoadString;

	
	//Buffer for the particles that are going to be drawn and something something
	//and such.
	ParticleStream* ParticleBuffer[MAX_DRAWABLE_PARTICLES]; 

	//Water stuff
	VertexBuffer SeaVB;
	IndexBuffer SeaIB;
	int SeaDensity;

	//Loading state
	float LoadPercent; 
	std::string LoadText;
	ProgressWidget progressWidget;


	bool LoadHeaderInfo( char* projectdotxml );
	bool LoadObjectsXML( char* filename );
	bool LoadEffectFile ( char* filename );

	//private initialization functions
	bool InitWaterShader();

	void InitSeaGeometry();


public:

	//Constructor/destructor
	WorldManager( Screen3D* screenptr, MeshManager* mmptr, SoundManager* am, GameTimer* timer, NetworkManager* nm);
	~WorldManager() { Cleanup(); }

	// Init/shutdown
	bool InitXML( const char* mod, const char* map );
	void Cleanup();

	// map functions
	void RestartLevel();

	//new map loading functions! HOTHOTHOT
	bool LoadMapXML(  std::string& mod, std::string& mapname  );

	void SetMusic ( char * MusicFile );

	ImposterManager* GetImposterManager() { return &m_imposterManager; }
	
	// Object Management functions
	SceneHandle Add ( SceneObject* object );
	SceneHandle Add ( std::string& type, std::string& typePath, std::string& scriptName, Vector3D& location, Vector3D& rotation );
	SceneObject* MakeObject( std::string& type, std::string& typePath, std::string& scriptName, Vector3D& location, Vector3D& rotation );
	void Kill ( SceneHandle Who );	
	
	SceneObject* GetDynamicObject( SceneHandle h );
	SceneObject* FindNearestObject( Position& Location, float MaxDistance , SceneObject* objectToIgnore=NULL);
	SceneHandle GetScriptObjectHandle( char* name );

	bool InWater( Position &ObjectLocation, Vector3D* WaterFlow=NULL );
	Real GetPlayerDistance( Position& Location );

	//Particle functions
	void AddStream( ParticleStream* stream );
	void ClearStreamList( );

	// Draw and update, both need to be called each frame
	void Update( Real FrameTime ); //call this every frame.
	void Draw( Screen3D& Screen );
	void DrawSea( Screen3D& Screen );
	void DrawBasicSea();
	void DrawFinal( Screen3D& Screen );
	void DrawReflectedScene( Screen3D& Screen );

	//Accessors
	int GetWidth() { return terrain->GetWidth(); }
	int GetHeight() { return terrain->GetHeight(); }
	int GetSpacing() { return terrain->GetSpacing(); }

	int GetNodeWidth() { return terrain->GetNodeWidth(); }
	int GetNodeHeight() { return terrain->GetNodeHeight(); }

	float* GetHeightmapArray() { return terrain->GetHeightmapArray() ;}

	void KickupDust( Real WorldX, Real WorldZ, Real Radius );

	Real GetHeightValue( Position &ObjectLocation, Real* Slope=NULL, 
		Vector3D* SlopeNormal=NULL, 
		D3DXPLANE* TPlane=NULL, 
		Vector3D* RoundSlopeNormal=NULL, 
		bool CheckWaterSurface=false  ) 
	{ 
		return terrain->GetHeightValue( ObjectLocation,Slope, SlopeNormal, TPlane, RoundSlopeNormal ) ;
	}

	bool CheckBulletCollision( Vector3D a, Vector3D b, Vector3D* outPoint, Vector3D* outNormal,  SceneObject** objectHit, SceneObject* objectToIgnore );

	bool CheckForCollision( Vector3D& p1, Vector3D& p2, CollisionInfo* Collision, bool CheckPlayer=true, bool LightCheck=false );
	
};

