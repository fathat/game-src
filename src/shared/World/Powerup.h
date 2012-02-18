#pragma once
#include "StaticObject.h"

enum StatType  { ST_HEALTH=0, ST_AMMO=1, ST_MONEY=2 };

class Powerup : public StaticObject
{
protected:
	StatType statToChange;
	float    powerupAmount ;

public:

	Powerup();
	~Powerup();

	void Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM, char* ObjectFile );

	StatType StatToChange() const { return statToChange ; } 
	float    ChangeAmount() const { return powerupAmount; } 
};