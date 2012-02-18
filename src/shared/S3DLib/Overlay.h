// I N C L U D E S //////////////////////////////////////////////////
#pragma once
#include "texturemanager.h"

// C L A S S E S ////////////////////////////////////////////////////

//Image overlay, drawn after everything else in the scene.
struct Overlay
{
	TextureHandle Image;
	float Alpha;

	int x, y;
	
	Overlay( TextureHandle img, float alpha, int _x, int _y ) :
		Image( img ),
		Alpha ( alpha),
		x ( _x) ,
		y ( _y ) 
	{
	}

};