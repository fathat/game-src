// I N C L U D E S ////////////////////////
#pragma once
#include <vector>
#include "..\s3dlib\VertexBuffer.h"
#include "..\mesh\meshmanager.h"
#include "staticobject.h"
#include "ImposterManager.h"
// D E F I N E S //////////////////////////
class WorldManager; // hooray forward defintions
class VegConstruction;

// C L A S S E S //////////////////////////
struct Vegetation
{
	bool Collide;
	MeshHandle hMesh;
	MeshHandle hCollisionMesh;
	Position Location;
	ImposterHandle hImposter;
	StructureHandle hStructure;
	VegConstruction* pConstruct;
	ConstructionHandle hConstruct;


	bool CheckForCollision( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager& MM ) ;

	Vegetation() {
		Collide = false;
		pConstruct = NULL;
	}


};

class VegetationManager
{
protected:
	WorldManager* WMPtr;
	Screen3D*  ScreenPtr;

	ImposterManager IM;

	int TotalPlantCount;

	std::vector<Vegetation> VegList;

public:

	VegetationManager();

	void GenerateVegetation( char* vegetationdesc, WorldManager* WM );

	bool CheckForCollision( Vector3D& p1, Vector3D& p2, CollisionInfo* Collision);

	int  GetTotalPlantCount ( ) { return TotalPlantCount; }

	void DrawAll( Screen3D& Screen, MeshManager& MM, Camera* Viewer);


};