#include "Conversation.h"


#include "..\defs\defs.h"

//Load a conversation
bool Conversation::LoadFile ( char* file )
{	
	// definitions
	HatFile CHF; // Conversation hat file
	TokenGroup* Root; //the root token group
	

	//Load in the hat file
    bool Ok = CHF.LoadHatFile( file );

	if(!Ok)
		return false;


	//Get the root of the hat file
	Root = CHF.GetRoot();
	if(!Root)
		return false;

	//Now, basically we need to traverse the tree and convert the
	//"Token groups" into conversation frames
	RootFrame = new ConversationFrame;
	CurrentFrame = RootFrame;
	CurrentFrame->Parent = NULL;
	BuildTree( Root );

	//make sure the current frame is where it should be
	ResetConversation();

	//JOORAH!
	return true;
}


//Build the conversation tree
void Conversation::BuildTree ( TokenGroup* CurrentGroup )
{
	//Definitions
	HatToken*   CurrentToken;
	TokenGroup* iGroup; //iterated group


	//Iterate through the tokens and fill out the 
	//conversation frame's structure
	for(int i=0; i<CurrentGroup->GetTokenCount(); i++)
	{
		CurrentToken = CurrentGroup->GetToken( i );
		
		if( CurrentToken->TokenName.compare( "Say" ) == 0)
		{
			CurrentFrame->Text = CurrentToken->TokenValue;
		}

		else if( CurrentToken->TokenName.compare( "Option" ) == 0)
		{
			CurrentFrame->AvatarText = CurrentToken->TokenValue;
		}

	}

	//Now build up child frames (if we need to)
	int SectionCount = CurrentGroup->GetSectionCount();
	if(SectionCount >3)
		SectionCount = 3;

	ConversationFrame* NewFrame;
	ConversationFrame* OldFrame=CurrentFrame;
	for(int j=0; j<SectionCount; j++)
	{
		//Get the group that we next need to recurse into
        iGroup = CurrentGroup->GetChildSection( j );
		

		//Build a frame for the new group
		NewFrame = new ConversationFrame;
		NewFrame->Parent = CurrentFrame;
		CurrentFrame = NewFrame;

		OldFrame->Choice[j] = CurrentFrame;

		

		//Construct the frame, and it's children if neccesary
		BuildTree( iGroup );

	}


	//About to exit function, so set the current frame back to its
	//parent, unless it's null, because then its at the root.
	if(CurrentFrame->Parent != NULL)
		CurrentFrame = CurrentFrame->Parent;

}




//Set the current frame to a frame one level deeper
//based on the option chosen
void Conversation::ChooseOption( int option_number )
{
	if(CurrentFrame)
	{
		if(CurrentFrame->Choice[option_number])
			CurrentFrame = CurrentFrame->Choice[option_number];
	}

}