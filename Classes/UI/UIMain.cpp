#include "UIMain.h"
#include "UISocial.h"
#include <stdlib.h>
#include "UIEmail.h"
#include "UIPirate.h"
#include "UICompanions.h"
#include "UIHandBook.h"
#include "MainTaskManager.h"
#include "CompanionTaskManager.h"
#include "UIOffLine.h"
#include "TVSceneLoader.h"
#include "UIEveryDaySign.h"

#include "UICommon.h"
#include "UIInform.h"
#include "UITask.h"
#include "UIStore.h"
#include "Utils.h"
#include "UIMap.h"
#include "UIGuild.h"
#include "UISetting.h"
#include "UIPackageDeals.h"
#include "UIVerifyAccount.h"
#include "UINationWarEntrance.h"
#include "TimeUtil.h"
//雨滴频率
#define  RAINRATE  1.0
//雨滴偏转角度 >0 向右 <0向左
#define  RAINANGLE -40
//雨滴直线下落的速度 数值越小夏洛越快
#define  RAINSPEED 20
UIMain::UIMain()
	:m_pCityDataResult(nullptr),
	m_pCheckResult(nullptr),
	m_nGuardIndex(0),
	m_pMainChat(nullptr),
	m_bMoveActionFlag(true),
	m_bisFirstShow(true),
	m_pCurTaskDefine(nullptr),
	m_nLastGuardTag(1)
{ 
	m_eUIType = UI_PORT;
	m_nShowChatTime = 0;
	m_bIsNeutralCityChange = false;
	m_nChatItem = 0;
	m_nToTalGuardPage = 1;
	m_nCurGuardPage = 1;
	m_bIsGuardShowFromSea = true;
	m_bIsFirstChat = true;
	m_nCompletedAllTasks = 0;
	isNewLanding = 0;

	lenNum = 0;
	lenAfter = 0;
	plusNum = 0;
	chatContent ="";
	anchPic = nullptr;
	chatTxt = nullptr;
	guardDiaolgOver = false;
	m_bPopItemFlag = true;
    m_raindrops = new std::vector<Sprite*>(5);
	m_bShowAndHideAction = true;
	m_palaceInfoResult = nullptr;
	isUsePropBackToCity = false;
	m_nTipImageIndex = IMAGE_TIP_NONE;
	m_pFastHireCrewResult = nullptr;
	m_nFastHireCrewNum = 0;
	m_bReceive = 0;
	m_bClifford = 0;
	m_bNewbieGuidePretask = false;
	m_friendValueResult = nullptr;
	m_producesResult = nullptr;
	m_bIsShowLibao = false;
	m_bIsGuardExit = false;
	m_bIsChargeVSuccees = false;
    m_loadingLayer = nullptr;
	m_bIsEnemyCity = false;
	m_bIsRepair = false;
	m_nRepairCost = 0;
}

UIMain::~UIMain()
{
    if (m_raindrops)
    {
        delete m_raindrops;
        m_raindrops = nullptr;
    }
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
	SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_city.plist");
	SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_uicommon0.plist");
	SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_uicommon1.plist");
	CC_SAFE_RELEASE(m_vRoot);
}

void UIMain::onEnter()
{
	
	UIBasicLayer::onEnter();
}

void UIMain::onExit()
{
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
	this->unschedule(schedule_selector(UIMain::updateMailNumber));
	this->unschedule(schedule_selector(UIMain::updateBySecond));
	UIBasicLayer::onExit();
}

UIMain* UIMain::createMainLayer()
{
	UIMain* mainLayer = new UIMain;
	if (mainLayer)
	{
		mainLayer->init();
		mainLayer->autorelease();
		return mainLayer;
	}
	CC_SAFE_DELETE(mainLayer);
	return nullptr;
}

bool UIMain::init()
{
	bool pRet = false;
	do 
	{
		UIBasicLayer::init();
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIMain::onServerEvent, this),this);
		ProtocolThread::GetInstance()->getCurrentCityData(UILoadingIndicator::create(this,m_eUIType));
		this->scheduleOnce(schedule_selector(UIMain::initRes),0);
		m_loadingLayer = UILoadingIndicator::create(this, 0);
		m_loadingLayer->showSelf();
		pRet = true;
	} while (0);
	return pRet;
}

void UIMain::initRes(const float fTime)
{
	if (m_vRoot)
	{
		return;
	}
	auto fakeRoot = dynamic_cast<Node*>(CSLoader::createNode("voyageUI_city.csb"));
	if (fakeRoot->getChildrenCount() == 1)
	{
		m_vRoot = dynamic_cast<Widget*>(fakeRoot->getChildren().at(0));
		m_vRoot->removeFromParent();
	}

	CC_SAFE_RETAIN(m_vRoot);
	addEventForChildren(m_vRoot);

	UISocial::getInstance()->getSocialChat().size();
	this->schedule(schedule_selector(UIMain::updateMailNumber), 5);

	auto l_events = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_vRoot, "listview_event"));
	auto b_events = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_events"));
	l_events->setVisible(false);
	b_events->setVisible(false);
}

void UIMain::showBarData()
{
	if (!m_vRoot)
	{
		initRes(0);
	} 
	if (!m_vRoot->getParent())
	{
		this->addChild(m_vRoot);
	}

	UISocial::getInstance()->m_nChatIndex = UISocial::getInstance()->getSocialChat().size() - 1;
	if(UISocial::getInstance()->m_nChatIndex < 0)
	{
		UISocial::getInstance()->m_nChatIndex = 0;
	}
	//中立城市变敌对
	SINGLE_HERO->m_iNwantedforceids = m_pCityDataResult->n_wantedforceids;
	SINGLE_HERO->m_iWantedforceids = m_pCityDataResult->wantedforceids;
	for (int j = 0; j < m_pCityDataResult->n_wantedforceids; j++)
	{
		if (m_pCityDataResult ->current_city_nation== m_pCityDataResult->wantedforceids[j])
		{
			m_bIsNeutralCityChange = true;
			break;
		}else
		{
			m_bIsNeutralCityChange = false;	
		}
	}
	flushCionAndGold(m_pCityDataResult->data->coin, m_pCityDataResult->data->gold);
	bgMusicAndSound();

	loadBackground();
	loadCityInfo();
	loadOtherInfo();
	villageButtonShow();
	flushMainChat();
	getAllUnreadImage(ProtocolThread::GetInstance()->getLastCheckMailBoxResult());
	if (m_loadingLayer)
	{
		m_loadingLayer->removeSelf();
		m_loadingLayer = nullptr;
	}
	//判断新手引导结束之后副官的情报对话
	if (ProtocolThread::GetInstance()->getMainTaskStatus() == 9)
	{
		ProtocolThread::GetInstance()->setMainTaskStatus(nullptr,10);
		newbieGuideEnd();
	}
	else
	{
		notifyCompleted(NONE);
	}

	showDorpservice();
	showBagOverWeight();
	this->schedule(schedule_selector(UIMain::updateBySecond), 1);
}

/*
* 当剧情或者对话动作结束时调用该函数
* actionIndex, 当前动作的index
*/
void UIMain::notifyCompleted(int actionIndex)
{
	//扫尾工作
	switch (actionIndex){
	case DAILY_SIGN_IN:
			break;
	case OFFLINE_DELEGATE:
		break;
	case MAIN_STORY:
			break;
	case SMALL_STORY:
		break;
	case COMPANION_STORY:
		break;
	case REDUCE_NOTICE:
		break;
	case FIRST_MATE:
		break;
	default:
		break;
	}
	doNextJob(actionIndex);
}

/*
* 执行下一个对话或者剧情操作
* actionIndex, 当前动作的index
*/
void UIMain::doNextJob(int actionIndex)
{
	actionIndex++;
	switch (actionIndex)
	{
		case MAIN_STORY:
			//剧情（主线）
			//主线任务完成对话
			if (MainTaskManager::GetInstance()->checkTaskAction())
			{
				auto mainTask1 = m_vRoot->getChildByName<ImageView*>("image_main_city_bg");
				addNeedControlWidgetForHideOrShow(mainTask1, true);
				allTaskInfo(false, MAIN_STORY);
				MainTaskManager::GetInstance()->notifyUIPartDelayTime(0.5);
			}
			else
			{
				notifyCompleted(MAIN_STORY);
			}
			break;
		case COMPANION_STORY:
			//小伙伴剧情
			if (CompanionTaskManager::GetInstance()->checkTaskAction())
			{
				auto mainTask1 = m_vRoot->getChildByName<ImageView*>("image_main_city_bg");
				addNeedControlWidgetForHideOrShow(mainTask1, true);
				allTaskInfo(false, COMPANION_STORY);
				CompanionTaskManager::GetInstance()->notifyUIPartDelayTime(0.5);
			}
			else
			{
				notifyCompleted(COMPANION_STORY);
			}
			break;
		case SMALL_STORY:
			//小剧情
			if (SINGLE_HERO->m_nTriggerId)
			{
				auto mainTask1 = m_vRoot->getChildByName<ImageView*>("image_main_city_bg");
				addNeedControlWidgetForHideOrShow(mainTask1, true);
				allTaskInfo(false, SMALL_STORY);
				//此处应该有callback吧？否则你咋知道什么时候结束呢？
				notifyUIPartDelayTime(0.5);
			}
			else
			{
				notifyCompleted(SMALL_STORY);
			}
			break;
		case DAILY_SIGN_IN:
			//每日签到
			if (isNewLanding && SINGLE_HERO->m_iDailyrewarddata)
			{
				m_bIsGuardShowFromSea = false;
				UIEveryDaySign*every = UIEveryDaySign::createEveryDaysign(1);
				this->addChild(every);
				every->setLocalZOrder(100);
			}
			else
			{
				notifyCompleted(DAILY_SIGN_IN);
			}
			break;
		case OFFLINE_DELEGATE:
		{
			//离线委托结算
			EmailLoginResult * result = ProtocolThread::GetInstance()->getLastLoginResult();
			//离线应该是offline好嘛，怎么是outline？签到和离线委托结算冲突？
			if (isNewLanding && SINGLE_HERO->m_isShowOutLineResult && result->delegatecoins != 0)
			{
				UIOffLine::openOffLineDelegateLayer(this, 1);
				SINGLE_HERO->m_isShowOutLineResult = false;
			}
			else
			{
				notifyCompleted(OFFLINE_DELEGATE);
			}
			break;
		}
		case REDUCE_NOTICE:
			//扣除玩家的声望和货币
			if (isNewLanding)
			{
				ProtocolThread::GetInstance()->reduceInNationWar(UILoadingIndicator::create(this));
				//notifyCompleted(REDUCE_NOTICE);
			}
			else
			{
				notifyCompleted(REDUCE_NOTICE);
			}
			break;
		case FIRST_MATE:
			//副官情报以及卫兵
			if (m_pCityDataResult->friendvalue < 0 && SINGLE_HERO->m_iCurCityNation != 8)
			{
				m_bIsEnemyCity = true;
			}
			if (isNewLanding)
			{
				openGuardChat(GUARD_INDEX::NONE);
			}
			else
			{
				m_bIsGuardExit = true;
				auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
				auto i_soldier = guard_dialog->getChildByName<ImageView*>("image_soldier");
				i_soldier->runAction(Sequence::create(DelayTime::create(1),CallFunc::create(CC_CALLBACK_0(UIMain::checkBountyBoard, this)), nullptr));
				notifyCompleted(FIRST_MATE);
			}
			break;
		default:
			break;
	}
}

void UIMain::loadBackground()
{
	auto picNum = 0;
	if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
	{
		picNum = SINGLE_SHOP->getCityAreaResourceInfo()[SINGLE_HERO->m_iCityID].background_village;
	}
	else
	{
		picNum = SINGLE_SHOP->getCityAreaResourceInfo()[SINGLE_HERO->m_iCityID].background_id;
	}
	std::string cityName = StringUtils::format("cities_resources/background/city_%d.jpg", picNum);
	auto i_city_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_main_city_bg"));
	i_city_bg->ignoreContentAdaptWithSize(false);

	//伦敦城市雾都
	if (SINGLE_HERO->m_iCityID == 3)
	{
		if (SINGLE_HERO->m_londonWeather == 0)
		{
			//随机种子
			srand(time(0));
			int a = rand() % 99;
			SINGLE_HERO->m_londonWeather = (a <= 49) ? 1 : 2;
		}
		if (SINGLE_HERO->m_londonWeather == 2)
		{
			m_raindrops->clear();
			for (int i = 0; i < 30; ++i)
			{
				auto rain = Sprite::create();
				rain->setScale(0.4);
				addChild(rain);
				m_raindrops->push_back(rain);
			}

			this->schedule(schedule_selector(UIMain::showRaindrops),RAINRATE);
			//伦敦下雨 背景暂时只用一张
			cityName = StringUtils::format("cities_resources/background/city_%d_%d.jpg", SINGLE_SHOP->getCityAreaResourceInfo()[SINGLE_HERO->m_iCityID].background_id, 1);
		}
	}
	i_city_bg->loadTexture(cityName);
}

void UIMain::flushCionAndGold(const int64_t nCoin, const int64_t nGold)
{
	SINGLE_HERO->m_iCoin = nCoin;
	SINGLE_HERO->m_iGold = nGold;
	auto t_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(m_vRoot,"label_coin_num"));
	auto t_gold = dynamic_cast<Text*>(Helper::seekWidgetByName(m_vRoot,"label_gold_num"));
	t_coin->setString(numSegment(StringUtils::format("%lld",nCoin)));
	t_gold->setString(numSegment(StringUtils::format("%lld", nGold)));
	setTextFontSize(t_coin);
	setTextFontSize(t_gold);
}

void UIMain::flushVActivityBonus()
{
	m_bIsChargeVSuccees = true;
	auto image_activity_bonus = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_activity_bonus"));
	image_activity_bonus->setVisible(false);
}

void UIMain::flushSudanMaskNum(const int nNum)
{
	m_pCityDataResult->sudanmasknum += nNum;
}

void UIMain::loadCityInfo()
{
	auto i_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot,"image_head"));
	auto i_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot,"image_flag"));
	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(m_vRoot,"label_city_name"));
	i_head->ignoreContentAdaptWithSize(false);
	i_head->loadTexture(getPlayerIconPath(m_pCityDataResult->data->iconidx));
	i_flag->ignoreContentAdaptWithSize(false);
	i_flag->loadTexture(getCountryIconPath(SINGLE_HERO->m_iCurCityNation));
	
	t_name->setString(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].name);
	dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot,"image_head_player_bg"))->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
	flushExpAndRepLv();
}

void UIMain::showRaindrops(float dt)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    unsigned rand_seed = (unsigned)(now.tv_sec * 1000 + now.tv_usec / 1000); 
    srand(rand_seed);

    static unsigned i = 0;
    if (++i == (m_raindrops->size()-1))
    {
        i = 0;
    }

    int x = rand() % static_cast<int>(Director::getInstance()->getVisibleSize().width);
	int y = rand() % static_cast<int>(Director::getInstance()->getVisibleSize().height);
	m_raindrops->at(i)->setPosition(x, y);
	m_raindrops->at(i)->setOpacity(255);

	//在主城特效-用时加载
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_city.plist");
	Vector<SpriteFrame *> frames;
	for (int i = 0; i <= 12; i++)
	{
		auto name = StringUtils::format("raindrops/eff_raindrops_%02d.png", i);
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
		frames.pushBack(frame);
	}

	Animation* animation = Animation::createWithSpriteFrames(frames, 1.0f / 16, 1);
	auto a = Animate::create(animation);
    a->setDuration(5.0f);
    m_raindrops->at(i)->stopAllActions();
	m_raindrops->at(i)->runAction(a);
	m_raindrops->at(i)->runAction(Sequence::create(MoveTo::create(RAINSPEED, Vec2(m_raindrops->at(i)->getPositionX(), 0)),nullptr));
}

void UIMain::loadOtherInfo()
{
	if (SINGLE_HERO->m_iCurCityNation == 8)
	{
		if (m_pCityDataResult->hassudanmask)
		{
			setMainLayerButton(true);
		}else
		{
			setMainLayerButton(false);
		}
	}else if (m_pCityDataResult->friendvalue < 0)
	{
		setMainLayerButton(false);
	}else if (m_bIsNeutralCityChange)
	{
		setMainLayerButton(false);
	}else
	{
		setMainLayerButton(true);
	}
	SINGLE_HERO->m_iHaslisence = m_pCityDataResult->haslisence;
	SINGLE_HERO->m_iEffectivelisence = m_pCityDataResult->license_can_use;
	auto button_unfold = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_unfold"));
	auto button_handbook = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_handbook"));
	auto button_companions = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_companions"));
	auto button_customerservice = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_customerservice"));
	button_unfold->setTouchEnabled(true);
	button_handbook->setTouchEnabled(true);
	button_companions->setTouchEnabled(true);
	button_customerservice->setTouchEnabled(true);
}

