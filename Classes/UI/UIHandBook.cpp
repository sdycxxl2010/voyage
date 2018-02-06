#include "UIHandBook.h"

#include "UICommon.h"
#include "UIInform.h"
#include "UIMain.h"
#include "TVSceneLoader.h"
#include "CompanionTaskManager.h"
#include "UIStoryLine.h"
#include "Utils.h"
UIHandBook::UIHandBook()
{
	m_selectedButton = nullptr;
	m_curCompaniesStatues.clear();
	m_curCompanionCanUnlock = false; 
	m_curCompaionStatus = nullptr;
	m_vButtonNormalPath.clear();
	m_vButtonPressedPath.clear();
	
	m_vButtonNormalPath.push_back("cocosstudio/login_ui/companions/btn_left_comp_off.png");
	m_vButtonNormalPath.push_back("cocosstudio/login_ui/companions/btn_left_landscape_off.png");
	m_vButtonNormalPath.push_back("cocosstudio/login_ui/companions/btn_left_relic_off.png");
	m_vButtonPressedPath.push_back("cocosstudio/login_ui/companions/btn_left_comp_on.png");
	m_vButtonPressedPath.push_back("cocosstudio/login_ui/companions/btn_left_landscape_on.png");
	m_vButtonPressedPath.push_back("cocosstudio/login_ui/companions/btn_left_relic_on.png");
	m_bMainButtonTouched = true;
}

UIHandBook::~UIHandBook()
{
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}

void UIHandBook::onEnter()
{
	UIBasicLayer::onEnter();
}
void UIHandBook::onExit()
{
	UIBasicLayer::onExit();
}
bool UIHandBook::init()
{
	if (!UIBasicLayer::init())
	{
		return false;
	}

	ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIHandBook::onServerEvent, this), this);
	ProtocolThread::GetInstance()->getCompanionsStatus(UILoadingIndicator::create(this));	
