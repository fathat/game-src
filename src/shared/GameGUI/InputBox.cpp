

//////// InputBox //////////////////////////////////////////////////////////////
//
// Description:
//
// Provides a mulitline, scrolling input console
//
// Inherits From:
//
// - [none]
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

#include "InputBox.h"
#include "..\ini\ini.h"

#include "..\defs\defs.h"

InputBox::InputBox()
{
	Text = "";
	LastInput = "";
	InputChecked = false;
	position = 0;
	ScrollOnEnter = true;
	ShowInput = true;
}
	
InputBox::~InputBox()
{

	
}




//////// Init /////////////////////////////////
//
// Description:
//
// Initializes neccesary console variables and
// shows the window.
//
///////////////////////////////////////////////

void InputBox::InitEvent( )
{
	//OnMove( WindowRect.left, WindowRect.top, WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top );
	time = timeGetTime();

	pFont = FM->GetFont( "tahoma", 8);
	char TempBuffer[128];
	char TempBufferA[128];

	//load up the background
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "consolebox", "background", TempBuffer);
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "consolebox", "background_a", TempBufferA);
	Background = ScreenPtr->GetTextureHandle(TempBuffer, TempBufferA);
	
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "consolebox", "inactive_background", TempBuffer);
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "consolebox", "inactive_background_a", TempBufferA);
	BackgroundInactive = ScreenPtr->GetTextureHandle(TempBuffer, TempBufferA);



}


//////// Close ////////////////////////////////
//
// Description:
//
// Hides the window.
//
///////////////////////////////////////////////

void InputBox::CloseEvent()
{
	time = timeGetTime();
}


//////// MoveEvent ////////////////////////////
//
// Description:
//
// Called by OnMove, specialized functionality
// for movement of this control put here.
//
///////////////////////////////////////////////

void InputBox::MoveEvent(int x, int y, int width, int height)
{
	//Figure out the size of the texture (if there is a texture)
	Texture* T = ScreenPtr->TM.GetTexture(Background);
	Real texturewidth=1, textureheight=1;
	if(T)
	{
		texturewidth  = T->GetWidth();
		textureheight = T->GetHeight();
	}




}



//////// KeyEvent /////////////////////////////
//
// Description:
//
// Called by OnKey, specialized code for
// responding to keys goes here.
//
///////////////////////////////////////////////

void InputBox::KeyEvent( char in, int VirtualKey )
{	
	
	//Handle virtual key events first
	if(VirtualKey == VK_LEFT)
	{
		position--;

		if(position<0)
			position = 0;
		return;
	}
	
	if(VirtualKey == VK_RIGHT)
	{
		position++;

		if(position > Text.size())
			position = Text.size();
		return;
	}

	if(VirtualKey == VK_DELETE)
	{
		if(Text.size() > 0)
			Text.erase(Text.begin()+position);

		return;

	}

	if( VirtualKey == VK_END )
	{
		position = Text.size();
		return;
	}

	if( VirtualKey == VK_HOME )
	{
		position = 0;
		return;
	}



	// Then handle ascii constants
	if(in == 8) //pressed backspace?
	{
		Delete();
		return;
	}



	if(in == '\r')
	{
		if(ScrollOnEnter)
			Execute();
		return;
	}

	//handle Esc key
	if( in == 27 )
		return;


	//Finally, handle anything that wasn't caught before (ie, characters)
	if(in > 0)
	{

		ShowCursor = true;
		time = timeGetTime();
		
		Text.insert(Text.begin()+position, in);
		position++;
	}


	
}



//////// PaintEvent ///////////////////////////
//
// Description:
//
// Called by OnPaint, specialized draw code
// goes here.
//
///////////////////////////////////////////////

