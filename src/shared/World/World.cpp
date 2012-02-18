#include <windows.h>

#include "world.h"
#include "..\ini\ini.h"
#include "tree.h"
#include "..\imageloader\image.h"
#include "weapon.h"
#include "StaticObject.h"
#include "character.h"
#include "..\shaders\constants.h"
#include "enemy.h"
#include "networkobject.h"
#include "vehicleobject.h"
#include "tankobject.h"
#include "crate.h"
#include "powerup.h"
#include "scenemanager.h"
//#define TIXML_USE_STL
#include "..\tinyxml\tinyxml.h"
#include "..\xml\xml.h"



using namespace std;



int TexGenTime=0;

union YawX
{
	float x;
	float yaw;
};
struct Test
{
	YawX lols;
};



inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

WorldManager::WorldManager( Screen3D* screenptr, MeshManager* mmptr, SoundManager* am, GameTimer* timer, NetworkManager* nm ) 
: progressWidget( screenptr )
{

	//record system thingies
	ScreenPtr = screenptr;
	MMPtr	  = mmptr;
	GTPtr     = timer;
	AMPtr     = am;
	NMPtr     = nm;

	//set values to somethings
	
	WaterMap = NULL;
	sceneManager = NULL;
	terrain      = NULL;
	WaterBumpMap = NULL;
	waterProjectionTexture = NULL;
	
	SeaLevel = -1;

	ObjectsDrawn  = 0;
	ObjectsTested = 0;
	RecursionCount= 0;

	TimeLeft = 10*60;

	rain = NULL;


//	StartedDeathSequence = false;
//	RespawnTime = 5000.0f;
	


	WaterDamage  = 0;

	UseAvatar = true;
	DrawSky = true;
	DrawingWater = false;
	DrawingReflection = false;
	
	AllowWaterDraw = true;
	SeaDensity = 16;
	InputFocus = &LocalPlayer;

	TextureFilterMode = D3DTEXF_ANISOTROPIC;



	this->DilationPercent = 0;
	//loading stuff
	LoadPercent = 0.0f;

	

	ZeroMemory( ParticleBuffer, MAX_DRAWABLE_PARTICLES*4 );

}


struct PhongShadedLine
{
	Vector3D PointA, NormalA;
	Vector3D PointB, NormalB;
	Vector3D TestPoint;
	Vector3D InterpolatedResult;
	D3DXPLANE CuttingPlane;
	
};


Vector3D InterpolateVectors(Vector3D& NormalA, Vector3D& NormalB, Real Percent )
{
	Vector3D FinalVector;
	D3DXVec3Lerp(
			(D3DXVECTOR3*)&FinalVector,
			(D3DXVECTOR3*)&NormalA,
			(D3DXVECTOR3*)&NormalB,
			Percent
			); 
	FinalVector.Normalize();
	return FinalVector;
}


void WorldManager::SetMusic ( char * MusicFile )
{

}



bool WorldManager::InitXML( const char* mod, const char* mapname)
{
	//start timing
	char Message[512];
	int startTime = timeGetTime();
	
	//create the scene manager
	sceneManager = new SceneManager( this );

	m_imposterManager.Init( *ScreenPtr, *MMPtr, 22.5 );

	//HAR!
	LoadPercent = 0.0f;


	
	//Init the shaders
	CVar.WriteLine( "Initializing water shader" );
	InitWaterShader();

	LoadPercent = 0.02f;

	//Load in the map
	if( !LoadMapXML( string(mod), string(mapname) ) )
		return false;
	LoadPercent = 0.5f;

	CVar.WriteLine( "LoadMap Succeeded" );


	//start recording the time it takes to build the tree
	int startTimeBT = timeGetTime();


	//record time spent on building the tree
	float timeSpentBT = timeGetTime() - startTimeBT;
	sprintf( Message, "Build Tree: %f     ", (float)(timeSpentBT/1000.0f) );
	LoadString += Message;

	WorldWeather.SetRenderStates( *ScreenPtr, true );
	
	LoadPercent = 0.9f;



	//Build scene manager stuffage
	CVar.WriteLine( "Building quadtree" );
	sceneManager->BuildTree();
	sceneManager->BuildCollisionTree();

	//record time spent on loading map
	float timeSpent = timeGetTime() - startTime;
	sprintf( Message, "Load Time: %f", (float)(timeSpent/1000.0f) );
	LoadString += Message;

	LoadPercent = 1.0f;

	WorldWeather.SetRenderStates( *ScreenPtr );

	InitSeaGeometry();

	CVar.WriteLine("Map Loaded" );

	return true;

}

