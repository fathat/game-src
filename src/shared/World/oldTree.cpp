#include "tree.h"
#include "..\ini\ini.h"
#include "world.h"

ProceduralTree::ProceduralTree()
{
	this->PitchMinA = -50;
	this->PitchMaxA = 50;
	this->PitchMinB = -75;
	this->PitchMaxB = 75;
	this->YawRange  = 75;
	this->Twist     = 5;
	this->GravityA  = .1;
	this->GravityB  = .15;
	this->DepthEffect = 1.0f;
	this->BranchSize  = 8;
	ShrinkRate        = .85f;
	BranchThickness   = .55f;
	BranchLeafThreshold = .2f;
	LeafSize          = 2.5f;

	nLeafs = 0;
}


void Branch::BuildPoints(Vector3D& Start, Vector3D& Dir,  ProceduralTree* MyTree )
{
	//declarations
	D3DXQUATERNION qR;
	D3DXVECTOR4 normal4;
	D3DXVECTOR4 v4[BRANCH_POINTS];
	D3DXMATRIX matR;
	D3DXMatrixIdentity(&matR);
	int rx;
	int ry;
	int rz;


	Rotation   = Parent->Rotation ;
	int i=0;

	//Initialize the something or other to some such thing
	Real t = MyTree->BranchThickness;
	t *= (Real)(MyTree->MaxDepth+1-MyTree->CurrentDepth ) / (Real)MyTree->MaxDepth;
	VertexList[0].x = -t;   VertexList[0].y =  0; VertexList[0].z =  t/2;
	VertexList[1].x =  0;   VertexList[1].y =  0; VertexList[1].z = t;
	VertexList[2].x =  t;   VertexList[2].y =  0; VertexList[2].z =  t/2;
	VertexList[3].x =  t/2; VertexList[3].y =  0; VertexList[3].z = -t;
	VertexList[4].x = -t/2; VertexList[4].y =  0; VertexList[4].z = -t;
	
	//Set the normals on these mothabeaches!
	Vector3D NormalList[BRANCH_POINTS];
	NormalList[0].x = -t;   NormalList[0].y =  0; NormalList[0].z =  t/2;
	NormalList[1].x =  0;   NormalList[1].y =  0; NormalList[1].z = t;
	NormalList[2].x =  t;   NormalList[2].y =  0; NormalList[2].z =  t/2;
	NormalList[3].x =  t/2; NormalList[3].y =  0; NormalList[3].z = -t;
	NormalList[4].x = -t/2; NormalList[4].y =  0; NormalList[4].z = -t;
		
	int n=0;
	for(n=0; n<BRANCH_POINTS; n++)
		NormalList[n].Normalize();
	

	//memcpy(VertexList, Parent->VertexList, sizeof(Vector3D)*4);


	//Figure out some ranges and such
	int PitchRangeA = MyTree->PitchMaxA - MyTree->PitchMinA;
	int PitchRangeB = MyTree->PitchMaxB - MyTree->PitchMinB;
	

	//Set us to where the parent is
	Point = Start;
	Normal = Dir;



	//OHNO! Root is the parent, so we have to CHEAT!!! KHhHH
	//Basically, since we want the tree to be balanced, and since
	//balancing it is easier at the root stage, we for the second
	//branch we just mirror the first one
	if(MyTree->CurrentDepth == 2 )
	{

		CustomVertex rootvertex;

		for(int i=0; i<BRANCH_POINTS; i++)
		{
			//store the position
			rootvertex.position = VertexList[i];
			
			//figure out the texture coordinates
			rootvertex.tu = i%3;
			rootvertex.tv = MyTree->BranchSize/10;
			//Figure out the vertex color
			rootvertex.normal.x = 0;
			rootvertex.normal.y = 0;
			rootvertex.normal.z = 0;
				
			MyTree->BranchVertexBuffer.push_back(rootvertex);
		}
	
		rx = 0;
		ry = 0;
		rz = 0;

	}

	// If it spawned off the root..
	else if(BranchHeritage == 0)
	{
		//..then we want to make this branch point more upwards
		rx = (rand()%PitchRangeA)+MyTree->PitchMinA ;
		//ry = -(rand()%360)-180;
		ry = -(rand()%MyTree->YawRange);
		rz = 0;
		Point.y -= MyTree->GravityA*Depth;

	}
	else
	{
		//otherwise it didn't spawn off the root, so
		//rotate the branch a bit randomely
		rx = (rand()%PitchRangeB)+MyTree->PitchMinB ;
		//ry = (rand()%MyTree->YawRange)-MyTree->YawRange/2;
		ry = (rand()%MyTree->YawRange);

		if(MyTree->Twist > 0)
			rz = (rand()%(MyTree->Twist ))-MyTree->Twist/2;
		else
			rz = 0;
		Point.y -= MyTree->GravityB*Depth;
		
	}

	//Create the leaves
	if( t <= MyTree->BranchLeafThreshold )
	{
		for(int l=0; l<LEAVES_PER_BRANCH; l++)
		{
			t = MyTree->LeafSize;
			t *= (MyTree->CurrentDepth/MyTree->MaxDepth);
			
			LeafList[l].LeafVertex[0].x = -t/2; LeafList[l].LeafVertex[0].y =  0; LeafList[l].LeafVertex[0].z =  0;
			LeafList[l].LeafVertex[1].x =  t/2; LeafList[l].LeafVertex[1].y =  0; LeafList[l].LeafVertex[1].z =  0;
			LeafList[l].LeafVertex[2].x =  0;   LeafList[l].LeafVertex[2].y =  0; LeafList[l].LeafVertex[2].z = -t;
			MyTree->nLeafs ++;
		}
	}


	//Figure out the angle that we want to rotate by
	Real DepthRotationModifier = ((Real)MyTree->CurrentDepth/(Real)MyTree->MaxDepth)*MyTree->DepthEffect;
	if(DepthRotationModifier > 1)
		DepthRotationModifier = 1;
	
	Real FixedRX = (Real)rx * DepthRotationModifier;
	Real FixedRY = (Real)ry * DepthRotationModifier;
	Real FixedRZ = (Real)rz * DepthRotationModifier;

	
	//Rotate the normal vector
	D3DXQuaternionRotationYawPitchRoll(&qR, RADIAN(FixedRY), RADIAN(FixedRX), RADIAN(FixedRZ));
	D3DXMatrixRotationQuaternion(&matR, &qR);
	
	//rotate the main normal
	D3DXVec3Transform( &normal4, (D3DXVECTOR3*)&Normal, &matR);

	//rotate.. TEH VERTEX NORMALS
	D3DXVECTOR4 nl;
	for(n=0; n<BRANCH_POINTS; n++)
	{
		D3DXVec3Transform( &nl, (D3DXVECTOR3*)&NormalList[n], &matR);
		NormalList[n].x = nl.x;
		NormalList[n].y = nl.y;
		NormalList[n].z = nl.z;
	}
	
	D3DXMatrixMultiply( &Rotation.Mat, &Parent->Rotation.Mat,  &matR);

	
	//Set the normal to the new rotated one
	Normal.x = normal4.x;
	Normal.y = normal4.y;
	Normal.z = normal4.z;
	Normal.Normalize();


	//Move the branch to its new location. JOORAH!1
	Point.x += Normal.x * MyTree->BranchSize * pow( MyTree->ShrinkRate, MyTree->CurrentDepth ) ;
	Point.y += Normal.y * MyTree->BranchSize * pow( MyTree->ShrinkRate, MyTree->CurrentDepth );
	Point.z += Normal.z * MyTree->BranchSize * pow( MyTree->ShrinkRate, MyTree->CurrentDepth );



	//Record where this branches vertices are
	VBPosition = MyTree->BranchVertexBuffer.size();

	//Rotate the vertices and add them to the temporary vertex buffer
	CustomVertex final_vertex;
	Real branchSize = MyTree->BranchSize * pow( MyTree->ShrinkRate, MyTree->CurrentDepth );
	for(i=0; i<BRANCH_POINTS; i++)
	{
		//Rotate the vertices
		D3DXVec3Transform( &v4[i], (D3DXVECTOR3*)&VertexList[i], &Rotation.Mat);
		VertexList[i].x = v4[i].x; 
		VertexList[i].y = v4[i].y; 
		VertexList[i].z = v4[i].z;
		VertexList[i] += Point;

		//Add it to the global vertex pool
		final_vertex.position = VertexList[i];
		
		final_vertex.tu = i%3;
		final_vertex.tv = (Depth%2)*(branchSize/10);
		final_vertex.tu2 = (i%3)*4;
		final_vertex.tv2 = ((Depth%2)*(branchSize/10))*4;

		//Figure out the vertex color
		final_vertex.normal = NormalList[i];
				
		//Push in the final vertex!
		MyTree->BranchVertexBuffer.push_back( final_vertex );
	}

	

	
	//Calculate the indices O_O
	
	//i1/i2 are used for a wrap around of I when it gets
	//to the last element (since we need to connect the
	//last point with the first point to make a circle)
	int i1, i2; //our vertex position
	int pi1, pi2; //parents vertex position
	Vector3D Tri[3];
	
	for(i=0; i<BRANCH_POINTS; i++)
	{
		//Set the values
		i1 = VBPosition+i;
		i2 = VBPosition+i+1;
		pi1 = Parent->VBPosition+i;
		pi2 = Parent->VBPosition+i+1;

		//Wrap around if neccesary
		if(i2 > VBPosition+BRANCH_POINTS-1)
			i2 = VBPosition;

		if(pi2 > Parent->VBPosition+BRANCH_POINTS-1)
			pi2 = Parent->VBPosition;


		
		//Plot indices O_O

		//tri1 0-4-5
		MyTree->BranchIndexBuffer.push_back( pi1 );
		MyTree->BranchIndexBuffer.push_back( i1 );
		MyTree->BranchIndexBuffer.push_back( i2 );

		//tri2 0-5-1
		MyTree->BranchIndexBuffer.push_back( pi1 );
		MyTree->BranchIndexBuffer.push_back( i2 );
		MyTree->BranchIndexBuffer.push_back( pi2 );		
		
		//add stuff to the collision mesh
        
		
		Tri[0] = MyTree->BranchVertexBuffer[i2].position;
		Tri[1] = MyTree->BranchVertexBuffer[i1].position;
		Tri[2] = MyTree->BranchVertexBuffer[pi1].position;
		MyTree->ColMesh.AddTriangles( Tri, 1 );
		MyTree->FullColMesh.AddTriangles( Tri, 1 );
		Tri[2] = MyTree->BranchVertexBuffer[pi1].position;
		Tri[1] = MyTree->BranchVertexBuffer[i2].position;
		Tri[0] = MyTree->BranchVertexBuffer[pi2].position;
		MyTree->ColMesh.AddTriangles( Tri, 1 );
		MyTree->FullColMesh.AddTriangles( Tri, 1 );
	}

	//Rotate the leaves KHHHHHHHH
	Matrix LeafMatrix;
	Matrix LeafRotation;
	int    LeafTwist;
	for(int lt=0; lt<LEAVES_PER_BRANCH; lt++)
	{
		//Make a matrix for rotating the leaf
		LeafTwist = (rand() % 360) - 180;
		D3DXQuaternionRotationYawPitchRoll(&qR, 0, 0, RADIAN(LeafTwist));
		D3DXMatrixRotationQuaternion(&matR, &qR);
		LeafRotation.Mat = matR;
		LeafMatrix = Rotation;
		LeafMatrix = LeafMatrix * LeafRotation;
			

		//Create the indices for the index buffer O_______O
		//tri1 0-1-2
		int indexstart = MyTree->LeafVertexBuffer.size();
		MyTree->LeafIndexBuffer.push_back( indexstart+0 );
		MyTree->LeafIndexBuffer.push_back( indexstart+1 );
		MyTree->LeafIndexBuffer.push_back( indexstart+2 );

	
		//rotate all three points and put in the vertex buffer
		for(int vt=0; vt<3; vt++)
		{
			D3DXVec3Transform( &v4[vt], (D3DXVECTOR3*)&LeafList[lt].LeafVertex[vt], &LeafMatrix.Mat);
			LeafList[lt].LeafVertex[vt].x = v4[vt].x; LeafList[lt].LeafVertex[vt].y = v4[vt].y; LeafList[lt].LeafVertex[vt].z = v4[vt].z;
			LeafList[lt].LeafVertex[vt] += Point;
						
			//Push the vertex into the vertex buffer O_O
			final_vertex.position = LeafList[lt].LeafVertex[vt];
			final_vertex.normal.x = Normal.x;
			final_vertex.normal.y = Normal.y;
			final_vertex.normal.z = Normal.z;

			//Set the texture coordinates
			if(vt == 0){ final_vertex.tu = 0; final_vertex.tv = 1; }
			if(vt == 1){ final_vertex.tu = 1; final_vertex.tv = 1; }
			if(vt == 2){ final_vertex.tu = .5; final_vertex.tv = 0; }
	
			//OMGS PUSH BACK INTO THE VERTEX BUFFER!!11 O_O
			MyTree->LeafVertexBuffer.push_back( final_vertex );

			Tri[vt] = final_vertex.position;
			
		}
		MyTree->FullColMesh.AddTriangles( Tri, 1 );

		
	}

}

