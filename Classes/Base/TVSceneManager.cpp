#include "TVSceneManager.h"
#include "TVSceneBattle.h"
#include "TVSceneLogin.h"
#include "TVScenePortal.h"
#include "TVSceneSolo.h"
#include "TVSceneSailing.h"


TVSceneManager::TVSceneManager()
{
	m_pCurScene = 0;
	m_pNextScene = 0;
	m_pPreScene = 0;
	m_mSceneMap.clear();

	Init();
}

TVSceneManager::~TVSceneManager()
{
	auto it = m_mSceneMap.begin();
	while (it != m_mSceneMap.end())
	{
		delete it->second;
		it++;
	}
	m_mSceneMap.clear();
}

void TVSceneManager::Init()
{
	m_mSceneMap[TVSceneType::_SceneBattle] = new TVSceneBattle();
	m_mSceneMap[TVSceneType::_SceneLogin] = new TVSceneLogin();
	m_mSceneMap[TVSceneType::_ScenePortal] = new TVScenePortal();
	m_mSceneMap[TVSceneType::_SceneSailing] = new TVSceneSailing();
	m_mSceneMap[TVSceneType::_SceneSolo] = new TVSceneSolo();

	m_pNextScene = m_mSceneMap[TVSceneType::_SceneLogin];
	Director::getInstance()->getScheduler()->scheduleUpdate(this, TV_E_UPDATE_PRIORITY::UPDATE_PRIORITY_HIGH, false);
}

void TVSceneManager::Tick(float dt)
{
	TickObjects(dt);
	TickScenes(dt);
}


void TVSceneManager::Destroy()
{

}


void TVSceneManager::update(float dt)
{
	this->Tick(dt);
}

void TVSceneManager::TickScenes(float dt)
{
	if (m_pCurScene != m_pNextScene)
	{
		if (m_pCurScene != NULL)
		{
			m_pCurScene->OnExitScene();
		}
		m_pPreScene = m_pCurScene;
		m_pCurScene = m_pNextScene;
		m_pCurScene->OnEnterScene();
		return;//one frame one change.
	}
	m_pCurScene->OnTickScene();
}

void TVSceneManager::TickObjects(float dt)
{
	auto it = m_vGameObjects.begin();
	while (it != m_vGameObjects.end())
	{
		(*it)->Tick(dt);
		it++;
	}
}


void TVSceneManager::setNextScene(TVSceneType sType)
{
	if (m_pNextScene->curSceneType() != sType)
	{
		m_pNextScene = m_mSceneMap[sType];
	}
}
