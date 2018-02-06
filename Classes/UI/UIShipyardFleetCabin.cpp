#include "UIShipyardFleetCabin.h"
#include "UICommon.h"
#include "UIInform.h"

#include "cocos2d.h"
#include "ModeLayerManger.h"
USING_NS_CC;
using namespace ui;

UIShipyardFleetCabin::UIShipyardFleetCabin()
{
	m_pCompanionButton = nullptr;
	m_pCompaniesResult = nullptr;
	m_vShipCabinNo.clear();
	m_nShipId = 0;
	m_bFirst = true;
	mscale = 1.0;
}

UIShipyardFleetCabin::~UIShipyardFleetCabin()
{
	unregisterCallBack();
	SINGLE_HERO->m_bClickTouched = false;
}

void UIShipyardFleetCabin::onEnter()
{
	UIBasicLayer::onEnter();
}

void UIShipyardFleetCabin::onExit()
{
	UIBasicLayer::onExit();
}

UIShipyardFleetCabin* UIShipyardFleetCabin::createFleetCabin(int sid)
{
	auto fc = new UIShipyardFleetCabin;
	if (fc && fc->init(sid))
	{
		fc->autorelease();
		return fc;
	}
	CC_SAFE_DELETE(fc);
	return nullptr;
}

bool UIShipyardFleetCabin::init(int sid)
{
	bool pRet = false;
	do
	{
		UIBasicLayer::init();
		m_nShipId = sid;
		treatShipCabinNo();
		registerCallBack();
	
		setTouchMode(Touch::DispatchMode::ALL_AT_ONCE);
		setTouchEnabled(true);
		//1.注册监听事件对象
		auto listener = EventListenerTouchAllAtOnce::create();      //多点触摸
		//2.定义监听对象的回调方法
		listener->onTouchesBegan = CC_CALLBACK_2(UIShipyardFleetCabin::onTouchesBegan, this);
		listener->onTouchesMoved = CC_CALLBACK_2(UIShipyardFleetCabin::onTouchesMoved, this);
		listener->onTouchesEnded = CC_CALLBACK_2(UIShipyardFleetCabin::onTouchesEnded, this);
		listener->onTouchesCancelled = CC_CALLBACK_2(UIShipyardFleetCabin::onTouchesCancelled, this);
		//3.在事件监听器中注册（事件监听器包含：触摸事件、键盘相应事件、加速记录事件、鼠标响应事件、自定义事件）
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	
		ProtocolThread::GetInstance()->getShipCompanions(UILoadingIndicator::create(this));
		pRet = true;
	} while (0);
	return pRet;
}

