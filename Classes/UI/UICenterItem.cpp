#include "UICenterItem.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UIVoyageSlider.h"

UICenterItem::UICenterItem():
	m_pParent(nullptr),
	m_pResult(nullptr),
	m_pMinorButton(nullptr),
	m_nItemIndex(-1)
{
	EquipNotUsing.clear();
	EquipUsing.clear();
}

UICenterItem::~UICenterItem()
{
	if (m_pResult)
	{
		get_personal_item_result__free_unpacked(m_pResult,0);
	}

	EquipNotUsing.clear();
	EquipUsing.clear();
}

UICenterItem* UICenterItem::createItem(UIBasicLayer *parent)
{
	UICenterItem* ci = new UICenterItem;
	if (ci)
	{
		ci->m_pParent = parent;
		ci->autorelease();
		return ci;
	}
	CC_SAFE_DELETE(ci);
	return nullptr;
}

void UICenterItem::openCreateItem()
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_INVENTORY_CSB]);
	m_pMinorButton = nullptr;
	changeMinorButtonState(nullptr);
	ProtocolThread::GetInstance()->getPersonalItems(ITEM_TYPE_GOOD,0,UILoadingIndicator::create(m_pParent));
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_INVENTORY_CSB]);
	auto p_button = view->getChildByName<Widget*>("panel_item_two_butter");
	auto l_button = p_button->getChildByName<cocos2d::ui::ScrollView*>("listview_two_butter");
	l_button->addEventListener(CC_CALLBACK_2(UICenterItem::scrollButtonEvent, this));
	auto b_more = view->getChildByName<Button*>("button_more");
	b_more->setVisible(true);
}

void UICenterItem::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* target = dynamic_cast<Widget*>(pSender);
		std::string name = target->getName();
		buttonEvent(target,name);
	}
}

void UICenterItem::buttonEvent(Widget *pSender,std::string name)
{
	//丢弃物品
	if (isButton(button_drop)) 
	{
		//SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		showDropItem();
		return;
	}
	//图片解释
	if (isButton(image_personal_weight) || isButton(image_ship_weight))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(m_pParent);
		UICommon::getInstance()->flushImageDetail(pSender);
		return;
	}
	//点击物品时
	if (isButton(button_items_))  
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nItemIndex = pSender->getTag();
		UICommon::getInstance()->openCommonView(m_pParent);
		bool b_isHide = false;
		switch (m_pResult->itemtype)
		{
		case ITEM_TYPE_GOOD:	
			UICommon::getInstance()->flushGoodsDetail(m_pResult->goods[m_nItemIndex],m_pResult->goods[m_nItemIndex]->goodsid,false);
			break;
		case ITEM_TYPE_SHIP:
			UICommon::getInstance()->flushShipDetail(m_pResult->ships[m_nItemIndex],m_pResult->ships[m_nItemIndex]->sid,false);
			break;
		case ITEM_TYPE_SHIP_EQUIP:
		{			
				for (int i = 0; i < EquipUsing.size(); i++)
				{
					if (EquipUsing.at(i) == m_nItemIndex)
					{
						b_isHide = true;
						break;
					}
				}
				UICommon::getInstance()->flushEquipsDetail(m_pResult->equipments[m_nItemIndex], m_pResult->equipments[m_nItemIndex]->equipmentid, b_isHide);
				break;
		}
		
		case ITEM_TYPE_PORP:
			UICommon::getInstance()->flushItemsDetail(m_pResult->equipments[m_nItemIndex],m_pResult->equipments[m_nItemIndex]->equipmentid,false);
			break;
		case ITEM_TYPE_ROLE_EQUIP:
		{
			for (int i = 0; i < EquipUsing.size(); i++)
			{
				if (EquipUsing.at(i) == m_nItemIndex)
				{
					b_isHide = true;
					break;
				}
			}
			UICommon::getInstance()->flushItemsDetail(m_pResult->equipments[m_nItemIndex], m_pResult->equipments[m_nItemIndex]->equipmentid, b_isHide);
			break;
		}
			
		case ITEM_TYPE_DRAWING:
			UICommon::getInstance()->flushDrawingDetail(m_pResult->drawings[m_nItemIndex],m_pResult->drawings[m_nItemIndex]->iid,false);
			break;
		case ITEM_TYPE_SPECIAL:
			UICommon::getInstance()->flushSpecialDetail(false);
			break;
		default:
			break;
		}
	}
	//增加背包容量按钮
	if (isButton(button_add_1))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto bag_info = SINGLE_SHOP->getBagExpandFeeInfo();
		size_t i = 0;
		for (; i < bag_info.size(); i++)
		{
			if (bag_info[i].capacity == m_pResult->maxpackagesize)
			{
				break;
			}
		}
		if (i == bag_info.size() - 1)
		{
			UIInform::getInstance()->openInformView(m_pParent);
			UIInform::getInstance()->openConfirmYes("TIP_CENTER_ITEM_BAG_EXPAND_MAX");
		}
		else
		{
			int cost_num = bag_info[i + 1].fee;
			UICommon::getInstance()->openCommonView(m_pParent);
			UICommon::getInstance()->flushSilverConfirmView("TIP_CENTER_ITEM_BAG_EXPAND_TITLE",
				"TIP_CENTER_ITEM_BAG_EXPAND_CONTENT", cost_num);
		}
		return;
	}
	//增加背包容量确认
	if (isButton(button_s_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		ProtocolThread::GetInstance()->expandPackageSize(UILoadingIndicator::create(m_pParent));
		return;
	}

	//物品之间的切换
	if (isButton(button_goods) || isButton(button_ships) || isButton(button_shipequipment) || isButton(button_human_equipment)
		|| isButton(button_drawings) || isButton(button_miscs) || isButton(button_special))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		int index = pSender->getTag() - 10;
		changeMinorButtonState(pSender);
		ProtocolThread::GetInstance()->getPersonalItems(index,0,UILoadingIndicator::create(m_pParent));
		return;
	}
	//技能信息
	if (isButton(image_skill_bg))
	{
		UICommon::getInstance()->openCommonView(m_pParent);
		UICommon::getInstance()->flushSkillView(m_vSkillDefine.at(pSender->getTag()));
		return;
	}
}

