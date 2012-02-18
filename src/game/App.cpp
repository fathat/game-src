// I N C L U D E S //////////////////////////////////////////
#include <conio.h>
#include <process.h>
#include "app.h"
#include "..\shared\s3dlib\types.h"
#include "..\shared\network\messageparser.h"
#include "..\shared\ini\ini.h"
#include "..\shared\world\vehicle.h"
#include "..\shared\world\vehicleobject.h"
#include "..\shared\world\powerup.h"
#include "..\shared\world\avatarconstruction.h"
#include "..\shared\world\scenemanager.h"

#include "systemobjects.h"

#include "..\shared\defs\defs.h"

bool g_bLoading=false;
bool g_bLoadThreadDone=false;
CRITICAL_SECTION LoadingCS;


//HAY!
HatKernel* GetSSLKernel()
{
	return Game->GetHatKernel();
}


//Initialize all default configuration values here.
void GameApp::InitDefaults()
{	
	DrawReflection    = true;
	DebugFieldOfView  = false;
	ShowFPS			  = false;
	MouseSensitivity  = 1.25f;
	MovieSkipFlag     = false;
	InMovie			  = false;
	AlwaysActive	  = true;
	ShareCPU		  = true;
	FramerateLimit    = -1;
	ScreenDarkLevel   = 0.0f;
	testX = 0;
	testY = 0;
	lastSampleTime = 0;
	samplesPerSecond = 0;
	sampleCounter = 0;
	ShowObjective = 0;
	GameOver = false;
	Unload   = false;
	GamePaused = false;
	HintColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );



}


GameApp::GameApp(HWND hWnd, HINSTANCE hInstance, Screen3D& screenref, Input& inputref, SoundManager& soundref ) :
	Screen ( screenref),
	InputSystem ( inputref ),
	Sound (soundref )
{
	WM=NULL;
	InitDefaults();
}



//////// InitSystemObjects ////////////////////
//
// Description:
//
// Creates all hardware devices.
//
///////////////////////////////////////////////

bool GameApp::InitSystemObjects(HWND hWnd, HINSTANCE hInstance)
{
	//Determine what CPU is in the system and thusly if 3dnow stuff
	//should be used.
	DetectCPU();

	//Make off-screen rendering texture
	int TextureSize = 512;
	if(Screen.DisplayMode.Width < 800 )
		TextureSize = 256;
	ScreenTexture.Init( &Screen, "screen", Screen.GetWidth(), Screen.GetHeight(), D3DFMT_X8R8G8B8 );
	AccumulationBuffer.Init( &Screen, "accumulation", Screen.GetWidth(), Screen.GetHeight(), D3DFMT_A8R8G8B8);

	
		
	D3DXCreateSphere( Screen.D3DDevice, 1.0f, 12, 12, &tempMesh, NULL );
	return true;
}





//////// ShutdownSystemObjects ////////////////////
//
// Description:
//
// Releases all hardware devices.
//
///////////////////////////////////////////////

void GameApp::ShutdownSystemObjects()
{
	InputSystem.Destroy();
	Screen.Destroy();
	
}


//////// Init /////////////////////////////////
//
// Description:
//
// Initializes all non-map objects.
//
///////////////////////////////////////////////	

bool GameApp::Init(HWND hWnd, HINSTANCE hInstance)
{
	//declarations
	char TempBuffer[255];
	
	//setup the system objects
	dInitODE();
	InitSystemObjects( hWnd, hInstance );

	//init the mesh manager
	MM.Init(&Screen);

	//set render states
	InitRenderStates();
	DrawWorld = true;

		
	//Init GUI
	UserInterface.InitGUI( Screen, *WM);
	GameOS.TaskMan.SetDefaultConsole( UserInterface.GameConsole);

	//set CVAR stuff
	CVar.SetConsole( UserInterface.GameConsole );
	CVar.Timer = &Timer;

	//print out texture width
	CVar.WriteLine( "Screen Texture Width: %dx%d", ScreenTexture.GetWidth(), ScreenTexture.GetHeight() );
	CVar.WriteLine( "Accumulation Texture Width: %dx%d", AccumulationBuffer.GetWidth(), AccumulationBuffer.GetHeight() );

	GameOS.RunCMD( UserInterface.GameConsole, "intro" );
	
	return true;

}



void GameApp::LoadMap( const char* modName, const char* mapName )
{
	WM = new WorldManager( &Screen, &MM, &Sound,&Timer, NULL );
	if( !WM->InitXML( modName, mapName) ) 
	{
		CVar.WriteLine( "Could not load map file" ) ;
		delete WM;
		WM = NULL;
	}
	else
	{
		//Set camera? (To be honest, I forget why this is here)
		//TODO: look at this
		WM->Viewer.Zoom = 0;
		WM->Viewer.OffsetY = WM->LocalPlayer.GetHeight()*.85f;
		WM->Viewer.Follow(WM->LocalPlayer.GetLocation());
		WM->Viewer.CS = CS_FIRST_PERSON;

		ShowObjective = 3.0f;

		UserInterface.ToggleMenu();
		GameOS.RunCMD(UserInterface.GameConsole, "maploaded");
	
	}
	
}

//////// Shutdown /////////////////////////////
//
// Description:
//
// Releases all system objects.
//
///////////////////////////////////////////////

void GameApp::Shutdown()
{
	if(WM)
		delete WM;
	WM = NULL;
	MM.Destroy();
	ShutdownSystemObjects();
}



//////// InitRenderStates /////////////////////
//
// Description:
//
// Inits the appropriate render states.
//
///////////////////////////////////////////////


