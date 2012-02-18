// I N C L U D E S /////////////////////////////////////////////
#pragma once

#include "..\shared\s3dlib\screen.h"
#include "..\shared\s3dlib\audio.h"
#include "..\shared\s3dlib\soundmanager.h"
#include "..\shared\s3dlib\input.h"
#include "..\shared\s3dlib\mp3.h"
#include "..\shared\mesh\mesh.h"
#include "..\shared\gamegui\InputBox.h"
#include "..\shared\ini\ini.h"

// C L A S S E S //////////////////////////////////////////////


//Keeps track of device objects the game uses for things like
//graphics and sound and whatnot.
class DeviceManager
{
public:
	static Screen3D Screen;
	static Input    InputSystem;
	static SoundManager Sound;
	

	static DisplayModeInfo DisplayMode;
	static bool WindowedMode;
	static bool HardwareVS;

	static bool InitSystemObjects ( HWND hWnd, HINSTANCE hInstance );

	static SoundManagerAL OpenALSound;
};