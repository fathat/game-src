// I N C L U D E S /////////////////////////////////////
#pragma once

#include <map>
#include <string>

#include "KDNode.h"
#include "Collision.h"

// D E F I N E S ///////////////////////////////////////



// C L A S S E S ///////////////////////////////////////


//A KDTree used for collision detections on objects. 
//Important note: KDTree's depth must be an even number.
//IE: 2, 4, 6 are all OK, but 1, 3, etc. are not. This
//is so that Leaf nodes will always end up being square,
//which greatly simplified object caching. 
class KDTree
{
protected:

	//Root node
	KDNode* root;

	int maxDepth;

	//All the objects in this tree. 
	//std::vector<KDObjectContainer> objects;
	std::map< SceneObject*, KDObjectContainer* > objects;

	
	//Resets all objects to "not checked"
	void ClearCheckedFlag();

public:

	//Constructor/Destructor 
	KDTree()  { root = NULL; }
	~KDTree() { Cleanup(); }

	//Initializes the KDTree
	void Init( float width, float depth, int aMaxDepth  );

	//Deletes all the resources the KDTree is using
	void Cleanup();

	int MaxDepth() { return maxDepth; }

	//Adds an object to the KD tree
	void Add( SceneObject* sceneObject );

	//Removes an object from the KD tree
	void Remove( SceneObject* object); 

	int NumberOfObjects() { return objects.size(); }


	//Figures out the node in which the specified object is in.
	//This is necessary if the object has moved. 
	void DetermineObjectLocation( SceneObject* object );


	//Finds the closest object that intersects the line segment defined by ab. Returns false if nothing found.
	bool CastRay( Vector3D& a, Vector3D& b, Vector3D* outPos, Vector3D* outNormal, SceneObject** outObject );

};