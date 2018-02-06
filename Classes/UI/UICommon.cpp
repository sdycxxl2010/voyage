#include "UICommon.h"
#include "UIInform.h"

#include "UIExchange.h"
#include "TVSceneLoader.h"
#include "UIMain.h"
#include "Utils.h"
#include "UICenter.h"
#include "UIShipyard.h"
#include "UISailHUD.h"
#include "ModeLayerManger.h"
std::string manmesh_names[] =
{
	//帽子
	"man_hats_M_100_1",
	//衣服
	"man_frock_M_100_1",
	"man_pants_M_100_1",
	//项链
	"man_necklace_M_100_1",
	//鞋子
	"man_shoe_M_100_1",
};
std::string womanmesh_names[]
{
	//帽子
	"woman_hats_M_100_1",
	//衣服
	"woman_frock_M_100_1",
	"woman_pants_M_100_1",
	//项链
	"woman_necklace_M_100_1",
	//鞋子
	"woman_shoe_M_100_1",
	//手套
	"woman_gloves_M_100_1",
};
UICommon*  UICommon::m_commonView = nullptr; 
UICommon::UICommon()
{
	init();
	m_inputNum = nullptr;
	m_pCaptainAddExp = nullptr;
	m_pCaptainAddExpBattle = nullptr;
	m_brokennum = 0;
	m_brokenItems = nullptr;
	m_captainlist = nullptr;
	m_listitems.clear();
	m_bitemsaddCompleted = false;
	m_pEquipDetail = nullptr;
	m_curShipId = 0;
	m_proficiencyResult = nullptr;
	m_curView = 0;
	m_curEquipId = 0;
	m_shipInfoResult = nullptr;
	m_operaModel = nullptr;
	m_touchBegan = Vec2(0, 0);
	m_curuserInfo = nullptr;
	m_needClose = true;
	m_ShipInfoResult = nullptr;
	m_forceResult = nullptr;
	m_shipEquimentShow = false;
	m_ListClose = false;
	n_ForceId = -1;
}

UICommon::~UICommon()
{
	m_operaModel = nullptr;
}

void UICommon::openCommonView(UIBasicLayer *parent)
{
	while (m_vFilePath.size() != 0)
	{
		closeView();
	}
	UIInform::getInstance()->closeAutoInform(0);
	this->removeFromParentAndCleanup(true);
	parent->addChild(this, 20);
	if (SINGLE_HERO->m_heroIsOnsea)
	{
		this->setCameraMask(4, true);
	}
	else
	{
		this->setCameraMask(1, true);
	}
	this->setPosition(STARTPOS);
	m_pParent = parent;
}

void UICommon::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	Widget* w_btn = dynamic_cast<Widget*>(pSender);
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		buttonEventByName(w_btn,w_btn->getName());
	}
}

void UICommon::buttonYesEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		closeView();
		m_pParent->menuCall_func(pSender,Widget::TouchEventType::ENDED);
	}
}

void UICommon::imageCallEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		auto name = ((Widget*)pSender)->getName();
		if (isButton(panel_attribute))
		{
			this->unschedule(schedule_selector(UICommon::imageDetailAutoColse));
			if (getViewRoot(COMMOM_COCOS_RES[C_VIEW_ATTRIBUTE_CSB]))
			{
				closeView(COMMOM_COCOS_RES[C_VIEW_ATTRIBUTE_CSB],0);
			}
			return;
		}
		flushImageDetail((Widget*)pSender);
	}
}
//tag > IMAGE_INDEX2 right :false          tag < IMAGE_INDEX2 left :ture;
void UICommon::flushImageDetail(Widget *target)
{
	int tag = target->getTag();
	if (!getViewRoot(COMMOM_COCOS_RES[C_VIEW_ATTRIBUTE_CSB]))
	{
		openView(COMMOM_COCOS_RES[C_VIEW_ATTRIBUTE_CSB]);
	}
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ATTRIBUTE_CSB]);
	auto scene = _director->getRunningScene();
	auto cameras = scene->getCameras();
	if (cameras.size() > 2)
	{
		auto flag = (unsigned short)(cameras.at(cameras.size() - 1)->getCameraFlag());
		view->setCameraMask(flag, true);
	}
	else
	{
		view->setCameraMask(1, true);
	}
	if (view)
	{
		view->setLocalZOrder(100);
		view->setPosition(STARTPOS);
		auto p_attribute = view->getChildByName<Widget*>("panel_attribute");
		view->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));
		this->scheduleOnce(schedule_selector(UICommon::imageDetailAutoColse),2);
		auto i_attribute = p_attribute->getChildByName<ImageView*>("image_attribute");
		auto t_attribute = p_attribute->getChildByName<Text*>("label_attribute_name");
		p_attribute->setAnchorPoint(Vec2(0.5,0.5));
		p_attribute->setPositionY(target->getWorldPosition().y);
		if (tag < IMAGE_INDEX2)
		{
			tag -= IMAGE_INDEX1;
			p_attribute->setPositionX(target->getWorldPosition().x-target->getContentSize().width/2-p_attribute->getContentSize().width/2);
		}else
		{
			tag -= IMAGE_INDEX2;
			p_attribute->setPositionX(target->getWorldPosition().x+target->getContentSize().width/2+p_attribute->getContentSize().width/2);
		}
		switch (tag)
		{
		case IMAGE_ICON_ATTACK:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_ATTACK"]);
				i_attribute->setColor(Color3B(58,32,2));
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_1.png",tag));
				break;
			}
		case IMAGE_ICON_DEFENCE:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_DEFENCE"]);
				i_attribute->setColor(Color3B(58,32,2));
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_2.png",tag));
				break;
			}
		case IMAGE_ICON_DURABLE:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_DURABLE"]);
				i_attribute->setColor(Color3B(58,32,2));
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_3.png",tag));
				break;
			}
		case IMAGE_ICON_SPEED:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_SPEED_UP"]);
				i_attribute->setColor(Color3B(58,32,2));
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_4.png",tag));
				break;
			}
		case IMAGE_ICON_STEERING:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_LOADING"]);
				i_attribute->setColor(Color3B(58,32,2));
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_5.png",tag));
				break;
			}
		case IMAGE_ICON_SAILOR:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_SAILOR"]);
				i_attribute->setColor(Color3B(58,32,2));
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_6.png",tag));
				break;
			}
		case IMAGE_ICON_CARGO:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_WEIGHT"]);
				i_attribute->setColor(Color3B(58,32,2));
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_7.png",tag));
				break;
			}
		case IMAGE_ICON_BAGGO:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_P_WEIGHT"]);
				i_attribute->setColor(Color3B(58,32,2));
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_8.png",tag));
				break;
			}
		case IMAGE_ICON_WEIGHT:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_WEIGHT"]);
				i_attribute->setColor(Color3B(58,32,2));
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_9.png",tag));
				break;
			}
		case IMAGE_ICON_SUPPLY:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_SUPPLY"]);
				i_attribute->setColor(Color3B(58,32,2));
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_10.png",tag));
				break;
			}
		case IMAGE_ICON_CRAFT:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_DEVE_MAKE"]);
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_%d.png",tag));
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_SILVER:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_GENERAL_SILVER"]);
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_%d.png",tag));
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_CLOCK:
			{
				t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_GENERAL_CLOCK"]);
				i_attribute->loadTexture(StringUtils::format("res/shipAttIcon/att_%d.png",tag));
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_PORTUGAL:
			{
				t_attribute->setString(SINGLE_SHOP->getNationInfo()[1]);
				i_attribute->loadTexture("res/country_icon/flag_1.png");
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_SPAIN:
			{
				t_attribute->setString(SINGLE_SHOP->getNationInfo()[2]);
				i_attribute->loadTexture("res/country_icon/flag_2.png");
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_ENGLAND:
			{
				t_attribute->setString(SINGLE_SHOP->getNationInfo()[3]);
				i_attribute->loadTexture("res/country_icon/flag_3.png");
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_NETHERLANDS:
			{
				t_attribute->setString(SINGLE_SHOP->getNationInfo()[4]);
				i_attribute->loadTexture("res/country_icon/flag_4.png");
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_GENOVA:
			{
				t_attribute->setString(SINGLE_SHOP->getNationInfo()[5]);
				i_attribute->loadTexture("res/country_icon/flag_5.png");
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_VENICE:
			{
				t_attribute->setString(SINGLE_SHOP->getNationInfo()[6]);
				i_attribute->loadTexture("res/country_icon/flag_6.png");
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_SWEDEN:
			{
				t_attribute->setString(SINGLE_SHOP->getNationInfo()[7]);
				i_attribute->loadTexture("res/country_icon/flag_7.png");
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_SUDAN:
			{
				t_attribute->setString(SINGLE_SHOP->getNationInfo()[8]);
				i_attribute->loadTexture("res/country_icon/flag_8.png");
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_FRANCE:
			{
				t_attribute->setString(SINGLE_SHOP->getNationInfo()[9]);
				i_attribute->loadTexture("res/country_icon/flag_9.png");
				i_attribute->setColor(Color3B(255,255,255));
				break;
			}
		case IMAGE_ICON_PRESTIGE:
		{
			   t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_PRESTIGE"]);
			   i_attribute->loadTexture("res/shipAttIcon/reputation_2.png");
			   i_attribute->setColor(Color3B(255, 255, 255));
			  break;
		}
		case IMAGE_ICON_COINS:
		{
			   t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_COINS"]);
			   i_attribute->loadTexture("res/shipAttIcon/att_12.png");
			   i_attribute->setColor(Color3B(255, 255, 255));
			   break;
		}
		case IMAGE_ICON_VTICKET:
		{
			   t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKET"]);
			   i_attribute->loadTexture("res/shipAttIcon/v.png");
			   i_attribute->setColor(Color3B(255, 255, 255));
			   break;
		}
		case IMAGE_ICON_POPULATION:
		{
			 t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_POPULATION"]);
			 i_attribute->loadTexture("cocosstudio/login_ui/palace_720/city_population.png");
			// i_attribute->setColor(Color3B(58, 32, 2));
			 break;
		}
		case IMAGE_ICON_TRADE:
		{
			 t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_TRADE"]);
			// i_attribute->setColor(Color3B(58, 32, 2));
			 i_attribute->loadTexture("cocosstudio/login_ui/palace_720/city_trade.png");
			 break;
		}
		case IMAGE_ICON_MANUFACTURE:
		{
			 t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_MANUFACTURE"]);
			 //i_attribute->setColor(Color3B(58, 32, 2));
			i_attribute->loadTexture("cocosstudio/login_ui/palace_720/city_manufacture.png");
			 break;
		}
		case IMAGE_ICON_PACKAGE:
		{
			 t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_PACKAGE"]);
			 i_attribute->setColor(Color3B(58, 32, 2));
			 i_attribute->loadTexture("res/shipAttIcon/att_8.png");
			 break;
		}
		case IMAGE_ICON_HEALTHPOWER:
		{
			 t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_HEALTHPOWER"]);
			 i_attribute->setColor(Color3B(58, 32, 2));
			 i_attribute->loadTexture("cocosstudio/login_ui/common/blood.png");
			 break;
		}
		case IMAGE_ICON_ATTACKPOWER:
		{
			 t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_ATTACKPOWER"]);
			 i_attribute->setColor(Color3B(58, 32, 2));
			 i_attribute->loadTexture("cocosstudio/login_ui/common/atk.png");
			break;
		}
		case IMAGE_ICON_DEFENSEPOWER:
		{
			t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_DEFENSEPOWER"]);
			i_attribute->setColor(Color3B(58, 32, 2));
			i_attribute->loadTexture("cocosstudio/login_ui/common/def.png");
		    break;
		}
		
		case IMAGE_ICON_AMOUNTOFSAILORS:
	   {
		 t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_AMOUNTOFSAILORS"]);
		 //i_attribute->setColor(Color3B(58, 32, 2));
		 i_attribute->loadTexture("cocosstudio/login_ui/common/sailor_2.png");
		 break;
		}
		case IMAGE_ICON_MOUNTOFSUPPLIES:
		{
		t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_MOUNTOFSUPPLIES"]);
		//i_attribute->setColor(Color3B(58, 32, 2));
		i_attribute->loadTexture("cocosstudio/login_ui/common/supply.png");
	    break;
		}
		case IMAGE_ICON_SAILINGDAYS:
		{
		t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_SAILINGDAYS"]);
		//i_attribute->setColor(Color3B(58, 32, 2));
		i_attribute->loadTexture("cocosstudio/login_ui/sailing_720/clock.png");
		break;
		}
		case IMAGE_ICON_COORDINATE:
		{
		t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_SETTING_ITEM_S_COORDINATE"]);
		//i_attribute->setColor(Color3B(58, 32, 2));
		i_attribute->loadTexture("cocosstudio/login_ui/sailing_720/coordinates.png");
		break;
		}
		case IMAGE_ICON_FRIEND:
		{
        t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_ITEM_S_FRIEND"]);
		//i_attribute->setColor(Color3B(58, 32, 2));
		i_attribute->loadTexture("cocosstudio/login_ui/player_720/friend.png");
		break;
		}
		case IMAGE_ICON_ENEMY:
		{
        t_attribute->setString(SINGLE_SHOP->getTipsInfo()["TIP_ITEM_S_ENEMY"]);
		//i_attribute->setColor(Color3B(58, 32, 2));
		i_attribute->loadTexture("cocosstudio/login_ui/player_720/enemy.png");
		 break;
		}
		case IMAGE_ICON_LICENSE:
		{
		 t_attribute->setString(SINGLE_SHOP->getTipsInfo()["IMAGE_ICON_LICENSE"]);
		//i_attribute->setColor(Color3B(58, 32, 2));
		 i_attribute->loadTexture("cocosstudio/login_ui/palace_720/business_license.png");
		break;
		}
		default:
			break;
		}

		Vector<Node*> arrayRootChildren = view->getChildren();
		for (auto obj: arrayRootChildren)
		{
			Widget* child = (Widget*)(obj);
			child->setOpacity(0);
			child->runAction(FadeIn::create(0.5));
		}
	}
}

void UICommon::imageDetailAutoColse(float time)
{
	//openView(COMMOM_COCOS_RES[C_VIEW_ATTRIBUTE_CSB]);
	auto view =getViewRoot(COMMOM_COCOS_RES[C_VIEW_ATTRIBUTE_CSB]);

	if (view)
	{
		Vector<Node*> arrayRootChildren = view->getChildren();
		for (auto obj: arrayRootChildren)
		{
			Widget* child = (Widget*)(obj);
			child->runAction(FadeOut::create(1));
		}
		view->runAction(Sequence::createWithTwoActions(DelayTime::create(1),Place::create(ENDPOS)));
	}
}

void UICommon::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetRepairEquipmentNeedResult:
		{
			GetRepairEquipmentNeedResult* result = (GetRepairEquipmentNeedResult*)message;
			if (result->failed == 0)
			{
				openView(COMMOM_COCOS_RES[COMMON_CONFIRM_REPAIR]);
				auto view = getViewRoot(COMMOM_COCOS_RES[COMMON_CONFIRM_REPAIR]);
				auto p_item = view->getChildByName<Widget*>("panel_item_1");
				auto i_item_bg = p_item->getChildByName<ImageView*>("image_material_bg");
				auto i_item = i_item_bg->getChildByName<ImageView*>("image_item");
				auto t_item = p_item->getChildByName<Text*>("label_items_name");
				auto t_needNum = p_item->getChildByName<Text*>("label_num_1");
				auto t_curNum = p_item->getChildByName<Text*>("label_num_2");
				auto b_yes = view->getChildByName<Button*>("button_repair_equip_yes");
				auto b_v = view->getChildByName<Button*>("button_repair_equip_v");

				i_item->loadTexture(getItemIconPath(result->itemiid));
				t_item->setString(getItemName(result->itemiid));
				t_needNum->setString(StringUtils::format("%d ", result->itemneedcount));
				t_curNum->setString(StringUtils::format("/ %d", result->itemcurrentcount));
				t_needNum->setPositionX(t_curNum->getPositionX() - t_curNum->getBoundingBox().size.width);
				if (result->itemneedcount > result->itemcurrentcount)
				{
					t_needNum->setTextColor(TEXT_RED);
					b_yes->setVisible(false);
					b_v->setVisible(true);
					b_v->setTag(result->equipid);
					auto t_v = b_v->getChildByName<Text*>("label_num");
					t_v->setString(StringUtils::format("%lld", result->goldneed));
				}
				else
				{
					b_yes->setVisible(true);
					b_v->setVisible(false);
					b_yes->setTag(result->equipid);
				}
				setBgButtonFormIdAndType(i_item_bg, result->itemiid, ITEM_TYPE_SHIP_EQUIP);
				setTextColorFormIdAndType(t_item, result->itemiid, ITEM_TYPE_SHIP_EQUIP);
			}
			break;
		}
	case PROTO_TYPE_RepairEquipmentResult:
	{
		RepairEquipmentResult* result = (RepairEquipmentResult*)message;
		if (result->failed == 0)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_DOCK_REPAIR_ALL_SUCCESS");
			SINGLE_HERO->m_iGold = result->golds;
			bool isHideDrop = false;
			auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_EQUIP_DETAIL_CSB]);
			if (view)
			{
				Button* btn_drop = view->getChildByName<Button*>("button_drop");
				isHideDrop = !btn_drop->isVisible();
			}
			
			closeView(COMMOM_COCOS_RES[COMMON_CONFIRM_REPAIR]);
			m_pEquipDetail->durable = m_pEquipDetail->maxdurable;
			flushEquipsDetail(m_pEquipDetail, m_pEquipDetail->equipmentid, isHideDrop);
			Utils::consumeVTicket("23", 1, result->useglods);
			auto parent = this->getParent();
			if (dynamic_cast<UIExchange*>(parent) != nullptr)
			{
				if (dynamic_cast<UIExchange*>(parent)->getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]) != nullptr)
				{
					dynamic_cast<UIExchange*>(parent)->updateCoinNum(SINGLE_HERO->m_iGold, SINGLE_HERO->m_iCoin);
					ProtocolThread::GetInstance()->getPersonalItems(2, 1, UILoadingIndicator::create(m_pParent));
				}
				if (dynamic_cast<UIExchange*>(parent)->getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_CSB]) != nullptr)
				{
					ProtocolThread::GetInstance()->getItemsToSell(2, 0, UILoadingIndicator::create(this));;
				}
			}
			if (dynamic_cast<UICenter*>(parent) != nullptr)
			{
				ProtocolThread::GetInstance()->getPersonalItems(2, 0, UILoadingIndicator::create(m_pParent));
			}
			if (dynamic_cast<UIShipyard*>(parent) != nullptr)
			{
				dynamic_cast<UIShipyard*>(parent)->flushEquipInfo(m_pEquipDetail);
			}
 			unregisterCallBack();
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_GOLD_NOT");
		}
		break;
	}
	case PROTO_TYPE_GetProficiencyValuesResult:
	{
		GetProficiencyValuesResult * result = (GetProficiencyValuesResult *)message;
		if (result->failed == 0)
		{
			m_proficiencyResult = result;
			judgeProficiencyLevel(m_curView);
			unregisterCallBack();
		}
		break;
	}
	case PROTO_TYPE_GetPlayerShipListResult:
	{
	    GetPlayerShipListResult * result = (GetPlayerShipListResult *)message;
		if (result->failed == 0)
		{
			unregisterCallBack();
			flushShipListView(result);			
		}
		else
		{
//舰队中没有船只
			openView(COMMOM_COCOS_RES[C_VIEW_PLAYER_FLEET_CSB]);
			auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PLAYER_FLEET_CSB]);
			auto l_ships = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_ships"));
			auto p_gear = Helper::seekWidgetByName(view, "panel_gear");;
			auto t_no = Helper::seekWidgetByName(view, "text_no");		
			p_gear->setVisible(false);
			l_ships->setVisible(false);
			dynamic_cast<Text*>(t_no)->setString(SINGLE_SHOP->getTipsInfo()["TIP_SHIPLIST_NO_SHIP"]);
			t_no->setVisible(true);
			m_needClose = true;	
			unregisterCallBack();
		}
		break;
	}
	case PROTO_TYPE_GetPlayerEquipShipInfoResult:
	{
		GetPlayerEquipShipInfoResult * result = (GetPlayerEquipShipInfoResult *)message;
		if (result->failed == 0)
		{					
			int modelId = SINGLE_SHOP->getShipData()[result->sid].model_id;
			showShipModelView(modelId);
			flushShipEquimentView(result);
			unregisterCallBack();
		}
		break;
	}
	case PROTO_TYPE_GetHeroEquipResult:
	{
	    GetHeroEquipResult * result = (GetHeroEquipResult *)message;
		if (result->failed == 0)
		{
			flushCharacterModelView(result);
			unregisterCallBack();
		}
		break;
	}
	case PROTO_TYPE_GetItemsDetailInfoResult:
	{
		GetItemsDetailInfoResult *result = (GetItemsDetailInfoResult*)message;
		if (result->failed == 0)
		{
			if (result)
			{
				UICommon::getInstance()->flushEquipsDetail(result->equipment, result->itemid, true, true);
			}
			unregisterCallBack();
		}
		break;
	}
	/*case PROTO_TYPE_GetForceCityResult:
	{
		
		flushContryListview();
		GetForceCityResult  *result = (GetForceCityResult*)message;
		if (result->failed == 0)
		{
			UICommon::getInstance()->openCestatusView(result);
		 }
		break;
	}*/
	default:
		break;
	}
}

void UICommon::buttonEventByName(Widget* target,std::string name)
{
	this->stopAllActions();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (isButton(button_close))
	{
		if (_cameraMask == 4 && getViewRoot(COMMOM_COCOS_RES[C_VIEW_FRIEND_DETAIL_CSB]))
		{
			auto m_MapUI = dynamic_cast<UISailHUD*>(m_pParent);
			if (m_MapUI)
			{
				dynamic_cast<UISailHUD*>(m_MapUI)->shipPause(false);
			}			
		}
		auto instance = ModeLayerManger::getInstance();
		auto layer = instance->getModelLayer();
		auto type1 = instance->getNeedShowLayerType();
		if (layer != nullptr)
		{
			auto type = instance->getCurLayerType();
			if (type == COMMON_SHIP)
			{
				if (m_needClose && type1 != -1)
				{
					instance->showModelLayer(false);
// 				    instance->operaModels(type1);
					m_needClose = false;
				}
				else if (m_needClose)
				{
					instance->removeLayer();
					m_needClose = false;
				}
				else if (m_shipEquimentShow)
				{
					m_needClose = true;
				}
			}

			if (type == COMMON_CHARACTER)
			{
				if (m_needClose && type1 != -1)
				{
					instance->showModelLayer(false);
					// 				instance->operaModels(type1);
					m_needClose = false;
				}
				else if (m_needClose)
				{
					instance->removeLayer();
					m_needClose = false;
				}
				else if (layer->isVisible())
				{
					instance->showModelLayer(true);
					m_needClose = true;
				}
			}

			
		}
		n_ForceId = -1;
		closeView();
		return;
	}
	if (isButton(button_drop))
	{
		closeView();//关闭当前窗口，继续传递事件
	}
	if (isButton(button_s_no))
	{
		closeView();
		/*在酒吧女郎对话里需要向下传递*/
		//return;
	}
	if (isButton(button_v_no))
	{
		closeView();
		return;
	}
	if (isButton(button_w_yes))
	{
		closeView();
		return;
	}
	if(isButton(button_rank_close))
	{
		closeView();
		return;
	}
	if (isButton(panel_levelup))
	{
		closeView();
	}

	if (isButton(button_skip) || isButton(panel_captain_addexp))
	{
		if (m_listitems.size() == m_captainlist->getItems().size())
		{
			target->setTouchEnabled(false);
			unschedule(schedule_selector(UICommon::addItem));
			closeView();
			SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_uicommon0.plist");
			SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_uicommon1.plist");
		}
		else
		{
			if (isButton(button_skip))
			{
				m_bitemsaddCompleted = true;
			}	
			addItem(0);
			return;
		}
	}
	if ( isButton(panel_r_levelup) || isButton(panel_r_leveldown))
	{
		closeView();
	}
	if (isButton(image_item))
	{
		int id = target->getTag();
		DRAWINGSDESIGNINFO draws = SINGLE_SHOP->getDrawingsDesignInfo()[id];
		if (draws.type == 0)
		{
			flushShipDetail(nullptr,draws.define_id,true);
		}else
		{
			flushEquipsDetail(nullptr,draws.define_id,true);
		}
		return;
	}
	if (isButton(image_material))
	{
		int id = target->getTag();
		flushItemsDetail(nullptr,id,true);
		return;
	}

	if (isButton(button_del))
	{
		std::string str = m_inputNum->getString();
		if (!str.empty())
		{
			str.erase(str.end()-1);
			m_inputNum->setString(str);
		}
		return;
	}

	if (isButton(button_yes) || isButton(panel_numpad))
	{
		closeNumpad();
		std::string str = m_inputNum->getString();
		if (str.empty())
		{
			m_inputNum->setString("0");
		}
		return;
	}
	if (isButton(button_bank_info))
	{
		flushInfoBtnView("TIP_ENHANCE_VIEW_INFO_NAME", "TIP_ENHANCE_VIEW_INFO");
		return;
	}
	//取消修理装备
	if (isButton(button_repair_equip_no))
	{
		unregisterCallBack();
		closeView(COMMOM_COCOS_RES[COMMON_CONFIRM_REPAIR]);
		return;
	}
	//确认修理装备--道具充足时
	if (isButton(button_repair_equip_yes))
	{
		ProtocolThread::GetInstance()->repairEquipment(target->getTag(), 1, UILoadingIndicator::create(this));
		return;
	}
	//确认修理装备--道具不充足时
	if (isButton(button_repair_equip_v))
	{
		ProtocolThread::GetInstance()->repairEquipment(target->getTag(), 2, UILoadingIndicator::create(this));
		return;
	}
	//修理装备
	if (isButton(button_repair))
	{
		registerCallBack();
		ProtocolThread::GetInstance()->getRepairEquipmentNeed(target->getTag(),UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_ship_parts))
	{
		flushShipPartsView();
		return;
	}
	if (isButton(button_ship_Cabins))
	{
		flushShipCabinsView();
		return;
	}

	if (isButton(button_parts))
	{
// 		auto layer = dynamic_cast<Layer*>(_director->getRunningScene()->getChildByName("shipmodelayer"));
// 		if (layer != nullptr)
// 		{
// 			bool show = layer->isVisible();
// 			layer->setVisible(!show);
// 			m_shipEquimentShow = !m_shipEquimentShow;
// 			showEquipmentOrShipModel(!show);
// 		} 
		auto layer = ModeLayerManger::getInstance()->getModelLayer();
		if (layer != nullptr)
		{
			bool show = layer->isVisible();
			layer->setVisible(!show);
			m_shipEquimentShow = !m_shipEquimentShow;
			showEquipmentOrShipModel(!show);
		} 
		return;
	}
	if (isButton(button_viewequipment))
	{
		registerCallBack();
		ProtocolThread::GetInstance()->getHeroEquip(m_curuserInfo->usercid, UILoadingIndicator::createWithMask(this, _cameraMask));
		return;
	}
	if (isButton(button_viewfleet))
	{
		registerCallBack();
		ProtocolThread::GetInstance()->getPlayerShipList(m_curuserInfo->usercid, UILoadingIndicator::createWithMask(this, _cameraMask));
		return;
	}
	if (isButton(button_ship_))
	{
		registerCallBack();
		int tag = target->getTag();
		auto i_on = target->getChildByName("image_btn_on");
		if (i_on->isVisible())
		{
			return;
		}
		changeSlectShipButton(target);
		ProtocolThread::GetInstance()->getPlayerEquipShipInfo(tag, UILoadingIndicator::createWithMask(this, _cameraMask));
		return;
	}
	if (isButton(image_equip))
	{
		int tag = target->getParent()->getTag();
		if (tag == 0)
		{
			return;
		}
//		auto charlayer = dynamic_cast<Layer*>(_director->getRunningScene()->getChildByName("charactermodelayer"));
		auto instance = ModeLayerManger::getInstance();
		auto type = instance->getCurLayerType();
		if (type == COMMON_CHARACTER)
		{
			instance->showModelLayer(false);
			m_needClose = false;
			flushItemsDetail(nullptr, tag,true);
		}
		if (type == COMMON_SHIP)
		{
			m_needClose = false;
			registerCallBack();
			int id = getEquipIID(tag,m_ShipInfoResult);

			ProtocolThread::GetInstance()->getItemsDetailInfo(id, ITEM_TYPE_SHIP_EQUIP, tag, UILoadingIndicator::createWithMask(m_pParent, _cameraMask));
		}
		return;
	}
	if (isButton(panel_clone) || isButton(panel_expand) || isButton(button_droplistdown) || isButton(button_listdown))
	{
		if (isButton(panel_clone))
		{
			n_ForceId = target->getTag();
		}
		if (n_ForceId > -1)
		{
			ProtocolThread::GetInstance()->getForceCity(n_ForceId);
		}
		else
		{
			ProtocolThread::GetInstance()->getForceCity(SINGLE_HERO->m_iCurCityNation);
		}
		m_ListClose = false;
		return;
	}
	if (isButton(image_input_bg) || isButton(button_down) || isButton(button_dropdown))
	{
		if (isButton(button_down))
		{
			log("%d",1);
		}
		flushContryListview();
		return;
	}
	m_pParent->menuCall_func(target,Widget::TouchEventType::ENDED);
}

