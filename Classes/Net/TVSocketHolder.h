#ifndef _H_TV_SOCKET_HOLDER_
#define _H_TV_SOCKET_HOLDER_

#include "ODSocket.h"
#include "thread"
#include <string>
#include "ThreadQueue.h"
#include "TVBasic.h"
#include "BaseNetMsg.h"

class TVSocketHolder : public SingletonPtr2<TVSocketHolder>
{
	friend class SingletonPtr<TVSocketHolder>;
public:	
	TVSocketHolder();
	~TVSocketHolder(void);

public:
	int    start();  

	int    Connect();

	int    Disconnect();

	int    SelectSocket();

	bool   isConnected();

	bool   isGoodConnection();

	bool   isConnecting();

	bool   couldConnect()
	{
		return customIp.length() > 0;
	}

	int    StartChatThread();

	


public:
	ODSocket getSocket();
	ODSocket& getChatSocket();
	//int state;
	bool m_connected;
	bool m_chatServerConnected;
	bool m_isConnecting;
	
	void setConnectionStatus(bool isConnect);
	void setCustomIp(const char*ip,int port, const char*chatUrl,int chatPort)
	{
		customIp = ip;m_port = port;m_chatPort = chatPort;customChatIp = chatUrl;
	}

public:
	std::vector<BaseResponseMsg*>& backendRspQueue()
	{
		return m_vBackendRspQueue;
	}

	void AppendBackendRspMsg(BaseResponseMsg* msg)
	{
		m_vBackendRspQueue.push_back(msg);
	}

	void ResetBackendRspQueue()
	{
		m_vBackendRspQueue.clear();
	}

public:
	std::vector<BaseRequestMsg*>& frontendReqQueue()
	{
		return m_vFrontednReqQueue;
	}

	void AppendFrontendReqMsg(BaseRequestMsg* req)
	{
		m_vFrontednReqQueue.push_back(req);
	}

	void ResetFrontendReqQueue()
	{
		m_vFrontednReqQueue.clear();
	}
private:
	ODSocket  csocket;
	ODSocket* chatSocket;



	std::string customIp;
	int m_port;
	std::string customChatIp;
	int m_chatPort;

	std::thread* chatThreadInstance;
	void* chatThread();
	
	std::vector<BaseResponseMsg*> m_vBackendRspQueue;

	std::vector<BaseRequestMsg*> m_vFrontednReqQueue;

};

#endif
