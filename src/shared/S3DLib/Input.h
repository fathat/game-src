// I N C L U D E S ////////////////////////////////
#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>



// D E F I N E S /////////////////////////////////
#define MOUSE_INPUTBUFFER_SIZE 200
#ifndef Real
#define Real float
#endif

//Actions
#define INPUT_FORWARD     1L
#define INPUT_BACK        2L
#define INPUT_TURNLEFT    3L
#define INPUT_TURNRIGHT   4L
#define INPUT_STRAFELEFT  5L
#define INPUT_STRAFERIGHT 6L
#define INPUT_FIRE1       7L
#define INPUT_FIRE2       8L
#define INPUT_CROUCH      9L
#define INPUT_JUMP        10L
#define INPUT_USE         11L
#define INPUT_MENU        12L


// C L A S S E S /////////////////////////////////

struct KeyMapping
{
	bool Keyboard;
	int  DeviceMapping;

};


struct MouseInput
{
	Real MouseDeltaX, MouseDeltaY;
};


//////// Input ////////////////////////////////
//
// Description:
//
// Class for getting mouse and keyboard data.
// This should be extended for joysticks and
// buffered input.
//
// Known Problems:
//
// - None
//
// TODO:
//
// - Add in joystick support, buffer input
//
//
///////////////////////////////////////////////

class Input
{
protected:
	bool isInitialized;
	int mx, my;

	LPDIRECTINPUT8 lpDI;
	LPDIRECTINPUTDEVICE8 lpDIKeyboard;
	LPDIRECTINPUTDEVICE8 lpDIMouse;
	DIMOUSESTATE MouseState;
	UCHAR        KeyState[255];
	
	HWND HandleWnd; //hwnd and hInst used to recreate object if input is lost
	HINSTANCE HandleInst; 

	bool DirectInputMouse;
public:
	Input();
	~Input();

	bool Create(HWND hwnd, HINSTANCE hInst);
	void Destroy();


	void  Unaquire();
	void  Reaquire();
	void  PollData();

	UCHAR* GetKeyData() {
		return KeyState;
	}

	POINT GetMouseDelta() ;
	bool  GetMouseData( DIDEVICEOBJECTDATA* dataArray, DWORD* nElements );

	bool  LeftButtonDown() {
		if(MouseState.rgbButtons[0] )
			return true;
		else
			return false;
	}
	bool  RightButtonDown(){
		if(MouseState.rgbButtons[1] )
			return true;
		else
			return false;
	}

	bool  IsInitialize() { return isInitialized; }
};
