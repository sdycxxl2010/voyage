#include "UIGuideCenterCharactorSkill.h"
#include "UINoviceStoryLine.h"
#include "UICenterCharactor.h"
#include "UICenterLocal.h"
#include "UICenterItem.h"
#include "UICenterCharactorEquip.h"
#include "UIInform.h"
#include "UICenterCharactorSkill.h"
#include "Utils.h"

UIGuideCenterCharactorSkill::UIGuideCenterCharactorSkill()
:m_pLocalLayer(nullptr)
, m_pItemLayer(nullptr)
, m_pCurMainButton(nullptr)
, m_pCharaLayer(nullptr)
, sprite_hand(nullptr)
, t_size(0)
, m_chatnum(0)
, m_cursay(false)
, m_chatcomplete(false)
, m_contentLength(0)
, m_allChatNum(0)
, m_asideBg(nullptr)
, m_aside_chat(nullptr)
, m_guideStage(0)
, m_skillpointnum(2)
, m_dialogLayer(nullptr)
{
	m_eUIType = UI_CENTER;	
}

UIGuideCenterCharactorSkill::~UIGuideCenterCharactorSkill()
{
	CC_SAFE_RELEASE(m_pLocalLayer);
	CC_SAFE_RELEASE(m_pCharaLayer);
	CC_SAFE_RELEASE(m_pItemLayer);
	unregisterCallBack();
	m_names.clear();
	m_dialogdatas.clear();
	talks.clear();
	m_words.clear();

}

UIGuideCenterCharactorSkill* UIGuideCenterCharactorSkill::createCenter()
{
	UIGuideCenterCharactorSkill* cl = new UIGuideCenterCharactorSkill;
	if (cl && cl->init())
	{
		cl->autorelease();
		return cl;
	}
	CC_SAFE_DELETE(cl);
	return nullptr;
}

bool UIGuideCenterCharactorSkill::init()
{
	bool pRet = false;
	do
	{

		CC_BREAK_IF(!UIBasicLayer::init());
		registerCallBack();

		openView(PLAYER_COCOS_RES[PLAYER_CSB]);
		auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_CSB]);
		
		auto t_charactor = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_charactor"));
		auto t_item = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_items"));
		auto t_warehouse = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_warehouse"));

		m_pCharaLayer = UICenterCharactor::createCharactor(this);
		m_pLocalLayer = UICenterLocal::createLocal(this);
		m_pItemLayer = UICenterItem::createItem(this);
		CC_SAFE_RETAIN(m_pCharaLayer);
		CC_SAFE_RETAIN(m_pLocalLayer);
		CC_SAFE_RETAIN(m_pItemLayer);
		scrollToView(nullptr);
		pRet = true;
	} while (0);
	sprite_hand = Sprite::create();
	sprite_hand->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
	sprite_hand->setVisible(false);
	this->addChild(sprite_hand,10);

	readJsonFile();
	
	return pRet;
}

void UIGuideCenterCharactorSkill::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* button = dynamic_cast<Widget*>(pSender);
		std::string name = button->getName();
		buttonEvent(button, name);
		return;
	}
}

void UIGuideCenterCharactorSkill::buttonEvent(Widget* button, std::string name)
{
	if (isButton(button_back))//返回主城
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_pCharaLayer->getCenterEquip()->getFinishData() == false)
		{
			m_pCharaLayer->getCenterEquip()->setFinishData(true);
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_CENTER_FINSIH_EQUIP_DATA");
			return;
		}
		button_callBack();
		return;
	}
	//装备信息
	if (name.compare("button_role") == 0 || name.compare("button_items") == 0 || name.compare("button_localitems") == 0)//角色信息 背包 本地仓库按钮点击事件
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto scene = _director->getRunningScene();
		if (scene->getChildByName("characterlayer"))
		{
			auto layer = scene->getChildByName("characterlayer");
			layer->removeFromParentAndCleanup(true);
		}
		if (m_pCharaLayer->getCenterEquip()->getFinishData() == false)
		{
			m_pCharaLayer->getCenterEquip()->setFinishData(true);
			m_pCharaLayer->getCenterEquip()->saveEquipHero();
		}
		scrollToView(button);
		return;
	}

	distributeEvent(button, name);
}

