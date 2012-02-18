#include "construction.h"
#include "solver.h"
#include "..\world\world.h"

//#include "..\defs\defs.h"




Construction::Construction()
{
	ObjectList = NULL;
	JointList  = NULL;
	nObjects   = 0;
	nJoints    = 0;
	mySolver   = NULL;
	GeomGroup  = NULL;
	CollisionHandler = NULL;
	LinearDisableEpsilon = .2;
	AngularDisableEpsilon = .1f;
	Active = false;
}

Construction::~Construction()
{
	Deconstruct();
}

std::string Construction::GetTypeString()
{
	return "Construction";
}

//since this is just the base level construction, we'll just make a sphere (WHOO, EXCITING!!!!)
bool Construction::Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen,MeshManager& MM, Position& Location,  ICollisionHandler* ch )
{
	//deconstruct any old stuff
	Deconstruct();

	//save the solver pointer
	mySolver = solver;

	CollisionHandler = ch;

	//create the body list
	ObjectList = new DynamicsObject[1];
	this->nObjects = 1;

	//Create the geom group
	GeomGroup = dSimpleSpaceCreate (solver->GetSpaceID(false));  //dCreateGeomGroup (solver->GetSpaceID(false));  
	
	//Create the actual body ( a sphere! )
	ObjectList[0].CreateBody( solver );
	dBodySetPosition ( ObjectList[0].Body, Location.x, Location.y, Location.z);
	dMassSetSphere ( &ObjectList[0].Mass, 1.0, 5.0 );
	dMassAdjust (&ObjectList[0].Mass, 1.0 );
	dBodySetMass( ObjectList[0].Body, &ObjectList[0].Mass);
	ObjectList[0].Geom = dCreateSphere (0,5.0);
	dGeomSetData( ObjectList[0].Geom, &ObjectList[0].SurfaceDesc );
	dGeomSetBody (ObjectList[0].Geom,ObjectList[0].Body);
	dSpaceAdd (GeomGroup,ObjectList[0].Geom);
	ObjectList[0].HasGeom = true;

	//set owner
	for(int i=0; i<nObjects; i++)
	{
		ObjectList[i].SurfaceDesc.Owner = &ObjectList[i];
		ObjectList[i].SurfaceDesc.ParentConstruction = this;
		ObjectList[i].Owner = this;
		//ObjectList[i].HasGeom = true;
	}

	LinearDisableEpsilon = .1;
	AngularDisableEpsilon = .01f;

	//create the mesh for drawing
	D3DXCreateSphere( Screen.D3DDevice, 5.5f, 10, 10, &DrawMesh, NULL );

	

	return true;
}

void Construction::Deconstruct()
{
	
	if( JointList)
		delete [] JointList;
	JointList  = NULL;
	nJoints    = 0;

	if( ObjectList )
		delete [] ObjectList;
	ObjectList = NULL;
	nObjects   = 0;

	/*if( GeomGroup != NULL )
		dSpaceDestroy( GeomGroup);*/
	//GeomGroup  = NULL;
	
	
	
}

void Construction::ApplyForce( Vector3D force )
{
	for( int i=0; i< nObjects; i++ )
		dBodyAddForce(  ObjectList[i].Body, force.x, force.y, force.z );
}

void Construction::MainUpdate( Real FrameTime, WorldManager* SM )
{
	Update( FrameTime, SM );

	//check if something something active? EXACTLY
	if(IsActive())
		mySolver->CacheConstruction(this);
}


void Construction::OnCollide( ICollisionHandler* objectHit )
{ 
	if( this->CollisionHandler != NULL ) 
		CollisionHandler->OnCollide(objectHit); 
}
void Construction::OnDamage ( const Vector3D& source, const Vector3D& collisionPoint, float damageAmount )
{
	if( CollisionHandler != NULL ) 
		CollisionHandler->OnDamage( source, collisionPoint, damageAmount );
}

void Construction::Update( Real FrameTime, WorldManager* SM )
{
	//stuff
}

void Construction::Turn( Real Force , Real Angle) 
{
	Activate();
	OnTurn( Force, Angle );
	
}

void Construction::Accelerate( Real Force, Real SideForce, Real UpForce )
{
	Activate();
	OnAccelerate( Force, SideForce, UpForce );
}

