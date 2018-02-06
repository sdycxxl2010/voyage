
#include "UISailHUD.h"
#include "SystemVar.h"
#include "TVSingle.h"
#include "UIShip.h"
#include "TVBattleManager.h"
#include "ProtocolThread.h"
#include "TVSailDefineVar.h"
#include "UISocial.h"
#include <map>
#include "UISailManage.h"
#include "TVSeaEvent.h"

#include "UIEmail.h"
#include "UIMap.h"
#include "UITask.h"
#include "UIBattleHUD.h"
#include "UICommon.h"
#include "UIInform.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "TVSceneLoader.h"
#include "UIPirate.h"
#include "Utils.h"
#include "UIStore.h"
#include "CompanionTaskManager.h"
#include "UIWorldNotify.h"
#include "UINationWarLand.h"
#include "TimeUtil.h"

UISailHUD::UISailHUD()
{
	m_bCurrentInSafeArea = true;
	m_SafeAreaCheck = nullptr;
	m_pSubPanel = nullptr;
	m_bIsBlocking = false;
	m_nLandCityId = 0;
	m_nCurMapIndex = -1;
	m_pShip = 0;
	m_nCurCrewNum = 0;
	m_nCurSupplies = 0;
	m_nFleetShipNum = 0;
	m_mainChat = nullptr;
	m_pPanelExp = nullptr;
	m_pPanelRep = nullptr;
	m_pButton_warning_1 = nullptr;
	m_pButton_warning_2 = nullptr;
	m_nConfirmIndex = 0;
	m_pResult_fleet = nullptr;
	m_pListview_result = nullptr;
	m_pFloatResultview = nullptr;
	m_nEventversion = 0;
	m_nFindcityId=0;
	for (int i = 0; i < 19; i++)
	{
		std::string mapName = StringUtils::format("ship/seaFiled_1/map_%d.jpg",i+ 1);
		TextureCache::getInstance()->addImage(mapName);
	}
	m_nDisableCancelFight = 0;
	m_nMaxCrewNum = 0;
	m_pProgressbar_supply = 0;
	m_pLandName = 0;
	m_pPanel_fleet_info = 0;
	m_pShipFlag = 0;
	m_pSmallMap = 0;
	m_pProgressbar_sailor = 0;
	m_pFleetScrollView = 0;
	m_pButtonShipinfo = 0;
	m_pImage_title_bg = 0;
	m_pPanelActionbar = 0;
	m_pFleet_image_title_bg = 0;
	m_bIsLevelUp = false;
	m_bIsPrestigeUp = false;
	m_bIsPrestigeDown = false;
	m_bIsCaptainUp = false;
	m_bIsFailed = false;
	m_nShowChatTime = 0;
	m_nForceFightNpcId = 0;
	m_bIsNewCityCauseLvUp=false;
	m_bIsNewCityCauseFameUp = false;
	m_nChatItem = 0;
	m_nMaxSupply = 0;
	m_vOpenViews.clear();
	m_vCaptainList.clear();
	m_pFightResult=nullptr;
	m_pEndFightResult = nullptr;
	m_pTriggerSeaResult = nullptr;
	m_pDiscoverCityResult = nullptr;
	m_bIsFightCauseLvUp = false;
	m_bIsFightCauseFameLvChange = false;
	m_pIsNewCityAddCaptainExp = false;
	m_pIsFightAddCaptainExp = false;
	m_bIsFirstChat = true;
	m_bIsCity = false;
	m_bIsLoot = false;
	m_bIsNpc = false;
	m_pNpc = nullptr;
	m_pCityResult = nullptr;
	//对话走字
	lenNum = 0;
	lenAfter = 0;
	plusNum = 0;
	chatContent = "";
	anchPic = nullptr;
	chatTxt = nullptr;
	m_bSaillingDiaolgOver = false;
	m_bIsSalvage = false;
	m_bAutoToPersonCenter = false;
	m_nSalvageCost = 0;
	m_pAttactPirateResult = nullptr;
	m_touchEventLayer = nullptr;
	m_CanLoot = false;
	m_confirmType = CONFIRM_INDEX_ACCEPT_ADDFRIEND;
	m_nInviteGuildTag = 0;
	m_operateWidgetTag = 0;
	m_bSalvageClick = true;
	m_robforbid = false;
	m_vSkillDefine.clear();
	m_bInvitedToguildFailed = false;
	m_pSafeAreaLabel = nullptr;
	m_bIsWar = false;
	m_nSaveshipcost = 0;
	m_nCancelfightcost = 0;
	m_eUIType = UI_MAP;
	m_pLeaveCityResult = nullptr;
	m_pMyNationWarResult = nullptr;
}

UISailHUD::~UISailHUD()
{
	m_vOpenViews.clear();
	m_vCaptainList.clear();
	_userData = nullptr;
	delete m_SafeAreaCheck;
	unscheduleAllCallbacks();
	//SINGLE_SHOP->releaseLandData();
	for (int i = 0; i < 19; i++)
	{
		std::string mapName = StringUtils::format("ship/seaFiled_1/map_%d.jpg",i+ 1);
		TextureCache::getInstance()->removeTextureForKey(mapName);
	}
	NotificationCenter::getInstance()->removeAllObservers(this);
	this->unschedule(schedule_selector(UISailHUD::updateMailNumber)); 
	this->unschedule(schedule_selector(UISailHUD::updateBySecond));
	this->unschedule(schedule_selector(UISailHUD::updateNationWarBySecond));
	SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_uicommon0.plist");
	SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_uicommon1.plist");
}

UISailHUD* UISailHUD::create()
{
	UISailHUD* mapui = new UISailHUD;
	mapui->setCameraMask(4);
	if (mapui && mapui->init())
	{
		mapui->autorelease();
		return mapui;
	}
	CC_SAFE_DELETE(mapui);
	return nullptr;
}

bool UISailHUD::init()
{
	bool pRet = false;
	do 
	{
		this->initf(0);
		pRet = true;
	} while (0);
	m_SafeAreaCheck = new SafeAreaCheck();
	NotificationCenter::getInstance()->addObserver(this,callfuncO_selector(UISailHUD::closeMap),"mapHaveClosed_notAuto",NULL);
	return pRet;
}


void UISailHUD::initf(float f)
{
	//海域划分
	//SINGLE_SHOP->loadLandData();
	auto landInfo = SINGLE_SHOP->getLandInfo();
	for (auto i = 0;i < 19;i++)
	{
		m_MapRect[i].setRect(landInfo[i + 1].left_down_x, landInfo[i + 1].left_down_y, landInfo[i + 1].right_up_x - landInfo[i + 1].left_down_x, 3727);
	}

	//海上主界面 摄像机1，底层
	openView(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
	if (SINGLE_HERO->m_forceNPCId>0)
	{
		addLayerForSeaEvent();
	}
	auto m_pSubPanel = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
	auto image_clock = dynamic_cast<ImageView *>(Helper::seekWidgetByName(m_pSubPanel, "image_clock"));
	image_clock->setTouchEnabled(true);
	image_clock->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	image_clock->setTag(IMAGE_ICON_CLOCK + IMAGE_INDEX2);
	auto image_coordinates = dynamic_cast<ImageView *>(Helper::seekWidgetByName(m_pSubPanel, "image_coordinates"));
	image_coordinates->setTouchEnabled(true);
	image_coordinates->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	image_coordinates->setTag(IMAGE_ICON_COORDINATE + IMAGE_INDEX1);
	auto panel_map = m_pSubPanel->getChildByName<Widget*>("panel_map");
	m_pSmallMap = panel_map->getChildByName<ImageView*>("image_map_2_m");
	ImageView *image_cityname_bg = m_pSubPanel->getChildByName<ImageView*>("image_cityname_bg");
	m_pLandName = image_cityname_bg->getChildByName<Text*>("label_map_name");
	m_pPanelActionbar = m_pSubPanel->getChildByName<Widget*>("panel_actionbar");
	auto w_head = m_pPanelActionbar->getChildByName<Button*>("button_head_player_bg");
	auto image_head_lv = w_head->getChildByName<ImageView*>("image_head_lv");
	ImageView* i_head_icon = w_head->getChildByName<ImageView*>("image_head");
	m_playerLv = dynamic_cast<Text*>(image_head_lv->getChildByName<Widget*>("label_lv"));
	m_prestigeLv = dynamic_cast<Text*>(image_head_lv->getChildByName<Widget*>("label_lv_r"));
	m_pImage_title_bg = m_pPanelActionbar->getChildByName<ImageView*>("image_title_bg");
	ImageView* image_progressbar_sailor = m_pImage_title_bg->getChildByName<ImageView*>("image_progressbar_sailor");
	m_pProgressbar_sailor = image_progressbar_sailor->getChildByName<LoadingBar*>("progressbar_durable");

	ImageView* image_progressbar_supply = m_pImage_title_bg->getChildByName<ImageView*>("image_progressbar_supply");
	m_pProgressbar_supply = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_durable");

	auto image_safe_bg = m_pSubPanel->getChildByName<ImageView*>("image_sea_area_warning");
	m_pSafeAreaLabel = image_safe_bg->getChildByName<Text*>("label_warning_name");
	m_pSafeAreaLabel->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAFE_AREA"]);
	m_pSafeAreaLabel->setTextColor(Color4B(17, 222, 31, 255));
	m_pSafeAreaLabel->setTouchEnabled(true);
	m_pSafeAreaLabel->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));

	i_head_icon->ignoreContentAdaptWithSize(false);
	std::string iconPath = StringUtils::format("res/player_icon/icon_%d.png",SINGLE_HERO->m_iIconidx);
	i_head_icon->loadTexture(iconPath);
	m_pButton_warning_1 = m_pImage_title_bg->getChildByName<Button*>("button_warning_1");
	m_pButton_warning_1->setBright(true);
	m_pButton_warning_2 = m_pImage_title_bg->getChildByName<Button*>("button_warning_2");
	m_pButton_warning_2->setBright(true);
	m_pPanelRep = w_head->getChildByName<Widget*>("panel_rep");
	m_pPanelExp = w_head->getChildByName<Widget*>("panel_exp");


	m_pShipFlag = ImageView::create(SHIP_ICON_PATH);
	m_pShipFlag->setScale(1);
	m_pShipFlag->setAnchorPoint(Vec2(0.5,0.5));
	//TODO: fix it via cocostudio.
	m_pSmallMap->setContentSize(Size(724,480));
	m_pSmallMap->addChild(m_pShipFlag);
	m_pSmallMap->setCameraMask(4, true);
	m_pShipFlag->setPosition(Vec2(1000,1000));

	m_playerLv->setString(StringUtils::format("%d", SINGLE_HERO->m_iLevel));
	m_prestigeLv->setString(StringUtils::format("%d", SINGLE_HERO->m_iPrestigeLv));
	this->scheduleUpdate();
	this->schedule(schedule_selector(UISailHUD::mewSound),15.f);

	//海上对话
	flushSeaChat();
	this->schedule(schedule_selector(UISailHUD::updateMailNumber), 5);
	setAllUnreadImage(ProtocolThread::GetInstance()->getLastCheckMailBoxResult());
}

void UISailHUD::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		auto button = dynamic_cast<Widget*>(pSender);
		std::string name = button->getName();
		buttonEvent(button,name);
	}
}

void UISailHUD::onSeaEvent_float(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}

	if (m_bIsBlocking){
		return;
	}
	auto button = dynamic_cast<Button*>(pSender);
	std::string name = button->getName();
	m_bIsBlocking = true;
	shipPause(true);
	ProtocolThread::GetInstance()->triggerSeaEvent(button->getTag(), 0, UILoadingIndicator::createWithMask(this, 4));
}

