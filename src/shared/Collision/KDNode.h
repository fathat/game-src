// I N C L U D E S //////////////////////////////////////////////
#pragma once
#include <vector>
#include "..\s3dlib\types.h"

// D E F I N I T I O N S ///////////////////////////////////////

//forward declarations
class SceneObject;
class KDTree;

// C L A S S E S ///////////////////////////////////////////////

//A container to hold objects. The reason for this classes existance
//is that there's additional data that needs to be kept track of for
//each object, but it's better not to place it in the object class 
//itself.
struct KDObjectContainer
{
	SceneObject* object;
	bool checked;

	KDObjectContainer() { object= NULL; checked = false; } 
	KDObjectContainer( SceneObject* anObject ) { object = anObject; checked = false; }

	bool Checked() { return false; }
	void SetChecked() { checked = true; }
	void ClearChecked() { checked = false; }
};


//A node in the KDTree.
struct KDNode
{
	//Objects that are inside this node. Note that
	//only leafs hold objects.
	std::vector< KDObjectContainer* > objectList;
	Plane3D splitPlane;
	AABB boundingBox;

	//parents
	KDTree* kdTree;
	KDNode* parent;

	//children
	KDNode* front;
	KDNode* back;

	KDNode( KDTree* myTree, KDNode* theParent ) { kdTree = myTree ; parent = theParent; front = NULL; back = NULL; }
	~KDNode() ;


	//Builds the tree recursively
	void BuildTree(  KDTree* tree, int currentDepth, AABB bb, int axis );

	//Recurses the tree and fills a list with the objects that might potentially collide with the provided ray
	void GetPotentialColliders( Vector3D& a, Vector3D& b,  std::vector< KDObjectContainer*>& colliderList );

	//recurses the tree and adds the object to appropriate subnodes
	void AddObject( KDObjectContainer* container );
	

};