//	ProtocolThread::GetInstance()->getCompanies(LoadingLayer::create(this));
	return true;
}
UIHandBook *UIHandBook::create()
{
	auto layer = new UIHandBook();
	if (layer && layer->init())
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	layer = nullptr;
	return layer;
}
void UIHandBook::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}
	if (!m_bMainButtonTouched)
	{
		return;
	}  
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	auto btn = dynamic_cast<Widget*>(pSender);
	std::string name = btn->getName();
	if (isButton(button_close))
	{
		if (getViewRoot(COMPANION_RES[DETAILS_VIEWCARD_CSB]) != nullptr)
		{
			closeView(COMPANION_RES[DETAILS_VIEWCARD_CSB]);
		}
		if (getViewRoot(COMPANION_RES[COMPANIONS_INSUFFICIENT]) != nullptr)
		{
			closeView(COMPANION_RES[COMPANIONS_INSUFFICIENT]);
		}
		else
		{
			this->removeFromParentAndCleanup(true);
		}
		return;
	}

	if (isButton(image_comp_bg))
	{
		int tag = btn->getTag();
		flushCompanionInfo(tag);
		changeSelectButtonState(btn);
		return;
	}

	if (isButton(button_unlock))
	{
		log("button_unlock ---------------////////////////////////////");
		if (m_curCompanionCanUnlock)
		{
			ProtocolThread::GetInstance()->unlockCompany(m_curCompaionStatus->define->protoid,0,UILoadingIndicator::create(this));
		}
		else if (getViewRoot(COMPANION_RES[COMPANIONS_INSUFFICIENT]) != nullptr)
		{

			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushVConfirmView("TIP_HANDBOOK_USE_V_TICKET_TITLE", "TIP_HANDBOOK_USE_V_TICKET_CONTENT",getUnclockVTicket());
		}
		else
		{
			if (m_curCompaionStatus != nullptr)
			{
				openConfirmPayView();
			}		
		}
		return;
	}
	if (isButton(image_companions))
	{
		int tag = btn->getTag();
		openCompanionCard(tag,btn->getWorldPosition());
		return;
	}
	if (isButton(panel_v_ticketdetails))
    {
		closeView();
		return;
    }
	if (isButton(button_equip_bg))
	{
		auto t_lv = btn->getChildByName<Text*>("text_item_skill_lv");
		UICommon::getInstance()->openCommonView(this);
		int index = btn->getTag();
		SKILL_DEFINE skill_define;
		if (t_lv)
		{
			skill_define = m_vNormalSkillDefine.at(index);
		}
		else
		{
			skill_define = m_vSpecialSkillDefine.at(index);
		}
		UICommon::getInstance()->flushSkillView(skill_define,false);
		return;
	}

	if (isButton(image_item))
	{
		int tag = btn->getTag();
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushItemsDetail(nullptr, tag,true);
		return;
	}
	if (isButton(button_cancel))
	{
		closeView(COMPANION_RES[COMPANIONS_INSUFFICIENT]);
		return;
	}
	if (isButton(button_v_yes))
	{
		log("button_v_yes ---------------////////////////////////////");
		if (SINGLE_HERO->m_iGold >= getUnclockVTicket())
		{
			ProtocolThread::GetInstance()->unlockCompany(m_curCompaionStatus->define->protoid, 1, UILoadingIndicator::create(this));
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_BANK_INSURANCE_V_NOT_ENOUGH");
		}
		return;
	}
	if (isButton(button_comp))
	{
		m_bMainButtonTouched = false;
		setMainButtonStage(btn);
		ProtocolThread::GetInstance()->getCompanionsStatus(UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_landscape))
	{
		m_bMainButtonTouched = false;
		setMainButtonStage(btn);
		return;
	}
	if (isButton(button_relic))
	{
		m_bMainButtonTouched = false;
		setMainButtonStage(btn);
		return;
	}
	if (isButton(image_speed))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(btn);
		return;
	}
	if (isButton(image_atk))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(btn);
		return;
	}
	if (isButton(image_def))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(btn);
		return;
	}
}
void UIHandBook::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{

	case PROTO_TYPE_GetCompanionsStatusResult:
	{
										   auto result = (GetCompanionsStatusResult*)(message);
										   if (result->failed == 0)
										   {
											   m_statusResult = result;
											   initf(0);
										   }
										   else
										   {
										   }
										   break;
	}
	case PROTO_TYPE_UnlockCompanyResult:
	{
										   auto result = (UnlockCompanyResult*)(message);
										   if (result->failed == 0)
										   {
											   int protoid = result->companyprotoid;
											   auto scene = _director->getRunningScene();
											   auto mainLayer = dynamic_cast<UIMain*>(scene->getChildByTag(100 + MAIN_TAG));
											   mainLayer->flushCionAndGold(result->coins, result->golds);
											   closeView(COMPANION_RES[COMPANIONS_INSUFFICIENT]);
											   auto novice = UIStoryLine::GetInstance();
											   if (novice->getParent() == nullptr)
											   {
												   scene->addChild(novice, 10001);
											   }
											   novice->onPartnerDialogMessage(1, 0);
											   ProtocolThread::GetInstance()->getCompanionsStatus(UILoadingIndicator::create(this));
											   Utils::consumeVTicket("7", 1, result->totalcost);
										   }
										   else
										   {
										   }
										   break;
	}
	default:
		break;
	}
}
void UIHandBook::initf(float dt)
{
	openView(COMPANION_RES[COMPANIONS_HANDBOOK_CSB]);
	auto view = getViewRoot(COMPANION_RES[COMPANIONS_HANDBOOK_CSB]);
	int n_compaionSize = m_statusResult->n_companions;
	auto list_comp = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_comp"));
	auto list_summary = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_summary"));
	auto btn_unlock = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_unlock"));
	auto i_title = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_titile_bg"));
	auto i_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown"));
	auto p_compIcon = dynamic_cast<Layout*>(Helper::seekWidgetByName(view, "panel_1"));
	auto btn_comp = Helper::seekWidgetByName(view, "button_comp");
	auto btn_landscape = Helper::seekWidgetByName(view, "button_landscape");
	auto btn_relic = Helper::seekWidgetByName(view, "button_relic");;
	list_summary->setVisible(false);
	btn_unlock->setVisible(false);
	i_title->setVisible(false);
	i_pulldown->setVisible(false);
	list_comp->removeAllItems();
	m_curCompaniesStatues.clear();
	m_selectedButton = nullptr;
	setMainButtonStage(btn_comp);
	int n_pnum = ceil(float(n_compaionSize)/ 3);
	int b = 0;
	for (int a = 0; a < n_pnum; a++)
	{
		auto p_comp = p_compIcon->clone();

		for (int c = 0; c < 3; c++)
		{
			auto i_icon = p_comp->getChildByName<ImageView*>(StringUtils::format("image_comp_bg_%d", c + 1));
			auto i_comp = i_icon->getChildByName<ImageView*>("image_comp");
			auto i_ing = i_icon->getChildByName<ImageView*>("image_ing");
			auto i_on = i_icon->getChildByName("image_on");
			i_on->setVisible(false);
			i_icon->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func,this));
			if (b == n_compaionSize -1)
			{
				auto info = m_statusResult->companions[b];
				i_icon->setTag(b);
				changeHeadImageStatus(i_icon, info);
				break;
			}
			else
			{		
				auto info = m_statusResult->companions[b];
				i_icon->setTag(b);
				changeHeadImageStatus(i_icon, info);
				
			}
			b++;		
		}
		list_comp->pushBackCustomItem(p_comp);
	}
}

