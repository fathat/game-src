#include "window.h"
#include "guimanager.h"
#include "..\..\shared\ini\ini.h"

#include "..\..\shared\defs\defs.h"

UINT DefaultMessageProcedure( Event* m )
{
	switch(m->Message)
	{
	case WINM_COMMAND:
		if(m->ControlID == "IDOK")
		{
			//do something something here
		}
		break;
	case WINM_CONTENTSCHANGED:
		break;
	case WINM_KEYPRESSED:
		break;
	};

	return 0;
}

GuiWindow::GuiWindow()
{
	ScreenPtr   = NULL; 
	GM          = NULL; 
	Parent      = NULL;
	ShowWindow  = true; 
	State       = W_INACTIVE; 
	GMOwned     = true;
	MessageFunc = DefaultMessageProcedure;
	Name        = "noname";
	
}

void GuiWindow::OnInit(GUIManager* GMPtr, Screen3D& Screen, FontManager* fm, RECT& NewRect, GuiWindow* parent)
{
	ScreenPtr = &Screen;
	Parent = parent;
	GM     = GMPtr;
	FM     = fm;

	OnMove(NewRect.left, NewRect.top, NewRect.right - NewRect.left, NewRect.bottom - NewRect.top);
	Show(true);

	char TempBuffer[64];
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "general", "font", TempBuffer);
	pFont = FM->GetFont( TempBuffer, 8 );

	InitEvent();


}

void GuiWindow::OnClose()
{
	//declare the iterator
	std::list<WindowHandle>::iterator i;

	//set the iterator to the start of the list
	ChildList.erase(ChildList.begin(), ChildList.end());

	
	Show(false);
	CloseEvent();
}





void GuiWindow::AddWindow( WindowHandle H )
{
	ChildList.push_back( H );


}


void GuiWindow::RemoveWindow(  WindowHandle win )
{
	std::list<WindowHandle>::iterator i;
	for(i=ChildList.begin(); i!=ChildList.end(); i++)
	{
		if( win == (*i))
		{
			ChildList.erase( i );
		}
	}
}



WindowHandle GuiWindow::GetChildWindowIndex( int index )
{
	//declare the iterator
	std::list<WindowHandle>::iterator i;

	//set the iterator to the start of the list
	i = ChildList.begin();

	//move it to desired item
	for(int n=0; n<index; n++)
		i ++;
	
	return *i;
}

int GuiWindow::GetChildWindowCount( )
{
	return ChildList.size();
}


bool GuiWindow::PointInWindow( int x, int y)
{
	// declarations
	POINT xy;
	RECT R;
	
	xy.x = x; xy.y = y;
	SetRect(&R, (int)ScreenRect.left, (int)ScreenRect.top, (int)ScreenRect.right, (int)ScreenRect.bottom);
	
	if ( PtInRect( &R, xy) )
		return true;

	return false;
}


void GuiWindow::ThrowMessage(int Message, int Parameter1, int Parameter2)
{
	//Prepare an event structure
	Event SomeEvent;
	SomeEvent.Message   = Message;
	SomeEvent.ControlID = Name;
	SomeEvent.Control   = this;
	SomeEvent.Param1    = Parameter1;
	SomeEvent.Param2    = Parameter2;

	//Pass it to whatever message procedure is handling it
	MessageFunc(&SomeEvent);

	return;
}


void GuiWindow::SetMessageHandler( UINT (*MessageHandler)(Event*) )
{
	MessageFunc = MessageHandler;
	std::list<WindowHandle>::iterator i;

	for(i = ChildList.begin(); i!= ChildList.end(); ++i)
	{
        GuiWindow* Win = GM->WindowList[(*i)];
		Win->SetMessageHandler(MessageHandler);
	}

}

void GuiWindow::InitEvent()
{
	
}


void GuiWindow::CloseEvent()
{
}

void GuiWindow::MoveEvent(int x, int y, int width, int height)
{

}

void GuiWindow::ClickEvent(int x, int y)
{

}

void GuiWindow::DblClickEvent(int x, int y)
{

}

void GuiWindow::KeyEvent( char key, int VirtualKey )
{

}

void GuiWindow::MouseEnterEvent()
{

}

void GuiWindow::MouseExitEvent()
{

}

void GuiWindow::MouseMoveEvent(int x, int y)
{

}

void GuiWindow::PaintEvent( Screen3D& Screen )
{

}


