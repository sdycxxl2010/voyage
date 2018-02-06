#include "UINationWarLand.h"
#include "SystemVar.h"
#include "ProtocolThread.h"
#include "UITips.h"
#include "TVSingle.h"

#include "UIInform.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "UIMain.h"
#include "TVSceneLoader.h"
#include "UISailHUD.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UIStore.h"
#include "Utils.h"
#include "UINationWarStatus.h"
#include "TimeUtil.h"

USING_NS_CC;
using namespace rapidjson;
using namespace cocostudio;
UINationWarLand::UINationWarLand():
m_pResultCityStatus(nullptr)
{
	m_pResult_fleet = nullptr;
	m_pTempButton2 = nullptr;
	m_pFastHireCrewResult = nullptr;
	m_pResultNationWarEntrance = nullptr;
	m_pFallen = nullptr;
	m_nShipRepairIndex = NONE;
	m_nNomalRepairNum = 0;
	m_nFastHireCrewNum = 0;
	m_nWarTime = 0;
	m_nConfirm = 0;
	m_bIsFirstInit = false;
};
UINationWarLand::~UINationWarLand()
{
	this->unschedule(schedule_selector(UINationWarLand::everySecondReflush));
	this->unschedule(schedule_selector(UINationWarLand::updateWarNumber));
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}

