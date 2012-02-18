#include "vehicle.h"
#include "..\physics\solver.h"
#include "..\physics\dynamicsobject.h"
#include "..\ini\ini.h"


#include "..\defs\defs.h"

std::string  Vehicle4Wheeled::GetTypeString()
{
	return "Vehicle4Wheeled";
}

Vehicle4Wheeled::Vehicle4Wheeled() : Construction()
{
	ChassisWidth=16; 
	ChassisHeight=6.5;
	ChassisLength=30;
	ChassisX=0;
	ChassisY=0; 
	ChassisZ=0;
	ChassisMass=8.0f;
	MaxSpeed = 90.0f;

	GeomGroup = NULL;



	//Center of gravity
	CenterX = 0; 
	CenterY = 0; 
	CenterZ = 0;

	SteeringForce=0;
	SteeringAngle=0;
	PedalForce = 0;

	RPM = 0;
	ClutchDepressed = false;
	Gear = 0;

	pMM = NULL;
}


bool Vehicle4Wheeled::Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen,MeshManager& MM, Position& Location, ICollisionHandler* collisionHandler )
{
	//deconstruct any old stuff
	Deconstruct();

	//save the solver pointer
	mySolver = solver;
	pMM = &MM;

	Location.y += 5.0f;
	//load some values
	char Buffer[256];
	ChassisWidth =GetIniValueFloat( descriptionFile, "vehicle4wheeled", "ChassisWidth", Buffer ) ; 
	ChassisHeight=GetIniValueFloat( descriptionFile, "vehicle4wheeled", "ChassisHeight", Buffer ) ; ;
	ChassisLength=GetIniValueFloat( descriptionFile, "vehicle4wheeled", "ChassisLength", Buffer ) ; ;
	ChassisX=0;
	ChassisY=GetIniValueFloat( descriptionFile, "vehicle4wheeled", "ChassisY", Buffer ) ; 
	ChassisZ=0;
	ChassisMass=6.0f;

	FrontWheelZOffset= GetIniValueFloat( descriptionFile, "vehicle4wheeled", "FrontWheelZOffset", Buffer ) ; 
	BackWheelZOffset = GetIniValueFloat( descriptionFile, "vehicle4wheeled", "BackWheelZOffset", Buffer ) ; 
	WheelXOffset     = GetIniValueFloat( descriptionFile, "vehicle4wheeled", "WheelXOffset", Buffer ) ; 
	WheelYOffset     = GetIniValueFloat( descriptionFile, "vehicle4wheeled", "WheelYOffset", Buffer ) ; 

	float wheelRadius =GetIniValueFloat( descriptionFile, "vehicle4wheeled", "WheelRadius", Buffer ) ;  


	PedalForce = 0.0f;
	MaxSpeed   = GetIniValueFloat( descriptionFile, "vehicle4wheeled", "MaxSpeed", Buffer ) ;  
	HandBrake  = false;

	//Center of gravity
	CenterX = 0; 
	CenterY = 0; 
	CenterZ = 0;

	//create the body list
	ObjectList = new DynamicsObject[5];
	this->nObjects = 5;

	//Create the geom group
	GeomGroup = dSimpleSpaceCreate (solver->GetSpaceID(false));  //dCreateGeomGroup (solver->GetSpaceID(false));  
	
	//Create the wheels
	for(int i=0; i<4; i++)
	{
		ObjectList[i].CreateBody( solver );
		
		dMassSetSphere ( &ObjectList[i].Mass, 1.0, wheelRadius );
		dMassAdjust (&ObjectList[i].Mass, ChassisMass );
		dBodySetMass( ObjectList[i].Body, &ObjectList[i].Mass);
		ObjectList[i].Geom = dCreateSphere (0,wheelRadius);
		dGeomSetData( ObjectList[i].Geom, &ObjectList[i].SurfaceDesc );
		dGeomSetBody (ObjectList[i].Geom,ObjectList[i].Body);
		dSpaceAdd (GeomGroup,ObjectList[i].Geom);
		ObjectList[i].HasGeom = true;

		dBodySetFiniteRotationMode( ObjectList[i].Body, 1 );
	}
	
	//comment
	dBodySetPosition ( ObjectList[0].Body, Location.x-WheelXOffset, Location.y-WheelYOffset, Location.z+FrontWheelZOffset);
	dBodySetPosition ( ObjectList[1].Body, Location.x+WheelXOffset, Location.y-WheelYOffset, Location.z+FrontWheelZOffset);
	dBodySetPosition ( ObjectList[2].Body, Location.x-WheelXOffset, Location.y-WheelYOffset, Location.z-BackWheelZOffset);
	dBodySetPosition ( ObjectList[3].Body, Location.x+WheelXOffset, Location.y-WheelYOffset, Location.z-BackWheelZOffset);

	//create surface desc for chassis
	ObjectList[4].SurfaceDesc.ContactSlip1 = .9f;
	ObjectList[4].SurfaceDesc.ContactSlip2 = .9f;

	//create the chassis
	ObjectList[4].CreateBody(solver);
	dBodySetPosition (ObjectList[4].Body ,Location.x,Location.y+ChassisY,Location.z);
	dMassSetBox (&ObjectList[4].Mass,4,ChassisWidth*2 ,ChassisHeight,ChassisLength*2);
	dMassAdjust (&ObjectList[4].Mass, ChassisMass);
	//dMassTranslate(&ObjectList[4].Mass, 0,-10, 0 );
	dBodySetMass (ObjectList[4].Body,&ObjectList[4].Mass);
	ObjectList[4].Geom = dCreateBox (0,ChassisWidth ,ChassisHeight,ChassisLength);
	dGeomSetData( ObjectList[4].Geom, &ObjectList[4].SurfaceDesc );
	dGeomSetBody (ObjectList[4].Geom,ObjectList[4].Body);
	dSpaceAdd( GeomGroup, ObjectList[4].Geom );
	ObjectList[4].HasGeom = true;



	//create front and back wheel hinges
	JointList = new dJointID[4];
	nJoints=4;

	for (int i=0; i<4; i++) {
		JointList[i] = dJointCreateHinge2 (solver->GetWorldID(),0);
		dJointAttach (JointList[i],ObjectList[4].Body,ObjectList[i].Body);
		const dReal *a = dBodyGetPosition (ObjectList[i].Body);
		dJointSetHinge2Anchor (JointList[i],a[0],a[1],a[2]);
		dJointSetHinge2Axis1 (JointList[i],0,1,0);
		dJointSetHinge2Axis2 (JointList[i],1,0,0);
	}

	// set joint suspension
	for (int i=0; i<4; i++) {
		dJointSetHinge2Param (JointList[i],dParamSuspensionERP,0.5);
		dJointSetHinge2Param (JointList[i],dParamSuspensionCFM,0.001);
	}

	// lock wheels along the steering axis
	for (int i=0; i<4; i++) {
		// set stops to make sure wheels always stay in alignment
		dJointSetHinge2Param (JointList[i],dParamLoStop,0);
		dJointSetHinge2Param (JointList[i],dParamHiStop,0);
	
	}

	//set stuff
	for(int i=0; i<nObjects; i++)
	{
		ObjectList[i].SurfaceDesc.SoftCFM = .001f;
		ObjectList[i].SurfaceDesc.SoftERP = .95f;
		ObjectList[i].SurfaceDesc.mu	  = 0.8f;
		ObjectList[i].Owner = this;

		dBodyDisable( ObjectList[i].Body );
	}

	//create the mesh for drawing
	D3DXCreateBox( Screen.D3DDevice, ChassisWidth, ChassisHeight, ChassisLength,  &DrawMesh, NULL );
	D3DXCreateSphere( Screen.D3DDevice, 3.6f, 10, 10, &d3dxWheelMesh, NULL );


	GetIniValue( descriptionFile, "vehicle4wheeled", "ChassisMesh", Buffer);
	ChassisMesh = MM.GetMeshHandle( Buffer );
	GetIniValue( descriptionFile, "vehicle4wheeled", "WheelMesh", Buffer);
	WheelMesh = MM.GetMeshHandle (  Buffer );

	//yay
	LinearDisableEpsilon = .1;
	AngularDisableEpsilon = .01f;
    	

	return true;
}

