#include "BaseNetMsg.h"
#include <stdlib.h>
#include <memory.h>

BaseNetMsg::BaseNetMsg():
m_pReqPayload(NULL),
m_pRspPayload(NULL)
{

}

BaseNetMsg::~BaseNetMsg()
{

}


BaseResponseMsg::BaseResponseMsg(void)
{
	message=0;
}
BaseResponseMsg::~BaseResponseMsg(void)
{

}


BaseRequestMsg::BaseRequestMsg():
m_pBuf(NULL)
{

}

BaseRequestMsg::~BaseRequestMsg()
{
	if (m_pBuf != NULL)
	{
		free(m_pBuf);
	}
}

void BaseRequestMsg::SetContent(char* buf, size_t len)
{
	m_pBuf = (char*)malloc(len);
	m_szBufLen = len;

	memset(m_pBuf, 0, len);
	memcpy(m_pBuf, buf, len);
}

char* BaseRequestMsg::GetContent(size_t* len)
{
	*len = m_szBufLen;
	return m_pBuf;
}