void GameApp::InitRenderStates()
{
	// Set up a material. 
    D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = 1;mtrl.Specular.r = 0; mtrl.Ambient.r = 0.3f;
    mtrl.Diffuse.g = 1;mtrl.Specular.g = 0; mtrl.Ambient.g = 0.3f;
    mtrl.Diffuse.b = 1;mtrl.Specular.b = 0; mtrl.Ambient.b = 0.3f;
    mtrl.Diffuse.a = 1;mtrl.Specular.a = 0; mtrl.Ambient.a = 0.3f;
	

    Screen.D3DDevice->SetMaterial( &mtrl );

    // Set up a white, directional light.
    D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
	light.Specular.r  = 1.0f;
    light.Specular.g  = 1.0f;
    light.Specular.b  = 1.0f;
	light.Ambient.r   = 1.0f;
	light.Ambient.g   = 1.0f;
	light.Ambient.b   = 1.0f;
    vecDir = D3DXVECTOR3(0,
                         -1.0f,
                         0);
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    light.Range       = 1000.0f;
    Screen.D3DDevice->SetLight( 0, &light );
    Screen.D3DDevice->LightEnable( 0, TRUE );
	Screen.D3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	Screen.D3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
	Screen.D3DDevice->SetRenderState( D3DRS_ALPHAREF, 10);

    // Finally, turn on some ambient light.
    Screen.D3DDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(45, 45, 45) );
	Screen.D3DDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE);
	Screen.D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);

	BlurTexture =  Screen.GetTextureHandle( "..\\base\\art\\ui\\blur.bmp","..\\base\\art\\ui\\blur.bmp" );
	InverseBlurTexture =  Screen.GetTextureHandle( "..\\base\\art\\ui\\blur_inverted.bmp","..\\base\\art\\ui\\blur_inverted.bmp" );

}


void GameApp::CheckForHint()
{
	if(!WM )
		return;

	SceneObject* so = WM->FindNearestObject( WM->LocalPlayer.GetLocation(), 30, &WM->LocalPlayer );
	Powerup* p = dynamic_cast< Powerup* > (so );
	VehicleObject* v = dynamic_cast< VehicleObject* > (so );

	if( p )
	{
		CurrentHint = "Press E to collect item";
		HintColor = D3DCOLOR_ARGB( 200, 0, 222, 128 );
		return;
	}

	if( v )
	{
		CurrentHint = "Press E to drive the vehicle";
		HintColor = D3DCOLOR_ARGB( 200, 0, 222, 128 );
		return;
	}

	else
	{
		HintColor = D3DCOLOR_ARGB( 200, 0, 128, 222 );
		if( WM->TimeLeft > 8.5* 60 )
			CurrentHint = "Press Tab to see objective";
		else
			CurrentHint = "";
	}


}


bool GameApp::CheckForEndState()
{
	if(!WM || GameOver)
		return true;

	if( WM->TimeLeft <= 0 )
	{
		GameOS.RunCMD( UserInterface.GameConsole, "endingLoss" );
		GameOver = true;
		delete WM;
		WM = NULL;
		UserInterface.ToggleMenu();

		return true;
	}

	SceneHandle h = WM->sceneManager->GetObjectByScriptName( "exit" );
	SceneObject* object = WM->sceneManager->GetObjectPtr( h );
	if( object ) {

		Vector3D d =  object->GetLocation().GetVector() - WM->LocalPlayer.GetLocation().GetVector();
		if( d.Mag() < 50 )
		{
			if( WM->LocalPlayer.GetMoney() < 100 )
				return false;
			if( WM->LocalPlayer.GetMoney() <= 100 )
			{
				//ending 1
				GameOS.RunCMD( UserInterface.GameConsole, "endingC" );
				GameOver = true;
				//UserInterface.ToggleMenu();
				return true;
			}

			else if( WM->LocalPlayer.GetMoney() <= 900 )
			{
				GameOS.RunCMD( UserInterface.GameConsole, "endingB" );
				GameOver = true;
				//delete WM;
				//WM = NULL;
				//UserInterface.ToggleMenu();
				return true;
			}
			else if( WM->LocalPlayer.GetMoney() > 900 )
			{
				GameOS.RunCMD( UserInterface.GameConsole, "endingA" );
				GameOver = true;
				//delete WM;
				//WM = NULL;
				//UserInterface.ToggleMenu();
				return true;
			}



		}
	}

	return false;

}


void GameApp::DisplayLogos()
{
	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
	Screen.D3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	GameOS.TaskMan.BuildScript( "..\\scripts\\intro.shil", NULL);
	

	WM->WorldWeather.SetRenderStates( Screen );

}


//////// DetectCPU ////////////////////////////
//
// Description:
//
// Determines if there is any special instruction
// set support and adjusts the appropriate function
// if there is.
//
///////////////////////////////////////////////

void GameApp::DetectCPU()
{
	bool b3DNow=false;

	m_atan   =  s_atan  ; 
	m_atan2  =  s_atan2 ;
	m_acos   =  s_acos  ;
	m_asin   =  s_asin  ;
	m_log    =  s_log   ;
	m_log10  =  s_log10 ;
	m_pow    =  s_pow   ;
	m_exp    =  s_exp   ;
	m_sqrt   =  s_sqrt  ;
	m_fabs   =  s_fabs  ;
	m_ceil   =  s_ceil  ;
	m_floor  =  s_floor ;
	m_frexp  =  s_frexp ;
	m_ldexp  =  s_ldexp ;
	m_modf   =  s_modf  ;
	m_fmod   =  s_fmod  ;
	m_sin    =  s_sin   ; 
	m_cos    =  s_cos   ;
	m_tan    =  s_tan   ;
				
				 
}				 


void GameApp::MoveObjects()
{
	
	if( !WM || UserInterface.GM.GetState() || GamePaused )
		return;

	Vector3D Velocity = WM->LocalPlayer.GetVelocity();


	if( WM->LocalPlayer.GetHealth() >= 0 )
	{
		WM->DilationPercent -= FrameTime;

		if( WM->DilationPercent < 0 )
			WM->DilationPercent = 0;
	}
 
	WM->Update( FrameTime );


	CheckForHint();
	if( CheckForEndState() )
		return;

	DeviceManager::OpenALSound.SetListener( WM->LocalPlayer.GetLocation().AsSoundSource() );
	DeviceManager::OpenALSound.Update();
	

	

}