void UISailHUD::onSeaEvent_npc(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	if (m_bIsBlocking){
		return;
	}
	if (m_bIsSalvage)
	{
		m_bIsNpc = true;
		m_pNpc = pSender;
		openIsStopSalvage();
	}
	else
	{
    auto button = dynamic_cast<UINPCShip*>(pSender);
	int t = button->getEventID();
	m_nDisableCancelFight = 0;
	m_bIsBlocking = true;
	shipPause(true);
	dynamic_cast<UIShip*>(m_pShip)->startPauseTime();
	ProtocolThread::GetInstance()->engageInFight(button->getEventID(), m_nDisableCancelFight, m_nEventversion, FIGHT_TYPE_NORMAL, 0, UILoadingIndicator::createWithMask(this, 4));
	}
	
}
void UISailHUD::onSeaEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView(MAPUI_COCOS_RES[INDEX_UI_FLOAT_CSB]);
		
		if (m_pTriggerSeaResult->type == EV_SEA_FLOAT)
		{
			salvageFloatResult(m_pTriggerSeaResult);
		}
		else
		{
			//海上事件结算
			flushSeaEventView(m_pTriggerSeaResult, seaEventTitle);
		}
	}
}
void UISailHUD::buttonEvent(Widget* target,std::string name,bool isRefresh)
{
	//个人中心
	if (isButton(button_head_player_bg))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (getIsSalvaging())
		{
			openIsStopSalvage();
			return;
		}
		if (dynamic_cast<UIShip*>(m_pShip)->m_IsAutoSailing)
		{ 
			dynamic_cast<UIShip*>(m_pShip)->m_PauseTime = dynamic_cast<UIShip*>(m_pShip)->getCurrentTimeUsev();
			m_bAutoToPersonCenter = true;
			m_nConfirmIndex = UISailHUD::CONFIRM_INDEX_NOTAUTO;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_COMMON_CSB_WHARF_AUTOSEA_TITLE", "TIP_SAILING_NOTAUTO_GOSEA_ASK");
			Director::getInstance()->pause();
			return;
		}
		else
		{
			SINGLE_HERO->m_bSeaToCenter = true;
			CHANGETO(SCENE_TAG::CENTER_TAG);
		}
		return;
	}
	//任务界面
	if (isButton(button_task))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		shipPause(false);
		UITask *t = new UITask;
		t->openTaskLayer(0,this, 4);
		return;
	}
	//邮件
	if (isButton(button_mail))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		shipPause(false);
		UIEmail *e = new UIEmail;
		e->openEmailView(this, 4);
		return;
	}
	//停止航行
	if (isButton(button_stopping))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (!m_pShip)
		{
			return;
		}
		dynamic_cast<UIShip*>(m_pShip)->stopShip(true);
	}

	//小地图
	if (isButton(button_map))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UIMap*mainMap = UIMap::createMap(2);
		mainMap->setCameraMask(4, true);
		mainMap->setLocalZOrder(100);
		this->addChild(mainMap);
		if (dynamic_cast<UIShip*>(m_pShip)->m_IsAutoSailing){
			SINGLE_HERO->m_bShipStateIsAuto=true;
		}
		else{
			SINGLE_HERO->m_bShipStateIsAuto=false;
			dynamic_cast<UIShip*>(m_pShip)->startPauseTime();
			
		}
		return;
	}
	

	//关闭界面
	if (isButton(button_close) || isButton(button_back) || isButton(button_salvage_no))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bIsBlocking = false;
		shipPause(false);
		closeView();
		return;
	}
	//打捞背包已满确定按钮
	if (isButton(button_error_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (!m_bInvitedToguildFailed)
		{
			m_bIsBlocking = false;
			shipPause(false);
		}
		m_bInvitedToguildFailed = false;
		closeView(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
		return;
	}
	
	//打捞花费
	if (isButton(button_salvage_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->startSalvage(m_pShip->getPositionX(), m_pShip->getPositionY(), UILoadingIndicator::createWithMask(this, 4));
		return;
	}

	//打捞
	if (isButton(button_salvage) && m_bSalvageClick)
	{
		m_bSalvageClick = false;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bIsSalvage)
		{
			Director::getInstance()->pause();
			m_nConfirmIndex = CONFIRM_INDEX_INTERRUPT_SALVAGE;
			shipPause(false);
			openCheifChat();
		}
		else
		{
			m_bIsSalvage = true;
			//是否是自动航行
			if (dynamic_cast<UIShip*>(m_pShip)->m_IsAutoSailing)
			{
				dynamic_cast<UIShip*>(m_pShip)->stopShip(true);
			}
			else
			{
				ProtocolThread::GetInstance()->getSalvageCondition(m_pShip->getPositionX(), m_pShip->getPositionY(), UILoadingIndicator::createWithMask(this, 4));
				shipPause(true);
				dynamic_cast<UIShip*>(m_pShip)->stopShip(true);
			}
		}
		return;
	}
	//关闭船只信息界面
	if (isButton(button_subclose))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
	//关闭开战界面
	if (isButton(button_challenge_close))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//SINGLE_AUDIO->vresumeBGMusic();
		if (m_nDisableCancelFight == 0)
		{		
			ProtocolThread::GetInstance()->cancelFight(0,0, UILoadingIndicator::createWithMask(this, 4));
		}
		else if (m_nDisableCancelFight == 1)
		{
			openView(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB], 11);
			auto w_taskConfirm = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_ESCORT_CSB]);
			auto t_title = w_taskConfirm->getChildByName<Text*>("label_repair");
			auto t_content = w_taskConfirm->getChildByName<Text*>("label_content");
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_NPC_BATTLE_FLEE_TITLE"]);
			t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_NPC_BATTLE_FLEE_CONTENT"]);
		}
		else
		{
			openView(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
			auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
			auto t_title = view->getChildByName<Text*>("label_advanced_repair");
			auto t_content = view->getChildByName<Text*>("label_repair_content");
			auto lv_costNum = view->getChildByName<ListView*>("listview_cost");
			auto w_costNum = lv_costNum->getItem(1);
			auto t_costNum = w_costNum->getChildByName<Text*>("label_cost_num");
			std::string s_cost = numSegment(StringUtils::format("%d", m_pFightResult->cancelfightcost));
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_NPC_BATTLE_RANSOM_TITLE"]);
			t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_NPC_BATTLE_RANSOM_CONTENT"]);
			t_costNum->setString(s_cost);
			auto image_silver_1 = w_costNum->getChildByName<ImageView*>("image_silver_1");
			image_silver_1->setPositionX(t_costNum->getPositionX() -
				t_costNum->getBoundingBox().size.width - image_silver_1->getBoundingBox().size.width * 0.7);
		}
		return;
	}
	
	if (isButton(button_s_yes))
	{
		//增加背包容量确认
		if (m_nConfirmIndex == CONFIRM_INDEX_CANCEL_OVER_WEIGHT)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			ProtocolThread::GetInstance()->expandPackageSize(UILoadingIndicator::createWithMask(this, 4));
			return;
		}

		//使用停战道具
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto npcId = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(NPC_ID).c_str(), -1);
		//正常战斗(新手引导除外)
		if (npcId == m_pFightResult->npcid && (m_pFightResult->fighttype == FIGHT_TYPE_NORMAL || m_pFightResult->fighttype == FIGHT_TYPE_ROB) && !ProtocolThread::GetInstance()->isInTutorial())
		{
			int myShipNum = 0;
			EndFightShipInfo **myships = new EndFightShipInfo*[m_pFightResult->myinfo->shipnum];
			for (size_t i = 0; i < m_pFightResult->myinfo->shipnum; i++)
			{
				auto st_my_sailor = StringUtils::format(MY_SHIP_SAILOR, i + 1);
				auto st_my_blood = StringUtils::format(MY_SHIP_BLOOD, i + 1);
				auto my_sailor = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_my_sailor.c_str()).c_str(), 0);
				auto my_blood = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_my_blood.c_str()).c_str(), 0);
				if (my_sailor > 0 && my_blood > 0)
				{
					myships[myShipNum] = new EndFightShipInfo;
					end_fight_ship_info__init(myships[myShipNum]);
					myships[myShipNum]->hp = my_blood;
					myships[myShipNum]->sailornum = my_sailor;
					myships[myShipNum]->shipid = m_pFightResult->myships[i]->shipid;
					myShipNum++;
				}
			}
			EndFight *endFight = new EndFight;
			end_fight__init(endFight);
			endFight->reason = BATTLE_FLEE;
			endFight->n_myships = myShipNum;
			endFight->myships = myships;
			endFight->n_usedprops = m_pFightResult->n_props;
			for (size_t i = 0; i < m_pFightResult->n_props; i++)
			{
				auto st_prop = StringUtils::format(PROP_NUM, m_pFightResult->props[i]->propid);
				auto hex = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName(st_prop.c_str()).c_str());
				char *st_num = 0;
				//解密
				if (hex.length() > 0)
				{
					st_num = getStringFromEncryptedHex((char*)hex.c_str());
				}
				int num = 0;
				if (st_num)
				{
					num = atoi(st_num);
					free(st_num);
				}
				m_pFightResult->props[i]->num = num;
			}
			endFight->usedprops = m_pFightResult->props;
			auto morale = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(MY_SHIP_MORALE).c_str(), 0);
			endFight->morale = morale;
			endFight->fighttype = m_pFightResult->fighttype;
			endFight->isautofight = 0;
			auto remainTime = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(FIGHT_TIME).c_str(), 0);
			endFight->remainsec = remainTime;
			auto catchshipnum = 0;
			auto catchgoodsshipnum = 0;
			auto catchshipids = new int[10];
			auto catchgoodsshipids = new int[10];
			for (size_t i = 1; i <= 10; i++)
			{
				auto capturing_ship = StringUtils::format(CAPTURINGSHIPID, i);
				auto hex = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName(capturing_ship.c_str()).c_str());
				char *st_num = 0;
				//解密
				if (hex.length() > 0)
				{
					st_num = getStringFromEncryptedHex((char*)hex.c_str());
				}
				int shipid = 0;
				if (st_num)
				{
					shipid = atoi(st_num);
					free(st_num);
				}
				if (shipid > 0)
				{
					catchshipids[catchshipnum] = shipid;
					catchshipnum++;
				}

				auto food_plunder = StringUtils::format(FOODPLUNDERSHIPID, i);
				auto hex2 = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName(food_plunder.c_str()).c_str());
				char *st_num2 = 0;
				//解密
				if (hex2.length() > 0)
				{
					st_num2 = getStringFromEncryptedHex((char*)hex2.c_str());
				}
				int shipid2 = 0;
				if (st_num2)
				{
					shipid2 = atoi(st_num2);
					free(st_num2);
				}
				if (shipid2 > 0)
				{
					catchgoodsshipids[catchshipnum] = shipid2;
					catchgoodsshipnum++;
				}
			}

			ProtocolThread::GetInstance()->cancelFight(1, endFight, UILoadingIndicator::createWithMask(this, 4));
			for (size_t i = 0; i < myShipNum; i++)
			{
				delete myships[i];
			}
			delete myships;
			delete endFight;
			delete[]catchshipids;
			delete[]catchgoodsshipids;
		}
		else
		{
			ProtocolThread::GetInstance()->cancelFight(1, 0, UILoadingIndicator::createWithMask(this, 4));
		}
		return;
	}
	//花费V票
	if (isButton(button_v_yes))
	{
		if (m_nDisableCancelFight == 2)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			auto npcId = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(NPC_ID).c_str(), -1);
			//正常战斗(新手引导除外)
			if (npcId == m_pFightResult->npcid && (m_pFightResult->fighttype == FIGHT_TYPE_NORMAL || m_pFightResult->fighttype == FIGHT_TYPE_ROB) && !ProtocolThread::GetInstance()->isInTutorial())
			{
				int myShipNum = 0;
				EndFightShipInfo **myships = new EndFightShipInfo*[m_pFightResult->myinfo->shipnum];
				for (size_t i = 0; i < m_pFightResult->myinfo->shipnum; i++)
				{
					auto st_my_sailor = StringUtils::format(MY_SHIP_SAILOR, i + 1);
					auto st_my_blood = StringUtils::format(MY_SHIP_BLOOD, i + 1);
					auto my_sailor = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_my_sailor.c_str()).c_str(), 0);
					auto my_blood = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_my_blood.c_str()).c_str(), 0);
					if (my_sailor > 0 && my_blood > 0)
					{
						myships[myShipNum] = new EndFightShipInfo;
						end_fight_ship_info__init(myships[myShipNum]);
						myships[myShipNum]->hp = my_blood;
						myships[myShipNum]->sailornum = my_sailor;
						myships[myShipNum]->shipid = m_pFightResult->myships[i]->shipid;
						myShipNum++;
					}
				}
				EndFight *endFight = new EndFight;
				end_fight__init(endFight);
				endFight->reason = BATTLE_FLEE;
				endFight->n_myships = myShipNum;
				endFight->myships = myships;
				endFight->n_usedprops = m_pFightResult->n_props;
				for (size_t i = 0; i < m_pFightResult->n_props; i++)
				{
					auto st_prop = StringUtils::format(PROP_NUM, m_pFightResult->props[i]->propid);
					auto hex = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName(st_prop.c_str()).c_str());
					char *st_num = 0;
					//解密
					if (hex.length() > 0)
					{
						st_num = getStringFromEncryptedHex((char*)hex.c_str());
					}
					int num = 0;
					if (st_num)
					{
						num = atoi(st_num);
						free(st_num);
					}
					m_pFightResult->props[i]->num = num;
				}
				endFight->usedprops = m_pFightResult->props;
				auto morale = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(MY_SHIP_MORALE).c_str(), 0);
				endFight->morale = morale;
				endFight->fighttype = m_pFightResult->fighttype;
				endFight->isautofight = 0;
				auto remainTime = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(FIGHT_TIME).c_str(), 0);
				endFight->remainsec = remainTime;
				auto catchshipnum = 0;
				auto catchgoodsshipnum = 0;
				auto catchshipids = new int[10];
				auto catchgoodsshipids = new int[10];
				for (size_t i = 1; i <= 10; i++)
				{
					auto capturing_ship = StringUtils::format(CAPTURINGSHIPID, i);
					auto hex = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName(capturing_ship.c_str()).c_str());
					char *st_num = 0;
					//解密
					if (hex.length() > 0)
					{
						st_num = getStringFromEncryptedHex((char*)hex.c_str());
					}
					int shipid = 0;
					if (st_num)
					{
						shipid = atoi(st_num);
						free(st_num);
					}
					if (shipid > 0)
					{
						catchshipids[catchshipnum] = shipid;
						catchshipnum++;
					}

					auto food_plunder = StringUtils::format(FOODPLUNDERSHIPID, i);
					auto hex2 = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName(food_plunder.c_str()).c_str());
					char *st_num2 = 0;
					//解密
					if (hex2.length() > 0)
					{
						st_num2 = getStringFromEncryptedHex((char*)hex2.c_str());
					}
					int shipid2 = 0;
					if (st_num2)
					{
						shipid2 = atoi(st_num2);
						free(st_num2);
					}
					if (shipid2 > 0)
					{
						catchgoodsshipids[catchshipnum] = shipid2;
						catchgoodsshipnum++;
					}
				}
				endFight->n_catchshipids = catchshipnum;
				endFight->catchshipids = catchshipids;
				endFight->n_catchgoodsshipids = catchgoodsshipnum;
				endFight->catchgoodsshipids = catchgoodsshipids;
				ProtocolThread::GetInstance()->cancelFight(2, endFight, UILoadingIndicator::createWithMask(this, 4));
			
				for (size_t i = 0; i < myShipNum; i++)
				{
					delete myships[i];
				}
				delete myships;
				delete endFight;
				delete[]catchshipids;
				delete[]catchgoodsshipids;
			}
			else
			{
				ProtocolThread::GetInstance()->cancelFight(2, 0, UILoadingIndicator::createWithMask(this, 4));
			}
		}
		else if (m_nDisableCancelFight == 4)
		{
			if (m_pFightResult)
			{
				ProtocolThread::GetInstance()->endFailedFightByVTicket(1, m_pFightResult->fighttype, UILoadingIndicator::createWithMask(this, 4));
			}
			else
			{
				ProtocolThread::GetInstance()->endFailedFightByVTicket(1, FIGHT_TYPE_NORMAL, UILoadingIndicator::createWithMask(this, 4));
			}
		}
		else
		{
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
		}
		return;
	}

	if (isButton(button_v_no) || isButton(button_s_no))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (CONFIRM_INDEX_CANCEL_V == m_nConfirmIndex)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_RANSOM", "ARE_YOU_OK");
		}
		else if (CONFIRM_INDEX_CANCEL_OVER_WEIGHT == m_nConfirmIndex)
		{
			//不做操作
		}
		else
		{
			closeView();
			if (m_nDisableCancelFight == 4 || m_nDisableCancelFight == 5)
			{
				if (m_pFightResult)
				{
					ProtocolThread::GetInstance()->endFailedFightByVTicket(0, m_pFightResult->fighttype, UILoadingIndicator::createWithMask(this, 4));
				}
				else
				{
					ProtocolThread::GetInstance()->endFailedFightByVTicket(0, FIGHT_TYPE_NORMAL, UILoadingIndicator::createWithMask(this, 4));
				}
			}
		}
		
		return;
	}
	
	//打开开始战斗界面
	if (isButton(button_battle))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (!target)
		{
			return;
		}
		if (m_pShip)
		{
			closeView();
			openView(MAPUI_COCOS_RES[INDEX_UI_BATTLE_VS_CSB]);
			auto viewFight = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BATTLE_VS_CSB]);
			viewFight->setCameraMask(4,true);
			flushBattleVS(viewFight,m_pFightResult);
		}
		return;
	}
	//开战
	if (isButton(button_start_battle))
	{
		closeView();
		// sound effect  fight
		removeLayerForSeaEvent();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_IN_COMBAT_17);
		openFightEvent();
		return;
	}
	//取消
	if (isButton(button_cancel))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
	//确认
	if (isButton(button_ok))
	{
		closeView();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_IN_COMBAT_17);
		openFightEvent();
		return;
	}
	//战斗胜利确认
	if (isButton(button_victory_ok))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
	//战斗失败确认
	if (isButton(button_defeated_ok))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		return;
	}
	//关闭界面
	if (isButton(button_draw_ok))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
	//自动战斗
	if (isButton(button_automatic))
	{
		auto npcId = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(NPC_ID).c_str(), -1);
		//正常战斗(新手引导除外)
		if (npcId == m_pFightResult->npcid && (m_pFightResult->fighttype == FIGHT_TYPE_NORMAL || m_pFightResult->fighttype == FIGHT_TYPE_ROB) && !ProtocolThread::GetInstance()->isInTutorial())
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SAIL_AUTO_FIGHTING");
		}
		else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_IN_COMBAT_17);
			//SINGLE_AUDIO->vresumeBGMusic();
			removeLayerForSeaEvent();
			ProtocolThread::GetInstance()->startAutoFight(UILoadingIndicator::createWithMask(this, 4));
		}
		
		return;
	}
	
	//confirm if enter city
	//登陆确认
	if (isButton(button_confirm_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		
		if (m_operateWidgetTag == 0  && m_nInviteGuildTag == 0)
		{
			if (CONFIRM_INDEX_CANCEL_V == m_nConfirmIndex)
			{
				if (m_nDisableCancelFight == 4 || m_nDisableCancelFight == 5)
				{
					if (m_pFightResult)
					{
						ProtocolThread::GetInstance()->endFailedFightByVTicket(0, m_pFightResult->fighttype, UILoadingIndicator::createWithMask(this, 4));
					}
					else
					{
						ProtocolThread::GetInstance()->endFailedFightByVTicket(0, FIGHT_TYPE_NORMAL, UILoadingIndicator::createWithMask(this, 4));
					}
				}
				return;
			}

			if (CONFIRM_INDEX_INTOCITY == m_nConfirmIndex){

				ProtocolThread::GetInstance()->reachCity(SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].x, MAP_CELL_WIDTH* MAP_WIDTH - SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].y, UILoadingIndicator::createWithMask(this, 4));
				return;
			}
			if (CONFIRM_INDEX_NOTAUTO == m_nConfirmIndex){

				dynamic_cast<UIShip*>(m_pShip)->isContinueAutoSailing(true);
				if (m_bAutoToPersonCenter)
				{
					m_bAutoToPersonCenter = false;
					SINGLE_HERO->m_bSeaToCenter = true;
					CHANGETO(SCENE_TAG::CENTER_TAG);
				}
				if (m_bIsSalvage)
				{
					ProtocolThread::GetInstance()->getSalvageCondition(m_pShip->getPositionX(), m_pShip->getPositionY(), UILoadingIndicator::createWithMask(this, 4));
					m_bIsBlocking = true;
					shipPause(true);
				}
				return;
			}

			if (CONFIRM_INDEX_INTERRUPT_SALVAGE == m_nConfirmIndex)
			{
				m_bIsSalvage = false;
				Director::getInstance()->resume();
				auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
				auto b_salvage = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_salvage"));
				b_salvage->setBright(true);
				shipPause(false);

				dynamic_cast<UIShip*>(m_pShip)->setFalgVisible(true);
				dynamic_cast<UIShip*>(m_pShip)->removeChildByTag(111);
				dynamic_cast<UIShip*>(m_pShip)->removeChildByTag(222);
				dynamic_cast<UIShip*>(m_pShip)->removeChildByTag(333);
				m_pShip->getChildByName("salvage_dh")->removeFromParentAndCleanup(true);
				return;
			}
			if (CONFIRM_INDEX_START_ROB == m_nConfirmIndex)
			{
				Director::getInstance()->resume();
				dynamic_cast<UIShip*>(m_pShip)->isContinueAutoSailing(true);
				m_bIsBlocking = false;
				shipPause(true);
				ProtocolThread::GetInstance()->findLootPlayer(UILoadingIndicator::createWithMask(this, 4));
				return;
			}
			if (CONFIRM_INDEX_GET_PLAYERINFO == m_nConfirmIndex)
			{
				Director::getInstance()->resume();
				dynamic_cast<UIShip*>(m_pShip)->isContinueAutoSailing(true);
				m_bIsBlocking = false;
				shipPause(true);			
				ProtocolThread::GetInstance()->getUserInfoById(playerInfoId,UILoadingIndicator::createWithMask(this, 4));
				return;
			}
		}
		else
		{
			shipPause(false);
			if (m_confirmType == CONFIRM_INDEX_SOCIAL_INVITE_GUILD)
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
				ProtocolThread::GetInstance()->inviteUserToGuild(m_nInviteGuildTag, UILoadingIndicator::create(this));
				m_nInviteGuildTag = 0;
			}
			else
			{
				//拉黑
				if (m_confirmType == CONFIRM_INDEX_FRIEND_BLOCK || m_confirmType == CONFIRM_INDEX_STRANGER_BLOCK)
				{
					ProtocolThread::GetInstance()->friendsOperation(m_operateWidgetTag, 2, UILoadingIndicator::create(this));
					m_operateWidgetTag = 0;
				}
				//删除好友
				else if (m_confirmType == CONFIRM_INDEX_REMOVEFRIEND)
				{
					ProtocolThread::GetInstance()->friendsOperation(m_operateWidgetTag, 1, UILoadingIndicator::create(this));
					m_operateWidgetTag = 0;
				}
				else if (m_confirmType == CONFIRM_INDEX_BLOCK_ADDFRIEND || m_confirmType == CONFIRM_INDEX_STRANGER_ADDFRIEND)
				{
					ProtocolThread::GetInstance()->friendsOperation(m_operateWidgetTag, 0, UILoadingIndicator::create(this));
					m_operateWidgetTag = 0;
				}
				//移除黑名单
				else if (m_confirmType == CONFIRM_INDEX_BLOCK_REMOVE)
				{
					ProtocolThread::GetInstance()->friendsOperation(m_operateWidgetTag, 5, UILoadingIndicator::create(this));
					m_operateWidgetTag = 0;
				}
			}
		}
		shipPause(false);
		return;
	}
	//confirm if enter city
	//登录取消
	if (isButton(button_confirm_no))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bSalvageClick = true;
		if (m_operateWidgetTag > 0)
		{
			m_operateWidgetTag = 0;
			return;
		}
		if (CONFIRM_INDEX_NOTAUTO == m_nConfirmIndex){

			dynamic_cast<UIShip*>(m_pShip)->isContinueAutoSailing(false);
			auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
			auto b_salvage = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_salvage"));
			b_salvage->setBright(true);
			m_bIsSalvage = false;
			return;
		}

		if (CONFIRM_INDEX_INTERRUPT_SALVAGE == m_nConfirmIndex)
		{
			dynamic_cast<UIShip*>(m_pShip)->isContinueAutoSailing(false);
			return;
		}

		if (CONFIRM_INDEX_START_ROB == m_nConfirmIndex)
		{
			Director::getInstance()->resume();
			shipPause(false);
			return;
		}

		if (CONFIRM_INDEX_GET_PLAYERINFO == m_nConfirmIndex)
		{
			Director::getInstance()->resume();
			shipPause(false);
			return;
		}
		return;
	}
	//聊天
	if (isButton(button_social))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->openSocialLayer();
		UISocial::getInstance()->setChatCall();
		return;
	}
	//聊天
	if (isButton(button_chat))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->openSocialLayer();
		UISocial::getInstance()->setChatCall();
		UISocial::getInstance()->showChat(4);
		return;
	}
	//船队信息
	if (isButton(button_fleetinfo))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (dynamic_cast<UIShip*>(m_pShip)->m_IsAutoSailing){

		}
		else{

			dynamic_cast<UIShip*>(m_pShip)->startPauseTime();

		}

		dynamic_cast<UISailManage*>(_parent)->mapUICall_func(name);

		return;
	}
	//船只信息
	if (isButton(button_shipinfo))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//舰队信息
		openView(MAPUI_COCOS_RES[INDEX_UI_SHIPDETAILS_CSB]);
		initShipDetails(target->getTag());
		return;
	}
	//弹出界面确认按钮
	if (isButton(button_result_yes))
	{
		//SINGLE_AUDIO->vresumeBGMusic();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		shipPause(false);
		//水手为0是，立即死亡
		if (m_nCurCrewNum <= 0)
		{
			dynamic_cast<UISailManage*>(_parent)->openFailureDialog();
			dynamic_cast<UIShip*>(m_pShip)->stopShip(true);
			dynamic_cast<UIShip*>(m_pShip)->unscheduleUpdate();
			dynamic_cast<UIShip*>(m_pShip)->stopAllActions();
			return;
		}
		if (m_bIsLevelUp)
		{
			m_bIsLevelUp = false;
			dynamic_cast<UIShip*>(m_pShip)->m_autoSailInterrupt = false;
			shipPause(true);
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPlayerLevelUp();
			return;
		}

		if (m_bIsPrestigeUp)
		{
			m_bIsPrestigeUp = false;
			dynamic_cast<UIShip*>(m_pShip)->m_autoSailInterrupt = false;
			shipPause(true);
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();		
			return;
		}

		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			dynamic_cast<UIShip*>(m_pShip)->m_autoSailInterrupt = false;
			shipPause(true);
			//发现新城市增加船长经验
			if (m_pDiscoverCityResult)
			{
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->getCaptainAddexp();
				UICommon::getInstance()->flushCaptainLevelUp(m_pDiscoverCityResult->n_captains,m_pDiscoverCityResult->captains);
			}
			return;
		}

		if(dynamic_cast<UIShip*>(m_pShip)->m_autoSailInterrupt){
			dynamic_cast<UIShip*>(m_pShip)->m_autoSailInterrupt = false;
			dynamic_cast<UIShip*>(m_pShip)->m_IsAutoSailing = true;
			dynamic_cast<UIShip*>(m_pShip)->autoSail();
			m_nConfirmIndex = UISailHUD::CONFIRM_INDEX_NOTAUTO;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_COMMON_CSB_WHARF_AUTOSEA_TITLE", "TIP_SAILING_NOTAUTO_GOSEA_ASK");
			Director::getInstance()->pause();
		}else{

		}
		return;
	}
	//登陆
	if (isButton(button_landCity))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		
		dynamic_cast<UISailManage*>(_parent)->GoBackCity();
		return;

	}
	//登陆
	if (isButton(button_land))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		ProtocolThread::GetInstance()->reachCity(SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].x, MAP_CELL_WIDTH* MAP_WIDTH - SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].y, UILoadingIndicator::createWithMask(this, 4));
		return;
	}
	//警告
	if (isButton(button_warning_1))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		bool isRedWaring = !dynamic_cast<Button*>(target)->isBright();
		
		if (isRedWaring)
		{
			UICommon::getInstance()->flushWarning(SUPPLY_URGENT_WARNING);
		}
		else
		{
			UICommon::getInstance()->flushWarning(SUPPLY_WARNING);
		}
		return;
	}
	//警告
	if (isButton(button_warning_2))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		bool isRedWaring = !dynamic_cast<Button*>(target)->isBright();

		if (isRedWaring)
		{
			UICommon::getInstance()->flushWarning(SAILOR_URGENT_WARNING);
		}
		else
		{
			UICommon::getInstance()->flushWarning(SAILOR_WARNING);
		}
		return;

	}
	//物品详情
	if (isButton(image_material_bg))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openGoodInfo(nullptr,5,target->getTag());
		return;
	}
	//继续
	if (isButton(button_continue_1))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		if (m_bIsLevelUp)
		{
			m_bIsLevelUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPlayerLevelUp();
			return;
		}
		if (m_bIsPrestigeUp)
		{
			m_bIsPrestigeUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
			return;
		}
		if (m_bIsPrestigeDown)
		{
			m_bIsPrestigeDown = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelDown();
			return;
		}
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			//战斗增加船长经验
			if (m_pIsFightAddCaptainExp)
			{
				
				if (!m_pEndFightResult) return;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->getCaptainAddexp(CAPTAIN_EXPUI::CAPTAIN_EXPUI_BATTLE);
				//战斗引起升级
				UICommon::getInstance()->flushCaptainLevelUp(m_pEndFightResult->n_captains,m_pEndFightResult->captains);
			}
			//发现新城市增加经验
			if (m_pIsNewCityAddCaptainExp)
			{
				m_pIsNewCityAddCaptainExp = false;
				if (!m_pDiscoverCityResult) return;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->getCaptainAddexp();
				//发现新城市引起升级
				UICommon::getInstance()->flushCaptainLevelUp(m_pDiscoverCityResult->n_captains,m_pDiscoverCityResult->captains);
			}
			return;
		}
		//当都没有升级时，打开第二个结算面
		flushBattleResult2(m_pEndFightResult);
		return;
	}
	//角色升级
	if (isButton(panel_levelup))
	{  
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		shipPause(false);
		if (m_bIsPrestigeUp || dynamic_cast<UISailManage*>(_parent)->m_HeroFameLevelUp)
		{
			m_bIsPrestigeUp = false;
			dynamic_cast<UISailManage*>(_parent)->m_HeroFameLevelUp = false;
			shipPause(true);
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
			return;
		}
		if (m_bIsPrestigeDown)
		{
			m_bIsPrestigeDown = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelDown();
			return;
		}
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			//战斗增加船长经验
			if (m_pIsFightAddCaptainExp)
			{
				if (!m_pEndFightResult) return;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->getCaptainAddexp(CAPTAIN_EXPUI::CAPTAIN_EXPUI_BATTLE);
				//战斗引起升级
				UICommon::getInstance()->flushCaptainLevelUp(m_pEndFightResult->n_captains,m_pEndFightResult->captains);
			}
			//发现新城市增加经验
			if (m_pIsNewCityAddCaptainExp)
			{
				m_pIsNewCityAddCaptainExp = false;
				if (!m_pDiscoverCityResult) return;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->getCaptainAddexp();
				//发现新城市引起升级
				UICommon::getInstance()->flushCaptainLevelUp(m_pDiscoverCityResult->n_captains,m_pDiscoverCityResult->captains);
			}
			return;
		}
		//升级完后显示战斗结算第二个界面
		if (m_bIsFightCauseLvUp)
		{
			m_bIsFightCauseLvUp = false;
			flushBattleResult2(m_pEndFightResult);
			return;
		}
	}
	//角色升级 角色降级
	if (isButton(panel_r_levelup)||isButton(panel_r_leveldown))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		shipPause(false);
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			//战斗增加船长经验
			if (m_pIsFightAddCaptainExp)
			{
				if (!m_pEndFightResult) return;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->getCaptainAddexp(CAPTAIN_EXPUI::CAPTAIN_EXPUI_BATTLE);
				//战斗引起升级
				UICommon::getInstance()->flushCaptainLevelUp(m_pEndFightResult->n_captains,m_pEndFightResult->captains);
			}
			//发现新城市增加经验
			if (m_pIsNewCityAddCaptainExp)
			{
				m_pIsNewCityAddCaptainExp = false;
				if (!m_pDiscoverCityResult) return;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->getCaptainAddexp();
				//发现新城市引起升级
				UICommon::getInstance()->flushCaptainLevelUp(m_pDiscoverCityResult->n_captains,m_pDiscoverCityResult->captains);
			}
			return;
		}
		//升级完后显示战斗结算第二个界面
		if (m_bIsFightCauseFameLvChange)
		{
			m_bIsFightCauseFameLvChange = false;
			flushBattleResult2(m_pEndFightResult);
			return;
		}
	}
	//船长经验增加
	if (isButton(panel_captain_addexp)||isButton(button_skip))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//战斗完显示
		if (m_pIsFightAddCaptainExp)
		{
			m_pIsFightAddCaptainExp = false;
			flushBattleResult2(m_pEndFightResult);
			return;
		}

		//登陆城市时显示
		if (dynamic_cast<UISailManage*>(_parent)->m_bCaptainToHeroUI)
		{
			dynamic_cast<UISailManage*>(_parent)->m_bCaptainToHeroUI = false;
			//登录城市时经验或者声望升级
			if (dynamic_cast<UISailManage*>(_parent)->m_HeroExpLevelUp)
			{
				dynamic_cast<UISailManage*>(_parent)->m_HeroExpLevelUp = false;
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->flushPlayerLevelUp();
			}
			if (dynamic_cast<UISailManage*>(_parent)->m_HeroFameLevelUp)
			{
				if (!(dynamic_cast<UISailManage*>(_parent)->m_HeroExpLevelUp))
				{
					dynamic_cast<UISailManage*>(_parent)->m_HeroFameLevelUp = false;
					UICommon::getInstance()->openCommonView(this);
					UICommon::getInstance()->flushPrestigeLevelUp();
				}
			}
			dynamic_cast<UISailManage*>(_parent)->openCountUI(dynamic_cast<UISailManage*>(_parent)->m_pReachCityResult);
			return;
		}
		if (m_pIsNewCityAddCaptainExp)
		{
			m_pIsNewCityAddCaptainExp = false;
			shipPause(false);
		}
		return;
	}
	//继续
	if (isButton(button_continue_2))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bIsBlocking = false;
		if (m_bIsFailed)
		{
			openSaillingDialog(2, m_pEndFightResult->lostcoin);
		}else
		{
			shipPause(false);
			dynamic_cast<UIShip*>(m_pShip)->stopPauseTime();
			closeView();
		}
		return;
	}
	//发现新城市
	if (isButton(button_findnew_city)||isButton(panel_found_newcity))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->addDiscoveredCity(m_nFindcityId,UILoadingIndicator::createWithMask(this,4));
		closeView();
		return;
	}
	//技能详情
	if (isButton(image_skill_bg))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		auto index = target->getTag();
		UICommon::getInstance()->flushSkillView(m_vSkillDefine.at(index));
		return;
	}
	//技能详情
	if (isButton(button_equip_bg_))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto t_skill_lv = target->getChildByName<Text*>("text_item_skill_lv");
		UICommon::getInstance()->openCommonView(this);
		SKILL_DEFINE skillDefine;
		skillDefine.id = target->getTag() - START_INDEX;
		skillDefine.lv = atoi(t_skill_lv->getString().data());
		skillDefine.skill_type = target->getParent()->getTag();
		skillDefine.icon_id = t_skill_lv->getTag();
		UICommon::getInstance()->flushSkillView(skillDefine);
		return;
	}

	//小伙伴特殊技能
	if (isButton(button_sp_equip_bg_))
	{
		auto image_lock = target->getChildByName<ImageView*>("image_lock");
		UICommon::getInstance()->openCommonView(this);
		SKILL_DEFINE skillDefine;
		skillDefine.id = target->getTag() - START_INDEX;
		skillDefine.lv = 0;
		skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
		skillDefine.icon_id = image_lock->getTag();
		UICommon::getInstance()->flushSkillView(skillDefine);
	}
	//船队阵亡对话
	if (isButton(Panel_faildialog_root))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bSaillingDiaolgOver)
		{
			ProtocolThread::GetInstance()->reachCity(-1, -1, UILoadingIndicator::createWithMask(this, 4));
			dynamic_cast<UIBasicLayer*>(this->getParent()->getParent())->button_callBack();
		}
		else
		{
			chatTxt->setString(chatContent);
			this->unschedule(schedule_selector(UISailHUD::showSaillingFailedText));
			m_bSaillingDiaolgOver = true;
			anchPic->setVisible(true);
		}
		return;
	}

	//攻击海盗基地
	if (isButton(button_attact_pirate))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->getAttackPirateInfo(SINGLE_HERO->m_nBossEventId, UILoadingIndicator::createWithMask(this, 4));
		return;
	}
	
	//打劫
	if (isButton(button_outlook) || isButton(button_refresh))
	{
		if (m_bIsSalvage)
		{
			m_bIsLoot = true;
			openIsStopSalvage();
		}
		else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			if (dynamic_cast<UIShip*>(m_pShip)->m_IsAutoSailing)
			{
				shipPause(true);
				dynamic_cast<UIShip*>(m_pShip)->stopShip(true);
				m_nConfirmIndex = CONFIRM_INDEX_START_ROB;
			}
			else
			{
				shipPause(true);
				dynamic_cast<UIShip*>(m_pShip)->stopShip(true);
				ProtocolThread::GetInstance()->findLootPlayer(UILoadingIndicator::createWithMask(this, 4));

			}
		}
		return;
	}
	if (isButton(button_start_rob))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_robforbid)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_LOOT_PLAYER_FORBID");
		}
		else
		{
			ProtocolThread::GetInstance()->lootPlayer(UILoadingIndicator::createWithMask(this, 4));
		}	
		return;
	}

	//关闭开战界面
	if (isButton(button_quit))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		ProtocolThread::GetInstance()->cancelFight(0, 0,UILoadingIndicator::createWithMask(this, 4));
		shipPause(false);
		return;
	}
	//工会邀请
	if (isButton(button_invite))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nInviteGuildTag = target->getTag();
		m_confirmType = CONFIRM_INDEX_SOCIAL_INVITE_GUILD;
		if (SINGLE_HERO->m_InvitedToGuildNation == SINGLE_HERO->m_iNation)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_INVITE_GUILD_TITLE", "TIP_GUILD_INVINT_GUILD");
		}
		else
		{
			m_bInvitedToguildFailed = true;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_GUILD_INVITED_FAILED_NOT_SAME_COUNTRY_CONTENT");
		}
		
		return;
	}
	//添加好友
	//加为好友
	if (isButton(button_add_friend)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_confirmType = CONFIRM_INDEX_STRANGER_ADDFRIEND;
		m_operateWidgetTag = target->getParent()->getTag();
		if (target->getParent()->getTag() == SINGLE_HERO->m_iID)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_NOT_ADD_FRIENDED");
		}
		else if (target->getTag() == 0)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_AGAIN_ADD_FRIENDS");
		}
		else if (target->getTag() == 1)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_ALREADY_GOOD_FRIEND");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_ADD_FRIEND_TITLE", "TIP_SOCIAL_ADD_FRIEND");
		}
		return;
	}
	//删除好友
	if (isButton(button_friend_delete))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_confirmType = CONFIRM_INDEX_REMOVEFRIEND;
		m_operateWidgetTag = target->getParent()->getTag();
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_DELETE_FRIEND_TITLE", "TIP_SOCIAL_DELETE_FRIEND");
		return;
	}
	//拉黑玩家
	if (isButton(button_friend_block))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_confirmType = CONFIRM_INDEX_FRIEND_BLOCK;
		m_operateWidgetTag = target->getParent()->getTag();
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_FRIEND");
		return;
	}
	//拉黑陌生人
	if (isButton(button_stranger_block))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_confirmType = CONFIRM_INDEX_STRANGER_BLOCK;
		m_operateWidgetTag = target->getParent()->getTag();
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_STRANGER");
		return;
	}
	//移除好友
	if (isButton(button_stranger_remove))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_confirmType = CONFIRM_INDEX_BLOCK_REMOVE;
		m_operateWidgetTag = target->getParent()->getTag();
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_REMOVE_BLOCK_TITLE", "TIP_REMOVE_BLOCK");
		return;
	}

	if (isButton(button_continue))
	{
		//  躲过海上事件后的处理
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		shipPause(false);
		m_bIsBlocking = false;
		closeView(MAPUI_COCOS_RES[INDEX_UI_FLOAT_CSB]);
		return;
	}
	if (isButton(button_equip_items))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		int id = target->getTag();
		if (id > 10000)
		{
			openGoodInfo(nullptr, ITEM_TYPE_PORP, id - 10000);
		}
		else
		{
			openGoodInfo(nullptr, ITEM_TYPE_SHIP_EQUIP, id);
		}
	
		return;
	}

	//掉落服务
	if (isButton(image_dorp_service))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openView(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
		auto view = getViewRoot(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
		std::string st_content = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_DROPS_TIME"];
		auto layer = dynamic_cast<UISailManage*>(this->getParent());
		LeaveCityResult * result = layer->getLeaveCityResult();
		int h = result->drop_rate_increase_ramian_time / 3600;
		int m = (result->drop_rate_increase_ramian_time % 3600) / 60;
		int s = result->drop_rate_increase_ramian_time % 60;
		std::string old_value = "[time]";
		std::string new_value = StringUtils::format("%02d:%02d:%02d", h, m, s);
		repalce_all_ditinct(st_content, old_value, new_value);
		auto t_content = dynamic_cast<Text*>(view->getChildByName("label_dropitem_tiptext"));
		t_content->setString(st_content);
		return;
	}

	//个人背包超重按钮
	if (isButton(image_bag_over_weight))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
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
		return;
	}
	//个人背包扩容
	if (isButton(button_game_reconn))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto bag_info = SINGLE_SHOP->getBagExpandFeeInfo();
		size_t i = 0;
		for (; i < bag_info.size(); i++)
		{
			if (bag_info[i].capacity == m_pLeaveCityResult->max_package_size)
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
			m_nConfirmIndex = CONFIRM_INDEX_CANCEL_OVER_WEIGHT;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushSilverConfirmView("TIP_CENTER_ITEM_BAG_EXPAND_TITLE",
				"TIP_CENTER_ITEM_BAG_EXPAND_CONTENT", cost_num);
		}
		closeView();
		return;
	}
	//进入个人中心物品界面
	if (isButton(button_game_exit))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (getIsSalvaging())
		{
			openIsStopSalvage();
			return;
		}
		if (dynamic_cast<UIShip*>(m_pShip)->m_IsAutoSailing)
		{
			dynamic_cast<UIShip*>(m_pShip)->m_PauseTime = dynamic_cast<UIShip*>(m_pShip)->getCurrentTimeUsev();
			m_bAutoToPersonCenter = true;
			m_nConfirmIndex = UISailHUD::CONFIRM_INDEX_NOTAUTO;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_COMMON_CSB_WHARF_AUTOSEA_TITLE", "TIP_SAILING_NOTAUTO_GOSEA_ASK");
			Director::getInstance()->pause();
			return;
		}
		else
		{
			SINGLE_HERO->m_bSeaToCenter = true;
			CHANGETO(SCENE_TAG::CENTER_TAG);
		}
		return;
	}

	//打劫开始战斗--问号解释
	if (isButton(button_rob_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_SAILING_BATTLE_NOT_LOSE_TITLE", "TIP_SAILING_BATTLE_NOT_LOSE_CONTENT");
		return;
	}

	//正常开始战斗--问号解释
	if (isButton(button_batt_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		switch (target->getTag())
		{
		case NOT_PAY_RANSON:
			UICommon::getInstance()->flushInfoBtnView("TIP_SAILING_BATTLE_NOT_PAY_RANSON_TITLE", "TIP_SAILING_BATTLE_NOT_PAY_RANSON_CONTENT");
			break;
		case PAY_RANSON:
			UICommon::getInstance()->flushInfoBtnView("TIP_SAILING_BATTLE_PAY_RANSON_TITLE", "TIP_SAILING_BATTLE_PAY_RANSON_CONTENT");
			break;
		case NOT_LOSE:
			UICommon::getInstance()->flushInfoBtnView("TIP_SAILING_BATTLE_NOT_LOSE_TITLE", "TIP_SAILING_BATTLE_NOT_LOSE_CONTENT");
			break;
		default:
			break;
		}
		return;
	}
	if (isButton(image_durable) || isButton(image_weight) || isButton(image_sailor) || isButton(image_supply))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_atk) || isButton(image_speed) || isButton(image_def) || isButton(image_steering) || isButton(image_coordinates) || isButton(image_clock))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(label_warning_name))
	{ 
		if (SINGLE_HERO->m_bInSafeArea == true)
		{
			UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYes("TIP_SETTING_SAFE_AREA");
	}
		if (SINGLE_HERO->m_bInSafeArea == false)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SETTING_DANGEROUS_AREA");
		}
		dynamic_cast<UIShip*>(m_pShip)->stopShip(true);
		return;
	}
}

