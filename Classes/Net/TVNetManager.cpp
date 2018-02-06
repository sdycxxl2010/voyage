#include "TVNetManager.h"
#include "Utils.h"

TVNetMsg::TVNetMsg()
{
	isNeedLoading = true;
	m_mParams.clear();
}

TVNetMsg::~TVNetMsg()
{
	m_mParams.clear();
}

void TVNetMsg::AddParam(const char* key, const char* val)
{
	m_mParams.insert(make_pair(key, val));
}

//////////////////////////////////////////////////////////////////////////
TVNetManager::TVNetManager()
{
	Init();
}

TVNetManager::~TVNetManager()
{
}

void TVNetManager::Init()
{
	
}

void TVNetManager::Tick(float dt)
{

}

