#include "TVSocketWorker.h"
#include "ProtocolThread.h"
#include "TVEventManager.h"

TVSocketWorker::TVSocketWorker():
m_pChildThead(NULL),
m_pSocketHolder(NULL),
m_CurSt(TVSocketState::_NONE_SOCKET),
m_NextSt(TVSocketState::_NONE_SOCKET),
m_vSwapRspQueue(NULL)
{
	m_vSwapRspGuiBuf.clear();
	Init();
}

TVSocketWorker::~TVSocketWorker()
{

}

void TVSocketWorker::Init()
{
	//new TVSocketHolder();

	TVEventManager::RegEvent0(TVEventType::_NET_RECONNECT, bind(&TVSocketWorker::OnReconnectSignal, this));
	TVEventManager::RegEvent0(TVEventType::_NET_SELECT_SERVER, bind(&TVSocketWorker::OnSelectServer, this));

	m_vSwapRspQueue = new ThreadQueue<BaseResponseMsg*>();

	m_pSocketHolder = TVSocketHolder::getInstance();
	m_pChildThead = new std::thread(std::bind(&TVSocketWorker::SubThreadProc, this));

	Startup();
}

void TVSocketWorker::OnReconnectSignal()
{
	SetNextState(TVSocketState::_INIT_);
}

void TVSocketWorker::OnSelectServer()
{
	if (m_CurSt == TVSocketState::_NONE_SOCKET)
	{
		SetNextState(TVSocketState::_INIT_);
	}else if (m_CurSt != TVSocketState::_CONNECTED || m_CurSt != TVSocketState::_CONNECTING)
	{
		SetNextState(TVSocketState::_INIT_);
	}
}

int TVSocketWorker::Startup()
{
	SetNextState(TVSocketState::_INIT_);
	return 0;
}

int TVSocketWorker::Quit()
{
	return 0;
}

void TVSocketWorker::SubThreadProc()
{
	assert(m_pSocketHolder != NULL);
	do 
	{
		EnterStates();
		UpdateStates();
		BackendSwapMessages();

		std::this_thread::sleep_for(std::chrono::microseconds(10));
	} while (true);
}

void TVSocketWorker::EnterStates()
{
	if (m_CurSt != m_NextSt)
	{
		m_CurSt = m_NextSt;
		switch (m_CurSt)
		{
		case TVSocketState::_INIT_:
			OnEnterInit();
			break;

		case TVSocketState::_CONNECTING:
			OnEnterConnecting();
			break;

		case TVSocketState::_CONNECTED:
			OnEnterConnected();
			break;

		case TVSocketState::_DISCONNECTING:
			OnEnterDisconnecting();
			break;

		case TVSocketState::_DISCONNECTED:
			OnEnterDisconnected();
			break;

		default:
			break;
		}
	}
}

void TVSocketWorker::UpdateStates()
{
	switch (m_CurSt)
	{
	case TVSocketState::_INIT_:
		OnUpdateInit();
		break;

	case TVSocketState::_CONNECTING:
		OnUpdateConnecting();
		break;

	case TVSocketState::_CONNECTED:
		OnUpdateConnected();
		break;

	case TVSocketState::_DISCONNECTING:
		OnUpdateDisconnecting();
		break;

	case TVSocketState::_DISCONNECTED:
		OnUpdateDisconnected();
		break;

	default:
		break;
	}
}

void TVSocketWorker::BackendSwapMessages()
{
	vector<BaseResponseMsg*>& curQueue = m_pSocketHolder->backendRspQueue();
	if (curQueue.size() > 0)
	{
		for (auto it = curQueue.begin(); it != curQueue.end(); it++)
		{
			m_vSwapRspQueue->push((*it));
		}
		m_pSocketHolder->ResetBackendRspQueue();
	}
}

void TVSocketWorker::Tick(float dt)
{
	FrontendDispatchMessages();
}


void TVSocketWorker::FrontendDispatchMessages()
{
	if (m_vSwapRspQueue->size() > 0)
	{
		m_vSwapRspGuiBuf.clear();
		do
		{
			m_vSwapRspGuiBuf.push_back(m_vSwapRspQueue->pop());
		} while (m_vSwapRspQueue->size() > 0);

		if (m_vSwapRspGuiBuf.size() > 0)
		{
			for (auto it = m_vSwapRspGuiBuf.begin(); it != m_vSwapRspGuiBuf.end(); it++)
			{
				BaseResponseMsg* msg = *it;
				if (msg != NULL)
				{
					ProtocolThread::GetInstance()->dispatchResponseCallbacks(msg);
					delete msg;
				}
			}
			m_vSwapRspGuiBuf.clear();
		}
	}
}

void TVSocketWorker::SetNextState(TVSocketState state)
{
	if (m_CurSt != state)
	{
		m_NextSt = state;
	}
}
 

void TVSocketWorker :: OnEnterInit()
{
}

void TVSocketWorker::OnEnterConnecting()
{
	m_pSocketHolder->Connect();
}

void TVSocketWorker :: OnEnterConnected()
{

}

void TVSocketWorker::OnEnterDisconnecting()
{
	m_pSocketHolder->Disconnect();
}

void TVSocketWorker::OnEnterDisconnected()
{

}

void TVSocketWorker::OnUpdateInit()
{
	if (m_pSocketHolder->couldConnect())
	{
		SetNextState(TVSocketState::_CONNECTING);
	}
}

void TVSocketWorker::OnUpdateConnecting()
{
	if (m_pSocketHolder->isConnected())
	{
		SetNextState(TVSocketState::_CONNECTED);
	}
}

void TVSocketWorker::OnUpdateConnected()
{
	m_pSocketHolder->SelectSocket();
	//ProtocolThread::GetInstance()->broadcastConnectionDisconnected();

	if (!m_pSocketHolder->isConnected() || !m_pSocketHolder->isGoodConnection())
	{
		SetNextState(TVSocketState::_DISCONNECTING);
	}
}

void TVSocketWorker::OnUpdateDisconnecting()
{
	SetNextState(TVSocketState::_DISCONNECTED);
}

void TVSocketWorker::OnUpdateDisconnected()
{

}
