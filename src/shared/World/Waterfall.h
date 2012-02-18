// I N C L U D E S /////////////////////////////
#include "sceneobject.h"



// C L A S S E S ///////////////////////////////

struct WaterFall : public SceneObject
{
public:

	void Update( Real FrameTime, WorldManager* SM );
	void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );

};
