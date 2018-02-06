/*
*Header File:
*@author emps
*/


#ifndef _H_TVNET_MGR_
#define _H_TVNET_MGR_
#include "TVBasic.h"
#include "TVGlobalTicker.h"


class TVNetMsg : public Ref
{
public:
	TVNetMsg();
	virtual ~TVNetMsg();

public:
	static TVNetMsg* createWithCallback(const network::ccHttpRequestCallback& callback);

public:
	void SetRequestId(string requestId)
	{
		
	}

	void AddParam(const char* key, const char* val);

	map<string, string>& params()
	{
		return m_mParams;
	}

public:
	bool  isNeedLoading;

private:
	map<string, string> m_mParams;
};


class TVNetManager : public SingletonPtr<TVNetManager>, public TVGlobalTicker
{
	friend class SingletonPtr<TVNetManager>;
public:
	TVNetManager();
	~TVNetManager();

public:
	void SendMsg(TVNetMsg* msg);

public:
	virtual void Tick(float dt) override;

private:
	void Init();

private:
	string m_sStatusBaseUrl;

};



#endif