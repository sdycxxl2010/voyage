#include "UICompanions.h"
#include "UICommon.h"
#include "UIInform.h"

#include "Utils.h"
#include "TVSceneLoader.h"
#include "UIMain.h"
#include "UIStore.h"

UICompanions::UICompanions()
{
	m_pCompaniesResult = nullptr;
	m_nIndex = -1;
	m_nCompaionType = -1;
	m_vNormalSkillDefine.clear();
	m_vSpecialSkillDefine.clear();
	for (size_t i = 0; i < 6; i++)
	{
		m_vEquipableItem[i].clear();
		m_bFirstFlush[i] = true;
	}
	m_pTempButton = nullptr;
	m_eType = SUB_TYPE_ROLE_EQUIP_HAT;
	m_eUIType = UI_PORT;
}

UICompanions::~UICompanions()
{
	unregisterCallBack();
	if (m_pCompaniesResult)
	{
		get_companies_result__free_unpacked(m_pCompaniesResult, 0);
	}
	SINGLE_HERO->m_bClickTouched = false;
}

void UICompanions::onEnter()
{
	UIBasicLayer::onEnter();
}

void UICompanions::onExit()
{
	UIBasicLayer::onExit();
}

UICompanions* UICompanions::createCompanionLayer()
{
	auto cp = new UICompanions;
	if (cp && cp->init())
	{
		cp->autorelease();
		return cp;
	}
	CC_SAFE_DELETE(cp);
	return nullptr;
}

bool UICompanions::init()
{
	bool pRet = false;
	do
	{
		UIBasicLayer::init();
		registerCallBack();

		ProtocolThread::GetInstance()->getCompanies(UILoadingIndicator::create(this));
		pRet = true;
	} while (0);
	return pRet;
}