// messages
void GuiWindow::OnMove(int x, int y, int width, int height)
{
	
	


	if(Parent)
	{
		ScreenRect.left   = (Real)x + Parent->ScreenRect.left;
		ScreenRect.top    = (Real)y + Parent->ScreenRect.top;
		ScreenRect.right  = (Real)(x+width) + Parent->ScreenRect.left;
		ScreenRect.bottom = (Real)(y+height) + Parent->ScreenRect.top;

	}
	else
	{
		ScreenRect.left   = (Real)x ;
		ScreenRect.top    = (Real)y ;
		ScreenRect.right  = (Real)(x+width);
		ScreenRect.bottom = (Real)(y+height) ;

	}

	

	ClientRect.left   = (Real)x;
	ClientRect.right  = (Real)x+width;
	ClientRect.top    = (Real)y;
	ClientRect.bottom = (Real)y+height;

	MoveEvent( x,  y,  width,  height);


	//move through list and send message to children
	std::list<WindowHandle>::iterator i;
	for(i = ChildList.begin(); i!= ChildList.end(); ++i)
	{
        GuiWindow* Win = GM->WindowList[(*i)];	
		Win->OnMove(Win->ClientRect.left, Win->ClientRect.top, Win->ClientRect.right - Win->ClientRect.left , Win->ClientRect.bottom-Win->ClientRect.top);
	}

	

}

void GuiWindow::OnClick(int x, int y)
{
	//handle it here
	ClickEvent(x, y );

	//check to see if we're farthest down, if we are then this window gets the focus
	if(ChildList.begin() == ChildList.end())
		State = W_FOCUS;
	else
		State = W_ACTIVE;

	// then pass it on to children (if there are any)
	
	//declare the iterator
	std::list<WindowHandle>::iterator i;

	//move through list and send click message to any children it may have hit
	bool WindowFound=false;
	for(i = ChildList.begin(); i!= ChildList.end(); ++i)
	{
        GuiWindow* Win = GM->WindowList[(*i)];

		if( Win->PointInWindow( x, y ) )
		{
			Win->OnClick( x, y );
			WindowFound=true;
		}
	}

	if(!WindowFound)
		State = W_FOCUS;

}

void GuiWindow::OnDblClick(int x, int y)
{
	//handle it here
	DblClickEvent(x, y);

	// then pass it on to children
	
	//declare the iterator
	std::list<WindowHandle>::iterator i;

	//move through list and send click message to any children it may have hit
	for(i = ChildList.begin(); i!= ChildList.end(); ++i)
	{
        GuiWindow* Win = GM->WindowList[(*i)];

		if( Win->PointInWindow( x, y ) )
			Win->OnDblClick( x, y );
	}

}

void GuiWindow::OnKey(char key, int VirtualKey)
{
	//handle the key
	if(State == W_FOCUS)
	{
		KeyEvent(key, VirtualKey);
		return ;
	}


	
	//move through list and send key message to any children that are active
	std::list<WindowHandle>::iterator i;
	for(i = ChildList.begin(); i!= ChildList.end(); ++i)
	{
        GuiWindow* Win = GM->WindowList[(*i)];

		if( Win->State != W_INACTIVE )
			Win->OnKey(key, VirtualKey);
	}




}

void GuiWindow::OnMouseEnter()
{
	MouseEnterEvent();

}

void GuiWindow::OnMouseExit()
{
	MouseExitEvent();

}

void GuiWindow::OnMouseMove(int x, int y)
{
	MouseMoveEvent(x, y);

}

void GuiWindow::OnPaint( Screen3D& Screen )
{
	if(!ShowWindow)
		return;

	//set the viewport

	Rect2D DrawRect = ScreenRect;

	if(Parent)
	{
		
		if(DrawRect.left < Parent->ScreenRect.left)
			DrawRect.left = Parent->ScreenRect.left;

		if(DrawRect.top < Parent->ScreenRect.top)
			DrawRect.top = Parent->ScreenRect.top;

		if(DrawRect.bottom > Parent->ScreenRect.bottom)
			DrawRect.bottom = Parent->ScreenRect.bottom;

		if(DrawRect.right < Parent->ScreenRect.right)
			DrawRect.right = Parent->ScreenRect.right;
	}


	Screen.SetViewport((int)DrawRect.left, (int)DrawRect.top, (int)DrawRect.right - (int)DrawRect.left, (int)DrawRect.bottom - (int)DrawRect.top);

	//handle the event
	PaintEvent(Screen);

	

	//declare the iterator
	std::list<WindowHandle>::iterator i;

	//send draw message to children of the class
	for(i = ChildList.begin(); i!= ChildList.end(); ++i)
	{
        GuiWindow* Win = GM->WindowList[(*i)];

		Win->OnPaint( Screen );
	}

	//set back to parents viewport (in other words, undoing our own changes to it)
	if(Parent)
		Screen.SetViewport(Parent->ScreenRect.left, Parent->ScreenRect.top, Parent->ScreenRect.right - Parent->ScreenRect.left, Parent->ScreenRect.bottom - Parent->ScreenRect.top);

}