void UIShipyardFleetCabin::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}

	Widget* widget = dynamic_cast<Widget*>(pSender);
	auto name = widget->getName();
	//花费银币解锁船舱
	if (isButton(button_s_yes))
	{
		if (m_pCompanionButton)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			ProtocolThread::GetInstance()->unlockShipCabin(m_pShipCResult->shipid, m_nShipId, m_pCompanionButton->getTag(), UILoadingIndicator::create(this));
		}
		return;
	}

	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//保存数据，返回界面
	if (isButton(button_back))
	{
		auto instance = ModeLayerManger::getInstance();
		auto layer = instance->getModelLayer();
		if (layer != nullptr)
		{
			int tag = layer->getTag();
			if (tag == 1)
			{
				instance->showModelLayer(true);
			}
		}
		closeView();
		this->removeFromParent();
		return;
	}
	//小伙伴界面解释
	if (isButton(button_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_DOCK_FLEET_CABIN_TITLE", "TIP_DOCK_FLEET_CABIN_CONTENT");
		return;
	}
	//小伙伴船舱位置解释
	if (isButton(button_room_info))
	{
		if (m_pCompanionButton)
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushInfoBtnView("NULL", "NULL", getRoomType(m_pCompanionButton->getTag()));
		}
		return;
	}
	//去除详情
	if (isButton(panel_cabin))
	{
		if (m_pCompanionButton)
		{
			m_pCompanionButton->setTouchEnabled(true);
			auto image_on = m_pCompanionButton->getChildByName<ImageView*>("image_on");
			image_on->setVisible(false);
		}
		m_pCompanionButton = nullptr;
		companionDetails();
		return;
	}
	//工作室
	if (isButton(button_cabin_))
	{
		if (m_pCompanionButton)
		{
			m_pCompanionButton->setTouchEnabled(true);
			auto image_on = m_pCompanionButton->getChildByName<ImageView*>("image_on");
			image_on->setVisible(false);
		}
		m_pCompanionButton = widget;
		if (m_pCompanionButton)
		{
			m_pCompanionButton->setTouchEnabled(false);
			auto image_on = m_pCompanionButton->getChildByName<ImageView*>("image_on");
			image_on->setVisible(true);
		}
		companionDetails();
		return;
	}
	//配置小伙伴
	if (isButton(button_head_player))
	{
		if (m_pCompanionButton)
		{
			ProtocolThread::GetInstance()->getCompanies(UILoadingIndicator::create(this));
		}
		return;
	}
	//解锁工作室
	if (isButton(button_accept))
	{
		int price = ceil(SINGLE_SHOP->getShipData().at(m_nShipId).price * 0.05);
		if (price < 500)
		{
			price = 500;
		}
	    UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushSilverConfirmView("TIP_DOCK_FELLT_CABIN_UNLOCK_FEE_TITLE", "TIP_DOCK_FELLT_CABIN_UNLOCK_FEE_CONTENT", price);
		openView(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB],20);
		auto panel_numpad = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
		panel_numpad->setPosition(STARTPOS);
		panel_numpad->setTouchEnabled(false);
		auto num_root = panel_numpad->getChildByName<Widget*>("panel_numpad");
		num_root->setVisible(false);
		auto panel_silver = panel_numpad->getChildByName<Widget*>("panel_silver");
		panel_silver->runAction(MoveTo::create(0.4f, Vec2(0, 560)));
		updateCoinNum(SINGLE_HERO->m_iGold, SINGLE_HERO->m_iCoin);
		return;
	}
	
	//普通技能--下方浮动部分
	if (isButton(button_skill_bg_))
	{
		if (m_pCompanionButton)
		{
			auto image_on = m_pCompanionButton->getChildByName<ImageView*>("image_on");
			auto shipGD = m_pShipCResult->details[image_on->getTag()]->normalskill[widget->getTag()];
			UICommon::getInstance()->openCommonView(this);
			SKILL_DEFINE skillDefine;
			skillDefine.id = shipGD->id;
			skillDefine.lv = shipGD->level;
			if (m_pShipCResult->details[image_on->getTag()]->iscaptain)
			{
				skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
			}
			else
			{
				skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
			}
			skillDefine.icon_id = m_pShipCResult->details[image_on->getTag()]->proto_id;
			UICommon::getInstance()->flushSkillView(skillDefine);
		}
		return;
	}
	//特殊技能--下方浮动部分
	if (isButton(button_sp_skill_bg_))
	{
		if (m_pCompanionButton)
		{
			auto image_on = m_pCompanionButton->getChildByName<ImageView*>("image_on");
			auto shipGD = m_pShipCResult->details[image_on->getTag()]->specialskill[widget->getTag()];
			UICommon::getInstance()->openCommonView(this);
			SKILL_DEFINE skillDefine;
			skillDefine.id = shipGD->id;
			skillDefine.lv = shipGD->level;
			skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
			skillDefine.icon_id = m_pShipCResult->details[image_on->getTag()]->proto_id;
			UICommon::getInstance()->flushSkillView(skillDefine);
		}
		return;
	}

	//普通技能--配置小伙伴
	if (isButton(button_equip_bg_))
	{
		auto image_div = widget->getParent()->getParent()->getChildByName<ImageView*>("image_div");
		auto index = image_div->getTag();
		CompanyDefine *companyDefine;
	
		switch (index)
		{
		case UIShipyardFleetCabin::INFLEET_COMPANION:
			companyDefine = m_pCompaniesResult->infleet[widget->getParent()->getTag()];
			break;
		case UIShipyardFleetCabin::IDLE_COMPANION:
			companyDefine = m_pCompaniesResult->idlecompanies[widget->getParent()->getTag() - m_pCompaniesResult->n_infleet];
			break;
		case UIShipyardFleetCabin::IDLE_CAPTAIN:
			companyDefine = m_pCompaniesResult->idlecaptains[widget->getParent()->getTag() - m_pCompaniesResult->n_infleet - m_pCompaniesResult->n_idlecompanies];
			break;
		default:
			break;
		}

		UICommon::getInstance()->openCommonView(this);
		SKILL_DEFINE skillDefine;

		skillDefine.id = companyDefine->normalskill[widget->getTag()]->id;
		skillDefine.lv = companyDefine->normalskill[widget->getTag()]->level;
		if (companyDefine->iscaptain)
		{
			skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
		}
		else
		{
			skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
		}
		skillDefine.icon_id = companyDefine->protoid;
		UICommon::getInstance()->flushSkillView(skillDefine);
		return;
	}
	//特殊技能--配置小伙伴
	if (isButton(button_sp_equip_bg_))
	{
		auto image_div = widget->getParent()->getParent()->getChildByName<ImageView*>("image_div");
		auto index = image_div->getTag();

		CompanyDefine *companyDefine;
		switch (index)
		{
		case UIShipyardFleetCabin::INFLEET_COMPANION:
			companyDefine = m_pCompaniesResult->infleet[widget->getParent()->getTag()];
			break;
		case UIShipyardFleetCabin::IDLE_COMPANION:
			companyDefine = m_pCompaniesResult->idlecompanies[widget->getParent()->getTag() - m_pCompaniesResult->n_infleet];
			break;
		case UIShipyardFleetCabin::IDLE_CAPTAIN:
			companyDefine = m_pCompaniesResult->idlecaptains[widget->getParent()->getTag() - m_pCompaniesResult->n_infleet - m_pCompaniesResult->n_idlecompanies];
			break;
		default:
			break;
		}

		UICommon::getInstance()->openCommonView(this);
		SKILL_DEFINE skillDefine;
		skillDefine.id = companyDefine->specialskill[widget->getTag()]->id;
		skillDefine.lv = companyDefine->specialskill[widget->getTag()]->level;
		skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
		skillDefine.icon_id = companyDefine->protoid;
		UICommon::getInstance()->flushSkillView(skillDefine);
		return;
	}
	if (isButton(button_close))
	{
		closeView();
		return;
	}
	//选择小伙伴
	if (isButton(panel_captain_1))
	{
		auto image_div = widget->getChildByName<ImageView*>("image_div");
		auto index = image_div->getTag();
		CompanyDefine *companyDefine;
		switch (index)
		{
		case UIShipyardFleetCabin::INFLEET_COMPANION:
			companyDefine = m_pCompaniesResult->infleet[widget->getTag()];
			break;
		case UIShipyardFleetCabin::IDLE_COMPANION:
			companyDefine = m_pCompaniesResult->idlecompanies[widget->getTag()];
			break;
		case UIShipyardFleetCabin::IDLE_CAPTAIN:
			companyDefine = m_pCompaniesResult->idlecaptains[widget->getTag()];
			break;
		default:
			break;
		}
		ProtocolThread::GetInstance()->setShipCompanions(companyDefine->id, m_pCompanionButton->getTag(), companyDefine->iscaptain, m_pShipCResult->shipid, UILoadingIndicator::create(this));
		return;
	}
	//移除小伙伴
	if (isButton(panel_captain_remove))
	{
		ProtocolThread::GetInstance()->setShipCompanions(0, m_pCompanionButton->getTag(), 0, m_pShipCResult->shipid, UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_s_no))
	{
		closeView();
		closeView(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	}
}
	