void Construction::OnTurn( Real Force, Real Angle )
{
}

void Construction::OnAccelerate ( Real Force,Real SideForce, Real UpForce )
{

}

void Construction::EnableAllGeoms()
{
	for(int i=0; i<nObjects; i++)
	{
		if(ObjectList[i].HasGeom )
			dGeomEnable( ObjectList[i].Geom );
	}
}

void Construction::DisableAllGeoms()
{
	for(int i=0; i<nObjects; i++)
	{
		if(ObjectList[i].HasGeom)
			dGeomDisable( ObjectList[i].Geom );
	}
}

void Construction::Activate()
{
	for(int i=0; i<nObjects; i++)
	{

		dBodyEnable( ObjectList[i].Body  );
		if( ObjectList[i].HasGeom )
			dGeomEnable( ObjectList[i].Geom );

	}

	Active = true;
}

bool Construction::Disable()
{

	//check if all subconstructions are disabled
/*	int ndisabled=0;
	for(int i=0; i<nObjects; i++)
	{
	
		if( dBodyIsEnabled( ObjectList[i].Body ) == 0)
		{
			ndisabled++;
		}
	}

	//if they're all disabled, then return true
	if (ndisabled == nObjects)
	{
		Active = false;
		return true;
	}

*/

	/*if(dBodyIsEnabled( ObjectList[0].Body ) == 0 )
		return true;*/



	//otherwise loop through the bodies and get their speed, then
	//disable them if the speed is below a certain epsilon
	//value
/*	dReal* v;
	dReal* a;
	Vector3D vMag;

	ndisabled = 0;
	for(int i=0; i<nObjects; i++)
	{
		v = (dReal*)dBodyGetLinearVel( ObjectList[i].Body );
		a = (dReal*)dBodyGetAngularVel( ObjectList[i].Body );
		vMag.x = v[0];
		vMag.y = v[1];
		vMag.z = v[2];

		if( vMag.Mag() < LinearDisableEpsilon )
		{
			ndisabled++;
		}
	}

	if (ndisabled == nObjects)
	{*/
		//for(int i=0; i<nObjects; i++)
		//{
			//dBodyDisable( ObjectList[0].Body );
		//}
		//Active = false;
		return true;
	//}

	//return false;
}


bool Construction::IsActive()
{
	
	for(int i=0; i<nObjects; i++)
	{
		if(dBodyIsEnabled( ObjectList[i].Body ) == false )
			return false;
	}


	return true;
}

bool Construction::IsGeomActive()
{
	for(int i=0; i<nObjects; i++)
	{
		if(ObjectList[i].HasGeom )
		{
			if(dGeomIsEnabled( ObjectList[i].Geom ) )
				return true;
		}
	}
	return false;
}

bool Construction::CheckForParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager* MM )
{
	return false;
}

DynamicsObject* Construction::GetSubObject( int i )
{
	if(i < nObjects && i >= 0)
		return &ObjectList[i];

	return NULL;
}

void Construction::GetLocation(Vector3D& v)
{
	const dReal* position = dBodyGetPosition  ( ObjectList[0].Body );
	v.x = position[0];
	v.y = position[1];
	v.z = position[2];

}

void Construction::GetVelocity(Vector3D& v)
{
	const dReal* velocity = dBodyGetLinearVel  ( ObjectList[0].Body );
	v.x = velocity[0];
	v.y = velocity[1];
	v.z = velocity[2];
}

void Construction::SetLocation ( Real x, Real y, Real z )
{
	dBodySetPosition( ObjectList[0].Body, x, y, z );
}

void Construction::SetVelocity ( Real x, Real y, Real z )
{
	dBodySetLinearVel ( ObjectList[0].Body, x, y, z );
}

void Construction::GetMatrix( Matrix* M )
{
	if(!M)
		return;

	mySolver->GetTransform( ObjectList[0].Geom, M );

}

void Construction::Draw(Screen3D& Screen)
{
	Matrix M;
	mySolver->GetTransform( ObjectList[0].Geom, &M );
		
	Screen.SetTransform(M);
	Screen.SetTexture(-1, 0);
	DrawMesh->DrawSubset(0);
	//Screen.ShowText(0, 80, "%f %f %f", L[0], L[1], L[2] );

	
}