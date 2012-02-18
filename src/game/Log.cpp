#include "log.h"
#include <fstream>
#include <stdlib.h>

using namespace std;


extern BOOL LogActions;


void ClearLogs()
{
	ofstream file;

	file.open("debug.txt", ios::out );
	//file.write(" ", 1);
	file.close();

}

void OutputMessage(char* text)
{
	if(!LogActions)
		return;

	ofstream file;

	file.open("debug.txt", ios::app);
	file.write(text, strlen(text));
	file.close();

}

void OutputMessage(int num)
{
	ofstream file;

	char numtext[15];

	itoa(num, numtext, 10);

	file.open("debug.txt", ios::app);
	file.write(numtext, strlen(numtext));
	file.close();

}