void WorldManager::KickupDust( Real WorldX, Real WorldZ, Real Radius )
{
	/*Position DustPosition;
	DustPosition.x = WorldX;
	DustPosition.z = WorldZ;
	Real Height = this->GetHeightValue( DustPosition );

	DustParticles.MoveSpawn( WorldX, Height, WorldZ);
	DustParticles.SpawnParticles( Radius, false );*/
	
}



void WorldManager::RestartLevel()
{
	delete sceneManager;
	sceneManager = new SceneManager( this );

	LocalPlayer.SetHealth (1000);
	LocalPlayer.DropAll();
}

//new map loading functions! HOTHOTHOT
bool WorldManager::LoadMapXML( std::string& mod, std::string& mapname )
{
	MapName = mapname;
	ModName = mod;

	std::string path = "..\\";
	path += mod;
	path += "\\maps\\";
	path += mapname;
	path += "\\";



	//Setup basic stuff
	WorldWeather.InitDefaults( this );
	//WorldWeather.Init( *this->ScreenPtr, "..\\base\\weather\\weather_stormy.ini", LocalPlayer.Location, this );

	CVar.WriteLine( "Loading header" );
	//Load header
	if( ! LoadHeaderInfo( (char*)(path + "project.xml").c_str() ) )
		return false;
	
	CVar.WriteLine( "Loading lightmap" );
	LM.LoadLightmap( *ScreenPtr ,(path+"shadows.png" ).c_str());
	progressWidget.Tick();

	//Initialize the terrain
	CVar.WriteLine( "Loading terrain" );
	terrain = new Terrain( this, (path + "heightmap.bin") .c_str(), (path + "layers.layer").c_str() );

	///initialize the solver
	CVar.WriteLine( "Initializing solver" );
	Solver.Init( 8, GetWidth() * GetSpacing(), GetHeight() * GetSpacing() );

	terrain->InitializeGeom();


	CVar.WriteLine( "Loading map objects" );
	//load in objects
	if( ! LoadObjectsXML( (char*)(path + "objects.mb2").c_str()  )  )
		return false;
	

	rain = new Rain(this);

	return true;
}



bool WorldManager::LoadHeaderInfo( char* projectdotxml )
{
	TiXmlDocument doc;
	if ( doc.LoadFile( projectdotxml ) == false )
		return false;

	char* t;

	progressWidget.NewTask( "Loading Map Header" );

	//read root element
	TiXmlElement * root = doc.RootElement();
	if( root )
	{
		//Read in spacing between vertices
		if( xmlnodeexists( root, "Spacing" ) )
			Spacing = xmlreadf( root, "Spacing" );	

		//Read in information about the water
		TiXmlElement* sea = root->FirstChildElement( "Sea" );
		if( sea )
		{
			//read in the sea level
			SeaLevel = xmlreadf( sea, "SeaLevel" );
			
			//read in the water texture
			hWaterTexture = ScreenPtr->GetTextureHandle( (char*)xmlreads( sea, "WaterTexture" ) );
			progressWidget.Tick();
		}

		//Read in information about the atmosphere
		TiXmlElement* xmlatmosphere = root->FirstChildElement( "Atmosphere" );
		if(xmlatmosphere )
		{	
			if( xmlnodeexists( xmlatmosphere, "Sky" ) )
			{
				WorldWeather.SkyBox.Init( ScreenPtr, xmlgetnode( xmlatmosphere, "Sky" ) );
			}
			
			if( xmlnodeexists( xmlatmosphere, "ParticleSource" )  ) {
				const char* streamFile = xmlreads(xmlatmosphere, "ParticleSource");
				WorldWeather.CurrentState.Stream = new ParticleStream();
				if(!WorldWeather.CurrentState.Stream->Init(*ScreenPtr, (char*)streamFile, this))
					WorldWeather.CurrentState.Stream = NULL;
				delete streamFile;
				//DrawSky = xmlreadb( xmlatmosphere, "ParticleSource" );
			}


			if( xmlnodeexists( xmlatmosphere, "DrawSkyBox" )  )
				DrawSky = xmlreadb( xmlatmosphere, "DrawSkyBox" );
			else
				DrawSky = false;

			if( xmlnodeexists( xmlatmosphere, "FogStart" ) )
				WorldWeather.CurrentState.FogStart = xmlreadf( xmlatmosphere, "FogStart" ) ;
			else
				WorldWeather.CurrentState.FogStart = 100;

			if( xmlnodeexists( xmlatmosphere, "FogEnd" ) )
				WorldWeather.CurrentState.FogEnd = xmlreadf( xmlatmosphere, "FogEnd" ) ;
			else
				WorldWeather.CurrentState.FogEnd = 1000;

			if( xmlnodeexists( xmlatmosphere, "ViewDistance" ) )
				WorldWeather.CurrentState.ViewingDistance = xmlreadf( xmlatmosphere, "ViewDistance" ) ;
			else
				WorldWeather.CurrentState.ViewingDistance = 1000;

			TiXmlElement* fogcolor = (TiXmlElement*)xmlgetnode	( xmlatmosphere, "FogColor" );
			if( fogcolor )
			{
				double r, g, b;
				fogcolor->Attribute( "r", &r );
				fogcolor->Attribute( "g", &g );
				fogcolor->Attribute( "b", &b );

				WorldWeather.CurrentState.R = r;
				WorldWeather.CurrentState.G = g;
				WorldWeather.CurrentState.B = b;
				WorldWeather.CurrentState.FR = r;
				WorldWeather.CurrentState.FG = g;
				WorldWeather.CurrentState.FB = b;
				

			}

			TiXmlElement* suncolor = (TiXmlElement*)xmlgetnode	( xmlatmosphere, "SunColor" );
			if( suncolor )
			{
				double r, g, b;
				suncolor->Attribute( "r", &r );
				suncolor->Attribute( "g", &g );
				suncolor->Attribute( "b", &b );
				WorldWeather.CurrentState.SR = r;
				WorldWeather.CurrentState.SG = g;
				WorldWeather.CurrentState.SB = b;
			}

			TiXmlElement* amblight = (TiXmlElement*)xmlgetnode	( xmlatmosphere, "AmbientLight" );
			if( amblight )
			{
				double r, g, b;
				amblight->Attribute( "r", &r );
				amblight->Attribute( "g", &g );
				amblight->Attribute( "b", &b );
				WorldWeather.CurrentState.AR = r;
				WorldWeather.CurrentState.AG = g;
				WorldWeather.CurrentState.AB = b;
			}

		}

	}

	progressWidget.Progress( 1.0f );
	

	return true;
}


