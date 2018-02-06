#include "UICenterCharactorSkill.h"
#include "UICommon.h"
#include "UIInform.h"

#include "ShopData.h"
#include "UIStore.h"
#include "Utils.h"

UICenterCharactorSkill::UICenterCharactorSkill()
	:m_pParent(nullptr),
	m_pResult(nullptr),
	m_pTempButton(nullptr),
	m_pSkillTreeMainButton(nullptr),
	m_pSkillTreeMinorButton(nullptr),
	m_nSoltIndex(-1),
	m_nSkillPoints(-1)
{
	m_pSelPosResult = nullptr;
	m_pHeroSkillResult = nullptr;
	m_eUIType = UI_CENTER;
}

UICenterCharactorSkill::~UICenterCharactorSkill()
{
	if (m_pResult)
	{
		get_skills_details_result__free_unpacked(m_pResult,0);
	}

	if (m_pSelPosResult)
	{
		get_selected_positive_skills_result__free_unpacked(m_pSelPosResult, 0);
	}

	if (m_pHeroSkillResult)
	{
		get_hero_positive_skill_list_result__free_unpacked(m_pHeroSkillResult, 0);
	}
}

UICenterCharactorSkill* UICenterCharactorSkill::createSkill(UIBasicLayer *parent)
{
	UICenterCharactorSkill* cs = new UICenterCharactorSkill;
	if (cs && cs->init())
	{
		cs->m_pParent = parent;
		cs->autorelease();
		return cs;
	}
	CC_SAFE_DELETE(cs);
	return nullptr;
}

void UICenterCharactorSkill::openCenterSkill()
{
	ProtocolThread::GetInstance()->getSelectedPositiveSkills(UILoadingIndicator::create(m_pParent));
}

void UICenterCharactorSkill::pushResultInMap()
{
	auto& skillTrees = SINGLE_SHOP->getSkillTrees();
	std::map <int, SKILL_TREE>::iterator m1_Iter;
	for ( m1_Iter = skillTrees.begin( ); m1_Iter != skillTrees.end( ); m1_Iter++ ){
		auto& item = m1_Iter->second;
		item.cur_skill_level = 0;
	}
//	for (int i = 1; i < skillTrees.size() + 1; i++)  //技能个数
//	{
//		skillTrees[i].cur_skill_level = 0;
//	}

	for (int i = 0; i < m_pResult->n_skills;i++)
	{
		int level = m_pResult->skills[i]->level;
		int nSkill_id = m_pResult->skills[i]->id;
		SKILL_TREE &skillInfo = skillTrees[nSkill_id];
		skillInfo.cur_skill_level = level;
	}
	openSkillTreeView();
}

void UICenterCharactorSkill::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* target = dynamic_cast<Widget*>(pSender);
		std::string name = target->getName();
		buttonEvent(target,name);
	}
}

void UICenterCharactorSkill::buttonEvent(Widget *pSender,std::string name)
{
	//确认重置技能点
	if (isButton(button_v_yes))
	{
		
		if (SINGLE_HERO->m_iGold >= 100 || m_pResult->freeresettimes > 0)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			ProtocolThread::GetInstance()->resetSkillPoints(UILoadingIndicator::create(m_pParent));
		}
		else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			m_nSoltIndex = GOLD_NOT_FAIL;
			UIInform::getInstance()->openInformView(m_pParent);
			UIInform::getInstance()->openGoldNotEnoughYesOrNo(100);
		}	
		return;
	}
	//重置技能点按钮
	if (isButton(button_reset))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (SINGLE_HERO->m_iLevel - 1 == m_nSkillPoints)
		{
			UIInform::getInstance()->openInformView(m_pParent);
			UIInform::getInstance()->openConfirmYes("TIP_CENTER_SKILL_NOT_RESET");
		}
		else
		{
			UICommon::getInstance()->openCommonView(m_pParent);
			if(m_pResult->freeresettimes > 0){
				UICommon::getInstance()->flushVConfirmView("TIP_CENTER_RESET_SKILL_TITLE", "TIP_CENTER_RESET_SKILL_FREE_CONTENT", 0);
			}else{
				UICommon::getInstance()->flushVConfirmView("TIP_CENTER_RESET_SKILL_TITLE", "TIP_CENTER_RESET_SKILL_CONTENT", 100);
			}
		}
		return;
	}
	//主动技能孔
	if (isButton(button_combar_skil))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pTempButton = nullptr;
		m_nSoltIndex = pSender->getTag() - START_INDEX;
		ProtocolThread::GetInstance()->getHeroPositiveSkillList(UILoadingIndicator::create(m_pParent));
		return;
	}
	//选择主动技能
	if (isButton(button_equip_items))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_pTempButton)
		{
			m_pTempButton->setBright(true);
		}
		m_pTempButton = pSender;
		m_pTempButton->setBright(false);
		auto panel_skill = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_SLOTCHANGE_CSB]);
		panel_skill->getChildByName<Button*>("button_ok")->setBright(true);
		panel_skill->getChildByName<Button*>("button_ok")->setTouchEnabled(true);
		updataSelectSkill(pSender->getTag() - START_INDEX);
		return;
	}
	//卸载主动技能
	if (isButton(button_uninstall))  
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UIInform::getInstance()->openInformView(m_pParent);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_CENTER_SKILL_REMOVE","TIP_CENTER_SKILL_REMOVE");
		return;
	}
	//关闭提示界面
	if (isButton(button_cancel) || isButton(button_close))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pParent->closeView();
		return;
	}
	//选择主动技能确定
	if (isButton(button_ok))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->selectHeroPositiveSkill(m_nSoltIndex,m_pTempButton->getTag() - START_INDEX,UILoadingIndicator::create(m_pParent));
		return;
	}
	//主动技能问号
	if (isButton(button_skill_info)) 
	{	
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(m_pParent);
		UICommon::getInstance()->flushInfoBtnView("TIP_CENTER_SKILL_INFO_TITLE","TIP_CENTER_SKILL_INFO_CONTENT");
		return;
	}
	//技能树
	if (isButton(button_skilltree)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->getSkillsDetails(UILoadingIndicator::create(m_pParent));
		return;
	}
	//返回主动界面
	if (isButton(button_skill_back)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pParent->closeView();
		ProtocolThread::GetInstance()->getSelectedPositiveSkills(UILoadingIndicator::create(m_pParent));
		return;
	}
	//战斗技能
	if (isButton(button_fight)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		changeMainButtonState(pSender);
		changeMinorButtonState(nullptr);
		openSkillTreeView();
		return;
	}
	//贸易技能
	if (isButton(button_economic)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		changeMainButtonState(pSender);
		changeMinorButtonState(nullptr);
		openSkillTreeView();
		return;
	}
	//左侧技能导航按钮
	if (isButton(button_skill1)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		changeMinorButtonState(pSender);
		openSkillTreeView();
		return;
	}
	//左侧技能导航按钮
	if (isButton(button_skill2)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		changeMinorButtonState(pSender);
		openSkillTreeView();
		return;
	}
	//左侧技能导航按钮
	if (isButton(button_skill3))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		changeMinorButtonState(pSender);
		openSkillTreeView();
		return;
	}
	//左侧技能导航按钮
	if (isButton(button_skill4)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		changeMinorButtonState(pSender);
		openSkillTreeView();
		return;
	}
	//调出技能加点框
	if (isButton(button_skill_image))  
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		int nSkill_id = pSender->getTag();
		openUpgradeView(nSkill_id);
		return;
	}

	if (isButton(button_confirm_yes)) 
	{
		if (m_nSoltIndex == GOLD_NOT_FAIL) //v 票不足
		{
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
		}
		else //卸载确定
		{ 
			ProtocolThread::GetInstance()->selectHeroPositiveSkill(m_nSoltIndex, -1, UILoadingIndicator::create(m_pParent));
		}

		return;
	}
	//加技能点
	if (isButton(button_add))  
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto& skill = SINGLE_SHOP->getSkillTrees()[pSender->getTag()];
		if (skill.max_level <= skill.cur_skill_level )
		{
			UIInform::getInstance()->openInformView(m_pParent);
			UIInform::getInstance()->openConfirmYes("TIP_CENTERSKILL_MAX_SKILL_POINT");
		}else if (SINGLE_HERO->m_iLevel < skill.require_level)
		{
			UIInform::getInstance()->openInformView(m_pParent);
			UIInform::getInstance()->openConfirmYes("TIP_CENTER_NOT_HEROLV_REQIUE");
		}else if (skill.base_skill_id && SINGLE_SHOP->getSkillTrees()[skill.base_skill_id].cur_skill_level < skill.base_skill_require_lv)
		{
			UIInform::getInstance()->openInformView(m_pParent);
			UIInform::getInstance()->openConfirmYes("TIP_CENTER_NOT_SKILL_REQIUE");
		}else
		{
			ProtocolThread::GetInstance()->addSkillPoint(pSender->getTag(),1,UILoadingIndicator::create(m_pParent));
		}
		return;
	}
	//已装备技能和可以装备的主动技能的详情
	if (isButton(current_equip_bg) || isButton(select_equip_bg))
	{
		auto t_skill_lv = pSender->getChildByName<Text*>("text_item_skill_lv");
		SKILL_DEFINE skillDefine;
		skillDefine.id = pSender->getTag();
		skillDefine.lv = atoi(t_skill_lv->getString().data());
		skillDefine.skill_type = SKILL_TYPE_PLAYER;
		skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
		UICommon::getInstance()->openCommonView(m_pParent);
		UICommon::getInstance()->flushSkillView(skillDefine);
		return;
	}
}