void Vehicle4Wheeled::OnTurn( Real Force, Real Angle )
{
	SteeringAngle = Angle;
	SteeringForce = 4000;
}

void Vehicle4Wheeled::OnAccelerate ( Real Force, Real SideForce, Real UpForce  )
{
	PedalForce = Force;
}

void Vehicle4Wheeled::SetGear( int g ) 
{ 
	int GearDif = g-Gear;
	
	if (ClutchDepressed)
	{
		Gear = g; 
		RPM -= 1000*GearDif;
	}
	
}

Real Vehicle4Wheeled::GetGearTorque( int g, Real rpm )
{
	if ( g== -1 )
		return 50*log( rpm * 0.01f );
	else if( g == 0)
		return 0;
    else if( g == 1)
		return 50*log( rpm * 0.01f );
	else if( g == 2 )
		return 40*log( rpm * 0.01f );
	else if( g == 3 )
		return 30*log( rpm * 0.01f );
	else if( g == 4 )
		return 10*log( rpm * 0.01f );

	return 0;
}

Real Vehicle4Wheeled::GetGearTopSpeed(int g )
{
	if ( g== -1 )
		return -10;
	else if( g == 0)
		return 100000; // gear doesn't have a "top" speed because it's neutral
    else if( g == 1)
		return 15;
	else if( g == 2 )
		return 25;
	else if( g == 3 )
		return 35;
	else if( g == 4 )
		return 40;

	return 0;
}

