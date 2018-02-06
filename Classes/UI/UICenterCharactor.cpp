#include "UICenterCharactor.h"
#include "UICenterCharactorRole.h"
#include "UICenterCharactorEquip.h"
#include "UICenterCharactorSkill.h"
#include "UISocial.h"
#include "UICommon.h"

#include "UIVoyageSlider.h"
#include "UIInform.h"
#include "ModeLayerManger.h"
UICenterCharactor::UICenterCharactor():
	m_pRoleLayer(nullptr),
	m_pEquipLayer(nullptr),
	m_pSkillLayer(nullptr),
	m_pParent(nullptr),
	m_pMinorButton(nullptr)
{
	m_curPage = 0;
	m_curOperateItem = nullptr;
	m_curItemMaxNum = 0;
	m_pSlider = nullptr;
	m_pLabelNum = nullptr;
	m_taddProficiency = nullptr;
	m_loadingbar = nullptr;
	m_curPercent = 0;
	m_nUseItemNum = 0;
	t_proNum = nullptr; 
	m_curProficiency = 0;
	m_proficiencyResult = nullptr;
}

UICenterCharactor::~UICenterCharactor()
{
	CC_SAFE_RELEASE(m_pSkillLayer);
	CC_SAFE_RELEASE(m_pRoleLayer);
	CC_SAFE_RELEASE(m_pEquipLayer);
}

UICenterCharactor* UICenterCharactor::createCharactor(UIBasicLayer* parent)
{
	UICenterCharactor* cc = new UICenterCharactor;
	if (cc)
	{
		cc->m_pParent = parent;
		cc->init();
		cc->autorelease();
		return cc;
	}
	CC_SAFE_DELETE(cc);
	return nullptr;
}

bool UICenterCharactor::init()
{
	bool pRet = false;
	do 
	{
		CC_BREAK_IF(!UIBasicLayer::init());
		m_pRoleLayer = UICenterCharactorRole::createRole(m_pParent);
		m_pSkillLayer = UICenterCharactorSkill::createSkill(m_pParent);
		m_pEquipLayer = UICenterCharactorEquip::createEquip(m_pParent);
		CC_SAFE_RETAIN(m_pRoleLayer);
		CC_SAFE_RETAIN(m_pSkillLayer);
		CC_SAFE_RETAIN(m_pEquipLayer);
		pRet = true;
	} while (0);
	return pRet;
}

void UICenterCharactor::openCenterCharactor()
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_CSB]);
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_CSB]);
	auto btn_static = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_stastics"));
	auto btn_equip = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_equipment"));
	auto btn_skill = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_main_skill"));
	btn_static->setTag(1);
	btn_equip->setTag(2);
	btn_skill->setTag(3);
	m_pMinorButton = nullptr;
	scrollToView(nullptr);
	auto l_button = view->getChildByName<cocos2d::ui::ScrollView*>("listview_two_butter");
	l_button->addEventListener(CC_CALLBACK_2(UICenterCharactor::scrollButtonEvent, this));
	auto b_more = view->getChildByName<Button*>("button_more");
	b_more->setVisible(true);
}

void UICenterCharactor::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget* button = dynamic_cast<Widget*>(pSender);
	std::string name = button->getName();
	buttonEvent(button,name);
}

