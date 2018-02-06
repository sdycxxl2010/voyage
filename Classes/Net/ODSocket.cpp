#include <stdio.h>
#include "ODSocket.h"
#include <errno.h>
#include "cocos2d.h"

#ifdef WIN32
#pragma comment(lib, "wsock32")
#endif

ODSocket::ODSocket(SOCKET sock) {
	m_sock = sock;
	m_rc = 0;
}

ODSocket::~ODSocket() {
}

int ODSocket::Init() {
#ifdef WIN32
	/*
	 http://msdn.microsoft.com/zh-cn/vstudio/ms741563(en-us,VS.85).aspx

	 typedef struct WSAData {
	 WORD wVersion;								//winsock version
	 WORD wHighVersion;							//The highest version of the Windows Sockets specification that the Ws2_32.dll can support
	 char szDescription[WSADESCRIPTION_LEN+1];
	 char szSystemStatus[WSASYSSTATUS_LEN+1];
	 unsigned short iMaxSockets;
	 unsigned short iMaxUdpDg;
	 char FAR * lpVendorInfo;
	 }WSADATA, *LPWSADATA;
	 */
	WSADATA wsaData;
	//#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8))
	WORD version = MAKEWORD(2, 0);
	int ret = WSAStartup(version, &wsaData); //win sock start up
	if (ret) {
//		cerr << "Initilize winsock error !" << endl;
		return -1;
	}
#endif

	return 0;
}
//this is just for windows
int ODSocket::Clean() {
#ifdef WIN32
	return (WSACleanup());
#endif
	return 0;
}

ODSocket& ODSocket::operator =(SOCKET s) {
	m_sock = s;
	return (*this);
}

ODSocket::operator SOCKET() {
	return m_sock;
}
//create a socket object win/lin is the same
// af:
bool ODSocket::Create(int af, int type, int protocol) {
	m_sock = socket(af, type, protocol);
	cocos2d::log("ODSocket::Create fd:%d",m_sock);
	if (m_sock == INVALID_SOCKET) {
		return false;
	}
	return true;
}

int connect_with_timeout(int soc,struct sockaddr *addr,int to) {
  int res;
 // long arg;
  fd_set myset;
  struct timeval tv;
  int valopt = 0;
  socklen_t len;

#if WIN32
	unsigned long flag = 1;
	ioctlsocket (soc, FIONBIO, (unsigned long *) &flag);
#else
	int flags = fcntl(soc, F_GETFL, 0);
	fcntl(soc, F_SETFL, flags|O_NONBLOCK);
#endif
  // Trying to connect with timeout
  res = connect(soc, addr, sizeof(struct sockaddr));
  if (res < 0) {
     if (errno == EINPROGRESS) {
        fprintf(stderr, "EINPROGRESS in connect() - selecting\n");
        do {
           tv.tv_sec = to;
           tv.tv_usec = 0;
           FD_ZERO(&myset);
           FD_SET(soc, &myset);
           res = select(soc+1, NULL, &myset, NULL, &tv);
           if (res < 0 && errno != EINTR) {
              fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
              return -1;
           }
           else if (res > 0) {
              // Socket selected for write
              len = sizeof(int);
#ifndef WIN32
              if (getsockopt(soc, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &len) < 0) {
                 fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno));
                 return -1;
              }
#endif
              // Check the value returned...
              if (valopt) {
                 fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt) );
                 return -1;
              }
              break;
           }
           else {
              fprintf(stderr, "Timeout in select() - Cancelling!\n");
              return -1;
           }
        } while (1);
     }
     else {
        fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));
        return -1;
     }
  }
  return soc;
}

//use server name
bool ODSocket::ConnectWithName(const char* serverName, unsigned short port)
{
	struct sockaddr_in svraddr;
	struct hostent *host=gethostbyname(serverName) ;
	if(!host)
		return false;
	//serverAddr = host->h_addr_list[0];
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);
	svraddr.sin_port = htons(port);
	int ret = connect(m_sock, (struct sockaddr*) &svraddr, sizeof(svraddr));
	SetError();
	if (ret == SOCKET_ERROR) {
		cocos2d::log("ConnectWithName errno:%d,addr:%u,m_sock:%d",errno,svraddr.sin_addr.s_addr,m_sock);
		return false;
	}
	int one = 1;
	setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(one));

#if WIN32
	unsigned long flag = 1;
	ioctlsocket (m_sock, FIONBIO, (unsigned long *) &flag);
