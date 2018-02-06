/*
*Header File:TVSocketWorker
*@author emps
*
*/
#include "TVTemplates.h"
#include "TVBasic.h"
#include "TVGlobalTicker.h"
#include "TVSocketHolder.h"
#include "ThreadQueue.h"

#include <thread>

#ifndef _H_TVSOCKET_WORKER_
#define _H_TVSOCKET_WORKER_

typedef enum E_SOCKET_STATE
{
	_NONE_SOCKET,
	_INIT_,
	_CONNECTING,
	_CONNECTED,
	_DISCONNECTING,
	_DISCONNECTED,

}TVSocketState;

class TVSocketWorker : public SingletonPtr<TVSocketWorker>, public TVGlobalTicker
{

public:
	TVSocketWorker();
	~TVSocketWorker();

public:
	virtual void Tick(float dt) override;

public:
	int Startup();

	int Quit();


private:
	void Init();

	void OnReconnectSignal();

	void OnSelectServer();

	void SubThreadProc();

	void EnterStates();

	void UpdateStates();

	void BackendSwapMessages();

	void FrontendDispatchMessages();

	void OnEnterInit();

	void OnEnterConnecting();

	void OnEnterConnected();

	void OnEnterDisconnecting();

	void OnEnterDisconnected();



	void OnUpdateInit();

	void OnUpdateConnecting();

	void OnUpdateConnected();

	void OnUpdateDisconnecting();

	void OnUpdateDisconnected();

	void SetNextState(TVSocketState state);

private:

	std::thread*      m_pChildThead;
	TVSocketHolder*   m_pSocketHolder;

	TVSocketState     m_CurSt;
	TVSocketState     m_NextSt;
	bool		      m_bIsStateDirty;

	ThreadQueue<BaseResponseMsg*> *m_vSwapRspQueue;

	vector<BaseResponseMsg*> m_vSwapRspGuiBuf;

};

#endif
