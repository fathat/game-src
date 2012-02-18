#include "tankconstruct.h"
#include "..\physics\solver.h"
#include "..\ini\ini.h"


#include "..\defs\defs.h"

std::string TankConstruct::GetTypeString()
{
	return "TankConstruct";
}

TankConstruct::TankConstruct() : Construction()
{
	ChassisWidth=24; 
	ChassisHeight=6.5;
	ChassisLength=30;
	ChassisX=0;
	ChassisY=0; 
	ChassisZ=0;
	ChassisMass=1.0f;


	//Center of gravity
	CenterX = 0; 
	CenterY = 0; 
	CenterZ = 0;

	MaxSpeed = 3;

	Throttle = 0;
	TurnForce= 0;
	
	pMM = NULL;
}


bool TankConstruct::Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen,MeshManager& MM, Position& Location, ICollisionHandler* collisionHandler )
{
	//deconstruct any old stuff
	Deconstruct();

	//save the solver pointer
	mySolver = solver;
	pMM = &MM;

	//load some values
	char Buffer[256];
	ChassisWidth =GetIniValueFloat( descriptionFile, "tank", "ChassisWidth", Buffer ) ; 
	ChassisHeight=GetIniValueFloat( descriptionFile, "tank", "ChassisHeight", Buffer ) ; 
	ChassisLength=GetIniValueFloat( descriptionFile, "tank", "ChassisLength", Buffer ) ; 
	WheelRadius  = GetIniValueFloat( descriptionFile, "tank", "WheelRadius", Buffer ) ; 
	DriveWheelRadius  = GetIniValueFloat( descriptionFile, "tank", "DriveWheelRadius", Buffer ) ; 
	nWheels=GetIniValue( descriptionFile, "tank", "WheelCount" );
	WheelXOffset = GetIniValueFloat( descriptionFile, "tank", "WheelXOffset", Buffer ) ; 
	WheelYOffset = GetIniValueFloat( descriptionFile, "tank", "WheelYOffset", Buffer ) ; 
	TurretZOffset = GetIniValueFloat( descriptionFile, "tank", "TurretZOffset", Buffer ) ; 
	TurretYOffset = GetIniValueFloat( descriptionFile, "tank", "TurretYOffset", Buffer ) ; 
	GunZOffset = GetIniValueFloat( descriptionFile, "tank", "GunZOffset", Buffer ) ; 
	GunYOffset = GetIniValueFloat( descriptionFile, "tank", "GunYOffset", Buffer ) ; 
	GunXOffset = GetIniValueFloat( descriptionFile, "tank", "GunXOffset", Buffer ) ; 
	ChassisX=0;
	ChassisY=0; 
	ChassisZ=0;
	ChassisMass=0.5f;
	Throttle = 0;
	TurretYawVel =0;
	TurretPitchVel =0;
	MaxSpeed= GetIniValueFloat( descriptionFile, "tank", "MaxSpeed", Buffer ) ;

	LinearDisableEpsilon = 1.1f;
	AngularDisableEpsilon = .1f;

	LeftClutch =false;
	RightClutch=false;

	Location.y+= 5;
	//Center of gravity
	CenterX = 0; 
	CenterY = 0; 
	CenterZ = 0;

	//create the body list
	this->nObjects = 1+nWheels+4+1+1; //chassis + wheels + 4 drive wheels + turret + gun
	ObjectList = new DynamicsObject[nObjects];
	

	//Create the geom group
	GeomGroup = dSimpleSpaceCreate (solver->GetSpaceID(false));  

	
	//create surface desc for chassis
	ObjectList[0].SurfaceDesc.ContactSlip1 = .9f;
	ObjectList[0].SurfaceDesc.ContactSlip2 = .9f;

	//create the chassis
	ObjectList[0].CreateBody(solver);
	dMassSetBox (&ObjectList[0].Mass,1,ChassisWidth ,ChassisHeight,ChassisLength);
	dMassAdjust (&ObjectList[0].Mass, ChassisMass);
	//dMassTranslate(&ObjectList[0].Mass, 0,-10, 0);
	dBodySetMass (ObjectList[0].Body,&ObjectList[0].Mass);
	ObjectList[0].Geom = dCreateBox (0,ChassisWidth ,ChassisHeight,ChassisLength);
	dGeomSetData( ObjectList[0].Geom, &ObjectList[0].SurfaceDesc );
	dGeomSetBody (ObjectList[0].Geom,ObjectList[0].Body);
	dSpaceAdd( GeomGroup, ObjectList[0].Geom );
	ObjectList[0].HasGeom = true;

	dBodySetPosition (ObjectList[0].Body ,Location.x,Location.y+1,Location.z);

	//create the turret
	int TurretIndex = 1+nWheels+4;
	ObjectList[TurretIndex].CreateBody(solver);
	dMassSetBox (&ObjectList[TurretIndex].Mass,1,10,8,10);
	dMassAdjust (&ObjectList[TurretIndex].Mass, ChassisMass);
	dMassTranslate(&ObjectList[TurretIndex].Mass, 0,0, 0);
	dBodySetMass (ObjectList[TurretIndex].Body,&ObjectList[TurretIndex].Mass);
	/*ObjectList[TurretIndex].Geom = dCreateBox (0,10 ,8,10);
	dGeomSetData( ObjectList[TurretIndex].Geom, &ObjectList[TurretIndex].SurfaceDesc );
	dGeomSetBody (ObjectList[TurretIndex].Geom,ObjectList[TurretIndex].Body);
	dGeomGroupAdd( GeomGroup, ObjectList[TurretIndex].Geom );*/
	dBodySetPosition (ObjectList[TurretIndex].Body ,Location.x,Location.y+TurretYOffset,Location.z + TurretZOffset);
	ObjectList[TurretIndex].HasGeom = false;

	//create the gun
	int GunIndex = 1+nWheels+4+1;
	ObjectList[GunIndex].CreateBody(solver);
	dMassSetBox (&ObjectList[GunIndex].Mass,1,1 ,1, 60);
	dMassAdjust (&ObjectList[GunIndex].Mass, ChassisMass);
	dMassTranslate(&ObjectList[GunIndex].Mass, 0,0, 0);
	dBodySetMass (ObjectList[GunIndex].Body,&ObjectList[GunIndex].Mass);
	/*ObjectList[GunIndex].Geom = dCreateBox (0,1 ,1,60);
	dGeomSetData( ObjectList[GunIndex].Geom, &ObjectList[GunIndex].SurfaceDesc );
	dGeomSetBody (ObjectList[GunIndex].Geom,ObjectList[GunIndex].Body);
	dGeomGroupAdd( GeomGroup, ObjectList[GunIndex].Geom );*/
	dBodySetPosition (ObjectList[GunIndex].Body ,Location.x,Location.y+TurretYOffset+GunYOffset,Location.z + TurretZOffset+GunZOffset);
	ObjectList[GunIndex].HasGeom = false;


	
	//Create the wheels
	for(int i=1; i<nWheels+1; i++)
	{
		ObjectList[i].CreateBody( solver );
		
		dMassSetSphere ( &ObjectList[i].Mass, 3.0, WheelRadius );
		dMassAdjust (&ObjectList[i].Mass, ChassisMass );
		dBodySetMass( ObjectList[i].Body, &ObjectList[i].Mass);
		ObjectList[i].Geom = dCreateSphere (0,WheelRadius);
		dGeomSetData( ObjectList[i].Geom, &ObjectList[i].SurfaceDesc );
		dGeomSetBody (ObjectList[i].Geom,ObjectList[i].Body);
		dSpaceAdd (GeomGroup,ObjectList[i].Geom);
		ObjectList[i].HasGeom = true;

		dBodySetFiniteRotationMode( ObjectList[i].Body, 1 );
	}
	
	//set the wheel locations
	//basically, we split the wheels evenly between 
	//the two sides, and then space them evenly. 
	//This does not set the "drive wheels"

	Real offset = (ChassisLength-WheelRadius*2.0f-DriveWheelRadius*2.0f) / ((Real)(nWheels/2.0f)-1 ) ;
	Real startoffset = ((Location.z-(ChassisLength*.5f)) ) + WheelRadius+DriveWheelRadius;
	for( int i=0; i<(nWheels/2); i++ )
	{
		dBodySetPosition ( ObjectList[i+1].Body, Location.x-WheelXOffset, Location.y-WheelYOffset, startoffset+offset*i);
	}

	for( int i=0; i<(nWheels/2); i++ )
	{
		dBodySetPosition ( ObjectList[i+(nWheels/2)+1].Body, Location.x+WheelXOffset, Location.y-WheelYOffset, startoffset+offset*i);
	}

	//Create the drive wheels
	int indexoffset = 1+nWheels;
	for(int i=0; i<4; i++)
	{
		ObjectList[i+indexoffset].CreateBody( solver );
		
		dMassSetSphere ( &ObjectList[i+indexoffset].Mass, 1.0, DriveWheelRadius );
		dMassAdjust (&ObjectList[i+indexoffset].Mass, ChassisMass );
		dBodySetMass( ObjectList[i+indexoffset].Body, &ObjectList[i+indexoffset].Mass);
		ObjectList[i+indexoffset].Geom = dCreateSphere (0,DriveWheelRadius);
		dGeomSetData( ObjectList[i+indexoffset].Geom, &ObjectList[i+indexoffset].SurfaceDesc );
		dGeomSetBody (ObjectList[i+indexoffset].Geom,  ObjectList[i+indexoffset].Body);
		dSpaceAdd (GeomGroup,ObjectList[i+indexoffset].Geom);
		ObjectList[i+indexoffset].HasGeom = true;

		dBodySetFiniteRotationMode( ObjectList[i+indexoffset].Body, 1 );
	}
	

	dBodySetPosition ( ObjectList[indexoffset+0].Body, Location.x-WheelXOffset, (Location.y-WheelYOffset)+DriveWheelRadius, Location.z+ChassisLength/2.0f-DriveWheelRadius);
	dBodySetPosition ( ObjectList[indexoffset+1].Body, Location.x+WheelXOffset, (Location.y-WheelYOffset)+DriveWheelRadius, Location.z+ChassisLength/2.0f-DriveWheelRadius);
	dBodySetPosition ( ObjectList[indexoffset+2].Body, Location.x-WheelXOffset, (Location.y-WheelYOffset)+DriveWheelRadius, Location.z-ChassisLength/2.0f+DriveWheelRadius);
	dBodySetPosition ( ObjectList[indexoffset+3].Body, Location.x+WheelXOffset, (Location.y-WheelYOffset)+DriveWheelRadius, Location.z-ChassisLength/2.0f+DriveWheelRadius);

	//create front and back wheel hinges
	nJoints=nWheels+4+1+1; //wheels + 4 drive wheels + turret + gun
	JointList = new dJointID[nJoints];
	

	for (int i=0; i<nJoints-2; i++) {
		JointList[i] = dJointCreateHinge (solver->GetWorldID(),0);
		dJointAttach (JointList[i],ObjectList[0].Body,ObjectList[i+1].Body);
		const dReal *a = dBodyGetPosition (ObjectList[i+1].Body);
		dJointSetHingeAnchor (JointList[i],a[0],a[1],a[2]);
		dJointSetHingeAxis (JointList[i],1,0,0);
	}

	// set joint suspension
	for (int i=0; i<nJoints-2; i++) {
		dJointSetHingeParam (JointList[i],dParamSuspensionERP,0.9);
		dJointSetHingeParam (JointList[i],dParamSuspensionCFM,0.01);
	}

	//set turret joint
	int turretJointIndex = nJoints-2;
	JointList[turretJointIndex] = dJointCreateHinge (solver->GetWorldID(),0);
	dJointAttach (JointList[turretJointIndex],ObjectList[0].Body,ObjectList[TurretIndex].Body);
	const dReal *anc = dBodyGetPosition (ObjectList[TurretIndex].Body);
	dJointSetHingeAnchor (JointList[turretJointIndex],anc[0],anc[1],anc[2]);
	dJointSetHingeAxis (JointList[turretJointIndex],0,1,0);

	dJointSetHingeParam (JointList[turretJointIndex],dParamVel, 0);
	dJointSetHingeParam (JointList[turretJointIndex],dParamFMax,2000);
	dJointSetHingeParam (JointList[turretJointIndex],dParamSuspensionERP,0.9);
	dJointSetHingeParam (JointList[turretJointIndex],dParamSuspensionCFM,0.01);

	//set the main gun
	int gunJointIndex = nJoints-1;
	JointList[gunJointIndex] = dJointCreateHinge (solver->GetWorldID(),0);
	dJointAttach (JointList[gunJointIndex],ObjectList[TurretIndex].Body,ObjectList[GunIndex].Body);
	const dReal *anc2 = dBodyGetPosition (ObjectList[GunIndex].Body);
	dJointSetHingeAnchor (JointList[gunJointIndex],anc2[0],anc2[1],anc2[2]);
	dJointSetHingeAxis (JointList[gunJointIndex],1,0,0);

	dJointSetHingeParam (JointList[gunJointIndex],dParamVel, 0);
	dJointSetHingeParam (JointList[gunJointIndex],dParamFMax,2000);
	//dJointSetHingeParam (JointList[gunJointIndex],dParamSuspensionERP,0.9);
	//dJointSetHingeParam (JointList[gunJointIndex],dParamSuspensionCFM,0.01);

	//set stuff
	for(int i=0; i<nObjects; i++)
	{
		ObjectList[i].SurfaceDesc.SoftCFM = .001f;
		ObjectList[i].SurfaceDesc.SoftERP = .9f;
		ObjectList[i].SurfaceDesc.mu	  = 0.6f;
		ObjectList[i].Owner = this;
		dBodyDisable( ObjectList[i].Body );
		dBodySetAutoDisableFlag ( ObjectList[i].Body, 1 );
		dBodySetAutoDisableDefaults ( ObjectList[i].Body);
		dBodySetAutoDisableTime ( ObjectList[i].Body, 0.01f);
		dBodySetAutoDisableSteps ( ObjectList[i].Body, 0);
		dBodySetAutoDisableLinearThreshold (ObjectList[i].Body, 1.0f);
		dBodySetAutoDisableAngularThreshold( ObjectList[i].Body, 1.0f);
	}

	//create the mesh for drawing
	D3DXCreateBox( Screen.D3DDevice, ChassisWidth, ChassisHeight, ChassisLength,  &DrawMesh, NULL );
	D3DXCreateSphere( Screen.D3DDevice, WheelRadius, 10, 10, &d3dxWheelMesh, NULL );
	D3DXCreateSphere( Screen.D3DDevice, DriveWheelRadius, 10, 10, &d3dxDriveWheelMesh, NULL );


	GetIniValue( descriptionFile, "tank", "ChassisMesh", Buffer);
	ChassisMesh = MM.GetMeshHandle( Buffer );
	GetIniValue( descriptionFile, "tank", "WheelMesh", Buffer);
	WheelMesh = MM.GetMeshHandle (  Buffer );
	GetIniValue( descriptionFile, "tank", "DriveWheelMesh", Buffer);
	DriveWheelMesh = MM.GetMeshHandle (  Buffer );
	GetIniValue( descriptionFile, "tank", "TurretMesh", Buffer);
	TurretMesh = MM.GetMeshHandle (  Buffer );
	GetIniValue( descriptionFile, "tank", "GunMesh", Buffer);
	GunMesh = MM.GetMeshHandle (  Buffer );

	return true;
}

