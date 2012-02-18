#include "Rain.h"
#include "World.h"
#include "SceneManager.h"

Rain::Rain(WorldManager* wmptr) 
{
	wm = wmptr;
	nRainDrops = 4000;
	lifetime = 0.5f;
	drops = new RainDrop[nRainDrops];
	ZeroMemory(drops, sizeof(RainDrop)*nRainDrops);
	
	particleBuffer.Init( *wmptr->ScreenPtr, D3DFVF_PARTICLEVERTEX, sizeof(ParticleVertex), nRainDrops*3, true, true, D3DPT_TRIANGLELIST );
}

Rain::~Rain()
{
	delete[] drops;
}

void Rain::Update(float frametime)
{
	for(int i=0; i<nRainDrops; i++) 
	{
		//Respawn raindrop
		if(drops[i].timeLeft <= 0) 
		{
			drops[i].initialLifetime = ((rand() % 100) / 100.0f) * lifetime;
			drops[i].timeLeft = drops[i].initialLifetime;
			drops[i].x = wm->Viewer.Pos->x + ((rand() % 500) - 250);
			drops[i].z = wm->Viewer.Pos->z + ((rand() % 500) - 250);
			drops[i].y = wm->GetHeightValue(Position(Vector3D(drops[i].x, 0, drops[i].z)));
			drops[i].x2 = drops[i].x + rand()%20;
			drops[i].y2 = drops[i].y + 200 + rand()%20;
			drops[i].z2 = drops[i].z + rand()%20;

			/*if(wm->terrain->GetLightValue(drops[i].x, drops[i].z) < 0.8 )
			{
				Vector3D hitLocation, normal;
				SceneObject* objectHit;
				if(wm->CheckBulletCollision(
					Vector3D(drops[i].x2, drops[i].y2, drops[i].z2),
					Vector3D(drops[i].x, drops[i].y, drops[i].z),
					&hitLocation,
					&normal,
					&objectHit,
					&wm->LocalPlayer)) 
				{
					drops[i].x = hitLocation.x;
					drops[i].y = hitLocation.y;
					drops[i].z = hitLocation.z;
				}	
			}*/
		}
		
		//Reduce time left
		drops[i].timeLeft -= frametime;
		if(drops[i].timeLeft < 0) drops[i].timeLeft = 0;
	}
}

void Rain::Draw()
{
	ParticleVertex p[2];	
	Matrix Ident;
	wm->ScreenPtr->SetTransform( Ident );
	wm->ScreenPtr->SetTexture(-1, 0);
	wm->ScreenPtr->SetFVF(D3DFVF_PARTICLEVERTEX);
	wm->ScreenPtr->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	wm->ScreenPtr->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	wm->ScreenPtr->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	wm->ScreenPtr->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//Draw lines
	for(int i=0; i<nRainDrops; i++) 
	{
		if(drops[i].timeLeft > 0) 
		{
			int alpha = (int)((drops[i].timeLeft / drops[i].initialLifetime) * 32.0f);
			p[0].color = D3DCOLOR_ARGB(alpha, 200, 200, 255);
			p[0].position.x = drops[i].x;
			p[0].position.y = drops[i].y;
			p[0].position.z = drops[i].z;
			p[0].tu = 0;
			p[0].tv = 0;
			p[1].color = D3DCOLOR_ARGB(0, 200, 200, 255);
			p[1].position.x = drops[i].x2;
			p[1].position.y = drops[i].y2;
			p[1].position.z = drops[i].z2;
			p[1].tu = 1;
			p[1].tv = 1;
			
			wm->ScreenPtr->D3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, p, sizeof(ParticleVertex));
		}
	}

	//Draw splashes
	D3DXMATRIX matWorld, matView, invView;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &invView );
	wm->ScreenPtr->D3DDevice->GetTransform( D3DTS_VIEW, &matView );

	invView._11 = -matView._11; invView._21 = matView._12; invView._31 = -matView._13;
	invView._12 = -matView._21; invView._22 = matView._22; invView._32 = -matView._23;
	invView._13 = -matView._31; invView._23 = matView._32; invView._33 = -matView._33;
	invView._14=0.0f; invView._24=0.0f; invView._34=0.0f; invView._44=1.0f;

	ParticleVertex* vertex;
	particleBuffer.Lock((BYTE**)&vertex, D3DLOCK_DISCARD);

	ParticleVertex v[3];
	Real ParticleSize = 2;
	v[0].tu = 0;  v[0].tv = 1;
	v[1].tu = 1;  v[1].tv = 1;
	v[2].tu = .5; v[2].tv = 0;

	D3DXVECTOR3 triangle[3];
	triangle[0].x = -ParticleSize; triangle[0].y = 0; triangle[0].z = 0;
	triangle[1].x = ParticleSize; triangle[1].y = 0; triangle[1].z = 0;
	triangle[2].x = 0; triangle[2].y = ParticleSize*2; triangle[2].z = 0;
	int count = 0;
	for(int i=0; i<nRainDrops; i++) 
	{
		if(drops[i].timeLeft > 0) 
		{
			int alpha = (int)((drops[i].timeLeft / drops[i].initialLifetime) * 32.0f);

			
			//Create the matrix to translate the particles by
			invView._41 = drops[i].x;
			invView._42 = drops[i].y;
			invView._43 = drops[i].z;

			//Translate all the points by our matrix
			D3DXVECTOR4 R1, R2, R3;
			D3DXVec3Transform(  &R1, &triangle[0], &invView );
			D3DXVec3Transform(  &R2, &triangle[1], &invView );
			D3DXVec3Transform(  &R3, &triangle[2], &invView );

			v[0].position = (D3DXVECTOR3)R1;
			v[1].position = (D3DXVECTOR3)R2;
			v[2].position = (D3DXVECTOR3)R3;
			v[0].color = D3DCOLOR_RGBA(255, 255, 255, alpha);
			v[1].color = D3DCOLOR_RGBA(255, 255, 255, alpha);
			v[2].color = D3DCOLOR_RGBA(255, 255, 255, alpha);
		

			//Now set the *actual* vertices to our something something
			vertex[count+0] = v[0];
			vertex[count+1] = v[1];
			vertex[count+2] = v[2];
			count += 3;
		}
	}
	particleBuffer.Unlock();
	wm->ScreenPtr->SetTexture(wm->ScreenPtr->GetTextureHandle("../base/weather/rain.png"), 0);
	wm->ScreenPtr->SetTransform( Ident );
	wm->ScreenPtr->SetFVF(D3DFVF_PARTICLEVERTEX);
	particleBuffer.SetActive( 0 );
	wm->ScreenPtr->D3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, count / 3);
	wm->ScreenPtr->PolygonsRendered += count;
	wm->ScreenPtr->DrawCalls++;


}