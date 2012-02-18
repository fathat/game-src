#include "water.h"
#include "world.h"

#include "..\defs\defs.h"

void WaterEffect::Init( char* TextureName, Screen3D& Screen, WorldManager* wm )
{
	//Create textures for the water
	WaterTexture = Screen.GetTextureHandle(TextureName, TextureName);
	RippleTexture = Screen.GetBlankTexture("rippletexture", NULL, WATERDIM, WATERDIM);

	//Create 2 buffers for the water which we'll
	//page flip between.
	WaterBuffer  = new Real[WATERDIM*WATERDIM];
	WaterBuffer2 = new Real[WATERDIM*WATERDIM];

	//Set the pointers to the buffer that we use
	//to page flip (the WaterBuffer's aren't directly
	//used, but rather referenced through these).
	OldWater = WaterBuffer;
	NewWater = WaterBuffer2;

	//Clear both the buffers.
	ZeroMemory(OldWater, WATERDIM*WATERDIM*sizeof(Real));
	ZeroMemory(NewWater, WATERDIM*WATERDIM*sizeof(Real));
	ZeroMemory(JumpTable, GRID_DIM*GRID_DIM*4);


	//Now, go through and find all the points which are
	//in the land, and mask these out so that we don't
	//proccess them. Also, figure out the jump table values.

	// DivFactor is what we need to multiply the water
	// x and y by to get the heightmap points
	Real DivFactor = (Real)wm->GetWidth()/(Real)WATERDIM;
	
	
	for(int y=0; y<WATERDIM; y++)
	{
		for(int x=0; x<WATERDIM; x++)
		{

			//First, figure out which jump table element we should be at.
			int JtX = (int)((Real)((Real)x/(Real)WATERDIM) * (Real)GRID_DIM);
			int JtY = (int)((Real)((Real)y/(Real)WATERDIM) * (Real)GRID_DIM);

			//Next, figure out the map points that we'll need to reference
			int mx = (int)((Real)x * DivFactor);
			int my = (int)((Real)y * DivFactor);


			//If the point there is above water level, mask it out.
			if(wm->GetHeightmapArray()[mx+(my*wm->GetWidth())] > WATER_LEVEL)
				NewWater[x+y*WATERDIM] = -1;
			else
			{
				//Otherwise, update the jump table to include it, and
				//create a jump table element if neccesary.
				if(!JumpTable[JtX + JtY*GRID_DIM])
				{
					JumpTable[JtX + JtY*GRID_DIM] = new GridPoint;
					JumpTable[JtX + JtY*GRID_DIM]->StartX = JtX * (WATERDIM/GRID_DIM);
					JumpTable[JtX + JtY*GRID_DIM]->StartY = JtY * (WATERDIM/GRID_DIM);
					JumpTable[JtX + JtY*GRID_DIM]->EndX = JtX * (WATERDIM/GRID_DIM) + (WATERDIM/GRID_DIM);
					JumpTable[JtX + JtY*GRID_DIM]->EndY = JtY * (WATERDIM/GRID_DIM) + (WATERDIM/GRID_DIM);
					JumpTable[JtX + JtY*GRID_DIM]->WaterPoints = 0;

					if(JumpTable[JtX + JtY*GRID_DIM]->StartX == 0)
						JumpTable[JtX + JtY*GRID_DIM]->StartX = 1;

					if(JumpTable[JtX + JtY*GRID_DIM]->StartY == 0)
						JumpTable[JtX + JtY*GRID_DIM]->StartY = 1;

					if(JumpTable[JtX + JtY*GRID_DIM]->EndX >= WATERDIM)
						JumpTable[JtX + JtY*GRID_DIM]->EndX = WATERDIM-1;
					if(JumpTable[JtX + JtY*GRID_DIM]->EndY >= WATERDIM)
						JumpTable[JtX + JtY*GRID_DIM]->EndY = WATERDIM-1;
					
				}

				JumpTable[JtX + JtY*GRID_DIM]->WaterPoints ++;
			}

			 
		}
	}


	Width   = wm->GetWidth();
	Height  = wm->GetHeight();
	Spacing = wm->GetSpacing();
}

