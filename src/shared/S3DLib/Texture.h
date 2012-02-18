// I N C L U D E S //////////////////////////////////////////////
#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

// C L A S S E S ////////////////////////////////////////////////
class Screen3D;

//////// Texture //////////////////////////////
//
// Description:
//
// Represents a IDirectDrawSurface8 interface that's used
// as a Direct3D texture.
//
// Inherits From:
//
// - Nothing
//
// TODO:
//
// - Make bitmap loading code cleaner, more reliable.
//
// Known Bugs:
//
// - 
//
///////////////////////////////////////////////


class Texture
{
protected:

	//Static variable for keeping track of texture memory used
	static int TotalMemoryUsed;

	//Texture interface pointer 
	LPDIRECT3DTEXTURE9 TextureSurface;
	D3DFORMAT Format;

	Screen3D* ScreenPtr;

	//Filename
	char TextureFile[255];
	char AlphaFile[255];

	bool IsFile;
	bool HasAlpha;
	
	int Width, Height;
	int Pitch;

	int LODDivisor;


	friend class Screen3D;


	void ReadTextureInfo();

	
	
public:

	//constructor/destructor
	Texture();
	virtual ~Texture();


	//Function to load Textures
	virtual bool Init( Screen3D* Screen, const char* name, int newWidth=256, int newHeight=256, D3DFORMAT format=D3DFMT_A8R8G8B8);
	bool LoadTexture(Screen3D* Screen, const char *filename, const char* alphamask=NULL, D3DFORMAT format=D3DFMT_A8R8G8B8);
	bool Reload();
	bool AddAlpha( const char* filename);
	virtual void Destroy();

	D3DFORMAT GetTextureFormat( ) { return Format; }
	BYTE* ReadLock();
	void  ReadUnlock();

	bool ContainsAlpha() { return HasAlpha;}

	void LimitUploadSize ( int texSize );
	void UnlimitSize();

	//Accessor functions	
	char* GetFilename() { return TextureFile; }
	int   GetWidth() { return Width; }
	int   GetHeight() { return Height; }
	int   GetPitch() { return Pitch; }
	int   GetTextureMemorySize();

	static int GetTotalTextureMemory() { return TotalMemoryUsed; }
	
	LPDIRECT3DTEXTURE9 GetTextureSurface() { return TextureSurface; }

};
