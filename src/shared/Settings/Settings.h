#pragma once

enum DetailSetting  { LowDetail, MediumDetail, HighDetail };

struct GameSettings
{
	unsigned int  m_textureFiltering;
	DetailSetting m_textures;

	bool m_drawGrass;
    	
	GameSettings();
	
};


//global game settings
extern GameSettings g_gameSettings;