void UIShipyardFleetCabin::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetShipCompanionsResult:
		{
			auto shipCp = (GetShipCompanionsResult*)(message);
			if (shipCp->failed == 0)
			{
				m_pShipCResult = shipCp;
				shipCabinView();
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetCompaniesResult:
		{
			auto companiesResult = (GetCompaniesResult*)(message);
			if (companiesResult->failed == 0)
			{
				if (m_pCompanionButton)
				{
					int index = m_pCompanionButton->getChildByName<ImageView*>("image_on")->getTag();
					if (index >= 0)
					{
						m_pCompaniesResult = companiesResult;
						configCompanion();
					}
				}
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
	case PROTO_TYPE_SetShipCompanionsResult:
		{
			auto shipCResult = (SetShipCompanionsResult*)(message);
			if (shipCResult->failed == 0)
			{
				closeView();
				ProtocolThread::GetInstance()->getShipCompanions(UILoadingIndicator::create(this));
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
	case PROTO_TYPE_UnlockShipCabinResult:
		{
			closeView(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
			auto unlockShipCResult = (UnlockShipCabinResult*)(message);
			if (unlockShipCResult->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_DOCK_FELLT_CABIN_UNLOCK_FEE_SUCCESS");
				ProtocolThread::GetInstance()->getShipCompanions(UILoadingIndicator::create(this));
				updateCoinNum(unlockShipCResult->golds, unlockShipCResult->coins);
			}
			else if (unlockShipCResult->failed == 4)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
	default:
		break;
	}
}

void UIShipyardFleetCabin::shipCabinView()
{
	if (!this->getChildByTag(300))
	{
		std::string str = StringUtils::format("%s%0.2f",SINGLE_SHOP->getTipsInfo()["TIP_DOCK_CABIN_SCALE"].c_str(), mscale);
		auto pLabel = Label::create(str, "", 24);
		pLabel->setAnchorPoint(Vec2(0, 0));
		pLabel->setPosition(30, 30);
		this->addChild(pLabel, 300, 300);
		pLabel->setVisible(false);
	}

	//1:小 2:中 3:大
	int ship_type = SINGLE_SHOP->getShipData().at(m_nShipId).type;
	int n_grid;
	switch (ship_type)
	{
	case 1:
		ship_type = SHIPYARD_FLEET_GEAR_CABIN_S_CSB;
		n_grid = 16;
		break;
	case 2:
		ship_type = SHIPYARD_FLEET_GEAR_CABIN_M_CSB;
		n_grid = 24;
		break;
	case 3:
		ship_type = SHIPYARD_FLEET_GEAR_CABIN_L_CSB;
		n_grid = 33;
		break;
	default:
		ship_type = SHIPYARD_FLEET_GEAR_CABIN_S_CSB;
		n_grid = 16;
		break;
	}
	openView(SHIPYARD_COCOS_RES[ship_type]);
	auto view = getViewRoot(SHIPYARD_COCOS_RES[ship_type]);
	s_cabin = (cocos2d::ui::ScrollView*)(view->getChildByName("scrollView_cabin"));
	auto p_cabin = dynamic_cast<Widget*>(Helper::seekWidgetByName(s_cabin, "panel_cabin"));
	p_cabin->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleetCabin::menuCall_func,this));
	p_cabin->setTouchEnabled(true);
	if (m_bFirst)
	{
		m_bFirst = false;
		s_cabin->jumpToBottom();
		s_cabin->jumpToPercentHorizontal(50);
	}

	for (size_t i = 0; i < n_grid; i++)
	{
		auto item = p_cabin->getChildByName<Button*>(StringUtils::format("button_cabin_%d", i + 1));
		auto image_black = item->getChildByName<ImageView*>("image_black");
		auto image_lock = item->getChildByName<ImageView*>("image_lock");
		auto image_on = item->getChildByName<ImageView*>("image_on");
		item->setTag(i + 1);
		//船长室特殊处理
		if (i == 3)
		{
			item->setTouchEnabled(true);
			addOrDelCompanion(item, -10, 0, i + 1);
			image_on->setTag(-10);
		}
		else
		{
			//废弃舱室
			if (m_vShipCabinNo.at(i) < 1)
			{
				item->loadTextureNormal("cocosstudio/login_ui/cabin_720/cabin_no.png");
				item->setTouchEnabled(false);
				image_black->setVisible(true);
			}
			else
			{
				ShipGearDetail *shipGDInfo = nullptr;
				size_t j = 0;
				for (; j < m_pShipCResult->n_details; j++)
				{
					if (m_pShipCResult->details[j]->shipposition == i + 1)
					{
						shipGDInfo = m_pShipCResult->details[j];
						break;
					}
				}
				//未解锁
				if (!shipGDInfo)
				{
					image_black->setVisible(true);
					image_lock->setVisible(true);
					image_on->setTag(-1);
				}
				else
				{
					image_black->setVisible(false);
					image_lock->setVisible(false);
					image_on->setTag(j);
					int proto_id = -1;
					if (shipGDInfo->proto_id > 0)
					{
						proto_id = shipGDInfo->proto_id;
					}
					addOrDelCompanion(item, proto_id, shipGDInfo->iscaptain, i + 1);
				}
			}
		}
	}

	openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_GEAR_CABIN_CSB]);
	companionDetails();
}

void UIShipyardFleetCabin::companionDetails()
{
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_GEAR_CABIN_CSB]);
	auto image_mask = view->getChildByName<ImageView*>("image_mask");
	auto image_pannel = view->getChildByName<ImageView*>("image_pannel");

	if (m_pCompanionButton)
	{
		auto image_on = m_pCompanionButton->getChildByName<ImageView*>("image_on");
		image_mask->setVisible(true);
		image_pannel->setVisible(true);
		auto p_room = image_pannel->getChildByName<Widget*>("panel_room");
		auto p_room_locked = image_pannel->getChildByName<Widget*>("panel_room_locked");

		//没有解锁
		if (image_on->getTag() == -1)
		{
			p_room->setVisible(false);
			p_room_locked->setVisible(true);
			auto t_room = p_room_locked->getChildByName<Text*>("label_room_name_0");
			t_room->setString(getCompanionRoom(m_pCompanionButton->getTag(), m_nShipId));
			auto i_lock = p_room_locked->getChildByName<ImageView*>("image_lock_0");
			auto b_room_info = p_room_locked->getChildByName<Button*>("button_room_info");
			i_lock->setPositionX(t_room->getPositionX() + 
				t_room->getBoundingBox().size.width + i_lock->getBoundingBox().size.width * 0.8);
			b_room_info->setPositionX(i_lock->getPositionX() +
				i_lock->getBoundingBox().size.width * 0.5 + b_room_info->getBoundingBox().size.width * 0.5);
		}
		else
		{
			p_room->setVisible(true);
			p_room_locked->setVisible(false);
			auto b_head_player = p_room->getChildByName<Button*>("button_head_player");
			auto image_head = b_head_player->getChildByName<ImageView*>("image_head");
			auto image_refresh = b_head_player->getChildByName<ImageView*>("image_refresh");
			auto t_room_name = p_room->getChildByName<Text*>("label_room_name");
			auto t_com_name = p_room->getChildByName<Text*>("label_com_name");
			auto b_room_info = p_room->getChildByName<Button*>("button_room_info");
			auto p_skill = p_room->getChildByName<Widget*>("panel_skill");
			auto p_skill_sp = p_room->getChildByName<Widget*>("panel_skill_sp");
			auto t_empty_room = p_room->getChildByName<Text*>("label_empty_room");
			
			//舰长室是玩家自己(特殊处理)
			if (image_on->getTag() == -10)
			{
				b_head_player->setTouchEnabled(false); 
				image_head->setVisible(true);
				image_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));
				image_refresh->setVisible(false);
				t_room_name->setString(getCompanionRoom(m_pCompanionButton->getTag(), m_nShipId));
				t_room_name->setPositionY(p_room->getBoundingBox().size.height * 0.5);
				t_com_name->setVisible(false);
				p_skill->setVisible(false);
				p_skill_sp->setVisible(false);
				t_empty_room->setVisible(false);
				b_room_info->setPositionX(t_room_name->getPositionX() + t_room_name->getBoundingBox().size.width
					+ b_room_info->getBoundingBox().size.width * 0.8);
				b_room_info->setPositionY(p_room->getBoundingBox().size.height * 0.5);
			}
			else
			{
				t_room_name->setString(getCompanionRoom(m_pCompanionButton->getTag(), m_nShipId));
				b_head_player->setTouchEnabled(true);
				//有小伙伴时
				auto shipGD = m_pShipCResult->details[image_on->getTag()];
				if (shipGD->companionsid > 0)
				{
					b_head_player->setBright(true);
					image_head->setVisible(true);
					image_head->loadTexture(getCompanionIconPath(shipGD->proto_id, shipGD->iscaptain));
					image_refresh->setVisible(true);
					t_room_name->setPositionY(p_room->getBoundingBox().size.height * 0.6);
					t_com_name->setVisible(true);
					t_com_name->setString(getCompanionName(shipGD->proto_id, shipGD->iscaptain));
					b_room_info->setPositionX(t_room_name->getPositionX() + t_room_name->getBoundingBox().size.width
						+ b_room_info->getBoundingBox().size.width * 0.8);
					b_room_info->setPositionY(p_room->getBoundingBox().size.height * 0.6);
					//普通技能
					int skill1 = shipGD->n_normalskill;
					if (skill1 > 0)
					{
						p_skill->setVisible(true);
					}
					else
					{
						p_skill->setVisible(false);
					}

					for (size_t i = 0; i < 3; i++)
					{
						auto b_skill = p_skill->getChildByName<Button*>(StringUtils::format("button_skill_bg_%d", i + 1));
						if (i < skill1)
						{
							b_skill->setTouchEnabled(true);
							b_skill->setVisible(true);
							b_skill->setTag(i);
							auto t_skill = b_skill->getChildByName<Text*>("text_item_skill_lv");
							SKILL_TREE skill;
							if (shipGD->iscaptain == 1)
							{
								skill = SINGLE_SHOP->getCaptainSkillInfo().at(shipGD->normalskill[i]->id);
								b_skill->loadTextureNormal(getSkillIconPath(shipGD->normalskill[i]->id, SKILL_TYPE_CAPTAIN));
								if (skill.room_type == getRoomType(m_pCompanionButton->getTag()))
								{
									b_skill->setBright(true);
								}
								else
								{
									b_skill->setBright(false);
								}
							}
							else
							{
								skill = SINGLE_SHOP->getCompanionNormalSkillInfo().at(shipGD->normalskill[i]->id);
								b_skill->loadTextureNormal(getSkillIconPath(shipGD->normalskill[i]->id, SKILL_TYPE_COMPANION_NORMAL));
								if (skill.room_type == getRoomType(m_pCompanionButton->getTag()))
								{
									b_skill->setBright(true);
								}
								else
								{
									b_skill->setBright(false);
								}
							}
							setTextSizeAndOutline(t_skill, shipGD->normalskill[i]->level);
						}
						else
						{
							b_skill->setVisible(false);
						}
					}

					//特殊技能
					int skill2 = shipGD->n_specialskill;
					if (skill2 > 0)
					{
						p_skill_sp->setVisible(true);
						p_skill->setPositionY(p_room->getBoundingBox().size.height * 0.5);
					}
					else
					{
						p_skill_sp->setVisible(false);
						p_skill->setPositionY(p_room->getBoundingBox().size.height * 0.25);
					}

					for (size_t i = 0; i < 3; i++)
					{
						auto b_skill = p_skill_sp->getChildByName<Button*>(StringUtils::format("button_sp_skill_bg_%d", i + 1));
						if (i < skill2)
						{
							b_skill->setTouchEnabled(true);
							b_skill->setVisible(true);
							b_skill->setTag(i);
							auto image_lock = b_skill->getChildByName<ImageView*>("image_lock");
							b_skill->loadTextureNormal(getSkillIconPath(shipGD->specialskill[i]->id, SKILL_TYPE_PARTNER_SPECIAL));
							//解锁
							if (shipGD->specialskill[i]->level > 0)
							{
								image_lock->setVisible(false);
							}
							else
							{
								image_lock->setVisible(true);
							}

							auto skill = SINGLE_SHOP->getCompanionSpecialiesSkillInfo().at(shipGD->specialskill[i]->id);
							if (shipGD->specialskill[i]->level > 0 && skill.room_type == getRoomType(m_pCompanionButton->getTag()))
							{
								b_skill->setBright(true);
							}
							else
							{
								b_skill->setBright(false);
							}
						}
						else
						{
							b_skill->setVisible(false);
						}
					}
					t_empty_room->setVisible(false);
				}
				else
				{
					b_head_player->setTag(-1);
					b_head_player->setBright(false);
					image_head->setVisible(false);
					image_refresh->setVisible(false);
					t_room_name->setPositionY(p_room->getBoundingBox().size.height * 0.5);
					t_com_name->setVisible(false);
					p_skill->setVisible(false);
					p_skill_sp->setVisible(false);
					t_empty_room->setVisible(true);
					b_room_info->setPositionX(t_room_name->getPositionX() + t_room_name->getBoundingBox().size.width
						+ b_room_info->getBoundingBox().size.width * 0.8);
					b_room_info->setPositionY(p_room->getBoundingBox().size.height * 0.5);
				}
			}
		}
	}
	else
	{
		image_mask->setVisible(false);
		image_pannel->setVisible(false);
	}
}

