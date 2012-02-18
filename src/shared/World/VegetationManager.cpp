#include "vegetationmanager.h"
#include "world.h"
#include "vegconstruct.h"


#include "..\defs\defs.h"

VegetationManager::VegetationManager()
{
	WMPtr	  = NULL;
	ScreenPtr = NULL;
	TotalPlantCount = 0;
}

bool Vegetation::CheckForCollision( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager& MM) 
{
	//get the mesh pointer
	Mesh* pMesh = MM.Get( hCollisionMesh );

	if(pMesh)
	{
		Vector3D line[2];
		line[0] = p1;
		line[1] = p2;

		return pMesh->CheckForCollision( this->Location, line, c);
	}

	return false;
}


void VegetationManager::GenerateVegetation( char* vegetationdesc, WorldManager* WM )
{
	//Store some values
	WMPtr = WM;
	ScreenPtr = WM->ScreenPtr ;
	IM.Init( *ScreenPtr, *WM->MMPtr, 22.5 );

	//seed the randomizer
	srand(0);

	//load values
	char TempBuffer[256];
	ZeroMemory( TempBuffer, 256 );
	
	GetIniValue( vegetationdesc, "veg", "plant1", TempBuffer) ;
	MeshHandle hPlant1 = WM->MMPtr->GetMeshHandle( TempBuffer );
	GetIniValue( vegetationdesc, "veg", "plant1c", TempBuffer) ;
	MeshHandle hPlant1C = WM->MMPtr->GetMeshHandle( TempBuffer );

	GetIniValue( vegetationdesc, "veg", "plant2", TempBuffer) ;
	MeshHandle hPlant2 = WM->MMPtr->GetMeshHandle( TempBuffer );
	GetIniValue( vegetationdesc, "veg", "plant2C", TempBuffer) ;
	MeshHandle hPlant2C = WM->MMPtr->GetMeshHandle( TempBuffer );

	GetIniValue( vegetationdesc, "veg", "plant3", TempBuffer) ;
	MeshHandle hPlant3 = WM->MMPtr->GetMeshHandle( TempBuffer );
	GetIniValue( vegetationdesc, "veg", "plant3C", TempBuffer) ;
	MeshHandle hPlant3C = WM->MMPtr->GetMeshHandle( TempBuffer );

	GetIniValue( vegetationdesc, "veg", "shorelinetree1", TempBuffer) ;
	MeshHandle hTree1 = WM->MMPtr->GetMeshHandle( TempBuffer );
	GetIniValue( vegetationdesc, "veg", "shorelinetree1c", TempBuffer) ;
	MeshHandle hTree1C = WM->MMPtr->GetMeshHandle( TempBuffer );

	GetIniValue( vegetationdesc, "veg", "shorelinetree2", TempBuffer) ;
	MeshHandle hTree2 = WM->MMPtr->GetMeshHandle( TempBuffer );
	GetIniValue( vegetationdesc, "veg", "shorelinetree2C", TempBuffer) ;
	MeshHandle hTree2C = WM->MMPtr->GetMeshHandle( TempBuffer );


	int PlantDensity = GetIniValue( vegetationdesc, "veg", "plantdensity" );

	TotalPlantCount = 0;

	
	//place vegetation
	Structure* staticStructure ;
	VegConstruction* construct;
	Vegetation v;
	Mesh* mp=NULL;
	
	Real h, slope;
	int r;
	int nodesize=8;
	for(int y=0; y<WM->GetHeight()-1; y+=nodesize)
	{
		for(int x=0; x<WM->GetWidth()-1; x+=nodesize)
		{
			for(int i=0; i<PlantDensity; i++)
			{

				//figure out object location
				v.Location.x =( rand() % WM->GetSpacing() *nodesize) + x*WM->GetSpacing();
				v.Location.z =( rand() % WM->GetSpacing() *nodesize) + y*WM->GetSpacing();

				h = WMPtr->GetHeightValue( v.Location, &slope );
				v.Location.y = h;


			
				//place object if it's within the right height and slope range
				if( h > WMPtr->GetHeightValue( v.Location, NULL, NULL, NULL, NULL, true ) + 10 && slope > 0.8f )
				{
                    v.Location.y = h-1;
					v.Location.Yaw = RADIAN( (float)( rand()%360) );
					v.Collide = false;

					r = rand() % 24;

					if(r < 10)
					{
						v.hMesh = hPlant1;
						v.hCollisionMesh = hPlant1C;
					}
					else if(r < 18)
					{
						v.hMesh = hPlant2;
						v.hCollisionMesh = hPlant2C;
					}
					else if(r < 20)
					{
						v.hMesh = hPlant3;
						v.hCollisionMesh = hPlant3C;
					}
					else if(r < 22)
					{
						v.hMesh = hTree1;
						v.hCollisionMesh = hTree1C;
					}
					else 
					{
						v.hMesh = hTree2;
						v.hCollisionMesh = hTree2C;
					}	
					
					
					//skip placing this object if the mesh is NULL
					mp = WMPtr->MMPtr->Get( v.hMesh );
					if(!mp)
						continue;

					//also skip placing this object if there's another object too close by
					if(WMPtr->FindNearestObject( v.Location, mp->FarthestPoint ) != NULL)
						continue;

					v.hImposter = IM.GetImposterHandle( v.hMesh );

					/*
					//construct the structure from the mesh
					staticStructure = new Structure();
					Vector3D l = v.Location;
					staticStructure->Init( v.hCollisionMesh, WMPtr->MMPtr, &WM->Solver, l, v.Location.Yaw, v.Location.Pitch, v.Location.Roll );
					v.hStructure = WM->Solver.RegisterStructure( staticStructure );

					//dGeomDisable( staticStructure->Geom );
	

					

					staticStructure = NULL;*/

					v.pConstruct = new VegConstruction();
					v.pConstruct->Construct( vegetationdesc, &WM->Solver, *ScreenPtr, *WMPtr->MMPtr, v.Location );
					v.hConstruct = WM->Solver.RegisterConstruction( v.pConstruct );

					VegList.push_back(v);
					

					TotalPlantCount ++;

				}           		
			}
		}
	}

	return;
}

