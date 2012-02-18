#pragma once
// I N C L U D E S ///////////////////////////////////
#include <ode\ode.h>
#include "..\s3dlib\screen.h"

// C L A S S E S /////////////////////////////////////


//////// IMesh /////////////////////////////////
//
// Description:
//
// Mesh interface class
//
// Known Problems:
//
// - None
//
// TODO:
//
// - Re-add combining with other objects.
// - Fix = operator
//
///////////////////////////////////////////////

class IMesh
{
public:
	IMesh () { }
	virtual ~IMesh() { Destroy(); }

	//not implemented (yet)
	virtual void  operator = ( const Mesh& m ) ;

	//Create/Destroy
	virtual bool Create  ( char* MeshFile, Screen3D& Screen, bool Static = true ) ;
	virtual void Destroy ( );

	//accessors
	virtual char* GetFilename() { return NULL; }

		
	//Real functions
	virtual bool CheckVisibility( Screen3D& Screen, Position& MeshLocation);
	virtual void CombineWith( Mesh* mesh, Mesh* output, Real perc);
	virtual bool CheckForCollision( Position& MeshLocation, Vector3D* Line, CollisionInfo* c);
	virtual bool CheckForSphereCollision( Position& MeshLocation, Vector3D& point, Real radius, CollisionInfo* c);
	virtual void Draw ( Screen3D& Screen, Position* camera=NULL, Position* Location=NULL, bool IgnoreAlpha = false );


};