void UISailHUD::landCity(GetCityStatusResult* result)
{
    if (m_bIsBlocking){
        return;
    }

    SINGLE_HERO->m_iCityID = result->cityid;

    m_bIsBlocking = true;
    shipPause(true);
    m_nLandCityId = result->cityid;
    openView(MAPUI_COCOS_RES[INDEX_UI_LOAD_CSB]);
    auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_LOAD_CSB]);
    view->setCameraMask(4, true);
    auto panel_friend = view->getChildByName<Layout*>("panel_friend");
    auto small_image_flag = panel_friend->getChildByName<ImageView*>("image_flag");
    auto city_name = view->getChildByName<Text*>("label_goods_name");
    auto t_wanted = view->getChildByName<Text*>("label_wanted");
    auto image_flag = view->getChildByName<ImageView*>("image_flag");
    image_flag->ignoreContentAdaptWithSize(false);
	
	if (result->nation)
	{
		SINGLE_HERO->m_iCurCityNation = result->nation;
	}
	else
	{
		SINGLE_HERO->m_iCurCityNation = SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].nation;
	}
	image_flag->loadTexture(getCountryIconPath(SINGLE_HERO->m_iCurCityNation));

    auto t_license = view->getChildByName<Text*>("label_price_num");
    auto t_end = view->getChildByName<Text*>("label_wanted_0");

    auto image_friend = panel_friend->getChildByName<ImageView*>("image_friend");
    city_name->setString(SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].name);

    if (result->haslicense)
    {
        t_license->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_HAVE_CITY_LICENSE"]);
    }
    else
    {
        t_license->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_NOT_CITY_LICENSE"]);
    }
    image_friend->ignoreContentAdaptWithSize(false);

    //被通缉
    if (result->iswanted > 0)
    {
        image_friend->loadTexture(ICON_FRIEND_NPC);
        setGLProgramState(image_friend, true);
        t_wanted->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_WANTED_CHAT"]);
        small_image_flag->ignoreContentAdaptWithSize(false);
		small_image_flag->loadTexture(getCountryIconPath(SINGLE_HERO->m_iCurCityNation));
        t_end->setString(".");

        image_friend->setPositionX(t_wanted->getPositionX() - t_wanted->getContentSize().width*0.5f - image_friend->getContentSize().width);
        small_image_flag->setPositionX(t_wanted->getPositionX() + t_wanted->getContentSize().width*0.5f + small_image_flag->getContentSize().width);
        t_end->setPositionX(small_image_flag->getPositionX() + small_image_flag->getContentSize().width);
    }
    else
    {
        if (result->friendvalue >= 0)
        {
            image_friend->loadTexture(ICON_FRIEND_NPC);
        }
        else
        {
            image_friend->loadTexture(ICON_ENEMY_NPC);
        }

        image_friend->setPosition(t_wanted->getPosition());
        small_image_flag->setVisible(false);
    }
}
void UISailHUD::countryWarLandCity(GetCityStatusResult* result)
{
	if (m_bIsBlocking){
		return;
	}

	SINGLE_HERO->m_iCityID = result->cityid;
	m_bIsWar = true;
	m_bIsBlocking = true;
	shipPause(true);
	m_nLandCityId = result->cityid;
	auto layer = UINationWarLand::createCountryWarLand(result);
	this->addChild(layer);
}

void UISailHUD::showShipInfo(Ref* pSender, Widget::TouchEventType TouchType){

	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* target = dynamic_cast<Widget*>(pSender);
		if (target)
		{
			auto i_good = target->getChildByName<ImageView*>("image_goods");
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushShipDetail(m_pEndFightResult->catchshipdefine[target->getTag()], i_good->getTag(), true);
		}
	}
}

void UISailHUD::showItemInfo(Ref* pSender, Widget::TouchEventType TouchType){

	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* target = dynamic_cast<Widget*>(pSender);
		if (target)
		{
			ImageView* i_good = nullptr;
			auto fight_good =dynamic_cast<ImageView*>( target->getChildByName<ImageView*>("image_goods"));
			auto salvage_good = dynamic_cast<ImageView*>(target->getChildByName<ImageView*>("image_item"));
			if (fight_good)
			{
				i_good = fight_good;
			}
			else if (salvage_good)
			{
				i_good = salvage_good;
			}
			openGoodInfo(nullptr, target->getTag(), i_good->getTag());
		}
	}
}

void UISailHUD::autofightevent(const EndFightResult* result)
{
	m_pLeaveCityResult->current_package_size = result->current_package_size;
	m_pLeaveCityResult->max_package_size = result->max_package_size;
	showBagOverWeight();

	flushBattleResult1(result);
	if (result->newlevel)
	{
		m_bIsLevelUp = true;
		m_bIsFightCauseLvUp = true;
		SINGLE_HERO->m_iLevel = result->newlevel;
	}
	//战斗可能引起声望减少
	if (result->new_fame_level)
	{
		//声望降级时服务器返回new_fame_level为负值，代表降级
		if (result->new_fame_level<SINGLE_HERO->m_iPrestigeLv&&result->new_fame_level<0)
		{
			m_bIsPrestigeDown = true;
			SINGLE_HERO->m_iPrestigeLv = -result->new_fame_level;
		}
		else
		{
			m_bIsPrestigeUp = true;
			SINGLE_HERO->m_iPrestigeLv = result->new_fame_level;
		}	
		m_bIsFightCauseFameLvChange = true;
		
	}

	if (result->n_captains)
	{
		//有船长增加的经验大于0时才显示船长增加经验界面
		for (int i=0;i<result->n_captains;i++)
		{
			if (result->captains[i]->lootexp>0)
			{
				//战斗增加船长经验
				m_bIsCaptainUp = true;
				m_pIsFightAddCaptainExp = true;
				break;
			}
			else
			{
				m_bIsCaptainUp = false;
			}
		}
	}

	//update player UI data
	SINGLE_HERO->m_iCoin = result->coins;
	SINGLE_HERO->m_iGold = result->golds;
	setSupplies(result->totalsupply,m_nMaxSupply);
	setCrew(result->sailorcount);
	setExpAndFame(result->totalexp,result->totalfame);
	setPlayerLv(SINGLE_HERO->m_iLevel,SINGLE_HERO->m_iPrestigeLv);
}

void UISailHUD::flushBattleResult1(const EndFightResult* result)
{
	SINGLE_HERO->m_iLevel = EXP_NUM_TO_LEVEL(result->totalexp);
	SINGLE_HERO->m_iPrestigeLv = FAME_NUM_TO_LEVEL(result->totalfame);
	SINGLE_HERO->m_iCoin = result->coins;
	SINGLE_HERO->m_iGold = result->golds;

	openView(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_1_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_1_CSB]);

	auto panel_addexp = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_addexp"));

	auto image_head_player_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_addexp, "image_head_player_bg"));
	auto image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_head_player_bg, "image_head"));
	auto label_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_lv"));
	auto label_lv_r = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_addexp, "label_lv_r"));
	auto panel_coin = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted"));
	auto label_coin_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_coin, "label_hosted_num"));
	auto i_skill = panel_coin->getChildByName<ImageView*>("image_skill_bg_2_2");
	auto t_skill = i_skill->getChildByName<Text*>("text_item_num");

	auto panel_supply = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted_2"));
	auto label_supply_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_supply, "label_hosted_num"));

	auto image_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_bg"));
	auto image_result = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_bg, "image_title_victory"));
	std::string path;
	switch (result->reason)
	{
	case BATTLE_WIN:
		path = WAR_VICTORY[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_DEFEATED:
		path = WAR_DEFEATED[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_DRAW:
		path = WAR_DRAW[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_FLEE:
		path = WAR_FLEE[SINGLE_SHOP->L_TYPE];
		break;
	default:
		break;
	}
	image_result->loadTexture(path);

	image_head->ignoreContentAdaptWithSize(false);
	image_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));

	float temp_exp = 0;
	if (result->explv < LEVEL_MAX_NUM)
	{
		temp_exp = (result->totalexp - LEVEL_TO_EXP_NUM(result->explv))*1.0 / (LEVEL_TO_EXP_NUM(result->explv + 1) - LEVEL_TO_EXP_NUM(result->explv));
	}
	float temp_rep = 0;
	if (result->famelv < LEVEL_MAX_NUM)
	{
		temp_rep = (result->totalfame - LEVEL_TO_FAME_NUM(result->famelv))*1.0 / (LEVEL_TO_FAME_NUM(result->famelv + 1) - LEVEL_TO_FAME_NUM(result->famelv));
	}
	Widget* panel_exp = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_addexp, "panel_exp"));
	panel_exp->setContentSize(Size(panel_exp->getContentSize().width, 120 * temp_exp));
	Widget* panel_rep = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_addexp, "panel_rep"));
	panel_rep->setContentSize(Size(panel_rep->getContentSize().width, 120 * temp_rep));

	std::string s_supply = StringUtils::format("+%d", result->supply + result->lootenemysupply);
	std::string s_coin = StringUtils::format("+%lld", result->lootcoins + result->extralootcoins);
	label_lv->setString(String::createWithFormat("%d", SINGLE_HERO->m_iLevel)->getCString());
	label_lv_r->setString(StringUtils::format("%d", SINGLE_HERO->m_iPrestigeLv));

	//add exp and rep
	auto p_exp = panel_addexp->getChildByName<Widget*>("panel_exp");
	auto p_rep = panel_addexp->getChildByName<Widget*>("panel_r");
	auto t_exp = p_exp->getChildByName<Widget*>("label_exp");
	auto i_rep = p_rep->getChildByName<Widget*>("image_r");
	auto label_exp_num = p_exp->getChildByName<Text*>("label_exp_num");
	label_exp_num->setString(StringUtils::format("+%lld", result->lootexp));
	auto label_r_num = p_rep->getChildByName<Text*>("label_r_num");
	if (result->lootfame < 0)
	{
		label_r_num->setString(StringUtils::format("%lld", result->lootfame));
	}
	else
	{
		label_r_num->setString(StringUtils::format("+%lld", result->lootfame));
	}
	t_exp->setPositionX(p_exp->getBoundingBox().size.width / 2 - label_exp_num->getBoundingBox().size.width / 2 - 10);
	label_exp_num->setPositionX(t_exp->getPositionX() + t_exp->getBoundingBox().size.width / 2 + 20);
	i_rep->setPositionX(p_rep->getBoundingBox().size.width / 2 - label_r_num->getBoundingBox().size.width / 2 - 10);
	label_r_num->setPositionX(i_rep->getPositionX() + i_rep->getBoundingBox().size.width / 2 + 20);

	label_coin_num->setString(s_coin);
	label_supply_num->setString(s_supply);
	
	if (result->skill_plunder_master > 0 || result->skill_stealing_from_thieves > 0)
	{
		i_skill->setVisible(true);
		i_skill->setTouchEnabled(true);
		i_skill->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
		if (result->skill_plunder_master > 0)
		{
			i_skill->loadTexture(getSkillIconPath(SKILL_PREDATORY_MASTE, SKILL_TYPE_PLAYER));
			i_skill->setTag(SKILL_PREDATORY_MASTE);
			setTextSizeAndOutline(t_skill, result->skill_plunder_master);
		}
		else
		{
			i_skill->loadTexture(getSkillIconPath(SKILL_BLACK_EAT_BLACK, SKILL_TYPE_PLAYER));
			i_skill->setTag(SKILL_BLACK_EAT_BLACK);
			setTextSizeAndOutline(t_skill, result->skill_stealing_from_thieves);
		}
		label_coin_num->setPositionX(i_skill->getPositionX() - i_skill->getBoundingBox().size.width / 2);
	}
	else
	{
		i_skill->setVisible(false);
	}

	auto image_item = view->getChildByName<ImageView*>("image_items");
	image_item->setTouchEnabled(true);
	auto panel_ship = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted_3"));
	auto l_captured = panel_ship->getChildByName<ListView*>("list_captured");
	auto t_no = panel_ship->getChildByName<Text*>("text_content");
	if (result->n_catchshipdefine < 1)
	{
		t_no->setVisible(true);
	}

	for (int i = 0; i < result->n_catchshipdefine; i++)
	{
		auto item = image_item->clone();
		auto i_item = item->getChildByName<ImageView*>("image_goods");
		auto t_num = item->getChildByName<Text*>("text_item_num");
		item->addTouchEventListener(CC_CALLBACK_2(UISailHUD::showShipInfo, this));
		t_num->setVisible(false);
		item->setTag(i);
		i_item->setTag(result->catchshipdefine[i]->sid);
		i_item->loadTexture(getShipIconPath(result->catchshipdefine[i]->sid));
		setBgButtonFormIdAndType(item, result->catchshipdefine[i]->sid, ITEM_TYPE_SHIP);
		l_captured->pushBackCustomItem(item);
	}
	l_captured->setPositionX(l_captured->getPositionX() + (5 - result->n_catchshipdefine) * 63);


	auto l_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_content"));
	auto panel_items = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_hosted_4"));
	int n_gird = ceil(result->n_lootitemsarray / 9.0);
	auto t_no2 = panel_items->getChildByName<Text*>("text_content");
	if (result->n_lootitemsarray < 1)
	{
		t_no2->setVisible(true);
	}
	for (int i = 0; i < n_gird; i++)
	{
		Widget* p_item;
		if (i < 1)
		{
			p_item = panel_items;
		}
		else
		{
			p_item = panel_items->clone();
			l_content->pushBackCustomItem(p_item);
			p_item->getChildByName<ImageView*>("image_loot")->setVisible(false);
			p_item->getChildByName<Text*>("label_loots")->setVisible(false);
		}
		auto l_item = p_item->getChildByName<ListView*>("list_item");
		l_item->removeAllItems();
		size_t j = 0;
		for (; j < 9; j++)
		{
			if (i * 9 + j < result->n_lootitemsarray)
			{
				auto item = image_item->clone();
				auto i_item = item->getChildByName<ImageView*>("image_goods");
				auto t_num = item->getChildByName<Text*>("text_item_num");
				item->addTouchEventListener(CC_CALLBACK_2(UISailHUD::showItemInfo, this));
				std::string name;
				std::string path;
				getItemNameAndPath(result->lootitemsarray[i * 9 + j]->itemtype, result->lootitemsarray[i * 9 + j]->itemid, name, path);
				i_item->setVisible(true);
				t_num->setVisible(true);
				item->setTag(result->lootitemsarray[i * 9 + j]->itemtype);
				i_item->loadTexture(path);
				i_item->setTag(result->lootitemsarray[i * 9 + j]->itemid);
				setTextSizeAndOutline(t_num, result->lootitemsarray[i * 9 + j]->count);
				setBgButtonFormIdAndType(item, result->lootitemsarray[i * 9 + j]->itemid, result->lootitemsarray[i * 9 + j]->itemtype);
				l_item->pushBackCustomItem(item);
			}
			else
			{
				break;
			}
		}
		l_item->setPositionX(l_item->getPositionX() + (9 - j) * 63);
	}
	l_content->setClippingEnabled(true);
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2 + 3);
	addListViewBar(l_content, image_pulldown);
	view->setCameraMask(4);
}

void UISailHUD::flushBattleResult2(const EndFightResult* result)
{
	
	openView(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_2_CSB]);
	Widget* view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BUTTLE_RESULT_2_CSB]);
	view->setCameraMask(4,true);

	ImageView* image_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_bg"));
	auto image_result = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_bg, "image_title_victory"));
	std::string path;
	switch (result->reason)
	{
	case BATTLE_WIN:
		path = WAR_VICTORY[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_DEFEATED:
		path = WAR_DEFEATED[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_DRAW:
		path = WAR_DRAW[SINGLE_SHOP->L_TYPE];
		break;
	case BATTLE_FLEE:
		path = WAR_FLEE[SINGLE_SHOP->L_TYPE];
		break;
	default:
		break;
	}
	image_result->loadTexture(path);

	for (int i = 0; i < 5; i++)
	{
		auto ship_bg = view->getChildByName<Widget*>(StringUtils::format("image_ship_bg_%d",i+1));
		ship_bg->setVisible(true);

		auto image_ship_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(ship_bg, "image_ship_bg"));
		auto image_item_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(ship_bg, "image_item_bg_lv"));
		auto ship_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(ship_bg,"image_ship"));
		auto ship_position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(ship_bg,"image_num"));
		auto panel_durable = dynamic_cast<Widget*>(Helper::seekWidgetByName(ship_bg,"panel_durable"));
		auto progressbar_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_durable,"progressbar_durable"));
		auto image_17 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_durable,"Image_17"));
		auto progressbar_sailor = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(ship_bg,"progressbar_sailor"));
		auto image_dead = ship_bg->getChildByName<ImageView*>("image_dead");
		auto panel_ship_content = ship_bg->getChildByName<Widget*>("panel_ship_content");
	
		auto image_durable=dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship_content, "image_durable"));
		auto text_max = dynamic_cast<Text*>(Helper::seekWidgetByName(image_durable, "text_max"));
		text_max->setString("MAX");
		auto image_div_4=dynamic_cast<ImageView*>(Helper::seekWidgetByName(ship_bg, "image_div_4"));
		auto goods_desc = panel_ship_content->getChildByName<Text*>("label_sailors_num");
		goods_desc->setContentSize(goods_desc->getContentSize() + Size(0, 200));
		if (i < result->n_survivedships+result->n_lostships)
		{
			bool isLostShip = false;
			int sid = 0;
			int lost_hp = 0;
			int lost_sailor = 0;
			int lost_goods = 0;
			int lost_max_hp = 0;
			int hp = 0;
			int max_hp = 0;
			int cur_sailor = 0;
			int max_sailor = 0;
			for (int j = 0; j < result->n_lostships; j++)
			{
				if (result->lostships[j]->position == i)
				{
					isLostShip = true;
					sid = result->lostships[j]->sid; 
					lost_hp = result->lostships[j]->losthp;
					lost_sailor = result->lostships[j]->lostsailornum;
					lost_max_hp = result->lostships[j]->lostmaxhp;
					hp = 0; 
					max_hp = result->lostships[j]->maxhp;
					cur_sailor = 0;
					max_sailor = 0;
					break;
				}
			}
			for (int j = 0; j < result->n_survivedships && !isLostShip; j++)
			{
				if (result->survivedships[j]->position == i)
				{
					sid = result->survivedships[j]->sid;
					lost_hp = result->survivedships[j]->losthp;
					lost_sailor = result->survivedships[j]->lostsailornum;
					lost_max_hp = 0;
					hp = result->survivedships[j]->hp;
					max_hp = result->survivedships[j]->maxhp;
					cur_sailor = result->survivedships[j]->sailornum;
					max_sailor = result->survivedships[j]->maxsailornum;
					break;
				}
			}
			

			ship_icon->ignoreContentAdaptWithSize(false);
			ship_icon->loadTexture(getShipIconPath(sid));
			ship_position->ignoreContentAdaptWithSize(false);
			ship_position->loadTexture(getPositionIconPath(i+1));
			//稀有度
			setBgButtonFormIdAndType(image_ship_bg, sid, ITEM_TYPE_SHIP);
			setBgImageColorFormIdAndType(image_item_bg_lv, sid, ITEM_TYPE_SHIP);
			if (result->reason == BATTLE_DEFEATED)
			{
				image_17->setVisible(false);
				progressbar_durable->setPercent(0);
				progressbar_sailor->setPercent(0);
				image_dead->setVisible(true);			
				image_durable->setVisible(false);		
				image_div_4->setVisible(false);
				setGLProgramState(ship_icon,true);
				goods_desc->setString(SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_SHIP_SINK"]);
			}else
			{
				image_dead->setVisible(false);				
				image_durable->setVisible(true);		
				image_div_4->setVisible(true);
				setGLProgramState(ship_icon,false);

				auto panel_ship_durable_num_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_ship_content,"panel_ship_durable_num_1"));
				auto label_ship_durable_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship_durable_num_1,"label_ship_durable_num_2"));
		
				if(isLostShip)
				{   //not normal
					if (lost_max_hp)
					{
						image_17->setVisible(true);
					}
					progressbar_durable->setPercent(0);
					progressbar_sailor->setPercent(0);
					if(result->reason == BATTLE_WIN)
					{
						lost_goods = 5;
					}else
					{
						lost_goods = 10;
					}
				}else
				{
					image_17->setVisible(false);
					progressbar_durable->setPercent(100.0*hp/max_hp);
					progressbar_sailor->setPercent(100.0*cur_sailor/max_sailor);
				}
				std::string s_hp_max = StringUtils::format("-%d",lost_max_hp);
				if (lost_max_hp == 0)
				{
					s_hp_max = "0";
				}
				std::string s_goods = SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_NOT_LOST_GOODS"];
				if (isLostShip)
				{
					s_goods = SINGLE_SHOP->getTipsInfo()["TIP_FIGHT_LOST_GOODS"];
					std::string old_value = "[num]";
					std::string new_value = StringUtils::format("%d",lost_goods);
					repalce_all_ditinct(s_goods,old_value,new_value);
				}
			
				label_ship_durable_num_2->setString(s_hp_max);
				goods_desc->setString(s_goods);
			}
		}else
		{
			ship_bg->setVisible(false);
		}
	}
}




void UISailHUD::openFightEvent()
{
	_parent->setVisible(false);
	auto map = _parent;
	map->retain();
	ProtocolThread::GetInstance()->unregisterMessageCallback(_parent);
	EngageInFightResult *result = (EngageInFightResult*)(_userData);
	SINGLE_HERO->m_pBattelData = result;
	SINGLE_HERO->shippos = m_pShip->getPosition();
	ProtocolThread::GetInstance()->forceFightStatus();
	CHANGETO(SCENE_TAG::BATTLE_TAG);
}

void UISailHUD::setShipPositon(const Vec2 pos){

	Text *label_coordinates_num = dynamic_cast<Text *>(Helper::seekWidgetByName(m_pPanelActionbar, "label_coordinates_num"));
	std::string s_pos = StringUtils::format("%ld,%ld", (long int)pos.x, (long int)pos.y);
	label_coordinates_num->setString(s_pos);
	label_coordinates_num->setTextVerticalAlignment(TextVAlignment::TOP);
}

void UISailHUD::setShip(Node* ship)
{
	m_pShip = ship;
}

//显示技能
void UISailHUD::setSkillLv(LeaveCityResult* result)	
{
	int num = 0;
	if (result->skill_ocean_exploration > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_OCEAN_EXPLORATION;
		skillDefine.lv = result->skill_ocean_exploration;
		skillDefine.skill_type = SKILL_TYPE_PLAYER;
		skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
		m_vSkillDefine.push_back(skillDefine);
		num++;
	}

	if (result->skill_good_cook > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_GOOD_COOK;
		skillDefine.lv = result->skill_good_cook;
		skillDefine.skill_type = SKILL_TYPE_PLAYER;
		skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
		m_vSkillDefine.push_back(skillDefine);
		num++;
	}

	if (result->captain_skill_good_cook > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_CAPTAIN_COOK_SKILL;
		skillDefine.lv = result->captain_skill_good_cook;
		if (result->captain_skill_good_cook_captain_type == 1)
		{
			skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
		}
		else
		{
			skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
		}
		skillDefine.icon_id = result->captain_skill_good_cook_captain_id;
		m_vSkillDefine.push_back(skillDefine);
		num++;
	}

	if (result->captain_skill_sail_operation > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_CAPTAIN_SAIL_OPERATION;
		skillDefine.lv = result->captain_skill_sail_operation;
		if (result->captain_skill_sail_operation_captain_type == 1)
		{
			skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
		}
		else
		{
			skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
		}
		skillDefine.icon_id = result->captain_skill_sail_operation_captain_id;
		m_vSkillDefine.push_back(skillDefine);
		num++;
	}

	if (result->captain_skill_logistics > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_CAPTAIN_LOGISTICS;
		skillDefine.lv = result->captain_skill_logistics;
		if (result->captain_skill_logistics_captain_type == 1)
		{
			skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
		}
		else
		{
			skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
		}
		skillDefine.icon_id = result->captain_skill_logistics_captain_id;
		m_vSkillDefine.push_back(skillDefine);
		num++;
	}

	auto m_pSubPanel = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);

	auto panel_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pSubPanel, "panel_1"));
	auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pSubPanel, "image_skill_bg"));
	image_skill->setVisible(false);
	auto captain_skill = SINGLE_SHOP->getCaptainSkillInfo();
	for (size_t i = 0; i < m_vSkillDefine.size(); i++)
	{
		if (i == 0)
		{
			image_skill->ignoreContentAdaptWithSize(false);
			image_skill->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill->setVisible(true);
			image_skill->setTag(i);
			image_skill->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func,this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
		}
		else
		{
			auto image_skill_clone = dynamic_cast<ImageView*>(image_skill->clone());
			image_skill_clone->ignoreContentAdaptWithSize(false);
			image_skill_clone->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill_clone->setVisible(true);
			image_skill_clone->setTag(i);
			image_skill_clone->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func,this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill_clone, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			image_skill_clone->setPositionX(image_skill->getPositionX() + i*1.1*image_skill->getBoundingBox().size.width);
			image_skill->getParent()->addChild(image_skill_clone);
		}
		panel_1->setVisible(true);
	}
	auto mailResult = ProtocolThread::GetInstance()->getLastCheckMailBoxResult();
	if (mailResult)
	{
		//保险图标
		auto i_hints_insurance = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pSubPanel, "image_hints_insurance"));

		auto counts = image_skill->getParent()->getChildrenCount();
		if (m_vSkillDefine.size() > 0)
		{
			i_hints_insurance->setPositionX(1.1*image_skill->getBoundingBox().size.width*(counts - 1) + i_hints_insurance->getBoundingBox().size.width);
		}
		else
		{
			i_hints_insurance->setPositionX(image_skill->getPositionX());
		}
		if (mailResult->insurancestatus)
		{
			num++;
			i_hints_insurance->setVisible(true);
			panel_1->setVisible(true);
		}
		else
		{
			i_hints_insurance->setVisible(false);
		}
	}


	if (result)
	{
		//掉落服务
		auto i_dorp_service = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pSubPanel, "image_dorp_service"));
		if (result->drop_rate_increase_ramian_time <= 0)
		{
			i_dorp_service->setVisible(false);
		}
		else
		{
			this->schedule(schedule_selector(UISailHUD::updateBySecond), 1);
			i_dorp_service->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func,this));
			i_dorp_service->setVisible(true);
			i_dorp_service->setPositionX(1.1*image_skill->getBoundingBox().size.width* num + i_dorp_service->getBoundingBox().size.width);
			if (result->drop_rate_increase_type == 1)
			{
				i_dorp_service->loadTexture(LOW_DROPS_IOCN);
			}
			else
			{
				i_dorp_service->loadTexture(SENIOR_DROPS_IOCN);
			}
			panel_1->setVisible(true);
		}
	}

	m_pLeaveCityResult = result;
	showBagOverWeight();
}

void UISailHUD::showBagOverWeight()
{
	if (m_pLeaveCityResult)
	{
		auto m_pSubPanel = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
		auto i_over_weight = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pSubPanel, "image_bag_over_weight"));
		if (i_over_weight)
		{
			i_over_weight->removeFromParentAndCleanup(true);
		}

		//个人背包
		if (m_pLeaveCityResult->current_package_size > m_pLeaveCityResult->max_package_size)
		{
			int num = 0;
			if (m_pLeaveCityResult->skill_ocean_exploration > 0)
			{
				num++;
			}

			if (m_pLeaveCityResult->skill_good_cook > 0)
			{
				num++;
			}

			if (m_pLeaveCityResult->captain_skill_good_cook > 0)
			{
				num++;
			}

			if (m_pLeaveCityResult->captain_skill_sail_operation > 0)
			{
				num++;
			}

			if (m_pLeaveCityResult->captain_skill_logistics > 0)
			{
				num++;
			}
			auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pSubPanel, "image_skill_bg"));
			auto i_dorp_service = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pSubPanel, "image_dorp_service"));
			auto panel_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pSubPanel, "panel_1"));
			auto i_bag = dynamic_cast<ImageView*>(i_dorp_service->clone());
			i_bag->setName("image_bag_over_weight");
			i_bag->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
			i_bag->setVisible(true);
			i_bag->setTouchEnabled(true);
			panel_1->addChild(i_bag);
			panel_1->setVisible(true);

			if (m_pLeaveCityResult->drop_rate_increase_ramian_time < 0)
			{
				i_bag->setPositionX(1.1*image_skill->getBoundingBox().size.width* num + i_bag->getBoundingBox().size.width);
			}
			else
			{
				i_bag->setPositionX(1.1*image_skill->getBoundingBox().size.width* num + 2 * 1.1 * i_bag->getBoundingBox().size.width);
			}
			i_bag->loadTexture(BAG_OVER_WEIGHT);
			m_pSubPanel->setCameraMask(4);
		}
	}
}

void UISailHUD::setSailingDay(const int days)
{
	auto listview_clock_num = dynamic_cast<ListView *>(Helper::seekWidgetByName(m_pPanelActionbar, "listview_clock_num"));
	listview_clock_num->setClippingEnabled(false);
	Text *label_ship_days_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(listview_clock_num, "label_ship_days_num_1"));
	std::string sdays = StringUtils::format("%d  %s",days,SINGLE_SHOP->getTipsInfo()["TIP_DAY"].data());
	label_ship_days_num_1->setString(sdays);
}

void UISailHUD::setCrew(const int crewnum,const int maxCrewNum,const int fleetShipNum)
{
	m_nMaxCrewNum = maxCrewNum;
	m_nCurCrewNum = crewnum;
	m_nFleetShipNum = fleetShipNum;
}

