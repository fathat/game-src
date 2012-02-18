#pragma once

#include "..\s3dlib\screen.h"
#include "..\s3dlib\vertexbuffer.h"
#include "..\s3dlib\types.h"
#include "..\s3dlib\camera.h"
#include "..\ini\ini.h"
#include "..\xml\xml.h"


#define SKY_EXTENTS 10

class Sky
{
protected:
	friend class WorldManager;

	TextureHandle TopTexture, BottomTexture;
	TextureHandle North, South, East, West;
	TextureHandle BlendTexture;
	
public:
	
	bool Init ( Screen3D& Screen, char* WeatherFilename );
	bool Init( Screen3D* screen, TiXmlElement* skynode );
	void Draw ( Screen3D& Screen, Camera& Viewer, Real r, Real g, Real b );
	void DrawReflected( Screen3D& Screen, Camera& Viewer, Real r, Real g, Real b );
	void DrawLensFlare(Screen3D& Screen, Camera& Viewer );
};