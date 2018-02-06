#include "UINationWarEntrance.h"
#include "ProtocolThread.h"
#include "UICommon.h"
#include "UIInform.h"

#include "UINationWarRanking.h"
#include "UIChatHint.h"
#include "TVSceneLoader.h"

UINationWarEntrance::UINationWarEntrance()
{
	m_pMainButton = nullptr;
	m_pView = nullptr;
	m_bAttackMoveActionFlag = true;
	m_padStirng = "";
	m_usingNumPad = M_USINGPAD_NONE;
	isOnWaring = true;
	m_UsingNumPadText = nullptr;
	m_PrepareResult = nullptr;
	m_Times = -1;
	m_repairAllTime = -1;
	m_addGunAllTime = -1;
	m_nMainbuttonTag = N_BUTTON_NONE;
	m_bChangePrepareToWar = false;
	m_loadingLayer = nullptr;

	m_vWigetInfo.clear();

}


UINationWarEntrance::~UINationWarEntrance()
{
	this->unschedule(schedule_selector(UINationWarEntrance::everySecondflush));
	this->unschedule(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds));
	unregisterCallBack();
	m_pMainButton = nullptr;
	m_pView = nullptr;
	m_UsingNumPadText = nullptr;
	m_vWigetInfo.clear();
}
UINationWarEntrance* UINationWarEntrance::createCountryWarLayer()
{
	auto layer= new UINationWarEntrance;
	if (layer && layer->init())
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}

bool UINationWarEntrance::init()
{
	bool pRet = false;
	do
	{
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UINationWarEntrance::onServerEvent, this), this);
		ProtocolThread::GetInstance()->getCityPrepareStatus(UILoadingIndicator::create(this,0));
		m_loadingLayer = UILoadingIndicator::create(this, 0);
		m_loadingLayer->showSelf();
		
		pRet = true;
	} while (0);
	return pRet;
}
void UINationWarEntrance::onEnter()
{
	UIBasicLayer::onEnter();
}

