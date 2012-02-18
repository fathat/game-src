#pragma once
// I N C L U D E S //////////////////////////////////////////////////
#include <windows.h>

// F U N C T I O N S ////////////////////////////////////////////////

//Functions for reading/writing ini files. Normally I'd put stuff like this in a class
//but in this case a class isn't really neccesary since there isn't any
//data we need to store, or anything that needs to be encapsulated.

//Basically, these are just aliases for Windows' GetPrivateProfile functions. Mainly, they're
//here just because it's easier to remember these, and the GetPrivateProfile functions
//aren't all that well known.

inline bool GetIniValue(char* Filename, char* Section, char* Key, char* Value) {
	GetPrivateProfileString(Section, Key, "nostring", Value, 128, Filename);
	return true;
}

inline int GetIniValue(char* Filename, char* Section, char* Key) {
	return GetPrivateProfileInt(Section, Key, 0, Filename);
}
inline float GetIniValueFloat(char* Filename, char* Section, char* Key, char* Value) {
	GetPrivateProfileString(Section, Key, "nostring", Value, 128, Filename);
	if(stricmp(Value, "nostring" ) == 0)
		return 0;
	return (float)atof(Value);
}

inline float GetIniValueFloat(char* Filename, char* Section, char* Key ) {
	char Buffer[128];
	GetPrivateProfileString(Section, Key, "nostring", Buffer, 128, Filename);
	if(stricmp(Buffer, "nostring" ) == 0)
		return 0;
	return (float)atof(Buffer);
}

inline void SetIniValue(char* Filename, char* Section, char* Key, char* NewValue) {
	WritePrivateProfileString ( Section, Key, NewValue, Filename );
	
}
inline void SetIniValue(char* Filename, char* Section, char* Key, int   NewValue) {
	char TempString[32];
	_itoa(NewValue, TempString, 10);
	WritePrivateProfileString ( Section, Key, TempString, Filename );
}


