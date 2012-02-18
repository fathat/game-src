#include "mp3.h"

Mp3Player Mp3;

bool Mp3Player::Init(HWND hWnd )
{
	// intialize default device
	
	if (!BASS_Init(-1,44100, NULL, hWnd, NULL)) 
	{
	    // couldn't initialize device, so use no sound
	    BASS_Init(0,44100,0,hWnd, NULL);
	}
	//BASS_SetVolume( 5 );
	BASS_Start();
       
    return true;
}

void Mp3Player::Shutdown()
{
	BASS_Free();

}

void Mp3Player::Run()
{
	BASS_ChannelPlay( Mp3Stream, FALSE);
}

void Mp3Player::Pause()
{
	BASS_ChannelPause( Mp3Stream );
}

void Mp3Player::PlayFromStart() 
{
	
	BASS_ChannelPlay( Mp3Stream, TRUE);
}

bool Mp3Player::LoadFile( char* mp3file )
{
	if(Mp3Stream )
	{
		//BASS_ChannelStop(Mp3Stream);
		BASS_StreamFree(Mp3Stream);
	}
	Mp3Stream = BASS_StreamCreateFile(FALSE,mp3file,0,0,0);

	if(Mp3Stream)
		return true;   

	return false;
}