void UICenterItem::onDropItem(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* target = dynamic_cast<Widget*>(pSender);
		std::string name = target->getName();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//丢弃物品确认
		if (isButton(button_yes))
		{
			
			auto sslider = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
			switch (m_pResult->itemtype)
			{
			case ITEM_TYPE_GOOD:
				{
					HatchItemsDefine* itemDefine = m_pResult->goods[m_nItemIndex];
					ProtocolThread::GetInstance()->destoryItems(itemDefine->goodsid,sslider->getCurrentNumber(),m_pResult->itemtype,UILoadingIndicator::create(m_pParent));
					break;
				}
			case ITEM_TYPE_SHIP:
				{
					ShipDefine* itemDefine = m_pResult->ships[m_nItemIndex];
					ProtocolThread::GetInstance()->destoryItems(itemDefine->id,sslider->getCurrentNumber(),m_pResult->itemtype,UILoadingIndicator::create(m_pParent));
					break;
				}
			case ITEM_TYPE_SHIP_EQUIP:
			case ITEM_TYPE_PORP:
			case ITEM_TYPE_ROLE_EQUIP:
				{
					EquipmentDefine* itemDefine = m_pResult->equipments[m_nItemIndex];
					ProtocolThread::GetInstance()->destoryItems(itemDefine->uniqueid,sslider->getCurrentNumber(),m_pResult->itemtype,UILoadingIndicator::create(m_pParent));
					break;
				}
			case ITEM_TYPE_DRAWING:
				{
					DrawingItemsDefine* itemDefine = m_pResult->drawings[m_nItemIndex];
					ProtocolThread::GetInstance()->destoryItems(itemDefine->uniqueid,sslider->getCurrentNumber(),m_pResult->itemtype,UILoadingIndicator::create(m_pParent));
					break;
				}
			case ITEM_TYPE_SPECIAL:
				{
					break;
				}
			default:
				break;
			}
		}
		m_pParent->closeView();
	}
}

