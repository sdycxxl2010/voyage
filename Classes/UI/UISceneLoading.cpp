#include "UISceneLoading.h"
#include "UIInform.h"
#include "UIGuidePort.h"
#include "ParticleManager.h"
#include "UIBank.h"
#include "UIExchange.h"
#include "UIShipyard.h"
#include "UITavern.h"
#include "UICenter.h"
#include "UISailManage.h"
#include "UIPalace.h"
#include "UIMain.h"
#include "UIGuideMain.h"
#include "UIGuidePalace.h"
#include "UIGuidePort.h"
#include "UIGuideCenterCharactorSkill.h"
#include "UIGuideShipyard.h"
#include "UIGuideTarven.h"
#include "UIPort.h"
#include "TVSceneLoader.h"
#include "UIGuideExchange.h"
#include "WarManage.h"
#include "TVBattleManager.h"
#include "AppHelper.h"

int UISceneLoading::particleloaded = false;

#define  LOAD_COUNT_PERFRAME  5
#define  LOAD_FRAME_INTERVAL  2
#define  LOAD_WAIT_INTERVAL  2

UISceneLoading::UISceneLoading() :
mParent(nullptr),
m_Paths(nullptr),
m_Tag(0),
m_Max(0),
m_bStartLoadPrc(false),
m_nTickCnt(0),
m_nLoadCnt(0),
m_nWaitCnt(0)
{

}

UISceneLoading::~UISceneLoading()
{
	_eventDispatcher->removeEventListenersForTarget(this);
}

 void UISceneLoading::onEnter(){
 	Layer::onEnter();
 }

void UISceneLoading::onExit()
{
	Layer::onExit();
}

/*
 * Create Loadinglayer
 * Params:
 * paths, the path to the picture resources.
 * tag, the scene to be loaded tag.
 */