void InputBox::PaintEvent( Screen3D& Screen )
{
	// Declarations
	std::string temp = "";
	unsigned int CurrentTime = timeGetTime();


	// toggle the cursor if neccesary
	if(CurrentTime > time + 500)
	{
		time = CurrentTime;

		ShowCursor = !ShowCursor;
		
		if(State != W_FOCUS)
			ShowCursor = false;
	}


	// Add a cursor to the input line, and a prompt symbol
	temp = "> ";
	temp += Text;

	if(ShowCursor && position <= temp.size() && position+2 > 0)
		temp.insert(temp.begin()+position+2, '|');
	else if(position+2 < temp.size() && position+2 > 0)
		temp.insert(temp.begin()+position+2, '|');


	

	
	// Draw the background for the console box
	ScreenPtr->SetTextureMode(TMNormal);
	ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	
	
	RECT WindowDimensions;
	SetRect ( &WindowDimensions, ScreenRect.left, ScreenRect.top, ScreenRect.right, ScreenRect.bottom);
	ScreenPtr->SetFVF(D3DFVF_SCREENVERTEX);
	
	if(State != W_INACTIVE)
		ScreenPtr->DrawSpriteRect(WindowDimensions, Background, false);
	else
		ScreenPtr->DrawSpriteRect(WindowDimensions, BackgroundInactive, false);
		


	//Figure out how much to offset the text lines, and how many text lines we can actually display
	int offset = (ClientRect.bottom-ClientRect.top) - ((MAX_LINES*(pFont->GetFontSize()+3)) + pFont->GetFontSize()*2 );
	int MaxWindowLines = (ClientRect.bottom-ClientRect.top) / (pFont->GetFontSize()+3) - 1;
	

	//loop through and draw all the lines
	int StartIndex=0;
	if(MAX_LINES-MaxWindowLines < 0)
	{
		StartIndex=0;		
	}
	else
		StartIndex= MAX_LINES-MaxWindowLines;

	for(int i=StartIndex; i<MAX_LINES; i++)
	{
		pFont->Draw(Screen, ScreenRect.left + 9, ScreenRect.top + ((i*(pFont->GetFontSize()+3))+ offset),false, D3DCOLOR_XRGB(255, 255, 255), (char*)line[i].c_str());
	}
	
	
	//Finally, draw the input line
	if(ShowInput)
	{
		pFont->Draw(Screen, ScreenRect.left + 9, ScreenRect.bottom - (pFont->GetFontSize() * 2), false, D3DCOLOR_XRGB(255, 255, 255), (char*)temp.c_str());
	}



}



//////// Execute //////////////////////////////
//
// Description:
//
// Moves everything one line up and clears
// buffers.
//
///////////////////////////////////////////////

void InputBox::Execute()
{
	//add text to InputBox

	if( Text.empty() )
		return;
		
	if(ShowInputOnEnter)
	{
		ScrollUp();
	}

	LastInput = Text;
	InputChecked = false;

	Text.append("\n");
	ThrowMessage(WINM_CONTENTSCHANGED, (int)Text.c_str(), 0 );

	Clear();
	
	time = timeGetTime();
}




//////// ScrollUp /////////////////////////////
//
// Description:
//
// Moves all lines up by 1
//
///////////////////////////////////////////////

void InputBox::ScrollUp()
{

	for(int i=0; i<MAX_LINES-1; i++)
	{
		line[i] = line[i+1];	
	}

	line[MAX_LINES-1] = Text;
}


//////// Clear ////////////////////////////////
//
// Description:
//
// Clears the input line.
//
///////////////////////////////////////////////

void InputBox::Clear()
{

	position = 0;

	Text = "";
}




//////// Delete ///////////////////////////////
//
// Description:
//
// Moves cursor one space back (like backspace)
//
///////////////////////////////////////////////

void InputBox::Delete()
{
	ShowCursor = true;
	time = timeGetTime();
	

	if (position > 0)
	{
		position--;
		Text.erase(Text.begin()+position);
	}

}



//////// NewMessage ///////////////////////////
//
// Description:
//
// Returns true if the user has inputted a new
// line into the input box since the last check.
//
///////////////////////////////////////////////

bool InputBox::NewMessage( std::string& Output ) { 
	Output = LastInput;

	if(!InputChecked)
	{
		InputChecked = true;
		return true;
	}
	return false;
}



//////// InputLine ////////////////////////////
//
// Description:
//
// Adds text to the InputBox forgoing the input
// Text.
//
///////////////////////////////////////////////

void InputBox::InputLine(char* in)
{
	std::string tempbuffer;
	

	tempbuffer = Text;
	Text     = in;
	ScrollUp();
	Text = tempbuffer;
}




