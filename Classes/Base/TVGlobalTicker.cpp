#include "TVGlobalTicker.h"

TVGlobalTicker::TVGlobalTicker()
{
	TVGlobalTickerMgr::getInstance()->AddGlobalTicker(this);
}

TVGlobalTicker::~TVGlobalTicker()
{
	TVGlobalTickerMgr::getInstance()->RemoveGlobalTicker(this);
}

//////////////////////////////////////////////////////////////////////////
TVGlobalTickerMgr::TVGlobalTickerMgr()
{
	m_vTickers.clear();

	Init();
}

TVGlobalTickerMgr::~TVGlobalTickerMgr()
{
}

void TVGlobalTickerMgr::Init()
{
	Director::getInstance()->getScheduler()->scheduleUpdate(this, TV_E_UPDATE_PRIORITY::UPDATE_PRIORITY_HIGH, false);
}

void TVGlobalTickerMgr::AddGlobalTicker(TVGlobalTicker* ticker)
{
	m_vTickers.push_back(ticker);
}

void TVGlobalTickerMgr::RemoveGlobalTicker(TVGlobalTicker* ticker)
{
	auto it = m_vTickers.begin();
	while (it != m_vTickers.end() && (*it) == ticker)
	{
		m_vTickers.erase(it);
		break;;
	}
}

void TVGlobalTickerMgr::update(float dt)
{
	if (m_vTickers.size() > 0)
	{
		auto it = m_vTickers.begin();
		while (it != m_vTickers.end())
		{
			(*it)->Tick(dt);
			it++;
		}
	}
}


