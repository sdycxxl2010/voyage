#include "UIShipyardFleet.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UIShipyard.h"
#include "UIShipyardEquip.h"

#include "Utils.h"
#include <UITips.h>

UIShipyardFleet::UIShipyardFleet(UIBasicLayer* parent):
	m_nTempIndex(-1),
	m_nGlobalIndex(-1),
	m_parent(parent),
	m_pPressButton(nullptr),
	m_pTempButton(nullptr),
	bIsEnter(false)
{
	m_pFleetAndDockResult = nullptr;
	m_bShipToDockFormFleet = false;
	m_pInputText = nullptr;
	m_vUnusedCaptain.clear();
	m_vChangeShipNameId.clear();
	m_vAllShips.clear();
	m_vFleetShip.clear();
	m_vCatchShip.clear();
	m_vDockShip.clear();
	m_vCaptainList.clear();
	m_pFleetList = nullptr;
	m_pDockList = nullptr;
	m_nInputNameIndex = -1;
	m_vIid.clear();
	m_nConfirmIndex = CONFIRM_INDEX_SHIP_TO_DOCK;
}

UIShipyardFleet::~UIShipyardFleet()
{
	if (m_pFleetAndDockResult)
	{
		get_fleet_and_dock_ships_result__free_unpacked(m_pFleetAndDockResult, 0);
	}
}

UIShipyardFleet* UIShipyardFleet::createFleet(UIBasicLayer* parent)
{
	UIShipyardFleet* fv = new UIShipyardFleet(parent);
	if (fv && fv->init())
	{
		fv->autorelease();
		return fv;
	}
	CC_SAFE_DELETE(fv);
	return nullptr;
}

bool UIShipyardFleet::init()
{
	bool pRet = false;
	do 
	{
		UIBasicLayer::init();
		m_pInputText = ui::EditBox::create(Size(100,100), "input.png");
		m_pInputText->setPosition(Vec2(-200, -200));
		m_pInputText->setDelegate(this);
		m_pInputText->setTag(300);
		m_pInputText->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
		m_pInputText->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		m_parent->addChild(m_pInputText, 100);
		pRet = true;
	} while (0);
	return pRet;
}

void UIShipyardFleet::buttonEventDis(Widget* target,std::string name)
{
	//增加船长
	if (isButton(button_head_player))
	{
		m_nGlobalIndex = target->getTag() - START_INDEX;
		if (m_nGlobalIndex > m_vCaptainList.size())
		{
			m_nGlobalIndex = m_vCaptainList.size() - 1;
		}
		if (m_nGlobalIndex == 0)
		{
			UIInform::getInstance()->openInformView(m_parent);
			UIInform::getInstance()->openConfirmYes("TIP_DOCK_FIRST_CAPTAIN_NOT");
			return;
		}
		if(m_pPressButton)
		{
			m_pPressButton->setBright(true);
			m_pPressButton = nullptr;
		}
		pushCaptainList(m_pFleetAndDockResult->captains,m_pFleetAndDockResult->n_captains);
		return;
	}
	//修改船只名称
	if (isButton(button_changename))
	{
		dynamic_cast<UIShipyard*>(m_parent)->setFleetFinishData();
		if (m_pInputText)
		{
			auto parent = target->getParent();
			m_nInputNameIndex = parent->getTag();
			auto w_parent = dynamic_cast<Widget*>(parent);
			auto t_inputName = dynamic_cast<Text*>(w_parent->getChildByName<Widget*>("label_ship_name"));	
			m_pInputText->setText((t_inputName->getString()).c_str());
			m_pInputText->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
		}
		return;
	}
	//选择船长
	if (isButton(panel_captain))
	{
		m_nTempIndex = target->getTag() - START_INDEX;
		if (m_nTempIndex > m_vUnusedCaptain.size())
		{
			m_nTempIndex = m_vUnusedCaptain.size() - 1;
		}
		switchCaptain();
		/*
		auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
		auto b_finish = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_finish"));
		b_finish->setBright(true);
		b_finish->setTouchEnabled(true);
		*/
		dynamic_cast<UIShipyard*>(m_parent)->setFleetFinishData();
		m_parent->closeView();
		return;
	}
	//提示
	if (isButton(button_confirm_yes))
	{
		/*
		if (m_pTempButton && strcmp(m_pTempButton->getName().data(), "button_finish") == 0)
		{
			dynamic_cast<DockLayer*>(m_parent)->setFleetFinishData(true);
			sendDataToServer();
			return;
		}
		*/
		if (m_nConfirmIndex == CONFIRM_INDEX_SHIP_TO_DOCK)
		{
			if (m_pTempButton && strcmp(m_pTempButton->getName().data(), "button_change") == 0)
			{
				addShipToDock(m_pPressButton->getTag() - START_INDEX);
				updateFee();
				/*
				auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
				auto b_finish = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_finish"));
				b_finish->setBright(true);
				b_finish->setTouchEnabled(true);
				*/
				dynamic_cast<UIShipyard*>(m_parent)->setFleetFinishData();
				return;
			}
		}
		else
		{
			if (m_pPressButton && m_pPressButton->getName() == "button_dock_ship")
			{
				addShipToCatch(m_pPressButton->getTag() - START_INDEX);
				dynamic_cast<UIShipyard*>(m_parent)->setFleetFinishData();
				return;
			}
		}
		return;
	}
	//调换船只里的位置
	if (isButton(button_ranking))
	{
		dynamic_cast<UIShipyard*>(m_parent)->setFleetFinishData();
		m_parent->openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_POSITION_CSB]);
		auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_POSITION_CSB]);
		m_nGlobalIndex = target->getTag() - START_INDEX;
		if (m_nGlobalIndex > m_vCaptainList.size())
		{
			m_nGlobalIndex = m_vCaptainList.size() - 1;
		}
		auto l_result = view->getChildByName<ListView*>("listview_result");
		for (int i = 0; i < 4; i++)
		{
			auto item = l_result->getItem(i);
			if (i < m_vFleetShip.size() - 1)
			{
				auto i_num = item->getChildByName<ImageView*>("image_num");
				std::string path;
				if (i < m_nGlobalIndex)
				{
					path = getPositionIconPath(i + 1);
				}else
				{
					path = getPositionIconPath(i + 2);
				}
				i_num->loadTexture(path);
				item->setVisible(true);
				item->setTag(i + START_INDEX);
			}else
			{
				item->setVisible(false);
			}
		}
		return;
	}
	//选择位置
	if (isButton(button_num))
	{
		dynamic_cast<UIShipyard*>(m_parent)->closeView();
		int index = target->getTag() - START_INDEX;
		if (index  >= m_nGlobalIndex)
		{
			index++;
		}
		switchShipOrder(m_nGlobalIndex,index);
		auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
		/*
		auto b_finish = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_finish"));
		b_finish->setBright(true);
		b_finish->setTouchEnabled(true);
		*/
		dynamic_cast<UIShipyard*>(m_parent)->setFleetFinishData();
		return;
	}
	/*
	//保存
	if (isButton(button_finish)) 
	{
		m_pTempButton = target;
		InformView::getInstance()->openInformView(m_parent);
		InformView::getInstance()->openConfirmYesOrNO("TIP_DOCK_FLEET_CONFIRM_SAVE_TITLE","TIP_DOCK_FLEET_CONFIRM_SAVE");
		return;
	}
	*/
	//选择船只
	if (isButton(button_fleet_ship) || isButton(button_dock_ship))
	{
		if (m_pPressButton)
		{
			m_pPressButton->setBright(true);
		}
		target->setBright(false);
		m_pPressButton = target;
		auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
		auto b_change = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_change"));
		if (isButton(button_fleet_ship))
		{
			b_change->loadTextureNormal("cocosstudio/login_ui/shipyard_720/change_r.png");
		}
		else
		{
			b_change->loadTextureNormal("cocosstudio/login_ui/shipyard_720/change_l.png");
		}
		return;
	}
	//更改按钮
	if (isButton(button_change))
	{
		if (m_pPressButton == nullptr)
		{
			UIInform::getInstance()->openInformView(m_parent);
			UIInform::getInstance()->openConfirmYes("TIP_DOCK_NOT_SELECT_SHIP");
			return;
		}
		if (m_pPressButton->getName() == "button_fleet_ship")
		{
			int index = m_pPressButton->getTag() - START_INDEX;
			//舰队中必须有一个旗舰
			if (m_vFleetShip.size() == 1 && index < m_vFleetShip.size()) 
			{
				UIInform::getInstance()->openInformView(m_parent);
				UIInform::getInstance()->openConfirmYes("TIP_DOCK_FLEET_NOT_NULL");
				return;
			}

			if (m_bShipToDockFormFleet || index >= m_vFleetShip.size())
			{
				addShipToDock(index);
				updateFee();
				auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
				auto b_change = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_change"));
				b_change->loadTextureNormal("cocosstudio/login_ui/shipyard_720/change.png");
				/*
				auto b_finish = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_finish"));
				b_finish->setBright(true);
				b_finish->setTouchEnabled(true);
				*/
				dynamic_cast<UIShipyard*>(m_parent)->setFleetFinishData();
			}else
			{
				m_bShipToDockFormFleet = true;
				m_pTempButton = target;
				m_nConfirmIndex = CONFIRM_INDEX_SHIP_TO_DOCK;
				UIInform::getInstance()->openInformView(m_parent);
				UIInform::getInstance()->openConfirmYesOrNO("TIP_DOCK_CANCEL_DOCK_FORM_FLEET_TITLE","TIP_DOCK_CANCEL_DOCK_FORM_FLEET");
			}	
			return;
		}
		
		if (m_pPressButton->getName() == "button_dock_ship")
		{
			auto shipDefine = getShipDefine(m_vDockShip.at(m_pPressButton->getTag() - START_INDEX));
//			auto nation_limit = SINGLE_SHOP->getShipData().at(shipDefine->sid).nation_limit;
// 			if (shipDefine->old_ship != 1 && nation_limit > 0 && nation_limit != SINGLE_HERO->m_iNation)
// 			{
// 				if (!m_vFleetShip.empty() && m_vCatchShip.size() < m_pFleetAndDockResult->cantotalcatchshipnum)
// 				{
// 					m_nConfirmIndex = CONFIRM_INDEX_SHIP_TO_CATCH;
// 					InformView::getInstance()->openInformView(m_parent);
//					InformView::getInstance()->openConfirmYesOrNO("TIP_DOCK_CATCH_TITLE", "TIP_DOCK_CATCH_CONTENT");
// 				}
// 				else
// 				{
// 					InformView::getInstance()->openInformView(m_parent);
// 					InformView::getInstance()->openConfirmYes("TIP_DOCK_NATION_LIMIT");
// 				}
// 				return;
// 			}
			if (SINGLE_SHOP->getShipData().at(shipDefine->sid).require_level > SINGLE_HERO->m_iLevel)
			{
				if (!m_vFleetShip.empty() && m_vCatchShip.size() < m_pFleetAndDockResult->cantotalcatchshipnum)
				{
					m_nConfirmIndex = CONFIRM_INDEX_SHIP_TO_CATCH;
					UIInform::getInstance()->openInformView(m_parent);
					UIInform::getInstance()->openConfirmYesOrNO("TIP_DOCK_CATCH_TITLE", "TIP_DOCK_CATCH_CONTENT");
				}
				else
				{
					UIInform::getInstance()->openInformView(m_parent);
					UIInform::getInstance()->openConfirmYes("TIP_DOCK_HERO_LEVEL_NOT");
				}
				return;
			}

			auto parent = dynamic_cast<UIShipyard*>(this->getParent());
			auto shipInfo = SINGLE_SHOP->getShipData().at(shipDefine->sid);
			int n_PcurLevel = parent->getShipOrEquipmentProficiencyLevel(shipInfo.proficiency_need);
			int n_PneedLevel = parent->getNeedProficiencyLevel(shipDefine->sid);

			if (n_PcurLevel < n_PneedLevel)
			{
				if (!m_vFleetShip.empty() && m_vCatchShip.size() < m_pFleetAndDockResult->cantotalcatchshipnum)
				{
					m_nConfirmIndex = CONFIRM_INDEX_SHIP_TO_CATCH;
					UIInform::getInstance()->openInformView(m_parent);
					UIInform::getInstance()->openConfirmYesOrNO("TIP_DOCK_CATCH_TITLE", "TIP_DOCK_CATCH_CONTENT");
				}
				else
				{
					UIInform::getInstance()->openInformView(m_parent);
					UIInform::getInstance()->openConfirmYes("TIP_EUQIP_PROFICIENCY_NOT_ENOUGH");
				}
				return;
			}

			if (m_vCaptainList.size() > 4)
			{
				if (m_vCatchShip.size() < m_pFleetAndDockResult->cantotalcatchshipnum)
				{
					m_nConfirmIndex = CONFIRM_INDEX_SHIP_TO_CATCH;
					UIInform::getInstance()->openInformView(m_parent);
					UIInform::getInstance()->openConfirmYesOrNO("TIP_DOCK_CATCH_TITLE", "TIP_DOCK_CATCH_CONTENT");
				}
				else
				{
					UIInform::getInstance()->openInformView(m_parent);
					UIInform::getInstance()->openConfirmYes("TIP_DOCK_FLEET_FULL");
				}
			}
			else if (n_PcurLevel >= n_PneedLevel)
			{
				dynamic_cast<UIShipyard*>(m_parent)->setFleetFinishData();
				addShipToFleet(m_pPressButton->getTag() - START_INDEX);
				updateFee();
				auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
				auto b_change = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_change"));
				b_change->loadTextureNormal("cocosstudio/login_ui/shipyard_720/change.png");
				/*
				auto b_finish = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_finish"));
				b_finish->setBright(true);
				b_finish->setTouchEnabled(true);
				*/
				dynamic_cast<UIShipyard*>(m_parent)->setFleetFinishData();
			}
		}
		return;
	}
	//费用的解释说明
	if (isButton(button_doubt))
	{
		UICommon::getInstance()->openCommonView(m_parent);
		UICommon::getInstance()->flushInfoBtnView("TIP_DOCK_SHIP_YARD_MARK_TITLE","TIP_DOCK_SHIP_YARD_MARK_CONTENT");
		return;
	}
	//船长技能详情
	if (isButton(button_equip_bg_))
	{
		auto t_skill_lv= dynamic_cast<Text*>(Helper::seekWidgetByName(target,"text_item_skill_lv"));
		UICommon::getInstance()->openCommonView(m_parent);
		SKILL_DEFINE skillDefine;
		skillDefine.id = target->getTag() - START_INDEX;
		skillDefine.lv = atoi(t_skill_lv->getString().c_str());
		skillDefine.skill_type = target->getParent()->getTag();
		skillDefine.icon_id = t_skill_lv->getTag();
		UICommon::getInstance()->flushSkillView(skillDefine);
		return;
	}
	//小伙伴特殊技能
	if (isButton(button_sp_equip_bg_))
	{
		auto image_lock = target->getChildByName<ImageView*>("image_lock");
		UICommon::getInstance()->openCommonView(m_parent);
		SKILL_DEFINE skillDefine;
		skillDefine.id = target->getTag() - START_INDEX;
		skillDefine.lv = 0;
		skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
		skillDefine.icon_id = image_lock->getTag();
		UICommon::getInstance()->flushSkillView(skillDefine);
	}
	//船只详情
	if (isButton(image_ship_bg))
	{
		UICommon::getInstance()->openCommonView(m_parent);
		int index = target->getTag();
		UICommon::getInstance()->flushShipDetail(getShipDefine(index),getShipDefine(index)->sid,true);
		return;
	}


}