void UICenterCharactorSkill::onServerEvent(struct ProtobufCMessage *message,int msgType)
{
	switch (msgType)
	{
	case PROTO_TYPE_GetSkillsDetailsResult:
		{
			m_pResult = (GetSkillsDetailsResult*)(message);
			if (m_pResult->failed == 0)
			{
				auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_CSB]);
				if (!view)
				{
					m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_CSB]);
					m_pSkillTreeMainButton = nullptr;
					m_pSkillTreeMinorButton = nullptr;
				}
				changeMainButtonState(nullptr);
				changeMinorButtonState(nullptr);
				pushResultInMap();
				updateMainUISkillPoint();
			}else
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_FLUSH_SKILL_FAIL");
			}
			break;
		}
	case PROTO_TYPE_ResetSkillPointsResult:
		{
			ResetSkillPointsResult* result = (ResetSkillPointsResult*)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_RESET_SKILL_POINTS_SUCCESS");
				m_nSkillPoints = result->totalpoints;
				//更新剩余免费次数
				m_pResult->freeresettimes = result->freeresettimes;
				updateMainUISkillPoint();
				ProtocolThread::GetInstance()->getSkillsDetails(UILoadingIndicator::create(m_pParent));
				if (result->costvtickets > 0)
				{
					Utils::consumeVTicket("21", 1, result->costvtickets);
				}

			}else if (result->failed == GOLD_NOT_FAIL)
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openGoldNotEnoughYesOrNo(100);
			}else
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openGoldNotEnoughYesOrNo(100);
			}
			break;
		}
	case PROTO_TYPE_AddSkillPointResult:
		{
			AddSkillPointResult* result = (AddSkillPointResult*)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_ADD_SKILL_POINT_SUCCESS");
				m_pParent->closeView();
				auto& skillTrees = SINGLE_SHOP->getSkillTrees();
				skillTrees[result->skillid].cur_skill_level = result->skilllv;
				m_nSkillPoints = result->remainskillpoints;
				updateMainUISkillPoint();
				openSkillTreeView();
			}else
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_ADD_SKILL_POINT_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetHeroPositiveSkillListResult:
		{
			GetHeroPositiveSkillListResult *result = (GetHeroPositiveSkillListResult*)message;
			if (result->failed == 0)
			{
				m_pHeroSkillResult = result;
				openAddSkillView();
			}else
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_SKILL_LIST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetSelectedPositiveSkillsResult:
		{
			GetSelectedPositiveSkillsResult *result = (GetSelectedPositiveSkillsResult*)message;
			if (result->failed == 0)
			{
				m_pSelPosResult = result;
				m_nSkillPoints = result->skillpoints;
				flushMainUIALLSkill();
				updateMainUISkillPoint();
			}else
			{
				ProtocolThread::GetInstance()->getSelectedPositiveSkills(UILoadingIndicator::create(m_pParent));
			}
			break;
		}
	case PROTO_TYPE_SelectHeroPositiveSkillResult:
		{
			SelectHeroPositiveSkillResult *result = (SelectHeroPositiveSkillResult*)message;
			if (result->failed == 0)
			{
				/*
				InformView::getInstance()->openInformView(m_pParent);
				if (result->skillid == -1)
				{
					InformView::getInstance()->openViewAutoClose("TIP_CENTER_SKILL_REMOVE_SUCCESS");
				}else
				{
					InformView::getInstance()->openViewAutoClose("TIP_CENTER_SKILL_SETING_SUCCESS");
				}
				*/
				ProtocolThread::GetInstance()->getSelectedPositiveSkills(UILoadingIndicator::create(m_pParent));
				m_pParent->closeView();
			}else if (result->failed > 10)
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_SKILL_REMOVE_FAIL");
			}else
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_SKILL_SETING_FAIL");
			}
			break;
		}
	default:
		break;
	}
}

void UICenterCharactorSkill::updateMainUISkillPoint()
{
	std::string s_points = StringUtils::format("%d",m_nSkillPoints);
	auto view1 = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_CSB]);
	if (view1)
	{
		auto b_tree = view1->getChildByName<Button*>("button_skilltree");
		auto i_unread = b_tree->getChildByName<ImageView*>("image_unread_1");
		auto skillpoint_num = b_tree->getChildByName<Text*>("text_skillnum");
		skillpoint_num->setString(s_points);

		if (m_nSkillPoints > 0)
		{
			i_unread->setVisible(true);
			skillpoint_num->setVisible(true);
		}
		else
		{
			i_unread->setVisible(false);
			skillpoint_num->setVisible(false);
		}
	}
	auto view2 = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_CSB]);
	if (view2)
	{
		auto b_reset = dynamic_cast<Button*>(Helper::seekWidgetByName(view2, "button_reset"));
		auto image_v = dynamic_cast<ImageView*>(Helper::seekWidgetByName(b_reset, "image_v"));
		auto t_reset = dynamic_cast<Text*>(Helper::seekWidgetByName(view2, "label_change"));
		if (SINGLE_HERO->m_iLevel - 1 == m_nSkillPoints)
		{
			b_reset->setBright(false);
			setGLProgramState(image_v, true);
		}
		else
		{
			std::string btn_txt = SINGLE_SHOP->getTipsInfo()["TIP_CENTER_RESET_SKILL_BTN"];
			if(m_pResult->freeresettimes > 0){
				std::string basic_str = SINGLE_SHOP->getTipsInfo()["TIP_CENTER_RESET_SKILL_FREE_BTN"];
				btn_txt = StringUtils::format(basic_str.c_str(), m_pResult->freeresettimes);
			}
			if(t_reset){
				t_reset->setString(btn_txt);
				t_reset->setContentSize(b_reset->getContentSize());
			}

			b_reset->setBright(true);
			setGLProgramState(image_v, false);
		}

		auto t_points = dynamic_cast<Text*>(Helper::seekWidgetByName(view2,"label_combat_0"));
		t_points->setString(s_points);
//chengyuan++
		auto p_content_1 = view2->getChildByName("panel_content_1");
		auto t_combat = p_content_1->getChildByName<Text*>("label_combat");
		auto i_imageSkill = p_content_1->getChildByName("image_skill");
		auto t_skillPoint = p_content_1->getChildByName<Text*>("label_combat_0");
		t_combat->ignoreContentAdaptWithSize(true);
		t_skillPoint->ignoreContentAdaptWithSize(true);
		i_imageSkill->setPositionX(t_combat->getPositionX() + t_combat->getContentSize().width / 2 + i_imageSkill->getContentSize().width / 2);
		t_skillPoint->setPositionX(i_imageSkill->getPositionX() + i_imageSkill->getContentSize().width / 2 + t_skillPoint->getContentSize().width / 2);
//
	}
}

void UICenterCharactorSkill::flushMainUIALLSkill()
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_CSB]);
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_CSB]);
	auto skill_bg = view->getChildByName<ImageView*>("image_image_gear_equip_bg");
	for (int i = 0; i < 4; i++)
	{
		auto panel_skill = skill_bg->getChildByName<Widget*>(StringUtils::format("panel_skill_%d",i+1));
		auto b_skill = panel_skill->getChildByName<Button*>("button_combar_skil");
		auto i_add = b_skill->getChildByName<ImageView*>("image_add");
		auto i_name_bg = panel_skill->getChildByName<ImageView*>("image_skill_name_bg");
		auto t_name = i_name_bg->getChildByName<Text*>("label_skillname");
		auto t_skill = panel_skill->getChildByName<Text*>("text_item_skill_lv");
		b_skill->ignoreContentAdaptWithSize(false);
		b_skill->setTag(i + START_INDEX + 1);
		bool iscan = false;
		int j = 0;
		for (; j < m_pSelPosResult->n_skills; j++)
		{
			if (m_pSelPosResult->skills[j]->slotindex == i+1)
			{
				iscan = true;
				break;
			}
		}

		if (iscan)
		{
			b_skill->loadTextureNormal(getSkillIconPath(m_pSelPosResult->skills[j]->id, SKILL_TYPE_PLAYER));
			i_add->setVisible(false);
			i_name_bg->setVisible(true);
			t_name->setString(getSkillName(m_pSelPosResult->skills[j]->id, SKILL_TYPE_PLAYER));
			t_skill->setVisible(true);
			setTextSizeAndOutline(t_skill,m_pSelPosResult->skills[j]->level);
			t_name->enableOutline(Color4B::BLACK, OUTLINE_MIN);
		}else
		{
			b_skill->loadTextureNormal("cocosstudio/login_ui/player_720/skill_initiative_bg.png");
			i_add->setVisible(true);
			i_name_bg->setVisible(false);
			t_skill->setVisible(false);
		}
	}
}