void UICenterCharactor::buttonEvent(Widget* target,std::string name)
{
	//个人信息按钮
	if (isButton(button_stastics))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->showChat(1);
		if (m_pEquipLayer->getFinishData() == false)
		{
			m_pEquipLayer->setFinishData(true);
			m_pEquipLayer->saveEquipHero();
		}
		scrollToView(target);
		return;
	}
	//装备信息按钮
	if (isButton(button_equipment))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->showChat(1);
		scrollToView(target);
		return;
	}
	//技能信息按钮
	if (isButton(button_main_skill))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->showChat(1);
		if (m_pEquipLayer->getFinishData() == false)
		{
			m_pEquipLayer->setFinishData(true);
			m_pEquipLayer->saveEquipHero();
		}
		UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(SKILL_NEW_POINT).c_str(), false);
		UserDefault::getInstance()->flush();
		target->getChildByName<ImageView*>("image_notify")->setVisible(false);
		scrollToView(target);
		return;
	}
	if (isButton(button_main_ship_proficiency))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->showChat(1);
		if (m_pEquipLayer->getFinishData() == false)
		{
			m_pEquipLayer->setFinishData(true);
			m_pEquipLayer->saveEquipHero();
		}
		scrollToView(target);
		m_curPage = PAGE_SHIP;
		ProtocolThread::GetInstance()->getProficiencyValues(UILoadingIndicator::create(m_pParent));
		return;
	}
	if (isButton(button_main_equip_proficiency))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UISocial::getInstance()->showChat(1);
		if (m_pEquipLayer->getFinishData() == false)
		{
			m_pEquipLayer->setFinishData(true);
			m_pEquipLayer->saveEquipHero();
		}
		scrollToView(target);
		m_curPage = PAGE_EQUIP;
		ProtocolThread::GetInstance()->getProficiencyValues(UILoadingIndicator::create(m_pParent));
		return;
	}
	if (isButton(button_learn))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_curOperateItem = target;
		int tag = target->getTag();
		if (target->isBright())
		{
			ProtocolThread::GetInstance()->getProficiencyBook(target->getTag());
		}
		else
		{
			UIInform::getInstance()->openInformView(m_pParent);
			UIInform::getInstance()->openViewAutoClose("TIP_PROFICIENCY_MAX_INFO");
		}

//		flushLearnProficiencyView(target);
		return;
	}
	if (isButton(button_cancel) && m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_LEARN_CSB]))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pParent->closeView();
		return;
	}
	if (isButton(button_price_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(m_pParent);
		UICommon::getInstance()->flushInfoBtnView("", "TIP_COUNTRY_PROFICIENCY_INFO");
		return;
	}
	if (isButton(button_good_bg))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto i_item = target->getChildByName("image_goods");
		int tag = i_item->getTag();
		UICommon::getInstance()->openCommonView(m_pParent);
		UICommon::getInstance()->flushItemsDetail(nullptr,tag,true);
		return;
	}
	if (isButton(button_plus))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_nUseItemNum >= m_curItemMaxNum)
		{
			return;
		}
		m_nUseItemNum++;
		flushLearnView();
		return;
	}
	if (isButton(button_minus))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_nUseItemNum <= 1)
		{
			return;
		}
		m_nUseItemNum--;
		flushLearnView();
		return;
	}
	if (isButton(button_ok) && m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_LEARN_CSB]))
	{
		if (m_nUseItemNum < 1)
		{
			return;
		}
		ProtocolThread::GetInstance()->addProficiency(m_operaInfo.pro.id, m_nUseItemNum, UILoadingIndicator::create(m_pParent));
		return;
	}
	int curPageIndex = m_pMinorButton->getTag();
	switch (curPageIndex)
	{
	case PAGE_ROLE://role info
		m_pRoleLayer->buttonEvent(target,name);
		break;
	case PAGE_EQUIP://equip
		m_pEquipLayer->buttonEvent(target,name);
		break;
	case PAGE_SKILL://skill
		m_pSkillLayer->buttonEvent(target,name);
		break;
	default:
		break;
	}	
}

