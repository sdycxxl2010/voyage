#include "TVSingle.h"
#include "json/document.h"
#include "editor-support/cocostudio/CCSGUIReader.h"
#include "env.h"

#include "TVGlobalConfig.h"
#include "ServerManager.h"
#include "TVGlobalTicker.h"
#include "TVSceneManager.h"
#include "TVEventManager.h"
#include "TVStaticDataManager.h"
#include "UIManager.h"
#include "TVNetManager.h"
#include "TVSocketWorker.h"
#include "Localization.h"


//#include "ShipPathData.h"

TVSingle* TVSingle::m_instance = nullptr;
std::map<int,std::vector<cocos2d::Point>> TVSingle::PATHS;
TVSingle::TVSingle()
{
	m_vHero = nullptr;
	m_AudioMG = nullptr;
	m_ShopData = nullptr;
	m_ChatData = nullptr;
	m_SafeAreaCheck = nullptr;
	
	InitGlobalSingleton();

}

TVSingle::~TVSingle()
{
	//m_vHero->release();
	delete m_vHero;
	delete m_AudioMG;
	delete m_ShopData;
	delete m_ChatData;
}

void TVSingle::InitGlobalSingleton()
{
	//very basics
	new TVGlobalConfig();//1 default config
	new TVGlobalTickerMgr();//2
	new TVEventManager();

	//basic manager;
	new Localization();
	new ServerManager();
	new TVStaticDataManager();
	new TVSocketWorker();
	new TVNetManager();
	new UIManager();

	//logic
	if (TVGlobalConfig::getInstance()->Enalbe_NewUIArch == 1)
	{
		new TVSceneManager();
	}
}


void TVSingle::InitUserSettings()
{
	if (UserDefault::getInstance()->getIntegerForKey(SOUND_EFFECT_KEY) != CLOSE_AUDIO)
	{
		SINGLE_AUDIO->setEffectON(true);
	}
	else
	{
		SINGLE_AUDIO->setEffectON(false);
	}

}


TVHero* TVSingle::getCharacter()
{
	if (!m_vHero) 
	{
		m_vHero = new TVHero();
	}

	return m_vHero;
}

TVAudioManager* TVSingle::getAudioMG()
{
	if (!m_AudioMG)
	{
		m_AudioMG = new TVAudioManager;
	}
	return m_AudioMG;
}

ShopData* TVSingle::getShopData()
{
	if (!m_ShopData)
	{
		m_ShopData = new ShopData;
		m_ShopData->loadShopData();
	}

	return m_ShopData;
}

ChatData* TVSingle::getChatData()
{
	if (!m_ChatData) 
	{
		m_ChatData = new ChatData();
	}

	return m_ChatData;
}

SafeAreaCheck* TVSingle::getSafeAreaCheck(){
	if(!m_SafeAreaCheck){
		m_SafeAreaCheck = new SafeAreaCheck();
		m_SafeAreaCheck->initLandHistTestData();
	}
	return m_SafeAreaCheck;
}