//////// HandleNetworkMessage /////////////////
//
// Description:
//
// Handles incoming messages from the server we're
// connected to.
//
///////////////////////////////////////////////


void GameApp::HandleNetworkMessages( )
{

}


//////// HandleGuiMessage /////////////////////
//
// Description:
//
// Checks for certain controls being pressed.
//
///////////////////////////////////////////////

void GameApp::HandleGuiMessage( Event* m ) 
{ 

	switch(m->Message)
	{

	//A "command" has been executed (usually a button being pressed
	//or something similiar.
	case WINM_COMMAND:

		//Message came from the resume button, so just get rid of the gui
		//(which will unpause the game).
		if(m->ControlID.compare("IDRESUME") == 0)
		{
			UserInterface.ToggleMenu();
			
		}

		if(m->ControlID.compare("IDMISSION") == 0)
		{
			LoadMap( "base", "testmap" );
			

		}


		if(m->ControlID.compare("IDFIRSTCHOICE") == 0)
		{
			SceneObject* NearestObject = WM->FindNearestObject( WM->LocalPlayer.GetLocation(), 30 );
			if(NearestObject)
				NearestObject->Use( 1, 0, 0, 0, FrameTime );
				
		}
		
		if(m->ControlID.compare("IDSECONDCHOICE") == 0)
		{
			SceneObject* NearestObject = WM->FindNearestObject( WM->LocalPlayer.GetLocation(), 30 );
			if(NearestObject)
				NearestObject->Use( 2, 0, 0,0, FrameTime );
				
		}
		
		if(m->ControlID.compare("IDTHIRDCHOICE") == 0)
		{
			SceneObject* NearestObject = WM->FindNearestObject( WM->LocalPlayer.GetLocation(), 30 );
			if(NearestObject)
				NearestObject->Use( 3, 0, 0, 0, FrameTime );
				
		}

		//Quit button pressed, just tell windows to shut this down.
		if(m->ControlID.compare("IDQUIT") == 0)
		{
			PostQuitMessage(0);
		}
		break;

    //the contents of a control has changed. Usually this is sent
	//if someone hits enter after typing something into an input box.
	case WINM_CONTENTSCHANGED:

		//Check if the message came from the console, and if it
		//did handle it all here.
		if(m->ControlID.compare("IDCONSOLE") == 0)
		{
			

			//Parse the text into a vector of words that
			//we can use.
			std::string TextString = (char*)m->Param1;
			StringArray WordList;
			if( !GetWords( (char*)TextString.c_str(), WordList) )
				return ;
     
			std::string NumberString;
			char buffer[32];
			for( unsigned int i=0; i<TextString.size(); i++)
			{
				sprintf( buffer, "%d", TextString[i]);
				NumberString += buffer;
				NumberString += ".";
			}
			CVar.WriteLine( NumberString.c_str() );

			//Quit command
			if( WordList[0].compare( "quit") == 0)
			{
				PostQuitMessage(0);
			}

			//map functions
			else if ( WordList[0].compare( "loadmapxml" ) == 0 )
			{
				if(WordList.size() != 3 )
					CVar.WriteLine( "Invalid parameters. The proper syntax is: \"loadmapxml modname mapname\"" );
				else
				{
					LoadMap( WordList[1].c_str(), WordList[2].c_str() );
				}


			}
			
			else if ( WordList[0].compare ( "unloadmap" ) == 0 )
			{
				if(WM)
					delete WM;
				WM = NULL;

			}

			//connect!
			/*else if(WordList[0].compare("connect") == 0 && WordList.size() > 2)
			{
				NM.JoinGame( (char*)WordList[1].c_str() , (char*)WordList[2].c_str() );
				ScreenName = WordList[2].c_str();
	
				Sleep (2000);
		
				M3DConnected ConnectPacket;
				NM.SendToServer( (BYTE*)&ConnectPacket, true );
			}

			else if(WordList[0].compare("addself") == 0 )
			{
				M3DAddEntPacket AddPacket;
				AddPacket.ID = 0;
				AddPacket.ClassName[0] =0;
				strcpy(AddPacket.ClassName, "avatar");
				strcpy(AddPacket.SubType, "avatar");
				NM.SendToServer((BYTE*)&AddPacket);
			}
			
			else if(WordList[0].compare("say") == 0)
			{
				M3DMessagePacket M;
				strcpy(M.Message, (char*)TextString.c_str() + strlen("say")+1 );
				NM.SendToServer( (BYTE*)&M );
			}*/

					

			else if(WordList[0].compare("mohealth") == 0)
			{
				WM->LocalPlayer.SetHealth( 1000000 );
			}
			else if(WordList[0].compare("timing") == 0)
			{
				CVar.OutputTimingData();
			}


			else if(WordList[0].compare("sensitivity") == 0 && WordList.size() > 1)
			{
				MouseSensitivity = atof( (char*)WordList[1].c_str());
			}

			else if(WordList[0].compare("alwaysactive") == 0 && WordList.size() > 1)
			{
				AlwaysActive = (bool) atoi( (char*)WordList[1].c_str()) != 0 ? true : false;
			}

			else if(WordList[0].compare("sharecpu") == 0 && WordList.size() > 1)
			{
				ShareCPU = (bool)atoi( (char*)WordList[1].c_str()) != 0 ? true : false;
			}

			else if (WordList[0].compare("simframerate") == 0 && WordList.size() > 1)
			{
				ForcedSleep = 1000/atoi( (char*)WordList[1].c_str()) - (1000/Timer.GetFPS()) ;
				FramerateLimit = atoi( (char*)WordList[1].c_str());
			}

			else if (WordList[0].compare("wireframe") == 0)
			{
                Screen.EnableWireframeDrawing();
			}

			else if (WordList[0].compare("solid") == 0)
			{ 		
				Screen.EnableSolidDrawing();
			}

            //Print the game's version			
			else if(TextString.compare("version\n") == 0)
			{
				InputBox* textbox = (InputBox*)m->Control;
				textbox->InputLine("Game - Release Alpha 7");
			}


			//Disconnect from the current game the player is in.
			//Haven't tested this yet.
			else if(TextString.compare("disconnect\n") == 0)
			{
				//NM.ExitGame();
			}


			//Switch for a testing variable. When you switch on
			//field of view debugging the culling functions are given
			//a "fake" camera from the players viewpoint, whereas
			//the real one used for drawing is placed overhead. This way
			//you can make sure things outside the view aren't being drawn
			//and such.
			else if(TextString.compare("dbfov\n") == 0)
			{
				DebugFieldOfView = !DebugFieldOfView;
			}

			
			//Toggles on or off a switch to show the framerate
			//and a lot of other related usefull info (polygons being drawn, etc.)
			else if(TextString.compare("showfps\n") == 0)
			{
				ShowFPS = !ShowFPS;
			}

			//get var
			else if(WordList[0].compare("getvars") == 0 && WordList.size() > 1)
			{
				
				std::string var;
				var = WordList[1].c_str();
				var += " = ";
				var += CVar.GetVarS( WordList[1] ) ;

				InputBox* textbox = (InputBox*)m->Control;
				textbox->InputLine((char*)var.c_str());
			}

			//get var
			else if(WordList[0].compare("getvarf") == 0 && WordList.size() > 1)
			{
				char buf[32];
				sprintf( buf, "%f", CVar.GetVarF( WordList[1] ));
				std::string var;
				var = WordList[1].c_str();
				var += " = ";
				var += buf ;

				InputBox* textbox = (InputBox*)m->Control;
				textbox->InputLine((char*)var.c_str());
			}

			//get var
			else if(WordList[0].compare("getvari") == 0 && WordList.size() > 1)
			{
				char buf[32];

				std::string var;
				var = WordList[1].c_str();
				var += " = ";
				var += itoa( CVar.GetVarI( WordList[1] ), buf, 32 )  ;

				InputBox* textbox = (InputBox*)m->Control;
				textbox->InputLine((char*)var.c_str());
			}


			//something something for executing the something or other
			else if(WordList[0].compare("/appinfo") == 0 && WordList.size() > 1)
			{
				std::string AppName = "..\\scripts\\";
				AppName += WordList[1].c_str();
				AppName += ".shil";
				//StateHandle s = GameOS.TaskMan.BuildScript((char*) AppName.c_str() ,(InputBox*)m->Control);	
				//GameOS.TaskMan.CallAppFunction( s, "ShowAppInfo", 0);
				//GameOS.TaskMan.Kill( s );
			}
			
			//execute it as an OS command
			else
			{
				GameOS.RunCMD( (InputBox*)m->Control, (char*)m->Param1);
			}
				
		}
		break;

	//A keypress was detected. We don't use this, and I can't
	//remember if the gui even really supports this yet.
	case WINM_KEYPRESSED:
		break;
	};

}


