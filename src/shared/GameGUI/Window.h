#pragma once

// I N C L U D E S ////////////////////////////////////////////////
#include "..\..\shared\s3dlib\screen.h"
#include "..\..\shared\s3dlib\font.h"
#include <list>
#include <string>

// D E F I N E S //////////////////////////////////////////////////
#define WindowHandle int
class GUIManager;

#define WINM_COMMAND         0
#define WINM_CONTENTSCHANGED 1
#define WINM_KEYPRESSED      2


enum WindowState { W_INACTIVE, W_ACTIVE, W_FOCUS };


// C L A S S E S /////////////////////////////////////////////////
class GuiWindow;

struct Event
{
	std::string ControlID;
	GuiWindow* Control;
	int  Message;
	int  Param1;
	int  Param2;
};


class GuiWindow
{
protected:
	//Message procedure variables
	UINT (*MessageFunc)(Event*);


public:
	//Boundaries of the window, only modified by an OnMove call.
	Rect2D ScreenRect; 
	Rect2D ClientRect;

protected:
	//These are used to determine where input goes
	bool ShowWindow;
	WindowState State;

	//Only window that this should be null on is the root window
	GuiWindow* Parent;

	//The integer that identifies this window. You need this if
	//you plan to respond to any messages from this window.
	std::string Name;
	WindowHandle WinHandle;

	std::string Text;

	//A list of all windows that this window is responsible for
	std::list<WindowHandle> ChildList;
	
	//images that make up the bounds of the window
	Screen3D* ScreenPtr;
	FontManager* FM;
	TextureHandle Background;
	Font2D* pFont;

	//pointer to the overall manager class
	GUIManager* GM;

	//used by the gui manager to know if an instance can be deleted by it,
	//or if it's owned by something else
	bool GMOwned;

	friend class GUIManager;

	//private virtual functions for events. These are called silently by the main OnWhatever
	//functions. Basically, in the OnSomethingSomething functions, all the base code is called, then
	//these functions are called to do more specific stuff.
	virtual void InitEvent( );
	virtual void CloseEvent( );
	virtual void MoveEvent(int x, int y, int width, int height);
	virtual void ClickEvent(int x, int y);
	virtual void DblClickEvent(int x, int y);
	virtual void KeyEvent( char key, int VirtualKey=0 );
	virtual void MouseEnterEvent();
	virtual void MouseExitEvent();
	virtual void MouseMoveEvent(int x, int y);
	virtual void PaintEvent( Screen3D& Screen );

	

public:
	
	GuiWindow(); 
	virtual ~GuiWindow() { OnClose(); }

	void Init( char* WindowName ) { Name = WindowName; }

	//Window state management
	void Show( bool show ) { ShowWindow = show; }
	virtual WindowState FocusState() { return State; }
	virtual void SetState(WindowState state) { State = state; }

	//container management
	virtual void AddWindow( WindowHandle H );
	virtual void RemoveWindow(  WindowHandle win );
	virtual WindowHandle GetChildWindowIndex( int index );
	virtual int GetChildWindowCount( );

	//Message handler functions
	virtual void SetMessageHandler( UINT (*MessageHandler)(Event*) );
	virtual void ThrowMessage(int Message, int Parameter1, int Parameter2);

	//Click detection functions
	virtual bool PointInWindow( int x, int y);

	WindowHandle GetHandle( ) { return WinHandle; }

	virtual void  SetText( std::string newText ) { Text = newText; }
	virtual char* GetText( ) { return (char*)Text.c_str(); }
	// messages. Generally these shouldn't be overloaded unless some sort of strange functionality
	// is neccessary.
	virtual void OnInit(GUIManager* GMPtr, Screen3D& Screen, FontManager* fm, RECT& NewRect, GuiWindow* parent ); 
	virtual void OnClose();
	virtual void OnMove(int x, int y, int width, int height);
	virtual void OnClick(int x, int y);
	virtual void OnDblClick(int x, int y);
	virtual void OnKey( char key, int VirtualKey=0 );
	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnMouseMove(int x, int y);
	virtual void OnPaint( Screen3D& Screen );

};