UINationWarLand*UINationWarLand::createCountryWarLand(GetCityStatusResult* result)
{
	auto warLand = new UINationWarLand;
	if (warLand)
	{
		warLand->m_pResultCityStatus = result;
		if (warLand->init())
		{
			warLand->autorelease();
			
			return warLand;
		}
	}
	CC_SAFE_DELETE(warLand);
	return nullptr;
}
bool UINationWarLand::init()
{
	bool pRet = false;
	do 
	{
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UINationWarLand::onServerEvent, this), this);
		ProtocolThread::GetInstance()->getNationWarEntrance(UINationWarLand::m_pResultCityStatus->cityid);
		initStatic(0);
		pRet = true;
	} while (0);
	return pRet;
}
void UINationWarLand::initStatic(float f)
{
	openView(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_CSB],1);
	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_CSB]);
	view->setCameraMask(4, true);
	view->setTouchEnabled(true);

	this->schedule(schedule_selector(UINationWarLand::everySecondReflush), 1.0f);
	this->schedule(schedule_selector(UINationWarLand::updateWarNumber), 3);

	auto image_bg = view->getChildByName<ImageView*>("image_bg");
	auto t_city_name = view->getChildByName<Text*>("label_title");

	auto l_content = view->getChildByName<ListView*>("listview_content");
	auto p_city = dynamic_cast<Widget*>(l_content->getItem(0));
	auto p_durable = p_city->getChildByName<Widget*>("panel_durable");
	auto t_durable_num = p_durable->getChildByName<Text*>("label_derable_num");
	auto i_progressbar_lever = p_durable->getChildByName<ImageView*>("image_progressbar_lever");
	auto progressbar_lever = i_progressbar_lever->getChildByName<LoadingBar*>("progressbar_lever");
	auto t_gun_number = dynamic_cast<Text*>(Helper::seekWidgetByName(p_city, "label_gun_number"));
	auto p_intensify_du = p_durable->getChildByName<Widget*>("panel_intensify_du");
	auto p_intensify_gun = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_city,"panel_intensify_gun"));

	auto p_status = dynamic_cast<Widget*>(l_content->getItem(2));
	auto i_country = p_status->getChildByName<ImageView*>("image_country");
	auto i_country_0 = p_status->getChildByName<ImageView*>("image_country_0");
	auto t_war_count_down = p_status->getChildByName<Text*>("label_event_0");

	auto p_depot = dynamic_cast<Widget*>(l_content->getItem(4));
	auto t_depot_desc = p_depot->getChildByName<Text*>("labe_depot_content");

	auto p_rules = dynamic_cast<Widget*>(l_content->getItem(6));
	auto t_rules_title = p_rules->getChildByName<Text*>("labe_tules");
	auto t_rules_content = p_rules->getChildByName<Text*>("labe_content");

	//国战倒计时
	t_war_count_down->setString("00:00:00");
	i_country->ignoreContentAdaptWithSize(false);
	t_city_name->setString(SINGLE_SHOP->getCitiesInfo()[m_pResultCityStatus->cityid].name);
	//城市耐久
	t_durable_num->setString("0 / 0");
	t_durable_num->setFontSize(t_durable_num->getFontSize() - 1);
	//城市耐久比例
	progressbar_lever->setPercent(100 * 1);
	//城市攻击力
	t_gun_number->setString("0");
	//暂时隐藏星级显示
	p_intensify_du->setVisible(false);
	p_intensify_gun->setVisible(false);

	t_depot_desc->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAIL_WAR_LAND_DEPOT_DESC"]);
	t_rules_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAIL_WAR_LAND_ATTACK_CITY_TITLE"]);
	t_rules_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAIL_WAR_LAND_ATTACK_CITY_DESC"]);
	auto tHeight = getLabelHight(t_rules_content->getString(), t_rules_content->getBoundingBox().size.width, t_rules_content->getFontName(), t_rules_content->getFontSize());
	t_rules_content->setContentSize(Size(t_rules_content->getContentSize().width, tHeight));
	t_rules_title->setPositionY(t_rules_title->getBoundingBox().size.height + t_rules_content->getBoundingBox().size.height + 5);
	t_rules_content->setPositionY(t_rules_content->getBoundingBox().size.height);
	p_rules->setContentSize(Size(p_rules->getContentSize().width, (t_rules_title->getContentSize().height + 30 + t_rules_content->getBoundingBox().size.height)));

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	image_pulldown->setVisible(false);

	auto button_close = view->getChildByName<Button*>("button_close");
	button_close->addTouchEventListener(CC_CALLBACK_2(UINationWarLand::menuCall_func,this));
	button_close->setName("b_close_land");
	auto button_status = p_status->getChildByName<Button*>("button_status");
	button_status->addTouchEventListener(CC_CALLBACK_2(UINationWarLand::menuCall_func, this));
	auto button_depot_enter = p_depot->getChildByName<Button*>("button_depot_enter");
	button_depot_enter->addTouchEventListener(CC_CALLBACK_2(UINationWarLand::menuCall_func, this));
	button_depot_enter->setBright(false);
	auto button_start = view->getChildByName<Button*>("button_start");
	button_start->addTouchEventListener(CC_CALLBACK_2(UINationWarLand::menuCall_func, this));
}
//刷新界面 
void UINationWarLand::everySecondReflush(const float fTime)
{
	if (m_nWarTime > -1)
	{
		if (m_nWarTime <= 0)
		{
			//国战结束
			this->unschedule(schedule_selector(UINationWarLand::everySecondReflush));
			this->unschedule(schedule_selector(UINationWarLand::updateWarNumber));
			m_nConfirm = 1;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COUNTRY_WAR_TIME_OVER");
		}
		else
		{
			auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_CSB]);
			int64_t myTime = m_nWarTime;
			m_nWarTime--;
			if (view)
			{
				auto t_war_count_down = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_event_0"));
				t_war_count_down->setString(timeUtil(myTime, TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_GM));
			}
		}
	}
}
void UINationWarLand::initWarMessage()
{
	openView(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_CSB], 1);
	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_CSB]);
	auto l_content = view->getChildByName<ListView*>("listview_content");
	auto i_country = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_country"));
	auto i_country_0 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_country_0"));
	auto button_depot_enter = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_depot_enter"));
	auto button_start = view->getChildByName<Button*>("button_start");
	button_start->setTouchEnabled(true);
	i_country->loadTexture(getCountryIconPath(m_pResultNationWarEntrance->fight_nations[0]->nation1));
	i_country_0->loadTexture(getCountryIconPath(m_pResultNationWarEntrance->fight_nations[0]->nation2));

	if (m_pResultNationWarEntrance->current_hp > 0)
	{
		auto t_durable_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_derable_num"));
		auto progressbar_lever = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view, "progressbar_lever"));
		auto t_gun_number = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_gun_number"));
		auto i_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_icon_3"));
		Sprite*spriteAnimate = Sprite::create();
		i_skill->addChild(spriteAnimate);
		spriteAnimate->setPosition(Vec2(i_skill->getContentSize().width / 2, i_skill->getContentSize().height / 2));
		spriteAnimate->setContentSize(Size(90, 90));

		//城市耐久比例 间隔三秒实时刷新
		t_durable_num->setString(StringUtils::format("%lld / %lld", m_pResultNationWarEntrance->current_hp, m_pResultNationWarEntrance->max_hp));
		if (m_pResultNationWarEntrance->current_hp && m_pResultNationWarEntrance->max_hp)
		{
			progressbar_lever->setPercent(100 * m_pResultNationWarEntrance->current_hp / m_pResultNationWarEntrance->max_hp);
		}
		//城市攻击力
		int64_t attackNum;
		if (m_pResultNationWarEntrance->extra_attack > 0)
		{
			attackNum = m_pResultNationWarEntrance->attack + m_pResultNationWarEntrance->extra_attack;
			i_skill->setVisible(true);
			

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
			spriteAnimate->runAction(RepeatForever::create(Sequence::create(FadeIn::create(0.5f), animateEff, FadeOut::create(0.5f), DelayTime::create(3.0f), nullptr)));

			t_gun_number->setTextColor(Color4B(28, 137, 25, 255));
		}
		else
		{
			attackNum = m_pResultNationWarEntrance->attack;
			i_skill->setVisible(false);
			spriteAnimate->stopAllActions();
			spriteAnimate->removeFromParentAndCleanup(true);
			t_gun_number->setTextColor(Color4B(46, 29, 14, 255));
		}
		t_gun_number->setString(StringUtils::format("%lld", attackNum));

		if (m_pResultNationWarEntrance->can_depot)
		{
			button_depot_enter->setBright(true);
		}
		else
		{
			button_depot_enter->setBright(false);
		}
		button_start->setBright(true);
	}
	else
	{
		//城市已沦陷
		auto p_city = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_city"));
		auto p_fallen = dynamic_cast<Widget*>(Helper::seekWidgetByName(l_content, "panel_fallen"));
		if (p_city)
		{
			l_content->removeItem(0);
		}
		if (!p_fallen)
		{
			auto p_fallen_0 = view->getChildByName<Widget*>("panel_fallen");
			auto p_fallen = p_fallen_0->clone();
			p_fallen->setCameraMask(4, true);
			l_content->insertCustomItem(p_fallen, 0);
		}
		button_depot_enter->setBright(false);
		button_start->setBright(false);
		button_start->setTouchEnabled(false);
	}
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_content, image_pulldown);
}

