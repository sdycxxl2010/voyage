
#include "UIGuideSailHUD.h"
#include "SystemVar.h"
#include "TVSingle.h"
#include "UIGuideShip.h"
#include "TVBattleManager.h"
#include "ProtocolThread.h"
#include "TVSailDefineVar.h"
#include <map>
#include "UIGuideSailManage.h"
#include "TVSeaEvent.h"

#include "UIBattleHUD.h"
#include "UICommon.h"
#include "UIInform.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "TVSceneLoader.h"


UIGuideSailHUD::UIGuideSailHUD()
{
	m_pSubPanel = nullptr;
	m_bIsBlocking = false;
	m_nLandCityId = 0;
	m_nCurMapIndex = -1;
	m_pShip = 0;
	m_pPanelExp = nullptr;
	m_pPanelRep = nullptr;
	m_nConfirmIndex = 0;
	m_nEventversion = 0;
	m_nFindcityId = 0;
	for (int i = 0; i < 9; i++)
	{
		std::string mapName = StringUtils::format("ship/seaFiled_1/map_%d.jpg", i + 1);
		TextureCache::getInstance()->addImage(mapName);
	}
	m_nDisableCancelFight = 0;
	m_nMaxCrewNum = 0;
	m_pProgressbar_supply = 0;
	m_pLandName = 0;
	m_pShipFlag = 0;
	m_pSmallMap = 0;
	m_pProgressbar_sailor = 0;
	m_pImage_title_bg = 0;
	m_pPanelActionbar = 0;
	m_bIsLevelUp = false;
	m_bIsPrestigeUp = false;
	m_bIsFailed = false;

	m_nForceFightNpcId = 0;
	m_bIsNewCityCauseLvUp = false;
	m_bIsNewCityCauseFameUp = false;
	
	m_nMaxSupply = 0;
	m_pDiscoverCityResult = nullptr;
	m_playerLv = nullptr;
	m_playerRepLv = nullptr;
	m_pSafeAreaLabel = nullptr;
	m_bCurrentInSafeArea = true;
	m_SafeAreaCheck = nullptr;
}

UIGuideSailHUD::~UIGuideSailHUD()
{
	_userData = nullptr;
	//this->unscheduleUpdate();
	this->unscheduleAllSelectors();

	//SINGLE_SHOP->releaseLandData();
	for (int i = 0; i < 9; i++)
	{
		std::string mapName = StringUtils::format("ship/seaFiled_1/map_%d.jpg", i + 1);
		TextureCache::getInstance()->removeTextureForKey(mapName);
	}
	NotificationCenter::getInstance()->removeAllObservers(this);
}

UIGuideSailHUD* UIGuideSailHUD::create()
{
	UIGuideSailHUD* mapui = new UIGuideSailHUD;
	mapui->setCameraMask(4);
	if (mapui && mapui->init())
	{
		mapui->autorelease();
		return mapui;
	}
	CC_SAFE_DELETE(mapui);
	return nullptr;
}

bool UIGuideSailHUD::init()
{
	bool pRet = false;
	do
	{
		this->initf(0);
		pRet = true;
	} while (0);
	m_SafeAreaCheck = new SafeAreaCheck();
	return pRet;
}


