#include "AnimatedMesh.h"
#include "..\ini\ini.h"
#include "..\shaders\constants.h"

#include "..\defs\defs.h"


//Initialize the animation state
void AnimationState::InitFromFile ( MeshManager& MM, char* name, char* file, char* recommendednextstate   )
{
	//Set some values
	Name = name; //Name is the common name we'll call it
	FileName = file; //Filename is its path and file
	RecommendedNextState = recommendednextstate; //recommended next state 
	
	//Load the file!
	hMesh = MM.GetMeshHandle( file, false ); 
}

// Inits/Destroys the mesh
bool AnimatedMesh::Init( Screen3D& Screen, MeshManager& MM, char* descriptionfile )
{
	ScreenPtr = &Screen;
	MMPtr = &MM;

	std::string debugStr = "Creating animated mesh ";
	debugStr += descriptionfile;
	OutputDebugString(debugStr.c_str());
	//Open the file!
	FILE* File = fopen( descriptionfile, "r+t" );
	if(!File)
		return false;
	char c[2];
	c[1] = 0;

	//Read until we reach the end of the file
	std::string NameBuffer;
	std::string ValBuffer1;
	std::string ValBuffer2;
	std::string TimeBuffer;
	int current_string=0;
	bool Done=false;
	while(!Done)
	{
		if(feof(File))
			Done = true;

		//read the character
		fread(c, 1, 1, File);
		if(strcmpi(c, "\n") == 0)
		{

			//LINE SI DONE!111
			current_string++;


			//yay, if this was the end of the third string
			//then we add the state! HOORAY!!1
			if(current_string >=4 )
			{
				//HOORAY! ADD THE STATE!
				current_string = 0;
				int t = atoi( TimeBuffer.c_str() );
				AddState( (char*)NameBuffer.c_str(), (char*)ValBuffer1.c_str(), (char*)ValBuffer2.c_str(), (Real)t / 1000.0f );
				
				//clear out teh buffers!
				NameBuffer = "";
				ValBuffer1 = "";
				ValBuffer2 = "";
				TimeBuffer = "";
			}
						
		}
		else
		{

			//Add the character to the appropriate string
			if(current_string == 0)
				NameBuffer += c;
			else if(current_string == 1)
				ValBuffer1 += c;
			else if(current_string == 2)
				ValBuffer2 += c;
			else if(current_string == 3)
				TimeBuffer += c;
		}
			

	}

	

	fclose(File);



    //WOOT!
	return true;
}


//delete something something and such. So yeah.
void AnimatedMesh::Cleanup()
{
}


// Add a "state" (pose) to a mesh
void AnimatedMesh::AddState ( char* Name, char* MeshFile, char* NextState, Real time )
{
	AnimationState NewState;
	NewState.InitFromFile( *MMPtr, Name, MeshFile, NextState );
	NewState.TransitionTime = time;
	StateList.push_back( NewState );
}

//remove one of the states
void AnimatedMesh::RemoveState ( char* Name)
{
	// NOT TESTED!
	int i = FindStateIndex( Name );

	if(i != -1 )
		StateList.erase( StateList.begin() + i );
}

//Find a state/pose
int  AnimatedMesh::FindStateIndex( char* StateName )
{
	for(unsigned int i=0; i<StateList.size(); i++)
	{
		//woohoo! we found our state
		if(StateList[i].Name.compare(StateName) == 0 )
		{
			return i;
		}
	}

	//FAILURE!
	return -1;
}


//Check if the mesh is visible. Still sort of hacked together
bool AnimatedMesh::CheckVisibility( Screen3D& Screen, Position& MeshLocation)
{
	//[HACK] This should do a check on the current animated state, not
	//       just the base state.
	if(StateList.size() < 1 )
		return false;
	
	//Get the mesh pointars!1
	Mesh* M1;
	M1 = MMPtr->Get( StateList[CurrentState].hMesh );
	
	//return the visibility state
	return M1->CheckVisibility( Screen, MeshLocation );

}