//////// MouseDown  ///////////////////////////
//
// Description:
//
// Handles a message from the windows procedure
// saying a mouse button has been pressed.
// Used for gui stuff.
//
///////////////////////////////////////////////

void GameApp::MouseDown( POINT mousepoint, int button )
{
	UserInterface.GM.MouseClicked(mousepoint.x, mousepoint.y, 0);

}

//////// KeyMessage ///////////////////////////
//
// Description:
//
// Handles a message from the window saying
// a virtual key has been pressed. Mostly used
// for GUI stuff that needs the windows keyboard
// timing.
//
///////////////////////////////////////////////


void GameApp::KeyMessage( int virtualkey )
{
	if( virtualkey == VK_ESCAPE )
	{
		UserInterface.ToggleMenu();
		if(UserInterface.State == GM_NONE )
			UserInterface.GM.SetState(false);
		else
			UserInterface.GM.SetState(true);

	}

	else if (virtualkey == VK_F1 )
	{
		ShowFPS = !ShowFPS;
	}

	else if( virtualkey == VK_F12)
	{
		if(WM)
		{
			WM->TextureFilterMode = D3DTEXF_ANISOTROPIC;
			Screen.D3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8 );
			Screen.SetAnisotropicTextureFiltering();
		}
	}

	else if( virtualkey == VK_F11)
	{
		if(WM)
		{
			WM->TextureFilterMode = D3DTEXF_LINEAR;
			Screen.SetLinearTextureFiltering();
		}
	}

	else if( virtualkey == VK_F7)
	{
		if(WM)
			WM->terrain->ShowGrass( !WM->terrain->ShowGrass() );
	}

	else if( virtualkey == VK_F8)
	{
		Screen.TM.LimitSize( 256 );
	}
	else if( virtualkey == VK_F9)
	{
		Screen.TM.UnlimitTextures();
	}

	else if( virtualkey == VK_F5)
	{
		//Declarations
		std::string SSFilename;
		char TempBuffer[32];
		ZeroMemory(TempBuffer, 32);

		//Figure out the file number
		int CurrentScreenshot = GetIniValue( "..\\screenshots\\screenshots.ini", "ssdir", "current_screenshot");
		itoa(CurrentScreenshot, TempBuffer, 10);
		
		//construct the filename and take the shot
		SSFilename = "ss";
		if(CurrentScreenshot<10)
			SSFilename += "000";
		else if(CurrentScreenshot<100)
			SSFilename += "00";
		else if(CurrentScreenshot<1000)
			SSFilename += "0";
		SSFilename += TempBuffer;
		Screen.TakeScreenshot((char*)SSFilename.c_str() );

		//Save the currentscreenshot number
		CurrentScreenshot++;
		SetIniValue( "..\\screenshots\\screenshots.ini", "ssdir", "current_screenshot", CurrentScreenshot);

	}

	else
	{
		UserInterface.GM.KeyPressed(0, virtualkey);
	}

}

