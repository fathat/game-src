#include "weapon.h"
#include "..\..\Game\systemobjects.h"
#include "..\ini\ini.h"
#include "..\defs\defs.h"


Weapon::Weapon() { 
	State = WS_IDLE; 
	ShowAmmoAmount  = true; 
	AimUsingRotation = true; 
	HitPlayer = true;  
	Bullets.StreamHeading.x=0;Bullets.StreamHeading.y=0;Bullets.StreamHeading.z=0; 
	DualWeapons = false;
	OddShot = false;
	InstantBullets = false;
	ObjectType = OT_WEAPON;
	MuzzleFlashFade = 0.0f;
}

void Weapon::Init( Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* WeaponFile )
{
	//Declarations
	char Buffer[128];
	char Buffer2[128];
    
	
	//see if it's a dual weapon
	DualWeapons = GetIniValue(WeaponFile, "weapon", "dual_weapons");
	OddShot = false;
	
	//Load the animated mesh
	GetIniValue(WeaponFile, "weapon", "animation_file", Buffer);
	MeshA.Init( Screen, MM, Buffer );

	if(DualWeapons)
		MeshB.Init( Screen, MM, Buffer );


	GetIniValue( WeaponFile, "weapon", "muzzle_flash", Buffer );
	MuzzleFlash = Screen.GetTextureHandle( Buffer ) ;

	//load the fire sound
	GetIniValue(WeaponFile, "weapon", "fire_sound", Buffer);
	//FiringSound = AM.GetSound(Buffer);
	SndFiring = DeviceManager::OpenALSound.GetSoundBuffer( Buffer ); 

	//load the reload sound
	GetIniValue(WeaponFile, "weapon", "reload_sound", Buffer);
	//ReloadSound = AM.GetSound(Buffer);
	SndReload = DeviceManager::OpenALSound.GetSoundBuffer( Buffer ); 

	GetIniValue(WeaponFile, "weapon", "aimer", Buffer);
	GetIniValue(WeaponFile, "weapon", "aimer_a", Buffer2);

	//load in the aimer for the gun
	Aimer = Screen.GetTextureHandle(Buffer, Buffer2);
	
	//Load object's icon
	GetIniValue(WeaponFile, "graphics", "icon", Buffer);
	GetIniValue(WeaponFile, "graphics", "icon_a", Buffer2);
	Icon = Screen.GetTextureHandle( Buffer, Buffer2 );

	//load in the gloss map for the gun
	GetIniValue(WeaponFile, "weapon", "glossmap", Buffer);
	GlossMap = Screen.GetTextureHandle(Buffer);



	//Store some pointers and such
	ScreenPtr = &Screen;
	WMPtr = WM;
	AMPtr = &AM;
	MMPtr = &MM;

	//Load in the weapon attributes
	MaxAmmo     = GetIniValue(WeaponFile, "weapon", "max_ammo");
	MaxClipAmmo = GetIniValue(WeaponFile, "weapon", "max_clip_ammo");
	RateOfFire  = GetIniValue(WeaponFile, "weapon", "rate_of_fire");
	ReloadTime  = GetIniValue(WeaponFile, "weapon", "reload_time");
	ProjectileSpeed = GetIniValue(WeaponFile, "weapon", "projectile_speed");
	Ammo        = GetIniValue(WeaponFile, "weapon", "ammo");
	RightHanded = (bool)GetIniValue(WeaponFile, "weapon", "right_handed");
	ClipAmmo    = MaxClipAmmo;
	Collectible = GetIniValue(WeaponFile, "properties", "collectible");
	InstantBullets = GetIniValue(WeaponFile, "weapon", "instant_bullets");
	DamageOnHit    = GetIniValueFloat( WeaponFile, "weapon", "damage" );
	State = WS_IDLE;

	if( DamageOnHit )
	{
		GetIniValue( WeaponFile, "weapon", "explosion_particles", Buffer ) ;
		Explosives.Init( *ScreenPtr, Buffer, WMPtr );
	}

	//Load in the smoke particles
	Smoke.Init(*ScreenPtr, "..\\base\\art\\objects\\weapons\\smoke.ini", WM);


	//Load in the particles for bullets and something something 
	GetIniValue(WeaponFile, "weapon", "particles", Buffer);
	if( strcmpi(Buffer, "nostring") != 0)
	{
		Bullets.Init(Screen, Buffer, WMPtr);
	}


}