void UICenterItem::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	switch (msgType)
	{
	case PROTO_TYPE_GetPersonalItemResult:
		{

			GetPersonalItemResult *result = (GetPersonalItemResult *)message;
			if (result->failed == 0)
			{
				m_pResult = result;
				//船只
				if (m_pResult->itemtype == ITEM_TYPE_SHIP)
				{
					updateShipsView();
				}//船装备，人装备
				else if (m_pResult->itemtype == ITEM_TYPE_SHIP_EQUIP || m_pResult->itemtype == ITEM_TYPE_ROLE_EQUIP)
				{
					updateShipEquipView();
				}
				else
				{
					updateItemsView();
				}
				m_vSkillDefine.clear();
				auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_INVENTORY_CSB]);
				auto image_skill = view->getChildByName<ImageView*>("image_skill_bg");
				image_skill->setVisible(false);
				if (result->skill_technique_of_cargo > 0)
				{
					SKILL_DEFINE skillDefine;
					skillDefine.id = SKILL_TECHNIQUE_OF_CARGO;
					skillDefine.lv = result->skill_technique_of_cargo;
					skillDefine.skill_type = SKILL_TYPE_PLAYER;
					skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
					m_vSkillDefine.push_back(skillDefine);
				}

				if (result->captain_skill_technique_of_cargo > 0)
				{
					SKILL_DEFINE skillDefine;
					skillDefine.id = SKILL_CAPTAIN_WAREHOUSING;
					skillDefine.lv = result->captain_skill_technique_of_cargo;
					if (result->captain_skill_technique_of_cargo_captain_type == 1)
					{
						skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
					}
					else
					{
						skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
					}
					skillDefine.icon_id = result->captain_skill_technique_of_cargo_captain_id;
					m_vSkillDefine.push_back(skillDefine);
				}

				for (size_t i = 0; i < m_vSkillDefine.size(); i++)
				{
					if (i == 0)
					{
						image_skill->ignoreContentAdaptWithSize(false);
						image_skill->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
						image_skill->setVisible(true);
						image_skill->setTag(i);
						image_skill->addTouchEventListener(CC_CALLBACK_2(UICenterItem::menuCall_func, this));
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
						image_skill_clone->addTouchEventListener(CC_CALLBACK_2(UICenterItem::menuCall_func, this));
						auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill_clone, "text_item_skill_lv"));
						setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
						image_skill_clone->setPositionX(image_skill->getPositionX() - i*1.1*image_skill->getBoundingBox().size.width);
						view->addChild(image_skill_clone);
					}
				}
			}else
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_GET_ITEM_FAIL");	
			}
			break;
		}
	case PROTO_TYPE_DestoryItemsResult:
		{
			WarehouseOprationResult *result = (WarehouseOprationResult*)message; 		
			if (result->failed == 0) 		
			{ 					
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_DETELE_ITEM_SUCCESS");				
				ProtocolThread::GetInstance()->getPersonalItems(m_pResult->itemtype,0,UILoadingIndicator::create(m_pParent));		
			}else 		
			{ 			
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_OPERATE_FAIL");	
			} 			
			break; 
		}
	case PROTO_TYPE_ExpandPackageSizeResult:
	{
		ExpandPackageSizeResult *result = (ExpandPackageSizeResult*)message;
		if (result->failed == 0)
		{
			UIInform::getInstance()->openInformView(m_pParent);
			UIInform::getInstance()->openViewAutoClose("TIP_CENTER_ITEM_BAG_EXPAND_RESULT");
			m_pResult->maxpackagesize = result->newpackagesize;
			updateBagAndShipCapacity();
			SINGLE_HERO->m_iCoin = result->coin;
			SINGLE_HERO->m_iGold = result->gold;
		}
		else if (result->failed == COIN_NOT_FAIL)
		{
			UIInform::getInstance()->openInformView(m_pParent);
			UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
		}
		break;
	}
	default:
		break;
	
	}
}

void UICenterItem::changeMinorButtonState(Widget *target)
{
	if (!target)
	{
		auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_INVENTORY_CSB]);
		target = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"button_goods"));
	}

	if (m_pMinorButton != nullptr)
	{
		m_pMinorButton->setBright(true);
		m_pMinorButton->setTouchEnabled(true);
		dynamic_cast<Text*>(m_pMinorButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}

	m_pMinorButton = target;
	m_pMinorButton->setBright(false);
	m_pMinorButton->setTouchEnabled(false);
	dynamic_cast<Text*>(m_pMinorButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
}

void UICenterItem::updateItemsView()
{
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_INVENTORY_CSB]);
	auto p_bag = view->getChildByName<Widget*>("panel_bag");
	auto l_item = p_bag->getChildByName<ListView*>("listview_item");
	l_item->removeAllChildrenWithCleanup(true);
	l_item->setItemsMargin(3);
	auto p_title = p_bag->getChildByName<Widget*>("panel_title");
	auto p_city = p_bag->getChildByName<Widget*>("panel_city");
	auto p_items = p_bag->getChildByName<Widget*>("panel_items");
	int n_items = 0;
	auto p_title_clone = p_title->clone();
	auto i_title = p_title_clone->getChildByName<ImageView*>("image_icon");
	auto t_title = p_title_clone->getChildByName<Text*>("label_city");
	switch (m_pResult->itemtype)
	{
	case ITEM_TYPE_GOOD:
	{
		n_items = m_pResult->n_goods;
		i_title->loadTexture("res/shipAttIcon/att_7.png");
		i_title->setColor(Color3B(58, 32, 2));
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CABIN"]);
		l_item->pushBackCustomItem(p_title_clone);
		break;
	}
		
	case ITEM_TYPE_SHIP:
	{
		//updateShipsView()
		break;
	}
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_ROLE_EQUIP:
	{
		//updateShipEquipView()
		break;
	}
	case ITEM_TYPE_PORP:
	{
		n_items = m_pResult->n_equipments;
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BAG"]);
		l_item->pushBackCustomItem(p_title_clone);
		break;
	}
		
	case ITEM_TYPE_DRAWING:
	{
		n_items = m_pResult->n_drawings;
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BAG"]);
		l_item->pushBackCustomItem(p_title_clone);
		break;
	}
		
	case ITEM_TYPE_SPECIAL:
	{
		n_items = m_pResult->n_specials;
		break;
	}
		
	default:
		break;
	}
	i_title->setPositionX(t_title->getPositionX() - i_title->getBoundingBox().size.width - t_title->getBoundingBox().size.width / 2);
	int itemId = 0;
	std::string itemName;
	std::string itemPath;
	int itemAmount = 0;
	int line_num = ceil(n_items/8.0);
	for (int i = 0; i < line_num; i++)
	{
		auto p_items_clone = p_items->clone();
		for (int j = 0; j < 8; j++)
		{
			if (i * 8 + j < n_items)
			{
				auto b_item = p_items_clone->getChildByName<Button*>(StringUtils::format("button_items_%d", j + 1));
				auto i_icon = b_item->getChildByName<ImageView*>("image_goods");
				auto t_num = i_icon->getChildByName<Text*>("text_item_num");
				
				switch (m_pResult->itemtype)
				{
				case ITEM_TYPE_GOOD:
				{
					itemId = m_pResult->goods[i * 8 + j]->goodsid;
					itemAmount = m_pResult->goods[i * 8 + j]->amount;
					t_num->setVisible(true);
					break;
				}				
				case ITEM_TYPE_PORP:
				{
					itemId = m_pResult->equipments[i * 8 + j]->equipmentid;
					itemAmount = m_pResult->equipments[i * 8 + j]->amount;
					t_num->setVisible(true);

					if (m_pResult->equipments[i * 8 + j]->optionalid > 0)
					{
						addStrengtheningIcon(b_item);
					}
					break;
				}				
				case ITEM_TYPE_DRAWING:
				{
					itemId = m_pResult->drawings[i * 8 + j]->iid;
					itemAmount = m_pResult->drawings[i * 8 + j]->amount;
					t_num->setVisible(true);
					break;
				}				
				case ITEM_TYPE_SPECIAL:
				{
					itemId = m_pResult->specials[i * 8 + j]->itemid;
					itemAmount = m_pResult->specials[i * 8 + j]->amount;
					t_num->setVisible(true);
					break;
				}				
				default:
					break;
				}
				getItemNameAndPath(m_pResult->itemtype,itemId,itemName,itemPath);
				i_icon->setVisible(true);
				i_icon->ignoreContentAdaptWithSize(false);
				i_icon->loadTexture(itemPath);
				setTextSizeAndOutline(t_num,itemAmount);
				b_item->setTag(i * 8 + j);
				b_item->setTouchEnabled(true);

				auto i_bgButton = b_item->getChildByName<ImageView*>("image_goods_bg");
				auto i_bgImage = b_item->getChildByName<ImageView*>("image_item_bg_lv");
				setBgButtonFormIdAndType(i_bgButton, itemId, m_pResult->itemtype);
				setBgImageColorFormIdAndType(i_bgImage, itemId, m_pResult->itemtype);
			}else
			{
				break;
			}
		}
		l_item->pushBackCustomItem(p_items_clone);
	}
	updateBagAndShipCapacity();

	auto image_pulldown_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown_item"));
	auto button_pulldown = image_pulldown_item->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX((image_pulldown_item->getBoundingBox().size.width / 2 - image_pulldown_item->getBoundingBox().size.width / 2) + 2);
	addListViewBar(l_item, image_pulldown_item);
}