void UIShipyardFleet::updateFleetAndDockView(GetFleetAndDockShipsResult* result)
{
	m_vFleetShip.clear();
	m_vCatchShip.clear();
	m_vDockShip.clear();
	m_vAllShips.clear();
	m_vUnusedCaptain.clear();
	m_vCaptainList.clear();
	m_vIid.clear();

	m_pPressButton = nullptr;
	m_nTempIndex = 0;
	m_nGlobalIndex = 0;

	m_pFleetAndDockResult = result;
	auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	m_pFleetList = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_fleet"));
	m_pDockList = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_dock"));

	auto t_dur = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_durable"));
	t_dur->setTouchEnabled(true);
	t_dur->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func, this));
	t_dur->setTag(IMAGE_ICON_DURABLE + IMAGE_INDEX2);
	auto t_sai = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_sailor"));
	t_sai->setTouchEnabled(true);
	t_sai->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func, this));
	t_sai->setTag(IMAGE_ICON_SAILOR + IMAGE_INDEX2);
	auto t_weight = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_weight"));
	t_weight->setTouchEnabled(true);
	t_weight->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func, this));
	t_weight->setTag(IMAGE_ICON_CARGO + IMAGE_INDEX2);
	auto t_sup = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_supply"));
	t_sup->setTouchEnabled(true);
	t_sup->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func, this));
	t_sup->setTag(IMAGE_ICON_SUPPLY + IMAGE_INDEX2);
	auto t_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_atk"));
	t_atk->setTouchEnabled(true);
	t_atk->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func, this));
	t_atk->setTag(IMAGE_ICON_ATTACKPOWER + IMAGE_INDEX2);
	auto t_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_def"));
	t_def->setTouchEnabled(true);
	t_def->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func, this));
	t_def->setTag(IMAGE_ICON_DEFENSEPOWER + IMAGE_INDEX2);
	auto t_spe = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_speed"));
	t_spe->setTouchEnabled(true);
	t_spe->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func, this));
	t_spe->setTag(IMAGE_ICON_SPEED + IMAGE_INDEX2);
	auto t_ste = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_steering"));
	t_ste->setTouchEnabled(true);
	t_ste->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func, this));
	t_ste->setTag(IMAGE_ICON_STEERING + IMAGE_INDEX2);
	auto p_no1 = view->getChildByName<Widget*>("panel_no1");
	auto p_no2 = view->getChildByName<Widget*>("panel_no2");
	/*
	auto b_finish = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_finish"));
	b_finish->setBright(false);
	b_finish->setTouchEnabled(false);
	*/
	auto w_fleet = view->getChildByName<Widget*>("button_ship_2");
	auto w_dock = view->getChildByName<Widget*>("button_ship_1");
	w_fleet->setVisible(false);
	w_dock->setVisible(false);
	/*
	auto w_fleetItemOne = view->getChildByName<Widget*>("button_ship_2")->clone();
	auto w_dockItemOne = view->getChildByName<Widget*>("button_ship_1")->clone();*/
	/*
	w_fleetItemOne->setBright(true);
	w_dockItemOne->setBright(true);
	w_fleetItemOne->setVisible(true);
	w_dockItemOne->setVisible(true);
*/
	m_pFleetList->removeAllItems();
	m_pDockList->removeAllItems();

	if (result->n_fleetships < 1)
	{
		//w_fleetItemOne->setVisible(false);
		//m_pFleetList->pushBackCustomItem(w_fleetItemOne);
		p_no1->setVisible(true);
	}else
	{
		p_no1->setVisible(false);
		for(int i = 0; i < result->n_fleetships; i++)
		{
			ShipDefine* fleetShip = result->fleetships[i];
			auto w_clone = w_fleet->clone();
			m_vCaptainList.push_back(fleetShip->captain_id);
			m_vIid.push_back(fleetShip->id);
			m_vAllShips.push_back(fleetShip);
			w_clone->setVisible(true);
			updateFleetShipInfo(w_clone,fleetShip,i);
		}

		for (int i = 0; i < result->n_catchships; i++)
		{
			ShipDefine* catchShip = result->catchships[i];
			auto w_clone = w_fleet->clone();
			m_vAllShips.push_back(catchShip);
			m_vCatchShip.push_back(catchShip->id);
			w_clone->setVisible(true);
			updateFleetCatchShipInfo(w_clone, catchShip, result->n_fleetships + i);
		}
	}
	
	if (result->n_dockships < 1)
	{
		/*m_pDockList->pushBackCustomItem(w_dockItemOne);
		w_dockItemOne->setVisible(false);*/
		p_no2->setVisible(true);
	}else
	{
		p_no2->setVisible(false);
		for (auto i = 0; i < result->n_dockships; i++)
		{
			auto w_clone = w_dock->clone();
			ShipDefine* dockShip = result->dockships[i];
			m_vAllShips.push_back(dockShip);
			w_clone->setVisible(true);
			updateDockShipInfo(w_clone,dockShip,i);
		}
	}
	updateFee();
}
void UIShipyardFleet::updateDockShipInfo(Widget* item,ShipDefine* shipDefine,int tag)
{
	
	shipDefine->crew_num = 0;
	shipDefine->current_cargo_size = 0;
	shipDefine->supply = 0;
	
	m_vDockShip.push_back(shipDefine->id);

	auto t_require = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_require_lv"));
	auto label_require=dynamic_cast<Text*>(item->getChildByName<Widget*>("label_require"));
	t_require->setString(String::createWithFormat("Lv. %d",shipDefine->requiredlv)->_string);
	if (shipDefine->requiredlv > SINGLE_HERO->m_iLevel)
	{
		label_require->setTextColor(Color4B(198,2,5,255));
		t_require->setTextColor(Color4B(198,2,5,255));
	}else
	{
		label_require->setTextColor(Color4B(40,25,13,255));
		t_require->setTextColor(Color4B(40,25,13,255));
	}

	auto i_icon_bg = dynamic_cast<ImageView*>(item->getChildByName<Widget*>("image_ship_bg"));
	i_icon_bg->setTag(shipDefine->id);
	i_icon_bg->setTouchEnabled(true);
	i_icon_bg->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func,this));
	auto i_icon = dynamic_cast<ImageView*>(i_icon_bg->getChildByName<Widget*>("image_ship"));
	auto t_name = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_ship_name"));

	auto i_durable = item->getChildByName<Widget*>("image_durable");
	auto l_durable = dynamic_cast<Widget*>(i_durable->getChildByName("panel_ship_durable_num_1"));
	auto t_durableNum1 = dynamic_cast<Text*>(l_durable->getChildByName("label_ship_durable_num_1"));
	auto t_durableNum2 = dynamic_cast<Text*>(l_durable->getChildByName("label_ship_durable_num_2"));

	auto t_sailorNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_ship_sailor_num_1"));
	auto t_sailorNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_sailor_num_2"));

	auto t_weightNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_ship_weight_num_1"));
	auto t_weightNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_weight_num_2"));

	auto t_supplyNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_ship_supply_num_1"));
	auto t_supplyNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_supply_num_2"));

	auto t_atkNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_atk_num"));
	auto t_defNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_defnum"));
	auto t_speedNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_speed_num"));
	auto t_steeringNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_steering_num"));
    t_durableNum1->setContentSize(Size(50, 10));
     t_durableNum2->setContentSize(Size(50, 10));
    t_sailorNum->setContentSize(Size(50, 10));
   t_sailorNum_1->setContentSize(Size(50, 10));
       t_weightNum->setContentSize(Size(50, 10));
      t_weightNum_1->setContentSize(Size(50, 10));
      t_supplyNum->setContentSize(Size(50, 10));
      t_supplyNum_1->setContentSize(Size(50, 10));
    setTextFontSize(t_durableNum1);
    setTextFontSize(t_durableNum2);
    setTextFontSize(t_sailorNum);
    setTextFontSize(t_sailorNum_1);
    setTextFontSize(t_weightNum);
    setTextFontSize(t_weightNum_1);
    setTextFontSize(t_supplyNum);
    setTextFontSize(t_supplyNum_1);
    
    
    
	auto w_changeName = item->getChildByName<Widget*>("button_changename");
	item->setName("button_dock_ship");
	item->setVisible(true);
	item->setTag(tag + START_INDEX);
	w_changeName->setTag(tag + START_INDEX);
	std::string filePath = getShipIconPath(shipDefine->sid);
	i_icon->loadTexture(filePath);
	if (shipDefine->user_define_name == nullptr || std::strcmp(shipDefine->user_define_name,"") == 0)
	{
		std::string name = getShipName(shipDefine->sid);
		t_name->setString(name);
	}else
	{
		t_name->setString(String::createWithFormat("%s",shipDefine->user_define_name)->_string);
	}

	if(shipDefine->current_hp_max < shipDefine->hp_max)
	{   //not normal
		t_durableNum1->setString(StringUtils::format("%d /",shipDefine->hp));
		t_durableNum2->setString(StringUtils::format(" %d",shipDefine->current_hp_max));
		t_durableNum2->setTextColor(Color4B(198,2,5,255));
//chengyuan++
		t_durableNum1->ignoreContentAdaptWithSize(true);
		t_durableNum2->ignoreContentAdaptWithSize(true);
		t_durableNum1->setPositionX(t_durableNum2->getPositionX() - t_durableNum2->getContentSize().width);
//
	}else
	{
		t_durableNum1->setString(StringUtils::format("%d /",shipDefine->hp));
		t_durableNum2->setString(StringUtils::format(" %d",shipDefine->hp_max));
		t_durableNum2->setTextColor(Color4B(40,25,13,255));
//chengyuan++
		t_durableNum1->ignoreContentAdaptWithSize(true);
		t_durableNum2->ignoreContentAdaptWithSize(true);
		t_durableNum1->setPositionX(t_durableNum2->getPositionX() - t_durableNum2->getContentSize().width);
//
	}

	t_sailorNum_1->setString(StringUtils::format("%d /", shipDefine->crew_num));
	t_sailorNum->setString(StringUtils::format(" %d", shipDefine->max_crew_num));
	t_sailorNum_1->setPositionX(t_sailorNum->getPositionX() - t_sailorNum->getContentSize().width);

	t_weightNum_1->setString(StringUtils::format("%d /", shipDefine->current_cargo_size / 100));
	t_weightNum->setString(StringUtils::format(" %d", shipDefine->cargo_size / 100));
	t_weightNum_1->setPositionX(t_weightNum->getPositionX() - t_weightNum->getContentSize().width);

	t_supplyNum_1->setString(StringUtils::format("%d /", 0));
	t_supplyNum->setString(StringUtils::format(" %d", shipDefine->max_supply));
	t_supplyNum_1->setPositionX(t_supplyNum->getPositionX() - t_supplyNum->getContentSize().width);

	t_atkNum->setString(StringUtils::format("%d",shipDefine->attack));
	t_defNum->setString(StringUtils::format("%d",shipDefine->defence));
	t_speedNum->setString(StringUtils::format("%d",shipDefine->speed));
	t_steeringNum->setString(StringUtils::format("%d",shipDefine->steer_speed));

	item->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
	w_changeName->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
	m_pDockList->pushBackCustomItem(item);

	auto i_intensify = Helper::seekWidgetByName(i_icon_bg, "goods_intensify");
	if (i_intensify)
	{
		i_intensify->removeFromParentAndCleanup(true);
	}
	if (shipDefine->optionid1 > 0 || shipDefine->optionid2 > 0)
	{
		addStrengtheningIcon(i_icon_bg);
	}

	auto i_bgImage = i_icon_bg->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(i_icon_bg, shipDefine->sid, ITEM_TYPE_SHIP);
	setBgImageColorFormIdAndType(i_bgImage, shipDefine->sid, ITEM_TYPE_SHIP);
	setTextColorFormIdAndType(t_name, shipDefine->sid, ITEM_TYPE_SHIP);

	if (shipDefine && shipDefine->optionid1 != 0)
	{

		std::string attrSrc("0,0,0,0,0,0");
		if (shipDefine->optional_value)
		{
			attrSrc = shipDefine->optional_value;
		}
		attrSrc = attrSrc.substr(0, attrSrc.find('-'));
		attrSrc.push_back(',');
		int index_1 = -1;
		int value_1 = -1;
		int index_2 = -1;
		int value_2 = -1;
		UICommon::getInstance()->getNotZeroFromString(attrSrc, value_1, index_1, value_2, index_2);
		if (index_1 != -1)
		{
			confirmTextColor(item, index_1 + 2);
		}
		if (index_2 != -1)
		{
			confirmTextColor(item, index_1 + 2);
		}
	}

	if (shipDefine && shipDefine->optionid2 != 0)
	{

		std::string attrSrc("0,0,0,0,0,0");
		if (shipDefine->optional_value)
		{
			attrSrc = shipDefine->optional_value;
		}

		attrSrc = attrSrc.substr(attrSrc.find('-') + 1);
		attrSrc.push_back(',');

		int index_1 = -1;
		int value_1 = -1;
		int index_2 = -1;
		int value_2 = -1;
		UICommon::getInstance()->getNotZeroFromString(attrSrc, value_1, index_1, value_2, index_2);

		if (index_1 != -1)
		{
			confirmTextColor(item, index_1 + 2);

		}
		if (index_2 != -1)
		{
			confirmTextColor(item, index_1 + 2);
		}

	}

}

