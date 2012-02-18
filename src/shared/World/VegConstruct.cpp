#include "VegConstruct.h"
#include "..\ini\ini.h"


#include "..\defs\defs.h"
//////// GetTypeString ////////////////////////
//
// Description:
//
// Returns the type of construction that this is.
//
///////////////////////////////////////////////

std::string VegConstruction::GetTypeString()
{
	return "VegConstruction";
}

//////// Construct ////////////////////////////
//
// Description:
//
// Initializes the construction.
//
///////////////////////////////////////////////

bool VegConstruction::Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen, MeshManager& MM, Position& Location, ICollisionHandler* collisionHandler )
{
	//deconstruct any old stuff
	Deconstruct();

	//save the solver pointer
	mySolver = solver;
	pMM = &MM;
	//create the body list
	ObjectList = new DynamicsObject[1];
	this->nObjects = 1;

	//get values from ini file
	char INIBuffer[256];

	Width  = GetIniValueFloat( descriptionFile, "vegconstruct", "width", INIBuffer );
	Height = GetIniValueFloat( descriptionFile, "vegconstruct", "height", INIBuffer );
	Length = GetIniValueFloat( descriptionFile, "vegconstruct", "length", INIBuffer );

	GetIniValueFloat( descriptionFile, "vegconstruct", "mesh", INIBuffer );
    hMesh = MM.GetMeshHandle( INIBuffer, true );


	//Create the geom group
	GeomGroup = dSimpleSpaceCreate (solver->GetSpaceID(false));  //dCreateGeomGroup (solver->GetSpaceID(false));  
	
	Untouched=true;

	Location.y+=1;
	//Create the actual body ( a box! A cylinder would be better, but
	//ODE is a bitch and tricollider won't work with it)
	ObjectList[0].CreateBody( solver );
	dBodySetPosition ( ObjectList[0].Body, Location.x, Location.y+Height*.5f, Location.z);
	dMassSetBox (&ObjectList[0].Mass, 1.0f,  Width, Height, Length);
	dMassAdjust (&ObjectList[0].Mass,4.5f );
	dMassTranslate(&ObjectList[0].Mass, 0, -Height*.5f, 0 );
	dBodySetMass( ObjectList[0].Body, &ObjectList[0].Mass);
	ObjectList[0].Geom = dCreateBox (0,Width, Height,Length );
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
		ObjectList[i].HasGeom = true;
	}

	//Set the joint that holds it into the ground
	nJoints=1;
	JointList = new dJointID[nJoints];
	

	//create a universal joint
	JointList[0] = dJointCreateUniversal (solver->GetWorldID(),0);

	//attach it to the world
	dJointAttach (JointList[0],ObjectList[0].Body, 0 );

	//set some axis and such
	const dReal *a = dBodyGetPosition (ObjectList[0].Body);
	dJointSetUniversalAnchor (JointList[0],a[0],a[1]-Height*.5f,a[2]);
	dJointSetUniversalAxis1  (JointList[0],0,0,1);
	dJointSetUniversalAxis2  (JointList[0],1,0,0);
		
	
	//dJointSetBreakable( JointList[0], 0 );
	/*dJointSetBreakMode( JointList[0], dJOINT_BREAK_AT_B1_FORCE );
	dJointSetBreakForce (JointList[0], 0, 1500, 5000, 1500);
	dJointSetBreakTorque (JointList[0], 0, 10000, 10000, 10000);*/
	

	
	dBodyDisable( ObjectList[0].Body );



	//set up disable epsilons (minimum speed of object before it gets disabled
	LinearDisableEpsilon = .1;
	AngularDisableEpsilon = .01f;

	D3DXCreateBox( Screen.D3DDevice, Width, Height, Length, &DrawMesh, NULL  );

	return true;
}

//////// CheckForParticleCollision ////////////
//
// Description:
//
// Checks for particle collision and adds forces and stuff
//
///////////////////////////////////////////////