void UIGuideSailHUD::initf(float f)
{
	//海域划分
	//SINGLE_SHOP->loadLandData();
	auto landInfo = SINGLE_SHOP->getLandInfo();
	for (auto i = 0; i < 9; i++)
	{
		m_MapRect[i].setRect(landInfo[i + 1].left_down_x, landInfo[i + 1].left_down_y, 7518, 3727);
	}

	//海上主界面 摄像机1，底层
	openView(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
	auto m_pSubPanel = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
	//按钮不可按
	auto button_chat = m_pSubPanel->getChildByName<Button*>("button_chat");
	auto panel_btn = m_pSubPanel->getChildByName<Widget*>("panel_btn");
	auto button_stopping = panel_btn->getChildByName<Button*>("button_stopping");
	auto button_mail = panel_btn->getChildByName<Button*>("button_mail");
	auto button_task = panel_btn->getChildByName<Button*>("button_task");
	auto button_fleetinfo = panel_btn->getChildByName<Button*>("button_fleetinfo");
	auto button_salvage = panel_btn->getChildByName<Button*>("button_salvage");
	button_chat->setTouchEnabled(false);
	button_stopping->setTouchEnabled(false);
	button_mail->setTouchEnabled(false);
	button_task->setTouchEnabled(false);
	button_fleetinfo->setTouchEnabled(false);
	button_salvage->setTouchEnabled(false);
	
	auto image_chat = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pSubPanel, "image_chat"));
	image_chat->setVisible(false);
	//地图
	auto panel_map = m_pSubPanel->getChildByName<Widget*>("panel_map");
	m_pSmallMap = panel_map->getChildByName<ImageView*>("image_map_2_m");
	ImageView *image_cityname_bg = m_pSubPanel->getChildByName<ImageView*>("image_cityname_bg");
	m_pLandName = image_cityname_bg->getChildByName<Text*>("label_map_name");
	//m_pLandName->enableOutline(Color4B::BLACK, OUTLINE_MIN);
	auto image_safe_bg = m_pSubPanel->getChildByName<ImageView*>("image_sea_area_warning");
	m_pSafeAreaLabel = image_safe_bg->getChildByName<Text*>("label_warning_name");
	m_pSafeAreaLabel->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAFE_AREA"]);
	m_pSafeAreaLabel->setTextColor(Color4B(17, 222, 31, 255));
	m_pPanelActionbar = m_pSubPanel->getChildByName<Widget*>("panel_actionbar");
	auto w_head = m_pPanelActionbar->getChildByName<Button*>("button_head_player_bg");
	w_head->setTouchEnabled(false);
	auto image_head_lv = w_head->getChildByName<ImageView*>("image_head_lv");
	ImageView* i_head_icon = w_head->getChildByName<ImageView*>("image_head");
	m_playerLv = dynamic_cast<Text*>(image_head_lv->getChildByName<Widget*>("label_lv"));
	m_playerRepLv = dynamic_cast<Text*>(image_head_lv->getChildByName<Widget*>("label_lv_r"));
	m_pImage_title_bg = m_pPanelActionbar->getChildByName<ImageView*>("image_title_bg");
	ImageView* image_progressbar_sailor = m_pImage_title_bg->getChildByName<ImageView*>("image_progressbar_sailor");
	m_pProgressbar_sailor = image_progressbar_sailor->getChildByName<LoadingBar*>("progressbar_durable");

	ImageView* image_progressbar_supply = m_pImage_title_bg->getChildByName<ImageView*>("image_progressbar_supply");
	m_pProgressbar_supply = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_durable");

	auto expNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAEXPNUM").c_str());
	auto repNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAREPNUM").c_str());
	auto expLv = EXP_NUM_TO_LEVEL(expNovice);
	auto repLv = FAME_NUM_TO_LEVEL(repNovice);

	std::string level = StringUtils::format("%d", expLv);
	std::string repLevel = StringUtils::format("%d", repLv);
	i_head_icon->ignoreContentAdaptWithSize(false);
	std::string iconPath = StringUtils::format("res/player_icon/icon_%d.png", SINGLE_HERO->m_iIconidx);
	i_head_icon->loadTexture(iconPath);
	m_pPanelRep = w_head->getChildByName<Widget*>("panel_rep");
	m_pPanelExp = w_head->getChildByName<Widget*>("panel_exp");


	m_pShipFlag = ImageView::create(SHIP_ICON_PATH);
	m_pShipFlag->setScale(1);
	m_pShipFlag->setAnchorPoint(Vec2(0.5, 0.5));
	//TODO: fix it via cocostudio.
	m_pSmallMap->setContentSize(Size(724, 480));
	m_pSmallMap->addChild(m_pShipFlag);
	m_pSmallMap->setCameraMask(4, true);
	m_pShipFlag->setPosition(Vec2(1000, 1000));

	m_playerLv->setString(level);
	m_playerRepLv->setString(repLevel);
	this->scheduleUpdate();
	this->schedule(schedule_selector(UIGuideSailHUD::mewSound), 15.f);
	auto btn_loot = Helper::seekWidgetByName(m_pSubPanel, "button_outlook");
	btn_loot->setTouchEnabled(false);
	btn_loot->setVisible(false);
}

