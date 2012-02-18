// I N C L U D E S ///////////////////////////////////////
#pragma once
#include "Window.h"

// D E F I N E S /////////////////////////////////////////


// C L A S S E S /////////////////////////////////////////

//////// Button ///////////////////////////////////////////////////////////////
//
// Description:
//
// Simple button class. 
//
// TODO:
//
// - [none]
//
// Known Bugs:
//
// - [none]
//
///////////////////////////////////////////////////////////////////////////////
class Button : public GuiWindow
{
protected:
	bool  Depressed;

	TextureHandle BackgroundInactive;

	void InitEvent( );
	void ClickEvent(int x, int y);
	void PaintEvent( Screen3D& Screen );

public:

	Button() { Text = "No Name";  }
		
	void Resize ( Rect2D& NewRect ) { OnMove( (int)NewRect.left, (int)NewRect.top, (int)(NewRect.right-NewRect.left), (int)(NewRect.bottom-NewRect.top)); }
};


//////// WindowFrame //////////////////////////////////////////////////////////
//
// Description:
//
// Provides a frame around a window that can be used to close the window, resize
// it, etc.
//
// TODO:
//
// - [none]
//
// Known Bugs:
//
// - [none]
//
///////////////////////////////////////////////////////////////////////////////

class WindowFrame : public GuiWindow
{
protected:
	TextureHandle Border;
	TextureHandle Title;
	TextureHandle SizeButton;

	Button CloseButton;

	WindowHandle FramedWindow;


	void InitEvent( );
	void ClickEvent(int x, int y);
	void PaintEvent( Screen3D& Screen );
public:

	WindowFrame() { Text = "No Title"; }

};

//////// Dialog /////////////////////////////////////////////////////////
//
// Description:
//
// Window made for the sole purpose of containing other windows.
//
// TODO:
//
// - [none]
//
// Known Bugs:
//
// - [none]
//
///////////////////////////////////////////////////////////////////////////////

class DialogBox : public GuiWindow
{
protected:
	TextureHandle BackgroundInactive;
	WindowFrame WinControls;

	void InitEvent( );
	void PaintEvent( Screen3D& Screen );

public:
	void  SetText( std::string newText ) ;
	char* GetText( ) ;
};


//////// StaticBox //////////////////////////////////////////////////////
//
// Description:
//
// Window that just displays text.
//
// TODO:
//
// - [none]
//
// Known Bugs:
//
// - [none]
//
///////////////////////////////////////////////////////////////////////////////

class StaticBox : public GuiWindow
{
protected:
	
	void InitEvent( );
	void PaintEvent( Screen3D& Screen );
public:
	StaticBox() { Text = "static"; }
};