bool WorldManager::LoadObjectsXML( char* filename )
{
	//Declarations
	char strbuffer[1024];
	
	progressWidget.NewTask( "Loading Objects" );
	//Load document
	TiXmlDocument doc;
	if ( doc.LoadFile( filename ) == false)
		return false; 

	//read root element
	TiXmlElement * root = doc.RootElement();
	if( root )
	{

		//Read in all entities 
		for( TiXmlElement* node = root->FirstChildElement(  );
			node;
			node = node->NextSiblingElement( ) )
		{
			if( strcmp( node->Value(), "entity" ) != 0)
				continue;


			//object attributes
			string objType=""; 
			string objTypePath="";
			string objScriptName="";
			Vector3D objLocation;
			Vector3D objRotation;

			//Read in various attributes of node
			objType		  = xmlreads( node, "type" );
			objTypePath   = xmlreads( node, "typepath" );

			if(  xmlreads( node, "scriptname" ) )
				objScriptName = xmlreads( node, "scriptname" );
			
			objLocation.x = xmlreadattributef( node, "location", "x" );
			objLocation.y = xmlreadattributef( node, "location", "y" );
			objLocation.z = xmlreadattributef( node, "location", "z" );

			objRotation.y = xmlreadattributef( node, "rotation", "x" );
			objRotation.x = xmlreadattributef( node, "rotation", "y" );
			objRotation.z = xmlreadattributef( node, "rotation", "z" );


			//Make the object

			if( objType.compare( "avatar" ) == 0 )
			{
				LocalPlayer.Location.SetLocation ( objLocation );

				LocalPlayer.Init(*ScreenPtr, this, *MMPtr, *AMPtr,"..\\base\\art\\characters\\avatar.ini" );
				LocalPlayer.SetPosition( objLocation.x, objLocation.y, objLocation.z );
				LocalPlayer.SetYaw( objRotation.y );
				LocalPlayer.SpawnPoint = LocalPlayer.GetLocation();
				Viewer.Yaw = LocalPlayer.GetYaw();
				//Viewer.Yaw = 0;
				//Viewer.Pitch = 0;
			}
			else
			{
				SceneObject* newObject = sceneManager->MakeObject( objType.c_str() );		
				newObject->SetLocation( Position( objLocation, objRotation ) ); //set's location twice (once before, and once after)
				newObject->Init( *ScreenPtr, this, *MMPtr, *AMPtr, (char*) objTypePath.c_str() );
				newObject->SetName( (char*)objScriptName.c_str() );
				newObject->SetLocation( Position( objLocation, objRotation ) );
				sceneManager->AddObject( newObject );
			}

			progressWidget.Tick();
			
		}

	}
	progressWidget.Progress( 1.0f );
	return true;
}
//bool LoadEffectFile ( char* filename );
	