void Weapon::SetLocation( Position& NewLocation ) 
{ 	
	//Take a location and then offset the weapon a bit forward
	//of that. Yeah, its sort of inaccurate, but it seems to work.
	Vector3D Offset;
	Vector3D Offset2;

	//O____________O!11
	if(RightHanded)
	{
		Offset.z = 0.0f;
		Offset.Rotatey( -NewLocation.Yaw - RADIAN(20));
		Offset2.z =0.0f;
		Offset2.Rotatey( -NewLocation.Yaw + RADIAN(20));
	}
	else
	{
		Offset.z = 0.0f;
		Offset.Rotatey( -NewLocation.Yaw + RADIAN(20));
		Offset2.z = 0.0f;
		Offset2.Rotatey( -NewLocation.Yaw - RADIAN(20));
	}

	//OHTEHNOS!1
	Location = NewLocation ; 
	Location.x += Offset.x;
	Location.z += Offset.z;
	SecondaryWeaponLocation = NewLocation;
	SecondaryWeaponLocation.x += Offset2.x;
	SecondaryWeaponLocation.z += Offset2.z;

}

void Weapon::SetFireVector( Vector3D& Aim)
{
	//Point the bullet particles somewhere. Also
	//make sure that it ignores the rotation values
	//if we're using this.
	AimUsingRotation = false;
	Aim.Normalize();
	Bullets.StreamHeading.x = Aim.x*ProjectileSpeed;
	Bullets.StreamHeading.y = Aim.y*ProjectileSpeed;
	Bullets.StreamHeading.z = Aim.z*ProjectileSpeed;	

}


void Weapon::Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime)
{
 

	//Reload the weapon if it's not already
	//in the proccess of reloading
	if( function == WF_RELOAD )
	{
		if(State != WS_RELOADING)
		{
			MeshA.Change( "reloading", .06f );
			MeshB.Change( "reloading", .06f );
			State = WS_RELOADING;
			TimeLeft = ReloadTime;
			//AMPtr->Play( ReloadSound );
			DeviceManager::OpenALSound.PlaySoundImmediate( SndReload, Location.AsSoundSource() );
		}
	}


	//Fire the weapon, if it's not already discharging a bullet
	//or reloading.
	if( function == WF_FIRE1 )
	{	
		if(State != WS_FIRING && State != WS_RELOADING && (Ammo > 0 || ClipAmmo > 0) )
		{

			//Set it to the "firing" state and play the sound
			State = WS_FIRING;
			TimeLeft = RateOfFire;
			//AMPtr->Play( FiringSound );
			DeviceManager::OpenALSound.PlaySoundImmediate( SndFiring, Location.AsSoundSource() );

			if(!OddShot || !DualWeapons)
				MeshA.Change( "shooting", .020f );
			else
				MeshB.Change( "shooting", .020f );
			

			if(DualWeapons)
			{
				if(OddShot)
					OddShot = false;
				else
					OddShot = true;
			}

			
		
			//Spawn some hud smoke and create the particle
			//which represents the bullet.
			if(!InstantBullets )
			{
				Smoke.SpawnParticles(10, false);
				Bullets.SpawnParticles(1, false);
			}
			else
			{
				//do something here
				Vector3D arrow  = Bullets.StreamHeading;
				arrow.SetLength( 2000 );
				MuzzleFlashFade = 1.0f;

				Vector3D hitPoint;
				Vector3D hitNormal;
				SceneObject* objectHit;

				Vector3D a = Location.GetVector();
				Vector3D b = Location.GetVector() + arrow;

				bool c = WMPtr->CheckBulletCollision( a, b, &hitPoint, &hitNormal, &objectHit, &WMPtr->LocalPlayer );
				if( c )
				{
					if(objectHit ) 
					{
						arrow.SetLength( DamageOnHit );
						objectHit->Damage( a, arrow, hitPoint );
					}
					else
					{
						Explosives.MoveSpawn( hitPoint.x, hitPoint.y, hitPoint.z );
						Explosives.SpawnParticles( 15, false );
					}
				}
			}
		
			

			//Decrease the ammount of ammo in the clip, then
			//reload if we're out of ammo
			ClipAmmo --;
			if(ClipAmmo < 1 && Ammo > 0)
			{
				ClipAmmo = 0;
				Use(WF_RELOAD, NULL, NULL, NULL, FrameTime);
			}
		}
	}
	
}



