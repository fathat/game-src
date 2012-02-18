#include "MessageParser.h"
#include <windows.h>


//////// GetWords /////////////////////////////
//
// Description:
//
// Gives you back a list of words from any string
// you pass it.
//
///////////////////////////////////////////////

int GetWords (char* str, StringArray& output) //returns number of words
{
	//definitions
	bool WordChars=false;
	int WordCount=0;
	int offset=0;

	char* string;
	string = new char[strlen(str)+2];
	strcpy(string, str );
	strcat( string, "\n" );


	bool ReadingString = false;
	int length = strlen(string);
	//first figure out the word count
	for(int c=0; c<length; c++)
	{

		//check to see if we aren't currently reading word characters
		if(WordChars == false )
		{
			//check to see if it isn't a space
			if(string[c] != 32 && string[c] != '\r' && string[c] != '\n' && string[c] != '\t' )
			{

				//yay! start reading word characters
				WordCount++;
				WordChars = true;

				//store the offset -- we use this to fill in the character array.
				offset = c; 
				
			}
		}

		if( string[c] == '\"' )
		{
			if(ReadingString)
				ReadingString = false;
			else
				ReadingString = true;
		}

		if( ReadingString == true)
		{
			if( (( ( string[c] == '\r' || string[c] == '\n'  )  ) || c==strlen(string) || string[c]==NULL) && (WordChars == true) )
				ReadingString = false;
		}


		//stop reading words if we find one of these characters or if the word gets too long.

		if( ReadingString == false )
		{
			if( (( (string[c] == 32 || string[c] == '\r' || string[c] == '\n' || string[c] == '\t'  )  ) || c==strlen(string) || string[c]==NULL) && (WordChars == true) )
			{

				//declarations
				
				std::string AddString;

				WordChars = false;
				char* ParserTempText = new char[(c-offset)+1];


				//add it to the list
				memcpy(ParserTempText, &string[offset], c-offset);
				ParserTempText[c-offset] = 0;
				AddString = ParserTempText;            
				output.push_back(AddString);

				delete [] ParserTempText;
			}
		}
		
	}



	//return the number of words in the list.
	return WordCount;

}


//////// FoundWord ////////////////////////////
//
// Description:
//
// Use this to see if a certain word is in 
// the word list. Returns true if it finds
// the word.
//
///////////////////////////////////////////////

bool FoundWord(char *text, StringArray &input)
{

	for(int i=0; i<input.size(); i++)
	{
		if(strcmpi(text, input[i].c_str()) == 0)
			return true;
	}

	return false;

}