void ProceduralTree::Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile)
{

	//Load in the attributes
	char IniBuffer[128];
	char IniBuffer2[128];

	WMPtr = WM;

	//Load in the leaf texture
	GetIniValue( ObjectFile, "tree", "leaf", IniBuffer);
	GetIniValue( ObjectFile, "tree", "leaf_a", IniBuffer2);
	LeafTexture = Screen.GetTextureHandle( IniBuffer, IniBuffer2 );

	//bark texture
	GetIniValue( ObjectFile, "tree", "bark", IniBuffer);
	BarkTexture = Screen.GetTextureHandle( IniBuffer );
	
	//attributes
	MaxDepth = GetIniValue( ObjectFile, "tree", "depth");
	Twist     = GetIniValueFloat( ObjectFile, "tree", "twist", IniBuffer );
	PitchMinA = GetIniValueFloat( ObjectFile, "tree", "pitchmina", IniBuffer );
	PitchMinB = GetIniValueFloat( ObjectFile, "tree", "pitchminb", IniBuffer );
	PitchMaxA = GetIniValueFloat( ObjectFile, "tree", "pitchmaxa", IniBuffer );
	PitchMaxB = GetIniValueFloat( ObjectFile, "tree", "pitchmaxb", IniBuffer );
	YawRange  = GetIniValueFloat( ObjectFile, "tree", "yawrange", IniBuffer );
	DepthEffect  = GetIniValueFloat( ObjectFile, "tree", "deptheffect", IniBuffer );
	GravityA     = GetIniValueFloat( ObjectFile, "tree", "gravitya", IniBuffer );
	GravityB     = GetIniValueFloat( ObjectFile, "tree", "gravityb", IniBuffer );
	BranchSize   = GetIniValueFloat( ObjectFile, "tree", "branchsize", IniBuffer );
	ShrinkRate   = GetIniValueFloat( ObjectFile, "tree", "shrinkrate", IniBuffer );
	BranchThickness = GetIniValueFloat( ObjectFile, "tree", "branchthickness", IniBuffer );
	BranchLeafThreshold = GetIniValueFloat( ObjectFile, "tree", "branchleafthreshold", IniBuffer );
	LeafSize = GetIniValueFloat( ObjectFile, "tree", "leafsize", IniBuffer );

	
	//Reset some stuff to 0
	CurrentDepth = 0;
	nLeafs = 0;


	//Clear the buffers and such
	Root.DeleteChildren();
	BranchIndexBuffer.clear();
	BranchVertexBuffer.clear();
	LeafVertexBuffer.clear();
	LeafIndexBuffer.clear();
	

	//Create the whole tree
	BuildTree( NULL, 0 );


	//Create the collision mesh
	ColMesh.NextGroup();
	ColMesh.Finalize();
	FullColMesh.NextGroup();
	FullColMesh.Finalize();


	//get the base sphere
	BaseSphere.x = FullColMesh.MeshList[0].x;
	BaseSphere.y = FullColMesh.MeshList[0].y;
	BaseSphere.z = FullColMesh.MeshList[0].z;
	BaseSphere.Radius = FullColMesh.MeshList[0].Radius;

	//Reset the base bounding box
	BaseBox.AABBmin.x = 0;
	BaseBox.AABBmin.y = 0;
	BaseBox.AABBmin.z = 0;
	BaseBox.AABBmax.x = 0;
	BaseBox.AABBmax.y = 0;
	BaseBox.AABBmax.z = 0;

	Vector3D SphereTest;

	
	//Create the VB and IB for the tree
	TreeVB.Init( Screen, D3DFVF_CUSTOMVERTEX, sizeof(CustomVertex), BranchVertexBuffer.size() );
	TreeIB.Init( Screen, BranchIndexBuffer.size() );

	//Write stuff the the VB
	CustomVertex* VList;
	TreeVB.Lock( (BYTE**)&VList ); 
	for(int i=0; i<BranchVertexBuffer.size(); i++)
	{
		//Add the something something to the real vertex buffer
		VList[i] = BranchVertexBuffer[i];	

		//stretch out the bounding sphere as neccesary
		SphereTest.x = VList[i].position.x - BaseSphere.x;
		SphereTest.y = VList[i].position.y - BaseSphere.y;
		SphereTest.z = VList[i].position.z - BaseSphere.z;

		if(SphereTest.Mag() > BaseSphere.Radius)
			BaseSphere.Radius = SphereTest.Mag();

		//stretch out the bounding box as neccesary
		if(VList[i].position.x < BaseBox.AABBmin.x)
			BaseBox.AABBmin.x = VList[i].position.x;
		if(VList[i].position.y < BaseBox.AABBmin.y)
			BaseBox.AABBmin.y = VList[i].position.y;
		if(VList[i].position.z < BaseBox.AABBmin.z)
			BaseBox.AABBmin.z = VList[i].position.z;
		if(VList[i].position.x > BaseBox.AABBmax.x)
			BaseBox.AABBmax.x = VList[i].position.x;
		if(VList[i].position.y > BaseBox.AABBmax.y)
			BaseBox.AABBmax.y = VList[i].position.y;
		if(VList[i].position.z > BaseBox.AABBmax.z)
			BaseBox.AABBmax.z = VList[i].position.z;
	}
	TreeVB.Unlock();

	//Write some more stuff to the IB O_O
	WORD* IList;
	TreeIB.Lock( &IList );//[CRASH] I don't know if it's possible to use an STL vector in memcpy like this. O_O
	for(i=0; i<BranchIndexBuffer.size(); i++)
	{
		IList[i] = BranchIndexBuffer[i];
	
	}
	TreeIB.Unlock();

	//Create the VB and IB for the leafs
	LeafVB.Init( Screen, D3DFVF_CUSTOMVERTEX, sizeof(CustomVertex), LeafVertexBuffer.size() );
	LeafIB.Init( Screen, LeafIndexBuffer.size() );


	//Write stuff the the VB
	LeafVB.Lock( (BYTE**)&VList ); 
	for(i=0; i<LeafVertexBuffer.size(); i++)
	{
		VList[i] = LeafVertexBuffer[i];

		//stretch out the bounding sphere as neccesary
		SphereTest.x = VList[i].position.x - BaseSphere.x;
		SphereTest.y = VList[i].position.y - BaseSphere.y;
		SphereTest.z = VList[i].position.z - BaseSphere.z;

		if(SphereTest.Mag() > BaseSphere.Radius)
			BaseSphere.Radius = SphereTest.Mag();

		//stretch out the bounding box as neccesary
		if(VList[i].position.x < BaseBox.AABBmin.x)
			BaseBox.AABBmin.x = VList[i].position.x;
		if(VList[i].position.y < BaseBox.AABBmin.y)
			BaseBox.AABBmin.y = VList[i].position.y;
		if(VList[i].position.z < BaseBox.AABBmin.z)
			BaseBox.AABBmin.z = VList[i].position.z;
		if(VList[i].position.x > BaseBox.AABBmax.x)
			BaseBox.AABBmax.x = VList[i].position.x;
		if(VList[i].position.y > BaseBox.AABBmax.y)
			BaseBox.AABBmax.y = VList[i].position.y;
		if(VList[i].position.z > BaseBox.AABBmax.z)
			BaseBox.AABBmax.z = VList[i].position.z;
	
	}
	LeafVB.Unlock();

	BaseSphere.Radius += LeafSize;
	//Write some more stuff to the IB O_O
	LeafIB.Lock( &IList );
	for( i=0; i<LeafIndexBuffer.size(); i++)
	{
		IList[i] = LeafIndexBuffer[i];
	
	}
	LeafIB.Unlock();


	//Remove our temporary vertex and index buffers
	BranchIndexBuffer.clear();
	BranchVertexBuffer.clear();
	LeafVertexBuffer.clear();
	LeafIndexBuffer.clear();


}

