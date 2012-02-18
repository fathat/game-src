// I N C L U D E S /////////////////////////////////////
#pragma once
#include <vector>
#include "..\s3dlib\types.h"

// D E F I N I T I O N S //////////////////////////////

//forward declarations
class SceneManager;
class CollisionInfo;

//namespace directive
using namespace std;

//type definitions
#define SceneHandle int


class SphereNode
{


	Vector3D location;
	float radius;


	vector <int> objectList;

	//child nodes
	SphereNode* left; 
	SphereNode* right;

public:
	SphereNode()
	{
		left = NULL;
		right  = NULL;
	}

	SphereNode( vector<int>* objects )
	{
		left  = NULL;
		right = NULL;

		objectList = *objects;
	}
	void SetupAsRoot( SceneManager* scene );

	void AddObject( SceneHandle handle )
	{
		objectList.push_back( handle );
	}

	void CalculateExtents( SceneManager* scene );

	bool Collide( SceneManager* scene, Vector3D* l1, Vector3D* l2, CollisionInfo* c );

	void Build( SceneManager* scene, SphereNode* parent, bool splitZ, int currentDepth );

};