void UINationWarEntrance::onExit()
{
	UIBasicLayer::onExit();
}
void UINationWarEntrance::initStatic(float f)
{
	if (!m_PrepareResult)
	{
		return;
	}
	
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_CSB]);
	auto viewTwo = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_TWO_CSB]);
	if (!view&&!viewTwo)
	{
		openView(COUNTRY_WAR_RES[COUNTRY_WAR_CSB]);
		openView(COUNTRY_WAR_RES[COUNTRY_WAR_TWO_CSB]);
		view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_CSB]);
		viewTwo = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_TWO_CSB]);
	
		guardEnterScene(0);
		//聊天
		auto ch = UIChatHint::createHint();
		this->addChild(ch, 10);
		if (m_loadingLayer)
		{
			m_loadingLayer->removeSelf();
			m_loadingLayer = nullptr;
		}
	}

	auto panel_two_butter = view->getChildByName<Widget*>("panel_two_butter");
	panel_two_butter->setVisible(false);
	
	auto panel_actionbar = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewTwo,"panel_actionbar"));
	auto label_tavern = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_actionbar, "label_title"));

	auto t_timeTitle = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_actionbar, "label_ongoing"));
	auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_actionbar, "label_event_0"));
	label_tavern->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_TITLE"]);

	//btn text
	auto panel_two_butter_2 = viewTwo->getChildByName<Widget*>("panel_two_butter");
	auto button_report = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_two_butter_2, "button_report"));

	
	auto t_defend = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_two_butter_2, "label_defense"));
	auto t_rules = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_two_butter_2, "label_rules"));
	
	t_defend->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_MAIN_BTN_TEXT_DEFEND"]);
	t_rules->setString(SINGLE_SHOP->getTipsInfo()["TIP_ATTACT_PIRATE_BUTTON_RULES"]);
	auto image_time_start = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_actionbar, "image_time_start"));
	auto my_country = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_time_start, "image_country"));


	auto enemy_country = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_time_start, "image_country_1"));
	
	ImageView*other_country_clone = nullptr;
	//现在改为只能一对一
	for (int i = 0; i < m_PrepareResult->n_fight_nations; i++)
	{
		if (i==0)
		{
			other_country_clone = enemy_country;
		}
		else
		{
			other_country_clone = dynamic_cast<ImageView*>(enemy_country->clone());
			image_time_start->addChild(other_country_clone);
			
		}
		my_country->loadTexture(getCountryIconPath(m_PrepareResult->fight_nations[i]->nation1));
		other_country_clone->loadTexture(getCountryIconPath(m_PrepareResult->fight_nations[i]->nation2));
		other_country_clone->setPositionX(enemy_country->getPositionX() + 40 * i);
		
	}
	
	if (m_PrepareResult->is_in_war==2)//战斗阶段
	{
		button_report->setTouchEnabled(true);
		button_report->loadTextureNormal("cocosstudio/login_ui/palace_720/palace_btn_off.png");
		t_timeTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_ON_GOING_TITLE"]);
	}
	else if (m_PrepareResult->is_in_war == 1) //准备阶段
	{
		button_report->setTouchEnabled(false);
		button_report->loadTextureNormal("res/wharfRes/btn_off_black.png");
		t_timeTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_PREPARETIME_TITLE"]);
	}
	if (!isScheduled(schedule_selector(UINationWarEntrance::everySecondflush)))
	{
		everySecondflush(0);
		this->schedule(schedule_selector(UINationWarEntrance::everySecondflush), 1.0f);
	}
	
}
void UINationWarEntrance::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}
	auto button = dynamic_cast<Widget*>(pSender);
	std::string name = button->getName();
	auto inPutVorCoinsText = dynamic_cast<Text*>(pSender);
	if (!m_bAttackMoveActionFlag)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

	//防御准备
	if (isButton(button_defense))
	{
		m_bAttackMoveActionFlag = false;
		mainButtonChangeStatus(button);
		m_nMainbuttonTag = N_BUTTON_DEFENSE;
		if (!m_pView)
		{
			guardExitScene(0);
		}
		ProtocolThread::GetInstance()->getCityPrepareStatus(UILoadingIndicator::create(this));
	
		return;
	}
	//规则
	if (isButton(button_rules))
	{
		m_bAttackMoveActionFlag = false;
		m_nMainbuttonTag = N_BUTTON_RULES;
		mainButtonChangeStatus(button);
		if (!m_pView)
		{
			guardExitScene(0);
		}
		openRulesPanel();
		return;
	}
	//战斗日志
	if (isButton(button_report))
	{
			m_bAttackMoveActionFlag = false;
			m_nMainbuttonTag = N_BUTTON_STATUE;
			mainButtonChangeStatus(button);
			if (!m_pView)
			{
			guardExitScene(0);
			}
			ProtocolThread::GetInstance()->getNationWarCityLost(UILoadingIndicator::create(this));
		return;
	}
	//雇佣军
	if (isButton(label_funds_coins_num))
	{
		m_usingNumPad = M_INVEST_COIN;
		showNumpad(inPutVorCoinsText);
		return;
	}
	//建仓库
	if (isButton(label_v_num))
	{
		m_usingNumPad = M_INVEST_VTICKET;
		showNumpad(inPutVorCoinsText);
		return;
	}
	//升级耐久
	if (isButton(label_prepare_updatewall))
	{
		m_usingNumPad = M_UPGRADE_DURABLE;
		showNumpad(inPutVorCoinsText);
		return;
	}
	//升级火力
	if (isButton(label_prepare_updategun))
	{
		m_usingNumPad = M_UPGRADE_GUN;
		showNumpad(inPutVorCoinsText);
		return;
	}
	//排行
	if (isButton(button_ship_parts))
	{
		//openWarRanking();
		auto layer = UINationWarRanking::createCountryWarRanking();
		this->addChild(layer, 3);
		return;
	}
	//关闭
	if (isButton(button_close))
	{
		closeView();
		return;
	}
	//问号按钮—准备阶段雇佣海军
	if (isButton(button_prepare_attack_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_COUNTRY_WAR_REINFORCEMENTS_INFO_TITLE", "TIP_COUNTRY_WAR_REINFORCEMENTS_INFO_CONTENT");
		return;
	}
	//问号按钮—准备阶段仓库
	if (isButton(button_prepara_depot_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_COUNTRY_WAR_DEPOT_INFO_TITLE", "TIP_COUNTRY_WAR_DEPOT_INFO_CONTENT");
		return;
	}
	//问号按钮—战斗阶段修理耐久
	if (isButton(button_onwar_repairwall_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_COUNTRY_WAR_REPAIR_INFO_TITLE", "TIP_COUNTRY_WAR_REPAIR_INFO_CONTENT");
		return;
	}
	//问号按钮—战斗阶段火力
	if (isButton(button_onwar_fire_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_COUNTRY_WAR_SUPER_FIRE_INFO_TITLE", "TIP_COUNTRY_WAR_SUPER_FIRE_INFO_CONTENT");
		return;
	}
	//确定花费
	if (isButton(button_salvage_yes))
	{
		closeView();
		auto nums = atoll(m_padStirng.c_str());
		switch (m_usingNumPad)
		{
		case UINationWarEntrance::M_USINGPAD_NONE:
		break;
		case UINationWarEntrance::M_INVEST_COIN:
		{
			//雇佣军
			ProtocolThread::GetInstance()->warHiredArmy(SINGLE_HERO->m_iNation, nums, UILoadingIndicator::create(this));
			break;
		}
		case UINationWarEntrance::M_INVEST_VTICKET:
		{
			//建造仓库
			ProtocolThread::GetInstance()->buildcountryWarDepot(SINGLE_HERO->m_iNation, nums, UILoadingIndicator::create(this));
			break;
		}

		case UINationWarEntrance::M_UPGRADE_DURABLE:
		{
			//准备阶段升级耐久
			ProtocolThread::GetInstance()->addCityRepairPool(nums, UILoadingIndicator::create(this));
			break;
		}

		case UINationWarEntrance::M_UPGRADE_GUN:
		{
			//准备阶段升级火力
			ProtocolThread::GetInstance()->addCityAttackPool(nums, UILoadingIndicator::create(this));
			break;
		}

		case UINationWarEntrance::M_ONWARING_REPAIR_DURABLE_USE_COIN:
		{
			//战斗阶段银币修复
			ProtocolThread::GetInstance()->onWaringRepairDefense(0, nums, UILoadingIndicator::create(this));
			break;
		}

		case UINationWarEntrance::M_ONWARING_SUPER_FIRE:
		{
			//战斗阶段增强活力
			ProtocolThread::GetInstance()->onWaringEnhanceAttack(SINGLE_HERO->m_iCityID, nums, UILoadingIndicator::create(this));
			break;
		}

		default:
		break;
		}
		return;
	}
	//取消花费
	if (isButton(button_salvage_no))
	{
		closeView();
		return;
	}
	//回到王宫
	if (isButton(button_error_yes))
	{
		unregisterCallBack();
		CHANGETO(SCENE_TAG::PALACE_TAG);
		return;
	}

	//界面的返回按钮
	if (isButton(button_backcity))
	{
		button_callBack();
		return;
	}
}
void UINationWarEntrance::onServerEvent(struct ProtobufCMessage *message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetCityPrepareStatusResult:
	{
		GetCityPrepareStatusResult *result = (GetCityPrepareStatusResult*)message;
		if (result->failed == 0)
		{
		
			m_PrepareResult = result;
			flushPrepareOrWarTime(result);
			updateWarCoin(result->golds, result->coins);
			if (m_PrepareResult->current_hp <= 0)
			{
				m_PrepareResult->current_hp = 0;
				m_PrepareResult->can_add_hp_in_war = 0;
				if (isScheduled(schedule_selector(UINationWarEntrance::everySecondflush)))
				{
					this->unschedule(schedule_selector(UINationWarEntrance::everySecondflush));
				}
				if (isScheduled(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds)))
				{
					this->unschedule(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds));
				}
			}
			if (m_bChangePrepareToWar)
			{
				initStatic(0);
				if (result->is_in_war ==2)
				{
					m_bChangePrepareToWar = false;
				}	
			}
			switch (m_nMainbuttonTag)
			{
			case UINationWarEntrance::N_BUTTON_NONE:
			{
					initStatic(0);
					break;
			}		
			case UINationWarEntrance::N_BUTTON_DEFENSE:
			{
					if (m_PrepareResult->is_in_war==2)
					{
						m_repairAllTime = m_PrepareResult->dis_to_refresh_hp_pool;
						if (m_PrepareResult->power_attack_status==1)//正在使用超级火力
						{
							m_addGunAllTime = m_PrepareResult->dis_to_power_attack_end-300;
						}
						else if (m_PrepareResult->power_attack_status == 2)//超级火力冷却中
						{
							m_addGunAllTime = m_PrepareResult->dis_to_power_attack_end;
						}
						
						openOnGoingWarPanel();
					}
					else if (m_PrepareResult->is_in_war == 1)
					{
						openPrepareDefendPanel();
					}
					break;
			}		
			case UINationWarEntrance::N_BUTTON_STATUE:
			break;
			case UINationWarEntrance::N_BUTTON_RULES:
			break;
			default:
			break;
			}
			
			
		}

		break;
	}
	case PROTO_TYPE_HireArmyResult:
	{
		HireArmyResult *result = (HireArmyResult*)message;
		if (result->failed == 0)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_SUCCESS",2,result->obtain_score);
			m_PrepareResult->mercenary_pool += result->consume_coins;
			updateWarCoin(SINGLE_HERO->m_iGold, result->coins);
			openPrepareDefendPanel();
			
		}
		else if (result->failed == 5)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_COIN_NOT");
		}
		else if (result->failed ==20)//已满
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_HAVE_BEEN_FULL");
		}
		else if (result->failed == 4)//不在国战状态
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_PALACE_COUNTRY_YOU_NOT_IN_WAR");
		}
		break;
	}
	case PROTO_TYPE_AddSupplyStationResult:
	{
			AddSupplyStationResult *result = (AddSupplyStationResult*)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_SUCCESS",2,result->obtain_score);
				m_PrepareResult->depot_pool += result->consume_golds;
				m_PrepareResult->depot_pool = (m_PrepareResult->depot_pool >= m_PrepareResult->max_depot ?
					m_PrepareResult->max_depot : m_PrepareResult->depot_pool);
				updateWarCoin(result->golds, SINGLE_HERO->m_iCoin);
				openPrepareDefendPanel();
			}
			else if (result->failed == 5)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GOLD_NOT");
			}
			else if (result->failed == 20)//已满
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_HAVE_BEEN_FULL");
			}
			else if (result->failed == 4)//不在国战状态
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_PALACE_COUNTRY_YOU_NOT_IN_WAR");
			}
			break;
	}
	case PROTO_TYPE_AddCityRepairPoolResult:
	{
		    AddCityRepairPoolResult *result = (AddCityRepairPoolResult*)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_SUCCESS",2,result->obtain_score);
				flushPrepareDurable(result);
				updateWarCoin(SINGLE_HERO->m_iGold, result->coins);
			}
			else if (result->failed == 5)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COIN_NOT");
			}
			else if (result->failed == 20)//已满
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_HAVE_BEEN_FULL");
			}
			else if (result->failed == 4)//不在国战状态
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_PALACE_COUNTRY_YOU_NOT_IN_WAR");
			}
			break;
	}
	case PROTO_TYPE_AddCityAttackPoolResult:
	{
			AddCityAttackPoolResult *result = (AddCityAttackPoolResult*)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_SUCCESS",2,result->obtain_score);
				flushPrepareGun(result);
				updateWarCoin(SINGLE_HERO->m_iGold, result->coins);
			}
			else if (result->failed == 5)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COIN_NOT");
			}
			else if (result->failed == 20)//已满
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_HAVE_BEEN_FULL");
			}
			else if (result->failed == 4)//不在国战状态
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_PALACE_COUNTRY_YOU_NOT_IN_WAR");
			}
			break;
	}
	// 战斗中城市被掠夺的情况
	case PROTO_TYPE_GetNationWarCityLostResult:
	{
			GetNationWarCityLostResult *result = (GetNationWarCityLostResult*)message;
			if (result->failed == 0)
			{
				openWarstatus(result);
			}
			else
			{

			}

			break;
	}
	case PROTO_TYPE_RepairCityDefenseInWarResult:
	{
			RepairCityDefenseInWarResult *result = (RepairCityDefenseInWarResult*)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_SUCCESS",2,result->obtain_score);
				m_repairAllTime = result->remain_time;
				flushOnWarDurable(result);
				updateWarCoin(result->golds, result->coins);
			}
			else if (result->failed ==5)
			{
				if (result->type ==0)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_COIN_NOT");
				}
				else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_GOLD_NOT");
				}
			}
			else if (result->failed == 20)//已满
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_HAVE_BEEN_FULL");
			}
			else if (result->failed == 4)//不在国战状态
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_PALACE_COUNTRY_YOU_NOT_IN_WAR");
			}

			break;
	}
	case PROTO_TYPE_EnhanceAttackResult:
	{
			EnhanceAttackResult *result = (EnhanceAttackResult*)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_SUCCESS",2,result->obtain_score);
				if (result->status == 1)//正在使用超级火力
				{
					m_addGunAllTime = result->dis_to_end - 300;
				}
				else if (result->status == 2)//超级火力冷却中
				{
					m_addGunAllTime = result->dis_to_end;
				}
				flushOnWarGun(result);
				updateWarCoin(SINGLE_HERO->m_iGold, result->coins);
			}
			else if (result->failed == 5)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COIN_NOT");
			}
			else if (result->failed == 20)//已满
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_INVEST_HAVE_BEEN_FULL");
			}
			else if (result->failed == 4)//不在国战状态
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_PALACE_COUNTRY_YOU_NOT_IN_WAR");
			}
			break;
	}
	default:
	break;
	}

}
void UINationWarEntrance::guardEnterScene(const float time)
{
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_CSB]);
	auto panel_adjutant_dialog = view->getChildByName<Widget*>("panel_adjutant_dialog");
	panel_adjutant_dialog->setPosition(STARTPOS);
	auto i_soldier = panel_adjutant_dialog->getChildByName<ImageView*>("image_bargirl");

	auto i_dialog = panel_adjutant_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_guard = i_dialog->getChildByName<Text*>("label_bargirl");
	auto t_content = i_dialog->getChildByName<Text*>("label_content");
	auto i_anchor = i_dialog->getChildByName<ImageView*>("image_anchor");

	i_dialog->setOpacity(0);
	t_guard->setOpacity(0);
	t_content->setOpacity(0);
	i_anchor->setVisible(false);
	i_soldier->loadTexture(getCheifIconPath());
	i_soldier->runAction(MoveBy::create(0.5, Vec2(i_soldier->getBoundingBox().size.width, 0)));
	auto tips = SINGLE_SHOP->getTipsInfo();
	if (SINGLE_HERO->m_iGender == 1)
	{
		t_guard->setString(tips["TIP_MAIN_FEMAIE_AIDE"]);
	}
	else
	{
		t_guard->setString(tips["TIP_MAIN_MAIE_AIDE"]);
	}
	
	t_content->setString(tips["TIP_COUNTRY_WAR_GUARD_CONTENT"]);

	i_dialog->setPositionX(i_dialog->getPositionX() + i_soldier->getBoundingBox().size.width);
	i_dialog->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), FadeIn::create(0.5f)));
	t_guard->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), FadeIn::create(0.5f)));
	t_content->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), FadeIn::create(0.5f)));
	//i_anchor->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), FadeIn::create(0.5f)));
}
void UINationWarEntrance::guardExitScene(const float time)
{
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_CSB]);
	auto panel_adjutant_dialog = view->getChildByName<Widget*>("panel_adjutant_dialog");
	panel_adjutant_dialog->setPosition(STARTPOS);
	auto i_soldier = panel_adjutant_dialog->getChildByName<ImageView*>("image_bargirl");

	auto i_dialog = panel_adjutant_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_guard = i_dialog->getChildByName<Text*>("label_bargirl");
	auto t_content = i_dialog->getChildByName<Text*>("label_content");
	auto i_anchor = i_dialog->getChildByName<ImageView*>("image_anchor");

	i_dialog->stopAllActions();
	t_guard->stopAllActions();
	t_content->stopAllActions();
	i_anchor->stopAllActions();

	i_dialog->runAction(FadeOut::create(0.5f));
	t_guard->runAction(FadeOut::create(0.5f));
	t_content->runAction(FadeOut::create(0.5f));
	//i_anchor->runAction(FadeOut::create(0.5f));

	i_soldier->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), EaseBackOut::create(MoveBy::create(0.3, Vec2(-i_soldier->getBoundingBox().size.width, 0)))));
}
void UINationWarEntrance::mainButtonChangeStatus(Widget* target)
{
	if (m_pMainButton)
	{
		m_pMainButton->runAction(MoveBy::create(0.2, Vec2(20, 0)));
		m_pMainButton->setBright(true);
		m_pMainButton->setTouchEnabled(true);
	}
	m_pMainButton = target;
	if (m_pMainButton)
	{
		m_pMainButton->runAction(MoveBy::create(0.2, Vec2(-20, 0)));
		m_pMainButton->setBright(false);
		m_pMainButton->setTouchEnabled(false);
	}
}
void UINationWarEntrance::openPrepareDefendPanel()
{
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_PREPARE_DEFENSE_CSB]);
	if (!view)
	{
		openView(COUNTRY_WAR_RES[COUNTRY_WAR_PREPARE_DEFENSE_CSB]);
		view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_PREPARE_DEFENSE_CSB]);
		view->setPosition(ENDPOS3);
		auto listview_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_content"));
		auto image_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown"));
		auto button_pulldown = dynamic_cast<Button*>(Helper::seekWidgetByName(image_pulldown, "button_pulldown"));
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
		addListViewBar(listview_content, image_pulldown);
		mainPageMoveEnd(view);
	}

	auto listview_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_content"));
	auto image_city_property = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_city_property"));
	auto text_fort_upgrade = dynamic_cast<Text*>(Helper::seekWidgetByName(image_city_property, "text_fort"));
	text_fort_upgrade->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_PREPARE_UPDATE_FORT"]);
	//城市防御

	auto panel_current_dur = dynamic_cast<Widget*>(Helper::seekWidgetByName(image_city_property, "panel_current_dur"));
	auto panel_current_gun = dynamic_cast<Widget*>(Helper::seekWidgetByName(image_city_property, "panel_current_gun"));

	auto image_city_durable = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_city_property, "image_city_property"));
	auto panel_durable = dynamic_cast<Widget*>(Helper::seekWidgetByName(image_city_property, "panel_durable"));
	
	auto label_derable_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_city_property, "label_derable_lv"));
	auto label_derable_num = dynamic_cast<Text*>(Helper::seekWidgetByName(image_city_property, "label_derable_num"));
	auto image_progressbar_lever_d = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_city_property, "image_progressbar_lever_d"));
	
	auto t_current_hp_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_current_dur, "label_current_num"));
	auto t_current_gun_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_current_gun, "label_current_num"));
	t_current_hp_coin->setString(numSegment(StringUtils::format("%lld", m_PrepareResult->current_hp_pool)));
	t_current_gun_coin->setString(numSegment(StringUtils::format("%lld", m_PrepareResult->current_attack_pool)));
	label_derable_lv->setString(StringUtils::format("Max:%lld", m_PrepareResult->final_max_hp));
	label_derable_num->setString(StringUtils::format("%lld", m_PrepareResult->current_hp));
	
	auto label_prepare_updatewall = dynamic_cast<Text*>(Helper::seekWidgetByName(image_city_property,"label_prepare_updatewall"));
	label_prepare_updatewall->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::menuCall_func, this));
	for (int i = 0; i < 5;i++)
	{
		auto star = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_durable, StringUtils::format("image_intensify_%d", i + 1)));
		if (m_PrepareResult->hp_star>i)
		{
			setGLProgramState(star, false);
		}
		else
		{
			setGLProgramState(star, true);
		}
	}
	//城市攻击
	auto panel_gun = dynamic_cast<Widget*>(Helper::seekWidgetByName(image_city_property, "panel_gun"));
	
	auto label_gun_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_gun, "label_gun_lv"));
	auto label_gun_number = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_gun, "label_gun_number"));
	auto image_progressbar_lever_g = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_city_property, "image_progressbar_lever_g"));
	

	label_gun_lv->setString(StringUtils::format("Max:%lld", m_PrepareResult->final_max_attack));
	label_gun_number->setString(StringUtils::format("%lld", m_PrepareResult->attack));


	auto label_prepare_updategun = dynamic_cast<Text*>(Helper::seekWidgetByName(image_city_property, "label_prepare_updategun"));
	label_prepare_updategun->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::menuCall_func, this));
	for (int i = 0; i < 5; i++)
	{
		auto star = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_gun, StringUtils::format("image_intensify_%d", i + 1)));
		if (m_PrepareResult->attack_star>i)
		{
			setGLProgramState(star, false);
		}
		else
		{
			setGLProgramState(star, true);
		}
	}
	
	auto i_reinforcements_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(listview_content, "image_reinforcements_bg_1"));
	auto i_reinforcements_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(listview_content, "image_reinforcements_bg_0_0"));
	auto text_fort_hire= dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_1, "text_fort"));
	auto text_fort_depot = dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_2, "text_fort"));
	text_fort_hire->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_PREPARE_HIRE_REINFORCEMENTS"]);
	text_fort_depot->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_PREPARE_BULID_DEPOT"]);

	//已雇佣的

	auto panel_rein = dynamic_cast<Widget*>(Helper::seekWidgetByName(i_reinforcements_1, "panel_rein"));
	auto image_div_2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_reinforcements_1, "image_div_2"));
	auto image_input_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_reinforcements_1, "image_input_bg")); 
	auto label_employment_costs_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_1, "label_employment_costs_0"));
	auto label_employment_costs_title = dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_1, "label_employment_costs:"));

	auto label_num = dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_1, "label_num"));
	label_num->setString(StringUtils::format("%lld/50", m_PrepareResult->mercenary_pool / m_PrepareResult->one_mercenary_cost));
	

	auto image_progressbar_shipp_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(i_reinforcements_1, "image_progressbar_shipp_1"));
	auto progressbar_shipp = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(image_progressbar_shipp_1, "progressbar_shipp"));
	auto label_shipp_num = dynamic_cast<Text*>(Helper::seekWidgetByName(image_progressbar_shipp_1, "label_shipp_num"));


	auto label_v_costs = dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_2, "label_employment_costs_0"));
	auto label_v_num = dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_2, "label_v_num"));
	auto image_progressbar_shipp_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(i_reinforcements_2, "image_progressbar_shipp_2"));
	auto progressbar_depot = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(image_progressbar_shipp_2, "progressbar_depot"));
	auto label_depot_num = dynamic_cast<Text*>(Helper::seekWidgetByName(image_progressbar_shipp_2, "label_depot_num"));

	auto label_coin_costs = dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_1, "label_employment_costs_0"));
	auto label_coin_num = dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_1, "label_funds_coins_num"));
	label_coin_costs->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_PREPARE_INVEST_COIN_MIN_NUM"]);
	label_v_costs->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_PREPARE_INVEST_V_TICKEY_MIN_NUM"]);
	label_coin_num->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::menuCall_func, this));
	label_v_num->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::menuCall_func, this));
	auto current_mercenary_cost = m_PrepareResult->mercenary_pool%m_PrepareResult->one_mercenary_cost;

	label_shipp_num->setString(numSegment(StringUtils::format("%lld", current_mercenary_cost)) + "/" + numSegment(StringUtils::format("%lld", m_PrepareResult->one_mercenary_cost)));
	progressbar_shipp->setPercent(100.0*current_mercenary_cost / m_PrepareResult->one_mercenary_cost);
	if (m_PrepareResult->mercenary_pool>=m_PrepareResult->max_mercenary)
	{
		panel_rein->setPositionX(240);
		image_div_2->setVisible(false);
		image_progressbar_shipp_1->setVisible(false);
		image_input_bg->setVisible(false);
		label_employment_costs_0->setVisible(false);
		label_employment_costs_title->setVisible(false);
		label_num->setTextColor(Color4B(0,200,83,255));
	}
	else
	{
		panel_rein->setPositionX(25.82);
		image_div_2->setVisible(true);
		image_progressbar_shipp_1->setVisible(true);
		image_input_bg->setVisible(true);
		label_employment_costs_0->setVisible(true);
		label_employment_costs_title->setVisible(true);
		label_num->setTextColor(Color4B(45, 25, 13, 255));
	}
	//仓库
	auto panel_de = dynamic_cast<Widget*>(Helper::seekWidgetByName(i_reinforcements_2,"panel_de"));
	auto label_reinforcements_complete = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_de, "label_reinforcements_0"));
	label_reinforcements_complete->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_PREPARE_STATE_DEPOT_SUCC"]);
	auto image_input_bg_depot = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_reinforcements_2, "image_input_bg"));
	auto label_employment_costs_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_2, "label_employment_costs:"));
	auto label_employment_costs_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(i_reinforcements_2, "label_employment_costs_0"));
	auto image_div_2_depot = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_reinforcements_2, "image_div_2"));
	label_depot_num->setString(StringUtils::format("%d/%d", m_PrepareResult->depot_pool, m_PrepareResult->max_depot));
	progressbar_depot->setPercent(100.0*m_PrepareResult->depot_pool / m_PrepareResult->max_depot);
	if (m_PrepareResult->depot_pool >= m_PrepareResult->max_depot)
	{
		panel_de->setPositionX(240);
		image_div_2_depot->setVisible(false);
		label_employment_costs_1->setVisible(false);
		label_employment_costs_2->setVisible(false);
		image_progressbar_shipp_2->setVisible(false);
		image_input_bg_depot->setVisible(false);
		label_reinforcements_complete->setVisible(true);

	}
	else
	{
		panel_de->setPositionX(20);
		image_div_2_depot->setVisible(true);
		label_employment_costs_1->setVisible(true);
		label_employment_costs_2->setVisible(true);
		image_progressbar_shipp_2->setVisible(true);
		image_input_bg_depot->setVisible(true);
		label_reinforcements_complete->setVisible(false);
	}
	
	if (!isScheduled(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds)))
	{
		this->schedule(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds), 3.0f);
	}
}
void UINationWarEntrance::flushPrepareDurable(AddCityRepairPoolResult*result)
{
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_PREPARE_DEFENSE_CSB]);
	if (view)
	{
		auto image_city_property = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_city_property"));
		auto panel_current_dur = dynamic_cast<Widget*>(Helper::seekWidgetByName(image_city_property, "panel_current_dur"));
		//城市防御
		auto t_current_hp_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_current_dur, "label_current_num"));
		auto image_city_durable = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_city_property, "image_city_property"));
		auto panel_durable = dynamic_cast<Widget*>(Helper::seekWidgetByName(image_city_property, "panel_durable"));

		t_current_hp_coin->setString(numSegment(StringUtils::format("%lld", result->current_hp_pool)));
		auto label_derable_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_city_property, "label_derable_lv"));
		auto label_derable_num = dynamic_cast<Text*>(Helper::seekWidgetByName(image_city_property, "label_derable_num"));
	
		label_derable_lv->setString(StringUtils::format("Max:%lld", result->final_max_hp));
		label_derable_num->setString(StringUtils::format("%lld", result->defense));

	
		for (int i = 0; i < 5; i++)
		{
			auto star = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_durable, StringUtils::format("image_intensify_%d", i + 1)));
			if (result->defense_star>i)
			{
				setGLProgramState(star, false);
			}
			else
			{
				setGLProgramState(star, true);
			}
		}
	}
}
void UINationWarEntrance::flushPrepareGun(AddCityAttackPoolResult*result)
{
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_PREPARE_DEFENSE_CSB]);
	if (view)
	{
		auto image_city_property = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_city_property"));
		auto panel_current_gun = dynamic_cast<Widget*>(Helper::seekWidgetByName(image_city_property, "panel_current_gun"));

		//城市攻击
		auto t_current_gun_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_current_gun, "label_current_num"));
		auto panel_gun = dynamic_cast<Widget*>(Helper::seekWidgetByName(image_city_property, "panel_gun"));
		auto label_gun_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_gun, "label_gun_lv"));
		auto label_gun_number = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_gun, "label_gun_number"));
	
		t_current_gun_coin->setString(numSegment(StringUtils::format("%lld", result->current_attack_pool)));
		label_gun_lv->setString(StringUtils::format("Max:%lld", result->final_max_attack));
		label_gun_number->setString(StringUtils::format("%lld", result->attack));

		for (int i = 0; i < 5; i++)
		{
			auto star = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_gun, StringUtils::format("image_intensify_%d", i + 1)));
			if (result->attack_star>i)
			{
				setGLProgramState(star, false);
			}
			else
			{
				setGLProgramState(star, true);
			}
		}
	}
}
void UINationWarEntrance::openOnGoingWarPanel()
{
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_DEFEND_CSB]);
	if (!view)
	{
		openView(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_DEFEND_CSB]);
		view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_DEFEND_CSB]);
		view->setPosition(ENDPOS3);
		mainPageMoveEnd(view);
	}
	auto text_fort = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "text_fort"));
	text_fort->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_PREPARE_UPDATE_FORT"]);
	//城市耐久
	auto panel_durable = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_durable"));
	auto panel_intensify_du = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_intensify_du"));
	auto label_derable_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_durable, "label_derable_num"));
	auto progressbar_lever = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_durable, "progressbar_lever"));
	label_derable_num->setString(numSegment(StringUtils::format("%lld", m_PrepareResult->current_hp)) +"/"+numSegment(StringUtils::format("%lld", m_PrepareResult->max_hp)));
	setTextFontSize(label_derable_num);
	progressbar_lever->setPercent(100.0*m_PrepareResult->current_hp / m_PrepareResult->max_hp);
	panel_intensify_du->setVisible(false);

	//火力
	auto panel_gun = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_gun"));
	auto panel_intensify_gun = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_gun, "panel_intensify_gun"));
	auto label_number = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_gun, "label_number"));
	if (m_PrepareResult->power_attack_status==1)
	{
		label_number->setString(StringUtils::format("%lld", m_PrepareResult->attack+m_PrepareResult->extra_attack_in_war));
	}
	else
	{
		label_number->setString(StringUtils::format("%lld", m_PrepareResult->attack));
	}
	
	panel_intensify_gun->setVisible(false);
	
	//修复城墙
	auto panel_repair = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_repair"));
	auto label_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_repair, "label_num"));//耐久
	auto progressbar_shipp_repair = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_repair, "progressbar_shipp"));
	auto label_repairWall_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_repair, "label_repairWall_num"));
	label_repairWall_num->setTouchEnabled(true);
	label_repairWall_num->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::onWaringEvent, this));
	label_num->setString(StringUtils::format("%lld", m_PrepareResult->can_add_hp_in_war));

	//超级火力
	auto panel_firepower = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_firepower"));
	auto panel_off = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_firepower, "panel_off"));

	auto progressbar_shipp_gun = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_firepower, "progressbar_shipp"));
	auto label_firePower_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_firepower, "label_firePower_num"));
	label_firePower_num->setTouchEnabled(true);
	label_firePower_num->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::onWaringEvent, this));
	progressbar_shipp_gun->setPercent(100.0*m_PrepareResult->power_attack_pool / m_PrepareResult->max_power_attack);
	auto image_sliver_gun = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_off, "image_sliver_gun"));
	auto label_level = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_off, "label_level"));
	
	label_level->setString(numSegment(StringUtils::format("%lld", m_PrepareResult->power_attack_pool)) + "/" + numSegment(StringUtils::format("%lld", m_PrepareResult->max_power_attack)));
	label_level->enableOutline(Color4B::BLACK, OUTLINE_MIN);
	image_sliver_gun->setPositionX(label_level->getPositionX() - label_level->getContentSize().width / 2 - 20);
	//auto label_num_add_gun = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_on, "label_num"));
	//label_num_add_gun->setString(StringUtils::format("%lld", m_PrepareResult->extra_attack_in_war));

	//V票修复
	auto panel_quickrepair = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_quickrepair"));
	auto button_v = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_quickrepair, "button_v"));
	button_v->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::onWaringEvent, this));

	if (!isScheduled(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds)))
	{
		this->schedule(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds), 3.0f);
	}
}
	
	
void UINationWarEntrance::openRulesPanel()
{
	openView(COUNTRY_WAR_RES[COUNTRY_WAR_RULES_CSB]);
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_RULES_CSB]);
	view->setPosition(ENDPOS3);
	/////////////////////

	auto l_rules = view->getChildByName<ListView*>("listview_rules");

	auto t_reward = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "text_rewards_0"));
	auto str_reward = SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_REWARD"];
	auto size = Size(t_reward->getContentSize().width, getLabelHight(str_reward, t_reward->getContentSize().width, t_reward->getFontName(), t_reward->getFontSize()) + 10);
	t_reward->setString(str_reward);
	t_reward->setContentSize(size);
	t_reward->setPositionY(0);

	auto t_rule = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "text_rules_war"));
	for (size_t i = 0; i < 3; i++)
	{
		Text *t_rule_clone;
		if (i == 0)
		{
			t_rule_clone = t_rule;
		}
		else
		{
			t_rule_clone = dynamic_cast<Text*>(t_rule->clone());
			l_rules->pushBackCustomItem(t_rule_clone);
		}
		auto str_content = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_PALACE_WAR_RULE_%d", i + 1)];
		auto size1 = Size(t_rule->getContentSize().width, getLabelHight(str_content, t_rule->getContentSize().width, t_rule->getFontName(), t_rule->getFontSize()) + 10);
		t_rule_clone->setString(str_content);
		t_rule_clone->setContentSize(size1);
		t_rule->setPositionY(0);
	}

	//////////////////////
	auto  i_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	i_pulldown->setVisible(true);
	auto  button_pulldown = view->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(i_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_rules, i_pulldown);
	mainPageMoveEnd(view);
}
void UINationWarEntrance::mainPageMoveEnd(Widget*view)
{
	float time = 1.0f;
	if (m_pView)
	{
		time = 0.5f;
		m_pView->runAction(EaseBackOut::create(MoveTo::create(0.5f, ENDPOS3)));
	}

	view->runAction(Sequence::createWithTwoActions(DelayTime::create(time), EaseBackOut::create(MoveTo::create(0.5f, STARTPOS))));
	m_pView = view;
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(time + 0.5f), CallFunc::create(CC_CALLBACK_0(UINationWarEntrance::attackMoveEnd, this))));
}
void UINationWarEntrance::closeOtherView(const float fTime)
{
	auto view1 = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_REPORT_CSB]);
	if (m_pView != view1)
	{
		closeView(COUNTRY_WAR_RES[COUNTRY_WAR_REPORT_CSB]);
	}
	auto view2 = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_PREPARE_DEFENSE_CSB]);
	if (m_pView != view2)
	{
		closeView(COUNTRY_WAR_RES[COUNTRY_WAR_PREPARE_DEFENSE_CSB]);
	}
	auto view3 = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_RULES_CSB]);
	if (m_pView != view3)
	{
		closeView(COUNTRY_WAR_RES[COUNTRY_WAR_RULES_CSB]);
	}
	auto view4 = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_DEFEND_CSB]);
	if (m_pView != view4)
	{
		closeView(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_DEFEND_CSB]);
	}
}
void UINationWarEntrance::updateWarCoin(int64_t golds, int64_t coins)
{
	SINGLE_HERO->m_iCoin = coins;
	SINGLE_HERO->m_iGold = golds;
	auto viewPad = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	if (viewPad)
	{
		auto panel_silver = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewPad,"panel_silver"));
		auto label_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_silver, "label_silver_num"));
		auto label_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_silver, "label_gold_num"));
		label_silver_num->setString(numSegment(StringUtils::format("%lld", coins)));
		label_gold_num->setString(numSegment(StringUtils::format("%lld", golds)));
		setTextFontSize(label_silver_num);
		setTextFontSize(label_gold_num);
	}
}
void UINationWarEntrance::showNumpad(Text*m_usingText)
{
	m_padStirng.clear();
	m_UsingNumPadText = m_usingText;
	m_UsingNumPadText->setString("");
	openView(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	auto numpad = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	if (numpad)
	{
		numpad->setPosition(STARTPOS);
		auto num_root = numpad->getChildByName<Widget*>("panel_numpad");
		num_root->setPosition(ENDPOS4);
		num_root->runAction(MoveTo::create(0.5, STARTPOS));
		num_root->getChildByName<Button*>("button_yes")->setTouchEnabled(true);
		numpad->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::numPadEvent, this));
		numpad->setTouchEnabled(true);
		updateWarCoin(SINGLE_HERO->m_iGold, SINGLE_HERO->m_iCoin);
		for (int i = 0; i < 10; i++)
		{
			auto b_num = num_root->getChildByName<Button*>(StringUtils::format("button_%d", i));
			b_num->setTouchEnabled(true);
			b_num->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::numPadEvent, this));
		}
		auto button_del = dynamic_cast<Button*>(Helper::seekWidgetByName(num_root,"button_del"));
		button_del->setTouchEnabled(true);
		button_del->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::numPadEvent, this));
		auto button_yes = dynamic_cast<Button*>(Helper::seekWidgetByName(num_root, "button_yes"));
		button_yes->setTouchEnabled(true);
		button_yes->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::numPadEvent, this));

	}
}
void UINationWarEntrance::numPadEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}
	auto button = dynamic_cast<Widget*>(pSender);
	std::string name = button->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(panel_numpad) || isButton(button_yes))
	{
		auto numpad = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
		if (numpad)
		{
			
			numpad->setTouchEnabled(false);
			auto num_root = numpad->getChildByName<Widget*>("panel_numpad");
			for (int i = 0; i < 10; i++)
			{
				auto b_num = num_root->getChildByName<Button*>(StringUtils::format("button_%d", i));
				b_num->setTouchEnabled(false);
			}
			auto button_del = dynamic_cast<Button*>(Helper::seekWidgetByName(num_root, "button_del"));
			button_del->setTouchEnabled(false);
			auto button_yes = dynamic_cast<Button*>(Helper::seekWidgetByName(num_root, "button_yes"));
			button_yes->setTouchEnabled(false);
			num_root->runAction(MoveTo::create(0.5, ENDPOS4));
			numpad->runAction(Sequence::create(DelayTime::create(0.7), Place::create(ENDPOS), CallFunc::create([=]{closeView(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]); }),nullptr));
		}
	
		auto nums = atoll(m_padStirng.c_str());
		if (m_UsingNumPadText&&m_usingNumPad != M_ONWARING_REPAIR_DURABLE_USE_V)
		{
			m_UsingNumPadText->setString("");
			if (nums <= 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_BANK_INSURANCE_TEXTFIELD_CONTENT_NOT_NUM");
				return;
			}

			if (m_usingNumPad == M_INVEST_VTICKET)
			{
				costResultPanle(nums, 1);
			}
			else
			{
				costResultPanle(nums, 0);
			}
		}
		return;
	}
	if (isButton(button_del))
	{
		if (!m_padStirng.empty())
		{
			m_padStirng.erase(m_padStirng.end() - 1);
		}
	}
	else
	{
		int tag = button->getTag();

		if (m_padStirng.size() < 2 && m_padStirng == "0")
		{
			m_padStirng.erase(m_padStirng.end() - 1);
		}
		m_padStirng += StringUtils::format("%d", tag);
	}
	if (m_UsingNumPadText)
	{
		m_UsingNumPadText->setString(numSegment(m_padStirng));
	}
}
void UINationWarEntrance::onWaringEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	auto button = dynamic_cast<Widget*>(pSender);
	std::string name = button->getName();
	auto inPutVorCoinsText = dynamic_cast<Text*>(pSender);
	if (m_PrepareResult->current_hp<=0)
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_WAR_CITY_HAVE_FAILED");
		return;
	}
	//V票修复城市耐久
	if (isButton(button_v))
	{
		if (SINGLE_HERO->m_iGold<1)
		{
			openSuccessOrFailDialog("TIP_PALACE_COUNTRY_WAR_ON_WAR_FIX_DURABLE_USE_V");
			return;
		}
		repairWallByVticket();
		return;
	}
	//使用银币修复城墙
	if (isButton(label_repairWall_num))
	{
		m_usingNumPad = M_ONWARING_REPAIR_DURABLE_USE_COIN;
		showNumpad(inPutVorCoinsText);
		return;
	}
	//add funds,增强火力
	if (isButton(label_firePower_num))
	{
		m_usingNumPad = M_ONWARING_SUPER_FIRE;
		showNumpad(inPutVorCoinsText);
		return;
	}
	//使用V票快速修复
	if (isButton(label_currency_num))
	{
		m_usingNumPad = M_ONWARING_REPAIR_DURABLE_USE_V;
		showNumpad(inPutVorCoinsText);
		return;
	}
	//快速修复
	if (isButton(button_yes))
	{
		auto nums = atoll(m_padStirng.c_str());
		closeView(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_REPAIR_BY_V_CSB]);
		//type 0,coin 1 v
		if (nums<1)
		{
			openSuccessOrFailDialog("TIP_COUNTRY_WAR_ONWAR_V_AT_LEAST");
			return;
		}
		ProtocolThread::GetInstance()->onWaringRepairDefense(1, nums, UILoadingIndicator::create(this));
		return;
	}
}
void UINationWarEntrance::everySecondflush(const float fTime)
{
	char buf[100];
	if (m_Times >= -1)
	{
		if (m_Times < 0)
		{			
			if (m_PrepareResult->is_in_war == 1)//准备状态
			{
				m_bChangePrepareToWar = true;
				ProtocolThread::GetInstance()->getCityPrepareStatus(UILoadingIndicator::create(this));
			}
			else//战斗结束
			{	
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_COUNTRY_WAR_END_TO_PALACE");		
			}
			if (isScheduled(schedule_selector(UINationWarEntrance::everySecondflush)))
			{
				this->unschedule(schedule_selector(UINationWarEntrance::everySecondflush));
			}
			if (isScheduled(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds)))
			{
				this->unschedule(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds));
			}
		}
		else
		{
			auto viewTwo = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_TWO_CSB]);
			auto panel_actionbar = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewTwo, "panel_actionbar"));

			int64_t myTime = m_Times;
			m_Times--;
			snprintf(buf, sizeof(buf), "%02lld:%02lld:%02lld", myTime / 3600, (myTime % 3600) / 60, myTime % 60);
			if (viewTwo)
			{
				auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_actionbar, "label_event_0"));

				t_time->setString(buf);
			}
		}
	}
	//战斗阶段相关倒计时
	auto viewOnwar = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_DEFEND_CSB]);
	if (viewOnwar)
	{
		
		if (m_repairAllTime>=0)
		{
			auto times = m_repairAllTime;
			m_repairAllTime--;		
			//修复
			
			auto panel_repair = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewOnwar, "panel_repair"));
			auto label_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_repair, "label_num"));//耐久
			auto label_num_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_repair, "label_num_0"));//倒计时
			auto progressbar_shipp = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_repair, "progressbar_shipp"));
			auto image_hummer = dynamic_cast<ImageView*>(Helper::seekWidgetByName(progressbar_shipp, "image_hummer"));

			label_num->setString(StringUtils::format("%lld", m_PrepareResult->can_add_hp_in_war));
			progressbar_shipp->setPercent(100.0*(600 - times) / 600);
			image_hummer->setPositionX(progressbar_shipp->getContentSize().width*(100.0*(600 - times) / 600 / 100.0));
			label_num_0->setString(StringUtils::format("%02d:%02d", (times % 3600) / 60, times % 60));
		}
		if (m_addGunAllTime>=0)
		{
			
			//火力
			auto panel_firepower = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewOnwar, "panel_firepower"));
			auto panel_off = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_firepower, "panel_off"));//进度条

			auto label_title_no = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_firepower, "label_recovery_0_0"));//不能使用
			auto label_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_firepower, "label_num_0_0"));//时间
			auto label_repairing_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_firepower, "label_repairing_0"));//标题
			auto image_icon_3_0 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_firepower, "image_icon_3_0"));//图标
			
			auto label_level = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_off, "label_level"));

			auto progressbar_shipp_gun = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_off, "progressbar_shipp"));
	
		
			//numSegment(StringUtils::format("%lld", m_PrepareResult->power_attack_pool)) + "/" + numSegment(StringUtils::format("%lld", m_PrepareResult->max_power_attack));
			label_level->setString(numSegment(StringUtils::format("%lld", m_PrepareResult->power_attack_pool)) + "/" + numSegment(StringUtils::format("%lld", m_PrepareResult->max_power_attack)));
			progressbar_shipp_gun->setPercent(100.0*m_PrepareResult->power_attack_pool / m_PrepareResult->max_power_attack);
			label_time->setVisible(true);
			int times = m_addGunAllTime;
			m_addGunAllTime--;
			
			if (m_PrepareResult->power_attack_status == 0)//不可使用
			{
				image_icon_3_0->setVisible(false);
				label_time->setVisible(false);
				label_title_no->setVisible(true);
				label_repairing_0->setVisible(false);
				panel_off->setVisible(true);

			}
			else if (m_PrepareResult->power_attack_status == 1)//正在使用
			{
				
				image_icon_3_0->setVisible(true);
				label_time->setVisible(true);
				label_title_no->setVisible(false);
				label_repairing_0->setVisible(true);
				label_repairing_0->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_SUPER_FIRE_USING"]);
				panel_off->setVisible(false);
			}
			else if (m_PrepareResult->power_attack_status == 2)//正在冷却
			{
				
				image_icon_3_0->setVisible(false);
				label_time->setVisible(true);
				label_title_no->setVisible(false);
				label_repairing_0->setVisible(true);
				label_repairing_0->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_SUPER_FIRE_COOLING"]);
				panel_off->setVisible(true);
			}
			
			label_time->setString(StringUtils::format("%02d:%02d", (times % 3600) / 60, times % 60));
			label_repairing_0->setPositionX(label_time->getPositionX() - label_repairing_0->getContentSize().width - 10);
		}
		
	}

}
void UINationWarEntrance::repairWallByVticket()
{
	
	openView(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_REPAIR_BY_V_CSB]);
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_REPAIR_BY_V_CSB]);
	auto label_title_sailors = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title_sailors"));
	auto image_v = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_v"));
	auto image_ic = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_ic"));
	auto labe_content = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "labe_content"));
	//labe_content->setString("");
	image_v->ignoreContentAdaptWithSize(false);
	image_ic->ignoreContentAdaptWithSize(false);
	
	image_v->loadTexture("res/Vticket_coin/v_ticket.png");
	image_ic->loadTexture("login_ui/war_720/icon_city_durable_2.png");
	label_title_sailors->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_GENERAL_NORMAL_UPGRADE"]);


	auto progressbar_lever = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view, "progressbar_lever"));
	auto label_level = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_level"));
	progressbar_lever->setPercent(100.0*m_PrepareResult->current_hp / m_PrepareResult->max_hp);
	
	label_level->setString(numSegment(StringUtils::format("%lld", m_PrepareResult->current_hp)) + "/" + numSegment(StringUtils::format("%lld", m_PrepareResult->max_hp)));

	auto button_yes = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_yes"));
	auto label_currency_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_currency_num"));
	label_currency_num->setTouchEnabled(true);
	label_currency_num->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::onWaringEvent, this));
	button_yes->addTouchEventListener(CC_CALLBACK_2(UINationWarEntrance::onWaringEvent, this));
}
void UINationWarEntrance::flushDefendPanelSeconds(const float fTime)
{
	auto viewDefendPrepar = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_PREPARE_DEFENSE_CSB]);
	auto viewDefendOnwar = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_DEFEND_CSB]);
	auto viewReport = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_REPORT_CSB]);
	if (viewDefendOnwar || viewDefendPrepar)
	{
		ProtocolThread::GetInstance()->getCityPrepareStatus();
	}
	else if (viewReport)
	{	
		ProtocolThread::GetInstance()->getNationWarCityLost();
	}
	else
	{
		if (isScheduled(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds)))
		{
			this->unschedule(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds));
		}
	}
}
void UINationWarEntrance::flushOnWarDurable(RepairCityDefenseInWarResult *result)
{
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_DEFEND_CSB]);
	if (view)
	{
		//城市耐久
		auto panel_durable = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_durable"));
		auto label_derable_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_durable, "label_derable_num"));
		auto progressbar_lever = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_durable, "progressbar_lever"));
		label_derable_num->setString(numSegment(StringUtils::format("%lld", result->defense)) + "/" + numSegment(StringUtils::format("%lld", m_PrepareResult->max_hp)));
		progressbar_lever->setPercent(100.0*result->defense / m_PrepareResult->max_hp);
	}
}
void UINationWarEntrance::flushOnWarGun(EnhanceAttackResult *result)
{
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_ON_WARRING_DEFEND_CSB]);
	if (view)
	{
		//火力
		auto panel_firepower = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_firepower"));
		auto panel_off = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_firepower, "panel_off"));
		auto label_level = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_off, "label_level"));
		auto image_sliver_gun = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_off, "image_sliver_gun"));
		auto progressbar_shipp_gun = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(panel_off, "progressbar_shipp"));
		label_level->setString(numSegment(StringUtils::format("%lld", result->power_attack_pool)) + "/" + numSegment(StringUtils::format("%lld", m_PrepareResult->max_power_attack)));
		progressbar_shipp_gun->setPercent(100.0*result->power_attack_pool / m_PrepareResult->max_power_attack);
		image_sliver_gun->setPositionX(label_level->getPositionX() - label_level->getContentSize().width / 2 - 20);
	}
}
void UINationWarEntrance::openWarstatus(GetNationWarCityLostResult* result)
{
	auto view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_REPORT_CSB]);
	if (!view)
	{
		openView(COUNTRY_WAR_RES[COUNTRY_WAR_REPORT_CSB]);
		view = getViewRoot(COUNTRY_WAR_RES[COUNTRY_WAR_REPORT_CSB]);
		view->setPosition(ENDPOS3);
		mainPageMoveEnd(view);
	}
	auto image_titile_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_titile_bg"));
	auto label_title = dynamic_cast<Text*>(Helper::seekWidgetByName(image_titile_bg, "label_title"));
	label_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_REPORT_PANEL_TITLE"]);

	auto l_content = view->getChildByName<ListView*>("listview_content");
	auto p_score = dynamic_cast<Widget*>(l_content->getItem(0));
	auto panel_r = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_score, "panel_r"));
	//玩家分数
	auto t_score_num = p_score->getChildByName<Text*>("label_score_num");
	t_score_num->setString(StringUtils::format("%lld", result->my_score));
	//玩家银币数
	auto t_coin_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_coin_num"));
	t_coin_num->setString(StringUtils::format("%lld", result->rob_coins));
	auto t_icsailor_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_icsailor_num"));
	t_icsailor_num->setString(StringUtils::format("%lld", result->rob_sailor_coins));
	auto t_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_score, "label_gold_num"));
	t_gold_num->setString(StringUtils::format("%lld", result->rob_golds));
	auto t_r_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_r, "label_gold_num"));
	t_r_num->setString(StringUtils::format("%lld", result->rob_prestige));

	if (result->n_my_city_lost || result->n_enemy_city_loss)
	{
		auto p_capital = dynamic_cast<Widget*>(l_content->getItem(2));
		auto p_city_own_1 = p_capital->getChildByName<Widget*>("panel_city_own_1");
		//损失银币总数
		auto t_loss_num = p_city_own_1->getChildByName<Text*>("label_looted_num");
		auto i_silver_loss = p_city_own_1->getChildByName<ImageView*>("image_silver");

		auto p_city_enemy_1 = p_capital->getChildByName<Widget*>("panel_city_enemy_1");
		//战利品银币总数
		auto t_looted_num = p_city_enemy_1->getChildByName<Text*>("label_looted_num");
		auto i_silver_looted = p_city_enemy_1->getChildByName<ImageView*>("image_silver");

		auto p_city_2 = dynamic_cast<Widget*>(l_content->getItem(3));
		p_city_2->setVisible(true);
		auto p_city_own_2 = p_city_2->getChildByName<Widget*>("panel_city_own_2");
		auto p_city_enemy_2 = p_city_2->getChildByName<Widget*>("panel_city_enemy_2");

		std::string fallenCityName;
		int coinNum = 0;
		if (result->n_my_city_lost)
		{
			p_city_own_2->setVisible(true);
			auto t_loss_city_name = p_city_own_2->getChildByName<Text*>("label_looted");
			auto t_loss_city_coin = p_city_own_2->getChildByName<Text*>("label_looted_num");
			auto i_silver_loss = p_city_own_2->getChildByName<ImageView*>("image_silver");

			t_loss_city_name->setString(SINGLE_SHOP->getCitiesInfo()[result->my_city_lost[0]->city_id].name);
			t_loss_city_coin->setString(StringUtils::format("%lld", result->my_city_lost[0]->robbed_coins));
			i_silver_loss->setPositionX(t_loss_city_coin->getPositionX() - t_loss_city_coin->getBoundingBox().size.width - i_silver_loss->getContentSize().width);
			for (int i = 0; i < result->n_my_city_lost; i++)
			{
				coinNum += result->my_city_lost[i]->robbed_coins;
			}
		}
		else
		{
			p_city_own_2->setVisible(false);
			coinNum = 0;
		}
		t_loss_num->setString(StringUtils::format("%d", coinNum));
		i_silver_loss->setPositionX(t_loss_num->getPositionX() - t_loss_num->getBoundingBox().size.width - i_silver_loss->getContentSize().width);

		if (result->n_enemy_city_loss)
		{
			p_city_enemy_2->setVisible(true);
			auto i_flag = p_city_enemy_2->getChildByName<ImageView*>("image_flag");
			auto t_looted_city_name = p_city_enemy_2->getChildByName<Text*>("label_looted");
			auto t_looted_city_coin = p_city_enemy_2->getChildByName<Text*>("label_looted_num");
			auto i_silver_looted = p_city_enemy_2->getChildByName<ImageView*>("image_silver");

			i_flag->loadTexture(getCountryIconPath(result->enemy_city_loss[0]->nation_id));
			fallenCityName = SINGLE_SHOP->getCitiesInfo()[result->enemy_city_loss[0]->city_id].name;
			fallenCityName += SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_STATUS_CITYNAME_PLUS_FALLEN"];
			t_looted_city_name->setString(fallenCityName);
			t_looted_city_coin->setString(StringUtils::format("%lld", result->enemy_city_loss[0]->robbed_coins));
			i_silver_looted->setPositionX(t_looted_city_coin->getPositionX() - t_looted_city_coin->getBoundingBox().size.width - i_silver_looted->getContentSize().width);
			for (int i = 0; i < result->n_enemy_city_loss; i++)
			{
				coinNum += result->enemy_city_loss[i]->robbed_coins;
			}
		}
		else
		{
			p_city_enemy_2->setVisible(false);
			coinNum = 0;
		}
		t_looted_num->setString(StringUtils::format("%d", coinNum));
		i_silver_looted->setPositionX(t_looted_num->getPositionX() - t_looted_num->getBoundingBox().size.width - i_silver_looted->getContentSize().width);

		for (int i = 0; i < m_vWigetInfo.size(); i++)
		{
			m_vWigetInfo[i]->removeFromParentAndCleanup(true);
		}
		m_vWigetInfo.clear();

		if (result->n_my_city_lost > 1 && result->n_my_city_lost >= result->n_enemy_city_loss)
		{
			for (int i = 1; i < result->n_my_city_lost; i++)
			{
				auto p_city_clone = p_city_2->clone();
				l_content->pushBackCustomItem(p_city_clone);
				m_vWigetInfo.push_back(p_city_clone);
				auto p_city_own_2 = p_city_clone->getChildByName<Widget*>("panel_city_own_2");
				auto p_city_enemy_2 = p_city_clone->getChildByName<Widget*>("panel_city_enemy_2");
				p_city_own_2->setVisible(true);
				p_city_enemy_2->setVisible(true);

				auto t_loss_city_name = p_city_own_2->getChildByName<Text*>("label_looted");
				auto t_loss_city_coin = p_city_own_2->getChildByName<Text*>("label_looted_num");
				auto i_silver_loss = p_city_own_2->getChildByName<ImageView*>("image_silver");

				t_loss_city_name->setString(SINGLE_SHOP->getCitiesInfo()[result->my_city_lost[i]->city_id].name);
				t_loss_city_coin->setString(StringUtils::format("%lld", result->my_city_lost[i]->robbed_coins));
				i_silver_loss->setPositionX(t_loss_city_coin->getPositionX() - t_loss_city_coin->getBoundingBox().size.width - i_silver_loss->getContentSize().width);
				if (i >= result->n_enemy_city_loss)
				{
					p_city_enemy_2->setVisible(false);
				}
				else
				{
					auto i_flag = p_city_enemy_2->getChildByName<ImageView*>("image_flag");
					auto t_looted_city_name = p_city_enemy_2->getChildByName<Text*>("label_looted");
					auto t_looted_city_coin = p_city_enemy_2->getChildByName<Text*>("label_looted_num");
					auto i_silver_looted = p_city_enemy_2->getChildByName<ImageView*>("image_silver");

					i_flag->loadTexture(getCountryIconPath(result->enemy_city_loss[i]->nation_id));
					fallenCityName = SINGLE_SHOP->getCitiesInfo()[result->enemy_city_loss[i]->city_id].name;
					fallenCityName += SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_STATUS_CITYNAME_PLUS_FALLEN"];
					t_looted_city_name->setString(fallenCityName);
					t_looted_city_coin->setString(StringUtils::format("%lld", result->enemy_city_loss[i]->robbed_coins));
					i_silver_looted->setPositionX(t_looted_city_coin->getPositionX() - t_looted_city_coin->getBoundingBox().size.width - i_silver_looted->getContentSize().width);
				}
			}
		}
		else if (result->n_enemy_city_loss > 1 && result->n_enemy_city_loss >= result->n_my_city_lost)
		{
			for (int i = 1; i < result->n_enemy_city_loss; i++)
			{
				auto p_city_clone = p_city_2->clone();
				l_content->pushBackCustomItem(p_city_clone);
				m_vWigetInfo.push_back(p_city_clone);
				auto p_city_own_2 = p_city_clone->getChildByName<Widget*>("panel_city_own_2");
				auto p_city_enemy_2 = p_city_clone->getChildByName<Widget*>("panel_city_enemy_2");
				p_city_own_2->setVisible(true);
				p_city_enemy_2->setVisible(true);

				auto i_flag = p_city_enemy_2->getChildByName<ImageView*>("image_flag");
				auto t_looted_city_name = p_city_enemy_2->getChildByName<Text*>("label_looted");
				auto t_looted_city_coin = p_city_enemy_2->getChildByName<Text*>("label_looted_num");
				auto i_silver_looted = p_city_enemy_2->getChildByName<ImageView*>("image_silver");

				i_flag->loadTexture(getCountryIconPath(result->enemy_city_loss[i]->nation_id));
				fallenCityName = SINGLE_SHOP->getCitiesInfo()[result->enemy_city_loss[i]->city_id].name;
				fallenCityName += SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_STATUS_CITYNAME_PLUS_FALLEN"];
				t_looted_city_name->setString(fallenCityName);
				t_looted_city_coin->setString(StringUtils::format("%lld", result->enemy_city_loss[i]->robbed_coins));
				i_silver_looted->setPositionX(t_looted_city_coin->getPositionX() - t_looted_city_coin->getBoundingBox().size.width - i_silver_looted->getContentSize().width);

				if (i >= result->n_my_city_lost)
				{
					p_city_own_2->setVisible(false);
				}
				else
				{
					auto t_loss_city_name = p_city_own_2->getChildByName<Text*>("label_looted");
					auto t_loss_city_coin = p_city_own_2->getChildByName<Text*>("label_looted_num");
					auto i_silver_loss = p_city_own_2->getChildByName<ImageView*>("image_silver");

					t_loss_city_name->setString(SINGLE_SHOP->getCitiesInfo()[result->my_city_lost[i]->city_id].name);
					t_loss_city_coin->setString(StringUtils::format("%lld", result->my_city_lost[i]->robbed_coins));
					i_silver_loss->setPositionX(t_loss_city_coin->getPositionX() - t_loss_city_coin->getBoundingBox().size.width - i_silver_loss->getContentSize().width);
				}
			}
		}
	}
	else
	{
		auto p_capital = dynamic_cast<Widget*>(l_content->getItem(2));
		auto p_city_own_1 = p_capital->getChildByName<Widget*>("panel_city_own_1");
		auto t_loss_num = p_city_own_1->getChildByName<Text*>("label_looted_num");
		auto i_silver_loss = p_city_own_1->getChildByName<ImageView*>("image_silver");
		//损失银币总数
		t_loss_num->setString("0");
		i_silver_loss->setPositionX(t_loss_num->getPositionX() - t_loss_num->getBoundingBox().size.width - i_silver_loss->getContentSize().width);

		auto p_city_enemy_1 = p_capital->getChildByName<Widget*>("panel_city_enemy_1");
		auto t_looted_num = p_city_enemy_1->getChildByName<Text*>("label_looted_num");
		auto i_silver_looted = p_city_enemy_1->getChildByName<ImageView*>("image_silver");
		//战利品银币总数
		t_looted_num->setString("0");
		i_silver_looted->setPositionX(t_looted_num->getPositionX() - t_looted_num->getBoundingBox().size.width - i_silver_looted->getContentSize().width);

		auto p_city_2 = dynamic_cast<Widget*>(l_content->getItem(3));
		p_city_2->setVisible(false);
	}
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2 + 3);
	addListViewBar(l_content, image_pulldown);
	if (!isScheduled(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds)))
	{
		this->schedule(schedule_selector(UINationWarEntrance::flushDefendPanelSeconds), 3.0f);
	}
}
void UINationWarEntrance::flushPrepareOrWarTime(GetCityPrepareStatusResult *result)
{
	if (result->is_in_war==2)//战斗阶段
	{
		int64_t inWarTime = 0;
		for (int i = 0; i < result->n_fight_nations; i++)
		{
			if (result->fight_nations[i]->time_sign == 1)
			{
				inWarTime = (inWarTime >= result->fight_nations[i]->dis_to_end ? inWarTime : result->fight_nations[i]->dis_to_end);
			}
		}
		m_Times = inWarTime;
	}
	else if (result->is_in_war == 1) //准备阶段
	{
		int64_t prepareTime = 8 * 3600;
		for (int i = 0; i < result->n_fight_nations; i++)
		{
			if (result->fight_nations[i]->time_sign == 0)
			{
				prepareTime = (prepareTime <= result->fight_nations[i]->dis_to_end ? prepareTime : result->fight_nations[i]->dis_to_end);
			}
		}
		m_Times = prepareTime;
	}
	else if (result->is_in_war == 0)//没有战争
	{
		//button_callBack();
	}
}
void UINationWarEntrance::costResultPanle(const int64_t nums,int costType)
{
	openView(COMMOM_COCOS_RES[C_VIEW_SALVAGE_COIN]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SALVAGE_COIN]);
	auto label_repair = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_repair"));
	label_repair->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_DRINK_CONFIRM_TITLE"]);
	auto label_cost_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cost_num"));
	label_cost_num->setString(numSegment(StringUtils::format("%lld", nums)));
	auto image_silver_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_silver_1"));
	image_silver_1->loadTexture(SINGLE_COIN_PIC);
	label_cost_num->setPositionX(image_silver_1->getPositionX() +label_cost_num->getContentSize().width / 2 + 20);
	if (costType ==1)
	{
		image_silver_1->loadTexture(SINGLE_V_PIC);
	}
	
}