void UIMain::openGuardChat(int index)
{
	m_nGuardIndex = index;
	auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
	auto i_soldier = guard_dialog->getChildByName<ImageView*>("image_soldier");
	auto t_guard = guard_dialog->getChildByName<Text*>("label_guard");
	switch (index)
	{
	case GUARD_INDEX::NONE:
	case BATTLE_NPC_FAIL:
		{
			if (m_pCityDataResult->reason == 2)
			{
				m_nLastGuardTag = 1;
				if (m_bisFirstShow)
				{
					m_bisFirstShow = false;
					m_nGuardIndex = BATTLE_NPC_FAIL;
					log("BATTLE_NPC_FAIL guard enter");
					guardEnterScene(0);
				}
				break;
			}
		}
	case SIDE_TASK_BATTLE_NPC:
		{
			if (m_pCityDataResult->completetaskid && SINGLE_SHOP->getCoreTaskById(m_pCityDataResult->completecoretaskid).finish_target_npcId)
			{
				m_nLastGuardTag = 1;
				if (m_bisFirstShow)
				{
					m_bisFirstShow = false;
					m_nGuardIndex = SIDE_TASK_BATTLE_NPC;
					log("SIDE_TASK_BATTLE_NPC guard enter");
					guardEnterScene(0);
				}
				break;
			}
		}
	case ENEMY_CITY_CHAT:
		{
			//敌对城市 士兵提醒
			if (m_pCityDataResult->friendvalue < 0 && SINGLE_HERO->m_iCurCityNation != 8)
			{
				m_nLastGuardTag = 2;
				m_nGuardIndex = ENEMY_CITY_CHAT;
				if (m_bisFirstShow)
				{
					log("ENEMY_CITY_CHAT guard enter");
					m_bisFirstShow = false;
					if (m_bIsNeutralCityChange)
					{
						m_nGuardIndex = NEUTRAL_CHANGETO_ENEMY_CHAT;
					}
					guardEnterScene(0);
				}else
				{
					log("ENEMY_CITY_CHAT guard fadeout fadein");
					if (i_soldier->getPositionX() < 0)
					{
						guardEnterScene(0);
					}
					else if (strcmp(t_guard->getString().c_str(), SINGLE_SHOP->getTipsInfo()["TIP_MAIN_GUARD"].c_str()) != 0)
					{
						guardExitScene(0);
						this->scheduleOnce(schedule_selector(UIMain::guardEnterScene), 1.2);
					}
					else
					{
						chatFadeOut(0);
						//副官已在舞台上 直接改变对话显示内容
						this->scheduleOnce(schedule_selector(UIMain::chatFadeIn), 0.5);
					}
				}
				break;
			}
		}
	case DISGUISE_CITY_CAHT:
		{
			//使用伪装道具进城 士兵提醒
			if (SINGLE_HERO->m_iCurCityNation == 8 && !m_pCityDataResult->hassudanmask)
			{
				m_nLastGuardTag = 2;
				m_nGuardIndex = DISGUISE_CITY_CAHT;
				if (m_bisFirstShow)
				{
					log("DISGUISE_CITY_CAHT guard enter");
					if (m_bIsNeutralCityChange)
					{
						m_nGuardIndex = NEUTRAL_CHANGETO_ENEMY_CHAT;
					}
					m_bisFirstShow = false;
					guardEnterScene(0);
				}else
				{
					log("DISGUISE_CITY_CAHT guard fadeout fadein");
					if (i_soldier->getPositionX() < 0)
					{
						guardEnterScene(0);
					}
					else if (strcmp(t_guard->getString().c_str(), SINGLE_SHOP->getTipsInfo()["TIP_MAIN_GUARD"].c_str()) != 0)
					{
						guardExitScene(0);
						this->scheduleOnce(schedule_selector(UIMain::guardEnterScene), 1.2);
					}
					else
					{
						chatFadeOut(0);
						//副官已在舞台上 直接改变对话显示内容
						this->scheduleOnce(schedule_selector(UIMain::chatFadeIn), 0.5);
					}
				}
				break;
			}
		}
	case NEUTRAL_CHANGETO_ENEMY_CHAT:
		{
			//副官提醒
			if (m_bIsNeutralCityChange)
			{
				m_nLastGuardTag = 1;
				m_nGuardIndex = NEUTRAL_CHANGETO_ENEMY_CHAT;
				if (m_bisFirstShow)
				{
					log("NEUTRAL_CHANGETO_ENEMY_CHAT guard enter");
					m_bisFirstShow = false;
					guardEnterScene(0);
				}else
				{
					log("NEUTRAL_CHANGETO_ENEMY_CHAT guard fadeout fadein");
					if (i_soldier->getPositionX() < 0)
					{
						guardEnterScene(0);
					}
					else if (strcmp(t_guard->getString().c_str(), SINGLE_SHOP->getTipsInfo()["TIP_MAIN_GUARD"].c_str()) == 0)
					{
						guardExitScene(0);
						this->scheduleOnce(schedule_selector(UIMain::guardEnterScene), 1.2);
					}
					else
					{
						chatFadeOut(0);
						//副官已在舞台上 直接改变对话显示内容
						this->scheduleOnce(schedule_selector(UIMain::chatFadeIn), 0.5);
					}
				}
				break;
			}
		}
	/*case POPULAR_ITEM:
	{
		//不敌对或者苏丹国家用了道具 副官提示流行品
		if ((m_pCityDataResult->friendvalue >= 0 || (SINGLE_SHOP->getCitiesInfo()[m_pCityDataResult->data->lastcity->cityid].nation == 8 && m_pCityDataResult->hassudanmask)) && m_pCityDataResult->n_populargoodsids)
		{
			if (!m_bIsNeutralCityChange)
			{
				if (m_bisFirstShow)
				{
					log("POPULAR_ITEM guard enter");
					m_bisFirstShow = false;
					m_nLastGuardTag = 1;
					m_nGuardIndex = POPULAR_ITEM;
					guardEnterScene(0);
				}
				else
				{
					if (m_nLastGuardTag != 1)
					{
						m_nLastGuardTag = 1;
					}
					if (m_bPopItemFlag)
					{
						log("POPULAR_ITEM guard fadeout fadein");
						m_nGuardIndex = POPULAR_ITEM;
						if (i_soldier->getPositionX() < 0)
						{
							guardEnterScene(0);
						}
						else if (strcmp(t_guard->getString().c_str(), SINGLE_SHOP->getTipsInfo()["TIP_MAIN_GUARD"].c_str()) == 0)
						{
							guardExitScene(0);
							this->scheduleOnce(schedule_selector(UIMain::guardEnterScene), 1.2);
						}
						else
						{
							chatFadeOut(0);
							//副官已在舞台上 直接改变对话显示内容
							this->scheduleOnce(schedule_selector(UIMain::chatFadeIn), 0.5);
						}
					}
				}
			}
			break;
		}
	}*/
	case GAME_EVENT:
		{
			//副官提醒
			if (false)
			{
				if (m_bisFirstShow)
				{
					//log("GAME_EVENT guard enter");
					m_nLastGuardTag = 1;
					m_bisFirstShow = false;
					m_nGuardIndex = GAME_EVENT;
					guardEnterScene(0);
				}else
				{
					m_nGuardIndex = GAME_EVENT;
					if (m_nLastGuardTag == 1)
					{
						log("GAME_EVENT guard fadeout fadein");
						if (i_soldier->getPositionX() < 0)
						{
							guardEnterScene(0);
						}
						else if (strcmp(t_guard->getString().c_str(), SINGLE_SHOP->getTipsInfo()["TIP_MAIN_GUARD"].c_str()) == 0)
						{
							guardExitScene(0);
							this->scheduleOnce(schedule_selector(UIMain::guardEnterScene), 1.2);
						}
						else
						{
							chatFadeOut(0);
							//副官已在舞台上 直接改变对话显示内容
							this->scheduleOnce(schedule_selector(UIMain::chatFadeIn), 0.5);
						}
					}else
					{
						log("GAME_EVENT guard fadeout fadein");
						m_nLastGuardTag = 1;
						if (i_soldier->getPositionX() < 0)
						{
							guardEnterScene(0);
						}
						else if (strcmp(t_guard->getString().c_str(), SINGLE_SHOP->getTipsInfo()["TIP_MAIN_GUARD"].c_str()) == 0)
						{
							guardExitScene(0);
							this->scheduleOnce(schedule_selector(UIMain::guardEnterScene), 1.2);
						}
						else
						{
							chatFadeOut(0);
							//副官已在舞台上 直接改变对话显示内容
							this->scheduleOnce(schedule_selector(UIMain::chatFadeIn), 0.5);
						}
					}
				}
				break;
			}
		}
	default:
		{
			m_bMoveActionFlag = false;
			if (i_soldier->getPositionX() >= 0)
			{
				log("default guard exit");
				m_bIsGuardExit = true;
				this->scheduleOnce(schedule_selector(UIMain::guardExitScene), 0);
				guard_dialog->runAction(Sequence::createWithTwoActions(DelayTime::create(1.0f),Place::create(ENDPOS)));
			}
			else
			{
				m_bIsGuardExit = true;
				auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
				auto i_soldier = guard_dialog->getChildByName<ImageView*>("image_soldier");
				i_soldier->runAction(Sequence::create(DelayTime::create(1), CallFunc::create(CC_CALLBACK_0(UIMain::checkBountyBoard, this)), nullptr));
			}
			notifyCompleted(FIRST_MATE);
			break;
		}
	}
}

void UIMain::guardEnterScene(const float fTime)
{
	m_bMoveActionFlag = false;
	auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
	guard_dialog->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
	guard_dialog->setPosition(STARTPOS);
	auto i_soldier = guard_dialog->getChildByName<ImageView*>("image_soldier");
	auto i_dialog = guard_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_guard = guard_dialog->getChildByName<Text*>("label_guard");
	auto t_content = guard_dialog->getChildByName<Text*>("label_content");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
	i_soldier->setPositionX(- i_soldier->getBoundingBox().size.width);
	i_soldier->setOpacity(255);
	i_dialog->setOpacity(0);
	t_guard->setOpacity(0);
	t_content->setOpacity(0);
	i_anchor->setOpacity(0);
	std::string path;
	
	if (m_nLastGuardTag == 2)
	{
		path = getNpcPath(m_pCityDataResult->data->lastcity->cityid,FLAG_SOLDIER);
	}else
	{
		path = getCheifIconPath();
	}
	i_soldier->ignoreContentAdaptWithSize(false);
	i_soldier->loadTexture(path);
	if (i_soldier->getPositionX()>=0)
	{
		return;
	}
	i_soldier->runAction(MoveTo::create(0.5,Vec2(0,0)));

	this->scheduleOnce(schedule_selector(UIMain::chatFadeIn), 0.5);
}

void UIMain::guardExitScene(const float fTime)
{
	//对话先消失 副官再退出屏幕
	this->scheduleOnce(schedule_selector(UIMain::chatFadeOut), 0);
	m_bMoveActionFlag = false;
	auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
	auto i_soldier = guard_dialog->getChildByName<ImageView*>("image_soldier");
	i_soldier->runAction(Sequence::create(DelayTime::create(0.5), MoveTo::create(0.5, Vec2(-i_soldier->getBoundingBox().size.width, 0)), CallFunc::create(CC_CALLBACK_0(UIMain::checkBountyBoard, this)), nullptr));
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
	i_anchor->stopAllActions();
}

void UIMain::chatFadeIn(const float fTime)
{
	m_bMoveActionFlag = false;
	auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
	auto i_dialog = guard_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_guard = guard_dialog->getChildByName<Text*>("label_guard");
	auto p_content = guard_dialog->getChildByName<Widget*>("panel_content");
	auto t_content = p_content->getChildByName<Text*>("label_content");
	anchPic = guard_dialog->getChildByName<ImageView*>("image_anchor");
	std::string content="";
	auto tips = SINGLE_SHOP->getTipsInfo();
	if (m_nGuardIndex == ENEMY_CITY_CHAT)
	{
		t_guard->setString(tips["TIP_MAIN_GUARD"]);
		if (SINGLE_HERO->m_iPrestigeLv < PRESTIGE_MIN)
		{
			content = tips["TIP_MAIN_ENEMY1"];
		}else if(SINGLE_HERO->m_iPrestigeLv < PRESTIGE_MID)
		{
			content = tips["TIP_MAIN_ENEMY2"];
		}else if(SINGLE_HERO->m_iPrestigeLv < PRESTIGE_MAX)
		{
			content = tips["TIP_MAIN_ENEMY3"];
		}else
		{
			content = tips["TIP_MAIN_ENEMY4"];
		}
	}else if (m_nGuardIndex == DISGUISE_CITY_CAHT)
	{
		t_guard->setString(tips["TIP_MAIN_GUARD"]);
		content = tips["TIP_MAIN_DISGUISE"];
	}else if (m_nGuardIndex == NEUTRAL_CHANGETO_ENEMY_CHAT)
	{
		if (SINGLE_HERO->m_iGender == 1)
		{
			t_guard->setString(tips["TIP_MAIN_FEMAIE_AIDE"]);
		}else
		{
			t_guard->setString(tips["TIP_MAIN_MAIE_AIDE"]);
		}
		content = tips["TIP_MAINCITY_ESCORT_CHAT"];
	}
	else if (m_nGuardIndex == GAME_NEWBIE_GUIDE_END1 || m_nGuardIndex == GAME_NEWBIE_GUIDE_END2)
	{
		if (SINGLE_HERO->m_iGender == 1)
		{
			t_guard->setString(tips["TIP_MAIN_FEMAIE_AIDE"]);
		}
		else
		{
			t_guard->setString(tips["TIP_MAIN_MAIE_AIDE"]);
		}
		std::string nation[] = { "", "PORTUGAL", "SPAIN", "ENGLAND", "NETHERLANDS", "GENOVA" };
		std::string tip_index;
		if (m_nGuardIndex == GAME_NEWBIE_GUIDE_END1)
		{
			tip_index = StringUtils::format("TIP_%s_GUIDE_END_FIRSTWORD", nation[SINGLE_HERO->m_iNation].c_str());
		}
		else
		{
			tip_index = StringUtils::format("TIP_%s_GUIDE_END_SECONDWORD", nation[SINGLE_HERO->m_iNation].c_str());
		}
		content = tips[tip_index];
	}
	else
	{
		if (SINGLE_HERO->m_iGender == 1)
		{
			t_guard->setString(tips["TIP_MAIN_FEMAIE_AIDE"]);
		}else
		{
			t_guard->setString(tips["TIP_MAIN_MAIE_AIDE"]);
		}
		
		if (m_nGuardIndex == BATTLE_NPC_FAIL)
		{
			content = tips["TIP_MAIN_BATTLE_NPC_FAIL"];
		}else
		{
			content = getChiefContent();
		}
	}

	std::string new_vaule = SINGLE_HERO->m_sName;
	std::string old_vaule = "[heroname]";
	repalce_all_ditinct(content,old_vaule,new_vaule);

	/*if (m_nGuardIndex == POPULAR_ITEM)
	{
		new_vaule = getCityItemName();
		old_vaule = "[goods]";
		repalce_all_ditinct(content, old_vaule, new_vaule);
	}*/
	p_content->setContentSize(Size(p_content->boundingBox().size.width, t_content->getFontSize() * 3));
	m_nToTalGuardPage = ceil(getLabelHight(content, t_content->getBoundingBox().size.width, t_content->getFontName()) / (t_content->getFontSize() * 3));
	m_nCurGuardPage = 1;
	t_content->setPosition(Vec2(0, t_content->getFontSize() * 3));
	//t_content->setString(content);
	i_dialog->runAction(Sequence::createWithTwoActions(FadeIn::create(0.5),CallFunc::create(CC_CALLBACK_0(UIMain::moveEnd,this))));
	t_guard->runAction(FadeIn::create(0.5));
	t_content->runAction(FadeIn::create(0.5));
	this->scheduleOnce(schedule_selector(UIMain::anchorAction),0.5);
	//显示主城对话
	chatContent = content;
	t_content->setString("");
	chatTxt = t_content;
	this->schedule(schedule_selector(UIMain::showCityGuardText), 0.1/3);
}

void UIMain::chatFadeOut(const float fTime)
{
	m_bMoveActionFlag = false;
	auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
	auto i_dialog = guard_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_guard = guard_dialog->getChildByName<Text*>("label_guard");
	auto t_content = guard_dialog->getChildByName<Text*>("label_content");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
	i_dialog->runAction(FadeOut::create(0.5));
	t_guard->runAction(FadeOut::create(0.5));
	t_content->runAction(FadeOut::create(0.5));
	i_anchor->runAction(FadeOut::create(0.5));
}

void UIMain::anchorAction(const float fTime)
{
	auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(255);
	i_anchor->setVisible(false);
	i_anchor->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5,Vec2(0,10))),EaseBackOut::create(MoveBy::create(0.5, Vec2(0,-10))))));
}

std::string UIMain::getChiefContent()
{
	auto tips = SINGLE_SHOP->getTipsInfo();
	std::string gender[] = {"","FEMAIE","MAIE"}; //男-女，女-男
	std::string nation[] = {"","PORTUGAL","SPAIN","ENGLAND","NETHERLANDS","GENOVA"};
	std::string guard[] = { "", "BATTLE_NPC_FAIL", "BATTLEN", "ENEMY", "DISGUISE", " ", "ITEM", "EVENT" };
	int n = cocos2d::random() % 3 + 1;
	/*if ( m_nGuardIndex == POPULAR_ITEM)
	{
		m_bisFirstShow = false;
		n = cocos2d::random() % 5 + 1;
	}*/
	std::string tip_index = StringUtils::format("TIP_MAIN_%s_%s_%s%d",nation[SINGLE_HERO->m_iNation].c_str(),gender[SINGLE_HERO->m_iGender].c_str(),guard[m_nGuardIndex].c_str(),n);
	std::string content = tips[tip_index];
	return content;
}

