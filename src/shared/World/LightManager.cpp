#include "LightManager.h"
#include "world.h"

bool Light::LightsObject( Real ox, Real oy, Real oz, Real oRadius )
{
	Vector3D DistanceVector;
	DistanceVector.x = ox - x;
	DistanceVector.y = oy - y;
	DistanceVector.z = oz - z;

	Real distance = DistanceVector.Mag();
	if( distance <= oRadius || distance <= Radius )
		return true;

	return false;

}


bool Light::Visible( Screen3D &Screen )
{
	BoundingSphere BS;

	BS.x( x );
	BS.y( y );
	BS.z( z );
	BS.Radius = Radius+10;
	
	return Screen.SphereInFrustum( BS );
}


LightManager::LightManager() 
{
	LightmapGenerated = false; 
	m_width = 0; 
	m_height = 0; 
	m_opacity = NULL; 
}

LightManager::~LightManager()
{
	if( m_opacity )
		delete[] m_opacity; 
}

void LightManager::ReadDataFromLightmapTexture()
{

	m_width  = TerrainLightmap.GetWidth();
	m_height = TerrainLightmap.GetHeight();

	m_opacity = new float[m_width*m_height];

	BYTE* data = TerrainLightmap.ReadLock();

	for ( int y=0; y<m_height; y++)
	{
		for ( int x=0; x<m_width; x++)
		{
			m_opacity[x+y*m_width] = (float)(data[(x+y*m_width)*4+1]) / 255.0f;
		}
	}
	TerrainLightmap.ReadUnlock();
}

bool LightManager::LoadLightmap( Screen3D& Screen, const char* lightmap )
{
	LightmapGenerated = true;
	if( !TerrainLightmap.LoadTexture( &Screen, lightmap )  )
	{
		return false;
	}

	ReadDataFromLightmapTexture();

	return true;
}


void LightManager::RegisterLight(Light* NewLight)
{
	LightList.push_back(NewLight);
}

void LightManager::UnregisterLight(Light* DeadLight)
{
	for(unsigned int i=0; i<LightList.size(); i++)
	{
		if(LightList[i] == DeadLight)
			LightList.erase( LightList.begin() + i );

	}
}


void LightManager::UnlightAll( Screen3D& Screen )
{
	//Clear all the lights
	for(int l=1; l<8; l++)
	{
		Screen.D3DDevice->LightEnable( l, FALSE );
	}
}

void LightManager::LightScene( Screen3D& Screen )
{
	//Clear all the lights
	for(int l=1; l<8; l++)
	{
		Screen.D3DDevice->LightEnable( l, FALSE );
	}


	//Find all visible lights and assign them to d3d lights
	int D3DLightIndex=1;
	for(unsigned int i=0; i<LightList.size(); i++)
	{
		if(LightList[i]->Visible(Screen) && D3DLightIndex < 8)
		{
			D3DLIGHT9 light;
			ZeroMemory( &light, sizeof(D3DLIGHT9) );
			light.Type       = D3DLIGHT_POINT;
			light.Diffuse.r  = LightList[i]->r;
			light.Diffuse.g  = LightList[i]->g;
			light.Diffuse.b  = LightList[i]->b;
			light.Ambient.r  = LightList[i]->r/7;
			light.Ambient.g  = LightList[i]->g/7;
			light.Ambient.b  = LightList[i]->b/7;
			light.Range = LightList[i]->Radius;
			light.Falloff = 1;
			light.Position.x = LightList[i]->x;
			light.Position.y = LightList[i]->y;
			light.Position.z = LightList[i]->z;

			light.Attenuation0 = .5f;
			//light.Attenuation1 = 1;
		
			Screen.D3DDevice->SetLight( D3DLightIndex, &light );
			Screen.D3DDevice->LightEnable( D3DLightIndex, TRUE );

			D3DLightIndex++;
		}
			

	}
	
}

void LightManager::LightObject( Screen3D& Screen, Real x, Real y, Real z, Real Radius )
{
	//Clear all the lights
	for(int l=1; l<8; l++)
	{
		Screen.D3DDevice->LightEnable( l, FALSE );
	}


	//Find all visible lights and assign them to d3d lights
	int D3DLightIndex=1;
	for(unsigned int i=0; i<LightList.size(); i++)
	{
		if(LightList[i]->LightsObject( x, y, z, Radius ) && D3DLightIndex < 8)
		{
			D3DLIGHT9 light;
			ZeroMemory( &light, sizeof(D3DLIGHT9) );
			light.Type       = D3DLIGHT_POINT;
			light.Diffuse.r  = LightList[i]->r;
			light.Diffuse.g  = LightList[i]->g;
			light.Diffuse.b  = LightList[i]->b;
			light.Ambient.r  = LightList[i]->r/3;
			light.Ambient.g  = LightList[i]->g/3;
			light.Ambient.b  = LightList[i]->b/3;
			light.Range = LightList[i]->Radius;
			light.Falloff = 1;
			light.Position.x = LightList[i]->x;
			light.Position.y = LightList[i]->y;
			light.Position.z = LightList[i]->z;

			light.Attenuation0 = .5f;
			//light.Attenuation1 = 1;
		
			Screen.D3DDevice->SetLight( D3DLightIndex, &light );
			Screen.D3DDevice->LightEnable( D3DLightIndex, TRUE );

			D3DLightIndex++;
		}
			

	}
}