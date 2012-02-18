#include "Network.h"
#include "..\shared\network\messages.h"
#include "..\shared\s3dlib\dxutil.h"

bool NetworkManager::Init()
{
	return true;
}


void NetworkManager::Shutdown()
{
	NS = NS_BLANK;
}



bool NetworkManager::JoinGame(char* GameName, char* PlayerName)
{

	return true;
}

int NetworkManager::GetQueuedMessages()
{

	return 0;
}

bool NetworkManager::SendToServer(BYTE* Packet, bool Reliable)
{

	return true;
}

void NetworkManager::AddIncoming( BYTE* Message )
{
}


bool NetworkManager::Receive(BYTE *buffer)
{

	return true;
}