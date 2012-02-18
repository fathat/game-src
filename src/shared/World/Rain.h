#pragma once

#include "..\S3DLib\Types.h"
#include "..\S3DLib\VertexBuffer.h"

class WorldManager;

struct RainDrop
{
	Real x, y, z;
	Real x2, y2, z2;
	Real timeLeft;
	Real initialLifetime;
};

class Rain
{
	RainDrop* drops;
	int nRainDrops;

	WorldManager* wm;

	Real lifetime;

	VertexBuffer particleBuffer;

public:
	Rain(WorldManager* wmptr);
	~Rain();

	void Update(float frametime);
	void Draw();
};