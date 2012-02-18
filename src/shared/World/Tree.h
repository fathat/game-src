// I N C L U D E S ///////////////////////////////////
#pragma once
#include <vector>
#include "..\s3dlib\types.h"
#include "..\s3dlib\screen.h"
#include "..\s3dlib\vertexbuffer.h"
#include "SceneObject.h"
#include "..\collision\collision.h"
#include "..\physics\solver.h"
#include "..\physics\construction.h"

// D E F I N E S /////////////////////////////////////
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define BRANCH_POINTS 6
#define LEAVES_PER_BRANCH 4
class ProceduralTree;



// C L A S S E S /////////////////////////////////////


struct Leaf
{
	Real x, y, z;
	Vector3D LeafVertex[4];
};


struct Branch
{
	Vector3D Point;
	Vector3D Normal;
	int BranchHeritage; //whether it's on left or right of tree (0 left, 1 for right)
	int Depth;
	int VBPosition; //where this branch's triangles left off at

	Vector3D VertexList[BRANCH_POINTS];
	Matrix Rotation;
	D3DXQUATERNION qRotation;

	Leaf LeafList[LEAVES_PER_BRANCH];
	bool ShowLeafs;

	Branch* Parent;
	Branch* ChildBranch[3];

	Branch() { 
		Point.x = -1; Point.y = -1; Point.z = -1; 
		ChildBranch[0] = 0; ChildBranch[1] = 0; ChildBranch[2] = 0;
		Parent = NULL;
		VBPosition = 0;
	}

	~Branch() {
		if(ChildBranch[0])
			delete ChildBranch[0];
		if(ChildBranch[1])
			delete ChildBranch[1];
		if(ChildBranch[2])
			delete ChildBranch[2];
	}
	
	void BuildPoints( Vector3D& Start, Vector3D& Dir,  ProceduralTree* MyTree  );
	void DeleteChildren() {
		if(ChildBranch[0])
			delete ChildBranch[0];
		if(ChildBranch[1])
			delete ChildBranch[1];
		if(ChildBranch[2])
			delete ChildBranch[2];
		ChildBranch[0] = NULL;
		ChildBranch[1] = NULL;
		ChildBranch[2] = NULL;
	}
};




//////// Tree /////////////////////////////////
//
// Description:
//
// A procedurally generated tree.
//
// Known Problems:
//
// - None
//
// TODO:
//
//
///////////////////////////////////////////////

class ProceduralTree : public SceneObject
{
public:
	
	friend class  Branch;
	friend struct Leaf;
	
	//Root branch of the tree
	Branch Root;

	int CurrentDepth;
	int MaxDepth;

	
	// Temporary Vertex and Index buffers
	std::vector<TreeVertex> BranchVertexBuffer;
	std::vector<int>      BranchIndexBuffer;
	std::vector<TreeVertex> LeafVertexBuffer;
	std::vector<int>      LeafIndexBuffer;

	//The final results of our initialization
	TextureHandle LeafTexture, BarkTexture;
	VertexBuffer TreeVB;
	IndexBuffer  TreeIB;
	VertexBuffer LeafVB;
	IndexBuffer  LeafIB;
	Screen3D* ScreenPtr;
	
	int nLeafs;
	
	AABB BaseBox;
	BoundingSphere BaseSphere;

	CollisionMesh ColMesh;
	CollisionMesh FullColMesh;

	//Stuffage for physics engine
	StructureHandle hTreeStructure;


	//Properties of the tree
	int Twist; //How much a branch segment can twist (degrees)
	int PitchMin;
	int PitchMax;
	int YawRange; //Amount of branch yaw allowed
	Real DepthEffect; //How much effect depth has. Default 1.
	Real Gravity;
	Real BranchSizeMin; 
	Real BranchSizeMax;
	Real ShrinkRate; //How quickly the branch shrinks. Range of Zero to One.
	Real BranchThickness; //How thick the branch is
	Real BranchLeafThreshold; //How thin the branch needs to be before leaves appear
	Real LeafSize;

	//Statistics
	int LeafPolygons;
	int BranchPolygons;

	bool BuildTree( Branch* Parent, int branch );

	void DrawAll ( Screen3D& Screen );
public:

	ProceduralTree();

	void Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile);
	void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );


	void AdjustBoundingVolume();
	bool CheckVisibility( Screen3D& Screen, bool RecordTime=true );
	bool LightCollision ( Vector3D& point1, Vector3D& point2, CollisionInfo* Collision );
	bool Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision );
};