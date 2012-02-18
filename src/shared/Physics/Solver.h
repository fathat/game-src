// I N C L U D E S ///////////////////////////////
#pragma once
#include <ode\ode.h>
#include "..\S3DLib\types.h"

// D E F I N I T I O N S ////////////////////////
#define ConstructionHandle int
#define StructureHandle int


#define CONSTRUCTION_CACHE_SIZE 2048

#define MAX_CONSTRUCTIONS 2048
#define MAX_STRUCTURES 8192
// C L A S S E S ////////////////////////////////
class Construction;
class Structure;
class WorldManager;
class MeshManager;
struct CollisionInfo;
class SceneObject;


//it solves! but what is solved? it is mystery!!
class DynamicsSolver
{
public:
	dWorldID WorldID;
	dSpaceID* StaticSpaceID;
	int* SpaceDist;
	dSpaceID DynamicSpaceID;
	dSpaceID ActiveSpaceID;


	int SpaceDim;
	Real SpaceWidth;
	Real SpaceHeight; 

	dJointGroupID ContactGroup;

	dReal Gravity;

	bool IsSetup;

	Construction* ConstructionList[MAX_CONSTRUCTIONS];
	int nConstructions;
	Structure* StructureList[MAX_STRUCTURES];
	int nStructures;

	Construction* ActiveConstructionCache[CONSTRUCTION_CACHE_SIZE];
	int nCachedConstructions;

	
	friend static void nearCallback (void *data, dGeomID o1, dGeomID o2);
	friend class Construction;
	friend class Structure;
public:
	DynamicsSolver();
	~DynamicsSolver();
	bool Init( int spaceDim, Real spaceWidth, Real spaceHeight );
	void Shutdown();
	
	void GetTransform ( dGeomID geom, Matrix* M );
	void GetTransform ( dBodyID body, Matrix* M );
	void GetRotationMatrix( dGeomID geom, Matrix* M );
	void GetRotationMatrix( dBodyID geom, Matrix* M );
	dWorldID GetWorldID() { return WorldID; }
	dSpaceID GetSpaceID( bool Static, Real x=0, Real y=0, Real z=0);
	dSpaceID GetActiveSpaceID () { return ActiveSpaceID; }
	

	Construction* GetConstruction(ConstructionHandle h) ;
	Structure* GetStructure(StructureHandle h) ;

	ConstructionHandle RegisterConstruction( Construction* construction);
	StructureHandle    RegisterStructure( Structure* structure);
	void DestroyStructure( StructureHandle h );
	void DestroyConstruction( ConstructionHandle h );
	void DestroyAllObjects();

	void DisableInactiveConstructions();
	void DisableInactiveStructures();

	void DrawAll( Screen3D& Screen );

	bool CheckParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo * c, MeshManager* mmptr );

	void Step( float FrameTime, WorldManager* WM  );

	//stuff for active construction cache
	void CacheConstruction ( Construction* c );
	void GarbageCollectCache ();

	void CacheSpaceAddConstruction( Construction* c);
	void CacheSpaceRemoveConstruction( Construction* C );
};