#include "UIShipyard.h"
#include "TaskVar.h"
#include "UIShipyardFleet.h"
#include "UIShipyardEquip.h"
#include "UIChatHint.h"
#include "CompanionTaskManager.h"
#include "MainTaskManager.h"
#include "UIStore.h"
#include "UICommon.h"
#include "UIInform.h"
#include "Utils.h"
#include "UISocial.h"
#include "TimeUtil.h"
#include "ModeLayerManger.h"
void convertToVector(char* _src, std::vector<int>& arr)
{
	std::string src(_src);
	if (src.empty())
	{
		return;
	}
	int prePos = 0;
	for (size_t i = 0; i != std::string::npos; i++)
	{
		prePos = i;
		i = src.find(",", prePos);
		if (i == std::string::npos)
		{
			break;
		}
		int id = atoi(src.substr(prePos, i - prePos).c_str());
		arr.push_back(id);
	}
	arr.push_back(atoi(src.substr(prePos).c_str()));

}

UIShipyard::UIShipyard():
	m_pFleetView(nullptr),
	m_pShipEquipView(nullptr),
	m_pMainPressButton(nullptr),
	m_pMinorPressButton(nullptr),
	m_pTempButton(nullptr),
	m_pTempButton2(nullptr),
	m_pEquipForce(nullptr),
	m_nOptionItem(-1),
	m_nPostionFlag(-1),
	m_nMainIndex(-1),
	m_nGlobalIndex(-1),
	m_nTempIndex(-1),
	m_nConfirmIndex(CONFIRM_INDEX_SHIP_REINFORCE),
	m_nShipRepairIndex(REPAIR_GOLD)
{
	m_eUIType = UI_DOCK;
	m_pFleetShipResult = nullptr;
	m_pEquipDrawingsResult = nullptr;
	m_pShipBuildingResult = nullptr;
	m_pShipDrawingsResult = nullptr;
	m_pEquipBuildingResult = nullptr;
	m_pReinforceEquipResult = nullptr;
	m_pReinforceShipResult = nullptr;
	m_bIsAllDrawingDefine = false;
	m_vCurrentCityCanShipBuild.clear();
	m_vCurrentCityNoShipBuild.clear();
	m_vCurrentCityCanEquipBuild.clear();
	m_vCurrentCityNoEquipBuild.clear();
	m_vUpdateTimeList.clear();
	m_vUpdateTotleTimeList.clear();
	m_equipmentsId.clear();
	m_bFleetFinishData = true;
	m_bShipEquipFinishData = true;
	m_bFirstGetEquip = true;
	m_nAll_city_dock_ship_num = 0;
	m_repairView = SHIPYARD_REPAIR_FLEET;
	m_loadingLayer = nullptr;
	m_bIsMaterialEnough = false;
	m_bIsCoinEnough = false;
	m_bIsRightCity = false;
	m_curSelectEquipment = nullptr;
}

UIShipyard::~UIShipyard()
{
	unregisterCallBack();

	CC_SAFE_RELEASE(m_pFleetView);
	CC_SAFE_RELEASE(m_pShipEquipView);

	if(m_pShipDrawingsResult)
	{
		get_drawings_result__free_unpacked(m_pShipDrawingsResult,0);
	}

	if (m_pShipBuildingResult)
	{
		get_building_ships_result__free_unpacked(m_pShipBuildingResult,0);
	}

	if (m_pEquipBuildingResult)
	{
		get_building_equipment_result__free_unpacked(m_pEquipBuildingResult,0);
	}
	if (m_pReinforceShipResult)
	{
		get_reinforce_ship_list_result__free_unpacked(m_pReinforceShipResult,0);
	}
	if (m_pReinforceEquipResult)
	{
		get_reinforce_equip_list_result__free_unpacked(m_pReinforceEquipResult,0);
	}
}

UIShipyard* UIShipyard::createDock()
{
	UIShipyard *dock = new UIShipyard;
	if (dock && dock->init())
	{
		dock->autorelease();
		return dock;
	}
	CC_SAFE_DELETE(dock);
	return nullptr;
}

bool UIShipyard::init()
{
	bool pRet = false;
	do 
	{
		CC_BREAK_IF(!UIBasicLayer::init());
		CompanionTaskManager::GetInstance()->reportCityArea(AREA__SHIPYARD_BOSS);
		registerCallBack();
		ProtocolThread::GetInstance()->getProficiencyValues(UILoadingIndicator::create(this));
		if (SINGLE_HERO->m_bGoshipFixDirect)
		{
			SINGLE_HERO->m_bGoshipFixDirect = false;
			m_nMainIndex = MAIN_REPAIR;
			initStaticData();
		}
		else
		{
			showChiefDialog();
		}

		//聊天
		auto ch = UIChatHint::createHint();
		this->addChild(ch, 10);
		pRet = true;
	} while (0);
	return pRet;
}

void UIShipyard::initStaticData()
{	
	openView(SHIPYARD_COCOS_RES[SHIPYARD_CSB]);
	//装备损坏的提示
    auto viewPanelTitle = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_CSB]);
	auto image_notify = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewPanelTitle, "image_notify"));
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
	if (m_nMainIndex == MAIN_FLEET)
	{
		b_notify = false;
	}
	image_notify->setVisible(b_notify);
	
	switch (m_nMainIndex)
	{
	case UIShipyard::MAIN_REPAIR:
		m_pTempButton = dynamic_cast<Button*>(Helper::seekWidgetByName(viewPanelTitle, "button_role"));
		ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::create(this));
		break;
	case UIShipyard::MAIN_FLEET:
		m_pTempButton = dynamic_cast<Button*>(Helper::seekWidgetByName(viewPanelTitle, "button_items"));
		ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::create(this));
		break;
	case UIShipyard::MAIN_WORKSHOP:
		m_pTempButton = dynamic_cast<Button*>(Helper::seekWidgetByName(viewPanelTitle, "button_localitems"));
		ProtocolThread::GetInstance()->getReinforceShipList(UILoadingIndicator::create(this));
		break;
	default:
		break;
	}

	m_pFleetView = UIShipyardFleet::createFleet(this);
	this->addChild(m_pFleetView, 10);
	m_pFleetView->retain();
	m_pShipEquipView = UIShipyardEquip::createShipEquip(this);
	this->addChild(m_pShipEquipView, 10);
	m_pShipEquipView->retain();
	this->schedule(schedule_selector(UIShipyard::updateBySecond), 1.0f);
}
/*
* 当剧情或者对话动作结束时调用该函数
* actionIndex, 当前动作的index
*/
void UIShipyard::notifyCompleted(int actionIndex)
{
	//扫尾工作
	switch (actionIndex){
	case MAIN_STORY:
		break;
	case COMPANION_STORY:
		break;
	case SMALL_STORY:
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
void UIShipyard::doNextJob(int actionIndex)
{
	actionIndex++;
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	auto viewTarven = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	switch (actionIndex)
	{
	case MAIN_STORY:
		//剧情（主线）
		//主线任务完成对话
		if (MainTaskManager::GetInstance()->checkTaskAction())
		{
			addNeedControlWidgetForHideOrShow(nullptr, true);
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
			
			auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
			auto bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
			auto p_actionbar = view->getChildByName<Widget*>("panel_actionbar");
			auto p_shipyard = view->getChildByName<Widget*>("panel_shipyard");
			addNeedControlWidgetForHideOrShow(bargirl_dialog, true);
			addNeedControlWidgetForHideOrShow(p_actionbar, false);
			addNeedControlWidgetForHideOrShow(p_shipyard, false);
			allTaskInfo(false, COMPANION_STORY, 0);
			CompanionTaskManager::GetInstance()->notifyUIPartDelayTime(0);
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
			addNeedControlWidgetForHideOrShow(nullptr, true);
			allTaskInfo(false, SMALL_STORY);
			notifyUIPartDelayTime(0.5);
		}
		else
		{
			notifyCompleted(SMALL_STORY);
		}
		break;
	default:
		break;
	}
}
void UIShipyard::flushCoinInfo(int64_t coin,int64_t gold)
{
	SINGLE_HERO->m_iCoin = coin;
	SINGLE_HERO->m_iGold = gold;
	Text* t_silver = nullptr;
	Text* t_gold = nullptr;
	
	auto view1 = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	if (view1)
	{
		t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName(view1,"label_silver_num"));
		t_gold = dynamic_cast<Text*>(Helper::seekWidgetByName(view1,"label_gold_num"));
	}
	auto view2 = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_REPAIR_CSB]);
	if (view2)
	{
		t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName(view2,"label_silver_num"));
		t_gold = dynamic_cast<Text*>(Helper::seekWidgetByName(view2,"label_gold_num"));
	}
 	auto view3 = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
 	if (view3)
 	{
 		t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName(view3, "label_silver_num"));
 		t_gold = dynamic_cast<Text*>(Helper::seekWidgetByName(view3, "label_gold_num"));
 	}
	if (view1 || view2 || view3)
	{
		t_silver->setString(numSegment(StringUtils::format("%lld", coin)));
		t_gold->setString(numSegment(StringUtils::format("%lld", gold)));
	}
	setTextFontSize(t_gold);
	setTextFontSize(t_silver);
}

void UIShipyard::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}

	auto button = static_cast<Widget*>(pSender);
  	std::string name = button->getName();
	UISocial::getInstance()->showChat(1);
	//商城
	if (isButton(button_silver))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType,1);
		return;
	}
	if (isButton(button_gold))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType,0);
		return;
	}
	buttonEventDis(button,name);
}

void UIShipyard::formationEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	std::string name = "button_items";
	auto button = dynamic_cast<Widget*>(Helper::seekWidgetByName(getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_CSB]),"button_items"));
	buttonEventDis(button,name);
}

void UIShipyard::buttonEventDis(Widget* target,std::string name)
{

	auto instance = ModeLayerManger::getInstance();
	auto layer = instance->getModelLayer();
	if (layer != nullptr && name != "button_cabin")
	{
		instance->removeLayer();
	}
	//回到船坞主界面
	if (isButton(button_last_widget))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->showChat(1);
		closeView();
		return;
	}
	//回到主城
	if (isButton(button_back))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->showChat(1);
		if (!m_bFleetFinishData)
		{
			m_bFleetFinishData = true;
			m_pFleetView->sendDataToServer();
		}
		
		if (!m_bShipEquipFinishData)
		{
			m_pShipEquipView->submitDataToserver();
		}
		
		button_callBack();
		return;
	}
	if (target && target->getTag() < 4)
	{
		if (m_pTempButton == target)
		{
			return;
		}
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		
		if (!m_bFleetFinishData)
		{
			m_bFleetFinishData = true;
			m_pFleetView->sendDataToServer();
		}
		
		if (!m_bShipEquipFinishData)
		{
			m_pShipEquipView->submitDataToserver();
		}
		m_pTempButton = target;
		int index = target->getTag();
	
		auto viewPanelTitle = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_CSB]);
		auto image_notify = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewPanelTitle, "image_notify"));
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
		if (index == MAIN_FLEET)
		{
			b_notify = false;
		}
		image_notify->setVisible(b_notify);

		switch (index)
		{
		case UIShipyard::MAIN_REPAIR:
			m_vUpdateTimeList.clear();
			m_vUpdateTotleTimeList.clear();
			m_repairView = SHIPYARD_REPAIR_FLEET;

			UISocial::getInstance()->showChat();
			ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::create(this));
			break;
		case UIShipyard::MAIN_FLEET:
			m_vUpdateTimeList.clear();
			m_vUpdateTotleTimeList.clear();
			m_pMinorPressButton = nullptr;
			m_pTempButton2 = nullptr;

			UISocial::getInstance()->showChat();
			ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::create(this));
			break;
		case UIShipyard::MAIN_WORKSHOP:
			m_pMinorPressButton = nullptr;
			m_pTempButton2 = nullptr;

			UISocial::getInstance()->showChat();
			ProtocolThread::GetInstance()->getReinforceShipList(UILoadingIndicator::create(this));
			break;
		default:
			break;
		}
		return;

	}
	//关闭
	if (isButton(button_close) || isButton(button_no) || isButton(button_result_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();

		//主线任务完成对话
		addNeedControlWidgetForHideOrShow(nullptr, true);
		if (MainTaskManager::GetInstance()->checkTaskAction())
		{
			allTaskInfo(false, MAIN_STORY);
			MainTaskManager::GetInstance()->notifyUIPartDelayTime(0.5);
		}
		return;
	}
	//舰队--舰队
	if (isButton(button_formation))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->showChat();
		if (!m_bShipEquipFinishData)
		{
			m_pShipEquipView->m_bIsFormationButtonSubmite = true;
			m_bShipEquipFinishData = true;
			m_pShipEquipView->submitDataToserver();
		}
		m_pTempButton2 = target;
		ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::create(this));
		return;
	}
	//舰队--装备
	if (isButton(button_gear_equip))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto viewLeftPanel = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_CSB]);
		auto image_leftnotify = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewLeftPanel, "image_notify"));
		image_leftnotify->setVisible(false);
		UISocial::getInstance()->showChat(4);
		if (!m_bFleetFinishData)
		{
			m_bFleetFinishData = true;
			m_pFleetView->sendDataToServer();
		}
		
		m_pTempButton2 = target;
		m_bFirstGetEquip = true;
		ProtocolThread::GetInstance()->getEquipShipInfo(0,UILoadingIndicator::create(this));
		return;
	}

	switch (m_nMainIndex)
	{
	case MAIN_REPAIR:
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->showChat();
		pageview_1_event(target,name);
		break;
	case MAIN_FLEET:
		if (m_pMinorPressButton == nullptr || strcmp(m_pMinorPressButton->getName().data(),"button_formation") == 0)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			UISocial::getInstance()->showChat();
			pageview_2_event_1(target,name);
		}else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			UISocial::getInstance()->showChat(4);
			pageview_2_event_2(target,name);
		}
		break;
	case MAIN_WORKSHOP:
		UISocial::getInstance()->showChat();
		pageview_3_event(target,name);
		break;
	default:
		break;
	}
	if (isButton(image_durable))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_weight))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_sailor))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_supply))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
		if (isButton(image_atk))
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushImageDetail(target);
			return;
		}
		if (isButton(image_def) || isButton(image_clock))
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushImageDetail(target);
			return;
		}
		if (isButton(image_speed) || isButton(image_silver))
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushImageDetail(target);
			return;
		}
		if (isButton(image_durable) || isButton(image_city_crafting))
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushImageDetail(target);
			return;
		}
		if (isButton(image_steering) || isButton(image_e_property))
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushImageDetail(target);
			return;
		}
	}