void UICompanions::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetCompaniesResult:
		{
			auto companiesResult = (GetCompaniesResult*)(message);
			if (companiesResult->failed == 0)
			{
				m_pCompaniesResult = companiesResult;
				openCompanionList();
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
	case  PROTO_TYPE_GetEquipableItemsResult:
		{
			GetEquipableItemsResult* pResult = (GetEquipableItemsResult*)message;
			if (pResult->failed == 0)
			{
				if (m_bFirstFlush[pResult->type])
				{
					m_bFirstFlush[pResult->type] = false;
					m_vEquipableItem[pResult->type].clear();
					for (size_t i = 0; i < pResult->n_items; i++)
					{
						EquipableItem itemInfo;
						itemInfo.id = pResult->items[i]->id;
						itemInfo.iid = pResult->items[i]->iid;
						itemInfo.num = pResult->items[i]->num;
						m_vEquipableItem[pResult->type].push_back(itemInfo);
					}
				}
				flushEquipView();
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_GET_ROLL_EQUIP_FAIL");
			}
			break;
		}
	case  PROTO_TYPE_EquipCompanyResult:
		{
			 EquipCompanyResult* pResult = (EquipCompanyResult*)message;
			 if (pResult->failed == 0)
			 {
				 ProtocolThread::GetInstance()->getCompanies(UILoadingIndicator::create(this));
			 }
			 else
			 {
				 UIInform::getInstance()->openInformView(this);
				 UIInform::getInstance()->openConfirmYes("TIP_CENTER_ROLL_EQUIP_SAVE_FAIL");
			 }
			 break;
		}
	case  PROTO_TYPE_CompanionRebirthResult:
		{
			CompanionRebirthResult* pResult = (CompanionRebirthResult*)message;
			if (pResult->failed == 0)
			{
				ProtocolThread::GetInstance()->getCompanies(UILoadingIndicator::create(this));
				SINGLE_HERO->m_iGold = pResult->golds;
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_COMPANION_REBIRTH_SUCC");
				for (int i = 1; i < 6; i++)
				{
					m_bFirstFlush[i] = true;
				}
				auto currentScene = Director::getInstance()->getRunningScene();
				auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
				if (mainlayer)
				{
					mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
				}	
				Utils::consumeVTicket("22", 1, pResult->usegolds);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
	default:
		break;
	}
}

void UICompanions::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}

	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	Widget* widget = dynamic_cast<Widget*>(pSender);
	auto name = widget->getName();
	//回到主城
	if (isButton(button_main_city))
	{
		this->removeFromParentAndCleanup(true);
		return;
	}
	//小伙伴详情
	if (isButton(button_captain_head) || isButton(image_companions_bg_))
	{
		m_nIndex = widget->getTag();
		auto i_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(widget, "image_captain_head"));
		m_nCompaionType = i_head->getTag();
		openCompanionDetails();
		return;
	}
	//关闭伙伴详情(---不要与其他的合并)
	if (isButton(button_close))
	{
		for (size_t i = 1; i < 6; i++)
		{
			if (!m_bFirstFlush[i])
			{
				ProtocolThread::GetInstance()->equipCompany(widget->getTag(), widget->getParent()->getTag(), m_curEquipItem[SUB_TYPE_ROLE_EQUIP_HAT].id,
					m_curEquipItem[SUB_TYPE_ROLE_EQUIP_CLOTHES].id, m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ARM].id, m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ACCESSORY].id,
					m_curEquipItem[SUB_TYPE_ROLE_EQUIP_SHOE].id, UILoadingIndicator::create(this));
				break;
			}
		}

		closeView();
		for (size_t i = 1; i < 6; i++)
		{
			m_bFirstFlush[i] = true;
		}
		return;
	}
	//则图片详情
	if (isButton(panel_v_ticketdetails))
	{
		closeView();
		return;
	}
	//图片详情
	if (isButton(image_companions))
	{
		auto image_bg = widget->getChildByName<ImageView*>("image_companions_bg");
		openView(COMPANION_RES[DETAILS_VIEWCARD_CSB]);
		auto view = getViewRoot(COMPANION_RES[DETAILS_VIEWCARD_CSB]);
		view->addTouchEventListener(CC_CALLBACK_2(UICompanions::menuCall_func, this));
		auto i_companion = view->getChildByName<ImageView*>("image_companions");
		auto endPos = i_companion->getPosition();
		i_companion->setPosition(widget->getPosition());
		i_companion->loadTexture(getCompanionCardPath(widget->getTag(), image_bg->getTag()));
		i_companion->setScale(0.95f);
		i_companion->runAction(Spawn::createWithTwoActions(ScaleTo::create(0.1f, 1.1), MoveTo::create(0.1f, endPos)));
		return;
	}
	//技能升级
	if (isButton(button_equip_bg))
	{
		auto t_lv = widget->getChildByName<Text*>("text_item_skill_lv");
		UICommon::getInstance()->openCommonView(this);
		int index = widget->getTag();
		SKILL_DEFINE skill_define;
		if (t_lv)
		{
			skill_define = m_vNormalSkillDefine.at(index);
		}
		else
		{
			skill_define = m_vSpecialSkillDefine.at(index);
		}
		UICommon::getInstance()->flushSkillView(skill_define);
		return;
	}
	//装备--帽子
	if (isButton(button_maozi))
	{
		m_eType = SUB_TYPE_ROLE_EQUIP_HAT;
		if (m_bFirstFlush[SUB_TYPE_ROLE_EQUIP_HAT])
		{
			ProtocolThread::GetInstance()->getEquipableItems(SUB_TYPE_ROLE_EQUIP_HAT, 1, UILoadingIndicator::create(this));
		}
		else
		{
			flushEquipView();
		}
		return;
	}

	//装备--衣服
	if (isButton(button_yifu))
	{
		m_eType = SUB_TYPE_ROLE_EQUIP_CLOTHES;
		if (m_bFirstFlush[SUB_TYPE_ROLE_EQUIP_CLOTHES])
		{
			ProtocolThread::GetInstance()->getEquipableItems(SUB_TYPE_ROLE_EQUIP_CLOTHES, 1, UILoadingIndicator::create(this));
		}
		else
		{
			flushEquipView();
		}
		return;
	}
	//装备--鞋子
	if (isButton(button_xiezi))
	{
		m_eType = SUB_TYPE_ROLE_EQUIP_SHOE;
		if (m_bFirstFlush[SUB_TYPE_ROLE_EQUIP_SHOE])
		{
			ProtocolThread::GetInstance()->getEquipableItems(SUB_TYPE_ROLE_EQUIP_SHOE, 1, UILoadingIndicator::create(this));
		}
		else
		{
			flushEquipView();
		}
		return;
	}
	//装备--武器
	if (isButton(button_wuqi))
	{
		m_eType = SUB_TYPE_ROLE_EQUIP_ARM;
		if (m_bFirstFlush[SUB_TYPE_ROLE_EQUIP_ARM])
		{
			ProtocolThread::GetInstance()->getEquipableItems(SUB_TYPE_ROLE_EQUIP_ARM, 1, UILoadingIndicator::create(this));
		}
		else
		{
			flushEquipView();
		}
		return;
	}
	//装备--首饰
	if (isButton(button_shoushi))
	{
		m_eType = SUB_TYPE_ROLE_EQUIP_ACCESSORY;
		if (m_bFirstFlush[SUB_TYPE_ROLE_EQUIP_ACCESSORY])
		{
			ProtocolThread::GetInstance()->getEquipableItems(SUB_TYPE_ROLE_EQUIP_ACCESSORY, 1, UILoadingIndicator::create(this));
		}
		else
		{
			flushEquipView();
		}
		return;
	}
	//装备的详情
	if (isButton(select_equip_bg) || isButton(current_equip_bg))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushItemsDetail(nullptr, widget->getTag(), true);
		return;
	}

	//提示取消按钮
	if (isButton(button_confirm_no))
	{
		m_pTempButton = nullptr;
		return;
	}

	//选择装备取消按钮
	if (isButton(button_cancel))
	{
		m_pTempButton = nullptr;
		closeView();
		return;
	}

	//确认按钮
	if (isButton(button_confirm_yes))
	{
		EquipableItem itemInfo;
		itemInfo.id = m_curEquipItem[m_eType].id;
		itemInfo.iid = m_curEquipItem[m_eType].iid;
		itemInfo.num = m_curEquipItem[m_eType].num;
		m_curEquipItem[m_eType].id = 0;
		m_curEquipItem[m_eType].num = 0;
		m_vEquipableItem[m_eType].push_back(itemInfo);
		closeView();
		updataEquipDetails();
		m_pTempButton = nullptr;
		return;
	}

	//选择
	if (isButton(button_ok))
	{
		if (m_pTempButton)
		{
			if (m_curEquipItem[m_eType].num != 0)
			{
				auto tempInfo = m_curEquipItem[m_eType];
				m_curEquipItem[m_eType] = m_vEquipableItem[m_eType].at(m_pTempButton->getTag());
				m_vEquipableItem[m_eType].at(m_pTempButton->getTag()) = tempInfo;
			}
			else
			{
				m_curEquipItem[m_eType] = m_vEquipableItem[m_eType].at(m_pTempButton->getTag());
				m_vEquipableItem[m_eType].erase(m_vEquipableItem[m_eType].begin ()+ m_pTempButton->getTag());
			}
		}
		m_pTempButton = nullptr;
		closeView();
		updataEquipDetails();
		return;
	}

	//装备--卸载
	if (isButton(button_uninstall))
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_DOCK_FLEET_CONFIRM_SAVE_TITLE", "TIP_CENTER_ROLL_EQUIP_UNINSTALL");
		return;
	}
	//选中物品
	if (isButton(button_equip_items))
	{
		if (m_pTempButton)
		{
			m_pTempButton->setBright(true);
		}
		m_pTempButton = widget;
		m_pTempButton->setBright(false);
		auto p_addequip = getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CHANGE_CSB]);
		auto b_ok = p_addequip->getChildByName<Button*>("button_ok");
		b_ok->setBright(true);
		b_ok->setTouchEnabled(true);
		updataSelectEquip();
		return;
	}
	//小伙伴重生
	if (isButton(button_reborn))
	{
		if (widget->isBright())
		{
			CompanyDefine* companionInfo;
			switch (m_nCompaionType)
			{
			case UICompanions::INFLEET_COMPANION:
				companionInfo = m_pCompaniesResult->infleet[m_nIndex];
				break;
			case UICompanions::IDLE_COMPANION:
				companionInfo = m_pCompaniesResult->idlecompanies[m_nIndex];
				break;
			case UICompanions::IDLE_CAPTAIN:
				companionInfo = m_pCompaniesResult->idlecaptains[m_nIndex];
				break;
			default:
				break;
			}
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushVConfirmView("TIP_COMPANION_REBORN_TITLE", "TIP_COMPANION_REBORN_CONTENT", companionInfo->rebirthgolds);
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COMPANION_NO_REBIRTH");
		}
		return;
	}

	if (isButton(button_v_yes))
	{
		CompanyDefine* companionInfo;
		switch (m_nCompaionType)
		{
		case UICompanions::INFLEET_COMPANION:
			companionInfo = m_pCompaniesResult->infleet[m_nIndex];
			break;
		case UICompanions::IDLE_COMPANION:
			companionInfo = m_pCompaniesResult->idlecompanies[m_nIndex];
			break;
		case UICompanions::IDLE_CAPTAIN:
			companionInfo = m_pCompaniesResult->idlecaptains[m_nIndex];
			break;
		default:
			break;
		}

		if (SINGLE_HERO->m_iGold >= companionInfo->rebirthgolds)
		{
			ProtocolThread::GetInstance()->companionRebirth(companionInfo->id, companionInfo->iscaptain, UILoadingIndicator::create(this));
		}
		else
		{
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
		}
	}
	if (isButton(image_speed))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(widget);
		return;
	}
	if (isButton(image_atk))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(widget);
		return;
	}
	if (isButton(image_def))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(widget);
		return;
	}
}