void UIGuideCenterCharactorSkill::distributeEvent(Widget* target, std::string name)
{
	int curPageIndex = m_pCurMainButton->getTag();
	switch (curPageIndex)
	{
	case PAGE_CHARACTOR://个人信息界面
	{
						   m_pCharaLayer->buttonEvent(target, name);
						   break;
	}
	case PAGE_ITEM://背包界面
	{
					  if (m_pCharaLayer->getCenterEquip()->getFinishData() == false)
					  {
						  SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
						  m_pCharaLayer->getCenterEquip()->setFinishData(true);
						  UIInform::getInstance()->openInformView(this);
						  UIInform::getInstance()->openConfirmYes("TIP_CENTER_FINSIH_EQUIP_DATA");
						  return;
					  }
					  m_pItemLayer->buttonEvent(target, name);
					  break;
	}
	case PAGE_LOCAL: //背面仓库界面
	{
					   if (m_pCharaLayer->getCenterEquip()->getFinishData() == false)
					   {
						   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
						   m_pCharaLayer->getCenterEquip()->setFinishData(true);
						   UIInform::getInstance()->openInformView(this);
						   UIInform::getInstance()->openConfirmYes("TIP_CENTER_FINSIH_EQUIP_DATA");
						   return;
					   }
					   m_pLocalLayer->buttonEvent(target, name);
					   break;
	}
	default:
		break;
	}
}

void UIGuideCenterCharactorSkill::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	if (msgType == PROTO_TYPE_SetMainTaskStatusResult)
	{
		 for (auto view : m_vRoots)
		 {
			 setButtonsTouch(view, true);

		 }
		
		 UserDefault::getInstance()->setIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str(), 9);

	}
	if (msgType == PROTO_TYPE_SetMainTaskStatus)
	{
		log("hehe");
	}
	switch (m_pCurMainButton->getTag())
	{
	case PAGE_CHARACTOR:
		m_pCharaLayer->onServerEvent(message, msgType);
		guide();
		break;
	case PAGE_ITEM:
		m_pItemLayer->onServerEvent(message, msgType);
		break;
	case PAGE_LOCAL:
		m_pLocalLayer->onServerEvent(message, msgType);
		break;
	default:
		break;
	}
}

void UIGuideCenterCharactorSkill::scrollToView(Widget* target)
{
	if (!target)
	{
		auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_CSB]);
		target = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_role"));
	}

	if (m_pCurMainButton)
	{
		m_pCurMainButton->setBright(true);
		dynamic_cast<Text*>(m_pCurMainButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_NORMAL);
		m_pCurMainButton->setTouchEnabled(true);
	}
	m_pCurMainButton = target;
	m_pCurMainButton->setBright(false);
	dynamic_cast<Text*>(m_pCurMainButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_PASSED);
	m_pCurMainButton->setTouchEnabled(false);

	//保留最底层的csb
	int num = m_vFilePath.size();
	for (int i = 1; i < num; i++)
	{
		closeView();
	}

	switch (m_pCurMainButton->getTag())
	{
	case PAGE_CHARACTOR:
		m_pCharaLayer->openCenterCharactor();
		break;
	case PAGE_ITEM:
		m_pItemLayer->openCreateItem();
		break;
	case PAGE_LOCAL:
		m_pLocalLayer->openCenterLocal();
		break;
	default:
		break;
	}
}