void UIShipyardFleetCabin::configCompanion()
{
	closeView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_GEAR_CABIN_CHOOSE_CSB]);
	openView(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_GEAR_CABIN_CHOOSE_CSB]);
	auto view = getViewRoot(SHIPYARD_COCOS_RES[SHIPYARD_FLEET_GEAR_CABIN_CHOOSE_CSB]);
	auto l_content = view->getChildByName<ListView*>("listview_content");
	auto p_item = view->getChildByName<Widget*>("panel_captain_1");
	p_item->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleetCabin::menuCall_func, this));
	p_item->setTouchEnabled(true);
	int index = m_pCompanionButton->getChildByName<ImageView*>("image_on")->getTag();
	
	l_content->getItem(1)->addTouchEventListener(CC_CALLBACK_2(UIShipyardFleetCabin::menuCall_func, this));
	l_content->getItem(1)->setTouchEnabled(true);
	//当前没有伙伴时
	if (m_pShipCResult->details[index]->companionsid < 1)
	{
		l_content->removeItem(1);
		l_content->removeItem(0);
	}
	else
	{
		l_content->getItem(0)->setTouchEnabled(false);
	}
	int n_grid1 = m_pCompaniesResult->n_infleet;
	int n_grid2 = m_pCompaniesResult->n_idlecompanies;
	int n_grid3 = m_pCompaniesResult->n_idlecaptains;
	int n_grid = n_grid1 + n_grid2 + n_grid3;
	int n_id = 0;
	int n_captain = 0;
	CompanyDefine *companyDefine;
	for (size_t i = 0; i < n_grid; i++)
	{
		auto item_clone = p_item->clone();
		Widget *item;
		if (i < n_grid1)
		{
			item_clone->setTag(i);
			n_id = m_pCompaniesResult->infleet[i]->id;
			n_captain = m_pCompaniesResult->infleet[i]->iscaptain;
			companyDefine = m_pCompaniesResult->infleet[i];
			if (n_id == m_pShipCResult->details[index]->companionsid && n_captain == m_pShipCResult->details[index]->iscaptain)
			{
				item = l_content->getItem(0);
				item->setTag(i);
			}
			else
			{
				item = item_clone;
				l_content->pushBackCustomItem(item_clone);
			}
			auto image_div = item->getChildByName<ImageView*>("image_div");
			auto i_position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_num_2"));
			auto t_position = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_position"));
			image_div->setTag(INFLEET_COMPANION);
			i_position->loadTexture(getPositionIconPath(companyDefine->fleetposition));
			if (t_position)
			{
				t_position->setString(getCompanionRoom(companyDefine->shipposition, m_nShipId));
			}
		}
		else if (i < n_grid1 + n_grid2)
		{
			item_clone->setTag(i - n_grid1);
			companyDefine = m_pCompaniesResult->idlecompanies[i - n_grid1];
			item = item_clone;
			l_content->pushBackCustomItem(item_clone);
			auto image_div = item_clone->getChildByName<ImageView*>("image_div");
			auto i_position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item_clone, "image_num_2"));
			auto t_position = dynamic_cast<Text*>(Helper::seekWidgetByName(item_clone, "label_position"));
			auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_captain_name"));
			image_div->setTag(IDLE_COMPANION);
			i_position->setVisible(false);
			t_position->setVisible(false);
			t_name->setPositionY(item->getBoundingBox().size.height * 0.5);
		}
		else if (i < n_grid1 + n_grid2 + n_grid3)
		{
			item_clone->setTag(i - n_grid1 - n_grid2);
			companyDefine = m_pCompaniesResult->idlecaptains[i - n_grid1 - n_grid2];
			item = item_clone;
			l_content->pushBackCustomItem(item_clone);
			auto image_div = item->getChildByName<ImageView*>("image_div");
			auto i_position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_num_2"));
			auto t_position = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_position"));
			auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_captain_name"));
			image_div->setTag(IDLE_CAPTAIN);
			i_position->setVisible(false);
			t_position->setVisible(false);
			t_name->setPositionY(item->getBoundingBox().size.height * 0.5);
		}
		
		auto i_captain_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_captain_head"));
		auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_lv"));
		auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_captain_name"));
		auto p_skill = dynamic_cast<Widget*>(Helper::seekWidgetByName(item, "panel_skill"));
		auto p_skill_sp = dynamic_cast<Widget*>(Helper::seekWidgetByName(item, "panel_skill_sp"));

		i_captain_head->loadTexture(getCompanionIconPath(companyDefine->protoid, companyDefine->iscaptain));
		t_lv->setString(StringUtils::format("Lv. %d", EXP_NUM_TO_LEVEL(companyDefine->exp)));
		t_name->setString(getCompanionName(companyDefine->protoid, companyDefine->iscaptain));
		

		//普通技能
		int skill1 = companyDefine->n_normalskill;
		if (skill1 > 0)
		{
			p_skill->setVisible(true);
			p_skill->setTag(i);
		}
		else
		{
			p_skill->setVisible(false);
		}

		for (size_t j = 0; j < 3; j++)
		{
			auto b_skill = p_skill->getChildByName<Button*>(StringUtils::format("button_equip_bg_%d", j + 1));
			if (j < skill1)
			{
				b_skill->setTouchEnabled(true);
				b_skill->setVisible(true);
				b_skill->setTag(j);
				auto t_skill = b_skill->getChildByName<Text*>("text_item_skill_lv");
				SKILL_TREE skill;
				if (companyDefine->iscaptain == 1)
				{
					skill = SINGLE_SHOP->getCaptainSkillInfo().at(companyDefine->normalskill[j]->id);
					b_skill->loadTextureNormal(getSkillIconPath(companyDefine->normalskill[j]->id, SKILL_TYPE_CAPTAIN));
					if (skill.room_type == getRoomType(companyDefine->shipposition) || (companyDefine->fleetposition > 1 && skill.effect_type == 1))
					{
						b_skill->setBright(true);
					}
					else
					{
						b_skill->setBright(false);
					}
				}
				else
				{
					skill = SINGLE_SHOP->getCompanionNormalSkillInfo().at(companyDefine->normalskill[j]->id);
					b_skill->loadTextureNormal(getSkillIconPath(companyDefine->normalskill[j]->id, SKILL_TYPE_COMPANION_NORMAL));
					if (skill.room_type == getRoomType(companyDefine->shipposition) || (companyDefine->fleetposition > 1 && skill.effect_type == 1))
					{
						b_skill->setBright(true);
					}
					else
					{
						b_skill->setBright(false);
					}
				}
				setTextSizeAndOutline(t_skill, companyDefine->normalskill[j]->level);
			}
			else
			{
				b_skill->setVisible(false);
			}
		}

		//特殊技能
		int skill2 = companyDefine->n_specialskill;
		if (skill2 > 0)
		{
			p_skill_sp->setVisible(true);
			p_skill_sp->setTag(i);
			p_skill->setPositionY(item->getBoundingBox().size.height * 0.5);
		}
		else
		{
			p_skill_sp->setVisible(false);
			p_skill->setPositionY(item->getBoundingBox().size.height * 0.25);
		}

		for (size_t j = 0; j < 3; j++)
		{
			auto b_skill = p_skill_sp->getChildByName<Button*>(StringUtils::format("button_sp_equip_bg_%d", j + 1));
			if (j < skill2)
			{
				b_skill->setTouchEnabled(true);
				b_skill->setVisible(true);
				b_skill->setTag(j);
				auto image_lock = b_skill->getChildByName<ImageView*>("image_lock");
				b_skill->loadTextureNormal(getSkillIconPath(companyDefine->specialskill[j]->id, SKILL_TYPE_PARTNER_SPECIAL));
				//解锁
				if (companyDefine->specialskill[j]->level > 0)
				{
					image_lock->setVisible(false);
				}
				else
				{
					image_lock->setVisible(true);
				}
				auto skill = SINGLE_SHOP->getCompanionSpecialiesSkillInfo().at(companyDefine->specialskill[j]->id);
				if (companyDefine->specialskill[j]->level > 0 && (skill.room_type == getRoomType(companyDefine->shipposition) || (companyDefine->fleetposition > 1 && skill.effect_type == 1)))
				{
					b_skill->setBright(true);
				}
				else
				{
					b_skill->setBright(false);
				}
			}
			else
			{
				b_skill->setVisible(false);
			}
		}
		
	}
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_content, image_pulldown);
}

