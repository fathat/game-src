#include "SSL.H"
#include <stdio.h>
#include <time.h>
#include "..\\world\\scriptapi.h"

using namespace SHilScript;

struct SleepInfo
{
	clock_t	end;
};

int _SHILSCRIPT_CALL SleepCallback(State * state, void * user, int mode)
{
	SleepInfo * si = (SleepInfo*)user;

	if(mode == WAIT_QUERY)
	{
		if(clock() > si->end)
		{
			delete si;
			return 1;
		}
		else
			return 0;
	}
	else // mode == WAIT_TERM
	{
		delete si;
		return 1;
	}
}

int _SHILSCRIPT_CALL SleepFunc(State * state, int params, void * user)
{
	Value cycles = state->Pop();
	SleepInfo* si = new SleepInfo;
	si->end = clock() + int(cycles.GetNum());
	state->SetWaitFunc(SleepCallback, si);
	return 1;
}

int _SHILSCRIPT_CALL Print(State * state, int params, void * user)
{
	Value str = state->Pop();
	char outbuffer [2048];
	sprintf(outbuffer, "%s", str.GetStr().c_str());
	state->GetOutputter()->Write(  outbuffer );
	state->Push(Value(""));

	return 1;
}

int _SHILSCRIPT_CALL CheckForKill(State * state, int params, void * user)
{
	Script* S = (Script*) user;
	state->Push(Value( S->NeedKill  ));

	return 1;
}

//Build the state object
StateHandle TaskManager::BuildScript( char* filename, InputBox* destinationconsole ) 
{
	for(int i=0; i<MAX_STATES; i++)
	{
		if(	StateList[i] == NULL)
		{
			StateList[i] = new Script;

			MyInputter In( filename );

			if(destinationconsole== NULL)
				destinationconsole = DefaultConsole;

			StateList[i]->Out.console = destinationconsole;
			StateList[i]->VM.SetUserVal( NULL );
			StateList[i]->VM.AddFunc("Print", Print, 1, PARAMS_EXACT, NULL);
			StateList[i]->VM.AddFunc("Sleep", SleepFunc, 1, PARAMS_EXACT, NULL);
			StateList[i]->VM.AddFunc("CheckForKill", CheckForKill, 0, PARAMS_EXACT, &StateList[i]);
			ScriptAPI::AddApiFunctions( StateList[i] );
			StateList[i]->VM.Create(&In, &StateList[i]->Out,  NULL);
				
			
			
			return i;
			

		}
	}

	return -1;
}

//Kill a state. Doesn't kill immediatly, but
//rather just tells each state it needs to shut down.
void TaskManager::Kill(StateHandle s )
{
	if(StateList[s])
	{
		StateList[s]->NeedKill = true;
		StateList[s]->Out.console->SetShowInput( true );
	}
}

//Tell all states to terminate.
void TaskManager::KillAll()
{
	for(int i=0; i < MAX_STATES; i++)
		Kill(i);
	

}

//Notifies all states that they needkill, at which
//point it'll execute every script for 1000 cycles or
//until it quits, then it'll clean up all resources
void TaskManager::Shutdown()
{
	KillAll();
	RunActiveScripts(this->CyclesToKill);
	if(!GarbageCollect(false))
	{
		//give out some message that not everything died here

		//now force everything out
		GarbageCollect(true);
	}
}


//Runs every script that's active.
void TaskManager::RunActiveScripts ( int cyclesperscript )
{
	for(int i=0; i<MAX_STATES; i++)
	{
		if(	StateList[i] )
			StateList[i]->VM.Step( cyclesperscript);
	}
	
}


//Add a parameter for the function that's going to be called
void  TaskManager::AddFunctionParameter( StateHandle s, Value param ) 
{
	if(StateList[s])
		StateList[s]->VM.Push( param );
}

//get return value from the last called function
Value TaskManager::GetFunctionReturnValue( StateHandle s )
{
	if(StateList[s])
		return StateList[s]->VM.Pop();

	return Value(0);
}

//Call a function in a script
void  TaskManager::CallAppFunction( StateHandle s, char* name, int params)
{
	if(StateList[s])
	{
		StateList[s]->VM.CallFunc( name, params);
		while(StateList[s]->VM.Step( 1000));
	}
	
}

//Clean up all scripts that have ceased execution. Returns
//true if all scripts were garbage collected, otherwise
//returns false.
bool TaskManager::GarbageCollect ( bool ForceKill )
{
	bool KilledAll=true;
	for(int i=0; i<MAX_STATES; i++)
	{
		if(StateList[i])
		{
			if(StateList[i]->VM.Done() || ForceKill)
			{
				StateList[i]->Out.console->SetShowInput( true );
				delete StateList[i];
				StateList[i] = NULL;
			}
			else
			{
				KilledAll = false;
			}
		}
	}

	return KilledAll;
}

//Runs a script file with the specified name
void HatKernel::RunCMD( InputBox* Console, const char* Command )
{
	//Parse the text into a vector of words that
	//we can use.
	std::string TextString = Command;
	StringArray WordList;
	if( !GetWords( (char*)TextString.c_str(), WordList) )
		return ;

	if(TextString.compare("date\n") == 0)
	{
		Console->InputLine( "  `777`O__________O!" );		
	}

	//not a command
	else
	{
		//Check if it's a program the user wants to run
		int endlpos = TextString.find("\n", 0);
		
		
		if ( endlpos != string::npos )
			TextString[endlpos] = 0;
		std::string FileName;
		FileName = "..\\scripts\\";
		FileName += TextString.c_str();
		FileName += ".shil";
		Console->InputLine((char*) FileName.c_str() );

		FILE* File = fopen( (char*)FileName.c_str(), "r+t");
		if(File)
		{
			fclose(File);
			if( TaskMan.BuildScript( (char*)FileName.c_str(), Console ) != -1)
			{
				Console->SetText("");
				
			}
		}

		//Not a program, so tell the user they entered an invalid
		//command or some such thing.
		else
		{
			std::string out;
			out = "`700` error: ";
			out += TextString;
			out += ": command not found.";
			
			Console->InputLine((char*) out.c_str() );
		}
	}

}

void HatKernel::UpdateOS()
{
	TaskMan.RunActiveScripts( 100 );
	TaskMan.GarbageCollect( false );
}