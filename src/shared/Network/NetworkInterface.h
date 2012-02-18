// I N C L U D E S /////////////////////////////////////////////
#pragma once

#include <windows.h>
#include <vector>


// D E F I N E S ///////////////////////////////////////////////
enum NetworkState { NS_BLANK,      //Network is uninitialized and cannot do anything
					NS_READY,      //Network is initialized, but not connected to anything
					NS_CONNECTING, //Network is in the proccess of connecting
					NS_CONNECTED   //Network is connected and completly ready to go
					
};

#define MAX_PACKETS 1024

// C L A S S E S ///////////////////////////////////////////////


class INetworkManager
{
	virtual bool Init() { return false; }
	virtual void Shutdown() {}

	virtual int GetPacketNumber() { return 0; }
	virtual int GetQueuedMessages() { return 0; }
	

	virtual bool JoinGame(char* GameName, char* PlayerName) { return false; }
	virtual void ExitGame(){ }

	virtual NetworkState CheckNetworkState() { return NS_BLANK; }

	virtual bool SendToServer( BYTE* Packet, bool Reliable=true) { return false ;}
	
	virtual void AddIncoming( BYTE* Message ) {} 
	virtual bool Receive( BYTE* buffer ) { return false ; }
};
