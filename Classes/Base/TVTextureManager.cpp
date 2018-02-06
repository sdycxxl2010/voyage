#include "TVTextureManager.h"


TVTextureManager* TVTextureManager::m_Instance = nullptr;

TVTextureManager::TVTextureManager()
	:m_IsPrintLog(false),
	m_LifeTime(100),
	m_LifeFlag(0)
{
	startTextureManage();
}
TVTextureManager::~TVTextureManager()
{
	stopTextureManage();
}

void TVTextureManager::startTextureManage()
{
	cocos2d::Director::getInstance()->getScheduler()->schedule(CC_CALLBACK_1(TVTextureManager::check,this),this,0,false,"texturemanage");
}
void TVTextureManager::stopTextureManage()
{
	cocos2d::Director::getInstance()->getScheduler()->unschedule("texturemanage",this);
}

void TVTextureManager::check(float dt)
{
	m_LifeFlag++;
	if (m_LifeFlag > m_LifeTime)
	{
		auto textCache = cocos2d::TextureCache::getInstance();
		textCache->removeUnusedTextures();
		if (m_IsPrintLog)
		{
			std::string info = textCache->getCachedTextureInfo();
			info = info.substr(info.length() - 100);
			cocos2d::log("log: %s",info.c_str());
		}
	}
}