void UIShipyard::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
	case PROTO_TYPE_EquipShipResult:
		{
			EquipShipResult *result = (EquipShipResult *)message;
			if (result->failed == 0)
			{
				m_pShipEquipView->reflushShipInfo();
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_EQUIP_FAIL");
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
				flushCoinInfo(result->coin,result->gold);
				ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::create(this));
				Utils::consumeVTicket("4", 1, result->cost);
			}else if (result->failed == GOLD_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openGoldNotEnoughYesOrNo(result->cost);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_REPAIR_SHIP_FAIL");
			}
			break;
		}
	case PROTO_TYPE_FleetFormationResult:
		{
			FleetFormationResult *result = (FleetFormationResult *)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_DOCK_FLEET_SEND_SUCCESS");
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_FLEET_SEND_FAIL");
			}
			break;
		}
	case PROTO_TYPE_RepairAllShipsResult:
		{
			RepairAllShipsResult *result = (RepairAllShipsResult *)message;
			if (result->failed == 0)
			{
				flushCoinInfo(result->coin,result->gold);
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_DOCK_REPAIR_ALL_SUCCESS");
				ProtocolThread::GetInstance()->getFleetAndDockShips(UILoadingIndicator::create(this));
			}else if (result->failed == COIN_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_REPAIR_ALL_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetDrawingsResult:
		{
			GetDrawingsResult *result = (GetDrawingsResult *)message;
			if (result->failed == 0)
			{
				m_bIsAllDrawingDefine = false;
				m_pShipDrawingsResult = result;
				m_pEquipDrawingsResult = result;
				updateShipAndEquipDrawing(result);
				flushCoinInfo(result->coin, result->gold);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_GET_DRAWING_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetBuildingShipsResult:
		{
			if (m_pShipBuildingResult != nullptr)
			{
				get_building_ships_result__free_unpacked(m_pShipBuildingResult,0);
			}
			GetBuildingShipsResult* result = (GetBuildingShipsResult*)message;
			if (result->failed == 0)
			{
				m_pShipBuildingResult = result;
				updatePageView_3_shipBuild();
				changeMainButtonState(m_pTempButton);
			}else
			{
				m_pShipBuildingResult = nullptr;
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_GET_BUILDING_SHIP_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetBuildingEquipmentResult:
		{
			GetBuildingEquipmentResult* result = (GetBuildingEquipmentResult*)message;
			if (result->failed == 0)
			{
				auto _result = m_pEquipBuildingResult;
				m_pEquipBuildingResult = result;
				updatePageView_3_equipBuild();
				changeMainButtonState(m_pTempButton);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_GET_BUILDING_EQUIP_FAIL");
			}
			break;
		}
	case PROTO_TYPE_BuildShipResult:
		{
			BuildShipResult *result = (BuildShipResult*)message;
			if (result->failed == 0)
			{
				if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_building") == 0)
				{
					ProtocolThread::GetInstance()->getBuildingShips(UILoadingIndicator::create(this));
				}else
				{
					ProtocolThread::GetInstance()->getBuildingEquipment(1,0,UILoadingIndicator::create(this));
				}
				showBuildResult(result);
				flushCoinInfo(result->coin, result->gold);
			}else if (result->failed == COIN_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			}else
			{
				UIInform::getInstance()->openInformView(this);
				if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_building") == 0)
				{
					UIInform::getInstance()->openConfirmYes("TIP_DOCK_BUILD_SHIP_FAIL");
				}else
				{
					UIInform::getInstance()->openConfirmYes("TIP_DOCK_BUILD_SHIP_FAIL");
				}	
			}
			break;
		}
	case PROTO_TYPE_GetFleetAndDockShipsResult:
		{
			GetFleetAndDockShipsResult* result = (GetFleetAndDockShipsResult*)message;

			if (result->failed == 0)
			{
				closeView(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
				m_pFleetShipResult = result;				
				m_nAll_city_dock_ship_num = result->all_city_dock_ship_num;
				if (m_pTempButton->getTag() == MAIN_REPAIR)
				{
					updatePageView_1();
				}else
				{
					updatePageView_2_1();
					//目前检测主线
					notifyCompleted(NONE);
				}
				changeMainButtonState(m_pTempButton);
				flushCoinInfo(result->coins, result->golds);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_GET_FLEET_FAIL");
			}
			break;
		}
	case PROTO_TYPE_CancelBuildResult:
		{
			CancelBuildResult *result = (CancelBuildResult*)message;
			if (result->failed == 0)
			{
				flushCoinInfo(result->coin,result->gold);
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_DOCK_CANCEL_BUILD_SUCCESS");
				if (m_nConfirmIndex == CONFIRM_INDEX_SHIP_CANCEL)
				{
					ProtocolThread::GetInstance()->getBuildingShips(UILoadingIndicator::create(this));
				}
				else //if(m_nConfirmIndex == CONFIRM_INDEX_EQUIP_CANCEL)
				{
					ProtocolThread::GetInstance()->getBuildingEquipment(1, 0, UILoadingIndicator::create(this));
				}
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_CANCEL_BUILD_FAIL");
			}
			break;
		}
	case PROTO_TYPE_FinishBuildingResult:
		{
			FinishBuildingResult *result = (FinishBuildingResult*)message;
			if (result->failed == 0)
			{
				if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_building") == 0)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_DOCK_FINSH_BUILD_SUCCESS");
					ProtocolThread::GetInstance()->getBuildingShips(UILoadingIndicator::create(this));
					flushCoinInfo(result->coin, result->gold);
					Utils::consumeVTicket("5", 1, result->cost);
				}else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_DOCK_FINSH_EQUIP_BUILD_SUCCESS");
					ProtocolThread::GetInstance()->getBuildingEquipment(1,0,UILoadingIndicator::create(this));
					Utils::consumeVTicket("6", 1, result->cost);
				}
			}
			else if (result->failed == GOLD_NOT_FAIL || result->failed == 4)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openGoldNotEnoughYesOrNo(result->gold);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_FINSH_BUILD_FAIL");
			}
			break;
			
		}
	case PROTO_TYPE_GetReinforceShipListResult:
		{
			if (m_pReinforceShipResult != nullptr)
			{
				get_reinforce_ship_list_result__free_unpacked(m_pReinforceShipResult,0);
				m_pReinforceShipResult = nullptr;
			}
			GetReinforceShipListResult* result = (GetReinforceShipListResult*)message;
			if (result->failed == 0)
			{
				closeView(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
				m_pReinforceShipResult = result;
				updatePageView_3_ship();
				changeMainButtonState(m_pTempButton);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_REINFORCE_SHIP_LIST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetReinforceEquipListResult:
		{
			if (m_pReinforceEquipResult != nullptr)
			{
				get_reinforce_equip_list_result__free_unpacked(m_pReinforceEquipResult,0);
				m_pReinforceEquipResult = nullptr;
			}
			GetReinforceEquipListResult* result = (GetReinforceEquipListResult*)message;
			if (result->failed == 0)
			{
				m_pReinforceEquipResult = result;
				updatePageView_3_equip();
				changeMainButtonState(m_pTempButton);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_REINFORCE_EQUIP_LIST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_ReinforceEquipResult:
		{
			ReinforceEquipResult* result = (ReinforceEquipResult*)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_DOCK_REINFORCE_EQUIP_SUCCESS");
				ProtocolThread::GetInstance()->getReinforceEquipList(UILoadingIndicator::create(this));
				flushCoinInfo(result->coin, result->gold);
				if (m_nConfirmIndex == CONFIRM_INDEX_EQUIP_REINFORCE_CHANGE)
				{
					Utils::consumeVTicket("15", 1, result->cost);
				}
			}
			else if (result->failed == 10 || result->failed == 11)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_REINFORCE_EQUIP_LOSE");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_REINFORCE_EQUIP_FAIL");
			}
			break;
		}
	case PROTO_TYPE_ReinforceShipResult:
		{
			ReinforceShipResult* result = (ReinforceShipResult*)message;
			if (result->failed == 0)
			{
				ProtocolThread::GetInstance()->getReinforceShipList(UILoadingIndicator::create(this));
				flushCoinInfo(result->coin,result->gold);
				if (m_nConfirmIndex == CONFIRM_INDEX_SHIP_REINFORCE_CHANGE)
				{
					Utils::consumeVTicket("14", 1, result->cost);
				}
				//主线任务完成对话
				addNeedControlWidgetForHideOrShow(nullptr, true);
				if (MainTaskManager::GetInstance()->checkTaskAction())
				{
					allTaskInfo(false, MAIN_STORY);
					MainTaskManager::GetInstance()->notifyUIPartDelayTime(0.5);
				}
				else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_DOCK_REINFORCE_SHIP_SUCCESS");
				}

			}else if (result->failed == COIN_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			}else if (result->failed == GOLD_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openGoldNotEnoughYesOrNo(result->cost);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_REINFORCE_SHIP_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetEquipShipInfoResult:
		{
			GetEquipShipInfoResult* result = (GetEquipShipInfoResult*)message;
			if (result->failed == 0)
			{
				if (!m_bShipEquipFinishData && !m_bFirstGetEquip)
				{
					m_bShipEquipFinishData = true;
				}
				updatePageView_2_2(result);
				changeMainButtonState(m_pTempButton);
			}else if (result->failed == COIN_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			}else if (result->failed == GOLD_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYesOrNO("TIP_GOLD_NOT", "TIP_GOLD_NOT");
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_GET_EQUIP_SHIP_LIST_FAIL");
			}
			m_bFleetFinishData = true;
			break;
		}
	case PROTO_TYPE_GetEquipableItemsResult:
		{
			GetEquipableItemsResult* result = (GetEquipableItemsResult*)message;
			if (result->failed == 0)
			{
				m_pShipEquipView->setEquipView(result);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_GET_EQUIP_ENABLE_FAIL");
			}
			break;
		}
	case PROTO_TYPE_RerollEquipResult:
		{
			RerollEquipResult* result = (RerollEquipResult*)message;
			if (result->failed == 0)
			{
				flushCoinInfo(result->coin,result->gold);
				ProtocolThread::GetInstance()->getReinforceEquipList(UILoadingIndicator::create(this));
				Utils::consumeVTicket("20", 1, result->cost);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_EQUIP_RESET_FAIL");
			}
			break;
		}
	case PROTO_TYPE_RerollShipResult:
		{
			RerollShipResult* result = (RerollShipResult*)message;
			if (result->failed == 0)
			{
				flushCoinInfo(result->coin,result->gold);
				ProtocolThread::GetInstance()->getReinforceShipList(UILoadingIndicator::create(this));
				Utils::consumeVTicket("19", 1, result->cost);
			}else if (result->failed == GOLD_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openGoldNotEnoughYesOrNo(result->gold);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_SHIP_RESET_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetItemsDetailInfoResult:
		{
			GetItemsDetailInfoResult *result = (GetItemsDetailInfoResult*)message;
			if (result->failed == 0)
			{
				openGoodInfo(result, result->itemtype, result->itemid);
			}
			break;
		}
	case PROTO_TYPE_GetProficiencyValuesResult:
		{
			GetProficiencyValuesResult * result = (GetProficiencyValuesResult *)message;
			if (result->failed == 0)
			{
				m_proficiencyResult = result;
			}
			break;
		}
	default:
		break;
	}
}

void UIShipyard::changeMainButtonState(Widget* target)
{
	if (target == nullptr)
	{
		return;
	}
	if (m_pMainPressButton != nullptr)
	{
		m_pMainPressButton->setBright(true);
		m_pMainPressButton->setTouchEnabled(true);
		dynamic_cast<Text*>(m_pMainPressButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_NORMAL);
	}
	m_nMainIndex = target->getTag();
	target->setBright(false);
	m_pMainPressButton = target;
	m_pMainPressButton->setTouchEnabled(false);
	dynamic_cast<Text*>(m_pMainPressButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_PASSED);
	
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_CSB]);
	auto i_repair_bg = view->getChildByName<ImageView*>("image_bg");
	auto i_work_bg = view->getChildByName<ImageView*>("image_1");
	switch (m_nMainIndex)
	{
	case UIShipyard::MAIN_REPAIR:
		i_repair_bg->setVisible(true);
		i_work_bg->setVisible(false);
		break;
	case UIShipyard::MAIN_FLEET:
	case UIShipyard::MAIN_WORKSHOP:
		changeMinorButtonState(m_pTempButton2);
		i_repair_bg->setVisible(false);
		i_work_bg->setVisible(true);
		break;
	default:
		break;
	}
}

void UIShipyard::changeMinorButtonState(Widget* target)
{
	if (target == nullptr)
	{
		if (m_nMainIndex == MAIN_FLEET)
		{
			auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_CSB]);
			target = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_formation"));
		}else
		{
			auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_CSB]);
			target = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_ship_enhance"));
		}	
		m_pTempButton2 = target;
	}
	if (m_pMinorPressButton != nullptr)
	{
		m_pMinorPressButton->setBright(true);
		m_pMinorPressButton->setTouchEnabled(true);
		dynamic_cast<Text*>(m_pMinorPressButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}
	target->setBright(false);
	m_pMinorPressButton = target;
	m_pMinorPressButton->setTouchEnabled(false);
	dynamic_cast<Text*>(m_pMinorPressButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
}

void UIShipyard::pageview_1_event(Widget* target,std::string name)
{
	//银币修理
	if (isButton(button_s_yes)) 
	{
		if (m_repairView == SHIPYARD_REPAIR_FLEET)
		{
			ProtocolThread::GetInstance()->repairAllShips(0, UILoadingIndicator::create(this));
		}
		else
		{
			ProtocolThread::GetInstance()->repairAllShips(1, UILoadingIndicator::create(this));
		}	
		return;
	}
	//v票修理
	if (isButton(button_v_yes)) 
	{
		if (m_nShipRepairIndex == REPAIR_GOLD)
		{
			int id;
			if (m_repairView == SHIPYARD_REPAIR_FLEET)
			{
				if (m_pTempButton2->getTag() - START_INDEX < m_pFleetShipResult->n_fleetships)
				{
					id = m_pFleetShipResult->fleetships[m_pTempButton2->getTag() - START_INDEX]->id;
				}
				else
				{
					id = m_pFleetShipResult->catchships[m_pTempButton2->getTag() - START_INDEX - m_pFleetShipResult->n_fleetships]->id;
				}
			}
			else
			{
				id = m_pFleetShipResult->dockships[m_pTempButton2->getTag() - START_INDEX]->id;
			}

			ProtocolThread::GetInstance()->repairShipMaxHP(id, UILoadingIndicator::create(this));
		}
		else
		{
			ProtocolThread::GetInstance()->repairShipMaxHP(0,UILoadingIndicator::create(this));
		}
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
		
		if (m_repairView == SHIPYARD_REPAIR_FLEET)
		{
			for (auto i = 0; i < m_pFleetShipResult->n_fleetships + m_pFleetShipResult->n_catchships; i++)
			{
				ShipDefine* sd;
				if (i < m_pFleetShipResult->n_fleetships)
				{
					sd = m_pFleetShipResult->fleetships[i];
				}
				else
				{
					sd = m_pFleetShipResult->catchships[i - m_pFleetShipResult->n_fleetships];
				}
				if (sd->hp < sd->current_hp_max)
				{
					m_nShipRepairIndex = REPAIR_COIN;
					updateRepairShipDialog();
					return;
				}
			}
		}
		else
		{
			for (auto i = 0; i < m_pFleetShipResult->n_dockships; i++)
			{
				ShipDefine* sd = m_pFleetShipResult->dockships[i];
				if (sd->hp < sd->current_hp_max)
				{
					m_nShipRepairIndex = REPAIR_COIN;
					updateRepairShipDialog();
					return;
				}
			}
		}
		
		int n = 0;


		if (m_repairView == SHIPYARD_REPAIR_FLEET)
		{
			for (auto i = 0; i < m_pFleetShipResult->n_fleetships + m_pFleetShipResult->n_catchships; i++)
			{
				ShipDefine* sd;
				if (i < m_pFleetShipResult->n_fleetships)
				{
					sd = m_pFleetShipResult->fleetships[i];
				}
				else
				{
					sd = m_pFleetShipResult->catchships[i - m_pFleetShipResult->n_fleetships];
				}

				if (sd->current_hp_max < sd->hp_max)
				{
					n++;
				}
			}
		}
		else
		{
			for (auto i = 0; i < m_pFleetShipResult->n_dockships; i++)
			{
				ShipDefine* sd = m_pFleetShipResult->dockships[i];
				if (sd->current_hp_max < sd->hp_max)
				{
					n++;
				}
			}
		}

		if (n > 1)
		{
			m_nConfirmIndex = CONFIRM_INDEX_REPAIR_SHIP;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_DOCK_SHIP_REPAIR_COST_TITLE","TIP_DOCK_REPAIR_ALL_SHIP");
		}else if (n == 1)
		{
			m_nShipRepairIndex = REPAIR_GOLDS;
			updateRepairShipDialog();
		}else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_DOCK_NOT_REPAIR_SHIP");
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

	if (isButton(button_confirm_no))
	{
		return;
	}
	//修理解释
	if (isButton(button_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_DOCK_SHIP_REPAIR_MARK_TITLE","TIP_DOCK_SHIP_REPAIR_MARK_CONTENT");
		return;
	}
	//船只详情
	if (isButton(button_good_bg_1))
	{
		ShipDefine *shipInfo;
		if (m_repairView == SHIPYARD_REPAIR_FLEET)
		{
			if (target->getTag() - START_INDEX < m_pFleetShipResult->n_fleetships)
			{
				shipInfo = m_pFleetShipResult->fleetships[target->getTag() - START_INDEX];
			}
			else
			{
				shipInfo = m_pFleetShipResult->catchships[target->getTag() - START_INDEX - m_pFleetShipResult->n_fleetships];
			}
			
		}
		else
		{
			shipInfo = m_pFleetShipResult->dockships[target->getTag() - START_INDEX];
		}
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushShipDetail(shipInfo, shipInfo->sid, true);
		return;
	}
	if (isButton(button_switch))
	{
		m_loadingLayer = UILoadingIndicator::create(this);
		auto scene = _director->getRunningScene();
		scene->addChild(m_loadingLayer, 10000);
		if (m_repairView == SHIPYARD_REPAIR_DOCK)
		{
			m_repairView = SHIPYARD_REPAIR_FLEET;
		}
		else
		{
			m_repairView = SHIPYARD_REPAIR_DOCK;
		}
		updatePageView_1();
		return;
	}
}

void UIShipyard::setFleetFinishData(bool finsh)
{
	m_bFleetFinishData = finsh;
}

bool UIShipyard::getFleetFinishData()
{
	return m_bFleetFinishData;
}

void UIShipyard::setShipEquipFinishData(bool finsh)
{
	m_bShipEquipFinishData = finsh;
}

bool UIShipyard::getShipEquipFinishData()
{
	return m_bShipEquipFinishData;
}

void UIShipyard::pageview_2_event_1(Widget* target,std::string name)
{
	m_pFleetView->buttonEventDis(target,name);
}

void UIShipyard::pageview_2_event_2(Widget* target,std::string name)
{
	m_pShipEquipView->buttonEventDis(target,name);
}

void UIShipyard::pageview_3_event(Widget* target,std::string name)
{ 
	//强化方案详情
	if (isButton(button_captain_head))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openGoodInfo(nullptr,3,target->getTag());
		return;
	}
	//工坊--船只强化
	if (isButton(button_ship_enhance))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pTempButton2 = target;
		ProtocolThread::GetInstance()->getReinforceShipList(UILoadingIndicator::create(this));
		m_vUpdateTimeList.clear();
		m_vUpdateTotleTimeList.clear();
		return;
	}
	//工坊--装备强化
	if (isButton(button_gear_enhance))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pTempButton2 = target;
		ProtocolThread::GetInstance()->getReinforceEquipList(UILoadingIndicator::create(this));
		m_vUpdateTimeList.clear();
		m_vUpdateTotleTimeList.clear();
		return;
	}
	//工坊--船只制造
	if (isButton(button_ship_building))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pTempButton2 = target;
		ProtocolThread::GetInstance()->getBuildingShips(UILoadingIndicator::create(this));
		return;
	}
	//工坊--装备制造
	if (isButton(button_gear_crafting))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pTempButton2 = target;
		ProtocolThread::GetInstance()->getBuildingEquipment(1,0,UILoadingIndicator::create(this));
		return;
	}

	if (isButton(button_goods)) //当前城市的图纸
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bIsAllDrawingDefine = false;
		auto loading = UILoadingIndicator::create(this);
		auto curScene = Director::getInstance()->getRunningScene();
		curScene->addChild(loading,TAG_LAODING,TAG_LAODING);
		updateShipAndEquipDrawing(m_pShipDrawingsResult);
		return;
	}

	if (isButton(button_ships))//所有的图纸
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bIsAllDrawingDefine = true;
		auto loading = UILoadingIndicator::create(this);
		auto curScene = Director::getInstance()->getRunningScene();
		curScene->addChild(loading,TAG_LAODING,TAG_LAODING);
		updateShipAndEquipDrawing(m_pEquipDrawingsResult);
		return;
	}

	if (isButton(panel_ship_1)) //快速建造
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nGlobalIndex = target->getTag()-START_INDEX;
		showBuildItemDetails(target->getTag()-START_INDEX);
		return;
	}
	//点击强化
	if (isButton(button_enhance_slot) || isButton(image_enhance_bg_))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pEquipForce = nullptr;
		std::string name_temp;
		if (isButton(button_enhance_slot))
		{
			name_temp = target->getParent()->getName();
		}else
		{
			name_temp = target->getName();
		}
		m_nPostionFlag = atoi(name_temp.substr(name_temp.length()- 1).c_str());
		m_nGlobalIndex = target->getTag() - START_INDEX;	
		//船只
		if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_enhance") == 0)
		{
		
			int optionItem = 0;
			if (m_nPostionFlag == 1)
			{
				optionItem = m_pReinforceShipResult->ship[m_nGlobalIndex]->optionalitem1;
			}else
			{
				optionItem = m_pReinforceShipResult->ship[m_nGlobalIndex]->optionalitem2;
			}
			
			if(optionItem)
			{
				m_nOptionItem = optionItem;
				updateResetAndReplaceDialog(optionItem);
			}else
			{
				flushSpecialMaterial();
			}
		}else//装备
		{
			ReinforceEquipDefine *currentEquipDefine;
			if (m_nGlobalIndex < m_pReinforceEquipResult->n_equipmentonship)
			{
				currentEquipDefine = m_pReinforceEquipResult->equipmentonship[m_nGlobalIndex];
			}else
			{
				currentEquipDefine = m_pReinforceEquipResult->equipmentsinbag[m_nGlobalIndex - m_pReinforceEquipResult->n_equipmentonship];
			}
			if(currentEquipDefine->optionalitem)
			{
				m_nOptionItem = currentEquipDefine->optionalitem;
				updateResetAndReplaceDialog(currentEquipDefine->optionalitem);
			}else
			{
				flushSpecialMaterial();
			}
		}
		return;
	}
	//选择强化方案
	if (isButton(panel_captain))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_pEquipForce)
		{
			m_pEquipForce->getChildByName("image_press")->setVisible(false);
		}
		target->getChildByName("image_press")->setVisible(true);
		m_pEquipForce = target;
		auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CHOOSE_CSB]);
		auto btn_ok = dynamic_cast<Button*>(view->getChildByName<Widget*>("button_build"));
		auto b_enhance = btn_ok->getChildByName<Button*>("button_Enhance");	
		auto w_icon_silver = btn_ok->getChildByName<ImageView*>("image_silver");
		btn_ok->setBright(true);
		btn_ok->setTouchEnabled(true);
		b_enhance->setBright(true);
		m_nTempIndex = target->getTag() - START_INDEX;
		w_icon_silver->setOpacity(255);
		setGLProgramState(w_icon_silver, false);
		return;
	}
	//修改强化方案解释
	if (isButton(button_change_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_DOCK_CHANGE_REINFORCE_MARK_TITLE","TIP_DOCK_CHANGE_REINFORCE_MARK_CONTENT");
		return;
	}
	//重置强化方案解释
	if (isButton(button_redo_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_DOCK_RESET_REINFORCE_MARK_TITLE","TIP_DOCK_RESET_REINFORCE_MARK_CONTENT");
		return;
	}
	//迅速制造解释
	if (isButton(button_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_DOCK_SPEEDUP_BUILDING_MARK_TITLE","TIP_DOCK_SPEEDUP_BUILDING_MARK_CONTENT");
		return;
	}
	//制造栏解释
	if (isButton(button_doubt))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_DOCK_SHIP_BUILDING_MARK_TITLE","TIP_DOCK_SHIP_BUILDING_MARK_CONTENT");
		return;
	}
	//修改强化方案提示
	if (isButton(button_change))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();

		if (m_pReinforceShipResult->reinforceshipcostgold <= SINGLE_HERO->m_iGold)
		{
			flushSpecialMaterial(1);
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openGoldNotEnoughYesOrNo(m_pReinforceShipResult->reinforceshipcostgold);
		}
		return;
	}
	//重置强化方案提示
	if (isButton(button_redo))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		closeView();

		if (m_pReinforceShipResult->rerollshipcostgold <= SINGLE_HERO->m_iGold)
		{
			if (strcmp(m_pMinorPressButton->getName().data(), "button_ship_enhance") == 0)
			{
				m_nConfirmIndex = CONFIRM_INDEX_SHIP_REINFORCE_RESET;
				UICommon::getInstance()->flushVConfirmView("TIP_DOCK_SHIP_REINFORCE_RESET_TITLE",
					"TIP_DOCK_SHIP_REINFORCE_RESET_CONTENT", m_pReinforceShipResult->rerollshipcostgold);
			}
			else
			{
				m_nConfirmIndex = CONFIRM_INDEX_EQUIP_REINFORCE_RESET;
				UICommon::getInstance()->flushVConfirmView("TIP_DOCK_EQUIP_REINFORCE_RESET_TITLE",
					"TIP_DOCK_EQUIP_REINFORCE_RESET_CONTENT", m_pReinforceShipResult->rerollshipcostgold);
			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openGoldNotEnoughYesOrNo(m_pReinforceShipResult->rerollshipcostgold);
		}
		return;
	}
	//确认建造按钮
	if (isButton(button_building_cofirm))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		
		if (!m_bIsMaterialEnough)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_DOCK_BUILD_SHIP_OR_EQUIP_MATERIAL_NOT_ENOUGH");
			return;
		}
		if (!m_bIsRightCity)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_DOCK_BUILD_SHIP_OR_EQUIP_NOT_RIGHT_CITY");
			return;
		}
		if (!m_bIsCoinEnough)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_COIN_NOT");
			return;
		}
		closeView();
		closeView();
		if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_building") == 0)
		{
			ShipdrawingDefine* currentShipDrawing = nullptr;
			if (m_bIsAllDrawingDefine)
			{
				currentShipDrawing = m_pShipDrawingsResult->shipdrawing[m_nGlobalIndex];
			}else
			{
				currentShipDrawing = m_vCurrentCityCanShipBuild[m_nGlobalIndex];
			}
	
			ProtocolThread::GetInstance()->buildShip(currentShipDrawing->drawingid,UILoadingIndicator::create(this));
		}else
		{
			EquipdrawingDefine *currentEquipDrawing = nullptr;
			if (m_bIsAllDrawingDefine)
			{
				currentEquipDrawing = m_pEquipDrawingsResult->equipdrawing[m_nGlobalIndex];
			}else
			{
				currentEquipDrawing = m_vCurrentCityCanEquipBuild[m_nGlobalIndex];
			}

			ProtocolThread::GetInstance()->buildShip(currentEquipDrawing->drawingid,UILoadingIndicator::create(this));
		}
		return;
	}
	//船只--装备建造
	if (isButton(button_building))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (SINGLE_HERO->m_iHaslisence)
		{

			if (SINGLE_HERO->m_iEffectivelisence == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PALACE_LICENSE_DO");
			}
			else
			{
				if (strcmp(m_pMinorPressButton->getName().data(), "button_ship_building") == 0)
				{
					ProtocolThread::GetInstance()->getDrawings(0, 0, UILoadingIndicator::create(this));
				}
				else
				{
					ProtocolThread::GetInstance()->getDrawings(1, 0, UILoadingIndicator::create(this));
				}
			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_EXCHANGE_NOT_LICENSE");
		}
		return;
	}
	//选择强化方案后 提示花费界面
	if (isButton(button_build))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_enhance") == 0)
		{
			if (target->getTag()-START_INDEX == 0)
			{
				m_nConfirmIndex = CONFIRM_INDEX_SHIP_REINFORCE;
				UICommon::getInstance()->flushSilverConfirmView("TIP_DOCK_SHIP_REINFORCE_COST_TITLE",
					"TIP_DOCK_SHIP_REINFORCE_COST_CONTENT",m_pReinforceShipResult->reinforceshipcostcoin);
			}else
			{
				m_nConfirmIndex = CONFIRM_INDEX_SHIP_REINFORCE_CHANGE;
				UICommon::getInstance()->flushVConfirmView("TIP_DOCK_SHIP_REINFORCE_CHANGE_TITLE",
					"TIP_DOCK_REINFORCE_CHANGE_CONTENT",m_pReinforceShipResult->reinforceshipcostgold);
			}	
		}else
		{
			if (target->getTag()-START_INDEX == 0)
			{
				m_nConfirmIndex = CONFIRM_INDEX_EQUIP_REINFORCE;
				UICommon::getInstance()->flushSilverConfirmView("TIP_DOCK_EQUIP_REINFORCE_COST_TITLE",
					"TIP_DOCK_EQUIP_REINFORCE_COST_CONTENT",m_pReinforceEquipResult->reinforceequipcostcoin);
			}else
			{
				m_nConfirmIndex = CONFIRM_INDEX_EQUIP_REINFORCE_CHANGE;
				UICommon::getInstance()->flushVConfirmView("TIP_DOCK_EQUIP_REINFORCE_CHANGE_TITLE",
					"TIP_DOCK_REINFORCE_CHANGE_CONTENT",m_pReinforceEquipResult->reinforceequipcostgold);
			}
		}
		return;
	}
	//确认按钮
	if (isButton(button_s_yes) || isButton(button_v_yes))
	{	
		//sound effect reinforce
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_PORP_REINFORCE_24);
		if (m_nConfirmIndex == CONFIRM_INDEX_SHIP_REINFORCE || m_nConfirmIndex == CONFIRM_INDEX_SHIP_REINFORCE_CHANGE)
		{
			closeView();
			if (m_nPostionFlag == 1)
			{
				ProtocolThread::GetInstance()->reinforceShip(m_pReinforceShipResult->ship[m_nGlobalIndex]->shipid,
					m_nTempIndex, 1, 1, UILoadingIndicator::create(this));
			}else
			{
				ProtocolThread::GetInstance()->reinforceShip(m_pReinforceShipResult->ship[m_nGlobalIndex]->shipid,
					m_nTempIndex, 2, 1, UILoadingIndicator::create(this));
			}
			return;
		}

		if (m_nConfirmIndex == CONFIRM_INDEX_EQUIP_REINFORCE || m_nConfirmIndex == CONFIRM_INDEX_EQUIP_REINFORCE_CHANGE)
		{
			closeView();
			ReinforceEquipDefine *currentEquipDefine;

			if (m_nGlobalIndex < m_pReinforceEquipResult->n_equipmentonship)
			{
				currentEquipDefine = m_pReinforceEquipResult->equipmentonship[m_nGlobalIndex];
			}else
			{
				currentEquipDefine = m_pReinforceEquipResult->equipmentsinbag[m_nGlobalIndex - m_pReinforceEquipResult->n_equipmentonship];
			}
			ProtocolThread::GetInstance()->reinforceEquip(currentEquipDefine->equipmentid,
				m_nTempIndex, 1, currentEquipDefine->shipid, currentEquipDefine->equipposition, UILoadingIndicator::create(this));
			return;
		}

		if (m_nConfirmIndex == CONFIRM_INDEX_SHIP_REINFORCE_RESET)
		{
			int shipId = m_pReinforceShipResult->ship[m_nGlobalIndex]->shipid;
			int option1 = m_pReinforceShipResult->ship[m_nGlobalIndex]->optionalitem1;
			int option2 = m_pReinforceShipResult->ship[m_nGlobalIndex]->optionalitem2;
			if (m_nPostionFlag == 1)
			{
				ProtocolThread::GetInstance()->rerollShip(shipId,option1,0,1,UILoadingIndicator::create(this));
			}else
			{
				ProtocolThread::GetInstance()->rerollShip(shipId,0,option2,1,UILoadingIndicator::create(this));
			}
			return;
		}

		if (m_nConfirmIndex == CONFIRM_INDEX_EQUIP_REINFORCE_RESET)
		{
			ReinforceEquipDefine *currentEquipDefine;
			if (m_nGlobalIndex < m_pReinforceEquipResult->n_equipmentonship)
			{
				currentEquipDefine = m_pReinforceEquipResult->equipmentonship[m_nGlobalIndex];
			}else
			{
				currentEquipDefine = m_pReinforceEquipResult->equipmentsinbag[m_nGlobalIndex - m_pReinforceEquipResult->n_equipmentonship];
			}
			int equipId = currentEquipDefine->equipmentid;
			int optionId = currentEquipDefine->optionalitem;
			ProtocolThread::GetInstance()->rerollEquip(equipId,optionId,1,currentEquipDefine->shipid,currentEquipDefine->equipposition,UILoadingIndicator::create(this));
			return;
		}

		if (m_nConfirmIndex == CONFIRM_INDEX_SHIP_SPEEDUP)
		{
			closeView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_SLOT_CSB]);
			ProtocolThread::GetInstance()->finishBuilding(m_pShipBuildingResult->ship[m_nGlobalIndex]->shipmanafactureid,UILoadingIndicator::create(this));
			return;
		}

		if (m_nConfirmIndex == CONFIRM_INDEX_SHIP_CANCEL)
		{
			closeView();
			ProtocolThread::GetInstance()->cancelBuild(m_pShipBuildingResult->ship[m_nGlobalIndex]->shipmanafactureid,UILoadingIndicator::create(this));
			return;
		}

		if (m_nConfirmIndex == CONFIRM_INDEX_EQUIP_SPEEDUP)
		{
			closeView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_SLOT_CSB]);
			ProtocolThread::GetInstance()->finishBuilding(m_pEquipBuildingResult->equipment[m_nGlobalIndex]->shipmanafactureid,UILoadingIndicator::create(this));
			return;
		}
		
		if (m_nConfirmIndex == CONFIRM_INDEX_EQUIP_CANCEL)
		{
			closeView();
			ProtocolThread::GetInstance()->cancelBuild(m_pEquipBuildingResult->equipment[m_nGlobalIndex]->shipmanafactureid,UILoadingIndicator::create(this));
			return;
		}
		return;
	}
	//制造界面详情
	if (isButton(panel_itemdetails))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nGlobalIndex = target->getTag() - START_INDEX;
		updateShipAndEquipDialog(target->getTag() - START_INDEX);
		return;
	}
	//快速建造按钮
	if (isButton(button_repair_v))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		m_nGlobalIndex = target->getTag() - START_INDEX;
		if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_building") == 0)
		{
			int gold = ceil((m_pShipBuildingResult->ship[m_nGlobalIndex]->endtime - m_pShipBuildingResult->ship[target->getTag() - START_INDEX]->currenttime)/3600.0);
			if (gold <= SINGLE_HERO->m_iGold)
			{
				m_nConfirmIndex = CONFIRM_INDEX_SHIP_SPEEDUP;
				UICommon::getInstance()->flushVConfirmView("TIP_DOCK_SPEED_UP_TITLE", "TIP_DOCK_SHIP_SPEED_UP_CONTENT", gold);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openGoldNotEnoughYesOrNo(gold);
			}
		}else
		{
			int gold = ceil((m_pEquipBuildingResult->equipment[m_nGlobalIndex]->endtime - m_pEquipBuildingResult->equipment[target->getTag() - START_INDEX]->currenttime)/3600.0);
			if (gold <= SINGLE_HERO->m_iGold)
			{
				m_nConfirmIndex = CONFIRM_INDEX_EQUIP_SPEEDUP;
				UICommon::getInstance()->flushVConfirmView("TIP_DOCK_SPEED_UP_TITLE", "TIP_DOCK_EQUIP_SPEED_UP_CONTENT", gold);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openGoldNotEnoughYesOrNo(gold);
			}
		}
		return;
	}
	//取消正在创建的船只或装备
	if (isButton(button_cancel_buiding))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		m_nGlobalIndex = target->getTag() - START_INDEX;
		if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_building") == 0)
		{
			m_nConfirmIndex = CONFIRM_INDEX_SHIP_CANCEL;
			UICommon::getInstance()->flushSilverConfirmView("TIP_DOCK_SHIP_SPEED_UP_CANCEL_TITLE",
				"TIP_DOCK_SHIP_SPEED_UP_CANCEL_CONTENT",m_pShipBuildingResult->ship[m_nGlobalIndex]->buildcost / 2);
		}else
		{
			m_nConfirmIndex = CONFIRM_INDEX_EQUIP_CANCEL;
			UICommon::getInstance()->flushSilverConfirmView("TIP_DOCK_EQUIP_SPEED_UP_CANCEL_TITLE",
				"TIP_DOCK_EQUIP_SPEED_UP_CANCEL_CONTENT",m_pEquipBuildingResult->equipment[m_nGlobalIndex]->buildcost / 2);
		}
		return;
	}
	//物品详情
	if (isButton(image_ship_bg) || isButton(button_good_bg_1))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		int index = target->getTag();
		if (index >= 2000)
		{
 			index -= 2000;	
			int id = m_equipmentsId.at(index*2);
			int iid = m_equipmentsId.at(index*2 + 1);
			ProtocolThread::GetInstance()->getItemsDetailInfo(iid, ITEM_TYPE_SHIP_EQUIP, id, UILoadingIndicator::create(this));
        	m_curSelectEquipment = target;
		}else
		{
			index -= 1000;
			int id = m_pReinforceShipResult->ship[index]->shipid;
			int iid = m_pReinforceShipResult->ship[index]->sid;
			ProtocolThread::GetInstance()->getItemsDetailInfo(iid, ITEM_TYPE_SHIP, id, UILoadingIndicator::create(this));
		}
		return;
	}
	//制造船只--船只详情
	if (isButton(image_draw_bg_1))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openGoodInfo(nullptr, ITEM_TYPE_SHIP, target->getTag() - START_INDEX);
		return;
	}
	//制造装备--装备详情
	if (isButton(image_draw_bg_2))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openGoodInfo(nullptr, ITEM_TYPE_SHIP_EQUIP, target->getTag() - START_INDEX);
		return;
	}
	//物品详情
	if (isButton(button_good_bg))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openGoodInfo(nullptr, ITEM_TYPE_DRAWING, target->getTag() - START_INDEX);
		return;
	}
	//技能详情
	if (isButton(image_skill_bg_2_2) || isButton(image_skill_bg_1))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_MANUFACTURING_EXPERT;
		skillDefine.lv = atoi((target->getChildByName<Text*>("text_item_skill_lv")->getString()).data());
		skillDefine.skill_type = SKILL_TYPE_PLAYER;
		skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushSkillView(skillDefine);
		return;
	}
	//技能详情
	if (isButton(image_skill_bg_2))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		SKILL_DEFINE skillDefine;
		skillDefine.id = SKILL_SHIPBUILDING_TECHNOLOGY;
		skillDefine.lv = atoi((target->getChildByName<Text*>("text_item_skill_lv")->getString()).data());
		skillDefine.skill_type = SKILL_TYPE_PLAYER;
		skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
		UICommon::getInstance()->flushSkillView(skillDefine);
		return;
	}
	//物品详情
	if (isButton(button_reinforce_slot))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushItemsDetail(nullptr, target->getTag() - START_INDEX, true);
		return;
	}
	//工坊--建造时--物品详情
	if (isButton(button_ship_bg))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		if (strcmp(m_pMinorPressButton->getName().data(), "button_ship_building") == 0)
		{
			UICommon::getInstance()->flushShipDetail(nullptr, target->getTag() - START_INDEX, true);
		}
		else
		{
			UICommon::getInstance()->flushEquipsDetail(nullptr, target->getTag() - START_INDEX, true);
		}
		return;
	}
	//v票不足进入商城
	if (isButton(button_confirm_yes))
	{
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
		return;
	}
	//制造界面的提示按钮
	if (isButton(button_warning))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushWarningLongText("TIP_DOCK_BUILD_SHIP_OR_EQUIP_HIGHCOST_TITLE", "TIP_DOCK_BUILD_SHIP_OR_EQUIP_HIGHCOST_CONTENT");
		return;
	}
}

