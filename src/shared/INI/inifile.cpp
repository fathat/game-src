#include "IniFile.h"


void Section::BuildKeyValueList( char* filename )
{
	//Declarations
	char Buffer[2048];
	char Name[128];
	bool Done=false; //Records if we've finished collecting the names
	int  i=0; //index 
	int zerocount=0;
	bool NoWrite = false;


	//Make a list off all the names in the section
	ZeroMemory(Buffer, 2048);
	ZeroMemory(Name, 128);
	char* ID = (char*)Identification.c_str();
	GetPrivateProfileSection( ID ,  Buffer, 2048, filename );

	

	//Now collect the names
	Done=false;	
	while(!Done)
	{

		//If there's a character value,
		//add it to our name buffer.
		if(Buffer[i])
		{
			if(Buffer[i] == '=')
				NoWrite=true;


			if(!NoWrite)
			{
				char onechar[2];
				onechar[0] = Buffer[i];
				onechar[1] = 0;
				strcat( Name, onechar );
				zerocount=0;
			}
		}
		
		//Otherwise, insert the new section into the
		//list and move onto the next section.
		else
		{
			//Create the new section if the name buffer isn't
			//empty
			if(strcmpi(Name, "" ) )
			{
				KeyValue KV;
				KV.Key = Name;

				KeyValueList.push_back(KV);
				NoWrite = false;
			}

			//Reset the name buffer
			strcpy(Name, "");

			//Increase the zero count
			zerocount++;

		}

		//if there are two zero's in a row, then
		//we're at the end of the list
		if(zerocount == 2)
			Done = true;

		i++;
				
	}




}

bool INIFile::LoadFile( char* filename )
{
	//Declarations
	char Buffer[2048];

	//Make a list off all the section names
	GetPrivateProfileSectionNames( Buffer, 2048, filename );
	
	FileName = filename;
	
	char Name[128];
	strcpy(Name, "");
	bool Done=false; //Records if we've finished collecting the names
	int  i=0; //index 
	int zerocount=0;
	bool NoWrite=false;
	
	while(!Done)
	{

		//If there's a character value,
		//add it to our name buffer.
		if(Buffer[i] )
		{
			if(Buffer[i] == '=')
				NoWrite=true;


			if(!NoWrite)
			{
				char onechar[2];
				onechar[0] = Buffer[i];
				onechar[1] = 0;
				strcat(Name, onechar );
			}
			
			
			zerocount=0;
		}

		
		//Otherwise, insert the new section into the
		//list and move onto the next section.
		else
		{
			//Create the new section if the name buffer isn't
			//empty
			if(strcmpi( Name,  "" ) )
			{
				Section NewSection;
				NewSection.Identification = Name;
				NewSection.BuildKeyValueList( filename );
				SectionList.push_back(NewSection);
				NoWrite=false;
			}

			//Reset the name buffer
			strcpy(Name, "");

			//Increase the zero count
			zerocount++;

		}

		//if there are two zero's in a row, then
		//we're at the end of the list
		if(zerocount == 2)
			Done = true;

		i++;
				
	}

	return true;

}