//////// CharMessage //////////////////////////
//
// Description:
//
// Handles a message from the window saying
// a character has been pressed. Mostly used
// for GUI stuff that needs the windows keyboard
// timing.
//
///////////////////////////////////////////////

void GameApp::CharMessage( char charpressed, int repeat ) 
{
	//Check for movie skip !!1
	MovieSkipFlag = false;
	if(charpressed == ' ')
	{
		MovieSkipFlag=true;
	}

	SceneObject* NearestObject;
	//If the user interface is not active, then do some stuff
	//with the character instead of giving it to the user interface
	if(!UserInterface.GM.GetState())
	{
		if(charpressed == 'e' || charpressed == 'E' )
		{

			NearestObject = WM->FindNearestObject( WM->LocalPlayer.GetLocation(), 30);
			if(NearestObject)
			{
				//If it's collectible, pick it up
				if(NearestObject->GetCollectible())
				{
					WM->LocalPlayer.AddEntity( NearestObject );
				}

				//Otherwise just use it
				else
					NearestObject->Use( 0, 0, 0, 0, FrameTime );
			}
		}

		if( charpressed == 'x' && WM )
		{
			FMOD::Sound* snd = DeviceManager::OpenALSound.GetSoundBuffer( "..\\sound\\rain_l.wav" );
			DeviceManager::OpenALSound.PlaySoundImmediate( snd, WM->LocalPlayer.GetLocation().AsSoundSource() ) ;
		}

		if(WM && InMovie)
		{
			if(charpressed == '1' )
			{
				NearestObject = WM->FindNearestObject( WM->LocalPlayer.GetLocation(), 30 );
				if(NearestObject)
					NearestObject->Use( 0, 0, 0, 0, FrameTime );
			}

			if(charpressed == '2' )
			{
				NearestObject = WM->FindNearestObject( WM->LocalPlayer.GetLocation(), 30 );
				if(NearestObject)
					NearestObject->Use( 1, 0, 0, 0, FrameTime );
			}

			if(charpressed == '3' )
			{
				NearestObject = WM->FindNearestObject( WM->LocalPlayer.GetLocation(), 30 );
				if(NearestObject)
					NearestObject->Use( 2, 0, 0, 0, FrameTime );
			}
			if(charpressed == '4' )
			{
				NearestObject = WM->FindNearestObject( WM->LocalPlayer.GetLocation(), 30 );
				if(NearestObject)
					NearestObject->Use( 3, 0, 0, 0, FrameTime );
			}
		}

	}
	
	//If the user interface actually is active, give the key
	//message to it.
	else
	{

		//is there a repeat?
		if(repeat < 15) 
		{
			for(int i=0; i<repeat; i++)
			{
				UserInterface.GM.KeyPressed((char)charpressed);
			}
		}
		else //no repeat
		{
			UserInterface.GM.KeyPressed((char) charpressed);
		}
	}
}


//////// BeginFrame ///////////////////////////
//
// Description:
//
// Calculates timer data and does other things
// needed for work on the the frame.
//
///////////////////////////////////////////////

void GameApp::BeginFrame() 
{ 
	//Figure out how long the last frame took
	Timer.CalcFrameTime();
	FrameTime = Timer.GetFrameTime();

	lastSampleTime -= FrameTime;
	if( lastSampleTime < 0 )
	{
		lastSampleTime = 1.0f;
		samplesPerSecond = sampleCounter;
		sampleCounter = 0;
	}
	//If there's a frame rate limit set, Sleep
	//for appropriate period of time
	if(FramerateLimit != -1 )
	{
		float desiredFrameTime = 1.0f/FramerateLimit;
		float sleepAmount = desiredFrameTime-FrameTime;

		if( sleepAmount > 0)
			Sleep( (sleepAmount * 1000));

	}


	if(FrameTime > .1111111111)
		FrameTime = .111111111;
			
	GameOS.UpdateOS();

	if(Unload)
	{
		Unload = false;
		if( WM ) delete WM;
		WM = NULL;
	}
				

}

void GameApp::CheckBufferedInput()
{
	DIDEVICEOBJECTDATA mouseArray[MOUSE_INPUTBUFFER_SIZE];
	DWORD mouseElements = MOUSE_INPUTBUFFER_SIZE;
	float offset;
	InputSystem.GetMouseData( mouseArray, &mouseElements );

	if(UserInterface.State != GM_NONE || WM == NULL || InMovie )
		return;

	sampleCounter  += mouseElements;
	
	for( int i=0; i< mouseElements; i++ )
	{
		
		offset = 0.05f* (float)((signed int)mouseArray[i].dwData);

		switch( mouseArray[ i ].dwOfs )
		{
		case DIMOFS_X:
			
			WM->Viewer.Yaw += RADIAN( offset ) * MouseSensitivity;
			WM->LocalPlayer.SetYaw( WM->Viewer.Yaw );
			break;
		case DIMOFS_Y:
			if( WM->Viewer.CS != CS_FREE )
				WM->Viewer.Pitch += RADIAN( offset ) *MouseSensitivity ;

			if(WM->Viewer.Pitch > RADIAN( 90 ))
				WM->Viewer.Pitch = RADIAN(90);
			if(WM->Viewer.Pitch < RADIAN( -90 ))
				WM->Viewer.Pitch = RADIAN(-90);

			break;
			
		}
	}
}

//////// CheckInput ///////////////////////////
//
// Description:
//
// Checks for certain controls being pressed.
//
///////////////////////////////////////////////

