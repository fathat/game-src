#include "avatar.h"
#include "weapon.h"
#include "avatarconstruction.h"
#include "Powerup.h"
#include "SceneManager.h"
#include "..\defs\defs.h"


Avatar::Avatar()
{
	EquipedItem = 0;

	InventoryTime = 0;
	Health = 1000;
	Swimming = false;

	TargetVelocity.x = 0;
	TargetVelocity.y = 0;
	TargetVelocity.z = 0;
	Location.x = 0;
	Location.y = 0;
	Location.z = 0;

	this->Acceleration = 10;
	this->MaxSpeed = 10;

	ViewBob = 0;
	MaxViewBob = .6f;
	ViewBobDirection  = 1;
	ViewBobSpeed = 2;

	Money = 0;
	MoneyLimit = 1000;
	WMPtr	 = NULL;

	RespawnTime = 0;
//	ScreenPtr = NULL;
	

	State = AV_IDLE;

}

Avatar::~Avatar()
{

}


//////// Init /////////////////////////////////
//
// Description:
//
// Initializes data to acceptable starting
// values.
//
///////////////////////////////////////////////

void Avatar::Init( Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile )
{
	//load in the animated mesh
	char TempBuffer[255];
	GetIniValue(ObjectFile, "graphics", "animation_file", TempBuffer);
	MeshA.Init( Screen, MM, TempBuffer );


	Money = 0;
	

	//Store some pointery things
	WMPtr = WM;
	MMPtr = &MM;
	
	
	Construction* pAvatarConstruction = new AvatarConstruction( MaxSpeed );
	pAvatarConstruction->Construct ( ObjectFile, &WM->Solver, Screen, MM, Location, this );
    hConstruction = WM->Solver.RegisterConstruction( pAvatarConstruction );

	//Load in the character's primary weapon
	GetIniValue(ObjectFile, "weapon", "weapon", TempBuffer);
	if( strcmpi(TempBuffer, "nostring") != 0)
	{

		//Create the weapon
		Weapon* DefaultWeapon = new Weapon;
		DefaultWeapon->Init( Screen, WM, MM, AM, TempBuffer );

		//Add it to the world
		//WMPtr->Add( DefaultWeapon );

		//also add it locally
		this->AddEntity(DefaultWeapon );
	}

	//load in the side arm
	GetIniValue(ObjectFile, "weapon", "sidearm", TempBuffer);
	if( strcmpi(TempBuffer, "nostring") != 0)
	{
		Weapon* SideWeapon = new Weapon;
		Weapon* SecondSideWeapon = new Weapon;
		SideWeapon->Init( Screen, WM, MM, AM, TempBuffer );
		SecondSideWeapon->Init( Screen, WM, MM, AM, TempBuffer );
		

		//WMPtr->Add( SideWeapon );
		this->AddEntity(SideWeapon );
		//WMPtr->Add( SecondSideWeapon );
		//this->AddEntity(SecondSideWeapon );
		SecondSideWeapon->RightHanded = false;
	}

	//load in the third weapon
	GetIniValue(ObjectFile, "weapon", "third", TempBuffer);
	if( strcmpi(TempBuffer, "nostring") != 0)
	{
		Weapon* SideWeapon = new Weapon();
		SideWeapon->Init( Screen, WM, MM, AM, TempBuffer );

		//WMPtr->Add( SideWeapon );

		this->AddEntity(SideWeapon );
	}

	//load in the fourth weapon
	GetIniValue(ObjectFile, "weapon", "fourth", TempBuffer);
	if( strcmpi(TempBuffer, "nostring") != 0)
	{
		Weapon* SideWeapon = new Weapon();
		SideWeapon->Init( Screen, WM, MM, AM, TempBuffer );

		//WMPtr->Add( SideWeapon );

		this->AddEntity(SideWeapon );
	}

	//load in the fifth weapon
	GetIniValue(ObjectFile, "weapon", "fifth", TempBuffer);
	if( strcmpi(TempBuffer, "nostring") != 0)
	{
		Weapon* SideWeapon = new Weapon();
		SideWeapon->Init( Screen, WM, MM, AM, TempBuffer );

		//WMPtr->Add( SideWeapon );

		this->AddEntity(SideWeapon );
	}
	SecondaryEquipedItem = -1;


}

