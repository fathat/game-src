#include "Input.h"

#include "..\defs\defs.h"

Input::Input()
{
	isInitialized = false;
	DirectInputMouse = false;
	mx = 0;
	my = 0;

	lpDIMouse = NULL;
	lpDIKeyboard = NULL;

}

Input::~Input()
{
	Destroy();
}

bool Input::Create(HWND hwnd, HINSTANCE hInst)
{
	if(isInitialized == true) return true;
	HandleWnd  = hwnd;
	HandleInst = hInst;
	//init DirectInput
	if(DirectInput8Create(hInst,  DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDI, NULL) != DI_OK)
	{
		return false;
	}

	//Init keyboard
	if(lpDI->CreateDevice(GUID_SysKeyboard, &lpDIKeyboard, NULL) != DI_OK)
	{
		return false;
	}
	if(lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		return false;
	if(lpDIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		return false;
	
	if(lpDIKeyboard->Acquire()!=DI_OK)
		return false;

	//Init mouse
	if(lpDI->CreateDevice(GUID_SysMouse, &lpDIMouse, NULL) != DI_OK)
	{
		lpDIKeyboard->Unacquire();
		lpDIKeyboard->Release();
		return false;
	}
	if(lpDIMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
	{
		lpDIKeyboard->Unacquire();
		lpDIKeyboard->Release();
		return false;
	}
	if(lpDIMouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
	{
		lpDIKeyboard->Unacquire();
		lpDIKeyboard->Release();
		return false;
	}




	DIPROPDWORD dipdw;
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = MOUSE_INPUTBUFFER_SIZE;
	
	if (FAILED( lpDIMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph) )) {
		return false;
	}


	if(lpDIMouse->Acquire()!= DI_OK)
	{
		lpDIKeyboard->Unacquire();
		lpDIKeyboard->Release();
		return false;
	}

	isInitialized = true;
	
	return true;
}

void Input::Destroy()
{
	if(isInitialized == false) return;
	if(lpDI != NULL)
	{
		if(lpDIKeyboard != NULL)
		{
			lpDIKeyboard->Unacquire();
			lpDIKeyboard->Release();
			lpDIKeyboard = NULL;
		}
		if(lpDIMouse != NULL)
		{
			lpDIMouse->Unacquire();
			lpDIMouse->Release();
			lpDIMouse = NULL;
		}
		lpDI->Release();
		lpDI = NULL;
	}
	
	isInitialized = false;
}

bool Input::GetMouseData( DIDEVICEOBJECTDATA* dataArray, DWORD* nElements )
{
	if( !lpDIMouse ) return false;
	if( FAILED ( lpDIMouse->GetDeviceData(  sizeof(DIDEVICEOBJECTDATA), dataArray, nElements, 0 ) ) )
		return false;
	return true; 
}

POINT Input::GetMouseDelta() { //returns how far mouse has moved from last position

	//declaration
	POINT MousePoint;
	RECT WindowLocation;
	int centerx, centery;
	GetWindowRect(HandleWnd, &WindowLocation);

	//Figure out the coordinates of the center of the window
	centerx = WindowLocation.left + (WindowLocation.right-WindowLocation.left)/2;
	centery = WindowLocation.top  + (WindowLocation.bottom-WindowLocation.top)/2;
	
	//Return the delta
	if(!DirectInputMouse)
	{
		//If not using DirectInput, figure out the delta using the cursor offset
		POINT cursorposition;
		GetCursorPos( &cursorposition );
		MousePoint.x = (long)cursorposition.x-centerx;
		MousePoint.y = (long)cursorposition.y-centery;
		SetCursorPos( centerx, centery);
	}
	else
	{

		//return coordinates that were polled from direct input
		MousePoint.x = MouseState.lX ;
		MousePoint.y = MouseState.lY ;
		SetCursorPos( centerx, centery);
	}



	return MousePoint;
}



void Input::Unaquire()
{

	if( lpDIMouse )  lpDIMouse->Unacquire();
	if( lpDIKeyboard ) lpDIKeyboard->Unacquire();
}

void Input::Reaquire()
{
	if( lpDIMouse ) lpDIMouse->Acquire();
	if( lpDIKeyboard ) lpDIKeyboard->Acquire();
}

void Input::PollData()
{
	if(!isInitialized)
		return;
	
	if( FAILED( lpDIKeyboard->GetDeviceState(256, &KeyState) ) )
	{
		//Destroy();
		//Create(HandleWnd, HandleInst);
	}

	//lpDIMouse->GetDeviceData( 

	if(lpDIMouse)
	{
		if ( FAILED( lpDIMouse->GetDeviceState(sizeof(DIMOUSESTATE),(LPVOID)&MouseState) ) )
		{
			//Destroy();
			//Create(HandleWnd, HandleInst);
		}
	}
	else
	{
		//Destroy();
		//Create(HandleWnd, HandleInst);
	}
	mx += MouseState.lX;
	my += MouseState.lY;

}