void UISailHUD::setCrew(const int crewnum)
{
	ListView* listview_sailor_num = m_pImage_title_bg->getChildByName<ListView*>("listview_sailor_num");
	Text *label_ship_durable_num_1 = dynamic_cast<Text*>(listview_sailor_num->getChildByName("label_ship_durable_num_1"));
	Text *label_ship_durable_num_2 = dynamic_cast<Text*>(listview_sailor_num->getChildByName("label_ship_durable_num_2"));
	label_ship_durable_num_1->setString(StringUtils::format("%d", crewnum));
	label_ship_durable_num_2->setString(StringUtils::format("/%d", m_nMaxCrewNum));

	if (m_nMaxCrewNum == 0)
	{
		m_pProgressbar_sailor->setPercent(0);
	}else
	{
		m_pProgressbar_sailor->setPercent(crewnum * 100.0 / m_nMaxCrewNum);
	}
	
	if (m_nCurSupplies < 1)
	{
		int warning_day = 0;
		if (m_nFleetShipNum > 0)
		{
		    warning_day = crewnum/(m_nFleetShipNum*CARW_ONE_DAY_DIE);
		}
		if (warning_day <= CRITICAL_DAY)
		{
			label_ship_durable_num_1->setColor(Color3B(255,0,60));
			m_pButton_warning_2->setVisible(true);
			m_pButton_warning_2->setBright(false);
		}else if (warning_day <= WARNING_DAY)
		{
			label_ship_durable_num_1->setColor(Color3B(255,0,60));
			m_pButton_warning_2->setVisible(true);
			m_pButton_warning_2->setBright(true);
		}else
		{
			label_ship_durable_num_1->setColor(Color3B(255,255,255));
			m_pButton_warning_2->setVisible(false);
		}
	}
	listview_sailor_num->refreshView();
}

void UISailHUD::setSupplies(const int supplies,const int maxSupplies)
{
	m_nCurSupplies = supplies;
	m_nMaxSupply = maxSupplies;
	ListView* listview_supply_num = m_pImage_title_bg->getChildByName<ListView*>("listview_supply_num");
	Text *label_ship_durable_num_1 = dynamic_cast<Text*>(listview_supply_num->getChildByName("label_ship_durable_num_1"));
	Text *label_ship_durable_num_2 = dynamic_cast<Text*>(listview_supply_num->getChildByName("label_ship_durable_num_2"));
	label_ship_durable_num_1->setString(StringUtils::format("%d", supplies));
	label_ship_durable_num_2->setString(StringUtils::format("/%d", maxSupplies));
	if (maxSupplies == 0)
	{
		m_pProgressbar_supply->setPercent(0);
	}else
	{
		m_pProgressbar_supply->setPercent(supplies * 100.0 / maxSupplies);
	}
	int warning_day = 0;
	if (m_nCurCrewNum != 0)
	{
		warning_day = supplies/(m_nCurCrewNum*CARW_ONE_DAY_SUPPLY);
	}

	if (warning_day <= CRITICAL_DAY)
	{
		label_ship_durable_num_1->setColor(Color3B(255,0,60));
		m_pButton_warning_1->setVisible(true);
		m_pButton_warning_1->setBright(false);
	}else if (warning_day <= WARNING_DAY)
	{
		label_ship_durable_num_1->setColor(Color3B(255,0,60));
		m_pButton_warning_1->setVisible(true);
		m_pButton_warning_1->setBright(true);
	}else
	{
		label_ship_durable_num_1->setColor(Color3B(255,255,255));
		m_pButton_warning_1->setVisible(false);
	}
	listview_supply_num->refreshView();
}

void UISailHUD::setExpAndFame(const long int exp, const long int fame){

	int levle = EXP_NUM_TO_LEVEL(exp);
	int fameLevel = FAME_NUM_TO_LEVEL(fame);

	float temp_exp = 0;
	if (levle < LEVEL_MAX_NUM)
	{
		temp_exp = (exp - LEVEL_TO_EXP_NUM(levle))*1.0/(LEVEL_TO_EXP_NUM(levle+1)-LEVEL_TO_EXP_NUM(levle));
	}
	float temp_rep = 0;
	if (fameLevel < LEVEL_MAX_NUM)
	{
		temp_rep = (fame - LEVEL_TO_FAME_NUM(fameLevel))*1.0/(LEVEL_TO_FAME_NUM(fameLevel+1)-LEVEL_TO_FAME_NUM(fameLevel));
	}

	m_pPanelExp->setContentSize(Size(m_pPanelExp->getContentSize().width, 120 * temp_exp));
	m_pPanelRep->setContentSize(Size(m_pPanelRep->getContentSize().width, 120 * temp_rep));
}

void UISailHUD::setPlayerLv(const int lv, const int p_lv)
{
	m_playerLv->setString(StringUtils::format("%d",lv));
	m_prestigeLv->setString(StringUtils::format("%d", p_lv));
}

void UISailHUD::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	if(msgType == PROTO_TYPE_EngageInFightResult)
	{
		EngageInFightResult* result = (EngageInFightResult*)message;
		if(result && !result->failed)
		{
			//不可选择退出的战斗
			if (m_nDisableCancelFight)
			{
				_userData = result;
				m_pFightResult = result;
				openView(MAPUI_COCOS_RES[INDEX_UI_BATTLE_VS_CSB]);
				auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BATTLE_VS_CSB]);
				view->setCameraMask((unsigned short)CameraFlag::USER2, true);
				flushBattleVS(view, result);
			}
			//可选择退出的战斗
			else
			{
				_userData = result;
				m_pFightResult = result;
				//sound effect
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CRASH_18);
				openView(MAPUI_COCOS_RES[INDEX_UI_FLEETINFO_NPC_CSB]);
				auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FLEETINFO_NPC_CSB]);
				view->setCameraMask((unsigned short)CameraFlag::USER2, true);
				flushFleetinfoNpc(view, result);
			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SAILING_ENAGE_FIGHT_FAIL");
		}
	}else if (PROTO_TYPE_GetFleetAndDockShipsResult == msgType){
	
		m_pResult_fleet = (GetFleetAndDockShipsResult*)message;
		if (m_pResult_fleet->failed == 0)
		{
			if (!m_bIsWar)
			{
				openView(MAPUI_COCOS_RES[INDEX_UI_FLEETINFO_CSB]);
				m_pPanel_fleet_info = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FLEETINFO_CSB]);
				m_pPanel_fleet_info->setCameraMask(4, true);
				m_pButtonShipinfo = m_pPanel_fleet_info->getChildByName<Button*>("button_shipinfo");
				flushPanelFleetInfo(m_pResult_fleet);
			}
		}
		else
		{
			openSuccessOrFailDialog("TIP_DOCK_GET_FLEET_FAIL");
		}

	}else if(msgType == PROTO_TYPE_TriggerSeaEventResult){
		m_bIsBlocking = false;
	}else if(PROTO_TYPE_EndFightResult == msgType)
	{
		EndFightResult* result = (EndFightResult*)message;
		if (result->failed == 0)
		{
			m_pEndFightResult = result;
			SINGLE_HERO->m_iExp = result->totalexp;
			SINGLE_HERO->m_iRexp = result->totalfame;
			//最近离开的有许可证的城市
			SINGLE_HERO->m_iHave_licence_cityId = result->last_safe_city;
			setShipEquipBrokenLocal(result->n_brokenequips,result->brokenequips);
			if(result->reason == 2){
				m_bIsFailed = true;
			}

			if(result->isautofight){
				closeView();
				autofightevent(result);
				if (CompanionTaskManager::GetInstance()->checkTaskAction())
				{
					addNeedControlWidgetForHideOrShow(nullptr, true);
					allTaskInfo(false, COMPANION_STORY);
					CompanionTaskManager::GetInstance()->notifyUIPartDelayTime(0.5);
				}
			}else{
			
			}

			if (result->brokenequips && result->n_brokenequips > 0)
			{
				UICommon::getInstance()->openCommonView(this);
				UICommon::getInstance()->flushEquipBrokenView(result->brokenequips, result->n_brokenequips);
			}
		}else
		{

		}
	}else if (PROTO_TYPE_GetCityStatusResult == msgType)
	{
		GetCityStatusResult *result = (GetCityStatusResult*)message;
		if (result->failed == 0)
		{
			if (result->in_war)
			{
				countryWarLandCity(result);
			}
			else
			{
				if (m_bIsSalvage)
				{
					m_bIsCity = true;
					m_pCityResult = result;
					openIsStopSalvage();
				}
				else
				{
					landCity(result);
				}
			}
		}
	}
		else if(PROTO_TYPE_AddDiscoveredCityResult==msgType)
	{
		AddDiscoveredCityResult*result=(AddDiscoveredCityResult*)message;
		if (result->failed==0)
		{
			m_pDiscoverCityResult = result;
			SINGLE_HERO->m_iExp = result->totalexp;
			SINGLE_HERO->m_iRexp = result->totalfame;
			findCityResult(result->addedexp,result->addedfame);	
			if (result->newlevel)
			{
				m_bIsNewCityCauseLvUp = true;
				m_bIsLevelUp=true;
				//船长也会增加经验
				m_pIsNewCityAddCaptainExp = true;
				SINGLE_HERO->m_iLevel=result->newlevel;
			}
			if (result->new_fame_level)
			{
				m_bIsNewCityCauseFameUp = true;
				m_bIsPrestigeUp=true;
				SINGLE_HERO->m_iPrestigeLv=result->new_fame_level;
			}

			if (result->n_captains)
			{
				//有船长增加的经验大于0时才显示船长增加经验界面
				for (int i = 0; i<result->n_captains; i++)
				{
					if (result->captains[i]->lootexp>0)
					{
						//战斗增加船长经验
						m_bIsCaptainUp = true;
						m_pIsNewCityAddCaptainExp = true;
						break;
					}
					else
					{
						m_bIsCaptainUp = false;
					}
				}
			}
			//reset head
			setPlayerLv(SINGLE_HERO->m_iLevel, SINGLE_HERO->m_iPrestigeLv);
			setExpAndFame(result->totalexp,result->totalfame);

		}
		else
		{

		}
	}
	else if (PROTO_TYPE_StartSalvageResult == msgType)
	{
		StartSalvageResult*result = (StartSalvageResult*)message;
		if (result->failed == 0)
		{
			dynamic_cast<UIShip*>(m_pShip)->stopPauseTime();
			auto pSprite = Sprite::create();
			pSprite->setTexture("salvage_dh.png");
			pSprite->setName("salvage_dh");
			m_pShip->addChild(pSprite, -1);
			pSprite->setScale(0);
			pSprite->runAction(Sequence::createWithTwoActions(Repeat::create(Sequence::createWithTwoActions(Spawn::createWithTwoActions(FadeIn::create(1.0f), ScaleTo::create(1.0f, 1.0f)), Spawn::createWithTwoActions(FadeOut::create(0.4f), ScaleTo::create(0.4f, 0.0f))), 4), RemoveSelf::create()));
			dynamic_cast<UIShip*>(m_pShip)->setFalgVisible(false);

			auto t_salvaging = LabelTTF::create(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_SALVAGING"], CUSTOM_FONT_NAME_1, 16);
			t_salvaging->setPositionY(120);
			auto sp_bg = ImageView::create("ship/salvage_progress_bg.png");
			auto sp = Sprite::create();
			sp->setTexture("ship/salvage_progress.png");
			auto salvage_progress = ProgressTimer::create(sp);
			m_pShip->addChild(sp_bg, 1, 111);
			m_pShip->addChild(salvage_progress, 2, 222);
			m_pShip->addChild(t_salvaging, 3, 333);
			sp_bg->setPositionY(90);
			salvage_progress->setPositionY(90);
			salvage_progress->setMidpoint(Vec2(0, 0));
			salvage_progress->setPercentage(0);
			salvage_progress->setType(ProgressTimer::Type::BAR);
			salvage_progress->setBarChangeRate(Vec2(1, 0));
			salvage_progress->runAction(ProgressTo::create(5.0f, 100.0f));
			this->scheduleOnce(schedule_selector(UISailHUD::openSalvage), 5);

			auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
			auto b_salvage = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_salvage"));
			b_salvage->setBright(false);
		}
	}
	else if (PROTO_TYPE_SalvageResult == msgType)
	{
		SalvageResult*result = (SalvageResult*)message;
		if (result->failed == 0)
		{
			openSalvage(result);
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SAILING_SALVAG_NOT_ITEM");
		}
	}
	else if (PROTO_TYPE_GetSalvageConditionResult == msgType)
	{
		GetSalvageConditionResult*result = (GetSalvageConditionResult*)message;
		auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
		if (result->failed == 0)
		{
			m_nSalvageCost = result->needcoins;
			m_nConfirmIndex = CONFIRM_INDEX_SALVAGING;
			openCheifChat();
		}
		/*银币不足*/
		else if (result->failed == 2)
		{
			m_bIsSalvage = false;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			m_bSalvageClick = true;
		}
		/*背包不够用*/
		else if (result->failed == 3)
		{
			m_bIsSalvage = false;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_WHARF_BAG_OVERWEIGHT");
			m_bSalvageClick = true;
		}
		else
		{
			m_bSalvageClick = true;
		}
	}
	else if (PROTO_TYPE_CheckMailBoxResult == msgType)
	{
		CheckMailBoxResult *result = (CheckMailBoxResult*)message;
		if (result->failed == 0)
		{
			if (result->authenticated)
			{
				SINGLE_HERO->m_iMyEmailAccountVerified = true;
			}
			else
			{
				SINGLE_HERO->m_iMyEmailAccountVerified = false;
			}
			//保险图标
			auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
			auto image_skill_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_skill_bg"));
			auto i_hints_insurance = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_hints_insurance"));
			auto leavecicyResult = dynamic_cast<UISailManage*>(_parent)->getLeaveCityResult();
			if (leavecicyResult)
			{
				auto counts = image_skill_bg->getParent()->getChildrenCount();
				if (leavecicyResult->skill_ocean_exploration || leavecicyResult->skill_good_cook || leavecicyResult->captain_skill_good_cook)
				{
					i_hints_insurance->setPositionX(1.1*image_skill_bg->getBoundingBox().size.width*(counts - 1) + i_hints_insurance->getBoundingBox().size.width);
				}
				else
				{
					i_hints_insurance->setPositionX(image_skill_bg->getPositionX());
				}
				if (result->insurancestatus)
				{
					i_hints_insurance->setVisible(true);
				}
				else
				{
					i_hints_insurance->setVisible(false);
				}
			}
			
			setAllUnreadImage(result);
		}
	}
	else if (PROTO_TYPE_GetAttackPirateInfoResult == msgType)
	{
		GetAttackPirateInfoResult *result = (GetAttackPirateInfoResult *)message;
		if (result->failed == 0)
		{
			UIPriate*pirateLayer = new UIPriate();
			Scene* scene = Scene::createWithPhysics();
			scene->addChild(pirateLayer);
			pirateLayer->init(result, nullptr);
			Director::getInstance()->replaceScene(scene);
		}
		else
		{
			shipPause(true);
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_SERVER_DATA_FIAL");
		}
	}
	else if (PROTO_TYPE_FindLootPlayerResult == msgType)
	{
		FindLootPlayerResult * result = (FindLootPlayerResult *)message;
		if (result->failed == 0)
		{
			m_lootPlayerInfo = result;
			openLootView(result->battledata);
		}
		else if (result->failed == 2)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_LOOT_PLAYER_INFO");
		}
	}
	else if (PROTO_TYPE_LootPlayerResult == msgType)
	{
		LootPlayerResult * result = (LootPlayerResult*)message;
		if (result->failed == 0)
		{
		   openFightEvent();
		}else if (result->failed == 2)
		{		
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_LOOT_PLAYER_INFO");
		}else if (result->failed == 20)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_LOOT_PLAYER_FORBID");
			auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_ROB_UI_CSB]);
			auto btn = Helper::seekWidgetByName(view, "button_start_rob");
			m_robforbid = true; 
			btn->setTouchEnabled(true);
			btn->setBright(false);
		}
	}
	else if (PROTO_TYPE_InviteUserToGuildResult == msgType) 
	{
		InviteUserToGuildResult *result = (InviteUserToGuildResult *)message;
		if (result->failed == 0)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_GULID_INVINT_SUCCESS");
		}
		else if (result->failed == 11)
		{
			//邀请的对象不是同一势力
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_GUILD_INVITED_FAILED_NOT_SAME_COUNTRY_CONTENT");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_GULID_INVINT_FAIL");
		}
	}
	else if (PROTO_TYPE_GetUserInfoByIdResult == msgType)
	{
		GetUserInfoByIdResult *result = (GetUserInfoByIdResult *)message;
		if (result->failed == 0)
		{
			SINGLE_HERO->m_InvitedToGuildNation = result->nation;
			UICommon::getInstance()->openCommonView(this);
			if (result->usercid == SINGLE_HERO->m_iID)
			{
				UICommon::getInstance()->flushFriendDetail(result, true);
			}
			else
			{
				UICommon::getInstance()->flushFriendDetail(result, false);
			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_PLAYER_INFO_FAIL");
		}
	}
	else if (PROTO_TYPE_FriendsOperationResult == msgType)
	{
		FriendsOperationResult *pFriendsResult = (FriendsOperationResult*)message;
		if (pFriendsResult->failed == 0)
		{
			m_confirmType = CONFIRM_INDEX_ACCEPT_ADDFRIEND;
			//添加好友
			if (pFriendsResult->actioncode == 3)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADDED_FRIEND_SUCCESS");
			}
			//添加黑名单里的玩家为好友
			else if (pFriendsResult->actioncode == 2)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_BLACK_LIST_SUCCESS");
			}
			//删除好友
			else if (pFriendsResult->actioncode == 1)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_DETELE_FRIEND_SUCCESS");
			}
			//加为好友
			else if (pFriendsResult->actioncode == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_FRIEND_SUCCESS");
			}
			//移除拉黑玩家
			else if (pFriendsResult->actioncode == 5)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_REMOVE_BLOCK_SUCCESS");
			}
			UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_FRIEND_DETAIL_CSB]);
		}
		else if (pFriendsResult->failed == 105)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_REQUEST_ADDFRIENDED");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_ADD_FRIENDED_FAIL");
		}
	}
	else if (PROTO_TYPE_CancelFightResult == msgType)
	{
		CancelFightResult *cfResult = (CancelFightResult*)message;
		if (cfResult->failed == 0)
		{
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(NPC_ID).c_str(), -1);
			for (size_t i = 1; i <= 10; i++)
			{
				auto capturing_ship = StringUtils::format(CAPTURINGSHIPID, i);
				auto st_ship = StringUtils::format("%d", 0);
				UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(capturing_ship.c_str()).c_str(), getEncryptedPasswordHex((char*)st_ship.c_str()));
				auto food_plunder = StringUtils::format(FOODPLUNDERSHIPID, i);
				auto st_ship2 = StringUtils::format("%d", 0);
				UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(food_plunder.c_str()).c_str(), getEncryptedPasswordHex((char*)st_ship2.c_str()));
			}
			for (size_t i = 0; i < cfResult->n_user_positive_skills; i++)
			{
				auto st_skill = StringUtils::format(SKILL_NUM, cfResult->user_positive_skills[i]->id);
				UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_skill.c_str()).c_str(), 0);
			}
			UserDefault::getInstance()->flush();
			closeView();
			m_bIsBlocking = false;
			shipPause(false);
			if (m_nDisableCancelFight != 0)
			{
				closeView();
				dynamic_cast<UISailManage*>(_parent)->setLastNPCEventId(m_pFightResult->eventid);
			}
			m_nForceFightNpcId = 0;
			m_nDisableCancelFight = 0;
			removeLayerForSeaEvent();
			if (cfResult->usegolds > 0)
			{
				Utils::consumeVTicket("17", 1, cfResult->usegolds);
			}
		}
		else if (cfResult->failed == 2)
		{
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
		}
	}
	else if (PROTO_TYPE_GetSailInfoResult == msgType)
	{
		GetSailInfoResult*sailInfo = (GetSailInfoResult *)message;
		if (sailInfo ->failed == 0)
		{
			if (sailInfo->failedlastfightwithoutcheck)
			{
				openView(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
				auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
				auto t_title = view->getChildByName<Text*>("label_advanced_repair");
				auto t_content = view->getChildByName<Text*>("label_repair_content");
				auto lv_costNum = view->getChildByName<ListView*>("listview_cost");
				auto w_costNum = lv_costNum->getItem(1);
				auto t_costNum = w_costNum->getChildByName<Text*>("label_cost_num");
				std::string s_cost = numSegment(StringUtils::format("%d", sailInfo->saveshipcost));
				t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_BATTLE_SAVE_SHIP_TITLE"]);
				t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_BATTLE_SAVE_SHIP_CONTENT"]);
				t_costNum->setString(s_cost);
				auto image_silver_1 = w_costNum->getChildByName<ImageView*>("image_silver_1");
				image_silver_1->setPositionX(t_costNum->getPositionX() -
					t_costNum->getBoundingBox().size.width - image_silver_1->getBoundingBox().size.width * 0.7);
				if (sailInfo->gold >= sailInfo->saveshipcost)
				{
					m_nDisableCancelFight = 4;
				}
				else
				{
					m_nDisableCancelFight = 5;
				}
				m_nSaveshipcost = sailInfo->saveshipcost;
				m_bIsBlocking = true;
				shipPause(true);
				m_nConfirmIndex = CONFIRM_INDEX_CANCEL_V;
			}
		}
	}
	else if (msgType == PROTO_TYPE_EndFailedFightByVTicketResult)
	{
		auto result = (EndFailedFightByVTicketResult *)message;
		if (result->failed == 0)
		{
			SINGLE_HERO->m_iHave_licence_cityId = result->last_safe_city;
			if (result->usevticket > 0)
			{
				Utils::consumeVTicket("16", 1, result->usevticket);
				openSaillingDialog(3, result->lostcoin);
			}
			else
			{
				openSaillingDialog(4, result->lostcoin);
			}
			removeLayerForSeaEvent();
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(NPC_ID).c_str(), -1);
			for (size_t i = 1; i <= 10; i++)
			{
				auto capturing_ship = StringUtils::format(CAPTURINGSHIPID, i);
				auto st_ship = StringUtils::format("%d", 0);
				UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(capturing_ship.c_str()).c_str(), getEncryptedPasswordHex((char*)st_ship.c_str()));
				auto food_plunder = StringUtils::format(FOODPLUNDERSHIPID, i);
				auto st_ship2 = StringUtils::format("%d", 0);
				UserDefault::getInstance()->setStringForKey(ProtocolThread::GetInstance()->getFullKeyName(food_plunder.c_str()).c_str(), getEncryptedPasswordHex((char*)st_ship2.c_str()));
			}
			for (size_t i = 0; i < result->n_user_positive_skills; i++)
			{
				auto st_skill = StringUtils::format(SKILL_NUM, result->user_positive_skills[i]->id);
				UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_skill.c_str()).c_str(), 0);
			}
			UserDefault::getInstance()->flush();
		}
		else
		{
			if (m_pFightResult)
			{
				ProtocolThread::GetInstance()->endFailedFightByVTicket(1, m_pFightResult->fighttype, UILoadingIndicator::createWithMask(this, 4));
			}
			else
			{
				ProtocolThread::GetInstance()->endFailedFightByVTicket(1, FIGHT_TYPE_NORMAL, UILoadingIndicator::createWithMask(this, 4));
			}
		}
	}
	else if (msgType == PROTO_TYPE_ExpandPackageSizeResult)
	{
		ExpandPackageSizeResult *result = (ExpandPackageSizeResult*)message;
		if (result->failed == 0)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_CENTER_ITEM_BAG_EXPAND_RESULT");
			m_pLeaveCityResult->max_package_size = result->newpackagesize;
			showBagOverWeight();
			SINGLE_HERO->m_iCoin = result->coin;
			SINGLE_HERO->m_iGold = result->gold;
		}
		else if (result->failed == COIN_NOT_FAIL)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
		}
	}
	else if (msgType == PROTO_TYPE_GetMyNationWarResult)
	{
		GetMyNationWarResult *result = (GetMyNationWarResult*)message;
		if (result->failed == 0)
		{
			m_pMyNationWarResult = result;
			if (result->war_status != 0)
			{
				auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
				auto p_war = view->getChildByName<ImageView*>("panel_war_info");
				p_war->setVisible(true);
				auto i_nation1 = p_war->getChildByName<ImageView*>("image_country");
				auto i_nation2 = p_war->getChildByName<ImageView*>("image_country_1");
				i_nation1->loadTexture(getCountryIconPath(result->nation1));
				i_nation2->loadTexture(getCountryIconPath(result->nation2));
				auto t_content = p_war->getChildByName<Text*>("label_derable");
				auto s = result->time_left % 60;
				auto m = result->time_left % 3600 / 60;
				auto h = result->time_left / 3600;
				std::string content;
				if (result->war_status == 1)
				{
					content = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_START_TIME"];
				}
				else
				{
					content = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_END_TIME"];
				}
				std::string new_value = StringUtils::format("%02d:%02d:%02d", h, m, s);
				std::string old_value = "[time]";
				repalce_all_ditinct(content, old_value, new_value);
				t_content->setString(content);
				this->schedule(schedule_selector(UISailHUD::updateNationWarBySecond), 1);

				auto panel_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_1"));
				if (!panel_1->isVisible())
				{
					p_war->setPositionY(panel_1->getPositionY() + panel_1->getContentSize().height - p_war->getContentSize().height);
				}

				p_war->stopAllActions();
				p_war->setPositionX(0 - p_war->getContentSize().width / 2);
				p_war->setOpacity(0);

				p_war->setPositionX(p_war->getPositionX() - p_war->getContentSize().width / 2);
				auto action_1 = MoveBy::create(1, Vec2(p_war->getContentSize().width, 0));
				auto action_2 = Sequence::create(DelayTime::create(0.5), FadeIn::create(1),nullptr);
				auto spw_action = Spawn::createWithTwoActions(action_1, action_2);
				p_war->runAction(spw_action);
			}
		}
	}
}

void UISailHUD::openSalvage(const float fTime)
{
	if (m_bIsSalvage)
	{
		bool seaEventFloat = dynamic_cast<UISailManage*>(_parent)->getSeaEventLayerFloat();		
		if (seaEventFloat)
		{
			auto floart = dynamic_cast<UISailManage*>(_parent)->getSeaFloat();
			showSeaEvent(floart);
		}
		else
		{
			ProtocolThread::GetInstance()->salvage(m_pShip->getPositionX(), m_pShip->getPositionY(), UILoadingIndicator::createWithMask(this, 4));
		}
		
		dynamic_cast<UIShip*>(m_pShip)->setFalgVisible(true);
		dynamic_cast<UIShip*>(m_pShip)->removeChildByTag(111);
		dynamic_cast<UIShip*>(m_pShip)->removeChildByTag(222);
		dynamic_cast<UIShip*>(m_pShip)->removeChildByTag(333);
		openView(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
		auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
		auto b_salvage = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_salvage"));
		b_salvage->setBright(true);

		m_bIsSalvage = false;
		shipPause(true);
	}
}

void UISailHUD::openSalvage(SalvageResult *result)
{
	m_pLeaveCityResult->current_package_size = result->currentpackagesize;
	m_pLeaveCityResult->max_package_size = result->packagesize;
	showBagOverWeight();

	openView(COMMOM_COCOS_RES[C_VIEW_SALVAGE_RESULT]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SALVAGE_RESULT]);
	view->setCameraMask(4, true);
	auto shopData = SINGLE_SHOP->getItemData();
	//背景显示,当有item是，若rarity>=2,显示珍贵物品的背景图
	auto panel_item = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_item"));
	auto image_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_item, "image_bg"));
	auto label_content_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_item, "label_content_1"));
	auto label_content_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_content_2"));
	label_content_2->setVisible(false);
	int n = result->n_item;
	//物品的稀有度
	int item_rarity = 0;
	//银币和补给只可能有一个
	if (result->addcoins > 0 || result->addsupply > 0)
	{
		n++;
	}
	log("SINGLE_AUDIO::%d", SINGLE_AUDIO->getBackgroundMusicON());
	for (size_t i = 0; i < 3; i++)
	{
		auto item = view->getChildByName<ImageView*>(StringUtils::format("image_items_bg_%d", i + 1));
		auto i_icon = item->getChildByName<ImageView*>("image_item");
		auto t_num = item->getChildByName<Text*>("label_item_num");
		i_icon->ignoreContentAdaptWithSize(false);
		
		if (i < n)
		{
			if (result->n_item < n)
			{
				if (i == 0)
				{
					if (result->addcoins > 0)
					{
						item->setTouchEnabled(true);
						item->addTouchEventListener(CC_CALLBACK_2(UISailHUD::showItemInfo, this));
						item->setTag(ITEM_TYPE_SPECIAL);
						i_icon->setTag(10002);
						i_icon->loadTexture(getItemIconPath(10002));
						setTextSizeAndOutline(t_num, result->addcoins);
						image_bg->loadTexture("res/salvage_bg/coins.jpg");
					}
					else
					{
						i_icon->loadTexture("res/shipAttIcon/supply.png");
						setTextSizeAndOutline(t_num, result->addsupply);
						image_bg->loadTexture("res/salvage_bg/supply.jpg");
					}
				}
				else
				{
					item->setTouchEnabled(true);
					item->addTouchEventListener(CC_CALLBACK_2(UISailHUD::showItemInfo, this));
					int rarity = 0;
					std::map<int, ITEM_RES>::iterator m1_Iter;
					for (m1_Iter = shopData.begin(); m1_Iter != shopData.end(); m1_Iter++)
					{
						if (m1_Iter->second.id == result->item[i-1]->itemid)
						{
							item->setTag(m1_Iter->second.type);
							rarity = m1_Iter->second.rarity;
							break;
						}
					}
					
					i_icon->setTag(result->item[i - 1]->itemid);
					i_icon->loadTexture(getItemIconPath(result->item[i - 1]->itemid));
					
					if (rarity >= item_rarity)
					{
						item_rarity = rarity;
					}
					setTextSizeAndOutline(t_num, result->item[i - 1]->itemcount);
				}
			}
			else
			{
				item->setTouchEnabled(true);
				item->addTouchEventListener(CC_CALLBACK_2(UISailHUD::showItemInfo, this));
				int  rarity = 0;
				std::map<int, ITEM_RES>::iterator m1_Iter;
				for (m1_Iter = shopData.begin(); m1_Iter != shopData.end(); m1_Iter++)
				{
					if (m1_Iter->second.id == result->item[i]->itemid)
					{
						item->setTag(m1_Iter->second.type);
						rarity = m1_Iter->second.rarity;
						break;
					}
				}			
				i_icon->setTag(result->item[i]->itemid);
				i_icon->loadTexture(getItemIconPath(result->item[i]->itemid));
				if (rarity >= item_rarity)
				{
					item_rarity = rarity;
				}
				setTextSizeAndOutline(t_num, result->item[i]->itemcount);
			}
			item->setVisible(true);
		}
		else
		{
			item->setVisible(false);
		}
		//当打捞到物品时
		if (item_rarity == 1)
		{
			image_bg->loadTexture("res/salvage_bg/prop.jpg");//普通物品
		}
		else if (item_rarity >= 2)
		{
			image_bg->loadTexture("res/salvage_bg/precious.jpg");//珍贵物品
		}
		//位置微调
		if (n == 1)
		{
			if (i == 0)
			{
				item->setPositionX(item->getPositionX() + item->getBoundingBox().size.width*1.3);
			}
		}
		else if (n == 2)
		{
			if (i == 0 || i == 1)
			{
				item->setPositionX(item->getPositionX() + item->getBoundingBox().size.width*0.8);
			}
		}
	}

	log("SINGLE_AUDIO::%d", SINGLE_AUDIO->getBackgroundMusicON());
	//什么也没打捞到
	if (result->n_item < 1 && result->addcoins < 1 && result->addsupply < 1)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_SALVAGE_FAIL_40);
		auto image_title = view->getChildByName<ImageView*>("image_title");
		image_title->loadTexture("res/sailing/salvage_title_bg_2.png");
		image_bg->loadTexture("res/salvage_bg/nothing.jpg");
		
		auto panel_item_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_item_2"));
		for (size_t i = 0; i < 3; i++)
		{
			auto item = view->getChildByName<ImageView*>(StringUtils::format("image_items_bg_%d", i + 1));
			item->setVisible(false);
		}
		//内容
		label_content_2->setVisible(true);
		label_content_2->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAPUI_SALAVE_NOTHING_TIP"]);
		label_content_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAPUI_SALAVE_WASTES_TIP"]);
	}
	else
	{
		if (result->n_item>=1)
		{
			if (item_rarity>=2)
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_SALVAGE_SPECIAL_41);
				label_content_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAPUI_SALAVE_TREASURE_TIP"]);
			}
			else
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_SALVAGE_SUCCEED_42);
				label_content_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAPUI_SALAVE_ITEM_TIP"]);
			}
		}
		else if (result->addcoins>1)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_SALVAGE_SUCCEED_42);
			label_content_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAPUI_SALAVE_COIN_TIP"]);
		}
		else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_SALVAGE_SUCCEED_42);
			label_content_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAPUI_SALAVE_SUPPLIES_TIP"]);
		}
		Sprite*spriteAnimate = Sprite::create();
		spriteAnimate->setAnchorPoint(Vec2(.5, 0.5));
		spriteAnimate->setScale(7);
		spriteAnimate->setCameraMask(4, true);
		view->addChild(spriteAnimate, -1);
		spriteAnimate->setPosition(Vec2(view->getContentSize().width / 2 + 80, view->getContentSize().height / 2 - 80));
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
		Animation* animation_shiplight = Animation::createWithSpriteFrames(frames, 2.0 / 28, 1);
		auto animateEff = Animate::create(animation_shiplight);
		spriteAnimate->runAction(RepeatForever::create(animateEff));
	}
	setSupplies(result->supply, m_nMaxSupply);
}

