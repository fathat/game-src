#include "Particle.h"
#include "..\s3dlib\types.h"
#include "..\ini\ini.h"
#include "world.h"
#include "SceneManager.h"
#include "..\Physics\DynamicsObject.h"

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

static void particleNearCallback (void *data, dGeomID o1, dGeomID o2)
{
	Particle_S* particle = (Particle_S*) data;
	dContact contact;
	int n = dCollide( o1, o2, 1,  &contact.geom, sizeof( dContact)  );

	if( n )
	{

		if( n > 1)
		{
			float r;
			r = 0;
		}
		//do collision stuff
		//contact.geom.depth
		//contact.geom.normal
		//contact.geom.pos
		float l = sqrt( particle->vx*particle->vx + particle->vy*particle->vy + particle->vz*particle->vz );
	
		particle->x = contact.geom.pos[0];
		particle->y = contact.geom.pos[1];
		particle->z = contact.geom.pos[2];
		
		Vector3D velocity = Vector3D(particle->vx, particle->vy, particle->vz);
		velocity.Normalize();
		Vector3D normal = Vector3D(contact.geom.normal[0], contact.geom.normal[1], contact.geom.normal[2]);
		Vector3D reflected = velocity.Reflect(normal);
		particle->vx = reflected.x*l*0.8;
		particle->vy = reflected.y*l*0.8;
		particle->vz = reflected.z*l*0.8;
		particle->NotifyCollision();

		dBodyID b1, b2;
		b1 = dGeomGetBody(contact.geom.g1);
		b2 = dGeomGetBody(contact.geom.g2);


		float mass = particle->streamPtr->ParticleMass ;
		if(b1)
		{
			//dBodyAddForce ( b1, 
			
			dBodyEnable( b1 );
			dBodyAddForceAtPos( b1, particle->vx *mass*.25f , particle->vy *mass*.25f, particle->vz *mass*.25f, particle->x, particle->y, particle->z );

			DynamicsObject* dynObj = (DynamicsObject*)dBodyGetData( b1 );
			if (dynObj )
			{
				if( dynObj->isAvatar )
				{
					dynObj->OnDamage( Vector3D( particle->x, particle->y, particle->z ), Vector3D( particle->x, particle->y, particle->z ), particle->mass );
				}
			}

		}

		if(b2)
		{
			dBodyEnable( b2 );
			dBodyAddForceAtPos( b2, particle->vx *mass*.25f , particle->vy *mass*.25f, particle->vz *mass*.25f, particle->x, particle->y, particle->z );
		
		}

			
		
	}

	//dContact contact[N];
	//n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
    
}

void Particle_S::NotifyCollision () 
{
	if(streamPtr->ExplosionStream )
	{
		streamPtr->ExplosionStream->MoveSpawn(x,y, z);
		streamPtr->ExplosionStream->SpawnParticles(streamPtr->ParticlesPerExplosion, false);
		active = false;
	}

	if ( life > streamPtr->DeathDelay )
		life = streamPtr->DeathDelay;
	if(streamPtr->DeathDelay <= 0 )
	{
		active = false;
	}
}

void Particle_S::Activate()
{
	active  = true;

}
void Particle_S::Deactivate()
{

	life = 0;
	active = false;
}