void UIShipyard::flushSpecialMaterial(int flag)
{
	openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CHOOSE_CSB]);
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CHOOSE_CSB]);
	auto lv_left = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_captain"));
	auto w_item = view->getChildByName<Widget*>("panel_captain");
	w_item->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	auto btn_ok = dynamic_cast<Button*>(view->getChildByName<Widget*>("button_build"));
	auto w_icon_silver = btn_ok->getChildByName<ImageView*>("image_silver");
	auto t_silver = btn_ok->getChildByName<Text*>("label_silver_num");	
	auto b_enhance = btn_ok->getChildByName<Button*>("button_Enhance");	
	btn_ok->setBright(false);
	btn_ok->setTouchEnabled(false);
	b_enhance->setBright(false);
	b_enhance->setTouchEnabled(false);
	btn_ok->setTag(flag+START_INDEX);
	lv_left->removeAllChildrenWithCleanup(true);
	
	if (flag == 0)
	{
		m_nOptionItem = 0;
		w_icon_silver->loadTexture("res/Vticket_coin/silver.png");
		w_icon_silver->setOpacity(128);
	}else
	{
		w_icon_silver->loadTexture("res/Vticket_coin/v_ticket.png");
		setGLProgramState(w_icon_silver,true);
	}

	int n_item = 0;
	ItemDefine** items = new ItemDefine*[m_pReinforceShipResult->n_item];
	
	if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_enhance") == 0)
	{
		n_item = m_pReinforceShipResult->n_item;
		memcpy(items,m_pReinforceShipResult->item,sizeof(ItemDefine*)*n_item);

		if (flag == 0)
		{
			std::string gold = StringUtils::format("%d",m_pReinforceShipResult->reinforceshipcostcoin);
			t_silver->setString(numSegment(gold));
		}else
		{
			std::string gold = StringUtils::format("%d",m_pReinforceShipResult->reinforceshipcostgold);
			t_silver->setString(numSegment(gold));
		}
	}else
	{
		int iid = 0;
		if (m_nGlobalIndex < m_pReinforceEquipResult->n_equipmentonship)
		{
			iid = m_pReinforceEquipResult->equipmentonship[m_nGlobalIndex]->iid;;
		}else
		{
			iid = m_pReinforceEquipResult->equipmentsinbag[m_nGlobalIndex - m_pReinforceEquipResult->n_equipmentonship]->iid;;
		}
		int curEquipType = SINGLE_SHOP->getItemData()[iid].sub_type;
		for (int i = 0; i < m_pReinforceEquipResult->n_item; i++)
		{
			int itemIid = m_pReinforceEquipResult->item[i]->item_id;
			int sub_item_type = SINGLE_SHOP->getItemData()[m_pReinforceEquipResult->item[i]->item_id].property4;
			switch (curEquipType)
			{
			case SUB_TYPE_SHIP_FIGUREHEAD:
				break;
			case SUB_TYPE_SHIP_EMBOLON:
				break;
			case SUB_TYPE_SHIP_STERN_GUN:
				break;
			case SUB_TYPE_SHIP_SAIL:
				if (sub_item_type == SUB_SUB_TYPE_EQUIP_SAIL_PLAN && itemIid != m_nOptionItem)
				{
					items[n_item] = m_pReinforceEquipResult->item[i];
					n_item++;
				}
				break;
			case SUB_TYPE_SHIP_ABANDON:
				break;
			case SUB_TYPE_SHIP_ARMOUR:
				if (sub_item_type == SUB_SUB_TYPE_EQUIP_ARM_PLAN && itemIid != m_nOptionItem)
				{
					items[n_item] = m_pReinforceEquipResult->item[i];
					n_item++;
				}
				break;
			case SUB_TYPE_SHIP_GUN:
				if (sub_item_type == SUB_SUB_TYPE_EQUIP_GUN_PLAN && itemIid != m_nOptionItem)
				{
					items[n_item] = m_pReinforceEquipResult->item[i];
					n_item++;
				}
				break;
			default:
				break;
			}
		}
		
		if (flag == 0)
		{
			std::string gold = StringUtils::format("%d",m_pReinforceEquipResult->reinforceequipcostcoin);
			t_silver->setString(numSegment(gold));
		}else
		{
			std::string gold = StringUtils::format("%d",m_pReinforceEquipResult->reinforceequipcostgold);
			t_silver->setString(numSegment(gold));
		}
	}
	 
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	auto t_no1 = dynamic_cast<Text*>(view->getChildByName("label_no_enhance_1"));
	auto t_no2 = dynamic_cast<Text*>(view->getChildByName("label_no_enhance_2"));
	if (n_item < 1)
	{
		t_no1->setVisible(true);
		t_no2->setVisible(true);
		image_pulldown->setVisible(false);
		return;
	}
	t_no1->setVisible(false);
	t_no2->setVisible(false);
	
	for (int i = 0; i < n_item; ++i)
	{
		auto w_equip = w_item->clone();
		int iid = items[i]->item_id;
		if (iid == m_nOptionItem)
		{
			if (n_item == 1)
			{
				t_no1->setVisible(true);
				t_no2->setVisible(true);
				button_pulldown->setVisible(false);
			}
			continue;
		}
		std::string itemName = getItemName(iid);
		std::string itemPath = getItemIconPath(iid);
		std::string attrSrc = SINGLE_SHOP->getItemData()[iid].property2;
		std::string value1 = "";
		std::string value2 = "";
		int index1 = -1;
		int index2 = -1;
		UICommon::getInstance()->getNotZeroFromString(attrSrc, value1, value2, index1, index2);
		w_equip->setTag(iid + START_INDEX);
		
		auto t_name = dynamic_cast<Text*>(w_equip->getChildByName<Widget*>("label_name"));
		auto w_icon = w_equip->getChildByName<Widget*>("button_captain_head");
		auto i_icon_bg = w_icon->getChildByName<ImageView*>("image_item_bg_lv");
		auto i_icon = dynamic_cast<ImageView*>(w_icon->getChildByName<Widget*>("image_item"));
		auto t_num = dynamic_cast<Text*>(w_icon->getChildByName<Widget*>("text_item_num"));

		auto i_weight = dynamic_cast<ImageView*>(w_equip->getChildByName<Widget*>("image_weight"));
		auto t_weight = dynamic_cast<Text*>(w_equip->getChildByName<Widget*>("label_weight_num"));
		auto i_sailor = dynamic_cast<ImageView*>(w_equip->getChildByName<Widget*>("image_sailor"));
		auto t_sailor = dynamic_cast<Text*>(i_sailor->getChildByName<Widget*>("label_sailor_num"));

		w_icon->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
		w_icon->setTag(iid);
		t_name->setString(itemName);
		setTextSizeAndOutline(t_num,items[i]->currentnum);
		setBgButtonFormIdAndType(w_icon, iid, ITEM_TYPE_PORP);
		setBgImageColorFormIdAndType(i_icon_bg, iid, ITEM_TYPE_PORP);
		setTextColorFormIdAndType(t_name, iid, ITEM_TYPE_PORP);
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(itemPath);
		
		if (index1 == -1)
		{
			i_weight->setVisible(false);
		}
		else
		{
			i_weight->setVisible(true);
		}

		if (index2 == -1)
		{
			i_sailor->setVisible(false);
		}
		else
		{
			i_sailor->setVisible(true);
		}

		if (SINGLE_SHOP->getItemData()[iid].sub_type == SUB_TYPE_EQUIP_PLAN)
		{
			//特殊处理---船只是从1开始 装备是从0开始
			if (index1 != -1)
			{
				if (atoi(value1.c_str()) < 0)
				{
					t_weight->setString(StringUtils::format("+(%s)", value2.c_str()));
					i_weight->loadTexture(PROPERTY_ICON_PTAH[index2 - 1]);
				}
				else
				{
					t_weight->setString(StringUtils::format("+(%s)", value1.c_str()));
					i_weight->loadTexture(PROPERTY_ICON_PTAH[index1 - 1]);
				}
			}

			if (index2 != -1)
			{
				if(atoi(value1.c_str()) < 0)
				{
					t_sailor->setString(StringUtils::format("%s", value1.c_str()));
					t_sailor->setTextColor(TEXT_RED);
					i_sailor->loadTexture(PROPERTY_ICON_PTAH[index1 - 1]);
				}
				else
				{
					if (atoi(value2.c_str()) < 0)
					{
						t_sailor->setString(StringUtils::format("%s", value2.c_str()));
						t_sailor->setTextColor(TEXT_RED);
					}
					else
					{
						t_sailor->setString(StringUtils::format("+(%s)", value2.c_str()));
					}
					i_sailor->loadTexture(PROPERTY_ICON_PTAH[index2 - 1]);
				}
			}
		}
		else
		{
			if (index1 != -1)
			{
				if (atoi(value1.c_str()) < 0)
				{
					t_weight->setString(StringUtils::format("+(%s)", value2.c_str()));
					i_weight->loadTexture(PROPERTY_ICON_PTAH[index2]);
				}
				else
				{
					t_weight->setString(StringUtils::format("+(%s)", value1.c_str()));
					i_weight->loadTexture(PROPERTY_ICON_PTAH[index1]);
				}
			}

			if (index2 != -1)
			{
				if (atoi(value1.c_str()) < 0)
				{
					t_sailor->setString(StringUtils::format("%s", value1.c_str()));
					t_sailor->setTextColor(TEXT_RED);
					i_sailor->loadTexture(PROPERTY_ICON_PTAH[index1]);
				}
				else
				{
					if (atoi(value2.c_str()) < 0)
					{
						t_sailor->setString(StringUtils::format("%s", value2.c_str()));
						t_sailor->setTextColor(TEXT_RED);
					}
					else
					{
						t_sailor->setString(StringUtils::format("+(%s)", value2.c_str()));
					}
					i_sailor->loadTexture(PROPERTY_ICON_PTAH[index2]);
				}
			}
		}
		i_weight->setColor(Color3B(58, 30, 2));
		i_sailor->setColor(Color3B(58, 30, 2));

		lv_left->pushBackCustomItem(w_equip);
	}
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(lv_left,image_pulldown);
//	delete[] items;
}

void UIShipyard::updatePageView_1()
{
	if (m_pMainPressButton && (m_pMainPressButton->getTag() == MAIN_FLEET || m_pMainPressButton->getTag() == MAIN_WORKSHOP))
	{
		closeView();
		closeView();
	}

	openView(SHIPYARD_COCOS_RES[SHIPYARD_REPAIR_CSB]);
	auto lv_ships = m_pRoot->getChildByName<ListView*>("listview_ship");
	auto button_repair = m_pRoot->getChildByName<Button*>("button_repair");
	auto p_no = m_pRoot->getChildByName<Widget*>("panel_no");
	int n = 0;
	auto panel_ship = m_pRoot->getChildByName<Widget*>("panel_ship_1");
	auto b_switch = m_pRoot->getChildByName<Button*>("button_switch");
	auto t_fleet = m_pRoot->getChildByName<Text*>("text_fleet");
	t_fleet->setAnchorPoint(Vec2(0, 0.5));
	t_fleet->setPositionX(59.55);
	lv_ships->removeAllChildrenWithCleanup(true);
	auto tip = SINGLE_SHOP->getTipsInfo();
	lv_ships->setSwallowTouches(false);
	if (m_repairView == SHIPYARD_REPAIR_FLEET)
	{
		b_switch->setTitleText(tip["TIP_DOCK_REPAIR_SWITCH_1"]);
		t_fleet->setString(tip["TIP_DOCK_REPAIR_TEXT_1"]);
		for (int i = 0; i < m_pFleetShipResult->n_fleetships + m_pFleetShipResult->n_catchships; ++i)
		{
			ShipDefine *shipInfo;
			if (i < m_pFleetShipResult->n_fleetships)
			{
				shipInfo = m_pFleetShipResult->fleetships[i];
			}
			else
			{
				shipInfo = m_pFleetShipResult->catchships[i - m_pFleetShipResult->n_fleetships];
			}
			auto item = panel_ship->clone();
			item->setVisible(true);
			auto i_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_ship"));
			auto t_shipName = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_name"));
			auto l_durable = dynamic_cast<ListView*>(Helper::seekWidgetByName(item, "listview_ship_durable_num_1"));
			auto t_durable1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_durable_num_1"));
			auto t_durable2 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_durable_num_2"));
			auto b_repair = dynamic_cast<Button*>(Helper::seekWidgetByName(item, "button_repair_v"));
			auto p_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(item, "progressbar_durable"));
			auto i_durable = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_82"));
			auto t_position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_num"));

			i_ship->loadTexture(getShipIconPath(shipInfo->sid));
			if (shipInfo->user_define_name &&std::strcmp(shipInfo->user_define_name, "") != 0)
			{
				t_shipName->setString(shipInfo->user_define_name);
			}
			else
			{
				t_shipName->setString(getShipName(shipInfo->sid));
			}

			if (shipInfo->hp > shipInfo->current_hp_max)
			{
				shipInfo->hp = shipInfo->current_hp_max;
			}

			float temp = 0;
			if (shipInfo->current_hp_max < shipInfo->hp_max)
			{   //not normal
				t_durable1->setString(StringUtils::format("%d /", shipInfo->hp));
				t_durable2->setString(StringUtils::format(" %d", shipInfo->current_hp_max));
				t_durable2->setTextColor(Color4B(198, 2, 5, 255));
				i_durable->setVisible(true);
				if (shipInfo->current_hp_max != 0)
				{
					temp = (shipInfo->hp*1.0) / (shipInfo->current_hp_max*1.0);
				}
				p_durable->setPercent(87 * temp);
				b_repair->setBright(true);
				b_repair->setTag(i + START_INDEX);
				b_repair->setTouchEnabled(true);
				b_repair->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
				n++;
			}
			else
			{
				t_durable1->setString(StringUtils::format("%d /", shipInfo->hp));
				t_durable2->setString(StringUtils::format(" %d", shipInfo->hp_max));
				t_durable2->setTextColor(Color4B(40, 25, 13, 255));
				i_durable->setVisible(false);
				if (shipInfo->hp_max != 0)
				{
					temp = (shipInfo->hp*1.0) / (shipInfo->hp_max*1.0);
				}
				if (temp < 1)
				{
					n++;
				}
				p_durable->setPercent(100 * temp);
				b_repair->setBright(false);
				b_repair->setTouchEnabled(false);
			}
			auto i_ship_bg = dynamic_cast<Button*>(Helper::seekWidgetByName(item, "button_good_bg_1"));
			i_ship_bg->setTouchEnabled(true);
			i_ship_bg->setTag(i + START_INDEX);
			l_durable->refreshView();

			auto i_intensify = Helper::seekWidgetByName(i_ship_bg, "goods_intensify");
			if (i_intensify)
			{
				i_intensify->removeFromParentAndCleanup(true);
			}
			if (shipInfo->optionid1 > 0 || shipInfo->optionid2 > 0)
			{
				addStrengtheningIcon(i_ship_bg);
			}
			if (i < m_pFleetShipResult->n_fleetships)
			{
				t_position->setVisible(true);
				t_position->loadTexture(getPositionIconPath(i + 1));
			}
			else
			{
				t_position->setVisible(false);
			}
			
			setBgButtonFormIdAndType(i_ship_bg, shipInfo->sid, ITEM_TYPE_SHIP);
			setTextColorFormIdAndType(t_shipName, shipInfo->sid, ITEM_TYPE_SHIP);
			lv_ships->pushBackCustomItem(item);
		}
		if (m_loadingLayer != nullptr)
		{
			m_loadingLayer->removeFromParentAndCleanup(true);
		}
	}
	else if (m_repairView == SHIPYARD_REPAIR_DOCK)
	{
		b_switch->setTitleText(tip["TIP_DOCK_REPAIR_SWITCH_2"]);
		t_fleet->setString(tip["TIP_DOCK_REPAIR_TEXT_2"]);
		for (int i = 0; i < m_pFleetShipResult->n_dockships; ++i)
		{
			auto item = panel_ship->clone();
			item->setVisible(true);
			auto i_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_ship"));
			auto t_shipName = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_name"));
			auto l_durable = dynamic_cast<ListView*>(Helper::seekWidgetByName(item, "listview_ship_durable_num_1"));
			auto t_durable1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_durable_num_1"));
			auto t_durable2 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_durable_num_2"));
			auto b_repair = dynamic_cast<Button*>(Helper::seekWidgetByName(item, "button_repair_v"));
			auto p_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(item, "progressbar_durable"));
			auto i_durable = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_82"));
			auto t_position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_num"));

			i_ship->loadTexture(getShipIconPath(m_pFleetShipResult->dockships[i]->sid));
			if (m_pFleetShipResult->dockships[i]->user_define_name &&std::strcmp(m_pFleetShipResult->dockships[i]->user_define_name, "") != 0)
			{
				t_shipName->setString(m_pFleetShipResult->dockships[i]->user_define_name);
			}
			else
			{
				t_shipName->setString(getShipName(m_pFleetShipResult->dockships[i]->sid));
			}

			if (m_pFleetShipResult->dockships[i]->hp > m_pFleetShipResult->dockships[i]->current_hp_max)
			{
				m_pFleetShipResult->dockships[i]->hp = m_pFleetShipResult->dockships[i]->current_hp_max;
			}

			float temp = 0;
			if (m_pFleetShipResult->dockships[i]->current_hp_max < m_pFleetShipResult->dockships[i]->hp_max)
			{   //not normal
				t_durable1->setString(StringUtils::format("%d /", m_pFleetShipResult->dockships[i]->hp));
				t_durable2->setString(StringUtils::format(" %d", m_pFleetShipResult->dockships[i]->current_hp_max));
				t_durable2->setTextColor(Color4B(198, 2, 5, 255));
				i_durable->setVisible(true);
				if (m_pFleetShipResult->dockships[i]->current_hp_max != 0)
				{
					temp = (m_pFleetShipResult->dockships[i]->hp*1.0) / (m_pFleetShipResult->dockships[i]->current_hp_max*1.0);
				}
				p_durable->setPercent(87 * temp);
				b_repair->setBright(true);
				b_repair->setTag(i + START_INDEX);
				b_repair->setTouchEnabled(true);
				b_repair->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
				n++;
			}
			else
			{
				t_durable1->setString(StringUtils::format("%d /", m_pFleetShipResult->dockships[i]->hp));
				t_durable2->setString(StringUtils::format(" %d", m_pFleetShipResult->dockships[i]->hp_max));
				t_durable2->setTextColor(Color4B(40, 25, 13, 255));
				i_durable->setVisible(false);
				if (m_pFleetShipResult->dockships[i]->hp_max != 0)
				{
					temp = (m_pFleetShipResult->dockships[i]->hp*1.0) / (m_pFleetShipResult->dockships[i]->hp_max*1.0);
				}
				if (temp < 1)
				{
					n++;
				}
				p_durable->setPercent(100 * temp);
				b_repair->setBright(false);
				b_repair->setTouchEnabled(false);
			}
			auto i_ship_bg = dynamic_cast<Button*>(Helper::seekWidgetByName(item, "button_good_bg_1"));
			i_ship_bg->setTouchEnabled(true);
			i_ship_bg->setTag(i + START_INDEX);
			l_durable->refreshView();

			auto i_intensify = Helper::seekWidgetByName(i_ship_bg, "goods_intensify");
			if (i_intensify)
			{
				i_intensify->removeFromParentAndCleanup(true);
			}
			if (m_pFleetShipResult->dockships[i]->optionid1 > 0 || m_pFleetShipResult->dockships[i]->optionid2 > 0)
			{
				addStrengtheningIcon(i_ship_bg);
			}
			t_position->setVisible(false);
			auto i_bgImage = i_ship_bg->getChildByName<ImageView*>("image_item_bg_lv");
			setBgButtonFormIdAndType(i_ship_bg, m_pFleetShipResult->dockships[i]->sid, ITEM_TYPE_SHIP);
			setBgImageColorFormIdAndType(i_bgImage, m_pFleetShipResult->dockships[i]->sid, ITEM_TYPE_SHIP);
			setTextColorFormIdAndType(t_shipName, m_pFleetShipResult->dockships[i]->sid, ITEM_TYPE_SHIP);
			lv_ships->pushBackCustomItem(item);
		}
		if (m_loadingLayer != nullptr)
		{
			m_loadingLayer->removeFromParentAndCleanup(true);
		}
	}


	if (n > 0)
	{
		button_repair->setBright(true);
	}else
	{
		button_repair->setBright(false);
	}

	if (m_pFleetShipResult->n_fleetships < 1 && m_repairView == SHIPYARD_REPAIR_FLEET)
	{
		p_no->setVisible(true);
		p_no->getChildByName<Button*>("button_formation")->addTouchEventListener(CC_CALLBACK_2(UIShipyard::formationEvent,this));
	}
	else if (m_pFleetShipResult->n_dockships < 1 && m_repairView == SHIPYARD_REPAIR_DOCK)
	{
		p_no->setVisible(true);
		p_no->getChildByName<Button*>("button_formation")->addTouchEventListener(CC_CALLBACK_2(UIShipyard::formationEvent,this));
	}
	else
	{
		p_no->setVisible(false);
	}
}