void UICenterCharactor::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	switch (m_pMinorButton->getTag())
	{
	case PAGE_ROLE:
		m_pRoleLayer->onServerEvent(message,msgType);
		break;
	case PAGE_EQUIP:
		m_pEquipLayer->onServerEvent(message,msgType);
		break;
	case PAGE_SKILL:
		m_pSkillLayer->onServerEvent(message,msgType);
		break;
	default:
		break;
	}
	switch (msgType)
	{
	case PROTO_TYPE_GetProficiencyValuesResult:
	{
											GetProficiencyValuesResult * result = (GetProficiencyValuesResult *)message;
											if (result->failed == 0)
											{
												auto instance = UICommon::getInstance();
												auto views = instance->getRoots();
												if (views.size() > 0)
												{
													return;
												}
												m_proficiencyResult = result;
												flushProficiencyView(m_curPage);
											}
	}
		break;
	case PROTO_TYPE_GetProficiencyBookResult:
	{
												GetProficiencyBookResult * result = (GetProficiencyBookResult *)message;
												if (result->failed == 0)
												{
													flushLearnProficiencyView(m_curOperateItem,result);
												}
	}
		break;
	case PROTO_TYPE_AddProficiencyResult:
	{
											AddProficiencyResult * result = (AddProficiencyResult *)message;
											if (result->failed == 0)
											{
												m_pParent->closeView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_LEARN_CSB]);
												m_nUseItemNum = 0;
												UIInform::getInstance()->openInformView(m_pParent);
												UIInform::getInstance()->openViewAutoClose("TIP_COUNTRY_PROFICIENCY_LEARN_SUCCESS");
												ProtocolThread::GetInstance()->getProficiencyValues(UILoadingIndicator::create(m_pParent));
											}
	}
	default:
		break;
	}
}

