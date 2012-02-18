#pragma once

// I N C L U D E S ////////////////////////////////////////
#include "..\shared\gamegui\guimanager.h"
#include "..\shared\gamegui\controls.h"
#include "..\shared\gamegui\inputbox.h"


// D E F I N E S //////////////////////////////////////////
#define UPDATE_INTERVAL 200

// C L A S S E S //////////////////////////////////////////


//////// BaseApp //////////////////////////////
//
// Description:
//
// Base app class! Other apps are derived from
// this class!
//
// Known Problems:
//
// - None
//
// TODO:
//
//
///////////////////////////////////////////////



class BaseApp
{

    	
public:
	
	
	//GUI message sent to the game notifying it of something
	//that's happened in a window
	virtual void MouseDown( POINT mousepoint, int button ) { }
	virtual void KeyMessage( int virtualkey ) { }
	virtual void CharMessage( char charpressed, int repeat ) { }
	virtual void HandleGuiMessage( Event* m ) { } 

	virtual void Activate() { }
	virtual void Deactivate() { }

	//Functions which are more or less called every frame. The core
	//of the app class.
	virtual void Update() { }
	virtual void CheckInput()  { }
	virtual void RedrawScreen(){ } 

};