void UICenterItem::updateShipEquipView()
{
	EquipUsing.clear();
	EquipNotUsing.clear();
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_INVENTORY_CSB]);
	auto p_bag = view->getChildByName<Widget*>("panel_bag");
	auto l_item = p_bag->getChildByName<ListView*>("listview_item");
	l_item->removeAllChildrenWithCleanup(true);
	l_item->setItemsMargin(3);
	auto p_title = p_bag->getChildByName<Widget*>("panel_title");
	auto p_city = p_bag->getChildByName<Widget*>("panel_city");
	auto p_items = p_bag->getChildByName<Widget*>("panel_items");
	int n_items = 0;//数量

	 	for (int i = 0; i < m_pResult->n_equipments;i++)
	 	{
	 		if (!m_pResult->equipments[i]->amount)
	 		{
	 			EquipUsing.push_back(i);
	 		}
	 		else
	 		{
				EquipNotUsing.push_back(i);
	 		}
	 	}
		if (!EquipUsing.size() && !EquipNotUsing.size())
		{
			auto p_title_clone = p_title->clone();
			auto i_title = p_title_clone->getChildByName<ImageView*>("image_icon");
			auto t_title = p_title_clone->getChildByName<Text*>("label_city");
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BAG"]);
			i_title->setPositionX(t_title->getPositionX() - i_title->getBoundingBox().size.width - t_title->getBoundingBox().size.width / 2);
			l_item->pushBackCustomItem(p_title_clone);
		}
	if (EquipUsing.size()!=0)//正在使用的装备
	{
		auto p_title_clone_using = p_title->clone();
		auto i_title = p_title_clone_using->getChildByName<ImageView*>("image_icon");
		auto t_title = p_title_clone_using->getChildByName<Text*>("label_city");
		i_title->setVisible(false);
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PLAYERCENTER_EQUIP_HAVE_ON"]);
		l_item->pushBackCustomItem(p_title_clone_using);

	    n_items = EquipUsing.size();
		int itemId = 0;
		std::string itemName;
		std::string itemPath;
		int itemAmount = 0;
		int line_num = ceil(n_items / 8.0);
		for (int i = 0; i < line_num; i++)
		{
			auto p_items_clone = p_items->clone();
			for (int j = 0; j < 8; j++)
			{
				if (i * 8 + j < n_items)
				{
					auto b_item = p_items_clone->getChildByName<Button*>(StringUtils::format("button_items_%d", j + 1));
					auto i_icon = b_item->getChildByName<ImageView*>("image_goods");
					auto t_num = i_icon->getChildByName<Text*>("text_item_num");

					itemId = m_pResult->equipments[EquipUsing.at(i * 8 + j)]->equipmentid;
					itemAmount = m_pResult->equipments[EquipUsing.at(i * 8 + j)]->amount;
					t_num->setVisible(false);
					if (m_pResult->equipments[EquipUsing.at(i * 8 + j)]->optionalid > 0)
					{
						addStrengtheningIcon(b_item);
					}
					 m_pResult->equipments[EquipUsing.at(i * 8 + j)]->maxdurable;
					if (m_pResult->equipments[EquipUsing.at(i * 8 + j)]->durable <= SHIP_EQUIP_BROKEN||
						m_pResult->equipments[EquipUsing.at(i * 8 + j)]->durable <= m_pResult->equipments[EquipUsing.at(i * 8 + j)]->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
					{
						addequipBrokenIcon(b_item);
						setBrokenEquipRed(i_icon);
					}
								
					getItemNameAndPath(m_pResult->itemtype, itemId, itemName, itemPath);
					i_icon->setVisible(true);
					i_icon->ignoreContentAdaptWithSize(false);
					i_icon->loadTexture(itemPath);
					setTextSizeAndOutline(t_num, itemAmount);
					b_item->setTag(EquipUsing.at(i * 8 + j));
					b_item->setTouchEnabled(true);

					auto i_bgButton = b_item->getChildByName<ImageView*>("image_goods_bg");
					auto i_bgImage = b_item->getChildByName<ImageView*>("image_item_bg_lv");
					setBgButtonFormIdAndType(i_bgButton, itemId, m_pResult->itemtype);
					setBgImageColorFormIdAndType(i_bgImage, itemId, m_pResult->itemtype);
				}
				else
				{
					break;
				}
			}
			l_item->pushBackCustomItem(p_items_clone);
		}
	}
	//不使用的装备
	if (EquipNotUsing.size() != 0)
	{
		auto p_title_clone_notusing = p_title->clone();
		auto i_title = p_title_clone_notusing->getChildByName<ImageView*>("image_icon");
		auto t_title = p_title_clone_notusing->getChildByName<Text*>("label_city");
		i_title->setVisible(true);
		i_title->setPositionX(t_title->getPositionX() - i_title->getBoundingBox().size.width - t_title->getBoundingBox().size.width / 2);
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BAG"]);
		l_item->pushBackCustomItem(p_title_clone_notusing);

		 n_items = EquipNotUsing.size();
		int itemId = 0;
		std::string itemName;
		std::string itemPath;
		int itemAmount = 0;
		int line_num = ceil(n_items / 8.0);
		for (int i = 0; i < line_num; i++)
		{
			auto p_items_clone = p_items->clone();
			for (int j = 0; j < 8; j++)
			{
				if (i * 8 + j < n_items)
				{
					auto b_item = p_items_clone->getChildByName<Button*>(StringUtils::format("button_items_%d", j + 1));
					auto i_icon = b_item->getChildByName<ImageView*>("image_goods");
					auto t_num = i_icon->getChildByName<Text*>("text_item_num");
					
					itemId = m_pResult->equipments[EquipNotUsing.at(i * 8 + j)]->equipmentid;
					itemAmount = m_pResult->equipments[EquipNotUsing.at(i * 8 + j)]->amount;
					t_num->setVisible(true);
					if (m_pResult->equipments[EquipNotUsing.at(i * 8 + j)]->optionalid > 0)
					{
						addStrengtheningIcon(b_item);
					}
					if (m_pResult->equipments[EquipNotUsing.at(i * 8 + j)]->durable <= SHIP_EQUIP_BROKEN||
						m_pResult->equipments[EquipNotUsing.at(i * 8 + j)]->durable <= m_pResult->equipments[EquipNotUsing.at(i * 8 + j)]->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
					{
						addequipBrokenIcon(b_item);
						setBrokenEquipRed(i_icon);
					}
					getItemNameAndPath(m_pResult->itemtype, itemId, itemName, itemPath);
					i_icon->setVisible(true);
					i_icon->ignoreContentAdaptWithSize(false);
					i_icon->loadTexture(itemPath);
					setTextSizeAndOutline(t_num, itemAmount);
					b_item->setTag(EquipNotUsing.at(i * 8 + j));
					b_item->setTouchEnabled(true);

					auto i_bgButton = b_item->getChildByName<ImageView*>("image_goods_bg");
					auto i_bgImage = b_item->getChildByName<ImageView*>("image_item_bg_lv");
					setBgButtonFormIdAndType(i_bgButton, itemId, m_pResult->itemtype);
					setBgImageColorFormIdAndType(i_bgImage, itemId, m_pResult->itemtype);
				}
				else
				{
					break;
				}
			}
			l_item->pushBackCustomItem(p_items_clone);
		}
	}
	updateBagAndShipCapacity();
	auto image_pulldown_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown_item"));
	auto button_pulldown = image_pulldown_item->getChildByName<Button*>("button_pulldown");
    button_pulldown->setPositionX((image_pulldown_item->getBoundingBox().size.width / 2 - image_pulldown_item->getBoundingBox().size.width / 2)+2);
	addListViewBar(l_item, image_pulldown_item);
}
void UICenterItem::updateShipsView()
{
	bool b_IsExistFleetShip = false;//船坞是否有船
	bool b_IsExistCatchShip = false;//船坞是否有捕获的船
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_INVENTORY_CSB]);
	auto p_bag = view->getChildByName<Widget*>("panel_bag");
	auto l_item = p_bag->getChildByName<ListView*>("listview_item");
	l_item->removeAllChildrenWithCleanup(true);
	l_item->setItemsMargin(3);
	auto p_title = p_bag->getChildByName<Widget*>("panel_title");
	auto p_city = p_bag->getChildByName<Widget*>("panel_city");
	auto p_ship = p_bag->getChildByName<Widget*>("panel_ship");
	auto p_goods = p_bag->getChildByName<Widget*>("panel_items");

	auto p_title_clone = p_title->clone();
	auto i_title = p_title_clone->getChildByName<ImageView*>("image_icon");
	auto t_title = p_title_clone->getChildByName<Text*>("label_city");
	i_title->setVisible(false);
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ITEM_SHIP"]);
	
	std::map<int, std::vector<int>> shipIndex;
	std::map<int,std::vector<ShipDefine*>> ships;
	for (int i = 0; i < m_pResult->n_ships; i++)
	{
		int cid = 0;
		//cid = 0 ;fleet中的船  cid =1 捕获船只 其它城市id+1
		if (m_pResult->ships[i]->position == -1)
		{
			cid = m_pResult->ships[i]->city_id + 1;	
		}
		else if (m_pResult->ships[i]->position == -10)
		{
			cid = 1;
		}
		else
		{
			cid = 0;
		}
		ships[cid].push_back(m_pResult->ships[i]);
		shipIndex[cid].push_back(i);
	}

	auto &citysinfo = SINGLE_SHOP->getCitiesInfo();
	std::string itemName;
	std::string itemPath;
	int index = -1;
	Widget* p_items;
	int line_grad = 8;
	for (auto it = ships.begin(); it != ships.end();it++)
	{
		auto &oneCity = (*it).second;
		int n_ship = oneCity.size();
		int n_line = ceil(n_ship/8.0);

		auto p_city_clone = p_city->clone();//其它城市的船
		auto t_city = p_city_clone->getChildByName<Text*>("label_city");
		std::string st_city;
		//船坞有船
		if ((*it).first == 0)
		{
			line_grad = 5;
			p_items = p_ship;
			b_IsExistFleetShip = true;
			p_city_clone = p_title->clone();
			auto i_title = p_city_clone->getChildByName<ImageView*>("image_icon");
			t_city = p_city_clone->getChildByName<Text*>("label_city");
			i_title->setVisible(false);
			st_city = SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ITEM_FLEET_SHIP_TITLE"];
		}else if ((*it).first == 1)
		{
			line_grad = 5;
			p_items = p_ship;
			b_IsExistCatchShip = true;
			p_city_clone = p_title->clone();
			auto i_title = p_city_clone->getChildByName<ImageView*>("image_icon");
			t_city = p_city_clone->getChildByName<Text*>("label_city");
			i_title->setVisible(false);
			st_city = SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ITEM_CATCH_SHIP_TITLE"];
		}else
		{
			line_grad = 8;
			p_items = p_goods;
			st_city = SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ITEM_DCOK_SHIP_TITLE"];
			std::string old_value = "[city]";
			std::string new_value = citysinfo[(*it).first - 1].name;
			repalce_all_ditinct(st_city,old_value,new_value);
		}
		t_city->setString(st_city);
		l_item->pushBackCustomItem(p_city_clone);

		for (int i = 0; i < n_line; i++)
		{
			auto p_items_clone = p_items->clone();
			for (int j = 0; j < line_grad; j++)
			{
				if (i * line_grad + j < n_ship)
				{
					index = shipIndex[(*it).first].at(i * line_grad + j);
					auto b_item = p_items_clone->getChildByName<Button*>(StringUtils::format("button_items_%d", j + 1));
					auto i_icon = b_item->getChildByName<ImageView*>("image_goods");
					auto t_num = i_icon->getChildByName<Text*>("text_item_num");
					getItemNameAndPath(m_pResult->itemtype, m_pResult->ships[index]->sid, itemName, itemPath);
					i_icon->setVisible(true);
					i_icon->ignoreContentAdaptWithSize(false);
					i_icon->loadTexture(itemPath);
					t_num->setVisible(false);
					b_item->setTag(index);
					b_item->setTouchEnabled(true);

					auto i_bgButton = b_item->getChildByName<ImageView*>("image_goods_bg");
					setBgButtonFormIdAndType(i_bgButton, m_pResult->ships[index]->sid, m_pResult->itemtype);
				
					if (m_pResult->ships[index]->optionid1 > 0 || m_pResult->ships[index]->optionid2 > 0)
					{
						addStrengtheningIcon(b_item);
					}
				}else
				{
					if (line_grad == 5)
					{
						auto b_item = p_items_clone->getChildByName<Button*>(StringUtils::format("button_items_%d", j + 1));
						auto i_bgButton = b_item->getChildByName<ImageView*>("image_goods_bg");
						setGLProgramState(i_bgButton,true);
					}
				}
			}
			l_item->pushBackCustomItem(p_items_clone);
		}
	
	}
	if (b_IsExistFleetShip)
	{
		if (b_IsExistCatchShip)
		{
			l_item->insertCustomItem(p_title_clone, 4);
		}
		else
		{
			l_item->insertCustomItem(p_title_clone, 2);
		}
	}
	else
	{
		l_item->insertCustomItem(p_title_clone, 0);
	}
	
	updateBagAndShipCapacity();
	auto image_pulldown_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown_item"));
	auto button_pulldown = image_pulldown_item->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX((image_pulldown_item->getBoundingBox().size.width / 2 - image_pulldown_item->getBoundingBox().size.width / 2) + 2);
	addListViewBar(l_item, image_pulldown_item);
}