void UIShipyard::updatePageView_2_1()
{
	if (m_pMainPressButton)
	{
		closeView();
		if (m_pMainPressButton->getTag() == MAIN_WORKSHOP)
		{		
			closeView();
		}
	}
	openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_CSB]);
	openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	auto viewFormation = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	auto button_finish = dynamic_cast<Button*>(Helper::seekWidgetByName(viewFormation, "button_finish"));
	button_finish->setVisible(false);
	//装备损坏的提示
	auto viewLeftPanel = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_CSB]);
	if (viewLeftPanel)
	{
		auto image_leftnotify = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewLeftPanel, "image_notify"));
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
		image_leftnotify->setVisible(b_notify);
	}
	

	m_pFleetView->updateFleetAndDockView(m_pFleetShipResult);
}
void UIShipyard::updatePageView_2_2(const GetEquipShipInfoResult* result)
{
	if (m_pMinorPressButton->getName() == "button_formation")
	{
		closeView();
		openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
	}	
	if (m_bFirstGetEquip)
	{
		m_pShipEquipView->setShipView(result,true);
		m_bFirstGetEquip = false;
	}
	else
	{
		m_pShipEquipView->setShipView(result);
	}
}

void UIShipyard::updatePageView_3_ship()
{
	if (m_pReinforceShipResult == nullptr)
	{
		return;
	}
	
	Widget* view = nullptr;

	if (!m_pMainPressButton)
	{
		openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_CSB]);
		openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CSB]);
		view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CSB]);
	}
	else
	{
		if (m_pMainPressButton->getTag() == MAIN_WORKSHOP)
		{
			view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CSB]);
			if (!view)
			{
				closeView();
				openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CSB]);
				view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CSB]);
			}
		}
		else
		{
			closeView();
			if (m_pMainPressButton->getTag() == MAIN_FLEET)
			{
				closeView();
			}
			openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_CSB]);
			openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CSB]);
			view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CSB]);
		}
	}
	auto lv_ship_container  = dynamic_cast<ListView*>(view->getChildByName("listview_container_ship"));
	auto lv_equip_container = dynamic_cast<ListView*>(view->getChildByName("listview_container_gear")); 
	auto  t_reinforceTitle = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_enhancable_ship"));
	t_reinforceTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_REINFORCE_LIST"]);
	lv_ship_container->setVisible(true);
	lv_equip_container->setVisible(false);
	
	auto w_item = view->getChildByName<Widget*>("panel_ship_enhance_bg")->clone();
	w_item->setVisible(true);
	lv_ship_container->removeAllChildrenWithCleanup(true);

	auto p_no = dynamic_cast<Widget*>(Helper::seekWidgetByName((Widget*)view,"panel_no"));
	p_no->setVisible(false);
	auto t_no1 = p_no->getChildByName<Text*>("label_no_item1");
	auto t_no2 = p_no->getChildByName<Text*>("label_no_item2");
	if (m_pReinforceShipResult->n_ship < 1)
	{
		lv_ship_container->pushBackCustomItem(w_item);
		w_item->setVisible(false);
		p_no->setVisible(true);
		t_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_NOT_SHIP_REINFORCE1"]);
		t_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_NOT_SHIP_REINFORCE2"]);
		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		image_pulldown->setVisible(false);
		return;
	}
	int fleetShipNum = 0;
	for (int i = 0; i < m_pReinforceShipResult->n_ship; i++)
	{
		if(m_pReinforceShipResult->ship[i]->position >= 0){
			fleetShipNum ++;
		}else{
			break;
		}
	}

	if(fleetShipNum > 0){
		int num = (fleetShipNum)/2;
		int idx = 0;
		while(idx<num){
			auto tmp = m_pReinforceShipResult->ship[idx];
			m_pReinforceShipResult->ship[idx] = m_pReinforceShipResult->ship[fleetShipNum - idx -1];
			m_pReinforceShipResult->ship[fleetShipNum - idx -1] = tmp;
			idx++;
		}
	}

	for (auto i = 0; i < m_pReinforceShipResult->n_ship; ++i)
	{
		auto w_curItem = w_item->clone();
		w_curItem->setTag(i+START_INDEX);
		auto *curShipInfo = m_pReinforceShipResult->ship[i];
		updateReinforceShip(w_curItem,curShipInfo,i);
		lv_ship_container->pushBackCustomItem(w_curItem);
	}
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 + 3);
	addListViewBar(lv_ship_container,image_pulldown);
}

void UIShipyard::updateResetAndReplaceDialog(int id)
{
	openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_SLOT_CSB]);
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_SLOT_CSB]);

	auto i_enhance_bg = dynamic_cast<ImageView*>(view->getChildByName("image_enhance_bg"));
	auto b_enhance = dynamic_cast<Button*>(i_enhance_bg->getChildByName("button_reinforce_slot"));
	auto i_enhance = b_enhance->getChildByName<ImageView*>("image_enhance");
	b_enhance->setTag(id + START_INDEX);

	auto t_property1 = dynamic_cast<Text*>(i_enhance_bg->getChildByName<Widget*>("label_e_property_num_1"));
	auto t_property2 = dynamic_cast<Text*>(i_enhance_bg->getChildByName<Widget*>("label_e_property_num_2"));
	auto i_property1 = dynamic_cast<ImageView*>(i_enhance_bg->getChildByName<Widget*>("image_e_property_1"));
	auto i_property2 = dynamic_cast<ImageView*>(i_enhance_bg->getChildByName<Widget*>("image_e_property_2"));
	auto l_num = dynamic_cast<Text*>(i_enhance_bg->getChildByName<Widget*>("label_num"));
	l_num->setVisible(false);
	std::string itemPath = getItemIconPath(id);
	i_enhance->ignoreContentAdaptWithSize(false);
	i_enhance->loadTexture(itemPath);

	//注意理解1、2的作用
	int index1[2] = { -1 };
	int index2[2] = { -1 };
	int value1[7] = { 0 };
	int value2[7] = { 0 };

	if (strcmp(m_pMinorPressButton->getName().data(), "button_ship_enhance") == 0)
	{
		if (m_pReinforceShipResult->ship[m_nGlobalIndex]->optionalvalue)
		{
			get_ship_enhance_value_array_from_string(m_pReinforceShipResult->ship[m_nGlobalIndex]->optionalvalue, &value1, &value2, &index1, &index2);
		}
	}
	else
	{
		ReinforceEquipDefine *currentEquipDefine;
		if (m_nGlobalIndex < m_pReinforceEquipResult->n_equipmentonship)
		{
			currentEquipDefine = m_pReinforceEquipResult->equipmentonship[m_nGlobalIndex];
		}
		else
		{
			currentEquipDefine = m_pReinforceEquipResult->equipmentsinbag[m_nGlobalIndex - m_pReinforceEquipResult->n_equipmentonship];
		}
		if (currentEquipDefine->optionalvalue)
		{
			get_ship_enhance_value_array_from_string(currentEquipDefine->optionalvalue, &value1, &value2, &index1, &index2);
		}
	}
	int index3 = -1;
	int index4 = -1;
	std::string value3;
	std::string value4;
	//那个槽
	if (m_nPostionFlag == 1)
	{
		index3 = index1[0];
		index4 = index1[1];
		if (index3 != -1)
		{
			value3 = StringUtils::format("%d", value1[index3]);
		}
		if (index4 != -1)
		{
			value4 = StringUtils::format("%d", value1[index4]);
		}
	}
	else
	{
		index3 = index2[0];
		index4 = index2[1];
		if (index3 != -1)
		{
			value3 = StringUtils::format("%d", value2[index3]);
		}
		if (index4 != -1)
		{
			value4 = StringUtils::format("%d", value2[index4]);
		}
	}


	int icon_index1 = index3;
	int icon_index2 = index4;
	if (strcmp(m_pMinorPressButton->getName().data(), "button_ship_enhance") == 0)
	{
		icon_index1++;
		icon_index2++;
	}
	else
	{

	}

	if (index3 != -1)
	{
		if (atoi(value3.c_str()) < 0)
		{
			t_property1->setString("+" + value4);
			i_property1->loadTexture(PROPERTY_ICON_PTAH[icon_index2]);
			i_property1->setTouchEnabled(true);
			i_property1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
			i_property1->setTag(icon_index2 +1 + IMAGE_INDEX2);
		}
		else
		{
			t_property1->setString("+" + value3);
			i_property1->loadTexture(PROPERTY_ICON_PTAH[icon_index1]);
			i_property1->setTouchEnabled(true);
			i_property1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
			i_property1->setTag(icon_index1 +1 + IMAGE_INDEX2);
		}

		i_property1->setColor(Color3B(58, 30, 2));
	}
	else
	{
		i_property1->setVisible(false);
		t_property1->setVisible(false);
	}
	if (index4 != -1)
	{
		if (atoi(value3.c_str()) < 0)
		{
			i_property2->loadTexture(PROPERTY_ICON_PTAH[icon_index1]);
			t_property2->setString(value3);
			t_property2->setTextColor(TEXT_RED);
			i_property2->setTouchEnabled(true);
			i_property2->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
			i_property2->setTag(icon_index1 +1 + IMAGE_INDEX2);
		}
		else
		{
			i_property2->loadTexture(PROPERTY_ICON_PTAH[icon_index2]);
			i_property2->setTouchEnabled(true);
			i_property2->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
			i_property2->setTag(icon_index2 +1 + IMAGE_INDEX2);
			if (atoi(value4.c_str()) < 0)
			{
				t_property2->setString(value4);
				t_property2->setTextColor(TEXT_RED);
;
			}
			else
			{
				t_property2->setString("+" + value4);
			}
		}

		i_property2->setColor(Color3B(58, 30, 2));
	}
	else
	{
		i_property2->setVisible(false);
		t_property2->setVisible(false);
	}
}

void UIShipyard::updateRepairShipDialog()				
{
	float GOLD_REPAIR = m_pFleetShipResult->shipyard_repair_ship_cost_gold;
	float COIN_REPAIR = m_pFleetShipResult->shipyard_repair_ship_cost_coin;
	int coin = 0;
	if (m_nShipRepairIndex == REPAIR_COIN)
	{
		if (m_repairView == SHIPYARD_REPAIR_FLEET)
		{
			for (int i = 0; i < m_pFleetShipResult->n_fleetships + m_pFleetShipResult->n_catchships; i++)
			{
				ShipDefine* shipInfo;
				if (i <  m_pFleetShipResult->n_fleetships)
				{
					shipInfo = m_pFleetShipResult->fleetships[i];
				}
				else
				{
					shipInfo = m_pFleetShipResult->catchships[i - m_pFleetShipResult->n_fleetships];
				}
				coin += shipInfo->current_hp_max - shipInfo->hp;
			}
		}
		else
		{
			for (int i = 0; i < m_pFleetShipResult->n_dockships; i++)
			{
				coin += m_pFleetShipResult->dockships[i]->current_hp_max - m_pFleetShipResult->dockships[i]->hp;
			}
		}
		coin = (int)ceil(coin * COIN_REPAIR);

		if(SINGLE_HERO->m_iCoin < coin)
		{
			//closeView();
			m_pTempButton2 = nullptr;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			return;
		}
	}else
	{
		if (m_nShipRepairIndex == REPAIR_GOLD)
		{
			if (m_pTempButton2)
			{
				int index = m_pTempButton2->getTag() - START_INDEX;
				if (m_repairView == SHIPYARD_REPAIR_FLEET)
				{
					ShipDefine* shipInfo;
					if (index <  m_pFleetShipResult->n_fleetships)
					{
						shipInfo = m_pFleetShipResult->fleetships[index];
					}
					else
					{
						shipInfo = m_pFleetShipResult->catchships[index - m_pFleetShipResult->n_fleetships];
					}
					coin += ceil((shipInfo->hp_max - shipInfo->current_hp_max) * GOLD_REPAIR * sqrt(SINGLE_SHOP->getShipData()[shipInfo->sid].rarity));
				}
				else
				{
					coin += ceil((m_pFleetShipResult->dockships[index]->hp_max - m_pFleetShipResult->dockships[index]->current_hp_max) * GOLD_REPAIR * sqrt(SINGLE_SHOP->getShipData()[m_pFleetShipResult->dockships[index]->sid].rarity));
				}
			}
		}else
		{
			if (m_repairView == SHIPYARD_REPAIR_FLEET)
			{
				for (int i = 0; i < m_pFleetShipResult->n_fleetships + m_pFleetShipResult->n_catchships; i++)
				{
					ShipDefine* shipInfo;
					if (i <  m_pFleetShipResult->n_fleetships)
					{
						shipInfo = m_pFleetShipResult->fleetships[i];
					}
					else
					{
						shipInfo = m_pFleetShipResult->catchships[i - m_pFleetShipResult->n_fleetships];
					}
					coin += ceil((shipInfo->hp_max - shipInfo->current_hp_max) * GOLD_REPAIR * sqrt(SINGLE_SHOP->getShipData()[shipInfo->sid].rarity));
				}
			}
			else
			{
				for (int i = 0; i < m_pFleetShipResult->n_dockships; i++)
				{
					coin += ceil((m_pFleetShipResult->dockships[i]->hp_max - m_pFleetShipResult->dockships[i]->current_hp_max) * GOLD_REPAIR * sqrt(SINGLE_SHOP->getShipData()[m_pFleetShipResult->dockships[i]->sid].rarity));
				}
			}
		}
		if(SINGLE_HERO->m_iGold < coin)
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
		UICommon::getInstance()->flushSilverConfirmView("TIP_DOCK_SHIP_REPAIR_COST_TITLE","TIP_DOCK_SHIP_REPAIR_COST_CONTENT",coin);
	}
	else
	{
		UICommon::getInstance()->flushVConfirmView("TIP_DOCK_SHIP_ADVREPAIR_COST_TITLE","TIP_DOCK_SHIP_ADVREPAIR_COST_CONTENT",coin);
	}
}
int* UIShipyard::convertToIntArr(char* _src,int* arr,int n_num)
{
	std::string src(_src);
	if(src.empty())
	{
		return nullptr;
	}
	int prePos = 0;
	int index = 0;
	for (size_t i = 0; i != std::string::npos;i++,index++)
	{	
		prePos = i;
		i = src.find(",",prePos);
		if (i == std::string::npos)
		{
			break;
		}
		int id = atoi(src.substr(prePos,i - prePos).c_str());
		arr[index] = id;
	}
	arr[index] = atoi(src.substr(prePos).c_str());
	return arr;
}