//check if something ran into our damned mesh O__O
bool AnimatedMesh::CheckForCollision( Position& MeshLocation, Vector3D* Line, CollisionInfo* c)
{
	//[HACK] This should do a check on the current animated state, not
	//       just the base state. I don't know how though, because
	//       when I tried to interpolate the stuff it ran at about
	//       5 frames per second. Damn.
	if(StateList.size() < 1 )
		return false;
	
	//Get the mesh pointars!1
	Mesh* M1;
	M1 = MMPtr->Get( StateList[CurrentState].hMesh );

	return M1->CheckForCollision( MeshLocation, Line, c );
}

//Get teh bounding box!11
AABB AnimatedMesh::GetBox()
{
	if(StateList.size() < 1 )
	{
		AABB NullBox;
		return NullBox;
	}
	
	Mesh* M1, *M2;
	M1 = MMPtr->Get( StateList[CurrentState].hMesh );
	M2 = MMPtr->Get( StateList[NextState].hMesh );
	
	//Interpolate between the two boxes as long as there is a second
	//box to interpolate with
	AABB ResultBox = M1->Box;
	if(M2 != NULL && M1 != M2)
		ResultBox = M1->Box.InterpolateBox( M2->Box, Transition/TransitionTime ) ;
	else
		ResultBox = M1->Box;
	
	return ResultBox;
}


//Get teh bounding sphere's radius
Real AnimatedMesh::GetSphereRadius()
{
	if(StateList.size() < 1 )
		return false;
	
	//Get the mesh pointars!1
	Mesh* M1, *M2;
	M1 = MMPtr->Get( StateList[CurrentState].hMesh );
	M2 = MMPtr->Get( StateList[NextState].hMesh );
	
	//Get the interpolated sphere radius assuming we have
	//two points, otherwise just get the base meshes sphere
	//radius
	Real Percent = (Transition/TransitionTime);
	Real InvPercent = 1.0f-Percent;
	Real FP;
	if(M2 && M1 != M2)
		FP = M1->FarthestPoint*InvPercent + M2->FarthestPoint*Percent;
	else
		FP = M1->FarthestPoint;
	return FP;
}

// update the mesh's current state. This
// should be called every frame.
void AnimatedMesh::Update ( Real FrameTime )
{
	if(StateList.size() < 1 )
		return;
	//The main idea here is that we want to move the animation
	//timer along, and handle changes when the timer finished
	Transition += FrameTime*TimeMultiplier;


	//OHNO! We've run out of time on these two states, NEXT PLEASE!
	if(Transition >= TransitionTime )
	{
		//Find out if we have a next state specified
		Transition = 0;
		
		//the destination state is now the source state O___O
		CurrentState = NextState;

		//Set the default next state. If there isn't a recommended one we just go
		//back to zero
		NextState = FindStateIndex( (char*)StateList[CurrentState].RecommendedNextState.c_str() );
		if( NextState == -1)
		{
			NextState = 0;
			NextStateName = StateList[0].Name;
		}
		else
		{
			NextStateName = StateList[CurrentState].RecommendedNextState;
			TransitionTime = StateList[NextState].TransitionTime;
		}

		
	}
}

