#include "tree.h"
#include "..\ini\ini.h"
#include "world.h"
#include "..\physics\structure.h"

ProceduralTree::ProceduralTree()
{
	nLeafs = 0;
	ObjectType = OT_STATIC;
}


void Branch::BuildPoints(Vector3D& Start, Vector3D& Dir,  ProceduralTree* MyTree )
{
	//declarations
	D3DXQUATERNION qR;
	D3DXVECTOR4 normal4;
	D3DXVECTOR4 v4[BRANCH_POINTS];
	D3DXMATRIX matR, matR2, matR3, matR4;
	
	D3DXMatrixIdentity(&matR);
	int rx;
	int ry;
	int rz;


	Rotation   = Parent->Rotation ;
	qRotation  = Parent->qRotation;


	//Initialize the something or other to some such thing
	
	Real t = MyTree->BranchThickness;
	//
	//if(MyTree->CurrentDepth>MyTree->MaxDepth/2)
		//t *=  pow( .5, MyTree->CurrentDepth );
	//else
		t *= (Real)(MyTree->MaxDepth+1-MyTree->CurrentDepth ) / (Real)MyTree->MaxDepth;
	VertexList[0].x =  0; VertexList[0].y =  0; VertexList[0].z =  t;
	VertexList[1].x =  t; VertexList[1].y =  0; VertexList[1].z =  t/2;
	VertexList[2].x =  t; VertexList[2].y =  0; VertexList[2].z = -t/2;
	VertexList[3].x =  0; VertexList[3].y =  0; VertexList[3].z = -t;
	VertexList[4].x = -t; VertexList[4].y =  0; VertexList[4].z = -t/2;
	VertexList[5].x = -t; VertexList[5].y =  0; VertexList[5].z =  t/2;

	//Set the normals on these mothabeaches!
	Vector3D NormalList[BRANCH_POINTS];
	NormalList[0].x =  0; NormalList[0].y =  0; NormalList[0].z =  t;
	NormalList[1].x =  t; NormalList[1].y =  0; NormalList[1].z =  t/2;
	NormalList[2].x =  t; NormalList[2].y =  0; NormalList[2].z = -t/2;
	NormalList[3].x =  0; NormalList[3].y =  0; NormalList[3].z = -t;
	NormalList[4].x = -t; NormalList[4].y =  0; NormalList[4].z = -t/2;
	NormalList[5].x = -t; NormalList[5].y =  0; NormalList[5].z =  t/2;
		
	int n=0;
	for(n=0; n<BRANCH_POINTS; n++)
	{
		//NormalList[n] *= -1.0f;
		NormalList[n].Normalize();
	}

	//memcpy(VertexList, Parent->VertexList, sizeof(Vector3D)*4);


	//Figure out some ranges and such
	int PitchRange = MyTree->PitchMax - MyTree->PitchMin;
	

	//Set us to where the parent is
	Point = Start;
	Normal.x=0;//Dir;
	Normal.y=1;
	Normal.z=0;



	//OHNO! Root is the parent, so we have to CHEAT!!! 
	//Basically, since we want the tree to be balanced, and since
	//balancing it is easier at the root stage, we for the second
	//branch we just mirror the first one
	if(MyTree->CurrentDepth == 2 )
	{

		TreeVertex rootvertex;

		for(int i=0; i<BRANCH_POINTS; i++)
		{
			//store the position
			rootvertex.position = VertexList[i];
			
			//figure out the texture coordinates
			rootvertex.tu = i%2;
			rootvertex.tv = MyTree->BranchSizeMax/20;
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
		rx = (rand() % PitchRange) + MyTree->PitchMin;
		ry = 0+((rand() % MyTree->YawRange)-(MyTree->YawRange/2));
		rz = 0;

		

	}
	else if(BranchHeritage == 1)
	{
		//otherwise it didn't spawn off the root, so
		//rotate the branch a bit randomely
		rx = -((rand() % PitchRange) + MyTree->PitchMin);
		ry = -60+((rand() % MyTree->YawRange)-(MyTree->YawRange/2));
		rz = 0;
	}

	else if(BranchHeritage == 2)
	{
		//..then we want to make this branch point more upwards
		rx = -((rand() % PitchRange) + MyTree->PitchMin) ;
		ry = 60+((rand() % MyTree->YawRange)-(MyTree->YawRange/2));
		rz = 0;
	
	}

	if(MyTree->CurrentDepth > 4 )
	{
		Point.y -= (rand()%(int)MyTree->Gravity)*((Real)MyTree->CurrentDepth/(Real)MyTree->MaxDepth);
	}

	//Create the leaves
	if( t <= MyTree->BranchLeafThreshold )
	{
		for(int l=0; l<LEAVES_PER_BRANCH; l++)
		{
			t = MyTree->LeafSize;
			
			LeafList[l].LeafVertex[0].x = -t/2; LeafList[l].LeafVertex[0].y =  0; LeafList[l].LeafVertex[0].z =  0;
			LeafList[l].LeafVertex[1].x =  t/2; LeafList[l].LeafVertex[1].y =  0; LeafList[l].LeafVertex[1].z =  0;
			LeafList[l].LeafVertex[2].x =  t/2; LeafList[l].LeafVertex[2].y =  t; LeafList[l].LeafVertex[2].z = 0;
			LeafList[l].LeafVertex[3].x = -t/2; LeafList[l].LeafVertex[3].y =  t; LeafList[l].LeafVertex[3].z = 0;
			MyTree->nLeafs ++;
		}
	}


	//Figure out the angle that we want to rotate by
	Real FixedRX = (Real)rx ;
	Real FixedRY = (Real)ry ;
	Real FixedRZ = (Real)rz ;

	
	//Rotate the normal vector
	D3DXQuaternionRotationYawPitchRoll(&qR, RADIAN(FixedRY), RADIAN(FixedRX), RADIAN(FixedRZ));
	D3DXMatrixRotationQuaternion(&matR, &qR);
	
	//rotate the main normal
	D3DXVec3Transform( &normal4, (D3DXVECTOR3*)&Normal, &matR);

	//store the overall rotation
	D3DXQuaternionMultiply( &qRotation,  &Parent->qRotation, &qR );
	D3DXMatrixRotationQuaternion( &Rotation.Mat, &qRotation);
	
	//Set the normal to the new rotated one
	Normal.x = normal4.x;
	Normal.y = normal4.y;
	Normal.z = normal4.z;
	Normal.Normalize();

	//rotate.. TEH VERTEX NORMALS
	D3DXVECTOR4 nl;
	for(n=0; n<BRANCH_POINTS; n++)
	{
		D3DXVec3Transform( &nl, (D3DXVECTOR3*)&NormalList[n], &Rotation.Mat);
		NormalList[n].x = nl.x;
		NormalList[n].y = nl.y;
		NormalList[n].z = nl.z;

		NormalList[n].Normalize();
	}


	Real BranchSize = rand() % (int)abs(MyTree->BranchSizeMax - MyTree->BranchSizeMin) + MyTree->BranchSizeMin;

	//Move the branch to its new location. 
	Point.x += Normal.x * BranchSize * pow( MyTree->ShrinkRate, MyTree->CurrentDepth ) ;
	Point.y += Normal.y * BranchSize * pow( MyTree->ShrinkRate, MyTree->CurrentDepth );
	Point.z += Normal.z * BranchSize * pow( MyTree->ShrinkRate, MyTree->CurrentDepth );

	//Record where this branches vertices are
	VBPosition = MyTree->BranchVertexBuffer.size();

	//Rotate the vertices and add them to the temporary vertex buffer
	TreeVertex final_vertex;
	for(int i=0; i<BRANCH_POINTS; i++)
	{
		//Rotate the vertices
		//D3DXVec3Transform( &v4[i], (D3DXVECTOR3*)&VertexList[i], &Rotation.Mat);
		//VertexList[i].x = v4[i].x; 
		//VertexList[i].y = v4[i].y; 
		//VertexList[i].z = v4[i].z;
		VertexList[i] += Point;
	
		//Add it to the global vertex pool
		final_vertex.position = VertexList[i];
		
		final_vertex.tu = i%2;
		final_vertex.tv = (Depth%2)*4;

		//Figure out the vertex color
		final_vertex.normal = (D3DXVECTOR3)NormalList[i];
				
		//Push in the final vertex!
		MyTree->BranchVertexBuffer.push_back( final_vertex );
	}

	//Calculate the indices
	
	//i1/i2 are used for a wrap around of I when it gets
	//to the last element (since we need to connect the
	//last point with the first point to make a circle)
	int i1, i2; //our vertex position
	int pi1, pi2; //parents vertex position
	Vector3D Tri[3];

	for(int i=0; i<BRANCH_POINTS; i++)
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


		//Plot indices

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

	//Rotate the leaves 
	Matrix LeafMatrix;
	Matrix LeafRotation;
	int    LeafTwist;
	int    LeafOffset;
	for(int lt=0; lt<LEAVES_PER_BRANCH; lt++)
	{
		//Make a matrix for rotating the leaf
		LeafTwist = (rand() % 360) - 180;
		LeafOffset = (rand() % 30) - 15;
		D3DXQuaternionRotationYawPitchRoll(&qR, RADIAN(FixedRY), RADIAN(FixedRX)+RADIAN(90)+RADIAN(LeafOffset),RADIAN(LeafTwist));
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

		//tri2 2-3-0
		MyTree->LeafIndexBuffer.push_back( indexstart+2 );
		MyTree->LeafIndexBuffer.push_back( indexstart+3 );
		MyTree->LeafIndexBuffer.push_back( indexstart+0 );		

		//rotate all four points and put in the vertex buffer
		for(int vt=0; vt<4; vt++)
		{
			D3DXVec3Transform( &v4[vt], (D3DXVECTOR3*)&LeafList[lt].LeafVertex[vt], &LeafMatrix.Mat);
			LeafList[lt].LeafVertex[vt].x = v4[vt].x; LeafList[lt].LeafVertex[vt].y = v4[vt].y; LeafList[lt].LeafVertex[vt].z = v4[vt].z;
			LeafList[lt].LeafVertex[vt] += Point;
			
			//Push the vertex into the vertex buffer
			final_vertex.position = LeafList[lt].LeafVertex[vt];
			final_vertex.normal.x = 0;
			final_vertex.normal.y = 1;
			final_vertex.normal.z = 0;

			//Set the texture coordinates
			if(vt == 0){ final_vertex.tu = 0; final_vertex.tv = 1; }
			if(vt == 1){ final_vertex.tu = 1; final_vertex.tv = 1; }
			if(vt == 2){ final_vertex.tu = 1; final_vertex.tv = 0; }
			if(vt == 3){ final_vertex.tu = 0; final_vertex.tv = 0; }

			//place in vertex buffer
			MyTree->LeafVertexBuffer.push_back( final_vertex );

			Tri[vt] = final_vertex.position;
		}

		MyTree->FullColMesh.AddTriangles( Tri, 1 );
		
	}

}

void ProceduralTree::Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* descfile)
{

	//Load in the attributes
	char IniBuffer[128];
	char IniBuffer2[128];
	
	WMPtr = WM;

	//Load in the leaf texture
	GetIniValue( descfile, "treev2", "leaf", IniBuffer);
	GetIniValue( descfile, "treev2", "leaf_a", IniBuffer2);
	LeafTexture = Screen.GetTextureHandle( IniBuffer, IniBuffer2 );

	//bark texture
	GetIniValue( descfile, "treev2", "bark", IniBuffer);
	BarkTexture = Screen.GetTextureHandle( IniBuffer );
	
	//attributes
	MaxDepth     = GetIniValue( descfile, "treev2", "depth");
	Twist        = (float)GetIniValueFloat( descfile, "treev2", "twist", IniBuffer );
	PitchMin     = (float)GetIniValueFloat( descfile, "treev2", "pitchmin", IniBuffer );
	PitchMax     = (float)GetIniValueFloat( descfile, "treev2", "pitchmaxa", IniBuffer );
	YawRange     = (float)GetIniValueFloat( descfile, "treev2", "yawrange", IniBuffer );
	DepthEffect  = (float)GetIniValueFloat( descfile, "treev2", "deptheffect", IniBuffer );
	Gravity      = (float)GetIniValueFloat( descfile, "treev2", "gravity", IniBuffer );
	BranchSizeMin= (float)GetIniValueFloat( descfile, "treev2", "branchsizemin", IniBuffer );
	BranchSizeMax= (float)GetIniValueFloat( descfile, "treev2", "branchsizemax", IniBuffer );
	ShrinkRate   = (float)GetIniValueFloat( descfile, "treev2", "shrinkrate", IniBuffer );
	BranchThickness = (float)GetIniValueFloat( descfile, "treev2", "branchthickness", IniBuffer );
	BranchLeafThreshold = (float)GetIniValueFloat( descfile, "treev2", "branchleafthreshold", IniBuffer );
	LeafSize = (float)GetIniValueFloat( descfile, "treev2", "leafsize", IniBuffer );

	
	CurrentDepth = 0;
	nLeafs = 0;

	Root.DeleteChildren();
	BranchIndexBuffer.clear();
	BranchVertexBuffer.clear();
	LeafVertexBuffer.clear();
	LeafIndexBuffer.clear();
	
	BuildTree( NULL, 0 );


		//Create the collision mesh
	ColMesh.NextGroup();
	ColMesh.Finalize();
	FullColMesh.NextGroup();
	FullColMesh.Finalize();


	//get the base sphere
	BaseSphere.x( FullColMesh.MeshList[0].x );
	BaseSphere.y( FullColMesh.MeshList[0].y );
	BaseSphere.z( FullColMesh.MeshList[0].z);
	BaseSphere.Radius = FullColMesh.MeshList[0].Radius;

	//Reset the base bounding box
	BaseBox.min.x = 0;
	BaseBox.min.y = 0;
	BaseBox.min.z = 0;
	BaseBox.max.x = 0;
	BaseBox.max.y = 0;
	BaseBox.max.z = 0;

	Vector3D SphereTest;

	//Create the VB and IB for the tree
	TreeVB.Init( Screen, D3DFVF_TREEVERTEX, sizeof(TreeVertex), BranchVertexBuffer.size() );
	TreeIB.Init( Screen, BranchIndexBuffer.size(), IB16Bit );

	//Write stuff the the VB
	TreeVertex* VList;
	TreeVB.Lock( (BYTE**)&VList ); 
	for(int i=0; i<(int)BranchVertexBuffer.size(); i++)
	{
		VList[i] = BranchVertexBuffer[i];
	
	}
	TreeVB.Unlock();

	
	WORD* IList;
	TreeIB.Lock( &IList );
	for(int i=0; i<(int)BranchIndexBuffer.size(); i++)
	{
		IList[i] = BranchIndexBuffer[i];
	
	}
	TreeIB.Unlock();

	//Create the VB and IB for the leafs
	LeafVB.Init( Screen, D3DFVF_TREEVERTEX, sizeof(TreeVertex), LeafVertexBuffer.size() );
	LeafIB.Init( Screen, LeafIndexBuffer.size(), IB16Bit );


	//Write stuff the the VB
	LeafVB.Lock( (BYTE**)&VList ); 
	for(int i=0; i<(int)LeafVertexBuffer.size(); i++)
	{
		VList[i] = LeafVertexBuffer[i];

		//stretch out the bounding sphere as neccesary
		SphereTest.x = VList[i].position.x - BaseSphere.x();
		SphereTest.y = VList[i].position.y - BaseSphere.y();
		SphereTest.z = VList[i].position.z - BaseSphere.z();

		if(SphereTest.Mag() > BaseSphere.Radius)
			BaseSphere.Radius = SphereTest.Mag();

		//stretch out the bounding box as neccesary
		if(VList[i].position.x < BaseBox.min.x)
			BaseBox.min.x = VList[i].position.x;
		if(VList[i].position.y < BaseBox.min.y)
			BaseBox.min.y = VList[i].position.y;
		if(VList[i].position.z < BaseBox.min.z)
			BaseBox.min.z = VList[i].position.z;
		if(VList[i].position.x > BaseBox.max.x)
			BaseBox.max.x = VList[i].position.x;
		if(VList[i].position.y > BaseBox.max.y)
			BaseBox.max.y = VList[i].position.y;
		if(VList[i].position.z > BaseBox.max.z)
			BaseBox.max.z = VList[i].position.z;
	
	}
	LeafVB.Unlock();

	
	LeafIB.Lock( &IList );
	for(int i=0; i<(int)LeafIndexBuffer.size(); i++)
	{
		IList[i] = LeafIndexBuffer[i];
	}
	LeafIB.Unlock();

	Structure* treeStructure = new Structure();
	Vector3D l = Location;
	hTreeStructure = treeStructure->Init(ColMesh, &WMPtr->Solver, l, Location.Yaw, Location.Pitch, Location.Roll);
	WMPtr->Solver.RegisterStructure( treeStructure );



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
		Root.Normal.x = 1;
		Root.Normal.y = 0;
		Root.Normal.z = 0;

		Real t = BranchThickness;
		Root.VertexList[0].x = -t; Root.VertexList[0].y =  0; Root.VertexList[0].z =  t;
		Root.VertexList[1].x =  t; Root.VertexList[1].y =  0; Root.VertexList[1].z =  t;
		Root.VertexList[2].x =  t; Root.VertexList[2].y =  0; Root.VertexList[2].z = -t;
		Root.VertexList[3].x = -t; Root.VertexList[3].y =  0; Root.VertexList[3].z = -t;


	

		D3DXQuaternionRotationYawPitchRoll( &Root.qRotation, 0, 0, 0);

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
			int a=0;
			int b=0;
			int c=0;
			


			if(CurrentDepth > 4 )
			{
				a=rand()%100;
				b=rand()%100;
				c=rand()%100;
			}
				
			if(a < 70)
				BuildTree(CurrentBranch, 0 );
			if(b < 70)
				BuildTree(CurrentBranch, 1 );
			if(c < 70)
				BuildTree(CurrentBranch, 2 );

		}
		else
		{
			CurrentBranch->ChildBranch[0] = NULL;
			CurrentBranch->ChildBranch[1] = NULL;
			CurrentBranch->ChildBranch[2] = NULL;

		}
	}


	CurrentDepth --;
	return true;
}


