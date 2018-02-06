#include "UIGuideShipyard.h"
#include "UINoviceStoryLine.h"

UIGuideShipyard::UIGuideShipyard()
{
	//攻击力、防御、方向、船速数值设置  方向船速暂定
	equipNum = 1;
	atkNum = 0;
	defNum = 100;
	speedNum = 151;
	steeringNum = 150;
	sprite_hand = nullptr;
	m_guideStep = DOCK_STEP_1_ARMOR;
	m_eUIType = UI_DOCK;
	m_loadingNode = nullptr;
}
UIGuideShipyard::~UIGuideShipyard()
{
}
void UIGuideShipyard::onEnter()
{
	UIBasicLayer::onEnter();
}
void UIGuideShipyard::onExit()
{
	UIBasicLayer::onExit();
}
UIGuideShipyard* UIGuideShipyard::createDockLayerGuide()
{
	auto dockGuide= new UIGuideShipyard;
	
	if (dockGuide && dockGuide->init())
	{
		dockGuide->autorelease();
		return dockGuide;
	}
	CC_SAFE_DELETE(dockGuide);

	return nullptr;
}
bool UIGuideShipyard::init()
{
	bool pRet = false;
	do
	{
		m_loadingNode = UILoadingIndicator::create(this, m_eUIType);
		this->addChild(m_loadingNode);
		initStatic();
		pRet = true;
	} while (0);
	return pRet;
}
void UIGuideShipyard::initStatic()
{
	SINGLE_AUDIO->setMusicType();
	playAudio();
	//加载小手
	sprite_hand = Sprite::create();
	sprite_hand->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
	sprite_hand->setVisible(false);
	this->addChild(sprite_hand, 1000);

	openView(SHIPYARD_COCOS_RES[SHIPYARD_CSB]);
	auto shipyardMain = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_CSB]);
	auto panel_root = shipyardMain->getChildByName<Widget*>("panel_root");
	auto button_items = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_root, "button_items"));
	auto image_bg = panel_root->getChildByName("image_bg");
	image_bg->setVisible(false);
	setButtonsDisable(panel_root);
	button_items->setBright(false);

	openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_CSB]);
	auto leftbar = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_CSB]);
	auto button_gear_equip = dynamic_cast<Button*>(Helper::seekWidgetByName(leftbar, "button_gear_equip"));
	button_gear_equip->setBright(false);
	auto label_gear_equip = button_gear_equip->getChildByName<Text*>("label_gear_equip");
	label_gear_equip->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
	setButtonsDisable(leftbar);
	//装备界面
	openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
	auto fleetFear = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
	auto listview_ships = dynamic_cast<ListView*>(Helper::seekWidgetByName(fleetFear, "listview_ships"));
	auto button_ship_1 = listview_ships->getItem(0);
	button_ship_1->setVisible(true);
	auto image_btn_on = button_ship_1->getChildByName("image_btn_on");
	image_btn_on->setVisible(true);
	auto image_ship = button_ship_1->getChildByName<ImageView*>("image_ship");
	auto b_switch = Helper::seekWidgetByName(fleetFear, "button_switch");
	dynamic_cast<Button*>(b_switch)->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_SWITCH_SHIP"]);
	std::string path = "res/ships_128/ships_2.png";
	switch (SINGLE_HERO->m_iNation)//葡萄牙，西班牙，英国，荷兰，热那亚
	{
	case 1: 
	path = "res/ships_128/ships_2.png";
	break;
	case 2:
	path = "res/ships_128/ships_2.png";
	break;
	case 3:
	path = "res/ships_128/ships_3.png";
	break;
	case 4:
	path = "res/ships_128/ships_3.png";
	break;
	case 5:
	path = "res/ships_128/ships_2.png";
	break;
	default:
	break;
	}
	image_ship->loadTexture(path);
	for (int i = 1; i < 5;i++)
	{
		auto button_ship = listview_ships->getItem(i);
		button_ship->setVisible(false);
	}
	setButtonsDisable(listview_ships);

	auto panel_gear = fleetFear->getChildByName<Widget*>("panel_gear");
	auto label_ship_name = panel_gear->getChildByName<Text*>("label_ship_name");
	auto shipname = UserDefault::getInstance()->getStringForKey(ProtocolThread::GetInstance()->getFullKeyName("LUASHIPNAME").c_str());
	label_ship_name->setString(shipname);
	auto image_durable = panel_gear->getChildByName("image_durable");
	auto  panel_ship_durable_num_1 = image_durable->getChildByName("panel_ship_durable_num_1");

	//船耐久初始显示
	auto label_ship_durable_num_1 = panel_ship_durable_num_1->getChildByName<Text*>("label_ship_durable_num_1");
	label_ship_durable_num_1->setString("600");
	auto label_ship_durable_num_2 = panel_ship_durable_num_1->getChildByName<Text*>("label_ship_durable_num_2");
	label_ship_durable_num_2->setString("/600");
	label_ship_durable_num_1->setPositionX(label_ship_durable_num_2->getPositionX() - label_ship_durable_num_2->getContentSize().width);
	//船初始水手数显示
	auto image_sailor = panel_gear->getChildByName("image_sailor");
	auto label_sailors_num = image_sailor->getChildByName<Text*>("label_sailors_num");
	label_sailors_num->setString("0/35");
	//船初始重量显示
	auto image_weight = panel_gear->getChildByName("image_weight");
	auto label_capacity_num = image_weight->getChildByName<Text*>("label_capacity_num");
	label_capacity_num->setString("0/40");
	//船初始补给显示
	auto image_supply = panel_gear->getChildByName("image_supply");
	auto label_suppy_num = image_supply->getChildByName<Text*>("label_suppy_num");
	label_suppy_num->setString("0/350");
	//船初始攻击力显示
	auto image_property_1_1 = panel_gear->getChildByName("image_property_1");
	auto label_attack_num = image_property_1_1->getChildByName<Text*>("label_attack_num");
	label_attack_num->setString(StringUtils::format("%d",atkNum));
	//船初始防御显示
	auto image_property_2 = panel_gear->getChildByName("image_property_2");
	auto label_defense_num = image_property_2->getChildByName<Text*>("label_defense_num");
	label_defense_num->setString(StringUtils::format("%d", defNum));
	//船初始速度显示
	auto image_property_3 = panel_gear->getChildByName("image_property_3");
	auto label_speed_num = image_property_3->getChildByName<Text*>("label_speed_num");
	label_speed_num->setString(StringUtils::format("%d", speedNum));
	//船初始掌舵力显示
	auto image_property_4 = panel_gear->getChildByName("image_property_4");
	auto label_steering_num = image_property_4->getChildByName<Text*>("label_steering_num");
	label_steering_num->setString(StringUtils::format("%d", steeringNum));
	//隐藏撞角船首像，多余的按钮
	auto button_firsthead = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_gear,"button_firsthead"));
	auto button_spur_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_gear, "button_spur_1"));
	button_firsthead->setVisible(false);
	button_spur_1->setVisible(false);
	for (int i = 1; i < 3;i++)
	{
		auto button_sail = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_gear,StringUtils::format("button_sail_%d",i+1)));
		button_sail->setVisible(false);
	}
	for (int i = 1; i < 3; i++)
	{
		auto button_armor= dynamic_cast<Button*>(Helper::seekWidgetByName(panel_gear, StringUtils::format("button_armor_%d", i + 1)));
		button_armor->setVisible(false);
	}
	for (int i = 2; i < 8; i++)
	{
		auto button_gun = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_gear, StringUtils::format("button_gun_%d", i + 1)));
		button_gun->setVisible(false);
	}
	setButtonsDisable(fleetFear, "button_armor_1");
	auto button_armor_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(fleetFear,"button_armor_1"));
	handpicFocusOnButton(button_armor_1);
	m_loadingNode->removeFromParentAndCleanup(true);
}
void UIGuideShipyard::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		auto target = dynamic_cast<Widget*>(pSender);
		auto name = target->getName();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//甲板
		if (isButton(button_armor_1))
		{
			m_guideStep++;
			sprite_hand->setVisible(false);
			openEquipedView();
			CCLOG("");
			return;
		}
		//装备
		if (isButton(button_items))
		{
			m_guideStep++;	
			openEquipedView();
			return;
		}
		//火炮1
		if (isButton(button_gun_1))
		{
			m_guideStep++;
			sprite_hand->setVisible(false);
			openEquipedView();
			return;
		}
		//火炮2
		if (isButton(button_gun_2))
		{
			m_guideStep++;
			sprite_hand->setVisible(false);
			openEquipedView();
			return;
		}
		//确定
		if (isButton(button_ok))
		{
			m_guideStep++;
			closeView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAT_CHANGE_CSB]);
			updateFleetView();
			return;
		}
		//返回
		if (isButton(button_back))
		{
			Scene*currentScene = Director::getInstance()->getRunningScene();
			auto guideDialogLayer = UINoviceStoryLine::GetInstance();
			if (!guideDialogLayer->getParent())
			{
				currentScene->addChild(guideDialogLayer, 10001);
			}
			guideDialogLayer->onGuideTaskMessage(UINoviceStoryLine::SHIPYARD_TWO_DIALOG, 0);
			this->removeFromParent();
			SINGLE_HERO->m_iStage = 1;
//			button_callBack();
			return;
		}
	}
	
}
void UIGuideShipyard::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
}
void UIGuideShipyard::setButtonsDisable(Node * node, std::string btn_name)
{
	auto children = node->getChildren();
	Vector<Widget*>btns;
	if (children.size() == 0)
	{
		return;
	}
	for (auto btn : children)
	{
		std::string name = btn->getName();
		if (btn->getChildren().size() >= 0)
		{
			if (btn->getName().find("button_") == 0)
			{
				auto c_btn = dynamic_cast<Widget*>(btn);
				setButtonsDisable(btn, btn_name);
			}
			else
			{
				setButtonsDisable(btn, btn_name);
			}
		}
		if (btn->getName().compare(btn_name) == 0)
		{
			auto c_btn = dynamic_cast<Widget*>(btn);
			c_btn->setTouchEnabled(true);
		}
		else if (btn->getName().find("button_") == 0)
		{
			auto c_btn = dynamic_cast<Widget*>(btn);
			c_btn->setTouchEnabled(false);
		}
	}
}
void UIGuideShipyard::handpicFocusOnButton(Node * psender)
{
	sprite_hand->setVisible(true);
	auto c_psender = dynamic_cast<Widget*>(psender);
	Size win = _director->getWinSize();
	Vec2 pos = c_psender->getWorldPosition();
	Size cut_hand = sprite_hand->getContentSize() / 2;
	Size cut_psendet = psender->getContentSize() / 2;
	std::string name = dynamic_cast<Widget*>(psender)->getName();
	//提示的小手在屏幕中的四个不同方位方位需要调整的角度
	if (pos.x < win.width / 2 && pos.y > win.height / 2)
	{
		sprite_hand->setRotation(-180);
		sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
			c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
	}
	else if (pos.x > win.width / 2 && pos.y > win.height / 2)
	{
		sprite_hand->setRotation(-110);
		sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
			c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
	}
	else if (pos.x < win.width / 2 && pos.y < win.height / 2)
	{
		if (name.compare("button_ok") == 0)
		{
			sprite_hand->setRotation(-110);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
		}
		else
		{
			if (name.compare("") == 0)		{
			}
			else
			{
				sprite_hand->setRotation(70);
				sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
					c_psender->getWorldPosition().y + c_psender->getContentSize().height / 2 * 0.6 + sprite_hand->getContentSize().height / 2 * 0.6));
			}
		}
	}
	else if (pos.x > win.width / 2 && pos.y < win.height / 2)
	{
		if (name.compare("button_ok") == 0)
		{
			sprite_hand->setRotation(0);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x- sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y +c_psender->getContentSize().height / 2 * 0.6 +sprite_hand->getContentSize().height / 2 * 0.6));
		}
		else
		{
			sprite_hand->setRotation(0);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y + c_psender->getContentSize().height / 2 * 0.6 + sprite_hand->getContentSize().height / 2 * 0.6));
		}
	}
	sprite_hand->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
}
void UIGuideShipyard::openEquipedView()
{
	openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAT_CHANGE_CSB]);
	auto p_shipyard_fleet_gear_change = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAT_CHANGE_CSB]);
	auto panel_addequip = p_shipyard_fleet_gear_change->getChildByName<Widget*>("panel_addequip");
	panel_addequip->setVisible(true);
	auto current_equip_bg = panel_addequip->getChildByName<Button*>("current_equip_bg");
	current_equip_bg->setTouchEnabled(false);
	auto  image_current_equip = current_equip_bg->getChildByName<ImageView*>("image_current_equip");
	image_current_equip->setVisible(false);
	auto label_current_itemname = panel_addequip->getChildByName<Text*>("label_current_itemname");
	label_current_itemname->setString("N/A");
	auto button_uninstall = panel_addequip->getChildByName<Button*>("button_uninstall");
	button_uninstall->setTouchEnabled(false);
	button_uninstall->setBright(false);
	auto label_select_itemname_0 = panel_addequip->getChildByName("label_select_itemname_0");
	label_select_itemname_0->setVisible(false);
	//装备详细信息
	auto listview_p = panel_addequip->getChildByName("listview_p");
	auto panel_property_1 = listview_p->getChildByName("panel_property_1");
	listview_p->removeChild(panel_property_1);
	//船装备要求等级
	auto panel_require = listview_p->getChildByName("panel_require");
	auto label_require = panel_require->getChildByName<Text*>("label_require");
	label_require->setVisible(true);
	auto label_require_lv = panel_require->getChildByName<Text*>("label_require_lv");
	label_require_lv->setString("Lv.1");
	label_require_lv->setVisible(true);
	
	auto label_select_itemname = panel_addequip->getChildByName<Text*>("label_select_itemname");
	auto select_equip_bg = panel_addequip->getChildByName<Button*>("select_equip_bg");
	select_equip_bg->setTouchEnabled(false);
	auto image_select_equip = select_equip_bg->getChildByName<ImageView*>("image_select_equip");
	//名字
	label_select_itemname->setString("N/A");
	//图标
	image_select_equip->setVisible(false);
	auto panel_durable = listview_p->getChildByName("panel_durable");
	panel_durable->setVisible(true);
	auto label_durable_num_1 = panel_durable->getChildByName<Text*>("label_ship_durable_num_1");
	label_durable_num_1->setTextHorizontalAlignment(cocos2d::TextHAlignment::RIGHT);
	label_durable_num_1->setString("100");
	auto label_durable_num_2 = panel_durable->getChildByName<Text*>("label_ship_durable_num_2");
	label_durable_num_2->setTextHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
	label_durable_num_2->setString("/100");

	
	auto panel_property_2 = listview_p->getChildByName("panel_property_2");
	panel_property_2->setVisible(true);
	auto image_property_def = panel_property_2->getChildByName("image_property_2");
	auto image_atk = image_property_def->getChildByName<ImageView*>("image_def");

	image_atk->loadTexture("res/shipAttIcon/att_2.png");
	auto label_attack_num_addequip = image_property_def->getChildByName<Text*>("label_defense_num");
	label_attack_num_addequip->setString(StringUtils::format("%d", 25));

	auto panel_up = panel_property_2->getChildByName("panel_up");
	panel_up->setVisible(true);
	auto label_up_num = panel_up->getChildByName<Text*>("label_up_num");
	label_up_num->setString("25");

	auto panel_range = listview_p->getChildByName("panel_range");
	panel_range->setVisible(false);
	auto listview_range = panel_range->getChildByName("listview_range");
	auto image_range1 = listview_range->getChildByName<ImageView*>("image_range_1");
	auto image_range2 = listview_range->getChildByName<ImageView*>("image_range_2");
	auto image_range3 = listview_range->getChildByName<ImageView*>("image_range_3");
	image_range1->loadTexture("login_ui/common/range_4.png");
	image_range2->loadTexture("login_ui/common/range_4.png");
	image_range3->loadTexture("login_ui/common/range_4.png");

	

	auto  image_pulldown = panel_addequip->getChildByName("image_pulldown");
	image_pulldown->setVisible(false);

	 //装备容器
	auto listview_left = panel_addequip->getChildByName<ListView*>("listview_left");
	listview_left->setVisible(false);
	auto panel_items = panel_addequip->getChildByName("panel_items");
	panel_items->setPosition(listview_left->getWorldPosition().x, listview_left->getWorldPosition().y + 321);
	panel_items->setVisible(true);
	auto image_items_1 = panel_items->getChildByName("image_items_1");
	auto image_items_2 = panel_items->getChildByName("image_items_2");
	auto image_items_3 = panel_items->getChildByName("image_items_3");
	auto image_items_4 = panel_items->getChildByName("image_items_4");
	auto image_items_5 = panel_items->getChildByName("image_items_5");
	auto image_items_6 = panel_items->getChildByName("image_items_6");
	//容器中的装备显示
	auto button_items_1 = image_items_1->getChildByName<Button*>("button_items");
	button_items_1->setTouchEnabled(false);
	auto image_goods1 = image_items_1->getChildByName<ImageView*>("image_goods");
	image_goods1->setVisible(true);
	auto label_num1 = image_goods1->getChildByName<Text*>("text_item_num");
										
	label_num1->setString("1");
	auto  button_items_2 = image_items_2->getChildByName<Button*>("button_items");
	button_items_2->setTouchEnabled(false);
	auto image_goods2 = image_items_2->getChildByName<ImageView*>("image_goods");
	image_goods2->setVisible(false);
	auto label_num2 = image_goods2->getChildByName<Text*>("text_item_num");
	label_num2->setString("1");

	auto button_ok = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_addequip,"button_ok"));

	switch (m_guideStep)
	{
	case DOCK_STEP_1_ARMOR:
	{
			break;
	}
	case DOCK_STEP_2_CHOOSE_ARMOR:
	{
			setButtonsDisable(panel_addequip);
			button_items_1->setTouchEnabled(true);
			image_goods1->loadTexture(getItemIconPath(1025));
			handpicFocusOnButton(button_items_1);
			listview_p->setVisible(false);
			break;
	}
	case DOCK_STEP_3_SURE_CHOOSE_ARMOR:
	{
			button_items_1->setBright(false);
			setButtonsDisable(panel_addequip, "button_ok");
			handpicFocusOnButton(button_ok);
			listview_p->setVisible(true);
			image_select_equip->setVisible(true);
			image_select_equip->loadTexture(getItemIconPath(1025));
			label_select_itemname->setString(SINGLE_SHOP->getItemData()[1025].name);
			defNum +=25;
			break;
	}
	case DOCK_STEP_4_GUN_1:
	{
			break;
	}
	case DOCK_STEP_5_CHOOSE_GUN_1:
	{ 
			setButtonsDisable(panel_addequip);
			button_items_1->setTouchEnabled(true);
			image_goods1->loadTexture(getItemIconPath(1001));
			image_goods2->setVisible(true);
			image_goods2->loadTexture(getItemIconPath(1001));
			handpicFocusOnButton(button_items_1);
			listview_p->setVisible(false);
			break;
	}
	case DOCK_STEP_6_SURE_CHOOSE_GUN_1:
	{
			button_items_1->setBright(false);
			setButtonsDisable(panel_addequip, "button_ok");
			handpicFocusOnButton(button_ok);
			listview_p->setVisible(true);
			image_select_equip->setVisible(true);
			image_select_equip->loadTexture(getItemIconPath(1001));
			label_select_itemname->setString(SINGLE_SHOP->getItemData()[1001].name);
			image_goods2->setVisible(true);
			image_goods2->loadTexture(getItemIconPath(1001));
			panel_range->setVisible(true);
			image_atk->loadTexture("res/shipAttIcon/att_1.png");
			label_up_num->setString("120");
			atkNum += 120;
			label_attack_num_addequip->setString(StringUtils::format("%d", atkNum));
			break;
	}
	case DOCK_STEP_7_GUN_2:
	{
			break;
	}
	case DOCK_STEP_8_CHOOSE_GUN_2:
	{
			setButtonsDisable(panel_addequip);
			button_items_1->setTouchEnabled(true);
			image_goods1->loadTexture(getItemIconPath(1001));
			handpicFocusOnButton(button_items_1);
			listview_p->setVisible(false);
			break;
	}
	case DOCK_STEP_9_SURE_CHOOSE_GUN_2:
	{
			button_items_1->setBright(false);
			setButtonsDisable(panel_addequip, "button_ok");
			handpicFocusOnButton(button_ok);
			listview_p->setVisible(true);
			image_select_equip->setVisible(true);
			image_select_equip->loadTexture(getItemIconPath(1001));
			label_select_itemname->setString(SINGLE_SHOP->getItemData()[1001].name);
			panel_range->setVisible(true);
			image_atk->loadTexture("res/shipAttIcon/att_1.png");
			label_up_num->setString("120");
			label_attack_num_addequip->setString(StringUtils::format("%d", atkNum));
			atkNum += 120;
			break;
	}
	case DOCK_STEP_10_BACK:
	{
			break;
	}
	default:
	break;
	}																											
}
void UIGuideShipyard::updateFleetView()
{
	openView(SHIPYARD_COCOS_RES[SHIPYARD_CSB]);
	auto shipyardMain = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_CSB]);
	auto button_back = dynamic_cast<Button*>(Helper::seekWidgetByName(shipyardMain,"button_back"));
	auto fleetFear = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_FEAR_CSB]);
	auto panel_gear = fleetFear->getChildByName<Widget*>("panel_gear");
	auto button_armor_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(fleetFear, "button_armor_1"));
	auto button_add_armor = dynamic_cast<Button*>(Helper::seekWidgetByName(button_armor_1, "button_add_armor"));
	auto image_equip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(button_armor_1, "image_equip"));
	
	auto button_gun_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(fleetFear, "button_gun_1"));
	auto button_add_gun_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(button_gun_1, "button_add_gun"));
	auto image_equip_gun_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(button_gun_1, "image_equip"));
	auto panel_range_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(button_gun_1, "panel_range"));

	auto button_gun_2 = dynamic_cast<Button*>(Helper::seekWidgetByName(fleetFear, "button_gun_2"));
	auto button_add_gun_2 = dynamic_cast<Button*>(Helper::seekWidgetByName(button_gun_2, "button_add_gun"));
	auto image_equip_gun_2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(button_gun_2, "image_equip"));
	auto panel_range_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(button_gun_2, "panel_range"));

	auto image_property_1_1 = panel_gear->getChildByName("image_property_1");
	auto label_attack_num = image_property_1_1->getChildByName<Text*>("label_attack_num");
	label_attack_num->setString(StringUtils::format("%d", atkNum));
	//船初始防御显示
	auto image_property_2 = panel_gear->getChildByName("image_property_2");
	auto label_defense_num = image_property_2->getChildByName<Text*>("label_defense_num");
	label_defense_num->setString(StringUtils::format("%d", defNum));
	switch (m_guideStep)
	{
	case DOCK_STEP_1_ARMOR:
	{
		break;
	}
	case DOCK_STEP_2_CHOOSE_ARMOR:
	{
						
	    break;
	}
	case DOCK_STEP_3_SURE_CHOOSE_ARMOR:
	{
							
		break;
	}
	case DOCK_STEP_4_GUN_1:
	{
							  button_add_armor->setVisible(false);
							  image_equip->setVisible(true);
							  button_armor_1->setBright(false);
							  image_equip->loadTexture(getItemIconPath(1025));
							
							  setButtonsDisable(panel_gear, "button_gun_1");
							  handpicFocusOnButton(button_gun_1);
		break;
	}
	case DOCK_STEP_5_CHOOSE_GUN_1:
	{
		break;
	}
	case DOCK_STEP_6_SURE_CHOOSE_GUN_1:
	{
		break;
	}
	case DOCK_STEP_7_GUN_2:
	{					 
							  button_add_gun_1->setVisible(false);
							  image_equip_gun_1->setVisible(true);
							  button_gun_1->setBright(false);
							  image_equip_gun_1->loadTexture(getItemIconPath(1001));
							  for (int i = 0; i < 3; i++)
							  {
								  auto btnRange = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_range_1, StringUtils::format("button_range_%d", i + 1)));
								  btnRange->loadTextureNormal("login_ui/common/range_4.png");
							  }
							  setButtonsDisable(panel_gear, "button_gun_2");
							  handpicFocusOnButton(button_gun_2);
		break;
	}
	case DOCK_STEP_8_CHOOSE_GUN_2:
	{
		break;
	}
	case DOCK_STEP_9_SURE_CHOOSE_GUN_2:
	{
		break;
	}
	case DOCK_STEP_10_BACK:
	{
							  button_add_gun_2->setVisible(false);
							  image_equip_gun_2->setVisible(true);
							  button_gun_2->setBright(false);
							  image_equip_gun_2->loadTexture(getItemIconPath(1001));
							  for (int i = 0; i < 3; i++)
							  {
								  auto btnRange = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_range_2, StringUtils::format("button_range_%d", i + 1)));
								  btnRange->loadTextureNormal("login_ui/common/range_4.png");
							  }
							  setButtonsDisable(panel_gear);
							  setButtonsDisable(shipyardMain, "button_back");
							  handpicFocusOnButton(button_back);
		break;
	}
	default:
	break;
	}



}
