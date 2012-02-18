// I N C L U D E S ///////////////////////////
#pragma once
#include "..\s3dlib\types.h"
#include "..\File\hatfile.h"

// D E F I N E S /////////////////////////////


// C L A S S E S /////////////////////////////

//Time frame in a conversation, ie
struct ConversationFrame
{
     //Timing variables, set FrameLength to 0
     //if it should only end on a key press;
     Real FrameLength;
     Real TimeLeft;

     //stuff to write, usually conversation
     //junk
     std::string Text;       // what
     std::string AvatarText; // what the player said to start
							 // this conversation frame



     //Conversation choices
     ConversationFrame* Choice[3];
	 ConversationFrame* Parent; // choice that spawned this
								// frame

	 ConversationFrame() { FrameLength = 0; 
		TimeLeft = 0; 
		Choice[0] = NULL;
		Choice[1] = NULL;
		Choice[2] = NULL;
		Parent  = NULL;
	 }
	 ~ConversationFrame() {
		 if(Choice[0])
			 delete Choice[0];
		 if(Choice[1])
			 delete Choice[1];
		 if(Choice[2])
			 delete Choice[2];
	 }


};


//Represents a conversation tree
class Conversation
{
protected:

	//"Frame's", ie, conversation fragments
	//for each level
	ConversationFrame* RootFrame;    //Frame that contains all subframes
	ConversationFrame* CurrentFrame; //Frame where current conversation
									 //is going on
	int CurrentLevel;

public:

	//Constructor/destructor
	Conversation() {
		RootFrame    = NULL;
		CurrentFrame = NULL;
	}

	~Conversation() {
		Cleanup();
	}

	//Load a conversation
	bool LoadFile ( char* file );
	void BuildTree ( TokenGroup* CurrentGroup );
	void Cleanup() {
		if(RootFrame)
			delete RootFrame;
		RootFrame    = NULL;
		CurrentFrame = NULL;

	}

	//Conversation options
	void ResetConversation ( ) { CurrentFrame = RootFrame; }
	void ChooseOption( int option_number );


	ConversationFrame* GetCurrentFrame() { return CurrentFrame; }
	ConversationFrame* GetRootFrame() { return RootFrame; }


};