bool Particle_S::CheckForCollision( WorldManager* WMPtr, Real FrameTime, ParticleStream& Stream )
{
	//First check to see if it's gone off the edges of the map
	if(x < 0)
	{
		life = 0;
		HasForce = false;
		active = false;
	}
	else if(z < 0)
	{
		life = 0;
		active = false;
		HasForce = false;
	}
	else if(x > (WMPtr->GetWidth()-2)*WMPtr->GetSpacing())
	{
		life = 0;
		HasForce = false;
		active = false;
	}
	else if(z > (WMPtr->GetHeight()-2)*WMPtr->GetSpacing())
	{
		active = false;
		HasForce = false;
		life = 0;
	}

	if( geom  && active  && life)
	{
		dGeomEnable( geom );
		dSpaceCollide2( geom, (dxGeom*)WMPtr->Solver.GetActiveSpaceID() , this, &particleNearCallback);
		
		dSpaceCollide2( geom, (dxGeom*)WMPtr->Solver.GetSpaceID( false ) , this, &particleNearCallback);
		dSpaceCollide2( geom, (dxGeom*)WMPtr->Solver.GetSpaceID( true ) , this, &particleNearCallback);

		Vector3D outPosition, outNormal ;
		SceneObject* object;
		Vector3D a( x, y, z ), b( x+vx*FrameTime, y+vy*FrameTime, z+vz*FrameTime); 

		SceneObject* objectToIgnore=NULL;
		if( Stream.HitPlayer)
			objectToIgnore = &(WMPtr->LocalPlayer);

		if( WMPtr->sceneManager->BasicCheckForCollision( a, b, outPosition, outNormal, &object, objectToIgnore )  )
		{
			
			x = outPosition.x;
			y = outPosition.y;
			z = outPosition.z;

			vx = outNormal.x;
			vy = outNormal.y;
			vz = outNormal.z;
			object->Damage( a, (b-a).Normalize() * mass , outPosition );
			

			if(streamPtr->ExplosionStream )
			{
				streamPtr->ExplosionStream->MoveSpawn(x,y, z);
				streamPtr->ExplosionStream->SpawnParticles(streamPtr->ParticlesPerExplosion, false);
				active = false;
			}

			if ( life > streamPtr->DeathDelay )
				life = streamPtr->DeathDelay;
			if(streamPtr->DeathDelay <= 0 )
			{
				active = false;
			}
            
		}

		

		dGeomDisable( geom );
	}
	


	return false;

	/*if(life <= 0 || !active )
	{
		return false;
	}


	//First check to see if it's gone off the edges of the map
	if(x < 0)
	{
		life = 0;
		HasForce = false;
		active = false;
	}
	else if(z < 0)
	{
		life = 0;
		active = false;
		HasForce = false;
	}
	else if(x > (WMPtr->GetWidth()-2)*WMPtr->GetSpacing())
	{
		life = 0;
		HasForce = false;
		active = false;
	}
	else if(z > (WMPtr->GetHeight()-2)*WMPtr->GetSpacing())
	{
		active = false;
		HasForce = false;
		life = 0;
	}
	else
	{

			
		//Still in the map, so set up vectors and such to 
		///use for collision testing
		Vector3D CollisionNormal, ParticleVector, VectorLocation;
		
		Position particle_location;
		particle_location.x = x;
		particle_location.y = y;
		particle_location.z = z;	
        
		VectorLocation.x = x;
		VectorLocation.y = y;
		VectorLocation.z = z;	

		ParticleVector.x = vx;
		ParticleVector.y = vy;
		ParticleVector.z = vz;

		//check for collisions with objects
		if(HasForce && Stream.CollisionType == COL_LANDSCAPE_AND_OBJECTS )
		{
			Vector3D Old;
			Old.x = ox;
			Old.y = oy;
			Old.z = oz;
	
			Vector3D v = VectorLocation - (VectorLocation-Old);
					
			CollisionInfo C;
			C.Mass = mass ;
			bool collision=false;

			if( WMPtr->CheckForCollision( v, VectorLocation, &C, Stream.HitPlayer)  )
				collision = true;
			
			if(collision)
			{
			
			    life = Stream.DeathDelay;
				if(Stream.DeathDelay <= 0 )
				{
					active = 0;
				}
				Real Mag = ParticleVector.Mag();
				ParticleVector = ParticleVector.Reflect( C.PlaneNormal);

				ParticleVector.Normalize();
				ParticleVector *= Mag;

				x = C.CollisionPoint.x;
				y = C.CollisionPoint.y;
				z = C.CollisionPoint.z;
				
			
				if(Stream.Reflect)
				{
					vx = ParticleVector.x/4;
					vy = ParticleVector.y/4;
					vz = ParticleVector.z/4;
				}
				else
				{
					vx = 0;
					vy = 0;
					vz = 0;
				}

				if(collidex == startx && collidey == starty && collidez == startz )
				{
					collidex = x;
					collidey = y;
					collidez = z;
				}

				HasForce = false;

				return true;
			}
		}	
			
		//check for an intersection with the landscape
		Real HeightValue = WMPtr->GetHeightValue( particle_location, NULL, &CollisionNormal);

		if(y < HeightValue)
		{
			Real Mag = ParticleVector.Mag();
			ParticleVector = ParticleVector.Reflect( CollisionNormal);

			ParticleVector.Normalize();
			ParticleVector *= Mag;
			
		
			//if(!Stream.Reflect)
			//{
			//	ParticleVector *= 0.0f;
			//}
			vx = ParticleVector.x;
			vy = ParticleVector.y;
			vz = ParticleVector.z;
			

			y  = HeightValue;

			HasForce = false;


			if(Mag > 50 && !Stream.NoDust)
			{
				int DustParticles = Mag*4*FrameTime+1;
				WMPtr->KickupDust( x, z, DustParticles );
			}
            			
			if(life > Stream.DeathDelay)
				life = Stream.DeathDelay;
			if (life <= 0)
				active = false;


			if( collidex == startx && collidey == starty && collidez == startz )
			{
				collidex = ox;
				collidey = oy;
				collidez = oz;
			}


			return true;	
		}
	} 
	return false;*/

}