void UIShipyardFleet::updateFleetShipInfo(Widget* item,ShipDefine* shipDefine,int tag)
{
	auto b_image = dynamic_cast<Button*>(item->getChildByName("button_head_player"));
	auto i_image = dynamic_cast<ImageView*>(b_image->getChildByName("image_head"));
	auto t_playername = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_player_name"));
	auto t_level = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_lv"));
	auto t_playerNO = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_player_name_no"));
	
	auto w_rank = item->getChildByName<Widget*>("button_ranking");
	auto t_order = dynamic_cast<ImageView*>(w_rank->getChildByName<Widget*>("image_num"));

	auto i_icon_bg = item->getChildByName<Widget*>("image_ship_bg");

	i_icon_bg->setTag(shipDefine->id);
	i_icon_bg->setTouchEnabled(true);
	i_icon_bg->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func,this));
	auto i_icon = dynamic_cast<ImageView*>(i_icon_bg->getChildByName("image_ship"));
	auto t_shipName = dynamic_cast<Text*>(item->getChildByName("label_ship_name"));
	auto i_durable = item->getChildByName<Widget*>("image_durable");
	auto l_durable = dynamic_cast<Widget*>(i_durable->getChildByName("panel_ship_durable_num_1"));

	auto t_durableNum1 = dynamic_cast<Text*>(l_durable->getChildByName("label_ship_durable_num_1"));
	auto t_durableNum2 = dynamic_cast<Text*>(l_durable->getChildByName("label_ship_durable_num_2"));


	auto t_sailorNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_ship_sailor_num_1"));
	auto t_weightNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_ship_weight_num_1"));
	auto t_supplyNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_ship_supply_num_1"));

	auto t_sailorNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_sailor_num_2"));
	auto t_weightNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_weight_num_2"));
	auto t_supplyNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_supply_num_2"));

	auto t_atkNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_atk_num"));
	auto t_defNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_defnum"));
	auto t_speedNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_speed_num"));
	auto t_steeringNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item,"label_steering_num"));
    t_durableNum1->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_durableNum2->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_sailorNum->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_sailorNum_1->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_weightNum->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_weightNum_1->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_supplyNum->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_supplyNum_1->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_durableNum1->setContentSize(Size(50, 10));
    t_durableNum2->setContentSize(Size(50, 10));
    t_sailorNum->setContentSize(Size(50, 10));
    t_sailorNum_1->setContentSize(Size(50, 10));
    t_weightNum->setContentSize(Size(50, 10));
    t_weightNum_1->setContentSize(Size(50, 10));
    t_supplyNum->setContentSize(Size(50, 10));
    t_supplyNum_1->setContentSize(Size(50, 10));
    setTextFontSize(t_durableNum1);
    setTextFontSize(t_durableNum2);
    setTextFontSize(t_sailorNum);
    setTextFontSize(t_sailorNum_1);
    setTextFontSize(t_weightNum);
    setTextFontSize(t_weightNum_1);
    setTextFontSize(t_supplyNum);
    setTextFontSize(t_supplyNum_1);
	m_vFleetShip.push_back(shipDefine->id);
	w_rank->setTag(tag + START_INDEX);
	b_image->setTag(tag + START_INDEX);
	item->setName("button_fleet_ship");
	item->setTag(tag + START_INDEX);

	if (tag == 0)
	{
		b_image->setBright(true);
		i_image->setVisible(true);
		i_image->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));
		t_level->setString(String::createWithFormat("Lv: %d",SINGLE_HERO->m_iLevel)->_string);
		t_playername->setString(SINGLE_HERO->m_sName);
		t_level->setVisible(true);
		t_playername->setVisible(true);
		t_playerNO->setVisible(false);
	}
	else if (m_vCaptainList[tag] == nullptr || m_vCaptainList[tag]->captainid == 0)
	{
		b_image->setBright(false);
		i_image->setVisible(false);
		t_level->setVisible(false);
		t_playername->setVisible(false);
		t_playerNO->setVisible(true);	
	}else
	{
		CaptainDefine* captainDefine = getCaptainDefine(m_vCaptainList[tag]->captainid, m_vCaptainList[tag]->iscaptain);
		if (captainDefine)
		{
			std::string captainPath = getCompanionIconPath(captainDefine->protoid, captainDefine->iscaptain);
			b_image->setBright(true);
			i_image->setVisible(true);
			i_image->loadTexture(captainPath);
			t_level->setString(String::createWithFormat("Lv:%d",captainDefine->level)->_string);
			t_playername->setString(getCompanionName(captainDefine->protoid, captainDefine->iscaptain));
			t_level->setVisible(true);
			t_playername->setVisible(true);
			t_playerNO->setVisible(false);
		}
	}
	if (shipDefine->user_define_name == nullptr || std::strcmp(shipDefine->user_define_name,"") == 0)
	{
		std::string captainName = getShipName(shipDefine->sid);
		t_shipName->setString(captainName);
	}else
	{
		t_shipName->setString(String::createWithFormat("%s",shipDefine->user_define_name)->_string);
	}

	std::string filePath = getShipIconPath(shipDefine->sid);
	i_icon->loadTexture(filePath);
	
	if(shipDefine->current_hp_max < shipDefine->hp_max)
	{   //not normal
		t_durableNum1->setString(StringUtils::format("%d /",shipDefine->hp));
		t_durableNum2->setString(StringUtils::format(" %d",shipDefine->current_hp_max));
		t_durableNum2->setTextColor(Color4B(198,2,5,255));
//chengyuan++
		t_durableNum1->ignoreContentAdaptWithSize(true);
		t_durableNum2->ignoreContentAdaptWithSize(true);
		t_durableNum1->setPositionX(t_durableNum2->getPositionX() - t_durableNum2->getContentSize().width);
//
	}else
	{
		t_durableNum1->setString(StringUtils::format("%d /",shipDefine->hp));
		t_durableNum2->setString(StringUtils::format(" %d",shipDefine->hp_max));
		t_durableNum2->setTextColor(Color4B(40,25,13,255));
//chengyuan++
		t_durableNum1->ignoreContentAdaptWithSize(true);
		t_durableNum2->ignoreContentAdaptWithSize(true);
		t_durableNum1->setPositionX(t_durableNum2->getPositionX() - t_durableNum2->getContentSize().width);
//
	}
	
	t_sailorNum_1->setString(StringUtils::format("%d /",shipDefine->crew_num));
	t_sailorNum->setString(StringUtils::format(" %d",shipDefine->max_crew_num));
	t_sailorNum_1->setPositionX(t_sailorNum->getPositionX() - t_sailorNum->getContentSize().width);

	t_weightNum_1->setString(StringUtils::format("%d /",shipDefine->current_cargo_size/100));
	t_weightNum->setString(StringUtils::format(" %d", shipDefine->cargo_size / 100));
	t_weightNum_1->setPositionX(t_weightNum->getPositionX() - t_weightNum->getContentSize().width );

	t_supplyNum_1->setString(StringUtils::format("%d /",shipDefine->supply));
	t_supplyNum->setString(StringUtils::format(" %d",shipDefine->max_supply));
	t_supplyNum_1->setPositionX(t_supplyNum->getPositionX() - t_supplyNum->getContentSize().width);

	t_atkNum->setString(StringUtils::format("%d",shipDefine->attack));
	t_defNum->setString(StringUtils::format("%d",shipDefine->defence));
	t_speedNum->setString(StringUtils::format("%d",shipDefine->speed));
	t_steeringNum->setString(StringUtils::format("%d",shipDefine->steer_speed));

	t_order->loadTexture(getPositionIconPath(tag + 1));

	item->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
	b_image->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
	w_rank->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
	m_pFleetList->pushBackCustomItem(item);

	auto i_intensify = Helper::seekWidgetByName(i_icon_bg, "goods_intensify");
	if (i_intensify)
	{
		i_intensify->removeFromParentAndCleanup(true);
	}
	if (shipDefine->optionid1 > 0 || shipDefine->optionid2 > 0)
	{
		addStrengtheningIcon(i_icon_bg);
	}

	auto i_bgImage = i_icon_bg->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(i_icon_bg, shipDefine->sid, ITEM_TYPE_SHIP);
	setBgImageColorFormIdAndType(i_bgImage, shipDefine->sid, ITEM_TYPE_SHIP);
	setTextColorFormIdAndType(t_shipName, shipDefine->sid, ITEM_TYPE_SHIP);

	if (shipDefine && shipDefine->optionid1 != 0)
	{
	
		std::string attrSrc("0,0,0,0,0,0");
		if (shipDefine->optional_value)
		{
			attrSrc = shipDefine->optional_value;
		}
		attrSrc = attrSrc.substr(0, attrSrc.find('-'));
		attrSrc.push_back(',');
		int index_1 = -1;
		int value_1 = -1;
		int index_2 = -1;
		int value_2 = -1;
		UICommon::getInstance()->getNotZeroFromString(attrSrc, value_1, index_1, value_2, index_2);
		if (index_1 != -1)
		{
			confirmTextColor(item, index_1 + 2);
		}
		if (index_2 != -1)
		{
			confirmTextColor(item, index_1 + 2);
		}
	}

	if (shipDefine && shipDefine->optionid2 != 0)
	{
	
		std::string attrSrc("0,0,0,0,0,0");
		if (shipDefine->optional_value)
		{
			attrSrc = shipDefine->optional_value;
		}

		attrSrc = attrSrc.substr(attrSrc.find('-') + 1);
		attrSrc.push_back(',');

		int index_1 = -1;
		int value_1 = -1;
		int index_2 = -1;
		int value_2 = -1;
		UICommon::getInstance()->getNotZeroFromString(attrSrc, value_1, index_1, value_2, index_2);

		if (index_1 != -1)
		{
			confirmTextColor(item, index_1 + 2);

		}
		if (index_2 != -1)
		{
			confirmTextColor(item, index_1 + 2);
		}
	}
}