void GameApp::CheckInput()
{

	//Declarations
	POINT NewMousePoint;
	
	//Check input, update objects, and draw the screen
	InputSystem.PollData();
	UCHAR* KeyBuffer = InputSystem.GetKeyData();

	//Only accept key input if the menu isn't up
	if(UserInterface.State == GM_NONE && WM != NULL && !InMovie	 && !GamePaused)
	{
		//handle input
		NewMousePoint = InputSystem.GetMouseDelta();
		WM->InputFocus->HandleInput( FrameTime,  NewMousePoint, KeyBuffer );


		//Mouse movement
		if(FrameTime < 1)
		{
			
			//WM->LocalPlayer.SetYaw( WM->LocalPlayer.GetYaw() + RADIAN((NewMousePoint.x) * (MouseSensitivity*.05f)) );
			WM->Viewer.Yaw = WM->Viewer.Yaw + RADIAN((NewMousePoint.x) * (MouseSensitivity*.05f));
			
			if( WM->Viewer.CS != CS_FREE )
				WM->Viewer.Pitch += RADIAN((NewMousePoint.y) * (MouseSensitivity*.05f));
			
			if(WM->Viewer.Pitch > RADIAN( 90 ))
				WM->Viewer.Pitch = RADIAN(90);
			if(WM->Viewer.Pitch < RADIAN( -90 ))
				WM->Viewer.Pitch = RADIAN(-90);

			if(WM->Viewer.CS == CS_FIRST_PERSON)
				WM->LocalPlayer.SetPitch( WM->Viewer.Pitch);
			else
				WM->LocalPlayer.SetPitch( 0 );
		}

		//camera something something
		if((InputSystem.RightButtonDown() || HIWORD( GetKeyState(VK_RBUTTON ) ) ))
		{
			Screen.SetFieldOfView( 45 );
			WM->Viewer.FOVDrift = -90.0f;
			Screen.CalculateFOV();
			Zoomed = true;
		}
		else 
		{
			WM->Viewer.FOVDrift = 90.0f;
			Screen.SetFieldOfView( WM->Viewer.FOV );
			Screen.CalculateFOV();
			Zoomed = false;
		}


		//Camera controls	
		if(KeyBuffer[DIK_F1] & 0x80 || HIWORD(GetKeyState(VK_F1)))
			WM->Viewer.CS = CS_FIRST_PERSON;
		if(KeyBuffer[DIK_F2] & 0x80 || HIWORD(GetKeyState(VK_F2)))
			WM->Viewer.CS = CS_FOLLOW;
		if(KeyBuffer[DIK_F3] & 0x80 || HIWORD(GetKeyState(VK_F3)))
			WM->Viewer.CS = CS_FREE;
		if( KeyBuffer[DIK_NEXT] & 0x80)
			WM->Viewer.Zoom -= 20*FrameTime;
		if( KeyBuffer[DIK_PRIOR] & 0x80)
			WM->Viewer.Zoom += 20*FrameTime;

		if( KeyBuffer[DIK_TAB] & 0x80 )
		{
			ShowObjective += FrameTime*3;
			if( ShowObjective > 1 )
				ShowObjective = 1;
		}
		else
		{
			ShowObjective -= FrameTime;
			if( ShowObjective < 0 )
				ShowObjective = 0;
		}
	
	}
}



//Draws black borders on the top and bottom of the screen, and possibly
//subtitles.
void GameApp::DrawMovieOverlay()
{
	//Draw movie dialog something or other
	if(InMovie)
	{
		RECT TopRect, BotRect;
		TopRect.left   = 0;
		TopRect.top    = 0;
		TopRect.right  = Screen.GetWidth();
		TopRect.bottom = (Screen.GetHeight()/8);

		BotRect.left   = 0;
		BotRect.top    = Screen.GetHeight()-(Screen.GetHeight()/8);
		BotRect.right  = Screen.GetWidth();
		BotRect.bottom = Screen.GetHeight();

		TextureHandle BlackBox = Screen.GetTextureHandle("..\\base\\art\\ui\\black.bmp");

		Screen.DrawSpriteRect( TopRect, BlackBox, true, D3DCOLOR_ARGB( 255, 255, 255, 255 ));
		Screen.DrawSpriteRect( BotRect, BlackBox, true);

		Font2D* DrawFont = Screen.FM.GetFont( "Arial", 16);

		DrawFont->Draw( Screen, 10, 0, false, D3DCOLOR_XRGB(255, 255, 255), (char*)TopMovieText.c_str() );
		DrawFont->Draw( Screen, 10, Screen.GetHeight()-60+2, false, D3DCOLOR_XRGB(255, 255, 255), (char*)BottomMovieText.c_str() );
	}

}


//////// RedrawScreen /////////////////////////
//
// Description:
//
// Redraws everything on the screen.
//
///////////////////////////////////////////////