void UIGuideCenterCharactorSkill::guide()
{


	m_guideStage++;
	switch (m_guideStage)
	{
	case FOCUS_SKILL_PAGE:
	{ 
							 for (auto view : m_vRoots)
							 {
								 setButtonsTouch(view,false);
							 }
							 auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_STASTICS_CSB]);
							 auto l_btnlist = view->getChildByName<ListView*>("listview_infomation");
							 l_btnlist->setDirection(SCROLLVIEW_DIR_NONE);
							 l_btnlist = getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_CSB])->getChildByName<ListView*>("listview_two_butter");
							 focusOnButton(l_btnlist->getItem(5));
							 dynamic_cast<Button*>(l_btnlist->getItem(5))->setTouchEnabled(true);

							 initGuideInfo();

	}
		break;
	case START_DIALOG:
	{
   
						 for (auto view : m_vRoots)
						 {
							 setButtonsTouch(view,false);
						 }
						 sprite_hand->setVisible(false);
						 auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_CSB]);
						 auto t_skillnum = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "text_skillnum"));
						 t_skillnum->setText(StringUtils::format("%d", m_skillpointnum));
						 m_dialogLayer = UINoviceStoryLine::GetInstance();
						 m_dialogLayer->onGuideTaskMessage(UINoviceStoryLine::CENTER_TWO_DIALOG, 0);
						 this->addChild(m_dialogLayer, 10000);
	}
		break;
	case FOCUS_SKILL_TREE:
	{
							 m_dialogLayer->removeFromParentAndCleanup(true);
							 for (auto view : m_vRoots)
							 {
								 setButtonsTouch(view,false);
							 }
							 auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_CSB]);
							 auto btn_skilltree = view->getChildByName<Button*>("button_skilltree");
							 btn_skilltree->setTouchEnabled(true);
							 auto scene = _director->getRunningScene();
							 UICenterCharactorSkill* layer = dynamic_cast<UICenterCharactorSkill*>(m_pCharaLayer->getSkillLayer());
							 SINGLE_HERO->m_iLevel = 3;
							 if (layer != nullptr)
							 {
								 layer->setSkillPoints(m_skillpointnum);
							 }
							 focusOnButton(btn_skilltree);

	}
		break;
	case SKILL_TREE_DIALOG:
	{
							  for (auto view : m_vRoots)
							  {
								  setButtonsTouch(view,false);
							  }
							  sprite_hand->setVisible(false);
							  m_dialogLayer = UINoviceStoryLine::GetInstance();
							  m_dialogLayer->onGuideTaskMessage(UINoviceStoryLine::CENTER_THREE_DIALOG, 0);
							  this->addChild(m_dialogLayer, 10000);
							  auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SKILL_TREE_CSB]);
							  auto skillnum = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_combat_0"));
							  skillnum->setString(StringUtils::format("%d", m_skillpointnum));
	}
		break;
	case SKILL_GUIDE_COMPLETE:
	{
							ProtocolThread::GetInstance()->setMainTaskStatus(nullptr,9);						
	}
		break;
	default:
		break;
	}


}

void UIGuideCenterCharactorSkill::openAsideLayer(Node * parent)
{
	this->openView("voyageUI_city.csb");
	auto view = getViewRoot("voyageUI_city.csb");
	auto guard_dialog = view->getChildByName<Widget*>("panel_dialog");
	guard_dialog->setPosition(STARTPOS);
 	guard_dialog->setTouchEnabled(true);
 	guard_dialog->addTouchEventListener(CC_CALLBACK_2(UIGuideCenterCharactorSkill::asideTouchEvent,this));
 	guard_dialog->setLocalZOrder(10);
 	auto p_city = view->getChildByName("image_main_city_bg");
 	p_city->setVisible(false);
	auto i_soldier = guard_dialog->getChildByName("image_soldier");
 	i_soldier->setVisible(false);

	guard_dialog->setTouchEnabled(true);
	sprite_hand->setVisible(false);
	auto i_dialog_bg = view->getChildByName("image_dialog_bg");
	i_dialog_bg->setVisible(false);
	view->getChildByName("label_guard")->setVisible(false);
	view->getChildByName<Text*>("label_content")->setTextColor(Color4B::WHITE);
	Sprite * m_chatBg = Sprite::create();
	m_chatBg->setTexture("cocosstudio/login_ui/start_720/aside_bg.png");
	m_chatBg->setScale(Director::getInstance()->getVisibleSize().width / m_chatBg->getContentSize().width, Director::getInstance()->getVisibleSize().width / m_chatBg->getContentSize().width);
	m_chatBg->setAnchorPoint(ccp(0, 0));
	guard_dialog->addChild(m_chatBg);
	m_chatBg->setLocalZOrder(-1);
	view->getChildByName("image_anchor")->setVisible(false);
	auto p_dialog = view->getChildByName("panel_dialog");
	auto panel = p_dialog->getChildByName<Layout*>("panel_content");
	panel->setPositionX(225.0 / 1280 * Director::getInstance()->getVisibleSize().width);
	panel->setContentSize(Size(865, 800));
	auto i_anchor = p_dialog->getChildByName("image_anchor");
	i_anchor->setVisible(true);
	i_anchor->setOpacity(0);
	panel->setPositionY(m_chatBg->getPositionY() + m_chatBg->getContentSize().height - (157 / 286.0)*m_chatBg->getBoundingBox().size.height - 60 - 100);
	i_anchor->setPosition(Vec2(Director::getInstance()->getVisibleOrigin().x + Director::getInstance()->getVisibleSize().width - 80, Director::getInstance()->getVisibleOrigin().y + 40));
	auto content = view->getChildByName<Text*>("label_content");
	content->setContentSize(Size(865, 400));
	content->setPositionY(content->getPositionY() + 100);
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5),
		CallFunc::create([=]{
		p_dialog->runAction(FadeIn::create(0.5));
	    content->runAction(FadeIn::create(0.5));
		this->schedule(schedule_selector(UIGuideCenterCharactorSkill::textShow), 0.08); 
	}))
	);
	

}