bool ProceduralTree::BuildTree( Branch* Parent, int branch )
{
	//Mark that we're farther down
	CurrentDepth ++;
	
	//If we're building the parent make a special case
	if( Parent == NULL)
	{
		Root.Point.x = 0;
		Root.Point.y = 0;
		Root.Point.z = 0;
		Root.Normal.x = 0;
		Root.Normal.y = 1;
		Root.Normal.z = 0;

		Real t = BranchThickness;
		Root.VertexList[0].x = -t; Root.VertexList[0].y =  0; Root.VertexList[0].z =  t;
		Root.VertexList[1].x =  t; Root.VertexList[1].y =  0; Root.VertexList[1].z =  t;
		Root.VertexList[2].x =  t; Root.VertexList[2].y =  0; Root.VertexList[2].z = -t;
		Root.VertexList[3].x = -t; Root.VertexList[3].y =  0; Root.VertexList[3].z = -t;

		BuildTree( &Root, 0 );
	}
	else
	{
		//Otherwise we're not the root branch, so create the
		//memory for the branch
		Parent->ChildBranch[branch] = new Branch;
		Branch* CurrentBranch = Parent->ChildBranch[branch];

		CurrentBranch->Depth = CurrentDepth;

		CurrentBranch->BranchHeritage = branch;

		//Then figure out the branch's orientation and points and such
		CurrentBranch->Parent = Parent;
		CurrentBranch->BuildPoints( Parent->Point, Parent->Normal, this);


		//Then build any child branches if we need to
		if(CurrentDepth <= MaxDepth)
		{
			BuildTree(CurrentBranch, 0 );
			BuildTree(CurrentBranch, 1 );

		}
		else
		{
			CurrentBranch->ChildBranch[0] = NULL;
			CurrentBranch->ChildBranch[1] = NULL;

		}
	}


	CurrentDepth --;
	return true;
}