void UICommon::flushShipDetail(ShipDefine* shipInfo,int id,bool isHideDrop)
{
	openView(COMMOM_COCOS_RES[C_VIEW_SHIP_DETAIL_CSB]);
	SHIP_RES ship = SINGLE_SHOP->getShipData()[id];
	m_curShipId = id;
	int cur_hp = ship.hp_max;
	int cur_crew = 0;
	int cur_cargo = 0;
	int cur_supply = 0;
	int cur_attack = 0;
	int cur_position = 0;
	//熟练度id
	int n_poficiency = 0;
	//需要的熟练度等级
	int n_rarity = 0;
	n_poficiency = ship.proficiency_need;
	n_rarity = ship.rarity;
	if (shipInfo)
	{
		if (shipInfo->user_define_name &&std::strcmp(shipInfo->user_define_name,"")!= 0)
		{
			ship.name = shipInfo->user_define_name;
		}
		ship.price = shipInfo->price;
		ship.hp_max = shipInfo->hp_max;
		ship.supply_max = shipInfo->max_supply;
		ship.crew_max = shipInfo->max_crew_num;
		ship.crew_require = shipInfo->crew_required;
		ship.defense = shipInfo->defence;
		ship.speed = shipInfo->speed;
		ship.steer_speed = shipInfo->steer_speed;
		ship.rig_num = shipInfo->rigged_num;
		ship.armor_num = shipInfo->armor_num;
		ship.spinnnaker_num = shipInfo->spinnaker_num;
		ship.cannon_num = shipInfo->cannon_num;
		ship.cargo_kinds = shipInfo->cargo_kind;
		ship.cargo_size = shipInfo->cargo_size;
		cur_hp = shipInfo->hp;
		cur_crew = shipInfo->crew_num;
		cur_cargo = shipInfo->current_cargo_size;
		cur_supply = shipInfo->supply;
		cur_attack = shipInfo->attack;
		cur_position = shipInfo->position;
	}

	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SHIP_DETAIL_CSB]);
	if (view)
	{
		auto t_shipName = view->getChildByName<Text*>("label_ship_name");
		auto t_shipStory = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_story"));
		auto t_shipName_story = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_name_0"));
		auto nMaxStringNum = 0;
		auto languageTypeNum = 0;
		if (isChineseCharacterIn(ship.name.c_str()))
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
		t_shipName->setString(apostrophe(ship.name, nMaxStringNum));
		float height = getLabelHight(ship.name, t_shipName_story->getContentSize().width, t_shipName_story->getFontName(), t_shipName_story->getFontSize());
		float height1 = getLabelHight(ship.desc, t_shipStory->getContentSize().width, t_shipStory->getFontName(), t_shipStory->getFontSize());
		t_shipName_story->setContentSize(Size(t_shipName_story->getContentSize().width, height));
		t_shipStory->setContentSize(Size(t_shipStory->getContentSize().width, height1));
		t_shipName_story->setString(SINGLE_SHOP->getShipData()[id].name);
		t_shipName_story->setContentSize(Size(t_shipName_story->getContentSize().width, getLabelHight(SINGLE_SHOP->getShipData()[id].name, t_shipName_story->getContentSize().width, t_shipName_story->getFontName(), t_shipName_story->getFontSize())));
		t_shipName->setFontName("");
		setTextColorFormIdAndType(t_shipName, id, ITEM_TYPE_SHIP, true);
		t_shipStory->setString(ship.desc);
		auto list_story = view->getChildByName<ListView*>("listview_ship_story");
		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
		addListViewBar(list_story, image_pulldown);
		
		auto i_shipIcon = view->getChildByName<ImageView*>("image_ship");
		i_shipIcon->ignoreContentAdaptWithSize(false);
		i_shipIcon->loadTexture(getShipIconPath(id,IMAGE_ICON_SHADE));

		auto t_durable = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ship_durable_num_1"));
		auto t_weight = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ship_weight_num_1"));
		auto t_sailor = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ship_sailor_num_1"));
		auto t_supply = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ship_supply_num_1"));
		auto t_durable_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_2"));
		auto t_weight_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_weight_num_2"));
		auto t_sailor_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_sailor_num_2"));
		auto t_supply_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_supply_num_2"));


		auto l_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view,"progressbar_durable"));
		auto l_weight = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view,"progressbar_weight"));
		auto l_sailor = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view,"progressbar_sailor"));
		auto l_supply = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view,"progressbar_supply"));

		
		t_durable->setString(String::createWithFormat("%d/",cur_hp)->getCString());
		t_durable_1->setString(String::createWithFormat("%d",ship.hp_max)->getCString());
		t_durable_1->setPositionX(t_durable->getPositionX() + t_durable->getContentSize().width);

		t_sailor->setString(String::createWithFormat("%d/",cur_crew)->getCString());
		t_sailor_1->setString(String::createWithFormat("%d",ship.crew_max)->getCString());
		t_sailor_1->setPositionX(t_sailor->getPositionX() + t_sailor->getContentSize().width);

		t_weight->setString(String::createWithFormat("%d/",cur_cargo/100)->getCString());
		t_weight_1->setString(String::createWithFormat("%d",ship.cargo_size / 100)->getCString());
		t_weight_1->setPositionX(t_weight->getPositionX() + t_weight->getContentSize().width);

		t_supply->setString(String::createWithFormat("%d/",cur_supply)->getCString());
		t_supply_1->setString(String::createWithFormat("%d",ship.supply_max)->getCString());
		t_supply_1->setPositionX(t_supply->getPositionX() + t_supply->getContentSize().width);

		if (ship.hp_max < 1)
		{
			l_durable->setPercent(0);
		}else
		{
			l_durable->setPercent(100.0 * cur_hp/ship.hp_max);
		}
		if (ship.crew_max < 1)
		{
			l_sailor->setPercent(0);
		}else
		{
			l_sailor->setPercent(100.0 * cur_crew/ship.crew_max);
		}
		if (ship.cargo_size < 1)
		{
			l_weight->setPercent(0);
		}else
		{
			l_weight->setPercent(100.0 * cur_cargo/ship.cargo_size);
		}
		if (ship.supply_max < 1)
		{
			l_supply->setPercent(0);
		}else
		{
			l_supply->setPercent(100*cur_supply/ship.supply_max);
		}
	
		auto w_attack_bg = view->getChildByName("image_property_1");
		auto w_dense_bg = view->getChildByName("image_property_2");
		auto w_speed_bg = view->getChildByName("image_property_3");
		auto w_steer_bg = view->getChildByName("image_property_4");
		//attack
		auto t_attack = w_attack_bg->getChildByName<Text*>("label_atk_num");
		std::string s_attack = StringUtils::format("%d",cur_attack);
		t_attack->setString(s_attack);
		//defense
		auto t_defense = w_dense_bg->getChildByName<Text*>("label_defnum");
		std::string s_defense = StringUtils::format("%d",ship.defense);
		t_defense->setString(s_defense);
		//speed
		auto t_speed = w_speed_bg->getChildByName<Text*>("label_speed_num");
		std::string s_speed = StringUtils::format("%d",ship.speed);
		t_speed->setString(s_speed);
		//steer
		auto t_steer = w_steer_bg->getChildByName<Text*>("label_steering_num");
		std::string s_steer = StringUtils::format("%d",ship.steer_speed);
		t_steer->setString(s_steer);

		auto t_enhanceNum = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_enhancement_num"));
		int enhanceNum = 0;

		auto i_option1 = view->getChildByName<ImageView*>("image_enhancement_property");
		auto i_option2 = view->getChildByName<ImageView*>("label_enhancement_property_0");
		auto i_property1 = i_option1->getChildByName<ImageView*>("image_enhance");
		auto i_property2 = i_option2->getChildByName<ImageView*>("image_enhance");

		auto i_position1_1 = i_option1->getChildByName<ImageView*>("image_e_property_1");
		auto i_position1_2 = i_option1->getChildByName<ImageView*>("image_e_property_2");
		auto t_position1_1 = i_option1->getChildByName<Text*>("label_e_property_num_1");
		auto t_position1_2 = i_option1->getChildByName<Text*>("label_e_property_num_2");

		auto t_position2_1 = i_option2->getChildByName<Text*>("label_e_property_num_1");
		auto t_position2_2 = i_option2->getChildByName<Text*>("label_e_property_num_2");
		auto i_position2_1 = i_option2->getChildByName<ImageView*>("image_e_property_1");
		auto i_position2_2 = i_option2->getChildByName<ImageView*>("image_e_property_2");

		if (ship.enchant_slots == 0)
		{
			i_option1->setVisible(false);
			i_option2->setVisible(false);
		}else if (ship.enchant_slots == 1)
		{
			i_option1->setVisible(true);
			i_option2->setVisible(false);
		}else
		{
			i_option1->setVisible(true);
			i_option2->setVisible(true);
		}

		//注意理解1、2的作用
		int index1[2] = { -1 };
		int index2[2] = { -1 };
		int value1[7] = { 0 };
		int value2[7] = { 0 };
		if (shipInfo&&shipInfo->optional_value)
		{
			get_ship_enhance_value_array_from_string(shipInfo->optional_value, &value1, &value2, &index1, &index2);
		}

		if (shipInfo && shipInfo->optionid1 != 0)
		{
			enhanceNum++;	
			i_property1->setVisible(true);
			i_property1->ignoreContentAdaptWithSize(false);
			i_property1->loadTexture(getItemIconPath(shipInfo->optionid1,IMAGE_ICON_SHADE));

			if (index1[0] != -1)
			{
				i_position1_1->setVisible(true);
 				t_position1_1->setVisible(true);
				
				if (value1[index1[0]] < 0)
				{
					t_position1_1->setString(StringUtils::format("+%d", value1[index1[1]]));
					i_position1_1->loadTexture(PROPERTY_ICON_PTAH[index1[1] + 1]);
				}
				else
				{
					t_position1_1->setString(StringUtils::format("+%d", value1[index1[0]]));
					i_position1_1->loadTexture(PROPERTY_ICON_PTAH[index1[0] + 1]);
				}
				
			}else
			{
				i_position1_1->setVisible(false);
				t_position1_1->setVisible(false);
			}
			if (index1[1] != -1)
			{
 				i_position1_2->setVisible(true);
 				t_position1_2->setVisible(true);
				
				if (value1[index1[0]] < 0)
				{
					t_position1_2->setTextColor(TEXT_RED);
					t_position1_2->setString(StringUtils::format("-%d", -value1[index1[0]]));
					i_position1_2->loadTexture(PROPERTY_ICON_PTAH[index1[0] + 1]);
				}else
				{
					if (value1[index1[1]] < 0)
					{
						t_position1_2->setTextColor(TEXT_RED);
						t_position1_2->setString(StringUtils::format("-%d", -value1[index1[1]]));
					}
					else
					{
						t_position1_2->setString(StringUtils::format("+%d", value1[index1[1]]));
					}
					i_position1_2->loadTexture(PROPERTY_ICON_PTAH[index1[1] + 1]);
				}
			}else
			{
				i_position1_2->setVisible(false);
				t_position1_2->setVisible(false);
			}
		}else 
		{
			i_property1->setVisible(false);
			i_position1_1->setVisible(false);
			t_position1_1->setVisible(false);
			i_position1_2->setVisible(false);
			t_position1_2->setVisible(false);
		}

		if (shipInfo && shipInfo->optionid2 != 0)
		{
			enhanceNum++;
			i_property2->setVisible(true);
			i_property2->ignoreContentAdaptWithSize(false);
			i_property2->loadTexture(getItemIconPath(shipInfo->optionid2,IMAGE_ICON_SHADE));

			if (index2[0] != -1)
			{
				i_position2_1->setVisible(true);
				t_position2_1->setVisible(true);

				if (value2[index2[0]] < 0)
				{
					i_position2_1->loadTexture(PROPERTY_ICON_PTAH[index2[1] + 1]);
					t_position2_1->setString(StringUtils::format("%d", value2[index2[1]]));
				}
				else
				{
					i_position2_1->loadTexture(PROPERTY_ICON_PTAH[index2[0] + 1]);
					t_position2_1->setString(StringUtils::format("+%d", value2[index2[0]]));
				}
			}else
			{
				i_position2_1->setVisible(false);
				t_position2_1->setVisible(false);
			}
			if (index2[1]!= -1)
			{
				i_position2_2->setVisible(true);
				t_position2_2->setVisible(true);
				
				if (value2[index2[0]] < 0)
				{
					t_position2_2->setTextColor(TEXT_RED);
					t_position2_2->setString(StringUtils::format("-%d", -value2[index2[0]]));
					i_position2_2->loadTexture(PROPERTY_ICON_PTAH[index2[0] + 1]);
				}
				else
				{
					if (value2[index2[1]] < 0)
					{
						t_position2_2->setTextColor(TEXT_RED);
						t_position2_2->setString(StringUtils::format("-%d", -value2[index2[1]]));
					}
					else
					{
						t_position2_2->setString(StringUtils::format("+%d", value2[index2[1]]));
					}
					i_position2_2->loadTexture(PROPERTY_ICON_PTAH[index2[1] + 1]);
				}
			}else
			{
				i_position2_2->setVisible(false);
				t_position2_2->setVisible(false);
			}
		}else
		{
			i_property2->setVisible(false);
			i_position2_1->setVisible(false);
			t_position2_1->setVisible(false);
			i_position2_2->setVisible(false);
			t_position2_2->setVisible(false);
		}
		
		std::string s_enhanceNum = StringUtils::format("(%d/%d)",enhanceNum,ship.enchant_slots);
		t_enhanceNum->setString(s_enhanceNum);
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_require_lv"));
		t_lv->setString(StringUtils::format("Lv. %d",ship.require_level));
		auto t_require=dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_require"));
		auto i_requirebg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_lv_bg"));
		if (SINGLE_HERO->m_iLevel < ship.require_level)
		{
			t_lv->setTextColor(Color4B(255, 255, 255, 255));
			i_requirebg->loadTexture("cocosstudio/login_ui/market_720/required_lv_bg_2.png");
		}

		auto l_require = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_require_0"));
		l_require->setClippingEnabled(false);
		l_require->refreshView();

		Button* btn_drop = view->getChildByName<Button*>("button_drop");
		if (cur_position >= 0 || isHideDrop)//fleet ship not remove
		{
			btn_drop->setVisible(false);
		}else
		{
			btn_drop->setVisible(true);
		}

		auto i_dur = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_durable"));
		auto i_sai = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_sailor"));
		auto i_wei = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_weight"));
		auto i_sup = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_supply"));
		i_dur->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));
		i_dur->setTouchEnabled(true);
		i_dur->setTag(IMAGE_ICON_DURABLE+IMAGE_INDEX2);
		i_sai->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));
		i_sai->setTouchEnabled(true);
		i_sai->setTag(IMAGE_ICON_SAILOR+IMAGE_INDEX2);
		i_wei->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));
		i_wei->setTouchEnabled(true);
		i_wei->setTag(IMAGE_ICON_CARGO+IMAGE_INDEX2);
		i_sup->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));
		i_sup->setTouchEnabled(true);
		i_sup->setTag(IMAGE_ICON_SUPPLY+IMAGE_INDEX2);

		auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_atk"));
		auto i_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_def"));
		auto i_spe = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_speed"));
		auto i_ste = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_steering"));
		i_atk->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));
		i_atk->setTouchEnabled(true);
		i_atk->setTag(IMAGE_ICON_ATTACK+IMAGE_INDEX2);
		i_def->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));
		i_def->setTouchEnabled(true);
		i_def->setTag(IMAGE_ICON_DEFENCE+IMAGE_INDEX2);
		i_spe->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));
		i_spe->setTouchEnabled(true);
		i_spe->setTag(IMAGE_ICON_SPEED+IMAGE_INDEX2);
		i_ste->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
		i_ste->setTouchEnabled(true);
		i_ste->setTag(IMAGE_ICON_STEERING + IMAGE_INDEX2);
	
		auto l_sort = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_sort"));
		l_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMMON_SHIP_TYPE"]);

		auto label_limit = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_limit"));
		auto image_limit = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_limit"));
//	去除国家
#if 1
		label_limit->removeFromParent();
		image_limit->removeFromParent();
#else
		image_limit->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
		image_limit->setTouchEnabled(true);
		image_limit->setTag(ship.nation_limit + 13 + IMAGE_INDEX2);
		if (ship.nation_limit > 0)
		{
			label_limit->setVisible(true);
			image_limit->setVisible(true);
			image_limit->loadTexture(getCountryIconPath(ship.nation_limit));
			image_limit->setPositionX(label_limit->getContentSize().width + image_limit->getContentSize().width/2);
		}
		else
		{
			label_limit->setVisible(false);
			image_limit->setVisible(false);
			label_limit->removeFromParent();
			image_limit->removeFromParent();
		}
#endif 
//chengyuan++ 船只是否可以交易
		auto t_untradable = Helper::seekWidgetByName(view, "label_untradable");
		if (ship.no_trade == 1)
		{
			t_untradable->setVisible(true);
		}
		else
		{
			t_untradable->setVisible(false);
		}


		
		for (int i = 1; i < 8; i++)
		{
			int value = value1[i - 1] + value2[i - 1];
			//强化表达
			switch (i)
			{
			case ICON_ATTACK:
				break;
			case ICON_DEFENSE:
				if (value > 0)
				{
					t_defense->setTextColor(Color4B(1, 120, 5, 255));
				}
				else if (value < 0)
				{
					t_defense->setTextColor(TEXT_RED);
				}
				break;
			case ICON_DURABLE:
				if (value > 0)
				{
					t_durable_1->setTextColor(Color4B(1, 120, 5, 255));
				}
				else if (value < 0)
				{
					t_durable_1->setTextColor(TEXT_RED);
				}
				break;
			case ICON_SPEED:
				if (value > 0)
				{
					t_speed->setTextColor(Color4B(1, 120, 5, 255));
				}
				else if (value < 0)
				{
					t_speed->setTextColor(TEXT_RED);
				}
				break;
			case ICON_STEERING:
				if (value > 0)
				{
					t_steer->setTextColor(Color4B(1, 120, 5, 255));
				}
				else if (value < 0)
				{
					t_steer->setTextColor(TEXT_RED);
				}
				break;
			case ICON_SAILORS:
				if (value > 0)
				{
					t_sailor_1->setTextColor(Color4B(1, 120, 5, 255));
				}
				else if (value < 0)
				{
					t_sailor_1->setTextColor(TEXT_RED);
				}
				break;
			case ICON_CAPACITY:
				if (value > 0)
				{
					t_weight_1->setTextColor(Color4B(1, 120, 5, 255));
				}
				else if (value < 0)
				{
					t_weight_1->setTextColor(TEXT_RED);
				}
				break;
			case ICON_SUPPLY:
				if (value > 0)
				{
					t_supply_1->setTextColor(Color4B(1, 120, 5, 255));
				}
				else if (value < 0)
				{
					t_supply_1->setTextColor(TEXT_RED);
				}
				break;
			default:
				break;
			}
		}	
		auto t_proficiency = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_require_bluep"));
		auto l_shipview = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_ship"));
		auto i_pulldown_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown_1"));
		std::string str = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE_REQUIRE"] + SINGLE_SHOP->getProficiencyInfo()[n_poficiency].name + " Lv.%d";
		if (n_poficiency == 0)
		{
			t_proficiency->setVisible(false);
		}
		else
		{
			t_proficiency->setString(StringUtils::format(str.c_str(), n_rarity));
		}
		auto b_pulldown = i_pulldown_1->getChildByName<Button*>("button_pulldown");
		b_pulldown->setPositionX(b_pulldown->getPositionX()-9);
		addListViewBar(l_shipview, i_pulldown_1);
		m_curView = VIEW_SHIP;
		auto dock = dynamic_cast<UIShipyard*>(_director->getRunningScene()->getChildByTag(100 + DOCK_TAG));
		auto center = dynamic_cast<UICenter*>(_director->getRunningScene()->getChildByTag(100 + CENTER_TAG));
		if (dock != nullptr || center != nullptr)
		{
			if (dock != nullptr)
			{
				m_proficiencyResult = dock->getProficiencyResult();
			}else if (center != nullptr)
			{
				auto character = center->getCharacterLayer();
				m_proficiencyResult = character->getProficiencyResult();
			}
			if (m_proficiencyResult == nullptr)
			{
				registerCallBack();
				ProtocolThread::GetInstance()->getProficiencyValues(UILoadingIndicator::create(this));
				return;
			}
			judgeProficiencyLevel(m_curView);
		}
		else
		{
			registerCallBack();
			ProtocolThread::GetInstance()->getProficiencyValues(UILoadingIndicator::create(this));
		}
	}
}