std::string UIMain::getCityItemName() {
	std::string content;
	int i = 0;
	int max = m_pCityDataResult->n_populargoodsids;
	for (; i < max; i++)
	{
		if (max == 1)
		{

		}
		else if (max == 2)
		{
			if (i == 1)
			{
				content.append(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_AND2"]);
			}
		}
		else
		{
			if (i > 0 && i < max - 1)
			{
				content.append(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_AND1"]);
			}
			else if (i > 0)
			{
				content.append(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_AND2"]);
			}
		}

		content.append(SINGLE_SHOP->getGoodsData()[m_pCityDataResult->populargoodsids[i]].name);
	}
	return content;
}

void UIMain::menuCall_func(Ref *pSender,Widget::TouchEventType touchType)
{
	if (Widget::TouchEventType::ENDED != touchType)
	{
		return;
	}
	Widget* widget = dynamic_cast<Widget*>(pSender);
	std::string tmp(widget->getName());
	callEventByName(tmp,widget);
}

void UIMain::callEventByName(std::string& name, Node* node)
{
	//对话
	if (isButton(panel_dialog)) 
	{
		if (!m_bMoveActionFlag)
		{
			return;
		}
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto guard_dialog = m_vRoot->getChildByName<Widget*>("panel_dialog");
		auto p_content = guard_dialog->getChildByName<Widget*>("panel_content");
		auto t_content = p_content->getChildByName<Text*>("label_content");
		if (!guardDiaolgOver)
		{
			t_content->setString(chatContent);
			this->unschedule(schedule_selector(UIMain::showCityGuardText));
			guardDiaolgOver = true;
			anchPic->setVisible(true);
		}
		else
		{
			if (m_nCurGuardPage < m_nToTalGuardPage)
			{
				m_nCurGuardPage++;
				t_content->runAction(Sequence::create(FadeOut::create(0.5), Place::create(Vec2(0, m_nCurGuardPage * t_content->getFontSize() * 3)),
					FadeIn::create(0.5), nullptr));
			}
			else if (m_nGuardIndex == DISGUISE_CITY_CAHT)
			{
				if (m_pCityDataResult->sudanmasknum)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYesOrNO("TIP_MAIN_USE_PROP1_TITLE", "TIP_MAIN_USE_PROP1");
				}
				else
				{
					m_nGuardIndex = GAME_EVENT;
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_MAIN_USE_PROP2");
				}
			}
			else if (m_nGuardIndex == GAME_NEWBIE_GUIDE_END1)
			{
				m_nGuardIndex = GAME_NEWBIE_GUIDE_END2;
				chatFadeOut(0);
				//副官已在舞台上 直接改变对话显示内容
				this->scheduleOnce(schedule_selector(UIMain::chatFadeIn), 0.5);
			}
			else
			{
				if (m_nGuardIndex == GAME_NEWBIE_GUIDE_END2)
				{
					m_bNewbieGuidePretask = true;
					ProtocolThread::GetInstance()->getPretaskItems(UILoadingIndicator::create(this));
				}
				t_content->stopAllActions();
				t_content->setOpacity(255);
				m_nGuardIndex++;
				openGuardChat(m_nGuardIndex);
			}
		}
	}
	//主城右上方控制按钮
	else if (isButton(button_unfold))
	{
		if (m_bShowAndHideAction)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			showAndHideCityButton();
		}
	}
	else if (isButton(button_Supply_yes))
	{
		   //补给界面的确定按钮
		   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			closeView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
			if (m_nFastHireCrewNum > 0)
			{
				ProtocolThread::GetInstance()->getCrew(m_nFastHireCrewNum, UILoadingIndicator::create(this), 1);
			}
			return;
	}
	//增加背包容量确认
	else if (isButton(button_s_yes))
	{
		if (m_bIsRepair)
		{
			if (SINGLE_HERO->m_iCoin >= m_nRepairCost)
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
				ProtocolThread::GetInstance()->repairAllShips(0, UILoadingIndicator::create(this));
			}
			else
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			}
		}
		else
		{
			// 运送任务提交反馈
			if (getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]))
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
				closeView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
				ProtocolThread::GetInstance()->handleTask(m_pCheckResult->sidetaskid, 4, UILoadingIndicator::create(this));
			}
			//增加背包容量确认
			else
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
				ProtocolThread::GetInstance()->expandPackageSize(UILoadingIndicator::createWithMask(this, 4));
			}
		}
		return;
	}
	//高级修理
	else if (isButton(button_v_yes))
	{
		if (SINGLE_HERO->m_iGold >= m_nRepairCost)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			ProtocolThread::GetInstance()->repairShipMaxHP(0, UILoadingIndicator::create(this));
		}
		else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openGoldNotEnoughYesOrNo(m_nRepairCost);
		}
		
		return;
	}
	else
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (isButton(button_coin))  //银币商城
		{
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 1);
			//openCountryWarPoster(0);
			return;
		}
		if (isButton(button_gold))	//V票商城
		{
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
			return;
		}
		if (isButton(button_market))  //交易所
		{
			if (SINGLE_HERO->m_iCurCityNation == 8)
			{
				if (m_pCityDataResult->hassudanmask){
					CHANGETO(SCENE_TAG::EXCHANG_TAG);
				}
				else{
					m_nGuardIndex = DISGUISE_CITY_CAHT;
					m_bisFirstShow = true;
					openGuardChat(DISGUISE_CITY_CAHT);
				}
			}
			else if (m_pCityDataResult->friendvalue < 0)
			{
				m_nGuardIndex = ENEMY_CITY_CHAT;
				m_bisFirstShow = true;
				openGuardChat(ENEMY_CITY_CHAT);
			}
			else if (m_bIsNeutralCityChange)
			{
				m_nGuardIndex = NEUTRAL_CHANGETO_ENEMY_CHAT;
				m_bisFirstShow = true;
				m_bPopItemFlag = false;
				openGuardChat(NEUTRAL_CHANGETO_ENEMY_CHAT);
			}
			else
			{
				CHANGETO(SCENE_TAG::EXCHANG_TAG);
			}
			return;
		}
		if (isButton(button_shipyard))  //船坞
		{
			if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
			{
				openSuccessOrFailDialog("TIP_VILLAGE_CANNOT_REPAIR_SHIP_CONTENT");
				return;
			}
			if (SINGLE_HERO->m_iCurCityNation == 8)
			{
				if (m_pCityDataResult->hassudanmask){
					CHANGETO(SCENE_TAG::DOCK_TAG);
				}
				else{
					m_nGuardIndex = DISGUISE_CITY_CAHT;
					m_bisFirstShow = true;
					openGuardChat(DISGUISE_CITY_CAHT);
				}
			}
			else if (m_pCityDataResult->friendvalue < 0)
			{
				m_nGuardIndex = ENEMY_CITY_CHAT;
				m_bisFirstShow = true;
				openGuardChat(ENEMY_CITY_CHAT);
			}
			else if (m_bIsNeutralCityChange)
			{
				m_nGuardIndex = NEUTRAL_CHANGETO_ENEMY_CHAT;
				m_bisFirstShow = true;
				m_bPopItemFlag = false;
				openGuardChat(NEUTRAL_CHANGETO_ENEMY_CHAT);
			}
			else
			{
				CHANGETO(SCENE_TAG::DOCK_TAG);
			}
			return;
		}
		if (isButton(button_tavern))  //酒馆
		{
			CHANGETO(SCENE_TAG::PUPL_TAG);
			return;
		}
		if (isButton(button_palace) || isButton(button_cityhall)) //王宫
		{
			if (SINGLE_HERO->m_iCurCityNation == 8)
			{
				if (m_pCityDataResult->hassudanmask){
					CHANGETO(SCENE_TAG::PALACE_TAG);
				}
				else{
					m_nGuardIndex = DISGUISE_CITY_CAHT;
					m_bisFirstShow = true;
					openGuardChat(DISGUISE_CITY_CAHT);
				}
			}
			else if (m_pCityDataResult->friendvalue < 0)
			{
				m_nGuardIndex = ENEMY_CITY_CHAT;
				m_bisFirstShow = true;
				openGuardChat(ENEMY_CITY_CHAT);
			}
			else if (m_bIsNeutralCityChange)
			{
				m_nGuardIndex = NEUTRAL_CHANGETO_ENEMY_CHAT;
				m_bisFirstShow = true;
				m_bPopItemFlag = false;
				openGuardChat(NEUTRAL_CHANGETO_ENEMY_CHAT);
			}
			else
			{
				CHANGETO(SCENE_TAG::PALACE_TAG);
			}
			return;
		}
		if (isButton(button_city_name_bg))  //城市信息
		{
			ProtocolThread::GetInstance()->getPalaceInfo(UILoadingIndicator::create(this));
			return;
		}
		if (isButton(button_bank) || isButton(image_hints_insurance)) //银行
		{
			if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
			{
				openSuccessOrFailDialog("TIP_VILLAGE_CANNOT_ENTER_BANK_HERE");
				return;
			}
			if (m_bIsNeutralCityChange)
			{
				m_nGuardIndex = NEUTRAL_CHANGETO_ENEMY_CHAT;
				m_bisFirstShow = true;
				m_bPopItemFlag = false;
				openGuardChat(NEUTRAL_CHANGETO_ENEMY_CHAT);
			}
			else
			{
				CHANGETO(SCENE_TAG::BANK_TAG);
			}
			return;
		}
		if (isButton(button_dock))  //港口
		{
			CHANGETO(SCENE_TAG::WHARF_TAG);
			return;
		}

		if (isButton(button_task)) //任务
		{
			UITask::getInstance()->openTaskLayer();
			return;
		}
		if (isButton(button_map)) //地图
		{
			UIMap *mainMap = UIMap::createMap(1);
			this->addChild(mainMap);
			return;
		}
		if (isButton(button_friend)) //好友
		{
			UISocial::getInstance()->openSocialLayer();
			UISocial::getInstance()->setFirendCall();
			UISocial::getInstance()->showChat(1);
			return;
		}
		if (isButton(button_guild)) //公会
		{
			UIGuild::getInstance()->openGuildLayer();
			return;
		}
		if (isButton(button_shop))	//商城
		{
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
			return;
		}

		if (isButton(button_chat)) //聊天室
		{
			UISocial::getInstance()->openSocialLayer();
			UISocial::getInstance()->setChatCall();
			UISocial::getInstance()->showChat();
			return;
		}
		if (isButton(button_mail))	//邮件
		{
			UIEmail::getInstance()->openEmailView();
			return;
		}
		if (isButton(button_set)) //设置
		{
			UISetting::getInstance()->openSettingLayer();
			return;
		}
		if (isButton(button_confirm_no)) //取消
		{
			switch (m_nTipImageIndex)
			{
			case IMAGE_TIP_LACK_SAILORS:
			{
					break;
			}
			case IMAGE_TIP_LACK_SHIP_DURE:
			{
					break;
			}
			default:
			{
					   m_nGuardIndex = GAME_EVENT;
					   openGuardChat(m_nGuardIndex);
					   break;
			}		
		}			
			return;
		}

		if (isButton(button_confirm_yes)) //确定
		{
			switch (m_nTipImageIndex)
			{
				case IMAGE_TIP_LACK_SAILORS:  //水手不足
				{
						break;
				}
				case IMAGE_TIP_LACK_SHIP_DURE:  //船只损坏
				{
						//v票不足
						UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
						break;
				}
				default:
				{
						   if (m_pCityDataResult->sudanmasknum)
						   {
							   ProtocolThread::GetInstance()->useSudanMask(m_pCityDataResult->sudanmaskid, UILoadingIndicator::create(this));
						   }
						   else
						   {
							   m_nGuardIndex = GAME_EVENT;
							   openGuardChat(m_nGuardIndex);
						   }
						   break;
				}			
			}		
			return;
		}

		if (isButton(image_head_player_bg)) //个人中心
		{
			CHANGETO(SCENE_TAG::CENTER_TAG);
		}

		if (isButton(button_libao))
		{
			UIPackageDeals*giftlayer = UIPackageDeals::createGiftLayer();
			this->addChild(giftlayer);
			return;
		}
		if (isButton(button_close)) //关闭
		{
			closeView(CITYINFO_RES[CITYINFO_CSB]);
			closeView(GIFTPACK_RES[LOTTERY_CSB]);
			closeView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			closeView(GIFTPACK_RES[BONUS_CSB]);
			closeView(GIFTPACK_RES[COUNTRY_WAR_PREPARE_POSTER]);
			m_nTipImageIndex = IMAGE_TIP_NONE;
			return;
		}
		if (isButton(button_good_bg))  //物品详情
		{
			int popularItemId = node->getTag();
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushGoodsDetail(nullptr, popularItemId, true);
			return;
		}
		if (isButton(button_escort_yes))//护送服务确认框
		{
			closeView(COMMOM_COCOS_RES[C_VIEW_RESULTTEXT_CSB]);
			return;
		}
		if (isButton(button_village_cityhall))//村庄王宫
		{
			openSuccessOrFailDialog("TIP_MAIN_VILLAGE_NOT_OPEN");
			return;

		}
		if (isButton(button_village_tavern))//村庄市酒馆
		{
			CHANGETO(SCENE_TAG::PUPL_TAG);
			return;
		}
		//账户验证按钮
		if (isButton(button_verify))
		{
			auto vertifylayer = UIVerifyAccount::createVertifyLayer();
			this->addChild(vertifylayer);
			return;
		}

		if (isButton(button_activity_bases))
		{
			ProtocolThread::GetInstance()->getPirateAttackInfo(UILoadingIndicator::create(this));
			return;
		}

		if (isButton(button_activity_pirate))
		{
			UITask::getInstance()->openTaskLayer(1);
			return;
		}

		if (isButton(button_activity_lotto))
		{
			ProtocolThread::GetInstance()->obtainLottoMessage(UILoadingIndicator::create(this));
			return;
		}
		//运送任务提交
		if (isButton(button_board))
		{
			openView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB], 11);
			auto w_taskConfirm = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
			auto t_title = w_taskConfirm->getChildByName<Text*>("label_repair");
			auto t_content = w_taskConfirm->getChildByName<Text*>("label_content");
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_DELIVER_GOODS_TASK_COMMIT_TITLE"]);
	
			std::string content;
			std::string new_vaule;
			std::string old_vaule;
			content = SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_DELIVER_GOODS_TASK_COMMIT_CONTENT"];
			new_vaule = SINGLE_SHOP->getGoodsData()[m_pCheckResult->sidetaskneedgoodsid].name;
			old_vaule = "[goods_name]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = StringUtils::format("%d", m_pCheckResult->sidetaskneednum + m_pCheckResult->sidetaskoffernum);
			old_vaule = "[num]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			t_content->setString(content);
			return;
		}
		if (isButton(button_s_no))
		{
			closeView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
			return;
		}
		//领取facebook被邀请奖励
		if (isButton(button_invent))
		{
			openGetFaceBookReward();
			return;
		}
		//领取奖励
		if (isButton(button_facebook_invited_reward))
		{
			ProtocolThread::GetInstance()->getInviteBonus(UILoadingIndicator::create(this));
			closeView();
			return;
		}
		if (isButton(button_invite_others))
		{
			closeView();
			UISocial::getInstance()->openSocialLayer();
			UISocial::getInstance()->setFaceBookInvite();
			return;
		}
		//首充v活动界面
		if (isButton(button_bonus))
		{
			openVActivityBonus();
			return;
		}
		//首充v活动界面现在充值按钮
		if (isButton(button_recharge_now))
		{
			closeView();
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
			return;
		}
		//关闭界面
		if (isButton(button_facebook_close) || isButton(button_facebook_reward_close))
		{
			closeView();
			return;
		}
		
		//图鉴
		if (isButton(button_handbook))
		{
			auto layer = UIHandBook::create();
			this->addChild(layer);
			return;
		}
		//小伙伴
		if (isButton(button_companions))
		{
			if (!SINGLE_HERO->m_bClickTouched)
			{
				SINGLE_HERO->m_bClickTouched = true;
				auto cp = UICompanions::createCompanionLayer();
				this->addChild(cp);
			}
			return;
		}
		//客服
		if (isButton(button_customerservice))
		{
			std::string  emailContent = ProtocolThread::GetInstance()->getLoginEmail();
			Utils::ShowFreshDesk(emailContent, SINGLE_HERO->m_sName);
			return;
		}

		if (isButton(b_result_yes))
		{
			closeView();
			UICommon::getInstance()->closeView();
			notifyCompleted(REDUCE_NOTICE);
			return;
		}

		if (isButton(button_result_yes))
		{
			closeView();
			UICommon::getInstance()->closeView();
			return;
		}
		//船坞损坏的提示
		if (isButton(image_hints_dua))
		{
			m_nTipImageIndex = IMAGE_TIP_LACK_SHIP_DURE;
			/*
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_MAINCITY_SHIP_DAMAGE_TIP_TITLE", "TIP_MAINCITY_SHIP_DAMAGE_TIP_CONTENT");
			*/
			openView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			auto btn_close = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_close"));
			btn_close->setVisible(true);
			std::string text = SINGLE_SHOP->getTipsInfo()[name];
			auto label_content = view->getChildByName<Text*>("label_content");
			label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_SHIP_DAMAGE"]);
			auto b_game_recc = view->getChildByName<Button*>("button_game_reconn");
			auto b_game_exit = view->getChildByName<Button*>("button_game_exit");
			b_game_recc->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_SHIP_DAMAGE_GO_SHIPYARD"]);
			b_game_exit->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_SHIP_DAMAGE_REPAIR"]);
			b_game_recc->setName("button_go_shipyard");
			b_game_exit->setName("button_ship_repair");
			return;
		}
		//水手不足的提示
		if (isButton(image_hints_sailor))
		{
			openView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			auto btn_close = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_close"));
			btn_close->setVisible(true);
			std::string text = SINGLE_SHOP->getTipsInfo()[name];
			auto label_content = view->getChildByName<Text*>("label_content");
			label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_SAILOR_HIRE_TIP_CONTENT"]);
			auto i_game_recc = view->getChildByName<Button*>("button_game_reconn");
			auto i_game_exit = view->getChildByName<Button*>("button_game_exit");
			i_game_recc->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_SAILOR_GOTO_TARVEN_CONTENT"]);
			i_game_exit->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_SAILOR_QUICH_HIRE_BUTTON_CONTENT"]);
			return;
		}
		//快速雇佣
		if (isButton(button_game_exit))
		{
			m_nTipImageIndex = IMAGE_TIP_LACK_SAILORS;
			ProtocolThread::GetInstance()->calFastHireCrew(UILoadingIndicator::create(this));
			closeView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			return;
		}
		//去酒馆
		if (isButton(button_game_reconn))
		{
			closeView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			CHANGETO(SCENE_TAG::PUPL_TAG);
			return;
		}
		//关闭快速补给界面
		if (isButton(button_Supply_no))
		{
			closeView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
			return;
		}
		//掉落服务
		if (isButton(image_dorp_service))
		{
			openView(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
			auto view = getViewRoot(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
			std::string st_content = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_DROPS_TIME"];
			int h = m_pCityDataResult->drop_rate_increase_ramian_time / 3600;
			int m = (m_pCityDataResult->drop_rate_increase_ramian_time % 3600) / 60;
			int s = m_pCityDataResult->drop_rate_increase_ramian_time % 60;
			std::string old_value = "[time]";
			std::string new_value = StringUtils::format("%02d:%02d:%02d", h, m, s);
			repalce_all_ditinct(st_content, old_value, new_value);
			auto t_content = dynamic_cast<Text*>(view->getChildByName("label_dropitem_tiptext"));
			t_content->setString(st_content);
			return;
		}
		//掉落服务
		if (isButton(button_error_yes))
		{
			closeView(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
			return;
		}
		if (isButton(button_port_info))
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushInfoBtnView(getPortTypeOrInfo(2).c_str(), getPortTypeOrInfo(1).c_str());
			return;
		}

		//个人背包超重按钮
		if (isButton(image_bag_over_weight))
		{
			openView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			std::string text = SINGLE_SHOP->getTipsInfo()["TIP_BAG_OVER_WEIGHT_CONTETN"];
			auto label_content = view->getChildByName<Text*>("label_content");
			auto b_close = view->getChildByName<Button*>("button_close");
			b_close->setVisible(true);
			label_content->setString(text);
			auto i_game_recc = view->getChildByName<Button*>("button_game_reconn");
			auto i_game_exit = view->getChildByName<Button*>("button_game_exit");
			i_game_recc->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_BAG_OVER_WEIGHT_RIGHT"]);
			i_game_exit->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_BAG_OVER_WEIGHT_LEFT"]);
			i_game_recc->setName("button_over_weight_right");
			i_game_exit->setName("button_over_weight_life");
			return;
		}
		//个人背包扩容
		if (isButton(button_over_weight_right))
		{
			auto bag_info = SINGLE_SHOP->getBagExpandFeeInfo();
			size_t i = 0;
			for (; i < bag_info.size(); i++)
			{
				if (bag_info[i].capacity == m_pCityDataResult->max_package_size)
				{
					break;
				}
			}
			if (i == bag_info.size() - 1)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_ITEM_BAG_EXPAND_MAX");
			}
			else
			{
				int cost_num = bag_info[i + 1].fee;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->flushSilverConfirmView("TIP_CENTER_ITEM_BAG_EXPAND_TITLE",
					"TIP_CENTER_ITEM_BAG_EXPAND_CONTENT", cost_num);
			}
			closeView();
			return;
		}
		//进入个人中心物品界面
		if (isButton(button_over_weight_life))
		{
			CHANGETO(SCENE_TAG::CENTER_TAG);
			return;
		}
	}
	if (isButton(image_city_population) || isButton(image_city_trade) || isButton(image_city_manufacture) || isButton(image_friendliness_status))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(dynamic_cast<Widget*>(node));
		return;
	}
	//进入主城国战
	if (isButton(button_war))
	{
		ProtocolThread::GetInstance()->getCityPrepareStatus(UILoadingIndicator::create(this));
		return;
	}
	//活动列表的小按钮
	if (isButton(button_events))
	{
		auto listview_event = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_vRoot, "listview_event"));
		if (listview_event)
		{
			listview_event->scrollToBottom(0.5f, true);
		}
	}
	//海报进入国战
	if (isButton(button_enterfort))
	{
		UINationWarEntrance*pirateLayer = UINationWarEntrance::createCountryWarLayer();
		Scene* scene = Scene::createWithPhysics();
		scene->addChild(pirateLayer);
		Director::getInstance()->replaceScene(scene);
	}
	//进入船务
	if (isButton(button_go_shipyard))
	{
		m_nTipImageIndex = IMAGE_TIP_NONE;
		closeView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
		if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
		{
			openSuccessOrFailDialog("TIP_VILLAGE_CANNOT_REPAIR_SHIP_CONTENT");
			return;
		}
		if (SINGLE_HERO->m_iCurCityNation == 8)
		{
			if (m_pCityDataResult->hassudanmask){
				SINGLE_HERO->m_bGoshipFixDirect = true;
				CHANGETO(SCENE_TAG::DOCK_TAG);
			}
			else{
				m_nGuardIndex = DISGUISE_CITY_CAHT;
				m_bisFirstShow = true;
				openGuardChat(DISGUISE_CITY_CAHT);
			}
		}
		else if (m_pCityDataResult->friendvalue < 0)
		{
			m_nGuardIndex = ENEMY_CITY_CHAT;
			m_bisFirstShow = true;
			openGuardChat(ENEMY_CITY_CHAT);
		}
		else if (m_bIsNeutralCityChange)
		{
			m_nGuardIndex = NEUTRAL_CHANGETO_ENEMY_CHAT;
			m_bisFirstShow = true;
			m_bPopItemFlag = false;
			openGuardChat(NEUTRAL_CHANGETO_ENEMY_CHAT);
		}
		else
		{
			SINGLE_HERO->m_bGoshipFixDirect = true;
			CHANGETO(SCENE_TAG::DOCK_TAG);
		}
		return;
	}

	//快速修理
	if (isButton(button_ship_repair))
	{
		if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5 ||
			(SINGLE_HERO->m_iCurCityNation == 8 && m_pCityDataResult->hassudanmask == 0)
			|| (SINGLE_HERO->m_iCurCityNation != 8 && m_pCityDataResult->friendvalue < 0) || m_bIsNeutralCityChange)
		{
			m_nTipImageIndex = IMAGE_TIP_NONE;
			closeView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_MAIN_NOT_QUICK_REPAIR");
		}
		else 
		{
			ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::create(this));
		}
		return;
	}
}