void  UIShipyardFleetCabin::treatShipCabinNo()
{
	m_vShipCabinNo.clear();
	auto room_define = SINGLE_SHOP->getShipData().at(m_nShipId).room_define;
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
	int ship_type = SINGLE_SHOP->getShipData().at(m_nShipId).type;
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
					room_type = item.room_type;
					break;
				}
				else if (room_num.at(item.room_type - 1) > 0)
				{
					room_num.at(item.room_type - 1)--;
					room_type = item.room_type;
					break;
				}
				else
				{
					room_type = 0;
					break;
				}
			}
		}
		m_vShipCabinNo.push_back(room_type);
		//log("====room_type:%d====", room_type);
	}
}

void UIShipyardFleetCabin::addOrDelCompanion(Widget* parent, int id, bool isCaptain, int shipPosition)
{
	if (id == -1)
	{
		auto i_companion = parent->getChildByName<ImageView*>("Image_Companion");
		if (i_companion)
		{
			i_companion->removeFromParentAndCleanup(true);
		}
	}
	else
	{
		auto i_companion = parent->getChildByName<ImageView*>("Image_Companion");
		std::string path;
		//舰长室是玩家自己(特殊处理)
		if (shipPosition == 4)
		{
			path = StringUtils::format("res/companion_figure/player_icon_%d.png", SINGLE_HERO->m_iIconidx);
		}
		else if (isCaptain)
		{
			if (id < 101)
			{
				int idMan = id % 6 + 1;
				path = StringUtils::format("res/companion_figure/captain_man_%d.png", idMan);
			}
			else
			{
				int idWom = id % 6 + 1;
				path = StringUtils::format("res/companion_figure/captain_wom_%d.png", idWom);
			}
		}
		else
		{
			path = StringUtils::format("res/companion_figure/companion_%d.png", SINGLE_SHOP->getCompanionInfo().at(id).model_id);
		}

		if (!i_companion)
		{
			i_companion = ImageView::create(path);
			i_companion->setName("Image_Companion");
			i_companion->ignoreContentAdaptWithSize(false);

			auto room_type = getRoomType(shipPosition);
			i_companion->setContentSize(Size(parent->getContentSize().width / 2, parent->getContentSize().height / 2));
			i_companion->setAnchorPoint(Vec2(0.5, 0));
			//瞭望台
			if (room_type == 4)
			{
				i_companion->setPosition(Size(parent->getContentSize().width * 0.467, parent->getContentSize().height * 0.428));
			}
			else
			{
				i_companion->setPosition(Vec2(parent->getContentSize().width * 0.467, 20));
			}
			parent->addChild(i_companion);
		}
		else
		{
			i_companion->loadTexture(path);
		}
	}
}

