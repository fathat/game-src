#pragma once

#include <winsock2.h>

//send/receive flags
#define SOCKET_BLOCK  0x01


//////// Socket /////////////////////////////
//
// Description:
//
// Simple class which represents a WinSock
// socket.
//
// Inherits From:
//
// - Nothing
//
// TODO:
//
// - add in support for UDP and reliable
//   sends over UDP
// - more "server friendly" (implement
//   server functions).
//
// Known Bugs:
//
// - none
//
///////////////////////////////////////////////

class GDSocket
{
protected:
	SOCKET Socket;
	bool Connected;
	int  Timeout;   //predefined timeout for blocking

	UINT BytesSent;
	UINT BytesReceived;


	
public:

	SOCKET operator=(const GDSocket &arg);
	operator SOCKET ();


	GDSocket() { Connected = false; Timeout = 5000; Socket = NULL; BytesSent = 0; BytesReceived = 0;}
	~GDSocket() { if(Connected) Disconnect(); }
	
	
	bool Connect ( char* RemoteAddress, int port );
	void Disconnect ();


	void  StripEscapeCodes(char* string, char* output);
	int   Receive(char* Buffer); //returns a pointer to the message, and its size
	bool  Send (char* data, int size, int flags);
	

	void   SetConnected(bool state) { Connected = state; }
	SOCKET GetSocket()              { return Socket;     }
	void   SetSocket(SOCKET socket) { Socket = socket;   }
	void   SetTimeout(int timeout)  { Timeout = timeout; }

	int GetBytesSent()	   { return BytesSent; }
	int GetBytesReceived() { return BytesReceived; }

	

};