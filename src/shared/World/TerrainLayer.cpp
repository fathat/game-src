#include "TerrainLayer.h"
#include "World.h"


TerrainLayer::TerrainLayer(WorldManager* wmptr, const char* name, const char* textureFilename) 
{
	Name = name;
	TextureFilename = textureFilename;

	char maskFile[1024];
	sprintf( maskFile, "..\\%s\\maps\\%s\\layer_%s.png", wmptr->ModName.c_str(), wmptr->MapName.c_str(), name );
	

	ImageTexture = wmptr->ScreenPtr->GetTextureHandle( (char*)textureFilename );
	AlphaMask.LoadTexture( wmptr->ScreenPtr, maskFile);

	BYTE* buffer = AlphaMask.ReadLock();
	m_width  = AlphaMask.GetWidth();
	m_height = AlphaMask.GetHeight();

	m_opacity = new float[ m_width  * m_height ];
    
	for ( int y=0; y< m_height; y++ )
	{
		for ( int x=0; x< m_width; x++ )
		{
			float r = buffer[(x+y*m_width)*4 + 0];
			float g = buffer[(x+y*m_width)*4 + 1];
			float b = buffer[(x+y*m_width)*4 + 2];
			float a = buffer[(x+y*m_width)*4 + 3];
			m_opacity[x+y*m_width] = g/255.0f;
		}

	}

	AlphaMask.ReadUnlock();
	
}