void UICommon::flushEquipsDetail(EquipmentDefine* equipDetail, int id, bool isHideDrop, bool canNotRepair)
{
	ITEM_RES item = SINGLE_SHOP->getItemData()[id];
	m_curEquipId = id;
	int cur_durabe = item.max_durability;
	int att[5];
	std::string property2 = item.property2;
	property2 += ",";
	int startPos = 0;
	int n_poficnecy = item.poficiency;
	int n_rarity = item.rarity;
	for (int i = 0; ;i++)
	{
		if (property2.empty())
		{
			break;
		}
		std::string::size_type index = property2.find(',',startPos);
		if (index != std::string::npos)
		{
			std::string subStr = property2.substr(startPos,index - startPos);
			int num = atoi(subStr.c_str());
			att[i] = num;
			startPos = index + 1;
		}else
		{
			break;
		}
	}
	m_pEquipDetail = equipDetail;

	if (equipDetail)
	{
		att[0] = equipDetail->attack;
		att[1] = equipDetail->defense;
		att[2] = equipDetail->durable;
		att[3] = equipDetail->speed;
		att[4] = equipDetail->steer_speed;
		cur_durabe = equipDetail->durable;
	}
	
	openView(COMMOM_COCOS_RES[C_VIEW_EQUIP_DETAIL_CSB]);
	auto view =getViewRoot(COMMOM_COCOS_RES[C_VIEW_EQUIP_DETAIL_CSB]);
	auto cameras = Director::getInstance()->getRunningScene()->getCameras();
	if (cameras.size() >= 2){
		auto flag = (unsigned short)cameras[cameras.size() - 1]->getCameraFlag();
		view->setCameraMask((unsigned short)cameras[cameras.size() - 1]->getCameraFlag(), true);
	}
	if (view)
	{
		int range = item.property1;
		auto t_equipName = view->getChildByName<Text*>("label_equip_name");
		auto t_equipStory = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_equip_story"));

		t_equipName->setString(item.name);
		setTextColorFormIdAndType(t_equipName, id, ITEM_TYPE_SHIP_EQUIP,true);

		t_equipStory->setString(item.desc);
		t_equipStory->setContentSize(Size(t_equipStory->getContentSize().width,getLabelHight(item.desc,t_equipStory->getContentSize().width,t_equipStory->getFontName())));
		auto list_story = view->getChildByName<ListView*>("listview_equip_story");
		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
		addListViewBar(list_story,image_pulldown);

		auto i_equipIcon = view->getChildByName<ImageView*>("image_equip");
		i_equipIcon->ignoreContentAdaptWithSize(false);
		i_equipIcon->loadTexture(getItemIconPath(id,IMAGE_ICON_SHADE));

		auto t_durable = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ship_durable_num"));
		auto t_durable_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_0"));

		auto l_durable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view,"progressbar_durable"));
		t_durable->setText(String::createWithFormat("%d",cur_durabe)->getCString());
		t_durable_1->setText(String::createWithFormat("/%d",item.max_durability)->getCString());
		t_durable_1->setPositionX(t_durable->getPositionX() + t_durable->getContentSize().width);

		if (equipDetail && cur_durabe <= 5)
		{
			t_durable->setTextColor(Color4B::RED);
		}
		else
		{
			t_durable->setTextColor(Color4B(40,25,13,255));
		}

		if (item.max_durability < 0)
		{
			l_durable->setPercent(0);
		}else
		{
			l_durable->setPercent(100.0*cur_durabe/item.max_durability);
		}
		//weight
		auto t_weight = view->getChildByName<Text*>("label_ship_weight_num");
		t_weight->setString(String::createWithFormat("%0.2f",item.weight/100.0)->getCString());

		auto w_attack_bg = view->getChildByName("image_property_1");
		auto t_attack = w_attack_bg->getChildByName<Text*>("label_atk_num");
		auto i_attack = w_attack_bg->getChildByName<ImageView*>("image_atk");

		auto w_attack_bg_1 = view->getChildByName("image_property_2");
		auto t_attack_1 = w_attack_bg_1->getChildByName<Text*>("label_speed_num");
		auto i_attack_1 = w_attack_bg_1->getChildByName<ImageView*>("image_speed");

		auto i_range = view->getChildByName<Text*>("label_range");
		auto t_equip = view->getChildByName<Text*>("label_equip_content");
		//t_equip->setFontSize(22);
		i_attack->setTouchEnabled(true);
		i_attack->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));
	
		i_attack_1->setTouchEnabled(true);
		i_attack_1->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));

		auto t_enhanceNum = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_enhancement_num"));
		int enhanceNum = 0;

		auto i_option1 = view->getChildByName<ImageView*>("image_enhancement_property");
		auto i_option2 = view->getChildByName<ImageView*>("label_enhancement_property_0");
		auto i_property1 = i_option1->getChildByName<ImageView*>("image_enhance");
		auto i_property2 = i_option2->getChildByName<ImageView*>("image_enhance");

		auto i_position1_1 = i_option1->getChildByName<ImageView*>("image_e_property_1");
		auto i_position1_2 = i_option1->getChildByName<ImageView*>("image_e_property_2");
		auto t_position1_1 = i_option1->getChildByName<Text*>("label_e_property_num_1");
		auto t_position1_2 = i_option1->getChildByName<Text*>("label_e_property_num_2");

		auto t_position2_1 = i_option2->getChildByName<Text*>("label_e_property_num_1");
		auto t_position2_2 = i_option2->getChildByName<Text*>("label_e_property_num_2");
		auto i_position2_1 = i_option2->getChildByName<ImageView*>("image_e_property_1");
		auto i_position2_2 = i_option2->getChildByName<ImageView*>("image_e_property_2");

		if (item.property3 == 0)
		{
			i_option1->setVisible(false);
			i_option2->setVisible(false);
		}else if (item.property3 == 1)
		{
			i_option1->setVisible(true);
			i_option2->setVisible(false);
		}else
		{
			i_option1->setVisible(true);
			i_option2->setVisible(true);
		}

		//注意理解1、2的作用
		int index1[2] = { -1 };
		int index2[2] = { -1 };
		int value1[7] = { 0 };
		int value2[7] = { 0 };
		if (equipDetail&&equipDetail->optionalvalue)
		{
			get_ship_enhance_value_array_from_string(equipDetail->optionalvalue, &value1, &value2, &index1, &index2);
		}

		if (equipDetail && equipDetail->optionalid != 0)
		{
			enhanceNum++;	
			i_property1->setVisible(true);
			i_property1->ignoreContentAdaptWithSize(false);
			i_property1->loadTexture(::getItemIconPath(equipDetail->optionalid,IMAGE_ICON_SHADE));

			if (index1[0] != -1)
			{
				i_position1_1->setVisible(true);
 				t_position1_1->setVisible(true);
				
				if (value1[index1[0]] < 0)
				{
					i_position1_1->loadTexture(PROPERTY_ICON_PTAH[index1[1]]);
					t_position1_1->setString(StringUtils::format("+%d",value1[index1[1]]));
				}
				else
				{
					i_position1_1->loadTexture(PROPERTY_ICON_PTAH[index1[0]]);
					t_position1_1->setString(StringUtils::format("+%d", value1[index1[0]]));
				}
			}else
			{
				i_position1_1->setVisible(false);
				t_position1_1->setVisible(false);
			}
			if (index1[1] != -1)
			{
 				i_position1_2->setVisible(true);
 				t_position1_2->setVisible(true);
				
				if (value1[index1[0]] < 0)
				{
					t_position1_2->setTextColor(TEXT_RED);
					t_position1_2->setString(StringUtils::format("-%d", -value1[index1[0]]));
					i_position1_2->loadTexture(PROPERTY_ICON_PTAH[index1[0]]);
				}
				else
				{
					if (value1[index1[1]] < 0)
					{
						t_position1_2->setTextColor(TEXT_RED);
						t_position1_2->setString(StringUtils::format("-%d", -value1[index1[1]]));
					}
					else
					{
						t_position1_2->setString(StringUtils::format("+%d", value1[index1[1]]));
					}
					i_position1_2->loadTexture(PROPERTY_ICON_PTAH[index1[1]]);
				}
			}else
			{
				i_position1_2->setVisible(false);
				t_position1_2->setVisible(false);
			}

		}else 
		{
			i_property1->setVisible(false);
			i_position1_1->setVisible(false);
			t_position1_1->setVisible(false);
			i_position1_2->setVisible(false);
			t_position1_2->setVisible(false);
		}

		std::string s_enhanceNum = StringUtils::format("(%d/%d)",enhanceNum,item.property3);
		t_enhanceNum->setString(s_enhanceNum);
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_require_lv"));
		t_lv->setString(StringUtils::format("Lv. %d",item.required_lv));
		auto t_require=dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_require"));
		auto i_requirebg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_lv_bg"));
		if (SINGLE_HERO->m_iLevel<item.required_lv)
		{
			t_lv->setTextColor(Color4B(255,255,255,255));
			i_requirebg->loadTexture("cocosstudio/login_ui/market_720/required_lv_bg_2.png");
		}

		auto l_require = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_require"));
		l_require->refreshView();

		Button* btn_drop = view->getChildByName<Button*>("button_drop");
		if (isHideDrop)
		{
			btn_drop->setVisible(false);
		}else
		{
			btn_drop->setVisible(true);
		}
		auto i_durable = view->getChildByName<ImageView*>("image_durable");
		i_durable->setTag(IMAGE_ICON_DURABLE + IMAGE_INDEX2);
		i_durable->setTouchEnabled(true);
		i_durable->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));

		auto i_weight = view->getChildByName<ImageView*>("image_weight");
		i_weight->setTag(IMAGE_ICON_WEIGHT + IMAGE_INDEX2);
		i_weight->setTouchEnabled(true);
		i_weight->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));

		auto l_sort = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_sort"));
		l_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMMON_SHIPEQUIP_TYPE"]);
//chengyuan++ 装备是否可以交易
		auto t_untradable = Helper::seekWidgetByName(view, "label_untradable");
		if (item.no_trade == 1)
		{
			t_untradable->setVisible(true);
		}
		else
		{
			t_untradable->setVisible(false);
		}


		int sub_type = item.sub_type;
		switch (sub_type)
		{
		case SUB_TYPE_SHIP_FIGUREHEAD:
			{
				w_attack_bg->setVisible(false);
				i_range->setVisible(false);
				t_equip->setVisible(true);
				t_equip->setString(item.shortdesc);
				t_equip->setContentSize(Size(t_equip->getContentSize().width, getLabelHight(item.shortdesc, t_equip->getContentSize().width, t_equip->getFontName())));
				break;
			}
		case SUB_TYPE_SHIP_EMBOLON:
			{
				w_attack_bg->setVisible(true);
				i_range->setVisible(false);
				t_equip->setVisible(false);
				t_attack->setString(StringUtils::format("%d", att[0]));
				i_attack->loadTexture(PROPERTY_ICON_PTAH[ICON_ATTACK]);
				i_attack->setTag(IMAGE_ICON_ATTACK + IMAGE_INDEX2);
				break;
			}
		case SUB_TYPE_SHIP_STERN_GUN: //不存在了
			{
				break;
			}
		case SUB_TYPE_SHIP_SAIL:
			{
				w_attack_bg->setVisible(true);
				i_attack->loadTexture(PROPERTY_ICON_PTAH[ICON_SPEED]);
				i_attack->setTag(IMAGE_ICON_SPEED + IMAGE_INDEX2);
				t_attack->setString(StringUtils::format("%d", att[3]));
				i_range->setVisible(false);
				t_equip->setVisible(false);

				if (value1[ICON_SPEED] > 0)
				{
					t_attack->setTextColor(Color4B(1, 120, 5, 255));

				}
				else if (value1[ICON_SPEED] < 0)
				{
					t_attack->setTextColor(TEXT_RED);
				}

				w_attack_bg_1->setVisible(true);
				t_attack_1->setString(StringUtils::format("%d", att[4]));
				i_attack_1->loadTexture(PROPERTY_ICON_PTAH[ICON_STEERING]);
				i_attack_1->setTag(IMAGE_ICON_STEERING + IMAGE_INDEX2);
				if (value1[ICON_STEERING] > 0)
				{
					t_attack_1->setTextColor(Color4B(1, 120, 5, 255));
					
				}
				else if (value1[ICON_STEERING] < 0)
				{
					t_attack_1->setTextColor(TEXT_RED);
				}
				break;
			}
		case SUB_TYPE_SHIP_ABANDON:
			{
				break;
			}
		case SUB_TYPE_SHIP_ARMOUR:
			{
				w_attack_bg->setVisible(true);
				t_attack->setString(StringUtils::format("%d", att[1]));
				i_attack->loadTexture(PROPERTY_ICON_PTAH[ICON_DEFENSE]);
				i_attack->setTag(IMAGE_ICON_DEFENCE + IMAGE_INDEX2);
				i_range->setVisible(false);
				t_equip->setVisible(false);
				if (value1[ICON_DEFENSE] > 0)
				{
					t_attack->setTextColor(Color4B(1, 120, 5, 255));

				}
				else if (value1[ICON_DEFENSE] < 0)
				{
					t_attack->setTextColor(TEXT_RED);
				}

				if (att[3] < 0)
				{
					w_attack_bg_1->setVisible(true);
					t_attack_1->setString(StringUtils::format("%d", att[3]));
					i_attack_1->loadTexture(PROPERTY_ICON_PTAH[ICON_SPEED]);
					i_attack_1->setTag(IMAGE_ICON_SPEED + IMAGE_INDEX2);
					t_attack_1->setTextColor(TEXT_RED);
				}
				break;
			}
		case SUB_TYPE_SHIP_GUN:
			{
				w_attack_bg->setVisible(true);
				t_attack->setString(StringUtils::format("%d", att[0]));
				i_attack->loadTexture(PROPERTY_ICON_PTAH[ICON_ATTACK]);
				i_attack->setTag(IMAGE_ICON_ATTACK + IMAGE_INDEX2);
				i_range->setVisible(true);
				t_equip->setVisible(false);
				if (value1[ICON_ATTACK] > 0)
				{
					t_attack->setTextColor(Color4B(1, 120, 5, 255));

				}
				else if (value1[ICON_ATTACK] < 0)
				{
					t_attack->setTextColor(TEXT_RED);
				}
				for (int i = 0; i < 3; i++)
				{
					auto b_range = i_range->getChildByName<Button*>(StringUtils::format("button_range_%d", i + 1));
					b_range->setTouchEnabled(false);
					if (i < range)
					{
						b_range->setBright(false);
					}
					else
					{
						b_range->setBright(true);
					}
				}
				if (att[3] < 0)
				{
					w_attack_bg_1->setVisible(true);
					t_attack_1->setString(StringUtils::format("%d", att[3]));
					i_attack_1->loadTexture(PROPERTY_ICON_PTAH[ICON_SPEED]);
					i_attack_1->setTag(IMAGE_ICON_SPEED + IMAGE_INDEX2);
					t_attack_1->setTextColor(TEXT_RED);
				}
				break;
			}
		default:
			break;
		}

		auto b_repair = view->getChildByName<Button*>("button_repair");
		//先做隐藏
		auto i_limit = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_limit"));
		i_limit->setVisible(false);

		if (equipDetail && equipDetail->durable < equipDetail->maxdurable)
		{
			if (!canNotRepair)
			{
				b_repair->setVisible(true);
				b_repair->setTag(equipDetail->uniqueid);
			}
			else
			{
				b_repair->setVisible(false);
			}
		}
		else
		{
			b_repair->setVisible(false);
		}
		auto t_proficiency = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_require_bluep"));
		std::string str = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE_REQUIRE"] + SINGLE_SHOP->getProficiencyInfo()[n_poficnecy].name + " Lv.%d";
		if (n_poficnecy == 0)
		{
			t_proficiency->setVisible(false);
		}
		else
		{
			t_proficiency->setString(StringUtils::format(str.c_str(), n_rarity));
		}
		m_curView = VIEW_EQUIPMENT;
		auto dock = dynamic_cast<UIShipyard*>(_director->getRunningScene()->getChildByTag(100 + DOCK_TAG));
		auto center = dynamic_cast<UICenter*>(_director->getRunningScene()->getChildByTag(100 + CENTER_TAG));
		if (dock != nullptr || center != nullptr)
		{
			if (dock != nullptr)
			{
				m_proficiencyResult = dock->getProficiencyResult();
			}
			else if (center != nullptr)
			{
				auto character = center->getCharacterLayer();
				m_proficiencyResult = character->getProficiencyResult();
			}
			if (m_proficiencyResult == nullptr)
			{
				registerCallBack();
				ProtocolThread::GetInstance()->getProficiencyValues(UILoadingIndicator::create(this));
				return;
			}
			judgeProficiencyLevel(m_curView);
		}
		else
		{
			registerCallBack();
			ProtocolThread::GetInstance()->getProficiencyValues(UILoadingIndicator::create(this));
		}
	}
		
}
void UICommon::flushGoodsDetail(HatchItemsDefine* itemDefine,int id,bool isHideDrop)
{
	GOODS_RES item = SINGLE_SHOP->getGoodsData()[id];
	openView(COMMOM_COCOS_RES[C_VIEW_ITEM_DETAIL_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_DETAIL_CSB]);
	view->setCameraMask(_cameraMask,true);
	if (view)
	{
		auto l_name = view->getChildByName<Text*>("label_item_name");
		auto i_icon = view->getChildByName<ImageView*>("image_item");
		auto l_weight = view->getChildByName<Text*>("label_item_weight_num");
//		auto l_des = view->getChildByName<Text*>("label_itemdetails");
		auto l_story = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)view,"label_item_story"));
		l_name->setString(item.name);
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(getGoodsIconPath(id, IMAGE_ICON_SHADE));
		l_weight->setString(StringUtils::format("%0.2f",item.weight/100.0));

		l_story->setString(item.desc);
		l_story->setContentSize(Size(l_story->getContentSize().width,getLabelHight(item.desc,l_story->getContentSize().width,l_story->getFontName())));
		auto list_story = view->getChildByName<ListView*>("listview_item_story");
		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
		addListViewBar(list_story,image_pulldown);

		Button* btn_drop = view->getChildByName<Button*>("button_drop");
		if (isHideDrop)
		{
			btn_drop->setVisible(false);
		}else
		{
			btn_drop->setVisible(true);
		}
		auto i_weight = view->getChildByName<ImageView*>("image_weight");
		i_weight->setTag(IMAGE_ICON_WEIGHT + IMAGE_INDEX2);
		i_weight->setTouchEnabled(true);
		i_weight->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));

		auto l_sort = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)view,"label_sort"));
		l_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMMON_GOOD_TYPE"]);
		auto no_trade = Helper::seekWidgetByName((Widget*)view, "label_untradable");
		no_trade->setVisible(false);
	}
}


void UICommon::flushItemsDetail(EquipmentDefine* itemDefine,int id,bool isHideDrop)
{
	ITEM_RES item = SINGLE_SHOP->getItemData()[id];
	openView(COMMOM_COCOS_RES[C_VIEW_ENHANCE_DETAIL_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ENHANCE_DETAIL_CSB]);

	if (view)
	{
		auto l_name = view->getChildByName<Text*>("label_item_name");
		auto i_icon = view->getChildByName<ImageView*>("image_item");
		auto l_weight = view->getChildByName<Text*>("label_item_weight_num");
//		auto l_des = view->getChildByName<Text*>("label_itemdetails");
		auto l_story = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)view,"label_item_story"));
		l_name->setString(item.name);
		setTextColorFormIdAndType(l_name, id, item.type, true);
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(getItemIconPath(id,IMAGE_ICON_SHADE));
		l_weight->setString(StringUtils::format("%0.2f",item.weight/100.0));
		
		l_story->setString(item.desc);
		l_story->setContentSize(Size(l_story->getContentSize().width,getLabelHight(item.desc,l_story->getContentSize().width,l_story->getFontName())+30));
		l_story->setTextVerticalAlignment(TextVAlignment::TOP);

		auto list_story = view->getChildByName<ListView*>("listview_item_story");
		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
		addListViewBar(list_story,image_pulldown);
		
		
		view->setCameraMask(_cameraMask, true);
		auto i_property_1 = view->getChildByName("image_property_1");
		auto i_property_2 = view->getChildByName("image_property_2");
		auto btn_info = Helper::seekWidgetByName(view, "button_bank_info");
		i_property_1->setVisible(false);
		i_property_2->setVisible(false);
		btn_info->setVisible(false);

		Button* btn_drop = view->getChildByName<Button*>("button_drop");
		if (isHideDrop)
		{
			btn_drop->setVisible(false);
		}else
		{
			btn_drop->setVisible(true);
		}
		auto i_weight = view->getChildByName<ImageView*>("image_weight");
		i_weight->setTag(IMAGE_ICON_WEIGHT + IMAGE_INDEX2);
		i_weight->setTouchEnabled(true);
		i_weight->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));
		auto l_sort = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)view,"label_sort"));
		std::string sort;
	
		std::string str_1 = item.property2;
		std::string value1 = "";
		std::string value2 = "";
		int index1 = -1;
		int index2 = -1;
		auto t_property_num1 = i_property_1->getChildByName<Text*>("label_atk_num");
		auto i_property1 = i_property_1->getChildByName<ImageView*>("image_atk");
		auto t_property_num2 = i_property_2->getChildByName<Text*>("label_atk_num");
		auto i_property2 = i_property_2->getChildByName<ImageView*>("image_atk");
		switch (item.type)
		{
		case ITEM_TYPE_GOOD:
			break;
		case ITEM_TYPE_SHIP:
			break;
		case ITEM_TYPE_SHIP_EQUIP:
			break;
		case ITEM_TYPE_PORP:
			switch (item.sub_type)
			{
			case SUB_TYPE_USE:
				switch (item.property4)
				{
				case SUB_SUB_TYPE_COMBAT_PROP:
					sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_COMBAT_PROP"];
					break;
				case SUB_SUB_TYPE_SAILING_PROP:
					sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_SAILING_PROP"];
					break;
				case SUB_SUB_TYPE_DISGUISE_PROP:
					sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_DISGUISE_PROP"];
					break;
				case SUB_SUB_TYPE_SPECIAL_PROP:
					sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_SPECIAL_PROP"];
					break;
				default:
					log("Item sub sub type error!!!");
					break;
				}
				break;
			case SUB_TYPE_MATERIAL:
				sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_PROP_MATERIAL"];
				break;
			case SUB_TYPE_SHIP_PLAN:
			case SUB_TYPE_EQUIP_PLAN:
				getNotZeroFromString(str_1, value1, value2, index1, index2);
				//必有一个
				i_property_1->setVisible(true);
				if (index2 == -1)
				{
					i_property_2->setVisible(false);
					btn_info->setPositionY(i_property_1->getPositionY());
				}
				else
				{
					i_property_2->setVisible(true);
				}
				btn_info->setVisible(true);
				btn_info->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func,this));

				
				if (item.sub_type == SUB_TYPE_SHIP_PLAN)
				{
					sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_SHIP_PLAN"];
					if (index1 != -1)
					{
						if (atoi(value1.c_str()) < 0)
						{
							t_property_num1->setString(StringUtils::format("+(%s)", value2.c_str()));
							i_property1->loadTexture(PROPERTY_ICON_PTAH[index2]);
							i_property1->setTouchEnabled(true);
							i_property1->setTag(index2 + 1 + IMAGE_INDEX2);
							i_property1->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));

						}
						else
						{
							t_property_num1->setString(StringUtils::format("+(%s)", value1.c_str()));
							i_property1->loadTexture(PROPERTY_ICON_PTAH[index1]);
							i_property1->setTouchEnabled(true);
							i_property1->setTag(index1 + 1 + IMAGE_INDEX2);
							i_property1->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));

						}
					}

					if (index2 != -1)
					{
						if (atoi(value1.c_str()) < 0)
						{
							t_property_num2->setString(StringUtils::format("%s", value1.c_str()));
							i_property2->loadTexture(PROPERTY_ICON_PTAH[index1]);
							i_property2->setTag(index1 + 1 + IMAGE_INDEX2);
							i_property2->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
							i_property2->setTouchEnabled(true);

						}
						else
						{
							if (atoi(value2.c_str()) < 0)
							{
								t_property_num2->setString(StringUtils::format("%s", value2.c_str()));
							}
							else
							{
								t_property_num2->setString(StringUtils::format("+(%s)", value2.c_str()));
							}
							i_property2->loadTexture(PROPERTY_ICON_PTAH[index2]);

							i_property2->setTag(index2 + 1 + IMAGE_INDEX2);
							i_property2->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
							i_property2->setTouchEnabled(true);

						}
					}
				}
				else
				{
					sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_SHIP_EQUIP_PLAN"];
					//特殊处理---船只是从1开始 装备是从0开始
					if (index1 != -1)
					{
						if (atoi(value1.c_str()) < 0)
						{
							t_property_num1->setString(StringUtils::format("+(%s)", value2.c_str()));
							i_property1->loadTexture(PROPERTY_ICON_PTAH[index2 - 1]);
							i_property1->setTouchEnabled(true);
							i_property1->setTag(index2 + IMAGE_INDEX2);
							i_property1->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
						}
						else
						{
							t_property_num1->setString(StringUtils::format("+(%s)", value1.c_str()));
							i_property1->loadTexture(PROPERTY_ICON_PTAH[index1 - 1]);
							i_property1->setTouchEnabled(true);
							i_property1->setTag(index1  + IMAGE_INDEX2);
							i_property1->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
						}
					}

					if (index2 != -1)
					{
						if (atoi(value1.c_str()) < 0)
						{
							t_property_num2->setString(StringUtils::format("%s", value1.c_str()));
							i_property2->loadTexture(PROPERTY_ICON_PTAH[index1 - 1]);
							i_property2->setTouchEnabled(true);
							i_property2->setTag(index1  + IMAGE_INDEX2);
							i_property2->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
						}
						else
						{
							if (atoi(value2.c_str()) < 0)
							{
								t_property_num2->setString(StringUtils::format("%s", value2.c_str()));
							}
							else
							{
								t_property_num2->setString(StringUtils::format("+(%s)", value2.c_str()));
							}
							i_property2->loadTexture(PROPERTY_ICON_PTAH[index2 - 1]);
							i_property2->setTouchEnabled(true);
							i_property2->setTag(index2 + IMAGE_INDEX2);
							i_property2->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
						}
					}
				}			
				break;
			case SUB_TYPE_GIRL_GIFT:
				sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_GIRL_GIFT"];
				break;
			case SUB_TYPE_STORY_DEBRIS:
			   sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_STORY_DEBRIS_TYPE"];
			   break;
			default:
				log("Item sub type error!!!");
				break;
				sort += SINGLE_SHOP->getTipsInfo()["TIP_COMMON_CONSUMABLE"];
			}
			break;
		case ITEM_TYPE_ROLE_EQUIP:
			sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_ROLLEQUIP_TYPE"];
			break;
		case ITEM_TYPE_DRAWING:
			break;
		case ITEM_TYPE_SPECIAL:
			break;
		default:
			log("Item type error!!!");
			break;
		}
		l_sort->setString(sort);

//chengyuan++ 物品是否可以交易
		auto t_untradable = Helper::seekWidgetByName(view, "label_untradable");
		if (item.no_trade == 1)
		{
			t_untradable->setVisible(true);
			t_untradable->setPositionX(l_sort->getPositionX() + l_sort->getBoundingBox().size.width + t_untradable->getBoundingBox().size.width / 2);
		}
		else
		{
			t_untradable->setVisible(false);
		}
	}	
}

void UICommon::flushDrawingDetail(DrawingItemsDefine* drawDefine,int id,bool isHideDrop)
{
	ITEM_RES item = SINGLE_SHOP->getItemData()[id];
	DRAWINGSDESIGNINFO draws = SINGLE_SHOP->getDrawingsDesignInfo()[id];
	openView(COMMOM_COCOS_RES[C_VIEW_DRAWING_DETAIL_CSB]);
	auto view =getViewRoot(COMMOM_COCOS_RES[C_VIEW_DRAWING_DETAIL_CSB]);
	const std::string goodsbg = "cocosstudio/login_ui/common/goods_bg.png";
	const std::string goodsblackbg = "cocosstudio/login_ui/common/goods_bg_black.png";
	if (view)
	{
		auto citysInfo = SINGLE_SHOP->getCitiesInfo();
		auto l_city = view->getChildByName<ListView*>("listview_blueprint");
		auto t_city = dynamic_cast<Text*>(l_city->getItem(1));;
		std::string cityList[50];
		int nIndex = 0;
		std::string tempCity = draws.city_id_list;
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
				cityList[nIndex] = citysInfo[cityId].name;
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
		t_city->setString(str);
		t_city->setContentSize(Size(t_city->getContentSize().width,getLabelHight(str,t_city->getContentSize().width,t_city->getFontName())));
		auto image_pulldown_2 = view->getChildByName<ImageView*>("image_pulldown_2");
		auto button_pulldown_2 = image_pulldown_2->getChildByName<Button*>("button_pulldown");
		button_pulldown_2->setPositionX(button_pulldown_2->getBoundingBox().size.width/2 - button_pulldown_2->getBoundingBox().size.width/2 + 2.5);
		addListViewBar(l_city,image_pulldown_2);
		
		Text* t_drawName = view->getChildByName<Text*>("label_ship_name");
		auto nMaxStringNum = 0;
		auto languageTypeNum = 0;
		if (isChineseCharacterIn(item.name.c_str()))
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
		t_drawName->setString(apostrophe(item.name, nMaxStringNum));
		auto p_item = l_city->getItem(0);
		Node* w_subRoot = p_item->getChildByName("panel_blueprint");
		Widget* w_drawingBg = w_subRoot->getChildByName<Widget*>("button_item_bg");
		w_drawingBg->setTouchEnabled(false);
		//drawing icon
		ImageView* i_drawing = w_drawingBg->getChildByName<ImageView*>("image_item");
		auto image_item_bg_lv = w_drawingBg->getChildByName<ImageView*>("image_item_bg_lv");

		std::string path;
		if (draws.type == 0)//船
		{
			path = getShipIconPath(draws.define_id,IMAGE_ICON_SHADE);
			//稀有度
			setBgButtonFormIdAndType(w_drawingBg, draws.define_id, ITEM_TYPE_SHIP);
			setBgImageColorFormIdAndType(image_item_bg_lv, draws.define_id, ITEM_TYPE_SHIP);
			setTextColorFormIdAndType(t_drawName, draws.define_id, ITEM_TYPE_SHIP,true);
		}else//装备
		{
			path = getItemIconPath(draws.define_id,IMAGE_ICON_SHADE);
			setBgButtonFormIdAndType(w_drawingBg, draws.define_id, ITEM_TYPE_SHIP_EQUIP);
			setBgImageColorFormIdAndType(image_item_bg_lv, draws.define_id, ITEM_TYPE_SHIP_EQUIP);
			setTextColorFormIdAndType(t_drawName, draws.define_id, ITEM_TYPE_SHIP_EQUIP, true);
		}
		i_drawing->setTag(id);
		i_drawing->loadTexture(path);
		i_drawing->setTouchEnabled(true);
		i_drawing->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func,this));

		Text* t_mater_1_num = w_subRoot->getChildByName<Text*>("label_material_num_1");
		Text* t_mater_2_num = w_subRoot->getChildByName<Text*>("label_material_num_2");
		std::vector<DRAWINGMATERIALINFO>require_items  = draws.required_items;		


		int needMaterialNum = 0;
		for (int i = 0; i < require_items.size(); i++)
		{
			auto curItemMaterial = require_items.at(i);
			if (curItemMaterial.requried_item_num > 0)
			{
				needMaterialNum++;
				auto curItemMaterial = require_items.at(i);
				std::string name_bg = StringUtils::format("image_material_bg_%d", needMaterialNum);
				std::string name_image = "image_material";
				std::string name_label_1 = StringUtils::format("label_material_num_%d", needMaterialNum);
				auto i_material1_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, name_bg));
				auto i_material1 = i_material1_bg->getChildByName<ImageView*>(name_image);
				auto t_material1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, name_label_1));
				i_material1_bg->setVisible(true);
				i_material1->setVisible(true);
				t_material1->setVisible(true);
				i_material1_bg->loadTexture(goodsbg);
				i_material1->loadTexture(getItemIconPath(curItemMaterial.requried_item));
				i_material1->setTag(curItemMaterial.requried_item);
				i_material1->setTouchEnabled(true);
				i_material1->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func,this));
				t_material1->setString(StringUtils::format("x %d", curItemMaterial.requried_item_num));
			}		
		}
		while (needMaterialNum != 5)
		{
			needMaterialNum++;
			std::string name_bg = StringUtils::format("image_material_bg_%d", needMaterialNum);
			std::string name_image = "image_material";
			std::string name_label_1 = StringUtils::format("label_material_num_%d", needMaterialNum);
			std::string name_label_2 = StringUtils::format("label_material_num_%d_0", needMaterialNum);
			auto i_material1_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, name_bg));
			auto i_material1 = i_material1_bg->getChildByName<ImageView*>(name_image);
			auto t_material1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, name_label_1));
			i_material1_bg->loadTexture(goodsblackbg);
			i_material1->setVisible(false);
			t_material1->setVisible(false);
		}



		Node* w_city_requir = p_item->getChildByName("image_city_crafting");
		Text* t_city_requir = w_city_requir->getChildByName<Text*>("label_crafting_num");
		t_city_requir->setString(String::createWithFormat("%d",draws.manufacture_req)->getCString());
		
		Node* w_silver = p_item->getChildByName("image_silver");
		Text* t_silver = w_silver->getChildByName<Text*>("label_clock_num");
		t_silver->setString(numSegment(StringUtils::format("%d",draws.coin)));

		Node* w_clock = p_item->getChildByName("image_clock");
		Text* t_clock = w_clock->getChildByName<Text*>("label_clock_num");
		t_clock->ignoreContentAdaptWithSize(true);
		std::string s_times = StringUtils::format("%d",draws.required_time);
		s_times += " ";
		s_times += SINGLE_SHOP->getTipsInfo()["TIP_MINUTE"];
		t_clock->setString(s_times);

		ListView* lv_story = view->getChildByName<ListView*>("listview_ship_story");
		Text* t_story = dynamic_cast<Text*>(lv_story->getItem(0));
		t_story->setString(item.desc);
		t_story->setContentSize(Size(t_story->getContentSize().width,getLabelHight(item.desc,t_story->getContentSize().width,t_story->getFontName())));
		auto list_story = view->getChildByName<ListView*>("listview_ship_story");
		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
		addListViewBar(list_story,image_pulldown);
		
		Button* btn_drop = view->getChildByName<Button*>("button_drop");
		if (isHideDrop)
		{
			btn_drop->setVisible(false);
		}else
		{
			btn_drop->setVisible(true);
		}

		auto i_craft = p_item->getChildByName<ImageView*>("image_city_crafting");
		i_craft->setTag(IMAGE_ICON_CRAFT + IMAGE_INDEX2);
		i_craft->setTouchEnabled(true);
		i_craft->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));

		auto i_silver = p_item->getChildByName<ImageView*>("image_silver");
		i_silver->setTag(IMAGE_ICON_SILVER + IMAGE_INDEX2);
		i_silver->setTouchEnabled(true);
		i_silver->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));

		auto i_clock = p_item->getChildByName<ImageView*>("image_clock");
		i_clock->setTag(IMAGE_ICON_CLOCK + IMAGE_INDEX2);
		i_clock->setTouchEnabled(true);
		i_clock->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent,this));

		auto l_sort = p_item->getChildByName<Text*>("label_consumable_item");
		std::string sort = SINGLE_SHOP->getTipsInfo()["TIP_COMMON_DRAWING_TYPE"];
		sort += SINGLE_SHOP->getTipsInfo()["TIP_COMMON_CONSUMABLE"];
		l_sort->setString(sort);