void UICompanions::openCompanionList()
{
	openView(COMPANION_RES[COMPANIONS_CSB]);
	auto view = getViewRoot(COMPANION_RES[COMPANIONS_CSB]);
	auto t_title = view->getChildByName<Text*>("text_title");
	auto i_companions_no = view->getChildByName<ImageView*>("image_companions_bg_no");
	auto p_content = view->getChildByName<Widget*>("panel_content");
	auto l_companion = view->getChildByName<ListView*>("listview_content");
	l_companion->removeAllChildrenWithCleanup(true);
	int n_grid1 = m_pCompaniesResult->n_infleet;
	int n_line1 = ceil(n_grid1 / 3.0);

	auto t_title_clone1 = dynamic_cast<Text*>(t_title->clone());
	t_title_clone1->setString("  " + SINGLE_SHOP->getTipsInfo()["TIP_COMPANION_IN_FLEET_TITLE"]);
	l_companion->pushBackCustomItem(t_title_clone1);
	if (n_grid1 < 1)
	{
		auto i_no_clone = dynamic_cast<ImageView*>(i_companions_no->clone());
		auto t_no = i_no_clone->getChildByName<Text*>("label_name");
		t_no->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMPANION_IN_FLEET_CONTENT"]);
		l_companion->pushBackCustomItem(i_no_clone);
	}

	for (size_t i = 0; i < n_line1; i++)
	{
		auto p_content_clone = p_content->clone();
		for (size_t j = 0; j < 3; j++)
		{
			auto image_item = p_content_clone->getChildByName<ImageView*>(StringUtils::format("image_companions_bg_%d",j + 1));
			if (i * 3 + j < n_grid1)
			{
				auto infleetInfo = m_pCompaniesResult->infleet[3 * i + j];
				image_item->setVisible(true);
				auto b_head_bg = image_item->getChildByName<Button*>("button_captain_head");
				auto i_head = b_head_bg->getChildByName<ImageView*>("image_captain_head");
				auto i_position = b_head_bg->getChildByName<ImageView*>("image_num");
				auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(b_head_bg, "label_lv"));
				auto t_name = image_item->getChildByName<Text*>("label_name");
				auto t_position = image_item->getChildByName<Text*>("label_position");
				i_head->loadTexture(getCompanionIconPath(infleetInfo->protoid, infleetInfo->iscaptain));
				i_head->setTag(INFLEET_COMPANION);
				image_item->setTouchEnabled(true);
				image_item->setTag(3 * i + j);
				image_item->addTouchEventListener(CC_CALLBACK_2(UICompanions::menuCall_func, this));
				b_head_bg->setTag(3 * i + j);
				i_position->loadTexture(getPositionIconPath(infleetInfo->fleetposition));
				int level = EXP_NUM_TO_LEVEL(infleetInfo->exp);
				t_lv->setString(StringUtils::format("Lv. %d", level));
				t_name->setString(getCompanionName(infleetInfo->protoid, infleetInfo->iscaptain));
				t_position->setString(getCompanionRoom(infleetInfo->shipposition, infleetInfo->sid));
			}
			else
			{
				image_item->setVisible(false);
			}
		}
		l_companion->pushBackCustomItem(p_content_clone);
	}

	int n_grid2 = m_pCompaniesResult->n_idlecompanies;
	int n_line2 = ceil(n_grid2 / 3.0);
	auto t_title_clone2 = dynamic_cast<Text*>(t_title->clone());
	t_title_clone2->setString("  " + SINGLE_SHOP->getTipsInfo()["TIP_COMPANION_IDLE_COMPANION_TITLE"]);
	l_companion->pushBackCustomItem(t_title_clone2);
	if (n_grid2 < 1)
	{
		auto i_no_clone = dynamic_cast<ImageView*>(i_companions_no->clone());
		auto t_no = i_no_clone->getChildByName<Text*>("label_name");
		t_no->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMPANION_IDLE_COMPANION_CONTENT"]);
		l_companion->pushBackCustomItem(i_no_clone);
	}

	for (size_t i = 0; i < n_line2; i++)
	{
		auto p_content_clone = p_content->clone();
		for (size_t j = 0; j < 3; j++)
		{
			auto image_item = p_content_clone->getChildByName<ImageView*>(StringUtils::format("image_companions_bg_%d", j + 1));
			if (i * 3 + j < n_grid2)
			{
				auto idlecomInfo = m_pCompaniesResult->idlecompanies[3 * i + j];
				image_item->setVisible(true);
				auto b_head_bg = image_item->getChildByName<Button*>("button_captain_head");
				auto i_head = b_head_bg->getChildByName<ImageView*>("image_captain_head");
				auto i_position = b_head_bg->getChildByName<ImageView*>("image_num");
				auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(b_head_bg, "label_lv"));
				auto t_name = image_item->getChildByName<Text*>("label_name");
				auto t_position = image_item->getChildByName<Text*>("label_position");
				i_head->loadTexture(getCompanionIconPath(idlecomInfo->protoid, idlecomInfo->iscaptain));
				i_head->setTag(IDLE_COMPANION);
				image_item->setTouchEnabled(true);
				image_item->setTag(3 * i + j);
				image_item->addTouchEventListener(CC_CALLBACK_2(UICompanions::menuCall_func, this));
				b_head_bg->setTag(3 * i + j);
				i_position->setVisible(false);
				int level = EXP_NUM_TO_LEVEL(idlecomInfo->exp);
				t_lv->setString(StringUtils::format("Lv. %d", level));
				t_name->setString(getCompanionName(idlecomInfo->protoid, idlecomInfo->iscaptain));
				t_position->setVisible(false);
			}
			else
			{
				image_item->setVisible(false);
			}
		}
		l_companion->pushBackCustomItem(p_content_clone);
	}

	int n_grid3 = m_pCompaniesResult->n_idlecaptains;
	int n_line3 = ceil(n_grid3 / 3.0);
	auto t_title_clone3 = dynamic_cast<Text*>(t_title->clone());
	t_title_clone3->setString("  " + SINGLE_SHOP->getTipsInfo()["TIP_COMPANION_IDLE_CAPTAINS_TITLE"]);
	l_companion->pushBackCustomItem(t_title_clone3);
	if (n_grid3 < 1)
	{
		auto i_no_clone = dynamic_cast<ImageView*>(i_companions_no->clone());
		auto t_no = i_no_clone->getChildByName<Text*>("label_name");
		t_no->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMPANION_IDLE_CAPTAINS_CONTENT"]);
		l_companion->pushBackCustomItem(i_no_clone);
	}

	for (size_t i = 0; i < n_line3; i++)
	{
		auto p_content_clone = p_content->clone();
		for (size_t j = 0; j < 3; j++)
		{
			auto image_item = p_content_clone->getChildByName<ImageView*>(StringUtils::format("image_companions_bg_%d", j + 1));
			if (i * 3 + j < n_grid3)
			{
				auto idlecapInfo = m_pCompaniesResult->idlecaptains[3 * i + j];
				image_item->setVisible(true);
				auto b_head_bg = image_item->getChildByName<Button*>("button_captain_head");
				auto i_head = b_head_bg->getChildByName<ImageView*>("image_captain_head");
				auto i_position = b_head_bg->getChildByName<ImageView*>("image_num");
				auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(b_head_bg, "label_lv"));
				auto t_name = image_item->getChildByName<Text*>("label_name");
				auto t_position = image_item->getChildByName<Text*>("label_position");
				i_head->loadTexture(getCompanionIconPath(idlecapInfo->protoid, idlecapInfo->iscaptain));
				i_head->setTag(IDLE_CAPTAIN);
				image_item->setTouchEnabled(true);
				image_item->setTag(3 * i + j);
				image_item->addTouchEventListener(CC_CALLBACK_2(UICompanions::menuCall_func, this));
				b_head_bg->setTag(3 * i + j);
				i_position->setVisible(false);
				int level = EXP_NUM_TO_LEVEL(idlecapInfo->exp);
				t_lv->setString(StringUtils::format("Lv. %d", level));
				t_name->setString(getCompanionName(idlecapInfo->protoid, idlecapInfo->iscaptain));
				t_position->setVisible(false);
			}
			else
			{
				image_item->setVisible(false);
			}
		}
		l_companion->pushBackCustomItem(p_content_clone);
	}

	auto p_salary = view->getChildByName<Widget*>("panel_salary");
	auto t_salary = p_salary->getChildByName<Text*>("label_salary");
	auto i_silver = p_salary->getChildByName<ImageView*>("image_silver");
	i_silver->setPositionX(t_salary->getPositionX() + t_salary->getBoundingBox().size.width);
	auto t_wage = p_salary->getChildByName<Text*>("label_wage");
	t_wage->setString(StringUtils::format("%d/%s", m_pCompaniesResult->totalsalary, SINGLE_SHOP->getTipsInfo()["TIP_DAY"].c_str()));
	t_wage->setPositionX(i_silver->getPositionX() + i_silver->getBoundingBox().size.width + 5);
	
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_companion, image_pulldown);
	
	auto view1 = getViewRoot(COMPANION_RES[COMPANIONS_DETAILS_CSB]);
	if (view1)
	{
		openCompanionDetails();
	}
}

