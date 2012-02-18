#pragma once


//////// InputManager /////////////////////////
//
// Description:
//
// Translates a key into a description string
// based off a config file. So, it might change
// 'j' to "jump" of that's what it says in the
// config file.
//
// Known Problems:
//
// - None
//
// TODO:
//
// - Make it actually do something
//
///////////////////////////////////////////////

class InputManager
{
protected:
	

public:


	void GetAction(UCHAR key, char* OutBuffer);

};