void UICenterCharactorSkill::openAddSkillView()
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_SLOTCHANGE_CSB]);
	auto panel_skill = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_SLOTCHANGE_CSB]);
	panel_skill->setPosition(STARTPOS);
	auto i_curSkill = panel_skill->getChildByName<Button*>("current_equip_bg");
	auto t_name = panel_skill->getChildByName<Text*>("label_current_itemname");
	panel_skill->getChildByName<Button*>("button_ok")->setBright(false);
	panel_skill->getChildByName<Button*>("button_ok")->setTouchEnabled(false);
	auto i_pulldownRight = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_skill, "image_pulldown_0"));
	auto i_pulldownleft = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_skill, "image_pulldown"));
	i_pulldownRight->setVisible(false);
	i_pulldownleft->setVisible(false);
	int curIndex = -1; 
	for (int i = 0; i < m_pSelPosResult->n_skills; i++)
	{
		if (m_pSelPosResult->skills[i]->slotindex == m_nSoltIndex)
		{
			curIndex = i;
			break;
		}
	}
	auto t_curSkill_name = panel_skill->getChildByName<Text*>("label_current_itemname");
	auto b_curSkill = panel_skill->getChildByName<Button*>("current_equip_bg");
	auto t_skill_lv = b_curSkill->getChildByName<Text*>("text_item_skill_lv");
	b_curSkill->ignoreContentAdaptWithSize(false);
	if (curIndex >= 0)
	{
		b_curSkill->loadTextureNormal(getSkillIconPath(m_pSelPosResult->skills[curIndex]->id, SKILL_TYPE_PLAYER));
		t_skill_lv->setVisible(true);
		setTextSizeAndOutline(t_skill_lv,m_pSelPosResult->skills[curIndex]->level);
		t_curSkill_name->setString(getSkillName(m_pSelPosResult->skills[curIndex]->id, SKILL_TYPE_PLAYER));
		panel_skill->getChildByName<Button*>("button_uninstall")->setBright(true);
		panel_skill->getChildByName<Button*>("button_uninstall")->setTouchEnabled(true);
		b_curSkill->setTouchEnabled(true);
		b_curSkill->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorSkill::menuCall_func,this));
		b_curSkill->setTag(m_pSelPosResult->skills[curIndex]->id);
	}else
	{
//		b_curSkill->loadTextureNormal("cocosstudio/login_ui/player_720/skill_initiative_bg.png");
		b_curSkill->setTouchEnabled(false);
		t_skill_lv->setVisible(false);
		t_curSkill_name->setString("N/A");
		panel_skill->getChildByName<Button*>("button_uninstall")->setBright(false);
		panel_skill->getChildByName<Button*>("button_uninstall")->setTouchEnabled(false);
	}
	updataSelectSkill(-1);
	auto l_left = panel_skill->getChildByName<ListView*>("listview_left");
	auto p_no = panel_skill->getChildByName<Widget*>("panel_no");
	l_left->removeAllChildrenWithCleanup(true);
	p_no->setVisible(false);
	if (m_pHeroSkillResult->n_skills < 1)
	{
		p_no->setVisible(true);
		return;
	}
	auto p_items = panel_skill->getChildByName<Widget*>("panel_items");
	for (int i = 0; i < ceil(m_pHeroSkillResult->n_skills/6.0); i++)
	{
		auto p_items_clone = p_items->clone();
		for (int j = 0; j < 6; j++)
		{
			auto i_item = p_items_clone->getChildByName<ImageView*>(StringUtils::format("image_items_%d",j+1));
			if (i * 6 + j < m_pHeroSkillResult->n_skills)
			{
				i_item->setVisible(true);
				i_item->ignoreContentAdaptWithSize(false);
				i_item->loadTexture(getSkillIconPath(m_pHeroSkillResult->skills[i * 6 + j]->id, SKILL_TYPE_PLAYER));
				auto b_item = i_item->getChildByName<Button*>("button_equip_items");
				auto t_skill = dynamic_cast<Text*>(Helper::seekWidgetByName(i_item,"text_item_skill_lv"));
				b_item->setTag(m_pHeroSkillResult->skills[i * 6 + j]->id+START_INDEX);
				setTextSizeAndOutline(t_skill,m_pHeroSkillResult->skills[i * 6 + j]->level);
			}else
			{
				i_item->setVisible(false);
			}
		}
		l_left->pushBackCustomItem(p_items_clone);
	}
	auto l_list1 = dynamic_cast<ListView*>(Helper::seekWidgetByName(panel_skill, "listview_p"));
	auto l_list2 = dynamic_cast<ListView*>(Helper::seekWidgetByName(panel_skill, "listview_left"));
	auto i_pulldown1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_skill, "image_pulldown"));
	auto b_pulldown = i_pulldown1->getChildByName("button_pulldown");
	i_pulldown1->setVisible(false);
	auto i_pulldown2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_skill, "image_pulldown_0"));
	
	auto b_pulldown_1 = i_pulldown2->getChildByName("button_pulldown");
	b_pulldown->setPositionX(b_pulldown->getPositionX() - b_pulldown->getContentSize().width / 2);
	b_pulldown_1->setPositionX(b_pulldown_1->getPositionX() - b_pulldown_1->getContentSize().width / 2);
	addListViewBar(l_list1, i_pulldown1);
	addListViewBar(l_list2, i_pulldown2);
}

void UICenterCharactorSkill::updataSelectSkill(int nSkill_id)
{
	auto panel_skill = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_SLOTCHANGE_CSB]);
	auto t_selName = panel_skill->getChildByName<Text*>("label_select_itemname");
	auto t_seldesc = panel_skill->getChildByName<Text*>("label_select_itemname_0");
	auto b_select_skill = panel_skill->getChildByName<Button*>("select_equip_bg");
	auto t_skill_lv = b_select_skill->getChildByName<Text*>("text_item_skill_lv");

	t_selName->setVisible(true);
	t_seldesc->setVisible(true);
	if (nSkill_id == -1)
	{
		t_selName->setString("N/A");
		t_seldesc->setVisible(false);
		t_skill_lv->setVisible(false);
		b_select_skill->setTouchEnabled(false);
	}else
	{
		t_seldesc->setVisible(true);
		t_skill_lv->setVisible(true);
		b_select_skill->setTouchEnabled(true);
		b_select_skill->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorSkill::menuCall_func,this));
		b_select_skill->setTag(nSkill_id);
		b_select_skill->loadTextureNormal(getSkillIconPath(nSkill_id, SKILL_TYPE_PLAYER));
		auto skill = SINGLE_SHOP->getSkillTrees()[nSkill_id];
		int cur_lv = 0;
		for (int i = 0; i < m_pHeroSkillResult->n_skills; i++)
		{
			if (m_pHeroSkillResult->skills[i]->id == nSkill_id)
			{
				cur_lv = m_pHeroSkillResult->skills[i]->level;
				break;
			}
		}
		setTextSizeAndOutline(t_skill_lv, cur_lv);
		float number = 0;
		int cd = 0;
		switch (nSkill_id)
		{
		case SKILL_POWWEFUL_SHOOT:
		case SKILL_SALVO_SHOOT:
		case SKILL_INCENDIARY_SHOOT:
		case SKILL_FAST_RELOAD:
		case SKILL_WEAKNESS_ATTACK:
		case SKILL_ASSAULT_NAVIGATION:
		case SKILL_SNEAK_ATTACK:
		case SKILL_EMBOLON_ATTACK:
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
			{
				number = skill.effect_per_lv*cur_lv/SKILL_DER_MIN;
				cd = skill.cd - skill.cdreduce_per_lv*cur_lv;
				break;
			}
		case SKILL_HAMMER_SHOOT:
			{
				number = skill.duration_per_lv*cur_lv;
				cd = skill.cd - skill.cdreduce_per_lv*cur_lv;
				break;
			}
		case SKILL_INDOMITABLE_WILL:
		case SKILL_ROAR_OF_VICTORY:
		case SKILL_INDUCE_TO_CAPITULATE:
			{
				number = skill.rate_per_lv*cur_lv;
				cd = skill.cd - skill.cdreduce_per_lv*cur_lv;
				break;
			}
		case SKILL_HULL_TRACTION:
			{
				number = skill.effect_per_lv*skill.cur_skill_level;
				cd = skill.cd - skill.cdreduce_per_lv*skill.cur_skill_level;
				break;
			}
		default:
			{
				number = skill.effect_per_lv*cur_lv / SKILL_DER_MAX;
				cd = skill.cd - skill.cdreduce_per_lv*cur_lv;
				break;
			}
		}
		std::string str = skill.desc;
		std::string old_value = "[number1]";
		std::string new_value;

		if (nSkill_id == SKILL_TECHNIQUE_OF_BUSINESS || nSkill_id == SKILL_SHIPBUILDING_TECHNOLOGY || nSkill_id == SKILL_HULL_TRACTION)
		{
			new_value = StringUtils::format("%.0f",number);
		}else
		{
			new_value = StringUtils::format("%.2f",number);
		}
		repalce_all_ditinct(str,old_value,new_value);
		old_value = "[cd]";
		new_value = StringUtils::format("%d",cd);
		repalce_all_ditinct(str,old_value,new_value);
		t_selName->setString(skill.name);
		t_seldesc->setString(str);
	}
}