bool WorldManager::InitWaterShader()
{	
	//Setup the bump map

    // Create the bump map texture
	int bumpWidth = 256, bumpHeight = 256;
    if( FAILED( ScreenPtr->D3DDevice->CreateTexture( bumpWidth, bumpHeight, 1, 0 /* Usage */,
                                             D3DFMT_V8U8, D3DPOOL_MANAGED,
                                             &WaterBumpMap, NULL ) ) )
        return false;

    // Lock the surface and write in some bumps for the waves
    D3DLOCKED_RECT d3dlr;
    WaterBumpMap->LockRect( 0, &d3dlr, 0, 0 );
    CHAR* pDst = (CHAR*)d3dlr.pBits;
    CHAR  iDu, iDv;

    for( DWORD y=0; y<bumpHeight; y++ )
    {
        CHAR* pPixel = pDst;

        for( DWORD x=0; x<bumpWidth; x++ )
        {
            FLOAT fx = x/(FLOAT)bumpWidth - 0.5f;
            FLOAT fy = y/(FLOAT)bumpHeight - 0.5f;

            FLOAT r = sqrtf( fx*fx + fy*fy );

            iDu  = (CHAR)( 64 * cosf( 300.0f * r ) * expf( -r * 5.0f ) );
            iDu += (CHAR)( 32 * cosf( 150.0f * ( fx + fy ) ) );
            iDu += (CHAR)( 16 * cosf( 140.0f * ( fx * 0.85f - fy ) ) );

            iDv  = (CHAR)( 64 * sinf( 300.0f * r ) * expf( -r * 5.0f ) );
            iDv += (CHAR)( 32 * sinf( 150.0f * ( fx + fy ) ) );
            iDv += (CHAR)( 16 * sinf( 140.0f * ( fx * 0.85f - fy ) ) );

            *pPixel++ = iDu;
            *pPixel++ = iDv;
        }
        pDst += d3dlr.Pitch;
    }
    WaterBumpMap->UnlockRect(0);

	D3DXMatrixIdentity( &WaterBumpMatrix);
	


	//Hooray!
	return true;
}



void WorldManager::InitSeaGeometry()
{
	WaterVertex* List;
	SeaVB.Init( *ScreenPtr, D3DFVF_WATERVERTEX, sizeof( WaterVertex ), SeaDensity * SeaDensity );
	SeaVB.Lock((BYTE**)&List);

	Real Start=-20000;
	Real StepSize=(GetWidth()*GetSpacing()+40000) / SeaDensity;
	for( int y=0; y< SeaDensity; y++)
	{
		for( int x=0; x< SeaDensity; x++)
		{
			List[x+y*SeaDensity].position.x = Start+StepSize*x;
			List[x+y*SeaDensity].position.y = SeaLevel;
			List[x+y*SeaDensity].position.z = Start+StepSize*y;
			List[x+y*SeaDensity].color    = D3DCOLOR_ARGB( 128, 255, 255, 255 );
			List[x+y*SeaDensity].tu       = x*10;
			List[x+y*SeaDensity].tv       = y*10;
		}
	}


	SeaVB.Unlock( );


	//setup the sea IB
	WORD* IndexList;

	SeaIB.Init(*ScreenPtr, ((SeaDensity-1)*(SeaDensity-1)*6), IB16Bit);
	SeaIB.Lock(&IndexList);

	int IndexOffset=0;

	for(int y=0; y<SeaDensity-1; y++)
	{
		for(int x=0; x<SeaDensity-1; x++)
		{
			//add indices to the index buffer	
			IndexList[IndexOffset] = x+y*SeaDensity;
			IndexList[IndexOffset+1] = x+((y+1)*SeaDensity);
			IndexList[IndexOffset+2] = (x+1)+y*SeaDensity;
			IndexList[IndexOffset+3] = x+(y+1)*SeaDensity;
			IndexList[IndexOffset+4] = (x+1)+((y+1)*SeaDensity);
			IndexList[IndexOffset+5] = (x+1)+y*SeaDensity;
			IndexOffset += 6;
				
		}
	}

	SeaIB.Unlock();
}