void Weapon::Update( Real Frametime, WorldManager* SM )
{
	//Update the particle streams
	Smoke.Update( Frametime );
	Explosives.Update(Frametime );

	MuzzleFlashFade -= 6*Frametime;
	if( MuzzleFlashFade < 0 )
		MuzzleFlashFade = 0;
	//Countdown on the current action, and when it's done
	//set the gun to sort of a "ready" state
	TimeLeft -= 1000*Frametime;
	if(TimeLeft < 0)
	{

		//If we just finished reloading, make sure we add
		//to the ammo count.
		if(State == WS_RELOADING)
		{
			if (Ammo > 0)
			{
				Ammo -= (MaxClipAmmo - ClipAmmo);
				ClipAmmo += (MaxClipAmmo - ClipAmmo);
			}
		}

		//Set it to ready
		TimeLeft=0;
		State = WS_IDLE;
	
	}



	//If we're aiming with an angle rather than
	//with a vector set that stuff up.
	if(AimUsingRotation)
	{
		Vector3D FireVector;
		FireVector.x = 0;
		FireVector.y = 0;
		FireVector.z = 1;
		FireVector.Rotatex( Location.Pitch );
		FireVector.Rotatey( -Location.Yaw );
		Bullets.StreamHeading.x = FireVector.x*ProjectileSpeed;
		Bullets.StreamHeading.y = FireVector.y*ProjectileSpeed;
		Bullets.StreamHeading.z = FireVector.z*ProjectileSpeed;	
	}
	


	//Update the bullet particles
	Bullets.UseHeading = true;
	Bullets.HitPlayer = HitPlayer;

	if(!OddShot)
		Bullets.MoveSpawn(Location.x, Location.y, Location.z);
	else
		Bullets.MoveSpawn(SecondaryWeaponLocation.x, SecondaryWeaponLocation.y, SecondaryWeaponLocation.z);

	Bullets.Update(Frametime);

	MeshA.Update( Frametime);
	MeshB.Update( Frametime);


}


bool Weapon::CheckVisibility( Screen3D& Screen, bool RecordTime )
{

	/*if(RecordTime)
	{
		if( LastDrawn == WMPtr->GTPtr->GetFrameStart() )
			return false;

		LastDrawn = WMPtr->GTPtr->GetFrameStart();
	}*/

	if(MeshA.CheckVisibility( Screen, Location  ))
		return true;
	if(MeshB.CheckVisibility( Screen, Location  ))
		return true;

	if(Bullets.CheckVisibility( Screen ) )
		return true;


	return false;

}




