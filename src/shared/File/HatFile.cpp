#include "HatFile.h"
#include <windows.h>

bool HatFile::LoadHatFile(char* filename)
{
	//Cleanup any remaining data
	Cleanup();

	
	//Load in the file into the File buffer
	FILE* file = fopen( filename, "rt" );

	if(!file)
		return false;

	fseek(file, 0, SEEK_END ); //find the file size
	int filesize = ftell( file );
	File = new char[filesize+1];
	ZeroMemory( File, filesize+1 );
	fseek(file, 0, SEEK_SET ); //return to the start of the file
	fread(File, sizeof(char), filesize, file);
	fclose( file );

	//Create a token list to build from
	GetWords( File, WordList );

	//CONSTRUCT TEH HAT TREe!1!
	TokenGroup* CurrentGroup = NULL;
	int i=0;
	while( i<WordList.size() )
	{

		if(WordList[i].compare( "BEGIN" ) == 0)
		{
			if(CurrentGroup == NULL)
			{
				Root = new TokenGroup;
				CurrentGroup = Root;
				i++;

				CurrentGroup->Name = WordList[i];
				
				
			}
			else
			{
				//Move to the next word (We'll need it
				//in a moment
				i++;

				//Create the new token group
				TokenGroup* NewGroup = new TokenGroup;
				NewGroup->Parent = CurrentGroup;
				NewGroup->Name = WordList[i];
				
				//Add the new token group to the parent
				CurrentGroup->ChildList.push_back( NewGroup);

				//Set the current group to the new group
				CurrentGroup = NewGroup;
				
			}
		}
		else if(WordList[i].compare( "END" ) == 0 && CurrentGroup)
		{
			//if this is the highest level group, just exit out
			//because there's nothing left
			if(CurrentGroup->Parent == NULL)
			{
				break;
			}
			else
				//Otherwise move back up one level.
				CurrentGroup = CurrentGroup->Parent;

			
		}
		else
		{
			//Not BEGIN or END, so it must be a token
			HatToken Token;
			Token.TokenName = WordList[i];
			i+=2;
			Token.TokenValue = WordList[i];
			CurrentGroup->TokenList.push_back(Token);
			
			

		}

		i++;

	}



	return true;
}


void HatFile::BuildStringFromNode( TokenGroup* Node )
{
	//Declarations
	int i=0;

	//Add tabs for each level deep it is
	for(i=0; i<LevelsDeep; i++)
		SaveFile += "\t";

	//Add "BEGIN NodeName"
	SaveFile += "BEGIN ";
	SaveFile += Node->Name ;
	SaveFile += "\n";

	//Increase the levels deep so that
	//everything below this is a bit more in
	LevelsDeep ++;


	//Write all the node's tokens into the file
	for(int n=0; n<Node->GetTokenCount(); n++)
	{
		for(i=0; i<LevelsDeep; i++)
			SaveFile += "\t";

		SaveFile += Node->TokenList[n].TokenName;
		SaveFile += " = ";
		SaveFile += Node->TokenList[n].TokenValue;
		SaveFile += "\n";
		
	}

	//Save all the nodes below this node
	for(int m=0; m<Node->GetSectionCount(); m++)
		BuildStringFromNode( Node->ChildList[m] );

	LevelsDeep --;

	for(i=0; i<LevelsDeep; i++)
		SaveFile += "\t";
	SaveFile += "END\n";
	
	
}

bool HatFile::SaveHatFile( char* filename )
{
	FILE* file = fopen( filename, "wt" );

	if(file == NULL)
		return false;

	SaveFile = "";
	BuildStringFromNode( Root );

	fwrite((char*)SaveFile.c_str(), sizeof(char), SaveFile.size(), file );

	fclose(file);
	
	return true;
}

void HatFile::Cleanup()
{
	if(File)
		delete [] File;
	
	if(Root)
		delete Root;
	
	File = NULL;
	Root = NULL;

}



HatToken* TokenGroup::GetTokenByStr( std::string TokenName )
{
	
	for(int i=0; i<TokenList.size(); i++)
	{
		if( TokenList[i].TokenName.compare( TokenName ) == 0)
			return &TokenList[i];
	}

	return NULL;
}