void Avatar::AddEntity(SceneObject* ent )
{
	Powerup* p = dynamic_cast< Powerup* > (ent);

	if( p )
	{
		if( p->StatToChange() == ST_HEALTH )
			Health += p->ChangeAmount();
		if( p->StatToChange() == ST_MONEY )
			Money += p->ChangeAmount();


		WMPtr->sceneManager->RemoveObjectByPtr ( p );
		delete p;
	}
	else
	{
		AnimatedObject::AddEntity( ent );
	}
		
}

void Avatar::OnCollide( ICollisionHandler* collisionHandler )
{
	Construction* pConstruct = WMPtr->Solver.GetConstruction( hConstruction );
	pConstruct->Activate();

	((AvatarConstruction*)pConstruct)->SetOnGround( true );
}

void Avatar::HandleInput( Real FrameTime, POINT MouseDelta, UCHAR* KeyBuffer )
{
	//declarations
	Real SideDelta=0;
	Real ForwardDelta=0;
	Real SpeedTemp = MaxSpeed;//*Slope;


	SetYaw( WMPtr->Viewer.Yaw );
	SetPitch( WMPtr->Viewer.Pitch);

		//Weapon switching
	if(( KeyBuffer[DIK_1] & 0x80) || HIWORD(GetKeyState('1')) )
	{
		EquipItem(0);
		InventoryTime = 2.0f;
	}
	else if(( KeyBuffer[DIK_2] & 0x80) || HIWORD(GetKeyState('2')) )
	{
		EquipItem(1);
		InventoryTime = 2.0f;
	}
	else if(( KeyBuffer[DIK_3] & 0x80) || HIWORD(GetKeyState('3')))
	{
		EquipItem(2);
		InventoryTime = 2.0f;
	}
	else if(( KeyBuffer[DIK_4] & 0x80) || HIWORD(GetKeyState('4')) )
	{
		EquipItem(3);
		InventoryTime = 2.0f;
	}
	else if(( KeyBuffer[DIK_5] & 0x80) || HIWORD(GetKeyState('5')) )
	{
		EquipItem(4);
		InventoryTime = 2.0f;
	}
	else if(( KeyBuffer[DIK_6] & 0x80) || HIWORD(GetKeyState('6')) )
	{
		EquipItem(5);
		InventoryTime = 2.0f;
	}
	

	//Forward, back, left and right
	if( ( KeyBuffer[DIK_S] & 0x80)  || HIWORD(GetKeyState('S')))
		ForwardDelta -= SpeedTemp;		
	if(( KeyBuffer[DIK_W] & 0x80)  || HIWORD(GetKeyState('W')))
		ForwardDelta += SpeedTemp;
	if(( KeyBuffer[DIK_D] & 0x80)  || HIWORD(GetKeyState('D')))
		SideDelta += SpeedTemp;
	if(( KeyBuffer[DIK_A] & 0x80)  || HIWORD(GetKeyState('A')))
		SideDelta -= SpeedTemp;


	
	//Weapon firing
	if( HIWORD( GetKeyState(VK_LBUTTON ) ))  //InputSystem.LeftButtonDown() ||
	{
		Use(AU_USE, NULL, NULL, NULL, FrameTime);
	}
		


	//jump
	if(( KeyBuffer[DIK_SPACE] & 0x80)  || ( KeyBuffer[DIK_INSERT] & 0x80) || HIWORD(GetKeyState(VK_SPACE)) )
		Use(AU_JUMP, MaxSpeed, NULL, NULL, FrameTime );
	if(( KeyBuffer[DIK_C] & 0x80)  || HIWORD(GetKeyState('C')) )
		Use( AU_CROUCH, NULL, NULL, NULL, FrameTime  );

	//Reload
	if(( KeyBuffer[DIK_DELETE] & 0x80) || ( KeyBuffer[DIK_R] & 0x80) || HIWORD(GetKeyState('R')))
	{
		Use(AU_RELOAD, NULL, NULL, NULL, FrameTime);
	}

	MoveVector = Vector3D( SideDelta, 0, ForwardDelta);
	
	
}