void UIGuideSailHUD::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		auto button = dynamic_cast<Widget*>(pSender);
		std::string name = button->getName();
		buttonEvent(button, name);
	}
}
void UIGuideSailHUD::buttonEvent(Widget* target, std::string name, bool isRefresh /* = false */)
{
	//开始战斗
	if (isButton(button_start_battle))
	{
		closeView();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_IN_COMBAT_17);
		openFightEvent();
		return;
	}
	//引导中发现新城市结算
	if (isButton(button_result_yes))
	{
		SINGLE_AUDIO->vresumeBGMusic();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		shipPause(false);
		return;
	}
 	if (isButton(panel_r_levelup))
 	{
 		//shipPause(false);
		//改成固定值
		findCityResult(100, 50);
 		return;
 	}
	//回到主城
	if (isButton(button_landCity))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();

		dynamic_cast<UIGuideSailManage*>(_parent)->GoBackCity();
		SINGLE_HERO->m_heroIsOnsea = false;
		//新手引导海上流程走完，数据写在本地。包括：扣除补给花费的银币3500
		//auto coinNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUASILVERNUM").c_str());
		//UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUASILVERNUM").c_str(), coinNovice - 3500);
		UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str(), 5);
		UserDefault::getInstance()->flush();
		return;

	}
	//confirm if enter city
	if (isButton(button_confirm_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

		if (CONFIRM_INDEX_INTOCITY == m_nConfirmIndex){
			ProtocolThread::GetInstance()->reachCity(SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].x, MAP_CELL_WIDTH* MAP_WIDTH - SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].y, UILoadingIndicator::createWithMask(this, 4));
			return;
		}
		if (CONFIRM_INDEX_NOTAUTO == m_nConfirmIndex){

			dynamic_cast<UIGuideShip*>(m_pShip)->isContinueAutoSailing(true);
			return;
		}

		return;
	}
	//confirm if enter city
	if (isButton(button_confirm_no))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (CONFIRM_INDEX_NOTAUTO == m_nConfirmIndex){

			dynamic_cast<UIGuideShip*>(m_pShip)->isContinueAutoSailing(false);
			return;
		}
		return;
	}
	//登录城市
	if (isButton(button_land))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		ProtocolThread::GetInstance()->reachCity(SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].x, MAP_CELL_WIDTH* MAP_WIDTH - SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].y, UILoadingIndicator::createWithMask(this, 4));
		return;
	}
	//发现新城市
	if (isButton(button_findnew_city) || isButton(panel_found_newcity))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->addDiscoveredCity(m_nFindcityId, UILoadingIndicator::createWithMask(this, 4));
		closeView();
		return;
	}
}

void UIGuideSailHUD::landCity(GetCityStatusResult* result)
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
	auto panel_friend = view->getChildByName<Widget*>("panel_friend");
	auto i_image_flag = panel_friend->getChildByName<ImageView*>("image_flag");
	i_image_flag->setVisible(false);
	auto city_name = view->getChildByName<Text*>("label_goods_name");
	auto image_flag = view->getChildByName<ImageView*>("image_flag");
	auto t_license = view->getChildByName<Text*>("label_price_num");
	auto image_friend = view->getChildByName<ImageView*>("image_friend");
	city_name->setString(SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].name);
	image_flag->ignoreContentAdaptWithSize(false);
	image_flag->loadTexture(getCountryIconPath(SINGLE_SHOP->getCitiesInfo()[m_nLandCityId].nation));
	auto button_close = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_close"));
	button_close->setTouchEnabled(false);
	auto button_land = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_land"));
	//引导的指示
	auto handpic = Sprite::create();
	handpic->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
	handpic->setRotation(-180.0f);
	handpic->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
	button_land->addChild(handpic,10);
	handpic->setCameraMask(4, true);
	handpic->setLocalZOrder(1000); 
	//小手在按钮右下角
	handpic->setPosition(Vec2(button_land->getContentSize().width / 2 + handpic->getContentSize().width / 2, button_land->getContentSize().height / 2 - button_land->getContentSize().height / 2 * 0.6 - handpic->getContentSize().height / 2 * 0.6));
	view->setCameraMask(4, true);
	view->setSubNodeMaxCameraMask(4);
	
	t_license->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_HAVE_CITY_LICENSE"]);
	
	image_friend->ignoreContentAdaptWithSize(false);
	image_friend->loadTexture(ICON_FRIEND_NPC);
	image_friend->setPosition(Vec2(panel_friend->getContentSize().width/2,panel_friend->getContentSize().height/2));
	return;
}

void UIGuideSailHUD::openFightEvent()
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

void UIGuideSailHUD::setShipPositon(const Vec2 pos){

	Text *label_coordinates_num = dynamic_cast<Text *>(Helper::seekWidgetByName(m_pPanelActionbar, "label_coordinates_num"));
	std::string s_pos = StringUtils::format("%ld,%ld", (long int)pos.x, (long int)pos.y);
	label_coordinates_num->setString(s_pos);
	label_coordinates_num->setTextVerticalAlignment(TextVAlignment::TOP);
}

