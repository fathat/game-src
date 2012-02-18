// I N C L U D E S //////////////////////////////////
#pragma once
#include <vector>

// D E F I N E S ////////////////////////////////////

#define  StringArray std::vector<std::string>


// F U N C T I O N S ////////////////////////////////
//utility class for parsing messages in multiple ways
bool FoundWord(char* text, StringArray& input);
int  GetWords (char* string, StringArray& output); 
