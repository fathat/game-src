#include "tankconstruct.h"
#include "solver.h"
#include "..\ini\ini.h"

void TankConstruct::GetTypeString(char* typebuffer, int maxsize)
{
	strncpy( typebuffer, "tank", maxsize );
}

TankConstruct::TankConstruct()
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

	MaxSpeed = 10;

	Throttle = 0;
	TurnForce= 0;
	pMM = NULL;
}


bool TankConstruct::Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen,MeshManager& MM, Position& Location )
{
	//deconstruct any old stuff
	Deconstruct();

	//save the solver pointer
	mySolver = solver;
	pMM = &MM;

	//load some values
	char Buffer[256];
	ChassisWidth =GetIniValueFloat( descriptionFile, "tank", "ChassisWidth", Buffer ) ; 
	ChassisHeight=GetIniValueFloat( descriptionFile, "tank", "ChassisHeight", Buffer ) ; ;
	ChassisLength=GetIniValueFloat( descriptionFile, "tank", "ChassisLength", Buffer ) ; ;
	WheelRadius  = GetIniValueFloat( descriptionFile, "tank", "WheelRadius", Buffer ) ; ;
	DriveWheelRadius  = GetIniValueFloat( descriptionFile, "tank", "DriveWheelRadius", Buffer ) ; ;
	nWheels=GetIniValue( descriptionFile, "tank", "WheelCount" );
	WheelXOffset = GetIniValueFloat( descriptionFile, "tank", "WheelXOffset", Buffer ) ; ;
	WheelYOffset = GetIniValueFloat( descriptionFile, "tank", "WheelYOffset", Buffer ) ; ;
	ChassisX=0;
	ChassisY=0; 
	ChassisZ=0;
	ChassisMass=1.0f;
	Throttle  = 0;
	TurnForce = 0;

	//Center of gravity
	CenterX = 0; 
	CenterY = 0; 
	CenterZ = 0;

	//create the body list
	this->nObjects = 1; //chassis + wheels + 4 drive wheels
	ObjectList = new DynamicsObject[nObjects];
	

	//Create the geom group
	GeomGroup = dCreateGeomGroup (solver->GetSpaceID(false));  

	
	//create surface desc for chassis
	ObjectList[0].SurfaceDesc.ContactSlip1 = .9f;
	ObjectList[0].SurfaceDesc.ContactSlip2 = .9f;

	//create the chassis
	ObjectList[0].CreateBody(solver);
	dBodySetPosition (ObjectList[0].Body ,Location.x,Location.y+1,Location.z);
	dMassSetBox (&ObjectList[0].Mass,1,ChassisWidth*2 ,ChassisHeight,ChassisLength*2);
	dMassAdjust (&ObjectList[0].Mass, ChassisMass);
	dMassTranslate(&ObjectList[0].Mass, 0,-10, 0 );
	dBodySetMass (ObjectList[0].Body,&ObjectList[0].Mass);
	ObjectList[0].Geom = dCreateBox (0,ChassisWidth ,ChassisHeight,ChassisLength);
	dGeomSetData( ObjectList[0].Geom, &ObjectList[0].SurfaceDesc );
	dGeomSetBody (ObjectList[0].Geom,ObjectList[0].Body);
	dGeomGroupAdd( GeomGroup, ObjectList[0].Geom );


	
	//create the mesh for drawing
	D3DXCreateBox( Screen.D3DDevice, ChassisWidth, ChassisHeight, ChassisLength,  &DrawMesh, NULL );
	D3DXCreateSphere( Screen.D3DDevice, WheelRadius, 10, 10, &d3dxWheelMesh, NULL );
	D3DXCreateSphere( Screen.D3DDevice, DriveWheelRadius, 10, 10, &d3dxDriveWheelMesh, NULL );


	GetIniValue( descriptionFile, "tank", "ChassisMesh", Buffer);
	ChassisMesh = MM.GetMeshHandle( Buffer );
//	GetIniValue( descriptionFile, "vehicle4wheeled", "WheelMesh", Buffer);
//	WheelMesh = MM.GetMeshHandle (  Buffer );
    	

	return true;
}

void TankConstruct::Turn( Real Force, Real Angle )
{
	
}

void TankConstruct::Accelerate ( Real Force, Real SideForce, Real UpForce  )
{
//	PedalForce = Force;
	Throttle    = Force;
	LeftClutch  = false;
	RightClutch = false;

	TurnForce = SideForce;
	/*if(SideForce < 0)
		LeftClutch = true;
	if(SideForce > 0)
		RightClutch = true;*/
}


void TankConstruct::GetMatrix( Matrix* M )
{
	if(!M)
		return;

	mySolver->GetTransform( ObjectList[4].Geom, M );

}

void TankConstruct::Update( Real FrameTime, WorldManager* SM )
{

	//figure out the current speed
	const dReal* velocityVector = dBodyGetLinearVel  ( ObjectList[0].Body );
	CurrentSpeed = sqrt( velocityVector[0]*velocityVector[0] + 
						 velocityVector[1]*velocityVector[1] +
						 velocityVector[2]*velocityVector[2] );

	Matrix RotationMatrix;
	D3DXVECTOR3 v;
	v.x = 0;
	v.y = 0;
	v.z = 1;
    
	mySolver->GetRotationMatrix( ObjectList[0].Geom, &RotationMatrix );

	//rotate the vector
	D3DXVECTOR4 vfinal;
	D3DXVec3Transform( &vfinal, &v, &RotationMatrix.Mat );
	v.x = vfinal.x * Throttle;
	v.y = vfinal.y * Throttle;
	v.z = vfinal.z * Throttle;



	dBodyAddTorque( ObjectList[0].Body, 0, TurnForce*15, 0 );
	//dBodySetAngularVel( ObjectList[0].Body, 0, RADIAN(TurnForce), 0 );
	dBodyAddForce( ObjectList[0].Body, v.x, v.y, v.z );
	
	ObjectList[0].SurfaceDesc.ContactSlip1 = 0.9f;
	ObjectList[0].SurfaceDesc.ContactSlip2 = 0.9f;

	

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

	
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	Screen.D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	
	Screen.D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	
	DrawMesh->DrawSubset(0);
	Mesh* pChassisMesh = pMM->Get( ChassisMesh );
	Mesh* pWheelMesh;

	if(pChassisMesh)
		pChassisMesh->Draw(Screen );


	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
}
