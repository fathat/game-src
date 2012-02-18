#pragma once

// I N C L U D E S /////////////////////////////////////////////////
#include "window.h"

// C L A S S E S ///////////////////////////////////////////////////


//////// GuiWindowFrame ///////////////////////
//
// Description:
//
// Used as a frame fr
//
// Known Problems:
//
// - None
//
// TODO:
//
// - 
//
//
///////////////////////////////////////////////


class GuiWindowFrame : public GuiWindow
{
protected:
	TextureHandle Background;
	TextureHandle Border;
	TextureHandle Title;
	TextureHandle CloseButton;
	TextureHandle SizeButton;
	
public:
	
	GuiWindowFrame() ; 
	~GuiWindowFrame() ;


};