void UIGuideSailHUD::setShip(Node* ship)
{
	m_pShip = ship;
}
void UIGuideSailHUD::setSailingDay(const int days)
{
	auto listview_clock_num = dynamic_cast<ListView *>(Helper::seekWidgetByName(m_pPanelActionbar, "listview_clock_num"));
	listview_clock_num->setClippingEnabled(false);
	Text *label_ship_days_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(listview_clock_num, "label_ship_days_num_1"));
	std::string sdays = StringUtils::format("%d  %s", days, SINGLE_SHOP->getTipsInfo()["TIP_DAY"].data());
	label_ship_days_num_1->setString(sdays);
}
//在新手引导水手和补给都不会损失
void UIGuideSailHUD::setCrew(const int crewnum)
{
	ListView* listview_sailor_num = m_pImage_title_bg->getChildByName<ListView*>("listview_sailor_num");
	Text *label_ship_durable_num_1 = dynamic_cast<Text*>(listview_sailor_num->getChildByName("label_ship_durable_num_1"));
	Text *label_ship_durable_num_2 = dynamic_cast<Text*>(listview_sailor_num->getChildByName("label_ship_durable_num_2"));
	label_ship_durable_num_1->setString(StringUtils::format("%d", crewnum));
	label_ship_durable_num_2->setString(StringUtils::format("/%d", crewnum));

	m_pProgressbar_sailor->setPercent(100);
	listview_sailor_num->refreshView();
}
void UIGuideSailHUD::setSupplies(const int supplies, const int maxSupplies)
{
	ListView* listview_supply_num = m_pImage_title_bg->getChildByName<ListView*>("listview_supply_num");
	Text *label_ship_durable_num_1 = dynamic_cast<Text*>(listview_supply_num->getChildByName("label_ship_durable_num_1"));
	Text *label_ship_durable_num_2 = dynamic_cast<Text*>(listview_supply_num->getChildByName("label_ship_durable_num_2"));
	label_ship_durable_num_1->setString(StringUtils::format("%d", maxSupplies));
	label_ship_durable_num_2->setString(StringUtils::format("/%d", maxSupplies));

	m_pProgressbar_supply->setPercent(100);
	listview_supply_num->refreshView();
}
void UIGuideSailHUD::setExpAndFame(const long int exp, const long int fame){

	int levle = EXP_NUM_TO_LEVEL(exp);
	int fameLevel = FAME_NUM_TO_LEVEL(fame);

	float temp_exp = 0;
	if (levle < LEVEL_MAX_NUM)
	{
		temp_exp = (exp - LEVEL_TO_EXP_NUM(levle))*1.0 / (LEVEL_TO_EXP_NUM(levle + 1) - LEVEL_TO_EXP_NUM(levle));
	}
	float temp_rep = 0;
	if (fameLevel < LEVEL_MAX_NUM)
	{
		temp_rep = (fame - LEVEL_TO_FAME_NUM(fameLevel))*1.0 / (LEVEL_TO_FAME_NUM(fameLevel + 1) - LEVEL_TO_FAME_NUM(fameLevel));
	}

	m_pPanelExp->setContentSize(Size(m_pPanelExp->getContentSize().width, 120 * temp_exp));
	m_pPanelRep->setContentSize(Size(m_pPanelRep->getContentSize().width, 120 * temp_rep));
}

void UIGuideSailHUD::setPlayerLv(const int lv, const int repLv)
{
	m_playerLv->setString(StringUtils::format("%d", lv));
	m_playerRepLv->setString(StringUtils::format("%d", repLv));
}