SceneHandle WorldManager::Add ( SceneObject* object)
{
	return sceneManager->AddObject( object );
}

SceneObject* WorldManager::MakeObject( std::string& type, std::string& typePath, std::string& scriptName, Vector3D& location, Vector3D& rotation )
{	
	//declarations
	SceneObject* sceneobject = NULL;

	//Place object
	Position Location( location, rotation );
	Location.y = GetHeightValue( Location );


	if (strcmpi( type.c_str(), "character" ) == 0 )
	{
		sceneobject = new Character();
	}

	else if (strcmpi( type.c_str(), "enemy" ) == 0 )
	{
		sceneobject = new Enemy();
	}

	else if( strcmpi(type.c_str(), "weapon") == 0 )
	{
		sceneobject = new Weapon();
	}

	
	else if( strcmpi(type.c_str(), "vehicle4wheeled") == 0 )
	{
		sceneobject = new VehicleObject();	
	}

	else if( strcmpi(type.c_str(), "tank") == 0 )
	{
		sceneobject = new TankObject();	
	}

	
	else if( strcmpi(type.c_str(), "crate") == 0 )
	{
		sceneobject = new Crate();	
	}


	else if( strcmpi(type.c_str(), "random_tree") == 0 )
	{
		sceneobject = new ProceduralTree();
	}

	else if( strcmpi(type.c_str(), "powerup") == 0 )
	{
		sceneobject = new Powerup();
	}

	else
	{
		SceneObject* sceneobject = new StaticObject();	
		
	}


	sceneobject->SetLocation( Location );
	sceneobject->SetYaw(   rotation.x );
	sceneobject->SetPitch( rotation.y );
	sceneobject->SetRoll(  rotation.z );
	sceneobject->SetName((char*) scriptName.c_str() ); 
	sceneobject->Init(*ScreenPtr, this, *MMPtr, *AMPtr, (char*)type.c_str() );
	sceneobject->AdjustBoundingVolume();

	
	return sceneobject;		

}

SceneHandle WorldManager::Add ( std::string& type, std::string& typePath, std::string& scriptName, Vector3D& location, Vector3D& rotation )
{

	//Loop through all object slots
	return sceneManager->AddObject( MakeObject( type, typePath, scriptName, location, rotation ) );
    
}

void WorldManager::Cleanup()
{
	if(rain)
		delete rain;

	Solver.Shutdown();

	if( terrain)
		delete terrain;

	if(sceneManager)
		delete sceneManager;
	

	terrain = NULL;
	sceneManager = NULL;
		
}

void WorldManager::Kill ( SceneHandle Who )
{
	SceneObject* o = sceneManager->RemoveObject( Who );
	if (o )
		delete o;
	
}

void WorldManager::Update( Real FrameTime )
{
	//InMovie = false;
	//TopMovieText = "";
	//BottomMovieText = "";

	/*if(LocalPlayer.GetHealth() <= 0  && StartedDeathSequence == false)
	{
		RespawnTime			 = 5.0f;
		StartedDeathSequence = true;
	}*/
	if (LocalPlayer.GetHealth() <= 0  && LocalPlayer.RespawnTime > 0 )
	{
		/*RespawnTime -= FrameTime;
		if( RespawnTime <= 0 )
			RespawnTime = 0;*/
		DilationPercent = 1.0f - (LocalPlayer.RespawnTime/5.0f) ;
		TimeDilated = true;

	}

	TimeLeft -= FrameTime;
			
	Solver.Step( FrameTime, this );

	START_TIMING

	sceneManager->UpdateAll( FrameTime );


	if(Viewer.Pos )
		WorldWeather.Update(FrameTime, *Viewer.Pos);

	if(rain)
		rain->Update(FrameTime);

	if(UseAvatar)
		LocalPlayer.Update( FrameTime, this );
	

	DustParticles.Update( this->GTPtr->GetFrameTime());
	Viewer.Update( FrameTime );

	//update water
	float r = 0.04f;
	double t = (double)GTPtr->GetTime();
    WaterBumpMatrix._11 =  r * cosf( t * 11.0f );
    WaterBumpMatrix._12 = -r * sinf( t * 11.0f );
    WaterBumpMatrix._21 =  r * sinf( t * 11.0f );
    WaterBumpMatrix._22 =  r * cosf( t * 11.0f );


	terrain->UpdateLOD( &this->Viewer.Pos->GetVector() );
	terrain->Update();
	
	STOP_TIMING
}