void UIMain::onServerEvent(struct ProtobufCMessage *message,int msgType)
{
	if(!m_vRoot){
		initRes(0);
	}
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
	case PROTO_TYPE_CheckMailBoxResult:
		{
			CheckMailBoxResult *result = (CheckMailBoxResult*)message;
			if (result->failed == 0)
			{
				m_pCheckResult = result;
				m_bReceive = result->daily_activity_finish;
				m_bClifford = result->can_pray;
				if (result->authenticated)
				{
					SINGLE_HERO->m_iMyEmailAccountVerified = true;
				}
				else
				{
					SINGLE_HERO->m_iMyEmailAccountVerified = false;
				}
				//TODO: a work around to fix it by checking city data result as it depends on the m_pCityDataResult. This MailCheck will be executed every few seconds, so next time, user will see the right result.
				if(m_pCityDataResult){
					getAllUnreadImage(result);
					showInsuranceHints(result);
				}
			}
			break;
		}
	case PROTO_TYPE_GetUserTasksResult:
		{	
			GetUserTasksResult *result = (GetUserTasksResult*)message;
			if (result->failed == 0)
			{
				m_nCompletedAllTasks = result->completedalltasks;
				m_nCurMainTaskComplete = result->maintaskstatus;
				m_pCurTaskDefine = result->sidetask;
				getMainTaskUnreadImage();
				//更新邮件通知
				updateMailNumber(0);
			}
			break;
		}
	case PROTO_TYPE_GetPretaskItemsResult:
		{
			GetPretaskItemsResult *preTask = (GetPretaskItemsResult*)message;
			if (preTask->failed == 0 && m_bNewbieGuidePretask)
			{
				m_bNewbieGuidePretask = false;
				if (preTask->addcoin > 0 || preTask->bank > 0 || preTask->n_items > 0)
				{
					getNpcPresentItem(preTask);
				}
			}
			break;
		}
	case PROTO_TYPE_GetCurrentCityDataResult:
		{
			//log(" tracking 1 %d", Utils::getnanotime());
			GetCurrentCityDataResult *result = (GetCurrentCityDataResult*)message;
			if (result->failed == 0)
			{
				m_pCityDataResult = result;
				if (m_pCityDataResult->current_city_nation)
				{
					SINGLE_HERO->m_iCurCityNation = m_pCityDataResult->current_city_nation;
				}
				else
				{
					SINGLE_HERO->m_iCurCityNation = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].nation;
				}
				if (isUsePropBackToCity)
				{
					//使用道具回城 重新检测流行品提示
					isUsePropBackToCity = false;
					isNewLanding = 1;
				}
				else
				{
					isNewLanding = m_pCityDataResult->isnewlanding;
				}
				SINGLE_HERO->m_heroIsOnsea = false;
				//等级礼包
				if (result->onsale_remain_time)
				{
					showGiftPackBg(true);
					showLimitPackTime(0);
					this->schedule(schedule_selector(UIMain::showLimitPackTime), 1.0f);
				}
				else
				{
					showGiftPackBg(false);

				}
				//账户验证按钮
				if (result->authenticated)
				{
					SINGLE_HERO->m_iMyEmailAccountVerified = true;
					//是否领取奖励
					if (m_pCityDataResult->getemailverifiedreward)
					{
						showVerify(false);
					}
					else
					{
						showVerify(true);
					}
				}
				else
				{
					SINGLE_HERO->m_iMyEmailAccountVerified = false;
					showVerify(true);
				}
				
				SINGLE_HERO->m_nTriggerId = result->dialogid;

				showBarData();

				//显示船只提醒
				buttonImageNotifyShow();
				//显示水手提醒
				showSailorOrShipDur();
				ProtocolThread::GetInstance()->getUserTasks(/*UILoading::create(this)*/);
			}
			else
			{
				openSuccessOrFailDialog("TIP_MAIN_GET_CITY_FAIL");
				//ProtocolThread::GetInstance()->getCurrentCityData(UILoading::create(this));
			}
			break;
		}
	case PROTO_TYPE_ReduceInNationWarResult:
	{
		ReduceInNationWarResult *result = (ReduceInNationWarResult *)message;
		if (result->failed == 0 && result->n_info > 0)
		{
			SINGLE_HERO->m_iPrestigeLv = FAME_NUM_TO_LEVEL(result->fames);
			flushCionAndGold(result->coins, result->golds);
			flushExpAndRepLv();

			openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
			auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
			auto b_result_yes = view->getChildByName<Button*>("button_result_yes");
			b_result_yes->setName("b_result_yes");
			b_result_yes->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));

			auto t_title = view->getChildByName<Text*>("label_title");
			auto p_result = view->getChildByName<Widget*>("panel_result");
			auto l_result = p_result->getChildByName<ListView*>("listview_result");
			auto panel_war_r = p_result->getChildByName<Widget*>("panel_war_r");
			auto p_sailor = p_result->getChildByName<Widget*>("panel_silver_2");
			auto i_div = p_result->getChildByName<ImageView*>("image_div_1");

			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_END_LOSS_TITLE"]);
			
			for (int i = 0; i < result->n_info; i++)
			{
				rapidjson::Document root;
				root.Parse<0>(result->info[i]->reason);
				int type = 0;
				int city_id = 0;
				//type = 0时，表示国战声望倒退
				type = DictionaryHelper::getInstance()->getIntValue_json(root, "type");
				city_id = DictionaryHelper::getInstance()->getIntValue_json(root, "city_id");
				std::string content;
				if (city_id > 0)
				{
					content = SINGLE_SHOP->getCitiesInfo()[city_id].name;
				}
				if (type == 1)
				{
					content += SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_STATUS_CITYNAME_PLUS_FALLEN"];
				}
				content += ":";

				if (result->info[i]->reduce_fames)
				{
					auto p_fames_reduce = p_sailor->clone();
					l_result->pushBackCustomItem(p_fames_reduce);
					auto t_label = p_fames_reduce->getChildByName<Text*>("label_force_relation");
					auto t_num = p_fames_reduce->getChildByName<Text*>("label_buy_num");
					auto i_silver = p_fames_reduce->getChildByName<ImageView*>("image_silver");
					
					t_label->setString(content);
					t_num->setString(StringUtils::format("%lld", result->info[i]->reduce_fames));
					i_silver->loadTexture("cocosstudio/login_ui/common/reputation_2.png");
					i_silver->setPositionX(t_num->getPositionX() - t_num->getBoundingBox().size.width - i_silver->getContentSize().width/2 - 3);
				}
				if (result->info[i]->reduce_coins)
				{
					auto p_coins_reduce = p_sailor->clone();
					l_result->pushBackCustomItem(p_coins_reduce);
					auto t_label = p_coins_reduce->getChildByName<Text*>("label_force_relation");
					auto t_num = p_coins_reduce->getChildByName<Text*>("label_buy_num");
					auto i_silver = p_coins_reduce->getChildByName<ImageView*>("image_silver");
					
					t_label->setString(content);
					t_num->setString(StringUtils::format("%lld", result->info[i]->reduce_coins));
					i_silver->loadTexture("cocosstudio/login_ui/silver.png");
					i_silver->setPositionX(t_num->getPositionX() - t_num->getBoundingBox().size.width - i_silver->getContentSize().width / 2 - 3);
				}
				if (result->info[i]->reduce_golds)
				{
					auto p_golds_reduce = p_sailor->clone();
					l_result->pushBackCustomItem(p_golds_reduce);
					auto t_label = p_golds_reduce->getChildByName<Text*>("label_force_relation");
					auto t_num = p_golds_reduce->getChildByName<Text*>("label_buy_num");
					auto i_silver = p_golds_reduce->getChildByName<ImageView*>("image_silver");
					
					t_label->setString(content);
					t_num->setString(StringUtils::format("%lld", result->info[i]->reduce_golds));
					i_silver->loadTexture("cocosstudio/login_ui/v_ticket.png");
					i_silver->setPositionX(t_num->getPositionX() - t_num->getBoundingBox().size.width - i_silver->getContentSize().width / 2 - 3);
				}
				if (result->info[i]->reduce_sailor_coins)
				{
					auto p_sailor_coins_reduce = p_sailor->clone();
					l_result->pushBackCustomItem(p_sailor_coins_reduce);
					auto t_label = p_sailor_coins_reduce->getChildByName<Text*>("label_force_relation");
					auto t_num = p_sailor_coins_reduce->getChildByName<Text*>("label_buy_num");
					auto i_silver = p_sailor_coins_reduce->getChildByName<ImageView*>("image_silver");
					
					t_label->setString(content);
					t_num->setString(StringUtils::format("%lld", result->info[i]->reduce_sailor_coins));
					i_silver->loadTexture("cocosstudio/login_ui/activity_720/ic_sailor.png");
					i_silver->setPositionX(t_num->getPositionX() - t_num->getBoundingBox().size.width - i_silver->getContentSize().width / 2 - 3);
				}
			}

		}
		else
		{
			notifyCompleted(REDUCE_NOTICE);
		}
		break;
	}
	case PROTO_TYPE_UseSudanMaskResult:
		{
			UseSudanMaskResult* result = (UseSudanMaskResult*)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_MAIN_USE_PROP_SUCCESS");
				setMainLayerButton(true);
				m_pCityDataResult->hassudanmask = 1;
				openGuardChat(BATTLE_NPC_FAIL);
			}
			else
			{
				ProtocolThread::GetInstance()->useSudanMask(m_pCityDataResult->sudanmaskid, UILoadingIndicator::create(this));
			}
			break;
		}
	case PROTO_TYPE_GetPalaceInfoResult:
		{
			GetPalaceInfoResult *result = (GetPalaceInfoResult *)message;
			if (result->failed == 0)
			{
				m_palaceInfoResult = result;
				if (result->nationidx)
				{
					SINGLE_HERO->m_iCurCityNation = result->nationidx;
				}
				else
				{
					SINGLE_HERO->m_iCurCityNation = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].nation;
				}
				ProtocolThread::GetInstance()->getCityProduces(UILoadingIndicator::create(this));
			}
			break;
		}
	case PROTO_TYPE_GetPirateAttackInfoResult:
		{
			GetPirateAttackInfoResult *result = (GetPirateAttackInfoResult *)message;
			if (result->failed == 0)
			{
				UIPriate*pirateLayer = new UIPriate();
				Scene* scene = Scene::createWithPhysics();
				scene->addChild(pirateLayer);
				pirateLayer->init(nullptr, result);
				Director::getInstance()->replaceScene(scene);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_ACTIVITY_NOT_STARTE");
			}
			break;
		}
	case PROTO_TYPE_ObtainLottoMessageResult:
		{
			auto result = (ObtainLottoMessageResult *)message;
			if (result->failed == 0)
			{
				for (size_t i = 0; i < result->n_lottomsg; i++)
				{
					auto lottomsg = result->lottomsg[i];
					//本周的
					if (lottomsg->type == 0)
					{
						openView(GIFTPACK_RES[LOTTERY_CSB]);
						auto view = getViewRoot(GIFTPACK_RES[LOTTERY_CSB]);
						auto p_time = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_lotto_time"));
						auto p_req = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_requirement"));
						auto p_prize = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_prize"));
						auto t_time = p_time->getChildByName<Text*>("label_content_2");
						auto t_req = p_req->getChildByName<Text*>("label_content_2");
						auto t_prize = p_prize->getChildByName<Text*>("label_content_2");
						if (lottomsg->time_zone >= 0)
						{
							t_time->setString(StringUtils::format("%s (GMT+%d)", SINGLE_SHOP->getTipsInfo()["TIP_CITY_LOTTO_TIME"].c_str(),lottomsg->time_zone));
						}
						else
						{
							t_time->setString(StringUtils::format("%s (GMT-%d)", SINGLE_SHOP->getTipsInfo()["TIP_CITY_LOTTO_TIME"].c_str(), -lottomsg->time_zone));
						}
						t_req->setString(lottomsg->require_text);
						t_prize->setString(lottomsg->reward_text);
						m_pCityDataResult->showlottomessage = 1;
 						auto panel_activity_lotto = Helper::seekWidgetByName(m_vRoot, "panel_activity_lotto");
 						panel_activity_lotto->setVisible(false);
						break;
					}
				}
			}
			break;
		}
	case PROTO_TYPE_GetInviteBonusResult:
	{
						
			GetInviteBonusResult *result = (GetInviteBonusResult *)message;
			if (result->failed ==0)
			{
				SINGLE_HERO->m_iCoin = result->coins;
				SINGLE_HERO->m_iGold = result->golds;
				flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
				showFaceBookInvitedHints(false);
				openFacebookInviteOthers();
				m_pCityDataResult->haveinvitebonus = 0;
			}
			else
			{

			}
			break;
	}
	case PROTO_TYPE_GetCityProducesResult:
	{
			 GetCityProducesResult *result = (GetCityProducesResult*)message;
			if (result->failed == 0)
			{
				m_producesResult = result;
				ProtocolThread::GetInstance()->getFriendValue(1, UILoadingIndicator::create(this));
			}
			break;
	}
	case PROTO_TYPE_HandleTaskResult:
	{
			HandleTaskResult *result = (HandleTaskResult*)message;
			Widget* escortResultPanel;
			Text* t_title;
			Text* t_content;
			if (result->failed == 200 || result->failed == 201 || result->failed == 202)
			{
				openView(COMMOM_COCOS_RES[C_VIEW_RESULTTEXT_CSB]);
				escortResultPanel = getViewRoot(COMMOM_COCOS_RES[C_VIEW_RESULTTEXT_CSB]);
				escortResultPanel->setLocalZOrder(100);
				t_title = escortResultPanel->getChildByName<Text*>("label_result");
				t_content = escortResultPanel->getChildByName<Text*>("label_content");
			}
			std::string content;
			std::string new_vaule;
			std::string old_vaule;
			auto image_task = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_task"));
			switch (result->failed)
			{
			case 200:
				//运货任务目前玩家没有该物品
				t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_DELIVER_GOODS_TASK_COMMIT_FEEDBACK_TITLE_FAILED"]);

				content = SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_DELIVER_GOODS_TASK_COMMIT_FEEDBACK1"];
				new_vaule = SINGLE_SHOP->getGoodsData()[m_pCheckResult->sidetaskneedgoodsid].name;
				old_vaule = "[goods_name]";
				repalce_all_ditinct(content, old_vaule, new_vaule);
				new_vaule = StringUtils::format("%d", m_pCheckResult->sidetaskneednum);
				old_vaule = "[num]";
				repalce_all_ditinct(content, old_vaule, new_vaule);
				t_content->setString(content);
				break;
			case 201:
				//运货任务目前玩家全部交付该物品
				t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_DELIVER_GOODS_TASK_COMMIT_FEEDBACK_TITLE_SUCCEED"]);

				content = SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_DELIVER_GOODS_TASK_COMMIT_FEEDBACK3"];
				new_vaule = SINGLE_SHOP->getCitiesInfo()[m_pCurTaskDefine->completecityid].name;
				old_vaule = "[cityname]";
				repalce_all_ditinct(content, old_vaule, new_vaule);
				t_content->setString(content);
				image_task->setVisible(false);
				break;
			case 202:
				//运货任务目前玩家交付一部分该物品
				t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_DELIVER_GOODS_TASK_COMMIT_FEEDBACK_TITLE_FAILED"]);

				content = SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_DELIVER_GOODS_TASK_COMMIT_FEEDBACK2"];
				new_vaule = SINGLE_SHOP->getGoodsData()[m_pCheckResult->sidetaskneedgoodsid].name;
				old_vaule = "[goods_name]";
				repalce_all_ditinct(content, old_vaule, new_vaule);
				new_vaule = StringUtils::format("%d", result->task->completeamount);
				old_vaule = "[num1]";
				repalce_all_ditinct(content, old_vaule, new_vaule);
				new_vaule = StringUtils::format("%d", SINGLE_SHOP->getCoreTaskById(m_pCurTaskDefine->taskcoreid).finish_goods_amount[m_pCurTaskDefine->npcindex] - result->task->completeamount);
				old_vaule = "[num2]";
				repalce_all_ditinct(content, old_vaule, new_vaule);
				t_content->setString(content);
				updateMailNumber(0);
				break;
			default:
				break;
			}
			break;
	}
	case PROTO_TYPE_GetCrewResult:
	{
			GetCrewResult *result = (GetCrewResult *)message;
			if (result->failed == 0)
			{
				SINGLE_HERO->m_iCoin = result->coin;
				SINGLE_HERO->m_iGold = result->gold;
				auto i_hints_sailor = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_hints_sailor"));
				int max_crewNum = 0;
				int require_crewNum = 0;
				if (i_hints_sailor)
				{
					for (int i = 0; i < result->n_shipcrew; i++)
					{
						require_crewNum += result->shipcrew[i]->crewrequire;
						max_crewNum += result->shipcrew[i]->crewmax;
					}
					if (max_crewNum - result->needcrewnum >= require_crewNum)
					{
						i_hints_sailor->setVisible(false);
						m_pCityDataResult->needaddsailors = 0;
					}
					else
					{
						i_hints_sailor->setVisible(true);
						i_hints_sailor->loadTexture("cocosstudio/login_ui/common/hints_sailor_1.png");
						if (max_crewNum - result->needcrewnum <= max_crewNum*0.1)
						{
							i_hints_sailor->loadTexture("cocosstudio/login_ui/common/hints_sailor_2.png");
						}
					}
				}
				flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_PUP_SAILOR_RECRUIT_SUCCESS");

			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PUP_BUY_CREW_FAIL");
			}
			break;
	}
	case PROTO_TYPE_CalFastHireCrewResult:
	{
				CalFastHireCrewResult *result = (CalFastHireCrewResult *)message;
				if (result->failed == 0)
				{
					m_pFastHireCrewResult = result;
					UIInform::getInstance()->openInformView(this);
					if (SINGLE_HERO->m_iCoin<1100 && SINGLE_HERO->m_iCoin>=600)
					{
						UIInform::getInstance()->openConfirmYes("TIP_MAINCITY_SAILOR_HIRE_COIN_VERY_LITTLE");
						break;
					}
					else if (SINGLE_HERO->m_iCoin < 600)
					{
						UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
						break;
					}
					showFastHirePanelData();
				}
				else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_PUP_BUY_CREW_FAIL");
				}
				break;
	}
	case PROTO_TYPE_GetFriendValueResult:
	{
				GetFriendValueResult* result = (GetFriendValueResult*)message;
				if (result->failed == 0)
				{
					showMaincityInfo(m_palaceInfoResult,m_producesResult,result);
				}
				break;
	}
	case PROTO_TYPE_ExpandPackageSizeResult:
		{
			ExpandPackageSizeResult *result = (ExpandPackageSizeResult*)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_ITEM_BAG_EXPAND_RESULT");
				m_pCityDataResult->max_package_size = result->newpackagesize;
				showBagOverWeight();
				SINGLE_HERO->m_iCoin = result->coin;
				SINGLE_HERO->m_iGold = result->gold;
				flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
			}
			else if (result->failed == COIN_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			}
			break;
		}
	case PROTO_TYPE_GetCityPrepareStatusResult:
	{
			GetCityPrepareStatusResult *result = (GetCityPrepareStatusResult*)message;
			if (result->failed == 0)
			{
				if (result->is_in_war ==0)
				{
					if (m_pCityDataResult)
					{
						m_pCityDataResult->is_in_state_war = 0;
					}
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_HAVE_BEEN_OVER");
				}
				else
				{
					openCountryWarPoster(result);
				}			
			}
			else
			{
				if (m_pCityDataResult)
				{
					m_pCityDataResult->is_in_state_war = 0;
				}			
				
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_HAVE_BEEN_OVER");
			}
			break;
	}
	case PROTO_TYPE_GetFleetAndDockShipsResult:
	{
		GetFleetAndDockShipsResult* result = (GetFleetAndDockShipsResult*)message;
		if (result->failed == 0)
		{
			float GOLD_REPAIR = result->shipyard_repair_ship_cost_gold;
			float COIN_REPAIR = result->shipyard_repair_ship_cost_coin;
			int coin = 0;
			int gold = 0;
			bool isRepairCoin = false;
			for (int i = 0; i < result->n_fleetships + result->n_catchships; i++)
			{
				ShipDefine* shipInfo;
				if (i <  result->n_fleetships)
				{
					shipInfo = result->fleetships[i];
				}
				else
				{
					shipInfo = result->catchships[i - result->n_fleetships];
				}
				coin += shipInfo->current_hp_max - shipInfo->hp;
			}
			coin = (int)ceil(coin * COIN_REPAIR);

			for (int i = 0; i < result->n_fleetships + result->n_catchships; i++)
			{
				ShipDefine* shipInfo;
				if (i <  result->n_fleetships)
				{
					shipInfo = result->fleetships[i];
				}
				else
				{
					shipInfo = result->catchships[i - result->n_fleetships];
				}
				gold += ceil((shipInfo->hp_max - shipInfo->current_hp_max) * GOLD_REPAIR * sqrt(SINGLE_SHOP->getShipData()[shipInfo->sid].rarity));
			}
			m_bIsRepair = true;
			closeView(COMMOM_COCOS_RES[C_VIEW_EXIT_RECONNECT]);
			if (coin > 0)
			{
				m_nRepairCost = coin;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->flushSilverConfirmView("TIP_DOCK_SHIP_REPAIR_COST_TITLE", "TIP_DOCK_SHIP_REPAIR_COST_CONTENT", coin);
			}
			else if (gold > 0)
			{
				m_nRepairCost = gold;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->flushVConfirmView("TIP_DOCK_SHIP_ADVREPAIR_COST_TITLE", "TIP_DOCK_SHIP_ADVREPAIR_COST_CONTENT", gold);
			}
			else
			{
				m_nTipImageIndex = IMAGE_TIP_NONE;
				m_bIsRepair = false;
				auto i_hints_dua = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_hints_dua"));
				i_hints_dua->setVisible(false);
				//修理完成
			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_DOCK_GET_FLEET_FAIL");
		}
		break;
	}
	case PROTO_TYPE_RepairShipResult:
	{
			RepairShipResult *result = (RepairShipResult *)message;

			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_DOCK_REPAIR_SHIP_SUCCESS");
				flushCionAndGold(result->coin, result->gold);
				Utils::consumeVTicket("4", 1, result->cost);
				m_nTipImageIndex = IMAGE_TIP_NONE;
				m_bIsRepair = false;
				auto i_hints_dua = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_hints_dua"));
				i_hints_dua->setVisible(false);
			}
			else if (result->failed == GOLD_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openGoldNotEnoughYesOrNo(result->cost);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_REPAIR_SHIP_FAIL");
			}
			break;
	}
	case PROTO_TYPE_RepairAllShipsResult:
	{
		RepairAllShipsResult *result = (RepairAllShipsResult *)message;
		if (result->failed == 0)
		{
			flushCionAndGold(result->coin, result->gold);
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_DOCK_REPAIR_ALL_SUCCESS");
			ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::create(this));
		}
		else if (result->failed == COIN_NOT_FAIL)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_DOCK_REPAIR_ALL_FAIL");
		}
		break;
	}
	case PROTO_TYPE_GetForceCityResult:
	{
      GetForceCityResult  *result = (GetForceCityResult*)message;
         if (result->failed == 0)
          {
             UICommon::getInstance()->openCommonView(this);
             UICommon::getInstance()->openCestatusView(result);
          }
        break;
	}
	default:
		break;
	}
}