void UICompanions::openCompanionDetails()
{
	openView(COMPANION_RES[COMPANIONS_DETAILS_CSB]);
	auto view = getViewRoot(COMPANION_RES[COMPANIONS_DETAILS_CSB]);
	auto i_speed = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_speed"));
	i_speed->setTouchEnabled(true);
	i_speed->addTouchEventListener(CC_CALLBACK_2(UICompanions::menuCall_func, this));
	i_speed->setTag(IMAGE_ICON_HEALTHPOWER + IMAGE_INDEX2);
	auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_atk"));
	i_atk->setTouchEnabled(true);
	i_atk->addTouchEventListener(CC_CALLBACK_2(UICompanions::menuCall_func, this));
	i_atk->setTag(IMAGE_ICON_ATTACKPOWER + IMAGE_INDEX2);
	auto i_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_def"));
	i_def->setTouchEnabled(true);
	i_def->addTouchEventListener(CC_CALLBACK_2(UICompanions::menuCall_func, this));
	i_def->setTag(IMAGE_ICON_DEFENSEPOWER + IMAGE_INDEX2);


	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_item_name"));
	auto i_companion = view->getChildByName<ImageView*>("image_companions");	
	auto image_bg = i_companion->getChildByName<ImageView*>("image_companions_bg");
	CompanyDefine* companionInfo;
	switch (m_nCompaionType)
	{
	case UICompanions::INFLEET_COMPANION:
		companionInfo = m_pCompaniesResult->infleet[m_nIndex];
		break;
	case UICompanions::IDLE_COMPANION:
		companionInfo = m_pCompaniesResult->idlecompanies[m_nIndex];
		break;
	case UICompanions::IDLE_CAPTAIN:
		companionInfo = m_pCompaniesResult->idlecaptains[m_nIndex];
		break;
	default:
		break;
	}
	t_name->setString(getCompanionName(companionInfo->protoid, companionInfo->iscaptain));
	i_companion->loadTexture(getCompanionCardPath(companionInfo->protoid, companionInfo->iscaptain));
	i_companion->setTag(companionInfo->protoid);
	image_bg->setTag(companionInfo->iscaptain);
	i_companion->setTouchEnabled(true);
	i_companion->addTouchEventListener(CC_CALLBACK_2(UICompanions::menuCall_func, this));

	auto p_content = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_content"));
	auto panel1 = p_content->getChildByName<Widget*>("Panel_1");
	auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel1, "label_lv"));
	auto t_exp_title = panel1->getChildByName<Text*>("label_exp");
	auto t_exp_num = panel1->getChildByName<Text*>("label_exp_num");
	int level = EXP_NUM_TO_LEVEL(companionInfo->exp);
	t_lv->setString(StringUtils::format("Lv. %d", level));
	std::string cur_exp = StringUtils::format("%d",companionInfo->exp);
	std::string next_exp = StringUtils::format("%d", LEVEL_TO_EXP_NUM(level + 1));
	t_exp_num->setString(StringUtils::format("%s/%s",numSegment(cur_exp).c_str(), numSegment(next_exp).c_str()));
	t_exp_num->setPositionX(t_exp_title->getPositionX() + t_exp_title->getBoundingBox().size.width);
	
	m_vNormalSkillDefine.clear();
	//普通技能
	for (size_t i = 0; i < 3; i++)
	{
		auto p_skill = panel1->getChildByName<ImageView*>(StringUtils::format("panel_skill_%d", i + 1));
		if (i < companionInfo->n_normalskill)
		{
			auto skill = companionInfo->normalskill[i];
			p_skill->setVisible(true);
			auto b_skill = p_skill->getChildByName<Button*>("button_equip_bg");
			auto t_lv = b_skill->getChildByName<Text*>("text_item_skill_lv");
			auto t_name = p_skill->getChildByName<Text*>("label_current_itemname");
			SKILL_TYPE skill_type;
			if (companionInfo->iscaptain)
			{
				skill_type = SKILL_TYPE_CAPTAIN;
			}
			else
			{
				skill_type = SKILL_TYPE_COMPANION_NORMAL;
			}
			b_skill->loadTextureNormal(getSkillIconPath(skill->id, skill_type));
			setTextSizeAndOutline(t_lv, skill->level);
			t_name->setString(getSkillName(skill->id, skill_type));
			b_skill->setTouchEnabled(true);
			b_skill->setTag(i);

			SKILL_DEFINE skillDefine;
			skillDefine.id = skill->id;
			skillDefine.lv = skill->level;
			skillDefine.skill_type = skill_type;
			skillDefine.icon_id = companionInfo->protoid;
			m_vNormalSkillDefine.push_back(skillDefine);
		}
		else
		{
			p_skill->setVisible(false);
		}
	}

	m_vSpecialSkillDefine.clear();
	if (companionInfo->n_specialskill < 1)
	{
		p_content->setContentSize(Size(590, 518));
		p_content->getChildByName<Text*>("text_title_sp_skill")->setVisible(false);
		panel1->setPositionY(155);
	}
	//特殊技能
	for (size_t i = 0; i < 3; i++)
	{
		auto p_skill = p_content->getChildByName<ImageView*>(StringUtils::format("panel_skill_%d", i + 4));
		if (i < companionInfo->n_specialskill)
		{
			auto skill = companionInfo->specialskill[i];
			p_skill->setVisible(true);
			auto b_skill = p_skill->getChildByName<Button*>("button_equip_bg");
			auto t_name = p_skill->getChildByName<Text*>("label_current_itemname");
			auto i_lock = p_skill->getChildByName<ImageView*>("image_lock");
			b_skill->loadTextureNormal(getSkillIconPath(skill->id, SKILL_TYPE_PARTNER_SPECIAL));
			t_name->setString(getSkillName(skill->id, SKILL_TYPE_PARTNER_SPECIAL));
			b_skill->setTouchEnabled(true);
			b_skill->setTag(i);

			if (skill->level > 0)
			{
				i_lock->setVisible(false);
				b_skill->setBright(true);
			}
			else
			{
				i_lock->setVisible(true);
				b_skill->setBright(false);
			}

			SKILL_DEFINE skillDefine;
			skillDefine.id = skill->id;
			skillDefine.lv = skill->level;
			skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
			skillDefine.icon_id = companionInfo->protoid;
			m_vSpecialSkillDefine.push_back(skillDefine);
		}
		else
		{
			p_skill->setVisible(false);
		}
	}

	auto p_salary = p_content->getChildByName<ImageView*>("panel_salary");
	auto i_silver = p_content->getChildByName<ImageView*>("image_silver");
	auto t_wage = p_content->getChildByName<Text*>("label_wage");
	t_wage->setString(StringUtils::format("%d/%s", companionInfo->salary, SINGLE_SHOP->getTipsInfo()["TIP_DAY"].c_str()));
	t_wage->setPositionX(i_silver->getPositionX() + i_silver->getBoundingBox().size.width / 2 + t_wage->getBoundingBox().size.width / 2 + 5);

	auto l_content = view->getChildByName<ListView*>("listview_content");
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_content, image_pulldown);
	if (companionInfo->head)
	{
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_HAT].iid = companionInfo->head->iid;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_HAT].id = companionInfo->head->uniqueid;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_HAT].num = 1;
	}
	else
	{
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_HAT].id = 0;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_HAT].num = 0;
	}

	if (companionInfo->clothes)
	{
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_CLOTHES].iid = companionInfo->clothes->iid;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_CLOTHES].id = companionInfo->clothes->uniqueid;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_CLOTHES].num = 1;
	}
	else
	{
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_CLOTHES].id = 0;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_CLOTHES].num = 0;
	}


	if (companionInfo->weapon)
	{
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ARM].iid = companionInfo->weapon->iid;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ARM].id = companionInfo->weapon->uniqueid;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ARM].num = 1;
	}
	else
	{
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ARM].id = 0;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ARM].num = 0;
	}

	if (companionInfo->access)
	{
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ACCESSORY].iid = companionInfo->access->iid;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ACCESSORY].id = companionInfo->access->uniqueid;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ACCESSORY].num = 1;
	}
	else
	{
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ACCESSORY].id = 0;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ACCESSORY].num = 0;
	}

	if (companionInfo->shoe)
	{
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_SHOE].iid = companionInfo->shoe->iid;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_SHOE].id = companionInfo->shoe->uniqueid;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_SHOE].num = 1;
	}
	else
	{
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_SHOE].id = 0;
		m_curEquipItem[SUB_TYPE_ROLE_EQUIP_SHOE].num = 0;
	}
	view->setTag(companionInfo->iscaptain);
	view->getChildByName<Button*>("button_close")->setTag(companionInfo->id);
	updataEquipDetails();
	auto b_reborn = view->getChildByName<Button*>("button_reborn");
	auto image_v = b_reborn->getChildByName<ImageView*>("image_v");
	if (level > 1)
	{
		b_reborn->setBright(true);
		setGLProgramState(image_v, false);
	}
	else
	{
		b_reborn->setBright(false);
		setGLProgramState(image_v, true);
	}
}