void UICenterCharactorSkill::openSkillTreeView()
{
	auto panel_skill = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_CSB]);
	auto p_flghting = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_skill,"panel_skilltree_flghting"));
	auto p_ecinimic = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_skill,"panel_skilltree_ecinimic"));
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_skill,"label_title_11"));
	auto p_skill_3 = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_flghting, "panel_skill_3"));

	auto& skill = SINGLE_SHOP->getSkillTrees();
	int skillOrder = 0;
	if (strcmp(m_pSkillTreeMainButton->getName().data(),"button_fight") == 0)
	{
		// 为以后修改方便先对应id
		int skill_1[2] = {0,0};
		int skill_2[4] = {0,0,0,0};
		int skill_3 = SKILL_GREAT_SHOOTER;
		if (strcmp(m_pSkillTreeMinorButton->getName().data(),"button_skill1") == 0)
		{
			skillOrder = 1;
			skill_1[0] = 5;
			skill_1[1] = 6;
			skill_2[0] = 1;
			skill_2[1] = 2;
			skill_2[2] = 3;
			skill_2[3] = 4;
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_FIGHT_SKILL1"]);
			p_flghting->setVisible(true);
			p_ecinimic->setVisible(false);
			p_skill_3->setVisible(true);
		}else if (strcmp(m_pSkillTreeMinorButton->getName().data(),"button_skill2") == 0)
		{
			skillOrder = 2;
			skill_1[0] = 11;
			skill_1[1] = 12;
			skill_2[0] = 7;
			skill_2[1] = 8;
			skill_2[2] = 9;
			skill_2[3] = 10;
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_FIGHT_SKILL2"]);
			p_flghting->setVisible(true);
			p_ecinimic->setVisible(false);
			p_skill_3->setVisible(false);
		}
		else if (strcmp(m_pSkillTreeMinorButton->getName().data(), "button_skill3") == 0)
		{
			skillOrder = 3;
			skill_1[0] = 17;
			skill_1[1] = 18;
			skill_2[0] = 13;
			skill_2[1] = 14;
			skill_2[2] = 15;
			skill_2[3] = 16;
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_FIGHT_SKILL3"]);
			p_flghting->setVisible(true);
			p_ecinimic->setVisible(false);
			p_skill_3->setVisible(false);
		}
		else if (strcmp(m_pSkillTreeMinorButton->getName().data(), "button_skill4") == 0)
		{
			//战斗技能，将帅之才
			skillOrder = 4;
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_FIGHT_SKILL4"]);
			p_flghting->setVisible(false);
			p_ecinimic->setVisible(true);	
		}
		switch (skillOrder)
		{
		case 1:
		case 2:
		case 3:
		{
				  auto p_skilltree_1 = p_flghting->getChildByName<Widget*>("panel_skilltree_1");
				  auto b_line_1 = p_skilltree_1->getChildByName<Button*>("button_line_1");

				  auto p_skill_1 = p_skilltree_1->getChildByName<Widget*>("panel_skill_1");
				  auto b_skill_1 = p_skill_1->getChildByName<Button*>("button_skill_image");
				  auto i_lock_1 = b_skill_1->getChildByName<ImageView*>("image_lock");
				  auto t_skill_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1, "label_skillname"));
				  auto t_skill_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1, "text_item_skill_lv"));

				  auto p_skill_1_1 = p_skilltree_1->getChildByName<Widget*>("panel_skill_1_1");
				  auto b_skill_1_1 = p_skill_1_1->getChildByName<Button*>("button_skill_image");
				  auto i_lock_1_1 = b_skill_1_1->getChildByName<ImageView*>("image_lock");
				  auto t_skill_1_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1_1, "label_skillname"));
				  auto t_skill_1_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1_1, "text_item_skill_lv"));

				  b_skill_1->ignoreContentAdaptWithSize(false);
				  b_skill_1->loadTextureNormal(getSkillIconPath(skill_1[0], SKILL_TYPE_PLAYER));
				  b_skill_1->setTag(skill_1[0]);
				  t_skill_1_name->setString(skill[skill_1[0]].name);
				  t_skill_1_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_1_lv, skill[skill_1[0]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_1[0]].require_level)
				  {
					  i_lock_1->setVisible(false);
				  }
				  else
				  {
					  i_lock_1->setVisible(true);
					  t_skill_1_lv->setVisible(false);
				  }

				  if (skill[skill_1[0]].points_per_level > m_nSkillPoints)
				  {
					  t_skill_1_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_1_lv->setColor(Color3B::WHITE);
				  }

				  b_skill_1_1->ignoreContentAdaptWithSize(false);
				  b_skill_1_1->loadTextureNormal(getSkillIconPath(skill_1[1], SKILL_TYPE_PLAYER));
				  b_skill_1_1->setTag(skill_1[1]);
				  t_skill_1_1_name->setString(skill[skill_1[1]].name);
				  t_skill_1_1_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_1_1_lv, skill[skill_1[1]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_1[1]].require_level &&
					  skill[skill[skill_1[1]].base_skill_id].cur_skill_level >= skill[skill_1[1]].base_skill_require_lv)
				  {
					  i_lock_1_1->setVisible(false);
					  b_line_1->setBright(true);
				  }
				  else
				  {
					  i_lock_1_1->setVisible(true);
					  t_skill_1_1_lv->setVisible(false);
					  b_line_1->setBright(false);
				  }

				  if (skill[skill[skill_1[1]].base_skill_id].points_per_level > m_nSkillPoints)
				  {
					  t_skill_1_1_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_1_1_lv->setColor(Color3B::WHITE);
				  }

				  auto p_skilltree_2 = p_flghting->getChildByName<Widget*>("panel_skilltree_2");
				  auto b_line_2_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(p_skilltree_2, "button_line_2_1"));
				  auto b_line_2_2 = dynamic_cast<Button*>(Helper::seekWidgetByName(p_skilltree_2, "button_line_2_2"));
				  auto b_line_2_3 = dynamic_cast<Button*>(Helper::seekWidgetByName(p_skilltree_2, "button_line_2_3"));

				  auto p_skill_2 = p_skilltree_2->getChildByName<Widget*>("panel_skill_2");
				  auto b_skill_2 = p_skill_2->getChildByName<Button*>("button_skill_image");
				  auto i_lock_2 = b_skill_2->getChildByName<ImageView*>("image_lock");
				  auto t_skill_2_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2, "label_skillname"));
				  auto t_skill_2_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2, "text_item_skill_lv"));

				  auto p_skill_2_1 = p_skilltree_2->getChildByName<Widget*>("panel_skill_2_1");
				  auto b_skill_2_1 = p_skill_2_1->getChildByName<Button*>("button_skill_image");
				  auto i_lock_2_1 = b_skill_2_1->getChildByName<ImageView*>("image_lock");
				  auto t_skill_2_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2_1, "label_skillname"));
				  auto t_skill_2_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2_1, "text_item_skill_lv"));

				  auto p_skill_2_2 = p_skilltree_2->getChildByName<Widget*>("panel_skill_2_2");
				  auto b_skill_2_2 = p_skill_2_2->getChildByName<Button*>("button_skill_image");
				  auto i_lock_2_2 = b_skill_2_2->getChildByName<ImageView*>("image_lock");
				  auto t_skill_2_2_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2_2, "label_skillname"));
				  auto t_skill_2_2_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2_2, "text_item_skill_lv"));

				  auto p_skill_2_3 = p_skilltree_2->getChildByName<Widget*>("panel_skill_2_3");
				  auto b_skill_2_3 = p_skill_2_3->getChildByName<Button*>("button_skill_image");
				  auto i_lock_2_3 = b_skill_2_3->getChildByName<ImageView*>("image_lock");
				  auto t_skill_2_3_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2_3, "label_skillname"));
				  auto t_skill_2_3_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2_3, "text_item_skill_lv"));

				  b_skill_2->ignoreContentAdaptWithSize(false);
				  b_skill_2->loadTextureNormal(getSkillIconPath(skill_2[0], SKILL_TYPE_PLAYER));
				  b_skill_2->setTag(skill_2[0]);
				  t_skill_2_name->setString(skill[skill_2[0]].name);
				  t_skill_2_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_2_lv, skill[skill_2[0]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_2[0]].require_level)
				  {
					  i_lock_2->setVisible(false);
				  }
				  else
				  {
					  i_lock_2->setVisible(true);
					  t_skill_2_lv->setVisible(false);
				  }

				  if (skill[skill_2[0]].points_per_level > m_nSkillPoints)
				  {
					  t_skill_2_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_2_lv->setColor(Color3B::WHITE);
				  }

				  b_skill_2_1->ignoreContentAdaptWithSize(false);
				  b_skill_2_1->loadTextureNormal(getSkillIconPath(skill_2[1], SKILL_TYPE_PLAYER));
				  b_skill_2_1->setTag(skill_2[1]);
				  t_skill_2_1_name->setString(skill[skill_2[1]].name);
				  t_skill_2_1_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_2_1_lv, skill[skill_2[1]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_2[1]].require_level &&
					  skill[skill[skill_2[1]].base_skill_id].cur_skill_level >= skill[skill_2[1]].base_skill_require_lv)
				  {
					  i_lock_2_1->setVisible(false);
					  b_line_2_1->setBright(true);
				  }
				  else
				  {
					  i_lock_2_1->setVisible(true);
					  t_skill_2_1_lv->setVisible(false);
					  b_line_2_1->setBright(false);
				  }

				  if (skill[skill[skill_2[1]].base_skill_id].points_per_level > m_nSkillPoints)
				  {
					  t_skill_2_1_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_2_1_lv->setColor(Color3B::WHITE);
				  }

				  b_skill_2_2->ignoreContentAdaptWithSize(false);
				  b_skill_2_2->loadTextureNormal(getSkillIconPath(skill_2[2], SKILL_TYPE_PLAYER));
				  b_skill_2_2->setTag(skill_2[2]);
				  t_skill_2_2_name->setString(skill[skill_2[2]].name);
				  t_skill_2_2_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_2_2_lv, skill[skill_2[2]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_2[2]].require_level &&
					  skill[skill[skill_2[2]].base_skill_id].cur_skill_level >= skill[skill_2[2]].base_skill_require_lv)
				  {
					  i_lock_2_2->setVisible(false);
					  b_line_2_2->setBright(true);
				  }
				  else
				  {
					  i_lock_2_2->setVisible(true);
					  t_skill_2_2_lv->setVisible(false);
					  b_line_2_2->setBright(false);
				  }

				  if (skill[skill[skill_2[2]].base_skill_id].points_per_level > m_nSkillPoints)
				  {
					  t_skill_2_2_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_2_2_lv->setColor(Color3B::WHITE);
				  }

				  b_skill_2_3->ignoreContentAdaptWithSize(false);
				  b_skill_2_3->loadTextureNormal(getSkillIconPath(skill_2[3], SKILL_TYPE_PLAYER));
				  b_skill_2_3->setTag(skill_2[3]);
				  t_skill_2_3_name->setString(skill[skill_2[3]].name);
				  t_skill_2_3_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_2_3_lv, skill[skill_2[3]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_2[3]].require_level &&
					  skill[skill[skill_2[3]].base_skill_id].cur_skill_level >= skill[skill_2[3]].base_skill_require_lv)
				  {
					  i_lock_2_3->setVisible(false);
					  b_line_2_3->setBright(true);
				  }
				  else
				  {
					  i_lock_2_3->setVisible(true);
					  t_skill_2_3_lv->setVisible(false);
					  b_line_2_3->setBright(false);
				  }

				  if (skill[skill[skill_2[3]].base_skill_id].points_per_level > m_nSkillPoints)
				  {
					  t_skill_2_3_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_2_3_lv->setColor(Color3B::WHITE);
				  }

				  auto b_skill_3 = p_skill_3->getChildByName<Button*>("button_skill_image");
				  auto i_lock_3 = b_skill_3->getChildByName<ImageView*>("image_lock");
				  auto t_skill_3_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_3, "label_skillname"));
				  auto t_skill_3_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_3, "text_item_skill_lv"));
				  b_skill_3->ignoreContentAdaptWithSize(false);
				  b_skill_3->loadTextureNormal(getSkillIconPath(skill_3, SKILL_TYPE_PLAYER));
				  b_skill_3->setTag(skill_3);
				  t_skill_3_name->setString(skill[skill_3].name);
				  t_skill_3_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_3_lv, skill[skill_3].cur_skill_level);

				  if (SINGLE_HERO->m_iLevel >= skill[skill_3].require_level)
				  {
					  i_lock_3->setVisible(false);
				  }
				  else
				  {
					  i_lock_3->setVisible(true);
					  t_skill_3_lv->setVisible(false);
				  }

				  if (skill[skill_3].points_per_level > m_nSkillPoints)
				  {
					  t_skill_3_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_3_lv->setColor(Color3B::WHITE);
				  }
		}
		case 4:
		{
				  //新加的技能,将帅之才，都是被动技能
				  int skill_1[2] = { 0, 0 };
				  int skill_2[2] = { 0, 0 };
				  int skill_3[2] = { 0, 0 };
				  skill_1[0] = 31;
				  skill_1[1] = 32;
				  skill_2[0] = 33;
				  skill_2[1] = 34;
				  skill_3[0] = 35;
				  skill_3[1] = 36;
				  auto p_skilltree_1 = p_ecinimic->getChildByName<Widget*>("panel_skilltree_1");
				  auto b_line_1 = p_skilltree_1->getChildByName<Button*>("button_line_1");
				  auto p_skill_1 = p_skilltree_1->getChildByName<Button*>("panel_skill_1");
				  auto b_skill_1 = p_skill_1->getChildByName<Button*>("button_skill_image");
				  auto i_lock_1 = b_skill_1->getChildByName<Button*>("image_lock");
				  auto t_skill_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1, "label_skillname"));
				  auto t_skill_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1, "text_item_skill_lv"));

				  auto p_skill_1_1 = p_skilltree_1->getChildByName<Button*>("panel_skill_1_1");
				  auto b_skill_1_1 = p_skill_1_1->getChildByName<Button*>("button_skill_image");
				  auto i_lock_1_1 = b_skill_1_1->getChildByName<Button*>("image_lock");
				  auto t_skill_1_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1_1, "label_skillname"));
				  auto t_skill_1_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1_1, "text_item_skill_lv"));

				  if (skill_1[0] == 0)
				  {
					  p_skilltree_1->setVisible(false);
				  }
				  else
				  {
					  p_skilltree_1->setVisible(true);
				  }

				  b_skill_1->ignoreContentAdaptWithSize(false);
				  b_skill_1->loadTextureNormal(getSkillIconPath(skill_1[0], SKILL_TYPE_PLAYER));
				  b_skill_1->setTag(skill_1[0]);
				  t_skill_1_name->setString(skill[skill_1[0]].name);
				  t_skill_1_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_1_lv, skill[skill_1[0]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_1[0]].require_level)
				  {
					  i_lock_1->setVisible(false);
				  }
				  else
				  {
					  i_lock_1->setVisible(true);
					  t_skill_1_lv->setVisible(false);
				  }

				  if (skill[skill_1[0]].points_per_level > m_nSkillPoints)
				  {
					  t_skill_1_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_1_lv->setColor(Color3B::WHITE);
				  }

				  b_skill_1_1->ignoreContentAdaptWithSize(false);
				  b_skill_1_1->loadTextureNormal(getSkillIconPath(skill_1[1], SKILL_TYPE_PLAYER));
				  b_skill_1_1->setTag(skill_1[1]);
				  t_skill_1_1_name->setString(skill[skill_1[1]].name);
				  t_skill_1_1_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_1_1_lv, skill[skill_1[1]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_1[1]].require_level &&
					  skill[skill[skill_1[1]].base_skill_id].cur_skill_level >= skill[skill_1[1]].base_skill_require_lv)
				  {
					  i_lock_1_1->setVisible(false);
					  b_line_1->setBright(true);
				  }
				  else
				  {
					  i_lock_1_1->setVisible(true);
					  t_skill_1_1_lv->setVisible(false);
					  b_line_1->setBright(false);
				  }

				  if (skill[skill[skill_1[1]].base_skill_id].points_per_level > m_nSkillPoints)
				  {
					  t_skill_1_1_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_1_1_lv->setColor(Color3B::WHITE);
				  }

				  auto p_skilltree_2 = p_ecinimic->getChildByName<Widget*>("panel_skilltree_2");
				  auto b_line_2 = p_skilltree_2->getChildByName<Button*>("button_line_1");
				  auto p_skill_2 = p_skilltree_2->getChildByName<Button*>("panel_skill_1");
				  auto b_skill_2 = p_skill_2->getChildByName<Button*>("button_skill_image");
				  auto i_lock_2 = b_skill_2->getChildByName<Button*>("image_lock");
				  auto t_skill_2_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2, "label_skillname"));
				  auto t_skill_2_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2, "text_item_skill_lv"));

				  auto p_skill_2_1 = p_skilltree_2->getChildByName<Button*>("panel_skill_1_1");
				  auto b_skill_2_1 = p_skill_2_1->getChildByName<Button*>("button_skill_image");
				  auto i_lock_2_1 = b_skill_2_1->getChildByName<Button*>("image_lock");
				  auto t_skill_2_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2_1, "label_skillname"));
				  auto t_skill_2_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2_1, "text_item_skill_lv"));

				  if (skill_2[0] == 0)
				  {
					  p_skilltree_2->setVisible(false);
				  }
				  else
				  {
					  p_skilltree_2->setVisible(true);
				  }

				  b_skill_2->ignoreContentAdaptWithSize(false);
				  b_skill_2->loadTextureNormal(getSkillIconPath(skill_2[0], SKILL_TYPE_PLAYER));
				  b_skill_2->setTag(skill_2[0]);
				  t_skill_2_name->setString(skill[skill_2[0]].name);
				  t_skill_2_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_2_lv, skill[skill_2[0]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_2[0]].require_level)
				  {
					  i_lock_2->setVisible(false);
				  }
				  else
				  {
					  i_lock_2->setVisible(true);
					  t_skill_2_lv->setVisible(false);
				  }

				  if (skill[skill_2[0]].points_per_level > m_nSkillPoints)
				  {
					  t_skill_2_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_2_lv->setColor(Color3B::WHITE);
				  }

				  b_skill_2_1->ignoreContentAdaptWithSize(false);
				  b_skill_2_1->loadTextureNormal(getSkillIconPath(skill_2[1], SKILL_TYPE_PLAYER));
				  b_skill_2_1->setTag(skill_2[1]);
				  t_skill_2_1_name->setString(skill[skill_2[1]].name);
				  t_skill_2_1_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_2_1_lv, skill[skill_2[1]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_2[1]].require_level &&
					  skill[skill[skill_2[1]].base_skill_id].cur_skill_level >= skill[skill_2[1]].base_skill_require_lv)
				  {
					  i_lock_2_1->setVisible(false);
					  b_line_2->setBright(true);
				  }
				  else
				  {
					  i_lock_2_1->setVisible(true);
					  t_skill_2_1_lv->setVisible(false);
					  b_line_2->setBright(false);
				  }

				  if (skill[skill[skill_2[1]].base_skill_id].points_per_level > m_nSkillPoints)
				  {
					  t_skill_2_1_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_2_1_lv->setColor(Color3B::WHITE);
				  }

				  auto p_skilltree_3 = p_ecinimic->getChildByName<Widget*>("panel_skilltree_3");
				  auto b_line_3 = p_skilltree_3->getChildByName<Button*>("button_line_1");
				  auto p_skill_3 = p_skilltree_3->getChildByName<Button*>("panel_skill_1");
				  auto b_skill_3 = p_skill_3->getChildByName<Button*>("button_skill_image");
				  auto i_lock_3 = b_skill_3->getChildByName<Button*>("image_lock");
				  auto t_skill_3_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_3, "label_skillname"));
				  auto t_skill_3_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_3, "text_item_skill_lv"));

				  auto p_skill_3_1 = p_skilltree_3->getChildByName<Button*>("panel_skill_1_1");
				  auto b_skill_3_1 = p_skill_3_1->getChildByName<Button*>("button_skill_image");
				  auto i_lock_3_1 = b_skill_3_1->getChildByName<Button*>("image_lock");
				  auto t_skill_3_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_3_1, "label_skillname"));
				  auto t_skill_3_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_3_1, "text_item_skill_lv"));

				  if (skill_3[0] == 0)
				  {
					  p_skilltree_3->setVisible(false);
				  }
				  else
				  {
					  p_skilltree_3->setVisible(true);
				  }

				  b_skill_3->ignoreContentAdaptWithSize(false);
				  b_skill_3->loadTextureNormal(getSkillIconPath(skill_3[0], SKILL_TYPE_PLAYER));
				  b_skill_3->setTag(skill_3[0]);
				  t_skill_3_name->setString(skill[skill_3[0]].name);
				  t_skill_3_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_3_lv, skill[skill_3[0]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_3[0]].require_level)
				  {
					  i_lock_3->setVisible(false);
				  }
				  else
				  {
					  i_lock_3->setVisible(true);
					  t_skill_3_lv->setVisible(false);
				  }

				  if (skill[skill_3[0]].points_per_level > m_nSkillPoints)
				  {
					  t_skill_3_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_3_lv->setColor(Color3B::WHITE);
				  }

				  b_skill_3_1->ignoreContentAdaptWithSize(false);
				  b_skill_3_1->loadTextureNormal(getSkillIconPath(skill_3[1], SKILL_TYPE_PLAYER));
				  b_skill_3_1->setTag(skill_3[1]);
				  t_skill_3_1_name->setString(skill[skill_3[1]].name);
				  t_skill_3_1_lv->setVisible(true);
				  setTextSizeAndOutline(t_skill_3_1_lv, skill[skill_3[1]].cur_skill_level);
				  if (SINGLE_HERO->m_iLevel >= skill[skill_3[1]].require_level &&
					  skill[skill[skill_3[1]].base_skill_id].cur_skill_level >= skill[skill_3[1]].base_skill_require_lv)
				  {
					  i_lock_3_1->setVisible(false);
					  b_line_3->setBright(true);
				  }
				  else
				  {
					  i_lock_3_1->setVisible(true);
					  t_skill_3_1_lv->setVisible(false);
					  b_line_3->setBright(false);
				  }

				  if (skill[skill[skill_3[1]].base_skill_id].points_per_level > m_nSkillPoints)
				  {
					  t_skill_3_1_lv->setColor(TEXT_RED_2);
				  }
				  else
				  {
					  t_skill_3_1_lv->setColor(Color3B::WHITE);
				  }
		}
		default:
		break;
		}
		
	}else
	{
		int skill_1[2] = {0,0};
		int skill_2[2] = {0,0};
		int skill_3[2] = {0,0};
		if (strcmp(m_pSkillTreeMinorButton->getName().data(),"button_skill1") == 0)
		{
			skill_1[0] = 19;
			skill_1[1] = 20;
			skill_2[0] = 21;
			skill_2[1] = 22;
			skill_3[0] = 23;
			skill_3[1] = 24;
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ECONOMIC_SKILL1"]);
		}else if (strcmp(m_pSkillTreeMinorButton->getName().data(),"button_skill2") == 0)
		{
			skill_1[0] = 25;
			skill_1[1] = 26;
			skill_2[0] = 0; //暂无
			skill_2[1] = 0;	//暂无
			skill_3[0] = 0;	//暂无
			skill_3[1] = 0; //暂无
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ECONOMIC_SKILL2"]);
		}else
		{
			skill_1[0] = 27;
			skill_1[1] = 28;
			skill_2[0] = 29;
			skill_2[1] = 30;
			skill_3[0] = 0; //暂无
			skill_3[1] = 0; //暂无
			t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ECONOMIC_SKILL3"]);
		}
		p_flghting->setVisible(false);
		p_ecinimic->setVisible(true);
		auto p_skilltree_1 = p_ecinimic->getChildByName<Widget*>("panel_skilltree_1");
		auto b_line_1 = p_skilltree_1->getChildByName<Button*>("button_line_1");
		auto p_skill_1 = p_skilltree_1->getChildByName<Button*>("panel_skill_1");
		auto b_skill_1 = p_skill_1->getChildByName<Button*>("button_skill_image");
		auto i_lock_1 = b_skill_1->getChildByName<Button*>("image_lock");
		auto t_skill_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1,"label_skillname"));
		auto t_skill_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1,"text_item_skill_lv"));

		auto p_skill_1_1 = p_skilltree_1->getChildByName<Button*>("panel_skill_1_1");
		auto b_skill_1_1 = p_skill_1_1->getChildByName<Button*>("button_skill_image");
		auto i_lock_1_1 = b_skill_1_1->getChildByName<Button*>("image_lock");
		auto t_skill_1_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1_1,"label_skillname"));
		auto t_skill_1_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_1_1,"text_item_skill_lv"));

		if (skill_1[0] == 0)
		{
			p_skilltree_1->setVisible(false);
		}else
		{
			p_skilltree_1->setVisible(true);
		}

		b_skill_1->ignoreContentAdaptWithSize(false);
		b_skill_1->loadTextureNormal(getSkillIconPath(skill_1[0], SKILL_TYPE_PLAYER));
		b_skill_1->setTag(skill_1[0]);
		t_skill_1_name->setString(skill[skill_1[0]].name);
		t_skill_1_lv->setVisible(true);
		setTextSizeAndOutline(t_skill_1_lv,skill[skill_1[0]].cur_skill_level);
		if (SINGLE_HERO->m_iLevel >= skill[skill_1[0]].require_level)
		{
			i_lock_1->setVisible(false);
		}else
		{
			i_lock_1->setVisible(true);
			t_skill_1_lv->setVisible(false);
		}

		if (skill[skill_1[0]].points_per_level > m_nSkillPoints)
		{
			t_skill_1_lv->setColor(TEXT_RED_2);
		}
		else
		{
			t_skill_1_lv->setColor(Color3B::WHITE);
		}

		b_skill_1_1->ignoreContentAdaptWithSize(false);
		b_skill_1_1->loadTextureNormal(getSkillIconPath(skill_1[1], SKILL_TYPE_PLAYER));
		b_skill_1_1->setTag(skill_1[1]);
		t_skill_1_1_name->setString(skill[skill_1[1]].name);
		t_skill_1_1_lv->setVisible(true);
		setTextSizeAndOutline(t_skill_1_1_lv, skill[skill_1[1]].cur_skill_level);
		if (SINGLE_HERO->m_iLevel >= skill[skill_1[1]].require_level && 
				skill[skill[skill_1[1]].base_skill_id].cur_skill_level >= skill[skill_1[1]].base_skill_require_lv)
		{
			i_lock_1_1->setVisible(false);
			b_line_1->setBright(true);
		}else
		{
			i_lock_1_1->setVisible(true);
			t_skill_1_1_lv->setVisible(false);
			b_line_1->setBright(false);
		}

		if (skill[skill[skill_1[1]].base_skill_id].points_per_level > m_nSkillPoints)
		{
			t_skill_1_1_lv->setColor(TEXT_RED_2);
		}
		else
		{
			t_skill_1_1_lv->setColor(Color3B::WHITE);
		}

		auto p_skilltree_2 = p_ecinimic->getChildByName<Widget*>("panel_skilltree_2");
		auto b_line_2 = p_skilltree_2->getChildByName<Button*>("button_line_1");
		auto p_skill_2 = p_skilltree_2->getChildByName<Button*>("panel_skill_1");
		auto b_skill_2 = p_skill_2->getChildByName<Button*>("button_skill_image");
		auto i_lock_2 = b_skill_2->getChildByName<Button*>("image_lock");
		auto t_skill_2_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2,"label_skillname"));
		auto t_skill_2_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2,"text_item_skill_lv"));

		auto p_skill_2_1 = p_skilltree_2->getChildByName<Button*>("panel_skill_1_1");
		auto b_skill_2_1 = p_skill_2_1->getChildByName<Button*>("button_skill_image");
		auto i_lock_2_1 = b_skill_2_1->getChildByName<Button*>("image_lock");
		auto t_skill_2_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2_1,"label_skillname"));
		auto t_skill_2_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_2_1, "text_item_skill_lv"));

		if (skill_2[0] == 0)
		{
			p_skilltree_2->setVisible(false);
		}else
		{
			p_skilltree_2->setVisible(true);
		}

		b_skill_2->ignoreContentAdaptWithSize(false);
		b_skill_2->loadTextureNormal(getSkillIconPath(skill_2[0], SKILL_TYPE_PLAYER));
		b_skill_2->setTag(skill_2[0]);
		t_skill_2_name->setString(skill[skill_2[0]].name);
		t_skill_2_lv->setVisible(true);
		setTextSizeAndOutline(t_skill_2_lv,skill[skill_2[0]].cur_skill_level);
		if (SINGLE_HERO->m_iLevel >= skill[skill_2[0]].require_level)
		{
			i_lock_2->setVisible(false);
		}else
		{
			i_lock_2->setVisible(true);
			t_skill_2_lv->setVisible(false);
		}

		if (skill[skill_2[0]].points_per_level > m_nSkillPoints)
		{
			t_skill_2_lv->setColor(TEXT_RED_2);
		}
		else
		{
			t_skill_2_lv->setColor(Color3B::WHITE);
		}

		b_skill_2_1->ignoreContentAdaptWithSize(false);
		b_skill_2_1->loadTextureNormal(getSkillIconPath(skill_2[1], SKILL_TYPE_PLAYER));
		b_skill_2_1->setTag(skill_2[1]);
		t_skill_2_1_name->setString(skill[skill_2[1]].name);
		t_skill_2_1_lv->setVisible(true);
		setTextSizeAndOutline(t_skill_2_1_lv, skill[skill_2[1]].cur_skill_level);
		if (SINGLE_HERO->m_iLevel >= skill[skill_2[1]].require_level &&
				skill[skill[skill_2[1]].base_skill_id].cur_skill_level >= skill[skill_2[1]].base_skill_require_lv)
		{
			i_lock_2_1->setVisible(false);
			b_line_2->setBright(true);
		}else
		{
			i_lock_2_1->setVisible(true);
			t_skill_2_1_lv->setVisible(false);
			b_line_2->setBright(false);
		}

		if (skill[skill[skill_2[1]].base_skill_id].points_per_level > m_nSkillPoints)
		{
			t_skill_2_1_lv->setColor(TEXT_RED_2);
		}
		else
		{
			t_skill_2_1_lv->setColor(Color3B::WHITE);
		}

		auto p_skilltree_3 = p_ecinimic->getChildByName<Widget*>("panel_skilltree_3");
		auto b_line_3 = p_skilltree_3->getChildByName<Button*>("button_line_1");
		auto p_skill_3 = p_skilltree_3->getChildByName<Button*>("panel_skill_1");
		auto b_skill_3 = p_skill_3->getChildByName<Button*>("button_skill_image");
		auto i_lock_3 = b_skill_3->getChildByName<Button*>("image_lock");
		auto t_skill_3_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_3,"label_skillname"));
		auto t_skill_3_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_3,"text_item_skill_lv"));

		auto p_skill_3_1 = p_skilltree_3->getChildByName<Button*>("panel_skill_1_1");
		auto b_skill_3_1 = p_skill_3_1->getChildByName<Button*>("button_skill_image");
		auto i_lock_3_1 = b_skill_3_1->getChildByName<Button*>("image_lock");
		auto t_skill_3_1_name = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_3_1,"label_skillname"));
		auto t_skill_3_1_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_skill_3_1,"text_item_skill_lv"));
		
		if (skill_3[0] == 0)
		{
			p_skilltree_3->setVisible(false);
		}else
		{
			p_skilltree_3->setVisible(true);
		}

		b_skill_3->ignoreContentAdaptWithSize(false);
		b_skill_3->loadTextureNormal(getSkillIconPath(skill_3[0], SKILL_TYPE_PLAYER));
		b_skill_3->setTag(skill_3[0]);
		t_skill_3_name->setString(skill[skill_3[0]].name);
		t_skill_3_lv->setVisible(true);
		setTextSizeAndOutline(t_skill_3_lv,skill[skill_3[0]].cur_skill_level);
		if (SINGLE_HERO->m_iLevel >= skill[skill_3[0]].require_level)
		{
			i_lock_3->setVisible(false);
		}else
		{
			i_lock_3->setVisible(true);
			t_skill_3_lv->setVisible(false);
		}

		if (skill[skill_3[0]].points_per_level > m_nSkillPoints)
		{
			t_skill_3_lv->setColor(TEXT_RED_2);
		}
		else
		{
			t_skill_3_lv->setColor(Color3B::WHITE);
		}

		b_skill_3_1->ignoreContentAdaptWithSize(false);
		b_skill_3_1->loadTextureNormal(getSkillIconPath(skill_3[1], SKILL_TYPE_PLAYER));
		b_skill_3_1->setTag(skill_3[1]);
		t_skill_3_1_name->setString(skill[skill_3[1]].name);
		t_skill_3_1_lv->setVisible(true);
		setTextSizeAndOutline(t_skill_3_1_lv, skill[skill_3[1]].cur_skill_level);
		if (SINGLE_HERO->m_iLevel >= skill[skill_3[1]].require_level && 
				skill[skill[skill_3[1]].base_skill_id].cur_skill_level >= skill[skill_3[1]].base_skill_require_lv)
		{
			i_lock_3_1->setVisible(false);
			b_line_3->setBright(true);
		}else
		{
			i_lock_3_1->setVisible(true);
			t_skill_3_1_lv->setVisible(false);
			b_line_3->setBright(false);
		}

		if (skill[skill[skill_3[1]].base_skill_id].points_per_level > m_nSkillPoints)
		{
			t_skill_3_1_lv->setColor(TEXT_RED_2);
		}
		else
		{
			t_skill_3_1_lv->setColor(Color3B::WHITE);
		}
	}
}