void Particle_S::Update( ParticleStream& Stream, Real FrameTime)
{
	streamPtr = &Stream;
	if(life <= 0 && active )
	{

		//reinitialize particle here
		fade = Stream.ParticleLife ;
		life = fade;
		mass = Stream.ParticleMass;

		if( geom == NULL && Stream.CollisionType != COL_NONE )
			geom = dCreateRay( Stream.localSpace , 1.0f  );

		//if there's a plane, create the particles in it
		if(Stream.Width >1 || Stream.Depth > 1)
		{
			x = Stream.x+((rand() % (int)(Stream.Width*2))-Stream.Width);
			y = Stream.y;
			z = Stream.z+((rand() % (int)(Stream.Depth*2))-Stream.Depth);
		}
		//Otherwise create them at the origin
		else
		{
			x = Stream.x;
			y = Stream.y;
			z = Stream.z;
		}

        //Set up some of the default variables
		ox = x;
		oy = y;
		oz = z;
		startx = x;
		starty = y;
		startz = z;
		collidex = startx;
		collidey = starty;
		collidez = startz;
		xg = Stream.xg;
		yg = Stream.yg;
		zg = Stream.zg;
		r  = (Real)Stream.r;
		g  = (Real)Stream.g;
		b  = (Real)Stream.b;
		HasForce = true;

		//we want the randimization to be accurate to the tenths place, hence
		//the *10 /10 stuff
		Real px = (Real)(rand() % ((int)(Stream.Power * 10)))/10.0f; 
		Real py = (Real)(rand() % ((int)(Stream.Power * 10)))/10.0f;
		Real pz = (Real)(rand() % ((int)(Stream.Power * 10)))/10.0f;
	

		if(Stream.UseHeading)
		{
			vx = Stream.StreamHeading.x+(Real)((Real)( rand() % (int)100)/200.0f);;
			vy = Stream.StreamHeading.y+(Real)((Real)( rand() % (int)100)/200.0f);;
			vz = Stream.StreamHeading.z+(Real)((Real)( rand() % (int)100)/200.0f);;
		}
		else
		{
			vx = px - (Stream.Power/2);
			vy = py; 
			vz = pz - (Stream.Power/2);
		}
					
	}

	//move particle
	ox = x;
	oy = y;
	oz = z;
	x += vx * FrameTime;
	y += vy * FrameTime;
	z += vz * FrameTime;

	//Update the velocity with the gravity
	vx += xg * FrameTime;
	vy += yg * FrameTime;
	vz += zg * FrameTime;

	//If there's a constant distance then make sure the particles adhere
	if(Stream.ParticleConstantDistance > 0 )
	{
		Vector3D StreamDistance;
		StreamDistance.x = x - Stream.x;
		StreamDistance.y = y - Stream.y;
		StreamDistance.z = z - Stream.z;
		
		StreamDistance.Normalize();
		StreamDistance *= Stream.ParticleConstantDistance;

		x = Stream.x + StreamDistance.x;
		y = Stream.y + StreamDistance.y;
		z = Stream.z + StreamDistance.z;
	}

	//Update the geom info
	if( geom )
	{
		dGeomRaySet( geom, x, y, z, vx, vy, vz );
		float length = (float)sqrt ( vx*vx + vy*vy + vz*vz ); 
		dGeomRaySetLength( geom, length*FrameTime );
	}
	

	//If the new particle location stretches the box, then extend it
	if(x < Stream.Box.min.x )
		Stream.Box.min.x = x;
	if(y < Stream.Box.min.y )
		Stream.Box.min.y = y;
	if(z < Stream.Box.min.z )
		Stream.Box.min.z = z;
	if(x > Stream.Box.max.x )
		Stream.Box.max.x = x;
	if(y > Stream.Box.max.y )
		Stream.Box.max.y = y;
	if(z > Stream.Box.max.z )
		Stream.Box.max.z = z;
	
	
	life-=1000.0f * FrameTime;

	if(life < 0 && active)
	{
		Stream.ActiveParticles--;
		active = false;
		if(Stream.ExplosionStream )
		{
			Stream.ExplosionStream->MoveSpawn(x, y, z);
			Stream.ExplosionStream->SpawnParticles(Stream.ParticlesPerExplosion, false);
			
		}
	}


}

