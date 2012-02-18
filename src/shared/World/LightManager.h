// I N C L U D E S /////////////////////////////////////////////
#pragma once
#include <vector>
#include "..\s3dlib\screen.h"
#include "..\s3dlib\types.h"

// D E F I N E S //////////////////////////////////////////////
class WorldManager;

// C L A S S E S //////////////////////////////////////////////

struct Light
{
	Real x, y, z;
	Real r, g, b;
	Real Radius;

	int CurrentD3DLight;

	bool Visible( Screen3D& Screen );
	bool LightsObject( Real ox, Real oy, Real oz, Real oRadius );
};



class LightManager
{
protected:
	std::vector<Light*> LightList;
	
	D3DLIGHT9 D3DLightList[32];

	Texture TerrainLightmap;
	Position MapCenter;

	bool LightmapGenerated;

	int m_width, m_height;
	float* m_opacity; 

	void ReadDataFromLightmapTexture();

public:
	LightManager();
	~LightManager();
	
	bool LoadLightmap ( Screen3D &Screen, const char* lightmap );

	void RegisterLight(Light* NewLight);
	void UnregisterLight(Light* DeadLight);
	void UnlightAll( Screen3D& Screen );

	int Width() { return m_width; }
	int Height() { return m_height; }
	float LightValue( int x, int y ) { return m_opacity[x+y*m_width]; }

	Texture* GetTexture() { return &TerrainLightmap; }

	void SetLightmapGenerated( bool tf ) { LightmapGenerated = tf; }

	void LightScene( Screen3D& Screen );

	void LightObject( Screen3D& Screen,  Real x, Real y, Real z, Real Radius );

};