void UISailHUD::openIsStopSalvage()
{
	if (m_bSalvageClick)
	{
		Director::getInstance()->pause();
		m_nConfirmIndex = CONFIRM_INDEX_INTERRUPT_SALVAGE;
		shipPause(false);
		openCheifChat();
	}
}

void UISailHUD::flushSeaEventView(const TriggerSeaEventResult* result, std::string title){

	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	m_pFloatResultview = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);

	m_pFloatResultview->setCameraMask(4,true);
	m_pListview_result = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_pFloatResultview, "listview_result"));
	m_pListview_result->setCameraMask(4,true);
	m_pListview_result->setSubNodeMaxCameraMask(4);
	m_pListview_result->removeAllItems();

	Text* label_title = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pFloatResultview, "label_title"));
	label_title->setString(title);
	if (result->failed){

		return;
	}

	if (result->usedpropid)
	{
		lossProps(result->usedpropid);
	}
	else
	{
		switch (result->eventid)
		{
		case 0:{
				   if (result->lootcoins){

					   int n_coin = result->lootcoins;
					   addOrLossCoin(n_coin);

				   }

				   if (result->loot){

					   int n_id = result->loot->itemid;
					   int n_count = result->loot->count;
					   addOrLossItem(n_id, n_count);

				   }

				   if (result->loss){
					   int32_t n_lossCrewNum = result->loss->crewloss;
					   if (n_lossCrewNum){
						   lossCrewNum(n_lossCrewNum);
					   }

					   int32_t n_lossSupply = result->loss->supplyloss;
					   if (n_lossSupply){
						   lossSupply(n_lossSupply);
					   }


					   std::vector<SeaEventShipStatus*>m_lossShip;
					   bool isCatch = false;
					   for (int i = 0; i < result->loss->n_shipstatus; i++)
					   {
						   auto loosShip = result->loss->shipstatus[i];
						   if (loosShip->position == -10)
						   {
							   isCatch = true;
						   }
						   else
						   {
							   m_lossShip.push_back(loosShip);
						   }

					   }
					   if (isCatch)
					   {
						   m_lossShip.push_back(result->loss->shipstatus[0]);
					   }
					   

					   for (int i = 0; i < m_lossShip.size(); i++)
					   {
						   int32_t m_lossDurable = m_lossShip.at(i)->losthp;
						   int32_t m_lossMaxDurable = m_lossShip.at(i)->lostmaxhp;
						   int32_t hp = m_lossShip.at(i)->hp;
						   int32_t maxHp = m_lossShip.at(i)->current_max_hp;
						   std::string myShipName="";
						   if (m_lossShip.at(i)->shipname)
						   {
							   myShipName = m_lossShip.at(i)->shipname;
						   }
						   else
						   {
							   myShipName = getShipName(m_lossShip.at(i)->sid);
						   }
						   myShipName = changeShipName(myShipName);
						   if (m_lossDurable || m_lossMaxDurable){
							   //装备报废提示
							   if (result->brokenequips && result->n_brokenequips > 0)
							   {
								   UICommon::getInstance()->openCommonView(this);
								   UICommon::getInstance()->flushEquipBrokenView(result->brokenequips, result->n_brokenequips);
							   }
							   //装备是否有损坏
							   setShipEquipBrokenLocal(result->n_brokenequips, result->brokenequips);
							   lossDurable(m_lossShip.at(i)->sid, m_lossDurable, m_lossMaxDurable, hp, maxHp, myShipName, m_lossShip.at(i)->position, result->loss->n_shipstatus);
						  }				   
					   } 
				   }
		}
			break;

		default:
			break;
		}

	}
	
}


void UISailHUD::lossCrewNum(const int num){

	Widget* panel_sailor = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFloatResultview, "panel_sailor"));
	Widget* sailor_clone = panel_sailor->clone();
	Widget* image_div_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFloatResultview, "image_div_1"));
	auto image_div_line = (Widget*)image_div_1->clone();
	sailor_clone->setCameraMask(4,true);
	image_div_line->setCameraMask(4, true);
	auto  label_buy_num = dynamic_cast<Text*>(Helper::seekWidgetByName(sailor_clone, "label_buy_num"));
	auto image_sailor = dynamic_cast<ImageView*>(Helper::seekWidgetByName(sailor_clone, "image_silver_2"));
	CC_SAFE_RETAIN(sailor_clone);
	if (sailor_clone->getParent()){
		sailor_clone->removeFromParent();
	}
	std::string str_crewNum = StringUtils::format("-%d", num);
	label_buy_num->setTextColor(Color4B::RED);
	label_buy_num->setString(str_crewNum);
	image_sailor->setPositionX(label_buy_num->getPositionX() - label_buy_num->getContentSize().width - image_sailor->getContentSize().width/2-12);
	m_pListview_result->pushBackCustomItem(sailor_clone);
	m_pListview_result->pushBackCustomItem(image_div_line);
	CC_SAFE_RELEASE(sailor_clone);
	m_nCurCrewNum = m_nCurCrewNum - num;
	setCrew(m_nCurCrewNum);
}

void UISailHUD::lossSupply(const int num)
{
	Widget* panel_sailor = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFloatResultview, "panel_sailor"));
	Widget* sailor_clone = panel_sailor->clone();
	Widget* image_div_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFloatResultview, "image_div_1"));
	auto image_div_line = (Widget*)image_div_1->clone();
	image_div_line->setCameraMask(4, true);
	sailor_clone->setCameraMask(4, true);
	Text* label_buy_num = dynamic_cast<Text*>(Helper::seekWidgetByName(sailor_clone, "label_buy_num"));
	Text* label_force_relation = dynamic_cast<Text*>(Helper::seekWidgetByName(sailor_clone, "label_force_relation"));
	ImageView* image_silver_2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(sailor_clone, "image_silver_2"));
	label_force_relation->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_LOSS_SUPPLY"]);
	image_silver_2->loadTexture("res/shipAttIcon/att_10.png");
	std::string str_crewNum = StringUtils::format("-%d", num);
	label_buy_num->setTextColor(Color4B::RED);
	label_buy_num->setString(str_crewNum);
	image_silver_2->setPositionX(label_buy_num->getPositionX() - label_buy_num->getContentSize().width - image_silver_2->getContentSize().width / 2 - 12);
	m_pListview_result->pushBackCustomItem(sailor_clone);
	m_pListview_result->pushBackCustomItem(image_div_line);
	setSupplies(m_nCurSupplies - num, m_nMaxSupply);
}

void UISailHUD::lossDurable(const int shipid, int lossHp, int lossMaxhp, int hp, int maxHp, std::string shipname, int order, int shipCounts){
	//分隔线
	Widget * image_div_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFloatResultview, "image_div_1"));
	auto image_div_1_clone = image_div_1->clone();
	image_div_1_clone->setCameraMask(4, true);
	Widget* panel_ship_durable_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFloatResultview, "panel_ship_durable_1"));
	Widget* durable_1_clone = panel_ship_durable_1->clone();
	durable_1_clone->setCameraMask(4,true);
	auto image_material_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(durable_1_clone, "image_material_bg"));
	auto image_item_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(durable_1_clone, "image_item_bg_lv"));
	auto label_items_name = dynamic_cast<Text*>(Helper::seekWidgetByName(durable_1_clone, "label_items_name"));
	auto image_num_2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(durable_1_clone, "image_num_2"));//船顺序
	if (order == -10)
	{
		image_num_2->setVisible(false);
	}
	else
	{
		image_num_2->setVisible(true);
		image_num_2->loadTexture(getPositionIconPath(order + 1));
	}
	
	Text* label_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(durable_1_clone, "label_num_1"));
	Text* label_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(durable_1_clone, "label_num_2"));
	Text* label_num_3 = dynamic_cast<Text*>(Helper::seekWidgetByName(durable_1_clone, "label_num_3"));
	Text* label_num_4 = dynamic_cast<Text*>(Helper::seekWidgetByName(durable_1_clone, "label_num_4"));
	Text* label_num_5 = dynamic_cast<Text*>(Helper::seekWidgetByName(durable_1_clone, "label_num_5"));
	auto listview_num = dynamic_cast<ListView*>(Helper::seekWidgetByName(durable_1_clone, "listview_num"));
	auto image_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(durable_1_clone,"image_ship"));
	std::string s_durable = StringUtils::format("%d(-%d)/%d(-%d)", hp, lossHp, maxHp, lossMaxhp);
	CC_SAFE_RETAIN(durable_1_clone);
	if (durable_1_clone->getParent()){
		durable_1_clone->removeFromParent();
	}
	CC_SAFE_RETAIN(image_div_1_clone);
	if (image_div_1_clone->getParent())
	{
		image_div_1_clone->removeFromParent();
	}
	label_num_1->setString(StringUtils::format("%d(", hp));
	label_num_2->setString(StringUtils::format("-%d", lossHp));
	label_num_2->setTextColor(Color4B::RED);
	label_num_3->setString(StringUtils::format(")/%d(", maxHp));
	label_num_4->setString(StringUtils::format("-%d", lossMaxhp));
	if (lossMaxhp>0)
	{
		label_num_4->setTextColor(Color4B::RED);
	}
	else
	{
		label_num_4->setTextColor(Color4B(40, 25, 13,255));
	}
	label_num_5->setString(StringUtils::format(")"));
	image_ship->ignoreContentAdaptWithSize(false);
	image_ship->loadTexture(getShipIconPath(shipid));
	label_items_name->setString(shipname);
	listview_num->refreshView();
	m_pListview_result->pushBackCustomItem(durable_1_clone);
	if (order!=shipCounts)
	{
		//分割线
		m_pListview_result->pushBackCustomItem(image_div_1_clone);
	}
	//稀有度
	setBgButtonFormIdAndType(image_material_bg, shipid, ITEM_TYPE_SHIP);
	setBgImageColorFormIdAndType(image_item_bg_lv, shipid, ITEM_TYPE_SHIP);
	setTextColorFormIdAndType(label_items_name, shipid, ITEM_TYPE_SHIP);
	CC_SAFE_RELEASE(durable_1_clone);
	CC_SAFE_RELEASE(image_div_1_clone);
}
//丢失最大耐久
void UISailHUD::lossMaxDurable(const int shipid, const int num,std::string shipname){

	Layout* panel_ship_durable_2 = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_pFloatResultview, "panel_ship_durable_1"));
	Widget* durable_2_clone = panel_ship_durable_2->clone();
	durable_2_clone->setCameraMask(4,true);
	auto image_material_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(durable_2_clone, "image_material_bg"));
	auto image_item_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(durable_2_clone, "image_item_bg_lv"));
	auto label_items_name = dynamic_cast<Text*>(Helper::seekWidgetByName(durable_2_clone, "label_items_name"));

	Text* label_num = dynamic_cast<Text*>(Helper::seekWidgetByName(durable_2_clone, "label_num"));
	auto image_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(durable_2_clone,"image_ship"));
	std::string s_durable = StringUtils::format("-%d", num);
	CC_SAFE_RETAIN(durable_2_clone);
	if (durable_2_clone->getParent()){
		durable_2_clone->removeFromParent();
	}
	
	label_num->setTextColor(Color4B::RED);
	label_num->setString(s_durable);
	image_ship->ignoreContentAdaptWithSize(false);
	image_ship->loadTexture(getShipIconPath(shipid));
	label_items_name->setString(shipname);
	m_pListview_result->pushBackCustomItem(durable_2_clone);
	//稀有度
	setBgButtonFormIdAndType(image_material_bg, shipid, ITEM_TYPE_SHIP);
	setBgImageColorFormIdAndType(image_item_bg_lv, shipid, ITEM_TYPE_SHIP);
	setTextColorFormIdAndType(label_items_name, shipid, ITEM_TYPE_SHIP);
	CC_SAFE_RELEASE(durable_2_clone);
}

void UISailHUD::addOrLossItem(const int id, const int num){

	Layout* panel_item_1 = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_pFloatResultview, "panel_item_1"));
	Widget* panel_item_1_clone = panel_item_1->clone();
	panel_item_1_clone->setCameraMask(4,true);
	Text* label_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_item_1_clone, "label_num"));
	auto image_material_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_item_1_clone, "image_material_bg"));
	ImageView* image_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_item_1_clone, "image_item"));
	Text* label_items_name = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_item_1_clone, "label_items_name"));

	auto image_item_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_item_1_clone, "image_item_bg_lv"));

	std::string s_durable = StringUtils::format("+%d", num);
	std::string  icon_path = getItemIconPath(id);
	std::string  icon_name = SINGLE_SHOP->getItemData()[id].name;
	CC_SAFE_RETAIN(panel_item_1_clone);
	if (panel_item_1_clone->getParent()){
		panel_item_1_clone->removeFromParent();
	}
	image_material_bg->setTag(id);
	image_material_bg->setTouchEnabled(false);
	image_material_bg->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func,this));

	image_item->loadTexture(icon_path);
	label_num->setTextColor(Color4B(46,125,50,255));
	label_num->setString(s_durable);
	label_items_name->setString(icon_name);
	m_pListview_result->pushBackCustomItem(panel_item_1_clone);
	//稀有度
	setBgButtonFormIdAndType(image_material_bg, id, ITEM_TYPE_PORP);
	setBgImageColorFormIdAndType(image_item_bg_lv, id, ITEM_TYPE_PORP);
	setTextColorFormIdAndType(label_items_name, id, ITEM_TYPE_PORP);
	CC_SAFE_RELEASE(panel_item_1_clone);

}

void UISailHUD::addOrLossCoin(const int num){

	Layout* panel_silver = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_pFloatResultview, "panel_coin"));
	Widget* image_div_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pFloatResultview, "image_div_1"));
	auto image_div_line = (Widget*)image_div_1->clone();
	Widget* panel_silver_clone = panel_silver->clone();
	panel_silver_clone->setCameraMask(4,true);
	image_div_line->setCameraMask(4, true);
	Text* label_buy_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_silver_clone, "label_buy_num"));
	std::string pluscoin = "+";

	std::string s_durable = numSegment(StringUtils::format("%d", num));
	CC_SAFE_RETAIN(panel_silver_clone);
	if (panel_silver_clone->getParent()){
		panel_silver_clone->removeFromParent();
	}
	if (num < 0)
	{
		label_buy_num->setTextColor(Color4B(46, 125, 50, 255));
		label_buy_num->setString(s_durable);
	}
	else
	{
		label_buy_num->setString(pluscoin + s_durable);
	}
	
	m_pListview_result->pushBackCustomItem(panel_silver_clone);
	m_pListview_result->pushBackCustomItem(image_div_line);
	CC_SAFE_RELEASE(panel_silver_clone);

}

void UISailHUD::updateSeaEventView(const int type,const int viewFlag, TriggerSeaEventResult* result)
{
	if (m_vOpenViews.size() > 0)
	{
		return;
	}

	std::string s_chat("");
	std::string t_chat("");
	int cowId = 0;
 	auto tipInfo = SINGLE_SHOP->getTipsInfo();
	switch (type)
	{
	
	case EV_SEA_STORM:
		s_chat = tipInfo["TIP_SAILING_SHIP_EVENT_STORM"];
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_STORM"];
		break;
	case EV_SEA_FLOAT:
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_FLOAT_19);
		s_chat = tipInfo["TIP_SAILING_SHIP_EVENT_FLOAT"];
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_FLOAT"];
		if (result->whilewhale > 0 && result->n_bowicon > 0)
		{
			ShipBowIconInfo ** bowIcons = result->bowicon;
			cowId = confirmShipBowId(bowIcons, result->n_bowicon, EV_SEA_FLOAT);
		}
		break;
			
	case EV_SEA_ROCK:
		s_chat = tipInfo["TIP_SAILING_SHIP_EVENT_ROCK"];
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_ROCK"];
		break;
	case EV_SEA_BIG_FOG:
	case EV_SEA_BOSS:
	case EV_SEA_PESTIS:
        if (result->usedpropid != 0)
        {
            s_chat = tipInfo["TIP_SAILING_SHIP_EVENT_PESTIS_PROP"];
        }
        else
        {
            s_chat = tipInfo["TIP_SAILING_SHIP_EVENT_PESTIS"];
        }
		
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_PESTIS"];
		break;
	case EV_SEA_HOMESICK:
        if (result->usedpropid != 0)
        {
            s_chat = tipInfo["TIP_SAILING_SHIP_EVENT_HOMESICK_PROP"];
        }
        else
        {
            s_chat = tipInfo["TIP_SAILING_SHIP_EVENT_HOMESICK"];
        }
		
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_HOMESICK"];
		break;
	case EV_SEA_SEPSIS:
        if (result->usedpropid != 0)
        {
            s_chat = tipInfo["TIP_SAILING_SHIP_EVENT_SEPSIS_PROP"];
        }
        else
        {
            s_chat = tipInfo["TIP_SAILING_SHIP_EVENT_SEPSIS"];
        }
		
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_SEPSIS"];
		break;
	default:
		break;
	}
	//保存数据
	m_pTriggerSeaResult = result;
	seaEventTitle = t_chat;

	openView(MAPUI_COCOS_RES[INDEX_UI_FLOAT_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FLOAT_CSB]);
	view->setCameraMask(4, true);


	Size winSize = Director::getInstance()->getWinSize();
	view->setPosition(winSize/2);
	view->setScale(0);
	//海上事件按钮
	auto button_continue = view->getChildByName<Button*>("button_continue");
	button_continue->setName("button_seaevent_continue");
	button_continue->addTouchEventListener(CC_CALLBACK_2(UISailHUD::onSeaEvent,this));

	ImageView* image_accident_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_accident_1"));
	std::string s_event_bg = StringUtils::format("res/event_type/event_%d.jpg",type);
	image_accident_1->loadTexture(s_event_bg);
	
	ImageView* image_props=dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_items_1_0"));
	ImageView*image_goods=dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_props, "image_goods"));
	auto atlaslabel_num_1=dynamic_cast<Text*>(Helper::seekWidgetByName(image_props, "text_item_num"));
	auto btn_item = view->getChildByName("button_equip_items");
	image_props->setVisible(false);
	if (result && result->usedpropid != 0)
	{
		image_props->setVisible(true);
		image_goods->ignoreContentAdaptWithSize(false);
		image_goods->loadTexture(getItemIconPath(result->usedpropid));
		btn_item->setTag(result->usedpropid + 10000);
		//海上道具数量
		setTextSizeAndOutline(atlaslabel_num_1,result->usedpropremaincount+1);

        
        auto action1 = FadeIn::create(2.0f);
        auto action1Back = action1->reverse();
        auto action2 = CallFunc::create([=](){atlaslabel_num_1->setString(StringUtils::format("%d", result->usedpropremaincount)); });
        atlaslabel_num_1->runAction(Sequence::create(action1, action2, action1Back, nullptr));
	}
	auto spa = Spawn::createWithTwoActions(MoveTo::create(0.3f,Vec2(0,0)),ScaleTo::create(0.3f,1.0));
	view->runAction(Sequence::create(EaseQuinticActionOut::create(spa), CallFunc::create(this, callfunc_selector(UISailHUD::removeLayerForSeaEvent)), nullptr));
	Text* label_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title"));
	Text* label_content = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_content"));
	label_title->setString(t_chat);
	label_content->setString(s_chat);
	label_title->setVisible(true);
	label_content->setVisible(true);
	if (cowId > 0)
	{
		auto t_labelCount = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_content"));
		auto b_continue = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_continue"));
		auto i_image = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_goods"));
		auto t_atlas = Helper::seekWidgetByName(view, "text_item_num");
		auto iconPath = getItemIconPath(cowId);
		auto t_tile = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title"));
		ImageView* image_accident_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_accident_1"));
		std::string s_event_bg = StringUtils::format("res/event_type/event_%d.jpg", type);
		t_atlas->setVisible(false);
		i_image->loadTexture(iconPath);
		btn_item->setTag(cowId);
		image_props->setVisible(true);
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
		Animation* animation_shiplight = Animation::createWithSpriteFrames(frames, 2.0 / 28, 1);
		auto sp = Sprite::create();
		sp->setPosition(Vec2(1081, 554));
		sp->setCameraMask(4);
		sp->setScale(2);
		i_image->getParent()->setLocalZOrder(2);
		view->addChild(sp, 1);
		sp->runAction(RepeatForever::create(Animate::create(animation_shiplight)));
	}

	m_bIsBlocking = false;

}

void UISailHUD::update(float f)
{
	if(!m_pShip) return;
	Vec2 pos = m_pShip->getPosition();
	//SINGLE_HERO->m_bInSafeArea = m_bCurrentInSafeArea;
	if(m_SafeAreaCheck->landHitTest(pos.x,pos.y)){
		//安全海域
		//log("safe  %f, %f",pos.x, pos.y);
		if(!m_bCurrentInSafeArea){
			SINGLE_HERO->m_bInSafeArea = true;
			m_bCurrentInSafeArea = true;
			m_pSafeAreaLabel->stopAllActions();
			m_pSafeAreaLabel->setVisible(true);
			m_pSafeAreaLabel->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAFE_AREA"]);
			m_pSafeAreaLabel->setTextColor(Color4B(17, 222, 31, 255));
		}
	}else{
		//log("not safe  %f, %f",pos.x, pos.y);
		//危险海域
		if(m_bCurrentInSafeArea){
			SINGLE_HERO->m_bInSafeArea = false;
			m_bCurrentInSafeArea = false;
			m_pSafeAreaLabel->setString(SINGLE_SHOP->getTipsInfo()["TIP_DANGEROUS_AREA"]);
			m_pSafeAreaLabel->setTextColor(Color4B(251, 44, 44, 255));
			m_pSafeAreaLabel->runAction(Blink::create(2, 3));
		}
	}
	SINGLE_HERO->shippos=pos;
	bool b_noMap_zone = true;
	for (int i = 0; i< 19; i++)
	{
		if(m_MapRect[i].containsPoint(pos))
		{
			if (i != m_nCurMapIndex)
			{
				m_nCurMapIndex = i;
				std::string mapName = StringUtils::format("ship/seaFiled_1/map_%d.jpg",i+1);
				m_pSmallMap->loadTexture(mapName);
				m_pLandName->setString(SINGLE_SHOP->getLandInfo()[i + 1].name);
				mapNameAction();
				
			}
			ProtocolThread::GetInstance()->getSeaIndex(i + 1);
			b_noMap_zone = false;
			break;
		}
	}
	//no map zone
	
	if (b_noMap_zone)
	{
		m_pLandName->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_UNKNOW_SEA"]);
		m_pSmallMap->loadTexture("ship/seaFiled_1/no_map_zone.jpg");
		m_nCurMapIndex = -1;
	}
	//if (!m_MapRect[0].containsPoint(pos)&&!m_MapRect[1].containsPoint(pos)&&!m_MapRect[2].containsPoint(pos)&&!m_MapRect[3].containsPoint(pos)&&!m_MapRect[4].containsPoint(pos)
	//	&&!m_MapRect[5].containsPoint(pos)&&!m_MapRect[6].containsPoint(pos)&&!m_MapRect[7].containsPoint(pos)&&!m_MapRect[8].containsPoint(pos))
	//{
	//	m_pLandName->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_UNKNOW_SEA"]);
	//	m_pSmallMap->loadTexture("ship/seaFiled_1/no_map_zone.jpg");
	//	m_nCurMapIndex=-1;
	//}
	//更新雷达位置
	else
	{
		auto landInfo = SINGLE_SHOP->getLandInfo();
		Vec2 curZeroPos = Vec2(SmallMaps[m_nCurMapIndex].getMinX(),SmallMaps[m_nCurMapIndex].getMinY());
		Size smallSize = m_pSmallMap->getContentSize();
		Vec2 diffPos = pos - curZeroPos;
		int origBorder = 120;
		float imgScale = smallSize.height / 480;
		int border = origBorder * imgScale;
		float scaleX = ((smallSize.width - 2*border)/(SmallMaps[m_nCurMapIndex].size.width)); 
		float scaleY = ((smallSize.height - 2*border)/(SmallMaps[m_nCurMapIndex].size.height));
		if (m_nCurMapIndex == 9)//加勒比海东
		{
			diffPos.x = diffPos.x*  scaleX;
		}
		else
		{
			diffPos.x = (diffPos.x + (7518 - (landInfo[m_nCurMapIndex + 1].right_up_x - landInfo[m_nCurMapIndex + 1].left_down_x)))*  scaleX;
		}
		diffPos.y = diffPos.y *  scaleY;

		if (diffPos.x < 0)
		{
			diffPos.x = 0;
		}else if(diffPos.x > SmallMaps[m_nCurMapIndex].getMaxX())
		{
			diffPos.x = SmallMaps[m_nCurMapIndex].getMaxX();
		}
		if (diffPos.y < 0)
		{
			diffPos.y = 0;
		}else if(diffPos.y > SmallMaps[m_nCurMapIndex].getMaxY())
		{
			diffPos.y = SmallMaps[m_nCurMapIndex].getMaxY();
		}
		//TODO: we should put the m_SmallMap at (0,0), not in the center.
		m_pSmallMap->setPosition(Vec2(smallSize.width/2+border,smallSize.height/2+border)-(diffPos + Vec2(border,border)) - Vec2(40,40));
	}
	m_pSmallMap->setCameraMask(4, true);

	if(m_nForceFightNpcId > 0){
		m_nForceFightNpcId = 0;
		SINGLE_HERO->m_forceNPCId = 0;
		m_nDisableCancelFight = 2;
		m_bIsBlocking = true;
		shipPause(true);
		ProtocolThread::GetInstance()->engageInFight(-1, m_nDisableCancelFight, m_nEventversion, FIGHT_TYPE_NORMAL, 0, UILoadingIndicator::createWithMask(this, 4));
	}
}