void UIGuideCenterCharactorSkill::closeAsideLayer()
{
	closeView("voyageUI_city.csb");
}
void UIGuideCenterCharactorSkill::textShow(float dt)
{
	std::string  s;
	std::string  s1;
	auto view = getViewRoot("voyageUI_city.csb");
	auto guard_dialog = view->getChildByName<Widget*>("panel_dialog");
	auto t_guard = view->getChildByName<Text*>("label_guard");
	auto t_content = view->getChildByName<Text*>("label_content");
	if (talks.size() > 0 && t_size == 0)
	{
		m_talk = talks.at(m_allChatNum);
		s = m_talk;
	}
	else  
	{
		s = m_talk;
	}
	if (t_size == 0)
	{
		cutChat(s);
	}
	int m_wordsize = 0;
	t_size++;

	
	if (m_words.size() == t_size -1)
	{
		t_size = 0;
		m_contentLength = 0;
		m_cursay = true;
		m_chatnum--;
		m_allChatNum++;
		anchorAction();
		unschedule(schedule_selector(UIGuideCenterCharactorSkill::textShow));
		return;
	}

	t_content->setString(m_words.at(t_size-1));

	if (m_cursay)
	{

		t_content->setString(s);
		m_chatnum--;
		t_size = 0;
		m_allChatNum++;	
		anchorAction();
		unschedule(schedule_selector(UIGuideCenterCharactorSkill::textShow));
		return;
	}

}

void UIGuideCenterCharactorSkill::cutChat(std::string content)
{
	int i = 0;
	m_words.clear();
	while (i < content.length())
	{
		char ch = content.c_str()[i];

		if (ch > -127 && ch < 0)
		{
			i += 3;
		}
		else
		{
			i++;
		}
		std::string a;
		a = content.substr(0, i);
		m_words.push_back(a);
		m_contentLength++;
	}
}
void UIGuideCenterCharactorSkill::setButtonsTouch(Node * parent,bool touchable)
{
	auto children = parent->getChildren();
	Vector<Widget*>btns;
	for (auto btn : children)
	{
		std::string name = btn->getName();
		if (btn->getChildren().size() >= 0)
		{
			if (btn->getName().find("button_") == 0)
			{
				auto c_btn = dynamic_cast<Widget*>(btn);
				setButtonsTouch(btn, touchable);
			}
			else
			{
				setButtonsTouch(btn, touchable);
			}
		}
		if (btn->getName().find("button_") == 0)
		{
			auto c_btn = dynamic_cast<Widget*>(btn);
			c_btn->setTouchEnabled(touchable);
		}
	}
}
void UIGuideCenterCharactorSkill::focusOnButton(Widget * psender)
{
	sprite_hand->setVisible(true);
	//	sprite_hand->setAnchorPoint(Vec2(0, 1));
	auto c_psender = dynamic_cast<Widget*>(psender);
	Size win = _director->getWinSize();
	Vec2 pos = c_psender->getWorldPosition();
	Size cut_hand = sprite_hand->getContentSize() / 2;
	Size cut_psendet = psender->getContentSize() / 2;
	std::string name = dynamic_cast<Widget*>(psender)->getName();
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
		if (name.compare("button_result_yes") == 0)
		{
			sprite_hand->setRotation(-110);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
		}
		else
		{
			if (name.compare("") == 0)
			{

			}
			else
			{
				sprite_hand->setRotation(-180);
				sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
					c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
			}

		}

	}
	else if (pos.x > win.width / 2 && pos.y < win.height / 2)
	{
		sprite_hand->setRotation(0);
		sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
			c_psender->getWorldPosition().y + c_psender->getContentSize().height / 2 * 0.6 + sprite_hand->getContentSize().height / 2 * 0.6));
	}
	sprite_hand->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
}
void UIGuideCenterCharactorSkill::asideTouchEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		log("-------OK----------");
		return;
	}

	Widget* widget = dynamic_cast<Widget*>(pSender);
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

	auto name = widget->getName();
	if (isButton(panel_dialog))
	{

		auto view = getViewRoot("voyageUI_city.csb");
		auto guard_dialog = view->getChildByName<Widget*>("panel_dialog");
		auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
		

		if (m_chatcomplete)
		{
			return;
		}
		if (m_chatnum <= 0)
		{
			m_chatcomplete = true;
			closeAsideLayer();
			m_allChatNum = 0;
			guide();
			return;
		}
		m_chatcomplete = false;
		if (!m_cursay)
		{
			m_cursay = true;
			return;
		}
		else
		{
			if (m_chatnum)
			{
				t_size = 0;
				m_cursay = false;
				i_anchor->runAction(Sequence::createWithTwoActions(FadeOut::create(0.2), CallFunc::create([=](){ i_anchor->stopAllActions(); })));
				schedule(schedule_selector(UIGuideCenterCharactorSkill::textShow), 0.08);
			}

		}
	}
	

	return;

}