bool ParticleStream::Init(Screen3D& Screen, char* ParticleFile, WorldManager* WM)
{	
	// declarations
	char TempBuffer[256], TempBuffer2[256];

	// clean up any old stuff if we're reinitializing!
	Shutdown();

	//create the local collision space
	localSpace = dSimpleSpaceCreate ( NULL );

	ActiveParticles=0; ParticlesPerSecond=0; 

	// create an array for all the particles
	MaxParticles = GetIniValueFloat(ParticleFile,"graphics","particles", TempBuffer);
	if(MaxParticles)
		Particle = new Particle_S[MaxParticles];
	else
		return false;

	ParticleBuffer.Init( Screen, D3DFVF_PARTICLEVERTEX, sizeof(ParticleVertex), MaxParticles*3, true, true, D3DPT_TRIANGLELIST );
	


	//Load in the trail stream! (if there is one)
	GetIniValue(ParticleFile,"graphics","trail_stream",TempBuffer);

	//make sure it's defined and not "nostring"
	if(strcmpi( TempBuffer, "nostring") )
	{
		//..load it up
		TrailStream = new ParticleStream;
		TrailStream->Init( Screen, TempBuffer, WM);
	}
	else
	{
		//OHNOS!! No stream, set it to null so it doesn't
		//mess up the app later.
		TrailStream = NULL;
	}

	TrailsPerParticle = GetIniValueFloat(ParticleFile,"graphics","trails_per_particle", TempBuffer);
	
	//Load in the explosion stream! (if there is one)
	GetIniValue(ParticleFile,"graphics","explosion_stream",TempBuffer);

	//make sure it's defined and not "nostring"
	if(strcmpi( TempBuffer, "nostring") )
	{
		//..load it up
		ExplosionStream = new ParticleStream;
		ExplosionStream->Init( Screen, TempBuffer, WM);
	}
	else
	{
		//OHNOS!! No stream, set it to null so it doesn't
		//mess up the app later.
		ExplosionStream = NULL;
	}

	ParticlesPerExplosion = GetIniValueFloat(ParticleFile,"graphics","particles_per_explosion", TempBuffer);

	//Load in the 3dmodel name
	GetIniValue(ParticleFile,"graphics","object",TempBuffer);
	
	//Check if there was an object named..
	if(strcmpi( TempBuffer, "nostring") == 0)
	{
		//... if there wasn't just load in the billboard texture
		GetIniValue(ParticleFile,"graphics","texture",TempBuffer);
		GetIniValue(ParticleFile,"graphics","alpha",TempBuffer2);
		StreamTexture = Screen.GetTextureHandle(TempBuffer, TempBuffer2); //set it to default texture
		ShowObject = false;
	}
	else
	{
		//... otherwise load in the 3dmodel
		ParticleObject = WM->MMPtr->GetMeshHandle( TempBuffer );
		ShowObject = true;

		//load in the gloss map for the object (if, of course, there is one)
		GetIniValue(ParticleFile, "graphics", "glossmap", TempBuffer);
		GlossMap = Screen.GetTextureHandle(TempBuffer);
	}
	
	
	WMPtr = WM;
	
	ParticlesPerSecond = GetIniValue(ParticleFile,"graphics","particle_spawn_rate");
	StreamLife = GetIniValueFloat(ParticleFile,"graphics","particle_stream_life", TempBuffer);
	InitialStreamLife = StreamLife;
	NoDust = GetIniValue(ParticleFile,"graphics","particle_nodust");

	if(InitialStreamLife == 0)
		UseStreamTimer = false;
	else
		UseStreamTimer = true;

	x=0; y=0; z=0;
	ParticleFraction = 0;
	
	xg	  = GetIniValueFloat(ParticleFile,"graphics","particle_gravityx", TempBuffer);
	yg	  = GetIniValueFloat(ParticleFile,"graphics","particle_gravityy", TempBuffer);
	zg    = GetIniValueFloat(ParticleFile,"graphics","particle_gravityz", TempBuffer);
	
	Width = GetIniValueFloat(ParticleFile,"graphics","particle_stream_plane_width", TempBuffer);
	Depth = GetIniValueFloat(ParticleFile,"graphics","particle_stream_plane_depth", TempBuffer);
	Width++;
	Depth++;

	Reflect           = (bool)GetIniValue(ParticleFile,"graphics","particle_no_reflect");
	Reflect = !Reflect;
	SpawnContinuously = (bool)GetIniValue(ParticleFile,"graphics","particle_stream_forever");
	Power = GetIniValueFloat(ParticleFile,"graphics","particle_speed", TempBuffer);
	ParticleLife = GetIniValueFloat(ParticleFile,"graphics","particle_life", TempBuffer);
	ParticleConstantDistance = GetIniValueFloat(ParticleFile,"graphics","particle_constant_distance", TempBuffer);
	DeathDelay = GetIniValueFloat(ParticleFile,"graphics","particle_death_delay_on_hit", TempBuffer);
	
	ParticleSize = GetIniValueFloat(ParticleFile,"graphics","particle_size",TempBuffer2);
	ParticleMass = GetIniValueFloat(ParticleFile,"graphics","particle_mass",TempBuffer2 );
	
	r = (int)GetIniValue(ParticleFile,"graphics","color_red");
	g = (int)GetIniValue(ParticleFile,"graphics","color_green");
	b = (int)GetIniValue(ParticleFile,"graphics","color_blue");


	BlendMode        = (ParticleBlendMode)GetIniValue(ParticleFile,"graphics","blend_mode");
	
	CollisionType     = (ParticleCollisionType)GetIniValue(ParticleFile,"graphics","particle_collisions");
	LinearFade        = (bool)GetIniValue(ParticleFile,"graphics","particle_lineardecay");
	UseHeading        = false;
	HitPlayer         = true;
	ShowTrail         = (bool)GetIniValue(ParticleFile,"graphics","particle_lines");
	bool UsePlane     = false;


	return true;
}