void UIShipyard::updateReinforceShip(Widget *root,const ReinforceShipDefine *result,const int i)
{
	auto w_one = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root,"image_enhance_bg_1"));
	auto w_two = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root,"image_enhance_bg_2"));
	w_one->setTag(i+START_INDEX);
	w_one->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	w_two->setTag(i+START_INDEX);
	w_two->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));

	if (result->optionalnum == 0)
	{
		w_one->setVisible(false);
		w_two->setVisible(false);
	}
	//注意理解1、2的作用
	int index1[2] = { -1 };
	int index2[2] = { -1 };
	int value1[7] = { 0 };
	int value2[7] = { 0 };
	if (result->optionalvalue)
	{
		get_ship_enhance_value_array_from_string(result->optionalvalue, &value1, &value2, &index1, &index2);
	}

	if (result->optionalnum == 1)
	{
		w_one->setVisible(true);
		w_two->setVisible(false);
		auto t_num = w_one->getChildByName<Text*>("label_num");
		//TODO 先做了隐藏处理
		t_num->setVisible(false);
		
		auto b_enhance_slot = w_one->getChildByName<Button*>("button_enhance_slot");
		auto i_enhance = b_enhance_slot->getChildByName<ImageView*>("image_enhance");
		b_enhance_slot->setTag(i+START_INDEX);
		i_enhance->ignoreContentAdaptWithSize(false);

		auto t_property_num1 = w_one->getChildByName<Text*>("label_e_property_num_1");
		auto t_property_num2 = w_one->getChildByName<Text*>("label_e_property_num_2");
		auto i_property_num1 = w_one->getChildByName<ImageView*>("image_e_property_1");
		auto i_property_num2 = w_one->getChildByName<ImageView*>("image_e_property_2");
		
		auto t_empty_slot = w_one->getChildByName<Text*>("label_empty_slot");

		if (result->optionalitem1 != 0)
		{
			if (index1[0] != -1)
			{
				if (value1[index1[0]] < 0)
				{
					t_property_num1->setString(StringUtils::format("+%d", value1[index1[1]]));
					i_property_num1->loadTexture(PROPERTY_ICON_PTAH[index1[1] + 1]);
				}
				else
				{
					t_property_num1->setString(StringUtils::format("+%d", value1[index1[0]]));
					i_property_num1->loadTexture(PROPERTY_ICON_PTAH[index1[0] + 1]);
				}
				i_property_num1->setColor(Color3B(58, 30, 2));
			}
			else
			{
				i_property_num1->setVisible(false);
				t_property_num1->setVisible(false);
			}
			if (index1[1] != -1)
			{
				if (value1[index1[0]] < 0)
				{
					t_property_num2->setString(StringUtils::format("-%d", -value1[index1[0]]));
					t_property_num2->setTextColor(TEXT_RED);
					i_property_num2->loadTexture(PROPERTY_ICON_PTAH[index1[0] + 1]);
				}
				else
				{
					if (value1[index1[1]] < 0)
					{
						t_property_num2->setString(StringUtils::format("-%d", -value1[index1[1]]));
						t_property_num2->setTextColor(TEXT_RED);
					}
					else
					{
						t_property_num2->setString(StringUtils::format("+%d", value1[index1[1]]));
					}
					i_property_num2->loadTexture(PROPERTY_ICON_PTAH[index1[1] + 1]);
				}
				i_property_num2->setColor(Color3B(58, 30, 2));
			}
			else
			{
				i_property_num2->setVisible(false);
				t_property_num2->setVisible(false);
			}

			t_empty_slot->setVisible(false);
			i_enhance->setVisible(true);
			i_enhance->loadTexture(getItemIconPath(result->optionalitem1));
//			t_num->setString("1/1");
		}else
		{
			t_property_num1->setVisible(false);
			t_property_num2->setVisible(false);
			t_empty_slot->setVisible(true);
			t_empty_slot->setPositionY(w_one->getBoundingBox().size.height/2);
			i_property_num1->setVisible(false);
			i_property_num2->setVisible(false);
			i_enhance->setVisible(false);
//			t_num->setString("0/1");
		}
	}

	if (result->optionalnum == 2)
	{
		w_one->setVisible(true);
		w_two->setVisible(true);
		auto t_num1 = w_one->getChildByName<Text*>("label_num");
		auto t_num2 = w_two->getChildByName<Text*>("label_num");
		//TODO 先做了隐藏处理
		t_num1->setVisible(false);
		t_num2->setVisible(false);

		auto b_enhance_slot = w_one->getChildByName<Button*>("button_enhance_slot");
		auto i_enhance = b_enhance_slot->getChildByName<ImageView*>("image_enhance");
		b_enhance_slot->setTag(i+START_INDEX);
		i_enhance->ignoreContentAdaptWithSize(false);

		auto t_property_num1 = w_one->getChildByName<Text*>("label_e_property_num_1");
		auto t_property_num2 = w_one->getChildByName<Text*>("label_e_property_num_2");
		auto i_property_num1 = w_one->getChildByName<ImageView*>("image_e_property_1");
		auto i_property_num2 = w_one->getChildByName<ImageView*>("image_e_property_2");
		auto t_empty_slot = w_one->getChildByName<Text*>("label_empty_slot");

		if (result->optionalitem1 != 0)
		{
			if (index1[0] != -1)
			{
				if (value1[index1[0]] < 0)
				{
					t_property_num1->setString(StringUtils::format("+%d", value1[index1[1]]));
					i_property_num1->loadTexture(PROPERTY_ICON_PTAH[index1[1] + 1]);
				}
				else
				{
					t_property_num1->setString(StringUtils::format("+%d", value1[index1[0]]));
					i_property_num1->loadTexture(PROPERTY_ICON_PTAH[index1[0] + 1]);
				}
				i_property_num1->setColor(Color3B(58, 30, 2));
			}
			else
			{
				i_property_num1->setVisible(false);
				t_property_num1->setVisible(false);
			}
			if (index1[1] != -1)
			{
				if (value1[index1[0]] < 0)
				{
					t_property_num2->setString(StringUtils::format("-%d", -value1[index1[0]]));
					t_property_num2->setTextColor(TEXT_RED);
					i_property_num2->loadTexture(PROPERTY_ICON_PTAH[index1[0] + 1]);
				}
				else
				{
					if (value1[index1[1]] < 0)
					{
						t_property_num2->setString(StringUtils::format("-%d", -value1[index1[1]]));
						t_property_num2->setTextColor(TEXT_RED);
					}
					else
					{
						t_property_num2->setString(StringUtils::format("+%d", value1[index1[1]]));
					}
					i_property_num2->loadTexture(PROPERTY_ICON_PTAH[index1[1] + 1]);
				}
				i_property_num2->setColor(Color3B(58, 30, 2));
			}
			else
			{
				i_property_num2->setVisible(false);
				t_property_num2->setVisible(false);
			}

			t_empty_slot->setVisible(false);
			i_enhance->setVisible(true);
			i_enhance->loadTexture(getItemIconPath(result->optionalitem1));
		}else
		{
			t_property_num1->setVisible(false);
			t_property_num2->setVisible(false);
			t_empty_slot->setVisible(true);
			t_empty_slot->setPositionY(w_one->getBoundingBox().size.height / 2);
			i_property_num1->setVisible(false);
			i_property_num2->setVisible(false);
			i_enhance->setVisible(false);
		}

		auto b_enhance_slot_2 = w_two->getChildByName<Button*>("button_enhance_slot");
		auto i_enhance_2 = b_enhance_slot_2->getChildByName<ImageView*>("image_enhance");
		b_enhance_slot_2->setTag(i+START_INDEX);
		//b_enhance_slot_2->addTouchEventListener(CC_CALLBACK_2(DockLayer::menuCall_func, this));
		i_enhance_2->ignoreContentAdaptWithSize(false);

		auto t_property_num1_2 = w_two->getChildByName<Text*>("label_e_property_num_1");
		auto t_property_num2_2 = w_two->getChildByName<Text*>("label_e_property_num_2");
		auto i_property_num1_2 = w_two->getChildByName<ImageView*>("image_e_property_1");
		auto i_property_num2_2 = w_two->getChildByName<ImageView*>("image_e_property_2");
		auto t_empty_slot_2 = w_two->getChildByName<Text*>("label_empty_slot");
		if (result->optionalitem2 != 0)
		{
			if (index2[0] != -1)
			{
				if (value2[index2[0]] < 0)
				{
					t_property_num1_2->setString(StringUtils::format("+%d", value2[index2[1]]));
					i_property_num1_2->loadTexture(PROPERTY_ICON_PTAH[index2[1] + 1]);
				}
				else
				{
					t_property_num1_2->setString(StringUtils::format("+%d", value2[index2[0]]));
					i_property_num1_2->loadTexture(PROPERTY_ICON_PTAH[index2[0] + 1]);
				}
				i_property_num1_2->setColor(Color3B(58, 30, 2));
			}
			else
			{
				i_property_num1_2->setVisible(false);
				t_property_num1_2->setVisible(false);
			}
			if (index2[1] != -1)
			{
				if (value2[index2[0]] < 0)
				{
					t_property_num2_2->setString(StringUtils::format("-%d", -value2[index2[0]]));
					t_property_num2_2->setTextColor(TEXT_RED);
					i_property_num2_2->loadTexture(PROPERTY_ICON_PTAH[index2[0] + 1]);
				}
				else
				{
					if (value2[index2[1]] < 0)
					{
						t_property_num2_2->setString(StringUtils::format("-%d", -value2[index2[1]]));
						t_property_num2_2->setTextColor(TEXT_RED);
					}
					else
					{
						t_property_num2_2->setString(StringUtils::format("+%d", value2[index2[1]]));
					}
					i_property_num2_2->loadTexture(PROPERTY_ICON_PTAH[index2[1] + 1]);
				}
				i_property_num2_2->setColor(Color3B(58, 30, 2));
			}
			else
			{
				i_property_num2_2->setVisible(false);
				t_property_num2_2->setVisible(false);
			}

			t_empty_slot_2->setVisible(false);
			i_enhance_2->setVisible(true);
			i_enhance_2->loadTexture(getItemIconPath(result->optionalitem2));
		}else
		{
			t_property_num1_2->setVisible(false);
			t_property_num2_2->setVisible(false);
			t_empty_slot_2->setVisible(true);
			t_empty_slot_2->setPositionY(w_two->getBoundingBox().size.height / 2);
			i_property_num1_2->setVisible(false);
			i_property_num2_2->setVisible(false);
			i_enhance_2->setVisible(false);
		}
	}

	std::string itemPath = getShipIconPath(result->sid);
	std::string itemName = result->shipname;
	if (result->shipname)
	{
		if (itemName.empty() || itemName == " ")
		{
			itemName = getShipName(result->sid);
		}
	}else
	{
		itemName = getShipName(result->sid);
	}
	
	auto i_icon_bg = root->getChildByName<ImageView*>("image_ship_bg");
	i_icon_bg->setTouchEnabled(true);
	i_icon_bg->setTag(i+1000);
	i_icon_bg->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	auto i_icon = dynamic_cast<ImageView*>(i_icon_bg->getChildByName("image_ship"));
	auto i_position = dynamic_cast<ImageView*>(i_icon_bg->getChildByName("image_num"));
	auto t_shipName = dynamic_cast<Text*>(root->getChildByName("label_ship_name"));
	auto i_durable = root->getChildByName<Widget*>("image_durable");
	auto l_durable = dynamic_cast<Widget*>(i_durable->getChildByName("panel_ship_durable_num_1"));
	auto t_durableNum1 = dynamic_cast<Text*>(l_durable->getChildByName("label_ship_durable_num_1"));
	auto t_durableNum2 = dynamic_cast<Text*>(l_durable->getChildByName("label_ship_durable_num_2"));

	auto t_sailorNum = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_sailors_num_1"));
	auto t_sailorNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_sailors_num_2"));

	auto t_weightNum = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_ship_weight_num_1"));
	auto t_weightNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_ship_weight_num_2"));

	auto t_supplyNum = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_ship_supply_num_1"));
	auto t_supplyNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_ship_supply_num_2"));

	auto t_atkNum = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_atk_num"));
	auto t_defNum = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_defnum"));
	auto t_speedNum = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_speed_num"));
	auto t_steeringNum = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_steering_num"));
	
	i_icon->loadTexture(itemPath);
	auto nMaxStringNum = 0;
	auto languageTypeNum = 0;
	if (isChineseCharacterIn(itemName.c_str()))
	{
		languageTypeNum = 1;
	}
	if (languageTypeNum)
	{
		nMaxStringNum = 15;
	}
	else
	{
		nMaxStringNum = 30;
	}
	t_shipName->setString(apostrophe(itemName, nMaxStringNum));
	if (result->position >= 0)
	{
		i_position->setVisible(true);
		i_position->loadTexture(getPositionIconPath(result->position + 1));
	}else
	{
		i_position->setVisible(false);
	}

	auto i_intensify = Helper::seekWidgetByName(i_icon_bg, "goods_intensify");
	if (i_intensify)
	{
		i_intensify->removeFromParentAndCleanup(true);
	}

	if (result->optionalitem1 > 0 || result->optionalitem2 > 0)
	{
		addStrengtheningIcon(i_icon_bg);
	}

	setBgButtonFormIdAndType(i_icon_bg, result->sid, ITEM_TYPE_SHIP);
	setTextColorFormIdAndType(t_shipName, result->sid, ITEM_TYPE_SHIP);
	
	if(result->maxdurable < result->definedmaxdurable)
	{   //not normal
		t_durableNum1->setString(StringUtils::format("%d /",result->durable));
		t_durableNum2->setString(StringUtils::format(" %d",result->maxdurable));
		t_durableNum2->setTextColor(Color4B(198,2,5,255));
		t_durableNum1->setPositionX(t_durableNum2->getPositionX() - t_durableNum2->getContentSize().width);
	}else
	{
		t_durableNum1->setString(StringUtils::format("%d /",result->durable));
		t_durableNum2->setString(StringUtils::format(" %d",result->definedmaxdurable));
	
		t_durableNum2->setTextColor(Color4B(40,25,13,255));
//chengyuan++
		t_durableNum1->setPositionX(t_durableNum2->getPositionX() - t_durableNum2->getContentSize().width);
//
	}

	t_sailorNum->setString(StringUtils::format("%d / ", result->sailors));
	t_sailorNum_1->setString(StringUtils::format(" %d",result->maxsailors));
	t_sailorNum->setPositionX(t_sailorNum_1->getPositionX() - t_sailorNum_1->getContentSize().width);

	t_weightNum->setString(StringUtils::format("%d /", result->currentcargosize / 100));
	t_weightNum_1->setString(StringUtils::format(" %d",result->maxcargosize / 100));
	t_weightNum->setPositionX(t_weightNum_1->getPositionX() - t_weightNum_1->getContentSize().width);

	t_supplyNum->setString(StringUtils::format("%d /", result->supply));
	t_supplyNum_1->setString(StringUtils::format(" %d",result->maxsupply));
	t_supplyNum->setPositionX(t_supplyNum_1->getPositionX() - t_supplyNum_1->getContentSize().width);

	t_atkNum->setString(StringUtils::format("%d",result->attack));
	t_defNum->setString(StringUtils::format("%d",result->defense));
	t_speedNum->setString(StringUtils::format("%d",result->speed));
	t_steeringNum->setString(StringUtils::format("%d",result->steering));

	//表达改造
	for (size_t i = 0; i < 7; i++)
	{
		int value = value1[i] + value1[i];
		if (value > 0)
		{
			setTextGreen(root,i + 1, true);
		}
		else if (value < 0)
		{
			setTextGreen(root, i + 1, false);
		}
	}

	auto i_speed = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "image_durable"));
	i_speed->setTouchEnabled(true);
	i_speed->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_speed->setTag(IMAGE_ICON_DURABLE + IMAGE_INDEX2);
	auto i_wei = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "image_weight"));
	i_wei->setTouchEnabled(true);
	i_wei->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_wei->setTag(IMAGE_ICON_WEIGHT + IMAGE_INDEX2);
	auto i_sai = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "image_sailor"));
	i_sai->setTouchEnabled(true);
	i_sai->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_sai->setTag(IMAGE_ICON_SAILOR + IMAGE_INDEX2);
	auto i_sup = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "image_supply"));
	i_sup->setTouchEnabled(true);
	i_sup->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_sup->setTag(IMAGE_ICON_SUPPLY + IMAGE_INDEX2);
	auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "image_atk"));
	i_atk->setTouchEnabled(true);
	i_atk->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_atk->setTag(IMAGE_ICON_ATTACKPOWER + IMAGE_INDEX2);
	auto i_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "image_def"));
	i_def->setTouchEnabled(true);
	i_def->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_def->setTag(IMAGE_ICON_DEFENSEPOWER + IMAGE_INDEX2);
	auto i_spe = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "image_speed"));
	i_spe->setTouchEnabled(true);
	i_spe->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_spe->setTag(IMAGE_ICON_SPEED + IMAGE_INDEX2);
	auto i_ste = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "image_steering"));
	i_ste->setTouchEnabled(true);
	i_ste->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_ste->setTag(IMAGE_ICON_STEERING + IMAGE_INDEX2);

}

void UIShipyard::updateReinforceEquip(Widget *root,const ReinforceEquipDefine *result,const int i)
{
	auto i_dur1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "image_durable"));
	i_dur1->setTouchEnabled(true);
	i_dur1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_dur1->setTag(IMAGE_ICON_DURABLE + IMAGE_INDEX2);



	m_equipmentsId.push_back(result->equipmentid);
	m_equipmentsId.push_back(result->iid);
	auto w_one = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root,"image_enhance_bg_1"));
	auto w_two = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root,"image_enhance_bg_2"));
	auto t_attack = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_atk_num"));
	w_one->setTag(i+START_INDEX);
	w_one->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	w_two->setTag(i+START_INDEX);
	w_two->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	if (result->optionalnum == 0)
	{
		w_one->setVisible(false);
		w_two->setVisible(false);
	}

	//注意理解1、2的作用
	int index1[2] = { -1 };
	int index2[2] = { -1 };
	int value1[7] = { 0 };
	int value2[7] = { 0 };
	if (result->optionalvalue)
	{
		get_ship_enhance_value_array_from_string(result->optionalvalue, &value1, &value2, &index1, &index2);
	}
	if (result->optionalnum == 1)
	{
		w_one->setVisible(true);
		w_two->setVisible(false);
		auto t_num = w_one->getChildByName<Text*>("label_empty_slot_0");
		//TODO 先做了隐藏处理
		t_num->setVisible(false);

		auto b_enhance_slot = w_one->getChildByName<Button*>("button_enhance_slot");
		auto i_enhance = b_enhance_slot->getChildByName<ImageView*>("image_enhance");
		b_enhance_slot->setTag(i+ START_INDEX);
		i_enhance->ignoreContentAdaptWithSize(false);

		auto t_property_num1 = w_one->getChildByName<Text*>("label_e_property_num_1");
		auto t_property_num2 = w_one->getChildByName<Text*>("label_e_property_num_2");
		auto i_property_num1 = w_one->getChildByName<ImageView*>("image_e_property_1");
		auto i_property_num2 = w_one->getChildByName<ImageView*>("image_e_property_2");
		auto t_empty_slot = w_one->getChildByName<Text*>("label_empty_slot");
		if (result->optionalitem != 0)
		{
			if (index1[0] != -1)
			{
				if (value1[index1[0]] < 0)
				{
					t_property_num1->setString(StringUtils::format("+%d", value1[index1[1]]));
					i_property_num1->loadTexture(PROPERTY_ICON_PTAH[index1[1]]);
					i_property_num1->setTouchEnabled(true);
					i_property_num1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
					i_property_num1->setTag(index1[1] +1 + IMAGE_INDEX2);
				}
				else
				{
					t_property_num1->setString(StringUtils::format("+%d", value1[index1[0]]));
					i_property_num1->loadTexture(PROPERTY_ICON_PTAH[index1[0]]);
					i_property_num1->setTouchEnabled(true);
					i_property_num1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
					i_property_num1->setTag(index1[0] +1 + IMAGE_INDEX2);
				}
				i_property_num1->setColor(Color3B(58, 30, 2));
			}
			else
			{
				i_property_num1->setVisible(false);
				t_property_num1->setVisible(false);
			}
			if (index1[1] != -1)
			{
				if (value1[index1[0]] < 0)
				{
					t_property_num2->setTextColor(TEXT_RED);
					t_property_num2->setString(StringUtils::format("-%d", -value1[index1[0]]));
					i_property_num2->loadTexture(PROPERTY_ICON_PTAH[index1[0]]);
					i_property_num2->setTouchEnabled(true);
					i_property_num2->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
					i_property_num2->setTag(index1[0]+1 + IMAGE_INDEX2);

				}
				else
				{
					if (value1[index1[1]] < 0)
					{
						t_property_num2->setTextColor(TEXT_RED);
						t_property_num2->setString(StringUtils::format("-%d", -value1[index1[1]]));
					}
					else
					{
						t_property_num2->setString(StringUtils::format("+%d", value1[index1[1]]));
					}
					i_property_num2->loadTexture(PROPERTY_ICON_PTAH[index1[1]]);
					i_property_num2->setTouchEnabled(true);
					i_property_num2->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
					i_property_num2->setTag(index1[1]+1 + IMAGE_INDEX2);
				}
				i_property_num2->setColor(Color3B(58, 30, 2));
			}
			else
			{
				i_property_num2->setVisible(false);
				t_property_num2->setVisible(false);
			}
			t_empty_slot->setVisible(false);
			i_enhance->setVisible(true);
			i_enhance->loadTexture(getItemIconPath(result->optionalitem));
//			t_num->setString("1/1");
		}else
		{
			t_property_num1->setVisible(false);
			t_property_num2->setVisible(false);
			t_empty_slot->setVisible(true);
			t_empty_slot->setPositionY(w_one->getBoundingBox().size.height/2);
			i_property_num1->setVisible(false);
			i_property_num2->setVisible(false);
			i_enhance->setVisible(false);
//			t_num->setString("0/1");
		}
	}

	auto i_icon_bg = root->getChildByName<Button*>("button_good_bg_1");
	i_icon_bg->setTouchEnabled(true);
	i_icon_bg->setTag(i+2000);
	i_icon_bg->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	auto i_icon = dynamic_cast<ImageView*>(i_icon_bg->getChildByName("image_ship"));
	auto i_position = dynamic_cast<ImageView*>(i_icon_bg->getChildByName("image_num"));
	auto t_shipName = dynamic_cast<Text*>(root->getChildByName("label_ship_name"));
	auto i_durable = root->getChildByName<Widget*>("image_durable");
	auto l_durable = dynamic_cast<ListView*>(i_durable->getChildByName("listview_ship_durable_num_1"));
	auto t_durableNum1 = dynamic_cast<Text*>(l_durable->getChildByName("label_ship_durable_num_1"));
	auto t_durableNum2 = dynamic_cast<Text*>(l_durable->getChildByName("label_ship_durable_num_2"));

	auto w_durable = dynamic_cast<Widget*>(i_durable->getChildByName("image_progressbar_durable"));
	auto l_durableBar = dynamic_cast<LoadingBar*>(w_durable->getChildByName("progressbar_durable"));
	auto i_durable_p = dynamic_cast<ImageView*>(w_durable->getChildByName("image_82"));

	
	auto i_property = dynamic_cast<Widget*>(Helper::seekWidgetByName(root,"image_property_1"));
	auto t_atk = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_atk_num"));
	auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root,"image_atk"));
	auto i_property2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(root, "image_property_2"));
	auto t_atk2 = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_defnum"));
	auto i_atk2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "image_def"));
	auto w_range = dynamic_cast<Widget*>(Helper::seekWidgetByName(root,"panel_range"));
	auto t_equip = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_equip_content"));

	switch (SINGLE_SHOP->getItemData()[result->iid].sub_type)
	{
	case SUB_TYPE_SHIP_FIGUREHEAD:
		{
			break;
		}
	case SUB_TYPE_SHIP_EMBOLON:
		{
			i_property->setVisible(false);
			i_property2->setVisible(false);
			w_range->setVisible(false);
			t_equip->setVisible(true);
			break;
		}
	case SUB_TYPE_SHIP_STERN_GUN:
		{
			break;
		}
	case SUB_TYPE_SHIP_SAIL:
		{
			i_property->setVisible(true);
			i_property2->setVisible(true);
			w_range->setVisible(false);
			t_equip->setVisible(false);
			i_atk->loadTexture(PROPERTY_ICON_PTAH[ICON_SPEED]);
			i_atk->setTouchEnabled(true);
			i_atk->setTag(ICON_SPEED + 1 + IMAGE_INDEX2);
			i_atk->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
			t_atk->setString(StringUtils::format("%d",result->speed));
			if (value1[ICON_SPEED] > 0)
			{
				t_atk->setTextColor(Color4B(1, 120, 5, 255));
			}
			else if (value1[ICON_SPEED] < 0)
			{
				t_atk->setTextColor(TEXT_RED);
			}

			i_atk2->loadTexture(PROPERTY_ICON_PTAH[ICON_STEERING]);
			i_atk2->setTouchEnabled(true);
			i_atk2->setTag(ICON_STEERING + 1 + IMAGE_INDEX2);
			i_atk2->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
			t_atk2->setString(StringUtils::format("%d", result->steer_speed));
			if (value1[ICON_STEERING] > 0)
			{
				t_atk2->setTextColor(Color4B(1, 120, 5, 255));
			}
			else if (value1[ICON_STEERING] < 0)
			{
				t_atk2->setTextColor(TEXT_RED);
			}
			break;
		}
	case SUB_TYPE_SHIP_ABANDON:
		{
			break;
		}
	case SUB_TYPE_SHIP_ARMOUR:
		{
			i_property->setVisible(true);
			w_range->setVisible(false);
			t_equip->setVisible(false);
			i_atk->loadTexture(PROPERTY_ICON_PTAH[ICON_DEFENSE]);
			i_atk->setTouchEnabled(true);
			i_atk->setTag(ICON_DEFENSE + 1 + IMAGE_INDEX2);
			i_atk->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
			t_atk->setString(StringUtils::format("%d",result->defense));
			if (value1[ICON_DEFENSE] > 0)
			{
				t_atk->setTextColor(Color4B(1, 120, 5, 255));
			}
			else if (value1[ICON_DEFENSE] < 0)
			{
				t_atk->setTextColor(TEXT_RED);
			}

			if (result->speed == 0)
			{
				i_property2->setVisible(false);
			}
			else
			{
				i_property2->setVisible(true);
				i_atk2->loadTexture(PROPERTY_ICON_PTAH[ICON_SPEED]);
				i_atk2->setTouchEnabled(true);
				i_atk2->setTag(ICON_SPEED + 1 + IMAGE_INDEX2);
				i_atk2->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
				t_atk2->setString(StringUtils::format("%d", result->speed));
				t_atk2->setTextColor(TEXT_RED);
			}
			break;
		}
	case SUB_TYPE_SHIP_GUN:
		{
			w_range->setVisible(true);
			i_property->setVisible(true);
			t_equip->setVisible(false);
			Button* i_range = nullptr ;
			if (w_range->getPositionY() == 58.00)
			{
				w_range->setPositionY(w_range->getPositionY() + i_property->getBoundingBox().size.height * 2 / 3);
			}
			std::string path;
			for (int i = 0; i < 3; i++)
			{
				path = StringUtils::format("button_range_%d",i+1);
				i_range = dynamic_cast<Button*>(Helper::seekWidgetByName(root,path));
				if(i < SINGLE_SHOP->getItemData()[result->iid].property1)
				{
					i_range->setBright(false);
				}else
				{
					i_range->setBright(true);
				}
			}
			i_atk->loadTexture(PROPERTY_ICON_PTAH[ICON_ATTACK]);
			i_atk->setTouchEnabled(true);
			i_atk->setTag(ICON_ATTACK + 1 + IMAGE_INDEX2);
			i_atk->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
			t_atk->setString(StringUtils::format("%d",result->attack));
			if (value1[ICON_ATTACK] > 0)
			{
				t_atk->setTextColor(Color4B(1, 120, 5, 255));
			}
			else if (value1[ICON_ATTACK] < 0)
			{
				t_atk->setTextColor(TEXT_RED);
			}

			if (result->speed == 0)
			{
				i_property2->setVisible(false);
			}
			else
			{
				i_property2->setVisible(true);
				i_property2->setPosition(Vec2(i_property->getPositionX(), i_property->getPositionY() - i_property->getBoundingBox().size.height * 2/3));
				i_atk2->loadTexture(PROPERTY_ICON_PTAH[ICON_SPEED]);
				i_atk2->setTouchEnabled(true);
				i_atk2->setTag(ICON_SPEED + 1 + IMAGE_INDEX2);
				i_atk2->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
				t_atk2->setString(StringUtils::format("%d", result->speed));
				t_atk2->setTextColor(TEXT_RED);
				i_property->setPositionY(i_property->getPositionY() + i_property->getBoundingBox().size.height * 2/3);
			}
			break;
		}
	default:
		break;
	}

	std::string itemName = getItemName(result->iid);
	std::string itemPath = getItemIconPath(result->iid);
	i_icon->loadTexture(itemPath);
	t_shipName->setString(CutItemName(itemName));
	//t_shipName->setString(itemName);
	
	auto i_intensify = Helper::seekWidgetByName(i_icon_bg, "goods_intensify");
	if (i_intensify)
	{
		i_intensify->removeFromParentAndCleanup(true);
	}

	if (result->optionalitem > 0)
	{
		addStrengtheningIcon(i_icon_bg);
	}

	setBgButtonFormIdAndType(i_icon_bg, result->iid, ITEM_TYPE_SHIP_EQUIP);
	setTextColorFormIdAndType(t_shipName, result->iid, ITEM_TYPE_SHIP_EQUIP);

	float temp = 0;
	if(result->maxdurable < result->definedmaxdurable)
	{   //not normal
		t_durableNum1->setString(StringUtils::format("%d /",result->durable));
		t_durableNum2->setString(StringUtils::format(" %d",result->maxdurable));
		t_durableNum2->setTextColor(Color4B(198,2,5,255));
		i_durable_p->setVisible(true);
		l_durable->refreshView();
		if (result->durable <= result->definedmaxdurable * SHIP_EQUIP_BROKEN_PERCENT)
		{
			auto i_imageBg = i_icon_bg->getChildByName<ImageView*>("image_ship");
			addequipBrokenIcon(i_icon_bg);
			setBrokenEquipRed(i_imageBg);
		}
		if (result->maxdurable != 0)
		{
			temp = (result->durable*1.0)/(result->maxdurable*1.0);
		}
		l_durableBar->setPercent(87 * temp);
	}else
	{
		t_durableNum1->setString(StringUtils::format("%d /",result->durable));
		t_durableNum2->setString(StringUtils::format(" %d",result->definedmaxdurable));
		t_durableNum2->setTextColor(Color4B(40,25,13,255));
		i_durable_p->setVisible(false);
		l_durable->refreshView();
		if (result->durable <= result->definedmaxdurable * SHIP_EQUIP_BROKEN_PERCENT)
		{
			auto i_imageBg = i_icon_bg->getChildByName<ImageView*>("image_ship");
			addequipBrokenIcon(i_icon_bg);
			setBrokenEquipRed(i_imageBg);
		}
		if (result->definedmaxdurable != 0)
		{
			temp = (result->durable*1.0)/(result->definedmaxdurable*1.0);
		}
		l_durableBar->setPercent(100 * temp);
	}

	if (result->shipposition >= 0)
	{
		i_position->setVisible(true);
		i_position->loadTexture(getPositionIconPath(result->shipposition + 1));
	}else
	{
		i_position->setVisible(false);
	}
}
void UIShipyard::updatePageView_3_equip()
{
	if (m_pReinforceEquipResult == nullptr)
	{
		return;
	}
	m_equipmentsId.clear();
 

	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CSB]);
	if (!view)
	{
		closeView();
		openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CSB]);
		view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_ENHANCE_CSB]);
	}
	auto lv_ship_container  = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_container_ship"));
	auto lv_equip_container = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_container_gear")); 
	auto t_reinforceTitle = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)view,"label_enhancable_ship"));
	t_reinforceTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_EQUIP_REINFORCE_LIST"]);
	lv_ship_container->setVisible(false);
	lv_equip_container->setVisible(true);

	auto w_item = view->getChildByName<Widget*>("panel_gear_enhance_bg")->clone();
	w_item->setVisible(true);
	lv_equip_container->removeAllChildrenWithCleanup(true);

	auto p_no = dynamic_cast<Widget*>(Helper::seekWidgetByName((Widget*)view,"panel_no"));
	p_no->setVisible(false);
	auto t_no1 = p_no->getChildByName<Text*>("label_no_item1");
	auto t_no2 = p_no->getChildByName<Text*>("label_no_item2");
	if (m_pReinforceEquipResult->n_equipmentonship < 1 && m_pReinforceEquipResult->n_equipmentsinbag < 1)
	{
		lv_equip_container->pushBackCustomItem(w_item);
		w_item->setVisible(false);
		p_no->setVisible(true);
		t_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_NOT_EQUIP_REINFORCE1"]);
		t_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_NOT_EQUIP_REINFORCE2"]);
		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		image_pulldown->setVisible(false);
		return;
	}

	for (auto i = 0; i < m_pReinforceEquipResult->n_equipmentonship + m_pReinforceEquipResult->n_equipmentsinbag ; ++i)
	{
		auto w_curItem = w_item->clone();
		w_curItem->setTag(i+START_INDEX);
		ReinforceEquipDefine* curEquipInfo;
		if (i < m_pReinforceEquipResult->n_equipmentonship)
		{
			curEquipInfo = m_pReinforceEquipResult->equipmentonship[i];
		}else
		{
			curEquipInfo = m_pReinforceEquipResult->equipmentsinbag[i - m_pReinforceEquipResult->n_equipmentonship];
		}
		updateReinforceEquip(w_curItem,curEquipInfo,i);
		lv_equip_container->pushBackCustomItem(w_curItem);
	}
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 + 3);
	addListViewBar(lv_equip_container,image_pulldown);
}

