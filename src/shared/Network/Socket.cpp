#include "socket.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>


#include "..\defs\defs.h"

GDSocket::operator SOCKET () {
   return Socket;
}

SOCKET GDSocket::operator=(const GDSocket &arg)
{
	return this->GetSocket();
}





bool GDSocket::Connect(char *RemoteAddress, int port)
{


	//figure out the ipaddress from the host name
	char ip_address[48];
	HOSTENT* hostent = gethostbyname(RemoteAddress);
	
	//resolve the hostname into a IP address if it found it
	if(hostent)
	{
		BYTE i[4];
		i[0] = (UINT)hostent->h_addr_list[0][0];
		i[1] = (UINT)hostent->h_addr_list[0][1];
		i[2] = (UINT)hostent->h_addr_list[0][2];
		i[3] = (UINT)hostent->h_addr_list[0][3];

		
		sprintf(ip_address, "%d.%d.%d.%d", (int)i[0], (int)i[1], (int)i[2], (int)i[3]);
	}
	else
		strcpy(ip_address, RemoteAddress); //otherwise use the address provided by the user


	//get the socket
	Socket = socket (AF_INET, SOCK_STREAM, 0);

	
	//connect to the server
	sockaddr_in target;



	target.sin_family = AF_INET;
	target.sin_port   = htons (port);
	target.sin_addr.s_addr = inet_addr(ip_address); //connects to local machine for now
	
	
	if(connect(Socket, (sockaddr*)&target, sizeof(target)) == SOCKET_ERROR)
	{
		Connected = false;
		return false;
		
	}

	//set to socket to non-blocking
	unsigned long nonblocking=1;
	ioctlsocket(Socket, FIONBIO, &nonblocking);

	Connected = true;
	return true;
}

void GDSocket::Disconnect()
{
	if(!Connected) return;

	Connected = false;

	shutdown(Socket, 0x01);
	closesocket (Socket);
	Socket = 0;
}



//////// Send /////////////////////////////////
//
// Description:
//
// Sends specified data. Returns false if no
// data was sent. Note that this code isn't that
// great, since it'll hold up the system until
// it's done sending.
//
///////////////////////////////////////////////
 

bool GDSocket::Send (char* data, int size, int flags)
{
	if(!Connected) return false;

	//timeout variable
	int timeout = timeGetTime();
	bool Done=false;
	int  Bytes=0;

	char* senddata = data;
	

	//try to send
	while(!Done)
	{

		//send out data
		int RVal = send(Socket, (char*)senddata, size-Bytes, 0);

		//increment the bytes sent count if there isn't any error
		if(RVal != WSAEWOULDBLOCK)
		{
			Bytes+=RVal;
			senddata += RVal;
		}

		
		//check to see if all the data was sent
		if(Bytes == size)
			Done = true;
		
		//check to see if we've reached the timeout. Note that we don't return
		//if some data has been sent, which would cause problems. (partial message send)
		if(timeout > (int)timeGetTime() + Timeout && Bytes == 0)
			return false;
	}

	
	

	//sent OK on first try!
	return true;
}



//////// StripEscapeCodes /////////////////////
//
// Description:
//
// Removes all the gibberish that is sent
// with telnet messages, so that you just have
// the plain ASCII stuff.
//
///////////////////////////////////////////////

