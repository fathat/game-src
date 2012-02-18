#pragma once

// I N C L U D E S ////////////////////////////////////////////////
#include "..\..\shared\s3dlib\screen.h"
#include "..\..\shared\s3dlib\font.h"
#include "window.h"
#include <list>
#include <string>

// D E F I N E S //////////////////////////////////////////////////


// C L A S S E S /////////////////////////////////////////////////

class GuiScreen : GuiWindow
{
protected:
	//Renders the 3d view
	void PaintEvent( Screen3D& Screen );
};