//Particle functions
void WorldManager::AddStream( ParticleStream* stream )
{
	//loop through and find an empty spot to place it
	for(int i=0; i<MAX_DRAWABLE_PARTICLES; i++)
	{
		//blank spot?
		if(ParticleBuffer[i] == NULL)
		{
			//assign the texture a spot
			ParticleBuffer[i] = stream;
			
			//done, return
			return;
		}

	}
}

void WorldManager::ClearStreamList( )
{

	//loop through and kill things
	for(int i=0; i<MAX_DRAWABLE_PARTICLES; i++)
		ParticleBuffer[i] = NULL;
	
}

void WorldManager::DrawReflectedScene( Screen3D& Screen )
{
	
	//set camera transform
	Matrix LookAtMatrix = Viewer.GetLookAtMatrix( Viewer.Zoom );
	Screen.SetViewTransform( LookAtMatrix, true );

	WorldWeather.SkyBox.DrawReflected( Screen, Viewer , 255, 255, 255 );

	Screen.SetViewTransform( LookAtMatrix, true );

	//create reflection matrix
	Matrix reflectMat, transformMat, finalMat;
	D3DXPLANE waterPlane( 0, 1, 0, 0);
    D3DXPLANE clipPlane( 0, -1, 0, SeaLevel);
	D3DXPLANE clipPlane2( 0, 1, 0, SeaLevel);
	D3DXMatrixReflect ( &reflectMat.Mat, &waterPlane);

	transformMat.Translate( 0,SeaLevel*2, 0 );
	finalMat = reflectMat * transformMat;

	Screen.SetTransform( finalMat.Mat  );
	ScreenPtr->D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	//Draw the landscape
	Screen.D3DDevice->SetClipPlane(0, (const float*)&clipPlane );
	Screen.SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );

	Screen.SetRenderState( D3DRS_LIGHTING, FALSE );
	Screen.SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	

	DrawingWater = false;
	//DrawTree(Tree, false);
	//this->sceneManager->DrawAll();
	Screen.D3DDevice->SetClipPlane(0, (const float*)&clipPlane2 );
	Screen.SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE );

}

void WorldManager::DrawBasicSea( )
{
	//draw teh water
	ScreenPtr->SetRenderState(D3DRS_LIGHTING, FALSE );
	ScreenPtr->SetRenderState(D3DRS_FOGENABLE, TRUE );
	ScreenPtr->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );
	ScreenPtr->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	ScreenPtr->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	ScreenPtr->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	ScreenPtr->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	ScreenPtr->SetTransform( Matrix::MakeIdentity() );
	ScreenPtr->SetTexture( hWaterTexture, 0  );
	ScreenPtr->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	ScreenPtr->SetVertexShader(NULL);
	ScreenPtr->DrawPrimitiveVB(&SeaVB, &SeaIB, D3DPT_TRIANGLELIST, D3DFVF_WATERVERTEX ); 
	ScreenPtr->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );


}


void WorldManager::DrawSea( Screen3D& Screen )
{
	// draw the water
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    ScreenPtr->D3DDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	/*ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );*/
	ScreenPtr->SetLinearTextureFiltering();

	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
	ScreenPtr->D3DDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2);

	// Set up texture stage 0's states for the bump map
	ScreenPtr->D3DDevice->SetTexture( 0, WaterBumpMap );
	
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT00,   F2DW( WaterBumpMatrix._11 ) );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT01,   F2DW( WaterBumpMatrix._12 ) );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT10,   F2DW( WaterBumpMatrix._21 ) );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT11,   F2DW( WaterBumpMatrix._22 ) );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVLSCALE,  F2DW(0.8f) );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_BUMPENVLOFFSET, F2DW(0.0f) );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_BUMPENVMAPLUMINANCE );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );

	// Set up texture stage 1's states for the environment map
	ScreenPtr->SetTexture( waterProjectionTexture, 1 );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    				

	Matrix m;
	Screen.SetTransform( m );
	

	// Set up projected texture coordinates
	D3DXMATRIXA16 mat;
	mat._11 = 0.48f; mat._12 = 0.0f;   mat._13 = 0.0f;
	mat._21 = 0.0f;  mat._22 = -0.64f; mat._23 = 0.0f;
	mat._31 = -0.5f; mat._32 = 0.5f;   mat._33 = 1.0f;
	mat._41 = 0.0f;  mat._42 = 0.0f;   mat._43 = 0.0f;
	ScreenPtr->D3DDevice->SetTransform( D3DTS_TEXTURE1, &mat );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3|D3DTTFF_PROJECTED );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 1 );
						
	//draw teh water
	ScreenPtr->SetRenderState(D3DRS_LIGHTING, FALSE );
	ScreenPtr->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );
	ScreenPtr->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	ScreenPtr->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	ScreenPtr->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	ScreenPtr->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	Screen.SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	Screen.SetVertexShader(NULL);
	Screen.DrawPrimitiveVB(&SeaVB, &SeaIB, D3DPT_TRIANGLELIST, D3DFVF_WATERVERTEX ); 
	Screen.SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	//undo states
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	ScreenPtr->D3DDevice->SetTexture(1, NULL);

}



