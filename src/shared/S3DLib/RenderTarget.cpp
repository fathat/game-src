#include "RenderTarget.h"
#include "screen.h"




bool RenderTarget::Init(Screen3D* Screen, const char* name, int newWidth, int newHeight, D3DFORMAT format)
{
	ScreenPtr = Screen;
	Destroy();

	HRESULT hResult;

	strcpy(TextureFile, name);
	targetWidth  = newWidth;
	targetHeight = newHeight;
	targetFormat = format;
	
	Restore();

	Screen->TM.RegisterRenderTarget( this );

	return true;
}

void RenderTarget::Destroy()
{
	ScreenPtr->TM.UnregisterRenderTarget( this );
	Invalidate();
}

void RenderTarget::Restore()
{

	if(TextureSurface)
		Invalidate();

	HRESULT HR = D3DXCreateTexture(
					ScreenPtr->D3DDevice, 
					targetWidth, targetHeight, 
					1, 
					D3DUSAGE_RENDERTARGET, 
					targetFormat, 
					D3DPOOL_DEFAULT, 
					&TextureSurface);

	if( FAILED (HR ) )
		OutputDXError( HR, "Failed to restore render target: " );
		
	
	ReadTextureInfo();
	
}

void RenderTarget::Invalidate()
{
	if(TextureSurface)
		TextureSurface->Release();
	TextureSurface = NULL;
}