void Avatar::Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime )
{
	if( function == AU_USE )
	{
		if(EquipmentList[EquipedItem])
			EquipmentList[EquipedItem]->Use( WF_FIRE1,  Param1, Param2, Param3, WMPtr->GTPtr->GetFrameTime()  );
	}
	else if( function == AU_RELOAD )
	{
		if(EquipmentList[EquipedItem])
			EquipmentList[EquipedItem]->Use( WF_RELOAD, Param1, Param2, Param3, WMPtr->GTPtr->GetFrameTime()   );
	}
	else if( function == AU_JUMP )
	{
		Construction* c = WMPtr->Solver.GetConstruction( hConstruction);
		Vector3D velocity;
		c->GetVelocity( velocity );

		//if( OnPlatform)
			c->Accelerate( 0, 0, 60 );
	}

	if( function == AU_USE2 )
	{
		if( SecondaryEquipedItem > -1)
			if(EquipmentList[SecondaryEquipedItem])
				EquipmentList[SecondaryEquipedItem]->Use( WF_FIRE1,  Param1, Param2, Param3, WMPtr->GTPtr->GetFrameTime()  );
	}


}

//////// CheckForWeapons //////////////////////
//
// Description:
//
// Checks to see if there are any weapons in
// the player's immediate area.
//
///////////////////////////////////////////////


void Avatar::CheckForWeapons( WorldManager* WM  )
{
    
}


//////// Damage ///////////////////////////////
//
// Description:
//
// Notifies player of a force acting upon them
//
///////////////////////////////////////////////

void Avatar::OnDamage ( const Vector3D& source, const Vector3D& collisionPoint, float damageAmount ) 
{
	Health -= damageAmount*2;

	if( Health <= 0 && RespawnTime <= 0.0f )
	{
		RespawnTime = 4.0f; 
		Money /= 2;
	}

}


//////// Move /////////////////////////////////
//
// Description:
//
// Adjusts vectors and moves players by them. 
//
///////////////////////////////////////////////

void Avatar::Move( Real FrameTime, WorldManager* SM )
{
	
	//declarations
	Real Slope;
	Vector3D SlopeNormal;
	D3DXPLANE TestPlane;
	Vector3D WaterFlow;

	InventoryTime -= FrameTime;
	if( InventoryTime < 0 )
		InventoryTime = 0;

	//get player location and velocity
	Construction* c = WMPtr->Solver.GetConstruction( hConstruction);
	Vector3D position;
	c->GetLocation ( position );
	Location.x = position.x;
	Location.y = position.y-(GetHeight()/2.0f);
	Location.z = position.z;


	Swimming = SM->InWater( Location, &WaterFlow);

	if(Swimming && SM->WaterDamage)
	{
		Health -= (Real)SM->WaterDamage*FrameTime;

		if( Health < 0)
		{
			Location = SpawnPoint;
			Health   = 1000;
		}
	}

	//Animate the something something
	AvatarConstruction* construct = (AvatarConstruction*)SM->Solver.GetConstruction( hConstruction );
	construct->GetVelocity( CurrentVelocity );
	Vector3D LatVector = CurrentVelocity;
	LatVector.y = 0;
	if(State != AV_RUNNING && LatVector.Mag() >= 1  && !StrafeLeft && !StrafeRight )
	{
		if(State == AV_STRAFELEFT || State == AV_STRAFERIGHT )
		{
			State = AV_RUNNING;
			MeshA.Change("run1", 0.0f);
		}
		else
		{
			State = AV_RUNNING;
			MeshA.Change("run1", 1.0f);
		}
		
	}
	if(State != AV_STRAFELEFT && LatVector.Mag() >= 1  && StrafeLeft)
	{
		State = AV_STRAFELEFT;
		MeshA.Change("sl0", 0.0f);
		
	}
	if(State != AV_STRAFERIGHT && LatVector.Mag() >= 1  && StrafeRight)
	{
		State = AV_STRAFERIGHT;
		MeshA.Change("sr0", 0.0f);
		
	}
	if(State != AV_IDLE && LatVector.Mag() < 0.5f )
	{
		State = AV_IDLE;
		MeshA.Change("idle1", 0.0f);
	}
	
	if( LatVector.Mag() >= 1 )
	{
		MeshA.SetTimeMultiplier( LatVector.Mag()  /10.0f );
	}
	else
	{
		MeshA.SetTimeMultiplier( 1.0f );
	}

	


}