void UICenterCharactorSkill::changeMainButtonState(Widget *target)
{
	if (!target)
	{
		auto panel_skill = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_CSB]);
		target = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_skill,"button_fight"));
		auto t_fight = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_skill,"lable_fight"));
		auto t_economic = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_skill,"lable_economic"));
	}
	if (m_pSkillTreeMainButton)
	{
		m_pSkillTreeMainButton->setBright(true);
		m_pSkillTreeMainButton->setTouchEnabled(true);
		dynamic_cast<Text*>(m_pSkillTreeMainButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_NORMAL);
	}
	m_pSkillTreeMainButton = target;
	m_pSkillTreeMainButton->setBright(false);
	m_pSkillTreeMainButton->setTouchEnabled(false);
	dynamic_cast<Text*>(m_pSkillTreeMainButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_PASSED);


	auto panel_skill_main = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_CSB]);
	auto i_two_butter = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_skill_main, "listview_two_butter"));

	auto i_button_skill4 = dynamic_cast<Button*>(Helper::seekWidgetByName(i_two_butter, "button_skill4"));
	auto i_div_butter_5 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_two_butter, "image_div_butter_5"));
	if (strcmp(m_pSkillTreeMainButton->getName().data(), "button_fight") == 0)
	{
		i_button_skill4->setVisible(true);
		i_div_butter_5->setVisible(true);
	}
	else
	{
		i_button_skill4->setVisible(false);
		i_div_butter_5->setVisible(false);
	}
}