void UIShipyardFleetCabin::onTouchesBegan(const std::vector<Touch*>& pTouches, Event *pEvent)
{
	//如果触摸点不少于两个  
	if (pTouches.size() >= 2)  
	{
		auto iter = pTouches.begin();
		Point mPoint1 = ((Touch *)(*iter))->getLocationInView();
		mPoint1 = Director::sharedDirector()->convertToGL(mPoint1);
		iter++;
		Point mPoint2 = ((Touch *)(*iter))->getLocationInView();
		mPoint2 = Director::sharedDirector()->convertToGL(mPoint2);
		//计算两个触摸点距离  
		distance = sqrt((mPoint2.x - mPoint1.x)*(mPoint2.x - mPoint1.x) + (mPoint2.y - mPoint1.y)*(mPoint2.y - mPoint1.y));
	}
	log("onTouchesBegan..");
}
void UIShipyardFleetCabin::onTouchesMoved(const std::vector<Touch*>& pTouches, Event *pEvent)
{
	//如果移动时触摸点的个数不少于两个  
	if (pTouches.size() >= 2)  
	{
		auto iter = pTouches.begin();
		Point mPoint1 = ((Touch*)(*iter))->getLocationInView();
		mPoint1 = Director::sharedDirector()->convertToGL(mPoint1);
		iter++;
		Point mPoint2 = ((Touch*)(*iter))->getLocationInView();
		//获得新触摸点两点之间的距离  
		mPoint2 = Director::sharedDirector()->convertToGL(mPoint2);        
		double mdistance = sqrt((mPoint1.x - mPoint2.x)*(mPoint1.x - mPoint2.x) + (mPoint1.y - mPoint2.y)*(mPoint1.y - mPoint2.y));
		//新的距离 / 老的距离  * 原来的缩放比例，即为新的缩放比例 
		mscale = mdistance / distance * mscale;                       
		s_cabin->setScale(mscale);
		std::string str = StringUtils::format("%s%0.2f", SINGLE_SHOP->getTipsInfo()["TIP_DOCK_CABIN_SCALE"].c_str(), mscale);
		dynamic_cast<Label*>(this->getChildByTag(300))->setString(str);
	}
	log("onTouchesMoved..");
}
void UIShipyardFleetCabin::onTouchesEnded(const std::vector<Touch*>& pTouches, Event *pEvent)
{

}
void UIShipyardFleetCabin::onTouchesCancelled(const std::vector<Touch*>& pTouches, Event *pEvent)
{

}

