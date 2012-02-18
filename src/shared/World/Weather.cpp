#include "Weather.h"
#include "..\s3dlib\types.h"
#include "..\ini\ini.h"
#include "world.h"
#include "..\shaders\constants.h"

#include "..\defs\defs.h"

inline Real ValueFromPercent( Real Base, Real Dest, Real Percent )
{
	Real returnvalue = ((Dest-Base)*Percent) + Base;
	return returnvalue;
}

Weather::WeatherState::WeatherState()
{
	ZeroMemory( this, sizeof( WeatherState ) );
	ViewingDistance = 1000;
	FogStart = 100;
	FogEnd   = 1000;

	FR = 255.0f;
	FG = 255.0f;
	FB = 255.0f;
	SR = 255;
	SG = 255;
	SB = 255;
	AR = 0;
	AG = 0;
	AB = 0;
	this->R = 255;
	this->G = 255;
	this->B = 255;


}

Weather::Weather()
{
	this->WMPtr = NULL;
	FutureState = NULL;
	this->ChangeTimeSpent = 0;
	this->TotalChangeTime = 5000;

}

Weather::~Weather()
{

	Cleanup();
}

bool Weather::InitDefaults ( WorldManager* wm )
{
    WMPtr = wm;

	SetRenderStates( *wm->ScreenPtr );

	return true;
}

bool Weather::Init(Screen3D &Screen,char *WeatherFile,Position &LocalPlayer, WorldManager* WM)
{
	char TempBuffer[256];
	GetIniValue(WeatherFile,"graphics","weather_particles",TempBuffer);

	WMPtr = WM;

	CurrentState.Wind.x = GetIniValueFloat(WeatherFile,"graphics","weather_wind_x",TempBuffer);
	CurrentState.Wind.y = GetIniValueFloat(WeatherFile,"graphics","weather_wind_y",TempBuffer);
	CurrentState.Wind.z = GetIniValueFloat(WeatherFile,"graphics","weather_wind_z",TempBuffer);
	CurrentState.FogStart = GetIniValueFloat(WeatherFile,"graphics","weather_fog_start",TempBuffer);
	CurrentState.FogEnd = GetIniValueFloat(WeatherFile,"graphics","weather_fog_end",TempBuffer);
	CurrentState.ViewingDistance = GetIniValueFloat(WeatherFile,"graphics","weather_viewing_distance",TempBuffer);
	CurrentState.R = GetIniValue(WeatherFile,"graphics","weather_background_color_r");
	CurrentState.G = GetIniValue(WeatherFile,"graphics","weather_background_color_g");
	CurrentState.B = GetIniValue(WeatherFile,"graphics","weather_background_color_b");
	CurrentState.FR = GetIniValue(WeatherFile,"graphics","weather_fog_color_r");
	CurrentState.FG = GetIniValue(WeatherFile,"graphics","weather_fog_color_g");
	CurrentState.FB = GetIniValue(WeatherFile,"graphics","weather_fog_color_b");
	CurrentState.SR = GetIniValue(WeatherFile,"graphics","weather_sun_color_r");
	CurrentState.SG = GetIniValue(WeatherFile,"graphics","weather_sun_color_g");
	CurrentState.SB = GetIniValue(WeatherFile,"graphics","weather_sun_color_b");
	CurrentState.AR = GetIniValue(WeatherFile,"graphics","weather_ambient_r");
	CurrentState.AG = GetIniValue(WeatherFile,"graphics","weather_ambient_g");
	CurrentState.AB = GetIniValue(WeatherFile,"graphics","weather_ambient_b");

	GetIniValue( WeatherFile, "graphics", "weather_particles", TempBuffer);

	if( strcmpi( TempBuffer, "nostring" ) != 0 )
	{
		CurrentState.Stream = new ParticleStream;

		CurrentState.Stream->Init(Screen, TempBuffer, WM);		
	}
	else
	{
		CurrentState.Stream = NULL;
	}
		

	SkyBox.Init( Screen, WeatherFile );

	
	SetRenderStates(Screen);



	return true;
}

void Weather::Cleanup()
{
	
	if(FutureState)
		delete FutureState;

	FutureState  = NULL;
}

