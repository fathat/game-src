#include "TextureManager.h"
#include <string.h>
#include <algorithm>
#include "screen.h"
#include "RenderTarget.h"


DWORD VectortoRGBA( D3DXVECTOR3* v, FLOAT fHeight )
{
    DWORD r = (DWORD)( 127.0f * v->x + 128.0f );
    DWORD g = (DWORD)( 127.0f * v->y + 128.0f );
    DWORD b = (DWORD)( 127.0f * v->z + 128.0f );
    DWORD a = (DWORD)( 255.0f * fHeight );
    
    return( (a<<24L) + (r<<16L) + (g<<8L) + (b<<0L) );
}



void TextureManager::Clear()
{
	ScreenPtr = NULL;

	for(int i=0; i<MAX_TEXTURES; i++)
	{
		if(TextureList[i] != NULL)
			delete TextureList[i];

		TextureList[i] = NULL;
	}
}

void TextureManager::Destroy()
{
	Clear();
}

TextureHandle TextureManager::AddTexture(const char* Filename, const char* AlphaMask, int width, int height, bool notFile, D3DFORMAT Format )
{

	//create the new texture
	Texture* NewTexture = new Texture;

	if(!notFile)
	{
		if(!NewTexture->LoadTexture(  ScreenPtr, Filename, AlphaMask, Format ))
		{
			delete NewTexture;
			return -1;
		}
	}
	else
	{
		NewTexture->Init( ScreenPtr, Filename, width, height, Format);

		if(AlphaMask)
		{
			NewTexture->AddAlpha( AlphaMask);
		}
	}
	

	//loop through and find an empty spot to place it
	for(int i=0; i<MAX_TEXTURES; i++)
	{
		//blank spot?
		if(TextureList[i] == NULL)
		{
			//assign the texture a spot
			TextureList[i] = NewTexture;
			
			//done, return the handle
			return i;
		}

	}

	return -1;
}

void TextureManager::SetTextureMode( TextureMode TM, int BaseStage, bool ForceSet)
{
	if (TM == CurrentTextureMode && ForceSet == false)
		return; //no need to change -- states the same.
	ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU  );

	if(TM == TMNormal)
	{


		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		ScreenPtr->D3DDevice->SetTexture(1, 0);

	}
	else if(TM == TMAlphaMask)
	{
		// Not tested.
		//ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		//ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		//ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		ScreenPtr->SetLinearTextureFiltering(1);

		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
	}
	else if(TM == TMDarkMap)
	{
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);

		
	}

	else if(TM == TMDetailMap)
	{
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADDSIGNED );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);

	}

	else if(TM == TMAdd)
	{
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
	}

	else if(TM == TMModulate2x)
	{
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	}

	else if(TM == TMSecondMap)
	{
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD  );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	
	}
	
	else if(TM == TMBlendMap)
	{
		//Decals texture 1 over texture 0
		
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR  );
		
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );




	}

	else if (TM== TMSphereMap)
	{
		// Generate spheremap texture coords, and shift them over
        D3DXMATRIX mat;
        mat._11 = 0.5f; mat._12 = 0.0f; mat._13 = 0.0f; mat._14 = 0.0f; 
        mat._21 = 0.0f; mat._22 =-0.5f; mat._23 = 0.0f; mat._24 = 0.0f; 
        mat._31 = 0.0f; mat._32 = 0.0f; mat._33 = 1.0f; mat._34 = 0.0f; 
        mat._41 = 0.5f; mat._42 = 0.5f; mat._43 = 0.0f; mat._44 = 1.0f; 
        ScreenPtr->D3DDevice->SetTransform( D3DTS_TEXTURE1, &mat );
        ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
        ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		ScreenPtr->D3DDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD  );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		ScreenPtr->D3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		ScreenPtr->D3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	}

}


void TextureManager::RemoveTexture(TextureHandle ID)
{
	//destroy the texture
	if(TextureList[ID] != NULL)
		delete TextureList[ID];

	TextureList[ID] = NULL;

}

void TextureManager::RestoreTextures()
{
	for(int i=0; i<MAX_TEXTURES; i++)
	{
		if(TextureList[i])
			TextureList[i]->Reload();
	}

}

void TextureManager::LimitSize( int texSize )
{
	for(int i=0; i<MAX_TEXTURES; i++)
	{
		if(TextureList[i])
			TextureList[i]->LimitUploadSize( texSize );
	}

}

void TextureManager::UnlimitTextures()
{
	for(int i=0; i<MAX_TEXTURES; i++)
	{
		if(TextureList[i])
			TextureList[i]->UnlimitSize();
	}
}

TextureHandle TextureManager::TextureExists(const char* TextureName)
{
	//loop through all the textures to see if one
	//with the same name exists
	for(int i=0; i<MAX_TEXTURES; i++)
	{
		//if the texture exists in this spot..
		if(TextureList[i])
		{
			int RVal = strcmpi(TextureName, TextureList[i]->GetFilename());
			
			//did we find a texture with the same name?
			if(RVal == 0)
				return i;
		}
	}


	//nope, didn't find the texture
	return -1;

}

bool TextureManager::HasAlpha( TextureHandle ID ) {
	if(ID >= 0 && ID < MAX_TEXTURES)
		return TextureList[ID]->ContainsAlpha(); 
	else
		return false;

}

void TextureManager::PrintLoadedTexturesToDebug()
{
	for(int i=0; i<MAX_TEXTURES; i++)
	{
		//if the texture exists in this spot..
		if(TextureList[i])
		{
			char buffer[512];
			sprintf( buffer, "Tex: %s w: %d h: %d m: %f\n", 
				TextureList[i]->GetFilename(), 
				TextureList[i]->GetWidth(), 
				TextureList[i]->GetHeight(), 
				TextureList[i]->GetTextureMemorySize() / 1000.0f);

			//OutputDebugString("Tex: ");
			//OutputDebugString(  );
			OutputDebugString(buffer);
		}
	}
}


//Render target management
void TextureManager::RegisterRenderTarget( RenderTarget* rt ) 
{
	RenderTargets.push_back( rt );
}

void TextureManager::UnregisterRenderTarget( RenderTarget* rt ) 
{
	
	std::list<RenderTarget*>::iterator it = find( RenderTargets.begin(), RenderTargets.end(), rt );

	if (it != RenderTargets.end() )
		RenderTargets.remove( rt );
	
}

void TextureManager::InvalidateRenderTargets()
{
	std::list<RenderTarget*>::iterator it = RenderTargets.begin();

	for( ; it!=RenderTargets.end(); it++ )
	{
		(*it)->Invalidate();
	}
}


void TextureManager::RestoreRenderTargets()
{
	std::list<RenderTarget*>::iterator it = RenderTargets.begin();

	for( ; it!=RenderTargets.end(); it++ )
	{
		(*it)->Restore();
	}
}

void TextureManager::ClearRTList()
{
	RenderTargets.clear();
}