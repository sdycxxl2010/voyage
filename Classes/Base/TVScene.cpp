#include "TVScene.h"
#include "TVSceneManager.h"

TVScene::TVScene()
{
	m_pRootScene = NULL;
}

TVScene::~TVScene()
{

}


void TVScene::OnEnterScene()
{
	assert(m_pRootScene == NULL);
	m_pRootScene = Scene::create();
	if (Director::getInstance()->getRunningScene() == NULL)
	{
		Director::getInstance()->runWithScene(m_pRootScene);
	}
	else
	{
		Director::getInstance()->replaceScene(m_pRootScene);
	}
}

void TVScene::OnTickScene()
{

}

void TVScene::OnExitScene()
{
	m_pRootScene = NULL;
}

Scene* TVScene::rootNode()
{
	return m_pRootScene;
}

Node* TVScene::curSceneRootNode()
{
	TVScene* curScene =  TVSceneManager::getInstance()->curGameScene();
	if (curScene != NULL)
	{
		return curScene->rootNode();
	}
	return NULL;
}