void UICenterCharactor::scrollToView(Widget* target)
{
	if (!target)
	{
		auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_CSB]);
		target = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_stastics"));	
	}else
	{
		auto instance = ModeLayerManger::getInstance();
		auto layer = instance->getModelLayer();
		if (layer != nullptr)
		{
			instance->removeLayer();
		}
		//保留最底层的csb
		m_pParent->closeView(); 
	}

	if (m_pMinorButton)
	{
		m_pMinorButton->setBright(true);
		dynamic_cast<Text*>(m_pMinorButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		m_pMinorButton->setTouchEnabled(true);
	}
	m_pMinorButton = target;
	m_pMinorButton->setBright(false);
	dynamic_cast<Text*>(m_pMinorButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
	m_pMinorButton->setTouchEnabled(false);
	
	switch (m_pMinorButton->getTag())
	{
	case PAGE_ROLE:
		m_pRoleLayer->openCenterRole();
		break;
	case PAGE_EQUIP:
		m_pEquipLayer->setAddCharacterEnable();
		m_pEquipLayer->openCenterEquip();
		break;
	case PAGE_SKILL:
		m_pSkillLayer->openCenterSkill();
		break;
	default:
		break;
	}
}

UICenterCharactorEquip* UICenterCharactor::getCenterEquip()
{
	return m_pEquipLayer;
}
void UICenterCharactor::flushProficiencyView(int page)
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_CSB]);
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_CSB]);
	auto l_content = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_content"));
	auto i_pulldown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown"));
	l_content->removeAllChildrenWithCleanup(true);
	auto item = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "image_log_1"));
	getProficiencyInfo(m_proficiencyResult);
	auto tipInfo = SINGLE_SHOP->getTipsInfo();
	if (page == PAGE_SHIP && m_shipProficiency.size() > 0)
	{
		int index = 0;
		for (auto pro :m_shipProficiency)
		{
			auto c_item = item->clone();
			auto t_proname = dynamic_cast<Text*>(Helper::seekWidgetByName(c_item, "label_shipp_name"));
			auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(c_item, "label_lv"));
			auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_item, "image_ic"));
			auto t_shippnum = dynamic_cast<Text*>(Helper::seekWidgetByName(c_item, "label_shipp_num"));
			auto p_shippbar = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(c_item, "progressbar_shipp"));
			auto b_learn = dynamic_cast<Button*>(Helper::seekWidgetByName(c_item, "button_learn"));
			t_proname->setString(pro.pro.name);
			t_lv->setString(StringUtils::format("Lv.%d", getProficiencyLevel(pro.proValue)));
			c_item->setTag(index);
			std::string path = StringUtils::format("cocosstudio/login_ui/player_720/pro_%d.png", pro.pro.icon_id);
			i_icon->loadTexture(path);			
			t_shippnum->setString(numSegment(StringUtils::format("%d", getCurlevelProficiency(pro.proValue))) + "/" + numSegment(StringUtils::format("%d", getLevelProficiency(getProficiencyLevel(pro.proValue)))));
			float percent = ((float)(getCurlevelProficiency(pro.proValue)) / getLevelProficiency(getProficiencyLevel(pro.proValue))) * 100;
			b_learn->setTag(pro.pro.id);
			p_shippbar->setPercent(percent);
			l_content->pushBackCustomItem(c_item);
			if (pro.proValue >= 7300)
			{
				p_shippbar->setPercent(100);
				t_shippnum->setString(tipInfo["TIP_PROFICIENCY_MAX"]);
				b_learn->setBright(false);
			}	
			index++;
		}
	}else if (page == PAGE_EQUIPMENT && m_equimrntProficiency.size() > 0)
	{
		int index = 0;
		for (auto pro : m_equimrntProficiency)
		{
			auto c_item = item->clone();
			auto t_proname = dynamic_cast<Text*>(Helper::seekWidgetByName(c_item, "label_shipp_name"));
			auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(c_item, "label_lv"));
			auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(c_item, "image_ic"));
			auto t_shippnum = dynamic_cast<Text*>(Helper::seekWidgetByName(c_item, "label_shipp_num"));
			auto p_shippbar = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(c_item, "progressbar_shipp"));
			auto b_learn = dynamic_cast<Button*>(Helper::seekWidgetByName(c_item, "button_learn"));
			t_proname->setString(pro.pro.name);
			t_lv->setString(StringUtils::format("Lv. %d", getProficiencyLevel(pro.proValue)));
			c_item->setTag(index);
			std::string path = StringUtils::format("cocosstudio/login_ui/player_720/pro_%d.png", pro.pro.icon_id);
			i_icon->loadTexture(path);
			t_shippnum->setString(numSegment(StringUtils::format("%d", getCurlevelProficiency(pro.proValue))) + "/" + numSegment(StringUtils::format("%d", getLevelProficiency(getProficiencyLevel(pro.proValue)))));
			float percent = ((float)(getCurlevelProficiency(pro.proValue)) / getLevelProficiency(getProficiencyLevel(pro.proValue))) * 100;
			b_learn->setTag(pro.pro.id);
			p_shippbar->setPercent(percent);
			l_content->pushBackCustomItem(c_item);
			if (pro.proValue >= 7300)
			{
				p_shippbar->setPercent(100);
				t_shippnum->setString(tipInfo["TIP_PROFICIENCY_MAX"]);
				b_learn->setBright(false);			
			}
			index++;
		}
	}
	int tag = i_pulldown->getTag();
	if (tag != 1001)
	{
		auto b_pulldown = i_pulldown->getChildByName("button_pulldown");
		b_pulldown->setPositionX(b_pulldown->getPositionX() - b_pulldown->getContentSize().width / 2);
		addListViewBar(l_content, i_pulldown);
		i_pulldown->setTag(1001);
	}
}
void UICenterCharactor::flushLearnProficiencyView(Widget * p_sender, GetProficiencyBookResult * bookInfo)
{
	int tag = p_sender->getParent()->getTag();
	auto itemInfo = SINGLE_SHOP->getItemData();
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_LEARN_CSB]);
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_LEARN_CSB]);
	auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_lv"));
	auto t_pname = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_shipp_name"));
	auto t_shippnum = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_shipp_num"));
	auto t_iname = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_name"));
	auto t_add = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_add"));
	auto p_shipp = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view, "progressbar_shipp"));
	auto i_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_goods"));
	auto s_goods = dynamic_cast<Slider*>(Helper::seekWidgetByName(view, "slider_goods_num"));
	auto i_pShip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_progressbar_shipp"));
	auto t_goodsNum = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_goods_num"));
	auto i_select = Helper::seekWidgetByName(view, "image_selectnum_bg");
	auto b_info = Helper::seekWidgetByName(view, "button_price_info");
	auto b_item = Helper::seekWidgetByName(view, "button_good_bg");
	auto b_ok = Helper::seekWidgetByName(view, "button_ok");
	UIVoyageSlider* ss1 = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1001));
	if (ss1)
	{
		ss1->removeFromParentAndCleanup(true);
	}
	m_pSlider = s_goods;
	m_pLabelNum = t_goodsNum;
	m_loadingbar = p_shipp;
	m_taddProficiency = t_add;
	t_proNum = t_shippnum;
	m_curLevel = t_lv;
	PROFICIENCY proficiencyInfo;
	if (m_curPage == PAGE_SHIP)
	{
		proficiencyInfo = m_shipProficiency.at(tag);
		m_operaInfo = proficiencyInfo;
		auto item = itemInfo[proficiencyInfo.pro.item_need];
		i_item->loadTexture(getItemIconPath(item.id));
		setBgButtonFormIdAndType(b_item, item.id, ITEM_TYPE_PORP);
		t_iname->setString(item.name);
		t_pname->setString(proficiencyInfo.pro.name);
		i_item->setTag(item.id);
		t_lv->setString(StringUtils::format("Lv.%d", getProficiencyLevel(proficiencyInfo.proValue)));
		t_shippnum->setString(numSegment(StringUtils::format("%d", getCurlevelProficiency(proficiencyInfo.proValue))) + "/" + numSegment(StringUtils::format("%d", getLevelProficiency(getProficiencyLevel(proficiencyInfo.proValue)))));
		float percent = ((float)(getCurlevelProficiency(proficiencyInfo.proValue)) / getLevelProficiency(getProficiencyLevel(proficiencyInfo.proValue))) * 100;
		p_shipp->setPercent(percent);

	}else if (m_curPage == PAGE_EQUIPMENT)
	{
		proficiencyInfo = m_equimrntProficiency.at(tag);
		auto item = itemInfo[proficiencyInfo.pro.item_need];
		m_operaInfo = proficiencyInfo;
		i_item->loadTexture(getItemIconPath(item.id));
		setBgButtonFormIdAndType(b_item, item.id, ITEM_TYPE_PORP);
		t_iname->setString(item.name);
		t_pname->setString(proficiencyInfo.pro.name);
		i_item->setTag(item.id);
		t_lv->setString(StringUtils::format("Lv.%d", getProficiencyLevel(proficiencyInfo.proValue)));
		t_shippnum->setString(numSegment(StringUtils::format("%d", getCurlevelProficiency(proficiencyInfo.proValue))) + "/" + numSegment(StringUtils::format("%d", getLevelProficiency(getProficiencyLevel(proficiencyInfo.proValue)))));
		float percent = ((float)(getCurlevelProficiency(proficiencyInfo.proValue)) / getLevelProficiency(getProficiencyLevel(proficiencyInfo.proValue))) * 100;
		p_shipp->setPercent(percent);
	}
	int nMaxCount = 0;
	m_curProficiency = proficiencyInfo.proValue;
	if (bookInfo->count == 0)
	{
		m_nUseItemNum = 0;
		m_pSlider->setTouchEnabled(false);
		m_pSlider->setPercent(0);
		t_goodsNum->setString("0 / 0");
		t_add->setString("+0");
		nMaxCount = 0;
	}
	else if (bookInfo->count > 0)
	{
		m_nUseItemNum = 1;
		m_pSlider->setTouchEnabled(true);
		m_pSlider->setPercent((1.0 * 100 / m_curItemMaxNum));
		t_add->setString(StringUtils::format("+ %d", 1));
		if (bookInfo->count + proficiencyInfo.proValue > 7300)
		{
			nMaxCount = 7300 - proficiencyInfo.proValue;
		}
		else
		{
			nMaxCount = bookInfo->count;
		}
	}
	m_curItemMaxNum = nMaxCount;
	auto ss = UIVoyageSlider::create(i_select, nMaxCount, 0, true);
	if (nMaxCount > 0)
	{
		ss->setCurrentNumber(1);
		ss->setMaxPercent(nMaxCount);
	}
	else
	{
		ss->setCurrentNumber(0);
	}
	ss->setTag(1001);
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UICenterCharactor::updateForNumber, this));
	this->addChild(ss);
	b_info->addTouchEventListener(CC_CALLBACK_2(UICenterCharactor::menuCall_func, this));
	b_info->setTouchEnabled(true);
	b_item->setTouchEnabled(true);
	b_item->addTouchEventListener(CC_CALLBACK_2(UICenterCharactor::menuCall_func, this));
	flushLearnView();
	if (bookInfo->count <= 0)
	{
		b_ok->setBright(false);
		b_ok->setTouchEnabled(false);
	}
}
void UICenterCharactor::getProficiencyInfo(GetProficiencyValuesResult * result)
{
	auto proficiencyInfo = SINGLE_SHOP->getProficiencyInfo();
	m_shipProficiency.clear();
	m_equimrntProficiency.clear();
	for (int i = 0; i < result->n_values; i++)
	{
		auto id = result->values[i]->proficiencyid;
		auto proficiency = proficiencyInfo[id];
		//船只
		if (proficiency.type == 1)
		{
			PROFICIENCY define;
			define.pro = proficiencyInfo[id];
			define.proValue = result->values[i]->value;
			m_shipProficiency.push_back(define);
		}
		//装备
		else if (proficiency.type == 2)
		{
			PROFICIENCY define;
			define.pro = proficiencyInfo[id];
			define.proValue = result->values[i]->value;
			m_equimrntProficiency.push_back(define);
		}
	}
}

