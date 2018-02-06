#include "TVSceneLogin.h"
#include "UIStart.h"
#include "UIManager.h"

TVSceneLogin::TVSceneLogin()
{
}

TVSceneLogin:: ~TVSceneLogin()
{

}


void TVSceneLogin::OnEnterScene()
{
	TVScene::OnEnterScene();

	//rootNode()->addChild(UIStart::create());
	UIManager::PushUI(UI_WINDOWS::_UI_SPLASH);
}

void TVSceneLogin::OnTickScene()
{
	TVScene::OnTickScene();

}

void TVSceneLogin::OnExitScene()
{
	TVScene::OnExitScene();
}