void ParticleStream::SpawnParticles(Real nParticles, bool SpawnEverySecond)
{

	//Declarations
	bool Done=false;
	int i=0;

	//Set the neccesary stream variables
	SpawnContinuously = SpawnEverySecond;

	
	//Add "leftovers" from last time (partial particles)
	if(!SpawnContinuously)
	{
		nParticles += (int)ParticleFraction;
		ParticleFraction -= (int)ParticleFraction;
	}
	
	
	//Loop through and activate the needed number of particles
	while(!Done)
	{
		
		//Test to see if we've activated all the particles we need to
		if (nParticles < 1)
		{
			//Record the fraction of particles that we didn't spawn,
			//but only if spawn continuously isn't on (that will do it
			//for us)
			if(!SpawnContinuously)
				ParticleFraction += nParticles;

			//Finish up
			Done = true;
		}

		//Make sure we don't overrun the amount of particles
		if(i >= MaxParticles-1)
			Done = true;

		//Check if the particle in question is not in use, and
		//if its not then activate it.
		if(Particle != NULL && Particle[i].active == false && !Done)
		{
			Particle[i].active   = true;
			Particle[i].life     = -1; //-1 so that the update function knows to respawn it
			Particle[i].fade     =(Real)( rand() % (int)ParticleLife) + ParticleLife/3;
			Particle[i].HasForce = true;
			Particle[i].Update( *this, .1f );
			nParticles--;
			ActiveParticles++;
		}

		
		//Increment the index
		i++;
		
	}
	
}


void ParticleStream::Stop()
{
	SpawnContinuously=false;
}

//Moves particles to a new general location
void ParticleStream::TranslateParticles( Real nx, Real ny, Real nz )
{
	//loop through every particle
	for(int i=0; i < MaxParticles; i++)
	{
		//Before we move the particle, make sure that it hasn't
		//hit the ground (otherwise it'll look like its sliding along the ground)
		if(Particle[i].HasForce)
		{
			Particle[i].xg = (nx-x)*10;
			//Particle[i].gy += (ny-y);
			Particle[i].zg = (nz-z)*10;
		}
		
	}

    //Move the spawn.
	x = nx;
	y = ny;
	z = nz;


}

void ParticleStream::Update ( Real FrameTime)
{
	//Update the trailstream if we have one
	if(TrailStream)
		TrailStream->Update( FrameTime );

	//also update explosion stream
	if(ExplosionStream)
		ExplosionStream->Update( FrameTime );

	//Reset the box that represents the extents
	//(since it's going to be recalculated).
	FarthestPoint = 0;
	Box.min.x = x;
	Box.min.y = y;
	Box.min.z = z;
	Box.max.x = x;
	Box.max.y = y;
	Box.max.z = z;
	
	//Run down the stream life variable
	//if it's set
	if(UseStreamTimer)
	{
		StreamLife-=1000* FrameTime;
		if(StreamLife<=0)
		{
			Stop();
			UseStreamTimer = false;
		}
	}
	//Loop through all the particles and
	//run the neccesary calls on them. 
	int count = 0;
	for(int i=0; i < MaxParticles; i++)
	{
		if(count >= ActiveParticles )
			break;

		//Make sure this particle is alive before
		//we start doing stuff to it.
		if(Particle[i].active )
		{	
			
			//Check for collisions
			if(CollisionType != COL_NONE)
			{
				if(!Particle[i].CheckForCollision( WMPtr, FrameTime, *this ) )
					Particle[i].Update( *this, FrameTime );
				else if(ExplosionStream )
				{
					ExplosionStream->MoveSpawn(Particle[i].x, Particle[i].y, Particle[i].z);
					ExplosionStream->SpawnParticles(ParticlesPerExplosion, false);
					Particle[i].active = false;
				}
					
			}
			else
			{
				Particle[i].Update( *this, FrameTime );
			}

			//Update the particle
			count++;


			//Spawn some trailing particles behind the updated particle
			if(TrailStream)
			{
				TrailStream->MoveSpawn(Particle[i].x, Particle[i].y, Particle[i].z);
				TrailStream->SpawnParticles(TrailsPerParticle*FrameTime, false);
			}
		}
	}	
	
	//Spawn new particles if the SpawnContinuously flag
	//is set and there aren't too many particles/
	if(ActiveParticles < MaxParticles && SpawnContinuously)
	{
		ParticleFraction += ParticlesPerSecond*FrameTime;

		if((int)ParticleFraction)
		{
			SpawnParticles( (int)ParticleFraction, true );
			ParticleFraction -= (int)ParticleFraction;
		}
	}

}

