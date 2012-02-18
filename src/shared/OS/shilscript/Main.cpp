#include "SHilScript.h"

#include <stdio.h>
#include <time.h>

using namespace SHilScript;

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

class MyOutputter : public Outputter
{
public:
	MyOutputter() {}
	~MyOutputter() {}
	int Write(const char * line)
	{
		printf("%s", line);
		return 1;
	}
};

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
	if(str.GetStr().find((char)0) == SSString::npos)
		printf("%s", str.GetStr().c_str());
	else
	{
		for(int i=0; i<(signed)str.GetStr().size(); i++)
			printf("%c", str.GetStr()[i]);
	}
	state->Push(Value(""));
	return 1;
}

int _SHILSCRIPT_CALL PromptForNumber(State * state, int params, void * user)
{
	Value str = state->Pop();

	int i;

	while(1)
	{
		char buf[256];

		printf("%s", str.GetStr().c_str());
		fgets(buf, 256, stdin);
		int result = sscanf(buf, " %d ", &i);

		if(result != 1)
			printf("Error, invalid input. Please retry.\n");
		else
			break;
	}

	state->Push(Value((Real)i));

	return 1;
}

int main(int argc, char ** argv)
{
	if(argc < 2)
	{
		printf("Error! Not enough parameters.\n");
		printf("Usage: shilscript.exe <scriptfile>\n");
		return 1;
	}

	long start = clock();

	MyInputter inputter(argv[1]);
	MyOutputter outputter;
	State state, state2;
	state.AddFunc("print", Print, 1, PARAMS_EXACT, 0);
	state.AddFunc("PromptForNumber", PromptForNumber, 1, PARAMS_EXACT, 0);
	state.AddFunc("sleep", SleepFunc, 1, PARAMS_EXACT, 0);

	int opcodes = 0;

	if(state.Create(&inputter, &outputter, 0))
		while(state.Step(100)) opcodes += 100;

	long diff = clock()-start;
	float seconds = diff*(1.0f/(float)CLOCKS_PER_SEC);

	printf("------------------------Execution Complete-------------------------\n");
	printf("Total time: %0.3f seconds\nAbout %d opcodes executed\n", seconds, opcodes);

	return 0;
}