void UIHandBook::flushCompanionInfo(int tag)
{
	auto view = getViewRoot(COMPANION_RES[COMPANIONS_HANDBOOK_CSB]);
	auto i_speed = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_speed"));
	i_speed->setTouchEnabled(true);
	i_speed->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func, this));
	i_speed->setTag(IMAGE_ICON_HEALTHPOWER + IMAGE_INDEX2);
	auto i_atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_atk"));
	i_atk->setTouchEnabled(true);
	i_atk->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func, this));
	i_atk->setTag(IMAGE_ICON_ATTACKPOWER + IMAGE_INDEX2);
	auto i_def = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_def"));
	i_def->setTouchEnabled(true);
	i_def->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func, this));
	i_def->setTag(IMAGE_ICON_DEFENSEPOWER + IMAGE_INDEX2);

	auto list_summary = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_summary"));
	auto btn_unlock = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_unlock"));
	auto i_title = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_titile_bg"));
	auto i_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown"));
	auto t_desc = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_content"));
	auto t_compname = dynamic_cast<Text*>(Helper::seekWidgetByName(i_title, "label_item_name"));
	auto t_hp = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_speed_num"));
	auto t_attack = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_atk_num"));
	auto t_defence = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_defnum"));
	auto i_companions = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_companions"));
	auto i_companionsno = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_companions_no"));
	auto p_storycluse = view->getChildByName<Widget*>("panel_storycluse");
	auto p_quset = view->getChildByName<Widget*>("panel_quest");
	auto p_content = Helper::seekWidgetByName(view, "panel_content");
	
	auto t_storyClues_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_storycluse, "text_title_storycluse"));
	t_storyClues_0->ignoreContentAdaptWithSize(true);
	
	t_desc->setAnchorPoint(Vec2(0,1));
 	auto p_summary = list_summary->getItem(0);
	auto p_text = list_summary->getItem(1);
 	i_companions->setVisible(false);
	i_companions->setTag(tag);
	i_companionsno->setVisible(true);
	list_summary->setVisible(true);
	if (list_summary->getChildrenCount() > 2)
	{
		list_summary->removeItem(2);
	}
	i_title->setVisible(true);
	i_pulldown->setVisible(true);
	//本地数据
	auto companionData = SINGLE_SHOP->getCompanionInfo();
	//服务器获取的数据
	auto companionInfo = m_statusResult->companions[tag];
	auto comInfo = companionData[companionInfo->define->protoid];
	m_curCompaionStatus = companionInfo;
	m_vNormalSkillDefine.clear();
	m_vSpecialSkillDefine.clear();
	//判断当前小伙伴是否未拥有碎片

    if (m_curCompaniesStatues.at(tag) != COMPANIE_NONE_ITEM)
    {	
		float hight = getLabelHight(comInfo.desc, t_desc->getContentSize().width, t_desc->getFontName(), t_desc->getFontSize());
		t_attack->setString(StringUtils::format("%d", comInfo.ap));
		t_defence->setString(StringUtils::format("%d", comInfo.dp));
		t_hp->setString(StringUtils::format("%d", comInfo.hp));
		t_desc->setString(comInfo.desc);
		t_desc->setContentSize(Size(630, getLabelHight(comInfo.desc,630,t_desc->getFontName(),t_desc->getFontSize())));
		t_compname->setString(comInfo.name);
		i_companions->loadTexture(getCompanionCardPath(comInfo.id, false));
		i_companions->setVisible(true);
		i_companions->setTouchEnabled(true);
		i_companionsno->setVisible(false);
		i_companions->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func,this));
		btn_unlock->setVisible(true);
	
		int n_normalSkillNum = companionInfo->define->n_normalskill;
		int n_specialSkillNum = companionInfo->define->n_specialskill;

		//普通技能
		for (size_t i = 0; i < 3; i++)
		{
			auto b_skill = p_content->getChildByName<Button*>(StringUtils::format("button_equip_bg_%d", i + 1));
			if (i < n_normalSkillNum)
			{
				b_skill->setVisible(true);
				b_skill->setTag(i);
				b_skill->setTouchEnabled(true);
				auto t_lv = b_skill->getChildByName<Text*>("text_item_skill_lv");
				auto i_lock = b_skill->getChildByName("image_lock");
				t_lv->setVisible(false);
				b_skill->loadTextureNormal(getSkillIconPath(companionInfo->define->normalskill[i]->id, SKILL_TYPE_COMPANION_NORMAL));
				setTextSizeAndOutline(t_lv, companionInfo->define->normalskill[i]->level);
				SKILL_DEFINE skillDefine;
				skillDefine.id = companionInfo->define->normalskill[i]->id;
				skillDefine.lv = companionInfo->define->normalskill[i]->level;
				skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
				skillDefine.icon_id = companionInfo->define->protoid;
				m_vNormalSkillDefine.push_back(skillDefine);
			}
			else
			{
				b_skill->setVisible(false);
			}
		}

		//特殊技能
		for (size_t i = 0; i < 3; i++)
		{
			auto b_skill = p_content->getChildByName<Button*>(StringUtils::format("button_equip_bg_%d", i + 4));
			if (i < n_specialSkillNum)
			{
				b_skill->setVisible(true);
				b_skill->setTouchEnabled(true);
				b_skill->setTag(i);
				auto i_lock = b_skill->getChildByName("image_lock");
				i_lock->setVisible(true);
				b_skill->loadTextureNormal(getSkillIconPath(companionInfo->define->specialskill[i]->id, SKILL_TYPE_PARTNER_SPECIAL));
				SKILL_DEFINE skillDefine;
				skillDefine.id = companionInfo->define->specialskill[i]->id;
				skillDefine.lv = companionInfo->define->specialskill[i]->level;
				i_lock->setVisible(false);
				skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
				skillDefine.icon_id = companionInfo->define->protoid;
				m_vSpecialSkillDefine.push_back(skillDefine);
			}
			else
			{
				b_skill->setVisible(false);
			}
		}
		auto statue = m_curCompaniesStatues.at(tag);
		//当前伙伴的解锁进度
		auto p_storycluse_clone = p_storycluse->clone();
		auto t_storyClues = dynamic_cast<Text*>(Helper::seekWidgetByName(p_storycluse_clone, "text_title_storycluse"));
		auto p_storycluse_0 = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_storycluse_clone, "panel_storycluse_0"));
		auto p_quset_clone = p_quset->clone();
		auto t_titlequest = dynamic_cast<Text*>(Helper::seekWidgetByName(p_quset_clone, "text_title_quest"));
		auto p_quset_0 = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_quset_clone, "panel_quset_0"));
		
		switch (statue)
		{
		case UIHandBook::COMPANIE_NONE_ITEM:
		case UIHandBook::COMPANIE_HAVE_ITEM:
		case UIHandBook::COMPANIE_ITEM_ENOUGH:
			list_summary->pushBackCustomItem(p_storycluse_clone);
			btn_unlock->setVisible(true);
			showStorycluse(p_storycluse_0, companionInfo);
			break;
		case UIHandBook::COMPANIE_TASK:
			list_summary->pushBackCustomItem(p_quset_clone);
			btn_unlock->setVisible(false);
			showTaskStatue(p_quset_0, companionInfo);
			break;
		case UIHandBook::COMPANIE_UNLOCKED:
			btn_unlock->setVisible(false);
			break;
		default:
			break;
		}
	}
	else
	{
		//普通技能
		for (size_t i = 0; i < 3; i++)
		{
			auto b_skill = p_content->getChildByName<Button*>(StringUtils::format("button_equip_bg_%d", i + 1));
			b_skill->setVisible(true);
			auto t_lv = b_skill->getChildByName<Text*>("text_item_skill_lv");
			t_lv->setVisible(false);
			b_skill->loadTextureNormal("cocosstudio/login_ui/companions/skill_no.png");	
			b_skill->setTouchEnabled(false);
		}
		//特殊技能
		for (size_t i = 0; i < 3; i++)
		{
			auto b_skill = p_content->getChildByName<Button*>(StringUtils::format("button_equip_bg_%d", i + 4));
			b_skill->setVisible(true);
			auto i_lock = b_skill->getChildByName("image_lock");
			i_lock->setVisible(false);
			b_skill->loadTextureNormal("cocosstudio/login_ui/companions/skill_no.png");	
			b_skill->setTouchEnabled(false);
		}
		t_attack->setString("???");
		t_defence->setString("???");
		t_hp->setString("???");
		t_desc->setString("");
		t_desc->setContentSize(Size(0,0));
		t_compname->setString("???");
		btn_unlock->setVisible(true);
	}
	
	p_text->setContentSize(t_desc->getBoundingBox().size);
	t_desc->setPosition(Vec2(0, p_text->getContentSize().height));
	
	addListViewBar(list_summary, i_pulldown);
	auto b_pulldown = i_pulldown->getChildByName("button_pulldown");
	b_pulldown->setAnchorPoint(Vec2(0.5, 0));
	list_summary->scrollToTop(0.5,true);
}