void UIShipyardFleet::updateFleetCatchShipInfo(Widget* item, ShipDefine* shipDefine, int tag)
{
	auto b_image = dynamic_cast<Button*>(item->getChildByName("button_head_player"));
	auto t_playername = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_player_name"));
	auto t_level = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_lv"));
	auto w_rank = item->getChildByName<Widget*>("button_ranking");
	auto t_captured = item->getChildByName<Widget*>("label_captured");
	
	b_image->setVisible(false);
	t_playername->setVisible(false);
	t_level->setVisible(false);
	w_rank->setVisible(false);
	t_captured->setVisible(true);
	auto i_icon_bg = item->getChildByName<Widget*>("image_ship_bg");

	i_icon_bg->setTag(shipDefine->id);
	i_icon_bg->setTouchEnabled(true);
	i_icon_bg->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleet::menuCall_func,this));
	auto i_icon = dynamic_cast<ImageView*>(i_icon_bg->getChildByName("image_ship"));
	auto t_shipName = dynamic_cast<Text*>(item->getChildByName("label_ship_name"));
	auto i_durable = item->getChildByName<Widget*>("image_durable");
	auto l_durable = dynamic_cast<Widget*>(i_durable->getChildByName("panel_ship_durable_num_1"));

	auto t_durableNum1 = dynamic_cast<Text*>(l_durable->getChildByName("label_ship_durable_num_1"));
	auto t_durableNum2 = dynamic_cast<Text*>(l_durable->getChildByName("label_ship_durable_num_2"));

	auto t_sailorNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_sailor_num_1"));
	auto t_weightNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_weight_num_1"));
	auto t_supplyNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_supply_num_1"));

	auto t_sailorNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_sailor_num_2"));
	auto t_weightNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_weight_num_2"));
	auto t_supplyNum_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_ship_supply_num_2"));

	auto t_atkNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_atk_num"));
	auto t_defNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_defnum"));
	auto t_speedNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_speed_num"));
	auto t_steeringNum = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_steering_num"));
    
    t_durableNum1->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_durableNum2->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_sailorNum->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_sailorNum_1->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_weightNum->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_weightNum_1->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_supplyNum->setFontName(CUSTOM_FONT_NAME_1.c_str());
    t_supplyNum_1->setFontName(CUSTOM_FONT_NAME_1.c_str());

	w_rank->setTag(tag + START_INDEX);
	b_image->setTag(tag + START_INDEX);
	item->setName("button_fleet_ship");
	item->setTag(tag + START_INDEX);

	i_icon->loadTexture(getShipIconPath(shipDefine->sid));

	if (shipDefine->user_define_name == nullptr || std::strcmp(shipDefine->user_define_name, "") == 0)
	{
		std::string captainName = getShipName(shipDefine->sid);
		t_shipName->setString(captainName);
	}
	else
	{
		t_shipName->setString(String::createWithFormat("%s", shipDefine->user_define_name)->_string);
	}

	if (shipDefine->current_hp_max < shipDefine->hp_max)
	{   //not normal
		t_durableNum1->setString(StringUtils::format("%d /", shipDefine->hp));
		t_durableNum2->setString(StringUtils::format(" %d", shipDefine->current_hp_max));
		t_durableNum2->setTextColor(Color4B(198, 2, 5, 255));
		t_durableNum1->ignoreContentAdaptWithSize(true);
		t_durableNum2->ignoreContentAdaptWithSize(true);
		t_durableNum1->setPositionX(t_durableNum2->getPositionX() - t_durableNum2->getContentSize().width);
	}
	else
	{
		t_durableNum1->setString(StringUtils::format("%d /", shipDefine->hp));
		t_durableNum2->setString(StringUtils::format(" %d", shipDefine->hp_max));
		t_durableNum2->setTextColor(Color4B(40, 25, 13, 255));
		t_durableNum1->ignoreContentAdaptWithSize(true);
		t_durableNum2->ignoreContentAdaptWithSize(true);
		t_durableNum1->setPositionX(t_durableNum2->getPositionX() - t_durableNum2->getContentSize().width);
        
	}

	t_sailorNum_1->setString(StringUtils::format("%d /", shipDefine->crew_num));
	t_sailorNum->setString(StringUtils::format(" %d", shipDefine->max_crew_num));
	t_sailorNum_1->setPositionX(t_sailorNum->getPositionX() - t_sailorNum->getContentSize().width);

	t_weightNum_1->setString(StringUtils::format("%d /", shipDefine->current_cargo_size / 100));
	t_weightNum->setString(StringUtils::format(" %d", shipDefine->cargo_size / 100));
	t_weightNum_1->setPositionX(t_weightNum->getPositionX() - t_weightNum->getContentSize().width);

	t_supplyNum_1->setString(StringUtils::format("%d /", shipDefine->supply));
	t_supplyNum->setString(StringUtils::format(" %d", shipDefine->max_supply));
	t_supplyNum_1->setPositionX(t_supplyNum->getPositionX() - t_supplyNum->getContentSize().width);

	t_atkNum->setString(StringUtils::format("%d", shipDefine->attack));
	t_defNum->setString(StringUtils::format("%d", shipDefine->defence));
	t_speedNum->setString(StringUtils::format("%d", shipDefine->speed));
	t_steeringNum->setString(StringUtils::format("%d", shipDefine->steer_speed));

	item->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
	b_image->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
	w_rank->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
	m_pFleetList->pushBackCustomItem(item);

	auto i_intensify = Helper::seekWidgetByName(i_icon_bg, "goods_intensify");
	if (i_intensify)
	{
		i_intensify->removeFromParentAndCleanup(true);
	}
	if (shipDefine->optionid1 > 0 || shipDefine->optionid2 > 0)
	{
		addStrengtheningIcon(i_icon_bg);
	}

	auto i_bgImage = i_icon_bg->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(i_icon_bg, shipDefine->sid, ITEM_TYPE_SHIP);
	setBgImageColorFormIdAndType(i_bgImage, shipDefine->sid, ITEM_TYPE_SHIP);
	setTextColorFormIdAndType(t_shipName, shipDefine->sid, ITEM_TYPE_SHIP);

	if (shipDefine && shipDefine->optionid1 != 0)
	{

		std::string attrSrc("0,0,0,0,0,0");
		if (shipDefine->optional_value)
		{
			attrSrc = shipDefine->optional_value;
		}
		attrSrc = attrSrc.substr(0, attrSrc.find('-'));
		attrSrc.push_back(',');
		int index_1 = -1;
		int value_1 = -1;
		int index_2 = -1;
		int value_2 = -1;
		UICommon::getInstance()->getNotZeroFromString(attrSrc, value_1, index_1, value_2, index_2);
		if (index_1 != -1)
		{
			confirmTextColor(item, index_1 + 2);
		}
		if (index_2 != -1)
		{
			confirmTextColor(item, index_1 + 2);
		}
	}

	if (shipDefine && shipDefine->optionid2 != 0)
	{

		std::string attrSrc("0,0,0,0,0,0");
		if (shipDefine->optional_value)
		{
			attrSrc = shipDefine->optional_value;
		}

		attrSrc = attrSrc.substr(attrSrc.find('-') + 1);
		attrSrc.push_back(',');

		int index_1 = -1;
		int value_1 = -1;
		int index_2 = -1;
		int value_2 = -1;
		UICommon::getInstance()->getNotZeroFromString(attrSrc, value_1, index_1, value_2, index_2);

		if (index_1 != -1)
		{
			confirmTextColor(item, index_1 + 2);

		}
		if (index_2 != -1)
		{
			confirmTextColor(item, index_1 + 2);
		}
	}
}