void UIMain::updateMailNumber(const float fTime)
{
	ProtocolThread::GetInstance()->checkMailBox();
	//聊天提示(红点)
	auto b_unread = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_68"));
	bool unread = false;
	if (SINGLE_CHAT->m_public_unread_falg_zh)
	{
		unread = true;
	}
	if (SINGLE_CHAT->m_public_unread_falg)
	{
		unread = true;
	}
	if (SINGLE_CHAT->m_country_unread_falg)
	{
		unread = true;
	}
	if (SINGLE_CHAT->m_zone_unread_falg)
	{
		unread = true;
	}
	if (SINGLE_CHAT->m_guild_unread_falg)
	{
		unread = true;
	}

	std::map<std::string, bool>::iterator m1_Iter;
	for (m1_Iter = SINGLE_CHAT->m_private_unread_falg.begin(); m1_Iter != SINGLE_CHAT->m_private_unread_falg.end(); m1_Iter++)
	{
		if (m1_Iter->second)
		{
			unread = true;
		}
	}

	if (unread)
	{
		b_unread->setVisible(true);
	}
	else
	{
		b_unread->setVisible(false);
	}

	auto b_customerservice = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_customerservice"));
	auto i_customerService = b_customerservice->getChildByName<ImageView*>("image_unread_1");
	//客服提示
	if (Utils::getUnreadServiceMsg() > 0)
	{
		i_customerService->setVisible(true);
	}
	else
	{
		i_customerService->setVisible(false);
	}
}

void UIMain::setMainLayerButton(const bool isTouch)
{
	auto b_market = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot,"button_market"));
	auto b_shipyard = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot,"button_shipyard"));
	auto b_tavern = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot,"button_tavern"));
	auto b_palace_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot,"button_palace"));
	auto b_palace_2 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot,"button_cityhall"));
	auto b_bank = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot,"button_bank"));
    auto b_dock = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_dock"));

	b_market->setBright(isTouch);
	b_shipyard->setBright(isTouch);
    
	b_palace_1->setBright(isTouch);
	b_palace_2->setBright(isTouch);
	
	if (m_bIsNeutralCityChange)
    {
		b_bank->setBright(false);
    }
	else
	{
		b_bank->setBright(true);
	}

    //0英,1中
    int langType = SINGLE_SHOP->L_TYPE;

    b_market->loadTextures(MARKET_ENABLE[langType], "", MARKET_DISABLE[langType]);
    b_shipyard->loadTextures(SHIPYARD_ENABLE[langType], "", SHIPYARD_DISABLE[langType]);
    b_tavern->loadTextures(TAVERN_ENABLE[langType], "", TAVERN_DISABLE[langType]);
    b_palace_1->loadTextures(PALACE_ENABLE[langType], "", PALACE_DISABLE[langType]);
    b_palace_2->loadTextures(CITYHALL_ENABLE[langType], "", CITYHALL_DISABLE[langType]);
    b_bank->loadTextures(BANK_ENABLE[langType], "", BANK_DISABLE[langType]);
    b_dock->loadTextures(DOCK[langType], "", "");

}

/*
 * 刷新聊天室窗口信息
 */
void UIMain::flushMainChat()
{
	auto i_chat = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot,"image_chat"));
	auto p_chat = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot,"panel_chat"));
	auto seaChat = dynamic_cast<Text*>(Helper::seekWidgetByName(m_vRoot,"label_chat_content"));

	if (!m_pMainChat)
	{
		m_pMainChat = RichText::create();
		p_chat->addChild(m_pMainChat);
	}
	if (m_nChatItem)
	{
		m_pMainChat->removeElement(1);
		m_pMainChat->removeElement(0);
		m_nChatItem = 0;
	}
	
	if(UISocial::getInstance()->getSocialChat().empty())
	{
		m_bIsFirstChat = false;
		i_chat->setVisible(false);
		m_pMainChat->runAction(Sequence::createWithTwoActions(DelayTime::create(1),
				CallFunc::create(CC_CALLBACK_0(UIMain::flushMainChat,this))));
	}else
	{
		m_nChatItem = 2;
		if (UISocial::getInstance()->getSocialChat().size() - 1 > UISocial::getInstance()->m_nChatIndex)
		{
			UISocial::getInstance()->m_nChatIndex++;
			m_nShowChatTime = 0;
		}
		m_nShowChatTime++;
		if (m_bIsFirstChat)
		{
			m_bIsFirstChat = false;
			m_nShowChatTime = 5;
		}
		std::string name;
		name += "[";
		name += UISocial::getInstance()->getSocialChat().at(UISocial::getInstance()->m_nChatIndex).channel;
		name += "] ";
		std::string content;
		content += UISocial::getInstance()->getSocialChat().at(UISocial::getInstance()->m_nChatIndex).name;
		if (content == "")
		{
			
			if (UISocial::getInstance()->getSocialChat().at(UISocial::getInstance()->m_nChatIndex).channel == SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_PUBLIC"])
			{
				content += SINGLE_SHOP->getTipsInfo()["TIP_CHAT_SYSTEM_NOTICE"];
			}
			else if (UISocial::getInstance()->getSocialChat().at(UISocial::getInstance()->m_nChatIndex).channel == SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_CHANEL_PUBLIC_ZH"])
			{
				content += SINGLE_SHOP->getTipsInfo()["TIP_CHAT_SYSTEM_NOTICE_ZH"];
			}
			else
			{
				content += SINGLE_SHOP->getTipsInfo()["TIP_CHAT_GUILD_NOTICE"];
			}
		}
		content += ":";
		content += UISocial::getInstance()->getSocialChat().at(UISocial::getInstance()->m_nChatIndex).content;
		auto t_name = RichElementText::create(0, Color3B(154, 248, 255), 255, name, "Helvetica", 24);
		auto t_content = RichElementText::create(1, Color3B(255, 255, 255), 255, content, "Helvetica", 24);
		m_pMainChat->pushBackElement(t_name);
		m_pMainChat->pushBackElement(t_content);

		m_pMainChat->formatText();
		m_pMainChat->setPosition(Vec2(m_pMainChat->getContentSize().width*0.45 + i_chat->getContentSize().width, seaChat->getPositionY()));
		float t_tatol = m_pMainChat->getContentSize().width + i_chat->getContentSize().width;
		float time = t_tatol / 100;

		if (m_nShowChatTime < 4)
		{
			i_chat->setVisible(true);
			m_pMainChat->runAction(Sequence::createWithTwoActions(MoveTo::create(time, Vec2(-m_pMainChat->getContentSize().width*0.55, m_pMainChat->getPositionY())),
				CallFunc::create(CC_CALLBACK_0(UIMain::flushMainChat, this))));
		}
		else
		{
			i_chat->setVisible(false);
			m_pMainChat->runAction(Sequence::createWithTwoActions(DelayTime::create(1),
				CallFunc::create(CC_CALLBACK_0(UIMain::flushMainChat, this))));
		}
	}
}

void UIMain::bgMusicAndSound()
{
	if (UserDefault::getInstance()->getIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO) == OPEN_AUDIO)
	{
 		SINGLE_AUDIO->setBackgroundMusicON(true);
		playAudio();
	}else
	{
		SINGLE_AUDIO->setBackgroundMusicON(false);
	}
	if (UserDefault::getInstance()->getIntegerForKey(SOUND_EFFECT_KEY, OPEN_AUDIO) == OPEN_AUDIO)
	{
		SINGLE_AUDIO->setEffectON(true);
	}else
	{
		SINGLE_AUDIO->setEffectON(false);
	}
}

void UIMain::getAllUnreadImage(const CheckMailBoxResult *pResult)
{
	if (!pResult) 
	{
		return;
	}
	auto imageUnreadPlayer = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot,"image_notify"));
	auto imageUnreadEmail = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot,"image_unread_1"));
	auto imageUnreadFriend = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot,"image_unread_2"));
	auto imageUnreadGuild = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot,"image_unread_3"));
	auto imageUnreadTask = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot,"image_unread_4"));
	
	if (UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(MAIN_CITY_NEW_POINT).c_str()))
	{
		imageUnreadPlayer->setVisible(true);		   
	}else
	{
		imageUnreadPlayer->setVisible(false);		
	}
	
	if (pResult->newmailcount == 0)
	{
		imageUnreadEmail->setVisible(false);
	}else
	{
		imageUnreadEmail->setVisible(true);
	}

	if (pResult->newfriendsreqs == 0)
	{
		imageUnreadFriend->setVisible(false);
	}else
	{
		imageUnreadFriend->setVisible(true);
	}
	if (pResult->newguildreqs == 0)
	{
		imageUnreadGuild->setVisible(false);
	}else
	{
		imageUnreadGuild->setVisible(true);
	}
	
	if (pResult->taskcompleted != 0 || UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(MAIN_TASK_FINISH).c_str())|| m_bReceive > 0 || m_bClifford > 0)
	{
		imageUnreadTask->setVisible(true);
	}
	else
	{
		imageUnreadTask->setVisible(false);
	}

	//临时处理位置
	float posY[7] = { 518, 436.6, 358, 277,203.5,136,66};
	
	auto panel_libao = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_libao"));//礼包
	auto panel_activity_bases = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_activity_bases"));//海盗攻城
	auto panel_activity_pirate = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_activity_pirate"));//海盗基地
	auto panel_activity_lotto = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_activity_lotto"));//转盘
	auto panel_activity_invent = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_activity_invent"));//faceBook邀请
	auto panel_activity_bonus = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_activity_bonus"));//首冲
	auto panel_verify = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_verify"));//账户验证
	auto panel_activity_war = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_activity_war"));//国战


	auto listview_event = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_vRoot, "listview_event"));
	listview_event->setVisible(false);
	auto Panel_events = dynamic_cast<Widget*>(Helper::seekWidgetByName(listview_event, "Panel_events"));
	auto button_events = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_events"));
	auto showButtonsTotalNum = 0;
	if (m_bIsShowLibao)
	{
		showButtonsTotalNum += 1;
	}
	if (pResult->startpirateattact)
	{
		panel_activity_bases->setVisible(true);
	
		switch (showButtonsTotalNum)
		{
		case 0:
		panel_activity_bases->setPositionY(posY[0]);
		break;
		case 1:
		panel_activity_bases->setPositionY(posY[1]);
		break;
		case 2:
		panel_activity_bases->setPositionY(posY[2]);
		break;
		default:
		break;
		}
		showButtonsTotalNum += 1;
	}
	else
	{
		panel_activity_bases->setVisible(false);
	}

	if (pResult->startattackpirate)
	{
		panel_activity_pirate->setVisible(true);
	
		switch (showButtonsTotalNum)
		{
		case 0:
		panel_activity_pirate->setPositionY(posY[0]);
		break;
		case 1:
		panel_activity_pirate->setPositionY(posY[1]);
		break;
		case 2:
		panel_activity_pirate->setPositionY(posY[2]);
		break;
		default:
		break;
		}
		showButtonsTotalNum += 1;
	}
	else
	{
		panel_activity_pirate->setVisible(false);
	}
	//显示国战
	if (m_pCityDataResult->is_in_state_war)
	{
		panel_activity_war->setVisible(true);
		switch (showButtonsTotalNum)
		{
		case 0:
		panel_activity_war->setPositionY(posY[0]);
		break;
		case 1:
		panel_activity_war->setPositionY(posY[1]);
		break;
		case 2:
		panel_activity_war->setPositionY(posY[2]);
		break;
		default:
		break;
		}
		showButtonsTotalNum += 1;
	}
	else
	{
		panel_activity_war->setVisible(false);
	}
	if (m_pCityDataResult->showlottomessage == 0)
	{
		panel_activity_lotto->setVisible(true);
		switch (showButtonsTotalNum)
		{
		case 0:
		panel_activity_lotto->setPositionY(posY[0]);
			break;
		case 1:
		panel_activity_lotto->setPositionY(posY[1]);
			break;
		case 2:
		panel_activity_lotto->setPositionY(posY[2]);
		break;
		case 3:
		panel_activity_lotto->setPositionY(posY[3]);
			break;
		default:
			break;
		}
		showButtonsTotalNum += 1;
	}
	else
	{
		panel_activity_lotto->setVisible(false);
	}
	//facebook被邀请
	
	if (m_pCityDataResult->haveinvitebonus)
	{
		panel_activity_invent->setVisible(true);
		switch (showButtonsTotalNum)
		{
		case 0:
		panel_activity_invent->setPositionY(posY[0]);
			break;
		case 1:
		panel_activity_invent->setPositionY(posY[1]);
			break;
		case 2:
		panel_activity_invent->setPositionY(posY[2]);
			break;
		case 3:
		panel_activity_invent->setPositionY(posY[3]);
		break;
		case 4:
		panel_activity_invent->setPositionY(posY[4]);
			break;
		default:
			break;
		}
		showButtonsTotalNum += 1;
	}
	else
	{
		panel_activity_invent->setVisible(false);
	}

	//首充v奖励活动
	if (m_pCityDataResult->consume_mark || m_bIsChargeVSuccees)
	{
		//已经购买过v
		panel_activity_bonus->setVisible(false);
	}
	else
	{
		panel_activity_bonus->setVisible(true);
		switch (showButtonsTotalNum)
		{
		case 0:
		panel_activity_bonus->setPositionY(posY[0]);
			break;
		case 1:
		panel_activity_bonus->setPositionY(posY[1]);
			break;
		case 2:
		panel_activity_bonus->setPositionY(posY[2]);
			break;
		case 3:
		panel_activity_bonus->setPositionY(posY[3]);
		break;
		case 4:
		panel_activity_bonus->setPositionY(posY[4]);
		break;
		case 5:
		panel_activity_bonus->setPositionY(posY[5]);
			break;
		default:
			break;
		}
		showButtonsTotalNum += 1;
	}
	//账户验证
	if (pResult->authenticated)
	{
		//是否领取奖励
		if (m_pCityDataResult->getemailverifiedreward)
		{
			panel_verify->setVisible(false);
		}
		else
		{
			panel_verify->setVisible(true);
			switch (showButtonsTotalNum)
			{
			case 0:
			panel_verify->setPositionY(posY[0]);
			break;
			case 1:
			panel_verify->setPositionY(posY[1]);
			break;
			case 2:
			panel_verify->setPositionY(posY[2]);
			break;
			case 3:
			panel_verify->setPositionY(posY[3]);
			break;
			case 4:
			panel_verify->setPositionY(posY[4]);
			break;
			case 5:
			panel_verify->setPositionY(posY[5]);
			break;
			case 6:
			panel_verify->setPositionY(posY[6]);
			break;
			default:
			break;
			}
			showButtonsTotalNum += 1;
		}
	}
	else
	{
		panel_verify->setVisible(true);
		switch (showButtonsTotalNum)
		{
		case 0:
		panel_verify->setPositionY(posY[0]);
		break;
		case 1:
		panel_verify->setPositionY(posY[1]);
		break;
		case 2:
		panel_verify->setPositionY(posY[2]);
		break;
		case 3:
		panel_verify->setPositionY(posY[3]);
		break;
		case 4:
		panel_verify->setPositionY(posY[4]);
		break;
		case 5:
		panel_verify->setPositionY(posY[5]);
		break;
		case 6:
		panel_verify->setPositionY(posY[6]);
		break;
		case 7:
		panel_verify->setPositionY(posY[7]);
		break;
		default:
		break;
		}
		showButtonsTotalNum += 1;
	}
	//是否显示小按钮
	if (showButtonsTotalNum>4)
	{
		listview_event->setDirection(ListView::Direction::VERTICAL);
		button_events->setVisible(true);
	}
	else
	{
		listview_event->setDirection(ListView::Direction::HORIZONTAL);
		button_events->setVisible(false);
	}
	listview_event->refreshView();
	listview_event->setVisible(true);
	//临时处理赏金榜运送货物任务显示提示按钮
	auto panel_task = m_vRoot->getChildByName<Widget*>("panel_task");
	auto image_task = panel_task->getChildByName<ImageView*>("image_task");
	auto i_unread_4_0 = panel_task->getChildByName<ImageView*>("image_unread_4_0");
	if (m_pCurTaskDefine && m_bIsGuardExit)
	{
		if (pResult && pResult->sidetaskid && pResult->sidetaskneedgoodsid)
		{
			panel_task->setVisible(true);
			auto t_content_0 = image_task->getChildByName<Text*>("label_content_0");
			auto t_content_0_0 = image_task->getChildByName<Text*>("label_content_0_0");
			t_content_0->setString(SINGLE_SHOP->getSideTaskInfo()[pResult->sidetaskid].title);
			std::string content;
			std::string new_vaule;
			std::string old_vaule;
			content = SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_GOODS"];
			auto CoreTaskInfo = SINGLE_SHOP->getCoreTaskById(m_pCurTaskDefine->taskcoreid);
			new_vaule = StringUtils::format("%d/%d", pResult->sidetaskoffernum, CoreTaskInfo.finish_goods_amount[m_pCurTaskDefine->npcindex]);
			old_vaule = "[num]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = SINGLE_SHOP->getGoodsData()[CoreTaskInfo.finish_goods_id[m_pCurTaskDefine->npcindex]].name;
			old_vaule = "[good]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = SINGLE_SHOP->getCitiesInfo()[m_pCurTaskDefine->targetcity].name;
			old_vaule = "[city]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			t_content_0_0->setString(content);
		}
		else
		{
			panel_task->setVisible(false);
			i_unread_4_0->runAction(FadeOut::create(0.001));
		}
	}
	else
	{
		panel_task->setVisible(false);
		i_unread_4_0->runAction(FadeOut::create(0.001));
	}
}