void Weapon::AdjustBoundingVolume()
{
	//readjust bounding box
	Box.min.x = Location.x + MeshA.GetBox().min.x;
	Box.min.y = Location.y + MeshA.GetBox().min.y ;
	Box.min.z = Location.z + MeshA.GetBox().min.z;
	Box.max.x = Location.x + MeshA.GetBox().max.x;
	Box.max.y = Location.y + MeshA.GetBox().max.y;
	Box.max.z = Location.z + MeshA.GetBox().max.z;

	Sphere.location.x = Location.x;
	Sphere.location.y = Location.y+ MeshA.GetBox().max.y/2;
	Sphere.location.z = Location.z;

	Sphere.Radius = MeshA.GetSphereRadius()+4 ;

	if(Bullets.Box.min.x < Box.min.x )
		Box.min.x = Bullets.Box.min.x;
	if(Bullets.Box.min.y < Box.min.y )
		Box.min.y = Bullets.Box.min.y;
	if(Bullets.Box.min.z < Box.min.z )
		Box.min.z = Bullets.Box.min.z;
	if(Bullets.Box.max.x > Box.max.x )
		Box.max.x = Bullets.Box.max.x;
	if(Bullets.Box.max.y > Box.max.y )
		Box.max.y = Bullets.Box.max.y;
	if(Bullets.Box.max.z > Box.max.z )
		Box.max.z = Bullets.Box.max.z;
		
	if(Sphere.Radius < Bullets.FarthestPoint )
		Sphere.Radius = Bullets.FarthestPoint;


	LightSource.x = Location.x;
	LightSource.y = Location.y;
	LightSource.z = Location.z;

}





void Weapon::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{
	if(Parent == NULL)
	{
		MeshA.Draw( Screen, Location.GetMatrix() );

		if(DualWeapons)
			MeshB.Draw( Screen, SecondaryWeaponLocation.GetMatrix());
	}
	WMPtr->AddStream( &Bullets );
	if(InstantBullets)
		WMPtr->AddStream( &Explosives );
}

