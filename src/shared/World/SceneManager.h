// I N C L U D E S ///////////////////////////////////
#pragma once
#include <map> 
#include "..\s3dlib\types.h"
#include "..\collision\collision.h"
#include "..\collision\kdtree.h"

//Forward declarations
class WorldManager;
class SceneObject;
class SphereNode;


// D E F I N E S ////////////////////////////////////

#define SceneHandle int

#define SceneObjectMap std::map<int, SceneObject*>
#define SceneManagerIterator SceneObjectMap::iterator

// C L A S S E S ////////////////////////////////////

//Manages objects in the scene. 
class SceneManager
{
protected:
	
	//pointer to world manager
	WorldManager* WMPtr;

	//Stuff for object collisions
	KDTree kdtree;
	
	//ID stuff
	int currentID;
	int GenerateID();


	SphereNode* root;
	int MaxTreeDepth;

	//Objects this class is keeping track of
	std::map< int, SceneObject*> objectList; 


public:

	//Constructor/destructor
	SceneManager( WorldManager* wm );
	~SceneManager();

	void BuildCollisionTree();
	//Object management functions
	SceneHandle AddObject( SceneObject* o );
	SceneObject* RemoveObject( SceneHandle s );
	SceneObject* RemoveObjectByPtr( SceneObject* o );

	//Iterator stuff
	SceneManagerIterator Start();
	SceneManagerIterator End();

	int GetMaxDepth() { return MaxTreeDepth; }

	WorldManager* GetWMPtr() { return WMPtr; }


	KDTree* GetKDTree () { return &kdtree; }
    	

	SceneObject* GetObjectPtr(SceneHandle h) { return objectList[h]; }
	SceneHandle GetObjectByScriptName( const char* name );

	SceneObject* MakeObject( const char* type );

	//find the object closest to a location
	SceneObject* FindNearestObject( Vector3D* Location, float MaxDistance, SceneObject* objectToIgnore=NULL );

	//tree stuff
	void BuildTree();

	
	bool CastRay( Vector3D& a, Vector3D& b, Vector3D* outPos, Vector3D* outNormal, SceneObject** outObject )
	{
		 return kdtree.CastRay ( a, b, outPos, outNormal, outObject );
	}
	

	//Collision stuff
	bool CheckForCollision( Vector3D* p1, Vector3D* p2, CollisionInfo* Collision, bool CheckPlayer=true, bool LightCheck=false );

	bool BasicCheckForCollision( Vector3D& a,  Vector3D& b, Vector3D& hitPosition, Vector3D& normal, SceneObject** objectHit,  SceneObject* excludedObject=NULL);

	//update stuff?
	void UpdateAll( float frametime );

	//Stuff for drawing
	void DrawAll();

};