int UICenterCharactor::getProficiencyLevel(int value)
{
	int level = 0;
	if ( value < 100)
	{
		level = 0;
	}
	else if ( value < 300)
	{
		level =  1;
	}else if (value < 800)
	{
		level = 2;
	}else if (value < 2300)
	{
		level = 3;
	}
	else if (value < 7300)
	{
		level = 4;
	}else if (value >= 7300)
	{
		level = 5;
	}
	return level;
}
int UICenterCharactor::getLevelProficiency(int level)
{
	int value = 0;
	if (level == 0)
	{
		value = 100;
	}else if (level == 1)
	{
		value = 200;
	}
	else if (level == 2)
	{
		value = 500;
	}
	else if (level == 3)
	{
		value = 1500;
	}
	else if (level == 4)
	{
		value = 5000;
	}
	else if (level == 5)
	{
		value = 5000;
	}
	return value;
}
/**
*当前等级获取的经验
*/
int UICenterCharactor::getCurlevelProficiency(int value)
{
	int p_value = 0;
	int level = getProficiencyLevel(value);
	switch (level)
	{
	case 5:
		p_value += 5000;
	case 4:
		p_value += 1500;
	case 3:
		p_value += 500;
	case 2:
		p_value += 200;
	case 1:
		p_value += 100;
	case 0:
	default:
		break;
	}	
	return  value - p_value;
}
void UICenterCharactor::usingItemsSliderEvent(Ref* obj, cocos2d::ui::Slider::EventType type)
{
	if (type != Slider::EventType::ON_PERCENTAGE_CHANGED)
	{
		return;
	}

	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_LEARN_CSB]);
	auto t_goodsNum = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_goods_num"));

	auto slider_dropitem = dynamic_cast<Slider*>(obj);
	m_nUseItemNum = m_curItemMaxNum * slider_dropitem->getPercent() / 100.0;
	std::string str;
	if (m_nUseItemNum <= 1)//最少数量为1
	{
		str = "1 /";
		m_nUseItemNum = 1;
	}
	else
	{
		str = StringUtils::format("%d /", m_nUseItemNum);
	}
	t_goodsNum->setString(str + StringUtils::format(" %d", m_curItemMaxNum));
	m_taddProficiency->setString(StringUtils::format("+ %d", m_nUseItemNum));
	m_loadingbar->setPercent(m_curPercent + 100.0 * m_nUseItemNum / getLevelProficiency(getProficiencyLevel(m_operaInfo.proValue)));
	t_proNum->setString(numSegment(StringUtils::format("%d", getCurlevelProficiency(m_operaInfo.proValue)+m_nUseItemNum)) + "/" + numSegment(StringUtils::format("%d", getLevelProficiency(getProficiencyLevel(m_operaInfo.proValue)))));
}