void UINationWarLand::updateWarNumber(const float fTime)
{
	ProtocolThread::GetInstance()->getNationWarEntrance(m_pResultCityStatus->cityid);
}
void UINationWarLand::flushWarMessage()
{
	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_CSB]);
	auto l_content = view->getChildByName<ListView*>("listview_content");
	if (m_pResultNationWarEntrance->current_hp > 0)
	{
		auto t_durable_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_derable_num"));
		auto progressbar_lever = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view, "progressbar_lever"));
		auto t_gun_number = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_gun_number"));
		auto button_depot_enter = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_depot_enter"));
		auto i_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_icon_3"));
		Sprite*spriteAnimate = Sprite::create();
		i_skill->addChild(spriteAnimate);
		spriteAnimate->setPosition(Vec2(i_skill->getContentSize().width / 2, i_skill->getContentSize().height / 2));
		spriteAnimate->setContentSize(Size(90, 90));

		//城市耐久比例 间隔三秒实时刷新
		t_durable_num->setString(StringUtils::format("%lld / %lld", m_pResultNationWarEntrance->current_hp, m_pResultNationWarEntrance->max_hp));
		if (m_pResultNationWarEntrance->current_hp && m_pResultNationWarEntrance->max_hp)
		{
			progressbar_lever->setPercent(100 * m_pResultNationWarEntrance->current_hp / m_pResultNationWarEntrance->max_hp);
		}
		//城市攻击力
		int64_t attackNum;
		if (m_pResultNationWarEntrance->extra_attack > 0)
		{
			attackNum = m_pResultNationWarEntrance->attack + m_pResultNationWarEntrance->extra_attack;
			i_skill->setVisible(true);


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
			spriteAnimate->runAction(RepeatForever::create(Sequence::create(FadeIn::create(0.5f), animateEff, FadeOut::create(0.5f), DelayTime::create(3.0f), nullptr)));

			t_gun_number->setTextColor(Color4B(28, 137, 25, 255));
		}
		else
		{
			attackNum = m_pResultNationWarEntrance->attack;
			i_skill->setVisible(false);
			spriteAnimate->stopAllActions();
			spriteAnimate->removeFromParentAndCleanup(true);
			t_gun_number->setTextColor(Color4B(46, 29, 14, 255));
		}
		t_gun_number->setString(StringUtils::format("%lld", attackNum));

		if (m_pResultNationWarEntrance->can_depot)
		{
			button_depot_enter->setBright(true);
		}
		else
		{
			button_depot_enter->setBright(false);
		}
	}
	else
	{
		//城市已沦陷
		auto button_depot_enter = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_depot_enter"));
		auto button_start = view->getChildByName<Button*>("button_start");
		auto p_city = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_city"));
		auto p_fallen = dynamic_cast<Widget*>(Helper::seekWidgetByName(l_content, "panel_fallen"));
		if (p_city)
		{
			l_content->removeItem(0);
		}
		if (!p_fallen)
		{
			auto p_fallen_0 = view->getChildByName<Widget*>("panel_fallen");
			auto p_fallen = p_fallen_0->clone();
			p_fallen->setCameraMask(4, true);
			l_content->insertCustomItem(p_fallen, 0);
		}
		button_depot_enter->setBright(false);
		button_start->setBright(false);
		button_start->setTouchEnabled(false);
	}
}
void UINationWarLand::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		auto button = dynamic_cast<Widget*>(pSender);
		std::string name = button->getName();
		buttonEvent(button, name);
	}
}
void UINationWarLand::buttonEvent(Widget* target, std::string name, bool isRefresh)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

	//国战登陆相关的关闭
	if (isButton(b_close_land))
	{
		dynamic_cast<UISailHUD*>(_parent)->m_bIsBlocking = false;
		dynamic_cast<UISailHUD*>(_parent)->shipPause(false);
		dynamic_cast<UISailHUD*>(_parent)->m_bIsWar = false;
		closeView();
		this->removeFromParentAndCleanup(true);
		return;
	}

	if (isButton(button_close)||isButton(button_back) || isButton(button_Supply_no))
	{
		closeView();
		return;
	}

	if (isButton(button_error_yes))
	{
		if (m_nConfirm)
		{
			dynamic_cast<UISailHUD*>(_parent)->m_bIsBlocking = false;
			dynamic_cast<UISailHUD*>(_parent)->shipPause(false);
			dynamic_cast<UISailHUD*>(_parent)->m_bIsWar = false;
			closeView(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_CSB]);
			this->removeFromParentAndCleanup(true);
		}
		return;
	}

	if (isButton(button_status))
	{
		auto layer = UINationWarStatus::createCountryWarStatus();
		this->addChild(layer,100);
		return;
	}

	if (isButton(button_start))
	{
		//TODO 进入国战
		SINGLE_HERO->m_nAttackCityId = m_pResultCityStatus->cityid;
		CHANGETO(SCENE_TAG::COUNTRY_WAR_TAG);
		return;
	}

	//国战海上登陆打开补给界面
	if (isButton(button_depot_enter))
	{
		if (m_pResultNationWarEntrance)
		{
			if (m_pResultNationWarEntrance->current_hp > 0)
			{
				if (m_pResultNationWarEntrance->can_depot)
				{
					ProtocolThread::GetInstance()->reachDepot(UILoadingIndicator::createWithMask(this, 4));
				}
				else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_SAIL_WAR_LAND_NO_DEPOT_DESC");
				}
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SAIL_WAR_LAND_DEPOT_CLOSE_DESC");
			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_SAIL_WAR_LAND_NO_DEPOT_DESC");
		}
		return;
	}

	//国战海上登陆打开直接修理界面
	if (isButton(button_fleet_repair))
	{
		initRepairView();
		return;
	}

	//国战海上登陆打开直接雇佣水手界面
	if (isButton(button_sailor_result))
	{
		ProtocolThread::GetInstance()->getAvailCrewNum(UILoadingIndicator::createWithMask(this, 4));
		return;
	}

	//高级修理按钮
	if (isButton(button_repair_v))
	{
		m_pTempButton2 = target;
		m_nShipRepairIndex = REPAIR_GOLD;
		updateRepairShipDialog();
		return;
	}
	//普通修理按钮
	if (isButton(button_repair))
	{
		for (auto i = 0; i < m_pResult_fleet->n_fleetships + m_pResult_fleet->n_catchships; i++)
		{
			ShipDefine* sd;
			if (i < m_pResult_fleet->n_fleetships)
			{
				sd = m_pResult_fleet->fleetships[i];
			}
			else
			{
				sd = m_pResult_fleet->catchships[i - m_pResult_fleet->n_fleetships];
			}
			if (sd->hp < sd->current_hp_max)
			{
				m_nShipRepairIndex = REPAIR_COIN;
				updateRepairShipDialog();
				return;
			}
		}

		int n = 0;

		for (auto i = 0; i < m_pResult_fleet->n_fleetships + m_pResult_fleet->n_catchships; i++)
		{
			ShipDefine* sd;
			if (i < m_pResult_fleet->n_fleetships)
			{
				sd = m_pResult_fleet->fleetships[i];
			}
			else
			{
				sd = m_pResult_fleet->catchships[i - m_pResult_fleet->n_fleetships];
			}
			if (sd->current_hp_max < sd->hp_max)
			{
				n++;
			}
		}

		if (n > 1)
		{
			//m_nConfirmIndex = CONFIRM_INDEX_REPAIR_SHIP;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_DOCK_SHIP_REPAIR_COST_TITLE", "TIP_DOCK_REPAIR_ALL_SHIP");
		}
		else if (n == 1)
		{
			m_nShipRepairIndex = REPAIR_GOLDS;
			updateRepairShipDialog();
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_DOCK_NOT_REPAIR_SHIP");
		}
		return;
	}
	//银币修理
	if (isButton(button_s_yes))
	{
		ProtocolThread::GetInstance()->repairAllShips(0, UILoadingIndicator::create(this));
		return;
	}
	//v票修理
	if (isButton(button_v_yes))
	{
		if (m_nShipRepairIndex == REPAIR_GOLD)
		{
			int id;
			if (m_pTempButton2->getTag() - START_INDEX < m_pResult_fleet->n_fleetships)
			{
				id = m_pResult_fleet->fleetships[m_pTempButton2->getTag() - START_INDEX]->id;
			}
			else
			{
				id = m_pResult_fleet->catchships[m_pTempButton2->getTag() - START_INDEX - m_pResult_fleet->n_fleetships]->id;
			}

			ProtocolThread::GetInstance()->repairShipMaxHP(id, UILoadingIndicator::create(this));
		}
		else
		{
			ProtocolThread::GetInstance()->repairShipMaxHP(0, UILoadingIndicator::create(this));
		}
		return;
	}

	//修理提示
	if (isButton(button_confirm_yes))
	{
		if (m_nShipRepairIndex == REPAIR_NOT_GOLDS)
		{
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
		}
		else
		{
			m_pTempButton2 = nullptr;
			m_nShipRepairIndex = REPAIR_GOLDS;
			updateRepairShipDialog();
		}
		return;
	}

	if (isButton(button_s_no) || isButton(button_v_no) || isButton(button_confirm_no))
	{
		return;
	}
	
	//国战登陆补给站修理界面船只详情
	if (isButton(button_good_bg_1))
	{
		ShipDefine *shipInfo;
		if (target->getTag() - START_INDEX < m_pResult_fleet->n_fleetships)
		{
			if (m_pResult_fleet->n_fleetships)
			{
				shipInfo = m_pResult_fleet->fleetships[target->getTag() - START_INDEX];
			}
		}
		else
		{
			if (m_pResult_fleet->n_catchships)
			{
				shipInfo = m_pResult_fleet->catchships[target->getTag() - START_INDEX - m_pResult_fleet->n_fleetships];
			}
		}
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushShipDetail(shipInfo, shipInfo->sid, true);
		return;
	}

	//补给界面的确定按钮
	if (isButton(button_Supply_yes))
	{
		closeView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
		if (m_nFastHireCrewNum > 0)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			ProtocolThread::GetInstance()->getCrew(m_nFastHireCrewNum, UILoadingIndicator::create(this), 1);
		}
		return;
	}
}
void UINationWarLand::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	switch (msgType)
	{
	// 国战登陆相关信息
	case PROTO_TYPE_GetNationWarEntranceResult:
	{
		m_pResultNationWarEntrance = (GetNationWarEntranceResult*)message;
		//log("extraattack = %lld attack = %lld", m_pResultNationWarEntrance->extra_attack, m_pResultNationWarEntrance->attack);
		if (m_pResultNationWarEntrance->failed == 0)
		{
			m_nWarTime = m_pResultNationWarEntrance->fight_nations[0]->dis_to_end;
			if (!m_bIsFirstInit)
			{
				m_bIsFirstInit = true;
				initWarMessage();
			}
			flushWarMessage();
		}
		else
		{
			//进入港口失败，请重试（错误代码 3）.
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SAIL_WAR_LAND_ERROR_THREE");
		}

		break;
	}
	// 补给站
	case PROTO_TYPE_ReachDepotResult:
	{
		ReachDepotResult *result = (ReachDepotResult *)message;
		if (result->failed == 0)
		{
			initDepotView();
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SAIL_WAR_LAND_DEPOT_DATA_ERROR");
		}

		break;
	}
	// 获取舰队和船坞中船只船长装备的信息
	case PROTO_TYPE_GetFleetAndDockShipsResult:
	{
		m_pResult_fleet = (GetFleetAndDockShipsResult*)message;
		if (m_pResult_fleet->failed == 0)
		{
			flushWarPanelFleetInfo(m_pResult_fleet);
		}
		else
		{
			openSuccessOrFailDialog("TIP_DOCK_GET_FLEET_FAIL");
		}

		break;
	}
	//修复所有船只耐久到当前最大耐久
	case PROTO_TYPE_RepairAllShipsResult:
	{
		RepairAllShipsResult *result = (RepairAllShipsResult *)message;
		if (result->failed == 0)
		{
			flushCoinInfo(result->coin, result->gold);
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
	//维修舰队中的船只, 使用V票, 修复船只的最大耐久. shipId : 船只id(0为所有船只)
	case PROTO_TYPE_RepairShipResult:
	{
		RepairShipResult *result = (RepairShipResult *)message;

		if (result->failed == 0)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_DOCK_REPAIR_SHIP_SUCCESS");
			flushCoinInfo(result->coin, result->gold);
			ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::create(this));
			Utils::consumeVTicket("4", 1, result->cost);
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
	//获取可以招募的水手
	case PROTO_TYPE_GetAvailCrewNumResult:
	{
		GetAvailCrewNumResult* result = (GetAvailCrewNumResult *)message;
		switch (result->failed)
		{
		case 0:
		{
				if (result->currentcrewnum == result->maxcrewnum)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_PUP_CREW_FULL");
				}
				else
				{
					ProtocolThread::GetInstance()->calFastHireCrew(UILoadingIndicator::create(this));
				}
				break;
		}
		case COIN_NOT_FAIL:
		{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
				break;
		}
		case 3:
		{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PUP_HAVENOSHIP_HIRE_CREWS");
				break;
		}
		default:
		{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PUP_CREW_FULL");
				break;
		}
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
			if (SINGLE_HERO->m_iCoin < 1100 && SINGLE_HERO->m_iCoin >= 600)
			{
				UIInform::getInstance()->openConfirmYes("TIP_MAINCITY_SAILOR_HIRE_COIN_VERY_LITTLE");
				break;
			}
			else if (SINGLE_HERO->m_iCoin < 600)
			{
				UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
				break;
			}
			initHireSalorView();
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_PUP_BUY_CREW_FAIL");
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
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_PUP_SAILOR_RECRUIT_SUCCESS", 1);
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_PUP_BUY_CREW_FAIL");
		}

		break;
	}
	default:
		break;
	}
}
/*
*打开补给界面
*/
void UINationWarLand::initDepotView()
{
	//国战登陆补给界面
	openView(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_DEPOT_CSB],2);
	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_DEPOT_CSB]);
	view->setCameraMask(4, true);
	view->setTouchEnabled(true);
	auto t_choose = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "labe_choose"));
	t_choose->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAIL_WAR_LAND_DEPOT_CHOSE_DESC"]);

	auto button_back = view->getChildByName<Button*>("button_back");
	button_back->addTouchEventListener(CC_CALLBACK_2(UINationWarLand::menuCall_func, this));
	auto button_fleet_repair = view->getChildByName<Button*>("button_fleet_repair");
	button_fleet_repair->addTouchEventListener(CC_CALLBACK_2(UINationWarLand::menuCall_func, this));
	auto button_sailor_result = view->getChildByName<Button*>("button_sailor_result");
	button_sailor_result->addTouchEventListener(CC_CALLBACK_2(UINationWarLand::menuCall_func, this));
}
/*
*打开修理界面
*/
void UINationWarLand::initRepairView()
{
	openView(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_DEPOT_REPAIR_CSB], 3);
	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_DEPOT_REPAIR_CSB]);
	view->setCameraMask(4, true);
	view->setTouchEnabled(true);

	auto button_close = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_close"));
	button_close->setTouchEnabled(true);
	button_close->addTouchEventListener(CC_CALLBACK_2(UINationWarLand::menuCall_func, this));
	auto button_repair = view->getChildByName<Button*>("button_repair");
	button_repair->addTouchEventListener(CC_CALLBACK_2(UINationWarLand::menuCall_func, this));
	auto button_repair_v = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_repair_v"));
	button_repair_v->addTouchEventListener(CC_CALLBACK_2(UINationWarLand::menuCall_func, this));

	flushCoinInfo(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
	ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::createWithMask(this, 4));
}
/*
*打开雇佣水手界面
*/
void UINationWarLand::initHireSalorView()
{
	if (!m_pFastHireCrewResult)
	{
		return;
	}
	openView(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB],4);
	auto supplyView = getViewRoot(DOCK_COCOS_RES[VIEW_BUY_SUPPLY_CSB]);
	supplyView->setCameraMask(4, true);
	auto slider_dropitem_num = supplyView->getChildByName<Slider*>("slider_dropitem_num");
	slider_dropitem_num->addEventListenerSlider(this, sliderpercentchangedselector(UINationWarLand::fastHireCrewSliderChange));
	auto label_title = dynamic_cast<Text*>(Helper::seekWidgetByName(supplyView, "label_title"));
	label_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_SAILOR_QUICH_HIRE_BUTTON_CONTENT"]);

	auto image_supply = dynamic_cast<ImageView*>(supplyView->getChildByName("image_supply"));
	image_supply->loadTexture("cocosstudio/login_ui/common/sailor.png");

	auto image_progressbar_supply = image_supply->getChildByName("image_progressbar_supply");
	auto progressbar_supply = image_progressbar_supply->getChildByName<LoadingBar*>("progressbar_supply");
	progressbar_supply->setVisible(false);
	auto label_ship_supply_num_1 = image_supply->getChildByName<Text*>("label_ship_supply_num_1");
	label_ship_supply_num_1->setString(StringUtils::format("%d", m_pFastHireCrewResult->availmaxcrewnum));
	auto label_ship_supply_num_2 = image_supply->getChildByName<Text*>("label_ship_supply_num_2");
	label_ship_supply_num_2->setString(StringUtils::format("/%d", m_pFastHireCrewResult->availmaxcrewnum));
	label_ship_supply_num_2->setPositionX(label_ship_supply_num_1->getPositionX() + label_ship_supply_num_1->getContentSize().width );
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
//水手快速补给滑动条事件
void UINationWarLand::fastHireCrewSliderChange(Ref* obj, cocos2d::ui::SliderEventType type)
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
	label_ship_supply_num_2->setString(StringUtils::format("/%d",m_pFastHireCrewResult->availmaxcrewnum));
	label_ship_supply_num_2->setPositionX(label_ship_supply_num_1->getPositionX() + label_ship_supply_num_1->getContentSize().width);
}
/*
*更新银币和v票并刷新界面
*参数 gold:v票个数   coin:银币个数
*/
void UINationWarLand::flushCoinInfo(int64_t coin, int64_t gold)
{
	SINGLE_HERO->m_iCoin = coin;
	SINGLE_HERO->m_iGold = gold;
	Text* t_silver = nullptr;
	Text* t_gold = nullptr;

	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_DEPOT_REPAIR_CSB]);

	t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_silver_num"));
	t_gold = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_gold_num"));
	t_silver->setString(numSegment(StringUtils::format("%lld", coin)));
	t_gold->setString(numSegment(StringUtils::format("%lld", gold)));
}
/*
*国战修理时舰队信息显示
*result
*/
void UINationWarLand::flushWarPanelFleetInfo(GetFleetAndDockShipsResult* result)
{
	//补给站修理界面

	auto view = getViewRoot(MAPUI_COCOS_RES[COUNTRY_WAR_SEA_LAND_DEPOT_REPAIR_CSB]);
	auto p_ship_1 = view->getChildByName<Widget*>("panel_ship_1");
	auto b_repair = view->getChildByName<Button*>("button_repair");
	auto l_ships = view->getChildByName<ListView*>("listview_fleet");
	l_ships->removeAllChildrenWithCleanup(true);
	l_ships->setSwallowTouches(false);
	p_ship_1->setVisible(false);
	m_nNomalRepairNum = 0;
	if (result->n_fleetships)
	{
		for (int i = 0; i < result->n_fleetships + result->n_catchships; ++i)
		{
			ShipDefine* fleetShip = nullptr;
			if (i < result->n_fleetships)
			{
				fleetShip = result->fleetships[i];
			}
			else
			{
				fleetShip = result->catchships[i - result->n_fleetships];
			}
			Widget* w_clone = dynamic_cast<Widget*>(p_ship_1->clone());
			w_clone->setVisible(true);
			w_clone->setTag(i);
			initWarFleetShipInfo(w_clone, fleetShip, i);
			l_ships->pushBackCustomItem(w_clone);
			w_clone->setCameraMask(4, true);
		}
	}

	if (m_nNomalRepairNum > 0)
	{
		b_repair->setBright(true);
		b_repair->setTouchEnabled(true);
	}
	else
	{
		b_repair->setBright(false);
		b_repair->setTouchEnabled(false);
	}
}
/*
*初始化国战期间登陆时修理界面舰队每条船的信息
*item:需要加载船只信息的按钮
*shipDefine:船只信息
*tag:船的编号
*/
void UINationWarLand::initWarFleetShipInfo(Widget* item, ShipDefine* shipDefine, int tag)
{
	auto b_good_bg_1 = item->getChildByName<Button*>("button_good_bg_1");
	auto i_ship = b_good_bg_1->getChildByName<ImageView*>("image_ship");
	auto i_num = b_good_bg_1->getChildByName<ImageView*>("image_num");

	auto t_ship_name = item->getChildByName<Text*>("label_ship_name");
	auto l_ship_durable_num_1 = dynamic_cast<ListView*>(Helper::seekWidgetByName(item, "listview_ship_durable_num_1"));
	auto t_ship_durable_num_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_durable_num_1"));
	auto t_ship_durable_num_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_durable_num_2"));
	auto progressbar_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(item, "progressbar_durable"));
	auto i_durable = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_82"));

	auto b_repair_v = item->getChildByName<Button*>("button_repair_v");
	b_good_bg_1->setTag(START_INDEX + tag);

	std::string filePath = getShipIconPath(shipDefine->sid);
	i_ship->loadTexture(filePath);

	//稀有度
	setBgButtonFormIdAndType(b_good_bg_1, shipDefine->sid, ITEM_TYPE_SHIP);
	setTextColorFormIdAndType(t_ship_name, shipDefine->sid, ITEM_TYPE_SHIP);

	t_ship_durable_num_1->setString("");
	t_ship_durable_num_2->setString("");
	l_ship_durable_num_1->refreshView();

	if (shipDefine->user_define_name == nullptr || std::strcmp(shipDefine->user_define_name, "") == 0)
	{
		std::string captainName = getShipName(shipDefine->sid);
		t_ship_name->setString(captainName);
	}
	else
	{
		t_ship_name->setString(String::createWithFormat("%s", shipDefine->user_define_name)->_string);
	}

	if (shipDefine->hp > shipDefine->current_hp_max)
	{
		shipDefine->hp = shipDefine->current_hp_max;
	}

	float temp = 0;
	if (shipDefine->current_hp_max < shipDefine->hp_max)
	{   //not normal
		t_ship_durable_num_1->setString(StringUtils::format("%d", shipDefine->hp));
		t_ship_durable_num_2->setString(StringUtils::format(" / %d", shipDefine->current_hp_max));
		t_ship_durable_num_2->setTextColor(Color4B(198, 2, 5, 255));
		i_durable->setVisible(true);
		if (shipDefine->current_hp_max != 0)
		{
			temp = (shipDefine->hp*1.0) / (shipDefine->current_hp_max*1.0);
		}
		progressbar_durable->setPercent(87 * temp);
		b_repair_v->setBright(true);
		b_repair_v->setTag(tag + START_INDEX);
		b_repair_v->setTouchEnabled(true);
		m_nNomalRepairNum++;
	}
	else
	{
		t_ship_durable_num_1->setString(StringUtils::format("%d", shipDefine->hp));
		t_ship_durable_num_2->setString(StringUtils::format(" / %d", shipDefine->hp_max));
		t_ship_durable_num_2->setTextColor(Color4B(40, 25, 13, 255));
		i_durable->setVisible(false);
		if (shipDefine->hp_max != 0)
		{
			temp = (shipDefine->hp*1.0) / (shipDefine->hp_max*1.0);
		}
		if (temp < 1)
		{
			m_nNomalRepairNum++;
		}
		progressbar_durable->setPercent(100 * temp);
		b_repair_v->setBright(false);
		b_repair_v->setTouchEnabled(false);
	}
	if (tag < m_pResult_fleet->n_fleetships)
	{
		i_num->setVisible(true);
		i_num->loadTexture(getPositionIconPath(tag + 1));
	}
	else
	{
		i_num->setVisible(false);
	}

	l_ship_durable_num_1->refreshView();
}
/*
*修理花费界面提示
*/
void UINationWarLand::updateRepairShipDialog()
{
	float GOLD_REPAIR = m_pResult_fleet->shipyard_repair_ship_cost_gold;
	float COIN_REPAIR = m_pResult_fleet->shipyard_repair_ship_cost_coin;
	int coin = 0;
	if (m_nShipRepairIndex == REPAIR_COIN)
	{
		for (int i = 0; i < m_pResult_fleet->n_fleetships + m_pResult_fleet->n_catchships; i++)
		{
			ShipDefine* shipInfo;
			if (i <  m_pResult_fleet->n_fleetships)
			{
				shipInfo = m_pResult_fleet->fleetships[i];
			}
			else
			{
				shipInfo = m_pResult_fleet->catchships[i - m_pResult_fleet->n_fleetships];
			}
			coin += shipInfo->current_hp_max - shipInfo->hp;
		}
		coin = (int)ceil(coin * COIN_REPAIR);

		if (SINGLE_HERO->m_iCoin < coin)
		{
			m_pTempButton2 = nullptr;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			return;
		}
	}
	else
	{
		if (m_nShipRepairIndex == REPAIR_GOLD)
		{
			if (m_pTempButton2)
			{
				int index = m_pTempButton2->getTag() - START_INDEX;
				ShipDefine* shipInfo;
				if (index <  m_pResult_fleet->n_fleetships)
				{
					shipInfo = m_pResult_fleet->fleetships[index];
				}
				else
				{
					shipInfo = m_pResult_fleet->catchships[index - m_pResult_fleet->n_fleetships];
				}
				coin += ceil((shipInfo->hp_max - shipInfo->current_hp_max) * GOLD_REPAIR * sqrt(SINGLE_SHOP->getShipData()[shipInfo->sid].rarity));
			}
		}
		else
		{

			for (int i = 0; i < m_pResult_fleet->n_fleetships + m_pResult_fleet->n_catchships; i++)
			{
				ShipDefine* shipInfo;
				if (i <  m_pResult_fleet->n_fleetships)
				{
					shipInfo = m_pResult_fleet->fleetships[i];
				}
				else
				{
					shipInfo = m_pResult_fleet->catchships[i - m_pResult_fleet->n_fleetships];
				}
				coin += ceil((shipInfo->hp_max - shipInfo->current_hp_max) * GOLD_REPAIR * sqrt(SINGLE_SHOP->getShipData()[shipInfo->sid].rarity));
			}
		}
		if (SINGLE_HERO->m_iGold < coin)
		{
			m_pTempButton2 = nullptr;
			m_nShipRepairIndex = REPAIR_NOT_GOLDS;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openGoldNotEnoughYesOrNo(coin);
			return;
		}
	}

	UICommon::getInstance()->openCommonView(this);
	if (m_nShipRepairIndex == REPAIR_COIN)
	{
		UICommon::getInstance()->flushSilverConfirmView("TIP_DOCK_SHIP_REPAIR_COST_TITLE", "TIP_DOCK_SHIP_REPAIR_COST_CONTENT", coin);
	}
	else
	{
		UICommon::getInstance()->flushVConfirmView("TIP_DOCK_SHIP_ADVREPAIR_COST_TITLE", "TIP_DOCK_SHIP_ADVREPAIR_COST_CONTENT", coin);
	}
}