void TankConstruct::OnTurn( Real Force, Real Angle )
{
	TurretYawVel += Force*0.025f;
	TurretPitchVel += Angle*0.025f;
}

void TankConstruct::OnAccelerate ( Real Force, Real SideForce, Real UpForce  )
{
//	PedalForce = Force;
	Throttle    = Force;
	LeftClutch  = false;
	RightClutch = false;

	TurnForce = SideForce;
	if(SideForce < 0)
		LeftClutch = true;
	if(SideForce > 0)
		RightClutch = true;
}



bool TankConstruct::CheckForParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager* MM )
{
	Vector3D line[2];
	line[0] = p1;
	line[1] = p2;

	//check the chassis
	Matrix M;
	Mesh* pChassisMesh = MM->Get( ChassisMesh );
	mySolver->GetTransform( ObjectList[0].Geom, &M );

	if ( pChassisMesh->CheckForCollision( M, line, c ) )
	{
		Activate();

		//rotate our vector so we apply force in proper direction
		//apply force
		Vector3D v = p2-p1;
		v.Normalize();
		dBodyAddForceAtPos( ObjectList[0].Body, (v.x)*c->Mass , (v.y)*c->Mass, (v.z)*c->Mass, p1.x, p1.y, p1.z );
		return true;
	}

	//check the turret
	
	Mesh* pTurretMesh = MM->Get( TurretMesh );
	mySolver->GetTransform( ObjectList[1+nWheels+4].Body, &M );

	if ( pTurretMesh->CheckForCollision( M, line, c ) )
	{
		Activate();

		//rotate our vector so we apply force in proper direction
		//apply force
		Vector3D v = p2-p1;
		v.Normalize();
		dBodyAddForceAtPos( ObjectList[1+nWheels+4].Body, (v.x)*c->Mass , (v.y)*c->Mass, (v.z)*c->Mass, p1.x, p1.y, p1.z );
		return true;
	}

	return false;
}