bool Avatar::Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision )
{
	//Declarations
	Vector3D Line[2];
	Line[0] = point1;
	Line[1] = point2;
	
	//Check for the collision
	if( MeshA.CheckForCollision( Location, Line, Collision ) )
	{
		return true;
	}

	return false;
}

void Avatar::MoveByDelta ( Real ForwardDelta, Real SideDelta)
{
	if(Health <= 0)
	{
		EquipItem(9);
		return;
	}

	StrafeRight = false;
	StrafeLeft  = false;
	if(SideDelta > 0.0f) 
		StrafeRight = true;
	else if(SideDelta < 0.0f) 
		StrafeLeft = true;


	Vector3D moveVector;
	Construction* c = WMPtr->Solver.GetConstruction( hConstruction);

	if(ForwardDelta != 0)
	{
		moveVector.x += ( -ForwardDelta ) * (float)(*m_cos)(Location.Yaw + RADIAN(90));
		moveVector.z += ( ForwardDelta ) * (float)(*m_sin)(Location.Yaw + RADIAN(90));
	}

	if(SideDelta != 0)
	{
		moveVector.x += ( SideDelta ) * (float)(*m_cos)(Location.Yaw );
		moveVector.z += ( -SideDelta ) * (float)(*m_sin)(Location.Yaw );
	}

	if(ForwardDelta != 0 || SideDelta != 0 )
	{
		moveVector.Normalize();
		moveVector*=MaxSpeed;

		if(WMPtr->DilationPercent > 0.01)
		{
			moveVector*= 1.0+WMPtr->DilationPercent;
		}
	}
	c->Accelerate ( moveVector.z, moveVector.x, 0 );

}

void Avatar::SetPosition( Real x, Real y, Real z )
{	
	Construction* c = WMPtr->Solver.GetConstruction( hConstruction);
	c->SetLocation( x, y, z );
}

void Avatar::SetVelocity( Real x, Real y, Real z )
{
	Construction* c = WMPtr->Solver.GetConstruction( hConstruction);
	c->SetVelocity( x, y, z );
}


Vector3D Avatar::GetVelocity()
{
	Vector3D returnVector;
	Construction* c = WMPtr->Solver.GetConstruction( hConstruction);
	c->GetVelocity( returnVector );
	return returnVector;
}



//////// Update ///////////////////////////////
//
// Description:
//
// Lets avatar do any neccesary tasks for this
// frame.
//
///////////////////////////////////////////////

void Avatar::Update ( Real FrameTime, WorldManager* SM )
{
	//Update the player's position
	MoveByDelta( MoveVector.z, MoveVector.x);
	Move( FrameTime, SM);
	AdjustBoundingVolume();
	MeshA.Update( FrameTime );

	//Update the weapons
	for( int i=0; i<EquipmentList.size(); i++)
	{
		if(EquipmentList[i])
		{
			Position ItemLocation = Location;
			ItemLocation.y += MeshA.GetBox().max.y*.85f;
			EquipmentList[i]->SetLocation(ItemLocation);
			EquipmentList[i]->Update( FrameTime, SM );
		}
	}
	RespawnTime -= FrameTime;
    if ( RespawnTime < 0)
		RespawnTime = 0;
	if( Health <= 0 && RespawnTime <= 0 )
	{
		Location = SpawnPoint;
		SetPosition( SpawnPoint.x, SpawnPoint.y, SpawnPoint.z  );
		Health = 1000.0f;
	}
}