void GDSocket::StripEscapeCodes(char* string, char* output, bool AddColors)
{
	bool SkipEscapeCode=false;
	int skipstart=0;
	int mIndex=0; 
	

	char digit1[2] = {0, 0};
	char digit2[2] = {0, 0};
	
	for(int i=0; i<(int)strlen(string); i++)
	{
		//Test to see if the first character in the escape sequence is here
		if(string[i] == 27)
		{
			SkipEscapeCode = true;
			skipstart=i;
		}


		//Only record if it's not an escape code
		if(SkipEscapeCode == false)
		{
			
			output[mIndex] = string[i];

			if( output[mIndex] == '\n' )
			{
				output[mIndex] = ' ';
				//string[i+1] = 'n';
			}		

			if( output[mIndex] == '\r' )
			{
				output[mIndex] = ' ';
				//string[i+1] = 'r';
			}		
			
			mIndex++;
		}
		

		//Last character in escape sequence
		if( string[i] == 109 && SkipEscapeCode == true && !AddColors)
		{
			SkipEscapeCode = false;
		}

		if(string[i] == 109 && SkipEscapeCode == true && AddColors)
		{

			digit1[0] = string[skipstart+2];
			digit2[0] = string[skipstart+3];

			int idigit1 = atoi(digit1);
			int idigit2 = atoi(digit2);
			
			if( idigit1 == 3 && idigit2 == 1 )
			{
				output[mIndex+0] = '`'; 
				output[mIndex+1] = '6'; 
				output[mIndex+2] = '0'; 
				output[mIndex+3] = '0'; 
				output[mIndex+4] = '`'; 
			}
			else if( idigit1 == 3 && idigit2 == 2 )
			{
				output[mIndex+0] = '`'; 
				output[mIndex+1] = '0'; 
				output[mIndex+2] = '6'; 
				output[mIndex+3] = '0'; 
				output[mIndex+4] = '`'; 
			}

			else if( idigit1 == 3 && idigit2 == 3 )
			{
				output[mIndex+0] = '`'; 
				output[mIndex+1] = '6'; 
				output[mIndex+2] = '6'; 
				output[mIndex+3] = '0'; 
				output[mIndex+4] = '`'; 
			}

			else if( idigit1 == 3 && idigit2 == 4 )
			{
				output[mIndex+0] = '`'; 
				output[mIndex+1] = '0'; 
				output[mIndex+2] = '0'; 
				output[mIndex+3] = '6'; 
				output[mIndex+4] = '`'; 
			}

			else if( idigit1 == 3 && idigit2 == 5 )
			{
				output[mIndex+0] = '`'; 
				output[mIndex+1] = '6'; 
				output[mIndex+2] = '0'; 
				output[mIndex+3] = '6'; 
				output[mIndex+4] = '`'; 
			}

			else if( idigit1 == 3 && idigit2 == 6 )
			{
				output[mIndex+0] = '`'; 
				output[mIndex+1] = '0'; 
				output[mIndex+2] = '5'; 
				output[mIndex+3] = '6'; 
				output[mIndex+4] = '`'; 
			}

			else if( idigit2 != 1 )
			{
				output[mIndex+0] = '`'; 
				output[mIndex+1] = '7'; 
				output[mIndex+2] = '7'; 
				output[mIndex+3] = '7'; 
				output[mIndex+4] = '`'; 
			}

			


			mIndex += 5;

			
			
			SkipEscapeCode = false;
		}
		
	}

}



//////// Receive //////////////////////////////
//
// Description:
//
// Reads an incoming message and returns it
// to the caller. If the function failes, or
// there are no incoming messages, it returns
// 0. 
//
//
///////////////////////////////////////////////



int GDSocket::Receive(char* Buffer)
{
	//timeout variable
	int timeout = timeGetTime();
	ZeroMemory(MessageBuffer, 4096);
	MessageSize = 0;


	//still connected?
	if(!Connected) return false;

	//try to receive
	bool  Done = false;
	char* pBuffer = Buffer;
	int Size=0;

	while(!Done)
	{
		int RVal = recv(Socket, pBuffer, 1, 0);

		//if there's nothing left to read, return
		if(RVal == SOCKET_ERROR || RVal == 0)		
			Done = true;
		else
			Size ++;

		if(*pBuffer == 0 || *pBuffer == '\n')
		{
			*pBuffer=0;
			Done = true;
		}

		pBuffer++;
			
	}


	//Store this message in our buffer in case the app needs it again.
	if(Size)
	{
		
		memcpy( MessageBuffer, Buffer, Size );
		
		MessageSize = Size;
	}
	
	return Size; //return the message size
}


void GDSocket::GenWordList()
{
	ZeroMemory(CleanBuffer, 4096);
	StripEscapeCodes( MessageBuffer, CleanBuffer, false );
	
	GetWords (CleanBuffer, CurrentStringArray);
}