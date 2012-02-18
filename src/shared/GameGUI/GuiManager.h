#pragma once

// I N C L U D E S /////////////////////////////////////////////////
#include "window.h"
#include "..\..\shared\s3dlib\types.h"

// D E F I N E S ///////////////////////////////////////////////////
#define MAX_WINDOWS 1024

class WorldManager;

// C L A S S E S ///////////////////////////////////////////////////

class GUIManager
{
protected:

	//pointers to important windows
	GuiWindow* WindowList[MAX_WINDOWS];
	WindowHandle Focus; 

	Screen3D* ScreenPtr;
	FontManager* FM;
	WorldManager* WM;

	//screen/mouse stuff
	ScreenVertex SV[4];
	TextureHandle CursorHandle;
	POINT CursorPoint;

	int HandleCount;

	friend class GuiWindow;

	bool GuiActive;

public:

	GUIManager();
	~GUIManager();

	void Init( Screen3D& Screen, FontManager* fm, WorldManager* wm );

	WindowHandle FactoryCreateWindow(char* nametype, char* id, Rect2D& WinRect);

	//window adding functions
	WindowHandle AddWindow( GuiWindow* Window, WindowHandle Parent, bool AllowOwnership=true );
	GuiWindow* FindWindow( WindowHandle Window );
	GuiWindow* FindWindow( char* name);
	GuiWindow* GetRootWindow( ) { return WindowList[0]; }
		
	bool GetState( ) { return GuiActive; }
	void SetState( bool active ) { GuiActive = active; }

	
	//messages
	void MouseClicked ( int x, int y, int button );
    void KeyPressed( char KeyPressed, int VirtualKey=-1 );
	void DrawGUI(Screen3D& Screen);

};