void WorldManager::Draw( Screen3D& Screen )
{

	START_TIMING

	// Set the transforms back to default
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    Screen.SetTransform( matWorld );
	Screen.D3DDevice->SetTransform( D3DTS_VIEW, &matWorld );

	
	// Get the current lookat matrix
	
	Matrix LookAtMatrix = Viewer.GetLookAtMatrix( Viewer.Zoom );
	Screen.SetViewTransform( LookAtMatrix, true );

	if(DrawSky)
		WorldWeather.Draw(Screen,Viewer);
	else
		AddStream(WorldWeather.CurrentState.Stream);
	
	Screen.SetViewTransform( LookAtMatrix, true );
	ObjectsDrawn   = 0;
	ObjectsTested  = 0;
	RecursionCount = 0;

	//Set the landscape drawing render states
	Matrix M;
	ScreenPtr->SetTransform(M);

	
	//Draw the landscape and all the objects

	START_TIMING_BLOCK("Terrain::Draw" );
	DrawingWater = false;
	ScreenPtr->D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	ScreenPtr->SetRenderState( D3DRS_LIGHTING, TRUE );
	ScreenPtr->SetTextureFiltering(TextureFilterMode);
	ScreenPtr->D3DDevice->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, 8 );
	terrain->Draw();
	STOP_TIMING_BLOCK("Terrain::Draw" );

	START_TIMING_BLOCK("SceneManager::DrawAll");
	Screen.SetRenderState( D3DRS_FOGENABLE, TRUE );
	Screen.SetRenderState( D3DRS_LIGHTING, TRUE );
	sceneManager->DrawAll();
	STOP_TIMING_BLOCK("SceneManager::DrawAll");

	
	Solver.DrawAll(Screen);
	DustParticles.Draw( Screen, this->Viewer);

	//Draw the player if it's in 3rd person view
	if( UseAvatar)
	{
		LocalPlayer.Draw(Screen, *MMPtr, Viewer, *GTPtr);
	}

	//draw vegetation
	START_TIMING_BLOCK( "VegMan.DrawAll" )
	Screen.SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	//VegMan.DrawAll( Screen, *MMPtr , &Viewer );

	STOP_TIMING_BLOCK( "VegMan.DrawAll" )
	
	//Return the textures to the previous state
	ScreenPtr->SetTextureMode( TMNormal );
	ScreenPtr->SetTexture( -1, 1 );


	
	//draw the ocean
	if( AllowWaterDraw)
		DrawBasicSea(  );


	
	//loop through and find things in the buffer to draw
	for(int i=0; i<MAX_DRAWABLE_PARTICLES; i++)
	{
		//filled spot?
		if(ParticleBuffer[i])
		{
			//draw the crazy faceman!
			ParticleBuffer[i]->Draw(Screen, this->Viewer);
		}

	}
	if(rain)
		rain->Draw();

	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	Screen.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	Screen.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
	m_imposterManager.DrawAll( *ScreenPtr, *MMPtr );
	
	ClearStreamList();

    //wo0t, done
	STOP_TIMING
}




Real WorldManager::GetPlayerDistance( Position& Location )
{
	Vector3D DistanceVector;
	DistanceVector.x = LocalPlayer.GetLocation().x - Location.x;
	DistanceVector.y = LocalPlayer.GetLocation().y - Location.y;
	DistanceVector.z = LocalPlayer.GetLocation().z - Location.z;

	return DistanceVector.Mag();
}


bool WorldManager::InWater( Position &ObjectLocation, Vector3D* WaterFlow  )
{
	if(ObjectLocation.y <= GetHeightValue( ObjectLocation, NULL, WaterFlow, NULL, NULL, true  ))
	{
		return true;
	}

	return false;
}


struct CollisionBlock
{
	Vector3D point;
	Vector3D normal;
	SceneObject* object;
	float distance;

