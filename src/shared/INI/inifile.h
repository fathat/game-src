// I N C L U D E S ///////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "ini.h"

// D E F I N E S /////////////////////////////////////////

struct KeyValue
{
	std::string Key;
};


struct Section
{
	std::string Identification;
	std::vector<KeyValue> KeyValueList;

	void BuildKeyValueList( char* filename );
	
};


// C L A S S E S /////////////////////////////////////////


class INIFile
{
public:

	//Stores the INI file's structure
	std::vector< Section > SectionList;
	std::string FileName;

	char TempBuffer[1024];

public:
	bool LoadFile( char* filename );

	int   GetINT( char* Section, char* Key ) {
		return GetIniValue( (char*)FileName.c_str(), Section, Key );
	}
	float GetFloat( char* Section, char* Key ) {
		return GetIniValueFloat( (char*)FileName.c_str(), Section, Key, TempBuffer ); 
	}
	std::string GetString( char*Section, char* Key ) {
		std::string ReturnString;
		GetIniValue( (char*)FileName.c_str(), Section, Key, TempBuffer );
		ReturnString = TempBuffer;

		return ReturnString;
	}


};