void UIHandBook::openCompanionCard(int tag, Vec2 pos)
{
	openView(COMPANION_RES[DETAILS_VIEWCARD_CSB]);
	auto view = getViewRoot(COMPANION_RES[DETAILS_VIEWCARD_CSB]);
	view->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func, this));
	auto i_companion = view->getChildByName<ImageView*>("image_companions");
	auto info = m_statusResult->companions[tag]->define;
	i_companion->loadTexture(getCompanionCardPath(info->protoid,false));
	auto endPos = i_companion->getPosition();
	i_companion->setPosition(pos);
	i_companion->setScale(0.5f);
	i_companion->runAction(Spawn::createWithTwoActions(ScaleTo::create(0.1f, 1.1), MoveTo::create(0.2f, endPos)));
}

void UIHandBook::changeSelectButtonState(Node * p_sender)
{
	if (m_selectedButton == nullptr)
	{
		m_selectedButton = p_sender;
		auto i_on = p_sender->getChildByName("image_on");
		i_on->setVisible(true);
	}
	else
	{
		auto i_on = m_selectedButton->getChildByName("image_on");
		i_on->setVisible(false);
		auto i_on_1 = p_sender->getChildByName("image_on");
		i_on_1->setVisible(true);
		m_selectedButton = p_sender;
	}

}
//切换小伙伴头像状态
void UIHandBook::changeHeadImageStatus(ImageView * imageHead, CompanionsStatus* info)
{
	COMPANIE_STATUES statues;
	std::string s_lock1 = "cocosstudio/login_ui/companions/lock_1.png";
	std::string s_lock2 = "cocosstudio/login_ui/companions/lock_2.png";
	std::string s_task = "cocosstudio/login_ui/companions/quest.png";
	auto companionsData = SINGLE_SHOP->getCompanionInfo();
	auto companionInfo = companionsData[info->define->protoid];
	int itemnum_1 = info->req_item1_num;
	int itemnum_2 = info->req_item2_num;
	int itemnum_3 = info->req_item3_num;
	int needItemnum_1 = companionInfo.item1_amount;
	int needItemnum_2 = companionInfo.item2_amount;
	int needItemnum_3 = companionInfo.item3_amount;
	//需要收集碎片
	if (info->status == 0)
	{
		if (itemnum_1 == 0 && itemnum_2 == 0 && itemnum_3 == 0)
		{
			statues = COMPANIE_NONE_ITEM;
		}
		else if (itemnum_1 != 0 || itemnum_2 != 0 || itemnum_3 != 0)
		{
			if (itemnum_1 >= needItemnum_1 && itemnum_2 >= needItemnum_2 && itemnum_3 >= needItemnum_3)
			{
				statues = COMPANIE_ITEM_ENOUGH;
			}
			else
			{
				statues = COMPANIE_HAVE_ITEM;
			}	
		}
	}
	//当前小伙伴处于任务状态
	else if (info->status == 2)
	{
		statues = COMPANIE_TASK;
	}
	//已解锁当前小伙伴
	else if (info->status == 1)
	{
		statues = COMPANIE_UNLOCKED;
	}
	imageHead->setVisible(true);
	auto i_comp = imageHead->getChildByName<ImageView*>("image_comp");
	auto i_ing = imageHead->getChildByName<ImageView*>("image_ing");
	auto i_on = imageHead->getChildByName("image_on");
	i_on->setVisible(false);
	i_ing->setVisible(false);
	i_comp->setVisible(true);
	i_comp->loadTexture(getCompanionHeadPath(info->define->protoid));
	m_curCompaniesStatues.push_back(statues);
	switch (statues)
	{
	case UIHandBook::COMPANIE_NONE_ITEM:
		i_comp->setVisible(false);
		break;
	case UIHandBook::COMPANIE_HAVE_ITEM:
		i_ing->setVisible(true);
		i_ing->loadTexture(s_lock1);
		break;
	case UIHandBook::COMPANIE_ITEM_ENOUGH:
		i_ing->setVisible(true);
		i_ing->loadTexture(s_lock2);
		break;
	case UIHandBook::COMPANIE_TASK:
		i_ing->setVisible(true);
		i_ing->loadTexture(s_task);
		break;
	case UIHandBook::COMPANIE_UNLOCKED:
		i_ing->setVisible(false);
		break;
	default:
		break;
	}
	if (imageHead->getTag() == 0)
	{
		i_on->setVisible(true);
		flushCompanionInfo(0);
		changeSelectButtonState(imageHead);
	}
}
void UIHandBook::showStorycluse(Widget * p_sender, CompanionsStatus* info)
{

	ITEM_RES item;
	auto companionsData = SINGLE_SHOP->getCompanionInfo();
	auto companionInfo = companionsData[info->define->protoid];
	bool b_canUnlock;
	for (int i = 0; i < 3; i++)
	{
		auto p_item = p_sender->getChildByName(StringUtils::format("panel_item_%d", i + 1));
		auto i_item = p_item->getChildByName<ImageView*>("image_item");
		auto t_item = p_item->getChildByName<Text*>("label_items_name");
		auto t_item1num = p_item->getChildByName<Text*>("label_num");		
		if (i == 0)
		{
			item = SINGLE_SHOP->getItemData()[companionInfo.item1];
			i_item->loadTexture(getItemIconPath(companionInfo.item1));
			t_item->setString(item.name);
			i_item->setTag(companionInfo.item1);
			i_item->setTouchEnabled(true);
			i_item->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func,this));
			int curItemnum = info->req_item1_num;
			int needNum = companionInfo.item1_amount;

			if (curItemnum >= needNum)
			{
				t_item1num->setTextColor(Color4B(46,125,50,255));
				b_canUnlock = true;
			}
			else
			{
				t_item1num->setTextColor(Color4B(185,0,5,255));	
				b_canUnlock = false;
			}
			t_item1num->setString(StringUtils::format("%d / %d", curItemnum, needNum));
			if (needNum == 0)
			{
				p_item->setVisible(false);
			}
		}
		if (i == 1)
		{
			item = SINGLE_SHOP->getItemData()[companionInfo.item2];
			i_item->loadTexture(getItemIconPath(companionInfo.item2));
			t_item->setString(item.name);
			i_item->setTag(companionInfo.item2);
			i_item->setTouchEnabled(true);
			i_item->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func,this));
			int curItemnum = info->req_item2_num;
			int needNum = companionInfo.item2_amount;
			if (curItemnum >= needNum)
			{
				t_item1num->setTextColor(Color4B(46, 125, 50, 255));
				if (b_canUnlock == true)
				{
					b_canUnlock = true;
				}
			}
			else
			{
				t_item1num->setTextColor(Color4B(185,0,5,255));
				b_canUnlock = false;
			}
			t_item1num->setString(StringUtils::format("%d / %d", curItemnum, needNum));
			if (needNum == 0)
			{
				p_item->setVisible(false);
			}
		}
		if (i == 2)
		{
			item = SINGLE_SHOP->getItemData()[companionInfo.item3];
			i_item->loadTexture(getItemIconPath(companionInfo.item3));
			t_item->setString(item.name);
			i_item->setTag(companionInfo.item3);
			i_item->setTouchEnabled(true);
			i_item->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func,this));
			int curItemnum = info->req_item3_num;
			int needNum = companionInfo.item3_amount;
			if (curItemnum >= needNum)
			{
				t_item1num->setTextColor(Color4B(46, 125, 50, 255));
				if (b_canUnlock == true)
				{
					b_canUnlock = true;
				}
			}
			else
			{
				t_item1num->setTextColor(Color4B(185,0,5,255));
				b_canUnlock = false;
			}
			t_item1num->setString(StringUtils::format("%d / %d", curItemnum, needNum));
			if (needNum == 0)
			{
				p_item->setVisible(false);
			}
		}
	}
	m_curCompanionCanUnlock = b_canUnlock;
}

