// I N C L U D E S ////////////////////////////////////////////////
#include "SystemObjects.h"


// S T A T I C ////////////////////////////////////////////////////
//Declarations of static members
Screen3D DeviceManager::Screen;
Input    DeviceManager::InputSystem;
SoundManager DeviceManager::Sound;
SoundManagerAL DeviceManager::OpenALSound;


DisplayModeInfo DeviceManager::DisplayMode;
bool			DeviceManager::WindowedMode;
bool			DeviceManager::HardwareVS;


// F U N C T I O N S /////////////////////////////////////////////


//Initializes most of the system objects we'll need for graphics and sound and blah blah
bool DeviceManager::InitSystemObjects( HWND hWnd, HINSTANCE hInstance )
{
	int ViewDistance = GetIniValue("..\\system\\system.ini", "map", "viewdistance");

	if( ! OpenALSound.Init( ) )
	{
		OutputMessage( "Could not initialize OpenAL" );
	}
	//Create the screen 
	Screen.DisplayMode =DisplayMode;
	if(! Screen.Create(hWnd, Screen.DisplayMode, (Real)ViewDistance) )
	{
		MessageBox( hWnd, "Direct3D could not be initialized.", "D3D Init Failed", MB_OK);
		return false;
	}
	
	Mp3.Init( hWnd );
	InputSystem.Create(hWnd, hInstance);
	Sound.Init();


    

	return true;
}