//chengyuan++
		auto no_trade = Helper::seekWidgetByName(view, "label_untradable");
		if (item.no_trade == 1)
		{
			no_trade->setVisible(true);
		}
		else
		{
			no_trade->setVisible(false);
		}


		l_city->refreshView();
	}else
	{
		closeView();
	}
}

void UICommon::flushSpecialDetail(int id,bool isHideDrop)
{
	SPECIALSINFO item = SINGLE_SHOP->getSpecialsInfo()[id];
	openView(COMMOM_COCOS_RES[C_VIEW_SPECIAL_DETAIL_CSB]);
	auto view =getViewRoot(COMMOM_COCOS_RES[C_VIEW_SPECIAL_DETAIL_CSB]);
	if (view)
	{
		auto l_name = view->getChildByName<Text*>("label_item_name");
		auto i_icon = view->getChildByName<ImageView*>("image_item");
//		auto l_des = view->getChildByName<Text*>("label_itemdetails");
		auto l_story = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)view,"label_item_story"));
	
		l_name->setString(item.name);
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(getItemIconPath(id, IMAGE_ICON_SHADE));
		l_story->setString(item.desc);
		l_story->setContentSize(Size(l_story->getContentSize().width,getLabelHight(item.desc,l_story->getContentSize().width,l_story->getFontName())+30));
		auto list_story = view->getChildByName<ListView*>("listview_item_story");
		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
		addListViewBar(list_story,image_pulldown);

		Button* btn_drop = view->getChildByName<Button*>("button_drop");
		if (isHideDrop)
		{
			btn_drop->setVisible(false);
		}else
		{
			btn_drop->setVisible(true);
		}

		auto no_trade = Helper::seekWidgetByName(view, "label_untradable");
		no_trade->setVisible(false);

		auto l_sort = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_consumable_item_0"));
		l_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMMON_SPECIAL_ITEM"]);
	}
}

void UICommon::getNameAndPath(int type,int id,std::string &name,std::string &path)
{
	if (type == ITEM_TYPE_GOOD)
	{
		name = getGoodsName(id);
		path = getGoodsIconPath(id,IMAGE_ICON_SHADE);
	}else if(type == ITEM_TYPE_SHIP)
	{
		name = getShipName(id);
		path = getShipIconPath(id,IMAGE_ICON_SHADE);
	}else
	{
		name = getItemName(id);
		path = getItemIconPath(id,IMAGE_ICON_SHADE);
	}
}

void UICommon::flushSilverConfirmView(std::string titleName,std::string contentName,int coin)
{
	openView(COMMOM_COCOS_RES[C_VIEW_COST_SILVER_COM_CSB]);
	auto  view =getViewRoot(COMMOM_COCOS_RES[C_VIEW_COST_SILVER_COM_CSB]);
	if (view)
	{
		auto tipInfo = SINGLE_SHOP->getTipsInfo();
		std::string s_title = tipInfo[titleName];
		std::string s_content = tipInfo[contentName];

		Text* t_title = view->getChildByName<Text*>("label_repair");
		Text* t_content = view->getChildByName<Text*>("label_repair_content");

		ListView* lv_costNum = view->getChildByName<ListView*>("listview_cost");
		Node* w_costNum = lv_costNum->getItem(1);
		Text* t_costNum = w_costNum->getChildByName<Text*>("label_cost_num");

		std::string s_cost = StringUtils::format("%d",coin);
		auto  btn_Yes = view->getChildByName<Button*>("button_s_yes");

		t_title->setString(s_title);
		t_content->setString(s_content);
		t_costNum->setString(numSegment(s_cost));

		btn_Yes->addTouchEventListener(CC_CALLBACK_2(UICommon::buttonYesEvent,this));
//chengyuan++
		auto i_silver = view->getChildByName("image_silver_1");
		i_silver->setPositionX(t_costNum->getPositionX() - t_costNum->getContentSize().width - i_silver->getContentSize().width/2 - 5); 
	}
}

void UICommon::flushVConfirmView(std::string titleName,std::string contentName,int v)
{
	openView(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
	auto view =getViewRoot(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
	if (view)
	{
		auto tipInfo = SINGLE_SHOP->getTipsInfo();
		std::string s_title = tipInfo[titleName];
		std::string s_content = tipInfo[contentName];

		Text* t_title = view->getChildByName<Text*>("label_advanced_repair");
		Text* t_content = view->getChildByName<Text*>("label_repair_content");

		ListView* lv_costNum = view->getChildByName<ListView*>("listview_cost");
		Node* w_costNum = lv_costNum->getItem(1);
		Text* t_costNum = w_costNum->getChildByName<Text*>("label_cost_num");
		auto  btn_Yes = view->getChildByName<Button*>("button_v_yes");
		std::string s_cost = numSegment(StringUtils::format("%d",v));
		auto image_silver = w_costNum->getChildByName<ImageView*>("image_silver_1");

		t_title->setString(s_title);
		t_content->setString(s_content);
		t_costNum->setString(s_cost);
		btn_Yes->addTouchEventListener(CC_CALLBACK_2(UICommon::buttonYesEvent,this));
		image_silver->setPositionX(t_costNum->getPositionX() - t_costNum->getBoundingBox().size.width - image_silver->getBoundingBox().size.width);
	}
}
void UICommon::flushInfoBtnView(std::string titleName, std::string contentName, int roomId)
{
	openView(COMMOM_COCOS_RES[C_VIEW_INFOBTN_CSB]);
	auto view =getViewRoot(COMMOM_COCOS_RES[C_VIEW_INFOBTN_CSB]);
	if (view)
	{
		auto t_title = view->getChildByName<Text*>("label_item_name");
		auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)view, "label_info"));
		std::string st_str;
		if (roomId != -1)
		{
			t_title->setString(SINGLE_SHOP->getJobPositionInfo().at(roomId).room);
			st_str = SINGLE_SHOP->getJobPositionInfo().at(roomId).desc;
		}
		else
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()[titleName]);
			st_str = SINGLE_SHOP->getTipsInfo()[contentName];
		}
		
		t_content->setString(st_str);
		auto l_info = view->getChildByName<ListView*>("list_info");
		t_content->setContentSize(Size(t_content->getContentSize().width, getLabelHight(st_str, t_content->getContentSize().width, t_content->getFontName(), t_content->getFontSize())));
		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
		addListViewBar(l_info, image_pulldown);
	}
}

void UICommon::flushSkillView(const SKILL_DEFINE skillDefine,bool needShow)
{
	openView(COMMOM_COCOS_RES[C_VIEW_SKILL_DES_CSB]);
	auto view =getViewRoot(COMMOM_COCOS_RES[C_VIEW_SKILL_DES_CSB]);
	view->setCameraMask(_cameraMask,true);
	if (view)
	{
		auto image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_head_16"));
		auto i_skill = view->getChildByName<Button*>("button_skill_bg");
		auto t_skill_lv = i_skill->getChildByName<Text*>("text_item_skill_lv");
		auto t_skillName = view->getChildByName<Text*>("label_repair");
		auto t_sort = view->getChildByName<Text*>("label_sort");
		auto image_lv = view->getChildByName<ImageView*>("image_lv_bg");
		auto t_require_lv = image_lv->getChildByName<Text*>("label_require_lv");

		auto p_content = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_content"));
		auto t_change_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cost"));
		auto t_change = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cost_1"));
		auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_content_1"));
		auto t_next_change_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cost_0"));
		auto t_next_change = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cost_0_0"));
		auto image_div_2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_div_2"));
		t_change_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_SKILL_UP_CURRENT_LV"]);
		t_next_change_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_REQUIP_NEXT_LV"]);

		t_skill_lv->enableOutline(Color4B::BLACK, 2);
		SKILL_TREE skill;
		SPECIALIES_SKILL_INFO specialies_skill;
		float number = 0;
		int cd = 0;
		float number2 = 0;
		std::string str;
		std::string old_value;
		std::string new_value;
		auto skill_type = skillDefine.skill_type;
		auto skillId = skillDefine.id;
		auto skillLv = skillDefine.lv;
		auto iconId = skillDefine.icon_id;
		switch (skill_type)
		{
		case SKILL_TYPE_CAPTAIN:
			skill = SINGLE_SHOP->getCaptainSkillInfo()[skillId];
			image_head->loadTexture(getCompanionIconPath(iconId,true));
			i_skill->loadTextureNormal(getSkillIconPath(skillId, skill_type));
			t_skillName->setString(skill.name);
			if (needShow)
			{
				t_skill_lv->setString(StringUtils::format("%d/%d", skillLv, skill.max_level));
			}			
			//主动
			if (skill.ap_type == 1)
			{
				t_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ACTIVE"]);
			}
			else
			{
				t_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_PASSIVITY"]);
			}
			image_lv->setVisible(false);
			p_content->setPositionY(p_content->getPositionY() + 30);
			switch (skillId)
			{
			case SKILL_CAPTAIN_GUN_EXPERT:
			case SKILL_CAPTAIN_RAM_EXPERT:
			case SKILL_CAPTAIN_WEAKNESS_ATTACK:
			case SKILL_CAPTAIN_ARMOR_DEFENSE:
			case SKILL_CAPTAIN_BEST_SELLOR:
			case SKILL_CAPTAIN_COOK_SKILL:
			case SKILL_CAPTAIN_TRANSACTION_REPUTAION:
			case SKILL_CAPTAIN_MISSION_EXPERT:
			case SKILL_CAPTAIN_SECRETARY:
			case SKILL_CAPTAIN_ACCURACY:
			case SKILL_CAPTAIN_SAIL_OPERATION:
			case SKILL_CAPTAIN_OUTLOOK:
			case SKILL_CAPTAIN_WAREHOUSING:
			case SKILL_CAPTAIN_LOGISTICS:
				number = skill.effect_per_lv*skillLv / SKILL_DER_MIN;
				number2 = skill.effect_per_lv*(skillLv + 1) / SKILL_DER_MIN;
				break;
			case SKILL_CAPTAIN_HAMMER_SHOOT:
			case SKILL_CAPTAIN_UNBEATABLE_WILL:
			case SKILL_CAPTAIN_ROAR_OF_VICTORY:
				number = skill.rate_per_lv*skillLv;
				number2 = skill.rate_per_lv*(skillLv + 1);
				break;
			default:
				number = skill.effect_per_lv*skillLv / SKILL_DER_MIN;
				number2 = skill.effect_per_lv*(skillLv + 1) / SKILL_DER_MIN;
				break;
			}
			str = skill.next_desc;
			old_value = "[number1]";
			new_value = StringUtils::format("%.2f", number);
			repalce_all_ditinct(str, old_value, new_value);
			t_change->setString(str);
			t_change->setPositionX(t_change_title->getPositionX() + t_change_title->getBoundingBox().size.width);
			str = skill.desc;
			t_content->setString(str);
			str = skill.next_desc;
			new_value = StringUtils::format("%.2f", number2);
			repalce_all_ditinct(str, old_value, new_value);
			t_next_change->setString(str);
			t_next_change->setPositionX(t_next_change_title->getPositionX() + t_next_change_title->getBoundingBox().size.width);
			if (skillLv >= skill.max_level)
			{
				t_next_change->setVisible(false);
				t_next_change_title->setVisible(false);
				image_div_2->setVisible(false);
			}
			break;
		case SKILL_TYPE_PLAYER:
			skill = SINGLE_SHOP->getSkillTrees()[skillId];
			image_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));
			i_skill->loadTextureNormal(getSkillIconPath(skillId, skill_type));
			t_skillName->setString(skill.name);
			t_skill_lv->setString(StringUtils::format("%d/%d", skillLv, skill.max_level));
			//主动
			if (skill.ap_type == 1)
			{
				t_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ACTIVE"]);
			}
			else
			{
				t_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_PASSIVITY"]);
			}
			image_lv->setVisible(false);
			p_content->setPositionY(p_content->getPositionY() + 30);
			switch (skillId)
			{
			case SKILL_POWWEFUL_SHOOT:
			case SKILL_SALVO_SHOOT:
			case SKILL_INCENDIARY_SHOOT:
			case SKILL_FAST_RELOAD:
			case SKILL_WEAKNESS_ATTACK:
			case SKILL_ASSAULT_NAVIGATION:
			case SKILL_SNEAK_ATTACK:
			case SKILL_EMBOLON_ATTACK:
			case SKILL_ATTACKING_HORN:
			case SKILL_PROGRESSIVE_REPAIR:
			case SKILL_EMERGENT_REPAIR:
			case SKILL_FORTRESS_BATTLESHIP:
			case SKILL_FLEET_REPAIR:
			case SKILL_ARMOUR_OPTIMIZATION:
			case SKILL_REFORM_OF_HULL:
			case SKILL_TAX_PROTOCOL:
			case SKILL_EXPERT_SELLOR:
			case SKILL_PUBLIC_SPEAKING:
			case SKILL_ADMINISTRATION:
			case SKILL_TECHNIQUE_OF_CARGO:
			case SKILL_MANUFACTURING_EXPERT:
			case SKILL_OCEAN_EXPLORATION:
			case SKILL_GOOD_COOK:
			case SKILL_MISSION_TERMINATOR:
			case SKILL_LATE_SUBMISSION:
			case SKILL_COMBAT_READY:
			case SKILL_PREDATORY_MASTE:
			case SKILL_BLACK_EAT_BLACK:
			case SKILL_FOOD_LOOTING:
			case SKILL_GREAT_SHOOTER:
				number = skill.effect_per_lv*skillLv / SKILL_DER_MIN;
				cd = skill.cd - skill.cdreduce_per_lv*skillLv;
				number2 = skill.effect_per_lv*(skillLv + 1) / SKILL_DER_MIN;
				break;
			case SKILL_HAMMER_SHOOT:
				number = skill.duration_per_lv*skillLv;
				cd = skill.cd - skill.cdreduce_per_lv*skillLv;
				number2 = skill.duration_per_lv*(skillLv + 1);
				break;
			case SKILL_INDOMITABLE_WILL:
			case SKILL_ROAR_OF_VICTORY:
			case SKILL_INDUCE_TO_CAPITULATE:
				number = skill.rate_per_lv*skillLv;
				cd = skill.cd - skill.cdreduce_per_lv*skillLv;
				number2 = skill.rate_per_lv*(skillLv + 1);
				break;
			
			case SKILL_HULL_TRACTION:
				number = skill.effect_per_lv*skillLv;
				cd = skill.cd - skill.cdreduce_per_lv*skillLv;
				number2 = skill.effect_per_lv*(skillLv + 1);
				break;
			default:
				number = skill.effect_per_lv*skillLv / SKILL_DER_MAX;
				cd = skill.cd - skill.cdreduce_per_lv*skillLv;
				number2 = skill.effect_per_lv*(skillLv + 1) / SKILL_DER_MAX;
				break;
			}
			str = skill.next_desc;
			old_value = "[number1]";
			if (skillId == SKILL_TECHNIQUE_OF_BUSINESS || skillId == SKILL_SHIPBUILDING_TECHNOLOGY || skillId == SKILL_HULL_TRACTION)
			{
				new_value = StringUtils::format("%.0f", number);
			}
			else
			{
				new_value = StringUtils::format("%.2f", number);
			}
			repalce_all_ditinct(str, old_value, new_value);
			t_change->setString(str);
			t_change->setPositionX(t_change_title->getPositionX() + t_change_title->getBoundingBox().size.width);
			
			str = skill.desc;
			old_value = "[cd]";
			new_value = StringUtils::format("%d", cd);
			repalce_all_ditinct(str, old_value, new_value);
			t_content->setString(str);

			if (skillLv >= skill.max_level)
			{
				t_next_change->setVisible(false);
				t_next_change_title->setVisible(false);
				image_div_2->setVisible(false);
			}
			else
			{
				str = skill.next_desc;
				old_value = "[number1]";
				if (skillId == SKILL_TECHNIQUE_OF_BUSINESS || skillId == SKILL_SHIPBUILDING_TECHNOLOGY || skillId == SKILL_HULL_TRACTION)
				{
					new_value = StringUtils::format("%.0f", number2);
				}
				else
				{
					new_value = StringUtils::format("%.2f", number2);
				}
				repalce_all_ditinct(str, old_value, new_value);
				t_next_change->setString(str);
				t_next_change->setPositionX(t_next_change_title->getPositionX() + t_next_change_title->getBoundingBox().size.width);
			}
			break;
		case SKILL_TYPE_COMPANION_NORMAL:
			skill = SINGLE_SHOP->getCompanionNormalSkillInfo()[skillId];
			image_head->loadTexture(getCompanionIconPath(iconId, false));
			i_skill->loadTextureNormal(getSkillIconPath(skillId, skill_type));
			t_skillName->setString(skill.name);
			if (needShow)
			{
				t_skill_lv->setString(StringUtils::format("%d/%d", skillLv, skill.max_level));
			}	
			//主动
			if (skill.ap_type == 1)
			{
				t_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ACTIVE"]);
			}
			else
			{
				t_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_PASSIVITY"]);
			}
			image_lv->setVisible(false);
			p_content->setPositionY(p_content->getPositionY() + 30);
	
			switch (skillId)
			{
			case SKILL_COMPANION_NORMAL_GUN_EXPERT:
			case SKILL_COMPANION_NORMAL_RAM_EXPERT:
			case SKILL_COMPANION_NORMAL_WEAKNESS_ATTACK:
			case SKILL_COMPANION_NORMAL_ARMOR_DEFENSE:
			case SKILL_COMPANION_NORMAL_BEST_SELLOR:
			case SKILL_COMPANION_NORMAL_COOK_SKILL:
			case SKILL_COMPANION_NORMAL_TRANSACTION_REPUTAION:
			case SKILL_COMPANION_NORMAL_MISSION_EXPERT:
			case SKILL_COMPANION_NORMAL_SECRETARY:
			case SKILL_COMPANION_NORMAL_ACCURACY:
			case SKILL_COMPANION_NORMAL_SAIL_OPERATION:
			case SKILL_COMPANION_NORMAL_OUTLOOK:
			case SKILL_COMPANION_NORMAL_WAREHOUSING:
			case SKILL_COMPANION_NORMAL_LOGISTICS:
				number = skill.effect_per_lv*skillLv / SKILL_DER_MIN;
				number2 = skill.effect_per_lv*(skillLv + 1) / SKILL_DER_MIN;
				break;
			case SKILL_COMPANION_NORMAL_HAMMER_SHOOT:
			case SKILL_COMPANION_NORMAL_UNBEATABLE_WILL:
			case SKILL_COMPANION_NORMAL_ROAR_OF_VICTORY:
				number = skill.rate_per_lv*skillLv;
				number2 = skill.rate_per_lv*(skillLv + 1);
				break;
			default:
				number = skill.effect_per_lv*skillLv / SKILL_DER_MIN;
				number2 = skill.effect_per_lv*(skillLv + 1) / SKILL_DER_MIN;
				break;
			}
			str = skill.next_desc;
			old_value = "[number1]";
			new_value = StringUtils::format("%.2f", number);
			repalce_all_ditinct(str, old_value, new_value);
			t_change->setString(str);
			t_change->setPositionX(t_change_title->getPositionX() + t_change_title->getBoundingBox().size.width);
			str = skill.desc;
			t_content->setString(str);
			str = skill.next_desc;
			new_value = StringUtils::format("%.2f", number2);
			repalce_all_ditinct(str, old_value, new_value);
			t_next_change->setString(str);
			t_next_change->setPositionX(t_next_change_title->getPositionX() + t_next_change_title->getBoundingBox().size.width);
			if (skillLv >= skill.max_level)
			{
				t_next_change->setVisible(false);
				t_next_change_title->setVisible(false);
				image_div_2->setVisible(false);
			}
			break;
		case SKILL_TYPE_PARTNER_SPECIAL:
			specialies_skill = SINGLE_SHOP->getCompanionSpecialiesSkillInfo()[skillId];
			image_head->loadTexture(getCompanionIconPath(iconId, false));
			i_skill->loadTextureNormal(getSkillIconPath(skillId, skill_type));
			t_skillName->setString(specialies_skill.name);
			t_skill_lv->setVisible(false);
			t_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMMON_SKILL_PARTNER_SPECIAL"]);
			t_require_lv->setString(StringUtils::format("Lv. %d", specialies_skill.require_level));
			t_content->setPositionY(t_content->getPositionY() + 30);
			t_content->setString(specialies_skill.desc);
			t_content->setContentSize(Size(680, 250));
			t_change_title->setVisible(false);
			t_change->setVisible(false);
			t_next_change_title->setVisible(false);
			t_next_change->setVisible(false);
			image_div_2->setVisible(false);
			break;
		default:
			break;
		}
	}
}


void UICommon::flushFriendDetail(GetUserInfoByIdResult *result,bool isHideButton)
{
	openView(COMMOM_COCOS_RES[C_VIEW_FRIEND_DETAIL_CSB]);
	auto firendDetail = getViewRoot(COMMOM_COCOS_RES[C_VIEW_FRIEND_DETAIL_CSB]);
	firendDetail->setCameraMask(_cameraMask, true);
	m_curuserInfo = result;
	if (firendDetail)
	{
		auto i_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(firendDetail,"image_head"));
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(firendDetail,"label_lv"));
		auto t_r_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(firendDetail,"label_r_lv"));
		auto i_flag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(firendDetail,"image_flag"));
		auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(firendDetail,"label_player_name"));
		auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(firendDetail,"label_bio_content"));
		auto p_guild_right = dynamic_cast<Widget*>(Helper::seekWidgetByName(firendDetail,"panel_guild_right"));
		auto p_guild_no = dynamic_cast<Widget*>(Helper::seekWidgetByName(firendDetail,"panel_guild_no"));
		auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(firendDetail,"label_played_time"));
	
		i_head->ignoreContentAdaptWithSize(false);
		i_head->loadTexture(getPlayerIconPath(result->icon));
		t_lv->setString(StringUtils::format("%d",result->level));