void UICenterItem::updateBagAndShipCapacity()
{
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_INVENTORY_CSB]);
	auto p_bag = view->getChildByName<Widget*>("panel_bag");
	auto i_weight = p_bag->getChildByName<Widget*>("image_personal_weight");
	i_weight->setTag(IMAGE_ICON_BAGGO + IMAGE_INDEX2);
	i_weight->addTouchEventListener(CC_CALLBACK_2(UICenterItem::menuCall_func,this));
	auto t_weight = i_weight->getChildByName<Text*>("label_presonalcapacity_status_num");
	auto t_weight_0 = i_weight->getChildByName<Text*>("label_presonalcapacity_status_num_0");
	std::string st_weight = StringUtils::format("%.2f",m_pResult->curpackagesize / 100.0);
	std::string st_weight_0 = StringUtils::format(" / %.2f",m_pResult->maxpackagesize / 100.0);
	t_weight->setString(st_weight);
	t_weight_0->setString(st_weight_0);
	if (m_pResult->curpackagesize > m_pResult->maxpackagesize)
	{
		t_weight->setTextColor(Color4B::RED);
	}
	else
	{
		t_weight->setTextColor(Color4B::BLACK);
	}
	t_weight_0->setPositionX(t_weight->getBoundingBox().size.width + t_weight->getPositionX());

	auto i_ship_weight = p_bag->getChildByName<Widget*>("image_ship_weight");
	i_ship_weight->setTouchEnabled(true);
	i_ship_weight->setTag(IMAGE_ICON_CARGO + IMAGE_INDEX2);
	i_ship_weight->addTouchEventListener(CC_CALLBACK_2(UICenterItem::menuCall_func,this));

	auto i_personal_weight = p_bag->getChildByName<Widget*>("image_personal_weight");
	i_personal_weight->setTouchEnabled(true);
	i_personal_weight->setTag(IMAGE_ICON_PACKAGE + IMAGE_INDEX2);
	i_personal_weight->addTouchEventListener(CC_CALLBACK_2(UICenterItem::menuCall_func, this));

	auto t_ship_weight = i_ship_weight->getChildByName<Text*>("label_shipcapacity_status_num");
	auto t_ship_weight2 = i_ship_weight->getChildByName<Text*>("label_shipcapacity_status_num_2");
	t_ship_weight->setString(StringUtils::format("%.2f", m_pResult->curgoodsweight*0.01f));
	t_ship_weight2->setString(StringUtils::format(" / %.2f", m_pResult->maxgoodsweight*0.01f));

	if (m_pResult->curgoodsweight > m_pResult->maxgoodsweight)
	{
		t_ship_weight->setTextColor(Color4B::RED);
	}
	else
	{
		t_ship_weight->setTextColor(Color4B::BLACK);
	}
	t_ship_weight2->setPositionX(t_ship_weight->getBoundingBox().size.width + t_ship_weight->getPositionX());

	auto w_slider_item_bg = i_weight->getChildByName("image_progressbar_presonal_weight");
	auto w_slider_ship_bg = i_ship_weight->getChildByName("image_progressbar_ship_weight");
	auto lb_bar_item = w_slider_item_bg->getChildByName<LoadingBar*>("progressbar_weight");
	auto lb_bar_ship = w_slider_ship_bg->getChildByName<LoadingBar*>("progressbar_weight");
	lb_bar_item->setPercent(100.0 * m_pResult->curpackagesize / m_pResult->maxpackagesize);
	lb_bar_ship->setPercent(100.0 * m_pResult->curgoodsweight / m_pResult->maxgoodsweight);
}

