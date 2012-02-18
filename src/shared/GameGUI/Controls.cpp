#include "Controls.h"
#include "..\..\shared\ini\ini.h"
#include "guimanager.h"

#include "..\..\shared\defs\defs.h"

void Button::InitEvent( )
{
	char TempBuffer[128];
	char TempBufferA[128];

		
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "button", "normal_background", TempBuffer);
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "button", "normal_background_a", TempBufferA);
	Background = ScreenPtr->GetTextureHandle(TempBuffer, TempBufferA);

	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "button", "pressed_background", TempBuffer);
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "button", "pressed_background_a", TempBufferA);
	BackgroundInactive = ScreenPtr->GetTextureHandle(TempBuffer, TempBufferA);
	

	
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "button", "font", TempBuffer);
	pFont = FM->GetFont( TempBuffer, 8 );


}

void Button::ClickEvent(int x, int y)
{
	Depressed = true;
	ThrowMessage(WINM_COMMAND, 0, 0);
}



void Button::PaintEvent( Screen3D& Screen )
{
	RECT DrawRect;
	SetRect(&DrawRect, ScreenRect.left, ScreenRect.top, ScreenRect.right, ScreenRect.bottom);

	if(Depressed)
		Screen.DrawSpriteRect(DrawRect, BackgroundInactive, true);
	else
		Screen.DrawSpriteRect(DrawRect, Background, true);

	pFont->Draw(Screen, 
		DrawRect.left + ((DrawRect.right - DrawRect.left)/2), 
		DrawRect.top + ((DrawRect.bottom - DrawRect.top)/2), 
		true, D3DCOLOR_XRGB(255, 255, 255), (char*)Text.c_str() );

	Depressed = false;
}




void WindowFrame::InitEvent( )
{
	char TempBuffer[128], TempBufferA[128];
			
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "general", "title", TempBuffer);
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "general", "title_a", TempBufferA);
	Title = ScreenPtr->GetTextureHandle(TempBuffer);

	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "general", "font", TempBuffer);
	pFont = FM->GetFont( TempBuffer, 8 );

}

void WindowFrame::ClickEvent(int x, int y)
{
}

void WindowFrame::PaintEvent( Screen3D& Screen )
{
	RECT DrawRect;
	SetRect(&DrawRect, ScreenRect.left, ScreenRect.top, ScreenRect.right, ScreenRect.top + 15);
	Screen.DrawSpriteRect(DrawRect, Title, false);
	
	pFont->Draw(Screen, 
		DrawRect.left + ((DrawRect.right - DrawRect.left)/2), 
		DrawRect.top + ((DrawRect.bottom - DrawRect.top)/2), 
		true, D3DCOLOR_XRGB(255, 255, 255), (char*)Text.c_str() );


}




void DialogBox::InitEvent( )
{
	// declarations
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

	RECT R;
	R.top    = ClientRect.top;
	R.left   = ClientRect.left;
	R.right  = ClientRect.left;
	R.bottom = ClientRect.bottom;

	WinControls.Init( "IDTITLEBAR" );
	GM->AddWindow(&WinControls, this->WinHandle, false);


}

void DialogBox::PaintEvent( Screen3D& Screen )
{
	RECT WindowDimensions;
	SetRect ( &WindowDimensions, ScreenRect.left, ScreenRect.top, ScreenRect.right, ScreenRect.bottom);
	
	if(State == W_INACTIVE)
		Screen.DrawSpriteRect(WindowDimensions, BackgroundInactive, false);
	else
		Screen.DrawSpriteRect(WindowDimensions, Background, false);

}

void  DialogBox::SetText( std::string newText ) 
{ 
	Text = newText; 
	WinControls.SetText( newText.c_str() );
	
}
char* DialogBox::GetText( ) 
{
	return (char*)Text.c_str(); 
}

void StaticBox::InitEvent( )
{
	char TempBuffer[128];
	GetIniValue("..\\base\\art\\ui\\userinterface.ini", "button", "font", TempBuffer);
	pFont = FM->GetFont( TempBuffer, 8 );


}

void StaticBox::PaintEvent( Screen3D& Screen )
{
	RECT DrawRect;
	SetRect(&DrawRect, ScreenRect.left, ScreenRect.top, ScreenRect.right, ScreenRect.bottom);
	
	pFont->Draw(Screen, 
		DrawRect.left + ((DrawRect.right - DrawRect.left)/2), 
		DrawRect.top + ((DrawRect.bottom - DrawRect.top)/2), 
		true, D3DCOLOR_XRGB(255, 255, 255), (char*)Text.c_str() );

	
}