//		t_r_lv->enableOutline(Color4B::BLACK,OUTLINE_MIN);
		t_r_lv->setString(StringUtils::format("%lld",result->fame));
		i_flag->ignoreContentAdaptWithSize(false);
		i_flag->loadTexture(getCountryIconPath(result->nation));
		i_flag->setTouchEnabled(true);
		i_flag->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
		i_flag->setTag(result->nation + IMAGE_ICON_CLOCK+ IMAGE_INDEX2);
		t_name->enableOutline(Color4B::BLACK,OUTLINE_MAX);
		t_name->setString(result->heroname);
		if (result->bio)
		{
			if (strcmp(result->bio, "") != 0)
			{
				t_content->setString(result->bio);
			}
		}
		if (result->guildicon == 0)
		{
			p_guild_right->setVisible(false);
			p_guild_no->setVisible(true);
			auto t_na = p_guild_no->getChildByName<Text*>("label_guild_name");
			t_na->setAnchorPoint(Vec2(1, 0.5));
			auto b_invite = p_guild_no->getChildByName<Button*>("button_invite");
			if (result->caninvite)
			{
				b_invite->setTag(result->usercid);
				b_invite->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func,this));
				b_invite->setVisible(true);
			}else
			{
				b_invite->setVisible(false);
				t_na->setPositionX(b_invite->getPositionX() + b_invite->getContentSize().width / 2);
			}
			t_na->setTextHorizontalAlignment(TextHAlignment::RIGHT);
		}else
		{
			p_guild_right->setVisible(true);
			p_guild_no->setVisible(false);
			auto i_guild = p_guild_right->getChildByName<ImageView*>("image_guild_icon");
			auto t_guild_name = p_guild_right->getChildByName<Text*>("label_guild_name");
			i_guild->ignoreContentAdaptWithSize(false);
			i_guild->loadTexture(getGuildIconPath(result->guildicon));
			t_guild_name->setString(result->guildname);			
//chengyuan++
			 
			t_guild_name->setTextHorizontalAlignment(TextHAlignment::RIGHT);
			i_guild->setPositionX(t_guild_name->getPositionX() - t_guild_name->getContentSize().width - i_guild->getContentSize().width / 2);

		}
		t_time->setString(StringUtils::format("%.1f %s",result->onlinesecs/3600.0,SINGLE_SHOP->getTipsInfo()["TIP_HOUR"].data()));
	
		if (isHideButton)
		{
			auto p_friend = firendDetail->getChildByName<Widget*>("panel_btn_friend");
			auto p_block = firendDetail->getChildByName<Widget*>("panel_btn_block");
			auto p_stranger = firendDetail->getChildByName<Widget*>("panel_btn_stranger");
			p_friend->setVisible(false);
			p_block->setVisible(false);
			p_stranger->setVisible(false);
		}else if (result->relationstatus == 1)
		{
			auto p_friend = firendDetail->getChildByName<Widget*>("panel_btn_friend");
			p_friend->setTag(result->usercid);
			p_friend->setVisible(true);
		}else if (result->relationstatus == 2)
		{
			auto p_block = firendDetail->getChildByName<Widget*>("panel_btn_block");
			p_block->setTag(result->usercid);
			p_block->setVisible(true);
		}else
		{
			auto p_stranger = firendDetail->getChildByName<Widget*>("panel_btn_stranger");
			p_stranger->setTag(result->usercid);
			p_stranger->setVisible(true);
			if (result->relationstatus == 0)
			{
				auto b_add = p_stranger->getChildByName<Button*>("button_add_friend");
				b_add->setTag(result->relationstatus);
				b_add->setBright(false);
				auto i_add = b_add->getChildByName<ImageView*>("image_add");
				setGLProgramState(i_add,true);
			}
		}
	}
}

void UICommon::flushPlayerLevelUp()
{
	UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(MAIN_CITY_NEW_POINT).c_str(), true);
	UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(SKILL_NEW_POINT).c_str(), true);
	UserDefault::getInstance()->flush();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_PALYER_UPGRAD_23);
	openView(COMMOM_COCOS_RES[C_VIEW_LEVEL_UP_CSB]);
	auto level_up =getViewRoot(COMMOM_COCOS_RES[C_VIEW_LEVEL_UP_CSB]);
	level_up->setCameraMask(_cameraMask,true);
	level_up->setLocalZOrder(50);
	level_up->setPosition(STARTPOS);
	if (level_up)
	{
		level_up->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func,this));
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(level_up,"label_level"));
		t_lv->setString(StringUtils::format("%d",SINGLE_HERO->m_iLevel));
		t_lv->enableOutline(Color4B::BLACK,OUTLINE_MIN);

		auto node1 = level_up->getChildByName<ImageView*>("image_node1");
		auto node2 = level_up->getChildByName<ImageView*>("image_node2");
		node1->setVisible(true);
		node2->setVisible(false);
		node1->setScale(0.1);
		node1->runAction(ScaleTo::create(0.3,1));
		auto i_light = node1->getChildByName<ImageView*>("image_light");
		auto image_body_light = node1->getChildByName<ImageView*>("image_body_light");
		i_light->runAction(RotateBy::create(2,180));
		i_light->runAction(Sequence::createWithTwoActions(DelayTime::create(1.7),FadeOut::create(0.3)));
		image_body_light->runAction(RepeatForever::create(Sequence::createWithTwoActions(FadeTo::create(0.5,255),FadeTo::create(0.5,153))));
		Vector<Node*> arrayRootChildren = node1->getChildren();
		for (auto obj: arrayRootChildren)
		{
			Widget* child = (Widget*)(obj);
			child->setOpacity(0);
			child->runAction(FadeIn::create(0.2));
		}
		node2->runAction(Sequence::createWithTwoActions(DelayTime::create(0.7),Show::create()));
		Vector<Node*> arrayRootChildren1 = node2->getChildren();
		for (auto obj: arrayRootChildren1)
		{
			Widget* child = (Widget*)(obj);
			child->setOpacity(0);
			child->runAction(Sequence::createWithTwoActions(DelayTime::create(0.7),FadeIn::create(0.3)));
		}

		auto level_up = node2->getChildByName<ImageView*>("image_levelup");
		level_up->ignoreContentAdaptWithSize(true);
		level_up->loadTexture(LEVEL_UP[SINGLE_SHOP->L_TYPE]);

		//升级时刷新界面
		auto currentScene = Director::getInstance()->getRunningScene();
		auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
		if (mainlayer)
		{
			mainlayer->flushExpAndRepLv();
		}
		auto mapsLayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
	}
}

void UICommon::flushCaptainLevelUp(int n_captains, CaptainInfo **captains)
{
	auto winSize = Director::getInstance()->getWinSize();
	if (!m_pCaptainAddExp) return;
	Widget * captain_exp = nullptr;
	if (getViewRoot(COMMOM_COCOS_RES[C_VIEW_CAPTAIN_ADDEXP_CSB]) != nullptr)
	{
		m_pCaptainAddExp = m_pCaptainAddExp;
		m_pCaptainAddExpBattle = nullptr;
		captain_exp = m_pCaptainAddExp;
	}
	else
	{
		m_pCaptainAddExpBattle = m_pCaptainAddExp;
		m_pCaptainAddExp = nullptr;
		captain_exp = m_pCaptainAddExpBattle;
	}
	captain_exp->setCameraMask(_cameraMask,true);//设置摄像机
	captain_exp->setLocalZOrder(50);
	captain_exp->setVisible(true);
	captain_exp->setPosition(STARTPOS);
	m_bitemsaddCompleted = false;
	std::vector<Widget*>v_levelUpItems;
	std::vector<Widget*>v_normalItems;
	std::vector<Widget*>v_levelandSkillupItems;
	if (m_pCaptainAddExp != nullptr)
	{
		auto l_list = captain_exp->getChildByName<ListView*>("listview_content");
		m_captainlist = l_list;
		l_list->setVisible(true);
		m_listitems.clear();
		auto p_captain = captain_exp->getChildByName<Layout*>("panel_captain_2");
		auto i_pilldown = captain_exp->getChildByName<ImageView*>("image_pulldown");
		auto t_title = captain_exp->getChildByName<Text*>("label_title");
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMMON_SAILING_EXCAPTIAN_TITLE"]);
		i_pilldown->setVisible(false);
		for (int i = 0; i < n_captains; i++)
		{
			auto c_captain = p_captain->clone();
			auto i_caphead = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_captain_head"));
			auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(c_captain, "label_lv"));
			auto i_levelup = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_lvup"));
			auto t_capname = dynamic_cast<Text*>(Helper::seekWidgetByName(c_captain, "label_captain_name_2"));
			auto i_position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_num_2"));
			auto t_exp = dynamic_cast<Text*>(Helper::seekWidgetByName(c_captain, "label_exp"));
			auto t_expNum = dynamic_cast<Text*>(Helper::seekWidgetByName(c_captain, "label_exp_num"));
			auto i_levelup_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_levelup_2"));
			auto b_skill_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(c_captain, "button_equip_bg_1"));
			auto b_skill_2 = dynamic_cast<Button*>(Helper::seekWidgetByName(c_captain, "button_equip_bg_2"));
			auto i_light = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_light"));
			auto i_light_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_light_1"));
			i_light->setVisible(false);
			i_light_1->setVisible(false);
			i_levelup_1->setVisible(false);
			b_skill_1->setVisible(false);
			b_skill_2->setVisible(false);

			i_caphead->loadTexture(getCompanionIconPath(captains[i]->captain->protoid, captains[i]->captain->iscaptain));
			t_capname->setString(getCompanionName(captains[i]->captain->protoid, captains[i]->captain->iscaptain));
			t_exp->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_FINDNEWCITY_REWARD_EXP"]);
			t_expNum->setString(StringUtils::format("+%d", captains[i]->lootexp));
			i_position->loadTexture(getPositionIconPath(captains[i]->position));
			c_captain->setCameraMask(_cameraMask, true);
			t_lv->setString(StringUtils::format("Lv.%d", captains[i]->captain->level));
			Sprite * spritelight = Sprite::create();
			spritelight->setContentSize(Size(274, 274));
			spritelight->setAnchorPoint(Vec2(0.5, 0.5));
			spritelight->setPosition(i_light->getPosition() + Vec2(10, -15));
			spritelight->setCameraMask(4, true);
			c_captain->addChild(spritelight, -1);

			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon0.plist");
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon1.plist");
			//光芒四射
			Vector<SpriteFrame *> frames;
			for (int j = 0; j <= 27; j++)
			{
				std::string name = StringUtils::format("shinelight/eff_shinelight_%02d.png", j);
				SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
				frames.pushBack(frame);
			}
			Animation* animation_shiplight = Animation::createWithSpriteFrames(frames, 2.0 / 28, 1);
			auto reaction = RepeatForever::create(Animate::create(animation_shiplight));
			spritelight->runAction(reaction);
			c_captain->retain();
			if (captains[i]->newlevel)
			{
				spritelight->setVisible(true);
				i_levelup->setVisible(true);
			}
			else
			{
				spritelight->setVisible(false);
				i_levelup->setVisible(false);
			}
			SkillDefine * normalSkilllevelup = nullptr;
			SkillDefine * specialSkillunlock = nullptr;
			//解锁的特殊技能
			for (int j = 0; j < captains[i]->captain->n_specskills; j++)
			{
				if ( captains[i]->captain->skills[j]->level_up > 0)
				{
					specialSkillunlock = captains[i]->captain->specskills[j];
					break;
				}
			}
			//升级的普通技能
			for (int k = 0; k < captains[i]->captain->n_skills; k++)
			{
				if (captains[i]->captain->skills[k]->level_up > 0)
				{
					normalSkilllevelup = captains[i]->captain->skills[k];
					break;
				}
			}
			bool skill_level_up = false;
			if (specialSkillunlock != nullptr)
			{
				int special_id = specialSkillunlock->id;
				b_skill_1->loadTextureNormal(getSkillIconPath(special_id, SKILL_TYPE_PARTNER_SPECIAL));
				auto i_levelskillup = b_skill_1->getChildByName("image_lvup");
				b_skill_1->setVisible(true);
				i_light_1->setVisible(true);
				i_light_1->setOpacity(0);
				i_levelskillup->setVisible(false);
				auto seq = Sequence::createWithTwoActions(FadeIn::create(0.5), CallFunc::create([=]{
					i_light_1->runAction(RepeatForever::create(RotateBy::create(3, 180)));
				}));
				i_light_1->runAction(seq);
				skill_level_up = true;
			}
			if (normalSkilllevelup != nullptr)
			{
				int special_id = normalSkilllevelup->id;
				b_skill_2->loadTextureNormal(getSkillIconPath(special_id, SKILL_TYPE_COMPANION_NORMAL));
				auto i_levelskillup = b_skill_2->getChildByName("image_lvup");
				auto t_skill_level = b_skill_2->getChildByName<Text*>("text_item_skill_lv");
				t_skill_level->setString(StringUtils::format("Lv.%d", normalSkilllevelup->level));
				b_skill_2->setVisible(true);
				i_levelskillup->setOpacity(0);
				i_levelskillup->runAction(FadeIn::create(0.5));
				skill_level_up = true;
			}

			if (!skill_level_up && captains[i]->newlevel)
			{
				i_levelup_1->setVisible(true);
				i_levelup_1->setOpacity(0);
				i_levelup_1->runAction(FadeIn::create(1.5));
				i_levelup_1->setPositionY(t_expNum->getPositionY());
				v_levelUpItems.push_back(c_captain);
			}
			else if (skill_level_up && captains[i]->newlevel)
			{
				i_levelup_1->setVisible(true);
				i_levelup_1->setOpacity(0);
				i_levelup_1->runAction(FadeIn::create(1.5));
				v_levelandSkillupItems.push_back(c_captain);
			}else
			{
				v_normalItems.push_back(c_captain);
			}		
		}
    }
	
	if (m_pCaptainAddExpBattle != nullptr)
	{
		auto l_list = m_pCaptainAddExpBattle->getChildByName<ListView*>("listview_content");
		m_captainlist = l_list;
		l_list->setVisible(true);
		m_listitems.clear();
		auto p_captain = captain_exp->getChildByName<Layout*>("panel_captain_1");
		auto i_pilldown = captain_exp->getChildByName<ImageView*>("image_pulldown");
		auto t_title = captain_exp->getChildByName<Text*>("label_title");
		i_pilldown->setVisible(false);
		for (int i = 0; i < n_captains; i++)
		{
			auto c_captain = p_captain->clone();
			auto i_caphead = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_captain_head"));
			auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(c_captain, "label_lv"));
			auto i_levelup = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_lvup"));
			auto t_capname = dynamic_cast<Text*>(Helper::seekWidgetByName(c_captain, "label_captain_name_2"));
			auto i_position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_num_2"));
			auto t_exp = dynamic_cast<Text*>(Helper::seekWidgetByName(c_captain, "label_exp"));
			auto t_expNum = dynamic_cast<Text*>(Helper::seekWidgetByName(c_captain, "label_exp_num"));
			auto i_levelup_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_levelup_2"));
			auto b_skill_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(c_captain, "button_equip_bg_1"));
			auto b_skill_2 = dynamic_cast<Button*>(Helper::seekWidgetByName(c_captain, "button_equip_bg_2"));
			auto i_light = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_light_1"));
			auto i_light_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_light_2"));
			auto i_light_2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_captain, "image_light_3"));
			i_light->setVisible(false);
			i_light_1->setVisible(false);
			i_levelup_1->setVisible(false);
			b_skill_1->setVisible(false);
			b_skill_2->setVisible(false);

			i_caphead->loadTexture(getCompanionIconPath(captains[i]->captain->protoid, captains[i]->captain->iscaptain));
			t_capname->setString(getCompanionName(captains[i]->captain->protoid, captains[i]->captain->iscaptain));
			t_exp->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_FINDNEWCITY_REWARD_EXP"]);
			t_expNum->setString(StringUtils::format("+%d", captains[i]->lootexp));
			i_position->loadTexture(getPositionIconPath(captains[i]->position));
			c_captain->setCameraMask(_cameraMask, true);
			t_lv->setString(StringUtils::format("%d", captains[i]->captain->level));
			Sprite * spritelight = Sprite::create();
			spritelight->setContentSize(Size(274, 274));
			spritelight->setAnchorPoint(Vec2(0.5, 0.5));
			spritelight->setPosition(i_light_1->getPosition() + Vec2(10, -15));
			spritelight->setCameraMask(4, true);
			c_captain->addChild(spritelight, -1);
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon0.plist");
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon1.plist");
			//光芒四射
			Vector<SpriteFrame *> frames;
			for (int j = 0; j <= 27; j++)
			{
				std::string name = StringUtils::format("shinelight/eff_shinelight_%02d.png", j);
				SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
				frames.pushBack(frame);
			}
			Animation* animation_shiplight = Animation::createWithSpriteFrames(frames, 2.0 / 28, 1);
			auto reaction = RepeatForever::create(Animate::create(animation_shiplight));
			spritelight->runAction(reaction);
			c_captain->retain();
			if (captains[i]->newlevel)
			{
				spritelight->setVisible(true);
				i_levelup->setVisible(true);
			}
			else
			{
				spritelight->setVisible(false);
				i_levelup->setVisible(false);
			}
			SkillDefine * normalSkilllevelup = nullptr;
			SkillDefine * specialSkillunlock = nullptr;
			//解锁的特殊技能
			for (int j = 0; j < captains[i]->captain->n_specskills; j++)
			{
				if (captains[i]->captain->skills[j]->level_up > 0)
				{
					specialSkillunlock = captains[i]->captain->specskills[j];
					break;
				}
			}
			//升级的普通技能
			for (int k = 0; k < captains[i]->captain->n_skills; k++)
			{
				if (captains[i]->captain->skills[k]->level_up > 0)
				{
					normalSkilllevelup = captains[i]->captain->skills[k];
					break;
				}
			}
			bool skill_level_up = false;
			if (specialSkillunlock != nullptr)
			{
				int special_id = specialSkillunlock->id;
				b_skill_1->loadTextureNormal(getSkillIconPath(special_id, SKILL_TYPE_PARTNER_SPECIAL));
				auto i_levelskillup = b_skill_1->getChildByName("image_lvup");
				b_skill_1->setVisible(true);
				i_light_2->setVisible(true);
				i_light_2->setOpacity(0);
				i_levelskillup->setVisible(false);
				auto seq = Sequence::createWithTwoActions(FadeIn::create(0.5), CallFunc::create([=]{
					i_light_2->runAction(RepeatForever::create(RotateBy::create(3, 180)));
				}));
				i_light_2->runAction(seq);
				i_light->setVisible(true);
				i_light->setOpacity(0);
				i_light->runAction(Sequence::createWithTwoActions(FadeIn::create(0.5), CallFunc::create([=]{
					i_light->runAction(RepeatForever::create(RotateBy::create(3, 180)));
				})));
				skill_level_up = true;
			}
			if (normalSkilllevelup != nullptr)
			{
				int special_id = normalSkilllevelup->id;
				b_skill_2->loadTextureNormal(getSkillIconPath(special_id, SKILL_TYPE_COMPANION_NORMAL));
				auto i_levelskillup = b_skill_2->getChildByName("image_lvup");
				auto t_skill_level = b_skill_2->getChildByName<Text*>("text_item_skill_lv");
				t_skill_level->setString(StringUtils::format("%d", normalSkilllevelup->level));
				b_skill_2->setVisible(true);
				i_levelskillup->setOpacity(0);
				i_levelskillup->runAction(FadeIn::create(0.5));
				skill_level_up = true;
			}

			if (!skill_level_up && captains[i]->newlevel)
			{
				i_levelup_1->setVisible(true);
				i_levelup_1->setOpacity(0);
				i_levelup_1->runAction(FadeIn::create(1.5));
				i_levelup_1->setPositionY(t_expNum->getPositionY());
				v_levelUpItems.push_back(c_captain);
			}
			else if (skill_level_up && captains[i]->newlevel)
			{
				i_levelup_1->setVisible(true);
				i_levelup_1->setOpacity(0);
				i_levelup_1->runAction(FadeIn::create(1.5));
				v_levelandSkillupItems.push_back(c_captain);
			}
			else
			{
				v_normalItems.push_back(c_captain);
			}
		}
	}
	//排序 升级的船长优先级高
	for (int i = 0; i < v_levelandSkillupItems.size();i++)
	{
		m_listitems.push_back(v_levelandSkillupItems.at(i));
	}
	for (int i = 0; i < v_levelUpItems.size(); i++)
	{
		m_listitems.push_back(v_levelUpItems.at(i));
	}
	for (int i = 0; i < v_normalItems.size(); i++)
	{
		m_listitems.push_back(v_normalItems.at(i));
	}
	schedule(schedule_selector(UICommon::addItem), 0.5);
	captain_exp->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func,this));

}
void UICommon::flushWarning(WARNING_LV wargingLv)
{
	openView(COMMOM_COCOS_RES[C_VIEW_WARNING_CSB]);
	auto warning =getViewRoot(COMMOM_COCOS_RES[C_VIEW_WARNING_CSB]);
	warning->setCameraMask(_cameraMask, true);
	if (warning)
	{
		std::string name;
		std::string content;
		std::string path;
		switch (wargingLv)
		{
		case SUPPLY_WARNING:
			name = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_LACK_SUPPLY_TITLE"];
			content = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_LACK_SUPPLY_CONTENT"];
			path = "cocosstudio/login_ui/common/hints_supplies_1.png";
			break;
		case SUPPLY_URGENT_WARNING:
			name = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_LACK_URGNET_SUPPLY_TITLE"];
			content = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_LACK_URGNET_SUPPLY_CONTENT"];
			path = "cocosstudio/login_ui/common/hints_supplies_2.png";
			break;
		case SAILOR_WARNING:
			name = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_LACK_SAILOR_TITLE"];
			content = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_LACK_SAILOR_CONTENT"];
			path = "cocosstudio/login_ui/common/hints_sailor_1.png";
			break;
		case SAILOR_URGENT_WARNING:
			name = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_LACK_URGNET_SAILOR_TITLE"];
			content = SINGLE_SHOP->getTipsInfo()["TIP_SAILING_LACK_URGNET_SAILOR_CONTENT"];
			path = "cocosstudio/login_ui/common/hints_sailor_2.png";
			break;
		case SAILOR_NOT_ENOUGH_EFFECT_SPEED:
		name = SINGLE_SHOP->getTipsInfo()["TIP_LACK_URGNET_SAILOR_EFFECT_SAIL_SPEED_TITLE"];
		content = SINGLE_SHOP->getTipsInfo()["TIP_LACK_URGNET_SAILOR_EFFECT_SAIL_SPEED_CONTENT"];
		 path = "cocosstudio/login_ui/common/hints_sailor_1.png";
		break;
		default:
			break;
		}
		auto t_title = warning->getChildByName<Text*>("label_goods_name");
		auto t_content = warning->getChildByName<Text*>("label_price_num");
		auto b_warning = warning->getChildByName<Button*>("button_warning_1");
		t_title->setString(name);
		t_content->setString(content);
		b_warning->loadTextureNormal(path);
	}
}

void UICommon::flushPrestigeLevelUp()
{
	
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_PALYER_UPGRAD_23);
	openView(COMMOM_COCOS_RES[C_VIEW_R_LEVEL_UP_CSB]);
	auto prestige =getViewRoot(COMMOM_COCOS_RES[C_VIEW_R_LEVEL_UP_CSB]);
	prestige->setCameraMask(_cameraMask,true);
	//prestige->setLocalZOrder(50);
	//prestige->setPosition(STARTPOS);
	if (prestige)
	{
		prestige->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func,this));
		auto node1 = prestige->getChildByName<Widget*>("image_node1");
		auto node2 = prestige->getChildByName<Widget*>("image_node2");
		node1->setVisible(true);
		node2->setVisible(false);
		node1->setScale(0.1);
		node1->runAction(ScaleTo::create(0.5,1));
		auto image_light = node1->getChildByName<ImageView*>("image_light");
		auto image_light1 = node1->getChildByName<ImageView*>("image_light_1");
		auto button_rlvok = node2->getChildByName<Button*>("button_rlv_ok");
		button_rlvok->setVisible(false);
		image_light->runAction(RotateBy::create(2,180));
		image_light1->runAction(RepeatForever::create(RotateBy::create(2,90)));
		image_light->runAction(Sequence::createWithTwoActions(DelayTime::create(1.7),FadeOut::create(0.3)));
		image_light1->runAction(RepeatForever::create(Sequence::createWithTwoActions(FadeTo::create(0.5,255),FadeTo::create(0.5,153))));
		Vector<Node*> arrayRootChildren = node1->getChildren();
		for (auto obj: arrayRootChildren)
		{
			Widget* child = (Widget*)(obj);
			child->setOpacity(0);
			child->runAction(FadeIn::create(0.3));
		}
		node2->runAction(Sequence::createWithTwoActions(DelayTime::create(0.7),Show::create()));
		Vector<Node*> arrayRootChildren1 = node2->getChildren();
		for (auto obj: arrayRootChildren1)
		{
			Widget* child = (Widget*)(obj);
			child->setOpacity(0);
			child->runAction(Sequence::createWithTwoActions(DelayTime::create(0.7),FadeIn::create(0.3)));
		}

		auto t_lv = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(prestige,"atlasLabel_lv"));
		t_lv->setString(StringUtils::format("%d",SINGLE_HERO->m_iPrestigeLv));

		auto level_up = node2->getChildByName<ImageView*>("image_levelup");
		level_up->ignoreContentAdaptWithSize(true);
		level_up->loadTexture(LEVEL_UP[SINGLE_SHOP->L_TYPE]);
	}
}

void UICommon::flushRank(GetLeaderboardResult *result)
{
	
}

void UICommon::showNumpad(Text* inputNum)
{
	openView(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	auto numpad =getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	if (numpad)
	{
		numpad->setPosition(STARTPOS);
		auto num_root = numpad->getChildByName<Widget*>("panel_numpad");
		num_root->setPosition(ENDPOS4);
		num_root->runAction(MoveTo::create(0.5,STARTPOS));
		num_root->getChildByName<Button*>("button_yes")->setTouchEnabled(true);
		numpad->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func,this));
		numpad->setTouchEnabled(true);
		m_inputNum = inputNum;
		for (int i = 0; i < 10; i++)
		{
			auto b_num = num_root->getChildByName<Button*>(StringUtils::format("button_%d",i));
			b_num->addTouchEventListener(CC_CALLBACK_2(UICommon::buttonNumpadEvent,this));
		}
	}
}

void UICommon::closeNumpad()
{
	auto numpad =getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	if (numpad)
	{
		numpad->setTouchEnabled(false);
		auto num_root = numpad->getChildByName<Widget*>("panel_numpad");
		num_root->getChildByName<Button*>("button_yes")->setTouchEnabled(false);
		num_root->runAction(MoveTo::create(0.5,ENDPOS4));
		numpad->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5),Place::create(ENDPOS)));
	}
}

void UICommon::buttonNumpadEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto button = dynamic_cast<Button*>(pSender);
		int tag = button->getTag();
		std::string str = m_inputNum->getString();
		if (str.size() < 2 && str == "0")
		{
			str.erase(str.end()-1);
		}
		str += StringUtils::format("%d",tag);
		m_inputNum->setString(str);
	}
}

