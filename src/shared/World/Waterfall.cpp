#include "WaterFall.h"
#include "world.h"

void WaterFall::Update( Real FrameTime, WorldManager* SM )
{
	//declarations
	CustomVertex* SrcVertex;


	Mesh* watermesh = (SM->MMPtr->Get(hMesh));
	
	
	
	//loop through all the various "groups" in the mesh.
	for(int m=0; m<watermesh->nMeshes; m++)
	{

		//Get pointers to all the various vertex lists
		watermesh->VB[m].Lock( (BYTE**) &SrcVertex );
	

		//loop through the vertices in the group
		for(int i=0; i<watermesh->VB[m].nVertices; i++)
		{
			SrcVertex[i].tv+=FrameTime;
		}

		//Release the pointers		
		watermesh->VB[m].Unlock();

	}


}

void WaterFall::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{

	//set the transformation
	Screen.SetTransform( Location.GetMatrix() );

	//draw the object
	Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_ONE);
	Screen.D3DDevice->SetRenderState(D3DRS_CULLMODE,           D3DCULL_CCW);

	


	Mesh* watermesh = (WMPtr->MMPtr->Get(hMesh));
	if(watermesh)
		watermesh->Draw(Screen, (Position*)&Viewer, &Location, true);


	
	Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);

}