void UIGuideSailHUD::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	if (msgType == PROTO_TYPE_EngageInFightResult)
	{
		EngageInFightResult* result = (EngageInFightResult*)message;
		if (result && !result->failed)
		{
			//新手引导进入战斗	
			openView(MAPUI_COCOS_RES[INDEX_UI_BATTLE_VS_CSB]);
			auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BATTLE_VS_CSB]);
			view->setCameraMask((unsigned short)CameraFlag::USER2, true);
			flushBattleVS(view, result);
			_userData = result;
		}
		else
		{
			shipPause(false);
			openSuccessOrFailDialog("TIP_SAILING_ENAGE_FIGHT_FAIL");
		}
		m_bIsBlocking = false;
		return;
	}
	else if (msgType == PROTO_TYPE_TriggerSeaEventResult){
		m_bIsBlocking = false;
	}
	else if (PROTO_TYPE_EndFightResult == msgType)
	{
		EndFightResult* result = (EndFightResult*)message;
		if (result->failed == 0)
		{
			if (result->reason == 2){
				m_bIsFailed = true;
			}
			if (result->isautofight){
				closeView();
			}
		}
	}
	else if (PROTO_TYPE_GetCityStatusResult == msgType)
	{
		GetCityStatusResult *result = (GetCityStatusResult*)message;
		if (result->failed == 0)
		{
			landCity(result);
		}
	}
	else if (PROTO_TYPE_AddDiscoveredCityResult == msgType)
	{
		AddDiscoveredCityResult*result = (AddDiscoveredCityResult*)message;
		if (result->failed == 0)
		{
			m_pDiscoverCityResult = result;
			//改成固定值
			//findCityResult(100, 50);
			auto expNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAEXPNUM").c_str());
			auto repNovice = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAREPNUM").c_str());
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAEXPNUM").c_str(), expNovice + 100);
			UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAREPNUM").c_str(), repNovice + 50);
			UserDefault::getInstance()->flush();
			if (result->newlevel)
			{
				m_bIsNewCityCauseLvUp = true;
				m_bIsLevelUp = true;
				SINGLE_HERO->m_iLevel = result->newlevel;
			}
			if (result->new_fame_level)
			{
				m_bIsNewCityCauseFameUp = true;
				m_bIsPrestigeUp = true;
				SINGLE_HERO->m_iPrestigeLv = result->new_fame_level;
			}
			//reset head
			int levle = EXP_NUM_TO_LEVEL(expNovice+100);
			int repLv = FAME_NUM_TO_LEVEL(repNovice + 50);
			SINGLE_HERO->m_iLevel = levle;
			SINGLE_HERO->m_iPrestigeLv = repLv;
			setPlayerLv(levle,repLv);
			setExpAndFame(expNovice + 100, repNovice + 50);
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
		}
	}
}
void UIGuideSailHUD::update(float f)
{
	if (!m_pShip) return;
	Vec2 pos = m_pShip->getPosition();
	UIGuideSailManage * parent = dynamic_cast<UIGuideSailManage*>(this->getParent());
	if (parent->isFarFromEnemy()){
		//安全海域
		//log("safe  %f, %f",pos.x, pos.y);
		if (!m_bCurrentInSafeArea){
			SINGLE_HERO->m_bInSafeArea = true;
			m_bCurrentInSafeArea = true;
			m_pSafeAreaLabel->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAFE_AREA"]);
			m_pSafeAreaLabel->setTextColor(Color4B(17, 222, 31, 255));
		}
	}
	else{
		//log("not safe  %f, %f",pos.x, pos.y);
		//危险海域
		if (m_bCurrentInSafeArea){
			SINGLE_HERO->m_bInSafeArea = false;
			m_bCurrentInSafeArea = false;
			m_pSafeAreaLabel->setString(SINGLE_SHOP->getTipsInfo()["TIP_DANGEROUS_AREA"]);
			m_pSafeAreaLabel->setTextColor(Color4B(251, 44, 44, 255));
			m_pSafeAreaLabel->runAction(Blink::create(2, 3));
		}
	}
	SINGLE_HERO->shippos = pos;
	for (int i = 0; i< 9; i++)
	{
		if (m_MapRect[i].containsPoint(pos))
		{
			if (i != m_nCurMapIndex)
			{
				m_nCurMapIndex = i;
				std::string mapName = StringUtils::format("ship/seaFiled_1/map_%d.jpg", i + 1);
				m_pSmallMap->loadTexture(mapName);
				m_pLandName->setString(SINGLE_SHOP->getLandInfo()[i + 1].name);
				GuildmapNameAction();
			}
			break;
		}
	}
	//no map zone
	if (!m_MapRect[0].containsPoint(pos) && !m_MapRect[1].containsPoint(pos) && !m_MapRect[2].containsPoint(pos) && !m_MapRect[3].containsPoint(pos) && !m_MapRect[4].containsPoint(pos)
		&& !m_MapRect[5].containsPoint(pos) && !m_MapRect[6].containsPoint(pos) && !m_MapRect[7].containsPoint(pos) && !m_MapRect[8].containsPoint(pos))
	{
		m_pLandName->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_UNKNOW_SEA"]);
		m_pSmallMap->loadTexture("ship/seaFiled_1/no_map_zone.jpg");
		m_nCurMapIndex = -1;
	}
	//更新雷达位置
	else
	{
		Vec2 curZeroPos = Vec2(SmallMaps[m_nCurMapIndex].getMinX(), SmallMaps[m_nCurMapIndex].getMinY());
		Size smallSize = m_pSmallMap->getContentSize();
		Vec2 diffPos = pos - curZeroPos;
		int origBorder = 120;
		float imgScale = smallSize.height / 480;
		int border = origBorder * imgScale;
		float scaleX = ((smallSize.width - 2 * border) / (SmallMaps[m_nCurMapIndex].size.width)); //* m_vRoot->getScaleX()));
		float scaleY = ((smallSize.height - 2 * border) / (SmallMaps[m_nCurMapIndex].size.height));//* m_vRoot->getScaleY()));
		diffPos.x = diffPos.x *  scaleX;
		diffPos.y = diffPos.y *  scaleY;

		if (diffPos.x < 0)
		{
			diffPos.x = 0;
		}
		else if (diffPos.x > SmallMaps[m_nCurMapIndex].getMaxX())
		{
			diffPos.x = SmallMaps[m_nCurMapIndex].getMaxX();
		}
		if (diffPos.y < 0)
		{
			diffPos.y = 0;
		}
		else if (diffPos.y > SmallMaps[m_nCurMapIndex].getMaxY())
		{
			diffPos.y = SmallMaps[m_nCurMapIndex].getMaxY();
		}
		m_pSmallMap->setPosition(Vec2(smallSize.width / 2 + border, smallSize.height / 2 + border) - (diffPos + Vec2(border, border)) - Vec2(40, 40));
	}
	m_pSmallMap->setCameraMask(4, true);

	if (m_nForceFightNpcId > 0){
		m_nForceFightNpcId = 0;
		m_nDisableCancelFight = 2;
		m_bIsBlocking = true;
		shipPause(true);
		ProtocolThread::GetInstance()->engageInFight(-1, m_nDisableCancelFight, m_nEventversion, FIGHT_TYPE_NORMAL, 0, UILoadingIndicator::createWithMask(this, 4));
	}
}
bool UIGuideSailHUD::showSeaEvent(TVSeaEvent* se)
{
	LayerColor* blacklayer = LayerColor::create(Color4B(0, 0, 0, 255));
	blacklayer->setCameraMask(4, true);
	TintTo* tt = TintTo::create(.3f, 255, 255, 255);
	TintTo* tt_rev = TintTo::create(.3f, 0, 0, 0);
	//引导中海上事件屏蔽掉，只有战斗
	switch (se->m_Type)
	{
	case EV_SEA_NPC:
	{
		m_nDisableCancelFight = 1;
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_CRASH_18);
		SINGLE_AUDIO->vpauseBGMusic();			   
		ProtocolThread::GetInstance()->engageInFight(se->m_Event_id, m_nDisableCancelFight, m_nEventversion, FIGHT_TYPE_NORMAL, 0, UILoadingIndicator::createWithMask(this, 4));
		SINGLE_HERO->shipGuideEnemy = false;//战斗后重新回来海上不在加船
		//shipPause(false);
		break;
	}
	default:
		break;
	}
	return false;
}
void UIGuideSailHUD::shipPause(bool isPause)
{
	if (isPause)
	{
		dynamic_cast<UIGuideSailManage*>(_parent)->seaEventStart();
	}
	else
	{
		dynamic_cast<UIGuideSailManage*>(_parent)->seaEventEnd();
	}

}
//mew sound
void UIGuideSailHUD::mewSound(float f)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_MEW_06);
}