void UICenterCharactorSkill::changeMinorButtonState(Widget *target)
{
	if (!target)
	{
		auto panel_skill = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_CSB]);
		target = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_skill,"button_skill1"));
	}
	if (m_pSkillTreeMinorButton)
	{
		m_pSkillTreeMinorButton->setBright(true);
		m_pSkillTreeMinorButton->setTouchEnabled(true);
		dynamic_cast<Text*>(m_pSkillTreeMinorButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);	
	}
	m_pSkillTreeMinorButton = target;
	m_pSkillTreeMinorButton->setBright(false);
	m_pSkillTreeMinorButton->setTouchEnabled(false);
	dynamic_cast<Text*>(m_pSkillTreeMinorButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);	
	flushMinorButton();
}

void UICenterCharactorSkill::flushMinorButton()
{
	auto panel_skill = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_CSB]);
	auto t_skill1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_skill,"label_skill1"));
	auto t_skill2 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_skill,"label_skill2"));
	auto t_skill3 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_skill,"label_skill3"));
	auto t_skill4 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_skill, "label_skill4"));
	if (strcmp(m_pSkillTreeMainButton->getName().data(),"button_fight") == 0)
	{
		t_skill1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_FIGHT_SKILL1"]);
		t_skill2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_FIGHT_SKILL2"]);
		t_skill3->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_FIGHT_SKILL3"]);
		t_skill4->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_FIGHT_SKILL4"]);
	}else
	{
		t_skill1->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ECONOMIC_SKILL1"]);
		t_skill2->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ECONOMIC_SKILL2"]);
		t_skill3->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ECONOMIC_SKILL3"]);
	}
}