void UIMain::getMainTaskUnreadImage()
{
    if (!m_pCityDataResult)
    {
        return;
    }
    auto imageUnreadTask = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_unread_4"));
	
	if ((m_pCurTaskDefine && m_pCurTaskDefine->taskstatus) || m_nCompletedAllTasks != 1 && m_nCurMainTaskComplete == 1 || m_bReceive > 0 || m_bClifford > 0)
    {
        imageUnreadTask->setVisible(true);
    }
    else
    {
        imageUnreadTask->setVisible(false);
    }
}
void UIMain::checkBountyBoard()
{
	//临时处理赏金榜运送货物任务显示提示按钮
	if (m_bIsGuardExit && m_pCheckResult)
	{
		auto panel_task = m_vRoot->getChildByName<Widget*>("panel_task");
		auto image_task = panel_task->getChildByName<ImageView*>("image_task");
		auto i_unread_4_0 = panel_task->getChildByName<ImageView*>("image_unread_4_0");
		if (m_pCheckResult->sidetaskid && m_pCheckResult->sidetaskneedgoodsid)
		{
			panel_task->setVisible(true);
			image_task->setPositionX(-image_task->getContentSize().width / 2);
			image_task->setOpacity(0);
			auto t_content_0 = image_task->getChildByName<Text*>("label_content_0");
			auto t_content_0_0 = image_task->getChildByName<Text*>("label_content_0_0");
			i_unread_4_0->runAction(FadeOut::create(0.001));
			t_content_0->setString(SINGLE_SHOP->getSideTaskInfo()[m_pCheckResult->sidetaskid].title);
			std::string content;
			std::string new_vaule;
			std::string old_vaule;
			content = SINGLE_SHOP->getTipsInfo()["TIP_PUP_FINISH_GOODS"];
			auto CoreTaskInfo = SINGLE_SHOP->getCoreTaskById(m_pCurTaskDefine->taskcoreid);
			new_vaule = StringUtils::format("%d/%d", m_pCheckResult->sidetaskoffernum, CoreTaskInfo.finish_goods_amount[m_pCurTaskDefine->npcindex]);
			old_vaule = "[num]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = SINGLE_SHOP->getGoodsData()[CoreTaskInfo.finish_goods_id[m_pCurTaskDefine->npcindex]].name;
			old_vaule = "[good]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = SINGLE_SHOP->getCitiesInfo()[m_pCurTaskDefine->targetcity].name;
			old_vaule = "[city]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			t_content_0_0->setString(content);
			auto action1 = Spawn::createWithTwoActions(FadeIn::create(1), EaseExponentialOut::create(MoveTo::create(1, Vec2(image_task->getContentSize().width / 2, image_task->getPositionY()))));
			auto action2 = Spawn::createWithTwoActions(FadeOut::create(1),EaseExponentialIn::create(MoveTo::create(1, Vec2(-image_task->getContentSize().width / 2, image_task->getPositionY()))));
			image_task->runAction(Sequence::create(action1, DelayTime::create(8.0), action2, nullptr));
			i_unread_4_0->runAction(RepeatForever::create(Sequence::createWithTwoActions(FadeIn::create(1), FadeOut::create(1))));
		}
		else
		{
			panel_task->setVisible(false);
			i_unread_4_0->runAction(FadeOut::create(0.001));
		}
	}
}

void UIMain::showMaincityInfo(const GetPalaceInfoResult *result, const GetCityProducesResult *proudceResult, const GetFriendValueResult * friendInfo)
{
	if (!result){ return; };
	std::string html_color_begin;
	std::string html_color_end;
#if WIN32
	html_color_begin = "";
	html_color_end = "";
#else
	//下划线
	html_color_begin = "<html><u >";
	html_color_end = "</u></html>";
#endif
	openView(CITYINFO_RES[CITYINFO_CSB]);
	auto cityInfoPanel = getViewRoot(CITYINFO_RES[CITYINFO_CSB]);
	auto i_pop = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cityInfoPanel, "image_city_population"));
	i_pop->setTouchEnabled(true);
	i_pop->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
	i_pop->setTag(IMAGE_ICON_POPULATION + IMAGE_INDEX2);
	auto i_trade = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cityInfoPanel, "image_city_trade"));
	i_trade->setTouchEnabled(true);
	i_trade->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
	i_trade->setTag(IMAGE_ICON_TRADE + IMAGE_INDEX2);
	auto i_manu = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cityInfoPanel, "image_city_manufacture"));
	i_manu->setTouchEnabled(true);
	i_manu->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
	i_manu->setTag(IMAGE_ICON_MANUFACTURE + IMAGE_INDEX1);
	auto image_bg = cityInfoPanel->getChildByName<ImageView*>("image_bg");
	auto image_city_country = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_bg, "image_country"));
	auto label_city = dynamic_cast<Text*>(Helper::seekWidgetByName(cityInfoPanel, "label_city"));
	image_city_country->loadTexture(getCountryIconPath(SINGLE_HERO->m_iCurCityNation));

	label_city->setString(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].name);

	auto text_port = dynamic_cast<Text*>(Helper::seekWidgetByName(cityInfoPanel, "text_port"));
	std::string s_port = text_port->getString();
	std::string old_value = "[NeutralPort]";
	std::string new_value = SINGLE_SHOP->getTipsInfo()[getPortTypeOrInfo(2).c_str()];
	repalce_all_ditinct(s_port, old_value, new_value);
	text_port->setString(s_port);
	auto text_port_city = dynamic_cast<Text*>(Helper::seekWidgetByName(cityInfoPanel, "text_port_city"));
	std::string s_port_city;
	s_port_city += html_color_begin;
	s_port_city += text_port_city->getString();
	s_port_city += html_color_end;
	std::string old_value1 = "[England]";
	std::string new_value1 = SINGLE_SHOP->getNationInfo()[result->nationidx];
	repalce_all_ditinct(s_port_city, old_value1, new_value1);
	text_port_city->setString(s_port_city);
	text_port_city->setPositionX(text_port->getContentSize().width + text_port->getPositionX() + (text_port_city->getContentSize().width) / 2);
	text_port_city->setTouchEnabled(true);
	text_port_city->addTouchEventListener(CC_CALLBACK_2(UIMain::textEvent, this));

	auto i_citytra = cityInfoPanel->getChildByName<ImageView*>("panel_cityinfo");
	auto i_citytra1 = i_citytra->getChildByName<ImageView*>("listview_friend");
	auto i_citytra2 = i_citytra1->getChildByName<ImageView*>("panel_current");
	auto i_citytra3 = i_citytra2->getChildByName<ImageView*>("image_city_trade");


	cityInfoPanel->setPosition(STARTPOS);
	cityInfoPanel->setVisible(true);
	cityInfoPanel->setLocalZOrder(10);
	auto listview_friend = cityInfoPanel->getChildByName<ListView*>("listview_friend");
	auto panel_pp = listview_friend->getChildByName("panel_pp");
	auto ppWidth = panel_pp->getContentSize().width;
	auto panel_license_2 = listview_friend->getChildByName("panel_license_2");
	auto panel_city = listview_friend->getChildByName("panel_city");
	auto panel_current = listview_friend->getChildByName("panel_current");
	auto image_pulldown = cityInfoPanel->getChildByName<ImageView*>("image_pulldown");
	auto label_popular_product = panel_pp->getChildByName<Text*>("label_popular_product");
	auto label_license = panel_license_2->getChildByName<Text*>("label_license");
	auto panel_item_1 = panel_pp->getChildByName("panel_item_1");
	auto panel_item_2 = panel_pp->getChildByName("panel_item_2");
	auto panel_item_3 = panel_pp->getChildByName("panel_item_3");
	panel_item_1->setVisible(false);
	panel_item_2->setVisible(false);
	panel_item_3->setVisible(false);
	auto label_popular_product_no=panel_pp->getChildByName<Text*>("label_popular_product_no");
	label_popular_product_no->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_HAVE_NO_POPULAR_PRODUCT"]);
	if (m_pCityDataResult->haslisence)
	{
		label_license->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_HAVE_BUSINESS_LICENSE"]);
	}else
	{
		label_license->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_HAVE_NO_BUSINESS_LICENSE"]);
	}
	if (!m_pCityDataResult->n_populargoodsids)
	{
		label_popular_product_no->setVisible(true);
		label_popular_product->setVisible(false);
	}else
	{
		label_popular_product_no->setVisible(false);
		label_popular_product->setVisible(true);
		if (m_pCityDataResult->n_populargoodsids == 1)
		{
			panel_item_1->setVisible(true);
			panel_item_1->setPositionX(ppWidth*1.0/2.5);
		}
		else if (m_pCityDataResult->n_populargoodsids == 2)
		{
			panel_item_1->setVisible(true);
			panel_item_2->setVisible(true);
			panel_item_1->setPositionX(ppWidth*1.0 / 4);
			panel_item_2->setPositionX(ppWidth*1.0 / 3 * 2);
		}else 
		{
			m_pCityDataResult->n_populargoodsids = 3;
			panel_item_1->setVisible(true);
			panel_item_2->setVisible(true);
			panel_item_3->setVisible(true);
		}
		for (int i = 0; i < m_pCityDataResult->n_populargoodsids; i++)
		{
			std::string panelIndex = StringUtils::format("panel_item_%d",i + 1);
			auto panelItem = panel_pp->getChildByName(panelIndex);
			auto button_good_bg = panelItem->getChildByName<Button*>("button_good_bg");
			button_good_bg->setTouchEnabled(true);
			button_good_bg->setTag(m_pCityDataResult->populargoodsids[i]);
			auto image_goods = button_good_bg->getChildByName<ImageView*>("image_goods");
			image_goods->ignoreContentAdaptWithSize(false);
			image_goods->loadTexture(getGoodsIconPath(m_pCityDataResult->populargoodsids[i]));
			auto label_goods_name = panelItem->getChildByName<Text*>("label_goods_name");
			label_goods_name->setString(SINGLE_SHOP->getGoodsData()[m_pCityDataResult->populargoodsids[i]].name);
		}
	}
	//panel_city
	auto label_population_num = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panel_city,"label_population_num"));
	auto label_trade_num = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panel_city,"label_trade_num"));
	auto label_manufacture_num = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panel_city,"label_manufacture_num"));
	label_population_num->setString(StringUtils::format("%d",result->population));
	label_trade_num->setString(StringUtils::format("%d",result->trade));
	label_manufacture_num->setString(StringUtils::format("%d",result->manufacture));
	//panel_current
	auto image_country = panel_current->getChildByName<ImageView*>("image_country");
	image_country->loadTexture(getCountryIconPath(m_pCityDataResult->current_city_nation));
	auto listviewNameLv = panel_current->getChildByName<ListView*>("listview_name_lv");
	auto currMayorName = dynamic_cast<Text*>(listviewNameLv->getItem(0));
	if (result->mayorname)
	{
		currMayorName->setString(result->mayorname);
		image_country->setVisible(true);
	}else
	{
		currMayorName->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NOT_RANK"]);
		image_country->setVisible(false);
	}
	auto currMayorLv = dynamic_cast<Text*>(listviewNameLv->getItem(1));
	currMayorLv->setVisible(false);
	currMayorLv->setString(StringUtils::format("Lv. %d",result->mayorlevel));

	auto imageCitytrade = panel_current->getChildByName<ImageView*>("image_city_trade");
	auto labelPolice = imageCitytrade->getChildByName<Text*>("label_trade");
	listviewNameLv->refreshView();
	//develop way
	std::string  s_direct = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_TITLE"];
	int dirNum = result->olddevdirection;
	if (result->newdevdirection != 0)
	{	
		dirNum = result->newdevdirection;
	}
	if (dirNum == 1)
	{
		imageCitytrade->loadTexture("res/palace_developway/city_population.png");
		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_POPULATION"];
		i_citytra3->setTouchEnabled(true);
		i_citytra3->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
		i_citytra3->setTag(IMAGE_ICON_POPULATION + IMAGE_INDEX2);
	}else if (dirNum == 2)
	{
		imageCitytrade->loadTexture("res/palace_developway/city_trade.png");
		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_TRADE"];
		i_citytra3->setTouchEnabled(true);
		i_citytra3->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
		i_citytra3->setTag(IMAGE_ICON_TRADE + IMAGE_INDEX2);
	}else if (dirNum == 3)
	{
		imageCitytrade->loadTexture("res/palace_developway/city_manufacture.png");
		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_MAKING"];
		i_citytra3->setTouchEnabled(true);
		i_citytra3->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
		i_citytra3->setTag(IMAGE_ICON_MANUFACTURE + IMAGE_INDEX2);
	}else
	{
		imageCitytrade->loadTexture("res/palace_developway/city_no.png");
		s_direct +=SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NOT_RANK"];
	}
	labelPolice->setString(s_direct);
	//本地所产物品
	auto panel_cp_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(cityInfoPanel, "panel_cp_1"));
	auto panel_cp_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(cityInfoPanel, "panel_cp_2"));
	auto panel_Local_item_1 = panel_cp_1->getChildByName("panel_item_1");
	auto panel_Local_item_2 = panel_cp_1->getChildByName("panel_item_2");
	auto panel_Local_item_3 = panel_cp_1->getChildByName("panel_item_3");
	panel_Local_item_1->setVisible(false);
	panel_Local_item_2->setVisible(false);
	panel_Local_item_3->setVisible(false);
	auto panel_Local_item_4 = panel_cp_2->getChildByName("panel_item_4");
	auto panel_Local_item_5 = panel_cp_2->getChildByName("panel_item_5");
	auto panel_Local_item_6 = panel_cp_2->getChildByName("panel_item_6");
	panel_Local_item_4->setVisible(false);
	panel_Local_item_5->setVisible(false);
	panel_Local_item_6->setVisible(false);
	auto label_cp = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_cp_1, "label_cp"));
	label_cp->setString(SINGLE_SHOP->getTipsInfo()["TIP_CITY_INFORMATION_LOCAL_PRODUCTION_TITLE"]);
	for (int i = 0; i < proudceResult->n_produces; i++)
	{
		Widget* panelLocal = nullptr;
		if (i < 3){
			panelLocal = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_cp_1, StringUtils::format("panel_item_%d", i + 1)));
		}else{
			panelLocal = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_cp_2, StringUtils::format("panel_item_%d", i + 1)));
		}
		panelLocal->setVisible(true);

		auto b_goods = panelLocal->getChildByName<Button*>("button_good_bg");
		b_goods->setTouchEnabled(true);
		auto i_good = b_goods->getChildByName<ImageView*>("image_goods");
		auto i_black = b_goods->getChildByName<ImageView*>("image_black");
		auto i_lock = b_goods->getChildByName<ImageView*>("image_lock");
		auto t_name = panelLocal->getChildByName<Text*>("label_goods_name");
		
		b_goods->setTag(proudceResult->produces[i]->goodsid);
		i_good->ignoreContentAdaptWithSize(false);
		i_good->loadTexture(getGoodsIconPath(proudceResult->produces[i]->goodsid, IMAGE_ICON_OUTLINE));
		t_name->setString(getGoodsName(proudceResult->produces[i]->goodsid));
		if (proudceResult->produces[i]->requiredtradelevel <= proudceResult->currenttradelevel)
		{
			i_black->setVisible(false);
			i_lock->setVisible(false);	
			t_name->setOpacity(255);
		}
		else
		{
			i_black->setVisible(true);
			i_lock->setVisible(true);
			t_name->setOpacity(127);
		}
		setBgButtonFormIdAndType(b_goods, proudceResult->produces[i]->goodsid, ITEM_TYPE_GOOD);
	}
	


	auto listviewContent = cityInfoPanel->getChildByName("listview_friend");
	auto panelContent = listviewContent->getChildByName("panel_port");
	auto imageAllcountry = panelContent->getChildByName("image_personal_friendliness");
	auto label_port = dynamic_cast<Text*>(Helper::seekWidgetByName(cityInfoPanel, "label_port"));	
	//label_port->setString(getPortTypeOrInfo(0));
	for (int i = 1; i < friendInfo->n_nation; i++)
	{
		for (int j = 0; j < friendInfo->n_nation - i; j++)
		{
			if (friendInfo->nation[j]->nationid > friendInfo->nation[j + 1]->nationid)
			{
				auto temp = friendInfo->nation[j];
				friendInfo->nation[j] = friendInfo->nation[j + 1];
				friendInfo->nation[j + 1] = temp;
			}
		}
	}

	Widget *p_friend = nullptr;
	int p_friend_num = 0;
	for (int i = 1; i <= 7; ++i)
	{
		switch (i)
		{
		case 1:
			p_friend = imageAllcountry->getChildByName<Widget *>("image_portugal");
			break;
		case 2:
			p_friend = imageAllcountry->getChildByName<Widget *>("image_spain");
			break;
		case 3:
			p_friend = imageAllcountry->getChildByName<Widget *>("image_england");
			break;
		case 4:
			p_friend = imageAllcountry->getChildByName<Widget *>("image_venice");
			break;
		case 5:
			p_friend = imageAllcountry->getChildByName<Widget *>("image_sweden");
			break;
		case 6:
			p_friend = imageAllcountry->getChildByName<Widget *>("image_genoa");
			break;
		case 7:
			p_friend = imageAllcountry->getChildByName<Widget *>("image_holland");
			break;
		default:
			break;
		}
		p_friend->setTag(IMAGE_ICON_CLOCK + IMAGE_INDEX2 + friendInfo->nation[i - 1]->nationid);
		if (p_friend != nullptr)
		{

			if (friendInfo->nation[i - 1]->value < 0 && friendInfo->nation[i - 1]->value >= -100000)
			{
				p_friend_num = -1;
			}
			else if (friendInfo->nation[i - 1]->value < -100000)
			{
				p_friend_num = friendInfo->nation[i - 1]->value / 100000.0 - 0.5;
			}
			else
			{
				p_friend_num = friendInfo->nation[i - 1]->value / 100000.0 + 0.5;
			}
			((ImageView*)p_friend)->loadTexture(getCountryIconPath(friendInfo->nation[i - 1]->nationid));
			auto i_friend = p_friend->getChildByName<ImageView*>("image_friendliness_status");
			p_friend->setTouchEnabled(true);
			p_friend->addTouchEventListener(CC_CALLBACK_2(UIMain::friendValueFlag, this));

			if (p_friend_num >= 0)
			{
				i_friend->loadTexture(ICON_FRIEND_NPC);
				i_friend->setTouchEnabled(true);
				i_friend->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
				if (i <= 4)
				{
					i_friend->setTag(IMAGE_ICON_FRIEND + IMAGE_INDEX2);
				}
				else
				{
					i_friend->setTag(IMAGE_ICON_FRIEND + IMAGE_INDEX1);
				}
			}
			else
			{
				i_friend->loadTexture(ICON_ENEMY_NPC);
				i_friend->setTouchEnabled(true);
				i_friend->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
				if (i <= 4)
				{
					i_friend->setTag(IMAGE_ICON_ENEMY + IMAGE_INDEX2);
				}
				else
				{
					i_friend->setTag(IMAGE_ICON_ENEMY + IMAGE_INDEX1);
				}
			}
			Text *t_num = p_friend->getChildByName<Text*>("label_num");
			t_num->setString(StringUtils::format("%d", p_friend_num));
		}
	}

	listview_friend->refreshView();
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(listview_friend,image_pulldown);
}
void UIMain::villageButtonShow()
{
	if (!m_vRoot) return;
	auto button_Main_cityInfo= dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_city_name_bg"));
	auto btn_market = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_market"));
	auto btn_shipyard = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_shipyard"));
	auto btn_tavern = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_tavern"));
	auto btn_palace = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_palace"));
	auto btn_bank = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_bank"));
	auto btn_cityhall = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_cityhall"));
	auto btn_village_cityhall = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_village_cityhall"));
	auto btn_village_tavern = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_village_tavern"));

	auto image_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_flag"));
	auto image_village = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_village"));
	image_village->loadTexture(getCountryIconPath(SINGLE_HERO->m_iCurCityNation));
	btn_village_tavern->loadTextures(TAVERN_ENABLE[SINGLE_SHOP->L_TYPE], "", TAVERN_DISABLE[SINGLE_SHOP->L_TYPE]);
	if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)//村庄
	{
		btn_market->setVisible(false);
		btn_shipyard->setVisible(false);
		btn_tavern->setVisible(false);
		btn_palace->setVisible(false);
		btn_bank->setVisible(false);
		btn_cityhall->setVisible(false);
		btn_village_cityhall->setVisible(false);//王宫隐藏
		btn_village_tavern->setVisible(true);
		image_flag->setVisible(false);
		image_village->setVisible(true);

		button_Main_cityInfo->setTouchEnabled(false);
	}
	else
	{
		btn_market->setVisible(true);
		btn_shipyard->setVisible(true);
		btn_tavern->setVisible(true);
		btn_palace->setVisible(true);
		btn_bank->setVisible(true);
		btn_cityhall->setVisible(true);
		if (SINGLE_SHOP->getCitiesInfo()[m_pCityDataResult->data->lastcity->cityid].palace_type == 1)//市政厅，王宫的判断
		{
			btn_cityhall->setVisible(false);
			btn_palace->setVisible(true);
		}
		else
		{
			btn_palace->setVisible(false);
			btn_cityhall->setVisible(true);
		}
		btn_village_cityhall->setVisible(false);
		btn_village_tavern->setVisible(false);
		image_flag->setVisible(true);
		image_village->setVisible(false);

		button_Main_cityInfo->setTouchEnabled(true);
	}
}
void UIMain::mainCityEscortTip()
{
	if (SINGLE_HERO->m_escortCitySucces)
	{
		SINGLE_HERO->m_escortCitySucces = false;
		openView(COMMOM_COCOS_RES[C_VIEW_RESULTTEXT_CSB]);
		auto escortResultPanel = getViewRoot(COMMOM_COCOS_RES[C_VIEW_RESULTTEXT_CSB]);
		escortResultPanel->setLocalZOrder(100);
		auto label_title = escortResultPanel->getChildByName<Text*>("label_result");
		label_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_ESCORT_SUCCESS"]);
		auto label_content = escortResultPanel->getChildByName<Text*>("label_content");
		label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_ESCORT_RESULT"] + SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iBornCity].name);
	}
}

