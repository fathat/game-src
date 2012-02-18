//AnimatedMesh.H
#pragma once

// I N C L U D E S /////////////////////////////////////////////////////
#include <list>
#include "mesh.h"
#include "MeshManager.h"
#include "..\Collision\Collision.h"

// D E F I N E S ///////////////////////////////////////////////////////
#define MAX_ANIMATIONS 24

// C L A S S E S ///////////////////////////////////////////////////////


//////// Animation ////////////////////////////
//
// Description:
//
// Contains meshes which are cycled and 
// interpolated between to form an animation of 
// some sort.
//
// Known Problems:
//
// - None
//
// TODO:
//
// - 
//
//
///////////////////////////////////////////////

struct AnimationState
{
	// the name of the state
	std::string Name;
	std::string FileName;
	
	// mesh this state represents
	MeshHandle hMesh;
	
	//Recommended next state!
	std::string RecommendedNextState;

	//How long this state lasts
	Real TransitionTime;
	
    AnimationState() { hMesh = NULL; }
	~AnimationState() { }

	void InitFromFile ( MeshManager& MM, char* name, char* file, char* recommendednextstate  );
	
};



//////// AnimatedMesh /////////////////////////
//
// Description:
//
// Contains multiple states, which we move
// between to create animations. And stuff!
//
// Known Problems:
//
// - None
//
// TODO:
//
// - Make it load the recommended next state 
//   somehow. The best way would probably
//   just be a seperate section in the ini
//   file. 
//
//
///////////////////////////////////////////////


class AnimatedMesh
{
protected:

	// Array which contains pointers to animations.
	std::vector< AnimationState > StateList;


	// Pointers to the animation which is currently running
	// and the animation that is to be run next (NULL if we
	// just want to repeat the current one). Also, Transition
	// is how close we are to switching to NextAnimation.
	int CurrentState;
	int NextState;
	std::string NextStateName; //if this is "none" then we use the recommended one 
							   //from the current state
	Real Transition;  
	Real TransitionTime; //Seconds
	Real TimeMultiplier;

	// Stored at Init for later use
	Screen3D* ScreenPtr;
	MeshManager* MMPtr;

public:
	AnimatedMesh()  { ScreenPtr = NULL; MMPtr = NULL; CurrentState = 0; NextState=0; Transition=0; TransitionTime = 1; TimeMultiplier = 1.0f;}
	~AnimatedMesh() { Cleanup(); }
	
	// Inits/Destroys the mesh
	bool Init( Screen3D& Screen, MeshManager& MM, char* descriptionfile );
	void Cleanup();


	// Use these to add and remove meshes
	// from the animation list.
	void AddState ( char* Name, char* MeshFile, char* NextState, Real time );
	void RemoveState ( char* Name);
	int  FindStateIndex( char* StateName );

	//Sets the next animation.
	void SetTimeMultiplier( Real timeMult )
	{
		TimeMultiplier = timeMult;
	}

	Real GetTimeMultiplier ( )
	{
		return TimeMultiplier;
	}


	void Change ( char* Name, Real Time ) { 
		NextStateName = Name; 
		NextState = FindStateIndex(Name);
		if(NextState == -1)
			NextState = 0;
		if(Time != -1)
		{
			TransitionTime = Time;
			Transition = 0;
		}
	}
	
	bool CheckForCollision( Position& MeshLocation, Vector3D* Line, CollisionInfo* c);
	bool CheckVisibility( Screen3D& Screen, Position& MeshLocation);
	AABB GetBox();
	Real GetSphereRadius();


	// update the mesh's current state. This
	// should be called every frame.
	void Update ( Real FrameTime );

	// draw the mesh.
	void Draw( Screen3D& Screen, Matrix& Location);

};