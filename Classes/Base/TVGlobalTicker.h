#ifndef _H_TV_GLOBAL_TICKER_
#define _H_TV_GLOBAL_TICKER_
#include "TVBasic.h"

class TVGlobalTicker
{
public:
	TVGlobalTicker();
	virtual ~TVGlobalTicker();

public:
	virtual void Tick(float dt) = 0;
private:

};

class TVGlobalTickerMgr : public SingletonPtr<TVGlobalTickerMgr>
{
	friend class SingletonPtr<TVGlobalTickerMgr>;
public:
	TVGlobalTickerMgr();
	~TVGlobalTickerMgr();

public:
	void Init();
	void AddGlobalTicker(TVGlobalTicker* ticker);
	void RemoveGlobalTicker(TVGlobalTicker* ticker);

	void update(float dt);
private:
	vector<TVGlobalTicker*> m_vTickers;
};



#endif