void UIMain::showCityGuardText(float t)
{
	std::string	 text = chatTxt->getString();
	lenAfter = lenNum + text.length();
	std::string showT ="";
	plusNum = chatGetUtf8CharLen(chatContent.c_str()+lenAfter);
	lenAfter = lenAfter + plusNum;
	int maxLen = chatContent.length() + 1;
	showT = chatContent.substr(lenNum, lenAfter - lenNum);
	chatTxt->setString(showT);
	if (lenAfter >= maxLen-1)
	{
		guardDiaolgOver = true;
		anchPic->setVisible(true);
		chatTxt = nullptr;
		this->unschedule(schedule_selector(UIMain::showCityGuardText));
		if (m_nGuardIndex == GAME_NEWBIE_GUIDE_END2)
		{
			ProtocolThread::GetInstance()->getPretaskItems(UILoadingIndicator::create(this));
		}
	}
	else
	{
		guardDiaolgOver = false;
	}

}
void UIMain::showLimitPackTime(float f)
{
	auto label_libao_time = m_vRoot->getChildByName<Text*>("label_libao_time");
	m_pCityDataResult->onsale_remain_time--;
	
	int64_t Hours = (m_pCityDataResult->onsale_remain_time) / 3600 % 24;
	int64_t Mins = (m_pCityDataResult->onsale_remain_time / 60) % 60;
	int64_t Seconds = m_pCityDataResult->onsale_remain_time % 60;
	std::string strMins = "";
	std::string strSec = "";
	if (Mins < 10)
	{
		strMins = StringUtils::format("0%lld", Mins);
	}
	else
	{
		strMins = StringUtils::format("%lld", Mins);
	}
	if (Seconds < 10)
	{
		strSec = StringUtils::format("0%lld", Seconds);
	}
	else
	{
		strSec = StringUtils::format("%lld", Seconds);
	}
	std::string  leftTime = StringUtils::format("0%lld", Hours) + ":" + strMins + ":" + strSec;
	label_libao_time->setString(leftTime);
	if (!m_pCityDataResult->onsale_remain_time)
	{
		if (isScheduled(schedule_selector(UIMain::showLimitPackTime)))
		{
			this->unschedule(schedule_selector(UIMain::showLimitPackTime));
		}
		showGiftPackBg(false);
	}
}
void UIMain::showGiftPackBg(bool isShow)
{
	auto panel_libao = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_libao"));
	auto button_libao = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_libao, "button_libao"));
	auto i_libao_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_libao, "image_libao_bg"));
	auto t_libao_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_libao, "label_libao_time"));
	Sprite*spriteAnimate = Sprite::create();
	
	button_libao->addChild(spriteAnimate);
	spriteAnimate->setPosition(Vec2(button_libao->getContentSize().width / 2, button_libao->getContentSize().height / 2));
	
	spriteAnimate->setScale(0.6f);
	
	if (isShow)
	{
		m_bIsShowLibao = true;
		panel_libao->setVisible(true);

		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon0.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon1.plist");
		//光芒四射
		Vector<SpriteFrame *> frames;
		for (int i = 0; i <= 27; i++)
		{
			std::string name = StringUtils::format("shinelight/eff_shinelight_%02d.png", i);
			SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
			frames.pushBack(frame);
		}
		Animation* animation_shiplight = Animation::createWithSpriteFrames(frames, 1.0 / 10, 1);

		auto animateEff = Animate::create(animation_shiplight);
		spriteAnimate->setOpacity(0);
		spriteAnimate->runAction(RepeatForever::create(Sequence::create(FadeIn::create(0.5f),animateEff,FadeOut::create(0.5f),DelayTime::create(3.0f),nullptr)));
	}
	else
	{
		m_bIsShowLibao = false;
		spriteAnimate->stopAllActions();
		spriteAnimate->removeFromParentAndCleanup(true);
		panel_libao->setVisible(false);
	}
}
/*
 * 通知玩家是否存在船只损坏
 */
void UIMain::buttonImageNotifyShow()
{
	auto image_notify_shipyard = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_notify_shipyard"));
	bool b_notify = false;
	for (size_t i = 0; i < 5; i++)
	{
		std::string st_ship_position = StringUtils::format(SHIP_POSTION_EQUIP_BROKEN, i + 1);
		if (UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), false))
		{
			b_notify = true;
			break;
		}
	}

	image_notify_shipyard->setVisible(b_notify);
}

void UIMain::newbieGuideEnd()
{
	m_nGuardIndex = GAME_NEWBIE_GUIDE_END1;
	guardEnterScene(0);
}
void UIMain::showVerify( bool isShow)
{
	auto panel_verify = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_verify"));
	if (isShow)
	{
		panel_verify->setVisible(true);
	}
	else
	{
		panel_verify->setVisible(false);
	}
}

/*
 * 显示水手不足图标提示
 */
void UIMain::showSailorOrShipDur()
{
	auto i_hints_dua = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_hints_dua"));
	auto i_hints_sailor = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_hints_sailor"));
	i_hints_sailor->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func,this));
	i_hints_dua->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func,this));
	if (m_pCityDataResult->needrepiredship)
	{
		i_hints_dua->setVisible(true);
		i_hints_dua->setPosition(Vec2(i_hints_dua->getContentSize().width/2,i_hints_dua->getContentSize().height/2));
		i_hints_sailor->setPosition(Vec2(i_hints_dua->getContentSize().width + 50, i_hints_sailor->getContentSize().height / 2));
	}
	else
	{
		i_hints_dua->setVisible(false);
		i_hints_sailor->setPosition(Vec2(i_hints_dua->getContentSize().width/2, i_hints_sailor->getContentSize().height / 2));
	}
	if (m_pCityDataResult->needaddsailors)
	{
		i_hints_sailor->setVisible(true);
		i_hints_sailor->loadTexture("cocosstudio/login_ui/common/hints_sailor_1.png");
		if (m_pCityDataResult->currentcrewnum<=m_pCityDataResult->maxcrewnum*0.1)
		{
			i_hints_sailor->loadTexture("cocosstudio/login_ui/common/hints_sailor_2.png");
		}
	}
	else
	{
		i_hints_sailor->setVisible(false);
	}
}
void UIMain::closeRainEffect()
{
	if (SINGLE_HERO->m_iCityID == 3 && SINGLE_HERO->m_londonWeather == 2)
	{
		this->unschedule(schedule_selector(UIMain::showRaindrops));
		for (int i = 0; i < m_raindrops->size(); i++)
		{
			auto rain = m_raindrops->at(i);
			rain->removeFromParentAndCleanup(true);
		}
		m_raindrops->clear();
		SINGLE_HERO->m_londonWeather = 0;
	}
}

void UIMain::flushExpAndRepLv()
{
	auto p_exp = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_exp"));
	auto p_rep = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_rep"));
	auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(m_vRoot, "label_lv"));
	auto t_lv_r = dynamic_cast<Text*>(Helper::seekWidgetByName(m_vRoot, "label_lv_r"));
	t_lv->setString(StringUtils::format("%d", SINGLE_HERO->m_iLevel));
	t_lv_r->setString(StringUtils::format("%d", SINGLE_HERO->m_iPrestigeLv));

	float temp_exp = 0;
	if (SINGLE_HERO->m_iLevel < LEVEL_MAX_NUM)
	{
		temp_exp = (SINGLE_HERO->m_iExp - LEVEL_TO_EXP_NUM(SINGLE_HERO->m_iLevel))*1.0 / (LEVEL_TO_EXP_NUM(SINGLE_HERO->m_iLevel + 1) - LEVEL_TO_EXP_NUM(SINGLE_HERO->m_iLevel));
	}
	float temp_rep = 0;
	if (SINGLE_HERO->m_iPrestigeLv < PRESTIGE_MAX_NUM)
	{
		temp_rep = (SINGLE_HERO->m_iRexp - LEVEL_TO_FAME_NUM(SINGLE_HERO->m_iPrestigeLv))*1.0 / (LEVEL_TO_FAME_NUM(SINGLE_HERO->m_iPrestigeLv + 1) - LEVEL_TO_FAME_NUM(SINGLE_HERO->m_iPrestigeLv));
	}
	p_exp->setTouchEnabled(false);
	p_rep->setTouchEnabled(false);
	p_exp->setContentSize(Size(p_exp->getContentSize().width, 120 * (temp_exp)));
	p_rep->setContentSize(Size(p_rep->getContentSize().width, 120 * (temp_rep)));
}
/*
* 显示保险图标提示
*/
void UIMain::showInsuranceHints(CheckMailBoxResult*result)
{
	auto i_hints_insurance = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_hints_insurance"));
	auto i_hints_dua = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_hints_dua"));
	auto i_hints_sailor = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_hints_sailor"));
	i_hints_insurance->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func,this));
	//显示
	if (result->insurancestatus>0)
	{
		i_hints_insurance->setVisible(true);
	}
	else
	{
		i_hints_insurance->setVisible(false);
	}
	//调整位置
	if (m_pCityDataResult->needrepiredship&&m_pCityDataResult->needaddsailors)
	{
		i_hints_insurance->setPositionX(i_hints_sailor->getPositionX()+70);
	}
	else
	{
		if (m_pCityDataResult->needrepiredship)
		{
			i_hints_insurance->setPositionX(i_hints_dua->getPositionX()+70);
		}
		else if (m_pCityDataResult->needaddsailors)
		{
			i_hints_insurance->setPositionX(i_hints_sailor->getPositionX() + 70);
		}
		else
		{
			i_hints_insurance->setPositionX(i_hints_dua->getPositionX());
		}
	}
}
void UIMain::showFaceBookInvitedHints(bool isShow)
{
	auto i_facebook_invent = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "panel_activity_invent"));
	if (isShow)
	{
		i_facebook_invent->setVisible(true);
	}
	else
	{
		i_facebook_invent->setVisible(false);
	}
}
void UIMain::openGetFaceBookReward()
{
	openView(VERIFIY_ACCOUNT_RES[VIEW_FACEBOOK_GETREWARD]);
	auto view = getViewRoot(VERIFIY_ACCOUNT_RES[VIEW_FACEBOOK_GETREWARD]);
	auto panel_invited_content = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_invited_content"));
	auto text_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_invited_content, "text_1"));
	text_1->setString(StringUtils::format("x%d",m_pCityDataResult->haveinvitebonus));
	auto text_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_invited_content, "text_2"));
	text_2->setContentSize(Size(text_2->getContentSize().width,48));
	text_2->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_FACEBOOK_CONTENT_2"]);
	auto text_3 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_invited_content, "text_3"));
	text_3->setString(SINGLE_SHOP->getTipsInfo()["TIP_SOCIAL_FACEBOOK_CONTENT_3"]);
}
void UIMain::openFacebookInviteOthers()
{
	openView(VERIFIY_ACCOUNT_RES[VIEW_INVITE_OTHERS]);
	auto view = getViewRoot(VERIFIY_ACCOUNT_RES[VIEW_INVITE_OTHERS]);
	auto panel_reward_concent = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_reward_concent"));
	auto text_amount = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_reward_concent, "text_amount"));
	text_amount->setString(StringUtils::format("+%d", m_pCityDataResult->haveinvitebonus));
	auto text_invite = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_reward_concent, "text_invite"));
	text_invite->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_FACEBOOK_INVITE_OTHERS"]);
}
void UIMain::openVActivityBonus()
{
	openView(GIFTPACK_RES[BONUS_CSB]);
	auto view = getViewRoot(GIFTPACK_RES[BONUS_CSB]);
	auto panel_content = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_content"));
	auto label_bonus_content_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_content, "label_bonus_content_1"));
	label_bonus_content_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_V_ACTIVITY_BONUS_CONTENT_1"]);
	auto label_bonus_content_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_content, "label_bonus_content_2"));
	label_bonus_content_2->setString(SINGLE_SHOP->getTipsInfo()["TIP_V_ACTIVITY_BONUS_CONTENT_2"]);
	auto label_reward_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_content, "label_reward_num"));
	label_reward_num->setString(SINGLE_SHOP->getTipsInfo()["TIP_V_ACTIVITY_BONUS_CONTENT_3"]);
}