int UIShipyardFleetCabin::getRoomType(int shipPosition)
{
	if (shipPosition < 1)
	{
		return -1;
	} 
	auto shipType = SINGLE_SHOP->getShipData().at(m_nShipId).type;
	auto temp_shipPosition = 0;
	switch (shipType)
	{
	case 1:
		temp_shipPosition = shipPosition;
		break;
	case 2:
		temp_shipPosition = shipPosition + 16;
		break;
	case 3:
		temp_shipPosition = shipPosition + 16 + 24;
		break;
	default:
		break;
	}

	auto room_type = SINGLE_SHOP->getShipRoom().at(temp_shipPosition).room_type;
	return room_type;
}
void UIShipyardFleetCabin::updateCoinNum(const int64_t gold, const int64_t silver)
{
	SINGLE_HERO->m_iCoin = silver;
	SINGLE_HERO->m_iGold = gold;
	auto viewPad = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	if (viewPad)
	{
		auto panel_silver = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewPad, "panel_silver"));
		auto label_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_silver, "label_silver_num"));
		auto label_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_silver, "label_gold_num"));
		label_silver_num->setString(numSegment(StringUtils::format("%lld", silver)));
		label_gold_num->setString(numSegment(StringUtils::format("%lld", gold)));
		setTextFontSize(label_silver_num);
		setTextFontSize(label_gold_num);
	}
}