void WaterEffect::Cleanup()
{
	if(WaterBuffer)
		delete [] WaterBuffer;

	if(WaterBuffer2)
		delete [] WaterBuffer2;

	for(int i=0; i<GRID_DIM*GRID_DIM; i++)
	{
		if(JumpTable[i])
			delete JumpTable[i];

		JumpTable[i];
	}

	WaterBuffer  = NULL;
	WaterBuffer2 = NULL;
}

void WaterEffect::MakeRipple( Real x, Real z, Real Force )
{
	int rx = x/(Width*Spacing) * WATERDIM;
	int ry = z/(Height*Spacing)* WATERDIM;

	OldWater[rx+ry*WATERDIM] = Force;

}

void WaterEffect::UpdateTexture( Real FrameTime, Screen3D& Screen )
{
	
	//Disperse the water
	
	/*for(int y=1; y<WATERDIM-1; y++)
	{
		for(int x=1; x<WATERDIM-1; x++)
		{
			if(NewWater[x+y*WATERDIM] == -1)
				continue;

			NewWater[x+y * WATERDIM] = ((OldWater[x+ ((y-1)*WATERDIM)] +
							  OldWater[x+ ((y+1)*WATERDIM)] +
							  OldWater[(x+1)+ (y*WATERDIM)] +
							  OldWater[(x-1)+ (y*WATERDIM)] )*.5f) - NewWater[x+y * WATERDIM];
			NewWater[x+y * WATERDIM] -= (NewWater[x+y * WATERDIM]*0.0625) ;


			if(NewWater[x+y * WATERDIM] < 0)
				NewWater[x+y * WATERDIM] =0;

			if(NewWater[x+y * WATERDIM] > 250)
				NewWater[x+y * WATERDIM] = 250;

		}
	}*/

	for(int i=0; i<GRID_DIM*GRID_DIM; i++)
	{
		if(JumpTable[i])
		{
			for(int y=JumpTable[i]->StartY; y<JumpTable[i]->EndY; y++)
			{
				for(int x=JumpTable[i]->StartX; x<JumpTable[i]->EndX; x++)
				{
					if(NewWater[x+y*WATERDIM] == -1)
						continue;

					NewWater[x+y * WATERDIM] = ((OldWater[x+ ((y-1)*WATERDIM)] +
									  OldWater[x+ ((y+1)*WATERDIM)] +
									  OldWater[(x+1)+ (y*WATERDIM)] +
									  OldWater[(x-1)+ (y*WATERDIM)] )*.5f) - NewWater[x+y * WATERDIM];
					NewWater[x+y * WATERDIM] -= (NewWater[x+y * WATERDIM]*0.0625) * FrameTime;


					if(NewWater[x+y * WATERDIM] < 0)
						NewWater[x+y * WATERDIM] =0;

					if(NewWater[x+y * WATERDIM] > 250)
						NewWater[x+y * WATERDIM] = 250;

				}
			}

		}
	}




	//Update the texture
	LPDIRECT3DTEXTURE9 TextureSurface;
	D3DSURFACE_DESC SurfDesc;
	D3DLOCKED_RECT  LockedArea;
	BYTE* Surface;

	Texture* T = Screen.TM.GetTexture(RippleTexture);
	TextureSurface = T->GetTextureSurface();

	//Figure out how many "levels" (mip-maps) there are
	//in the texture
	int nLevels = TextureSurface->GetLevelCount();
	TextureSurface->GetLevelDesc(0, &SurfDesc);
	Real DivFactor=1;
	
	//loop through each level
	for(int l=0; l<nLevels; l++)
	{
		//Get the surface's attributes
		TextureSurface->GetLevelDesc(l, &SurfDesc);

		//lock down a pointer
		TextureSurface->LockRect(l, &LockedArea, NULL, NULL);

		//draw the pixel to the surface
		Surface = (BYTE*)LockedArea.pBits;

		if(SurfDesc.Format == D3DFMT_A8R8G8B8)
		{
	
			for(int y=0; y<WATERDIM/DivFactor; y++)
			{
				for(int x=0; x<WATERDIM/DivFactor; x++)
				{
					int rx=x*DivFactor;
					int ry=y*DivFactor;
					Surface[(x*4)+0+y*(SurfDesc.Width*4)] = 128+NewWater[rx+ry * WATERDIM];
					Surface[(x*4)+1+y*(SurfDesc.Width*4)] = 128+NewWater[rx+ry * WATERDIM];
					Surface[(x*4)+2+y*(SurfDesc.Width*4)] = 128+NewWater[rx+ry * WATERDIM];
				}
			}
		}	

		if(SurfDesc.Format == D3DFMT_L8)
		{
	
			for(int y=0; y<WATERDIM/DivFactor; y++)
			{
				for(int x=0; x<WATERDIM/DivFactor; x++)
				{
					int rx=x*DivFactor;
					int ry=y*DivFactor;
					Surface[(x+y*SurfDesc.Width)] = 128+NewWater[rx+ry * WATERDIM];
					
				}
			}
		}	
				
		DivFactor *=2;


		//unlock the surface
		TextureSurface->UnlockRect(l);
	}
	

		//Swap the buffers
	Real* Temp = NewWater;
	NewWater = OldWater;
	OldWater = Temp;

	

}

