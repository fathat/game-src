//I N C L U D E S ////////////////////////////
#pragma once
#include "..\s3dlib\screen.h"
#include "..\s3dlib\camera.h"
#include "..\mesh\meshmanager.h"
#include "imposter.h"
#include <vector>

// D E F I N E S /////////////////////////////
#define ImposterHandle int
#define MAX_IMPOSTER_TYPES 512


// C L A S S E S ////////////////////////////




//////// ImposterManager ///////////////////////
//
// Description:
//
// Manages ImposterTypes so there aren't duplicate
// copies in each object that uses imposters.
//
// Inherits From:
//
// - Nothing
//
// TODO:
//
// - MAKE STUFF
//
// Known Bugs:
//
// - 
//
///////////////////////////////////////////////


class ImposterManager
{
protected:

	//stuff
	Screen3D* ScreenPtr;
	MeshManager* MMPtr;

	ImposterType* ImposterTypeList[MAX_IMPOSTER_TYPES];
	Real AngleIncrement;
	
	ImposterHandle AddImposter( MeshHandle hBaseMesh );
	void RemoveImposter ( ImposterHandle hImposter );
	int Exists( MeshHandle hImposterMesh );

public:
	ImposterManager();
	~ImposterManager();

	bool Init( Screen3D& Screen, MeshManager& MM,  Real angleIncrement );
	void DestroyAll();


	ImposterHandle GetImposterHandle( MeshHandle hBaseMesh );
	ImposterType* GetImposter( ImposterHandle h );

	void DrawAll(Screen3D& Screen, MeshManager& MM);
};