void UISailHUD::flushSeaChat()
{
	auto viewSeaMain = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);

	auto i_chat = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewSeaMain, "image_chat"));
	auto p_chat = dynamic_cast<Widget*>(Helper::seekWidgetByName(i_chat, "panel_chat"));
	auto seaChat = dynamic_cast<Text*>(Helper::seekWidgetByName(p_chat, "label_chat_content"));
	if (!m_mainChat)
	{
		m_mainChat = RichText::create();
		p_chat->addChild(m_mainChat);
		p_chat->setCameraMask(4, true);
		p_chat->setLocalZOrder(50);
	}

	if (m_nChatItem)
	{
		m_mainChat->removeElement(1);
		m_mainChat->removeElement(0);
		m_nChatItem = 0;
	}
	
	if (UISocial::getInstance()->getSocialChat().empty())
	{
		m_bIsFirstChat = false;
		i_chat->setVisible(false);
		m_mainChat->runAction(Sequence::createWithTwoActions(DelayTime::create(1),
			CallFunc::create(CC_CALLBACK_0(UISailHUD::flushSeaChat, this))));
	}
	else
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
		m_mainChat->pushBackElement(t_name);
		m_mainChat->pushBackElement(t_content);
		m_mainChat->formatText();
		m_mainChat->setPosition(Vec2(m_mainChat->getContentSize().width*0.45 + i_chat->getContentSize().width, seaChat->getPositionY()));
		float t_tatol = m_mainChat->getContentSize().width + i_chat->getContentSize().width;
		float time = t_tatol / 100;
		if (m_nShowChatTime < 4)
		{
			i_chat->setVisible(true);
			m_mainChat->runAction(Sequence::createWithTwoActions(MoveTo::create(time, Vec2(-m_mainChat->getContentSize().width*0.55, m_mainChat->getPositionY())),
				CallFunc::create(CC_CALLBACK_0(UISailHUD::flushSeaChat, this))));
		}else
		{
			i_chat->setVisible(false);
			m_mainChat->runAction(Sequence::createWithTwoActions(DelayTime::create(1),
				CallFunc::create(CC_CALLBACK_0(UISailHUD::flushSeaChat, this))));
		}
		i_chat->setCameraMask(4, true);
	}
}

bool UISailHUD::showSeaEvent(TVSeaEvent* se)
{
	LayerColor* blacklayer = LayerColor::create(Color4B(0,0,0,255));
	blacklayer->setCameraMask(4,true);
	blacklayer->setTouchEnabled(true);
	auto listener1 = EventListenerTouchOneByOne::create();
	listener1->setSwallowTouches(true);
	listener1->onTouchBegan = [](Touch* touch, Event* event)
	{
		return true;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, blacklayer);
	//当blacklayer析构时，监听器将会被自动析构。
	TintTo* tt = TintTo::create(.3f,255,255,255);
	TintTo* tt_rev = TintTo::create(.3f,0,0,0);
	
	switch (se->m_Type)
	{
	case EV_SEA_NPC:
		{
			if(se->shouldTriggerBattle() && se->m_Event_id != dynamic_cast<UISailManage*>(_parent)->getLastNPCEventId()){
				m_nDisableCancelFight = 1;
			}
			//SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CRASH_18);
			//SINGLE_AUDIO->vpauseBGMusic();
			ProtocolThread::GetInstance()->engageInFight(se->m_Event_id, m_nDisableCancelFight, m_nEventversion, FIGHT_TYPE_NORMAL, 0, UILoadingIndicator::createWithMask(this, 4));
			break;
		}
	case EV_SEA_FLOAT:
		{
			//pause background music.
			//SINGLE_AUDIO->vpauseBGMusic();	
			ProtocolThread::GetInstance()->triggerSeaEvent(se->m_Event_id, 0, UILoadingIndicator::createWithMask(this, 4));
			break;
		}
	case EV_SEA_STORM:
		{
			//pause background music.
			//SINGLE_AUDIO->vpauseBGMusic();
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_STORM_02);

			blacklayer->runAction(Sequence::createWithTwoActions(Repeat::create(Sequence::createWithTwoActions(tt,
				tt_rev),3),CallFunc::create(CC_CALLBACK_0(UISailHUD::playAnimationEventEnd,this,blacklayer,se))));
			this->addChild(blacklayer);
			blacklayer->setCameraMask(4,true);
			break;
		}
	case EV_SEA_ROCK:
		{
			//pause background music.
			//SINGLE_AUDIO->vpauseBGMusic();
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_AGROUND_03);

			blacklayer->runAction(Sequence::createWithTwoActions(Repeat::create(Sequence::createWithTwoActions(tt,
				tt_rev),3),CallFunc::create(CC_CALLBACK_0(UISailHUD::playAnimationEventEnd,this,blacklayer,se))));
			this->addChild(blacklayer);
			blacklayer->setCameraMask(4,true);
			break;
		}

	case EV_SEA_BIG_FOG:
	case EV_SEA_BOSS:
	case EV_SEA_PESTIS:
	case EV_SEA_HOMESICK:
	case EV_SEA_SEPSIS:
		{
			//pause background music.
			//SINGLE_AUDIO->vpauseBGMusic();
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_EMERGENCY_22);

			blacklayer->runAction(Sequence::createWithTwoActions(Repeat::create(Sequence::createWithTwoActions(tt,
				tt_rev),3),CallFunc::create(CC_CALLBACK_0(UISailHUD::playAnimationEventEnd,this,blacklayer,se))));
			this->addChild(blacklayer);
			blacklayer->setCameraMask(4);
			return true;
		}
	default:
		break;
	}
	return false;
}

void UISailHUD::playAnimationEventEnd(Node* psend,TVSeaEvent* se)
{
	psend->removeFromParent();
	addLayerForSeaEvent();

	auto layer = dynamic_cast<UISailManage*>(this->getParent());
	LeaveCityResult * result = layer->getLeaveCityResult();
	if (result->angelrate >0 || result->avoidrockrate > 0 || result->avoidstormrate >0)
	{
		int rate = random(1,100);
		int avoidEventRate = -1;
		std::string titles = "";
		int BowIconId = 0;
		SEA_EVENT_TYPE even_tpye = EV_SEA_NPC;
		switch (se->m_Type)
		{
			//触礁 海豚
		case EV_SEA_ROCK:
		{
							avoidEventRate = result->avoidrockrate;
							titles = "TIP_MAPUI_AVOID_ROCKS_TIP";
							even_tpye = EV_SEA_ROCK;
							break;
		}
			//暴风雨 海神
		case EV_SEA_STORM:
		{
							 avoidEventRate = result->avoidstormrate;
							 titles = "TIP_MAPUI_AVOID_STORMS_TIP";
							 even_tpye = EV_SEA_STORM;
							 break;
		}
			//疾病 天使
		case  EV_SEA_SEPSIS:
		{
							   avoidEventRate = result->angelrate;
							   titles = "TIP_MAPUI_AVOID_SEPSIS_TIP";
							   even_tpye = EV_SEA_SEPSIS;
							   break;
		}
			//鼠疫 天使
		case  EV_SEA_PESTIS:
		{
							   avoidEventRate = result->angelrate;
							   titles = "TIP_MAPUI_AVOID_PESTIS_TIP";
							   even_tpye = EV_SEA_PESTIS;
							   break;
		}
			//思乡 天使
		case  EV_SEA_HOMESICK:
		{
								 avoidEventRate = result->angelrate;
								 titles = "TIP_MAPUI_AVOID_HOMESICK_TIP";
								 even_tpye = EV_SEA_HOMESICK;
								 break;
		}
		default:
			break;
		}
		if (avoidEventRate >= rate)
		{
			ShipBowIconInfo **bowicons = result->bowicon;
			int  n = result->n_bowicon;
			BowIconId = confirmShipBowId(result->bowicon, result->n_bowicon, even_tpye);
			shipStatueAvoidSeaEvent(titles, BowIconId, even_tpye);
		}
		else{
			switch (se->m_Type)
			{
			case EV_SEA_FLOAT:
			case EV_SEA_STORM:
			case EV_SEA_ROCK:
			case EV_SEA_BIG_FOG:
			case EV_SEA_BOSS:
			case EV_SEA_PESTIS:
			case EV_SEA_HOMESICK:
			case EV_SEA_SEPSIS:
			{
								  ProtocolThread::GetInstance()->triggerSeaEvent(se->m_Event_id, 0, UILoadingIndicator::createWithMask(this, 4));
								  break;
			}
			default:
				break;
			}
		}
	}
	else
	{
		switch (se->m_Type)
		{
		case EV_SEA_FLOAT:
		case EV_SEA_STORM:
		case EV_SEA_ROCK:
		case EV_SEA_BIG_FOG:
		case EV_SEA_BOSS:
		case EV_SEA_PESTIS:
		case EV_SEA_HOMESICK:
		case EV_SEA_SEPSIS:
		{
							  ProtocolThread::GetInstance()->triggerSeaEvent(se->m_Event_id, 0, UILoadingIndicator::createWithMask(this, 4));
							  break;
		}
		default:
			break;
		}
	}
}
void UISailHUD::shipPause(bool isPause)
{
	if (isPause)
	{
		dynamic_cast<UISailManage*>(_parent)->seaEventStart();
	}else
	{
		dynamic_cast<UISailManage*>(_parent)->seaEventEnd();
	}
	
}
//mew sound
void UISailHUD::mewSound(float f)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_MEW_06);
}

void UISailHUD::flushPanelFleetInfo(GetFleetAndDockShipsResult* result){

	Button* buttonShipInfo = dynamic_cast<Button*>(Helper::seekWidgetByName(m_pPanel_fleet_info, "button_shipinfo"));
	m_pFleetScrollView = dynamic_cast<cocos2d::ui::ScrollView*>(Helper::seekWidgetByName(m_pPanel_fleet_info, "fleet_scrollView"));
	m_pFleet_image_title_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pPanel_fleet_info, "fleet_image_title_bg"));
	Text* label_player_name = dynamic_cast<Text *>(Helper::seekWidgetByName(m_pFleet_image_title_bg, "label_name"));
	Text* label_lv = dynamic_cast<Text *>(Helper::seekWidgetByName(m_pFleet_image_title_bg, "label_lv"));
	auto i_nation = dynamic_cast<ImageView *>(Helper::seekWidgetByName(m_pFleet_image_title_bg, "image_flag"));
	
	label_lv->setString(String::createWithFormat("Lv. %d", SINGLE_HERO->m_iLevel)->_string);
	label_player_name->setString(SINGLE_HERO->m_sName);
	label_lv->setVisible(true);
	label_player_name->setVisible(true);
	i_nation->loadTexture(getCountryIconPath(SINGLE_HERO->m_iNation));

	ImageView* image_moral_bg = dynamic_cast<ImageView *>(Helper::seekWidgetByName(m_pFleet_image_title_bg, "image_moral_bg"));

	for (int i = 1; i <= 5; i++)
	{

		std::string str_moral = StringUtils::format("image_moral_%d", i);
		ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(m_pFleet_image_title_bg, str_moral));
		moral_image->loadTexture(getMoralIconPath(3));

	}
	int num_morale = result->morale;
	int n1000 = (num_morale / 1000) % 6;
	int n100 = num_morale - n1000 * 1000;
	for (int i = 1; i <= n1000; i++)
	{
		std::string str_moral = StringUtils::format("image_moral_%d", i);
		ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(m_pFleet_image_title_bg, str_moral));
		moral_image->loadTexture(getMoralIconPath(1));

	}
	if (n1000 < 5 && n100 >= 50)
	{
		std::string str_moral = StringUtils::format("image_moral_%d", n1000 + 1);
		ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(m_pFleet_image_title_bg, str_moral));
		moral_image->loadTexture(getMoralIconPath(2));
	}

	m_vCaptainList.clear();

	if (result->n_fleetships < 1)
	{
		buttonShipInfo->setVisible(false);
	}
	else
	{
		for (int i = 0; i < result->n_fleetships + result->n_catchships; ++i)
		{

			ShipDefine* fleetShip = nullptr;
			if (i < result->n_fleetships)
			{
				fleetShip = result->fleetships[i];
				m_vCaptainList.push_back(fleetShip->captain_id);
			}
			else
			{
				fleetShip = result->catchships[i - result->n_fleetships];
				m_vCaptainList.push_back(nullptr);
			}
			Button* w_clone = dynamic_cast<Button*>(buttonShipInfo->clone());
			w_clone->setTag(i);
			initFleetShipInfo(w_clone, fleetShip, i);
			w_clone->setPosition(Vec2(w_clone->getContentSize().width / 2 + 30 + (i % 2) * (m_pFleetScrollView->getContentSize().width / 2 - 30), m_pFleetScrollView->getInnerContainerSize().height - w_clone->getContentSize().height / 2 - (i / 2) * w_clone->getContentSize().height - 10));
			m_pFleetScrollView->addChild(w_clone);
			w_clone->setCameraMask(4, true);
		}
	}

	auto p_captured = dynamic_cast<Widget*>(Helper::seekWidgetByName(m_pPanel_fleet_info, "panel_captured"));
	auto t_captured = p_captured->getChildByName<Text*>("label_captured");
	auto t_captured_num = p_captured->getChildByName<Text*>("label_captured_num");
	t_captured_num->setString(StringUtils::format("%d/%d", result->n_catchships, result->cantotalcatchshipnum));
	t_captured->setPositionX(t_captured_num->getPositionX() - t_captured_num->getBoundingBox().size.width / 2 - t_captured->getBoundingBox().size.width/2);
}

CaptainDefine* UISailHUD::getCaptainDefine(int id) const
{
	for (int i = 0; i < m_pResult_fleet->n_captains; ++i)
	{
		if (id == m_pResult_fleet->captains[i]->id)
		{
			return m_pResult_fleet->captains[i];
		}
	}
	return nullptr;
}

void UISailHUD::initFleetShipInfo(Button* item, ShipDefine* shipDefine, int tag){
	
	Button *button_head_player = dynamic_cast<Button *>(Helper::seekWidgetByName(item, "button_head_player"));
	button_head_player->setTouchEnabled(false);
	ImageView *image_head = dynamic_cast<ImageView*>(button_head_player->getChildByName("image_head"));
	auto t_lv_bg = dynamic_cast<ImageView *>(Helper::seekWidgetByName(item, "image_head_lv"));
	Text* label_lv = dynamic_cast<Text *>(Helper::seekWidgetByName(item, "label_lv"));
	auto image_ship_bg_1 = dynamic_cast<Widget *>(Helper::seekWidgetByName(item, "image_ship_bg_1"));
	ImageView *image_item_bg_lv = dynamic_cast<ImageView *>(Helper::seekWidgetByName(item, "image_item_bg_lv"));

	ImageView *image_ship = dynamic_cast<ImageView *>(Helper::seekWidgetByName(item, "image_ship"));
	ImageView *image_num = dynamic_cast<ImageView *>(Helper::seekWidgetByName(item, "image_num"));
	Text* label_ship_name = dynamic_cast<Text *>(Helper::seekWidgetByName(item, "label_ship_name"));

	Layout* panel_durable = dynamic_cast<Layout *>(Helper::seekWidgetByName(item, "panel_durable"));
	Text* label_ship_durable_num_1 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_durable, "label_ship_durable_num_1"));
	Text* label_ship_durable_num_2 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_durable, "label_ship_durable_num_2"));
	LoadingBar* progressbar_durable = dynamic_cast<LoadingBar *>(Helper::seekWidgetByName(panel_durable, "progressbar_durable"));

	Layout* panel_sailor = dynamic_cast<Layout *>(Helper::seekWidgetByName(item, "panel_sailor"));
	Text* label_ship_sailor_num_1 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_sailor, "label_ship_sailor_num_1"));
	Text* label_ship_sailor_num_2 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_sailor, "label_ship_sailor_num_2"));
	LoadingBar* progressbar_sailor = dynamic_cast<LoadingBar *>(Helper::seekWidgetByName(panel_sailor, "progressbar_sailor"));

	int32_t hp = shipDefine->hp;
	int32_t max_hp = shipDefine->current_hp_max;
	label_ship_durable_num_1->setString(StringUtils::format("%d/", hp));
	label_ship_durable_num_2->setString(StringUtils::format("%d", max_hp));
	label_ship_durable_num_2->setPositionX(label_ship_durable_num_1->getPositionX() + label_ship_durable_num_1->getBoundingBox().size.width);
	if (shipDefine->current_hp_max < shipDefine->hp_max)
	{
		label_ship_durable_num_2->setTextColor(TEXT_RED);
	}

	if (0 == max_hp){
		progressbar_durable->setPercent(0);
	}
	else{
		progressbar_durable->setPercent(hp * 100 / max_hp);
	}

	int32_t sailorNum = shipDefine->crew_num;
	int32_t max_sailorNum = shipDefine->max_crew_num;
	label_ship_sailor_num_1->setString(StringUtils::format(" %d", sailorNum));
	label_ship_sailor_num_2->setString(StringUtils::format("/%d", max_sailorNum));
	label_ship_sailor_num_2->setPositionX(label_ship_sailor_num_1->getPositionX() + label_ship_sailor_num_1->getBoundingBox().size.width);
	if (0 == max_sailorNum){
		progressbar_sailor->setPercent(0);
	}
	else{
		progressbar_sailor->setPercent(sailorNum * 100 / max_sailorNum);
	}

	std::string num_name = getPositionIconPath(tag + 1);
	image_num->loadTexture(num_name);

	
	ImageView *image_moral_bg = dynamic_cast<ImageView *>(Helper::seekWidgetByName(m_pFleet_image_title_bg, "image_moral_bg"));
	auto i_capture = dynamic_cast<ImageView *>(Helper::seekWidgetByName(item, "image_capture"));
	auto image_div = dynamic_cast<ImageView *>(Helper::seekWidgetByName(item, "image_div"));
	i_capture->setVisible(false);
	if (tag == 0)
	{
		button_head_player->setBright(true);
		button_head_player->setVisible(true);
		image_head->setVisible(true);
		image_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));
		label_lv->setString(String::createWithFormat("Lv. %d", SINGLE_HERO->m_iLevel)->_string);
		t_lv_bg->setVisible(true);

	}
	else if (m_vCaptainList[tag] && m_vCaptainList[tag]->captainid == 0)
	{
		image_head->setVisible(false);
		t_lv_bg->setVisible(false);
		image_div->setVisible(false);
	}
	else if (!m_vCaptainList[tag])
	{
		i_capture->setVisible(true);
		image_div->setVisible(false);
		image_head->setVisible(false);
		t_lv_bg->setVisible(false);
		image_num->setVisible(false);
	}
	else
	{
		CaptainDefine* captainDefine = getCaptainDefine(m_vCaptainList[tag]->captainid);
		if (captainDefine)
		{
			std::string captainPath = getCompanionIconPath(captainDefine->protoid, captainDefine->iscaptain);
			button_head_player->setBright(true);
			image_head->setVisible(true);
			image_head->loadTexture(captainPath);
			label_lv->setString(String::createWithFormat("Lv. %d", captainDefine->level)->_string);
			t_lv_bg->setVisible(true);
		}
	}
	std::string shipNameStr;
	if (shipDefine->user_define_name == nullptr || std::strcmp(shipDefine->user_define_name, "") == 0)
	{
		shipNameStr = getShipName(shipDefine->sid);
	}
	else
	{
		shipNameStr = String::createWithFormat("%s", shipDefine->user_define_name)->_string;
	}
	auto nMaxStringNum = 0;
	auto languageTypeNum = 0;
	if (isChineseCharacterIn(shipNameStr.c_str()))
	{
		languageTypeNum = 1;
	}
	if (languageTypeNum)
	{
		if (tag == 0)
		{
			nMaxStringNum = 10;
		}
		else
		{
			nMaxStringNum = 15;
		}
	}
	else
	{
		if (tag == 0)
		{
			nMaxStringNum = 20;
		}
		else
		{
			nMaxStringNum = 30;
		}
	}

	label_ship_name->setString(apostrophe(shipNameStr.c_str(), nMaxStringNum));

	std::string filePath = getShipIconPath(shipDefine->sid);
	image_ship->loadTexture(filePath);
	setBgButtonFormIdAndType(image_ship_bg_1, shipDefine->sid, ITEM_TYPE_SHIP);
	setBgImageColorFormIdAndType(image_item_bg_lv, shipDefine->sid, ITEM_TYPE_SHIP);
	setTextColorFormIdAndType(label_ship_name, shipDefine->sid, ITEM_TYPE_SHIP);
}

void UISailHUD::initShipDetails(const int tag){
	auto panel_fleet_shipdetails = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SHIPDETAILS_CSB]);
	panel_fleet_shipdetails->setCameraMask(4,true);
		auto image_durable = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_durable"));
	auto image_weight = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_weight"));
	auto image_sailor = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_sailor"));
	auto image_supply = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_supply"));
	auto image_atk = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_atk"));
	auto image_def = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_def"));
	auto image_speed = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_speed"));
	auto image_steering = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_steering"));
	image_durable->setTouchEnabled(true);
	image_durable->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	image_durable->setTag(IMAGE_ICON_DURABLE + IMAGE_INDEX2);
	image_weight->setTouchEnabled(true);
	image_weight->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	image_weight->setTag(IMAGE_ICON_CARGO + IMAGE_INDEX2);
	image_sailor->setTouchEnabled(true);
	image_sailor->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	image_sailor->setTag(IMAGE_ICON_SAILOR + IMAGE_INDEX2);
	image_supply->setTouchEnabled(true);
	image_supply->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	image_supply->setTag(IMAGE_ICON_SUPPLY + IMAGE_INDEX2);
	image_atk->setTouchEnabled(true);
	image_atk->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	image_atk->setTag(IMAGE_ICON_ATTACK + IMAGE_INDEX2);
	image_def->setTouchEnabled(true);
	image_def->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	image_def->setTag(IMAGE_ICON_DEFENCE + IMAGE_INDEX2);
	image_speed->setTouchEnabled(true);
	image_speed->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	image_speed->setTag(IMAGE_ICON_SPEED + IMAGE_INDEX2);
	image_steering->setTouchEnabled(true);
	image_steering->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	image_steering->setTag(IMAGE_ICON_STEERING + IMAGE_INDEX2);
	auto image_ship_bg = dynamic_cast<Widget *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_ship_bg"));
	auto image_item_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_ship_bg, "image_item_bg_lv"));
	Text* label_ship_name = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "label_ship_name"));
	label_ship_name->setFontName("");
	ImageView* image_num = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_num"));
	ImageView* image_ship = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_ship"));
	Text* label_ship_durable_num_1 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "label_ship_durable_num_1"));
	LoadingBar* progressbar_durable = dynamic_cast<LoadingBar *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "progressbar_durable"));
	Text* label_ship_sailor_num_1 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "label_ship_sailor_num_1"));
	LoadingBar* progressbar_sailor = dynamic_cast<LoadingBar *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "progressbar_sailor"));
	Text* label_ship_weight_num_1 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "label_ship_weight_num_1"));
	LoadingBar* progressbar_weight = dynamic_cast<LoadingBar *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "progressbar_weight"));
	Text* label_ship_supply_num_1 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "label_ship_supply_num_1"));
	LoadingBar* progressbar_supply = dynamic_cast<LoadingBar *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "progressbar_supply"));
	Text* label_atk_num = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "label_atk_num"));
	Text* label_defnum = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "label_defnum"));
	Text* label_speed_num = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "label_speed_num"));
	Text* label_steering_num = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "label_steering_num"));

	auto t_captain = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "label_enhancement"));
	Layout* panel_captain = dynamic_cast<Layout *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "panel_captain"));
	ImageView* image_captain_head = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_captain, "image_captain_head"));
	Button* button_captain_head = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_captain, "button_captain_head"));
	Text* label_lv = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_captain, "label_lv"));
	Text* label_name = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_captain, "label_name"));

	Text* label_lv_1 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_captain, "label_lv_1"));
	ImageView* skill_1 = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_captain, "skill_1"));

	Text* label_lv_2 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_captain, "label_lv_2"));
	ImageView* skill_2 = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_captain, "skill_2"));

	Text* label_lv_3 = dynamic_cast<Text *>(Helper::seekWidgetByName(panel_captain, "label_lv_3"));
	ImageView* skill_3 = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_captain, "skill_3"));

	std::string num_name = getPositionIconPath(tag + 1);
	image_num->loadTexture(num_name);
	auto image_div_2 = dynamic_cast<ImageView *>(Helper::seekWidgetByName(panel_fleet_shipdetails, "image_div_2"));
	if (tag == 0)
	{
		button_captain_head->setBright(true);
		image_captain_head->setVisible(true);
		image_captain_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));
		label_lv->setString(String::createWithFormat("Lv. %d", SINGLE_HERO->m_iLevel)->_string);
		label_name->setString(SINGLE_HERO->m_sName);
		label_lv->setVisible(true);
		label_name->setVisible(true);

		for (int i = 0; i < 3; ++i)
		{
			auto b_skill = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_captain, StringUtils::format("button_equip_bg_%d", i + 1)));
			b_skill->setVisible(false);
		}
		for (int i = 0; i < 3; ++i)
		{
			auto b_skill = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_captain, StringUtils::format("button_sp_equip_bg_%d", i + 1)));
			b_skill->setVisible(false);
		}

	}
	else if (m_vCaptainList[tag] && m_vCaptainList[tag]->captainid == 0)
	{
		panel_captain->setVisible(false);
		t_captain->setVisible(false);
		image_div_2->setVisible(false);
	}
	else if (!m_vCaptainList[tag])
	{
		image_num->setVisible(false);
		panel_captain->setVisible(false);
		t_captain->setVisible(false);
		image_div_2->setVisible(false);
	}
	else
	{
		CaptainDefine* captainDefine = getCaptainDefine(m_vCaptainList[tag]->captainid);
		if (captainDefine)
		{
			std::string captainPath = getCompanionIconPath(captainDefine->protoid, captainDefine->iscaptain);
			button_captain_head->setBright(true);
			image_captain_head->setVisible(true);
			image_captain_head->loadTexture(captainPath);
			label_lv->setString(String::createWithFormat("Lv. %d", captainDefine->level)->_string);
			label_name->setString(getCompanionName(captainDefine->protoid, captainDefine->iscaptain));
			label_lv->setVisible(true);
			label_name->setVisible(true);
			

			for (int j = 0; j < 3; j++)
			{
				std::string str = StringUtils::format("button_equip_bg_%d", j + 1);
				auto b_skill = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_captain, str));
				if (j < captainDefine->n_skills)
				{
					auto t_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(b_skill, "text_item_skill_lv"));
					b_skill->setTag(captainDefine->skills[j]->id + START_INDEX);
					b_skill->setVisible(true);
					b_skill->ignoreContentAdaptWithSize(false);
					if (captainDefine->iscaptain)
					{
						b_skill->getParent()->setTag(SKILL_TYPE_CAPTAIN);
						b_skill->loadTextureNormal(getSkillIconPath(captainDefine->skills[j]->id, SKILL_TYPE_CAPTAIN));
					}
					else
					{
						b_skill->getParent()->setTag(SKILL_TYPE_COMPANION_NORMAL);
						b_skill->loadTextureNormal(getSkillIconPath(captainDefine->skills[j]->id, SKILL_TYPE_COMPANION_NORMAL));
					}
					b_skill->setTouchEnabled(true);
					b_skill->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func,this));
					setTextSizeAndOutline(t_skill_lv, captainDefine->skills[j]->level);
					t_skill_lv->setTag(captainDefine->protoid);
				}
				else
				{
					b_skill->setVisible(false);
				}
			}

			for (int j = 0; j < 3; j++)
			{
				std::string str = StringUtils::format("button_sp_equip_bg_%d", j + 1);
				auto b_skill = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_captain, str));
				if (j < captainDefine->n_specskills)
				{
					b_skill->setTag(captainDefine->specskills[j]->id + START_INDEX);
					b_skill->setVisible(true);
					b_skill->ignoreContentAdaptWithSize(false);
					b_skill->loadTextureNormal(getSkillIconPath(captainDefine->specskills[j]->id, SKILL_TYPE_PARTNER_SPECIAL));
					b_skill->setTouchEnabled(true);
					b_skill->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func,this));
					auto image_lock = b_skill->getChildByName<ImageView*>("image_lock");
					image_lock->setTag(captainDefine->protoid);
					//解锁
					if (captainDefine->specskills[j]->level)
					{
						image_lock->setVisible(false);
						b_skill->setBright(true);
					}
					else
					{
						image_lock->setVisible(true);
						b_skill->setBright(false);
					}
				}
				else
				{
					b_skill->setVisible(false);
				}
			}
			auto p_skill = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_captain, "panel_skill"));
			//没有特殊技能时
			if (captainDefine->n_specskills < 1)
			{
				p_skill->setPositionY(panel_captain->getBoundingBox().size.height / 2 - p_skill->getBoundingBox().size.height / 2);
			}

		}
	}

	ShipDefine* fleetShip;
	if (tag < m_pResult_fleet->n_fleetships)
	{
		fleetShip = m_pResult_fleet->fleetships[tag];
	}
	else
	{
		fleetShip = m_pResult_fleet->catchships[tag - m_pResult_fleet->n_fleetships];
	}
			
		if (fleetShip->user_define_name == nullptr || std::strcmp(fleetShip->user_define_name, "") == 0)
		{
			std::string captainName = getShipName(fleetShip->sid);
			label_ship_name->setString(captainName);
		}
		else
		{
			label_ship_name->setString(String::createWithFormat("%s", fleetShip->user_define_name)->_string);
		}

		std::string filePath = getShipIconPath(fleetShip->sid);

		image_ship->loadTexture(filePath);

		int32_t hp_max = fleetShip->current_hp_max;
		int32_t hp = fleetShip->hp;
		std::string str_ship_durable_num_1 = StringUtils::format("%d / %d", hp, hp_max);
		label_ship_durable_num_1->setString(str_ship_durable_num_1);
		if (0 == hp_max){
			progressbar_durable->setPercent(0);
		}
		else{
			progressbar_durable->setPercent(hp * 100 / hp_max);
		}
		int32_t weight_max = fleetShip->cargo_size / 100;
		int32_t weight = fleetShip->current_cargo_size / 100;
		std::string str_ship_weight_num_1 = StringUtils::format("%d / %d", weight, weight_max);
		label_ship_weight_num_1->setString(str_ship_weight_num_1);
		if (0 == weight_max){
			progressbar_weight->setPercent(0);
		}
		else{
			progressbar_weight->setPercent(weight * 100 / weight_max);
		}

		int32_t crewNum = fleetShip->crew_num;
		int32_t maxCrewNum = fleetShip->max_crew_num;
		std::string str_ship_sailor_num_1 = StringUtils::format("%d / %d", crewNum, maxCrewNum);
		label_ship_sailor_num_1->setString(str_ship_sailor_num_1);
		if (0 == maxCrewNum){
			progressbar_sailor->setPercent(0);
		}
		else{
			progressbar_sailor->setPercent(crewNum * 100 / maxCrewNum);
		}

		int32_t supply = fleetShip->supply;
		int32_t max_supply = fleetShip->max_supply;
		std::string str_ship_supply_num_1 = StringUtils::format("%d / %d", supply, max_supply);
		label_ship_supply_num_1->setString(str_ship_supply_num_1);
		if (0 == max_supply){
			progressbar_supply->setPercent(0);
		}
		else{
			progressbar_supply->setPercent(supply * 100 / max_supply);
		}


		std::string str_atk_num = StringUtils::format("%d", fleetShip->attack);
		label_atk_num->setString(str_atk_num);
		std::string str_defnum = StringUtils::format("%d", fleetShip->defence);
		label_defnum->setString(str_defnum);
		std::string str_speed_num = StringUtils::format("%d", fleetShip->speed);
		label_speed_num->setString(str_speed_num);
		std::string str_steering_num = StringUtils::format("%d", fleetShip->steer_speed);
		label_steering_num->setString(str_steering_num);

		setBgButtonFormIdAndType(image_ship_bg, fleetShip->sid, ITEM_TYPE_SHIP);
		setBgImageColorFormIdAndType(image_item_bg_lv, fleetShip->sid, ITEM_TYPE_SHIP);
		setTextColorFormIdAndType(label_ship_name, fleetShip->sid, ITEM_TYPE_SHIP,true);
}

