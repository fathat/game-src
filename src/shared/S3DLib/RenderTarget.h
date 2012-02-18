#pragma once
#include "texture.h"

class RenderTarget : public Texture
{

protected:
	int targetWidth, targetHeight;
	D3DFORMAT targetFormat;
public:
	
	bool Init( Screen3D* Screen, const char* name, int newWidth=256, int newHeight=256, D3DFORMAT format=D3DFMT_A8R8G8B8);
	void Destroy();

	void Restore();
	void Invalidate();

};