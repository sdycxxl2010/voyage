#include "UIPause.h"
#include "UISetting.h"
#include "UISailManage.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UIStart.h"
#include "TVSceneLoader.h"
#include "TVBattleManager.h"
#include "UISocial.h"

#if ANDROID
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif
UIPause*UIPause::m_gamePauseLayer = nullptr;
static bool BackMenuShowed = false;
UIPause::UIPause()
{
	m_seaLayer = nullptr;
	m_battleLayer = nullptr;
	m_bSeaPause = false;
	m_viewCamera = nullptr;
}

UIPause::~UIPause()
{

}

void UIPause::onEnter()
{
   	UIBasicLayer::onEnter();
}

void UIPause::onExit()
{
	UIBasicLayer::onExit();
}

void UIPause::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
 	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01); 
	auto layer = _director->getRunningScene()->getChildByName("characterlayer");
	if (isButton(button_1))//恢复游戏
	{
		BackMenuShowed = false;
		if (!m_bSeaPause)
		{
			if (m_seaLayer)
			{
				m_seaLayer->seaEventEnd();
			}
			if (m_battleLayer)
			{
				m_battleLayer->startBattle(0);
			}
		}
		closeView();
		SINGLE_AUDIO->vresumeBGMusic();
		return;
	}
	if (isButton(button_2))//设置
	{
		if (SINGLE_HERO->m_heroIsOnsea || layer != nullptr)
		{
			UISetting::getInstance()->openSettingLayer(_cameraMask);
		}else
		{
			UISetting::getInstance()->openSettingLayer(_cameraMask);
		}
		return;
	}
	if (isButton(button_3))//退出游戏
	{
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		Director::getInstance()->end();
		return;
	}

	if (isButton(button_4))//登出
	{
		UIInform::getInstance()->openInformView(this);
		if (layer != nullptr)
		{
			UIInform::getInstance()->setCameraMask(4);
		}
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SETTING_LOGOUT_TITLE", "TIP_SETTING_ACCOUNT_QUIT_GAME");
		return;
	}

	if (isButton(button_confirm_yes))
	{
#if ANDROID
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo, "com/piigames/voyage/AppActivity", "SignOut", "()V");
		cocos2d::log("SignOut method id:%x,%x", jniInfo.methodID, jniInfo.classID);
		if (id)
		{
			jniInfo.env->CallStaticVoidMethod(jniInfo.classID, jniInfo.methodID);
		}
#endif
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		ProtocolThread::GetInstance()->logout();
		BackMenuShowed = false;
		ProtocolThread::GetInstance()->saveEmail("");
		ProtocolThread::GetInstance()->savePassword("");
		UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(HAVE_ACCPET_EULA).c_str(), true);
		UserDefault::getInstance()->flush();
		SINGLE_CHAT->reset();
		UISocial::getInstance()->reset();
		Director::getInstance()->replaceScene(UIStart::createScene());
		return;
	}
}
void UIPause::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	
}
void UIPause::openGamePauseLayer()
{
	if(BackMenuShowed){
		return;
	}
	BackMenuShowed = true;
	auto currentScene = Director::getInstance()->getRunningScene();
	this->removeFromParentAndCleanup(true);
	currentScene->addChild(this, 10001);

	openView(COMMOM_COCOS_RES[C_VIEW_PAUSE_GAME]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PAUSE_GAME]);
	auto b_setting = Helper::seekWidgetByName(view,"button_2");
	auto b_logout = Helper::seekWidgetByName(view, "button_4");
	if (ProtocolThread::GetInstance()->getLastLoginResult())
	{
		b_setting->setOpacity(255);
		b_setting->setTouchEnabled(true);
		b_logout->setOpacity(255);
		b_logout->setTouchEnabled(true);
	}
	else
	{
		b_setting->setOpacity(125);
		b_setting->setTouchEnabled(false);
		b_logout->setOpacity(125);
		b_logout->setTouchEnabled(false);
	}
	//3d角色模型layer是否存在
// 	auto layer = dynamic_cast<Layer*>(_director->getRunningScene()->getChildByName("characterlayer"));
// 	auto shiplayer = dynamic_cast<Layer*>(_director->getRunningScene()->getChildByName<Layer*>("shipLayer"));
// 	if (SINGLE_HERO->m_heroIsOnsea || layer != nullptr || shiplayer != nullptr)
// 	{
// 		this->setCameraMask(4, true);
// 	}
// 	else
// 	{
// 		this->setCameraMask(1, true);
// 	}
// 
// 	if (layer != nullptr && m_viewCamera == nullptr)
// 	{
// 		auto size = Director::getInstance()->getWinSize();
// 		Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
// 		popCam->setCameraFlag(CameraFlag::USER2);
// 		popCam->setDepth(4);
// 		m_viewCamera = popCam;
// 		this->addChild(popCam);
// 		this->setCameraMask(4);
// 	}
//	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PAUSE_GAME]);
	auto cameras = Director::getInstance()->getRunningScene()->getCameras();
	if (cameras.size() >= 2){
		auto flag = (unsigned short)cameras[cameras.size() - 1]->getCameraFlag();
		view->setCameraMask((unsigned short)cameras[cameras.size() - 1]->getCameraFlag(), true);
		this->setCameraMask((unsigned short)cameras[cameras.size() - 1]->getCameraFlag(), true);
	}
	else
	{
		view->setCameraMask(1, true);
		this->setCameraMask(1, true);
	}

	m_bSeaPause = false;
	m_seaLayer = nullptr;
	m_battleLayer = nullptr;
	m_seaLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));//海上的层
	m_battleLayer = (TVBattleManager*)(currentScene->getChildByTag(SCENE_TAG::BATTLE_TAG + 100));
	if (m_battleLayer)
	{
		m_bSeaPause = m_battleLayer->getIsPause();
		if (!m_bSeaPause)
		{
			m_battleLayer->stopBattle();
		}
	}
	if (m_seaLayer)
	{
		m_bSeaPause = m_seaLayer->getIsPause();
		if (!m_bSeaPause)
		{
			m_seaLayer->seaEventStart();
		}
	}
	//TODO 暂时方案是把背景音乐关闭
	SINGLE_AUDIO->vpauseBGMusic();
}