bool ParticleStream::CheckVisibility( Screen3D& Screen)
{
	//Check the box for this stream against the camera's viewing
	//frustrum to see if its inside it at all. The box is calculated
	//every frame in the update function.
	if( Screen.CheckAABB(Box ) )
		return true;


	//ohnos! not visible at all!
	return false;
}

void ParticleStream::Draw(Screen3D& Screen, Camera& Viewer)
{
	if(TrailStream)
		TrailStream->Draw( Screen, Viewer);
	if(ExplosionStream)
		ExplosionStream->Draw( Screen, Viewer);


	if(!ActiveParticles)
		return;

	Screen.SetTextureMode(TMNormal);

	D3DXMATRIX matWorld, matView, InvView;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &InvView );
    Screen.D3DDevice->GetTransform( D3DTS_VIEW, &matView );

	InvView._11 = -matView._11; InvView._21 = matView._12; InvView._31 = -matView._13;
	InvView._12 = -matView._21; InvView._22 = matView._22; InvView._32 = -matView._23;
	InvView._13 = -matView._31; InvView._23 = matView._32; InvView._33 = -matView._33;

	InvView._14=0.0f;
	InvView._24=0.0f;
	InvView._34=0.0f;
	InvView._44=1.0f;


	// set render states

	
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	Screen.D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	Screen.D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	
	
	Screen.D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	Screen.D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_ONE);


	if(BlendMode == PARTICLEBLEND_ADDITIVE )
	{
		
		
		Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
		Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_ONE);
	}
	else
	{
		Screen.SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_REVSUBTRACT);
		//Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
		//Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	}
	Screen.D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	Screen.D3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	
	Screen.SetMaterial(NULL);
	Screen.SetTexture(StreamTexture, 0);

	Screen.SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);
	

    Mesh* ParticleMesh = WMPtr->MMPtr->Get( ParticleObject );
	if(ShowObject && ParticleMesh)
	{
		D3DMATERIAL9 SaveMat, ObjectMat;
		Screen.D3DDevice->GetMaterial( &SaveMat );
		Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
		Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
		Screen.D3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		
		ObjectMat = SaveMat;

		if(GlossMap != -1)
		{
			Screen.SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
			Screen.SetTextureMode( TMSphereMap);
			Screen.SetTexture(GlossMap, 1);
		}

		for(int i=0; i < MaxParticles; i++)
		{
			if( Particle[i].active == false )
				continue;
	
			
			D3DXVECTOR3 DirectionVector, Cross;
			D3DXVECTOR3 ObjPt (Particle[i].x, Particle[i].y, Particle[i].z);
			D3DXVECTOR3 ObjFuturePt (Particle[i].x+Particle[i].vx, Particle[i].y+Particle[i].vy, Particle[i].z+Particle[i].vz);
			D3DXVECTOR3 UpVec (0.0f,1.0f,0.0f);
			D3DXMATRIX matWorld, matInverse;

			D3DXVec3Normalize( &DirectionVector, &(UpVec - ObjPt) );
			D3DXVec3Cross( &Cross, &DirectionVector, &UpVec );
			D3DXMatrixLookAtLH( &matWorld, &ObjPt, &ObjFuturePt, &UpVec );

			Real Det;
			D3DXMatrixInverse( &matInverse, &Det, &matWorld );

			// Set up a material. 
			D3DMATERIAL9 mtrl;
			ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
			mtrl.Diffuse.r = mtrl.Specular.r = mtrl.Ambient.r = 1.0f;
			mtrl.Diffuse.g = mtrl.Specular.g = mtrl.Ambient.g = 1.0f;
			mtrl.Diffuse.b = mtrl.Specular.b = mtrl.Ambient.b = 1.0f;
			mtrl.Diffuse.a = mtrl.Specular.a = mtrl.Ambient.a = (Particle[i].life / Particle[i].fade);
			

			Screen.D3DDevice->SetMaterial( &mtrl );
			//Screen.D3DDevice->SetTransform(D3DTS_WORLDMATRIX(0), &matInverse);
			Screen.SetTransform( matInverse );
	        ParticleMesh->Draw( Screen );
		}

		Screen.D3DDevice->SetMaterial( &SaveMat );

	}

	else
	{
	
		//Assemble the basic vertex something or other
		ParticleVertex p[3];			
		

		//Lock the vertex buffer for the point sprites and fill it
		ParticleVertex* VertexList;
		if( ParticleBuffer.Lock( (BYTE**)&VertexList, D3DLOCK_DISCARD ) );
		{
			int count=0;
			
			p[0].tu = 0; p[0].tv = 0;
			p[1].tu = 1; p[1].tv = 0;
			p[2].tu = .5; p[2].tv = 1;

			D3DXVECTOR3 BasePosition[4];

			BasePosition[0].x = -(Real)ParticleSize;
			BasePosition[0].y = (Real)ParticleSize/2;
			BasePosition[0].z = 0;
			BasePosition[1].x = (Real)ParticleSize;
			BasePosition[1].y = (Real)ParticleSize/2;
			BasePosition[1].z = 0;
			BasePosition[2].x = -(Real)0;
			BasePosition[2].y = -(Real)ParticleSize*1.5;
			BasePosition[2].z = 0;
				
			//Loop through each particle
			for(int i=0; i < MaxParticles; i++)
			{
				//Don't draw the particles that are inactive (dead)
				if( Particle[i].active == false )
					continue;

				//Create the matrix to translate the particles by
				InvView._41 = Particle[i].x;
				InvView._42 = Particle[i].y;
				InvView._43 = Particle[i].z;
		
				//Translate all the points by our matrix
				D3DXVECTOR4 R1, R2, R3;
				D3DXVec3Transform(  &R1, &BasePosition[0], &InvView );
				D3DXVec3Transform(  &R2, &BasePosition[1], &InvView );
				D3DXVec3Transform(  &R3, &BasePosition[2], &InvView );

			
				p[0].position = (D3DXVECTOR3)R1;
				p[1].position = (D3DXVECTOR3)R2;
				p[2].position = (D3DXVECTOR3)R3;


				//Do some horrible stuff to figure out the alpha value
				Real alpha;
				if(LinearFade)
				{
					if(Particle[i].life < Particle[i].fade)
						alpha = (Particle[i].life / Particle[i].fade) ;
					else
						alpha = .5f;
					
					if(UseStreamTimer)
						alpha *= (StreamLife / InitialStreamLife);
				}
				else
				{
					if(Particle[i].life < Particle[i].fade / 10)
					{
						if(StreamLife)
							alpha = (Particle[i].life / (Particle[i].fade/10)) ;
						else
							alpha = (Particle[i].life / (Particle[i].fade/10)) ;

						if(UseStreamTimer)
							alpha *= (StreamLife / InitialStreamLife);
					}
					else
					{
						if(StreamLife)
						{
							alpha = 1 * (StreamLife / InitialStreamLife);
						}
						else
							alpha = 1;
					}

				}

				if(alpha < 0)
					alpha = 0;

				//Set the colors! ^_^
				p[0].color = D3DCOLOR_RGBA((int)((Real)Particle[i].r*alpha), (int)((Real)Particle[i].g*alpha), (int)((Real)Particle[i].b*alpha), (int)(alpha*255));
				p[1].color = D3DCOLOR_RGBA((int)((Real)Particle[i].r*alpha), (int)((Real)Particle[i].g*alpha), (int)((Real)Particle[i].b*alpha), (int)(alpha*255));
				p[2].color = D3DCOLOR_RGBA((int)((Real)Particle[i].r*alpha), (int)((Real)Particle[i].g*alpha), (int)((Real)Particle[i].b*alpha), (int)(alpha*255));
		

				//Now set the *actual* vertices to our something something
				VertexList[count+0] = p[0];
				VertexList[count+1] = p[1];
				VertexList[count+2] = p[2];
				count += 3;
			}
		

			//Unlock the buffer now that we're done writing to it
			ParticleBuffer.Unlock();

			//Draw the vertex buffer
			if(count)
			{
				Matrix Ident;
				Screen.SetTransform( Ident );
				Screen.SetFVF(D3DFVF_PARTICLEVERTEX);
				//Screen.D3DDevice->SetStreamSource(0, ParticleBuffer.VB, 0, sizeof(ParticleVertex ) );
				ParticleBuffer.SetActive( 0 );
				Screen.D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, count / 3);
				Screen.PolygonsRendered += count;
				Screen.DrawCalls++;
			}
		}
		
		
	}

	if(ShowTrail)
	{
		Matrix M;
		Screen.SetTransform(M);
		
		ParticleVertex p[2];
			
		p[0].tu = 0;
		p[0].tv = 0;
		p[1].tu = 1;
		p[1].tv = 1;

		Screen.SetFVF(D3DFVF_PARTICLEVERTEX);
		Screen.SetTexture( -1, 0 );
		Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
		Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
		Screen.D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
		Screen.SetRenderState(D3DRS_LIGHTING, FALSE);

		Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		
		Screen.D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);	
		Screen.D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		
	
		for(int i=0; i < MaxParticles; i++)
		{
			if( Particle[i].active == false )
				continue;

		
			//calculate amount of life left
			Real alpha;

			if(LinearFade)
			{
				if(Particle[i].life < Particle[i].fade)
					alpha = (Particle[i].life / Particle[i].fade) ;
				else
					alpha = .5f;

				if(UseStreamTimer)
					alpha *= (StreamLife / InitialStreamLife);
			}
			else
			{
				if(Particle[i].life < Particle[i].fade / 10)
				{
					if(StreamLife)
						alpha = (Particle[i].life / (Particle[i].fade/10)) * (StreamLife / InitialStreamLife);
					else
						alpha = (Particle[i].life / (Particle[i].fade/10)) ;

				}
				else
				{
					if(StreamLife)
						alpha = 1 * (StreamLife / InitialStreamLife);
					else
						alpha = 1;
				}

			}

			if(alpha < 0)
				alpha = 0;
			

			alpha *= .1f;

			//Not collided yet, so just draw one line
			if(Particle[i].collidex == Particle[i].startx && Particle[i].collidey == Particle[i].starty && Particle[i].collidez == Particle[i].startz)
			{
				
				p[0].position.x = Particle[i].startx;
				p[0].position.y = Particle[i].starty;
				p[0].position.z = Particle[i].startz;
				p[1].position.x = Particle[i].x;
				p[1].position.y = Particle[i].y;
				p[1].position.z = Particle[i].z;
				p[0].color = D3DCOLOR_RGBA((int)((Real)Particle[i].r), (int)((Real)Particle[i].g), (int)((Real)Particle[i].b), (int)(alpha*128));
				p[1].color = D3DCOLOR_RGBA((int)((Real)Particle[i].r), (int)((Real)Particle[i].g), (int)((Real)Particle[i].b), (int)(alpha*255));
	
			
				Screen.D3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, p, sizeof(ParticleVertex));

			}
			else if(Reflect)
			{
				
				p[0].position.x = Particle[i].startx;
				p[0].position.y = Particle[i].starty;
				p[0].position.z = Particle[i].startz;
				p[1].position.x = Particle[i].collidex;
				p[1].position.y = Particle[i].collidey;
				p[1].position.z = Particle[i].collidez;
				p[0].color = D3DCOLOR_RGBA((int)((Real)Particle[i].r), (int)((Real)Particle[i].g), (int)((Real)Particle[i].b), (int)(alpha*128));
				p[1].color = D3DCOLOR_RGBA((int)((Real)Particle[i].r), (int)((Real)Particle[i].g), (int)((Real)Particle[i].b), (int)(alpha*255));		
				Screen.D3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, p, sizeof(ParticleVertex));

				
				/*p[0].position.x = Particle[i].collidex;
				p[0].position.y = Particle[i].collidey;
				p[0].position.z = Particle[i].collidez;
				p[1].position.x = Particle[i].x;
				p[1].position.y = Particle[i].y;
				p[1].position.z = Particle[i].z;
				p[0].color = D3DCOLOR_RGBA((int)((Real)Particle[i].r), (int)((Real)Particle[i].g), (int)((Real)Particle[i].b), (int)(alpha*255));
				p[1].color = D3DCOLOR_RGBA((int)((Real)Particle[i].r), (int)((Real)Particle[i].g), (int)((Real)Particle[i].b), (int)(alpha*255));		
				Screen.D3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, p, sizeof(ParticleVertex));*/
				
			}
			
			
		}

		Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		
	}

	Screen.SetRenderState(D3DRS_BLENDOP,  D3DBLENDOP_ADD);
	Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
    Screen.D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	Screen.D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	Screen.D3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	D3DXMatrixIdentity( &matWorld );
	Screen.D3DDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