void UICompanions::updataEquipDetails()
{
	CompanyDefine* companionInfo;
	switch (m_nCompaionType)
	{
	case UICompanions::INFLEET_COMPANION:
		companionInfo = m_pCompaniesResult->infleet[m_nIndex];
		break;
	case UICompanions::IDLE_COMPANION:
		companionInfo = m_pCompaniesResult->idlecompanies[m_nIndex];
		break;
	case UICompanions::IDLE_CAPTAIN:
		companionInfo = m_pCompaniesResult->idlecaptains[m_nIndex];
		break;
	default:
		break;
	}

	auto view = getViewRoot(COMPANION_RES[COMPANIONS_DETAILS_CSB]);
	auto p_content = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_content"));
	auto panel1 = p_content->getChildByName<Widget*>("Panel_1");
	auto i_property_1 = panel1->getChildByName<ImageView*>("image_property_1");
	auto i_property_2 = panel1->getChildByName<ImageView*>("image_property_2");
	auto i_property_3 = panel1->getChildByName<ImageView*>("image_property_3");
	auto t_hp = i_property_1->getChildByName<Text*>("label_speed_num");
	auto t_atk = i_property_2->getChildByName<Text*>("label_atk_num");
	auto t_def = i_property_3->getChildByName<Text*>("label_defnum");
	int hp = companionInfo->hp;
	int atk = companionInfo->attack;
	int def = companionInfo->defense;
	auto itemInfo = SINGLE_SHOP->getItemData();

	if (companionInfo->head)
	{
		auto temp = getEquipAttribute(itemInfo[companionInfo->head->iid].property2);
		hp -= temp[CHARACTER_HP];
		atk -= temp[CHARACTER_ATTACK];
		def -= temp[CHARACTER_DEFENCE];
	}

	if (m_curEquipItem[SUB_TYPE_ROLE_EQUIP_HAT].num != 0)
	{
		auto temp = getEquipAttribute(itemInfo[m_curEquipItem[SUB_TYPE_ROLE_EQUIP_HAT].iid].property2);
		hp += temp[CHARACTER_HP];
		atk += temp[CHARACTER_ATTACK];
		def += temp[CHARACTER_DEFENCE];
	}

	if (companionInfo->clothes)
	{
		auto temp = getEquipAttribute(itemInfo[companionInfo->clothes->iid].property2);
		hp -= temp[CHARACTER_HP];
		atk -= temp[CHARACTER_ATTACK];
		def -= temp[CHARACTER_DEFENCE];
	}

	if (m_curEquipItem[SUB_TYPE_ROLE_EQUIP_CLOTHES].num != 0)
	{
		auto temp = getEquipAttribute(itemInfo[m_curEquipItem[SUB_TYPE_ROLE_EQUIP_CLOTHES].iid].property2);
		hp += temp[CHARACTER_HP];
		atk += temp[CHARACTER_ATTACK];
		def += temp[CHARACTER_DEFENCE];
	}

	if (companionInfo->weapon)
	{
		auto temp = getEquipAttribute(itemInfo[companionInfo->weapon->iid].property2);
		hp -= temp[CHARACTER_HP];
		atk -= temp[CHARACTER_ATTACK];
		def -= temp[CHARACTER_DEFENCE];
	}

	if (m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ARM].num != 0)
	{
		auto temp = getEquipAttribute(itemInfo[m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ARM].iid].property2);
		hp += temp[CHARACTER_HP];
		atk += temp[CHARACTER_ATTACK];
		def += temp[CHARACTER_DEFENCE];
	}

	if (companionInfo->access)
	{
		auto temp = getEquipAttribute(itemInfo[companionInfo->access->iid].property2);
		hp -= temp[CHARACTER_HP];
		atk -= temp[CHARACTER_ATTACK];
		def -= temp[CHARACTER_DEFENCE];
	}

	if (m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ACCESSORY].num != 0)
	{
		auto temp = getEquipAttribute(itemInfo[m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ACCESSORY].iid].property2);
		hp += temp[CHARACTER_HP];
		atk += temp[CHARACTER_ATTACK];
		def += temp[CHARACTER_DEFENCE];
	}

	if (companionInfo->shoe)
	{
		auto temp = getEquipAttribute(itemInfo[companionInfo->shoe->iid].property2);
		hp -= temp[CHARACTER_HP];
		atk -= temp[CHARACTER_ATTACK];
		def -= temp[CHARACTER_DEFENCE];
	}

	if (m_curEquipItem[SUB_TYPE_ROLE_EQUIP_SHOE].num != 0)
	{
		auto temp = getEquipAttribute(itemInfo[m_curEquipItem[SUB_TYPE_ROLE_EQUIP_SHOE].iid].property2);
		hp += temp[CHARACTER_HP];
		atk += temp[CHARACTER_ATTACK];
		def += temp[CHARACTER_DEFENCE];
	}

	t_hp->setString(StringUtils::format("%d", hp));
	t_atk->setString(StringUtils::format("%d", atk));
	t_def->setString(StringUtils::format("%d", def));

	auto b_maozi = panel1->getChildByName<Button*>("button_maozi");
	auto b_yifu = panel1->getChildByName<Button*>("button_yifu");
	auto b_xiezi = panel1->getChildByName<Button*>("button_xiezi");
	auto b_wuqi = panel1->getChildByName<Button*>("button_wuqi");
	auto b_shoushi = panel1->getChildByName<Button*>("button_shoushi");

	auto i_maozi = b_maozi->getChildByName<ImageView*>("image_equip");
	auto i_yifu = b_yifu->getChildByName<ImageView*>("image_equip");
	auto i_xiezi = b_xiezi->getChildByName<ImageView*>("image_equip");
	auto i_wuqi = b_wuqi->getChildByName<ImageView*>("image_equip");
	auto i_shoushi = b_shoushi->getChildByName<ImageView*>("image_equip");
	
	if (m_curEquipItem[SUB_TYPE_ROLE_EQUIP_HAT].num != 0)
	{
		b_maozi->setBright(false);
		i_maozi->setVisible(true);
		i_maozi->loadTexture(getItemIconPath(m_curEquipItem[SUB_TYPE_ROLE_EQUIP_HAT].iid));
		b_maozi->getChildByName<Button*>("button_add")->setVisible(false);
	}
	else
	{
		b_maozi->setBright(true);
		i_maozi->setVisible(false);
		b_maozi->getChildByName<Button*>("button_add")->setVisible(true);
	}

	if (m_curEquipItem[SUB_TYPE_ROLE_EQUIP_CLOTHES].num != 0)
	{
		b_yifu->setBright(false);
		i_yifu->setVisible(true);
		i_yifu->loadTexture(getItemIconPath(m_curEquipItem[SUB_TYPE_ROLE_EQUIP_CLOTHES].iid));
		b_yifu->getChildByName<Button*>("button_add")->setVisible(false);
	}
	else
	{
		b_yifu->setBright(true);
		i_yifu->setVisible(false);
		b_yifu->getChildByName<Button*>("button_add")->setVisible(true);
	}

	if (m_curEquipItem[SUB_TYPE_ROLE_EQUIP_SHOE].num != 0)
	{
		b_xiezi->setBright(false);
		i_xiezi->setVisible(true);
		i_xiezi->loadTexture(getItemIconPath(m_curEquipItem[SUB_TYPE_ROLE_EQUIP_SHOE].iid));
		b_xiezi->getChildByName<Button*>("button_add")->setVisible(false);
	}
	else
	{
		b_xiezi->setBright(true);
		i_xiezi->setVisible(false);
		b_xiezi->getChildByName<Button*>("button_add")->setVisible(true);
	}

	if (m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ARM].num != 0)
	{
		b_wuqi->setBright(false);
		i_wuqi->setVisible(true);
		i_wuqi->loadTexture(getItemIconPath(m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ARM].iid));
		b_wuqi->getChildByName<Button*>("button_add")->setVisible(false);
	}
	else
	{
		b_wuqi->setBright(true);
		i_wuqi->setVisible(false);
		b_wuqi->getChildByName<Button*>("button_add")->setVisible(true);
	}

	if (m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ACCESSORY].num != 0)
	{
		b_shoushi->setBright(false);
		i_shoushi->setVisible(true);
		i_shoushi->loadTexture(getItemIconPath(m_curEquipItem[SUB_TYPE_ROLE_EQUIP_ACCESSORY].iid));
		b_shoushi->getChildByName<Button*>("button_add")->setVisible(false);
	}
	else
	{
		b_shoushi->setBright(true);
		i_shoushi->setVisible(false);
		b_shoushi->getChildByName<Button*>("button_add")->setVisible(true);
	}
}

