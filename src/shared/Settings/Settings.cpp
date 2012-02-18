#include "Settings.h"
#include <d3d9.h>

GameSettings::GameSettings() 
{
	m_textureFiltering = D3DTEXF_ANISOTROPIC;
	m_textures		   = HighDetail;
	m_drawGrass		   = true;
}

GameSettings g_gameSettings;