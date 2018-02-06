#include "login.pb-c.h"
//#include "BaseRequestMsg.h"

#ifndef _H_BASE_NET_MSG
#define _H_BASE_NET_MSG
class BaseRequestMsg;
class BaseResponseMsg;
class BaseNetMsg
{
public:
	BaseNetMsg();
	virtual ~BaseNetMsg();
public:
	void SetReqPayload(BaseRequestMsg *req)
	{
		m_pReqPayload = req;
	}

	void SetRspPayLoad(BaseResponseMsg *rsp)
	{
		m_pRspPayload = rsp;
	}

	BaseRequestMsg* GetReqPayload()
	{
		return m_pReqPayload;
	}

	BaseResponseMsg* GetRspPayload()
	{
		return m_pRspPayload;
	}

private:
	BaseRequestMsg  *m_pReqPayload;
	BaseResponseMsg *m_pRspPayload;
};

class BaseResponseMsg
{

public:
	BaseResponseMsg(void);
	~BaseResponseMsg(void);
	struct ProtobufCMessage* message;
	int msgType;
};

class BaseRequestMsg
{
public:
	BaseRequestMsg();
	~BaseRequestMsg();

public:
	void   SetContent(char* buf, size_t len);

	char*  GetContent(size_t* len);
private:
	char* m_pBuf;
	size_t m_szBufLen;
};


#endif
