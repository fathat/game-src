// I N C L U D E S ///////////////////////////////////
#pragma once
#include <vector>
#include "..\s3dlib\types.h"
#include "..\s3dlib\screen.h"
#include "..\s3dlib\vertexbuffer.h"
#include "SceneObject.h"
#include "..\collision\collision.h"

// D E F I N E S /////////////////////////////////////
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define BRANCH_POINTS 5
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

	Leaf LeafList[LEAVES_PER_BRANCH];
	bool ShowLeafs;

	Branch* Parent;
	Branch* ChildBranch[2];

	Branch() { 
		Point.x = -1; Point.y = -1; Point.z = -1; 
		ChildBranch[0] = 0; ChildBranch[1] = 0; 
		Parent = NULL;
		VBPosition = 0;

	}

	~Branch() {
		if(ChildBranch[0])
			delete ChildBranch[0];
		if(ChildBranch[1])
			delete ChildBranch[1];
	}
	
	void BuildPoints( Vector3D& Start, Vector3D& Dir,  ProceduralTree* MyTree  );
	void DeleteChildren() {
		if(ChildBranch[0])
			delete ChildBranch[0];
		if(ChildBranch[1])
			delete ChildBranch[1];
		ChildBranch[0] = NULL;
		ChildBranch[1] = NULL;
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

	friend class Branch;
	friend class Leaf;
	

	//Root branch of the tree
	Branch Root;

	int CurrentDepth;
	int MaxDepth;


    // Temporary Vertex and Index buffers
	std::vector<CustomVertex> BranchVertexBuffer;
	std::vector<int>      BranchIndexBuffer;
	std::vector<CustomVertex> LeafVertexBuffer;
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

	//Properties of the tree
	int Twist; //How much a branch segment can twist (degrees)
	int PitchMinA;
	int PitchMaxA;
	int PitchMinB; //minimum amount of pitch, usually a negative # at 
	int PitchMaxB; //maximum amount of pitch, usually a positive #
	int YawRange; //Amount of branch yaw allowed
	Real DepthEffect; //How much effect depth has. Default 1.
	Real GravityA;
	Real GravityB;
	Real BranchSize; 
	Real ShrinkRate; //How quickly the branch shrinks. Range of Zero to One.
	Real BranchThickness; //How thick the branch is O_O
	Real BranchLeafThreshold; //How thin the branch needs to be before leaves appear
	Real LeafSize;

	//Statistics O_O
	int LeafPolygons;
	int BranchPolygons;

	bool BuildTree( Branch* Parent, int branch );
	void DrawAll ( Screen3D& Screen );

public:

	ProceduralTree();

	void Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile);
	void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );


	void AdjustBoundingBox();
	bool CheckVisibility( Screen3D& Screen, bool RecordTime=true );
	bool LightCollision ( Vector3D& point1, Vector3D& point2, CollisionInfo* Collision );
	bool Collision( Vector3D& point1, Vector3D& point2, CollisionInfo* Collision );

};