// draw the mesh.
void AnimatedMesh::Draw( Screen3D& Screen, Matrix &Location)
{
	if(StateList.size() < 1 )
		return;


	//Get the various matrices
	D3DXMATRIX matWorld, matView, matProj;


	matWorld = Location.Mat;
	ScreenPtr->D3DDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	ScreenPtr->D3DDevice->GetTransform( D3DTS_VIEW, &matView );

	// set the world matrix (used for lighting stuff mostly)
	D3DXMATRIX compoundMatrix;
	D3DXMATRIX invWorldMatrix;
	compoundMatrix = Location.Mat;
	D3DXMatrixInverse(&invWorldMatrix, NULL, &Location.Mat);
	ScreenPtr->SetVertexShaderConstant(CV_WORLD_0, (const D3DXVECTOR4*)&invWorldMatrix, 4);


	//set the combined world/proj/view matrix (for transformations and such)
	D3DXMatrixMultiply(&compoundMatrix, &compoundMatrix, &matView);
	D3DXMatrixMultiply(&compoundMatrix, &compoundMatrix, &matProj);
	ScreenPtr->SetVertexShaderConstant(CV_WORLDVIEWPROJ_0, &compoundMatrix);


	//set blend factor
	float fTimeDelta = (float)Transition / (float)TransitionTime;
	D3DXVECTOR4 blendFactor(fTimeDelta, 1.0f - fTimeDelta, 0.0f, 0.0f);
	ScreenPtr->SetVertexShaderConstant(CV_BLEND_FACTORS, &blendFactor, 1);

	D3DXVECTOR4 lightcolor( 1.0f, 1.0f, 1.0f, 1.0f );
	D3DXVECTOR4 materialcolor( 1.0f, 1.0f, 1.0f, 1.0f );
	ScreenPtr->SetVertexShaderConstant(10, &lightcolor, 1);
	ScreenPtr->SetVertexShaderConstant(11, &materialcolor, 1);

	//Get the mesh pointers
	Mesh* M1, *M2;
	M1 = MMPtr->Get( StateList[CurrentState].hMesh );
	M2 = MMPtr->Get( StateList[NextState].hMesh );

	//make sure we don't have a mismatch
	if( M1->GetSubmeshCount() != M2->GetSubmeshCount() )
		return;

	//Draw teh mesh list!
	ScreenPtr->SetRenderState( D3DRS_LIGHTING, FALSE );
	ScreenPtr->SetRenderState( D3DRS_FOGENABLE, FALSE );
	ScreenPtr->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	
	for(int m=0; m<M1->GetSubmeshCount(); m++)
	{
		ScreenPtr->D3DDevice->SetVertexDeclaration( MMPtr->GetShaderDeclaration());
		ScreenPtr->D3DDevice->SetVertexShader( MMPtr->GetShader() );
		ScreenPtr->D3DDevice->SetIndices(M1->GetSubmesh(m)->IB.GetD3DBufferInterface());
		if(M1->GetSubmesh(m)->materialIndex >= 0)
			ScreenPtr->SetTexture( M1->Material[M1->GetSubmesh(m)->materialIndex].Image, 0 );
		ScreenPtr->D3DDevice->SetStreamSource(0, M1->GetSubmesh(m)->VB.D3DBuffer(), 0, sizeof(KeyframeVertex ) );
		ScreenPtr->D3DDevice->SetStreamSource(1, M2->GetSubmesh(m)->VB.D3DBuffer(), 0, sizeof(KeyframeVertex ) );
		//ScreenPtr->SetStreamSource( 0, &M1->VB[m], sizeof( KeyframeVertex ) );
		//ScreenPtr->SetStreamSource( 1, &M2->VB[m], sizeof( KeyframeVertex ) );
		//M1->VB[m].SetActive(0);
		//M2->VB[m].SetActive( 1 );
		//ScreenPtr->D3DDevice->DrawPrimitive( D3DPT_POINTLIST, 0, M1->VB[m].nVertices); 
		ScreenPtr->D3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, M1->GetSubmesh(m)->VB.VertexCount(), 0, M1->GetSubmesh(m)->IB.GetIndexCount()/3);
		ScreenPtr->PolygonsRendered += M1->GetSubmesh(m)->IB.GetIndexCount()/3;
		ScreenPtr->DrawCalls++;
		ScreenPtr->SetStreamSource(NULL, 0 );
		ScreenPtr->SetStreamSource(NULL, 1 );
		
        
	}

	ScreenPtr->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	ScreenPtr->SetRenderState( D3DRS_LIGHTING, TRUE );
}