bool VegConstruction::CheckForParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager* MM )
{
	Vector3D line[2];
	line[0] = p1;
	line[1] = p2;

	//check the chassis
	Matrix M;
	Mesh* pMesh = MM->Get( hMesh );
	mySolver->GetTransform( ObjectList[0].Body, &M );


	if ( pMesh->CheckForCollision( M, line, c ) )
	{
		dBodyEnable( ObjectList[0].Body );

		//rotate our vector so we apply force in proper direction
		//apply force
		Vector3D v = p2-p1;
		v.Normalize();
		dBodyAddForceAtPos( ObjectList[0].Body, (v.x)*c->Mass*.25f , (v.y)*c->Mass*.25f, (v.z)*c->Mass*.25f, p1.x, p1.y, p1.z );
		return true;
	}

	return false;
}

//////// Update ///////////////////////////////
//
// Description:
//
// Updates the objects motors and such and such
//
///////////////////////////////////////////////


void VegConstruction::Update( Real FrameTime, WorldManager* WMPtr )
{
	
	/*dReal v = 0 - dJointGetHingeAngle (JointList[0]);
	if (v > 0.1) v = 0.1;
	if (v < -0.1) v = -0.1;
	v *= 40.0;
	dJointSetHingeParam (JointList[0],dParamVel,v);
	dJointSetHingeParam (JointList[0],dParamFMax,5000);
	dJointSetHingeParam (JointList[0],dParamLoStop,-0.25);
	dJointSetHingeParam (JointList[0],dParamHiStop,0.25);
	dJointSetHingeParam (JointList[0],dParamFudgeFactor,0.1);*/


/*	D3DXQUATERNION q, q2, qf;
	D3DXQuaternionRotationYawPitchRoll( &q, 0, RADIAN(180), 0);
	dQuaternion dq;
	
	const dReal* dx = dBodyGetQuaternion ( ObjectList[0].Body );
	q2.x = dx[0];
	q2.y = dx[1];
	q2.z = dx[2];
	q2.w = dx[3];

	D3DXQuaternionSlerp( &qf, &q, &q2, FrameTime*.5f );



	dq[0] = qf.x;
	dq[1] = qf.y;
	dq[2] = qf.z;
	dq[3] = qf.w;
	dBodySetQuaternion ( ObjectList[0].Body, dq );*/

	//const dReal* drot = dBodyGetAngularVel( ObjectList[0].Body );
	

	if( dJointGetBody (JointList[0], 0) != NULL )
	{
		Matrix M;
		Vector3D forcevector;
		D3DXVECTOR3 TreeVector(0, Height, 0);
		D3DXVECTOR4 FinalVector;

		mySolver->GetRotationMatrix( ObjectList[0].Body, &M );
		D3DXVec3Transform( &FinalVector, &TreeVector, &M.Mat );

		if(FinalVector.y > 0.85f * Height )
		{
    		dBodyAddForceAtRelPos( ObjectList[0].Body, 0, 250 + (Height-FinalVector.y)*1.5f ,0 , 0,Height, 0 );
			
		}
		else
		{
			//dJointAttach( JointList[0], 0, 0 );
		}

		if(!IsActive())
		{
			Untouched=true;
		}
	}
	else
	{
		Untouched = false;
	}

}


//////// Draw /////////////////////////////////
//
// Description:
//
// Draws the box 
//
///////////////////////////////////////////////
void VegConstruction::Draw(Screen3D& Screen)
{
	
	return;
    //declarations
	Matrix M;

	//draw the box
	mySolver->GetTransform( ObjectList[0].Geom, &M );
	Screen.SetTransform(M);
	Screen.SetTexture(-1, 0);
	Screen.SetTexture(-1, 1);
	Screen.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE );
	Screen.SetRenderState(D3DRS_ALPHAREF, 230 );
	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE );
	Screen.SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	Screen.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	Screen.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );

	//Mesh* pBoxMesh = pMM->Get( hMesh );
	//if(pBoxMesh)
	//	pBoxMesh->Draw( Screen );


	Screen.SetTexture(-1, 0);
	if(DrawMesh)
		DrawMesh->DrawSubset(0);

}