void UICommon::getNotZeroFromString(std::string src,int& value_1,int& index_1,int& value_2,int& index_2)
{

	if(src.empty())
	{
		return;
	}
	int prePos = 0;
	int index = 0;
	for (size_t i = 0; i != std::string::npos;i++)
	{	
		prePos = i;

		i = src.find(",",prePos);
		int breakFlag = 0;
		if (i == std::string::npos )
		{
			breakFlag = 1;
		}
		int id = atoi(src.substr(prePos,i - prePos).c_str());
		if (id != 0)
		{
			if (index_1 == -1)
			{
				value_1 = id;
				index_1 = index;
				continue;
			}
			if (index_2 == -1)
			{
				value_2 = id;
				index_2 = index;
				continue;
			}
		}
		index++;
		if(breakFlag)
			break;
	}
}
void UICommon::flushPlayerAddExpOrFrame(int64_t resultExp,int64_t resultFrame,int64_t addExp,int64_t addFrame)
{
	openView(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);
	auto view=getViewRoot(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);
	view->setTouchEnabled(false);
	this->setCameraMask(_cameraMask, true);
	auto imageHeadPlayer=view->getChildByName("image_head_player_bg");
	auto imageHead=imageHeadPlayer->getChildByName<ImageView*>("image_head");
	//头像
	imageHead->ignoreContentAdaptWithSize(false);
	imageHead->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));
	//经验条，声望条
	auto Panel_exp = imageHeadPlayer->getChildByName<Widget*>("panel_exp");
	auto Panel_rep=imageHeadPlayer->getChildByName<Widget*>("panel_rep");
	auto progressbarExp=Panel_exp->getChildByName<LoadingBar*>("progressbar_exp");
	auto progressbarRep=Panel_rep->getChildByName<LoadingBar*>("progressbar_reputation");
	SINGLE_HERO->m_iLevel = EXP_NUM_TO_LEVEL(resultExp);
	SINGLE_HERO->m_iPrestigeLv = FAME_NUM_TO_LEVEL(resultFrame);
	float temp_exp = 0;
	if (SINGLE_HERO->m_iLevel < LEVEL_MAX_NUM)
	{
		temp_exp = (resultExp - LEVEL_TO_EXP_NUM(SINGLE_HERO->m_iLevel))*1.0/(LEVEL_TO_EXP_NUM(SINGLE_HERO->m_iLevel+1)-LEVEL_TO_EXP_NUM(SINGLE_HERO->m_iLevel));
	}
	float temp_rep = 0;
	if (SINGLE_HERO->m_iPrestigeLv < LEVEL_MAX_NUM)
	{
		temp_rep = (resultFrame- LEVEL_TO_FAME_NUM(SINGLE_HERO->m_iPrestigeLv))*1.0/(LEVEL_TO_FAME_NUM(SINGLE_HERO->m_iPrestigeLv+1)-LEVEL_TO_FAME_NUM(SINGLE_HERO->m_iPrestigeLv));
	}
	Panel_exp->setContentSize(Size(Panel_exp->getSize().width,120*temp_exp));
	Panel_rep->setContentSize(Size(Panel_rep->getSize().width,120*temp_rep));
	//LV
	auto image_head_lv=imageHeadPlayer->getChildByName("image_head_lv");
	auto label_lv=image_head_lv->getChildByName<Text*>("label_lv");
	auto label_r_lv = image_head_lv->getChildByName<Text*>("label_lv_r");
	label_lv->setString(StringUtils::format("%d",SINGLE_HERO->m_iLevel));
	label_r_lv->setString(StringUtils::format("%d", SINGLE_HERO->m_iPrestigeLv));
	//增加的经验和声望
	auto p_exp = view->getChildByName<Widget*>("panel_exp");
	auto p_rep = view->getChildByName<Widget*>("panel_r");
	auto t_exp = p_exp->getChildByName<Widget*>("label_exp");
	auto i_rep = p_rep->getChildByName<Widget*>("image_r");
	auto label_exp_num = p_exp->getChildByName<Text*>("label_exp_num");
	label_exp_num->setString(StringUtils::format("+%lld",addExp));
	auto label_r_num = p_rep->getChildByName<Text*>("label_r_num");
	label_r_num->setString(StringUtils::format("+%lld",addFrame));
	t_exp->setPositionX(p_exp->getBoundingBox().size.width / 2 - label_exp_num->getBoundingBox().size.width / 2 - 10);
	label_exp_num->setPositionX(t_exp->getPositionX() + t_exp->getBoundingBox().size.width / 2 + 20);
	i_rep->setPositionX(p_rep->getBoundingBox().size.width / 2 - label_r_num->getBoundingBox().size.width / 2 - 10);
	label_r_num->setPositionX(i_rep->getPositionX() + i_rep->getBoundingBox().size.width / 2 + 20);
}
void UICommon::flushCostOnlyCoin(int64_t coin,std::string titleTest,std::string contentText,std::string costFor)
{
	openView(COMMOM_COCOS_RES[C_VIEW_HIRE_RESULT_CSB]);
	auto view=getViewRoot(COMMOM_COCOS_RES[C_VIEW_HIRE_RESULT_CSB]);
	auto tipInfos = SINGLE_SHOP->getTipsInfo();//读取tips文件
	std::string infoTitleTest = tipInfos[titleTest];
	std::string infoContentText = tipInfos[contentText];
	std::string infoCostFor= tipInfos[costFor];
	if (infoTitleTest=="")
	{
		infoTitleTest="infoTitleTest";
	}
	if (infoContentText=="")
	{
		infoContentText="infoContentText";
	}
	if (infoCostFor=="")
	{
		infoCostFor="infoCostFor";
	}
	auto titleContent=view->getChildByName<Text*>("label_result");
	auto mainContent=view->getChildByName<Text*>("label_content");
	auto costReason=dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_salary"));
	auto costNum=dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_cost_num"));
	costNum->ignoreContentAdaptWithSize(true);
	titleContent->setString(infoTitleTest);
	mainContent->setString(infoContentText);
	costReason->setString(infoCostFor);
	costNum->setString(numSegment(StringUtils::format("%lld",coin)));
	auto i_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_silver_1"));
	i_silver->setPositionX(costNum->getPositionX() - costNum->getBoundingBox().size.width - i_silver->getContentSize().width / 2 - 5);
	auto button_captain_yes=dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_captain_yes"));
	button_captain_yes->addTouchEventListener(CC_CALLBACK_2(UICommon::buttonYesEvent,this));

}
void UICommon::getCaptainAddexp(CAPTAIN_EXPUI flag)
{
	if (flag == CAPTAIN_EXPUI::CAPTAIN_EXPUI_NORMAL)
	{
		openView(COMMOM_COCOS_RES[C_VIEW_CAPTAIN_ADDEXP_CSB]);
		m_pCaptainAddExp = getViewRoot(COMMOM_COCOS_RES[C_VIEW_CAPTAIN_ADDEXP_CSB]);
	}
	else
	{
		openView(MAPUI_COCOS_RES[INDEX_UI_BATTLE_RESULT_M_CSB]);
		m_pCaptainAddExp= getViewRoot(MAPUI_COCOS_RES[INDEX_UI_BATTLE_RESULT_M_CSB]);
	}
}
void UICommon::flushPrestigeLevelDown()
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BATTLE_FAIL_28);//暂时还没有降级的音效
	openView(COMMOM_COCOS_RES[C_VIEW_R_LEVEL_DOWN_CSB]);
	auto prestige = getViewRoot(COMMOM_COCOS_RES[C_VIEW_R_LEVEL_DOWN_CSB]);
	prestige->setCameraMask(_cameraMask, true);
	if (prestige)
	{
		prestige->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func,this));
		auto buttonRlvdown_ok = dynamic_cast<Button*>(Helper::seekWidgetByName(prestige, "button_rlvdown_ok"));
		auto label_content = dynamic_cast<Text*>(Helper::seekWidgetByName(prestige, "label_content"));
		auto t_lv = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(prestige, "atlasLabel_lv"));
		buttonRlvdown_ok->setTitleText("OK");
		buttonRlvdown_ok->setVisible(false);
		t_lv->setString(StringUtils::format("%d", SINGLE_HERO->m_iPrestigeLv));
		auto image_down = dynamic_cast<ImageView*>(Helper::seekWidgetByName(prestige, "image_down"));
		image_down->runAction(RepeatForever::create(Blink::create(2, 4)));

		auto level_down = prestige->getChildByName<ImageView*>("image_leveldown");
		level_down->ignoreContentAdaptWithSize(true);
		level_down->loadTexture(LEVEL_DOWN[SINGLE_SHOP->L_TYPE]);
	}
}

void UICommon::getNotZeroFromString(std::string src, std::string& value1, std::string& value2, int& index1, int& index2)
{
	if (src.empty())
	{
		return;
	}
	int prePos = 0;
	int index = 0;
	for (size_t i = 0; i != std::string::npos; i++)
	{
		prePos = i;
		i = src.find(",", prePos);
		if (i)
		{
			index++;
		}
		int breakFlag = 0;
		if (i == std::string::npos)
		{
			breakFlag = 1;
		}
		int id = atoi(src.substr(prePos, i - prePos).c_str());
		if (id != 0)
		{
			if (index1 == -1)
			{
				value1 = src.substr(prePos, i - prePos).c_str();
				index1 = index;
			}else if (index1 != -1 && index2 == -1)
			{
				value2 = src.substr(prePos, i - prePos).c_str();
				index2 = index;
			}
		}
		if (breakFlag)
			break;
	}
}

void UICommon::flushEquipBrokenView(OutOfDurableItemDefine **brokenequips, size_t n_equipnum)
{
	if (m_brokennum == 0 && brokenequips)
	{
		m_brokenItems = new OutOfDurableItemDefine *[n_equipnum];
		for (int i = 0; i < n_equipnum;i++)
		{
			if (brokenequips[i]->durability == 0)
			{
				m_brokenItems[m_brokennum] = new OutOfDurableItemDefine;
				m_brokenItems[m_brokennum] = brokenequips[i];
				memcpy(m_brokenItems[m_brokennum], brokenequips[i], sizeof(OutOfDurableItemDefine));
				m_brokennum++;
			}
		}
	}
	else if (!brokenequips)
	{
		return;
	}
	if(m_brokennum <= 0)
		return;
	openView(COMMOM_COCOS_RES[C_VIEW_EQUIPMENT_BROKEN_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_EQUIPMENT_BROKEN_CSB]);
	auto viewBroken = view->getChildByName<Widget*>("Panel_broken");
	viewBroken->setScale(0);
	viewBroken->setCascadeOpacityEnabled(true);
	viewBroken->setOpacity(0);
	viewBroken->runAction(Spawn::createWithTwoActions(FadeIn::create(0.5), Sequence::create(ScaleTo::create(0.5, 1), nullptr)));
	this->setCameraMask(4, true);
	this->setSubNodeMaxCameraMask(4);
	int id = brokenequips[m_brokennum - 1]->iid;
	ITEM_RES item = SINGLE_SHOP->getItemData()[id];
	auto p_item = viewBroken->getChildByName<Widget*>("panel_item");
	auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_item, "image_item"));
	auto t_itemName = p_item->getChildByName<Text*>("label_items_name");
	auto t_durableNum = (Text*)Helper::seekWidgetByName(viewBroken, "label_ship_durable_num");
	auto i_shipPostion = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewBroken, "image_num"));
	std::string duraNum = StringUtils::format("%d/%d", brokenequips[m_brokennum - 1]->durability, item.max_durability);
	
	t_itemName->setText(item.name);
	t_durableNum->setString(duraNum);
	i_item->loadTexture(getItemIconPath(id));
	//setGLProgramState(i_item, true);
	i_shipPostion->loadTexture(getPositionIconPath(brokenequips[m_brokennum - 1]->position + 1));
	
	auto btn_close = viewBroken->getChildByName<Button*>("button_close");
	btn_close->setTouchEnabled(true);
	btn_close->addTouchEventListener(CC_CALLBACK_2(UICommon::equipBrokenTouchEvent,this));
	m_brokennum--;
}
void UICommon::equipBrokenTouchEvent(Ref * pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}

	std::string name = dynamic_cast<Button*>(pSender)->getName();
	if (isButton(button_close))
	{
		if (m_brokennum <= 0)
		{
			delete[] m_brokenItems;
			this->menuCall_func(pSender, TouchType);
		}
		else
		{
			closeView();
			flushEquipBrokenView(m_brokenItems);
		}
	}


}

void UICommon::flushBossDamageRank(GetAttackPirateRankInfoResult *result, bool isShowClose)
{
	openView(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RANKING_CSB]);
	auto view = getViewRoot(ACTIVITY_RES[ACTIVITY_PIRATES_ATTACK_RANKING_CSB]);
	auto panel_title = view->getChildByName<Widget*>("panel_title");
	
	if (isShowClose)
	{
		auto b_close = view->getChildByName<Button*>("button_close");
		b_close->setVisible(true);
		b_close->setName("button_boss_close");
		view->setPosition(Vec2(100,60));
		view->setTouchEnabled(true);
	}

	auto t_damageRanking = panel_title->getChildByName<Text*>("label_forces_relationship");
	auto t_rankingLv = panel_title->getChildByName<Text*>("label_forces_relationship_0");
	t_rankingLv->setString(StringUtils::format("(Lv. %d -- Lv. %d)", result->levelstart, result->levelend));

	t_damageRanking->setPositionX((panel_title->getBoundingBox().size.width - t_damageRanking->getBoundingBox().size.width -
	t_rankingLv->getBoundingBox().size.width - 20) / 2 + t_damageRanking->getBoundingBox().size.width/2);
	t_rankingLv->setPositionX(t_damageRanking->getBoundingBox().size.width / 2 + t_rankingLv->getBoundingBox().size.width / 2 + t_damageRanking->getPositionX() + 10);

	auto panel_list = view->getChildByName<Widget*>("panel_list");
	auto l_ranking = view->getChildByName<ListView*>("listview_ranking");
	l_ranking->removeAllChildrenWithCleanup(true);
	for (size_t i = 0; i < result->n_rankinfo; i++)
	{
		auto item = panel_list->clone();
		auto t_rank = item->getChildByName<Text*>("label_rank");
		auto i_trophy = item->getChildByName<ImageView*>("image_trophy");
		auto i_nation = item->getChildByName<ImageView*>("image_country");
		auto t_name = item->getChildByName<Text*>("label_name");
		auto t_lv = item->getChildByName<Text*>("label_lv");
		auto t_admage = item->getChildByName<Text*>("label_silver_num");
		t_rank->setString(StringUtils::format("%d", i + 1));
		if (i < 3)
		{
			if (i + 1 == 1)
			{
				i_trophy->loadTexture(RANK_GOLD);
			}
			else if (i + 1 == 2)
			{
				i_trophy->loadTexture(RANK_SILVER);
			}
			else
			{
				i_trophy->loadTexture(RANK_COPPER);
			}
		}
		else
		{
			i_trophy->setVisible(false);
			t_rank->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		}
		i_nation->loadTexture(getCountryIconPath(result->rankinfo[i]->nation));
		t_name->setString(result->rankinfo[i]->username);
		t_lv->setString(StringUtils::format("Lv. %d", result->rankinfo[i]->level));
		t_admage->setString(StringUtils::format("%d", result->rankinfo[i]->hurt));
		l_ranking->pushBackCustomItem(item);
	}

	auto image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_head_16"));
	auto t_rank_title = view->getChildByName<Text*>("label_you_rank");
	auto t_rank_num = view->getChildByName<Text*>("label_you_rank_num");
	auto t_damage_title = view->getChildByName<Text*>("label_you_total_invest");
	auto t_damage_num = view->getChildByName<Text*>("label_you_total_invest_num");
	if (result->myrank)
	{
		t_rank_num->setString(StringUtils::format("%d", result->myrank));
	}
	else
	{
		t_rank_num->setString("N/A");
	}
	
	t_rank_num->setPositionX(t_rank_title->getBoundingBox().size.width + t_rank_title->getPositionX() + 10);
	t_damage_num->setString(StringUtils::format("%d", result->mydamage));
	t_damage_num->setPositionX(t_damage_title->getBoundingBox().size.width + t_damage_title->getPositionX() + 10);
	image_head->loadTexture(getPlayerIconPath(result->headicon));
	auto  i_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	addListViewBar(l_ranking, i_pulldown);
	auto t_no_ranking = view->getChildByName<Text*>("label_no_ranking");
	if (result->n_rankinfo > 0)
	{
		t_no_ranking->setVisible(false);
	}
	else
	{
		t_no_ranking->setVisible(true);
	}

	if (SINGLE_HERO->m_heroIsOnsea)
	{
		view->setCameraMask(4);
	}
	else
	{
		view->setCameraMask(1);
	}
}
void UICommon::addItem(float dt)
{
	int a = m_listitems.size();

	if (a == m_captainlist->getItems().size())
	{

		unschedule(schedule_selector(UICommon::addItem));
		return;
	}
	else
	{
		if (m_bitemsaddCompleted)
		{
			for (int i = m_captainlist->getItems().size(); i < m_listitems.size();i++)
			{
				m_captainlist->pushBackCustomItem(m_listitems.at(i));
			}
			unschedule(schedule_selector(UICommon::addItem));
		}
		else
		{
			m_captainlist->pushBackCustomItem(m_listitems.at(m_captainlist->getItems().size()));
		}
		
	}
	if (m_captainlist->getItems().size() >= 4)
	{
		Widget * expview = nullptr;
		if (m_pCaptainAddExp != nullptr)
	    {
			expview = m_pCaptainAddExp;
		}
		else if (m_pCaptainAddExpBattle != nullptr)
		{
			expview = m_pCaptainAddExpBattle;
		}

		auto i_pilldown = expview->getChildByName<ImageView*>("image_pulldown");
		auto l_list = expview->getChildByName<ListView*>("listview_content");
	    if (i_pilldown->isVisible())
	    {
			return;
	    }
		addListViewBar(l_list, i_pilldown);
		auto btn = i_pilldown->getChildByName("button_pulldown");
		btn->setAnchorPoint(Vec2(0.5, 0));
	}

}
void UICommon::flushWarningLongText(std::string title, std::string contentText)
{
	openView(COMMOM_COCOS_RES[C_VIEW_WARNING_LONGTEXT_CSB]);
	auto warning = getViewRoot(COMMOM_COCOS_RES[C_VIEW_WARNING_LONGTEXT_CSB]);
	warning->setCameraMask(_cameraMask, true);
	if (warning)
	{
		auto button_warning_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(warning, "button_warning_1"));
		auto image_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(warning, "image_pulldown"));

		auto listview = dynamic_cast<ListView*>(Helper::seekWidgetByName(warning, "listview_text"));
		auto label_goods_name = dynamic_cast<Text*>(Helper::seekWidgetByName(warning, "label_goods_name"));
		auto label_price_num = dynamic_cast<Text*>(Helper::seekWidgetByName(warning, "label_price_num"));

		std::string name = SINGLE_SHOP->getTipsInfo()[title];
		std::string content = SINGLE_SHOP->getTipsInfo()[contentText];
		button_warning_1->setTouchEnabled(false);
		button_warning_1->loadTextureNormal("cocosstudio/login_ui/sailing_720/supply_over.png");
		label_goods_name->setString(name);
		label_price_num->setString(content);
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
		addListViewBar(listview, image_pulldown);
	}
}
//打开船只详细信息的船舱界面
void UICommon::flushShipCabinsView()
{
	openView(COMMOM_COCOS_RES[C_VIEW_SHIP_CABIN]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SHIP_CABIN]);
	auto item = Helper::seekWidgetByName(view, "panel_cabin");
	auto i_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown"));
	auto l_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_content"));

	std::map<int, int> m_carbin;
	getCabinsInfo(m_carbin);
	int n_pnum = ceil(float(m_carbin.size()) / 2);
	int b = 0;
	auto i = this->getCameraMask();
	auto carbinInfo = m_carbin.begin();
	for (int a = 0; a < n_pnum; a++)
	{
		auto c_item = item->clone();
		c_item->setCameraMask(i,true);
		for (int c = 0; c < 2; c++)
		{
			if (b == m_carbin.size() - 1)
			{
				auto name = StringUtils::format("image_cabin_%d", c + 1);
				auto i_carbin1 = Helper::seekWidgetByName(c_item, name);
				auto i_carbin2 = Helper::seekWidgetByName(c_item, "image_cabin_2");
				auto t_name1 = dynamic_cast<Text*>(Helper::seekWidgetByName(i_carbin1, "label_derable"));
				auto t_num1 = dynamic_cast<Text*>(Helper::seekWidgetByName(i_carbin1, "label_derable_num"));
				if (c == 0)
				{
					i_carbin2->setVisible(false);
				}
				t_name1->ignoreContentAdaptWithSize(true);
				t_name1->setString(SINGLE_SHOP->getJobPositionInfo()[carbinInfo->first].room);
				t_num1->setString(StringUtils::format("x%d", carbinInfo->second));
				dynamic_cast<ImageView*>(i_carbin1)->loadTexture(getCarbinTexturePath(carbinInfo->first));
				break;
			}
			else
			{
				auto name = StringUtils::format("image_cabin_%d", c + 1);
				auto i_carbin1 = Helper::seekWidgetByName(c_item, name);
				auto t_name1 = dynamic_cast<Text*>(Helper::seekWidgetByName(i_carbin1, "label_derable"));
				auto t_num1 = dynamic_cast<Text*>(Helper::seekWidgetByName(i_carbin1, "label_derable_num"));
				t_name1->ignoreContentAdaptWithSize(true);
				t_name1->setString(SINGLE_SHOP->getJobPositionInfo()[carbinInfo->first].room);
				t_num1->setString(StringUtils::format("x%d", carbinInfo->second));
				dynamic_cast<ImageView*>(i_carbin1)->loadTexture(getCarbinTexturePath(carbinInfo->first));
			}
				
			b++;
			carbinInfo++;
		}
		l_content->pushBackCustomItem(c_item);
	}
	auto b_pull = i_pulldown->getChildByName("button_pulldown");
	b_pull->setPositionX(b_pull->getPositionX() - b_pull->getContentSize().width / 2);
	addListViewBar(l_content, i_pulldown);
}

//打开船只详细信息的装备界面
void UICommon::flushShipPartsView()
{
	openView(COMMOM_COCOS_RES[C_VIEW_SHIP_PART]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SHIP_PART]);
	auto shipInfo = SINGLE_SHOP->getShipData()[m_curShipId];
	int cannon_num = shipInfo.cannon_num;
	int armornum = shipInfo.armor_num;
	int sailnum = shipInfo.spinnnaker_num;
	int n_spur = shipInfo.bowgun_num;
	int n_firsthead = shipInfo.bowicon_num;
	auto b_fisrthead = Helper::seekWidgetByName(view, "button_firsthead");
	auto b_spur = Helper::seekWidgetByName(view, "button_spur");
	if (n_firsthead < 1)
	{
		b_fisrthead->setVisible(false);
	}
	else
	{
		b_fisrthead->setTouchEnabled(false);
	}

	if (n_spur < 1)
	{
		b_spur->setVisible(false);
	}
	else
	{
		b_spur->setTouchEnabled(false);
	}

	for (int i = 3; i >= sailnum + 1; i--)
	{
		auto b_sail = Helper::seekWidgetByName(view, StringUtils::format("button_sail_%d", i - 1));
		b_sail->setVisible(false);
	}
	for (int i = 0; i <= 2; i++)
	{
		auto b_sail = Helper::seekWidgetByName(view, StringUtils::format("button_sail_%d", i));
		b_sail->setTouchEnabled(false);
	}
	for (int i = 3; i >= armornum + 1; i--)
	{
		auto b_armor = Helper::seekWidgetByName(view, StringUtils::format("button_armor_%d", i - 1));
		b_armor->setVisible(false);
	}
	for (int i = 0; i <= 2; i++)
	{
		auto b_armor = Helper::seekWidgetByName(view, StringUtils::format("button_armor_%d", i));
		b_armor->setTouchEnabled(false);
	}
	for (int i = 8; i >= cannon_num + 1; i--)
	{
		auto b_gun = Helper::seekWidgetByName(view, StringUtils::format("button_gun_%d", i));
		b_gun->setVisible(false);
	}
	for (int i = 1; i <= 8; i++)
	{
		auto b_gun = Helper::seekWidgetByName(view, StringUtils::format("button_gun_%d", i));
		b_gun->setTouchEnabled(false);
	}
}
void UICommon::getCabinsInfo(std::map<int, int>&carbins)
{
	carbins.clear();
	std::vector<int> v_type;
	auto room_define = SINGLE_SHOP->getShipData().at(m_curShipId).room_define;
	//解析
	std::vector<int>room_num;
	while (room_define.find(",") != -1)
	{
		int a = room_define.find_first_of(",");
		int value = atoi(room_define.substr(0, a).c_str());
		room_define = room_define.substr(a + 1, std::string::npos);
		room_num.push_back(value);
	}
	int a = atoi(room_define.c_str());
	room_num.push_back(a);

	//判断
	//1:小 2:中 3:大
	int ship_type = SINGLE_SHOP->getShipData().at(m_curShipId).type;
	int n_grid;
	switch (ship_type)
	{
	case 1:
		n_grid = 16;
		break;
	case 2:
		n_grid = 24;
		break;
	case 3:
		n_grid = 33;
		break;
	default:
		n_grid = 16;
		break;
	}
	auto shipRoom = SINGLE_SHOP->getShipRoom();
	int room_type = 0;
	int room_id = 0;
	for (size_t i = 0; i < n_grid; i++)
	{
		room_id = i + 1;
		room_type = 0;
		std::map <int, SHIP_ROOM_INFO>::iterator m1_Iter;
		for (m1_Iter = shipRoom.begin(); m1_Iter != shipRoom.end(); m1_Iter++){
			auto& item = m1_Iter->second;
			if (ship_type == item.ship_type && room_id == item.room_id)
			{
				//舰长室特殊处理(唯一)
				if (item.room_type == 20)
				{
					room_type = item.room_type;
					break;
				}//废弃
				else if (item.room_type == 0)
				{
					room_type = 0;
					break;
				}
				else if (room_num.at(item.room_type - 1) > 0)
				{
					room_num.at(item.room_type - 1)--;
					room_type = item.room_type;
					if (room_type == 11)
					{
						room_type = 0;
					}
					break;
				}
				else
				{
					room_type = 0;
					break;;
				}
			}
		}	
		if (room_type > 0)
		{
			v_type.push_back(room_type);
		}
	}
	for (int i = 0; i < 11; i++)
	{
		if (i == 10)
		{
			carbins.insert(std::pair<int, int>(20, 0));
		}
		else
		{
			carbins.insert(std::pair<int, int>(i + 1, 0));
		}
	}
	for (auto type : v_type)
	{
		if (type == CARBIN_CAPTAIN)
		{
			carbins[20] += 1;
		}
		else
		{
			carbins[type] += 1;
		}
	}
	std::map<int, int>v_sub;
	int index = 1;
	for (auto carbin :carbins)
	{
		if (carbin.second > 0)
		{
			if (carbin.first == 20)
			{
				index = 20;
			}
			v_sub.insert(std::pair<int, int>(index, carbin.second));
		}
		index++;
	}
	carbins = v_sub;
}
void UICommon::judgeProficiencyLevel(int m_curView)
{
	if (m_proficiencyResult == nullptr)
	{
		return;
	}
	if (m_curView == VIEW_SHIP)
	{
		auto shipInfo = SINGLE_SHOP->getShipData()[m_curShipId];
		auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SHIP_DETAIL_CSB]);
		auto t_proficiency = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_require_bluep"));
		int prolevel = 0;
		int needlevel = 0;
		for (int i = 0; i < m_proficiencyResult->n_values; i++)
		{
			auto proficiency = m_proficiencyResult->values[i];
			if (proficiency->proficiencyid == shipInfo.proficiency_need)
			{
				prolevel = getProficiencyLevel(proficiency->value);
				needlevel = shipInfo.rarity;
				break;
			}
		}
		if (needlevel > prolevel)
		{
			t_proficiency->setTextColor(Color4B(183, 28, 28, 255));
		}
		else
		{
			t_proficiency->setTextColor(Color4B(56, 28, 1, 255));
		}
	}else if (m_curView == VIEW_EQUIPMENT)
	{
		auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_EQUIP_DETAIL_CSB]);
		auto item = SINGLE_SHOP->getItemData()[m_curEquipId];
		auto t_proficiency = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_require_bluep"));
		int prolevel = 0;
		int needlevel = 0;
		for (int i = 0; i < m_proficiencyResult->n_values; i++)
		{
			auto proficiency = m_proficiencyResult->values[i];
			if (proficiency->proficiencyid == item.poficiency)
			{
				prolevel = getProficiencyLevel(proficiency->value);
				needlevel = item.rarity;
				break;
			}
		}
		if (needlevel > prolevel)
		{
			t_proficiency->setTextColor(Color4B(183, 28, 28, 255));
		}
		else
		{
			t_proficiency->setTextColor(Color4B(56, 28, 1, 255));
		}
	}
}
int UICommon::getProficiencyLevel(int proficiencyNum)
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