void WaterEffect::Draw( Screen3D& Screen )
{

	CustomVertex Water[4];

	Water[0].position.x=0;
	Water[0].position.y=WATER_LEVEL;
	Water[0].position.z=0;
	Water[0].normal.x = 0;
	Water[0].normal.y = 1;
	Water[0].normal.z = 0;
	Water[0].tu       = 0;
	Water[0].tv       = 0;
	Water[0].tu2      = 0;
	Water[0].tv2      = 0;
	
	Water[1].position.x=(Real) Width*Spacing;
	Water[1].position.y=WATER_LEVEL;
	Water[1].position.z=0;
	Water[1].normal.x = 0;
	Water[1].normal.y = 1;
	Water[1].normal.z = 0;
	Water[1].tu       = (Real)Width;
	Water[1].tv       = 0;
	Water[1].tu2      = 1;
	Water[1].tv2      = 0;
	
	Water[2].position.x=0;
	Water[2].position.y=WATER_LEVEL;
	Water[2].position.z=(Real)Height*Spacing;
	Water[2].normal.x = 0;
	Water[2].normal.y = 1;
	Water[2].normal.z = 0;
	Water[2].tu       = 0;
	Water[2].tv       = (Real)Height;
	Water[2].tu2      = 0;
	Water[2].tv2      = 1;
	
	Water[3].position.x=(Real) Width*Spacing;
	Water[3].position.y=WATER_LEVEL;
	Water[3].position.z=(Real) Height*Spacing;
	Water[3].normal.x = 0;
	Water[3].normal.y = 1;
	Water[3].normal.z = 0;
	Water[3].tu       = (Real)Width;
	Water[3].tv       = (Real)Height;
	Water[3].tu2      = 1;
	Water[3].tv2      = 1;
	



	// draw the water

	Screen.SetTextureMode( TMNormal );
	Screen.SetTexture(RippleTexture, 1);
	Screen.SetTexture(WaterTexture, 0);
	Screen.D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	Screen.D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	Screen.D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	Screen.D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	Screen.D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	Screen.SetFVF( D3DFVF_CUSTOMVERTEX );
	Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Water, sizeof(CustomVertex));
	Screen.D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	Screen.TM.SetTextureMode(TMNormal, 0, true);
	Screen.D3DDevice->SetTexture(1, NULL);

}
