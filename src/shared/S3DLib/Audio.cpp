#include "Audio.h"
#include <string.h>
#include "..\defs\defs.h"


bool SoundFile::LoadFile(char *filename, SoundManager* SM )
{
	
/*
	strcpy(Filename, filename);



	WCHAR wstrFileName[MAX_PATH] ;
	MultiByteToWideChar( CP_ACP, 0, filename, -1, 
                         wstrFileName, MAX_PATH );

 
    if (FAILED(SM->Loader->LoadObjectFromFile(
        CLSID_DirectMusicSegment,   // Class identifier.
        IID_IDirectMusicSegment8,   // ID of desired interface.
        wstrFileName,               // Filename.
        (LPVOID*) &Segment       // Pointer that receives interface.
    )))
    {
		return false;
    }

*/



	return true;
}

void SoundFile::Shutdown()
{
/*    if(Segment)
		Segment->Release();
	Segment = NULL;*/
}



bool SoundManager::Init()
{

	return false;
/*	//Clear out any old instances
	Shutdown();

	//Create the loader and performance
	CoInitialize(NULL);
    
	CoCreateInstance(CLSID_DirectMusicLoader, NULL, 
                     CLSCTX_INPROC, IID_IDirectMusicLoader8,
                     (void**)&Loader);

	CoCreateInstance(CLSID_DirectMusicPerformance, NULL,
                     CLSCTX_INPROC, IID_IDirectMusicPerformance8,
                     (void**)&Performance );


	// Initialize the performance with the standard audio path.
    // This initializes both DirectMusic and DirectSound and 
    // sets up the synthesizer. 
    Performance->InitAudio( NULL, NULL, NULL, 
                               DMUS_APATH_SHARED_STEREOPLUSREVERB, 64,
                               DMUS_AUDIOF_ALL, NULL );


	return true;
*/

}

void SoundManager::Shutdown()
{
	for(int i=0; i<MAX_SOUNDS; i++)
	{
		if(SoundList[i])
		{
			delete SoundList[i];
			SoundList[i] = NULL;
		}
	}
/*	if(Performance)
	{
		Performance->Stop(
		    NULL,   // Stop all segments.
		    NULL,   // Stop all segment states.
		    0,      // Do it immediately.
		    0       // Flags.
		);
 
		Performance->CloseDown();


	
		Performance->Release();
	}

	if(Loader)
		Loader->Release();

	Performance = NULL;
	Loader = NULL;
*/	

}


SoundHandle  SoundManager::AddSound ( char* Filename )
{
/*	for(int i=0; i<MAX_SOUNDS; i++)
	{
		if(SoundList[i] == NULL)
		{
			SoundList[i] = new SoundFile;
			if(!SoundList[i]->LoadFile( Filename, this) )
			{
				delete SoundList[i];
				SoundList[i] = NULL;
				return -1;
			}
				

			//first sound added, download it
			SoundList[i]->Segment->Download( Performance );

			
			return i;
		}
	}
*/
	return -1;

}

void SoundManager::RemoveSound ( SoundHandle ID ) 
{
	
	if(SoundList[ID])
	{
		delete SoundList[ID];
		SoundList[ID] = NULL;
	
		return;
	}

}

SoundHandle SoundManager::SoundExists( char* FileName )
{
	for(int i=0; i<MAX_SOUNDS; i++)
	{
		if(SoundList[i])
		{
			if(strcmpi( FileName, SoundList[i]->Filename) == 0)
			{
				return i;
			}
		}
	}
	

	return -1;
}

SoundHandle SoundManager::GetSound( char* SoundName )
{
	
	SoundHandle returnvalue = SoundExists(SoundName);

	if(returnvalue == -1)
	{
		returnvalue = AddSound( SoundName );
	}
	
	return returnvalue;
}
	
void SoundManager::Play( SoundHandle sound )
{
	if(sound < 0 || sound > MAX_SOUNDS-1)
		return;

/*	if(SoundList[sound])
	{
		Performance->PlaySegmentEx(
			SoundList[sound]->Segment,  // Segment to play.
			NULL,        // Used for songs; not implemented.
			NULL,        // For transitions. 
			0,           // Flags.
			0,           // Start time; 0 is immediate.
			NULL,        // Pointer that receives segment state.
			NULL,        // Object to stop.
			NULL         // Audiopath, if not default.
		);      
		
	}
*/	
}