void ProceduralTree::AdjustBoundingVolume()
{
	//Set the bounding sphere
	Sphere = BaseSphere;
	Sphere.location += Location;
	//Sphere.x += Location.x;
	//Sphere.y += Location.y;
	//Sphere.z += Location.z;

	//set the bounding box
	Box = BaseBox;
	Box.min.x += Location.x;
	Box.min.y += Location.y;
	Box.min.z += Location.z;
	Box.max.x += Location.x;
	Box.max.y += Location.y;
	Box.max.z += Location.z;
}



bool ProceduralTree::CheckVisibility( Screen3D& Screen, bool RecordTime )
{
	if( Screen.SphereInFrustum( Sphere ) )
		return true;

	return false;
}

bool ProceduralTree::LightCollision ( Vector3D& point1, Vector3D& point2, CollisionInfo* Collision )
{
	return FullColMesh.Collision( Location, point1, point2, Collision );
}

bool ProceduralTree::Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision )
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
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	ScreenPtr->SetTexture( BarkTexture, 0 );
	ScreenPtr->SetFVF( D3DFVF_TREEVERTEX );
	//ScreenPtr->D3DDevice->SetStreamSource( 0, TreeVB.VB,0,  TreeVB.VertexSize );
	TreeVB.SetActive(0);
	ScreenPtr->D3DDevice->SetIndices(TreeIB.GetD3DBufferInterface());
	ScreenPtr->D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,  TreeVB.VertexCount(), 0, TreeIB.GetIndexCount()/3);
	BranchPolygons = TreeIB.GetIndexCount() / 3;

	//Draw the leaves
	ScreenPtr->SetTexture( LeafTexture, 0 );
	ScreenPtr->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHAREF, 32);
	//ScreenPtr->D3DDevice->SetStreamSource( 0, LeafVB.VB, 0, LeafVB.VertexSize );
	LeafVB.SetActive( 0 );
	ScreenPtr->D3DDevice->SetIndices(LeafIB.GetD3DBufferInterface());
	ScreenPtr->D3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,0,  LeafVB.VertexCount(), 0, LeafIB.GetIndexCount()/3);
	LeafPolygons = LeafIB.GetIndexCount() / 3;
	
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHAREF, 10);
	Screen.PolygonsRendered += BranchPolygons + LeafPolygons;
	Screen.DrawCalls++;

	ScreenPtr->SetFVF( D3DFVF_CUSTOMVERTEX );
}

