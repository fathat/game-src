// I N C L U D E S ///////////////////////////////////////////
#pragma once
#include <windows.h>
#include <tchar.h>
#include <malloc.h>
#include <bass.h>

// D E F I N E S /////////////////////////////////////////////

// C L A S S E S /////////////////////////////////////////////

class Mp3Player
{
protected:
	HSTREAM Mp3Stream;

public:
	Mp3Player() {  
		Mp3Stream = NULL;
	}
	~Mp3Player() {
		Shutdown();
	}

	bool Init( HWND hWnd);
    void Shutdown();

	void Run();
	void PlayFromStart() ;
	void Pause();
	
	
	bool LoadFile( char* mp3file );
};

extern Mp3Player Mp3;