void UICenterCharactorSkill::openUpgradeView(const int nSkill_id)
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_VIEWSKILL_CSB]);
	auto skill_upGrade = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_VIEWSKILL_CSB]);
	auto t_skill_name = skill_upGrade->getChildByName<Text*>("label_repair");

	auto l_require = dynamic_cast<ListView*>(Helper::seekWidgetByName(skill_upGrade, "listview_require_0"));
	auto t_sort = dynamic_cast<Text*>(Helper::seekWidgetByName(skill_upGrade,"label_sort"));

	auto b_skill = skill_upGrade->getChildByName<Button*>("button_skill_bg");
	auto t_skill_lv = b_skill->getChildByName<Text*>("text_item_skill_lv");
	auto t_require_content = dynamic_cast<Text*>(Helper::seekWidgetByName(skill_upGrade,"label_content_1"));
	auto b_add = skill_upGrade->getChildByName<Button*>("button_add");
	auto b_add_0 = b_add->getChildByName<Button*>("button_add_0");

	auto t_cur_title = skill_upGrade->getChildByName<Text*>("label_cost");
	auto t_cur = skill_upGrade->getChildByName<Text*>("label_cost_1");
	auto t_next_title = skill_upGrade->getChildByName<Text*>("label_cost_0");
	auto t_next = skill_upGrade->getChildByName<Text*>("label_cost_0_0");
	auto t_cost_title = skill_upGrade->getChildByName<Text*>("label_cost_0_1");
	auto t_cost = skill_upGrade->getChildByName<Text*>("label_cost_0_0_0");

	auto image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(skill_upGrade, "image_head_16"));
	if (image_head)
	{
		image_head->ignoreContentAdaptWithSize(false);
		image_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));

	}
	auto &skill = SINGLE_SHOP->getSkillTrees()[nSkill_id];
	auto t_rquirelv = dynamic_cast<Text*>(Helper::seekWidgetByName(skill_upGrade, "label_require_lv"));
	auto t_Preskill =dynamic_cast<Text*>(Helper::seekWidgetByName(skill_upGrade, "label_player_lv"));
	auto i_lvBg =dynamic_cast<ImageView*>(Helper::seekWidgetByName(skill_upGrade, "image_lv_bg"));
	auto label_line = dynamic_cast<Text*>(Helper::seekWidgetByName(skill_upGrade, "label_line"));
	t_rquirelv->setString(StringUtils::format("Lv. %d", skill.require_level));
	label_line->setString(" |");
	label_line->setVisible(false);

	if (SINGLE_HERO->m_iLevel < skill.require_level)
	{
		t_rquirelv->setTextColor(Color4B::WHITE);
		i_lvBg->loadTexture("cocosstudio/login_ui/market_720/required_lv_bg_2.png");
	}

	t_skill_name->setString(skill.name);
	if (skill.base_skill_id)
	{
		label_line->setVisible(true);
		t_Preskill->setVisible(true);
		std::string st_skill = " ";
		st_skill += getSkillName(skill.base_skill_id, SKILL_TYPE_PLAYER);
		st_skill += StringUtils::format(" Lv. %d", skill.base_skill_require_lv);
		t_Preskill->setString(st_skill);
		if (SINGLE_SHOP->getSkillTrees()[skill.base_skill_id].cur_skill_level < skill.base_skill_require_lv)
		{
			t_Preskill->setTextColor(TEXT_RED);
		}
		else
		{
			t_Preskill->setTextColor(Color4B(56, 28, 1, 255));
		}
		
	}
	else
	{
		t_Preskill->setVisible(false);
	}
	
	
	if (skill.ap_type == 1)
	{
		t_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ACTIVE"]);
	}else
	{
		t_sort->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_PASSIVITY"]);
	}
	float number = 0;
	int cd = 0;
	float number1 = 0;
	switch (nSkill_id)
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
		{
			number = skill.effect_per_lv*skill.cur_skill_level/SKILL_DER_MIN;
			cd = skill.cd - skill.cdreduce_per_lv*skill.cur_skill_level;
			number1 = skill.effect_per_lv*(skill.cur_skill_level+1)/SKILL_DER_MIN;
			break;
		}
	case SKILL_HAMMER_SHOOT:
		{
			number = skill.duration_per_lv*skill.cur_skill_level;
			cd = skill.cd - skill.cdreduce_per_lv*skill.cur_skill_level;
			number1 = skill.duration_per_lv*(skill.cur_skill_level+1);
			break;
		}
	case SKILL_INDOMITABLE_WILL:
	case SKILL_ROAR_OF_VICTORY:
	case SKILL_INDUCE_TO_CAPITULATE:
		{
			number = skill.rate_per_lv*skill.cur_skill_level;
			cd = skill.cd - skill.cdreduce_per_lv*skill.cur_skill_level;
			number1 = skill.rate_per_lv*(skill.cur_skill_level+1);
			break;
		}
	case SKILL_HULL_TRACTION:
		{
			number = skill.effect_per_lv*skill.cur_skill_level;
			cd = skill.cd - skill.cdreduce_per_lv*skill.cur_skill_level;
			number1 = skill.effect_per_lv*(skill.cur_skill_level + 1);
			break;
		}
	default:
		{
			number = skill.effect_per_lv*skill.cur_skill_level / SKILL_DER_MAX;
			cd = skill.cd - skill.cdreduce_per_lv*skill.cur_skill_level;
			number1 = skill.effect_per_lv*(skill.cur_skill_level + 1) / SKILL_DER_MAX;
			break;
		}
	}
	
	std::string str = skill.desc;
	std::string old_value = "[number1]";
	std::string new_value1;
	std::string change = skill.next_desc;
	std::string next_change = change;
	std::string new_value2;
	std::string new_value3;
	if (nSkill_id == SKILL_TECHNIQUE_OF_BUSINESS || nSkill_id == SKILL_SHIPBUILDING_TECHNOLOGY || nSkill_id == SKILL_HULL_TRACTION)
	{
		new_value1 = StringUtils::format("%.0f",number);
		new_value2 = StringUtils::format("%.0f",number);
		new_value3 += StringUtils::format("%.0f",number1);
	}else
	{
		new_value1 = StringUtils::format("%.2f",number);
		new_value2 = StringUtils::format("%.2f",number);
		new_value3 = StringUtils::format("%.2f",number1);
	}
	repalce_all_ditinct(str,old_value,new_value1);
	repalce_all_ditinct(change,old_value,new_value2);
	repalce_all_ditinct(next_change,old_value,new_value3);
	old_value = "[cd]";
	new_value1 = StringUtils::format("%d",cd);
	repalce_all_ditinct(str,old_value,new_value1);

	t_require_content->setString(str);
	b_skill->ignoreContentAdaptWithSize(false);
	b_skill->loadTextureNormal(getSkillIconPath(nSkill_id, SKILL_TYPE_PLAYER));
	b_add->setTag(nSkill_id);
	t_cur_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_SKILL_UP_CURRENT_LV"]);
	t_cur->setString(change);
	t_cur->setPositionX(t_cur_title->getPositionX() + t_cur_title->getBoundingBox().size.width);

	std::string lv_str = StringUtils::format("%d/%d",skill.cur_skill_level,skill.max_level);
	t_skill_lv->setString(lv_str);
	t_skill_lv->enableOutline(Color4B::BLACK, 2);
	if (skill.max_level <= skill.cur_skill_level)
	{
		t_next_title->setVisible(false);
		t_next->setVisible(false);
		t_cost_title->setVisible(false);
		t_cost->setVisible(false);
	}
	else
	{
		t_next_title->setVisible(true);
		t_next->setVisible(true);
		t_cost_title->setVisible(true);
		t_cost->setVisible(true);
		t_next_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_REQUIP_NEXT_LV"]);
		t_next->setString(next_change);
		t_cost_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_SKILL_UP_COST_POINTS"]);
		t_cost->setString(StringUtils::format("%d %s", skill.points_per_level, SINGLE_SHOP->getTipsInfo()["TIP_CENTER_SKILL_UP_POINT"].data()));
		t_next->setPositionX(t_next_title->getPositionX() + t_next_title->getBoundingBox().size.width);
		t_cost->setPositionX(t_cost_title->getPositionX() + t_cost_title->getBoundingBox().size.width);
	}
	

	if (skill.max_level <= skill.cur_skill_level)
	{
		b_add->setVisible(false);
		b_add->setBright(false);
		b_add_0->setBright(false);
		t_cost->setTextColor(Color4B::BLACK);
		skill_upGrade->getChildByName<ImageView*>("image_div_2")->setVisible(false);
		
	}else if (SINGLE_HERO->m_iLevel < skill.require_level)
	{
		b_add->setBright(false);
		b_add_0->setBright(false);
		t_cost->setTextColor(Color4B::BLACK);
	}else if (skill.base_skill_id && SINGLE_SHOP->getSkillTrees()[skill.base_skill_id].cur_skill_level < skill.base_skill_require_lv)
	{
		b_add->setBright(false);
		b_add_0->setBright(false);
		t_cost->setTextColor(Color4B::BLACK);
	}else
	{
		b_add->setBright(true);
		b_add->setTouchEnabled(true);
		b_add_0->setBright(true);
		if (skill.points_per_level > m_nSkillPoints)
		{
			t_cost->setTextColor(TEXT_RED);
		}
		else
		{
			t_cost->setTextColor(Color4B::BLACK);
		}
	}

	l_require->refreshView();
}