void ProceduralTree::AdjustBoundingBox()
{
	//Set the bounding sphere
	Sphere = BaseSphere;
	Sphere.x += Location.x;
	Sphere.y += Location.y;
	Sphere.z += Location.z;

	//set the bounding box
	Box = BaseBox;
	Box.AABBmin.x += Location.x;
	Box.AABBmin.y += Location.y;
	Box.AABBmin.z += Location.z;
	Box.AABBmax.x += Location.x;
	Box.AABBmax.y += Location.y;
	Box.AABBmax.z += Location.z;
}



bool ProceduralTree::CheckVisibility( Screen3D& Screen, bool RecordTime )
{
	//Store the last time this was drawn, and make sure
	//we don't draw it twice in one frame
	if(RecordTime)
	{
		if( LastDrawn == WMPtr->GTPtr->GetFrameStart() )
			return false;

		LastDrawn = WMPtr->GTPtr->GetFrameStart();
	}

	//Check if it's the screen O_O
	if( Screen.SphereInFrustum( Sphere ) )
			return true;

	return false;
}

bool ProceduralTree::LightCollision ( Vector3D& point1, Vector3D& point2, CollisionInfo* Collision )
{
	return FullColMesh.Collision( Location, point1, point2, Collision );
}

bool ProceduralTree::Collision( Vector3D& point1, Vector3D& point2, CollisionInfo* Collision )
{
	return ColMesh.Collision( Location, point1, point2, Collision );
}


