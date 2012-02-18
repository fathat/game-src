#include "GUIManager.h"
#include "..\..\shared\ini\ini.h"

#include "..\..\shared\defs\defs.h"

GUIManager::GUIManager()
{
	//init the root window
	WindowList[0] = new GuiWindow();
	WindowList[0]->GMOwned = true;
	//clear all windows
	for(int i=1; i<MAX_WINDOWS; i++)
	{
		WindowList[i] = NULL;
	}

	//increase the handle count to 1 (first window always root window)
	HandleCount = 1;
	GuiActive = false;

	Focus = 0;


}

GUIManager::~GUIManager()
{
	for(int i=0; i < MAX_WINDOWS; i++)
	{
		if(WindowList[i])
		{
			//make sure this window is owned by the gui manager
			//before we delete it.
			if(WindowList[i]->GMOwned)
				delete WindowList[i];
			WindowList[i] = NULL;
		}
		
	}
	return;
}


void GUIManager::Init( Screen3D& Screen, FontManager* fm, WorldManager* wm )
{
	//declarations
	char TempBuffer[128];
	char TempBufferA[128];


	//set variables
	ScreenPtr = &Screen;
	FM = fm;
	WM = wm;

	//load a bitmap for the cursor
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "general", "cursor", TempBuffer);
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "general", "cursor_a", TempBufferA);
	CursorHandle = Screen.GetTextureHandle( TempBuffer, TempBufferA);

	
	
	SV[0].color = SV[1].color = SV[2].color = SV[3].color = D3DCOLOR_XRGB(255, 255, 255);
	SV[0].RHW = SV[1].RHW = SV[2].RHW = SV[3].RHW = 1;
	SV[0].z = SV[1].z = SV[2].z = SV[3].z = 0;

	SV[0].tu = 0; SV[0].tv = 0;
	SV[1].tu = 1; SV[1].tv = 0;
	SV[2].tu = 0; SV[2].tv = 1;
	SV[3].tu = 1; SV[3].tv = 1;

	//init the root window
	RECT WindowRect;

	WindowRect.left   = 0;
	WindowRect.top    = 0;
	WindowRect.right  = Screen.GetWidth();
	WindowRect.bottom = Screen.GetHeight();

	WindowList[0]->OnInit(this, Screen, FM, WindowRect, NULL);
	WindowList[0]->Init( "ROOT");
	WindowList[0]->WinHandle = 0;

}

//window adding functions
WindowHandle GUIManager::AddWindow( GuiWindow* Window, WindowHandle Parent, bool AllowOwnership )
{
	// declarations
	int h = -1;

	Window->GMOwned = AllowOwnership;

	// find an open spot for the window
	for(int i=0; i < MAX_WINDOWS; i++)
	{
		if(WindowList[i]==NULL)
		{
			//yay! found an open spot.
			h = i;			

			Window->WinHandle = h;
			WindowList[h] = Window;

			WindowList[Parent]->AddWindow( h );

			RECT DefaultRect;


			DefaultRect.left    = WindowList[Parent]->ClientRect.left;
			DefaultRect.top     = WindowList[Parent]->ClientRect.top;
			DefaultRect.right   = WindowList[Parent]->ClientRect.right;
			DefaultRect.bottom  = WindowList[Parent]->ClientRect.bottom;

			Window->OnInit(this, *ScreenPtr, FM, DefaultRect, WindowList[Parent]);

			return h;

		}
	}

    return 	h;
}

GuiWindow* GUIManager::FindWindow( WindowHandle Window )
{
	// find the gui in the global window list
	return WindowList[Window];

}

GuiWindow* GUIManager::FindWindow( char* name)
{
	// find an open spot for the window
	for(int i=0; i < MAX_WINDOWS; i++)
	{
		if(WindowList[i])
		{
			if(WindowList[i]->Name.compare( name) == 0)
			{
				//yay! found it.
				return WindowList[i];
			}

		}
	}
	return NULL;
}
	
//messages
void GUIManager::MouseClicked ( int x, int y, int button )
{
	// loop through all active windows and set them back to inactive
	for(int i=0; i < MAX_WINDOWS; i++)
	{
		if(WindowList[i])
		{
			WindowList[i]->SetState(W_INACTIVE);
		}
	}

	if(GuiActive)
		WindowList[0]->OnClick(x, y);
}

void GUIManager::KeyPressed( char KeyPressed, int VirtualKey )
{
	if( GuiActive )
	{
		WindowList[0]->OnKey( KeyPressed, VirtualKey);
	}
}

void GUIManager::DrawGUI(Screen3D& Screen)
{
	// Set up the render states to draw the window
	ScreenPtr->D3DDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	ScreenPtr->D3DDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
	ScreenPtr->D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	//ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	//ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	//ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	ScreenPtr->SetPointTextureFiltering(0);
	Screen.D3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	
	
	//draw all the windows (recursively)
	WindowList[0]->OnPaint(Screen);

	if(GuiActive)
	{
		//draw the cursor
		POINT CursorPoint;
		GetCursorPos(&CursorPoint);
		ScreenToClient(Screen.hWnd, &CursorPoint);

		RECT MouseRect;
		MouseRect.left   = CursorPoint.x-7;
		MouseRect.top    = CursorPoint.y-7;
		MouseRect.right  = CursorPoint.x+8;
		MouseRect.bottom = CursorPoint.y+8;

		Screen.DrawSpriteRect( MouseRect, CursorHandle, true );


		/*SV[0].x = CursorPoint.x-8;     SV[0].y = CursorPoint.y-8;
		SV[1].x = CursorPoint.x+8;  SV[1].y = CursorPoint.y-8;
		SV[2].x = CursorPoint.x-8;     SV[2].y = CursorPoint.y+8;
		SV[3].x = CursorPoint.x+8;  SV[3].y = CursorPoint.y+8;

		
		Screen.SetTexture( CursorHandle , 0);
		Screen.D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, SV, sizeof(ScreenVertex));*/
	}

	//Renable old settings
	Screen.D3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE );
	//ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//ScreenPtr->D3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	ScreenPtr->SetLinearTextureFiltering();
}