void GameApp::RedrawScreen()
{

	//definitions
	if(!Screen.D3DDevice || !DrawWorld)
		return;
	LPDIRECT3DSURFACE9 ScratchSurface, OriginalSurface;
	LPDIRECT3DTEXTURE9 ScratchTexture;
		

	Screen.D3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	Screen.D3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	
	//Set the viewport. The GUI system tends to screw with this,
	//which is why it's set each frame.
	Screen.SetViewport(0, 0, Screen.GetWidth(), Screen.GetHeight());
	
	//Set the field of view. Note that there are some constraints
	//to ensure that the FOV doesn't become too weird.
	if( WM != NULL)
	{
		if(WM->Viewer.FOV > 75.0f)
			WM->Viewer.FOV = 75.0f;
		if(WM->Viewer.FOV < 45.0f)
			WM->Viewer.FOV = 45.0f;
		Screen.SetFieldOfView( WM->Viewer.FOV * (1.0f +  WM->DilationPercent/5.0f ) );
	}
	
	//Save the old render target
	Screen.D3DDevice->GetRenderTarget( 0,  &OriginalSurface);

	//Set the new render target (our texture)
	ScratchTexture = ScreenTexture.GetTextureSurface();
	ScratchTexture->GetSurfaceLevel(0, &ScratchSurface);
	Screen.D3DDevice->SetRenderTarget(0,  ScratchSurface );

	//update the texture
	Screen.Clear(false );
	Screen.Start3D();
	
	if(DrawWorld && WM)
	{
		Screen.SetTextureFiltering( D3DTEXF_LINEAR );
		Screen.SetRenderState( D3DRS_FOGENABLE,  TRUE );
		WM->Draw(Screen);
		Screen.D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		WM->InputFocus->DrawHUD( Screen, MM, Screen.FM, WM->Viewer );
	}
	Screen.DrawAllAlpha();

	Screen.End3D();

			
	//Release surfaces we used
	ScratchSurface->Release();
	

	//Draw world texture into the accumulation buffer
	ScratchTexture = AccumulationBuffer.GetTextureSurface();
	ScratchTexture->GetSurfaceLevel(0, &ScratchSurface);
	Screen.D3DDevice->SetRenderTarget(0,  ScratchSurface );

	//Screen.Clear();
	Screen.Start3D();
	Screen.D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	Screen.SetRenderState( D3DRS_ZENABLE, FALSE );
	//Screen.SetPointTextureFiltering();
	RECT screenRect;
	screenRect.left   = 0;
	screenRect.top    = 0;
	screenRect.right  = Screen.GetWidth();
	screenRect.bottom = Screen.GetHeight();
	int blurAlpha = 255;
	if( WM)
		blurAlpha = 255- (int)( 128.0f*WM->DilationPercent );
	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	Screen.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	Screen.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//Screen.DrawSpriteNoHandle( screenRect , &ScreenTexture, true, D3DCOLOR_ARGB( blurAlpha, 255, 255, 255 ), false );
	Screen.SetTextureFiltering( D3DTEXF_POINT );
	//Screen.D3DDevice->SetSamplerState( 0, D3DSAMP_MAXMIPLEVEL, 0 );
	Screen.DrawSprite( 0, 0, &ScreenTexture, blurAlpha ) ;
	Screen.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
	Screen.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	if( WM &&  WM->DilationPercent > 0 )
	{
		int addColor = (int)( 8.0f*WM->DilationPercent );
		Screen.DrawSpriteNoHandle( screenRect , &ScreenTexture, true, D3DCOLOR_ARGB( 255, 32, 32, 32 ), false );
	}
	Screen.SetRenderState( D3DRS_ZENABLE, TRUE );
	Screen.End3D();

	//Restore the old render target
	Screen.D3DDevice->SetRenderTarget( 0, OriginalSurface  );
	OriginalSurface->Release();
	
	//clear the screen
	Screen.Clear(false);
	Screen.Start3D();

	Font2D* HudFont = Screen.FM.GetFont( "Verdana", 20);

	if(DrawWorld && WM != NULL )
	{
		//Draw the world and all the alpha blended objects
		Screen.SetRenderState(D3DRS_ZENABLE, FALSE );
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
		Screen.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		Screen.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
		RECT screenRect;
        screenRect.left   = 0;
		screenRect.top    = 0;
		screenRect.right  = Screen.GetWidth();
		screenRect.bottom = Screen.GetHeight();
		//Screen.SetPointTextureFiltering();
		Screen.DrawSprite( 0, 0, &AccumulationBuffer, 255 );
		//Screen.DrawSpriteNoHandle( screenRect , &ScreenTexture, true, 255, false );

		
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		Screen.D3DDevice->SetTransform( D3DTS_WORLD, &matWorld );
		Screen.D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
		
		//Get pointers to some fonts we'll need
		Font2D* SomeFont = Screen.FM.GetFont( "Tahoma", 8 );
						
		//Draw the HUD if the GUI is not active
		if(!UserInterface.GM.GetState())
		{ 		
			if(ShowFPS)
			{
				SomeFont->Draw(Screen, 0, 5, false,D3DCOLOR_XRGB(255, 255, 255), "Framerate: %d", (int)Timer.GetFPS());	
				SomeFont->Draw(Screen, 0, 15, false,D3DCOLOR_XRGB(255, 255, 255), "Player x: %f", WM->LocalPlayer.GetX());
				SomeFont->Draw(Screen, 0, 25, false,D3DCOLOR_XRGB(255, 255, 255), "Player y: %f", WM->LocalPlayer.GetY());
				SomeFont->Draw(Screen, 0, 35, false,D3DCOLOR_XRGB(255, 255, 255), "Player z: %f", WM->LocalPlayer.GetZ());
				SomeFont->Draw(Screen, 0, 45, false,D3DCOLOR_XRGB(255, 255, 255), "ActiveConstructionCache: %d", WM->Solver.nCachedConstructions  );
				SomeFont->Draw(Screen, 0, 55, false,D3DCOLOR_XRGB(255, 255, 255), "Objects Tested: %d", WM->ObjectsTested  );
				SomeFont->Draw(Screen, 0, 65, false,D3DCOLOR_XRGB(255, 255, 255), "Total Vegetation: %d", WM->VegMan.GetTotalPlantCount() );
				SomeFont->Draw(Screen, 0, 75, false,D3DCOLOR_XRGB(255, 255, 255), "Polygons: %d", Screen.PolygonsRendered );
				SomeFont->Draw(Screen, 0, 85, false,D3DCOLOR_XRGB(255, 255, 255), "Draw Calls: %d", Screen.DrawCalls );
				SomeFont->Draw(Screen, 0, 95, false,D3DCOLOR_XRGB(255, 255, 255), "Millions of Triangles Per Second: %f", ((Screen.PolygonsRendered*Timer.GetFPS())/1000000.0f) );
				SomeFont->Draw(Screen, 0, 105, false, D3DCOLOR_XRGB( 255, 255, 255 ), "Objects in KDTree: %d" , WM->sceneManager->GetKDTree()->NumberOfObjects()  );
				SomeFont->Draw(Screen, 0, 115, false,D3DCOLOR_XRGB(255, 255, 255), "Total Texture Memory Used (MB): %d", Texture::GetTotalTextureMemory() / 1000000);
				SomeFont->Draw(Screen, 0, 125, false,D3DCOLOR_XRGB(255, 255, 255), "Total Memory Used (MB): %f", Screen.GetUsedMemory() / 1000000.0f );
				//DWORD maxAnisotropy;
				//Screen.D3DDevice->GetSamplerState(0, D3DSAMP_MAXANISOTROPY, &maxAnisotropy);
				//SomeFont->Draw(Screen, 0, 115, false,D3DCOLOR_XRGB(255, 255, 255), "Current Anisotropy: %d", maxAnisotropy );
				SomeFont->Draw(Screen, 0, 135, false,D3DCOLOR_XRGB(255, 255, 255), "Player speed: %f", WM->LocalPlayer.GetVelocity().Mag() );
				SomeFont->Draw(Screen, 0, 145, false,D3DCOLOR_XRGB(255, 255, 255), "Sounds playing: %d", DeviceManager::OpenALSound.SoundsPlaying() );
				//SomeFont->Draw(Screen, 0, 145, false,D3DCOLOR_XRGB(255, 255, 255), "my: %d", testY );	
			}

			if( ((int)WM->TimeLeft)%60 < 10 )
			{
				HudFont->Draw( Screen, Screen.GetWidth()/2, 20, true, D3DCOLOR_ARGB( 200, 255, 255, 255 ) , "%d:0%d", ((int)WM->TimeLeft)/60, ((int)WM->TimeLeft)%60 ); 
			}
			else
			{
				HudFont->Draw( Screen, Screen.GetWidth()/2, 20, true, D3DCOLOR_ARGB( 200, 255, 255, 255 ) , "%d:%d", ((int)WM->TimeLeft)/60, ((int)WM->TimeLeft)%60 ); 
			}

			/*if(WM->Viewer.Pos)
			{
				Vector3D a = WM->LocalPlayer.GetLocation();
				Vector3D b = a;
				b.y -= 15;
				
				SceneObject* outObject;
				if( WM->sceneManager->GetKDTree()->CastRay( a, b, NULL, NULL, &outObject ) )
				{
					SomeFont->Draw(Screen, 0, 115, false, D3DCOLOR_XRGB( 255, 255, 255 ), "collision");
				}
			}*/

			

			
			//if( WM->LocalPlayer.GetHealth() < 0) 
			//{
				//HudFont->Draw(Screen, (Real)(Screen.GetWidth()/2), (Real)Screen.GetHeight()/2, true, D3DCOLOR_XRGB( 255, 0, 0 ), "You have died");
				//HudFont->Draw(Screen, (Real)(Screen.GetWidth()/2), (Real)Screen.GetHeight()/2+20, true, D3DCOLOR_XRGB( 255, 0, 0 ), "Press R to restart.");
			//}	
		}
		
		//Draw the GUI
		Screen.D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
		WM->DrawFinal(Screen);
		DrawMovieOverlay();

		
	}

	Screen.D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	
	
	if( WM)
	{
		
		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
		Screen.SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
		Screen.SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);


		//Draw darkness overlays
		Screen.SetRenderState(D3DRS_ZENABLE, FALSE );
		int screenA = (int)(ScreenDarkLevel * 255.0f);
		Screen.DrawRectangle( 0, 0, Screen.GetWidth(), Screen.GetHeight(), D3DCOLOR_ARGB( screenA, 0, 0, 0 ) );
		screenA = (int)(WM->DilationPercent * 255.0f);
		Screen.DrawRectangle( 0, 0, Screen.GetWidth(), Screen.GetHeight(), D3DCOLOR_ARGB( screenA, 0, 0, 0 ) );

		//Draw objective thingamajig
		screenA = 128.0f* ShowObjective;
		int textA = 255*ShowObjective;
		

		if( screenA > 128 )
			screenA = 128;


		if( textA > 255 )
			textA = 255;

		Font2D* objectiveFont = Screen.FM.GetFont( "Tahoma", 12 );
		float centerX=Screen.GetWidth()/2, centerY=Screen.GetHeight()/2;
		
		Screen.DrawRectangle(centerX-200, centerY-50, 400, 100, D3DCOLOR_ARGB( screenA, 0, 96, 220 ) );
		objectiveFont->Draw( Screen, Screen.GetWidth() /2, Screen.GetHeight() /2, true, D3DCOLOR_ARGB( textA, 255, 255, 255 ), "Collect as much treasure as you can\n and return back to your boat\n before time runs out"  ) ;


		//draw the hint
		if( CurrentHint.length() > 0 )
		{
			Screen.DrawRectangle( Screen.GetWidth()-200,  0, 200, 25, HintColor );
			objectiveFont->Draw( Screen, Screen.GetWidth() -100, 10, true, D3DCOLOR_ARGB( 220, 255, 255, 255 ), (char*) CurrentHint.c_str() ) ;
		}

		if( UserInterface.GM.GetState() )
		{
			Screen.DrawRectangle(0, 0, Screen.GetWidth(), Screen.GetHeight(), D3DCOLOR_ARGB( 128, 0, 0, 0 ) );

		}

		
	}

	UserInterface.GM.DrawGUI(Screen);

	Screen.SetRenderState(D3DRS_ZENABLE, TRUE );

	//done with the something something
	Screen.End3D();

	//update the screen
	Screen.UpdateScreen();

}