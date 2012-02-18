#pragma once
#include <windows.h>

class FileDialog
{
protected:
	char Filename[255];
	

public:

	bool OpenLoad(HWND hParent, char* TitleBar, char* TypeString);
	bool OpenSave(HWND hParent, char* TitleBar, char* TypeString);

	char* GetFilename() { return Filename; }

};