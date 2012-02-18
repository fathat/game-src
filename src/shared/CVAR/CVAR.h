// I N C L U D E S ////////////////////////////////////
#pragma once
#include <string>
#include <map>
#include "..\GameGUI\inputbox.h"
#include "..\s3dlib\Game Timer.h"

// D E F I N E S //////////////////////////////////////

struct TimeProfile
{
	double  StartTime; 
	double  TotalTimeSpent;
	__int64 TimesExecuted;

	TimeProfile()
	{
		StartTime      = 0;
		TotalTimeSpent = 0;
		TimesExecuted  = 0;
	}
	
};

typedef std::map<std::string, std::string> StringMap;
typedef std::map<std::string, float> FloatMap;
typedef std::map<std::string, int> IntMap;
typedef std::map<std::string, TimeProfile> TimeMap;

#define PROFILE 1


#ifdef PROFILE
#define START_TIMING  CVar.StartProfile( __FUNCSIG__ ); 
#define STOP_TIMING CVar.EndProfile( __FUNCSIG__ ); 
#define START_TIMING_BLOCK(a) CVar.StartProfile(a);
#define STOP_TIMING_BLOCK(a) CVar.EndProfile(a);
#define TIME_FUNCTION ScopedTimer localFunctionTimer( __FUNCSIG__ );
#else
#define START_TIMING
#define STOP_TIMING
#define START_TIMING_BLOCK(a)
#define STOP_TIMING_BLOCK(a)
#endif



// C L A S S E S //////////////////////////////////////



//Manages the global scripting/debug/gameplay variables
class CVARManager
{
protected:

	StringMap VarMapS;
	FloatMap  VarMapF;
	IntMap    VarMapI;
	
	
	TimeMap ProfileMap;

	InputBox* pConsole;

public:
	GameTimer* Timer;

public:
	CVARManager() { pConsole = NULL; Timer = NULL; }

	//profiling stuff
	void  StartProfile ( std::string BlockName ); 
	float EndProfile( std::string BlockName );

	TimeProfile& GetStartTime( std::string BlockName );

	//OMFMOG
	void  SetVarS ( std::string Name, const char* value );
	void  SetVarF ( std::string Name, float value );
	void  SetVarI ( std::string Name, int   value );

	std::string GetVarS ( std::string Name );
	float GetVarF ( std::string Name );
	int   GetVarI ( std::string Name );



	void  OutputAllVars ( );
	void  OutputTimingData ( );
	

	//writes stuff to the console, don't really need this in this class
	//but it's useful to have it here since it's global and such and such
	void  WriteLine( const char* c, ... );

	//set the console
	void SetConsole( InputBox* c ) { pConsole = c; }
};

extern CVARManager CVar;


//A scoped timer. Stops timing as soon as it goes out of scope.
struct ScopedTimer
{
	std::string BlockName;
	ScopedTimer( char* name ) 
	{
		BlockName = name;
		START_TIMING_BLOCK ( name )
	}
	~ScopedTimer()
	{
		STOP_TIMING_BLOCK( BlockName.c_str() );	
	}
};