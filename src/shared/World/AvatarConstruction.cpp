#include "..\physics\construction.h"
#include "..\physics\solver.h"
#include "..\physics\dynamicsobject.h"
#include "world.h"
#include "avatarconstruction.h"

#include "..\defs\defs.h"



std::string AvatarConstruction::GetTypeString()
{
	return "AvatarConstruction";
}

//since this is just the base level construction, we'll just make a sphere 
bool AvatarConstruction::Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen,MeshManager& MM, Position& Location, ICollisionHandler* aCollisionHandler )
{
	//deconstruct any old stuff
	Deconstruct();

	//save the solver pointer
	mySolver = solver;

	this->CollisionHandler = aCollisionHandler;

	//create the body list
	ObjectList = new DynamicsObject[1];
	this->nObjects = 1;

	//Create the geom group
	GeomGroup = dSimpleSpaceCreate (solver->GetSpaceID(false));  

	//set up surface properties
	ObjectList[0].SurfaceDesc.ContactSlip1 = 0.01f;
	ObjectList[0].SurfaceDesc.ContactSlip2 = 0.01f;
	
	//set some other properties
	for(int i=0; i<nObjects; i++)
	{
		ObjectList[i].SurfaceDesc.Owner = &ObjectList[i];
		ObjectList[i].SurfaceDesc.ParentConstruction = this;
		ObjectList[i].SurfaceDesc.SoftCFM = .01f;
		ObjectList[i].SurfaceDesc.SoftERP = .1f;
		ObjectList[i].SurfaceDesc.mu	  = 10.0f;
	
		ObjectList[i].isAvatar = true;
		ObjectList[i].Owner = this;
	}

	
	//Create the actual body ( a sphere! )
	ObjectList[0].CreateBody( solver );
	dBodySetPosition ( ObjectList[0].Body, Location.x, Location.y, Location.z);
	dMassSetSphere ( &ObjectList[0].Mass, 1.0, 6.0 );
	dMassAdjust (&ObjectList[0].Mass, 1.0 );
	dBodySetMass( ObjectList[0].Body, &ObjectList[0].Mass);
	ObjectList[0].Geom = dCreateSphere (0,6.0);
	dGeomSetData( ObjectList[0].Geom, &ObjectList[0].SurfaceDesc );
	dGeomSetBody (ObjectList[0].Geom,ObjectList[0].Body);
	dSpaceAdd (GeomGroup,ObjectList[0].Geom);
	ObjectList[0].HasGeom = true;

	//create the angular motor
	this->nJoints = 1;
	JointList = new dJointID[nJoints];
	JointList[0] = dJointCreateAMotor( solver->GetWorldID(), 0 );
	dJointSetAMotorMode	( JointList[0], dAMotorUser );
	dJointSetAMotorNumAxes( JointList[0], 3 );
	dJointSetAMotorAxis(JointList[0], 0, 0, 1, 0, 0); //x axis
	dJointSetAMotorAxis(JointList[0], 1, 0, 0, 1, 0); //y axis
	dJointSetAMotorAxis(JointList[0], 2, 0, 0, 0, 1); //z axis
	dJointAttach( JointList[0], ObjectList[0].Body, NULL); //attach to world



	//set some properties
	LinearDisableEpsilon = .2;
	AngularDisableEpsilon = .01f;
	JumpDelay = 1000;
	LastJumpTime = 0;

	ForceEnable = false;
	//create the mesh for drawing
	D3DXCreateSphere( Screen.D3DDevice, 6.0f, 20, 20, &DrawMesh, NULL );

	

	return true;
}

void AvatarConstruction::Deconstruct()
{
	
	if( JointList)
		delete [] JointList;
	if( ObjectList )
		delete [] ObjectList;

	JointList  = NULL;
	ObjectList = NULL;
	nObjects   = 0;
	nJoints    = 0;
}

void AvatarConstruction::Update( Real FrameTime, WorldManager* SM )
{
	if(ForceEnable)
		dBodyEnable(ObjectList[0].Body );
	ForceEnable=false;

	
	OnGround = false;
	Moving = false;
	
}

void AvatarConstruction::OnTurn( Real Force, Real Angle )
{
	//dBodyAddForce( ObjectList[0].Body, Force, 0, 0 );
}

void AvatarConstruction::OnAccelerate ( Real Force, Real SideForce, Real UpForce  )
{
	if(!OnGround)
		return;

	if(UpForce > 0 )
	{
		if( (timeGetTime()-LastJumpTime) < 1000 )
			UpForce = 0;
		else
			LastJumpTime = timeGetTime();
	}

	const dReal* currentSpeed = dBodyGetLinearVel( ObjectList[0].Body );
	Vector3D v, v2;
	v.x = currentSpeed[0];
	v.y = 0;
	v.z = currentSpeed[2];

	if( UpForce != 0 )
		dBodySetLinearVel( ObjectList[0].Body, v.x, UpForce, v.z );

	if(Force >0 || SideForce>0 || UpForce > 0)
		ForceEnable= true;

	float m=(float)sqrt(Force*Force+SideForce*SideForce);

	
	Vector3D r ( SideForce, 0, Force );
	r.Rotatey( RADIAN(-90 ));

	dBodySetAngularVel( ObjectList[0].Body, r.x, 0, r.z );
	

	if( SideForce != 0 || Force != 0)
		Moving = true;
}


bool AvatarConstruction::Disable()
{
	Active = true;
	return false; 
}

void AvatarConstruction::Draw(Screen3D& Screen)
{
	Matrix M;
	mySolver->GetTransform( ObjectList[0].Geom, &M );
		
	Screen.SetTransform(M);
	Screen.SetTexture(-1, 0);

	DWORD fillmode;
	Screen.D3DDevice->GetRenderState( D3DRS_FILLMODE, &fillmode );
	Screen.SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	Screen.SetVertexShader( NULL);
	DrawMesh->DrawSubset(0);

	Screen.SetRenderState( D3DRS_FILLMODE, fillmode );
	
}