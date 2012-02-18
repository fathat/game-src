#ifndef _console_h_
#define _console_h_

// I N C L U D E S /////////////////////////////////////////////////////
#include "..\..\shared\s3dlib\screen.h"
#include "..\..\shared\s3dlib\font.h"
#include "window.h"
#include "stdio.h"
#include <vector>
#include <string>


// D E F I N E S ///////////////////////////////////////////////////////
#define MAX_LINES        64


//////// InputBox /////////////////////////////////////////////////////////////
//
// Description:
//
// Provides an input box. Functions much like the Quake console.
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

class InputBox : public GuiWindow
{
protected:

	// Text data
	std::string line[MAX_LINES];
	int		position;

	// Cursor data
	DWORD	time;
	bool    ShowCursor;
	bool    ScrollOnEnter;
	bool    ShowInput;
	bool    ShowInputOnEnter;

	
	
	// Graphics data
	TextureHandle BackgroundInactive;

	//Alternative to the message procedure. Stores the last text
	//entered, and if you've read it.
	std::string LastInput;
	bool InputChecked;


	// These are just called internally
	void Delete();
	void ScrollUp();

	// message handlers
	void MoveEvent(int x, int y, int width, int height);
	void KeyEvent( char in, int VirtualKey=0 );
	void PaintEvent( Screen3D& Screen );

public:
	InputBox();
	~InputBox();
	

	// returns true if a new line has been inputed into the console
	bool NewMessage( std::string& Output );

	void SetShowInput ( bool s ) { ShowInput = s; }
	bool GetShowInput ( ) {  return ShowInput; }

	// standard GUI functions
	void InitEvent ( ); 
	void CloseEvent();

	// functions to add text to the box
	void InputLine(char* in);

	void SetText(std::string newText) { Text = newText; position = Text.size(); }

	void SetScrollOnEnter( bool truefalse ) { ScrollOnEnter=truefalse; }

	

	void Clear();
	void Execute();

};

#endif