void UICenterCharactor::flushLearnView()
{
	m_pLabelNum->setString(StringUtils::format("%d/", m_nUseItemNum) + StringUtils::format("%d", m_curItemMaxNum));
	m_taddProficiency->setString("+ " + numSegment(StringUtils::format("%d", m_nUseItemNum)));
	t_proNum->setString(numSegment(StringUtils::format("%d", getCurlevelProficiency(m_operaInfo.proValue + m_nUseItemNum))) + "/" + numSegment(StringUtils::format("%d", getLevelProficiency(getProficiencyLevel(m_curProficiency + m_nUseItemNum)))));
	m_curLevel->setString(StringUtils::format("Lv.%d", getProficiencyLevel(m_curProficiency + m_nUseItemNum)));
	if (m_curProficiency + m_nUseItemNum >= 7300)
	{
		auto str = SINGLE_SHOP->getTipsInfo()["TIP_PROFICIENCY_MAX"];
		t_proNum->setString("Max");
		m_loadingbar->setPercent(100);
	}
	else
	{
		int n_curP = getCurlevelProficiency(m_curProficiency + m_nUseItemNum);
		int n_leveP = getLevelProficiency(getProficiencyLevel(m_curProficiency + m_nUseItemNum));
		m_loadingbar->setPercent(100.0 * n_curP / n_leveP);
	}
}
void UICenterCharactor::updateForNumber(int num)
{
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_LEARN_CSB]);
	auto t_goodsNum = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_goods_num"));
	std::string str;
	m_nUseItemNum = num;