void UIHandBook::showTaskStatue(Widget * p_sender, CompanionsStatus* info)
{
	auto t_title = p_sender->getChildByName<Text*>("text_prompt");
	auto t_process = p_sender->getChildByName<Text*>("text_schedule");
	auto tasktitle = SINGLE_SHOP->getPartnerDialogJudgeInfo()[info->currenttaskstep+1].title;
	auto process = SINGLE_SHOP->getTipsInfo()["TIP_HANDBOOK_COMPANION_TASK_PROCESS"];
	t_process->setString(StringUtils::format("%s (%s%d / %d)",tasktitle.c_str(),process.c_str(),info->currenttaskstep, info->totaltasksteps));	
}

void UIHandBook::openConfirmPayView()
{
	openView(COMPANION_RES[COMPANIONS_INSUFFICIENT]);
	auto view = getViewRoot(COMPANION_RES[COMPANIONS_INSUFFICIENT]);
	ITEM_RES item;
	auto companionsData = SINGLE_SHOP->getCompanionInfo();
	auto companionInfo = companionsData[m_curCompaionStatus->define->protoid];
	auto p_item = view->getChildByName<Widget*>("panel_item_1");
	for (int i = 0; i < 3; i++)
	{
		auto p_item = view->getChildByName(StringUtils::format("panel_item_%d", i + 1));
		auto i_item = p_item->getChildByName<ImageView*>("image_item");
		auto t_item = p_item->getChildByName<Text*>("label_items_name");
		auto t_item1num = p_item->getChildByName<Text*>("label_num");
		if (i == 0)
		{
			item = SINGLE_SHOP->getItemData()[companionInfo.item1];
			i_item->loadTexture(getItemIconPath(companionInfo.item1));
			t_item->setString(item.name);
			i_item->setTag(companionInfo.item1);
			i_item->setTouchEnabled(true);
			i_item->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func,this));
			int curItemnum = m_curCompaionStatus->req_item1_num;
			int needNum = companionInfo.item1_amount;

			if (curItemnum >= needNum)
			{
				t_item1num->setTextColor(Color4B(46, 125, 50, 255));
			}
			else
			{
				t_item1num->setTextColor(Color4B(185, 0, 5, 255));
			}
			t_item1num->setString(StringUtils::format("%d / %d", curItemnum, needNum));
			if (needNum == 0)
			{
				p_item->setVisible(false);
			}
		}
		if (i == 1)
		{
			item = SINGLE_SHOP->getItemData()[companionInfo.item2];
			i_item->loadTexture(getItemIconPath(companionInfo.item2));
			t_item->setString(item.name);
			i_item->setTag(companionInfo.item2);
			i_item->setTouchEnabled(true);
			i_item->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func,this));
			int curItemnum = m_curCompaionStatus->req_item2_num;
			int needNum = companionInfo.item2_amount;

			if (curItemnum >= needNum)
			{
				t_item1num->setTextColor(Color4B(46, 125, 50, 255));
			}
			else
			{
				t_item1num->setTextColor(Color4B(185, 0, 5, 255));
			}
			t_item1num->setString(StringUtils::format("%d / %d", curItemnum, needNum));
			if (needNum == 0)
			{
				p_item->setVisible(false);
			}
		}
		if (i == 2)
		{
			item = SINGLE_SHOP->getItemData()[companionInfo.item3];
			i_item->loadTexture(getItemIconPath(companionInfo.item3));
			t_item->setString(item.name);
			i_item->setTag(companionInfo.item3);
			i_item->setTouchEnabled(true);
			i_item->addTouchEventListener(CC_CALLBACK_2(UIHandBook::menuCall_func,this));
			int curItemnum = m_curCompaionStatus->req_item3_num;
			int needNum = companionInfo.item3_amount;

			if (curItemnum >= needNum)
			{
				t_item1num->setTextColor(Color4B(46, 125, 50, 255));
			}
			else
			{
				t_item1num->setTextColor(Color4B(185, 0, 5, 255));
			}
			t_item1num->setString(StringUtils::format("%d / %d", curItemnum, needNum));
			if (needNum == 0)
			{
				p_item->setVisible(false);
			}
		}
	}
}
void UIHandBook::setMainButtonStage(Widget * button)
{
	auto view = getViewRoot(COMPANION_RES[COMPANIONS_HANDBOOK_CSB]);
	//小伙伴界面
	auto btn_comp = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_comp"));
	auto btn_landscape = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_landscape"));
	auto btn_relic = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_relic"));
	auto p_contentno = Helper::seekWidgetByName(view, "panel_centent_no");
	auto t_nofound_1 = p_contentno->getChildByName<Text*>("label_no_found_1_1");
	auto t_nofound_2 = p_contentno->getChildByName<Text*>("label_no_found_2_1");
	auto name = button->getName();
	bool showContent = false;
	std::string currentview;
	auto tipInfo = SINGLE_SHOP->getTipsInfo();
	if (name.find("button_comp") == 0)
	{
		btn_comp->loadTextureNormal(m_vButtonPressedPath.at(BUTTON_COMPANION));
		btn_landscape->loadTextureNormal(m_vButtonNormalPath.at(BUTTON_LANDSPACE));
		btn_relic->loadTextureNormal(m_vButtonNormalPath.at(BUTTON_RELIC));
		showContent = false;
	}
	if (name.find("button_landscape") == 0)
	{
		btn_comp->loadTextureNormal(m_vButtonNormalPath.at(BUTTON_COMPANION));
		btn_landscape->loadTextureNormal(m_vButtonPressedPath.at(BUTTON_LANDSPACE));
		btn_relic->loadTextureNormal(m_vButtonNormalPath.at(BUTTON_RELIC));
		t_nofound_1->setString(tipInfo["TIP_HANDBOOK_LANDSPACE"]);
		t_nofound_2->setString(tipInfo["TIP_HANDBOOK_TIP_CONTENT"]);
		showContent = true;
	}
	if (name.find("button_relic") == 0)
	{
		btn_comp->loadTextureNormal(m_vButtonNormalPath.at(BUTTON_COMPANION));
		btn_landscape->loadTextureNormal(m_vButtonNormalPath.at(BUTTON_LANDSPACE));
		btn_relic->loadTextureNormal(m_vButtonPressedPath.at(BUTTON_RELIC));
		t_nofound_1->setString(tipInfo["TIP_HANDBOOK_RELIC"]);
		t_nofound_2->setString(tipInfo["TIP_HANDBOOK_TIP_CONTENT"]);
		showContent = true;
	}
	//暂时用来显示功能未开启
	auto p_left = view->getChildByName("panel_left_2");
	auto i_title = view->getChildByName("image_titile_bg");
	auto l_summary = view->getChildByName("listview_summary");
	auto b_unlock = view->getChildByName("button_unlock");
	auto i_imagepull = view->getChildByName("image_pulldown");
	auto bg = Helper::seekWidgetByName(view, "image_left_bg");
	if (showContent)
	{
		p_left->setVisible(false);
		i_title->setVisible(false);
		l_summary->setVisible(false);
		b_unlock->setVisible(false);
		i_imagepull->setVisible(false);
		bg->setVisible(false);
		p_contentno->setVisible(true);
	}
	else
	{
		p_left->setVisible(true);
		i_title->setVisible(true);
		l_summary->setVisible(true);
		b_unlock->setVisible(true);
		i_imagepull->setVisible(true);
		p_contentno->setVisible(false);
		bg->setVisible(true);
	}
	m_bMainButtonTouched = true;
}
int UIHandBook::getUnclockVTicket()
{
	auto companion_info = SINGLE_SHOP->getCompanionInfo()[m_curCompaionStatus->define->protoid];
	int price = 0;
	if (companion_info.item1_amount > m_curCompaionStatus->req_item1_num)
	{
		price += (companion_info.item1_amount - m_curCompaionStatus->req_item1_num) * (m_curCompaionStatus->req_item1_price / 100);
	}
	if (companion_info.item2_amount > m_curCompaionStatus->req_item2_num)
	{
		price += (companion_info.item2_amount - m_curCompaionStatus->req_item2_num) * (m_curCompaionStatus->req_item2_price / 100);
	}
	if (companion_info.item3_amount > m_curCompaionStatus->req_item3_num)
	{
		price += (companion_info.item3_amount - m_curCompaionStatus->req_item3_num) * (m_curCompaionStatus->req_item3_price / 100);
	}
	return price;
}
