#include "BoxConstruct.h"
#include "..\ini\ini.h"

#include "..\defs\defs.h"


//////// GetTypeString ////////////////////////
//
// Description:
//
// Returns the type of construction that this is.
//
///////////////////////////////////////////////

std::string BoxConstruction::GetTypeString()
{
	return "BoxConstruction";
}

//////// Construct ////////////////////////////
//
// Description:
//
// Initializes the construction.
//
///////////////////////////////////////////////

bool BoxConstruction::Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen, MeshManager& MM, Position& Location, ICollisionHandler* collisionHandler )
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
	ZeroMemory(INIBuffer, 256 );


	//Load attributes
	Width  = GetIniValueFloat( descriptionFile, "box", "width", INIBuffer );
	Height = GetIniValueFloat( descriptionFile, "box", "height", INIBuffer );
	Length = GetIniValueFloat( descriptionFile, "box", "length", INIBuffer );

	//Load
	GetIniValue( descriptionFile, "box", "mesh", INIBuffer );
    hMesh = MM.GetMeshHandle( INIBuffer, true );


	//Create the geom group
	GeomGroup = dSimpleSpaceCreate (solver->GetSpaceID(false));  //dCreateGeomGroup (solver->GetSpaceID(false));  
	Location.y+= Height*.5f;

	//Create the actual body ( a sphere! )
	ObjectList[0].CreateBody( solver );
	dBodySetPosition ( ObjectList[0].Body, Location.x, Location.y, Location.z);
	dMassSetBox (&ObjectList[0].Mass, 1.0f,  Width, Height, Length);
	dMassAdjust (&ObjectList[0].Mass,  1.0f );
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
		dBodyDisable( ObjectList[i].Body );

		
		dBodySetAutoDisableFlag ( ObjectList[i].Body, 1 );
		dBodySetAutoDisableDefaults ( ObjectList[i].Body);
		dBodySetAutoDisableTime ( ObjectList[i].Body, 0.01f);
		dBodySetAutoDisableSteps ( ObjectList[i].Body, 0);
		dBodySetAutoDisableLinearThreshold (ObjectList[i].Body, 1.0f);
		dBodySetAutoDisableAngularThreshold( ObjectList[i].Body, 1.0f);
		//ObjectList[i].Setup = true;
		//ObjectList[i].HasGeom = true;
	}

	//set up disable epsilons (minimum speed of object before it gets disabled
	LinearDisableEpsilon = .1;
	AngularDisableEpsilon = .01f;

	return true;
}

//////// CheckForParticleCollision ////////////
//
// Description:
//
// Checks for particle collision and adds forces and stuff
//
///////////////////////////////////////////////

bool BoxConstruction::CheckForParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager* MM )
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
		Activate();

		//rotate our vector so we apply force in proper direction
		//apply force
		Vector3D v = p2-p1;
		v.Normalize();
		dBodyAddForceAtPos( ObjectList[0].Body, (v.x)*c->Mass*.25f , (v.y)*c->Mass*.25f, (v.z)*c->Mass*.25f, p1.x, p1.y, p1.z );
		return true;
	}

	return false;
}
//////// Draw /////////////////////////////////
//
// Description:
//
// Draws the box
//
///////////////////////////////////////////////
void BoxConstruction::Draw(Screen3D& Screen)
{
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

	Mesh* pBoxMesh = pMM->Get( hMesh );
	if(pBoxMesh)
		pBoxMesh->Draw( Screen );

}