void UIGuideCenterCharactorSkill::readJsonFile()
{
	std::string jsonpath;
	std::string name = confirmCityJson();
	// 	jsonpath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".json");
	// 	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);

	jsonpath = findJsonFile(name);
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);

	rapidjson::Document root;
	root.Parse<0>(contentStr.c_str());
	auto count = DictionaryHelper::getInstance()->getArrayCount_json(root, "pname");
	auto dialog1_count = DictionaryHelper::getInstance()->getArrayCount_json(root, "dialog1");

	for (int i = 0; i < count; i++)
	{
		const auto& dic = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root, "pname", i);
		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%d", i + 1);
		auto content = DictionaryHelper::getInstance()->getStringValue_json(dic, buffer);
		std::string str = (char*)content;
		m_names.push_back(str);
	}
	for (int i = 0; i < dialog1_count; i++)
	{
		const auto& dic = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root, "dialog1", i);
		int idx = 1;
		auto content = DictionaryHelper::getInstance()->getStringValue_json(dic, "1");
		if (!content){
			content = DictionaryHelper::getInstance()->getStringValue_json(dic, "2");
			idx = 2;
		}
		if (!content){
			content = DictionaryHelper::getInstance()->getStringValue_json(dic, "3");
			idx = 3;
		}
		if (!content){
			content = DictionaryHelper::getInstance()->getStringValue_json(dic, "0");
			idx = 0;
		}
		std::string str = (char*)content;
		if (content != 0){
			//std::string m_TipInfo;
			//m_TipInfo.insert(std::pair<int,std::string>(i,pname));
			auto item = new DIALOGDATA;
			item->dialog = str;
			item->personIdx = idx;
			m_dialogdatas.push_back(item);
		}
	}
}

std::string UIGuideCenterCharactorSkill::findJsonFile(std::string path){
	std::string jsonPath;
	LanguageType nType = LanguageType(Utils::getLanguage());
	char buffer[10];
	sprintf(buffer, "%d", ProtocolThread::GetInstance()->getIcon());
	//JOHN = ProtocolThread::GetInstance()->getHeroName();
	switch (nType)
	{
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
		log("language: chinese");

		jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(path + ".zh_HK.json");

		break;
	case cocos2d::LanguageType::CHINESE:
		log("language: chinese");

		jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(path + ".zh_CN.json");

		break;
	default:
		log("language: english");
		jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(path + ".json");
		break;
	}
	return jsonPath;
}