void UISailHUD::flushFleetinfoNpc(Widget* view, EngageInFightResult* result){

	Widget* panel_title_content = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_title_content"));
	Button* button_friend = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_title_content, "button_friend"));
	ImageView* image_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_content, "image_flag"));
	Text* label_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_content, "label_lv"));
	Text* label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_content, "label_name"));
	Button* button_battle = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_battle"));
	Button*button_cancel = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_challenge_close"));
 	
	if (result->enemyinfo->isfriend){

		button_friend->setBright(true);
	}
	else{
	
		button_friend->setBright(false);
	}
	std::string mylv = StringUtils::format("Lv. %d", result->enemyinfo->level);
	label_lv->setString(mylv);
	std::string e_strName = SINGLE_SHOP->getBattleNpcInfo()[result->npcid].name;
	label_name->setAnchorPoint(Vec2(0.5,0.5));
	label_name->setContentSize(Size(333,32));
	label_name->setPosition(Vec2(376,40));
	label_name->setFontSize(22);
	label_name->setString(e_strName);

	image_flag->ignoreContentAdaptWithSize(false);
	image_flag->loadTexture(getCountryIconPath(result->npcforceid));

	for (int i = 1; i <= 5; i++){

		std::string str_ship_num = StringUtils::format("panel_ship_%d", i);
		Widget* panel_ship = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, str_ship_num));
		panel_ship->setVisible(false);

	}
	
	for (int i = 1; i <= result->n_enemyships; i++){
	
		std::string str_ship_num = StringUtils::format("panel_ship_%d", i);
		Widget* panel_ship = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, str_ship_num));
		
		auto image_ship_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_ship, "image_ship_bg"));
		auto image_item_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship, "image_item_bg_lv"));
		ImageView* image_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship, "image_ship"));
		ImageView* image_num = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_ship, "image_num"));

		Text* label_ship_durable_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_durable_num_1"));
		Text* label_ship_durable_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_durable_num_2"));
		Text* label_ship_durable_num_3 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_durable_num_3"));
		LoadingBar* progressbar_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_ship, "progressbar_durable"));

		Text* label_ship_sailor_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_sailor_num_1"));
		Text* label_ship_sailor_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_sailor_num_2"));
		Text* label_ship_sailor_num_3 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ship, "label_ship_sailor_num_3"));
		LoadingBar* progressbar_sailor = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_ship, "progressbar_sailor"));

		std::string num_name = getPositionIconPath(i);
		image_num->loadTexture(num_name);
		FightShipInfo* enemyship = result->enemyships[i - 1];
		int32_t sid = enemyship->sid;
		std::string filePath = getShipIconPath(sid);
		image_ship->loadTexture(filePath);

		std::string str_hp = StringUtils::format("%d", enemyship->max_hp);
		label_ship_durable_num_1->setString(str_hp);
		label_ship_durable_num_2->setString(" / ");
		label_ship_durable_num_3->setString(str_hp);
		progressbar_durable->setPercent(100);

		std::string str_sailor = StringUtils::format("%d", enemyship->sailornum);
		label_ship_sailor_num_1->setString(str_sailor);
		label_ship_sailor_num_2->setString(" / ");
		label_ship_sailor_num_3->setString(str_sailor);
		progressbar_sailor->setPercent(100);
		setBgButtonFormIdAndType(image_ship_bg, sid, ITEM_TYPE_SHIP);
		setBgImageColorFormIdAndType(image_item_bg_lv, sid, ITEM_TYPE_SHIP);
		panel_ship->setVisible(true);
	}

}

void UISailHUD::flushBattleVS(Widget* view, EngageInFightResult* result){

	if (m_nDisableCancelFight != 0)
	{
		m_nDisableCancelFight = 2;
		for (size_t i = 0; i < result->n_props; i++)
		{
			if (result->props[i]->propid == PROP_FLEE_SMOKE && result->props[i]->num > 0)
			{
				m_nDisableCancelFight = 1;
			}
		}

		if (m_nDisableCancelFight == 2)
		{
			m_nCancelfightcost = result->cancelfightcost;
			if (result->vtickets < result->cancelfightcost)
			{
				m_nDisableCancelFight = 3;
			}
		}
	}

	Button*button_cancel = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_challenge_close"));
	switch (m_nDisableCancelFight)
	{
	case 0:
		//退出时用默认文案
		break;
	case 1:
		button_cancel->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_NPC_BATTLE_FLEE"]);
		break;
	case 2:
		button_cancel->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_NPC_BATTLE_RANSOM"]);
		break;
	default:
		break;
	}
	
	bool bContinueLastFight;
	auto npcId = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(NPC_ID).c_str(), -1);
	//正常战斗(新手引导除外)
	if (npcId == result->npcid && (result->fighttype == FIGHT_TYPE_NORMAL || result->fighttype == FIGHT_TYPE_ROB) && !ProtocolThread::GetInstance()->isInTutorial())
	{
		bContinueLastFight = true;
	}
	else
	{
		bContinueLastFight = false;
	}
	int myShipNum = result->myinfo->shipnum;
	int mySailNum = result->myinfo->sailnum;
	int enemyShipNum = result->enemyinfo->shipnum;
	int enemySailNum = result->enemyinfo->sailnum;
	int myMorale = result->morale;
	int enemyMorale = result->npcmorale;
	//读取保存的数据
	if (bContinueLastFight)
	{
		myShipNum = 0;
		mySailNum = 0;
		enemyShipNum = 0;
		enemySailNum = 0;
		myMorale = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(MY_SHIP_MORALE).c_str(), 0);;
		enemyMorale = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(ENEMY_SHIP_MORALE).c_str(), 0);
		for (size_t i = 0; i < result->myinfo->shipnum; i++)
		{
			auto st_my_sailor = StringUtils::format(MY_SHIP_SAILOR, i + 1);
			auto st_my_blood = StringUtils::format(MY_SHIP_BLOOD, i + 1);
			auto my_sailor = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_my_sailor.c_str()).c_str(), 0);
			auto my_blood = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_my_blood.c_str()).c_str(), 0);
			if (my_sailor > 0 && my_blood > 0)
			{
				mySailNum += my_sailor;
				myShipNum++;
			}
		}

		for (size_t i = 0; i < result->enemyinfo->shipnum; i++)
		{
			auto st_enemy_sailor = StringUtils::format(ENEMY_SHIP_SAILOR, i + 1);
			auto st_enemy_blood = StringUtils::format(ENEMY_SHIP_BLOOD, i + 1);
			auto enemy_sailor = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_enemy_sailor.c_str()).c_str(), 0);
			auto enemy_blood = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName(st_enemy_blood.c_str()).c_str(), 0);
			if (enemy_sailor > 0 && enemy_blood > 0)
			{
				enemySailNum += enemy_sailor;
				enemyShipNum++;
			}
		}
	}
	

	Widget* panel_title_left = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_title_left"));
	ImageView* image_flag_left = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_left, "image_flag_left"));
	Text* label_name_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_left, "label_name_left"));
	Text* label_lv_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_left, "label_lv_left"));
	ImageView* image_moral_left = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_left, "image_moral_left"));

	std::string m_country = StringUtils::format("res/country_icon/flag_%d.png", SINGLE_HERO->m_iNation);
	image_flag_left->loadTexture(m_country);
	label_name_left->setString(SINGLE_HERO->m_sName);
	std::string m_level = StringUtils::format("Lv. %d", result->userlv);
	label_lv_left->setString(m_level);
	flushMoral(image_moral_left, myMorale);

	Widget* panel_title_right = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_title_right"));
	ImageView* image_flag_right = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_right, "image_flag_right"));
	Text* label_name_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_right, "label_name_right"));
	Text* label_lv_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_right, "label_lv_right"));
	ImageView* image_moral_right = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_right, "image_moral_right"));

	std::string e_country = StringUtils::format("res/country_icon/flag_%d.png", result->npcforceid);
	image_flag_right->loadTexture(e_country);
	std::string e_strName = SINGLE_SHOP->getBattleNpcInfo()[result->npcid].name;
	label_name_right->setString(e_strName);
	std::string e_level = StringUtils::format("Lv. %d", result->enemyinfo->level);
	label_lv_right->setString(e_level);
	flushMoral(image_moral_right, enemyMorale);

	Widget* panel_combat_left = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_combat_left"));
	Text* label_combat_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_combat_left"));
	Text* label_ship_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_ship_left"));
	Text* label_sailor_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_sailor_left"));
	std::string m_combat = StringUtils::format("%d", result->myinfo->power);
	label_combat_left->setString(m_combat);
	std::string m_shipNum = StringUtils::format("%d", myShipNum);
	label_ship_left->setString(m_shipNum);
	std::string m_sailNum = StringUtils::format("%d", mySailNum);
	label_sailor_left->setString(m_sailNum);

	Widget* panel_combat_right = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_combat_right"));
	Text* label_win_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_win_num"));
	Text* label_combat_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_combat_right"));
	Text* label_ship_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_ship_right"));
	Text* label_sailor_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_sailor_right"));
	std::string e_combat = StringUtils::format("%d", result->enemyinfo->power);
	label_combat_right->setString(e_combat);
	std::string e_shipNum = StringUtils::format("%d", enemyShipNum);
	label_ship_right->setString(e_shipNum);
	std::string e_sailNum = StringUtils::format("%d", enemySailNum);
	label_sailor_right->setString(e_sailNum);
	std::string str_win_num = StringUtils::format("%d%%", result->victorypercent);
	label_win_num->setString(str_win_num);
	//快速战斗
	auto b_automatic = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_automatic"));
	if (result->victorypercent == 100  && !bContinueLastFight)
	{
		b_automatic->setBright(true);
		b_automatic->setTouchEnabled(true);
	}
	else
	{
		b_automatic->setBright(false);
		if (bContinueLastFight)
		{
			b_automatic->setTouchEnabled(true);
		}
		else
		{
			b_automatic->setTouchEnabled(false);
		}
		auto i_speedup = b_automatic->getChildByName<ImageView*>("image_speedup");
		setGLProgramState(i_speedup, true);
	}

    if (result->victorypercent < 30)
    {
        label_win_num->setTextColor(cocos2d::Color4B::RED);
    }
    else if (result->victorypercent < 60)
    {
        label_win_num->setTextColor(cocos2d::Color4B::ORANGE);
    }
    else
    {
		label_win_num->setTextColor(Color4B(40, 25, 13, 255));
    }

	auto b_info = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_batt_info"));
	auto t_ranson = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "labe_ranson"));
	std::string st_ranson;
	//不支持赎金
	if (result->supportransom == 0)
	{
		st_ranson = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_BATTLE_NOT_PAY_RANSON"];
		b_info->setTag(NOT_PAY_RANSON);
	}
	else
	{
		//不需要赎金
		if (result->userlv < 10)
		{
			st_ranson = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_BATTLE_NOT_LOSE"];
			b_info->setTag(NOT_LOSE);
		}
		else
		{
			st_ranson = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_BATTLE_PAY_RANSON"];
			b_info->setTag(PAY_RANSON);
		}
	}
	t_ranson->setString(st_ranson);
	t_ranson->setPositionX(view->getBoundingBox().size.width / 2 - b_info->getBoundingBox().size.width);
	b_info->setPositionX(t_ranson->getPositionX() + t_ranson->getBoundingBox().size.width / 2 + b_info->getBoundingBox().size.width);

//   InformView::getInstance()->openInformView(this);
//   InformView::getInstance()->openViewAutoClose("TIP_BATTLE_WARNING");
}

void UISailHUD::flushMoral(Widget* view, int num){

	for (int i = 1; i <= 5; i++)
	{

		std::string str_moral = StringUtils::format("image_moral_%d", i);
		ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(view, str_moral));
		moral_image->loadTexture(getMoralIconPath(3));

	}

	int num_morale = num;
	int n1000 = (num_morale / 1000) % 6;
	int n100 = num_morale - n1000 * 1000;

	for (int i = 1; i <= n1000; i++)
	{
		std::string str_moral = StringUtils::format("image_moral_%d", i);
		ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(view, str_moral));
		moral_image->loadTexture(getMoralIconPath(1));

	}
	if (n1000 < 5 && n100 >= 50)
	{
		std::string str_moral = StringUtils::format("image_moral_%d", n1000 + 1);
		ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(view, str_moral));
		moral_image->loadTexture(getMoralIconPath(2));
	}
}


void UISailHUD::closeMap(Ref*)
{
	m_bIsBlocking = false;
	shipPause(false);
	dynamic_cast<UIShip*>(m_pShip)->stopPauseTime();
}
void UISailHUD::findNewCity(const int cityId,Widget * psender,Point pos)
{
	m_nFindcityId=cityId;

	openView(MAPUI_COCOS_RES[INDEX_UI_FOUND_NEWCITY_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FOUND_NEWCITY_CSB]);
	view->setOpacity(0);
	view->setCameraMask(4,true);
	
	auto image_light=dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_light"));
	image_light->runAction(RepeatForever::create( RotateBy::create(2.0f, 360) ));
	auto button_cueta=dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_cueta"));
	button_cueta->setTouchEnabled(false);
	auto button_ok=dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_findnew_city"));
	button_ok->setTitleText("ok");
	auto label_cityname=dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_cityname"));
	label_cityname->setString(SINGLE_SHOP->getCitiesInfo()[cityId].name);
	auto image_nameBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_cityname_bg"));

	auto label_content = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_content"));
	std::string path;
	auto picNum = 0;
	if (SINGLE_SHOP->getCitiesInfo()[cityId].port_type == 5)
	{
		picNum = SINGLE_SHOP->getCityAreaResourceInfo()[cityId].icon_village;
	}
	else
	{
		picNum = SINGLE_SHOP->getCityAreaResourceInfo()[cityId].icon_id;
	}
	path = StringUtils::format("cities_resources/icon/city_%d.png", picNum);
	button_cueta->loadTextures(path,path,path);
	button_cueta->setPosition(psender->getPosition() - pos);
	image_light->setOpacity(0);
	image_nameBg->setOpacity(0);
	label_content->setOpacity(0);
	view->runAction(FadeIn::create(1));
	button_cueta->runAction(Sequence::createWithTwoActions(MoveTo::create(1, _director->getWinSize() / 2),
		CallFunc::create([=]{
		image_light->runAction(Spawn::createWithTwoActions(FadeIn::create(1), CallFunc::create([=]{    
			image_nameBg->runAction(FadeIn::create(1));
			label_content->runAction(Sequence::createWithTwoActions(DelayTime::create(1), FadeIn::create(1)));
			view->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func,this));
		})));
	})
		));
	
}
void UISailHUD::findCityResult(const int addLevelexp,const int addFame)
{
	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	view->setCameraMask(4,true);

	auto label_title=dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_title"));
	label_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_FINDNEWCITY_REWARD"]);
	auto panel_result=view->getChildByName<Widget*>("panel_result");
	auto listviewResult = panel_result->getChildByName<ListView*>("listview_result");
	auto panel_exp = panel_result->getChildByName<Widget*>("panel_silver");
	auto panel_coin = panel_result->getChildByName<Widget*>("panel_coin");
	Widget* image_div_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "image_div_1"));
	listviewResult->refreshView();
	//fame
	Widget*panel_fame_clone = (Widget*)panel_coin->clone();
	auto image_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_fame_clone,"image_silver"));
	image_silver->loadTexture("cocosstudio/login_ui/common/reputation_2.png");
	auto label_buy_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_fame_clone, "label_buy_num"));
	label_buy_num->setString(StringUtils::format("+%d", addFame));
	//exp
	Widget* panel_exp_clone =(Widget*)panel_exp->clone();
	auto label_expAdd= panel_exp_clone->getChildByName<Text*>("label_buy_num");
	auto label_force_relation=panel_exp_clone->getChildByName<Text*>("label_force_relation");
	label_force_relation->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_FINDNEWCITY_REWARD_EXP"]);
	label_expAdd->setTextColor(Color4B(46,125,50,255));
	label_expAdd->setString(StringUtils::format("+%d", addLevelexp));
	auto image_div_line = (Widget*)image_div_1->clone();
	auto image_div_line_2 = (Widget*)image_div_1->clone();


	listviewResult->removeAllChildrenWithCleanup(true);
	listviewResult->pushBackCustomItem(panel_exp_clone);	
	listviewResult->pushBackCustomItem(image_div_line);
	listviewResult->pushBackCustomItem(panel_fame_clone);
	listviewResult->pushBackCustomItem(image_div_line_2);
	listviewResult->setVisible(true);
	listviewResult->refreshView();
	listviewResult->setCameraMask(4,true);
	listviewResult->setSubNodeMaxCameraMask(4);
}
void UISailHUD::lossProps(const int itemId)
{
	Layout* panel_item_1 = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_pFloatResultview, "panel_item_1"));
	Widget* panel_item_1_clone = panel_item_1->clone();
	panel_item_1_clone->setCameraMask(4,true);
	Text* label_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_item_1_clone, "label_num"));
	auto image_material_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_item_1_clone, "image_material_bg"));
	ImageView* image_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_item_1_clone, "image_item"));
	Text* label_items_name = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_item_1_clone, "label_items_name"));
	
	std::string  icon_path = getItemIconPath(itemId);
	std::string  icon_name = SINGLE_SHOP->getItemData()[itemId].name;
	CC_SAFE_RETAIN(panel_item_1_clone);
	if (panel_item_1_clone->getParent()){
		panel_item_1_clone->removeFromParent();
	}
	image_item->loadTexture(icon_path);
	label_num->setColor(Color3B::RED);
	label_num->setString("-1");
	label_items_name->setString(icon_name);
	m_pListview_result->pushBackCustomItem(panel_item_1_clone);
	CC_SAFE_RELEASE(panel_item_1_clone);
}
void UISailHUD::getCurrSailorsNum(int sailorNums)
{
	m_nCurCrewNum = sailorNums;
}
void UISailHUD::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
#if 1
	UIBasicLayer::visit(renderer,parentTransform, parentFlags);
#else
	if(isVisitableByVisitingCamera()){
		Mat4 transform = parentTransform;
		std::vector<Node*> myParents;
		Node* start = this->getParent();
		while(start){
			myParents.push_back(start);
			start = start->getParent();
		}

		vector<Node*>::iterator iter;
		for( iter = myParents.end()-1; iter != myParents.begin()-1; iter-- )
		{
			Node*node = *iter;
			parentFlags = node->processParentFlags(transform,parentFlags);
			//node->sortAllChildren();
			transform = node->getModelViewTransform();
			_director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
			_director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
		}
		UIBasicLayer::visit(renderer,transform,parentFlags);
		for( iter = myParents.end()-1; iter != myParents.begin()-1; iter-- )
		{
			Node*node = *iter;
			_director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		}
	}else{

	}
#endif
}

void UISailHUD::WarningAction(std::string content)
{
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
	if (view)
	{
		auto tmp = view->getChildByName<ImageView*>("image_cityname_bg");
		auto nameBg = view->clone();
		auto m_pLandName = nameBg->getChildByName<Text*>("label_map_name");
		m_pLandName->setString(content);
		nameBg->stopAllActions();
		nameBg->setPositionX(0 - nameBg->getContentSize().width / 2);
		nameBg->setOpacity(0);

		nameBg->setPositionX(nameBg->getPositionX() - nameBg->getContentSize().width/2);
		auto action_1 = MoveBy::create(1, Vec2(nameBg->getContentSize().width, 0));
		auto action_2 = Sequence::create(DelayTime::create(0.5),FadeIn::create(1), DelayTime::create(3),
		Spawn::createWithTwoActions(MoveBy::create(1.5, Vec2(-nameBg->getContentSize().width, 0)), FadeOut::create(1))
		,nullptr
			);
		auto spw_action = Spawn::createWithTwoActions(action_1, action_2);
		nameBg->runAction(spw_action);
	}
}

void UISailHUD::mapNameAction()
{
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
	if (view)
	{
		auto nameBg = view->getChildByName<ImageView*>("image_cityname_bg");
		nameBg->stopAllActions();
		nameBg->setPositionX(0 - nameBg->getContentSize().width / 2);
		nameBg->setOpacity(0);

		nameBg->setPositionX(nameBg->getPositionX() - nameBg->getContentSize().width/2);
		auto action_1 = MoveBy::create(1, Vec2(nameBg->getContentSize().width, 0));
		auto action_2 = Sequence::create(DelayTime::create(0.5),FadeIn::create(1), DelayTime::create(3),
		Spawn::createWithTwoActions(MoveBy::create(1.5, Vec2(-nameBg->getContentSize().width, 0)), FadeOut::create(1))
		,nullptr
			);
		auto spw_action = Spawn::createWithTwoActions(action_1, action_2);
		nameBg->runAction(spw_action);
	}
}
void UISailHUD::setShipEquipBrokenLocal(size_t nBrokenEquips, OutOfDurableItemDefine**brokenEquipsItems)
{
	if (nBrokenEquips&&brokenEquipsItems)
	{
		for (int i = 0; i<nBrokenEquips; i++)
		{
			if (brokenEquipsItems[i]->durability>0)
			{
				std::string st_ship_position = StringUtils::format(SHIP_POSTION_EQUIP_BROKEN, brokenEquipsItems[i]->position + 1);
				UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), true);
			}
		}
		UserDefault::getInstance()->flush();
	}
}
std::string UISailHUD::changeShipName(std::string shipName)
{
	auto nameLen = shipName.length();
	LanguageType nType = LanguageType(Utils::getLanguage());
	switch (nType)
	{
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
	case cocos2d::LanguageType::CHINESE:
	{
//utf8大多数一个汉字3字节
		if (nameLen > 3 * 3)
		{
			shipName = shipName.substr(0, 3 *3) + "...";
		}
		break;
	}
	default:
	{
	 if (nameLen >8)
	 {
		 shipName = shipName.substr(0, 8) + "...";
	 }
	 break;
	}
	}
	return shipName;
}
void UISailHUD::openSaillingDialog(int tag, int64_t lostcoin)
{
	openView(MAPUI_COCOS_RES[INDEX_UI_FIGHT_DEFEATED_DIALOG_CSB]);
	auto viewfieldDiaog = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FIGHT_DEFEATED_DIALOG_CSB]);
	viewfieldDiaog->addTouchEventListener(CC_CALLBACK_2(UISailHUD::menuCall_func, this));
	viewfieldDiaog->setCameraMask(4, true);
	auto panel_defeat = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewfieldDiaog, "panel_defeat"));
	auto panel_sailor = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewfieldDiaog, "panel_sailor"));
	panel_defeat->setVisible(false);
	panel_sailor->setVisible(false);
	Text* i_text_content = nullptr;
	//水手全部死亡
	if (tag == 1)
	{
		panel_sailor->setVisible(true);
		i_text_content = panel_sailor->getChildByName<Text*>("text_content");
		//对话内容
		chatContent = SINGLE_SHOP->getTipsInfo()["TIP_SAIILING_CREW_DIE_ALL_DIALOG"];
	}
	else if (tag == 2)
	{
		panel_defeat->setVisible(true);
		i_text_content = panel_defeat->getChildByName<Text*>("text_content");
		//新手保护机制
		if (m_pEndFightResult->isprotect == 1)
		{
			//新手保护对话内容
			chatContent = SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_FAILED_DIALOG_FOR_NEWPLAYER"];
		}
		else
		{
			//对话内容
			chatContent = SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_FAILED_DIALOG"];
		}		
		std::string lossCoin = "[10000]";
		std::string newLossCoin = StringUtils::format("%lld", lostcoin);
		repalce_all_ditinct(chatContent, lossCoin, newLossCoin);
	}
	else if (tag == 3)
	{
		panel_defeat->setVisible(true);
		i_text_content = panel_defeat->getChildByName<Text*>("text_content");
		chatContent = SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_USE_VTICKES_FAILED_DIALOG"];
		std::string new_vaule = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iHave_licence_cityId].name;
		std::string old_vaule = "[last_city]";
		repalce_all_ditinct(chatContent, old_vaule, new_vaule);
	}
	else
	{
		panel_defeat->setVisible(true);
		i_text_content = panel_defeat->getChildByName<Text*>("text_content");
		chatContent = SINGLE_SHOP->getTipsInfo()["TIP_BATTLE_FAILED_DIALOG"];//对话内容
		std::string new_vaule = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iHave_licence_cityId].name;
		std::string old_vaule = "[lastCity]";
		repalce_all_ditinct(chatContent, old_vaule, new_vaule);
		std::string lossCoin = "[10000]";
		std::string newLossCoin = StringUtils::format("%lld", lostcoin);
		repalce_all_ditinct(chatContent, lossCoin, newLossCoin);
	}
	
	anchPic = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewfieldDiaog, "image_anchor"));
	anchPic->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5, Vec2(0, 10))), EaseBackOut::create(MoveBy::create(0.5, Vec2(0, -10))))));
	anchPic->setVisible(false);
	std::string new_vaule = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iHave_licence_cityId].name;
	std::string old_vaule = "[lastCity]";
	repalce_all_ditinct(chatContent, old_vaule, new_vaule);
	shipPause(false);
	i_text_content->setString("");
	chatTxt = i_text_content;
	this->schedule(schedule_selector(UISailHUD::showSaillingFailedText), 0.1 / 3);
}
void UISailHUD::showSaillingFailedText(float t)
{
	std::string	 text = chatTxt->getString();
	lenAfter = lenNum + text.length();
	std::string showT = "";
	plusNum = chatGetUtf8CharLen(chatContent.c_str()+lenAfter);
	lenAfter = lenAfter + plusNum;
	int maxLen = chatContent.length() + 1;
	showT = chatContent.substr(lenNum, lenAfter - lenNum);
	chatTxt->setString(showT);
	if (lenAfter >= maxLen-1)
	{
		m_bSaillingDiaolgOver = true;
		anchPic->setVisible(true);
		chatTxt = nullptr;
		this->unschedule(schedule_selector(UISailHUD::showSaillingFailedText));
	}
	else
	{
		m_bSaillingDiaolgOver = false;
	}
}

