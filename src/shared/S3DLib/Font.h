// I N C L U D E S ///////////////////////////////////////////////////////
#pragma once
#include <stdarg.h> //used for argument lists
#include <map>
#include <string>
#include "screen.h"
#include "d3dfont.h"

// D E F I N E S /////////////////////////////////////////////////////////


// C L A S S E S /////////////////////////////////////////////////////////

class Font2D
{
protected:
	CD3DFont* D3DFont;

	int FontSize;
	char FontName[32];
	char str[2048]; //buffer to hold outputted string

	bool Initialized;

public:

	Font2D() { D3DFont = NULL; Initialized = false; ZeroMemory( str, 2048); ZeroMemory( FontName, 32); }
	~Font2D() { Cleanup() ; }

	bool Init ( Screen3D& Screen, char* FontName, int FontSizeP );
	void Cleanup ();

	int GetFontSize( ) { return FontSize; }
	char* GetFontName() { return FontName; }

	bool IsInitialized() { return Initialized; }

	//works like sprintf
	void Draw ( Screen3D& Screen, Real x, Real y, bool Centered, D3DCOLOR color, char* string, ...);

};



class FontManager
{
protected:
	std::map<std::string , Font2D* > FontMap;

	Screen3D* ScreenPtr;
	
public:
	FontManager() { ScreenPtr = NULL;}
	~FontManager() { Cleanup(); }

	void Init(Screen3D& Screen);
	void Cleanup();

	Font2D* GetFont( char* fontname, int size  );
};