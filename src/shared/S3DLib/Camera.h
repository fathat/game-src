// I N C L U D E S ///////////////////////////////////////////////
#pragma once
#include "types.h"


// D E F I N E S /////////////////////////////////////////////////

// Camera states
enum CameraState { CS_FOLLOW, CS_FREE, CS_FIRST_PERSON, CS_FOLLOW_MATRIX };


// C L A S S E S /////////////////////////////////////////////////

//////// Camera ///////////////////////////////
//
// Description:
//
// Represents a camera in 3D space that can
// look at objects, etc.
//
//
///////////////////////////////////////////////

struct Camera 
{
	//how far away the camera is from the focal point.
	Real Zoom;

	//the focal point of the camera
	Position *Pos;
	Matrix* FollowMat;
	Matrix* FollowMatSky; 
	

	//what the camera is being used as right now (ie, a free floating camera,
	//third person camera, first person camera, etc.)
	CameraState CS;

	//Variables describing the camera.
	Real Yaw, Pitch, Roll;
	Real OffsetX, OffsetY, OffsetZ;

	Real HeightDisplacement; //used for "head-bob" effects, mostly
	Real MaxHeightDisplacement;

	Real YawDrift, PitchDrift, RollDrift;
	Real XDrift, YDrift, ZDrift;
	Real ZoomDrift;

	Real FOV;
	Real FOVDrift;

	//Constructor
	Camera();

	//Functions for the camera.
	Matrix GetViewMatrix( ); // gets a first person view matrix
	Matrix GetRotationMatrix();
	Matrix GetLookAtMatrix( Real Distance ); //gets a third person matrix 
											 //(or first, if distance is 0)

	//functions for setting where the camera is.
	void StopFollow(){Pos = NULL;};
	void Follow( Position& Location){Pos = &Location;};
	void Follow( Matrix& M, Matrix& R ) { FollowMat = &M; FollowMatSky = &R; }

	void Update( Real FrameTime );
};