bool Weather::Update(Real FrameTime, Position &Player)
{
	
	if(CurrentState.Stream)
	{
		CurrentState.Stream->xg = IntermediateState.Wind.x/2;
		CurrentState.Stream->zg = IntermediateState.Wind.z/2;
		//CurrentState.Stream->MoveSpawn( WMPtr->Viewer.Pos->x, WMPtr->Viewer.Pos->y+90, WMPtr->Viewer.Pos->z);
		CurrentState.Stream->TranslateParticles( WMPtr->Viewer.Pos->x+WMPtr->Viewer.OffsetX, WMPtr->Viewer.Pos->y+WMPtr->Viewer.OffsetY+80, WMPtr->Viewer.Pos->z+WMPtr->Viewer.OffsetZ );
		CurrentState.Stream->Update( FrameTime );
	}

	SetRenderStates(*WMPtr->ScreenPtr);

	
	if(FutureState)
	{
		ChangeTimeSpent += 1000 * FrameTime;

		if(ChangeTimeSpent >= TotalChangeTime)
		{
			ChangeTimeSpent = 0;

			if(CurrentState.Stream)
				delete CurrentState.Stream;

			CurrentState = *FutureState;	
			FutureState = NULL;

			SetRenderStates(*WMPtr->ScreenPtr);
		}

		SetRenderStates(*WMPtr->ScreenPtr);
		
	}

	

	return true;
}

bool Weather::Draw(Screen3D &Screen,Camera &Viewer)
{

	SkyBox.Draw( Screen, Viewer, IntermediateState.R, IntermediateState.G, IntermediateState.B);


	if(CurrentState.Stream)
	{
		WMPtr->AddStream(CurrentState.Stream);
	}

	return true;
}

bool Weather::SetRenderStates(Screen3D &Screen, bool LightingOnly)
{
// Turn on fogging in the distance

	IntermediateState = CurrentState;

	if( FutureState)
	{
		Real Percent=ChangeTimeSpent/TotalChangeTime;
		IntermediateState.FR = ValueFromPercent( CurrentState.FR, FutureState->FR, Percent);
		IntermediateState.FG = ValueFromPercent( CurrentState.FG, FutureState->FG, Percent);
		IntermediateState.FB = ValueFromPercent( CurrentState.FB, FutureState->FB, Percent);
		IntermediateState.SR = ValueFromPercent( CurrentState.SR, FutureState->SR, Percent);
		IntermediateState.SG = ValueFromPercent( CurrentState.SG, FutureState->SG, Percent);
		IntermediateState.SB = ValueFromPercent( CurrentState.SB, FutureState->SB, Percent);
		IntermediateState.AR = ValueFromPercent( CurrentState.AR, FutureState->AR, Percent);
		IntermediateState.AG = ValueFromPercent( CurrentState.AG, FutureState->AG, Percent);
		IntermediateState.AB = ValueFromPercent( CurrentState.AB, FutureState->AB, Percent);
		IntermediateState.R = ValueFromPercent( CurrentState.R, FutureState->R, Percent);
		IntermediateState.G = ValueFromPercent( CurrentState.G, FutureState->G, Percent);
		IntermediateState.B = ValueFromPercent( CurrentState.B, FutureState->B, Percent);
		IntermediateState.FogStart = ValueFromPercent( CurrentState.FogStart, FutureState->FogStart, Percent);
		IntermediateState.FogEnd   = ValueFromPercent( CurrentState.FogEnd, FutureState->FogEnd, Percent);
		IntermediateState.Wind.x   = ValueFromPercent( CurrentState.Wind.x, FutureState->Wind.x, Percent);
		IntermediateState.Wind.y   = ValueFromPercent( CurrentState.Wind.y, FutureState->Wind.y, Percent);
		IntermediateState.Wind.z   = ValueFromPercent( CurrentState.Wind.z, FutureState->Wind.z, Percent);

		if(ChangeTimeSpent/TotalChangeTime>.9f)
			IntermediateState.ViewingDistance = ValueFromPercent( CurrentState.ViewingDistance, FutureState->ViewingDistance, Percent);
		else
			IntermediateState.ViewingDistance = CurrentState.ViewingDistance;
		

	}

	if(!LightingOnly)
	{
		Screen.D3DDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
		Screen.D3DDevice->SetRenderState( D3DRS_FOGCOLOR, D3DCOLOR_XRGB((int)IntermediateState.FR, (int)IntermediateState.FG, (int)IntermediateState.FB) );
		Screen.D3DDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
		Screen.D3DDevice->SetRenderState( D3DRS_FOGSTART, *(DWORD *)(&IntermediateState.FogStart)   );
		Screen.D3DDevice->SetRenderState( D3DRS_FOGEND, *(DWORD *)(&IntermediateState.FogEnd)   );
		
		Screen.ChangeViewDepth(IntermediateState.ViewingDistance);
	}

	Screen.R = IntermediateState.R;
	Screen.G = IntermediateState.G;
	Screen.B = IntermediateState.B;

	  // Set up the sun color
    D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = (Real)IntermediateState.SR / 255.0f;
    light.Diffuse.g  = (Real)IntermediateState.SG / 255.0f;
    light.Diffuse.b  = (Real)IntermediateState.SB / 255.0f;
	light.Ambient = light.Diffuse;
	light.Specular = light.Diffuse;
	vecDir = D3DXVECTOR3(0,
                         -1.0f,
                         0);
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    light.Range       = 1000.0f;
    Screen.D3DDevice->SetLight( 0, &light );
    Screen.D3DDevice->LightEnable( 0, TRUE );
	Screen.D3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	

	SetVertexShaderConstants( Screen );

return true;
}