void ProceduralTree::Draw( Screen3D &Screen, MeshManager &MM, Camera &Viewer, GameTimer &Timer )
{
	Screen.SetTransform( Location.GetMatrix() );
	DrawAll(Screen );
}

void ProceduralTree::DrawAll ( Screen3D& Screen )
{


	//Store the pointer to the screen and draw both 
	//halves of the tree
	ScreenPtr = &Screen;
	
	//Draw the branches
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	ScreenPtr->SetTexture( BarkTexture, 0 );
	ScreenPtr->StandardShaderTransform();
	ScreenPtr->D3DDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
	ScreenPtr->D3DDevice->SetStreamSource( 0, TreeVB.VB, TreeVB.VertexSize );
	ScreenPtr->D3DDevice->SetIndices(TreeIB.GetD3DBufferInterface(), 0);
	ScreenPtr->D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, TreeVB.nVertices, 0, TreeIB.GetIndexCount()/3);
	BranchPolygons = TreeIB.GetIndexCount() / 3;

	//Draw the leaves
	ScreenPtr->SetTexture( LeafTexture, 0 );
	ScreenPtr->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHAREF, 32);
	ScreenPtr->D3DDevice->SetStreamSource( 0, LeafVB.VB, LeafVB.VertexSize );
	ScreenPtr->D3DDevice->SetIndices(LeafIB.GetD3DBufferInterface(), 0);
	ScreenPtr->D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, LeafVB.nVertices, 0, LeafIB.GetIndexCount()/3);
	LeafPolygons = LeafIB.GetIndexCount() / 3;

	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHAREF, 10);
	Screen.PolygonsRendered += BranchPolygons + LeafPolygons;
	
}