void UICompanions::flushEquipView()
{
	openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CHANGE_CSB]);
	auto p_addequip = getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CHANGE_CSB]);

	auto l_equip = p_addequip->getChildByName<ListView*>("listview_left");
	auto p_bag = l_equip->getItem(0);
	auto i_title = p_bag->getChildByName<ImageView*>("image_icon");
	auto tx_title = p_bag->getChildByName<Text*>("label_city");
	tx_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BAG"]);
	i_title->setPositionX(tx_title->getPositionX() - i_title->getBoundingBox().size.width - tx_title->getBoundingBox().size.width / 2);
	auto p_no = p_addequip->getChildByName<Widget*>("panel_no");
	p_no->setVisible(false);
	auto b_ok = p_addequip->getChildByName<Button*>("button_ok");
	b_ok->setBright(false);
	b_ok->setTouchEnabled(false);

	auto b_cur_equip = p_addequip->getChildByName<Button*>("current_equip_bg");
	auto cur_equip = b_cur_equip->getChildByName<ImageView*>("image_current_equip");
	auto b_uninstall = p_addequip->getChildByName<Button*>("button_uninstall");
	auto t_curname = p_addequip->getChildByName<Text*>("label_current_itemname");

	std::vector<EquipableItem*> itemsInfo;

	if (m_curEquipItem[m_eType].num != 0)
	{
		cur_equip->setVisible(true);
		cur_equip->ignoreContentAdaptWithSize(false);
		cur_equip->loadTexture(getItemIconPath(m_curEquipItem[m_eType].iid));
		b_uninstall->setBright(true);
		b_uninstall->setTouchEnabled(true);
		t_curname->setString(getItemName(m_curEquipItem[m_eType].iid));
		setBgButtonFormIdAndType(b_cur_equip, m_curEquipItem[m_eType].iid, ITEM_TYPE_ROLE_EQUIP);
		setTextColorFormIdAndType(t_curname, m_curEquipItem[m_eType].iid, ITEM_TYPE_ROLE_EQUIP);
		b_cur_equip->setTouchEnabled(true);
		b_cur_equip->addTouchEventListener(CC_CALLBACK_2(UICompanions::menuCall_func,this));
		b_cur_equip->setTag(m_curEquipItem[m_eType].iid);
	}
	else
	{
		cur_equip->setVisible(false);
		b_uninstall->setBright(false);
		b_uninstall->setTouchEnabled(false);
		t_curname->setString("N/A");
		b_cur_equip->setTouchEnabled(false);
	}

	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(p_addequip, "label_title_sell"));
	auto p_no1 = p_no->getChildByName<Text*>("label_no_item1");
	auto p_no2 = p_no->getChildByName<Text*>("label_no_item2");
	switch (m_eType)
	{
	case SUB_TYPE_ROLE_EQUIP_HAT:
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_HEAD"]);
			p_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_HEAD_NO1"]);
			p_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_HEAD_NO2"]);
			break;
		}
	case SUB_TYPE_ROLE_EQUIP_CLOTHES:
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_CLOTHES"]);
			p_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_CLOTHES_NO1"]);
			p_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_CLOTHES_NO2"]);
			break;
		}
	case SUB_TYPE_ROLE_EQUIP_ARM:
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_WEAPONS"]);
			p_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_WEAPON_NO1"]);
			p_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_WEAPON_NO2"]);
			break;
		}
	case SUB_TYPE_ROLE_EQUIP_ACCESSORY:
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_ACCESS"]);
			p_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_ACCESS_NO1"]);
			p_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_ACCESS_NO2"]);
			break;
		}
	case SUB_TYPE_ROLE_EQUIP_SHOE:
		{
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_SHOE"]);
			p_no1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_SHOE_NO1"]);
			p_no2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLL_EQUIP_SHOE_NO2"]);
			break;
		}
	default:
		break;
	}
	updataSelectEquip();
	if (m_vEquipableItem[m_eType].empty())
	{
		p_no->setVisible(true);
		auto l_list1 = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_addequip, "listview_p"));
		auto l_list2 = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_addequip, "listview_left"));
		auto i_pulldown1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_addequip, "image_pulldown"));
		auto b_pulldown = i_pulldown1->getChildByName("button_pulldown");
		auto i_pulldown2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_addequip, "image_pulldown_0"));
		auto b_pulldown_1 = i_pulldown2->getChildByName("button_pulldown");
		b_pulldown->setPositionX(b_pulldown->getPositionX() - b_pulldown->getContentSize().width / 2);
		b_pulldown_1->setPositionX(b_pulldown_1->getPositionX() - b_pulldown_1->getContentSize().width / 2);
		addListViewBar(l_list1, i_pulldown1);
		addListViewBar(l_list2, i_pulldown2);
		return;
	}
	auto p_item = p_addequip->getChildByName<Widget*>("panel_items");
	for (int i = 0; i < ceil(m_vEquipableItem[m_eType].size() / 6.0); i++)
	{
		auto p_item_clone = p_item->clone();
		for (int j = 0; j < 6; j++)
		{
			auto item = p_item_clone->getChildByName<Widget*>(StringUtils::format("image_items_%d", j + 1));
			auto i_goods = item->getChildByName<ImageView*>("image_goods");
			auto b_goods = item->getChildByName<Button*>("button_equip_items");
			if (i * 6 + j < m_vEquipableItem[m_eType].size())
			{
				i_goods->setVisible(true);
				b_goods->setTouchEnabled(true);
				i_goods->ignoreContentAdaptWithSize(false);
				i_goods->loadTexture(getItemIconPath(m_vEquipableItem[m_eType].at(i * 6 + j).iid));
				b_goods->setTag(i * 6 + j);
				item->setTag(m_vEquipableItem[m_eType].at(i * 6 + j).id);
				auto t_num = i_goods->getChildByName<Text*>("text_item_num");
				setTextSizeAndOutline(t_num, m_vEquipableItem[m_eType].at(i * 6 + j).num);
				setBgButtonFormIdAndType(item, m_vEquipableItem[m_eType].at(i * 6 + j).iid, ITEM_TYPE_ROLE_EQUIP);
			}
			else
			{
				i_goods->setVisible(false);
				b_goods->setTouchEnabled(false);
			}
		}
		l_equip->pushBackCustomItem(p_item_clone);
	}
	auto l_list1 = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_addequip, "listview_p"));
	auto l_list2 = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_addequip, "listview_left"));
	auto i_pulldown1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_addequip, "image_pulldown"));
	auto b_pulldown = i_pulldown1->getChildByName("button_pulldown");
	auto i_pulldown2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_addequip, "image_pulldown_0"));
	auto b_pulldown_1 = i_pulldown2->getChildByName("button_pulldown");
	b_pulldown->setPositionX(b_pulldown->getPositionX() - b_pulldown->getContentSize().width / 2);
	b_pulldown_1->setPositionX(b_pulldown_1->getPositionX() - b_pulldown_1->getContentSize().width / 2);
	addListViewBar(l_list1, i_pulldown1);
	addListViewBar(l_list2, i_pulldown2);
}