Real  Vehicle4Wheeled::GetRPMForGear( int g, Real torque )
{
   if ( g== -1 )
		return 100*pow((float)10, (float)torque/50.0f );
	else if( g == 0)
		return 0;
    else if( g == 1)
		return 100*pow((float)10, (float)torque/50.0f );
	else if( g == 2 )
		return 100*pow((float)10, (float)torque/40.0f );
	else if( g == 3 )
		return 100*pow((float)10, (float)torque/30.0f );
	else if( g == 4 )
		return 100*pow((float)10, (float)torque/10.0f );
	return 0;
}

void Vehicle4Wheeled::GetMatrix( Matrix* M )
{
	if(!M)
		return;

	mySolver->GetTransform( ObjectList[4].Geom, M );

}


bool Vehicle4Wheeled::CheckForParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager* MM )
{
	Vector3D line[2];
	line[0] = p1;
	line[1] = p2;

	//check the chassis
	Matrix M;
	Mesh* pChassisMesh = MM->Get( ChassisMesh );
	mySolver->GetTransform( ObjectList[4].Geom, &M );



	if ( pChassisMesh->CheckForCollision( M, line, c ) )
	{
		Activate();

		//rotate our vector so we apply force in proper direction
		//apply force
		Vector3D v = p2-p1;
		v.Normalize();
		dBodyAddForceAtPos( ObjectList[4].Body, (v.x)*c->Mass , (v.y)*c->Mass, (v.z)*c->Mass, p1.x, p1.y, p1.z );
		return true;
	}

	return false;
}

