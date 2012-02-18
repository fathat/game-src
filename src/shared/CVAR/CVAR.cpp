#include "CVar.h"
#include <iostream>
using namespace std;

#include "..\defs\defs.h"

CVARManager CVar;


//profiling stuff
void  CVARManager::StartProfile ( std::string BlockName )
{
	ProfileMap[ BlockName ].StartTime = Timer->GetTime() ;
	ProfileMap[ BlockName ].TimesExecuted = 1;
}

float CVARManager::EndProfile( std::string BlockName )
{
	Real f = Timer->GetTime() - ProfileMap[ BlockName ].StartTime;
	ProfileMap[ BlockName ].TotalTimeSpent = f; ;
	return f;
}

void  CVARManager::SetVarS ( std::string Name, const char* value )
{
	std::string NewValue = value;
	VarMapS[Name] = NewValue;
}

void  CVARManager::SetVarF ( std::string Name, float value )
{
	VarMapF[Name] = value;
}

void  CVARManager::SetVarI ( std::string Name, int   value )
{
	VarMapI[Name] = value;
}



std::string CVARManager::GetVarS ( std::string Name )
{
	StringMap::const_iterator i;
	i = VarMapS.find( Name );

	if(i == VarMapS.end() )
		return std::string("[null]"); //didn't find anything
	
	return i->second;
}

float CVARManager::GetVarF ( std::string Name )
{

	//find the var
	FloatMap::const_iterator i = VarMapF.find( Name );
	if(i == VarMapF.end() )
		return 0.0f; // didn't find anything, so return 0.
					 // there's probably a better way to handle this,
					 // but the old code already does something similiar
	
	//return the value
	return i->second;
}

int   CVARManager::GetVarI ( std::string Name )
{

	//Find our var
	IntMap::const_iterator i = VarMapI.find( Name );
	if(i == VarMapI.end() )
		return 0; //didn't find anything, return 0;
	
	//return the value
	return i->second;
}



void  CVARManager::OutputTimingData ( )
{

	//print out all string values
	TimeMap::const_iterator i;
	char REALLYBIGBUFFER[512];
	for ( i = ProfileMap.begin( ); i != ProfileMap.end( ); i++ )
	{
		WriteLine( "%s = %f ms", (i->first).c_str(), (float)(i->second).TotalTimeSpent * 1000 );
	}

}


void  CVARManager::OutputAllVars ( )
{

	//print out all string values
	StringMap::const_iterator i;
	for ( i = VarMapS.begin( ); i != VarMapS.end( ); i++ )
	{
		WriteLine( "%s = %s", (i->first).c_str(), (i->second).c_str() );
	}

	//print out all float values
	FloatMap::const_iterator i2;
	for ( i2 = VarMapF.begin( ); i2 != VarMapF.end( ); i2++ )
	{
		WriteLine( "%s = %f", (i2->first).c_str(), (i2->second) );
	}

	//print out all int values
	IntMap::const_iterator i3;
	for ( i3 = VarMapI.begin( ); i3 != VarMapI.end( ); i3++ )
	{
		WriteLine( "%s = %d", (i3->first).c_str(), (i3->second) );
	}

}

void  CVARManager::WriteLine( const char* c, ... )
{
	if( strlen(c) < 1)
		return;
	
	char* str = new char[512];
	ZeroMemory(str, sizeof(char) * 512 );
	va_list	argumentlist;
	va_start(argumentlist, c);
	vsprintf(str, c, argumentlist);
	va_end(argumentlist);

	if(pConsole)
		pConsole->InputLine( str );

#ifdef DEBUG
    OutputDebugString( str );
	OutputDebugString( "\n" );
#endif

#ifdef _CONSOLE
	cout << str << endl; 
#endif 

	OutputMessage( str );

	delete [] str;
}