EquipDetailInfo* UIShipyardFleet::getEquipDetailInfo(int id) const
{
	if (id == 0)
	{
		return nullptr;
	}
	for (auto i = 0; i < m_pFleetAndDockResult->n_equipdetails;i++)
	{
		if (id == m_pFleetAndDockResult->equipdetails[i]->id)
		{
			return m_pFleetAndDockResult->equipdetails[i];
		}
	}
	return nullptr;
}

void UIShipyardFleet::pushCaptainList(CaptainDefine** caps,int n_caps)
{
	m_parent->openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CHOOSECAPTAIN_CSB]);
	auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CHOOSECAPTAIN_CSB]);
	if (view == nullptr)
	{
		return;
	}                           
	m_vUnusedCaptain.clear();
	auto lv_caps = dynamic_cast<ListView*>(view->getChildByName("listview_captain"));
	auto b_captain = dynamic_cast<Widget*>(view->getChildByName("panel_captain"));
	auto t_nocaptain1 = dynamic_cast<Text*>(view->getChildByName("label_no_captain_1"));
	auto t_nocaptain2 = dynamic_cast<Text*>(view->getChildByName("label_no_captain_2"));
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");

	if (n_caps < 1)
	{
		lv_caps->setVisible(false);
		t_nocaptain1->setVisible(true);
		t_nocaptain2->setVisible(true);
		image_pulldown->setVisible(false);
		return;
	}else
	{
		lv_caps->setVisible(true);
		t_nocaptain1->setVisible(false);
		t_nocaptain2->setVisible(false);
	}
	lv_caps->removeAllChildrenWithCleanup(true);

	int j = 0;
	for (int i = 0; i < n_caps; i++)
	{
		//当前船只有船长
		if (m_vCaptainList.at(m_nGlobalIndex) && m_vCaptainList.at(m_nGlobalIndex)->captainid)
		{	//过滤本船上的小伙伴
			if (m_vCaptainList.at(m_nGlobalIndex)->captainid == caps[i]->id && m_vCaptainList.at(m_nGlobalIndex)->iscaptain == caps[i]->iscaptain)
			{
				continue;
			}
		}

		auto cap = b_captain->clone();
		lv_caps->pushBackCustomItem(cap);
		cap->setTag(j + START_INDEX);
		j++;
		
		auto t_level = dynamic_cast<Text*>(Helper::seekWidgetByName(cap,"label_lv"));
		auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(cap,"label_name"));
		auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cap,"image_captain_head"));
		auto t_reserve = dynamic_cast<Text*>(Helper::seekWidgetByName(cap,"label_name_0"));
		auto i_employNum = dynamic_cast<ImageView*>(Helper::seekWidgetByName(cap,"image_num"));

		std::string captainName = getCompanionName(caps[i]->protoid, caps[i]->iscaptain);
		std::string captainIcon = getCompanionIconPath(caps[i]->protoid, caps[i]->iscaptain);
		t_name->setString(captainName);
		i_icon->loadTexture(captainIcon);
		t_level->setString(StringUtils::format("Lv. %d",caps[i]->level));
		cap->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
		//employ
		if (caps[i]->shipid > 0)
		{
			i_employNum->setVisible(true);
			t_reserve->setVisible(false);
			int position = 1;
			for (size_t j = 0; j < m_vCaptainList.size(); j++)
			{
				if (caps[i]->id == m_vCaptainList.at(j)->captainid && caps[i]->iscaptain == m_vCaptainList.at(j)->iscaptain)
				{
					position = j + 1;
					break;
				}
			}
			//有问题,
			if (position > 1)
			{
				i_employNum->loadTexture(getPositionIconPath(position));
			}
			else
			{
				i_employNum->setVisible(false);
				t_reserve->setVisible(true);
			}
			
		}else
		{
			i_employNum->setVisible(false);
			t_reserve->setVisible(true);
		}
		/*
		FleetCaptain *unUsed = new FleetCaptain;
		unUsed->captainid = caps[i]->id;
		unUsed->iscaptain = caps[i]->iscaptain;
		*/
		m_vUnusedCaptain.push_back(caps[i]);

		for (int j = 0; j < 3; j++)
		{
			std::string str = StringUtils::format("button_equip_bg_%d",j+1);
			auto b_skill = dynamic_cast<Button*>(Helper::seekWidgetByName(cap, str));
			if (j < caps[i]->n_skills)
			{
				auto t_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(b_skill, "text_item_skill_lv"));
				b_skill->setTag(caps[i]->skills[j]->id + START_INDEX);
				b_skill->setVisible(true);
				b_skill->ignoreContentAdaptWithSize(false);
				if (caps[i]->iscaptain)
				{
					b_skill->getParent()->setTag(SKILL_TYPE_CAPTAIN);
					b_skill->loadTextureNormal(getSkillIconPath(caps[i]->skills[j]->id, SKILL_TYPE_CAPTAIN));
				}
				else
				{
					b_skill->getParent()->setTag(SKILL_TYPE_COMPANION_NORMAL);
					b_skill->loadTextureNormal(getSkillIconPath(caps[i]->skills[j]->id, SKILL_TYPE_COMPANION_NORMAL));
				}
				b_skill->setTouchEnabled(true);
				b_skill->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
				setTextSizeAndOutline(t_skill_lv,caps[i]->skills[j]->level);
				t_skill_lv->setTag(caps[i]->protoid);
			}else
			{
				b_skill->setVisible(false);
			}
		}

		for (int j = 0; j < 3; j++)
		{
			std::string str = StringUtils::format("button_sp_equip_bg_%d", j + 1);
			auto b_skill = dynamic_cast<Button*>(Helper::seekWidgetByName(cap, str));
			if (j < caps[i]->n_specskills)
			{
				b_skill->setTag(caps[i]->specskills[j]->id + START_INDEX);
				b_skill->setVisible(true);
				b_skill->ignoreContentAdaptWithSize(false);
				b_skill->loadTextureNormal(getSkillIconPath(caps[i]->specskills[j]->id, SKILL_TYPE_PARTNER_SPECIAL));
				b_skill->setTouchEnabled(true);
				b_skill->addTouchEventListener(CC_CALLBACK_2(UIShipyard::menuCall_func, dynamic_cast<UIShipyard*>(m_parent)));
				auto image_lock = b_skill->getChildByName<ImageView*>("image_lock");
				image_lock->setTag(caps[i]->protoid);
				//解锁
				if (caps[i]->specskills[j]->level)
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
		auto p_skill = dynamic_cast<Widget*>(Helper::seekWidgetByName(cap, "panel_skill"));
		//没有特殊技能时
		if (caps[i]->n_specskills < 1)
		{
			p_skill->setPositionY(cap->getBoundingBox().size.height / 2 - p_skill->getBoundingBox().size.height/2);
		}
	}

	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(lv_caps, image_pulldown);
}