void Vehicle4Wheeled::Update( Real FrameTime, WorldManager* SM )
{

	//figure out the current speed
	const dReal* velocityVector = dBodyGetLinearVel  ( ObjectList[4].Body );
	CurrentSpeed = sqrt( velocityVector[0]*velocityVector[0] + 
						 velocityVector[1]*velocityVector[1] +
						 velocityVector[2]*velocityVector[2] );

	if(PedalForce < 10)
	{
		ObjectList[4].SurfaceDesc.ContactSlip1 = 0.9f;
		ObjectList[4].SurfaceDesc.ContactSlip2 = 0.9f;
	}
	else
	{
		ObjectList[4].SurfaceDesc.ContactSlip1 = 0.001f;
		ObjectList[4].SurfaceDesc.ContactSlip2 = 0.001f;
	}


	// motor
	Real Force=0;	
	Real Vel = 0;
	
	
	if( PedalForce > 0 )
		Vel = MaxSpeed;

	if(PedalForce < 0)
		Vel = -MaxSpeed/4.0f; // for braking/reverse
	
	Force = 1600;//(fabs(PedalForce)/100.0f)*900.0f;

	if(HandBrake)
	{
		Vel = 0;
		Force = 2000.0f;
	}

	Real LForce=Force, RForce=Force;

		
	dJointSetHinge2Param (JointList[0],dParamVel2,-Vel);
	dJointSetHinge2Param (JointList[0],dParamFMax2,LForce);
	dJointSetHinge2Param (JointList[1],dParamVel2,-Vel);
	dJointSetHinge2Param (JointList[1],dParamFMax2,RForce);
	dJointSetHinge2Param (JointList[2],dParamVel2,-Vel);
	dJointSetHinge2Param (JointList[2],dParamFMax2,LForce);
	dJointSetHinge2Param (JointList[3],dParamVel2,-Vel);
	dJointSetHinge2Param (JointList[3],dParamFMax2,RForce);

	
	// steering
	//SteeringAngle = 0;
	dReal v = SteeringAngle - dJointGetHinge2Angle1 (JointList[0]);
	//if (v > 0.1) v = 0.1;
	//if (v < -0.1) v = -0.1;
	//v *= 40.0;
	v*=4;
	dJointSetHinge2Param (JointList[0],dParamVel,v);
	dJointSetHinge2Param (JointList[0],dParamFMax,SteeringForce);
	dJointSetHinge2Param (JointList[0],dParamLoStop,-0.75);
	dJointSetHinge2Param (JointList[0],dParamHiStop,0.75);
	//dJointSetHinge2Param (JointList[0],dParamFudgeFactor,0.001);
	
	v = SteeringAngle - dJointGetHinge2Angle1 (JointList[1]);
	v*=4;
	//if (v > 0.1) v = 0.1;
	//if (v < -0.1) v = -0.1;
	//v *= 40.0;
	dJointSetHinge2Param (JointList[1],dParamVel,v);
	dJointSetHinge2Param (JointList[1],dParamFMax,SteeringForce);
	dJointSetHinge2Param (JointList[1],dParamLoStop,-0.75);
	dJointSetHinge2Param (JointList[1],dParamHiStop,0.75);
	//dJointSetHinge2Param (JointList[1],dParamFudgeFactor,0.001);


	/*dJointSetHinge2Param (JointList[3],dParamStopERP,0.9);
	dJointSetHinge2Param (JointList[2],dParamStopERP,0.9);
	dJointSetHinge2Param (JointList[1],dParamStopERP,0.8);
	dJointSetHinge2Param (JointList[0],dParamStopERP,0.8);
	dJointSetHinge2Param (JointList[3],dParamStopCFM,0.01);
	dJointSetHinge2Param (JointList[2],dParamStopCFM,0.01);
	dJointSetHinge2Param (JointList[1],dParamStopCFM,0.01);
	dJointSetHinge2Param (JointList[0],dParamStopCFM,0.01);*/

	v = -dJointGetHinge2Angle1 (JointList[2]);
	if (v > 0.1) v = 0.1;
	if (v < -0.1) v = -0.1;
	//v*=40;
	dJointSetHinge2Param (JointList[2],dParamVel,v);
	dJointSetHinge2Param (JointList[2],dParamFMax,SteeringForce);

	v = -dJointGetHinge2Angle1 (JointList[3]);
	if (v > 0.1) v = 0.1;
	if (v < -0.1) v = -0.1;
	//v*=40;
	dJointSetHinge2Param (JointList[3],dParamVel,v);
	dJointSetHinge2Param (JointList[3],dParamFMax,SteeringForce);

	/*dJointSetHinge2Param (JointList[2],dParamLoStop,-0.001);
	dJointSetHinge2Param (JointList[2],dParamHiStop,0.001);
	dJointSetHinge2Param (JointList[3],dParamLoStop,-0.001);
	dJointSetHinge2Param (JointList[3],dParamHiStop,0.001);
	dJointSetHinge2Param (JointList[2],dParamFudgeFactor,0.01);
	dJointSetHinge2Param (JointList[3],dParamFudgeFactor,0.01);*/
	
}


void Vehicle4Wheeled::Draw(Screen3D& Screen)
{
	Matrix M;


	//draw the chassis
	mySolver->GetTransform( ObjectList[4].Geom, &M );
	Screen.SetTransform(M);
	Screen.SetTexture(-1, 0);
	Screen.SetTexture(-1, 1);
	Screen.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE );
	Screen.SetRenderState(D3DRS_ALPHAREF, 230 );
	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
	Screen.SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	Screen.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );

	
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	
	Screen.D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	
	//DrawMesh->DrawSubset(0);
	Mesh* pChassisMesh = pMM->Get( ChassisMesh );
	Mesh* pWheelMesh;

	if(pChassisMesh)
	{
		pChassisMesh->Draw(Screen );
	}


	//draw the wheels
	for(int i=0; i<4; i++)
	{
		mySolver->GetTransform( ObjectList[i].Geom, &M );
		Screen.SetTransform(M);
		

		pWheelMesh = pMM->Get( WheelMesh );

		if(pWheelMesh)
			pWheelMesh->Draw(Screen );
		
		Screen.SetTexture(-1, 0);
	//	d3dxWheelMesh->DrawSubset(0);
	}

	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	Screen.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );
}