void UIMain::showAndHideCityButton()
{
	m_bShowAndHideAction = false;
	auto button_unfold = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_unfold"));
	auto button_companions = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_companions"));
	auto button_customerservice = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_customerservice"));
	auto button_shop = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_shop"));
	auto button_map = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_map"));
	auto button_set = dynamic_cast<Button*>(Helper::seekWidgetByName(m_vRoot, "button_set"));
	int posX[5] = { 480, 386, 288, 194, 100 };
	float time = 0.1f;
	if (button_set->isVisible())
	{
		button_companions->runAction(MoveTo::create(time, Vec2(posX[2], button_companions->getPositionY())));
		button_customerservice->runAction(MoveTo::create(time, Vec2(posX[1], button_customerservice->getPositionY())));
		button_shop->runAction(MoveTo::create(time, Vec2(posX[0], button_shop->getPositionY())));
		button_map->runAction(Sequence::createWithTwoActions(Spawn::create(MoveTo::create(time, Vec2(posX[1] + 50, button_map->getPositionY())), FadeOut::create(time), ScaleTo::create(time, 0.1f), nullptr), Hide::create()));
		button_set->runAction(Sequence::createWithTwoActions(Spawn::create(MoveTo::create(time, Vec2(posX[0] + 50, button_set->getPositionY())), FadeOut::create(time), ScaleTo::create(time, 0.1f), nullptr), Hide::create()));
	}
	else
	{
		button_companions->runAction(MoveTo::create(time, Vec2(posX[4], button_companions->getPositionY())));
		button_customerservice->runAction(MoveTo::create(time, Vec2(posX[3], button_customerservice->getPositionY())));
		button_shop->runAction(MoveTo::create(time, Vec2(posX[2], button_shop->getPositionY())));
		button_map->runAction(Sequence::createWithTwoActions(Show::create(), Spawn::create(MoveTo::create(time, Vec2(posX[1], button_map->getPositionY())), FadeIn::create(time), ScaleTo::create(time, 1.0f), nullptr)));
		button_set->runAction(Sequence::createWithTwoActions(Show::create(), Spawn::create(MoveTo::create(time, Vec2(posX[0], button_set->getPositionY())), FadeIn::create(time), ScaleTo::create(time, 1.0f), nullptr)));
	}
	button_unfold->runAction(Sequence::createWithTwoActions(DelayTime::create(0.1f), CallFunc::create(CC_CALLBACK_0(UIMain::showAndHideEnd, this))));
	auto image_left = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_left"));
	image_left->runAction(RotateBy::create(0.1f, -180));
}
void UIMain::showFastHirePanelData()
{
	openView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	auto supplyView = getViewRoot(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	auto slider_dropitem_num = supplyView->getChildByName<Slider*>("slider_dropitem_num");
	slider_dropitem_num->addEventListenerSlider(this, sliderpercentchangedselector(UIMain::fastHireCrewSliderChange));
	auto label_title = dynamic_cast<Text*>(Helper::seekWidgetByName(supplyView, "label_title"));
	label_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_SAILOR_QUICH_HIRE_BUTTON_CONTENT"]);

	auto image_supply = dynamic_cast<ImageView*>(supplyView->getChildByName("image_supply"));
	image_supply->loadTexture("cocosstudio/login_ui/common/sailor.png");
	
	auto image_progressbar_supply = image_supply->getChildByName("image_progressbar_supply");
	auto progressbar_supply = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply");
	progressbar_supply->setVisible(false);
	//progressbar_supply->setPercent(100.0*(m_pFastHireCrewResult->owncrewnum*1.0 / (m_pFastHireCrewResult->availmaxcrewnum*1.0)));
	auto label_ship_supply_num_1 = image_supply->getChildByName<Text*>("label_ship_supply_num_1");
	auto label_ship_supply_num_2 = image_supply->getChildByName<Text*>("label_ship_supply_num_2");
	label_ship_supply_num_1->setString(StringUtils::format("%d", m_pFastHireCrewResult->availmaxcrewnum));
	label_ship_supply_num_2->setString(StringUtils::format("/%d", m_pFastHireCrewResult->availmaxcrewnum));
	label_ship_supply_num_2->setPositionX(label_ship_supply_num_1->getPositionX() + label_ship_supply_num_1->getContentSize().width);
	//progressbar_supply_add
	auto progressbar_supply_add = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply_add");
	progressbar_supply_add->setPercent(100);
	m_nFastHireCrewNum = m_pFastHireCrewResult->availmaxcrewnum;

	auto panel_salary = supplyView->getChildByName("panel_salary");
	auto label_cost_num = panel_salary->getChildByName<Text*>("label_cost_num");
	//花费金币数
	int coinNums = (ceil(m_nFastHireCrewNum * 1.0 / m_pFastHireCrewResult->hirecrewnumonce) + 1) * (m_pFastHireCrewResult->hirecrewfee) + m_pFastHireCrewResult->crewprice* m_nFastHireCrewNum;

	label_cost_num->setString(numSegment(StringUtils::format("%d", coinNums)));
	label_cost_num->ignoreContentAdaptWithSize(true);
	label_cost_num->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	auto i_silver = supplyView->getChildByName("image_silver_1");
	i_silver->setPositionX(label_cost_num->getPositionX() - label_cost_num->getContentSize().width - i_silver->getContentSize().width / 2);

}
void UIMain::fastHireCrewSliderChange(Ref* obj, cocos2d::ui::SliderEventType type)
{
	if (type != SliderEventType::SLIDER_PERCENTCHANGED)
	{
		return;
	}
	if (!m_pFastHireCrewResult){ return; }
	auto supplyView = getViewRoot(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	auto image_supply = supplyView->getChildByName("image_supply");
	auto label_ship_supply_num_1 = image_supply->getChildByName<Text*>("label_ship_supply_num_1");
	auto label_ship_supply_num_2 = image_supply->getChildByName<Text*>("label_ship_supply_num_2");
	auto image_progressbar_supply = image_supply->getChildByName("image_progressbar_supply");

	auto progressbar_supply = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply");
	//progressbar_supply->setPercent(100.0*(m_pFastHireCrewResult->owncrewnum*1.0 / (m_pFastHireCrewResult->availmaxcrewnum*1.0)));
	progressbar_supply->setVisible(false);
	auto slider_dropitem_num = supplyView->getChildByName<Slider*>("slider_dropitem_num");

	//增加补给
	auto progressbar_supply_add = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply_add");
	auto perChange = slider_dropitem_num->getPercent();
	progressbar_supply_add->setPercent(perChange);

	auto panel_salary = supplyView->getChildByName("panel_salary");
	auto label_cost_num = panel_salary->getChildByName<Text*>("label_cost_num");

	int32_t numChange = m_pFastHireCrewResult->availmaxcrewnum*slider_dropitem_num->getPercent() / 100.0;
	if (numChange == 0)
	{
		numChange = 1;
	}
	m_nFastHireCrewNum = numChange;
	int coinNums = (ceil(m_nFastHireCrewNum * 1.0 / m_pFastHireCrewResult->hirecrewnumonce) + 1) * (m_pFastHireCrewResult->hirecrewfee) + m_pFastHireCrewResult->crewprice* m_nFastHireCrewNum;
	
	label_cost_num->setString(numSegment(StringUtils::format("%d", coinNums)));
	label_ship_supply_num_1->setString(StringUtils::format("%d", numChange));
	label_ship_supply_num_2->setString(StringUtils::format("/%d", m_pFastHireCrewResult->availmaxcrewnum));
	label_ship_supply_num_2->setPositionX(label_ship_supply_num_1->getPositionX() + label_ship_supply_num_1->getContentSize().width);
}

void UIMain::getNpcPresentItem(const GetPretaskItemsResult *preTask)
{
	SINGLE_HERO->m_iCoin = preTask->coin;
	SINGLE_HERO->m_iGold = preTask->golds;
	flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);

	openView(COMMOM_COCOS_RES[C_VIEW_GETITEM_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_GETITEM_CSB]);
	auto l_result = view->getChildByName<ListView*>("listview_result");
	auto p_silver = l_result->getItem(0);
	auto p_bank = l_result->getItem(1);
	auto p_item = l_result->getItem(2);

	auto coinBoo = false;
	if (preTask->addcoin > 0)
	{
		auto t_silver = p_silver->getChildByName<Text*>("label_buy_num");
		std::string numString = numSegment(StringUtils::format("%lld", preTask->addcoin));
		t_silver->setString(StringUtils::format("+ %s", numString.c_str()));
		coinBoo = true;
	}

	if (preTask->addgolds > 0)
	{
		auto p_golds = p_silver->clone();
		l_result->addChild(p_golds);
		auto i_golds = p_golds->getChildByName<ImageView*>("image_silver");
		auto t_silver = p_golds->getChildByName<Text*>("label_buy_num");
		std::string numString = numSegment(StringUtils::format("%lld", preTask->addgolds));
		t_silver->setString(StringUtils::format("+ %s", numString.c_str()));
		i_golds->ignoreContentAdaptWithSize(false);
		i_golds->loadTexture(getVticketOrCoinPath(10000, 1));
	}

	if (preTask->bank > 0)
	{
		auto t_silver = p_bank->getChildByName<Text*>("label_buy_num");
		std::string numString = numSegment(StringUtils::format("%lld", preTask->bank));
		t_silver->setString(StringUtils::format("+ %s", numString.c_str()));
	}
	else
	{
		l_result->removeChild(p_bank);
	}

	if (!coinBoo)
	{
		l_result->removeChild(p_silver);
	}

	if (preTask->n_items < 1)
	{
		l_result->removeChild(p_item);
		return;
	}

	std::string name;
	std::string path;
	for (size_t i = 0; i < preTask->n_items; i++)
	{
		if (i == 0)
		{
			auto i_item_bg = p_item->getChildByName<ImageView*>("image_material_bg");
			auto i_item = i_item_bg->getChildByName<ImageView*>("image_item");
			auto t_item_name = p_item->getChildByName<Text*>("label_items_name");
			auto t_item_num = p_item->getChildByName<Text*>("label_num");
			getItemNameAndPath(preTask->items[i]->itemtype, preTask->items[i]->itemid, name, path);
			i_item->ignoreContentAdaptWithSize(false);
			i_item->loadTexture(path);
			t_item_name->setString(name);
			t_item_num->setString(StringUtils::format("+ %d", preTask->items[i]->itemamount));
			setBgButtonFormIdAndType(i_item_bg, preTask->items[i]->itemid, preTask->items[i]->itemtype);
			setTextColorFormIdAndType(t_item_num, preTask->items[i]->itemid, preTask->items[i]->itemtype);
		}
		else
		{
			auto p_item_clone = p_item->clone();
			auto i_item_bg = p_item_clone->getChildByName<ImageView*>("image_material_bg");
			auto i_item = i_item_bg->getChildByName<ImageView*>("image_item");
			auto t_item_name = p_item_clone->getChildByName<Text*>("label_items_name");
			auto t_item_num = p_item_clone->getChildByName<Text*>("label_num");
			getItemNameAndPath(preTask->items[i]->itemtype, preTask->items[i]->itemid, name, path);
			i_item->ignoreContentAdaptWithSize(false);
			i_item->loadTexture(path);
			t_item_name->setString(name);
			t_item_num->setString(StringUtils::format("+ %d", preTask->items[i]->itemamount));
			setBgButtonFormIdAndType(i_item_bg, preTask->items[i]->itemid, preTask->items[i]->itemtype);
			setTextColorFormIdAndType(t_item_num, preTask->items[i]->itemid, preTask->items[i]->itemtype);
			l_result->pushBackCustomItem(p_item_clone);
		}
	}
}

void UIMain::showDorpservice()
{
	int num = 0;
	if (m_pCheckResult && m_pCheckResult->insurancestatus > 0)
	{
		num++;
	}
	if(m_pCityDataResult->needrepiredship)
	{
		num++;
	}
	if (m_pCityDataResult->needaddsailors)
	{
		num++;
	}

	auto i_hints_dua = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_hints_dua"));
	auto i_dorp_service = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_dorp_service"));
	if (m_pCityDataResult->drop_rate_increase_ramian_time <= 0)
	{
		i_dorp_service->setVisible(false);
	}
	else
	{
		i_dorp_service->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func,this));
		i_dorp_service->setPositionX(i_hints_dua->getPositionX() + num * 70);
		i_dorp_service->setVisible(true);
		if (m_pCityDataResult->drop_rate_increase_type == 1)
		{
			i_dorp_service->loadTexture(LOW_DROPS_IOCN);
		}
		else
		{
			i_dorp_service->loadTexture(SENIOR_DROPS_IOCN);
		}
	}
}

void UIMain::updataDorpserviceTime(int sec, int type)
{
	if (m_pCityDataResult->drop_rate_increase_ramian_time <= 0)
	{
		this->schedule(schedule_selector(UIMain::updateBySecond), 1);
		auto i_dorp_service = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_dorp_service"));;
		i_dorp_service->setVisible(true);
	}
	m_pCityDataResult->drop_rate_increase_ramian_time = sec;
	m_pCityDataResult->drop_rate_increase_type = type;
	showDorpservice();
	showBagOverWeight();
}

void UIMain::updateBySecond(const float fTime)
{
	m_pCityDataResult->drop_rate_increase_ramian_time--;
	if (m_pCityDataResult->drop_rate_increase_ramian_time >= 0)
	{
		auto view = getViewRoot(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
		if (view)
		{
			std::string st_content = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_DROPS_TIME"];
			std::string old_value = "[time]";
			std::string new_value = timeUtil(m_pCityDataResult->drop_rate_increase_ramian_time, TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_GM); 
			repalce_all_ditinct(st_content, old_value, new_value);
			auto t_content = dynamic_cast<Text*>(view->getChildByName("label_dropitem_tiptext"));
			t_content->setString(st_content);
		}

		if (m_pCityDataResult->drop_rate_increase_ramian_time == 0)
		{
			closeView(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
			this->unschedule(schedule_selector(UIMain::updateBySecond));
			auto i_dorp_service = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_dorp_service"));;
			i_dorp_service->setVisible(false);
		}
	}
}

void UIMain::friendValueFlag(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	int tag = dynamic_cast<Widget*>(pSender)->getTag();
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushImageDetail(dynamic_cast<Widget*>(pSender));
}
std::string UIMain::getPortTypeOrInfo(int port)
{
	int portType = SINGLE_SHOP->getCitiesInfo()[m_pCityDataResult->data->lastcity->cityid].port_type;
	int type = 0;
	switch (portType)
	{
		//势力
	case 1:
		type = 1;
		break;
		//中立
	case 2:
		type = 2;
		break;
		//殖民
	case 3:
		type = 3;
		break;
		//敌对
	case 4:
		type = 1;
		break;
		//村庄
	case 5:
		type = 5;
		break;
	default:
		break;
	}
	std::string content;
	if (port == 0)
	{
		std::string key = StringUtils::format("TIP_PORT_TYPE_%d", type);
		content = SINGLE_SHOP->getTipsInfo()[key.c_str()];
	}else if (port == 1)
	{
		content = StringUtils::format("TIP_PORT_INFO_%d", type);
	}else if (port == 2)
	{
		content = StringUtils::format("TIP_PORT_TYPE_%d", type);
	}
	return content;
}

void UIMain::showBagOverWeight()
{
	int num = 0;
	if (m_pCheckResult && m_pCheckResult->insurancestatus > 0)
	{
		num++;
	}
	if (m_pCityDataResult->needrepiredship)
	{
		num++;
	}
	if (m_pCityDataResult->needaddsailors)
	{
		num++;
	}

	if (m_pCityDataResult->drop_rate_increase_ramian_time > 0)
	{
		num++;
	}


	auto i_over_weight = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_bag_over_weight"));
	if (i_over_weight)
	{
		i_over_weight->removeFromParentAndCleanup(true);
	}

	if (m_pCityDataResult->current_package_size > m_pCityDataResult->max_package_size)
	{
		auto i_hints_dua = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_hints_dua"));
		auto i_dorp_service = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_vRoot, "image_dorp_service"));
		auto panel_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_vRoot, "panel_hints"));
		auto i_bag = dynamic_cast<ImageView*>(i_dorp_service->clone());
		i_bag->setName("image_bag_over_weight");
		i_bag->addTouchEventListener(CC_CALLBACK_2(UIMain::menuCall_func, this));
		i_bag->setVisible(true);
		i_bag->setTouchEnabled(true);
		panel_1->addChild(i_bag);
		i_bag->setPositionX(i_hints_dua->getPositionX() + num * 70);
		i_bag->loadTexture(BAG_OVER_WEIGHT);
	}
}
void UIMain::openCountryWarPoster(const GetCityPrepareStatusResult *result)
{
	openView(GIFTPACK_RES[COUNTRY_WAR_PREPARE_POSTER]);
	auto viewPrepare = getViewRoot(GIFTPACK_RES[COUNTRY_WAR_PREPARE_POSTER]);
	auto listView_1 = dynamic_cast<ListView*>(Helper::seekWidgetByName(viewPrepare, "listView_1"));
	//1
	auto panel_war = dynamic_cast<Widget*>(Helper::seekWidgetByName(listView_1, "panel_war"));
	auto text_big_title = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_war, "text_1"));
	auto text_war = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_war, "text_1_0"));
	auto image_country = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_war, "image_country"));
	auto enemy_country = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_war, "image_country_1"));
	//2
	auto panel_what = dynamic_cast<Widget*>(Helper::seekWidgetByName(listView_1, "panel_what"));
	auto label_content_what_title = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_what, "label_content_1"));
	auto label_content_what = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_what, "label_content_2"));
	//3
	auto panel_reward = dynamic_cast<Widget*>(Helper::seekWidgetByName(listView_1, "panel_reward"));
	auto label_content_reward_title = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_reward, "label_content_1"));
	auto label_content_reward = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_reward, "label_content_2"));
	//4
	auto panel_when = dynamic_cast<Widget*>(Helper::seekWidgetByName(listView_1, "panel_when"));
	auto label_content_when_title = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_when, "label_content_1"));
	auto label_content_when = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_when, "label_content_2"));
	
	std::string myCountryName;
	std::string enemy_countryName;
	ImageView*other_country_clone = nullptr;
	int enemy_id = 0;
	for (int i = 0; i < result->n_fight_nations; i++)
	{
		myCountryName = SINGLE_SHOP->getNationInfo()[result->fight_nations[i]->nation1];
		enemy_id = result->fight_nations[i]->nation2;
		
		if (i == 0)
		{
			other_country_clone = enemy_country;
			
		}
		else
		{
			other_country_clone = dynamic_cast<ImageView*>(enemy_country->clone());
			panel_war->addChild(other_country_clone);
		}
		enemy_countryName = SINGLE_SHOP->getNationInfo()[enemy_id];
		image_country->loadTexture(getCountryIconPath(result->fight_nations[i]->nation1));
		other_country_clone->loadTexture(getCountryIconPath(enemy_id));
		other_country_clone->setPositionX(enemy_country->getPositionX() + 40 * i);
	}
	if (result->is_in_war ==1)
	{
		//准备海报	
		label_content_what_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_PREPARE_SAMLL_TITLE_1"]);
		label_content_reward_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_PREPARE_SAMLL_TITLE_2"]);
		label_content_when_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_PREPARE_SAMLL_TITLE_3"]);

		text_big_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_PREPARE_1"]);
		auto content = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_PREPARE_2"];
		auto old_mycountry = "[my_country]";
		auto old_enenmy = "[enemy_country]";
		repalce_all_ditinct(content, old_mycountry, myCountryName);
		repalce_all_ditinct(content, old_enenmy, enemy_countryName);
		text_war->setString(content);

		label_content_what->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_PREPARE_3"]);
		label_content_reward->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_PREPARE_4"]);	
		label_content_when->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_PREPARE_5"]);
	}
	else if (result->is_in_war == 2)
	{
		//战斗海报
		label_content_what_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_ON_WAR_SAMLL_TITLE_1"]);
		label_content_reward_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_ON_WAR_SAMLL_TITLE_2"]);
		label_content_when_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_ON_WAR_SAMLL_TITLE_3"]);

		text_big_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_ON_WAR_1"]);
		auto content = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_ON_WAR_2"];
		auto old_mycountry = "[my_country]";
		auto old_enenmy = "[enemy_country]";
		repalce_all_ditinct(content, old_mycountry, myCountryName);
		repalce_all_ditinct(content, old_enenmy, enemy_countryName);
		text_war->setString(content);

		auto content_1 = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_ON_WAR_3"];
		if (enemy_countryName == SINGLE_SHOP->getNationInfo()[SINGLE_HERO->m_iNation])
		{
			repalce_all_ditinct(content_1, old_enenmy, myCountryName);
		}
		else
		{
			repalce_all_ditinct(content_1, old_enenmy, enemy_countryName);
		}
		label_content_what->setString(content_1);
		label_content_reward->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_ON_WAR_4"]);
		label_content_when->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_WAR_POSTER_ON_WAR_5"]);
	}
	setTextFontSize(text_big_title);
	label_content_what->setContentSize(Size(736, getLabelHight(label_content_what->getString(),736,label_content_what->getFontName())));
	label_content_reward->setContentSize(Size(736, getLabelHight(label_content_reward->getString(), 736, label_content_reward->getFontName())));
	label_content_when->setContentSize(Size(736, getLabelHight(label_content_when->getString(), 736, label_content_when->getFontName())));
	panel_what->setContentSize(Size(736, label_content_what_title->getContentSize().height +10+ label_content_what->getContentSize().height + 20));
	panel_reward->setContentSize(Size(736, label_content_reward_title->getContentSize().height +10+ label_content_reward->getContentSize().height+20));
	panel_when->setContentSize(Size(736, label_content_when_title->getContentSize().height +10+ label_content_when->getContentSize().height +20));
	label_content_what_title->setPositionY(panel_what->getContentSize().height);
	label_content_reward_title->setPositionY(panel_reward->getContentSize().height);
	label_content_when_title->setPositionY(panel_when->getContentSize().height);
	label_content_what->setPositionY(label_content_what_title->getPositionY() - label_content_what_title->getContentSize().height - 10);
	label_content_reward->setPositionY(label_content_reward_title->getPositionY() - label_content_reward_title->getContentSize().height - 10);
	label_content_when->setPositionY(label_content_when_title->getPositionY() - label_content_when_title->getContentSize().height - 10);
	listView_1->refreshView();
	auto image_pulldown = viewPrepare->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(listView_1, image_pulldown);
}
void UIMain::textEvent(Ref* target, Widget::TouchEventType type)
{
	auto text = dynamic_cast<Text*>(target);
	if (Widget::TouchEventType::BEGAN == type)
	{
		text->setOpacity(120);
	}
	if (Widget::TouchEventType::ENDED == type)
	{
		text->setOpacity(255);
		ProtocolThread::GetInstance()->getForceCity(SINGLE_HERO->m_iCurCityNation);
	}
}