void UIShipyard::updatePageView_3_shipBuild()
{
	if (m_pShipBuildingResult == nullptr)
	{
		return;
	}

	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_CSB]);
	if (!view)
	{
		closeView();
		openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_CSB]);
		view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_CSB]);
	}
	auto lv_build_container  = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_building"));
	auto p_no = view->getChildByName<Widget*>("panel_no"); 

	auto  t_buildTitle = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)view,"label_building_list"));
	t_buildTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_BUILD_LIST"]);

	m_vUpdateTotleTimeList.clear();
	m_vUpdateTimeList.clear();

	auto l_workline = dynamic_cast<ListView*>(Helper::seekWidgetByName((Widget*)view,"listview_working_slot"));
	auto t_num = l_workline->getItem(1);
	auto skillTrees = SINGLE_SHOP->getSkillTrees();
	auto i_skill1 = dynamic_cast<ImageView*>(l_workline->getItem(3));
	auto i_skill2 = dynamic_cast<ImageView*>(l_workline->getItem(4));
	int level = 0;
	for (int i = 0; i < m_pShipBuildingResult->n_skills; i++)
	{
		if (m_pShipBuildingResult->skills[i]->id == SKILL_MANUFACTURING_EXPERT)
		{
			level = m_pShipBuildingResult->skills[i]->level;
			break;
		}
	}
	if (level > 0)
	{
		i_skill1->setVisible(true);
		i_skill1->ignoreContentAdaptWithSize(false);
		i_skill1->loadTexture(getSkillIconPath(SKILL_MANUFACTURING_EXPERT, SKILL_TYPE_PLAYER));
		auto t_lv = i_skill1->getChildByName<Text*>("text_item_skill_lv");
		setTextSizeAndOutline(t_lv,level);
	}else
	{
		i_skill1->setVisible(false);
	}
	level = 0;
	for (int i = 0; i < m_pShipBuildingResult->n_skills; i++)
	{
		if (m_pShipBuildingResult->skills[i]->id == SKILL_SHIPBUILDING_TECHNOLOGY)
		{
			level = m_pShipBuildingResult->skills[i]->level;
			break;
		}
	}
	if (level > 0)
	{
		i_skill2->setVisible(true);
		i_skill2->ignoreContentAdaptWithSize(false);
		i_skill2->loadTexture(getSkillIconPath(SKILL_SHIPBUILDING_TECHNOLOGY, SKILL_TYPE_PLAYER));
		auto t_lv = i_skill2->getChildByName<Text*>("text_item_skill_lv");
		setTextSizeAndOutline(t_lv,level);
	}else
	{
		i_skill2->setVisible(false);
	}
	i_skill1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	i_skill2->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	((Text*)t_num)->setString(StringUtils::format("%d/%d",m_pShipBuildingResult->n_ship,level*skillTrees[SKILL_SHIPBUILDING_TECHNOLOGY].effect_per_lv/1000+3));
	l_workline->refreshView();
	auto b_build = dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)view,"button_building"));
	auto b_build_2 = dynamic_cast<Button*>(b_build->getChildByName("button_image_build"));
	
	if(m_pShipBuildingResult->n_ship == level*skillTrees[SKILL_SHIPBUILDING_TECHNOLOGY].effect_per_lv/1000+3)
	{
		b_build->setTouchEnabled(false);
		b_build->setBright(false);
		b_build_2->setBright(false);
	}else
	{
		b_build->setTouchEnabled(true);
		b_build->setBright(true);
		b_build_2->setBright(true);
	}

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	if (m_pShipBuildingResult->n_ship < 1)
	{
		lv_build_container->setVisible(false);
		p_no->setVisible(true);
		auto t_item1 = dynamic_cast<Text*>(p_no->getChildByName("label_no_build_1"));
		auto t_item2 = dynamic_cast<Text*>(p_no->getChildByName("label_no_build_2"));
		t_item1->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_NOT_BUILDING_SHIP1"]);
		t_item2->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_NOT_BUILDING_SHIP2"]);
		button_pulldown->setVisible(false);
		button_pulldown->getParent()->setVisible(false);
		return;
	}else
	{
		lv_build_container->setVisible(true);
		p_no->setVisible(false);
	}
	
	auto w_item = lv_build_container->getItem(0)->clone();
	w_item->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	w_item->setVisible(true);
	lv_build_container->removeAllItems();

	if (m_pShipBuildingResult->n_ship < 1)
	{
		lv_build_container->pushBackCustomItem(w_item);
		w_item->setVisible(false);
		return;
	}

	for (auto i = 0; i < m_pShipBuildingResult->n_ship; ++i)
	{
		auto w_curItem = w_item->clone();
		w_curItem->setTag(i+START_INDEX);
		auto *curEquipInfo = m_pShipBuildingResult->ship[i];
		updateShipBuild(w_curItem,curEquipInfo,i);
		lv_build_container->pushBackCustomItem(w_curItem);
	}
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 + 3);
	addListViewBar(lv_build_container,image_pulldown);
	updateBySecond(0);
}

void UIShipyard::updatePageView_3_equipBuild()
{
	if (m_pEquipBuildingResult == nullptr)
	{
		return;
	}
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_CSB]);
	if (!view)
	{
		closeView();
		openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_CSB]);
		view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_CSB]);
	}

	auto lv_build_container  = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_building"));
	auto p_no = (view->getChildByName<Widget*>("panel_no")); 

	auto  t_buildTitle = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)view,"label_building_list"));
	t_buildTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_EQUIP_BUILD_LIST"]);

	m_vUpdateTotleTimeList.clear();
	m_vUpdateTimeList.clear();

	auto l_workline = dynamic_cast<ListView*>(Helper::seekWidgetByName((Widget*)view,"listview_working_slot"));
	auto t_num = l_workline->getItem(1);
	auto i_skill1 = dynamic_cast<ImageView*>(l_workline->getItem(3));
	auto i_skill2 = l_workline->getItem(4);
	int level = 0;
	for (int i = 0; i < m_pEquipBuildingResult->n_skills; i++)
	{
		if (m_pEquipBuildingResult->skills[i]->id == SKILL_MANUFACTURING_EXPERT)
		{
			level = m_pEquipBuildingResult->skills[i]->level;
			break;
		}
	}
	if (level > 0)
	{
		i_skill1->setVisible(true);
		i_skill1->ignoreContentAdaptWithSize(false);
		i_skill1->loadTexture(getSkillIconPath(SKILL_MANUFACTURING_EXPERT, SKILL_TYPE_PLAYER));
		auto t_lv = i_skill1->getChildByName<Text*>("text_item_skill_lv");
		setTextSizeAndOutline(t_lv,level);
	}else
	{
		i_skill1->setVisible(false);
	}
	i_skill1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	i_skill2->setVisible(false);
	((Text*)t_num)->setString(StringUtils::format("%d/%d",m_pEquipBuildingResult->n_equipment,3));
	l_workline->refreshView();

	auto b_build = dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)view,"button_building"));
	auto b_build_2 = dynamic_cast<Button*>(b_build->getChildByName("button_image_build"));
	if(m_pEquipBuildingResult->n_equipment == 3)
	{
		b_build->setTouchEnabled(false);
		b_build->setBright(false);
		b_build_2->setBright(false);
	}else
	{
		b_build->setTouchEnabled(true);
		b_build->setBright(true);
		b_build_2->setBright(true);
	}

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	if (m_pEquipBuildingResult->n_equipment < 1)
	{
		lv_build_container->setVisible(false);
		p_no->setVisible(true);
		auto t_item1 = dynamic_cast<Text*>(p_no->getChildByName("label_no_build_1"));
		auto t_item2 = dynamic_cast<Text*>(p_no->getChildByName("label_no_build_2"));
		t_item1->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_NOT_BUILDING_EQUIP1"]);
		t_item2->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_NOT_BUILDING_EQUIP2"]);
		button_pulldown->setVisible(false);
		button_pulldown->getParent()->setVisible(false);
		return;
	}else
	{
		lv_build_container->setVisible(true);
		p_no->setVisible(false);
	}
	

	auto w_item = lv_build_container->getItem(0)->clone();
	w_item->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	w_item->setVisible(true);
	lv_build_container->removeAllItems();

	if (m_pEquipBuildingResult->n_equipment < 1)
	{
		lv_build_container->pushBackCustomItem(w_item);
		w_item->setVisible(false);
		return;
	}

	for (auto i = 0; i < m_pEquipBuildingResult->n_equipment; ++i)
	{
		auto w_curItem = w_item->clone();
		w_curItem->setTag(i+START_INDEX);
		auto *curEquipInfo = m_pEquipBuildingResult->equipment[i];
		updateEquipBuild(w_curItem,curEquipInfo,i);
		lv_build_container->pushBackCustomItem(w_curItem);
	}
	
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 + 3);
	addListViewBar(lv_build_container,image_pulldown);
	updateBySecond(0);
}

void UIShipyard::updateShipBuild(Widget *root,const BuildingShipDefine *result,const int i)
{
	auto b_ship_bg = dynamic_cast<Button*>(Helper::seekWidgetByName(root, "button_ship_bg"));
	auto i_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root,"image_ship"));
	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_ship_name"));
	auto t_city = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_city"));
	auto b_repair = dynamic_cast<Button*>(Helper::seekWidgetByName(root,"button_repair_v"));
	auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_time"));

	b_repair->setTag(i+START_INDEX);
	b_repair->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	i_ship->ignoreContentAdaptWithSize(false);
	i_ship->loadTexture(getShipIconPath(result->ship->sid));
	t_name->setString(getShipName(result->ship->sid));
	t_city->setString(SINGLE_SHOP->getCitiesInfo()[result->cityid].name);

	int lenTime = result->endtime - result->currenttime;
	m_vUpdateTimeList.push_back(lenTime);
	int totalTime = result->endtime - result->starttime;
	m_vUpdateTotleTimeList.push_back(totalTime);
	b_ship_bg->setTag(result->ship->sid + START_INDEX);
	auto i_bgImage = b_ship_bg->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(b_ship_bg, result->ship->sid, ITEM_TYPE_SHIP);
	setBgImageColorFormIdAndType(i_bgImage, result->ship->sid, ITEM_TYPE_SHIP);
	setTextColorFormIdAndType(t_name, result->ship->sid, ITEM_TYPE_SHIP);

	t_time->setString(timeUtil(m_vUpdateTimeList[i], TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_GM));
	setTextFontSize(t_time);
}

void UIShipyard::updateEquipBuild(Widget *root,const BuildingEquipmentDefine *result,const int i)
{
	auto b_ship_bg = dynamic_cast<Button*>(Helper::seekWidgetByName(root, "button_ship_bg"));
	auto i_ship = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root,"image_ship"));
	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_ship_name"));
	auto t_city = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_city"));
	auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(root,"label_time"));
	auto b_repair = dynamic_cast<Button*>(Helper::seekWidgetByName(root,"button_repair_v"));
	auto p_wait = dynamic_cast<Widget*>(Helper::seekWidgetByName(root,"panel_build_wait"));
	auto p_time = dynamic_cast<Widget*>(Helper::seekWidgetByName(root,"panel_build_time"));
	auto l_time = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(root,"progressbar_building_time"));

	b_repair->setTag(i+START_INDEX);
	b_repair->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	i_ship->ignoreContentAdaptWithSize(false);
	i_ship->loadTexture(getItemIconPath(result->equipmentid));
	t_name->setString(getItemName(result->equipmentid));
	t_city->setString(SINGLE_SHOP->getCitiesInfo()[result->cicyid].name);

	int lenTime = result->endtime - result->currenttime;
	m_vUpdateTimeList.push_back(lenTime);
	int totalTime = result->endtime - result->starttime;
	m_vUpdateTotleTimeList.push_back(totalTime);
	b_ship_bg->setTag(result->equipmentid + START_INDEX);
	auto i_bgImage = b_ship_bg->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(b_ship_bg, result->equipmentid, ITEM_TYPE_SHIP_EQUIP);
	setBgImageColorFormIdAndType(i_bgImage, result->equipmentid, ITEM_TYPE_SHIP_EQUIP);
	setTextColorFormIdAndType(t_name, result->equipmentid, ITEM_TYPE_SHIP_EQUIP);

	t_time->setString(timeUtil(m_vUpdateTimeList[i], TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_GM));
	setTextFontSize(t_time);
}

void UIShipyard::updateBySecond(const float fTime)
{
	if (m_vUpdateTimeList.size() > 0)
	{
		int nums = m_vUpdateTimeList.size() - 1;
		for (int i = nums; i > -1; i--)
		{
			auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_CSB]);
			auto l_equip = view->getChildByName<ListView*>("listview_building");
			auto item = l_equip->getItem(i);
			auto p_time = item->getChildByName<Widget*>("panel_build_time");
			auto p_wait = item->getChildByName<Widget*>("panel_build_wait");
			auto b_addSpeed = item->getChildByName<Widget*>("button_repair_v");
			if (m_vUpdateTimeList[i] < 1)
			{
				p_time->setVisible(false);
				p_wait->setVisible(true);
				item->setTouchEnabled(false);
				b_addSpeed->setVisible(false);
			}else
			{
				p_time->setVisible(true);
				p_wait->setVisible(false);
				item->setTouchEnabled(true);
				b_addSpeed->setVisible(true);
				auto t_time = p_time->getChildByName<Text*>("label_time");
				auto l_time = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(p_time,"progressbar_building_time"));
				t_time->setString(timeUtil(m_vUpdateTimeList[i], TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_GM));
				l_time->setPercent(100.0 * (m_vUpdateTotleTimeList[i] - m_vUpdateTimeList[i]) / m_vUpdateTotleTimeList[i]);
				m_vUpdateTimeList[i]--;
			}
		}
	}
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_SLOT_CSB]);
	if (view)
	{
		showBuildItemDetails(m_nGlobalIndex);
	}
}

void UIShipyard::updateShipAndEquipDrawing(GetDrawingsResult *result)
{
	openView(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
	auto p_item = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_itemdetails"));
	p_item->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	auto l_draw = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_item"));
	auto l_button = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_two_butter"));
	for (int i = l_button->getChildrenCount() - 1; i > 4; i--)
	{
		l_button->removeItem(i);
	}
	l_draw->removeAllChildrenWithCleanup(true);

	auto b_currentDrawing = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_goods"));
	auto b_allDrawing = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_ships"));
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_title_create_sell_order"));
	auto t_subtitle = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_title_select_item"));

	dynamic_cast<Text*>(b_currentDrawing->getChildren().at(0))->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_DRAWING_AVAILABLE"]);
	dynamic_cast<Text*>(b_allDrawing->getChildren().at(0))->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_DRAWING_ALL"]);
	if(m_bIsAllDrawingDefine)
	{
		b_currentDrawing->setBright(true);
		b_allDrawing->setBright(false);
		dynamic_cast<Text*>(b_currentDrawing->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		dynamic_cast<Text*>(b_allDrawing->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
	}else
	{
		b_currentDrawing->setBright(false);
		b_allDrawing->setBright(true);
		dynamic_cast<Text*>(b_currentDrawing->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
		dynamic_cast<Text*>(b_allDrawing->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}
	
	auto image_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_pulldown"));
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setVisible(true);
	t_subtitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_DRAWING_SUBTITLE"]);
	if (result->drawingtype == 0)
	{
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_SHIP_DRAWING_TITLE"]);
		if (result->n_shipdrawing < 1)
		{
			button_pulldown->setVisible(false);
		}
		
		for (int i = 0; i < result->n_shipdrawing; i++)
		{
			bool isCurrentCity = false;
			std::vector<int> cityList;
			convertToVector(result->shipdrawing[i]->cityidlist,cityList);

			auto iter = std::find_if(cityList.begin(),cityList.end(),std::bind1st(std::equal_to<int>(),result->currentcityid));
			if (iter == cityList.end())
			{
				m_vCurrentCityNoShipBuild.push_back(result->shipdrawing[i]);
				if (!m_bIsAllDrawingDefine)
				{
					continue;
				}
			}else
			{
				isCurrentCity = true;
				m_vCurrentCityCanShipBuild.push_back(result->shipdrawing[i]);
			}
			
			auto item = p_item->clone();
			if (m_bIsAllDrawingDefine)
			{
				item->setTag(i + START_INDEX);
			}else
			{
				item->setTag(m_vCurrentCityCanShipBuild.size() - 1 + START_INDEX);
			}
			auto b_draw_bg = dynamic_cast<Button*>(Helper::seekWidgetByName(item,"button_good_bg"));
			auto i_draw = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item,"image_goods"));
			auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_goods_name"));
			auto t_num = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_goods_num"));
			b_draw_bg->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
			b_draw_bg->setTouchEnabled(true);
			b_draw_bg->setTag(result->shipdrawing[i]->itemid + START_INDEX);
			i_draw->ignoreContentAdaptWithSize(false);
			i_draw->loadTexture(getItemIconPath(result->shipdrawing[i]->itemid));
			t_name->setString(getItemName(result->shipdrawing[i]->itemid));
			t_num->setString(StringUtils::format("x %d",result->shipdrawing[i]->amount));
			l_draw->pushBackCustomItem(item);

			auto i_bgImage = b_draw_bg->getChildByName<ImageView*>("image_item_bg_lv");
			setBgButtonFormIdAndType(b_draw_bg, result->shipdrawing[i]->itemid, ITEM_TYPE_DRAWING);
			setBgImageColorFormIdAndType(i_bgImage, result->shipdrawing[i]->itemid, ITEM_TYPE_DRAWING);
			setTextColorFormIdAndType(t_name, result->shipdrawing[i]->itemid, ITEM_TYPE_DRAWING);

			if (!isCurrentCity)
			{
				t_name->setTextColor(Color4B(198, 2, 5, 255));
				t_num->setTextColor(Color4B(198, 2, 5, 255));
			}
		}
	}else
	{
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_EQUIP_DRAWING_TITLE"]);
		if (result->n_equipdrawing < 1)
		{
			button_pulldown->setVisible(false);
		}
		for (int i = 0; i < result->n_equipdrawing; i++)
		{
			bool isCurrentCity = false;
			std::vector<int> cityList;
			convertToVector(result->equipdrawing[i]->cityidlist,cityList);

			auto iter = std::find_if(cityList.begin(),cityList.end(),std::bind1st(std::equal_to<int>(),result->currentcityid));
			if (iter == cityList.end())
			{
				m_vCurrentCityNoEquipBuild.push_back(result->equipdrawing[i]);
				if (!m_bIsAllDrawingDefine)
				{
					continue;
				}
			}else
			{
				isCurrentCity = true;
				m_vCurrentCityCanEquipBuild.push_back(result->equipdrawing[i]);
			}

			auto item = p_item->clone();
			if (m_bIsAllDrawingDefine)
			{
				item->setTag(i + START_INDEX);
			}else
			{
				item->setTag(m_vCurrentCityCanEquipBuild.size() - 1 + START_INDEX);
			}
			auto b_draw_bg = dynamic_cast<Button*>(Helper::seekWidgetByName(item,"button_good_bg"));
			auto i_draw = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item,"image_goods"));
			auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_goods_name"));
			auto t_num = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_goods_num"));
			b_draw_bg->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
			b_draw_bg->setTouchEnabled(true);
			b_draw_bg->setTag(result->equipdrawing[i]->itemid + START_INDEX);
			i_draw->ignoreContentAdaptWithSize(false);
			i_draw->loadTexture(getItemIconPath(result->equipdrawing[i]->itemid));
			t_name->setString(getItemName(result->equipdrawing[i]->itemid));
			t_num->setString(StringUtils::format("x %d",result->equipdrawing[i]->amount));
			l_draw->pushBackCustomItem(item);

			auto i_bgImage = b_draw_bg->getChildByName<ImageView*>("image_item_bg_lv");
			setBgButtonFormIdAndType(b_draw_bg, result->equipdrawing[i]->itemid, ITEM_TYPE_DRAWING);
			setBgImageColorFormIdAndType(i_bgImage, result->equipdrawing[i]->itemid, ITEM_TYPE_DRAWING);
			setTextColorFormIdAndType(t_name, result->equipdrawing[i]->itemid, ITEM_TYPE_DRAWING);

			if (!isCurrentCity)
			{
				t_name->setTextColor(Color4B(198, 2, 5, 255));
				t_num->setTextColor(Color4B(198, 2, 5, 255));
			}
		}
	}
	
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 + 3);
	addListViewBar(l_draw,image_pulldown);
	auto curScene = Director::getInstance()->getRunningScene();
	curScene->removeChildByTag(TAG_LAODING);
}