void UICenterItem::showDropItem()
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_DROP_ITEM_CSB]);
	auto w_dropItemInfo = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_DROP_ITEM_CSB]);
	auto i_item_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(w_dropItemInfo, "image_item_bg"));
	auto i_item_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(w_dropItemInfo, "image_item_bg_lv"));
	auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(w_dropItemInfo,"image_item"));
	auto w_slider = dynamic_cast<Widget*>(Helper::seekWidgetByName(w_dropItemInfo,"panel_slider"));
	auto t_drop = dynamic_cast<Text*>(Helper::seekWidgetByName(w_dropItemInfo,"label_dropitem_num"));
	auto b_no = dynamic_cast<Button*>(Helper::seekWidgetByName(w_dropItemInfo,"button_no"));
	auto b_yes= dynamic_cast<Button*>(Helper::seekWidgetByName(w_dropItemInfo,"button_yes"));

	b_no->addTouchEventListener(CC_CALLBACK_2(UICenterItem::onDropItem,this));
	b_yes->addTouchEventListener(CC_CALLBACK_2(UICenterItem::onDropItem,this));
	std::string name("");
	std::string path("");
	int amount = 0;	
	int iid = 0;
	switch (m_pResult->itemtype)
	{
	case ITEM_TYPE_GOOD:
		{
			HatchItemsDefine* itemDefine = m_pResult->goods[m_nItemIndex];
			name = getGoodsName(itemDefine->goodsid);
			path = getGoodsIconPath(itemDefine->goodsid);
			amount = itemDefine->amount;
			iid = itemDefine->goodsid;
			break;
		}
	case ITEM_TYPE_SHIP:
		{
			ShipDefine* itemDefine = m_pResult->ships[m_nItemIndex];
			if (itemDefine->user_define_name)
			{
				name = itemDefine->user_define_name;
			}else
			{
				name = getShipName(itemDefine->sid);
			}
			path = getShipIconPath(itemDefine->sid);
			amount = 1;
			iid = itemDefine->sid;

			if (itemDefine->optionid1  > 0 || itemDefine->optionid2  > 0)
			{
				addStrengtheningIcon(i_item_bg);
			}
			break;
		}
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_PORP:
	case ITEM_TYPE_ROLE_EQUIP:
		{
			EquipmentDefine* itemDefine = m_pResult->equipments[m_nItemIndex];
			name = getItemName(itemDefine->equipmentid);
			path = getItemIconPath(itemDefine->equipmentid);
			amount = itemDefine->amount;
			iid = itemDefine->equipmentid;

			if (m_pResult->equipments[m_nItemIndex]->optionalid > 0)
			{
				addStrengtheningIcon(i_item_bg);
			}
			break;
		}
	case ITEM_TYPE_DRAWING:
		{
			DrawingItemsDefine* itemDefine = m_pResult->drawings[m_nItemIndex];
			name = getItemName(itemDefine->iid);
			path = getItemIconPath(itemDefine->iid);
			amount = itemDefine->amount;
			iid = itemDefine->iid;
			break;
		}
	case ITEM_TYPE_SPECIAL:
		{
			break;
		}
	default:
		break;
	}
	i_icon->ignoreContentAdaptWithSize(false);
	i_icon->loadTexture(path);
	t_drop->setString(StringUtils::format("x %d",amount));
	setBgButtonFormIdAndType(i_item_bg, iid, m_pResult->itemtype);
	setBgImageColorFormIdAndType(i_item_lv, iid, m_pResult->itemtype);


	auto sslider = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
	if (sslider)
	{
		sslider->removeFromParentAndCleanup(true);
	}
	UIVoyageSlider *ss = UIVoyageSlider::create(w_slider,amount,0,true);
	this->addChild(ss,1,1);
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UICenterItem::updataDropNum,this));
	ss->setMaxPercent(amount);
	ss->setCurrentNumber(amount);
}

void UICenterItem::updataDropNum(int num)
{
	auto w_dropItemInfo = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_DROP_ITEM_CSB]);
	auto t_drop = dynamic_cast<Text*>(Helper::seekWidgetByName(w_dropItemInfo,"label_dropitem_num"));
	t_drop->setString(StringUtils::format("x %d",num));
	UIVoyageSlider*	ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
	ss->setCurrentNumber(num);
}

void UICenterItem::scrollButtonEvent(Ref *pSender, cocos2d::ui::ScrollView::EventType type)
{
	if (type == cocos2d::ui::ScrollView::EventType::SCROLL_TO_TOP)
	{
		auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_INVENTORY_CSB]);
		auto b_more = view->getChildByName<Button*>("button_more");
		b_more->setBright(true);
	}
	else if (type == cocos2d::ui::ScrollView::EventType::SCROLL_TO_BOTTOM)
	{
		auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_INVENTORY_CSB]);
		auto b_more = view->getChildByName<Button*>("button_more");
		b_more->setBright(false);
	}
}