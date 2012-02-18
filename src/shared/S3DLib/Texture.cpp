#include "screen.h"
#include "..\..\shared\imageloader\image.h"

#define GetBit(bits, bitposition) (((1<<bitposition)&bits)>>bitposition)
#define SetBit(bits,bit, bitposition) (bits|=(bit<<bitposition))

int Texture::TotalMemoryUsed = 0;

Texture::Texture() 
{ 
	TextureSurface = NULL; 
	ScreenPtr	   = NULL;  
	IsFile		   = false; 
	Width		   = 0;
	Height		   = 0;
	LODDivisor     = 0;

	strcpy( TextureFile, "[null]" );
	strcpy( AlphaFile, "[null]" );
}

Texture::~Texture() 
{ 
	Destroy(); 
}


bool Texture::Init(Screen3D* Screen, const char* name, int newWidth, int newHeight, D3DFORMAT format)
{
	Destroy();

	DWORD Usage = 0;
	DWORD MipChains=0;
	D3DPOOL Pool = D3DPOOL_MANAGED;

	HRESULT hResult;

	strcpy(TextureFile, name);
	
	if( FAILED( hResult= D3DXCreateTexture(Screen->D3DDevice, newWidth, newHeight, MipChains, Usage, format, Pool, &TextureSurface) ) )
	{
		
		return false;
	}

	ReadTextureInfo();

	return true;
}


bool Texture::LoadTexture(Screen3D* Screen, const char *filename, const char* alphamask, D3DFORMAT format)
{
	D3DXIMAGE_INFO Info;
	Destroy();

	std::string pngfilename;
	int l = strlen(filename);
	if(filename[l-1] == 'p' 
		&& filename[l-2] == 'm' 
		&& filename[l-3] == 'b'
		&& filename[l-4] == '.')
	{
		pngfilename = filename;
		pngfilename[l-3] = 'p';
		pngfilename[l-2] = 'n';
		pngfilename[l-1] = 'g';
	}

	if( FAILED( D3DXCreateTextureFromFileExA(Screen->D3DDevice,
		filename,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		format,
		D3DPOOL_MANAGED,
		D3DX_FILTER_BOX  ,
		D3DX_DEFAULT,
		0,
		&Info,
		NULL,
		&TextureSurface) ) )
	{
		if( pngfilename.empty() || FAILED( D3DXCreateTextureFromFileExA(Screen->D3DDevice,
			pngfilename.c_str(),
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			0,
			format,
			D3DPOOL_MANAGED,
			D3DX_FILTER_BOX  ,
			D3DX_DEFAULT,
			0,
			&Info,
			NULL,
			&TextureSurface) ) )
		{
			OutputDebugString("Could not load ");
			OutputDebugString(filename);
			OutputDebugString("\n");
			return false;
		}
	}

	ScreenPtr = Screen;
	IsFile = true;

	Width  = Info.Width;
	Height = Info.Height;
	Format = Info.Format;

	//if(Width > 256 )
	//	TextureSurface->SetLOD( 2 );
	//this->LimitUploadSize( 512 );

	strcpy(TextureFile, filename);

	if(alphamask && strlen(alphamask) > 0 )
	{
		
		OutputDebugString( filename );
		OutputDebugString( "->" );
		if(AddAlpha(alphamask))
			HasAlpha = true;
		else
			HasAlpha = false;
		
		strcpy(AlphaFile, alphamask);
		
	}
	else
	{
		strcpy(AlphaFile, "null");
		HasAlpha = false;
	}



	Texture::TotalMemoryUsed += GetTextureMemorySize();
			
	return true;
}


void Texture::ReadTextureInfo()
{
	IDirect3DSurface9* level0;
	D3DSURFACE_DESC desc;
	TextureSurface->GetSurfaceLevel(0, &level0 );
	level0->GetDesc( &desc );
	level0->Release();

	IsFile = false;



	Width = desc.Width;//newWidth;
	Height = desc.Height;//newHeight;
	Format = desc.Format;
	HasAlpha = false;

	Texture::TotalMemoryUsed += GetTextureMemorySize();

}

int Texture::GetTextureMemorySize()
{
	
	int formatSize=0;
	if(Format == D3DFMT_A8R8G8B8 || Format == D3DFMT_X8R8G8B8 )
		formatSize = 4;
	else if(Format == D3DFMT_R8G8B8)
		formatSize = 3;
	else if(Format == D3DFMT_R5G6B5 || Format == D3DFMT_X1R5G5B5 || Format == D3DFMT_A1R5G5B5 || Format == D3DFMT_A4R4G4B4 || Format == D3DFMT_A8R3G3B2 )
		formatSize = 2;
	else if(Format == D3DFMT_A8)
		formatSize = 1;
	else
		formatSize = 4;

	return ((Width/(LODDivisor+1))*(Height/(LODDivisor+1))*formatSize);
	
}