// 	str = StringUtils::format("%d /", m_nUseItemNum);	
// 	t_goodsNum->setString(str + StringUtils::format(" %d", m_curItemMaxNum));
// 	m_taddProficiency->setString(StringUtils::format("+ %d", m_nUseItemNum));
// 	m_loadingbar->setPercent(m_curPercent + 100.0 * m_nUseItemNum / getLevelProficiency(getProficiencyLevel(m_operaInfo.proValue)));
// 	t_proNum->setString(numSegment(StringUtils::format("%d", getCurlevelProficiency(m_operaInfo.proValue) + m_nUseItemNum)) + "/" + numSegment(StringUtils::format("%d", getLevelProficiency(getProficiencyLevel(m_operaInfo.proValue)))));
	flushLearnView();
}

void UICenterCharactor::scrollButtonEvent(Ref *pSender, cocos2d::ui::ScrollView::EventType type)
{
	if (type == cocos2d::ui::ScrollView::EventType::SCROLL_TO_TOP)
	{
		auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_CSB]);
		auto b_more = view->getChildByName<Button*>("button_more");
		b_more->setBright(true);
	}
	else if (type == cocos2d::ui::ScrollView::EventType::SCROLL_TO_BOTTOM)
	{
		auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_CSB]);
		auto b_more = view->getChildByName<Button*>("button_more");
		b_more->setBright(false);
	}
}
