﻿#ifndef _ODSOCKET_H_
#define _ODSOCKET_H_

#ifdef WIN32
	#include <winsock2.h>
	typedef int				socklen_t;
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <netdb.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	typedef int				SOCKET;

	//#pragma region define win32 const variable in linux
	#define INVALID_SOCKET	-1
	#define SOCKET_ERROR	-1
	//#pragma endregion
#endif


class ODSocket {

public:
	ODSocket(SOCKET sock = INVALID_SOCKET);
	~ODSocket();

	// Create socket object for snd/recv data
	bool Create(int af, int type, int protocol = 0);

	// Connect socket
	bool Connect(const char* ip, unsigned short port);

	//use server name
	bool ConnectWithName(const char* serverName, unsigned short port);
	//#region server
	// Bind socket
	bool Bind(unsigned short port);

	// Listen socket
	bool Listen(int backlog = 5);

	// Accept socket
	bool Accept(ODSocket& s, char* fromip = NULL);
	//#endregion
	int Select();
	// Send socket
	int Send(const char* buf, int len, int flags = 0);

	// Recv socket
	int Recv(char* buf, int len, int flags = 0);

	// Close socket
	int Close(const char*fileName,int lineNum);


	int GetLastError()
	{
		if (m_rc != 0)
		return m_rc;
	}

	bool isGoodConnection();
	// Get errno

	//#pragma region just for win32
	// Init winsock DLL
	static int Init();
	// Clean winsock DLL
	static int Clean();
	//#pragma endregion

	// Domain parse
	static bool DnsParse(const char* domain, char* ip);

	ODSocket& operator = (SOCKET s);

	operator SOCKET ();

protected:
	int GetError();

	void SetError();

protected:
	SOCKET m_sock;
	fd_set  fdR;
	int m_rc;
};

#endif