void UIShipyard::updateShipAndEquipDialog(int index)
{
	openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_DIALOG_CSB]);
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_DIALOG_CSB]);

	const std::string goodsbg = "cocosstudio/login_ui/common/goods_bg.png";
	const std::string goodsblackbg = "cocosstudio/login_ui/common/goods_bg_black.png";

	auto t_buildTitle = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_build_ship"));
	auto i_ship_info = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_ship_info_bg"));
	auto i_equip_info = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_gear_info_bg"));
	auto t_craft = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_crafting_num_1"));
	auto t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_silver_num_1"));
	auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_clock_num"));
	auto t_city = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_city"));
	auto i_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_skill_bg_2_2"));
	i_skill->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
	auto button_warning = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_warning"));
	button_warning->setVisible(false);

	auto image_durable = view->getChildByName<ImageView*>("panel_build_ship");
	auto image_durable1 = image_durable->getChildByName<ImageView*>("image_gear_info_bg");
	auto image_durable2 = image_durable1->getChildByName<ImageView*>("panel_durable");
	auto image_durable3 = image_durable2->getChildByName<ImageView*>("image_durable");
	image_durable3->setTouchEnabled(true);
	image_durable3->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	image_durable3->setTag(IMAGE_ICON_DURABLE + IMAGE_INDEX2);

	auto image_dur = image_durable->getChildByName<ImageView*>("image_ship_info_bg");
	auto image_dur1 = image_dur->getChildByName<ImageView*>("image_durable");
	image_dur1->setTouchEnabled(true);
	image_dur1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	image_dur1->setTag(IMAGE_ICON_DURABLE + IMAGE_INDEX2);
	auto image_sai = image_dur->getChildByName<ImageView*>("image_sailor");
	image_sai->setTouchEnabled(true);
	image_sai->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	image_sai->setTag(IMAGE_ICON_SAILOR + IMAGE_INDEX2);
	auto image_wei = image_dur->getChildByName<ImageView*>("image_weight");
	image_wei->setTouchEnabled(true);
	image_wei->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	image_wei->setTag(IMAGE_ICON_CARGO + IMAGE_INDEX2);
	auto image_sup = image_dur->getChildByName<ImageView*>("image_supply");
	image_sup->setTouchEnabled(true);
	image_sup->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	image_sup->setTag(IMAGE_ICON_SUPPLY + IMAGE_INDEX2);
	auto image_atk = image_dur->getChildByName<ImageView*>("image_property_1");
	auto image_atk1 = image_atk->getChildByName<ImageView*>("image_atk");
	image_atk1->setTouchEnabled(true);
	image_atk1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	image_atk1->setTag(IMAGE_ICON_ATTACK +IMAGE_INDEX2);
	auto image_def = image_dur->getChildByName<ImageView*>("image_property_2");
	auto image_def1 = image_def->getChildByName<ImageView*>("image_def");
	image_def1->setTouchEnabled(true);
	image_def1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	image_def1->setTag(IMAGE_ICON_DEFENCE + IMAGE_INDEX2);
	auto image_spe = image_dur->getChildByName<ImageView*>("image_property_3");
	auto image_spe1 = image_spe->getChildByName<ImageView*>("image_speed");
	image_spe1->setTouchEnabled(true);
	image_spe1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	image_spe1->setTag(IMAGE_ICON_SPEED + IMAGE_INDEX2);
	auto image_ste = image_dur->getChildByName<ImageView*>("image_property_4");
	auto image_ste1 = image_ste->getChildByName<ImageView*>("image_steering");
	image_ste1->setTouchEnabled(true);
	image_ste1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	image_ste1->setTag(IMAGE_ICON_STEERING + IMAGE_INDEX2);



	auto i_ship_cra = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_city_crafting"));
	i_ship_cra->setTouchEnabled(true);
	i_ship_cra->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_ship_cra->setTag(IMAGE_ICON_MANUFACTURE + IMAGE_INDEX2);
	auto i_ship_sil = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_silver"));
	i_ship_sil->setTouchEnabled(true);
	i_ship_sil->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_ship_sil->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);
	auto i_ship_clo = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_clock"));
	i_ship_clo->setTouchEnabled(true);
	i_ship_clo->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
	i_ship_clo->setTag(IMAGE_ICON_CLOCK + IMAGE_INDEX2);

	int craft_required = 0;
	int craft_current = 0;
	int silver_required = 0;
	int64_t silver_current = 0;
	int lv = 0;
	int itemid = 0;
	float time = 0;

//是否可以打造
	bool isEnough = true;
	m_bIsMaterialEnough = true;
	if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_building") == 0)
	{
		t_buildTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_BUILDING_SHIP"]);
		i_ship_info->setVisible(true);
		i_equip_info->setVisible(false);

		ShipdrawingDefine *currentShipDrawing = nullptr;
		if (m_bIsAllDrawingDefine)
		{
			currentShipDrawing = m_pShipDrawingsResult->shipdrawing[index];
		}else
		{
			currentShipDrawing = m_vCurrentCityCanShipBuild[index];
		}
		if (!currentShipDrawing)
		{
			return;
		}
		auto i_draw_bg = dynamic_cast<Button*>(Helper::seekWidgetByName(i_ship_info,"image_draw_bg_1"));
		auto i_draw = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_ship_info,"image_ship"));
		auto t_draw = dynamic_cast<Text*>(Helper::seekWidgetByName(i_ship_info,"label_ship_name"));
		auto t_durable = dynamic_cast<Text*>(Helper::seekWidgetByName(i_ship_info,"label_sailors_num_0"));
		auto t_sailor = dynamic_cast<Text*>(Helper::seekWidgetByName(i_ship_info,"label_sailors_num"));
		auto t_weight = dynamic_cast<Text*>(Helper::seekWidgetByName(i_ship_info,"label_ship_weight_num_1"));
		auto t_supply = dynamic_cast<Text*>(Helper::seekWidgetByName(i_ship_info,"label_ship_supply_num_1"));
		auto t_atk = dynamic_cast<Text*>(Helper::seekWidgetByName(i_ship_info,"label_atk_num"));
		auto t_def = dynamic_cast<Text*>(Helper::seekWidgetByName(i_ship_info,"label_defnum"));
		auto t_speed = dynamic_cast<Text*>(Helper::seekWidgetByName(i_ship_info,"label_speed_num"));
		auto t_steer = dynamic_cast<Text*>(Helper::seekWidgetByName(i_ship_info,"label_steering_num"));

		i_draw_bg->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
		i_draw_bg->setTag(currentShipDrawing->ship->sid + START_INDEX);
		i_draw->loadTexture(getShipIconPath(currentShipDrawing->ship->sid));
		t_draw->setString(getShipName(currentShipDrawing->ship->sid));
		t_durable->setString(StringUtils::format("%d",currentShipDrawing->ship->hp_max));
		t_sailor->setString(StringUtils::format("%d",currentShipDrawing->ship->crew_max));
		t_weight->setString(StringUtils::format("%d",currentShipDrawing->ship->cargo_size/100));
		t_supply->setString(StringUtils::format("%d",currentShipDrawing->ship->supply_max));
		t_atk->setString("0"/*StringUtils::format("%d",currentShipDrawing->ship->armor_num)*/);
		t_def->setString(StringUtils::format("%d",currentShipDrawing->ship->defence));
		t_speed->setString(StringUtils::format("%d",currentShipDrawing->ship->speed));
		t_steer->setString(StringUtils::format("%d",currentShipDrawing->ship->steer_speed));

		auto i_bgImage = i_draw_bg->getChildByName<ImageView*>("image_item_bg_lv");
		setBgButtonFormIdAndType(i_draw_bg, currentShipDrawing->ship->sid, ITEM_TYPE_SHIP);
		setBgImageColorFormIdAndType(i_bgImage, currentShipDrawing->ship->sid, ITEM_TYPE_SHIP);
		setTextColorFormIdAndType(t_draw, currentShipDrawing->ship->sid, ITEM_TYPE_SHIP);

		craft_required = currentShipDrawing->requiredmanufacture;
		craft_current = m_pShipDrawingsResult->currentcitymanufacture;
		float craft_factor = 1.0f;
		if(craft_required > craft_current ){
			button_warning->setVisible(true);
			craft_factor = sqrt(craft_required*1.0/craft_current);
			craft_factor = craft_factor > 2.0f ? craft_factor : 2.0f;
		}

		silver_required = ceil(currentShipDrawing->coin * craft_factor * craft_factor);
		silver_current = m_pShipDrawingsResult->coin;
		DRAWINGSDESIGNINFO shipDesignInfo = SINGLE_SHOP->getDrawingsDesignInfo()[currentShipDrawing->itemid];

		std::vector<ItemDefine*> shipMaterial;
		shipMaterial.push_back(currentShipDrawing->requireditem1);
		shipMaterial.push_back(currentShipDrawing->requireditem2);
		for (int i = 0; i < currentShipDrawing->n_addrequireditem; i++)
		{		
			shipMaterial.push_back(currentShipDrawing->addrequireditem[i]);
		}
		int needMaterialNum = 0;
		for (int i = 0; i < shipMaterial.size(); i++)
		{
			if (shipMaterial.at(i)->requirednum > 0)
			{
				needMaterialNum++;
				auto curItemMaterial = shipMaterial.at(i);
				std::string name_bg = StringUtils::format("image_material_bg_%d",needMaterialNum);
				std::string name_image = "image_material_1";
				std::string name_label_1 = StringUtils::format("label_material_num_%d", needMaterialNum);
				std::string name_label_2 = StringUtils::format("label_material_num_%d_0", needMaterialNum);
				auto i_material1_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, name_bg));
				auto i_material1 = i_material1_bg->getChildByName<ImageView*>(name_image);
				auto t_material1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, name_label_1));
				auto t_material2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, name_label_2));
				i_material1_bg->setVisible(true);
				i_material1->setVisible(true);
				t_material1->setVisible(true);
				t_material2->setVisible(true);
				i_material1_bg->loadTexture(goodsbg);
				i_material1->loadTexture(getItemIconPath(curItemMaterial->item_id));
				i_material1->setTag(curItemMaterial->item_id);
				i_material1->setTouchEnabled(true);
				i_material1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::ItemEvent,this));
				t_material2->setString(StringUtils::format(" / %d", curItemMaterial->currentnum));
				t_material1->setString(StringUtils::format("%0.f", ceil(curItemMaterial->requirednum * craft_factor)));
				t_material2->setPositionX(t_material1->getPositionX() + t_material1->getContentSize().width);
				if (ceil(curItemMaterial->requirednum * craft_factor) > curItemMaterial->currentnum)
				{
					t_material1->setTextColor(Color4B(185, 0, 5, 255));
					t_material2->setTextColor(Color4B(10, 105, 15, 255));
					isEnough = false;
					m_bIsMaterialEnough = false;
				}
				else
				{
					t_material1->setTextColor(Color4B(10, 105, 15, 255));
					t_material2->setTextColor(Color4B(10, 105, 15, 255));
				}
			}
		}
		while (needMaterialNum != 5)
		{
			needMaterialNum++;
			std::string name_bg = StringUtils::format("image_material_bg_%d", needMaterialNum);
			std::string name_image = "image_material_1";
			std::string name_label_1 = StringUtils::format("label_material_num_%d", needMaterialNum);
			std::string name_label_2 = StringUtils::format("label_material_num_%d_0", needMaterialNum);
			auto i_material1_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, name_bg));
			auto i_material1 = i_material1_bg->getChildByName<ImageView*>(name_image);
			auto t_material1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, name_label_1));
			auto t_material2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, name_label_2));
			i_material1_bg->loadTexture(goodsblackbg);
			i_material1->setVisible(false);
			t_material1->setVisible(false);
			t_material2->setVisible(false);
		}


		t_craft->setString(StringUtils::format("%d", craft_required));
		std::string str1 = numSegment(StringUtils::format("%d", silver_required));
		std::string str2 = numSegment(StringUtils::format("%lld", silver_current));
		t_silver->setString(str1);

		for(int i = 0;i< m_pShipBuildingResult->n_skills; i++)
		{
			if (m_pShipBuildingResult->skills[i]->id == SKILL_MANUFACTURING_EXPERT)
			{
				lv = m_pShipBuildingResult->skills[i]->level;
				break;
			}
		}
		itemid = currentShipDrawing->itemid;
		time = ceil(currentShipDrawing->requiredtime * craft_factor * craft_factor) / 60;
	}else
	{
		t_buildTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_DOCK_BUILDING_EQUIP"]);
		i_ship_info->setVisible(false);
		i_equip_info->setVisible(true);
		EquipdrawingDefine *currentEquipDrawing = nullptr;
		if (m_bIsAllDrawingDefine)
		{
			currentEquipDrawing = m_pEquipDrawingsResult->equipdrawing[index];
		}else
		{
			currentEquipDrawing = m_vCurrentCityCanEquipBuild[index];
		}
		if (!currentEquipDrawing)
		{
			return;
		}	
		auto i_draw_bg = dynamic_cast<Button*>(Helper::seekWidgetByName(i_equip_info, "image_draw_bg_2"));
		auto i_draw = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_equip_info,"image_ship"));
		auto t_draw = dynamic_cast<Text*>(Helper::seekWidgetByName(i_equip_info,"label_ship_name"));
		auto t_durable = dynamic_cast<Text*>(Helper::seekWidgetByName(i_equip_info,"label_sailors_num_0"));
		auto p_durable = dynamic_cast<Layout*>(Helper::seekWidgetByName(i_equip_info, "panel_durable"));
		auto p_property = dynamic_cast<Layout*>(Helper::seekWidgetByName(i_equip_info, "panel_property"));
		auto i_property_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_equip_info, "image_property_1"));
		auto i_property_2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_equip_info, "image_property_2"));
		auto t_atk = dynamic_cast<Text*>(Helper::seekWidgetByName(i_equip_info,"label_atk_num"));
		auto t_def = dynamic_cast<Text*>(Helper::seekWidgetByName(i_equip_info,"label_defnum"));
		auto t_des = dynamic_cast<Text*>(Helper::seekWidgetByName(i_equip_info, "label_select_itemname_0"));
		auto l_listView = dynamic_cast<ListView*>(Helper::seekWidgetByName(i_equip_info, "listview_content"));
		auto i_imagepull = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_equip_info, "image_pulldown_1"));
		auto p_range = dynamic_cast<Widget*>(Helper::seekWidgetByName(i_equip_info, "panel_range"));
		
		i_imagepull->setVisible(false);
		int* arr =  new int[5];
		convertToIntArr(currentEquipDrawing->equipmentproperties,arr,5);
		i_draw_bg->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func,this));
		i_draw_bg->setTag(currentEquipDrawing->equipmentid + START_INDEX);
		i_draw->loadTexture(getItemIconPath(currentEquipDrawing->equipmentid));
		t_draw->setString(getItemName(currentEquipDrawing->equipmentid));
		auto item = SINGLE_SHOP->getItemData()[currentEquipDrawing->equipmentid];
		t_durable->setString(StringUtils::format("%d/%d",item.max_durability,item.max_durability));
		switch (item.sub_type)
		{
			//船首装备
		case SUB_TYPE_SHIP_FIGUREHEAD:
			//撞角
		case SUB_TYPE_SHIP_EMBOLON:
			{
				i_property_1->setVisible(true);
				i_property_2->setVisible(false);
				i_property_1->setPositionX(p_property->getBoundingBox().size.width / 2);
				t_atk->setString(StringUtils::format("%d", arr[0]));
				p_range->setVisible(false);
				break;
			}
		case SUB_TYPE_SHIP_SAIL: //船帆
			{
				i_property_1->setVisible(true);
				i_property_2->setVisible(true);
				t_atk->setString(StringUtils::format("%d", arr[3]));
				t_def->setString(StringUtils::format("%d", arr[4]));
				auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_equip_info, "image_atk"));
				auto i_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_equip_info, "image_def"));
				i_atk->loadTexture(PROPERTY_ICON_PTAH[ICON_SPEED]);
				i_atk->setTouchEnabled(true);
				i_atk->setTag(ICON_SPEED + 1 + IMAGE_INDEX2);
				i_atk->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));


				i_def->loadTexture(PROPERTY_ICON_PTAH[ICON_STEERING]);
				i_def->setTouchEnabled(true);
				i_def->setTag(ICON_STEERING + 1 + IMAGE_INDEX2);
				i_def->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
				p_range->setVisible(false);
				break;
			}
		case SUB_TYPE_SHIP_ARMOUR: //装甲
			{
				i_property_1->setVisible(true);
				i_property_2->setVisible(false);
				t_atk->setString(StringUtils::format("%d", arr[1]));
				auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_equip_info, "image_atk"));
				i_atk->loadTexture(PROPERTY_ICON_PTAH[ICON_DEFENSE]);
				i_atk->setTouchEnabled(true);
				i_atk->setTag(ICON_DEFENSE + 1 + IMAGE_INDEX2);
				i_atk->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
				i_property_1->setPositionX(p_property->getBoundingBox().size.width / 2);
				p_range->setVisible(false);
				break;
			}
		case SUB_TYPE_SHIP_GUN: //火炮
			{
				i_property_1->setVisible(true);
				i_property_2->setVisible(false);
				t_atk->setString(StringUtils::format("%d", arr[0]));
				auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_equip_info, "image_atk"));
				i_atk->loadTexture(PROPERTY_ICON_PTAH[ICON_ATTACK]);
				i_atk->setTouchEnabled(true);
				i_atk->setTag(ICON_ATTACK + 1 + IMAGE_INDEX2);
				i_atk->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, this));
				i_property_1->setPositionX(p_property->getBoundingBox().size.width / 2);
				p_range->setVisible(true);
				auto l_range = p_range->getChildByName<ListView*>("listview_range");
				int range = item.property1;
				for (size_t i = 0; i < 3; i++)
				{
					auto i_range = dynamic_cast<ImageView*>(l_range->getItem(i));
					if (i < range)
					{
						i_range->loadTexture("cocosstudio/login_ui/common/range_1.png");
					}
					else
					{
						i_range->loadTexture("cocosstudio/login_ui/common/range_2.png");
					}
				}
				break;
			}
		default:
			break;
		}
	
		craft_required = currentEquipDrawing->requiredmanufacture;
		craft_current = m_pEquipDrawingsResult->currentcitymanufacture;
		float craft_factor = 1.0f;
		if(craft_current <= 0){
			craft_current = 1;
		}
		if(craft_required > craft_current ){
			button_warning->setVisible(true);
			craft_factor = sqrt(craft_required*1.0/craft_current);
			craft_factor = craft_factor > 2.0f ? craft_factor : 2.0f;
		}
		silver_required = ceil(currentEquipDrawing->coin * craft_factor * craft_factor);
		silver_current = m_pEquipDrawingsResult->coin;
	
		std::vector<ItemDefine*> equipMaterial;
		equipMaterial.push_back(currentEquipDrawing->requireditem1);
		equipMaterial.push_back(currentEquipDrawing->requireditem2);
		for (int i = 0; i < currentEquipDrawing->n_addrequireditem; i++)
		{
			equipMaterial.push_back(currentEquipDrawing->addrequireditem[i]);
		}
		int needMaterialNum = 0;
		for (int i = 0; i < equipMaterial.size(); i++)
		{
			if (equipMaterial.at(i)->requirednum > 0)
			{
				needMaterialNum++;
				auto curItemMaterial = equipMaterial.at(i);
				std::string name_bg = StringUtils::format("image_material_bg_%d", needMaterialNum);
				std::string name_image = "image_material_1";
				std::string name_label_1 = StringUtils::format("label_material_num_%d", needMaterialNum);
				std::string name_label_2 = StringUtils::format("label_material_num_%d_0", needMaterialNum);
				auto i_material1_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, name_bg));
				auto i_material1 = i_material1_bg->getChildByName<ImageView*>(name_image);
				auto t_material1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, name_label_1));
				auto t_material2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, name_label_2));
				i_material1_bg->setVisible(true);
				i_material1->setVisible(true);
				t_material1->setVisible(true);
				t_material2->setVisible(true);
				i_material1_bg->loadTexture(goodsbg);
				i_material1->loadTexture(getItemIconPath(curItemMaterial->item_id));
				i_material1->setTag(curItemMaterial->item_id);
				i_material1->setTouchEnabled(true);
				i_material1->addTouchEventListener(CC_CALLBACK_2(UIShipyard::ItemEvent,this));
				t_material2->setString(StringUtils::format(" / %d", curItemMaterial->currentnum));
				t_material1->setString(StringUtils::format("%0.f", ceil(curItemMaterial->requirednum * craft_factor)));
				t_material2->setPositionX(t_material1->getPositionX() + t_material1->getContentSize().width);
				if (ceil(curItemMaterial->requirednum * craft_factor)> curItemMaterial->currentnum)
				{
					t_material1->setTextColor(Color4B(185, 0, 5, 255));
					t_material2->setTextColor(Color4B(10, 105, 15, 255));
					m_bIsMaterialEnough = false;
					isEnough = false;
				}
				else
				{
					t_material1->setTextColor(Color4B(10, 105, 15, 255));
					t_material2->setTextColor(Color4B(10, 105, 15, 255));
				}
			}
		}
		while (needMaterialNum != 5)
		{
			needMaterialNum++;
			std::string name_bg = StringUtils::format("image_material_bg_%d", needMaterialNum);
			std::string name_image = "image_material_1";
			std::string name_label_1 = StringUtils::format("label_material_num_%d", needMaterialNum);
			std::string name_label_2 = StringUtils::format("label_material_num_%d_0", needMaterialNum);
			auto i_material1_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, name_bg));
			auto i_material1 = i_material1_bg->getChildByName<ImageView*>(name_image);
			auto t_material1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, name_label_1));
			auto t_material2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, name_label_2));
			i_material1_bg->loadTexture(goodsblackbg);
			i_material1->setVisible(false);
			t_material1->setVisible(false);
			t_material2->setVisible(false);
		}
		setBgButtonFormIdAndType(i_draw_bg, currentEquipDrawing->equipmentid, ITEM_TYPE_SHIP_EQUIP);
		setTextColorFormIdAndType(t_draw, currentEquipDrawing->equipmentid, ITEM_TYPE_SHIP_EQUIP);
		t_craft->setString(StringUtils::format("%d", craft_required));
		std::string str1 = numSegment(StringUtils::format("%d", silver_required));
		t_silver->setString(str1);


		for(int i = 0;i< m_pEquipBuildingResult->n_skills; i++)
		{
			if (m_pEquipBuildingResult->skills[i]->id == SKILL_MANUFACTURING_EXPERT)
			{
				lv = m_pEquipBuildingResult->skills[i]->level;
				break;
			}
		}
		itemid = currentEquipDrawing->itemid;
		
		time = ceil(currentEquipDrawing->requiredtime * craft_factor *craft_factor) / 60;
		delete[] arr;
	}
	bool isRightCity = false;
	std::string s_time;
	std::string cityList[50];
	int nIndex = 0;
	std::string tempCity = SINGLE_SHOP->getDrawingsDesignInfo()[itemid].city_id_list;
	tempCity += ",";
	int startPos = 0;
	for (int i = 0; ;i++)
	{
		if (tempCity.empty())
		{
			break;
		}
		std::string::size_type index = tempCity.find(',',startPos);
		if (index != std::string::npos)
		{
			std::string subStr = tempCity.substr(startPos,index - startPos);
			int cityId = atoi(subStr.c_str());
			if (cityId == SINGLE_HERO->m_iCityID)
			{
				isRightCity = true;
			}
			cityList[nIndex] = SINGLE_SHOP->getCitiesInfo()[cityId].name;
			startPos = index + 1;
			nIndex++;
		}else
		{
			break;
		}
	}

	std::string s_cityList;
	for (int i = 0; i < nIndex; i++)
	{
		s_cityList += cityList[i];
		if (i == nIndex - 1)
		{

		}
		else if (i == nIndex - 2)
		{
			s_cityList += SINGLE_SHOP->getTipsInfo()["TIP_MAIN_AND2"];
		}
		else
		{
			s_cityList += SINGLE_SHOP->getTipsInfo()["TIP_MAIN_AND1"];
		}
	}

	std::string old_value = "[city]";
	std::string new_value = s_cityList;
	std::string str = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_CITY_WORKSHOP"];
	repalce_all_ditinct(str,old_value,new_value);
	auto l_city = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_working_slot"));
	t_city->setString(str);
	t_city->setContentSize(Size(t_city->getContentSize().width,getLabelHight(str,t_city->getContentSize().width,t_city->getFontName())));
	l_city->refreshView();

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown_2 = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown_2->setPositionX(button_pulldown_2->getBoundingBox().size.width / 2 - button_pulldown_2->getBoundingBox().size.width / 2 + 2.5);
	addListViewBar(l_city, image_pulldown);

	s_time += StringUtils::format("%d ",int(time));
	s_time += SINGLE_SHOP->getTipsInfo()["TIP_HOUR"];
	s_time += StringUtils::format(" %d ",int((time - int(time))*60));
	s_time += SINGLE_SHOP->getTipsInfo()["TIP_MINUTE"];
	if (lv > 0)
	{
		i_skill->setVisible(true);
		s_time += StringUtils::format("(-%0.2f%%)",SINGLE_SHOP->getSkillTrees()[SKILL_MANUFACTURING_EXPERT].effect_per_lv*lv/SKILL_DER_MIN);
		i_skill->ignoreContentAdaptWithSize(false);
		i_skill->loadTexture(getSkillIconPath(SKILL_MANUFACTURING_EXPERT, SKILL_TYPE_PLAYER));
		auto t_skill_num = i_skill->getChildByName<Text*>("text_item_skill_lv");
		setTextSizeAndOutline(t_skill_num,lv);
	}else
	{
		i_skill->setVisible(false);
	}

	t_time->setString(s_time);
	
	if (craft_required > craft_current)
	{
		t_craft->setTextColor(Color4B(198, 2, 5, 255));
		t_craft->setString(t_craft->getString() + "[" + SINGLE_SHOP->getTipsInfo()["TIP_HIGH_COST"] + "]");
		//isEnough = false;
	}else
	{
		t_craft->setTextColor(Color4B(56,28,1,255));
	}

	if (silver_required > silver_current)
	{
		t_silver->setTextColor(Color4B(198, 2, 5, 255));
		isEnough = false;
		m_bIsCoinEnough = false;
	}else
	{
		m_bIsCoinEnough = true;
		t_silver->setTextColor(Color4B(56,28,1,255));
	}
	if (isRightCity == false)
	{
		isEnough = false;
		m_bIsRightCity = false;
	}
	else
	{
		m_bIsRightCity = true;
	}

	button_warning->setPositionX(t_craft->getBoundingBox().size.width + t_craft->getPositionX() + button_warning->getBoundingBox().size.width * 0.8);

	/*auto b_cofirm = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_building_cofirm"));
	if (isEnough)
	{
	b_cofirm->setBright(true);
	b_cofirm->setTouchEnabled(true);
	}else
	{
	b_cofirm->setBright(false);
	b_cofirm->setTouchEnabled(false);
	}*/
}