void UICommon::flushShipListView(GetPlayerShipListResult* result)
{
	openView(COMMOM_COCOS_RES[C_VIEW_PLAYER_FLEET_CSB]);
//	showShipEquipmentorModelView(result->shipinfo[0]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PLAYER_FLEET_CSB]);
	auto l_ships = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_ships"));
	auto p_gear = Helper::seekWidgetByName(view, "panel_gear");;
	auto t_no = Helper::seekWidgetByName(view, "text_no");
	if (result->n_infos <= 0)
	{
		p_gear->setVisible(false);
		l_ships->setVisible(false);
		dynamic_cast<Text*>(t_no)->setString(SINGLE_SHOP->getTipsInfo()["TIP_SHIPLIST_NO_SHIP"]);
		t_no->setVisible(true);
		m_needClose = true;
		return;
	}
	for (int i = 0; i < 5;i++)
	{
		auto item = l_ships->getItem(i);
		if (i< result->n_infos)
		{
			auto shipInfo = result->infos[i];
			auto i_ship = item->getChildByName<ImageView*>("image_ship");
			setBgButtonFormIdAndType(item, shipInfo->sid, ITEM_TYPE_SHIP);
			i_ship->loadTexture(getShipIconPath(shipInfo->sid));
			item->setTag(shipInfo->ship_id);
			auto i_pos = item->getChildByName<ImageView*>("image_num");
			i_pos->loadTexture(getPositionIconPath(i + 1));
			if (shipInfo->used_enhance_slots > 0)
			{
				addStrengtheningIcon(item);
				auto icon = item->getChildByName("goods_intensify");
				icon->setCameraMask(item->getCameraMask(), true);
			}
			if (i == 0)
			{
				changeSlectShipButton(item);
			}
		}
		else
		{
			item->setVisible(false);
		}
	}
	auto i_ste = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_steering"));
	i_ste->setTouchEnabled(true);
	i_ste->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_ste->setTag(IMAGE_ICON_STEERING + IMAGE_INDEX2);
	auto i_sup = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_supply"));
	i_sup->setTouchEnabled(true);
	i_sup->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_sup->setTag(IMAGE_ICON_SUPPLY + IMAGE_INDEX2);
	auto i_wei = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_weight"));
	i_wei->setTouchEnabled(true);
	i_wei->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_wei->setTag(IMAGE_ICON_CARGO + IMAGE_INDEX2);
	auto i_dur = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_durable"));
	i_dur->setTouchEnabled(true);
	i_dur->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_dur->setTag(IMAGE_ICON_DURABLE + IMAGE_INDEX2);
	auto i_sai = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_sailor"));
	i_sai->setTouchEnabled(true);
	i_sai->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_sai->setTag(IMAGE_ICON_SAILOR + IMAGE_INDEX2);
	auto i_spe = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_speed"));
	i_spe->setTouchEnabled(true);
	i_spe->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_spe->setTag(IMAGE_ICON_SPEED + IMAGE_INDEX2);
	auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_atk"));
	i_atk->setTouchEnabled(true);
	i_atk->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_atk->setTag(IMAGE_ICON_ATTACKPOWER + IMAGE_INDEX2);
	auto i_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_def"));
	i_def->setTouchEnabled(true);
	i_def->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_def->setTag(IMAGE_ICON_DEFENSEPOWER + IMAGE_INDEX2);

	registerCallBack();
	ProtocolThread::GetInstance()->getPlayerEquipShipInfo(result->infos[0]->ship_id, UILoadingIndicator::createWithMask(this, _cameraMask));
}
void UICommon::showShipModelView(int modeId)
{
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PLAYER_FLEET_CSB]);
	view->setCameraMask(_cameraMask);
	auto shipInfos = SINGLE_SHOP->getShipData();
	auto i_bg = Helper::seekWidgetByName(view, "image_3d_bg");
	auto scene = _director->getRunningScene();
	auto b_switch = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_parts"));
// 	Layer * shiplayer = dynamic_cast<Layer*>(scene->getChildByName("shipmodelayer"));
// 	if (m_operaModel != nullptr)
// 	{
// 		int tag = m_operaModel->getTag();
// 		if (tag == modeId)
// 		{
// 			if (shiplayer != nullptr)
// 			{
// 				shiplayer->setVisible(true);
// 				log("1111");
// 				m_operaModel->setRotation3D(Vec3(0, -65, 0));
// 				return;
// 			}
// 		}
// 		else
// 		{
// 			m_operaModel->removeFromParentAndCleanup(true);
// 		}
// 	}
// 
// 	Node * node = nullptr;
// 	Layer * layer = nullptr;
// 	if (shiplayer != nullptr)
// 	{
// 		shiplayer->setVisible(true);
// 		layer = shiplayer;
// 		node = layer->getChildByName("shipNode");
// 	}
// 	else{
// 		layer = Layer::create();
// 		layer->setName("shipmodelayer");
// 		node = Node::create();
// 		node->setName("shipNode");
// 		layer->addChild(node);
// 		scene->addChild(layer, 1001);
// //船只camera
// 		Size s = CCDirector::getInstance()->getWinSize();
// 		auto m_camera = Camera::createPerspective(60, (GLfloat)s.width / s.height, 1, 1000);
// 		m_camera->setCameraFlag(CameraFlag::USER3);
// 		Vec3 shipPos = Vec3(0, 0, 0);
// 		m_camera->lookAt(shipPos, Vec3(0, 1, 0));
// 		m_camera->setRotation3D(Vec3(0, 0, 0));
// 		m_camera->setPosition3D(Vec3(0.4, 2.5, 6.5));
// 		m_camera->setDepth(5);
// 		node->addChild(m_camera);
// 		if (this->getCameraMask() != 4)
// 		{
// //界面Camera 管理loadingLayer
// 			auto size = Director::getInstance()->getWinSize();
// 			Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
// 			popCam->setCameraFlag(CameraFlag::USER2);
// 			popCam->setDepth(6);
// 			layer->addChild(popCam);
// 		}
// 		else
// 		{
// 			auto size = Director::getInstance()->getWinSize();
// 			Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
// 			popCam->setCameraFlag(CameraFlag::USER4);
// 			popCam->setDepth(6);
// 			layer->addChild(popCam);
// 		}
// 		auto listener = EventListenerTouchOneByOne::create();
// 		listener->onTouchBegan = CC_CALLBACK_2(UICommon::onTouchBegan, this);
// 		listener->onTouchMoved = CC_CALLBACK_2(UICommon::onTouchMoved, this);
// 		listener->onTouchEnded = CC_CALLBACK_2(UICommon::onTouchEnded, this);
// 		_director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, layer);
// 	}
// 	auto ship = ShipModel::create(modeId);
// 	ship->setCameraMask(8);
// 	ship->setScale(1.0 / 4);
// 	ship->setName("Ship");
// 	ship->setForceDepthWrite(true);
// 	m_operaModel = ship;
// 	auto position = i_bg->getWorldPosition();
// 	node->setPosition(i_bg->getWorldPosition());
// 	node->addChild(ship);
// 	auto size = i_bg->getContentSize();
// 	ship->setPosition(Vec2(2.2,1.2));
// 	ship->setRotation3D(Vec3(0, -65, 0));
// 	ship->setTag(modeId);
 	
// 	m_needClose = true;
 	m_shipEquimentShow = false;
	auto manger = ModeLayerManger::getInstance();
	manger->openModelLayer();
	manger->addShip(i_bg, modeId, COMMON_SHIP);
	showEquipmentOrShipModel(true);
}

void UICommon::flushShipEquimentView(GetPlayerEquipShipInfoResult * result)
{
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PLAYER_FLEET_CSB]);
	m_ShipInfoResult = result;
	auto shipInfo = SINGLE_SHOP->getShipData()[result->sid];
	auto p_shipInfo = Helper::seekWidgetByName(view, "panel_shipquip");
//船只属性
	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_name"));
	auto t_durable1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_1"));
	auto t_durable2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_2"));
	auto t_sailor1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_sailors_num_1"));
	auto t_sailor2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_sailors_num"));
	auto t_capacipy1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_capacity_num_1"));
	auto t_capacipy2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_capacity_num"));
	auto t_supply1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_suppy_num_1"));
	auto t_supply2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_suppy_num"));
	auto t_attack = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_attack_num"));
	auto t_defense = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_defense_num"));
	auto t_speed = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_speed_num"));
	auto t_steering = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_steering_num"));
//	auto sumInfo = getEquipDetailSumInfo(result);
	t_durable2->setString(StringUtils::format("??? / %d", result->max_durable));
	t_sailor2->setString(StringUtils::format("??? / %d", result->crew_num));
	t_capacipy2->setString(StringUtils::format("??? / %d", result->cargo_size / 100));
	t_supply2->setString(StringUtils::format("??? / %d", result->max_supply));

	t_attack->setString(StringUtils::format("%d", result->attack));
	t_defense->setString(StringUtils::format("%d", result->defense));
	t_speed->setString(StringUtils::format("%d", result->speed));
	t_steering->setString(StringUtils::format("%d", result->steering));
	std::string name = result->ship_name;
	if (name.size() != 0)
	{
		t_name->setString(result->ship_name);
		setTextColorFormIdAndType(t_name, result->sid, ITEM_TYPE_SHIP);
	}
	else
	{
		t_name->setString(shipInfo.name);
		setTextColorFormIdAndType(t_name, result->sid, ITEM_TYPE_SHIP);
	}
	//船只装备
	auto b_firsthead = dynamic_cast<Button*>(Helper::seekWidgetByName(p_shipInfo, "button_firsthead"));
	auto b_spur = dynamic_cast<Button*>(Helper::seekWidgetByName(p_shipInfo, "button_spur"));
	log("=====%s", result->cannonids);
	log("=====%s", result->armorids);
	log("=====%s", result->spinnakerids);

	std::string s_cannonids = result->cannonids;
	std::string s_armorids = result->armorids;
	std::string s_spinnakers = result->spinnakerids;

	std::vector<int> v_cannonIds;
	std::vector<int> v_armorids;
	std::vector<int> v_spinnakers;
	v_cannonIds.assign(shipInfo.cannon_num, 0);
	v_armorids.assign(shipInfo.armor_num, 0);
	v_spinnakers.assign(shipInfo.spinnnaker_num, 0);
	convertToVector(s_cannonids, v_cannonIds);
	convertToVector(s_armorids, v_armorids);
	convertToVector(s_spinnakers, v_spinnakers);
	//船首像
	if (shipInfo.bowicon_num > 0)
	{
		b_firsthead->setVisible(true);
		b_firsthead->setTouchEnabled(false);
		auto i_equip = b_firsthead->getChildByName<ImageView*>("image_equip");
		if (result->bowiconid > 0)
		{
			int iid = getEquipIID(result->bowiconid, result);
			i_equip->loadTexture(getItemIconPath(iid));
			i_equip->setVisible(true);
			i_equip->getParent()->setTag(result->bowiconid);

//			b_firsthead->setBright(false);
			i_equip->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
			i_equip->setTouchEnabled(true); 
			setBgButtonFormIdAndType(b_firsthead, iid, ITEM_TYPE_SHIP_EQUIP);
		}
		else
		{
			i_equip->setVisible(false);
			i_equip->getParent()->setTag(0);
			b_firsthead->setBright(true);
			b_firsthead->setTouchEnabled(false);
			b_firsthead->loadTextureNormal("cocosstudio/login_ui/shipyard_720/equip_firsthead.png");
		}
	}
	else
	{
		b_firsthead->setVisible(false);
	}
	//撞角
	if (shipInfo.bowgun_num > 0)
	{
		b_spur->setVisible(true);
		b_spur->setTouchEnabled(false);
		auto i_equip = b_spur->getChildByName<ImageView*>("image_equip");
		if (result->bowgunid > 0)
		{
			int iid = getEquipIID(result->bowgunid, result);
			i_equip->loadTexture(getItemIconPath(iid));
			i_equip->setVisible(true);
			i_equip->getParent()->setTag(result->bowgunid);

//			b_spur->setBright(false);
			i_equip->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
			i_equip->setTouchEnabled(true);
			setBgButtonFormIdAndType(b_spur, iid, ITEM_TYPE_SHIP_EQUIP);
		}
		else
		{
			i_equip->setVisible(false);
			i_equip->getParent()->setTag(0);
			b_spur->setBright(true);
			b_spur->setTouchEnabled(false);
			b_spur->loadTextureNormal("cocosstudio/login_ui/shipyard_720/equip_ram.png");
		}
	}
	else
	{
		b_spur->setVisible(false);
	}

	//火炮
	if (shipInfo.cannon_num > 0)
	{
		for(int i = 0; i < 8; i++)
		{
			std::string name = StringUtils::format("button_gun_%d", i + 1);
			auto b_cannon = Helper::seekWidgetByName(p_shipInfo, name);
			auto i_cannon = b_cannon->getChildByName<ImageView*>("image_equip");
			if (i < shipInfo.cannon_num)
			{
				b_cannon->setVisible(true);
				int iid = getEquipIID(v_cannonIds.at(i), result);
				if (iid != 0)
				{
					i_cannon->setVisible(true);
					i_cannon->loadTexture(getItemIconPath(iid));
					auto cannonInfo = getEquipDetailInfo(iid);
					i_cannon->getParent()->setTag(v_cannonIds.at(i));
					if (cannonInfo->optionalitemnum > 0)
					{
//						addStrengtheningIcon(b_cannon);
						auto i_flag = b_cannon->getChildByName("goods_intensify");
						if (i_flag == nullptr)
						{
							addStrengtheningIcon(b_cannon);
							auto flag = b_cannon->getChildByName("goods_intensify");
							flag->setCameraMask(b_cannon->getCameraMask(), true);
						}
					}
					else
					{
						auto i_flag = b_cannon->getChildByName("goods_intensify");
						if (i_flag != nullptr)
						{
							i_flag->removeFromParentAndCleanup(true);
						}
					}
//					b_cannon->setBright(false);
					b_cannon->setTouchEnabled(false);
					i_cannon->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
					i_cannon->setTouchEnabled(true);
					setBgButtonFormIdAndType(b_cannon, iid, ITEM_TYPE_SHIP_EQUIP);
				}
				else
				{
					i_cannon->setVisible(false);
					b_cannon->setBright(true);
					b_cannon->setTouchEnabled(false);
					dynamic_cast<Button*>(b_cannon)->loadTextureNormal("cocosstudio/login_ui/shipyard_720/equip_gun.png");
					auto i_flag = b_cannon->getChildByName("goods_intensify");
					if (i_flag != nullptr)
					{
						i_flag->removeFromParentAndCleanup(true);
					}
				}
			}
			else
			{
				b_cannon->setVisible(false);
			}
		}
	}
	//装甲
	if (shipInfo.armor_num > 0)
	{
		for (int i = 0; i < 3; i++)
		{
			std::string name = StringUtils::format("button_armor_%d", i + 1);
			auto b_armor = Helper::seekWidgetByName(p_shipInfo, name);
			auto i_armor = b_armor->getChildByName<ImageView*>("image_equip");
			if (i < shipInfo.armor_num)
			{
				int iid = getEquipIID(v_armorids.at(i), result);
				b_armor->setVisible(true);
				if (iid != 0)
				{
					i_armor->setVisible(true);
					i_armor->loadTexture(getItemIconPath(iid));
					i_armor->getParent()->setTag(v_armorids.at(i));
//					b_armor->setBright(false);
					b_armor->setTouchEnabled(false);
					i_armor->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
					i_armor->setTouchEnabled(true);
					setBgButtonFormIdAndType(b_armor, iid, ITEM_TYPE_SHIP_EQUIP);
					auto armorInfo = getEquipDetailInfo(iid);		
					if (armorInfo->optionalitemnum > 0)
					{
//						addStrengtheningIcon(b_armor);
						auto i_flag = b_armor->getChildByName("goods_intensify");
						if (i_flag == nullptr)
						{
							addStrengtheningIcon(b_armor);
							auto flag = b_armor->getChildByName("goods_intensify");
							flag->setCameraMask(b_armor->getCameraMask(), true);
						}
					}
					else
					{
						auto i_flag = b_armor->getChildByName("goods_intensify");
						if (i_flag != nullptr)
						{
							i_flag->removeFromParentAndCleanup(true);
						}
					}
				}
				else
				{
					i_armor->setVisible(false);
					b_armor->setBright(true);
					b_armor->setTouchEnabled(false);
					dynamic_cast<Button*>(b_armor)->loadTextureNormal("cocosstudio/login_ui/shipyard_720/equip_armor.png");
					auto i_flag = b_armor->getChildByName("goods_intensify");
					if (i_flag != nullptr)
					{
						i_flag->removeFromParentAndCleanup(true);
					}
				}
			}
			else
			{
				b_armor->setVisible(false);
			}
		}
	}
	else{
		for (int i = 0; i < 3; i++)
		{
			std::string name = StringUtils::format("button_armor_%d", i + 1);
			auto b_armor = Helper::seekWidgetByName(p_shipInfo, name);
			b_armor->setVisible(false);
		}
	}


	//船帆个数
	if (shipInfo.spinnnaker_num > 0)
	{
		for (int i = 0; i < 3; i++)
		{
			std::string name = StringUtils::format("button_sail_%d", i + 1);
			auto b_sail = Helper::seekWidgetByName(p_shipInfo, name);
			auto i_sail = b_sail->getChildByName<ImageView*>("image_equip");
			if (i < shipInfo.spinnnaker_num)
			{
				int iid = getEquipIID(v_spinnakers.at(i), result);
				b_sail->setVisible(true);
				if (iid != 0)
				{
					i_sail->setVisible(true);
					i_sail->loadTexture(getItemIconPath(iid));
					i_sail->getParent()->setTag(v_spinnakers.at(i));

//					b_sail->setBright(false);
					b_sail->setTouchEnabled(false);
					i_sail->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
					i_sail->setTouchEnabled(true);
					setBgButtonFormIdAndType(b_sail, iid, ITEM_TYPE_SHIP_EQUIP);
					auto sailInfo = getEquipDetailInfo(iid);
					if (sailInfo->optionalitemnum > 0)
					{
						auto i_flag = b_sail->getChildByName("goods_intensify");
						if (i_flag == nullptr)
						{
							addStrengtheningIcon(b_sail);
							auto flag = b_sail->getChildByName("goods_intensify");
							flag->setCameraMask(b_sail->getCameraMask(), true);
						}
					}
					else
					{
						auto i_flag = b_sail->getChildByName("goods_intensify");
						if (i_flag != nullptr)
						{
							i_flag->removeFromParentAndCleanup(true);
						}
					}
				}
				else
				{
					i_sail->setVisible(false);
					b_sail->setBright(true);
					b_sail->setTouchEnabled(false);
					auto i_flag = b_sail->getChildByName("goods_intensify");
					if (i_flag != nullptr)
					{
						i_flag->removeFromParentAndCleanup(true);
					}
					dynamic_cast<Button*>(b_sail)->loadTextureNormal("cocosstudio/login_ui/shipyard_720/equip_sails.png");
				}
			}
			else
			{
				b_sail->setVisible(false);
			}
		}
	}else{
		for (int i = 0; i < 3; i++)
		{
			std::string name = StringUtils::format("button_sail_%d", i + 1);
			auto b_sail = Helper::seekWidgetByName(p_shipInfo, name);
			b_sail->setVisible(false);
		}
	}
#if CC_ENABLE_PROFILERS
	ProfilingBeginTimingBlock("t_scene2_preload");
#endif
	showEquipmentOrShipModel(true);
#if CC_ENABLE_PROFILERS
	ProfilingEndTimingBlock("t_scene2_preload");
	Profiler::sharedProfiler()->displayTimers();
#endif
	m_needClose = true;
	m_shipEquimentShow = false;
}

GetPlayerEquipShipInfoResult* UICommon::getEquipDetailSumInfo(GetPlayerEquipShipInfoResult * shipInfo)
{
	auto equipDetailSum = new GetPlayerEquipShipInfoResult;
	auto shipLocalInfo = SINGLE_SHOP->getShipData()[shipInfo->sid];

	memcpy(equipDetailSum, shipInfo, sizeof(GetPlayerEquipShipInfoResult));

	int iid = getEquipIID(shipInfo->bowiconid, shipInfo);
	auto itemEquip = getEquipDetailInfo(iid);
	if (itemEquip)
	{
		equipDetailSum->attack += itemEquip->attack;
		equipDetailSum->defense += itemEquip->defense;
		equipDetailSum->speed += itemEquip->speed;
		equipDetailSum->steering += itemEquip->steer_speed;
	}
	iid = getEquipIID(shipInfo->bowgunid, shipInfo);
	itemEquip = getEquipDetailInfo(iid);
	if (itemEquip)
	{
		equipDetailSum->attack -= itemEquip->attack;
		equipDetailSum->defense -= itemEquip->defense;
		equipDetailSum->speed -= itemEquip->speed;
		equipDetailSum->steering -= itemEquip->steer_speed;
	}


// 	itemEquip = getEquipDetailInfo(m_nCurGun1_id);
// 	if (itemEquip)
// 	{
// 		equipDetailSum->attack += itemEquip->attack;
// 		equipDetailSum->defense += itemEquip->defense;
// 		equipDetailSum->speed += itemEquip->speed;
// 		equipDetailSum->steering += itemEquip->steer_speed;
// 	}
// 	itemEquip = getEquipDetailInfo(m_nFirst_Gun1_id);
// 	if (itemEquip)
// 	{
// 		equipDetailSum->attack -= itemEquip->attack;
// 		equipDetailSum->defense -= itemEquip->defense;
// 		equipDetailSum->speed -= itemEquip->speed;
// 		equipDetailSum->steering -= itemEquip->steer_speed;
// 	}

	std::string s_cannonids = shipInfo->cannonids;
	std::string s_armorids = shipInfo->armorids;
	std::string s_spinnakers = shipInfo->spinnakerids;

	std::vector<int> v_cannonIds;
	std::vector<int> v_armorids;
	std::vector<int> v_spinnakers;
	v_cannonIds.assign(shipLocalInfo.cannon_num, 0);
	v_armorids.assign(shipLocalInfo.armor_num, 0);
	v_spinnakers.assign(shipLocalInfo.spinnnaker_num, 0);
	convertToVector(s_cannonids, v_cannonIds);
	convertToVector(s_armorids, v_armorids);
	convertToVector(s_spinnakers, v_spinnakers);

	for (int i = 0; i < v_spinnakers.size(); i++)
	{
		iid = getEquipIID(v_spinnakers[i], shipInfo);	
		itemEquip = getEquipDetailInfo(iid);
		if (itemEquip)
		{
			equipDetailSum->attack += itemEquip->attack;
			equipDetailSum->defense += itemEquip->defense;
			equipDetailSum->speed += itemEquip->speed;
			equipDetailSum->steering += itemEquip->steer_speed;
		}

// 		itemEquip = getEquipDetailInfo(m_vFirst_CurSails_id[i]);
// 		if (itemEquip)
// 		{
// 			equipDetailSum->attack -= itemEquip->attack;
// 			equipDetailSum->defense -= itemEquip->defense;
// 			equipDetailSum->speed -= itemEquip->speed;
// 			equipDetailSum->steering -= itemEquip->steer_speed;
// 		}
	}

	for (int i = 0; i < v_cannonIds.size(); i++)
	{
		iid = getEquipIID(v_cannonIds[i], shipInfo);
		itemEquip = getEquipDetailInfo(iid);
		if (itemEquip)
		{
			equipDetailSum->attack += itemEquip->attack;
			equipDetailSum->defense += itemEquip->defense;
			equipDetailSum->speed += itemEquip->speed;
			equipDetailSum->steering += itemEquip->steer_speed;
		}

// 		itemEquip = getEquipDetailInfo(v_cannonIds[i]);
// 		if (itemEquip)
// 		{
// 			equipDetailSum->attack -= itemEquip->attack;
// 			equipDetailSum->defense -= itemEquip->defense;
// 			equipDetailSum->speed -= itemEquip->speed;
// 			equipDetailSum->steering -= itemEquip->steer_speed;
// 		}
	}

	for (int i = 0; i < v_armorids.size(); i++)
	{
		iid = getEquipIID(v_armorids[i], shipInfo);
		itemEquip = getEquipDetailInfo(iid);
		itemEquip = getEquipDetailInfo(iid);
		if (itemEquip)
		{
			equipDetailSum->attack += itemEquip->attack;
			equipDetailSum->defense += itemEquip->defense;
			equipDetailSum->speed += itemEquip->speed;
			equipDetailSum->steering += itemEquip->steer_speed;
		}

// 		itemEquip = getEquipDetailInfo(v_armorids[i]);
// 		if (itemEquip)
// 		{
// 			equipDetailSum->attack -= itemEquip->attack;
// 			equipDetailSum->defense -= itemEquip->defense;
// 			equipDetailSum->speed -= itemEquip->speed;
// 			equipDetailSum->steering -= itemEquip->steer_speed;
// 		}
	}

	return equipDetailSum;
}

//显示船只装备 或者 模型界面
void UICommon::showEquipmentOrShipModel(bool show)
{
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PLAYER_FLEET_CSB]);
	auto p_equipment = Helper::seekWidgetByName(view, "panel_shipquip");
	auto i_3dbg = Helper::seekWidgetByName(view, "image_3d_bg");
	auto b_switch = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_parts"));
	auto i_equipBg = Helper::seekWidgetByName(view, "button_image_gear_equip_bg");
	auto i_3dbg1 = Helper::seekWidgetByName(view, "button_image_gear_equip_bg_2");
	i_equipBg->setTouchEnabled(false);
	i_3dbg1->setTouchEnabled(false);
	if (show)
	{
		i_3dbg->setVisible(true);
		i_3dbg1->setVisible(true);
		i_equipBg->setVisible(false);
		p_equipment->setVisible(false);
		b_switch->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_SWITCH_SHIP_PARTS"]);
	}
	else
	{
		i_3dbg->setVisible(false);
		i_3dbg1->setVisible(false);
		i_equipBg->setVisible(true);
		p_equipment->setVisible(true);
		b_switch->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_SWITCH_SHIP"]);
	}
}
void UICommon::changeSlectShipButton(Widget * psender)
{
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PLAYER_FLEET_CSB]);
	auto l_ship = view->getChildByName<ListView*>("listview_ships");
	auto items = l_ship->getItems();
	for (auto item:items)
	{
		auto name1 = psender->getName();
		auto name2 = item->getName();
		auto i_on = psender->getChildByName("image_btn_on");
		auto i_on1 = item->getChildByName("image_btn_on");
		if (name1 == name2)
		{
			i_on->setVisible(true);
		}
		else
		{
			i_on1->setVisible(false);
		}
	}
}


void UICommon::flushCharacterModelView(GetHeroEquipResult *  info)
{
	openView(COMMOM_COCOS_RES[C_VIEW_PLAYER_EQUIP_CSB]);
	if (!info && !m_curuserInfo)
	{
		return;
	}
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PLAYER_EQUIP_CSB]);
	view->setCameraMask(_cameraMask);
	auto i_modelbg = Helper::seekWidgetByName(view, "image_male");
	std::string path;
	int gender = 0;
	if (m_curuserInfo->icon <= 4)
	{
		path = getCharacterModePath(1);
		gender = 1;
	}
	else
	{
		path = getCharacterModePath(2);
		gender = 2;
	}	
