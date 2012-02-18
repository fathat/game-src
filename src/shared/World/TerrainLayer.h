#pragma once

#include <string>
#include "..\s3dlib\screen.h"

class WorldManager;

struct TerrainLayer 
{
public:
	TextureHandle ImageTexture;
	Texture		  AlphaMask;

	std::string Name;
	std::string TextureFilename;

	int m_width, m_height;
	float* m_opacity;

	TerrainLayer( WorldManager* wmptr, const char* name, const char* textureFilename);

	int Width() { return m_width; }
	int Height() { return m_height; }

	float GetOpacity( int x, int y ) { return m_opacity[ x+ y *m_width]; }
};