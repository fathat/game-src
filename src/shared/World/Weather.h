#pragma once
// I N C L U D E S ///////////////////////////////////////
#include "..\s3dlib\screen.h"
#include "..\s3dlib\types.h"
#include "Sky.h"
#include "Particle.h"


// D E F I N E S ////////////////////////////////////////
class WorldManager;
#define CHANGE_TIME 8000



// C L A S S E S ////////////////////////////////////////

class Weather  
{
public:
	struct WeatherState
	{
		Real ViewingDistance;
		Real FogStart;
		Real FogEnd;
		Real R,G,B;//what color to clear the back buffer
		Real FR,FG,FB;//what color the fog is
		Real SR,SG,SB;//what color the sun is.
		Real AR,AG,AB;//ambient color (applies only to objects)
		Vector3D Wind;//wind vector	

		ParticleStream* Stream;

		WeatherState();
	};

	Sky SkyBox;

	WorldManager* WMPtr;

	WeatherState CurrentState;
	WeatherState IntermediateState;
	WeatherState* FutureState;

	Real ChangeTimeSpent;
	Real TotalChangeTime;


	Weather();
	~Weather();
	bool Update(Real FrameTime, Position &Player);
	bool Draw(Screen3D &Screen,Camera &Viewer);
	bool SetRenderStates(Screen3D &Screen, bool LightingOnly=false);
	void SetVertexShaderConstants( Screen3D &Screen );

	
	bool InitDefaults( WorldManager* wm );
	bool Init(Screen3D &Screen, char *WeatherFile,Position &Player, WorldManager* WM);
	void Cleanup();

	void ChangeState( Screen3D& Screen,Position &Player,  char* WeatherFile );

};

