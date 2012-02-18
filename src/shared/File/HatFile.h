// I N C L U D E S ////////////////////////////////////////
#pragma once
#include <string>
#include <vector>
#include "..\network\messageparser.h"

// D E F I N E S //////////////////////////////////////////



// C L A S S E S //////////////////////////////////////////


// Represents an x=y value in the hat file
struct HatToken
{
	std::string TokenName;
	std::string TokenValue;
};

// A group of tokens
struct TokenGroup
{

	//Name of the section, set at "BEGIN name"
	std::string Name;

	//List of "tokens" (x=y things) this group has
	std::vector<HatToken> TokenList;
	
	//List of groups under this group
	std::vector<TokenGroup*> ChildList;
	
	//Parent of this token group, NULL if it's the root
	TokenGroup* Parent;


	//Constructar!
	TokenGroup() {
		Parent = NULL;
	}

	~TokenGroup() {
		for(int i=0; i<ChildList.size(); i++)
		{
			delete ChildList[i];
		}
		ChildList.clear();
	}

	//functions
	int GetSectionCount() { return ChildList.size(); }
	int GetTokenCount() { return TokenList.size(); }

	TokenGroup* GetChildSection( int index ) { return ChildList[index]; }
	HatToken* GetToken( int index ) { return &TokenList[index]; }
	HatToken* GetTokenByStr( std::string TokenName );
};



// represents a hat file
class HatFile
{
protected:
	TokenGroup* Root;

	//Variables mostly used for loading
	char* File;
	StringArray WordList;
	std::string SaveFile;
	int LevelsDeep;


public:

	HatFile() { File = NULL; Root = NULL;}
	~HatFile() {
		Cleanup();
	}

	TokenGroup* GetRoot() { return Root; }

	bool LoadHatFile(char* filename);
	bool SaveHatFile(char* filename);
	void Cleanup();

	void BuildStringFromNode( TokenGroup* Node );
	
};