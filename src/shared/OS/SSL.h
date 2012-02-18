// I N C L U D E S /////////////////////////////////////
#pragma once
#include <memory.h>
#undef Real
#include "shilscript\SHilScript.h"
#undef Real
#define Real float
#include "..\gamegui\inputbox.h"
#include "..\network\socket.h"
#include "..\CVAR\CVAR.h"

_USING_SHILSCRIPT
// D E F I N E S ///////////////////////////////////////
#define StateHandle int
#define MAX_STATES 255 // I can't imagine ever having
					    // more than 255 shilscript programs
					    // running in lenix

#define MAX_CONNECTIONS 32

class MyOutputter;
class HatKernel;
class TaskManager;
// C L A S S E S ///////////////////////////////////////

//Handle loading shilscript files.
class MyInputter : public Inputter
{
private:
	FILE * fp;
public:
	MyInputter(const char * file) { fp = fopen(file, "rt"); }
	~MyInputter() { fclose(fp); }
	int ReadChar()
	{
		int c = fgetc(fp);
		if(c == EOF)
			return ENDOFSCRIPT;
		else
			return c;
	}
};




//Handles outputting things printed out by SHilscript;
class MyOutputter : public Outputter
{
public:
	InputBox* console;
public:
	MyOutputter() { console = NULL; }
	~MyOutputter() { }
	

	int Write(const char * line)
	{
		CVar.WriteLine( line );
		//if(console)
		//	console->InputLine( (char*)line );
		return 1;
	}
};

//Wrapper around a state. Later this might be
//extended to contain a message queue and such.
struct Script
{
	State VM;
	bool NeedKill;
	MyOutputter Out;
	

	Script () { NeedKill = false;  }
};


//passed to script api functions when they're called
struct ScriptAPIPacket
{
	TaskManager* HostTaskSystem;
	HatKernel*   HostKernel;
};

//Creates a "state" for a shilscript program with the
//neccesary library functions added
class TaskManager
{
protected:
	//List of "States", or basically running
	//proccesses (crazy shilscript notation!)
	Script* StateList[MAX_STATES];

	ScriptAPIPacket scriptpack;
	int CyclesToKill; // how many cycles to give a proccess
					  // before you kill it.

	HatKernel* Kernel;

	InputBox* DefaultConsole;

public:

	TaskManager() {
		memset( StateList, 0, MAX_STATES*4 );
		CyclesToKill = 1;
		Kernel = NULL;
		DefaultConsole = NULL; 
	}
	~TaskManager() {
		Shutdown();
	}

	

	void SetDefaultConsole( InputBox* c ) { DefaultConsole = c; }

	//Build the state object
	StateHandle BuildScript( char* filename, InputBox* destinationconsole ) ;
	
	//Get State
	Script* GetState( StateHandle s ) { return StateList[s]; } 

	void UseKernel ( HatKernel* KernelPtr ) { Kernel = KernelPtr; }

	//Kill a state. Doesn't kill immediatly, but
	//rather just tells each state it needs to shut down.
	void Kill(StateHandle s ); 

	//Tell all states to terminate.
	void KillAll();

	//Notifies all states that they needkill, at which
	//point it'll execute every script for 1000 cycles or
	//until it quits, then it'll clean up all resources
	void Shutdown();


	//Runs every script that's active.
	void RunActiveScripts ( int cyclesperscript );


	//Function calling thingy!
	void  AddFunctionParameter( StateHandle s, Value param ) ;
	Value GetFunctionReturnValue( StateHandle s );
	void  CallAppFunction( StateHandle s, char* name, int params);

	//Clean up all scripts that have ceased execution. Returns
	//true if all scripts were garbage collected, otherwise
	//returns false. If ForceKill is true, scripts are killed
	//even if they haven't completed, in which case the only
	//criteria is the NeedKill variable.
	bool GarbageCollect ( bool ForceKill );

};


class HatKernel
{
public:
	TaskManager TaskMan;
	GDSocket SockList[MAX_CONNECTIONS];
	


public:
	HatKernel() { TaskMan.UseKernel( this ); }
	void RunCMD( InputBox* Console, const char* Command );

	void UpdateOS();
};

HatKernel* GetSSLKernel();