#pragma once
#include <windows.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "types.h"

using std::vector;
using std::list;
using std::map;
using std::string;

#define SoundHandleAL int
const int MaxSounds  = 256;

struct PlayingSound
{
	FMOD::Channel* channel;

	PlayingSound()
	{
		channel = NULL;
	}

	PlayingSound( FMOD::Channel* c )
	{
		channel = c;
	}

	void SetPitch( float pitch )
	{
		if(channel)
			channel->setFrequency( 11025*pitch );
	}
	void UpdateLocation(SoundSource& sourceData )
	{
		if(!channel) return;
		FMOD_VECTOR pos = { sourceData.x, sourceData.y, sourceData.z };
		FMOD_VECTOR vel = { sourceData.vx, sourceData.vy, sourceData.vz };
		channel->set3DAttributes(&pos, &vel);

	}
	void Stop() {
		if( channel )
		{
			channel->stop();
			channel = NULL;
		}
	}
};


class SoundManagerAL
{
protected:
	FMOD::System* m_system;
	map< string, FMOD::Sound* > m_bufferMap;


public:
	SoundManagerAL();
	~SoundManagerAL();

	bool Init();

	void Shutdown();

	void SetListener ( SoundSource& listenerData ) ;

	FMOD::Sound* GetSoundBuffer( const char* filename ) ;
	void PlaySoundImmediate( FMOD::Sound* buffer, SoundSource& sourceData  );
	PlayingSound PlayPersistentSound(FMOD::Sound* buffer, SoundSource& sourceData );

	int SoundsPlaying() { return 0 ;} 
	
	void Update();	
};