UISceneLoading* UISceneLoading::create(std::vector<std::string> & paths, int tag)
{
	UISceneLoading *pRet = new UISceneLoading();
	if (pRet && pRet->init())
	{
		UIStoryLine::GetInstance()->stopAllAction();
		UIInform::getInstance()->closeAutoInform(0);
		pRet->m_Paths = &paths;
		pRet->m_Tag = tag;
		pRet->showLoading_1(0);
		pRet->cleanCache();
		if(!particleloaded){
			pRet->LoadingParticle();
		}
		pRet->loadEffectPlist();
		pRet->loadingPic();
		pRet->autorelease();
		pRet->retain();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

void UISceneLoading::cleanCache()
{
	AnimationCache::destroyInstance();
	SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
}

bool UISceneLoading::init()
{
	bool pRet = false;
	do
	{
		CC_BREAK_IF(!Layer::init());
		auto winSize = Director::getInstance()->getWinSize();
		auto image_bg = ImageView::create(StringUtils::format("res/loading/%d.jpg", SINGLE_SHOP->Loading_bg));
		image_bg->setTouchEnabled(true);
		image_bg->setContentSize(winSize);
		image_bg->setPosition(Vec2(winSize.width/2,winSize.height/2));
		this->addChild(image_bg);

		auto image_content_bg = ImageView::create("res/loading/content_bg.png");
		image_content_bg->setContentSize(Size(968, 144));
		image_content_bg->setPosition(Vec2(639, 135));
		this->addChild(image_content_bg);

		this->scheduleUpdate();
		pRet = true;
	} while (0);

	return pRet;
}


void UISceneLoading::showLoading_1(float loadDiffTag)
{
	auto t_content = Text::create();
	t_content->setContentSize(Size(770, 144));
	t_content->setPosition(Vec2(639, 135));
	t_content->ignoreContentAdaptWithSize(false);
	t_content->setAnchorPoint(Vec2(0.5, 0.5));
	t_content->setTextHorizontalAlignment(TextHAlignment::CENTER);
	t_content->setTextVerticalAlignment(TextVAlignment::CENTER);
	t_content->setFontSize(24);
	t_content->setTextColor(Color4B(255, 255, 255, 255));
	this->addChild(t_content);
	int indextext = cocos2d::random(1, 15);
	t_content->setString(SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_LOADING_TEXT_%d", indextext)]);

	auto image_load_bg = ImageView::create("res/loading/load_bg.png");
	image_load_bg->setContentSize(Size(60,60));
	image_load_bg->setPosition(Vec2(1050,46));
	image_load_bg->ignoreContentAdaptWithSize(false);
	image_load_bg->setAnchorPoint(Vec2(0.5,0.5));
	this->addChild(image_load_bg);
	auto  LoadBglrepeat = RepeatForever::create(RotateBy::create(1, -180));
	image_load_bg->runAction(LoadBglrepeat);

	auto image_load = ImageView::create("res/loading/load.png");
	image_load->setContentSize(Size(36, 36));
	image_load->setPosition(Vec2(1050, 46)); 
	image_load->ignoreContentAdaptWithSize(false);
	image_load->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(image_load);
	auto LoadBgSmalllrepeat = RepeatForever::create(RotateBy::create(1, 180));
	image_load->runAction(LoadBgSmalllrepeat);

	auto image_text_bg = ImageView::create("res/loading/text_bg.png");
	image_text_bg->setContentSize(Size(188, 46));
	image_text_bg->setPosition(Vec2(1186, 45));
	this->addChild(image_text_bg);

	auto label_loading = Text::create();
	label_loading->setContentSize(Size(180, 40));
	label_loading->setPosition(Vec2(1178, 45));
	label_loading->setTextHorizontalAlignment(TextHAlignment::LEFT);
	label_loading->setTextVerticalAlignment(TextVAlignment::CENTER);
	label_loading->setFontSize(22);
	label_loading->setTextColor(Color4B(255, 255, 255, 255));
	this->addChild(label_loading);
	label_loading->setString(SINGLE_SHOP->getTipsInfo()["TIP_LOADING_TEXT"]);
	auto act_1 = FadeOut::create(1.0f);
	auto act_2 = FadeIn ::create(1.0f);
	auto sequence = Sequence::create(act_1,act_2,NULL);
	auto labelrepeat = RepeatForever::create(sequence);
	label_loading->runAction(labelrepeat);
}

void UISceneLoading::loadingPic()
{
	int index = 0;
	m_Max = m_Paths->size();
	if(m_Max == 0)
	{
		goToNextScene();
		return;
	}
	/*
	for (index = 0; index < m_Max; index++)
	{
		log("preload: %s index:%d m_Max:%d", m_Paths->at(index).c_str(), index, m_Max);
		if (cocos2d::FileUtils::getInstance()->isFileExist(m_Paths->at(index)))
		{
			Director::getInstance()->getTextureCache()->addImageAsync(m_Paths->at(index), CC_CALLBACK_1(UISceneLoading::loadPicCallBack, this, index));
		}
		else{
			if (index == m_Max - 1){
				goToNextScene();
			}
		}
	}
	*/
}

void UISceneLoading::update(float f)
{
	if (!m_bStartLoadPrc)
	{
		m_nWaitCnt++;
		if (m_nWaitCnt > LOAD_WAIT_INTERVAL)
		{
			m_bStartLoadPrc = true;
			m_nLoadCnt = 0;
			m_nTickCnt = 0;
		}
	}
	else
	{
		m_nTickCnt--;
		if (m_nTickCnt <= 0)
		{
			log("AppHelper::frameCount:%d", AppHelper::frameCount);
			for (int i = 0; i < LOAD_COUNT_PERFRAME; i ++)
			{
				int idx = m_nLoadCnt * LOAD_COUNT_PERFRAME + i;
				if (idx >= m_Max)
				{
					goToNextScene();
					break;
				}
				log("preload: %s index:%d m_Max:%d", m_Paths->at(idx).c_str(), idx, m_Max);
				if (cocos2d::FileUtils::getInstance()->isFileExist(m_Paths->at(idx)))
				{
					Director::getInstance()->getTextureCache()->addImage(m_Paths->at(idx));
				}
			}
			m_nLoadCnt++;
			m_nTickCnt = LOAD_FRAME_INTERVAL;
		}
	}
}

void UISceneLoading::LoadingParticle()
{
	particleloaded = true;
	auto particle=ParticleManager::getInstance();
	particle->AddPlistData("bulletsmoke_1.plist","bulletsmoke");
}

void UISceneLoading::loadPicCallBack(cocos2d::Texture2D* texture, int index)
{
	log("UISceneLoading-preload: %d, %d", index, m_Max);
	if (index == m_Max - 1)
	{
		goToNextScene();
	}
}

void UISceneLoading::runNextScene(float dt)
{
	log("preload go to scene:%d", m_Tag);
	this->removeAllChildren();
	Scene* scene = Scene::createWithPhysics();
	if (scene)
	{
		UIBasicLayer* layer;
		switch(m_Tag)
		{
		case BANK_TAG:
			layer = UIBank::createBank();
			break;
		case WHARF_TAG:
			layer = UIPort::create();
			break;
		case DOCK_TAG:
			layer = UIShipyard::createDock();
			break;
		case EXCHANG_TAG:
			layer =UIExchange::createExchange();
			break;
		case PUPL_TAG:
			layer =UITavern::createPup();
			break;
		case CENTER_TAG:
			layer =UICenter::createCenter();
			break;
		case MAP_SAILING_TAG:
			layer =UISailManage::create(Point(-1,-1),1);
			break;
		case MAP_TAG:
			layer =UISailManage::create();
			break;
		case PALACE_TAG:
			layer =UIPalace::createPalace();
			break;
		case MAIN_TAG:
			layer = UIMain::createMainLayer();
			break;
		case MAINGUID_TAG:
			layer = UIGuideMain::create();
			break;
		case PALACEGUID_TAG:
			layer = UIGuidePalace::createPalace();
			break;
		case EXCHANGEGUID_TAG:
			layer = UIGuideExchange::createExchange();
			break;
		case SKILLGUIDE_TAG:
			layer = UIGuideCenterCharactorSkill::createCenter();
			break;
		case WHARF_GUIDE_TAG:
			layer = UIGuidePort::create();
		   break;
		case DOCK_GUIDE_TAG:
			layer = UIGuideShipyard::createDockLayerGuide();
			break;
		case PUP_GUIDE_TAG:
			layer = UIGuideTarven::createPupLayerGuide();
			break;
		case COUNTRY_WAR_TAG:
			layer = WarManage::createWarManage();
			break;
		case BATTLE_TAG:
			layer = TVBattleManager::createBattle(SINGLE_HERO->m_pBattelData, SINGLE_HERO->shippos);
			break;
		default:
			assert("no idea what it is.");
			break;


		}

		if(layer == nullptr)
		{
			log("preload layer is null.");
		}

		scene->addChild(layer,1,100 + m_Tag);
		Director::getInstance()->replaceScene(scene);
		//Director::getInstance()->replaceScene(TransitionFade::create(0.1f,scene));
	}
}

void UISceneLoading::goToNextScene()
{
	log("goToNextScene.");
	this->unschedule(schedule_selector(UISceneLoading::update));
	this->scheduleOnce(schedule_selector(UISceneLoading::runNextScene), 0.5f);
}

void UISceneLoading::loadEffectPlist()
{
	if (m_Tag == COUNTRY_WAR_TAG || m_Tag == BATTLE_TAG)
	{
		//战斗的plist文件预加载
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_battle0.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_battle1.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_battle2.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_battle3.plist");
		
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon0.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon1.plist");
		//主键转动动画
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/test/fx.plist");
	}
}