void UIGuideSailHUD::flushBattleVS(Widget* view, EngageInFightResult* result){

	Button*button_cancel = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_challenge_close"));
	Button*button_automatic = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_automatic"));
	Button*button_start_battle = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_start_battle"));
	auto handpic = Sprite::create();
	handpic->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
	handpic->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
	button_start_battle->addChild(handpic, 10);
	handpic->setCameraMask(4, true);
	handpic->setLocalZOrder(1000);
	//小手在按钮左上角
	handpic->setPosition(Vec2(button_start_battle->getContentSize().width / 2 - handpic->getContentSize().width / 2, button_start_battle->getContentSize().height / 2 * 1.6 + handpic->getContentSize().height / 2 * 0.6));

	
	button_cancel->setBright(false);
	button_automatic->setBright(false);
	button_cancel->setTouchEnabled(false);
	button_automatic->setTouchEnabled(false);
	auto i_speedup = button_automatic->getChildByName<ImageView*>("image_speedup");
	setGLProgramState(i_speedup, true);

	Widget* panel_title_left = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_title_left"));
	ImageView* image_flag_left = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_left, "image_flag_left"));
	Text* label_name_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_left, "label_name_left"));
	Text* label_lv_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_left, "label_lv_left"));
	ImageView* image_moral_left = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_left, "image_moral_left"));

	std::string m_country = StringUtils::format("res/country_icon/flag_%d.png", SINGLE_HERO->m_iNation);
	image_flag_left->loadTexture(m_country);
	label_name_left->setString(SINGLE_HERO->m_sName);
	std::string m_level = StringUtils::format("Lv. %d", 2);
	label_lv_left->setString(m_level);
	flushMoral(image_moral_left, result->morale);

	Widget* panel_title_right = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_title_right"));
	ImageView* image_flag_right = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_right, "image_flag_right"));
	Text* label_name_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_right, "label_name_right"));
	Text* label_lv_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_right, "label_lv_right"));
	ImageView* image_moral_right = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_right, "image_moral_right"));

	std::string e_country = StringUtils::format("res/country_icon/flag_%d.png", result->npcforceid);
	image_flag_right->loadTexture(e_country);
	std::string e_strName = SINGLE_SHOP->getBattleNpcInfo()[result->npcid].name;
	label_name_right->setString(e_strName);
	std::string e_level = StringUtils::format("Lv. %d", 1);
	label_lv_right->setString(e_level);
	flushMoral(image_moral_right, result->npcmorale);

	Widget* panel_combat_left = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_combat_left"));
	Text* label_combat_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_combat_left"));
	Text* label_ship_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_ship_left"));
	Text* label_sailor_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_sailor_left"));
	std::string m_combat = StringUtils::format("%d", result->myinfo->power);
	label_combat_left->setString(m_combat);
	std::string m_shipNum = StringUtils::format("%d", result->myinfo->shipnum);
	label_ship_left->setString(m_shipNum);
	std::string m_sailNum = StringUtils::format("%d", result->myinfo->sailnum);
	label_sailor_left->setString(m_sailNum);

	Widget* panel_combat_right = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_combat_right"));
	Text* label_win_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_win_num"));
	Text* label_combat_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_combat_right"));
	Text* label_ship_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_ship_right"));
	Text* label_sailor_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_sailor_right"));
	std::string e_combat = StringUtils::format("%d", 1000);
	label_combat_right->setString(e_combat);
	std::string e_shipNum = StringUtils::format("%d", 1);
	label_ship_right->setString(e_shipNum);
	std::string e_sailNum = StringUtils::format("%d",30);
	label_sailor_right->setString(e_sailNum);
	std::string str_win_num = StringUtils::format("%d%%", 100);
	label_win_num->setString(str_win_num);
    auto t_ranson = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "labe_ranson"));
	if (t_ranson)
	{
		t_ranson->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_BATTLE_NOT_LOSE"]);
	}
	auto b_info = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_batt_info"));
	if (b_info)
	{
		b_info->setTouchEnabled(false);
	}
}

