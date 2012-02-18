#include "screen.h"
#include "font.h"
#include "types.h"


//Initialize the font. 
bool Font2D::Init(Screen3D& Screen, char* fontname, int FontSizeP)
{
	Cleanup();

	HRESULT HR;

	//make a new font
	D3DFont = new CD3DFont(fontname, FontSizeP);
	HR = D3DFont->InitDeviceObjects(Screen.D3DDevice);

	FontSize = FontSizeP;
	if(FAILED ( HR ) )
	{
		OutputDXError(HR, "Font2D Init Error: ");
		return false;
	}

	HR = D3DFont->RestoreDeviceObjects();

	if(FAILED ( HR ) )
	{
		OutputDXError(HR, "Font2D Init Error: ");
		return false;
	}
	
	Initialized = true;
	strcpy(FontName, fontname);
	return true;

}


void Font2D::Cleanup()
{
	if(D3DFont)
		delete D3DFont;

	D3DFont = NULL;
	Initialized = false;

}


void Font2D::Draw ( Screen3D& Screen, Real x, Real y, bool Centered, D3DCOLOR color, char* string, ...)
{
	//check preconditions
	if(D3DFont == NULL)
		return;

	
	//Convert parameters into the final string
	va_list argumentlist;
	va_start(argumentlist, string);
	vsprintf(str, string, argumentlist);
	va_end(argumentlist);


	//Draw the text.
	DWORD Flags=0;
	if( Centered )
	{
		SIZE Size;
		D3DFont->GetTextExtent( str, &Size );
		D3DFont->DrawText(x-Size.cx/2, y-Size.cy/2, color, str, Flags);
	}
	else
		D3DFont->DrawText(x, y, color, str, Flags);


}



//Prepares FontManager for use.
void FontManager::Init( Screen3D& Screen)
{
	Cleanup();
	ScreenPtr = &Screen;

}


//Destroys all fonts
void FontManager::Cleanup()
{
	//Delete all the fonts FontManager is holding.
	std::map<std::string, Font2D*>::const_iterator i;
	for ( i = FontMap.begin( ); i != FontMap.end( ); i++ )
	{
		Font2D* f = (i->second);
		delete f;
	}
	FontMap.clear();
}


//Gives the caller the Font requested if it exists, and
//creates it if it doesn't exist.
Font2D* FontManager::GetFont( char* fontname, int size  )
{
	//remake the ID from the font information
	std::string id = fontname;
	id += size;

	//ask the map for the font. If it doesn't exist,
	//create it.
	Font2D* f = FontMap[id];
	if( f == NULL )
	{
		f = new Font2D( );
		f->Init( *ScreenPtr, fontname, size );
		FontMap[id] = f;
		return f;
	}

	//Font must exist, so return it.
	return f;
}