void UICompanions::updataSelectEquip()
{
	auto p_addequip = getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_EQUIP_CHANGE_CSB]);
	auto t_name = p_addequip->getChildByName<Text*>("label_select_itemname");
	auto t_desc = p_addequip->getChildByName<Text*>("label_select_itemname_0");
	auto b_sel_equip = p_addequip->getChildByName<Button*>("select_equip_bg");
	auto i_sel_equip = b_sel_equip->getChildByName<ImageView*>("image_current_equip");
	if (m_pTempButton)
	{
		t_desc->setVisible(true);
		t_name->setString(getItemName(m_vEquipableItem[m_eType].at(m_pTempButton->getTag()).iid));
		if (SINGLE_SHOP->getItemData()[m_vEquipableItem[m_eType].at(m_pTempButton->getTag()).iid].type == ITEM_TYPE_SHIP_EQUIP && SINGLE_SHOP->getItemData()[m_vEquipableItem[m_eType].at(m_pTempButton->getTag()).iid].sub_type == SUB_TYPE_SHIP_FIGUREHEAD)
		{
			t_desc->setString(SINGLE_SHOP->getItemData()[m_vEquipableItem[m_eType].at(m_pTempButton->getTag()).iid].shortdesc);
		}
		else
		{
			t_desc->setString(SINGLE_SHOP->getItemData()[m_vEquipableItem[m_eType].at(m_pTempButton->getTag()).iid].desc);
		}
		setTextColorFormIdAndType(t_name, m_vEquipableItem[m_eType].at(m_pTempButton->getTag()).iid, ITEM_TYPE_ROLE_EQUIP);
		i_sel_equip->setVisible(true);
		i_sel_equip->loadTexture(getItemIconPath(m_vEquipableItem[m_eType].at(m_pTempButton->getTag()).iid));
		b_sel_equip->setTouchEnabled(true);
		b_sel_equip->addTouchEventListener(CC_CALLBACK_2(UICompanions::menuCall_func,this));
		b_sel_equip->setTag(m_vEquipableItem[m_eType].at(m_pTempButton->getTag()).iid);
	}
	else
	{
		t_desc->setVisible(false);
		t_name->setString("N/A");
		b_sel_equip->setTouchEnabled(false);
		i_sel_equip->setVisible(false);
	}
}

std::vector<int> UICompanions::getEquipAttribute(const std::string attributeStr)
{
	auto substr = attributeStr;
	std::vector<int> valuevector;

	if (attributeStr.size() == 0 || attributeStr.find_first_not_of(","))
	{
		valuevector.push_back(0);
		valuevector.push_back(0);
		valuevector.push_back(0);
		valuevector.push_back(0);
		valuevector.push_back(0);
		return valuevector;
	}
	while (substr.find(",") != -1)
	{
		int a = substr.find_first_of(",");
		int value = atoi(substr.substr(0, a).c_str());
		substr = substr.substr(a + 1, std::string::npos);
		valuevector.push_back(value);
	}
	int a = atoi(substr.c_str());
	valuevector.push_back(a);
	return valuevector;
}