void UIShipyardFleet::switchShipOrder(int first,int second)
{
	if (first == second)
	{
		return;
	}
	auto lv_feet = m_pFleetList;
	auto w_first = lv_feet->getItem(first);
	auto w_second = lv_feet->getItem(second);


	w_first->retain();
	w_second->retain();
	lv_feet->removeItem(lv_feet->getIndex(w_first));
	lv_feet->removeItem(lv_feet->getIndex(w_second));
	auto btn_rank_1 = w_first->getChildByName<Widget*>("button_ranking");
	auto btn_rank_2 = w_second->getChildByName<Widget*>("button_ranking");

	auto t_index_1 = dynamic_cast<ImageView*>(btn_rank_1->getChildByName("image_num"));
	auto t_index_2 = dynamic_cast<ImageView*>(btn_rank_2->getChildByName("image_num"));
	auto w_head_1 = w_first->getChildByName<Widget*>("button_head_player");
	auto w_head_2 = w_second->getChildByName<Widget*>("button_head_player");
	w_head_1->setBright(false);
	w_head_2->setBright(false);

	w_head_1->setTag(START_INDEX + second);
	w_head_2->setTag(START_INDEX + first);
	btn_rank_1->setTag(START_INDEX + second);
	btn_rank_2->setTag(START_INDEX + first);
	w_first->setTag(START_INDEX + second);
	w_second->setTag(START_INDEX + first);
	t_index_1->loadTexture(getPositionIconPath(second + 1));
	t_index_2->loadTexture(getPositionIconPath(first + 1));
	{
		int shipid = m_vFleetShip[first];
		m_vFleetShip[first] = m_vFleetShip[second];
		m_vFleetShip[second] = shipid;

		if (first == 0 || second == 0)
		{
			
		}
		else
		{
			std::swap(m_vCaptainList.at(first),m_vCaptainList.at(second));
		}
		if (first < second)
		{
			lv_feet->insertCustomItem(w_second,first);
			lv_feet->insertCustomItem(w_first,second);
		}else
		{		
			lv_feet->insertCustomItem(w_first,second);
			lv_feet->insertCustomItem(w_second,first);
		}
	}
	
	if (first == 0 || second == 0)
	{
		auto myFirst = lv_feet->getItem(0);
		auto b_head = dynamic_cast<Button*>(myFirst->getChildByName<Widget*>("button_head_player"));
		auto i_head = dynamic_cast<ImageView*>(b_head->getChildByName<Widget*>("image_head"));
		auto t_level = dynamic_cast<Text*>(myFirst->getChildByName<Widget*>("label_lv"));
		auto t_name = dynamic_cast<Text*>(myFirst->getChildByName<Widget*>("label_player_name"));
		auto t_name_No = dynamic_cast<Text*>(myFirst->getChildByName<Widget*>("label_player_name_no"));

		int maxValue = std::max(first,second);
		
		auto mySecond = lv_feet->getItem(maxValue);
		auto b_head_2 = dynamic_cast<Button*>(mySecond->getChildByName<Widget*>("button_head_player"));
		auto i_head_2 = dynamic_cast<ImageView*>(b_head_2->getChildByName<Widget*>("image_head"));
		auto t_level_2 = dynamic_cast<Text*>(mySecond->getChildByName<Widget*>("label_lv"));
		auto t_name_2 = dynamic_cast<Text*>(mySecond->getChildByName<Widget*>("label_player_name"));
		auto t_name_No2 = dynamic_cast<Text*>(mySecond->getChildByName<Widget*>("label_player_name_no"));
		
		int id = m_vCaptainList[maxValue]->captainid;
		t_level_2->setString(t_level->getString());
		t_name_2->setString(t_name->getString());
		if (id)
		{
			i_head_2->loadTexture(getCompanionIconPath(getCaptainDefine(id, m_vCaptainList[maxValue]->iscaptain)->protoid, m_vCaptainList[maxValue]->iscaptain));
			t_name_No2->setVisible(false);
			t_level_2->setVisible(true);
			t_name_2->setVisible(true);
		}else
		{
			i_head_2->setVisible(false);
			t_name_No2->setVisible(true);
			t_level_2->setVisible(false);
			t_name_2->setVisible(false);
		}

		t_level->setString(String::createWithFormat("Lv:%d",SINGLE_HERO->m_iLevel)->_string);
		t_name->setString(SINGLE_HERO->m_sName);
		i_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));
		t_level->setVisible(true);
		t_name->setVisible(true);
		i_head->setVisible(true);
		t_name_No->setVisible(false);
	}
	w_first->release();
	w_second->release();
}

void UIShipyardFleet::switchCaptain()
{
	auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CHOOSECAPTAIN_CSB]);
	auto lv_captainList = dynamic_cast<ListView*>(view->getChildByName("listview_captain"));
	auto b_captain = dynamic_cast<Widget*>(view->getChildByName("panel_captain"));
	//log("m_nGlobalIndex = %d m_nTempIndex = %d m_vCaptainList.size() = %d m_vUnusedCaptain.size() = %d", m_nGlobalIndex, m_nTempIndex, m_vCaptainList.size(), m_vUnusedCaptain.size());
	
	for (size_t i = 0; i < m_vCaptainList.size(); i++)
	{
		if (m_vUnusedCaptain.at(m_nTempIndex)->id == m_vCaptainList.at(i)->captainid && m_vUnusedCaptain.at(m_nTempIndex)->iscaptain == m_vCaptainList.at(i)->iscaptain)
		{
			updateCaptainInfo(i, nullptr);
			if (getCaptainDefine(m_vCaptainList.at(i)->captainid, m_vCaptainList.at(i)->iscaptain))
			{
				getCaptainDefine(m_vCaptainList.at(i)->captainid, m_vCaptainList.at(i)->iscaptain)->shipid = 0;
			}
			if (getShipDefine(m_vFleetShip.at(i)))
			{
				if (getShipDefine(m_vFleetShip.at(i))->captain_id)
				{
					getShipDefine(m_vFleetShip.at(i))->captain_id->captainid = 0;
					getShipDefine(m_vFleetShip.at(i))->captain_id->iscaptain = 1;
				}
			}
			break;
		}
	}
	
	m_vCaptainList.at(m_nGlobalIndex)->captainid = m_vUnusedCaptain.at(m_nTempIndex)->id;
	m_vCaptainList.at(m_nGlobalIndex)->iscaptain = m_vUnusedCaptain.at(m_nTempIndex)->iscaptain;
	updateCaptainInfo(m_nGlobalIndex, getCaptainDefine(m_vCaptainList.at(m_nGlobalIndex)->captainid, m_vCaptainList.at(m_nGlobalIndex)->iscaptain));
	auto captainInfo = getShipDefine(m_vFleetShip.at(m_nGlobalIndex))->captain_id;
	if (captainInfo)
	{
		captainInfo->captainid = m_vCaptainList.at(m_nGlobalIndex)->captainid;
		captainInfo->iscaptain = m_vCaptainList.at(m_nGlobalIndex)->iscaptain;
	}
	else
	{
		/*
		captainInfo = new FleetCaptain();
		captainInfo->captainid = m_vCaptainList.at(m_nGlobalIndex)->captainid;
		captainInfo->iscaptain = m_vCaptainList.at(m_nGlobalIndex)->iscaptain;
		*/
	}
	
	if (getCaptainDefine(m_vCaptainList.at(m_nGlobalIndex)->captainid, m_vCaptainList.at(m_nGlobalIndex)->iscaptain))
	{
		getCaptainDefine(m_vCaptainList.at(m_nGlobalIndex)->captainid, m_vCaptainList.at(m_nGlobalIndex)->iscaptain)->shipid = getShipDefine(m_vFleetShip.at(m_nGlobalIndex))->id;
	}

	m_pPressButton = nullptr;
	m_nTempIndex = 0;
}

