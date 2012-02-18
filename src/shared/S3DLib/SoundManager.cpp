
#include "..\defs\defs.h"

#include "SoundManager.h"





SoundManagerAL::SoundManagerAL()
{
	m_system = NULL;
}

SoundManagerAL::~SoundManagerAL()
{
	Shutdown();
}

bool SoundManagerAL::Init()
{

	FMOD_RESULT result = FMOD::System_Create( &m_system );
	if (result != FMOD_OK)
	{
		return false;
	}

	result = m_system->init(  100, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, NULL);
	if (result != FMOD_OK)
	{
		return false;
	}


	result = m_system->set3DSettings(1.0, 1.0f, 1.0f);
	if (result != FMOD_OK)
	{
		return false;
	}

	return true;
}

void SoundManagerAL::Shutdown()
{

	if( m_system)
	{
		m_system->close();
		m_system->release();
	}
}

void SoundManagerAL::SetListener ( SoundSource& listenerData ) 
{

	FMOD_VECTOR pos			   = { listenerData.x, listenerData.y, listenerData.z  };
	FMOD_VECTOR forward        = { listenerData.ox, listenerData.oy, listenerData.oz  };
	FMOD_VECTOR up             = { 0.0f, 1.0f, 0.0f };
	FMOD_VECTOR vel			   = { listenerData.vx, listenerData.vy, listenerData.vz  };

	m_system->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
	

}

FMOD::Sound* SoundManagerAL::GetSoundBuffer( const char* filename ) 
{
	string key = filename;
	map< string, FMOD::Sound* >::iterator itor = m_bufferMap.find( key );
	if( itor == m_bufferMap.end() )
	{
		FMOD::Sound* snd=NULL; 
		if(m_system->createSound(filename, FMOD_HARDWARE | FMOD_3D, 0, &snd)  != FMOD_OK )
		{
			OutputMessage("Dios mio, couldn't create sound file!" ) ;
			
		}
		if( snd )
		{
			snd->set3DMinMaxDistance(30, 2000.0f);
			//snd->setMode(FMOD_LOOP_NORMAL);
		}
		m_bufferMap[key] = snd;
				
	}
	return m_bufferMap[key] ;
}

void SoundManagerAL::PlaySoundImmediate( FMOD::Sound* buffer, SoundSource& sourceData  )
{
	if(! buffer ) return ;
	FMOD_VECTOR pos = { sourceData.x, sourceData.y, sourceData.z };
	FMOD_VECTOR vel = { sourceData.vx, sourceData.vy, sourceData.vz };
	
	FMOD::Channel* channel;
	FMOD_RESULT result = m_system->playSound(FMOD_CHANNEL_FREE, buffer, true, &channel);
	result = channel->set3DAttributes(&pos, &vel);
	result = channel->setPaused(false);
	
}

PlayingSound SoundManagerAL::PlayPersistentSound(FMOD::Sound* buffer, SoundSource& sourceData )
{
	if(! buffer ) return false;
	FMOD_VECTOR pos = { sourceData.x, sourceData.y, sourceData.z };
	FMOD_VECTOR vel = { sourceData.vx, sourceData.vy, sourceData.vz };

	FMOD::Channel* channel;
	buffer->setMode( FMOD_LOOP_NORMAL );
	m_system->playSound(FMOD_CHANNEL_FREE, buffer, true, &channel);
	channel->set3DAttributes(&pos, &vel);
	channel->setPaused(false);
		
	return PlayingSound( channel );
}

void SoundManagerAL::Update()
{
	
	m_system->update() ;

}