#ifndef __SINGLE_H__
#define __SINGLE_H__
#include "TVBasic.h"

#include "TVHero.h"
#include "TVAudioManager.h"
#include "ShopData.h"
#include "ChatData.h"
#include "TVAudioData.h"
#include "SafeAreaCheck.h"

#define GETSINGLE TVSingle::getInstance()
#define SINGLE_HERO GETSINGLE->getCharacter()
#define SINGLE_AUDIO GETSINGLE->getAudioMG()
#define SINGLE_SHOP GETSINGLE->getShopData()
#define SINGLE_CHAT GETSINGLE->getChatData()
#define SINGLE_SAFEAREA GETSINGLE->getSafeAreaCheck()

class TVSingle
{
public:
	TVSingle();
	~TVSingle();

public:
	static TVSingle* m_instance;

	static std::map<int, std::vector<cocos2d::Point>> PATHS;

	static TVSingle*  getInstance()
	{
		if (m_instance == NULL)
		{
			m_instance = new TVSingle;
		}
		return m_instance;
	}

public:
	TVHero* getCharacter();
	TVAudioManager* getAudioMG();
	ShopData* getShopData();
	ChatData* getChatData();
	SafeAreaCheck* getSafeAreaCheck();

private:
	void InitGlobalSingleton();
	void InitUserSettings();
	
private:
	TVHero* m_vHero;
	TVAudioManager* m_AudioMG;
	ShopData* m_ShopData;
	ChatData* m_ChatData;
	SafeAreaCheck* m_SafeAreaCheck;
};

#endif