void Weather::SetVertexShaderConstants( Screen3D &Screen )
{
	//Set the light dir
	D3DXVECTOR4 lightDir;
	D3DXVECTOR3 vecDir;
    vecDir = D3DXVECTOR3(0, -10.0f, -1.0f);
    D3DXVec3Normalize( (D3DXVECTOR3*)&lightDir, &vecDir );
	lightDir.w = 1;
	Screen.D3DDevice->SetVertexShaderConstantF(CV_LIGHT_DIRECTION, (const float*)&lightDir, 1);

	
	//set teh light color
	Real r  = (Real)IntermediateState.SR / 255.0f;
    Real g  = (Real)IntermediateState.SG / 255.0f;
    Real b  = (Real)IntermediateState.SB / 255.0f;
	D3DXVECTOR4 light(r, g, b, 1);
	Screen.D3DDevice->SetVertexShaderConstantF(CV_LIGHT_COLOR, (const float*)&light, 1);

	//Set material color to white
	D3DXVECTOR4 materialcolor(1,1,1,1);
	Screen.D3DDevice->SetVertexShaderConstantF(CV_MATERIAL_COLOR, (const float*)&materialcolor, 1);
	
	//set the constants
	D3DXVECTOR4 constants(0,.5f,1,1);
	Screen.D3DDevice->SetVertexShaderConstantF(CV_CONST, (const float*)&constants, 1);
}

void Weather::ChangeState(Screen3D& Screen, Position &Player, char* WeatherFile)
{
	//Set up the future state structure
	char TempBuffer[256];
	GetIniValue(WeatherFile,"graphics","weather_particles",TempBuffer);
	
	if(FutureState)
		delete FutureState;

	SkyBox.Init( Screen, WeatherFile );

	FutureState = new WeatherState;
	FutureState->Wind.x = GetIniValueFloat(WeatherFile,"graphics","weather_wind_x",TempBuffer);
	FutureState->Wind.y = GetIniValueFloat(WeatherFile,"graphics","weather_wind_y",TempBuffer);
	FutureState->Wind.z = GetIniValueFloat(WeatherFile,"graphics","weather_wind_z",TempBuffer);
	FutureState->FogStart = GetIniValueFloat(WeatherFile,"graphics","weather_fog_start",TempBuffer);
	FutureState->FogEnd = GetIniValueFloat(WeatherFile,"graphics","weather_fog_end",TempBuffer);
	FutureState->ViewingDistance = GetIniValueFloat(WeatherFile,"graphics","weather_viewing_distance",TempBuffer);
	FutureState->R = GetIniValue(WeatherFile,"graphics","weather_background_color_r");
	FutureState->G = GetIniValue(WeatherFile,"graphics","weather_background_color_g");
	FutureState->B = GetIniValue(WeatherFile,"graphics","weather_background_color_b");
	FutureState->FR = GetIniValue(WeatherFile,"graphics","weather_fog_color_r");
	FutureState->FG = GetIniValue(WeatherFile,"graphics","weather_fog_color_g");
	FutureState->FB = GetIniValue(WeatherFile,"graphics","weather_fog_color_b");
	FutureState->SR = GetIniValue(WeatherFile,"graphics","weather_sun_color_r");
	FutureState->SG = GetIniValue(WeatherFile,"graphics","weather_sun_color_g");
	FutureState->SB = GetIniValue(WeatherFile,"graphics","weather_sun_color_b");
	FutureState->AR = GetIniValue(WeatherFile,"graphics","weather_ambient_r");
	FutureState->AG = GetIniValue(WeatherFile,"graphics","weather_ambient_g");
	FutureState->AB = GetIniValue(WeatherFile,"graphics","weather_ambient_b");
	Screen.R = FutureState->R;
	Screen.G = FutureState->G;
	Screen.B = FutureState->B;

	
	Screen.ChangeViewDepth(FutureState->ViewingDistance);

	GetIniValue( WeatherFile, "graphics", "weather_particles", TempBuffer);

	if( strcmpi( TempBuffer, "nostring" ) != 0 )
	{
		FutureState->Stream = new ParticleStream;

		FutureState->Stream->Init(Screen, TempBuffer, WMPtr );
				
	}
	else
	{
		FutureState->Stream = NULL;
	}

	SetRenderStates(Screen);

}
