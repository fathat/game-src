#pragma once

// I N C L U D E S ///////////////////////////////////
#include <windows.h>

// D E F I N E S /////////////////////////////////////
const int MSG_NONE       =0;
const int MSG_TEXTMSG    =1;
const int MSG_CONNECTED  =2;
const int MSG_ADDENTITY  =3;
const int MSG_REMENTITY  =4;
const int MSG_AUTHFAILED =5;
const int MSG_MOVEENTITY =6;
const int MSG_SETLOCALPLAYERID =7;
const int MSG_FIREGUN    =8;

#ifndef Real
#define Real float
#endif


// C L A S S E S /////////////////////////////////////

#pragma pack(1)

//General header that all messages must have
struct M3DPacketHeader
{
	M3DPacketHeader() { MessageType = MSG_NONE; Size = sizeof(M3DPacketHeader); }
	
	SHORT Size;
	BYTE  MessageType;
	DWORD PacketNumber;
	
};


//Sends a text message (such as chat, or action descriptions or whatnot)
struct M3DMessagePacket : M3DPacketHeader
{
	M3DMessagePacket() { 
		MessageType = MSG_TEXTMSG; Size = sizeof(M3DMessagePacket); 
		ZeroMemory( Message, 128 );
	}

	char Message[128];
};



//Notifies the server that the user has connected and wishes to recieve
//gamestate data.
struct M3DConnected : M3DPacketHeader
{
	M3DConnected() { MessageType = MSG_CONNECTED; Size = sizeof(M3DConnected); }
};


//Used for multiple purposes: used to confirm a login for a player (NetworkID becomes username,
//ID becomes what the players Id will be in the game, and typename is an avatar object type).
//Also used by the server to indicate to other clients to add an object to their list.  
struct M3DAddEntPacket : M3DPacketHeader
{
	M3DAddEntPacket() { MessageType = MSG_ADDENTITY; Size = sizeof(M3DAddEntPacket); }
	
	char NetworkID[32];  //Name of the entity, like "KILLARAOLDEWD141293" or something
	char ClassName[128]; //Generic type, IE "npc", "sceneobject", etc.
	char SubType[128];   //Subtype, like "woman", "soldier", etc. Basically an
						 //object of the class type above.
	int  ID;             //Identity of the entity
	bool LocalPlayer;

};

//Informs players to remove an object/player from their list
struct M3DRemEntPacket : M3DPacketHeader
{
	M3DRemEntPacket() { MessageType = MSG_REMENTITY; Size = sizeof(M3DRemEntPacket); }

	int  ID;
	
};

struct M3DSetLocalPlayerID : M3DPacketHeader
{
	M3DSetLocalPlayerID() { MessageType = MSG_SETLOCALPLAYERID; Size = sizeof(M3DSetLocalPlayerID); ID=-1; }
    	
	int ID;
};

//Informs a user if they were unable to connect because their username or password was incorrect
struct M3DAuthenticationFailed : M3DPacketHeader
{
	M3DAuthenticationFailed() { MessageType = MSG_AUTHFAILED; Size = sizeof(M3DAuthenticationFailed); }
	
};


//Sent whenever an object has updated its position
struct M3DMoveEnt : M3DPacketHeader
{
	M3DMoveEnt() { MessageType = MSG_MOVEENTITY; Size = sizeof(M3DMoveEnt); }

	short ID;
	Real x, y, z;
	Real vx, vy, vz;
	Real Yaw, Pitch;
	
};



//Weapon stuff!

struct M3DLoadWeapon : M3DPacketHeader
{
	int EntityID;  //person loading this weapon
	char WeaponFile[64];
	int ClipAmmo;
	int UnloadedAmmo;
};


struct M3DFirePacket : M3DPacketHeader
{
	M3DFirePacket() { Size = sizeof(M3DFirePacket); MessageType = MSG_FIREGUN; }
	short ShooterID;
	Real x, y, z;
	Real Yaw, Pitch, Roll;
};

#pragma pack(8)