#include "DecalManager.h"
#include "world.h"


#include "..\defs\defs.h"

void DecalManager::AddDecal(    CollisionInfo* collision, TextureHandle T, Real FrameTime )
{
	//Fill in decal information
    Decal* NewDecal = new Decal;
	NewDecal->texture = T;
		
	Position BulletLocation;
	for(int i=0; i<MAX_DECALS; i++)
	{
		if(DecalList[i] == NULL)
		{
            DecalList[i] = NewDecal;

			D3DXPLANE DecalPlane;
			D3DXVECTOR3 LP1, LP2;
			D3DXVECTOR3 Point;
			DecalPlane.a = collision->PlaneNormal.x;
			DecalPlane.b = collision->PlaneNormal.y;
			DecalPlane.c = collision->PlaneNormal.z;
			DecalPlane.d = 0;


            //Vertex 1			
			LP1.x = collision->CollisionPoint.x - collision->PlaneNormal.x*3;
			LP1.y = collision->CollisionPoint.y - collision->PlaneNormal.y*3;
			LP1.z = collision->CollisionPoint.z - collision->PlaneNormal.z*3;
			LP2.x = collision->CollisionPoint.x + collision->PlaneNormal.x*3;
			LP2.y = collision->CollisionPoint.y + collision->PlaneNormal.y*3;
			LP2.z = collision->CollisionPoint.z + collision->PlaneNormal.z*3;
			D3DXPlaneIntersectLine( &Point, &DecalPlane, &LP1, &LP2 );         
			
			DecalList[i]->VertexList[0].position.x = Point.x+collision->PlaneNormal;
			DecalList[i]->VertexList[0].position.y = Point.y+collision->PlaneNormal;
			DecalList[i]->VertexList[0].position.z = Point.z+collision->PlaneNormal;
			DecalList[i]->VertexList[0].normal.x = collision->PlaneNormal.x;
			DecalList[i]->VertexList[0].normal.y = collision->PlaneNormal.y;
			DecalList[i]->VertexList[0].normal.z = collision->PlaneNormal.z;


			//Vertex 2
			LP1.x = collision->CollisionPoint.x - collision->PlaneNormal.x*3;
			LP1.y = (collision->CollisionPoint.y+1) - collision->PlaneNormal.y*3;
			LP1.z = collision->CollisionPoint.z - collision->PlaneNormal.z*3;
			LP2.x = collision->CollisionPoint.x + collision->PlaneNormal.x*3;
			LP2.y = (collision->CollisionPoint.y+1) + collision->PlaneNormal.y*3;
			LP2.z = collision->CollisionPoint.z + collision->PlaneNormal.z*3;
			D3DXPlaneIntersectLine( &Point, &DecalPlane, &LP1, &LP2 );

			DecalList[i]->VertexList[1].position.x = Point.x+collision->PlaneNormal;
			DecalList[i]->VertexList[1].position.y = Point.y+collision->PlaneNormal;
			DecalList[i]->VertexList[1].position.z = Point.z+collision->PlaneNormal;
			DecalList[i]->VertexList[1].normal.x = collision->PlaneNormal.x;
			DecalList[i]->VertexList[1].normal.y = collision->PlaneNormal.y;
			DecalList[i]->VertexList[1].normal.z = collision->PlaneNormal.z;


			//Vertex 3
			LP1.x = (collision->CollisionPoint.x+1) - collision->PlaneNormal.x*3;
			LP1.y = collision->CollisionPoint.y - collision->PlaneNormal.y*3;
			LP1.z = collision->CollisionPoint.z - collision->PlaneNormal.z*3;
			LP2.x = (collision->CollisionPoint.x+1) + collision->PlaneNormal.x*3;
			LP2.y = collision->CollisionPoint.y + collision->PlaneNormal.y*3;
			LP2.z = collision->CollisionPoint.z + collision->PlaneNormal.z*3;
			D3DXPlaneIntersectLine( &Point, &DecalPlane, &LP1, &LP2 );

			DecalList[i]->VertexList[2].position.x = Point.x+collision->PlaneNormal.x;
			DecalList[i]->VertexList[2].position.y = Point.y+collision->PlaneNormal;
			DecalList[i]->VertexList[2].position.z = Point.z+collision->PlaneNormal;
			DecalList[i]->VertexList[2].normal.x = collision->PlaneNormal.x;
			DecalList[i]->VertexList[2].normal.y = collision->PlaneNormal.y;
			DecalList[i]->VertexList[2].normal.z = collision->PlaneNormal.z;

			//Vertex 3
			LP1.x = (collision->CollisionPoint.x+1) - collision->PlaneNormal.x*3;
			LP1.y = (collision->CollisionPoint.y+1) - collision->PlaneNormal.y*3;
			LP1.z = collision->CollisionPoint.z - collision->PlaneNormal.z*3;
			LP2.x = (collision->CollisionPoint.x+1) + collision->PlaneNormal.x*3;
			LP2.y = (collision->CollisionPoint.y+1) + collision->PlaneNormal.y*3;
			LP2.z = collision->CollisionPoint.z + collision->PlaneNormal.z*3;
			D3DXPlaneIntersectLine( &Point, &DecalPlane, &LP1, &LP2 );

			DecalList[i]->VertexList[3].position.x = Point.x+collision->PlaneNormal;
			DecalList[i]->VertexList[3].position.y = Point.y+collision->PlaneNormal.y ;
			DecalList[i]->VertexList[3].position.z = Point.z+collision->PlaneNormal.z ;
			DecalList[i]->VertexList[3].normal.x = collision->PlaneNormal.x;
			DecalList[i]->VertexList[3].normal.y = collision->PlaneNormal.y;
			DecalList[i]->VertexList[3].normal.z = collision->PlaneNormal.z;

		}
	}
	
    
}

void DecalManager::Clear()
{
	for(int i=0; i<MAX_DECALS; i++)
	{
		if(DecalList[i] == NULL)
			delete DecalList[i];
		DecalList[i] = NULL;
	}
}

void DecalManager::DrawAll( Screen3D& Screen )
{
	for(int i=0; i<MAX_DECALS; i++)
	{

		if(DecalList[i])
		{
			Screen.SetTexture(DecalList[i]->texture, 0);
			Screen.SetFVF( D3DFVF_CUSTOMVERTEX );
			Screen.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DecalList[i]->VertexList, sizeof(CustomVertex));
		}
	}

}