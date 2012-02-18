// I N C L U D E S //////////////////////////////////
#pragma once
#include "..\OS\SSL.h"
#include "..\..\Game\App.h"

#include "..\defs\defs.h"

// F U N C T I O N S ///////////////////////////////

class ScriptAPI
{
public:
		
	static void ScriptAPI::AddApiFunctions( Script* s )
	{
		s->VM.AddFunc("WriteLine", WriteLine, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("SetVarS", SetVarS, 2, PARAMS_EXACT, s);
		s->VM.AddFunc("SetVarF", SetVarF, 2, PARAMS_EXACT, s);
		s->VM.AddFunc("SetVarI", SetVarI, 2, PARAMS_EXACT, s);
		s->VM.AddFunc("GetVarS", GetVarS, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("GetVarF", GetVarF, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("GetVarI", GetVarI, 1, PARAMS_EXACT, s);

		s->VM.AddFunc("MovieMode", MovieMode, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("SkipMovie", SkipMovie, 0, PARAMS_EXACT, s);
		s->VM.AddFunc("SetMovieText", SetMovieText, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("SetMovieTitle", SetMovieTitle, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("SetWeather", SetWeather, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("ReplaceTexture", ReplaceTexture, 3, PARAMS_EXACT, s);

		s->VM.AddFunc("DrawWorld", DrawWorld, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("GetTime", GetTime, 0, PARAMS_EXACT, s);
		s->VM.AddFunc("ClearScreen", ClearScreen, 0, PARAMS_EXACT, s);
		s->VM.AddFunc("UpdateScreen", UpdateScreen, 0, PARAMS_EXACT, s);
		s->VM.AddFunc("PlayMusic", PlayMusic, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("PauseMusic", PauseMusic, 0, PARAMS_EXACT, s);
		s->VM.AddFunc("DrawImageCentered", DrawImageCentered, 3, PARAMS_EXACT, s);
		s->VM.AddFunc("DrawImageFullscreen", DrawImageFullscreen, 3, PARAMS_EXACT, s);


		s->VM.AddFunc("SaveCamera", SaveCamera, 0, PARAMS_EXACT, s);
		s->VM.AddFunc("RestoreCamera", RestoreCamera, 0, PARAMS_EXACT, s);
		s->VM.AddFunc("SetCameraTarget", SetCameraTarget, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("SetCameraRotation", SetCameraRotation, 3, PARAMS_EXACT, s);
		s->VM.AddFunc("RotationDrift", RotationDrift, 3, PARAMS_EXACT, s);
		s->VM.AddFunc("ZoomDrift", ZoomDrift, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("SetCameraZoom", SetCameraZoom, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("OffsetDrift", OffsetDrift, 3, PARAMS_EXACT, s);
		s->VM.AddFunc("SetCameraOffset", SetCameraOffset, 3, PARAMS_EXACT, s);
		s->VM.AddFunc("SetCameraFOV", SetCameraFOV, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("SetCameraFOVDrift", SetCameraFOVDrift, 1, PARAMS_EXACT, s);


		s->VM.AddFunc("PauseGame", PauseGame, 1, PARAMS_EXACT, s);

		s->VM.AddFunc("SetScreenDarkness", SetScreenDarkness, 1, PARAMS_EXACT, s);

		s->VM.AddFunc("Loadmap", Loadmap, 2, PARAMS_EXACT, s);
		s->VM.AddFunc("Unloadmap", Unloadmap, 0, PARAMS_EXACT, s);
		s->VM.AddFunc("QuitGame", QuitGame, 0, PARAMS_EXACT, s);

		s->VM.AddFunc("ActivateEntityFunction", ActivateEntityFunction, 2, PARAMS_EXACT, s);
		s->VM.AddFunc("SetEntityRotation", SetEntityRotation, 4, PARAMS_EXACT, s);
		s->VM.AddFunc("SetEntityLocation", SetEntityLocation, 4, PARAMS_EXACT, s);
		s->VM.AddFunc("SetEntityHealth", SetEntityHealth, 2, PARAMS_EXACT, s);
		s->VM.AddFunc("GetEntityHealth", GetEntityHealth, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("SetEntityItem", SetEntityItem, 2, PARAMS_EXACT, s);
		s->VM.AddFunc("ChangeEntityState", ChangeEntityState, 2, PARAMS_EXACT, s);
		s->VM.AddFunc("GetPlayerDistance", GetPlayerDistance, 1, PARAMS_EXACT, s);
		s->VM.AddFunc("EnableEntityAI", EnableEntityAI, 2, PARAMS_EXACT, s );
	}

	static int _SHILSCRIPT_CALL ScriptAPI::WriteLine (State* state, int params, void * user)
	{
		//set var
		Value VarVal = state->Pop();
		CVar.WriteLine( (char*)VarVal.GetStr().c_str() );

		//return null
		Value EmptyValue;
		state->Push( EmptyValue );

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetVarS (State* state, int params, void * user)
	{
		//set var
		Value VarVal = state->Pop();
		Value VarName = state->Pop();

		CVar.SetVarS( VarName.GetStr(), VarVal.GetStr().c_str() );

		//return null
		Value EmptyValue;
		state->Push( EmptyValue );

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetVarF (State* state, int params, void * user)
	{
		//set var
		Value VarVal = state->Pop();
		Value VarName = state->Pop();

		CVar.SetVarF( VarName.GetStr(), VarVal.GetNum() );
		
		//return null
		Value EmptyValue;
		state->Push( EmptyValue );

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetVarI (State* state, int params, void * user)
	{
		//set var
		Value VarVal = state->Pop();
		Value VarName = state->Pop();

		CVar.SetVarI( VarName.GetStr(), VarVal.GetNum() );
		
		//return null
		Value EmptyValue;
		state->Push( EmptyValue );

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::GetVarI (State* state, int params, void * user)
	{
		//pop variable name off the stack
		Value VarName = state->Pop();

		//return the value
		Value ReturnValue;
		ReturnValue.SetNum ( CVar.GetVarI( VarName.GetStr()  ) );
		state->Push( ReturnValue );

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::GetVarF (State* state, int params, void * user)
	{
		//pop variable name off the stack
		Value VarName = state->Pop();

		//return the value
		Value ReturnValue;
		ReturnValue.SetNum ( CVar.GetVarF( VarName.GetStr()  ) );
		state->Push( ReturnValue );

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::GetVarS (State* state, int params, void * user)
	{
		//pop variable name off the stack
		Value VarName = state->Pop();

		//return the value
		Value ReturnValue;
		ReturnValue.SetStr ( CVar.GetVarS( VarName.GetStr()  ) );
		state->Push( ReturnValue );

		return 1;
	}



	static int _SHILSCRIPT_CALL ScriptAPI::MovieMode(State * state, int params, void * user)
	{
		//Set if we're in the movie or not
		Value BoolVal = state->Pop();

		Game->InMovie = (bool)BoolVal.GetNum();
		Game->MovieSkipFlag = false;
		
		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetWeather(State * state, int params, void * user)
	{
		//Set if we're in the movie or not
		Value WeatherFile = state->Pop();

		if( Game->GetWorldManager() )
			Game->GetWorldManager()->WorldWeather.ChangeState( Game->Screen, Game->GetWorldManager()->LocalPlayer.GetLocation(), (char*)WeatherFile.GetStr().c_str());
		
		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetMovieText(State * state, int params, void * user)
	{
		//Set if we're in the movie or not
		Value MovieText = state->Pop();

		Game->BottomMovieText = MovieText.GetStr();

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetMovieTitle(State * state, int params, void * user)
	{
		//Set if we're in the movie or not
		Value MovieText = state->Pop();

		Game->TopMovieText = MovieText.GetStr();

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}




	static int _SHILSCRIPT_CALL ScriptAPI::ReplaceTexture(State * state, int params, void * user)
	{
		//Set if we're in the movie or not
		Value NewAlphaName = state->Pop();
		Value NewTextureName = state->Pop();
		Value TextureName = state->Pop();
		TextureHandle ht = Game->Screen.GetTextureHandle( (char*)TextureName.GetStr().c_str() ) ;

		if( Game->Screen.TM.TextureList[ht])
			delete Game->Screen.TM.TextureList[ht];

		
		Game->Screen.TM.TextureList[ht] = new Texture();

		if(NewAlphaName.IsNumerical())
			Game->Screen.TM.TextureList[ht]->LoadTexture( &Game->Screen, (char*)NewTextureName.GetStr().c_str() );
		else
			Game->Screen.TM.TextureList[ht]->LoadTexture( &Game->Screen, (char*)NewTextureName.GetStr().c_str(), (char*)NewAlphaName.GetStr().c_str() );


		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}


	static int _SHILSCRIPT_CALL ScriptAPI::DrawWorld(State * state, int params, void * user)
	{
		//set if the game can draw the world
		Value BoolVal  = state->Pop();
		Game->DrawWorld = (bool)BoolVal.GetNum();

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::GetTime(State * state, int params, void * user)
	{
		//return the current time in millseconds
		Value ReturnValue;
		ReturnValue.SetNum( timeGetTime() );
		state->Push( ReturnValue);

	
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SkipMovie(State * state, int params, void * user)
	{

		//tell the script if the user wants to skip the movie.
		Value ReturnValue;
		ReturnValue.SetNum( Game->MovieSkipFlag);
		state->Push( ReturnValue);

		Game->MovieSkipFlag = false;
	
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::GetPlayerDistance(State * state, int params, void * user)
	{
		Value TargetName  = state->Pop();

		//Get the handle of the target
		SceneHandle s = Game->GetWorldManager()->GetScriptObjectHandle( (char*)TargetName.GetStr().c_str() );
		if(s < 0)
		{
			//return null
			Value EmptyValue;
			EmptyValue.SetNum( -1 );
			state->Push(EmptyValue);

			return 1;
		}

		//Get the pointer to the target
		SceneObject* sp = Game->GetWorldManager()->GetDynamicObject( s );

		//return the distance
		if(sp)
		{
			Value ReturnValue;
			ReturnValue.SetNum( Game->GetWorldManager()->GetPlayerDistance( sp->GetLocation() ) );
			state->Push( ReturnValue);
		}

		else
		{
			Value EmptyValue;
			EmptyValue.SetNum( -1 );
			state->Push(EmptyValue);
		}


		
	
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::ClearScreen(State * state, int params, void * user)
	{
		//clear the screen, and also tell it to begin the scene
		Game->Screen.R = 0;
		Game->Screen.G = 0;
		Game->Screen.B = 0;
		Game->Screen.Clear();
		Game->Screen.Start3D();

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::DrawImageCentered(State * state, int params, void * user)
	{
		//end the scene then update the screen
		Value AlphaVal   = state->Pop();
		Value AlphaName  = state->Pop();
		Value ImageName  = state->Pop();
		
		//get the texture
		TextureHandle t = Game->Screen.GetTextureHandle( (char*)ImageName.GetStr().c_str(),NULL );
		Texture* texture = Game->Screen.TM.GetTexture( t);

		float a = (float)AlphaVal.GetNum();
		if(a < 0)
			a=0;
		if(a > 255)
			a=255;
		
		if(texture )
		{
			Game->Screen.PostOverlay( 
				(Game->Screen.GetWidth()/2)  - (texture->GetWidth()/2),
				(Game->Screen.GetHeight()/2)  - (texture->GetHeight()/2),
				t, 
				(a/255.0f) );
		}
		
		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}


	static int _SHILSCRIPT_CALL ScriptAPI::DrawImageFullscreen(State * state, int params, void * user)
	{
		//end the scene then update the screen
		Value AlphaVal   = state->Pop();
		Value AlphaName  = state->Pop();
		Value ImageName  = state->Pop();
		
		//get the texture
		TextureHandle t = Game->Screen.GetTextureHandle( (char*)ImageName.GetStr().c_str(),NULL );
		Texture* texture = Game->Screen.TM.GetTexture( t);

		int a = (int)AlphaVal.GetNum();
		if(a < 0)
			a=0;
		if(a > 255)
			a=255;
			
		//if the texture exists, draw it
		if(texture)
		{
			RECT ImageRect;
			ImageRect.left   = 0;
			ImageRect.top    = 0;
			ImageRect.right  = Game->Screen.GetWidth();
			ImageRect.bottom = Game->Screen.GetHeight();

			Game->Screen.SetTextureMode(TMNormal);
			Game->Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
			Game->Screen.SetRenderState(D3DRS_FOGENABLE, FALSE );
			Game->Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			Game->Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			Game->Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
			Game->Screen.D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			Game->Screen.D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			Game->Screen.D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			Game->Screen.DrawSpriteRect( ImageRect, t, true, D3DCOLOR_RGBA( 255, 255, 255, (int)a));
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::UpdateScreen(State * state, int params, void * user)
	{
		//end the scene then update the screen
		Game->Screen.End3D();
		Game->Screen.UpdateScreen();

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::PlayMusic(State * state, int params, void * user)
	{
		//end the scene then update the screen
		Value SongName  = state->Pop();
		Mp3.LoadFile( (char*)SongName.GetStr().c_str() );
		Mp3.Run();

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::PauseMusic(State * state, int params, void * user)
	{
		Mp3.Pause();		

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SaveCamera(State * state, int params, void * user)
	{
		if( Game->GetWorldManager() )
			Game->GetWorldManager()->SavedCamera = Game->GetWorldManager()->Viewer;

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::RestoreCamera(State * state, int params, void * user)
	{
		if( Game->GetWorldManager() )
			Game->GetWorldManager()->Viewer = Game->GetWorldManager()->SavedCamera ;

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::PauseGame (State* state, int params, void * user)
	{
		//pop opacity off the stack
		Value darkness = state->Pop();

		if( darkness.GetNum())
			Game->GamePaused = true;
		else
			Game->GamePaused = false;

		//return the value
		state->Push( Value() );

		return 1;
	}


	static int _SHILSCRIPT_CALL ScriptAPI::SetScreenDarkness (State* state, int params, void * user)
	{
		//pop opacity off the stack
		Value darkness = state->Pop();

		Game->ScreenDarkLevel = darkness.GetNum()/255.0f;

		//return the value
		state->Push( Value() );

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::Loadmap (State* state, int params, void * user)
	{
		//pop opacity off the stack
		Value mapname = state->Pop();
		Value modname = state->Pop();

		Game->LoadMap( modname.GetStr().c_str(), mapname.GetStr().c_str() );

		//return the value
		state->Push( Value() );

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::Unloadmap (State* state, int params, void * user)
	{
		//pop opacity off the stack
		Game->Unload = true;
		
		//return the value
		state->Push( Value() );

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::QuitGame (State* state, int params, void * user)
	{
		//pop opacity off the stack
		PostQuitMessage(0);

		//return the value
		state->Push( Value() );

		return 1;
	}


	static int _SHILSCRIPT_CALL ScriptAPI::SetCameraTarget(State * state, int params, void * user)
	{
		Value TargetName  = state->Pop();

		if(! Game->GetWorldManager() )
		{
			state->Push( Value() );
			return 1; 
		}
		if(stricmp((char*)TargetName.GetStr().c_str(), "player") == 0)
		{
			Game->GetWorldManager()->Viewer.Follow(Game->GetWorldManager()->LocalPlayer.GetLocation());
			Game->GetWorldManager()->Viewer.CS = CS_FREE;
		}

		//Get the handle of the target
		SceneHandle s = Game->GetWorldManager()->GetScriptObjectHandle( (char*)TargetName.GetStr().c_str() );
		if(s < 0)
		{
			//return null
			Value EmptyValue;
			state->Push(EmptyValue);

			return 1;
		}

		//Get the pointer to the target
		SceneObject* sp = Game->GetWorldManager()->GetDynamicObject( s );

		///Set the camera on the target
		if(sp)
		{
			Game->GetWorldManager()->Viewer.Follow( sp->GetLocation() );
			Game->GetWorldManager()->Viewer.CS = CS_FREE;
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::RotationDrift(State * state, int params, void * user)
	{
		//Get the various drift values
		Value RollDrift   = state->Pop();
		Value PitchDrift  = state->Pop();
		Value YawDrift    = state->Pop();

		//drift the camera	
		if( Game->GetWorldManager() )
		{
			Game->GetWorldManager()->Viewer.CS = CS_FREE;
			Game->GetWorldManager()->Viewer.YawDrift = RADIAN(YawDrift.GetNum());
			Game->GetWorldManager()->Viewer.PitchDrift = RADIAN(PitchDrift.GetNum());
			Game->GetWorldManager()->Viewer.RollDrift = RADIAN(RollDrift.GetNum());
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetCameraRotation(State * state, int params, void * user)
	{
		//Get the various drift values
		Value Roll   = state->Pop();
		Value Pitch  = state->Pop();
		Value Yaw    = state->Pop();

		//drift the camera	
		if( Game->GetWorldManager() ) 
		{
			Game->GetWorldManager()->Viewer.CS = CS_FREE;
			Game->GetWorldManager()->Viewer.Yaw = RADIAN(Yaw.GetNum());
			Game->GetWorldManager()->Viewer.Pitch = RADIAN(Pitch.GetNum());
			Game->GetWorldManager()->Viewer.Roll = RADIAN(Roll.GetNum());
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetCameraZoom(State * state, int params, void * user)
	{
		//Get the various drift values
		Value Zoom   = state->Pop();
		
		//drift the camera	
		if( Game->GetWorldManager() )
		{
			Game->GetWorldManager()->Viewer.CS = CS_FREE;
			Game->GetWorldManager()->Viewer.Zoom = Zoom.GetNum();
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::ZoomDrift(State * state, int params, void * user)
	{
		//Get the various drift values
		Value ZoomDrift   = state->Pop();
		
		//drift the camera	
		if( Game->GetWorldManager() )
		{
			Game->GetWorldManager()->Viewer.CS = CS_FREE;
			Game->GetWorldManager()->Viewer.ZoomDrift = ZoomDrift.GetNum();
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetCameraOffset(State * state, int params, void * user)
	{
		//Get the various drift values
		Value Z = state->Pop();
		Value Y = state->Pop();
		Value X = state->Pop();

		//drift the camera	
		if( Game->GetWorldManager() )
		{
			Game->GetWorldManager()->Viewer.CS = CS_FREE;
			Game->GetWorldManager()->Viewer.OffsetX = (X.GetNum());
			Game->GetWorldManager()->Viewer.OffsetY = (Y.GetNum());
			Game->GetWorldManager()->Viewer.OffsetZ = (Z.GetNum());
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		return 1;
	}
	static int _SHILSCRIPT_CALL ScriptAPI::OffsetDrift(State * state, int params, void * user)
	{
		//Get the various drift values
		Value ZDrift = state->Pop();
		Value YDrift = state->Pop();
		Value XDrift = state->Pop();

		//drift the camera	
		if( Game->GetWorldManager() )
		{
			Game->GetWorldManager()->Viewer.CS = CS_FREE;
			Game->GetWorldManager()->Viewer.XDrift = (XDrift.GetNum());
			Game->GetWorldManager()->Viewer.YDrift = (YDrift.GetNum());
			Game->GetWorldManager()->Viewer.ZDrift = (ZDrift.GetNum());
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetCameraFOVDrift(State * state, int params, void * user)
	{
		//Get the various drift values
		Value FOV   = state->Pop();
		
		//drift the camera	
		if( Game->GetWorldManager() )
		{
			Game->GetWorldManager()->Viewer.CS = CS_FREE;
			Game->GetWorldManager()->Viewer.FOVDrift = FOV.GetNum();
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		
		return 1;
	}
	static int _SHILSCRIPT_CALL ScriptAPI::SetCameraFOV(State * state, int params, void * user)
	{
		//Get the various drift values
		Value FOV   = state->Pop();
		
		//drift the camera	
		if( Game->GetWorldManager() )
		{
			Game->GetWorldManager()->Viewer.CS = CS_FREE;
			Game->GetWorldManager()->Viewer.FOV = FOV.GetNum();
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::ActivateEntityFunction(State * state, int params, void * user)
	{
		Value FunctionNumber  = state->Pop();
		Value TargetName  = state->Pop();

		//Get the handle of the target
		SceneHandle s = Game->GetWorldManager()->GetScriptObjectHandle( (char*)TargetName.GetStr().c_str() );
		if(s < 0)
		{
			//return null
			Value EmptyValue;
			state->Push(EmptyValue);
			return 1;
		}

		//Get the pointer to the target
		SceneObject* sp = Game->GetWorldManager()->GetDynamicObject( s );

		///Activate the target's function
		if(sp)
		{
			sp->Use( (int)FunctionNumber.GetNum(), 0, 0, NULL, Game->FrameTime );
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}


	static int _SHILSCRIPT_CALL ScriptAPI::SetEntityRotation(State * state, int params, void * user)
	{
		//Get the various drift values
		Value Roll  = state->Pop();
		Value Pitch = state->Pop();
		Value Yaw   = state->Pop();
		Value TargetName  = state->Pop();

		//Get the handle of the target
		SceneHandle s = Game->GetWorldManager()->GetScriptObjectHandle( (char*)TargetName.GetStr().c_str() );
		if(s < 0)
		{
			//return null
			Value EmptyValue;
			state->Push(EmptyValue);
			return 1;
		}

		//Get the pointer to the target
		SceneObject* sp = Game->GetWorldManager()->GetDynamicObject( s );

		///Set the target's orientation
		if(sp)
		{
			sp->SetYaw  ( RADIAN((float)Yaw.GetNum()));
			sp->SetPitch( RADIAN((float)Pitch.GetNum()));
			sp->SetRoll ( RADIAN((float)Roll.GetNum()));
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetEntityLocation(State * state, int params, void * user)
	{
		//Get the various drift values
		Value Z = state->Pop();
		Value Y = state->Pop();
		Value X = state->Pop();
		Value TargetName  = state->Pop();

		//Get the handle of the target
		SceneHandle s = Game->GetWorldManager()->GetScriptObjectHandle( (char*)TargetName.GetStr().c_str() );
		if(s < 0)
		{
			//return null
			Value EmptyValue;
			state->Push(EmptyValue);
			return 1;
		}

		//Get the pointer to the target
		SceneObject* sp = Game->GetWorldManager()->GetDynamicObject( s );

		///Set the target's orientation
		if(sp)
		{
			sp->SetX ( RADIAN(X.GetNum()));
			sp->SetY ( RADIAN(Y.GetNum()));
			sp->SetZ ( RADIAN(Z.GetNum()));
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetEntityHealth(State * state, int params, void * user)
	{
		Value Health  = state->Pop();
		Value TargetName  = state->Pop();

		//Get the handle of the target
		SceneHandle s = Game->GetWorldManager()->GetScriptObjectHandle( (char*)TargetName.GetStr().c_str() );
		if(s < 0)
		{
			//return null
			Value EmptyValue;
			state->Push(EmptyValue);
			return 1;
		}

		//Get the pointer to the target
		SceneObject* sp = Game->GetWorldManager()->GetDynamicObject( s );

		///Activate the target's function
		if(sp)
		{
			sp->SetHealth(Health.GetNum());
		}

		//return null
		Value EmptyValue;
		state->Push(EmptyValue);
		return 1;
	}
	
	static int _SHILSCRIPT_CALL ScriptAPI::GetEntityHealth(State * state, int params, void * user)
	{
		Value TargetName  = state->Pop();
		Value EmptyValue;
		//Get the handle of the target
		SceneHandle s = Game->GetWorldManager()->GetScriptObjectHandle( (char*)TargetName.GetStr().c_str() );
		if(s < 0)
		{
			//return null
			
			state->Push(EmptyValue);
			return 1;
		}

		//Get the pointer to the target
		SceneObject* sp = Game->GetWorldManager()->GetDynamicObject( s );

		///Activate the target's function
		if(sp)
		{
			//return null
			Value HealthValue;
			HealthValue.SetNum(sp->GetHealth());
			state->Push(HealthValue);
			
		}
		else
		{
			//return null
			state->Push(EmptyValue);
		}

		
		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::SetEntityItem(State * state, int params, void * user)
	{
		Value Item  = state->Pop();
		Value TargetName  = state->Pop();

		//Get the handle of the target
		SceneHandle s = Game->GetWorldManager()->GetScriptObjectHandle( (char*)TargetName.GetStr().c_str() );
		if(s < 0)
		{
			//return null
			Value EmptyValue;
			state->Push(EmptyValue);
			return 1;
		}

		//Get the pointer to the target
		SceneObject* sp = Game->GetWorldManager()->GetDynamicObject( s );

		///Activate the target's function
		if(sp)
		{
			sp->EquipItem( (int)Item.GetNum());
		}
		
		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	static int _SHILSCRIPT_CALL ScriptAPI::ChangeEntityState(State * state, int params, void * user)
	{
		Value newState  = state->Pop();
		Value TargetName  = state->Pop();

		//Get the handle of the target
		SceneHandle s = Game->GetWorldManager()->GetScriptObjectHandle( (char*)TargetName.GetStr().c_str() );
		if(s < 0)
		{
			//return null
			Value EmptyValue;
			state->Push(EmptyValue);
			return 1;
		}

		//Get the pointer to the target
		SceneObject* sp = Game->GetWorldManager()->GetDynamicObject( s );

		///Activate the target's function
		if(sp)
		{
			sp->ChangeState( newState.GetStr().c_str());
		}
		
		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}

	
	static int _SHILSCRIPT_CALL ScriptAPI::EnableEntityAI(State * state, int params, void * user)
	{
		Value BoolVal  = state->Pop();
		Value TargetName  = state->Pop();

		//Get the handle of the target
		SceneHandle s = Game->GetWorldManager()->GetScriptObjectHandle( (char*)TargetName.GetStr().c_str() );
		if(s < 0)
		{
			//return null
			Value EmptyValue;
			state->Push(EmptyValue);
			return 1;
		}

		//Get the pointer to the target
		SceneObject* sp = Game->GetWorldManager()->GetDynamicObject( s );

		///Activate the target's function
		if(sp)
		{
			if(BoolVal.GetNum() )
				sp->EnableThinking( true );
			else
				sp->EnableThinking( false );
		}
		
		//return null
		Value EmptyValue;
		state->Push(EmptyValue);

		return 1;
	}


};

