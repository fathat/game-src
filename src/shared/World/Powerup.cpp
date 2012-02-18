#include "Powerup.h"
#include "..\Ini\ini.h"

Powerup::Powerup() : StaticObject()
{
	statToChange  = ST_HEALTH;
	powerupAmount = 0;
}

Powerup::~Powerup()
{
}


void Powerup::Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM, char* ObjectFile )
{
	

	statToChange = (StatType)GetIniValue( ObjectFile, "powerup", "type" );
	powerupAmount= GetIniValueFloat( ObjectFile,  "powerup", "amount" );

	return StaticObject::Init( Screen, WM, MM, AM, ObjectFile );

}