bool VegetationManager::CheckForCollision( Vector3D& p1, Vector3D& p2, CollisionInfo* c)
{

	//This is really really slow
	//whoever wrote this is a lazy idiot..

	//oh wait.....

	for(int i=0; i<VegList.size(); i++)
	{
		if(VegList[i].CheckForCollision(p1, p2, c, *WMPtr->MMPtr  ) )
			return true;
	}


	return false;
}

void VegetationManager::DrawAll( Screen3D& Screen, MeshManager& MM, Camera* Viewer )
{
	if( !WMPtr || !ScreenPtr )
		return;

	
	ScreenPtr->SetMaterial( NULL );
	ScreenPtr->D3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
	ScreenPtr->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
	Screen.SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	Screen.D3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE);
	Screen.D3DDevice->SetRenderState( D3DRS_ALPHAREF, 10);
	Screen.SetRenderState( D3DRS_FOGENABLE, TRUE );

	
	Mesh* pMesh;
	Vector3D distanceVector;
	Real imposterDistance, imposterDieDistance;
	Matrix m;
		
	for( int i=0; i< VegList.size(); i++)
	{	
		pMesh = MM.Get( VegList[i].hMesh );

		if(pMesh)
		{
			//update the location
			Vector3D location;
			VegList[i].pConstruct->GetLocation(location);
			VegList[i].Location.x = location.x;
			VegList[i].Location.y = location.y;
			VegList[i].Location.z = location.z;
			

			if(pMesh->CheckVisibility( Screen, VegList[i].Location ))
			{

				distanceVector.x = VegList[i].Location.x - Viewer->Pos->x;
				distanceVector.y = 0;
				distanceVector.z = VegList[i].Location.z - Viewer->Pos->z;

				

				//imposter distance
				imposterDistance = ((pMesh->Box.max.y-pMesh->Box.min.y) / 100.0f) * 800.0f + 100;

				imposterDieDistance = ((pMesh->Box.max.y-pMesh->Box.min.y) / 25.0f)*5000.0f;
				

				if( (distanceVector.Mag() < imposterDistance ) || VegList[i].pConstruct->Touched() )
				{
				
					VegList[i].pConstruct->GetMatrix(&m) ;
					Screen.SetTransform( m);
					pMesh->Draw( Screen );
				}
				else if( distanceVector.Mag() < imposterDieDistance ) 
				{
					ImposterType* T = IM.GetImposter( VegList[i].hImposter );
					if(T)
					{
						T->WriteVertices( Screen, MM, Viewer, &VegList[i].Location );
					//	T->DrawOptimized( Screen, MM, Viewer, &VegList[i].Location );
					}


				}
			}
		}
	}

	IM.DrawAll(Screen, MM );
	Screen.SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
}