void TankConstruct::GetMatrix( Matrix* M )
{
	if(!M)
		return;

	mySolver->GetTransform( ObjectList[0].Geom, M );

}

void TankConstruct::Update( Real FrameTime, WorldManager* SM )
{

	//figure out the current speed
	const dReal* velocityVector = dBodyGetLinearVel  ( ObjectList[0].Body );
	CurrentSpeed = sqrt( velocityVector[0]*velocityVector[0] + 
						 velocityVector[1]*velocityVector[1] +
						 velocityVector[2]*velocityVector[2] );

	
	ObjectList[0].SurfaceDesc.ContactSlip1 = 0.9f;
	ObjectList[0].SurfaceDesc.ContactSlip2 = 0.9f;

	
	// motor
	Real Force=0;	
	Real Vel = MaxSpeed* (Throttle/100.0f);

	//if(Throttle < 0)
	//	Vel = -MaxSpeed; // for braking
	//if(Throttle == 0)
	//	Vel = 0;

	if((RightClutch || LeftClutch)  )
		Vel = MaxSpeed;


	//Force = (fabs(Throttle)/100.0f)*900.0f;
	Force = 5000;


	Real RVel=Vel, LVel=Vel; 
	Real LForce=Force, RForce=Force;

	if(RightClutch)
	{
		RVel *= -.5f;
		RForce *= 1.25f;
	}
	if(LeftClutch)
	{
		LVel *= -.5f;
		LForce *= 1.25f;
	}
	
	//left side
	for(int i=0; i<nWheels/2; i++ )
	{
		dJointSetHingeParam (JointList[i],dParamVel,-LVel);
		dJointSetHingeParam (JointList[i],dParamFMax,LForce);
		dJointSetHingeParam (JointList[i], dParamFudgeFactor, 0.1f );
	}

	//right side
	for(int i=nWheels/2; i<nWheels; i++ )
	{
		dJointSetHingeParam (JointList[i],dParamVel, -RVel);
		dJointSetHingeParam (JointList[i],dParamFMax,RForce);
		dJointSetHingeParam (JointList[i], dParamFudgeFactor, 0.1f );
	}

	//drive wheels
	//right side
	dJointSetHingeParam (JointList[nWheels],dParamVel, -LVel);
	dJointSetHingeParam (JointList[nWheels],dParamFMax,LForce);
	dJointSetHingeParam (JointList[nWheels+2],dParamVel, -LVel);
	dJointSetHingeParam (JointList[nWheels+2],dParamFMax,LForce);
	//left side
	dJointSetHingeParam (JointList[nWheels+1],dParamVel, -RVel);
	dJointSetHingeParam (JointList[nWheels+1],dParamFMax,RForce);
	dJointSetHingeParam (JointList[nWheels+3],dParamVel, -RVel);
	dJointSetHingeParam (JointList[nWheels+3],dParamFMax,RForce);

	//Turret
	dJointSetHingeParam (JointList[nWheels+4],dParamVel, -TurretYawVel);
	dJointSetHingeParam (JointList[nWheels+4],dParamFMax,8000);
	dJointSetHingeParam (JointList[nWheels+4], dParamFudgeFactor, 0.1f );
	
	//Main Gun

	//if ( TurretPitchVel > .45f ) 
		//TurretPitchVel = .1f;
	dJointSetHingeParam (JointList[nWheels+4+1],dParamVel, -TurretPitchVel);
	dJointSetHingeParam (JointList[nWheels+4+1],dParamFMax,3000);
	dJointSetHingeParam (JointList[nWheels+4+1],dParamHiStop, .45f);
	dJointSetHingeParam (JointList[nWheels+4+1],dParamLoStop, -.1f);
	//dJointSetHingeParam (JointList[nWheels+4+1], dParamFudgeFactor, 0.001f );

	//dJointSetHingeParam (JointList[nWheels+4+1],dParamStopERP,0.99);
	//dJointSetHingeParam (JointList[nWheels+4+1],dParamStopCFM,0.0001);
	

	TurretYawVel = 0;
	TurretPitchVel = 0;
	Throttle =0;
	
	
}