void UIGuideSailHUD::flushMoral(Widget* view, int num){

	for (int i = 1; i <= 5; i++)
	{

		std::string str_moral = StringUtils::format("image_moral_%d", i);
		ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(view, str_moral));
		moral_image->loadTexture(getMoralIconPath(3));

	}

	int num_morale = num;
	int n1000 = (num_morale / 1000) % 6;
	int n100 = num_morale - n1000 * 1000;
	for (int i = 1; i <= n1000; i++){

		std::string str_moral = StringUtils::format("image_moral_%d", i);
		ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(view, str_moral));
		moral_image->loadTexture(getMoralIconPath(1));

	}
	if (n1000 < 5 && n100 >0){
		std::string str_moral = StringUtils::format("image_moral_%d", n1000 + 1);
		ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(view, str_moral));
		moral_image->loadTexture(getMoralIconPath(2));
	}

}
void UIGuideSailHUD::findNewCity(const int cityId)
{
	m_nFindcityId = cityId;
	openView(MAPUI_COCOS_RES[INDEX_UI_FOUND_NEWCITY_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FOUND_NEWCITY_CSB]);
	view->setCameraMask(4, true);
	view->addTouchEventListener(CC_CALLBACK_2(UIGuideSailHUD::menuCall_func,this));
	auto image_light = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_light"));
	image_light->runAction(RepeatForever::create(RotateBy::create(2.0f, 360)));
	auto button_cueta = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_cueta"));
	button_cueta->setTouchEnabled(false);
	auto button_ok = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_findnew_city"));
	button_ok->setTitleText("ok");
	auto label_cityname = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cityname"));
	label_cityname->setString(SINGLE_SHOP->getCitiesInfo()[cityId].name);
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
	button_cueta->loadTextures(path, path, path);
}
void UIGuideSailHUD::findCityResult(const int addLevelexp, const int addFame)
{
	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	view->setCameraMask(4, true);
	//引导的小手
	auto buttonResult_yes = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_result_yes"));
	auto handpic = Sprite::create();
	handpic->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
	handpic->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
	buttonResult_yes->addChild(handpic, 10);
	handpic->setCameraMask(4, true);
	handpic->setLocalZOrder(1000);
	//小手在按钮右下角
	handpic->setRotation(-180.0f);
	handpic->setPosition(Vec2(buttonResult_yes->getContentSize().width / 2 + handpic->getContentSize().width / 2, buttonResult_yes->getContentSize().height / 2 - buttonResult_yes->getContentSize().height / 2 * 0.6 - handpic->getContentSize().height / 2 * 0.6));
	
	auto label_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title"));
	label_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_FINDNEWCITY_REWARD"]);
	auto panel_result = view->getChildByName("panel_result");
	auto listviewResult = panel_result->getChildByName<ListView*>("listview_result");
	auto panel_exp = panel_result->getChildByName<Widget*>("panel_silver");
	auto panel_coin = panel_result->getChildByName<Widget*>("panel_coin");
	Widget* image_div_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "image_div_1"));
	listviewResult->refreshView();
	//fame
	Widget*panel_fame_clone = (Widget*)panel_coin->clone();
	auto image_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_fame_clone, "image_silver"));
	image_silver->loadTexture("cocosstudio/login_ui/common/reputation_2.png");
	auto label_buy_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_fame_clone, "label_buy_num"));
	label_buy_num->setString(StringUtils::format("+%d", addFame));
	label_buy_num->setTextColor(Color4B(46, 125, 50, 255));
	//exp
	Widget* panel_exp_clone = (Widget*)panel_exp->clone();
	auto label_expAdd = panel_exp_clone->getChildByName<Text*>("label_buy_num");
	auto label_force_relation = panel_exp_clone->getChildByName<Text*>("label_force_relation");
	label_force_relation->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_FINDNEWCITY_REWARD_EXP"]);
	label_expAdd->setString(StringUtils::format("+%d", addLevelexp));
	label_expAdd->setTextColor(Color4B(46, 125, 50, 255));
	auto image_div_line = (Widget*)image_div_1->clone();
	auto image_div_line_2 = (Widget*)image_div_1->clone();
	

	listviewResult->removeAllChildrenWithCleanup(true);
	listviewResult->pushBackCustomItem(panel_exp_clone);
	listviewResult->pushBackCustomItem(image_div_line);
	listviewResult->pushBackCustomItem(panel_fame_clone);
	listviewResult->pushBackCustomItem(image_div_line_2);
	listviewResult->setVisible(true);
	listviewResult->refreshView();
	listviewResult->setCameraMask(4, true);
	listviewResult->setSubNodeMaxCameraMask(4);
}

void UIGuideSailHUD::GuildmapNameAction()
{
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_SAILLING_MAIN_CSB]);
	if (view)
	{
		auto nameBg = view->getChildByName<ImageView*>("image_cityname_bg");
		nameBg->stopAllActions();
		nameBg->setPositionX(0 - nameBg->getContentSize().width / 2);
		nameBg->setOpacity(0);
		nameBg->setPositionX(nameBg->getPositionX() - nameBg->getContentSize().width / 2);
		auto action_1 = MoveBy::create(1, Vec2(nameBg->getContentSize().width, 0));
		auto action_2 = Sequence::create(DelayTime::create(0.5), FadeIn::create(1), DelayTime::create(3),
			Spawn::createWithTwoActions(MoveBy::create(1.5, Vec2(-nameBg->getContentSize().width, 0)), FadeOut::create(1))
			, nullptr
			);
		auto spw_action = Spawn::createWithTwoActions(action_1, action_2);
		nameBg->runAction(spw_action);
	}
}