	CollisionBlock()
	{
		object = NULL;
	}
	CollisionBlock( Vector3D p, Vector3D n, SceneObject* o, float d )
	{
		point  = p;
		normal = n;
		object = o;
		distance = d;
	}

};

int compareCollisions( const void* a, const void* b )
{
	CollisionBlock* ca =(CollisionBlock*)a;
	CollisionBlock* cb =(CollisionBlock*)b;

	if( ca->distance < cb->distance )
		return -1;
	if( ca->distance > cb->distance )
		return 1;
	return 0;
}

void collisionsCallback (void *data, dGeomID o1, dGeomID o2)
{
	std::vector<CollisionBlock> * collisions=(std::vector<CollisionBlock> * ) data;

	dContact contact[16];
	int n = dCollide( o1, o2, 16,  &contact[0].geom, sizeof( dContact)  );

	for ( int i=0; i<n; i++ )
	{
		
		Vector3D p, n;
		p = Vector3D( contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2] );
		n = Vector3D( contact[i].geom.normal[0], contact[i].geom.normal[1], contact[i].geom.normal[2] );

		
		collisions->push_back( CollisionBlock( p, n, NULL, -1 ) );
		
		/*dBodyID b1, b2;
		b1 = dGeomGetBody(contact.geom.g1);
		b2 = dGeomGetBody(contact.geom.g2);*/
	}
}


bool WorldManager::CheckBulletCollision( Vector3D a, Vector3D b,  Vector3D* outPoint, Vector3D* outNormal,  SceneObject** objectHit, SceneObject* objectToIgnore )
{
	int count=0;
	CollisionBlock* blocks;
	Vector3D oP, oN;
	SceneObject* so;
	CollisionInfo ci;

	//cache some data
	Vector3D c = (b-a);
	float length = c.Length();

	if(length < 0.00001) return false;
	//create the ray
	dGeomID ray = dCreateRay( NULL , length );

	//set its position
	dGeomRaySet( ray, a.x, a.y, a.z, (b-a).x, (b-a).y, (b-a).z  );
	dGeomRaySetLength( ray, length );

	std::vector< CollisionBlock > collisions;
	//dSpaceCollide2	( ray, (dGeomID)Solver.GetActiveSpaceID(), &collisions, collisionsCallback );
	dSpaceCollide2	( ray, (dGeomID)Solver.GetSpaceID(true), &collisions, collisionsCallback );

	dGeomDestroy( ray );

	//if( sceneManager->BasicCheckForCollision( a, b, oP, oN, &so, objectToIgnore ) )
	//	collisions.push_back( CollisionBlock( oP, oN, so, (oP-a).MagSq() ));
	
	count = collisions.size();
	if( count)
	{
		blocks = new CollisionBlock[count];
	
		for(int i=0; i<count; i++)
		{
			blocks[i] = collisions[i];
			blocks[i].distance = (blocks[i].point - a ).MagSq();
		}	
	
		qsort( blocks, count, sizeof(CollisionBlock ), compareCollisions );
		*outPoint  = blocks[0].point;
		*outNormal = blocks[0].normal;
		*objectHit = blocks[0].object;
		delete [] blocks;
		return true;
	}

	
	return false;
	
}

bool WorldManager::CheckForCollision( Vector3D& p1, Vector3D& p2,  CollisionInfo* Collision, bool CheckPlayer,  bool LightCheck )
{

	//Check dynamic object lists for collisions

	if( sceneManager->CheckForCollision( &p1, &p2, Collision, true, LightCheck) )
		return true;
	

	//check the vegetation
	if( VegMan.CheckForCollision ( p1, p2, Collision ) )
		return true;

	//check the constructions
	if ( Solver.CheckParticleCollision( p1, p2, Collision, MMPtr ) )
		return true; 

	if(CheckPlayer)
	{
		if(LocalPlayer.Collision( p1, p2, Collision) )
			return true;
	}

	return false;


}




SceneObject* WorldManager::FindNearestObject( Position& Location, Real MaxDistance,  SceneObject* objectToIgnore)
{
	return sceneManager->FindNearestObject(&Location.GetVector(), MaxDistance, objectToIgnore ) ;
}

SceneHandle WorldManager::GetScriptObjectHandle( char* name )
{
	return sceneManager->GetObjectByScriptName( name );
}

SceneObject* WorldManager::GetDynamicObject( SceneHandle h ) 
{ 
	return sceneManager->GetObjectPtr(h); 
}



void WorldManager::DrawFinal( Screen3D& Screen )
{


}