void UIShipyardFleet::updateCaptainInfo(int index,CaptainDefine* capInfo)
{
	auto lv_feet = m_pFleetList;
	auto myFirst = lv_feet->getItem(index);
	auto b_head = dynamic_cast<Button*>(Helper::seekWidgetByName(myFirst,"button_head_player"));
	auto i_image = dynamic_cast<ImageView*>(Helper::seekWidgetByName(myFirst,"image_head"));
	auto t_level = dynamic_cast<Text*>(myFirst->getChildByName<Widget*>("label_lv"));
	auto t_name = dynamic_cast<Text*>(myFirst->getChildByName<Widget*>("label_player_name"));
	auto t_palyerNO = dynamic_cast<Text*>(myFirst->getChildByName<Widget*>("label_player_name_no"));
	if (capInfo && capInfo->protoid)
	{
		std::string captainName = getCompanionName(capInfo->protoid, capInfo->iscaptain);
		std::string captainPath = getCompanionIconPath(capInfo->protoid, capInfo->iscaptain);

		b_head->setBright(true);
		i_image->loadTexture(captainPath);
		i_image->setVisible(true);
		t_palyerNO->setVisible(false);
		t_level->setString(String::createWithFormat("Lv:%d", capInfo->level)->_string);
		t_level->setVisible(true);
		t_name->setString(captainName);
		t_name->setVisible(true);
	}
	else
	{
		b_head->setBright(false);
		i_image->setVisible(false);
		t_level->setVisible(false);
		t_name->setVisible(false);
		t_palyerNO->setVisible(true);
	}
}

CaptainDefine* UIShipyardFleet::getCaptainDefine(int id, int isCaptain) const
{
	for (int i = 0; i < m_pFleetAndDockResult->n_captains; ++i)
	{
		if (id == m_pFleetAndDockResult->captains[i]->id && isCaptain == m_pFleetAndDockResult->captains[i]->iscaptain)
		{
			return m_pFleetAndDockResult->captains[i];
		}
	}
	return nullptr;
}

void UIShipyardFleet::addShipToDock(int index)
{
	ShipDefine *shipDefine = nullptr;
	if (index >= m_vFleetShip.size())
	{
		int c_i = index - m_vFleetShip.size();
		shipDefine = getShipDefine(m_vCatchShip.at(c_i));
		if (shipDefine == nullptr)
		{
			log("catch list not index...", m_parent);
			return;
		}
		m_pFleetList->removeItem(index);
		m_vCatchShip.erase(m_vCatchShip.begin() + c_i);
	}
	else
	{
		shipDefine = getShipDefine(m_vFleetShip.at(index));

		if (shipDefine == nullptr)
		{
			log("fleet list not index...", m_parent);
			return;
		}
		if (index == 0)
		{
			if (m_vCaptainList.size() > 1 && m_vCaptainList.at(1)->captainid != 0)
			{
				getShipDefine(getCaptainDefine(m_vCaptainList.at(1)->captainid, m_vCaptainList.at(1)->iscaptain)->shipid)->captain_id = 0;
				getShipDefine(getCaptainDefine(m_vCaptainList.at(1)->captainid, m_vCaptainList.at(1)->iscaptain)->shipid)->position = 0;
				getCaptainDefine(m_vCaptainList.at(1)->captainid, m_vCaptainList.at(1)->iscaptain)->shipid = 0;
				m_vCaptainList.at(1)->captainid = 0;
			}

		}
		else
		{
			if (m_vCaptainList.at(index) && m_vCaptainList.at(index)->captainid != 0)
			{
				getShipDefine(getCaptainDefine(m_vCaptainList.at(index)->captainid, m_vCaptainList.at(index)->iscaptain)->shipid)->captain_id = 0;
				getShipDefine(getCaptainDefine(m_vCaptainList.at(index)->captainid, m_vCaptainList.at(index)->iscaptain)->shipid)->position = 0;
				getCaptainDefine(m_vCaptainList.at(index)->captainid, m_vCaptainList.at(index)->iscaptain)->shipid = 0;
				m_vCaptainList.at(index)->captainid = 0;
			}
		}

		if (index == 0 && m_vCaptainList.size() < 2)
		{
			auto ship = m_pFleetList->getItem(index);
			ship->setBright(true);
			ship->setVisible(false);
			m_vCaptainList.erase(m_vCaptainList.begin());
			/*
			update captain ifno
			*/
		}
		else if (index == 0)
		{
			m_pFleetList->removeItem(index);
			m_vCaptainList.erase(m_vCaptainList.begin() + 1);
		}
		else
		{
			m_pFleetList->removeItem(index);
			m_vCaptainList.erase(m_vCaptainList.begin() + index);
		}
		auto all_city_dock_ship_num = dynamic_cast<UIShipyard*>(m_parent)->getAllCityDockShipNum();
		++all_city_dock_ship_num;
		dynamic_cast<UIShipyard*>(m_parent)->saveAllCityDockShipNum(all_city_dock_ship_num);
		m_vFleetShip.erase(m_vFleetShip.begin() + index);
	}

	if (m_pDockList->getItems().size() == 0)
	{
		m_pDockList->removeItem(0);
	}
	auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	auto w_dock = view->getChildByName<Widget*>("button_ship_1");
	auto w_clone = w_dock->clone();
	w_clone->setVisible(true);
	view->getChildByName<Widget*>("panel_no2")->setVisible(false);
	updateDockShipInfo(w_clone,shipDefine,m_pDockList->getItems().size());
	flushFleetShipInfo();
	updateFee();
	m_pDockList->refreshView();
	m_pDockList->jumpToBottom();
	m_pPressButton = nullptr;
}
void UIShipyardFleet::addShipToFleet(int index)
{
	auto shipDefine = getShipDefine(m_vDockShip.at(index));
	if (shipDefine == nullptr)
	{
		log("dock list not index...");
		return;
	}

	if (m_pDockList->getItems().size() > 1)
	{
		m_pDockList->removeItem(index);
	}else
	{
		m_pDockList->removeItem(0);
	}
	
	auto all_city_dock_ship_num = dynamic_cast<UIShipyard*>(m_parent)->getAllCityDockShipNum();
	dynamic_cast<UIShipyard*>(m_parent)->saveAllCityDockShipNum(all_city_dock_ship_num);
	m_vDockShip.erase(m_vDockShip.begin() + index);
	auto w_fleetOne = m_pFleetList->getItem(0);
	auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	auto w_fleet = view->getChildByName<Widget*>("button_ship_2");
	auto w_clone = w_fleet->clone();
	w_clone->setVisible(true);
	FleetCaptain *captain = new FleetCaptain;
	if (m_pFleetList->getItems().size() == 0)
	{
		captain->captainid = SINGLE_HERO->m_iID;
		captain->iscaptain = 1;
		m_vCaptainList.push_back(captain);
		m_pFleetList->removeLastItem();
	}else
	{
		captain->captainid = 0;
		captain->iscaptain = 1;
		m_vCaptainList.push_back(captain);
	}
	//排序需要(先去除在加上)
	for (size_t i = 0; i < m_vCatchShip.size(); i++)
	{
		m_pFleetList->removeLastItem();
	}

	updateFleetShipInfo(w_clone,shipDefine,m_pFleetList->getItems().size());
	view->getChildByName<Widget*>("panel_no1")->setVisible(false);
	flushDockShipInfo();
	m_pFleetList->refreshView();
	m_pFleetList->jumpToBottom();
	m_pPressButton = nullptr;


	auto w_dockItemOne = view->getChildByName<Widget*>("button_ship_2");
	for (size_t i = 0; i < m_vCatchShip.size(); i++)
	{
		ShipDefine* catchShip = getShipDefine(m_vCatchShip.at(i));
		auto w_clone = w_dockItemOne->clone();
		w_clone->setVisible(true);
		updateFleetCatchShipInfo(w_clone, catchShip, m_vFleetShip.size() + i);
	}
}

void UIShipyardFleet::addShipToCatch(int index)
{
	auto shipDefine = getShipDefine(m_vDockShip.at(index));
	if (shipDefine == nullptr)
	{
		log("dock list not index...");
		return;
	}

	if (m_pDockList->getItems().size() > 1)
	{
		m_pDockList->removeItem(index);
	}
	else
	{
		m_pDockList->removeItem(0);
	}

	auto all_city_dock_ship_num = dynamic_cast<UIShipyard*>(m_parent)->getAllCityDockShipNum();
	dynamic_cast<UIShipyard*>(m_parent)->saveAllCityDockShipNum(all_city_dock_ship_num);
	m_vDockShip.erase(m_vDockShip.begin() + index);

	auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	auto w_dockItemOne = view->getChildByName<Widget*>("button_ship_2");
	auto w_clone = w_dockItemOne->clone();
	w_clone->setVisible(true);
	updateFleetCatchShipInfo(w_clone, shipDefine, m_vFleetShip.size() + m_vCatchShip.size());
	m_vCatchShip.push_back(shipDefine->id);

	flushDockShipInfo();
	m_pFleetList->refreshView();
	m_pFleetList->jumpToBottom();
	m_pPressButton = nullptr;
}

ShipDefine* UIShipyardFleet::getShipDefine(int id) const
{
	for (int i = 0; i < m_vAllShips.size(); i++)
	{
		if (m_vAllShips.at(i)->id == id)
		{
			return m_vAllShips.at(i);
		}
	}
	return nullptr;
}

void UIShipyardFleet::flushFleetShipInfo()
{
	auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	if (m_vFleetShip.size() < 1)
	{
		view->getChildByName<Widget*>("panel_no1")->setVisible(true);
	}else
	{
		view->getChildByName<Widget*>("panel_no1")->setVisible(false);
	}

	for (int i = 0; i < m_vFleetShip.size(); ++i)
	{
		auto item = m_pFleetList->getItem(i);
		auto captainImage = item->getChildByName<Button*>("button_head_player");
		auto i_captain = captainImage->getChildByName<ImageView*>("image_head");
		auto ranking = item->getChildByName<Widget*>("button_ranking");
		auto i_num = ranking->getChildByName<ImageView*>("image_num");

		
		item->setTag(i + START_INDEX);	
		ranking->setTag(i + START_INDEX);
		captainImage->setTag(i + START_INDEX);
		i_num->loadTexture(getPositionIconPath(i + 1));
		if (i == 0)
		{
			auto t_playername = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_player_name"));
			auto t_level = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_lv"));
			
			t_level->setString(String::createWithFormat("Lv: %d",SINGLE_HERO->m_iLevel)->_string);
			t_playername->setString(SINGLE_HERO->m_sName);
			i_captain->ignoreContentAdaptWithSize(false);
			i_captain->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));
			i_captain->setVisible(true);
			t_playername->setVisible(true);
			t_level->setVisible(true);
			item->getChildByName<Text*>("label_player_name_no")->setVisible(false);
		}
	}

	for (int i = 0; i < m_vCatchShip.size(); ++i)
	{
		auto item = m_pFleetList->getItem(i + m_vFleetShip.size());
		item->setTag(i + m_vFleetShip.size() + START_INDEX);
	}
}

