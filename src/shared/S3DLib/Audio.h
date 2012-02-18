// I N C L U D E S ///////////////////////////////////////
#pragma once

// D E F I N E S /////////////////////////////////////////
#define MAX_SOUNDS 200
#define SoundHandle int
class SoundManager;

// C L A S S E S /////////////////////////////////////////


struct SoundFile
{

	//IDirectMusicSegment8*     Segment;
	char Filename[256];

	SoundFile() { }
	~SoundFile() { Shutdown(); }

	bool LoadFile(char *filename, SoundManager* SM );
	void Shutdown();

};



class SoundManager
{
protected:
	SoundFile* SoundList[MAX_SOUNDS];

	/*IDirectMusicLoader8*      Loader ;
	IDirectMusicPerformance8* Performance ;*/

	friend struct SoundFile;

public:
	SoundManager() {
		//clear out the sound list
		for(int i=0; i<MAX_SOUNDS; i++)
		{
			SoundList[i] = 0;	
		}

	}
	~SoundManager() { Shutdown(); }

	bool Init();
	void Shutdown();


	SoundHandle  AddSound ( char* Filename ); //returns an ID for the texture
	void RemoveSound ( SoundHandle ID ) ; //remove the texture with specified ID
	SoundHandle SoundExists( char* FileName );

	SoundHandle GetSound( char* SoundName );
	
	void Play( SoundHandle sound );

};