void UIShipyard::ItemEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushItemsDetail(nullptr,((Widget*)pSender)->getTag(),true);
	}
}

void UIShipyard::showBuildResult(const BuildShipResult *result)
{
	openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_RESULT_CSB]);
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_RESULT_CSB]);
	auto p_item = view->getChildByName<Layout*>("panel_item_1");
	auto i_div = view->getChildByName<ImageView*>("image_div_1");
	auto p_cost = view->getChildByName<Layout*>("panel_cost");
	auto l_result = view->getChildByName<ListView*>("listview_result");
	auto image_pull = view->getChildByName<ImageView*>("image_pulldown");
	if (result->consumedcount1 > 0)
	{
		auto item = p_item->clone();
		auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item,"image_item"));
		auto t_name = item->getChildByName<Text*>("label_items_name");
		auto t_num = item->getChildByName<Text*>("label_num");
		i_item->loadTexture(getItemIconPath(result->consumeditemid1));
		t_name->setString(getItemName(result->consumeditemid1));
		t_num->setString(numSegment(StringUtils::format("%d",result->consumedcount1)));
		auto div = i_div->clone();
		l_result->pushBackCustomItem(item);
		l_result->pushBackCustomItem(div);
	}
	if (result->consumedcount2 > 0)
	{
		auto item = p_item->clone();
		auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_item"));
		auto t_name = item->getChildByName<Text*>("label_items_name");
		auto t_num = item->getChildByName<Text*>("label_num");
		i_item->loadTexture(getItemIconPath(result->consumeditemid2));
		t_name->setString(getItemName(result->consumeditemid2));
		t_num->setString(numSegment(StringUtils::format("%d", result->consumedcount2)));
		auto div = i_div->clone();
		l_result->pushBackCustomItem(item);
		l_result->pushBackCustomItem(div);
	}
	if (result->consumedcount3 > 0)
	{
		auto item = p_item->clone();
		auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_item"));
		auto t_name = item->getChildByName<Text*>("label_items_name");
		auto t_num = item->getChildByName<Text*>("label_num");
		i_item->loadTexture(getItemIconPath(result->consumeditemid3));
		t_name->setString(getItemName(result->consumeditemid3));
		t_num->setString(numSegment(StringUtils::format("%d", result->consumedcount3)));
		auto div = i_div->clone();
		l_result->pushBackCustomItem(item);
		l_result->pushBackCustomItem(div);
	}
	if (result->consumedcount4 > 0)
	{
		auto item = p_item->clone();
		auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_item"));
		auto t_name = item->getChildByName<Text*>("label_items_name");
		auto t_num = item->getChildByName<Text*>("label_num");
		i_item->loadTexture(getItemIconPath(result->consumeditemid4));
		t_name->setString(getItemName(result->consumeditemid4));
		t_num->setString(numSegment(StringUtils::format("%d", result->consumedcount4)));
		auto div = i_div->clone();
		l_result->pushBackCustomItem(item);
		l_result->pushBackCustomItem(div);
	}
	if (result->consumedcount5 > 0)
	{
		auto item = p_item->clone();
		auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_item"));
		auto t_name = item->getChildByName<Text*>("label_items_name");
		auto t_num = item->getChildByName<Text*>("label_num");
		i_item->loadTexture(getItemIconPath(result->consumeditemid5));
		t_name->setString(getItemName(result->consumeditemid5));
		t_num->setString(numSegment(StringUtils::format("%d", result->consumedcount5)));
		auto div = i_div->clone();
		l_result->pushBackCustomItem(item);
		l_result->pushBackCustomItem(div);
	}
	auto p_cost_clone = p_cost->clone();
	auto t_cost = p_cost_clone->getChildByName<Text*>("label_cost_num");
	t_cost->setString(numSegment(StringUtils::format("%d", result->cost)));
	l_result->pushBackCustomItem(p_cost_clone);
	auto b_image = image_pull->getChildByName("button_pulldown");
	b_image->setPositionX(b_image->getPositionX() - b_image->getContentSize().width / 2);
	addListViewBar(l_result, image_pull);
}

void UIShipyard::showBuildItemDetails(const int nIndex)
{
	openView(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_SLOT_CSB]);
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_BUILD_SLOT_CSB]);
	auto b_item_bg = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_ship_bg"));
	auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_ship"));
	auto t_item = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ship_name"));
	auto t_city = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_city"));
	auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_time"));
	auto l_time = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view,"progressbar_building_time"));
	auto b_repair = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_repair_v"));
	auto b_cancle = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_cancel_buiding"));
	auto i_bgImage = b_item_bg->getChildByName<ImageView*>("image_item_bg_lv");

	b_repair->setTag(nIndex + START_INDEX);
	b_cancle->setTag(nIndex + START_INDEX);
	char buf[30];
	if (strcmp(m_pMinorPressButton->getName().data(),"button_ship_building") == 0)
	{
		i_item->loadTexture(getShipIconPath(m_pShipBuildingResult->ship[nIndex]->ship->sid));
		t_item->setString(getShipName(m_pShipBuildingResult->ship[nIndex]->ship->sid));
		t_city->setString(SINGLE_SHOP->getCitiesInfo()[m_pShipBuildingResult->ship[nIndex]->cityid].name);
		int lenTime = m_vUpdateTimeList[nIndex];
		time_t myTime = lenTime;
		tm *nowTime = gmtime(&myTime);
		//strftime(buf,30,"%H:%M:%S",nowTime);
		int h = (nowTime->tm_mday-1)*24 + nowTime->tm_hour;
		int m = nowTime->tm_min;
		int s = nowTime->tm_sec;
		sprintf(buf,"%02d:%02d:%02d",h,m,s);
		t_time->setString(buf);
		int totalTime = m_pShipBuildingResult->ship[nIndex]->endtime - m_pShipBuildingResult->ship[nIndex]->starttime;
		int usedTime = totalTime - lenTime;
		l_time->setPercent(100.0 * usedTime / totalTime);

		b_item_bg->setTag(m_pShipBuildingResult->ship[nIndex]->ship->sid + START_INDEX);
		setBgButtonFormIdAndType(b_item_bg, m_pShipBuildingResult->ship[nIndex]->ship->sid, ITEM_TYPE_SHIP);
		setBgImageColorFormIdAndType(i_bgImage, m_pShipBuildingResult->ship[nIndex]->ship->sid, ITEM_TYPE_SHIP);
		setTextColorFormIdAndType(t_item, m_pShipBuildingResult->ship[nIndex]->ship->sid, ITEM_TYPE_SHIP);
	}else
	{
		i_item->loadTexture(getItemIconPath(m_pEquipBuildingResult->equipment[nIndex]->equipmentid));
		t_item->setString(getItemName(m_pEquipBuildingResult->equipment[nIndex]->equipmentid));
		t_city->setString(SINGLE_SHOP->getCitiesInfo()[m_pEquipBuildingResult->equipment[nIndex]->cicyid].name);
		int lenTime = (time_t)m_vUpdateTimeList[nIndex];
		time_t myTime = lenTime;
		tm *nowTime = gmtime(&myTime);
		//strftime(buf,30,"%H:%M:%S",nowTime);
		int h = (nowTime->tm_mday-1)*24 + nowTime->tm_hour;
		int m = nowTime->tm_min;
		int s = nowTime->tm_sec;
		sprintf(buf,"%02d:%02d:%02d",h,m,s);
		t_time->setString(buf);
		int totalTime = m_pEquipBuildingResult->equipment[nIndex]->endtime - m_pEquipBuildingResult->equipment[nIndex]->starttime;
		int usedTime = totalTime - lenTime;
		l_time->setPercent(100.0 * usedTime / totalTime);
		
		b_item_bg->setTag(m_pEquipBuildingResult->equipment[nIndex]->equipmentid + START_INDEX);
		setBgButtonFormIdAndType(b_item_bg, m_pEquipBuildingResult->equipment[nIndex]->equipmentid, ITEM_TYPE_SHIP_EQUIP);
		setBgImageColorFormIdAndType(i_bgImage, m_pEquipBuildingResult->equipment[nIndex]->equipmentid, ITEM_TYPE_SHIP_EQUIP);
		setTextColorFormIdAndType(t_item, m_pEquipBuildingResult->equipment[nIndex]->equipmentid, ITEM_TYPE_SHIP_EQUIP);
	}
}

/*
*船只属性变化后颜色变化--有可能变绿有可能变红
*/
void UIShipyard::setTextGreen(Widget * root, int index, bool isAdd)
{	
	auto t_atknum = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_atk_num"));
	auto t_defnum = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_defnum"));
	auto t_durable = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_ship_durable_num_2"));
	auto t_speednum = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_speed_num"));
	auto t_steernum = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_steering_num"));
	auto t_sailors = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_sailors_num_2"));
	auto t_weight = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_ship_weight_num_2"));
	auto t_supply = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "label_ship_supply_num_2"));
	switch (index)
	{
	case ICON_ATTACK:
		if (t_atknum)
		{
			if (isAdd)
			{
				t_atknum->setTextColor(Color4B(1, 120, 5, 255));
			}
			else
			{
				t_atknum->setTextColor(TEXT_RED);
			}
		}
		break;
	case ICON_DEFENSE:
		if (t_defnum)
		{
			if (isAdd)
			{
				t_defnum->setTextColor(Color4B(1, 120, 5, 255));
			}
			else
			{
				t_defnum->setTextColor(TEXT_RED);
			}
		}
		break;
	case ICON_DURABLE:
		if (t_durable)
		{
			if (isAdd)
			{
				t_durable->setTextColor(Color4B(1, 120, 5, 255));
			}
			else
			{
				t_durable->setTextColor(TEXT_RED);
			}
		}
		break;
	case ICON_SPEED:
		if (t_speednum)
		{
			if (isAdd)
			{
				t_speednum->setTextColor(Color4B(1, 120, 5, 255));
			}
			else
			{
				t_speednum->setTextColor(TEXT_RED);
			}
		}
		break;
	case ICON_STEERING:
		if (t_steernum)
		{
			if (isAdd)
			{
				t_steernum->setTextColor(Color4B(1, 120, 5, 255));
			}
			else
			{
				t_steernum->setTextColor(TEXT_RED);
			}
		}
		break;
	case ICON_SAILORS:
		if (t_sailors)
		{
			if (isAdd)
			{
				t_sailors->setTextColor(Color4B(1, 120, 5, 255));
			}
			else
			{
				t_sailors->setTextColor(TEXT_RED);
			}
		}
		break;
	case ICON_CAPACITY:
		if (t_weight)
		{
			if (isAdd)
			{
				t_weight->setTextColor(Color4B(1, 120, 5, 255));
			}
			else
			{
				t_weight->setTextColor(TEXT_RED);
			}
		}
		break;
	case ICON_SUPPLY:
		if (t_supply)
		{
			if (isAdd)
			{
				t_supply->setTextColor(Color4B(1, 120, 5, 255));
			}
			else
			{
				t_supply->setTextColor(TEXT_RED);
			}
		}
		break;
	default:
		break;
	}
}

void UIShipyard::showChiefDialog()
{
	openView(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB], 10);
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
	auto bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
	bargirl_dialog->setTouchEnabled(true);
	auto i_bargirl = bargirl_dialog->getChildByName<ImageView*>("image_bargirl");
	i_bargirl->ignoreContentAdaptWithSize(false);
	
	i_bargirl->loadTexture(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_SHIP_BOSS));
	bargirl_dialog->setPosition(STARTPOS);
	auto i_dialog_bg = bargirl_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_content = i_dialog_bg->getChildByName<Text*>("label_content");
	auto t_bargirl = i_dialog_bg->getChildByName<Text*>("label_bargirl");
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");
	t_bargirl->setString(SINGLE_SHOP->getTipsInfo()["TIP_SHIPYARD_CHEIF_NAME"]);
	t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_SHIPYARD_CHEIF_DIALOG"]);
	auto button_backcity = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_backcity"));
	button_backcity->addTouchEventListener(CC_CALLBACK_2(UIShipyard::barGirlButtonEvent,this));
	//V票银币
	auto label_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_silver_num"));
	label_silver_num->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iCoin)));
	auto label_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_gold_num"));
	label_gold_num->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iGold)));
	setTextFontSize(label_gold_num);
	setTextFontSize(label_silver_num);
	//背景图
	auto image_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_bg"));
	image_bg->loadTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_DOCK_AREA));
	i_anchor->setVisible(false);
	if (i_bargirl->getPositionX() < 0)
	{
		t_content->setOpacity(0);
		i_dialog_bg->setOpacity(0);
		t_bargirl->setOpacity(0);
		
		i_bargirl->runAction(MoveBy::create(0.5, Vec2(i_bargirl->boundingBox().size.width, 0)));
		i_dialog_bg->setPositionX(0.63*i_dialog_bg->getBoundingBox().size.width);
		t_content->runAction(Sequence::create(DelayTime::create(0.5), FadeIn::create(0.5), nullptr));
		i_dialog_bg->runAction(Sequence::create(DelayTime::create(0.5), FadeIn::create(0.5), nullptr));
		t_bargirl->runAction(Sequence::create(DelayTime::create(0.5), FadeIn::create(0.5), nullptr));
		//i_anchor->runAction(Sequence::create(DelayTime::create(0.5), FadeIn::create(0.5), nullptr));
	}
	else
	{
		t_content->runAction(Sequence::create(FadeOut::create(0.5), FadeIn::create(0.5), nullptr));
		i_dialog_bg->runAction(Sequence::create(FadeOut::create(0.5), FadeIn::create(0.5), nullptr));
		t_bargirl->runAction(Sequence::create(FadeOut::create(0.5), FadeIn::create(0.5), nullptr));
		//i_anchor->runAction(Sequence::create(FadeOut::create(0.5), FadeIn::create(0.5), nullptr));
		bargirl_dialog->setTouchEnabled(false);
	}

	auto image_notify = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_notify"));
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
	image_notify->setVisible(b_notify);
	this->scheduleOnce(schedule_selector(UIShipyard::mainButtonMoveToLeft),1);
}

void UIShipyard::barGirlButtonEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	m_nMainIndex = button->getTag();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	
	
	if (m_nMainIndex && m_nMainIndex < 4)
	{
		mainButtonMoveToRight();
		return;
	}
	else
	{
		//退出按钮
		if (isButton(button_backcity))
		{
			button_callBack();
			return;
		}
	}
}

void UIShipyard::mainButtonMoveToLeft(const float fTime)
{
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
	auto p_tavern = view->getChildByName<Widget*>("panel_shipyard");
	p_tavern->setPosition(STARTPOS);
	auto b_chat = p_tavern->getChildByName<Button*>("button_role");
	auto b_order = p_tavern->getChildByName<Button*>("button_items");
	auto b_sailor = p_tavern->getChildByName<Button*>("button_localitems");
	
	auto b_pos_x = b_chat->getPositionX();
	auto winsize = Director::getInstance()->getWinSize();
	b_chat->setPositionX(winsize.width + b_chat->boundingBox().size.width / 2);
	b_order->setPositionX(winsize.width + b_order->boundingBox().size.width / 2);
	b_sailor->setPositionX(winsize.width + b_sailor->boundingBox().size.width / 2);
	b_chat->addTouchEventListener(CC_CALLBACK_2(UIShipyard::barGirlButtonEvent,this));
	b_order->addTouchEventListener(CC_CALLBACK_2(UIShipyard::barGirlButtonEvent,this));
	b_sailor->addTouchEventListener(CC_CALLBACK_2(UIShipyard::barGirlButtonEvent,this));
	b_chat->runAction(Sequence::create(MoveBy::create(0.2, Vec2(-b_chat->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	b_order->runAction(Sequence::create(DelayTime::create(0.05), MoveBy::create(0.2, Vec2(-b_order->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	b_sailor->runAction(Sequence::create(DelayTime::create(0.1), MoveBy::create(0.2, Vec2(-b_sailor->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));

	//目前检测小伙伴
	notifyCompleted(NONE);
}

void UIShipyard::mainButtonMoveToRight()
{
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_DIALOG_CSB]);
	auto p_tavern = view->getChildByName<Widget*>("panel_shipyard");
	p_tavern->setPosition(STARTPOS);
	auto b_chat = p_tavern->getChildByName<Button*>("button_role");
	auto b_order = p_tavern->getChildByName<Button*>("button_items");
	auto b_sailor = p_tavern->getChildByName<Button*>("button_localitems");
	
	auto winsize = Director::getInstance()->getWinSize();
	b_chat->runAction(Sequence::create(MoveBy::create(0.2, Vec2(b_chat->boundingBox().size.width  + 50, 0)), nullptr));
	b_order->runAction(Sequence::create(DelayTime::create(0.05), MoveBy::create(0.2, Vec2(b_order->boundingBox().size.width + 50, 0)), nullptr));
	b_sailor->runAction(Sequence::create(DelayTime::create(0.1), MoveBy::create(0.2, Vec2(b_sailor->boundingBox().size.width + 50, 0)), nullptr));

	auto bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
	bargirl_dialog->setTouchEnabled(false);
	auto i_dialog_bg = bargirl_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_content = i_dialog_bg->getChildByName<Text*>("label_content");
	auto t_batgirl = i_dialog_bg->getChildByName<Text*>("label_bargirl");
	auto i_bargirl = bargirl_dialog->getChildByName<ImageView*>("image_bargirl");
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");
	i_anchor->stopAllActions();
	i_anchor->setVisible(false);
	t_content->runAction(FadeOut::create(0.5));
	i_dialog_bg->runAction(FadeOut::create(0.5));
	t_batgirl->runAction(FadeOut::create(0.5));
	i_bargirl->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), MoveBy::create(0.5, Vec2(-i_bargirl->boundingBox().size.width, 0))));
	bargirl_dialog->runAction(Sequence::createWithTwoActions(DelayTime::create(1), MoveTo::create(0, ENDPOS)));
	view->runAction(Sequence::create(DelayTime::create(1),CallFunc::create(this, callfunc_selector(UIShipyard::initStaticData)),nullptr));
}
std::string UIShipyard::CutItemName(std::string itemName)
{
	auto nameLen = itemName.length();
	LanguageType nType = LanguageType(Utils::getLanguage());
	switch (nType)
	{
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
	case cocos2d::LanguageType::CHINESE:
	{
			if (nameLen > 3 * 8)//utf8大多数一个汉字3字节
			{
				itemName = itemName.substr(0, 3 * 6) + "...";
			}
			break;
	}
	default:
	{
			if (nameLen > 16)
			{
				itemName = itemName.substr(0, 13) + "...";
			}
			break;
	}
	}
	return itemName;
}
void UIShipyard::flushEquipInfo(EquipmentDefine* equipInfo)
{
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
	if (getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]))
	{
		m_pShipEquipView->flushRepairEquipmentView(equipInfo);
	}
	if (getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_WORKSHOP_CSB]))
	{
		if (m_curSelectEquipment != nullptr)
		{
			auto child = Helper::seekWidgetByName(m_curSelectEquipment, "equip_broken_icon");
			if (child != nullptr)
			{
				auto i_bg = m_curSelectEquipment->getChildByName<ImageView*>("image_ship");
				child->removeFromParentAndCleanup(true);
				setBrokenEquipRed(i_bg, false);
			}
			int index = m_curSelectEquipment->getTag() - 2000;
			ReinforceEquipDefine* curEquipInfo;
			if (index < m_pReinforceEquipResult->n_equipmentonship)
			{
				curEquipInfo = m_pReinforceEquipResult->equipmentonship[index];
			}
			else
			{
				curEquipInfo = m_pReinforceEquipResult->equipmentsinbag[index - m_pReinforceEquipResult->n_equipmentonship];
			}
			curEquipInfo->durable = equipInfo->durable;
			auto parent = dynamic_cast<Widget*>(m_curSelectEquipment->getParent());
			updateReinforceEquip(parent, curEquipInfo, index);
		}
	}
}
int UIShipyard::getShipOrEquipmentProficiencyLevel(int proficiencyId)
{
	int level = 0;
	for (int i = 0; i < m_proficiencyResult->n_values; i++)
	{
		auto m_proficiency = m_proficiencyResult->values[i];
		if (m_proficiency->proficiencyid == proficiencyId)
		{
			level = getProficiencyLevel(m_proficiency->value);
		}
	}
	return level;
}
int UIShipyard::getProficiencyLevel(int proficiencyNum)
{
	int level = 0;
	if (proficiencyNum < 100)
	{
		level = 0;
	}
	else if (proficiencyNum < 300)
	{
		level = 1;
	}
	else if (proficiencyNum < 800)
	{
		level = 2;
	}
	else if (proficiencyNum < 2300)
	{
		level = 3;
	}
	else if (proficiencyNum < 7300)
	{
		level = 4;
	}
	else if (proficiencyNum >= 7300)
	{
		level = 5;
	}
	return level;
}
int UIShipyard::getNeedProficiencyLevel(int sid)
{
	SHIP_RES ship = SINGLE_SHOP->getShipData()[sid];
	int proid = ship.proficiency_need;
	int proficnencylevel = 0;
	if (proid != 0)
	{
		proficnencylevel = ship.rarity;
	}
	else
	{
		proficnencylevel = 0;
	}
	return proficnencylevel;
}