#else
	int flags = fcntl(m_sock, F_GETFL, 0);
	fcntl(m_sock, F_SETFL, flags|O_NONBLOCK);
#endif
	return true;
}

bool ODSocket::Connect(const char* ip, unsigned short port) {
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = inet_addr(ip);
	svraddr.sin_port = htons(port);
	int ret = connect(m_sock, (struct sockaddr*) &svraddr, sizeof(svraddr));
	SetError();
	if (ret == SOCKET_ERROR) {
		return false;
	}
	int one = 1;
	setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(one));

#if WIN32
	unsigned long flag = 1;
	ioctlsocket (m_sock, FIONBIO, (unsigned long *) &flag);
#else
	int flags = fcntl(m_sock, F_GETFL, 0);
	fcntl(m_sock, F_SETFL, flags|O_NONBLOCK);
#endif
	return true;
}

bool ODSocket::Bind(unsigned short port) {
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = INADDR_ANY;
	svraddr.sin_port = htons(port);

	int opt = 1;
	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt))
			< 0)
		return false;

	int ret = bind(m_sock, (struct sockaddr*) &svraddr, sizeof(svraddr));
	SetError();
	if (ret == SOCKET_ERROR) {
		return false;
	}
	return true;
}
//for server
bool ODSocket::Listen(int backlog) {
	int ret = listen(m_sock, backlog);
	SetError();
	if (ret == SOCKET_ERROR) {
		return false;
	}
	return true;
}

bool ODSocket::Accept(ODSocket& s, char* fromip) {
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	SOCKET sock = accept(m_sock, (struct sockaddr*) &cliaddr, &addrlen);
	SetError();
	if (sock == SOCKET_ERROR) {
		return false;
	}

	s = sock;
	if (fromip != NULL)
		sprintf(fromip, "%s", inet_ntoa(cliaddr.sin_addr));

	return true;
}

int ODSocket::Select(){
	FD_ZERO(&fdR);
	FD_SET(m_sock, &fdR);
	struct timeval mytimeout;
	mytimeout.tv_sec=1;
	mytimeout.tv_usec=0;
	int result= select(m_sock+1,&fdR,NULL,NULL,&mytimeout);
	//fd r_sockets,w_sockets,exceptions,timeout

	//case -1:                            error handled by u;
	SetError();

	if(result == -1){
		SetError();
	}else if(result==0){
		//time out
	}else {
		if(FD_ISSET(m_sock,&fdR)){
			return 1;
		}else {
			return 2;
		}
	}
	return result;
}



int ODSocket::Send(const char* buf, int len, int flags) {
	//int bytes;
	int count = send(m_sock, buf, len, flags);

// 	while (count < len) {
// 		const char* a= buf + count;
// 		bytes = send(m_sock, buf + count, len - count, flags);
// 		if (bytes == -1 || bytes == 0)
// 			return -1;
// 		count += bytes;
// 	}

	return count;
}

int ODSocket::Recv(char* buf, int len, int flags) {
	return (recv(m_sock, buf, len, flags));
}

int ODSocket::Close(const char*fileName,int lineNum) {
	if(fileName){
		cocos2d::log("ODSocket::Close fd:%d file:%s -- %d",m_sock,fileName,lineNum);
	}
	int res = 0;
#ifdef WIN32
	res = (closesocket(m_sock));
#else
	res = (close(m_sock));
#endif
	m_sock = -1;
	return res;
}

bool ODSocket::isGoodConnection()
{
	int err = 0;
	socklen_t len = sizeof (err);
	int retval = getsockopt(m_sock, SOL_SOCKET, SO_ERROR, (char*)&err, &len);

	if (retval != 0 || err != 0) 
	{
		SetError();
		return false;
	}
	return true;
}

void ODSocket::SetError()
{
	m_rc = 0;
	m_rc = GetError();
}

int ODSocket::GetError() 
{
#ifdef WIN32
	return (WSAGetLastError());
#else
	return errno;
#endif
}

bool ODSocket::DnsParse(const char* domain, char* ip) {
	struct hostent* p;
	if ((p = gethostbyname(domain)) == NULL)
		return false;

	sprintf(ip, "%u.%u.%u.%u", (unsigned char) p->h_addr_list[0][0],
			(unsigned char) p->h_addr_list[0][1],
			(unsigned char) p->h_addr_list[0][2],
			(unsigned char) p->h_addr_list[0][3]);

	return true;
}
