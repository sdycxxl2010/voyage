#include "TVEventManager.h"

TVEventer::TVEventer()
{
	m_eType = TVEventType::_NONE;
	m_pTarget = NULL;
	m_pCallback0 = []{};
	m_pCallback1 = [](const string&){};
}

TVEventer::~TVEventer()
{

}

void TVEventer::SetEventTarget0(TVEventType eType, Ref *target, const TVEventCallback0& callback)
{
	m_eType = eType;
	m_pCallback0 = callback;
	m_pTarget = target;
}

void TVEventer::SetEventTarget1(TVEventType eType, Ref *target, const TVEventCallback1& callback)
{
	m_eType = eType;
	m_pCallback1 = callback;
	m_pTarget = target;
}

void TVEventer::DoEvent()
{
	if (m_pCallback0 != NULL)
	{
		m_pCallback0();
	}

}

void TVEventer::DoEvent(const string& param)
{
	if (m_pCallback1 != NULL)
	{
		m_pCallback1(param);
	}
}

//////////////////////////////////////////////////////////////////////////
TVEventManager::TVEventManager()
{

}

TVEventManager::~TVEventManager()
{

}

void TVEventManager::PushEvent(TVEventType eType)
{
	TVEventManager::getInstance()->PushEventImpl(eType);
}

void TVEventManager::PushEvent(TVEventType eType, string& params)
{
	TVEventManager::getInstance()->PushEventImpl(eType, params);
}

void TVEventManager::RegEvent0(TVEventType eType, const TVEventCallback0& callback)
{
	TVEventManager::getInstance()->RegEventImpl(eType, callback);
}

void TVEventManager::RegEvent1(TVEventType eType, const TVEventCallback1& callback)
{
	TVEventManager::getInstance()->RegEventImpl(eType, callback);
}

void TVEventManager::UnRegEvent(TVEventType eType)
{
	TVEventManager::getInstance()->UnRegEventImpl(eType);
}


void TVEventManager::PushEventImpl(TVEventType eType)
{
	for (auto it = m_vEventList.begin(); it != m_vEventList.end(); it ++)
	{
		if ((*it)->eventType() == eType)
		{
			(*it)->DoEvent();
		}
	}
}

void TVEventManager::PushEventImpl(TVEventType eType, string& params)
{
	for (auto it = m_vEventList.begin(); it != m_vEventList.end(); it++)
	{
		if ((*it)->eventType() == eType)
		{
			(*it)->DoEvent(params);
		}
	}
}

void TVEventManager::RegEventImpl(TVEventType eType, const TVEventCallback0& callback)
{
	TVEventer *evt = new TVEventer();
	evt->SetEventTarget0(eType, NULL, callback);
	m_vEventList.push_back(evt);
}

void TVEventManager::RegEventImpl(TVEventType eType, const TVEventCallback1& callback)
{
	TVEventer *evt = new TVEventer();
	evt->SetEventTarget1(eType, NULL, callback);
	m_vEventList.push_back(evt);
	
}

void TVEventManager::UnRegEventImpl(TVEventType eType)
{
	for (auto it = m_vEventList.begin(); it != m_vEventList.end(); )
	{
		if ((*it)->eventType() == eType)
		{
			it = m_vEventList.erase(it);
		}
		else
		{
			it++;
		}
	}
}