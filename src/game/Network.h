#pragma once

// I N C L U D E S /////////////////////////////////////////////
#include "..\\shared\\network\\NetworkInterface.h"
#include <list>


// D E F I N E S ///////////////////////////////////////////////
static const GUID GUID_CHATSERVER = { 0xf2b1f801, 0x4829, 0x4bba, { 0xb8, 0x60, 0xb9, 0x10, 0xfa, 0xa2, 0x76, 0x41 } };


// C L A S S E S ///////////////////////////////////////////////



class NetworkManager : INetworkManager
{
protected:
	NetworkState NS;

	int PacketNumber;
	int SendTimeout;
	
	std::list<BYTE*> Incoming;

public:
	NetworkManager() { NS = NS_BLANK; SendTimeout=30;}
	~NetworkManager() { Shutdown(); }

	bool Init();
	void Shutdown();

	int GetPacketNumber() { return PacketNumber; }
	int GetQueuedMessages();
	

	bool JoinGame(char* GameName, char* PlayerName);
	void ExitGame(){}

	NetworkState CheckNetworkState() { return NS;}

	//This *must* be set by the directplay message handler when a CONNECT_COMPLETE
	//mesage comes in order for CheckNetworkState to work properly on connections.
	void ConnectComplete( HRESULT hr ) { }

	bool SendToServer( BYTE* Packet, bool Reliable=true);

	
	void AddIncoming( BYTE* Message );
	bool Receive( BYTE* buffer );
};