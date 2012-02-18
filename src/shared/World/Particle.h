#pragma once
// I N C L U D E S ////////////////////////////////////
#include "..\s3dlib\screen.h"
#include "..\s3dlib\vertexbuffer.h"
#include "..\s3dlib\types.h"
#include "..\s3dlib\camera.h"
#include "..\Mesh\meshmanager.h"
#include "..\physics\solver.h"
// D E F I N E S //////////////////////////////////////
enum ParticleDistribution {STREAM_STANDARD,STREAM_ROTATE};
enum ParticleCollisionType { COL_NONE, COL_LANDSCAPE, COL_LANDSCAPE_AND_OBJECTS };
enum ParticleBlendMode { PARTICLEBLEND_ADDITIVE=0, PARTICLEBLEND_MULTIPLY=1 };

class WorldManager;
class ParticleStream;

inline float RAND( float max, float min )
{
	int m = (int)(max*100);
	int n = (int)(min*100);

	Real returnvalue =(Real)( rand()%(m-n)+(n));

	returnvalue /= 100;

	return returnvalue;
}


// F U N C T I O N S ////////////////////////////////////

//represents an individual particle
struct	Particle_S
{
	float	life;					// Particle Life
	float	fade;					// Fade Speed
	BYTE	r,  g,  b;				// Color
	float	x, y, z;				// Position
	float   ox, oy, oz;             // Old position, used for collision detection
	float	vx, vy, vz;			    // Direction
	float	xg, yg, zg;				// Gravity
	float   mass; 
	float  startx, starty, startz;
	float  collidex, collidey, collidez;

	dGeomID geom;
	ParticleStream* streamPtr;
	
	bool    active;  //Is the particle currently being used?
	bool    HasForce; //true by default, but if this hits something then it
					  //doesn't have force anymore. (meaning it can't damage
					  //anything).

	Particle_S() { active = false; geom = NULL; streamPtr = NULL;}
	~Particle_S() { 
		//if( geom != NULL)
		//	dGeomDestroy( geom );
	}

	void Activate();
	void Deactivate();

	bool __inline CheckForCollision( WorldManager* WMPtr, Real FrameTime, ParticleStream& Stream );

	void NotifyCollision( );
	void __forceinline Update( ParticleStream& Stream, Real FrameTime );
};

//use this to make a particle effect somewhere
class ParticleStream
{
public:


	//Where particles are stored
	VertexBuffer ParticleBuffer;
	Particle_S* Particle;
	int MaxParticles;
	int ActiveParticles;

	//If the particles have a "trail" on them with other particles,
	//then this is created.
	ParticleStream* TrailStream;
	Real TrailsPerParticle;

	Real ParticleConstantDistance;
	
	//Explosion particles are activated when a particle dies to
	//blow something up (OMGS!_)
	ParticleStream* ExplosionStream;
	Real ParticlesPerExplosion;

	//Basic particle information
	Real Power;      //big explosion or little one?
	Real x, y, z ;    //where the particle stream is
	Real xg, yg, zg; //direction the particles are being pulled
	Real ParticleLife; //how long particles last
	Real ParticleMass;
	Real r, g, b;      //default color for particles
	Real Width, Depth; //width/height of a plane for use with snow and such
	Real ParticleSize;
	bool Active; //if false the stream freezes
	ParticleDistribution StreamType;
	Vector3D Wind;

	// Particle image data
	TextureHandle StreamTexture;
	TextureHandle GlossMap;
	MeshHandle ParticleObject; 
	bool ShowObject;
	bool ShowTrail;
	bool LinearFade;
	bool NoDust;


	// Visibility determination
	Real FarthestPoint;
	AABB Box;


	// Particle recycling variables
	bool SpawnContinuously;
	Real ParticlesPerSecond;

	//Stream life variables are used to disable the SpawnContinuously variable
	//after a set amount of time.
	bool UseStreamTimer;
	Real StreamLife; //current time left in stream
	Real InitialStreamLife; //this is what the stream's initial life was
	Real DeathDelay;

	bool Reflect;

	//Weird variable, once this is set to 1
	//a new particle is spawned and the variable is reset
	//to zero. The reason for this is because we can't spawn
	//.3 particles, so we just buffer the fraction of particle
	//requests until we can make a full one.
	Real ParticleFraction;
	
	//Used for bullets and other aimed particles
	Vector3D StreamHeading;
	Real HeadingVariance;
	bool UseHeading;

	
	
	//Reaction variables
	ParticleCollisionType CollisionType;
	bool HitPlayer;
	WorldManager* WMPtr; //used to test for reactions

	dSpaceID localSpace;

	ParticleBlendMode BlendMode ;

public:

	ParticleStream() { Particle = NULL; ActiveParticles=0; 
			ParticlesPerSecond=0; TrailStream = NULL; ExplosionStream=NULL; NoDust = false;
			StreamType = STREAM_STANDARD;
			ParticleConstantDistance = 0;
			localSpace = NULL;
			BlendMode = PARTICLEBLEND_ADDITIVE;
			DeathDelay = 0;
	}
	~ParticleStream() { Shutdown(); }

	bool Init( Screen3D& Screen, char* ParticleFile, WorldManager* WM );
	void Shutdown() {
		if( localSpace != NULL ) dSpaceDestroy( localSpace );
		if (TrailStream) delete TrailStream; TrailStream = NULL;
		if (Particle) delete [] Particle; Particle = NULL; 
		 
	}

	
	void SpawnParticles( Real nParticles, bool SpawnEverySecond );
	void ToggleActivity() { SpawnContinuously = !SpawnContinuously; }
	void Stop();  //stops emmitting particles
		
	void SetWind(Vector3D _Wind){Wind = _Wind;};

	bool CheckVisibility( Screen3D& Screen );
	void Draw(Screen3D& Screen, Camera& Viewer); //draw em!
	
	void Update( Real FrameTime); //move all the particles
	void MoveSpawn(Real nx, Real ny, Real nz){x=nx; y=ny; z=nz;};
	void TranslateParticles( Real nx, Real ny, Real nz );

	
};