// 	auto sp_character = Sprite3D::create(path);
// 	auto sextant = Sprite3D::create("Character3D/sextant.c3b");
// 	sextant->setCameraMask(8, true);
// 	sextant->setVisible(false);
// 	auto layer = Layer::create();
// 	auto characterNode = Node::create();
// 	layer->setName("charactermodelayer");
// 	characterNode->setName("characterNode");
// 	auto animation = Animation3D::create(path);
// 	auto animate = Animate3D::create(animation);
// 	sp_character->runAction(RepeatForever::create(animate));
// 	sp_character->setScale(0.9);
// 	sp_character->setVisible(true);
// 	sp_character->setForceDepthWrite(true);
// 	sp_character->setCameraMask(8,true);
// 	sp_character->setPosition(Vec2(-1.5, 0.5));
// 	auto pos = i_modelbg->getWorldPosition();
// 	characterNode->setPosition(pos);
// 	m_operaModel = sp_character;
// 	auto scene = _director->getRunningScene();
// 	characterNode->addChild(sp_character);
// 	layer->addChild(characterNode);
// 	scene->addChild(layer, 1001);
// 
// 	auto listener = EventListenerTouchOneByOne::create();
// 	listener->onTouchBegan = CC_CALLBACK_2(UICommon::onTouchBegan, this);
// 	listener->onTouchMoved = CC_CALLBACK_2(UICommon::onTouchMoved, this);
// 	listener->onTouchEnded = CC_CALLBACK_2(UICommon::onTouchEnded, this);
// 	_director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, layer);
// 
// 	//界面Camera 管理loadingLayer
// 
// 	if (_cameraMask != 4)
// 	{
// 		auto size = Director::getInstance()->getWinSize();
// 		Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
// 		popCam->setCameraFlag(CameraFlag::USER2);
// 		popCam->setDepth(6);
// 		layer->addChild(popCam);
// 	}
// 	else
// 	{
// 		auto size = Director::getInstance()->getWinSize();
// 		Camera *popCam = Camera::createOrthographic(size.width, size.height, -1024, 1024);
// 		popCam->setCameraFlag(CameraFlag::USER4);
// 		popCam->setDepth(6);
// 		layer->addChild(popCam);
// 	}
// 	//角色camera 管理角色
// 	Size s = CCDirector::getInstance()->getWinSize();
// 	auto m_camera = Camera::createPerspective(60, (GLfloat)s.width / s.height, 1, 1000);
// 	m_camera->setCameraFlag(CameraFlag::USER3);
// 	Vec3 characterPos = sp_character->getPosition3D();
// //	m_camera->setPosition3D(characterPos + Vec3(0, 20, 20));
// 	m_camera->lookAt(characterPos, Vec3(0, 1, 0));
// 	m_camera->setRotation3D(Vec3(0, 0, 0));
// 	m_camera->setPosition3D(Vec3(0.4, 2.4, 6.5));
// 	m_camera->setDepth(5);
// 	characterNode->addChild(m_camera);
// 	auto attach = sp_character->getAttachNode("joint_hem_LM_100_1");
// 	attach->addChild(sextant);
// 	confirmCharacterFaceAndHair(sp_character);
	CharacterEquiment equipInfo;
	if (info->clothes != nullptr)
	{
		equipInfo.clothId = info->clothes->iid;
	}
	if (info->head != nullptr)
	{
		equipInfo.hatId = info->head->iid;
	}
	if (info->access != nullptr )
	{
		equipInfo.necklaceId = info->access->iid;
	}
	if (info->shoe != nullptr)
	{
		equipInfo.shoesId = info->shoe->iid;
	}
	if (info ->weapon != nullptr)
	{
		equipInfo.weaponId = info->weapon->iid;
	}
	equipInfo.gender = gender;
	equipInfo.iconindex = m_curuserInfo->icon;
	auto instance = ModeLayerManger::getInstance();
	instance->openModelLayer();

 
	instance->addCharacter(i_modelbg, COMMON_CHARACTER, equipInfo);
	auto t_hp = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_speed_num"));
	auto t_attack = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_atk_num"));
	auto t_defence = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_defnum"));
	t_hp->setString("???");
	t_defence->setString("???");
	t_attack->setString("???");
	auto  b_hat = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_maozi"));
	auto  b_clothes = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_yifu"));
	auto  b_shoes = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_xiezi"));
	auto  b_weapon = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_wuqi"));
	auto  b_necklace = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_shoushi"));
	auto i_spe = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_speed"));
	i_spe->setTouchEnabled(true);
	i_spe->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_spe->setTag(IMAGE_ICON_HEALTHPOWER + IMAGE_INDEX2);
	auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_atk"));
	i_atk->setTouchEnabled(true);
	i_atk->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_atk->setTag(IMAGE_ICON_ATTACKPOWER + IMAGE_INDEX2);
	auto i_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_def"));
	i_def->setTouchEnabled(true);
	i_def->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
	i_def->setTag(IMAGE_ICON_DEFENSEPOWER + IMAGE_INDEX2);
//衣服
	if (info->clothes == nullptr)
	{
// 		if (gender == 1)
// 		{
// 			auto meshcloth = sp_character->getMeshByName(manmesh_names[MESH_CLOTHES]);
// 			auto meshpants = sp_character->getMeshByName(manmesh_names[MESH_PANTS]);
// 			meshcloth->setVisible(false);
// 			meshpants->setVisible(false);
// 		}
// 		else
// 		{
// 			auto meshcloth = sp_character->getMeshByName(womanmesh_names[MESH_CLOTHES]);
// 			auto meshgloves = sp_character->getMeshByName(womanmesh_names[MESH_GLOVES]);
// 			auto meshpants = sp_character->getMeshByName(womanmesh_names[MESH_PANTS]);
// 			meshcloth->setVisible(false);
// 			meshgloves->setVisible(false);
// 			meshpants->setVisible(false);
// 		}
		b_clothes->setTag(0);
		b_clothes->setTouchEnabled(false);
	}
	else
	{
		auto i_cloth = b_clothes->getChildByName<ImageView*>("image_equip");
		i_cloth->loadTexture(getItemIconPath(info->clothes->iid));
		i_cloth->setVisible(true);
		b_clothes->setTag(info->clothes->iid);
		b_clothes->setVisible(true);
		b_clothes->setBright(false);
		b_clothes->setTouchEnabled(false);
		i_cloth->setTouchEnabled(true);
		i_cloth->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
	}
//帽子
	if (info->head == nullptr)
	{
// 		if (gender == 1)
// 		{
// 			auto mesh = sp_character->getMeshByName(manmesh_names[MESH_HATS]);
// 			mesh->setVisible(false);
// 		}
// 		else
// 		{
// 			auto mesh = sp_character->getMeshByName(womanmesh_names[MESH_HATS]);
// 			mesh->setVisible(false);
// 		}
		b_hat->setTag(0);
		b_hat->setTouchEnabled(false);
	}
	else
	{
		auto i_hat = b_hat->getChildByName<ImageView*>("image_equip");
		i_hat->loadTexture(getItemIconPath(info->head->iid));
		i_hat->setVisible(true);
		b_hat->setTag(info->head->iid);
		b_hat->setBright(false);
		b_hat->setTouchEnabled(false);
		i_hat->setTouchEnabled(true);
		i_hat->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
	}
	if (info->shoe == nullptr)
	{
// 		if (gender == 1)
// 		{
// 			auto mesh = sp_character->getMeshByName(manmesh_names[MESH_SHOES]);
// 			mesh->setVisible(false);
// 		}
// 		else
// 		{
// 			auto mesh = sp_character->getMeshByName(womanmesh_names[MESH_SHOES]);
// 			mesh->setVisible(false);
// 		}
		b_shoes->setTouchEnabled(false);
	}
	else
	{
		auto i_shoes = b_shoes->getChildByName<ImageView*>("image_equip");
		i_shoes->loadTexture(getItemIconPath(info->shoe->iid));
		i_shoes->setVisible(true);
		b_shoes->setTag(info->shoe->iid);
		b_shoes->setBright(false);
		b_shoes->setTouchEnabled(false);
		i_shoes->setTouchEnabled(true);
		i_shoes->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
	}

	if (info->access == nullptr)
	{
// 		if (gender == 1)
// 		{
// 			auto mesh = sp_character->getMeshByName(manmesh_names[MESH_NECKLACE]);
// 			mesh->setVisible(false);
// 		}
// 		else
// 		{
// 			auto mesh = sp_character->getMeshByName(womanmesh_names[MESH_NECKLACE]);
// 			mesh->setVisible(false);
// 		}
		b_necklace->setTag(0);
		b_necklace->setTouchEnabled(false);
	}
	else
	{
		auto i_necklace = b_necklace->getChildByName<ImageView*>("image_equip");
		i_necklace->loadTexture(getItemIconPath(info->access->iid));
		i_necklace->setVisible(true);
		b_necklace->setTag(info->access->iid);
		b_necklace->setBright(false);
		b_necklace->setTouchEnabled(false);
		i_necklace->setTouchEnabled(true);
		i_necklace->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));

	}
	if (info->weapon != nullptr)
	{
// 		auto sp_weapon = Sprite3D::create("Character3D/weaponA.c3b");
// 		auto skeleton = sp_character->getSkeleton();
// 		auto bone = skeleton->getBoneByName("joint_weapon_R_100_1");
// 		auto attach = sp_character->getAttachNode(bone->getName());
// 		//添加武器
// 		if (attach->getChildren().size() == 0)
// 		{
// 			sp_weapon->setCameraMask(8, true);
// 			attach->addChild(sp_weapon);
// 		}
		auto i_weapon = b_weapon->getChildByName<ImageView*>("image_equip");
		i_weapon->loadTexture(getItemIconPath(info->weapon->iid));
		i_weapon->setVisible(true);
		b_weapon->setTag(info->weapon->iid);
		b_weapon->setBright(false);
		b_weapon->setTouchEnabled(false);
		i_weapon->setTouchEnabled(true);
		i_weapon->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
	}
	else
	{
		b_weapon->setTag(0);
		b_weapon->setTouchEnabled(false);
	}
	m_needClose = true;
}
bool UICommon::onTouchBegan(Touch *touch, Event *unused_event)
{
	if (m_operaModel->getName().find("Ship") == 0)
	{
		if (touch->getLocation().x >= 620 && touch->getLocation().x <= 1000 && touch->getLocation().y >= 180 && touch->getLocation().y <= 475)
		{
			m_touchBegan = touch->getLocation();
		}
		return true;
	}
	else
	{
		if (touch->getLocation().x >= 280 && touch->getLocation().x <= 630 && touch->getLocation().y >= 140 && touch->getLocation().y <= 580)
		{
			m_touchBegan = touch->getLocation();
		}
		return true;
	}
	return false;
}
void UICommon::onTouchMoved(Touch *touch, Event *unused_event)
{
	Point p1 = touch->getLocation();
	if (m_touchBegan != Vec2(0, 0))
	{
		if (p1.x - m_touchBegan.x > 0)
		{
			m_operaModel->setRotation3D(m_operaModel->getRotation3D() + Vec3(0, 5, 0));
		}
		else if (p1.x - m_touchBegan.x < 0)
		{
			m_operaModel->setRotation3D(m_operaModel->getRotation3D() + Vec3(0, -5, 0));
		}
		m_touchBegan = p1;
		log("hehe");
		Vec3 pos = m_operaModel->getPosition3D();
		Vec3 roat = m_operaModel->getRotation3D();
		log("x ::%.2f, y :: %.2f , z :: %.2f", pos.x, pos.y, pos.z);
		log("x ::%.2f, y :: %.2f , z :: %.2f", roat.x, roat.y, roat.z);
	}
}
void UICommon::onTouchEnded(Touch *touch, Event *unused_event)
{
	m_touchBegan = Vec2(0, 0);
}

void UICommon::confirmCharacterFaceAndHair(Sprite3D * character)
{
	std::string face;
	std::string hair;
	if (m_curuserInfo->icon <= 4)
	{
		face = "man_face_M_100";
		hair = "man_hair_M_100";
	}
	else 
	{
		face = "woman_face_M_100";
		hair = "woman_hair_M_100";
	}

	for (int i = 0; i < character->getMeshCount(); i++)
	{
		auto mesh = character->getMeshByIndex(i);
		if (mesh->getName().find(face) == 0)
		{
			std::string str_face = StringUtils::format("%s_%d", face.c_str(), m_curuserInfo->icon);
			log("Mesh name:%s", str_face.c_str());
			if (mesh->getName().find(str_face) == 0)
			{
				mesh->setVisible(true);
				log("LOGTEST:head is show");
				log("Mesh name:%s", str_face.c_str());
			}
			else
			{
				mesh->setVisible(false);
			}
		}
		if (mesh->getName().find(hair) == 0)
		{
			std::string str_hair = StringUtils::format("%s_%d", hair.c_str(), m_curuserInfo->icon);
			log("Mesh name:%s", str_hair.c_str());
			if (mesh->getName().find(str_hair) == 0)
			{
				mesh->setVisible(true);
				log("LOGTEST:head is show");
				log("Mesh name:%s", str_hair.c_str());
			}
			else
			{
				mesh->setVisible(false);
			}
		}
	}
}
void UICommon::convertToVector(std::string &_src, std::vector<int> &_des)
{
	if (_src.empty())
	{
		return;
	}
	if (_des.empty())
	{
		return;
	}
	int prePos = 0;
	int flag = 0;
	for (size_t i = 0; i != std::string::npos; i++, flag++)
	{
		prePos = i;
		i = _src.find(",", prePos);
		if (i == std::string::npos || i == _src.length() - 1)
		{
			break;
		}
		int id = atoi(_src.substr(prePos, i).c_str());
		_des[flag] = id;
	}
	_des[flag] = (atoi(_src.substr(prePos).c_str()));
}
int UICommon::getEquipIID(const int nId, GetPlayerEquipShipInfoResult * m_pShipInfoResult)
{

	for (auto i = 0; i < m_pShipInfoResult->n_equipdetails; i++)
	{
		if (nId == m_pShipInfoResult->equipdetails[i]->id)
		{
			return m_pShipInfoResult->equipdetails[i]->iid;
		}
	}
	return 0;
}
EquipDetailInfo * UICommon::getEquipDetailInfo(const int iid)
{
	for (auto i = 0; i < m_ShipInfoResult->n_equipdetails; i++)
	{
		if (iid == m_ShipInfoResult->equipdetails[i]->iid)
		{
			return m_ShipInfoResult->equipdetails[i];
		}
	}
	return nullptr;
}
void UICommon::openCestatusView(GetForceCityResult* result)
{
	openView(COMMOM_COCOS_RES[C_VIEW_CESTATUS_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_CESTATUS_CSB]);
	if (view)
	{
		auto panel_guild_detail = view->getChildByName<Widget*>("panel_guild_detail");
		auto label_title = panel_guild_detail->getChildByName<Text*>("label_title");
		auto button_down = panel_guild_detail->getChildByName<Text*>("button_down");
		button_down->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
		auto image_input_bg = view->getChildByName<ImageView*>("image_input_bg");
		image_input_bg->setTouchEnabled(true);
		image_input_bg->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
		auto image_flag = image_input_bg->getChildByName<ImageView*>("image_flag");
		auto label_city = image_input_bg->getChildByName<Text*>("label_city");
		auto image_press = panel_guild_detail->getChildByName<Text*>("image_press");
		auto button_dropdown = panel_guild_detail->getChildByName<Text*>("button_dropdown");
		button_dropdown->setTouchEnabled(false);
		image_press->setVisible(false);
		if (n_ForceId > -1)
		{
			image_flag->loadTexture(getCountryIconPath(n_ForceId));
			label_city->setString(SINGLE_SHOP->getNationInfo()[n_ForceId]);
		}
		else
		{
			image_flag->loadTexture(getCountryIconPath(SINGLE_HERO->m_iCurCityNation));
			label_city->setString(SINGLE_SHOP->getNationInfo()[SINGLE_HERO->m_iCurCityNation]);
		}
		auto listview_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(panel_guild_detail, "listview_content"));
		auto panel_city_1 = dynamic_cast<Layout*>(Helper::seekWidgetByName(panel_guild_detail, "panel_city_1"));
		for (int i = 0; i <result->n_cities; i++)
		{
			auto panel_city_clone = panel_city_1->clone();
			auto label_population_num = panel_city_clone->getChildByName<Text*>("label_population_num");
			auto label_trade_num = panel_city_clone->getChildByName<Text*>("label_trade_num");
			auto label_manufacture_num = panel_city_clone->getChildByName<Text*>("label_manufacture_num");
			label_manufacture_num->setString(StringUtils::format("%d", result->cities[i]->manufacture));
			auto label_city = panel_city_clone->getChildByName<Text*>("label_city");
			auto label_city_0 = panel_city_clone->getChildByName<Text*>("label_city_0");
			label_population_num->setString(StringUtils::format("%d", result->cities[i]->population));
			label_trade_num->setString(StringUtils::format("%d", result->cities[i]->trade));
			label_city->setString(SINGLE_SHOP->getCitiesInfo()[result->cities[i]->cityid].name);
			label_city_0->setString(SINGLE_SHOP->getTipsInfo()[getPortTypeOrInfo(2, result->cities[i]->cityid).c_str()]);
			auto image_city_population = panel_city_clone->getChildByName<ImageView*>("image_city_population");
			auto image_city_trade = panel_city_clone->getChildByName<ImageView*>("image_city_trade");
			auto image_city_manufacture = panel_city_clone->getChildByName<ImageView*>("image_city_manufacture");
			image_city_population->setTouchEnabled(true);
			image_city_trade->setTouchEnabled(true);
			image_city_manufacture->setTouchEnabled(true);
			image_city_population->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
			image_city_population->setTag(IMAGE_ICON_POPULATION + IMAGE_INDEX2);
			image_city_trade->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
			image_city_trade->setTag(IMAGE_ICON_TRADE + IMAGE_INDEX2);
			image_city_manufacture->addTouchEventListener(CC_CALLBACK_2(UICommon::imageCallEvent, this));
			image_city_manufacture->setTag(IMAGE_ICON_MANUFACTURE + IMAGE_INDEX1);
			listview_content->pushBackCustomItem(panel_city_clone);
		}
		listview_content->removeItem(0);
		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
		addListViewBar(listview_content, image_pulldown);
	}
}
void UICommon::openManufactureView(GetForceCityResult* result)
{
	openView(COMMOM_COCOS_RES[C_VIEW_DIALOG_CITYINFOBOUNS_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_DIALOG_CITYINFOBOUNS_CSB]);
	auto listview_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_content"));
	auto panel_lise_title = dynamic_cast<Layout*>(Helper::seekWidgetByName(view, "panel_lise_title"));
	auto panel_city_1 = dynamic_cast<Layout*>(Helper::seekWidgetByName(view, "panel_city_1"));
	auto label_city = panel_city_1->getChildByName<Text*>("label_city");
	auto label_city_0 = panel_city_1->getChildByName<Text*>("label_city_0");
	auto label_city_0_0 = panel_city_1->getChildByName<Text*>("label_city_0_0");
	for (size_t i = 0; i <result->n_cities; i++)
	{
		auto panel_clone = panel_city_1->clone();
		auto label_city = panel_clone->getChildByName<Text*>("label_city");
		auto label_city_0 = panel_clone->getChildByName<Text*>("label_city_0");
		auto label_city_0_0 = panel_clone->getChildByName<Text*>("label_city_0_0");
		label_city->setString(SINGLE_SHOP->getCitiesInfo()[result->cities[i]->cityid].name);
		label_city_0->setString(StringUtils::format("%d", result->cities[i]->manufacture));
		int a = atoi((label_city_0->getString()).c_str());
		auto num = a * result->bonus_rate;
		char* string = nullptr;
		//itoa(num, string, 10);
		//label_city_0_0->setText(string);
	}
		listview_content->removeItem(1);

		auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
		addListViewBar(listview_content, image_pulldown);
}

void UICommon::flushContryListview()
{
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_CESTATUS_CSB]);
	if (view)
	{
		auto panel_guild_detail = view->getChildByName<Widget*>("panel_guild_detail");
		auto panel_expand = panel_guild_detail->getChildByName<Widget*>("panel_expand");
		panel_expand->setTouchEnabled(true);
		panel_expand->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
		auto button_listdown = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_expand, "button_listdown"));
		button_listdown->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func, this));
		auto button_droplistdown = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_expand, "button_droplistdown"));
		button_droplistdown->setTouchEnabled(false);
		if (m_ListClose)
		{
			panel_expand->setVisible(false);
			m_ListClose = false;
		}
		else
		{
			button_listdown->setBright(false);
			panel_expand->setVisible(true);
			m_ListClose = true;
		}
		auto listview_content = panel_expand->getChildByName<ListView*>("listview_content");
		listview_content->removeAllChildrenWithCleanup(true);
		auto panel_city_1 = panel_expand->getChildByName<Layout*>("panel_city_1");
		auto image_expand_bg = panel_expand->getChildByName<Layout*>("image_expand_bg");
		auto image_flag = image_expand_bg->getChildByName<ImageView*>("image_flag");
		auto label_city = image_expand_bg->getChildByName<Text*>("label_city");
		if (n_ForceId > -1)
		{
			image_flag->loadTexture(getCountryIconPath(n_ForceId));
			label_city->setString(SINGLE_SHOP->getNationInfo()[n_ForceId]);
		}
		else
		{
			image_flag->loadTexture(getCountryIconPath(SINGLE_HERO->m_iCurCityNation));
			label_city->setString(SINGLE_SHOP->getNationInfo()[SINGLE_HERO->m_iCurCityNation]);
		}
		panel_city_1->setVisible(false);
		std::map<int, std::string>& m_nation = SINGLE_SHOP->getNationInfo();
		std::map<int, std::string>::iterator m_nation_Iter;
		for (m_nation_Iter = m_nation.begin(); m_nation_Iter != m_nation.end(); m_nation_Iter++)
		{
			auto panel_clone = panel_city_1->clone();
			panel_clone->setName("panel_clone");
			auto label_city = panel_clone->getChildByName<Text*>("label_city");
			auto image_flag = panel_clone->getChildByName<ImageView*>("image_flag");
			auto button_down = panel_clone->getChildByName<Button*>("button_down");
			panel_clone->setVisible(true);
			panel_clone->setTag(m_nation_Iter->first);
			panel_clone->addTouchEventListener(CC_CALLBACK_2(UICommon::menuCall_func,this));
			label_city->setString(m_nation_Iter->second);
			image_flag->loadTexture(getCountryIconPath(m_nation_Iter->first));
			listview_content->pushBackCustomItem(panel_clone);
		}
		listview_content->removeItem(9);
		auto image_pulldown_0 = view->getChildByName<ImageView*>("image_pulldown_0");
		auto button_pulldown = image_pulldown_0->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown_0->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
		addListViewBar(listview_content, image_pulldown_0);
	}
}
std::string UICommon::getPortTypeOrInfo(int port, int citiesId)
{
	int portType = SINGLE_SHOP->getCitiesInfo()[citiesId].port_type;
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
	}
	else if (port == 1)
	{
		content = StringUtils::format("TIP_PORT_INFO_%d", type);
	}
	else if (port == 2)
	{
		content = StringUtils::format("TIP_PORT_TYPE_%d", type);
	}
	return content;
}

ShipModel::ShipModel()
{
	m_flag_index = 0;
	m_oars_index = 0;
}
ShipModel::~ShipModel()
{

}
bool ShipModel::init(int modelId)
{
	if (!Sprite3D::initWithFile(getShipTestModePath_3D(modelId)))
	{
		return false;
	}
	addSail(this);
	return true;
}
void ShipModel::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	if (isVisitableByVisitingCamera()){
		Mat4 transform = parentTransform;
#if  0
		std::vector<Node*> myParents;
		Node* start = this->getParent();
		while (start){
			myParents.push_back(start);
			start = start->getParent();
		}
		std::vector<Node*>::iterator iter;
		for (iter = myParents.end() - 1; iter != myParents.begin() - 1; iter--)
		{
			Node*node = *iter;
// 			parentFlags = node->processParentFlags(transform, parentFlags);
// 			transform = node->getTransform;
		}
#endif
		Sprite3D::visit(renderer, transform, parentFlags);
	}
	else{

	}
}
ShipModel* ShipModel::create(int modelId)
{
	auto model = new ShipModel;
	CC_RETURN_IF(model);
	if (model && model->init(modelId))
	{
		model->autorelease();
		return model;
	}
	CC_SAFE_DELETE(model);
	return nullptr;
}
void ShipModel::addSail(Sprite3D * p_Sender)
{
	auto skele = p_Sender->getSkeleton();
	if (skele)
	{
		int boneNum = skele->getBoneCount();
		for (int i = 0; i < boneNum; i++)
		{
			auto bone = skele->getBoneByIndex(i);
			auto attach = p_Sender->getAttachNode(bone->getName());
			attach->addChild(confirmSail(bone));
		}
	}
}

Sprite3D * ShipModel::confirmSail(Bone3D * bone)
{
	auto name = bone->getName();
	std::vector<std::string> nameInfos;
	nameInfos = analysisName(name);
	Sprite3D * sp = Sprite3D::create();
	int a = atoi(nameInfos.at(3).c_str());
	float scaleValue = float(a) / 100;
	if (nameInfos.at(1).find("flag") != -1)
	{

		sp = Sprite3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		sp->setScale(scaleValue);
		sp->setName(nameInfos.at(0) + "_" + nameInfos.at(1));
		auto animation = Animation3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		if (animation)
		{
			auto animate = Animate3D::create(animation);
			sp->runAction(RepeatForever::create(animate));
		}
		for (int a = 0; a < sp->getMeshCount(); a++)
		{
			auto mesh = sp->getMeshByIndex(a);
			if (a == m_flag_index)
			{
				mesh->setVisible(true);
				continue;
			}
			mesh->setVisible(false);
		}
	}
	else if (nameInfos.at(1).find("sail") != -1)
	{
		sp = Sprite3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		auto animation = Animation3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		sp->setScale(scaleValue);
		if (animation)
		{
			auto animate = Animate3D::create(animation);
			sp->runAction(RepeatForever::create(animate));
		}
	}
	else if (nameInfos.at(1).find("oar") != -1)
	{
		sp = Sprite3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		auto animation = Animation3D::create(StringUtils::format("Sprite3DTest/%s.c3b", nameInfos.at(1).c_str()));
		sp->setScale(scaleValue);
		if (animation)
		{
			auto animate = Animate3D::create(animation);
			sp->runAction(RepeatForever::create(animate));

		}
		sp->setName(nameInfos.at(0) + "_" + nameInfos.at(1));
		/*		m_oars.pushBack(sp);*/
		for (int a = 0; a < sp->getMeshCount(); a++)
		{
			auto mesh = sp->getMeshByIndex(a);
			if (a == m_oars_index)
			{
				mesh->setVisible(true);
				continue;
			}
			mesh->setVisible(false);
		}
	}
	//	sp->setTag(2);
	sp->setCameraMask(8);
	return sp;
}
std::vector<std::string> ShipModel::analysisName(std::string name)
{
	std::string str = name;
	std::vector<std::string> nameInfos;
	while (str.find_first_of("_") != -1)
	{
		int a = str.find_first_of("_");
		std::string str1 = str.substr(0, a);
		nameInfos.push_back(str1);
		str = str.substr(a + 1, str.npos);
		if (str.find_first_of("_") == -1)
		{
			nameInfos.push_back(str);
		}
	}
	return nameInfos;
}