void UISailHUD::updateMailNumber(const float fTime)
{
	ProtocolThread::GetInstance()->checkMailBox();
}

void UISailHUD::setAllUnreadImage(const CheckMailBoxResult *pResult)
{
	if (!pResult)
	{
		return;
	}
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
	auto imageUnreadEmail = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_unread_4"));
	auto imageUnreadTask = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_unread_2"));
	if (pResult->newmailcount == 0)
	{
		imageUnreadEmail->setVisible(false);
	}
	else
	{
		imageUnreadEmail->setVisible(true);
	}

	if (pResult->taskcompleted != 0 || UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(MAIN_TASK_FINISH).c_str()))
	{
		imageUnreadTask->setVisible(true);
	}
	else
	{
		imageUnreadTask->setVisible(false);
	}
}

void UISailHUD::openCheifChat()
{
	m_bSalvageClick = false;
	auto guideWidget = Layer::create();
	guideWidget->setName("guideWidget");
	this->addChild(guideWidget,100);
	
	Size size(Director::getInstance()->getVisibleSize().width, Director::getInstance()->getVisibleSize().height);
	guideWidget->setContentSize(size);
	guideWidget->setAnchorPoint(Vec2(0, 0));

	auto callback = [](Touch *, Event *)
	{
		return true;
	};
	auto callback2 = [=](Touch* touch, Event* event)
	{
		UISailHUD::SalvageClickEvent();
	};

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = callback;
	listener->onTouchEnded = callback2;
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, guideWidget);

	auto AidePic = Sprite::create();
	AidePic->setTexture(getCheifIconPath());
	AidePic->setAnchorPoint(Vec2(0, 0));
	AidePic->setScale((576.0 / 720 * size.height) / AidePic->getContentSize().height, (576.0 / 720 * size.height) / AidePic->getContentSize().height);
	guideWidget->addChild(AidePic);

	auto dboxLeft = Sprite::create();
	dboxLeft->setTexture("res_lua/dboxLeft.png");
	dboxLeft->setAnchorPoint(Vec2(0, 0));
	dboxLeft->setScale((742.0 / 1280 * size.width) / dboxLeft->getContentSize().width, (742.0 / 1280 * size.width) / dboxLeft->getContentSize().width);
	dboxLeft->setPosition(95.0 / 1280 * size.width, 30.0);
	guideWidget->addChild(dboxLeft);

	auto anchr = Sprite::create();
	anchr->setTexture("res_lua/anchr.png");
	anchr->setPosition(dboxLeft->getPositionX() + dboxLeft->getBoundingBox().size.width - 83.0, dboxLeft->getPositionY() + 36.0);
	guideWidget->addChild(anchr);
	anchr->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5, Vec2(0, 10))), EaseBackOut::create(MoveBy::create(0.5, Vec2(0, -10))))));
	
	std::string guard_name;
	if(SINGLE_HERO->m_iGender == 1)
	{
		guard_name = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"];
	}
	else
	{
		guard_name = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"];
	}

	auto titleTxt = Label::createWithSystemFont(guard_name, CUSTOM_FONT_NAME_3, 30);
	titleTxt->setColor(Color3B(231, 192, 125));
	titleTxt->setAnchorPoint(Vec2(0, 0));
	titleTxt->enableShadow();
	titleTxt->setWidth(540);
	titleTxt->setPosition(dboxLeft->getPositionX() + 88.0, dboxLeft->getPositionY() + dboxLeft->getBoundingBox().size.height - 76.0);
	guideWidget->addChild(titleTxt);
	
	std::string content;
	if (m_nConfirmIndex == CONFIRM_INDEX_SALVAGING)
	{
		content = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_SALVAGE_COST"];
		std::string new_vaule = StringUtils::format("%d", m_nSalvageCost);
		std::string old_vaule = "[cost]";
		repalce_all_ditinct(content, old_vaule, new_vaule);
	}
	else
	{
		content = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_STOP_SALVAGING"];
	}

	auto txtTxt = Label::createWithSystemFont(content, CUSTOM_FONT_NAME_2, 24);
	txtTxt->setColor(Color3B(238, 230, 202));
	txtTxt->setPosition(dboxLeft->getPositionX() + 88.0, dboxLeft->getPositionY() + dboxLeft->getBoundingBox().size.height - 86.0);
	txtTxt->enableShadow();
	txtTxt->setWidth(540);
	txtTxt->setAnchorPoint(Vec2(0, 1));
	guideWidget->addChild(txtTxt);
	guideWidget->setCameraMask(4, true);
	SalvageClickEvent();
}

void UISailHUD::SalvageClickEvent()
{
	auto guideWidget = dynamic_cast<Layer*>(this->getChildByName("guideWidget"));
	if (!guideWidget || (guideWidget && guideWidget->getChildByName("zhixianBgPic")))
	{
		return;
	}
	std::string dialog1 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_THREE_SECTION_ONE_OPTION_ONE"];
	std::string dialog2 = SINGLE_SHOP->getTipsInfo()["TIP_CHAPTER_THREE_SECTION_ONE_OPTION_TWO"];
	auto zhixianBgPic = Sprite::create();
	zhixianBgPic->setTexture("login_ui/start_720/option_bg.png");
	zhixianBgPic->setPosition(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
	zhixianBgPic->setName("zhixianBgPic");
	guideWidget->addChild(zhixianBgPic);

	auto branchButton1 = Button::create();
	branchButton1->setName("branchButton1");
	branchButton1->setTitleFontSize(24);
	branchButton1->setTitleColor(Color3B(215, 190, 51));
	branchButton1->setTitleText(dialog1);
	branchButton1->ignoreContentAdaptWithSize(false);
	branchButton1->setContentSize(Size(534, 60));
	branchButton1->ignoreAnchorPointForPosition(false);
	branchButton1->setPosition(Vec2(265, 152));
	branchButton1->addTouchEventListener(CC_CALLBACK_2(UISailHUD::branchClickEvent,this));
	zhixianBgPic->addChild(branchButton1);

	auto branchButton2 = Button::create();
	branchButton2->setName("branchButton2");
	branchButton2->setTitleFontSize(24);
	branchButton2->setTitleColor(Color3B(215, 190, 51));
	branchButton2->setTitleText(dialog2);
	branchButton2->ignoreContentAdaptWithSize(false);
	branchButton2->setContentSize(Size(534, 60));
	branchButton2->ignoreAnchorPointForPosition(false);
	branchButton2->setPosition(Vec2(265, 92));
	branchButton2->addTouchEventListener(CC_CALLBACK_2(UISailHUD::branchClickEvent,this));
	zhixianBgPic->addChild(branchButton2);
	zhixianBgPic->setCameraMask(4, true);
}

void UISailHUD::branchClickEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto target = (Widget*)pSender;
		auto name = target->getName();
		target->setTouchEnabled(false);
		if (name == "branchButton1")
		{
			if (m_nConfirmIndex == CONFIRM_INDEX_SALVAGING)
			{
				ProtocolThread::GetInstance()->startSalvage(m_pShip->getPositionX(), m_pShip->getPositionY(), UILoadingIndicator::createWithMask(this, 4));
			}
			else
			{
				m_bIsSalvage = false;
				m_bIsBlocking = false;
				Director::getInstance()->resume();
				auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
				auto b_salvage = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_salvage"));
				b_salvage->setBright(true);
				shipPause(false);
				this->unschedule(schedule_selector(UISailHUD::openSalvage));
				dynamic_cast<UIShip*>(m_pShip)->setFalgVisible(true);
				dynamic_cast<UIShip*>(m_pShip)->removeChildByTag(111);
				dynamic_cast<UIShip*>(m_pShip)->removeChildByTag(222);
				dynamic_cast<UIShip*>(m_pShip)->removeChildByTag(333);
				m_pShip->getChildByName("salvage_dh")->removeFromParentAndCleanup(true);
				if (m_bIsNpc)
				{
					m_bIsNpc = false;
					auto button = dynamic_cast<UINPCShip*>(m_pNpc);
					int t = button->getEventID();
					m_nDisableCancelFight = 0;
					m_bIsBlocking = true;
					shipPause(true);
					dynamic_cast<UIShip*>(m_pShip)->startPauseTime();
					ProtocolThread::GetInstance()->engageInFight(button->getEventID(), m_nDisableCancelFight, m_nEventversion, FIGHT_TYPE_NORMAL, 0, UILoadingIndicator::createWithMask(this, 4));
				}
				if (m_bIsCity)
				{
					m_bIsCity = false;
					landCity(m_pCityResult);
				}
				if (m_bIsLoot)
				{
					m_bIsLoot = false;
					SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
					if (dynamic_cast<UIShip*>(m_pShip)->m_IsAutoSailing)
					{
						shipPause(true);
						dynamic_cast<UIShip*>(m_pShip)->stopShip(true);
						m_nConfirmIndex = CONFIRM_INDEX_START_ROB;
					}
					else
					{
						shipPause(true);
						dynamic_cast<UIShip*>(m_pShip)->stopShip(true);
						ProtocolThread::GetInstance()->findLootPlayer(UILoadingIndicator::createWithMask(this, 4));

					}
				}
			}
			
		}
		else if (name == "branchButton2")
		{
			if (m_nConfirmIndex == CONFIRM_INDEX_SALVAGING)
			{
				shipPause(false);
				m_bIsSalvage = false;
				m_bIsBlocking = false;
				m_bIsNpc = false;
				m_bIsCity = false;
				m_bIsLoot = false;
			}
			else
			{
				Director::getInstance()->resume();
				shipPause(false);
				m_bIsNpc = false;
				m_bIsBlocking = false;
				m_bIsCity = false;
				m_bIsLoot = false;
			}
		}
		this->removeChildByName("guideWidget");
	
		auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
		auto b_salvage = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_salvage"));
		b_salvage->setTouchEnabled(true);
		m_bSalvageClick = true;
	}
}

void UISailHUD::openAttactPirate(int npcId)
{
	shipPause(true);
	openView(MAPUI_COCOS_RES[INDEX_UI_LOAD_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_LOAD_CSB]);
	auto panel_friend = view->getChildByName<Layout*>("panel_friend");
	auto image_friend = panel_friend->getChildByName<ImageView*>("image_friend");
	auto boss_name = view->getChildByName<Text*>("label_goods_name");
	auto t_wanted = view->getChildByName<Text*>("label_wanted");
	auto image_flag = view->getChildByName<ImageView*>("image_flag");
	image_flag->loadTexture(getCountryIconPath(10));
	
	boss_name->setString(SINGLE_SHOP->getBattleNpcInfo()[npcId].name);
	image_friend->loadTexture(ICON_ENEMY_NPC);
	image_friend->setPositionX(panel_friend->getBoundingBox().size.width / 2);

	auto image_flag_1 = panel_friend->getChildByName<ImageView*>("image_flag");
	auto image_div_1 = view->getChildByName<ImageView*>("image_div_1");
	image_flag_1->setVisible(false);
	image_div_1->setVisible(false);

	auto button_attact_pirate = view->getChildByName<Button*>("button_land");
	button_attact_pirate->setName("button_attact_pirate");
}

void UISailHUD::openLootView(EngageInFightResult *battledata)
{
	_userData = battledata;
	m_robforbid = false;
	openView(MAPUI_COCOS_RES[INDEX_UI_ROB_UI_CSB]);
	Widget* view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_ROB_UI_CSB]);

	view->setCameraMask(4, true);
	Widget* panel_title_left = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_title_left"));
	ImageView* image_flag_left = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_left, "image_flag_left"));
	Text* label_name_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_left, "label_name_left"));
	Text* label_lv_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_left, "label_lv_left"));
	ImageView* image_moral_left = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_left, "image_moral_left"));

	std::string m_country = StringUtils::format("res/country_icon/flag_%d.png", SINGLE_HERO->m_iNation);
	image_flag_left->loadTexture(m_country);
	label_name_left->setString(SINGLE_HERO->m_sName);
	std::string m_level = StringUtils::format("Lv. %d", battledata->userlv);
	label_lv_left->setString(m_level);
	flushMoral(image_moral_left, battledata->morale);

	Widget* panel_title_right = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_title_right"));
	ImageView* image_flag_right = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_right, "image_flag_right"));
	Text* label_name_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_right, "label_name_right"));
	Text* label_lv_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_right, "label_lv_right"));
	ImageView* image_moral_right = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_right, "image_moral_right"));

	std::string e_country = StringUtils::format("res/country_icon/flag_%d.png", m_lootPlayerInfo->info->nation);
	image_flag_right->loadTexture(e_country);
	std::string e_strName = m_lootPlayerInfo->info->heroname;
	label_name_right->setString(e_strName);
	std::string e_level = StringUtils::format("Lv. %d", battledata->enemyinfo->level);
	label_lv_right->setString(e_level);
	flushMoral(image_moral_right, battledata->npcmorale);

	Widget* panel_combat_left = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_combat_left"));
	Text* label_combat_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_combat_left"));
	Text* label_ship_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_ship_left"));
	Text* label_sailor_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_sailor_left"));
	std::string m_combat = StringUtils::format("%d", battledata->myinfo->power);
	label_combat_left->setString(m_combat);
	std::string m_shipNum = StringUtils::format("%d", battledata->myinfo->shipnum);
	label_ship_left->setString(m_shipNum);
	std::string m_sailNum = StringUtils::format("%d", battledata->myinfo->sailnum);
	label_sailor_left->setString(m_sailNum);

	Widget* panel_combat_right = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_combat_right"));
	Text* label_win_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_win_num"));
	Text* label_combat_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_combat_right"));
	Text* label_ship_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_ship_right"));
	Text* label_sailor_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_sailor_right"));
	Text* label_rob_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_speed_num"));
	auto btn_rob = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_start_rob"));
	std::string e_combat = StringUtils::format("%d", battledata->enemyinfo->power);
	label_combat_right->setString(e_combat);
	std::string e_shipNum = StringUtils::format("%d", battledata->enemyinfo->shipnum);
	label_ship_right->setString(e_shipNum);
	std::string e_sailNum = StringUtils::format("%d", battledata->enemyinfo->sailnum);
	label_sailor_right->setString(e_sailNum);
	std::string str_win_num = StringUtils::format("%d%%", battledata->victorypercent);
	label_win_num->setString(str_win_num);
	std::string str_rob_coin =numSegment(StringUtils::format("%lld", m_lootPlayerInfo->canbelooted));
	label_rob_coin->setString(str_rob_coin);
	btn_rob->setBright(true);
	btn_rob->setTouchEnabled(true);
	if (battledata->victorypercent < 30)
	{
		label_win_num->setTextColor(cocos2d::Color4B::RED);
	}
	else if (battledata->victorypercent < 60)
	{
		label_win_num->setTextColor(cocos2d::Color4B::ORANGE);
	}
	else
	{
		label_win_num->setTextColor(Color4B(40, 25, 13, 255));
	}

	auto b_info = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_rob_info"));
	auto t_ranson = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "labe_ranson"));
	t_ranson->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_BATTLE_NOT_LOSE"]);
	t_ranson->setPositionX(view->getBoundingBox().size.width / 2 - b_info->getBoundingBox().size.width);
	b_info->setPositionX(t_ranson->getPositionX() + t_ranson->getBoundingBox().size.width / 2 + b_info->getBoundingBox().size.width);
}

void UISailHUD::showLootButton(bool show)
{
	m_CanLoot = show;
	if (getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]))
	{
		auto m_pSubPanel = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
		auto btn_loot = Helper::seekWidgetByName(m_pSubPanel, "button_outlook");
		if (m_CanLoot)
		{
			btn_loot->setVisible(true);
		}
		else
		{
			btn_loot->setVisible(false);
		}
	}
	else
	{
		return;
	}

}
void UISailHUD::addLayerForSeaEvent()
{
	//在海上事件动画出现前，吞噬触摸，不可点击其它button
	if (!m_touchEventLayer)
	{
		m_touchEventLayer = Layer::create();
		m_touchEventLayer->setCameraMask(4, true);
		m_touchEventLayer->setTouchEnabled(true);
		this->addChild(m_touchEventLayer);
		auto listener2 = EventListenerTouchOneByOne::create();
		listener2->setSwallowTouches(true);
		listener2->onTouchBegan = [](Touch* touch, Event* event)
		{
			return true;
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, m_touchEventLayer);

	}
}
void UISailHUD::removeLayerForSeaEvent()
{
	if (m_touchEventLayer)
	{
		m_touchEventLayer->removeFromParent();
		m_touchEventLayer = nullptr;
	}
}

void UISailHUD::avoidSeaEventTipAction(std::string name)

{
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
	if (view)
	{
		auto nameBg = view->getChildByName<ImageView*>("image_cityname_bg");
		auto label_map_name = nameBg->getChildByName<Text*>("label_map_name");
		label_map_name->setString(SINGLE_SHOP->getTipsInfo()[name]);
		nameBg->stopAllActions();
		nameBg->setPositionX(0 - nameBg->getContentSize().width / 2);
		nameBg->setOpacity(0);

		auto action_1 = MoveBy::create(1, Vec2(nameBg->getContentSize().width, 0));
		auto action_2 = Sequence::create(DelayTime::create(0.5), FadeIn::create(1), DelayTime::create(3),
			Spawn::createWithTwoActions(MoveBy::create(1.5, Vec2(-nameBg->getContentSize().width, 0)), FadeOut::create(2))
			, nullptr
			);
		auto spw_action = Spawn::createWithTwoActions(action_1, action_2);
		nameBg->runAction(spw_action);
	}
}
void UISailHUD::shipStatueAvoidSeaEvent(const std::string name, const int iconiid, const int type)
{
	//  躲过海上事件后的处理
	removeLayerForSeaEvent();
	openView(MAPUI_COCOS_RES[INDEX_UI_FLOAT_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FLOAT_CSB]);
	view->setLocalZOrder(100000);
	view->setCameraMask(4, true);


	std::string t_chat("");
	auto tipInfo = SINGLE_SHOP->getTipsInfo();
	switch (type)
	{
	case EV_SEA_STORM:
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_STORM"];
		break;
	case EV_SEA_ROCK:
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_ROCK"];
		break;
	case EV_SEA_PESTIS:
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_PESTIS"];
		break;
	case EV_SEA_HOMESICK:
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_HOMESICK"];
		break;
	case EV_SEA_SEPSIS:
		t_chat = tipInfo["TIP_SAILING_SHIPTITLE_EVENT_SEPSIS"];
		break;
	default:
		break;
	}

	auto t_labelCount = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_content"));
	auto b_continue = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_continue"));
	auto i_image = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_goods"));
	auto btn_item = view->getChildByName("button_equip_items");
	auto t_atlas = Helper::seekWidgetByName(view, "text_item_num");
	auto iconPath = getItemIconPath(iconiid);
	auto t_tile = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title"));
	ImageView* image_accident_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_accident_1"));
	std::string s_event_bg = StringUtils::format("res/event_type/event_%d.jpg", type);

	t_atlas->setVisible(false);
	image_accident_1->loadTexture(s_event_bg);
	t_tile->setText(t_chat);
	t_labelCount->setString(SINGLE_SHOP->getTipsInfo()[name]);
	i_image->loadTexture(iconPath);
	btn_item->setTag(iconiid);

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
	Animation* animation_shiplight = Animation::createWithSpriteFrames(frames, 2.0 / 28, 1);
	auto sp = Sprite::create();
	sp->setPosition(Vec2(1081,554));
	sp->setCameraMask(4);
	sp->setScale(2);
	i_image->getParent()->setLocalZOrder(2);
	view->addChild(sp, 1);
	sp->runAction(RepeatForever::create(Animate::create(animation_shiplight)));
}

int UISailHUD::confirmShipBowId(ShipBowIconInfo **bowicons, int bowNum, SEA_EVENT_TYPE type)
{
	int angel[4]{29, 243, 34, 248};
	int dolphin[4]{30, 244, 35, 249};
	int sea_god[4]{32, 246, 37, 251};
	int whale[4]{31, 245, 36, 250};
	int id = 0;

	vector<int> icons;
	for (int i = 0; i < bowNum; i++)
	{
		icons.push_back(bowicons[i]->bowiconiid);
	}
	switch (type)
	{
	case EV_SEA_STORM:
	{
						 for (int i = 0; i < bowNum; i++)
						 {
							 for (int j = 0; j < 4; j++)
							 {
								 if (bowicons[i]->bowiconiid == sea_god[j])
								 {
									 if (id == 246 && bowicons[i]->bowiconiid == 37)
									 {
										 id = bowicons[i]->bowiconiid;
									 }
									 else if (id == 37 && bowicons[i]->bowiconiid == 246)
									 {
										 id = 37;
									 }
									 else if (id < bowicons[i]->bowiconiid)
									 {
										 id = bowicons[i]->bowiconiid;
									 }
								 }
							 }
						 }
	}
		break;
	case EV_SEA_FLOAT:
	{
						 for (int i = 0; i < bowNum; i++)
						 {
							 for (int j = 0; j < 4; j++)
							 {
								 if (bowicons[i]->bowiconiid == whale[j])
								 {  
									 if (id == 245 && bowicons[i]->bowiconiid == 36)
									 {
										 id = bowicons[i]->bowiconiid;
									 }
									 else if (id == 36 && bowicons[i]->bowiconiid == 245)
									 {
										 id = 36;
									 }
									 else if (id < bowicons[i]->bowiconiid)
									 {
										 id = bowicons[i]->bowiconiid;
									 }
								 }
							 }
						 }
	}
		break;
	case EV_SEA_ROCK:
	{
						for (int i = 0; i < bowNum; i++)
						{
							for (int j = 0; j < 4; j++)
							{
								if (bowicons[i]->bowiconiid == dolphin[j])
								{
									if (id == 244 && bowicons[i]->bowiconiid == 35)
									{
										id = bowicons[i]->bowiconiid;
									}
									else if (id == 35 && bowicons[i]->bowiconiid == 246)
									{
										id = 35;
									}
									else if (id < bowicons[i]->bowiconiid)
									{
										id = bowicons[i]->bowiconiid;
									}
								}
							}
						}

	}
		break;
	case EV_SEA_PESTIS:
	case EV_SEA_HOMESICK:
	case EV_SEA_SEPSIS:
	{
						  for (int i = 0; i < bowNum; i++)
						  {
							  for (int j = 0; j < 4; j++)
							  {
								  if (id == 243 && bowicons[i]->bowiconiid == 34)
								  {
									  id = bowicons[i]->bowiconiid;
								  }
								  else if (id == 34 && bowicons[i]->bowiconiid == 243)
								  {
									  id = 34;
								  }
								  else if (id < bowicons[i]->bowiconiid)
								  {
									  id = bowicons[i]->bowiconiid;
								  }
							  }
						  }
	}
		break;
	default:
		break;
	}
	return id;
}
void UISailHUD::salvageFloatResult(const TriggerSeaEventResult* result)
{
	openView(COMMOM_COCOS_RES[C_VIEW_SALVAGE_RESULT]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SALVAGE_RESULT]);
	view->setCameraMask(4, true);
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_SALVAGE_SUCCEED_42);
	auto shopData = SINGLE_SHOP->getItemData();
	auto panel_item = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_item"));
	auto image_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_item, "image_bg"));
	auto label_content_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_item, "label_content_1"));
	auto label_content_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_content_2"));
	label_content_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAPUI_SALAVE_FLOATER_TIP"]);
	image_bg->loadTexture("res/salvage_bg/float.jpg");
	label_content_2->setVisible(false);
	int n = 0;
	if (result->lootcoins){
		n++;
	}
	if (result->loot){
		n++;
	}
	for (size_t i = 0; i < 3; i++)
	{
		auto item = view->getChildByName<ImageView*>(StringUtils::format("image_items_bg_%d", i + 1));
		auto i_icon = item->getChildByName<ImageView*>("image_item");
		auto t_num = item->getChildByName<Text*>("label_item_num");
		i_icon->ignoreContentAdaptWithSize(false);
		if (i < n)
		{
				if (i == 0)
				{
					if (result->lootcoins)
					{
						item->setTouchEnabled(true);
						item->addTouchEventListener(CC_CALLBACK_2(UISailHUD::showItemInfo, this));
						item->setTag(ITEM_TYPE_SPECIAL);
						i_icon->setTag(10002);
						i_icon->loadTexture(getItemIconPath(10002));
						setTextSizeAndOutline(t_num, result->lootcoins);		
					}
					else
					{
						item->setTouchEnabled(true);
						item->addTouchEventListener(CC_CALLBACK_2(UISailHUD::showItemInfo, this));
						i_icon->loadTexture(getItemIconPath(result->loot->itemid));
						setTextSizeAndOutline(t_num, result->loot->count);
						std::map<int, ITEM_RES>::iterator m1_Iter;
						for (m1_Iter = shopData.begin(); m1_Iter != shopData.end(); m1_Iter++)
						{
							if (m1_Iter->second.id == result->loot->itemid)
							{
								item->setTag(m1_Iter->second.type);
								break;
							}
						}
						i_icon->setTag(result->loot->itemid);
					}
				}
				else
				{
					item->setTouchEnabled(true);
					item->addTouchEventListener(CC_CALLBACK_2(UISailHUD::showItemInfo, this));
					i_icon->loadTexture(getItemIconPath(result->loot->itemid));
					setTextSizeAndOutline(t_num, result->loot->count);
					std::map<int, ITEM_RES>::iterator m1_Iter;
					for (m1_Iter = shopData.begin(); m1_Iter != shopData.end(); m1_Iter++)
					{
						if (m1_Iter->second.id == result->loot->itemid)
						{
							item->setTag(m1_Iter->second.type);
							break;
						}
					}
					i_icon->setTag(result->loot->itemid);
				}
			item->setVisible(true);
		}
		else
		{
			item->setVisible(false);
		}
		//位置微调
		if (n == 1)
		{
			if (i == 0)
			{
				item->setPositionX(item->getPositionX() + item->getBoundingBox().size.width*1.3);
			}
		}
		else if (n == 2)
		{
			if (i == 0 || i == 1)
			{
				item->setPositionX(item->getPositionX() + item->getBoundingBox().size.width*0.8);
			}
		}
	}
	Sprite*spriteAnimate = Sprite::create();
	spriteAnimate->setAnchorPoint(Vec2(.5, 0.5));
	spriteAnimate->setScale(7);
	spriteAnimate->setCameraMask(4, true);
	view->addChild(spriteAnimate, -1);
	spriteAnimate->setPosition(Vec2(view->getContentSize().width / 2 + 80, view->getContentSize().height / 2 - 80));

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
	Animation* animation_shiplight = Animation::createWithSpriteFrames(frames, 2.0 / 28, 1);
	auto animateEff = Animate::create(animation_shiplight);

	spriteAnimate->runAction(RepeatForever::create(animateEff));

}

void UISailHUD::updateBySecond(const float fTime)
{
	auto layer = dynamic_cast<UISailManage*>(this->getParent());
	LeaveCityResult * result = layer->getLeaveCityResult();
	result->drop_rate_increase_ramian_time--;
	if (result->drop_rate_increase_ramian_time >= 0)
	{
		auto view = getViewRoot(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
		if (view)
		{
			std::string st_content = SINGLE_SHOP->getTipsInfo()["TIP_MAIN_DROPS_TIME"];
			std::string old_value = "[time]";
			std::string new_value = timeUtil(result->drop_rate_increase_ramian_time, TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_GM);
			repalce_all_ditinct(st_content, old_value, new_value);
			auto t_content = dynamic_cast<Text*>(view->getChildByName("label_dropitem_tiptext"));
			t_content->setString(st_content);
		}

		if (result->drop_rate_increase_ramian_time == 0)
		{
			closeView(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
			this->unschedule(schedule_selector(UISailHUD::updateBySecond));
			auto m_pSubPanel = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
			//掉落服务
			auto i_dorp_service = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pSubPanel, "image_dorp_service"));
			i_dorp_service->setVisible(false);
		}
	}
}

void UISailHUD::updateNationWarBySecond(const float fTime)
{
	if (m_pMyNationWarResult)
	{
		m_pMyNationWarResult->time_left--;
		auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
		auto p_war = view->getChildByName<ImageView*>("panel_war_info");
		auto t_content = p_war->getChildByName<Text*>("label_derable");
		std::string content;
		if (m_pMyNationWarResult->war_status == 1)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_START_TIME"];
		}
		else
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_END_TIME"];
		}
		std::string new_value = timeUtil(m_pMyNationWarResult->time_left, TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_GM);
		std::string old_value = "[time]";
		repalce_all_ditinct(content, old_value, new_value);
		t_content->setString(content);

		if (m_pMyNationWarResult->time_left <= 0)
		{
			if (m_pMyNationWarResult->war_status == 1)
			{
				ProtocolThread::GetInstance()->getMyNationWar();
			}
			else
			{
				t_content->setString(content);
				this->schedule(schedule_selector(UISailHUD::updateNationWarBySecond), 1);
				p_war->stopAllActions();
				p_war->runAction(Spawn::createWithTwoActions(MoveBy::create(1.5, Vec2(- p_war->getContentSize().width, 0)), FadeOut::create(1)));
			}
			this->unschedule(schedule_selector(UISailHUD::updateNationWarBySecond));
		}
	}
}