void Texture::LimitUploadSize ( int texSize )
{
	LODDivisor = Width/texSize;
	if( LODDivisor > 0 && TextureSurface)
		TextureSurface->SetLOD( LODDivisor );
}

void Texture::UnlimitSize() 
{
	if(TextureSurface)
		TextureSurface->SetLOD( 0 );
}

bool Texture::AddAlpha(const char* filename)
{
	CGenImage AlphaImage;

	OutputDebugString( "\nAdding Alpha Mask: " );
	
	char pngfilename[255];
	int l = strlen(filename)-4;
	for(int i=0; i<l; i++)
	{
		pngfilename[i] = filename[i];
	}
	strcpy((char*)(pngfilename+l), ".png");
	

	OutputDebugString( filename );
	
	if(FAILED(AlphaImage.Load(filename)))
	{
		OutputDebugString( "  ...bad\n" );
		return false;
	}

	OutputDebugString( "  ...good\n" );
	Width  = AlphaImage.GetWidth ();
	Height = AlphaImage.GetHeight();

	ARGB_PIXEL* Bits = AlphaImage.GetBits();


	//Figure out how many "levels" (mip-maps) there are
	//in the texture
	int nLevels = TextureSurface->GetLevelCount();
	D3DSURFACE_DESC SurfDesc;
	D3DLOCKED_RECT  LockedArea;
	BYTE* Surface;

	int DivFactor=1;

	//loop through each level
	for(int l=0; l<nLevels; l++)
	{
		//Get the surface's attributes
		TextureSurface->GetLevelDesc(l, &SurfDesc);

		//lock down a pointer
		TextureSurface->LockRect(l, &LockedArea, NULL, NULL);

		//loop through each pixel in the surface
		for(unsigned int y=0; y<SurfDesc.Height; y++)
		{
			for(unsigned int x=0; x<SurfDesc.Width; x++)
			{
				BYTE Alpha = Bits[(x*DivFactor)+(y*DivFactor)*Width].b;
				Surface = (BYTE*)LockedArea.pBits;

				if(SurfDesc.Format == D3DFMT_A8R8G8B8)
				{
					Surface[(x*4)+3+y*(SurfDesc.Width*4)] = Alpha;
				}
				else if(SurfDesc.Format == D3DFMT_A4R4G4B4)
				{
					SHORT* ShortSurface = (SHORT*)LockedArea.pBits;

					Alpha = (BYTE)( ((Real)Alpha / 255.0f) * 32.0f);

					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Alpha, 0),12);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Alpha, 1),13);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Alpha, 2),14);
					SetBit( ShortSurface[x+y*(SurfDesc.Width)], GetBit(Alpha, 3),15);
					
					//Surface[x*2+1+y*(SurfDesc.Width*2)] &= (Alpha<<4) ;
					
					
				}

				else if(SurfDesc.Format == D3DFMT_A1R5G5B5)
				{
					if(Alpha > 100)
						Alpha = 1;
					else
						Alpha = 0;
					Surface[x*2+1+y*(SurfDesc.Width*2)] ^= (255-Alpha)<<7;
					
				}
				
			}
		}

		DivFactor *=2;

		//unlock the surface
		TextureSurface->UnlockRect(l);
	}

	HasAlpha = true;

	return true;
}


void Texture::Destroy() 
{ 
	if(TextureSurface)
	{
		Texture::TotalMemoryUsed -= GetTextureMemorySize();

		TextureSurface->Release();
	}

	TextureSurface = NULL;
}

bool Texture::Reload()
{
	if( ScreenPtr )
	{
		if(IsFile)
			return LoadTexture(ScreenPtr, GetFilename(), AlphaFile);
		else
			Init(ScreenPtr, TextureFile, Width, Height); 
	}

	return true;
}


BYTE* Texture::ReadLock()
{
	//Declarations
	D3DSURFACE_DESC SurfDesc;
	D3DLOCKED_RECT  LockedArea;

	//Get the surface's attributes
	TextureSurface->GetLevelDesc(0, &SurfDesc);

	//lock down a pointer
	TextureSurface->LockRect(0, &LockedArea, NULL, NULL);
	
	Pitch = LockedArea.Pitch;
	return (BYTE*)LockedArea.pBits;

}

void  Texture::ReadUnlock()
{
	TextureSurface->UnlockRect(0);
}