void UIShipyardFleet::flushDockShipInfo()
{
	updateFee();
	auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	if (m_vDockShip.size() < 1)
	{
		view->getChildByName<Widget*>("panel_no2")->setVisible(true);
	}else
	{
		view->getChildByName<Widget*>("panel_no2")->setVisible(false);
	}

	for (int i = 0; i < m_vDockShip.size(); ++i)
	{
		auto item = m_pDockList->getItem(i);
		item->setTag(i + START_INDEX);	
	}
}

void UIShipyardFleet::sendDataToServer()
{
	int*shipIds = nullptr;
	FleetCaptain** captainIds = nullptr;
	int*deletedShipIds = nullptr;
	int *renameShipIds = nullptr;
	char**renameShipNames = nullptr;

	shipIds = new int[m_vFleetShip.size()];
	captainIds = new FleetCaptain*[m_vFleetShip.size()];
	deletedShipIds = new int[0];
	renameShipIds = new int[m_vChangeShipNameId.size()];
	renameShipNames = getAllChangeShipName();

	int n_fleet = m_vFleetShip.size();
	int n_dock = m_vDockShip.size();
	if (m_vCaptainList.size() > 0)
	{
		m_vCaptainList[0]->captainid = 0;
	}
	for (auto i = 0; i < n_fleet; ++i)
	{
		shipIds[i] = m_vFleetShip[i];
		captainIds[i] = m_vCaptainList[i];
	}
	
	for (int i = 0; i < m_vChangeShipNameId.size(); i++)
	{
		renameShipIds[i] = m_vChangeShipNameId[i];
	}

	int* catchShipId = nullptr;
	if (!m_vCatchShip.empty())
	{
		catchShipId = new int[m_vCatchShip.size()];
		for (size_t i = 0; i < m_vCatchShip.size(); i++)
		{
			catchShipId[i] = m_vCatchShip[i];
		}
	}
	
	ProtocolThread::GetInstance()->fleetFormation(shipIds,m_vFleetShip.size(),captainIds,m_vFleetShip.size(),
		deletedShipIds, 0, renameShipIds, renameShipNames, m_vChangeShipNameId.size(), m_vCatchShip.size(), catchShipId, UILoadingIndicator::create(m_parent));
	
	delete []shipIds;
	delete []captainIds;
	delete []deletedShipIds;
	delete []renameShipIds;
	if (!m_vCatchShip.empty())
	{
		delete[]catchShipId;
	}
	for (auto i = 0; i < m_vChangeShipNameId.size(); ++i)
	{
		delete []renameShipNames[i];
	}
	
	delete []renameShipNames;
	m_vChangeShipNameId.clear();

	std::vector<bool> b_vEquipBroken;

	for (size_t i = 0; i < 5; i++)
	{
		std::string st_ship_position = StringUtils::format(SHIP_POSTION_EQUIP_BROKEN, i + 1);
		auto is_broken = UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), false);
		b_vEquipBroken.push_back(is_broken);
	}
	for (size_t i = 0; i < 5; i++)
	{
		std::string st_ship_position = StringUtils::format(SHIP_POSTION_EQUIP_BROKEN, i + 1);
		if (i < m_vIid.size())
		{
			size_t j = 0;
			bool isfind = false;
			for (; j < m_vFleetShip.size(); j++)
			{
				if (m_vIid.at(i) == m_vFleetShip.at(j))
				{
					isfind = true;
					break;
				}
			}
			if (isfind)
			{
				UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), b_vEquipBroken.at(j));
			}
			else
			{
				UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), false);
			}
		}
		else
		{
			UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(st_ship_position.c_str()).c_str(), false);
		}
	}
	UserDefault::getInstance()->flush();
}

char** UIShipyardFleet::getAllChangeShipName()
{
	char** allNames = new char*[m_vChangeShipNameId.size()];
	int n_fleet = m_vFleetShip.size();
	int n_dock = m_vDockShip.size();
	int n_catch = m_vCatchShip.size();
	int n = 0;

	for (int i = 0; i < n_fleet; ++i)
	{
		for (int j = 0; j < m_vChangeShipNameId.size(); j++)
		{
			if (m_vFleetShip[i] == m_vChangeShipNameId[j])
			{
				auto item = m_pFleetList->getItem(i);
				auto tf_name = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_ship_name"));
				std::string name = tf_name->getString();
				allNames[n] = new char[name.length() + 1];
				std::strcpy(allNames[n],name.c_str());
				n++;
			}
		}	
	}
	for (int i = 0; i < n_dock; ++i)
	{
		for (int j = 0; j < m_vChangeShipNameId.size(); j++)
		{
			if (m_vDockShip[i] == m_vChangeShipNameId[j])
			{
				auto item = m_pDockList->getItem(i);
				auto tf_name = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_ship_name"));
				std::string name = tf_name->getString();
				allNames[n] = new char[name.length() + 1];
				std::strcpy(allNames[n],name.c_str());
				n++;
			}
		}
	}

	for (int i = 0; i < n_catch; ++i)
	{
		for (int j = 0; j < m_vChangeShipNameId.size(); j++)
		{
			if (m_vCatchShip[i] == m_vChangeShipNameId[j])
			{
				auto item = m_pFleetList->getItem(i + n_fleet);
				auto tf_name = dynamic_cast<Text*>(item->getChildByName<Widget*>("label_ship_name"));
				std::string name = tf_name->getString();
				allNames[n] = new char[name.length() + 1];
				std::strcpy(allNames[n], name.c_str());
				n++;
			}
		}
	}

	return allNames;
}

void UIShipyardFleet::textFiledEvent(Ref* target, bool type)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	std::string value = m_pInputText->getText();
	
	int nMaxStringNum = 0;
	auto userNameLength = my_strlen_utf8_c((char*)value.data());
	if (isChineseCharacterIn(m_pInputText->getText()))
	{
		nMaxStringNum = 15;
	}
	else
	{
		nMaxStringNum = 40;
	}
	//m_pInputText->setMaxLength(nMaxStringNum);
	//m_pInputText->setMaxLengthEnabled(true);

	
	auto minLength = 0;
	//log("value.c_str() = %s userNameLength = %d", value.c_str(), userNameLength);

	if (userNameLength > nMaxStringNum)
	{
		m_pInputText->setText((Helper::getSubStringOfUTF8String(value.c_str(), 0, nMaxStringNum)).c_str());
		//命名超长
		UITips* tip = UITips::createTip(SINGLE_SHOP->getTipsInfo()["TIP_USER_NAME_VAID_LONG"].c_str());
		this->addChild(tip, 1);
	}
	else
	{
		if (m_nInputNameIndex > START_INDEX - 1)
		{
			int index = m_nInputNameIndex - START_INDEX;
			auto w_parent = m_pDockList->getItem(index);
			auto t_inputName = dynamic_cast<Text*>(w_parent->getChildByName<Widget*>("label_ship_name"));
			t_inputName->setString(value);
			auto shipDefine = getShipDefine(m_vDockShip.at(index));
			if(shipDefine){
				if(shipDefine->user_define_name){
					free(shipDefine->user_define_name);
					shipDefine->user_define_name = (char*)malloc(strlen(value.c_str())+1);
					std::strcpy(shipDefine->user_define_name,value.c_str());
				}
			}

			int flag = 0;

			for (int i = 0; i < m_vChangeShipNameId.size(); i++)
			{
				if (m_vChangeShipNameId[i] == m_vDockShip[index])
				{
					flag = 1;
				}
			}
			if (flag == 0)
			{
				m_vChangeShipNameId.push_back(m_vDockShip.at(index));
			}
		}else
		{
			auto w_parent = m_pFleetList->getItem(m_nInputNameIndex);
			auto t_inputName = dynamic_cast<Text*>(w_parent->getChildByName<Widget*>("label_ship_name"));
			t_inputName->setString(value);
			int flag = 0;
			for (int i = 0; i <  m_vChangeShipNameId.size(); i++)
			{		
				if (m_vChangeShipNameId[i] == m_vFleetShip[m_nInputNameIndex])
				{
					flag = 1;
				}
			}
			if (flag == 0)
			{
				m_vChangeShipNameId.push_back(m_vFleetShip.at(m_nInputNameIndex));
			}
		}
	}

	auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	dynamic_cast<UIShipyard*>(m_parent)->setFleetFinishData();
}

void UIShipyardFleet::updateFee()
{
	auto view = m_parent->getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FORMATION_CSB]);
	auto t_fee = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_fee_num"));
	auto i_charNum = dynamic_cast<UIShipyard*>(m_parent)->getAllCityDockShipNum()-3;
	if (i_charNum < 1)
	{
		i_charNum = 0;
	}
	std::string s_fee = numSegment(StringUtils::format("%d",i_charNum * 100))+"/";
	s_fee += SINGLE_SHOP->getTipsInfo()["TIP_DAY"];
	t_fee->setString(s_fee);
}
void UIShipyardFleet::confirmTextColor(Widget * view, int index)
{
	auto t_durable_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_2"));
	auto t_weight_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_weight_num_1"));
	auto t_sailor_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_sailor_num_1"));
	auto t_supply_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_supply_num_1"));


	auto t_attack = view->getChildByName<Text*>("label_atk_num");
	auto t_defense = view->getChildByName<Text*>("label_defnum");
	auto t_speed = view->getChildByName<Text*>("label_speed_num");
	auto t_steer = view->getChildByName<Text*>("label_steering_num");
	switch (index)
	{
	case 1:
		t_attack->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 2:
		t_defense->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 3:
		t_durable_1->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 4:
		t_speed->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 5:
		t_steer->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 6:
		t_sailor_1->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 7:
		t_weight_1->setTextColor(Color4B(1, 120, 5, 255));
		break;
	case 10:
		t_supply_1->setTextColor(Color4B(1, 120, 5, 255));
		break;
	default:
		break;
	}
}
void UIShipyardFleet::editBoxReturn(ui::EditBox* editBox)
{
	if (editBox->getTag() == m_pInputText->getTag())
	{
		bIsEnter = true;
		this->textFiledEvent(editBox, bIsEnter);
	}
}