std::string UIGuideCenterCharactorSkill::confirmCityJson()
{

	int city_id = SINGLE_HERO->m_iCityID;
	std::string str;
	switch (city_id)
	{
	case 17:
		str = "Lisben_";
		break;
	case 15:
		str = "Xibanya_";
		break;
	case 21:
		str = "England_";
		break;
	case 23:
		str = "Helan_";
		break;
	case 9:
		str = "Renaya_";
		break;
	default:
		break;
	}
	return StringUtils::format("res_lua/lua_config/%sScene", str.c_str());
}
void UIGuideCenterCharactorSkill::anchorAction()
{
	auto view = getViewRoot("voyageUI_city.csb");
	auto guard_dialog = view->getChildByName<Widget*>("panel_dialog");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(255);
	i_anchor->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5, Vec2(0, 10))), EaseBackOut::create(MoveBy::create(0.5, Vec2(0, -10))))));

}
void UIGuideCenterCharactorSkill::initGuideInfo()
{
	auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_STASTICS_CSB]);
	auto i_icon_bg = view->getChildByName<ImageView*>("image_head_player_bg");
	auto i_icon = i_icon_bg->getChildByName<ImageView*>("image_head");
	auto p_exp = i_icon_bg->getChildByName<Widget*>("panel_exp");
	auto p_rep = i_icon_bg->getChildByName<Widget*>("panel_reputation");
	auto i_head_lv = i_icon_bg->getChildByName<ImageView*>("image_head_lv");
	auto t_lv = i_head_lv->getChildByName<Text*>("label_lv");
	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_name"));
	auto i_country = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_country"));
	auto t_exp = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_exp_num"));
	auto t_r_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_lv_r"));
	auto t_prestige = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_reputation_num"));
	auto t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_silver_num"));
	auto t_daily = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_daily_expenses_num"));
	auto t_gold = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_gold_num"));
	auto t_bank = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_bank_deposits_num"));
	auto t_guild = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_guild_name"));
	auto t_time = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_time"));
	auto i_guild = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "Image_61"));
	auto t_bg_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_lv_bg"))->getChildByName<Text*>("label_lv");
	auto listView = view->getChildByName<ListView*>("listview_infomation");
	auto p_CInfo = listView->getItem(0);
	auto p_static = p_CInfo->getChildByName("panel_stastics");
	auto t_exLv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_static->getChildByName<Widget*>("panel_exp"), "label_lv"));
	auto t_rpLv = dynamic_cast<Text*>(Helper::seekWidgetByName(p_static->getChildByName<Widget*>("panel_reputation"), "label_lv_r"));

	int level = EXP_NUM_TO_LEVEL(SINGLE_HERO->m_iExp);
	int rlevel = FAME_NUM_TO_LEVEL(SINGLE_HERO->m_iRexp);
	t_lv->setString(StringUtils::format("%d", level));

	float temp_exp = 0;
	if (level < LEVEL_MAX_NUM)
	{
		temp_exp = (SINGLE_HERO->m_iExp - LEVEL_TO_EXP_NUM(level))*1.0 / (LEVEL_TO_EXP_NUM(level + 1) - LEVEL_TO_EXP_NUM(level));
	}
	float temp_rep = 0;
	if (rlevel < LEVEL_MAX_NUM)
	{
		temp_rep = (SINGLE_HERO->m_iRexp - LEVEL_TO_FAME_NUM(rlevel))*1.0 / (LEVEL_TO_FAME_NUM(rlevel + 1) - LEVEL_TO_FAME_NUM(rlevel));
	}
	t_exp->setString(StringUtils::format("%lld/%lld", SINGLE_HERO->m_iExp, LEVEL_TO_EXP_NUM(level + 1)));
	p_exp->setContentSize(Size(p_exp->getSize().width, 178 * temp_exp));
	t_prestige->setString(StringUtils::format("%lld/%lld", SINGLE_HERO->m_iRexp, LEVEL_TO_FAME_NUM(rlevel + 1)));
	p_rep->setContentSize(Size(p_rep->getSize().width, 178 * temp_rep));
	i_country->ignoreContentAdaptWithSize(false);
//	i_country->setTouchEnabled(true);
//	i_country->addTouchEventListener(this, toucheventselector(SkillGuide::menuCall_func));
	t_r_lv->setString(StringUtils::format("%d", rlevel));
	t_gold->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iVp)));
	t_silver->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iSilver)));
	t_bg_lv->setString(StringUtils::format("%d",level));
	t_exLv->setString(StringUtils::format("Lv %d",level));
	t_rpLv->setString(StringUtils::format("Lv %d", rlevel));
}