void Avatar::DrawHUD( Screen3D& Screen, MeshManager& MM, FontManager& FM, Camera& Viewer )
{
	//If the player has a weapon that is equipped, draw it and it's stats
	if( !EquipmentList.empty() && EquipmentList[EquipedItem] && Viewer.CS == CS_FIRST_PERSON)
	{
		EquipmentList[EquipedItem]->DrawHUD(Screen, MM, FM, Viewer )  ;
	}
	

	if(SecondaryEquipedItem > -1)
	{
		if(!EquipmentList.empty() && EquipmentList[SecondaryEquipedItem])
		{
			EquipmentList[SecondaryEquipedItem]->DrawHUD(Screen, MM, FM, Viewer )  ;
		}
	}

	//Set to point filtering
	Screen.D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	Screen.D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	Screen.D3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	//Draw the player's health
	TextureHandle bg;
	Screen.SetRenderState(D3DRS_ZENABLE, FALSE );
	Screen.SetRenderState(D3DRS_FOGENABLE, FALSE);
	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	Screen.SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	Screen.SetRenderState(D3DRS_ALPHATESTENABLE,   FALSE);
	bg = Screen.GetTextureHandle( "..\\base\\art\\UI\\icons\\health.bmp",  "..\\base\\art\\UI\\icons\\health_a.bmp" );
	Screen.DrawSprite( 10, Screen.GetHeight()-40, Screen.TM.GetTexture( bg ) );
	Font2D* HudFont = FM.GetFont( "Arial", 14);
	HudFont->Draw(Screen, 36, Screen.GetHeight()-36, false, D3DCOLOR_XRGB( 255, 255, 255 ), "%d", (int)Health/10);


	//Draw the coinage
	TextureHandle coin = Screen.GetTextureHandle( "..\\base\\art\\UI\\coin.dds" );
	Screen.DrawSprite( 100, Screen.GetHeight()-40, Screen.TM.GetTexture( coin ) );
	HudFont->Draw(Screen, 100+36, Screen.GetHeight()-36, false, D3DCOLOR_XRGB( 255, 255, 255 ), "%d", (int)Money, (int)MoneyLimit);


	//Draw the player's inventory
	if( InventoryTime > 0 )
	{
		RECT Dim;
		for( int i=0; i<EquipmentList.size(); i++)
		{
			if(EquipmentList[i])
			{
			
				Dim.left   = i*32+250;
				Dim.top    = Screen.GetHeight() - 40;
				Dim.bottom = Dim.top + 32;
				Dim.right  = Dim.left + 32;
				if(i == EquipedItem)
				{
					TextureHandle s;
					s = Screen.GetTextureHandle( "..\\base\\art\\UI\\icons\\select.bmp",  "..\\base\\art\\UI\\icons\\select_a.bmp" );
					Screen.DrawSpriteRect( Dim, s, true );
				}
				else
				{
					TextureHandle s;
					s = Screen.GetTextureHandle( "..\\base\\art\\UI\\icons\\itembg.bmp",  "..\\base\\art\\UI\\icons\\itembg_a.bmp" );
					Screen.DrawSpriteRect( Dim, s, true );
				}
				
				Screen.DrawSpriteRect( Dim, EquipmentList[i]->GetIcon(), true );

				
			}
			
		}
	}

	//set back to linear filtering
	//Screen.D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//Screen.D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//Screen.D3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	Screen.SetLinearTextureFiltering(0);

	Screen.SetRenderState(D3DRS_ZENABLE, TRUE );

	
}



//////// Draw /////////////////////////////////
//
// Description:
//
// Draws the avatar, animations and such included.
//
///////////////////////////////////////////////

void Avatar::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{
	//set the transformation
	Position DrawLocation;
	DrawLocation = Location;
	DrawLocation.Pitch = 0;
	
	Viewer.HeightDisplacement = 0;//ViewBob;

    //if(WMPtr->InMovie == false )	
	//	Viewer.OffsetY = 10;

	if(this != &WMPtr->LocalPlayer && this->ID != WMPtr->LocalPlayer.ID )
		MeshA.Draw( Screen, DrawLocation.GetMatrix() );
	else if (  Viewer.CS != CS_FIRST_PERSON && Viewer.CS != CS_FOLLOW_MATRIX )
		MeshA.Draw( Screen, DrawLocation.GetMatrix() );



	
	AvatarConstruction* c = (AvatarConstruction*)WMPtr->Solver.GetConstruction( hConstruction );
	c->Draw( Screen);
	
	
	for(int i=0; i<EquipmentList.size(); i++)
	{
		if(EquipmentList[i])
			EquipmentList[i]->Draw( Screen, MM, Viewer, Timer );

	}
}