void Weapon::DrawHUD( Screen3D& Screen, MeshManager& MM, FontManager& FM, Camera& Viewer )
{

	HitPlayer = false;

	Screen.Clear(true);
	// Set the matrices to the origin
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    Screen.SetTransform(  matWorld );

//Set render states
					ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
					ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
					ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
					ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
					ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
					ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
					ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
					ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	//Draw the weapon model
	WeaponFont = FM.GetFont("Lucida Console", 16);
	Screen.D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	Matrix ViewMatrix = Viewer.GetLookAtMatrix(  -10 );
	Matrix WorldMatrix;
	Matrix DisplacementMat;
	Matrix DisplacementMat2;
	Matrix ScaleMatrix;


	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	Screen.SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_ONE);
	Screen.SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_ONE);
	Screen.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE );
	Screen.SetRenderState(D3DRS_ZENABLE, FALSE );

	RECT muzzleDim = { Screen.GetWidth()/2, Screen.GetHeight()/2-Screen.GetHeight()/5,Screen.GetWidth()/2+(Screen.GetHeight()/5)*2, Screen.GetHeight()/2+Screen.GetHeight()/5};

	int fade = (int)(MuzzleFlashFade*255.0f);
	Screen.DrawSpriteRect( muzzleDim, MuzzleFlash, true, D3DCOLOR_RGBA( fade, fade, fade, fade ) );
	Screen.SetRenderState(D3DRS_ZENABLE, TRUE );

	Screen.SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
	//Screen.SetTextureMode( TMSphereMap);
	//Screen.SetTexture(GlossMap, 1);


	
	Matrix RMat;
	RMat.Rotate( Viewer.Yaw, Viewer.Pitch, Viewer.Roll);
	ScreenPtr->D3DDevice->MultiplyTransform( D3DTS_TEXTURE1, &RMat.Mat );
		
	//draw
	if(!RightHanded)
	{
		//First, apply a -1 scale to the transform
		//matrix so that the gun draws on the opposite side
		//(since it's left handed). Also, change the 
		//culling order so it doesn't draw inside out.
		Screen.SetRenderState( D3DRS_CULLMODE, D3DCULL_CW);

		DisplacementMat.Translate( 0.10f, 3.1f, -4.3f);
		ScaleMatrix.Scale( -1, 1, 1 );
		D3DXMatrixMultiply(&DisplacementMat.Mat, &DisplacementMat.Mat, &ScaleMatrix.Mat);
		D3DXMatrixMultiply(&ViewMatrix.Mat, &ViewMatrix.Mat, &DisplacementMat.Mat);
		D3DXMatrixInverse(&WorldMatrix.Mat, NULL, &ViewMatrix.Mat);
		Screen.SetTransform(WorldMatrix);


		//Draw the mesh now and return the culling state
		//DisplayMesh.Draw( Screen );
		Screen.SetRenderState(D3DRS_LIGHTING, FALSE );
		MeshA.Draw( Screen, WorldMatrix );
		Screen.SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
		D3DMATRIX OldState;
		Screen.D3DDevice->GetTransform( D3DTS_VIEW, &OldState);
		
		Matrix I;
		Screen.SetViewTransform(I);

		
				
		
		Smoke.MoveSpawn( -1.0f, -1.2f, 9.0f);
		Smoke.Draw( Screen, Viewer );
		
		if(DualWeapons)
		{
			Smoke.MoveSpawn( 1.0f, -1.1f, 8.0f);
			Smoke.Draw( Screen, Viewer );
		}
		
		Screen.D3DDevice->SetTransform( D3DTS_VIEW, &OldState);
       
	}
	else
	{
		DisplacementMat.Translate( -0.10f, 3.1f, -4.3f);
		D3DXMatrixMultiply(&ViewMatrix.Mat, &ViewMatrix.Mat, &DisplacementMat.Mat);
		D3DXMatrixInverse(&WorldMatrix.Mat, NULL, &ViewMatrix.Mat);
		Screen.SetTransform(WorldMatrix);

		//Draw the weapon
		Screen.D3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		MeshA.Draw( Screen, WorldMatrix );
		Screen.D3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		Matrix I;
		Screen.SetViewTransform(I);
				

		Smoke.MoveSpawn( 1.0f, -1.1f, 8.0f);
		Smoke.Draw( Screen, Viewer );

		if(DualWeapons)
		{
			Smoke.MoveSpawn( -1.0f, -1.1f, 8.0f);
			Smoke.Draw( Screen, Viewer );
		}

	}

	//Show the ammo information by the weapon
	Screen.D3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	Screen.SetRenderState(D3DRS_ZENABLE, FALSE );
	if(ShowAmmoAmount)
	{

		
		TextureHandle hAmmoBox = Screen.GetTextureHandle( "..\\base\\art\\UI\\icons\\ammobox.bmp", "..\\base\\art\\UI\\icons\\ammobox_a.bmp");
		Screen.DrawSprite(  Screen.GetWidth() - 145.0f, (Real)Screen.GetHeight() - 40.0f, Screen.TM.GetTexture( hAmmoBox ));
		WeaponFont->Draw(Screen, Screen.GetWidth() - 105.0f, (Real)Screen.GetHeight() - 32.0f, false, D3DCOLOR_RGBA(255, 255, 255, 255), "%d", ClipAmmo);
		WeaponFont->Draw(Screen, Screen.GetWidth() - 60.0f,  (Real)Screen.GetHeight() - 32.0f, false, D3DCOLOR_RGBA(255, 0, 0, 255), "%d", Ammo);
	}

		
	Screen.TM.SetTextureMode( TMNormal, 0, true);

	//draw the aimer
	if(ShowAmmoAmount)
	{
		Texture* AimerPtr = Screen.TM.GetTexture( Aimer );
		

		Screen.SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
		Screen.SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
		Screen.SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
		Screen.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE );

		RECT Dimensions;
		Dimensions.left   = Screen.GetWidth() /2 - (AimerPtr->GetWidth()/2);
		Dimensions.top   = Screen.GetHeight()/2 - (AimerPtr->GetHeight()/2);
		Dimensions.right  = Screen.GetWidth() /2 + AimerPtr->GetWidth()/2-1;
		Dimensions.bottom = Screen.GetHeight()/2 + AimerPtr->GetHeight()/2-1;
	
	
		if(AimerPtr)
			Screen.DrawSprite(Screen.GetWidth()/2 - (AimerPtr->GetWidth()/2), Screen.GetHeight()/2 - (AimerPtr->GetHeight()/2), AimerPtr);


	

		
	}

	ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	Screen.SetRenderState(D3DRS_ZENABLE, TRUE );
	Screen.SetRenderState( D3DRS_FOGENABLE,  TRUE );

}