void TankConstruct::Draw(Screen3D& Screen)
{

	Matrix M;

	//draw the chassis
	mySolver->GetTransform( ObjectList[0].Geom, &M );
	Screen.SetTransform(M);
	Screen.SetTexture(-1, 0);
	Screen.SetTexture(-1, 1);
	Screen.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE );
	Screen.SetRenderState(D3DRS_ALPHAREF, 230 );
	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
	Screen.SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);

	
	/*Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	
	Screen.D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );*/
	
	//DrawMesh->DrawSubset(0);
	Mesh* pChassisMesh = pMM->Get( ChassisMesh );
	Mesh* pWheelMesh;
	Mesh* pTurretMesh;
	Mesh* pGunMesh;

	if(pChassisMesh)
		pChassisMesh->Draw(Screen );


	//draw the wheels
	for(int i=1; i<nWheels+1; i++)
	{
		mySolver->GetTransform( ObjectList[i].Geom, &M );
		Screen.SetTransform(M);
		

		pWheelMesh = pMM->Get( WheelMesh );

		if(pWheelMesh)
			pWheelMesh->Draw(Screen );
		
		//Screen.SetTexture(-1, 0);
		//d3dxWheelMesh->DrawSubset(0);
	}


	//draw the drive wheels
	int indexoffset = 1+nWheels;

	for(int i=(nWheels+1); i<(nWheels+1)+4; i++)
	{
		mySolver->GetTransform( ObjectList[i].Geom, &M );
		Screen.SetTransform(M);
		

		pWheelMesh = pMM->Get( DriveWheelMesh );

		if(pWheelMesh)
			pWheelMesh->Draw(Screen );
		
		//Screen.SetTexture(-1, 0);
		//d3dxDriveWheelMesh->DrawSubset(0);
	}

	//Draw Turret
	pTurretMesh = pMM->Get( TurretMesh );
	if(pTurretMesh)
	{
		mySolver->GetTransform( ObjectList[nObjects-2].Body, &M );
		Screen.SetTransform(M);
		pTurretMesh->Draw( Screen );
	}

	//Draw main gun
	pGunMesh = pMM->Get( GunMesh );
	if(pGunMesh)
	{
		mySolver->GetTransform( ObjectList[nObjects-1].Body, &M );
		Screen.SetTransform(M);
		pGunMesh->Draw( Screen );
	}


	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
}
