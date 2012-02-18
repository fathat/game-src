#pragma once

// I N C L U D E S ////////////////////////////////////////
#include <vector>
#include "..\shared\s3dlib\screen.h"
#include "..\shared\s3dlib\rendertarget.h"
#include "..\shared\s3dlib\input.h"
#include "..\shared\s3dlib\game timer.h"
#include "..\shared\s3dlib\font.h"
#include "..\shared\s3dlib\audio.h"
//#include "..\shared\s3dlib\soundmanager.h"
#include "..\shared\network\messages.h"
#include "..\shared\network\socket.h"
#include "..\shared\s3dlib\mp3.h"
#include "..\shared\mesh\mesh.h"
#include "..\shared\mesh\animatedmesh.h"
#include "..\shared\mesh\meshmanager.h"
#include "..\shared\gamegui\InputBox.h"
#include "..\shared\world\world.h"
#include "..\shared\world\particle.h"
#include "..\shared\world\sky.h"
#include "..\shared\world\avatar.h"
#include "..\shared\world\weather.h"
#include "..\shared\world\weapon.h"
#include "..\shared\world\water.h"
#include "..\shared\CVAR\CVAR.h"

#include "baseapp.h"
#include "network.h"
#include "guihandler.h"
#include "..\shared\os\ssl.h"

class ScriptAPI;


// D E F I N E S //////////////////////////////////////////
#define UPDATE_INTERVAL 200

// C L A S S E S //////////////////////////////////////////


//////// GameApp //////////////////////////////
//
// Description:
//
// Data center for global variables, contains
// main functions. (with exception of WinMain).
//
// Known Problems:
//
// - None
//
// TODO:
//
// - Add in joystick support, buffer input
//
//
///////////////////////////////////////////////



class GameApp : public BaseApp
{
public:

	friend class ScriptAPI;
	
	//Movie stuff  (This should be moved to CVARS)
	bool MovieSkipFlag;
	bool InMovie;
	std::string TopMovieText;
	std::string BottomMovieText;
	
	//references to device objects
	Screen3D&  Screen;
	Input&     InputSystem;
	SoundManager& Sound;

	//Timer stuff
	GameTimer Timer;
	Real   FrameTime;
	
	//HI!
	//NetworkManager NM;
	HatKernel GameOS;
	bool AlwaysActive;
	bool ShareCPU;

	WorldManager* WM;
	

	RenderTarget ScreenTexture;
	RenderTarget AccumulationBuffer;
	
	LPDIRECT3DSURFACE9 ScratchDepth;
	TextureHandle BlurTexture, InverseBlurTexture;

	float ScreenDarkLevel;
	
	//stuff for the intro
	bool DrawWorld;
	
    char MapName[255];
	
	bool Zoomed;
	
	bool DrawReflection;

	//gui
	GameGUI UserInterface;
	
	//Debug Variables
	bool DebugFieldOfView;
	bool ShowSecondMap;
	bool ShowFPS;
	int  ForcedSleep; //Used for framerate simulating,
					  //sleeps this amount every frame
	int  FramerateLimit;

	Real MouseSensitivity;
	
	int testX, testY;
	int samplesPerSecond;
	float lastSampleTime;
	int sampleCounter;

	ID3DXMesh* tempMesh;

	float ShowObjective;


	bool GamePaused;

	bool GameOver;

	std::string CurrentHint;
	D3DCOLOR HintColor;

	bool Unload;
	

protected: //data
	
	MeshManager MM;

	void InitDefaults();
    	
public:


	GameApp(HWND hWnd, HINSTANCE hInstance, Screen3D& screenref, Input& inputref, SoundManager& soundref );


	bool Init(HWND hWnd, HINSTANCE hInstance);
	void Shutdown();
	bool InitSystemObjects(HWND hWnd, HINSTANCE hInstance);
	void ShutdownSystemObjects();
	void InitRenderStates();

	void DisplayLogos();

	void DetectCPU();

	WorldManager* GetWorldManager() { return WM; }
	Screen3D* GetScreen() { return &Screen; }
	HatKernel* GetHatKernel() { return &GameOS; }

	void SetMap( char* mapname ) { strcpy(MapName, mapname ); }
	void SetMusic( char* musicfile ) { Mp3.LoadFile( musicfile ); }
	bool IsAlwaysActive() { return AlwaysActive; }
	bool GetShareCPU() { return ShareCPU; }
	
	void HandleGuiMessage( Event* m );

	bool CheckForEndState();
	void CheckForHint() ;

	void MouseDown( POINT mousepoint, int button );
	void KeyMessage( int virtualkey );
	void CharMessage( char charpressed, int repeat ) ;

	void BeginFrame() ;
	void HandleNetworkMessages(  );
	void MoveObjects();
	void CheckInput();
	void CheckBufferedInput();
	void RedrawScreen();

	void UpdateGame();

	void LoadMap( const char* modName, const char* mapName );
	void DrawMovieOverlay();

};

extern GameApp* Game;