#include "UIGuideTarven.h"
#include "UICommon.h"

UIGuideTarven::UIGuideTarven()
{
	sprite_hand = nullptr;
	m_BarGirlContentLabel = nullptr;
	m_guideStep = 0;
	m_dialog_step = 0;
	m_sailor_step = 0;
	m_bDialogActionEnd = false;
	m_guideDialogLayer = nullptr;
	m_eUIType = UI_PUB;
}
UIGuideTarven::~UIGuideTarven()
{
	sprite_hand = nullptr;
	m_BarGirlContentLabel = nullptr;
	m_guideDialogLayer = nullptr;
}
void UIGuideTarven::onEnter()
{
	UIBasicLayer::onEnter();
}
void UIGuideTarven::onExit()
{
	UIBasicLayer::onExit();
}
UIGuideTarven* UIGuideTarven::createPupLayerGuide()
{
	auto pupGuide = new UIGuideTarven;
	if (pupGuide && pupGuide->init())
	{
		pupGuide->autorelease();
		return pupGuide;
	}
	CC_SAFE_DELETE(pupGuide);

	return nullptr;
}
bool UIGuideTarven::init()
{
	bool pRet = false;
	do
	{
		initStatic();
		pRet = true;
	} while (0);
	return pRet;
}
void UIGuideTarven::initStatic()
{
	SINGLE_HERO->m_bDialogToTarven = true;
	m_guideDialogLayer = UINoviceStoryLine::GetInstance();
	m_guideDialogLayer->setVisible(true);
	this->addChild(m_guideDialogLayer, 11000);

	m_guideDialogLayer->onGuideTaskMessage(UINoviceStoryLine::TARVEN_TWO_DIALOG, 0);
	if (UserDefault::getInstance()->getIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO) == OPEN_AUDIO)
	{
		playAudio();
	}
	sprite_hand = Sprite::create();
	sprite_hand->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
	sprite_hand->setVisible(false);
	this->addChild(sprite_hand, 1000);
	openView(TARVEN_COCOS_RES[TARVEN_CSB]);
	openView(TARVEN_COCOS_RES[TARVEN_TWO_CSB], 10);
	auto viewTarven = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);

	auto image_bg = viewTarven->getChildByName<ImageView*>("image_bg");
	image_bg->loadTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_PUP_AREA));
	auto bargirl_dialog = viewTarven->getChildByName<Widget*>("panel_bargirl_dialog");
	bargirl_dialog->setTouchEnabled(true);
	bargirl_dialog->addTouchEventListener(CC_CALLBACK_2(UIGuideTarven::menuCall_func,this));
	auto i_dialog_bg = bargirl_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_bargirl = i_dialog_bg->getChildByName<Text*>("label_bargirl");
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(i_dialog_bg, "label_content"));
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");

	if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
	{
		//村庄为酒吧老板
		t_bargirl->setString(SINGLE_SHOP->getTipsInfo()["TIP_VILLAGE_TAVERN_BOSS_NAME"]);
	}
	else
	{
		t_bargirl->setString(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].bargirl);
	}
	t_content->setVisible(false);
	m_BarGirlContentLabel = Label::create();
	t_content->getParent()->addChild(m_BarGirlContentLabel);
	m_BarGirlContentLabel->setAnchorPoint(Vec2(0, 1));
	m_BarGirlContentLabel->setSystemFontSize(t_content->getFontSize());
	m_BarGirlContentLabel->setSystemFontName(t_content->getFontName());
	m_BarGirlContentLabel->setWidth(t_content->getBoundingBox().size.width);
	m_BarGirlContentLabel->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_BARGIRL_WOLCOME_MIN1"]);
	auto p_content = i_dialog_bg->getChildByName<Widget*>("panel_content");
	p_content->setContentSize(Size(p_content->getContentSize().width, m_BarGirlContentLabel->getSystemFontSize() * 3));
	m_BarGirlContentLabel->setPositionX(t_content->getPositionX());
	m_BarGirlContentLabel->setPositionY(m_BarGirlContentLabel->getSystemFontSize() * 3);
	auto p_tavern = viewTarven->getChildByName<Widget*>("panel_tavern");

	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	view->setTouchEnabled(false);
	auto otherButton = view->getChildByName<Widget*>("panel_two_butter");
	otherButton->setVisible(false);
	otherButton->setOpacity(0);
	setButtonsDisable(view);
	m_dialog_step = BARGIRL_STEP_1;
	//showBarGirlChat(0);
	//银币
	SINGLE_HERO->m_iSilver = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUASILVERNUM").c_str());
	SINGLE_HERO->m_iVp = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("LUAGOLDNUM").c_str());
	updateCoin(SINGLE_HERO->m_iSilver, SINGLE_HERO->m_iVp);

	guideAddNeedControlWidgetForHideOrShow(viewTarven, true);
	
	for (auto child : viewTarven->getChildren())
	{
		if (child->getName()=="image_bg")
		{
			child->setVisible(true);
		}
		else
		{
			child->setVisible(false);
		}
	}
}
void UIGuideTarven::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}
		auto target = dynamic_cast<Widget*>(pSender);
		auto name = target->getName();
		if (isButton(button_recruit_sailor))
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			sprite_hand->setVisible(false);
			m_sailor_step = SAILOR_RECRUIT_STEP_1;
			mainButtonMoveToRight();
			this->scheduleOnce(schedule_selector(UIGuideTarven::openOtherMainButton), 0.4);
			return;
		}
		if (isButton(button_backcity))
		{
			SINGLE_HERO->m_iStage = 2;
			
			m_guideDialogLayer->onGuideTaskMessage(UINoviceStoryLine::TARVEN_THREE_DIALOG, 0);
		//	this->button_callBack();
			return;
		}
		if (isButton(button_chat_1))
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			sprite_hand->setVisible(false);
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushSilverConfirmView("TIP_PUP_FOODANDDRINK_TITLE", "TIP_PUP_FOODANDDRINK_CONTENT", 3000);
		
			auto view = UICommon::getInstance()->getViewRoot(COMMOM_COCOS_RES[C_VIEW_COST_SILVER_COM_CSB]);
			setButtonsDisable(view, "button_s_yes");
			handpicFocusOnButton(view->getChildByName("button_s_yes"));
			return;
		}
		if (isButton(button_s_yes))
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			sprite_hand->setVisible(false);
			//银币
			SINGLE_HERO->m_iSilver -= 3000;
			updateCoin(SINGLE_HERO->m_iSilver, SINGLE_HERO->m_iVp);
			auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_CSB]);
			auto b_chat2 = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_chat_2"));
			b_chat2->getChildByName<Text*>("label_content")->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT3"]);
			view->runAction(MoveTo::create(0.4f, ENDPOS3));
			auto p_button = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB])->getChildByName("panel_two_butter");
			p_button->setVisible(false);
			m_dialog_step = BARGIRL_STEP_2;
			this->scheduleOnce(schedule_selector(UIGuideTarven::showBarGirlChat), 0.4);
			return;
		}
		if (isButton(panel_bargirl_dialog))
		{
			sprite_hand->setVisible(false);
			if (m_bDialogActionEnd)
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
				m_bDialogActionEnd = false;
				m_sailor_step = SAILOR_RECRUIT_STEP_2;
				auto barchief_dialog = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB])->getChildByName<Widget*>("panel_bargirl_dialog");
				auto i_dialog_bg = barchief_dialog->getChildByName<ImageView*>("image_dialog_bg");
				auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");
				i_anchor->stopAllActions();
				i_anchor->setOpacity(0);
				this->scheduleOnce(schedule_selector(UIGuideTarven::openOtherMainButton), 0.4);
			}
			return;
		}
		if (isButton(button_chat_2))
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			sprite_hand->setVisible(false);
			openSailorConfirm();
			return;
		}
		if (isButton(button_ok))
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			sprite_hand->setVisible(false);
			closeView(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB]);
			crewSailorsAccount();

			return;
		}
		if (isButton(button_result_yes))
		{
			sprite_hand->setVisible(false);
			guideAddNeedControlWidgetForHideOrShow(nullptr, true);
			closeView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
			auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_CSB]);
			setButtonsDisable(view);
			SINGLE_HERO->m_iStage = 2;

			m_guideDialogLayer->onGuideTaskMessage(UINoviceStoryLine::TARVEN_THREE_DIALOG, 0);
			return;
		}
	}
void UIGuideTarven::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
}
void UIGuideTarven::setButtonsDisable(Node * node, std::string btn_name)
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
void UIGuideTarven::handpicFocusOnButton(Node * psender)
{
	sprite_hand->setVisible(true);
	sprite_hand->setLocalZOrder(100);
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
		if (name.compare("button_result_yes") == 0)
		{
			sprite_hand->setRotation(-110);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
		}
		else if (name.compare("button_chat_1") == 0)		
		{
			sprite_hand->setRotation(-180);
				sprite_hand->setPosition(Vec2(400,
					c_psender->getWorldPosition().y + c_psender->getContentSize().height / 2 * 0.6 + sprite_hand->getContentSize().height / 2 * 0.6-90));
		}
		else if (name.compare("button_chat_2") == 0)
		{
			sprite_hand->setRotation(-180);
			sprite_hand->setPosition(Vec2(400,
				c_psender->getWorldPosition().y + c_psender->getContentSize().height / 2 * 0.6 + sprite_hand->getContentSize().height / 2 * 0.6  - c_psender->getContentSize().height));
		}
	
	}
	else if (pos.x > win.width / 2 && pos.y < win.height / 2)
	{
		if (name.compare("button_s_yes") == 0)
		{
			sprite_hand->setRotation(-180);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2+100,
				c_psender->getWorldPosition().y + c_psender->getContentSize().height / 2 * 0.6 + sprite_hand->getContentSize().height / 2 * 0.6-110));
		}
		else if (name.compare("button_recruit_sailor") == 0)
		{
			sprite_hand->setRotation(-110);	
			sprite_hand->setPosition(Vec2(1000, 250));
		}
		else if (name.compare("button_ok") == 0)
		{
			sprite_hand->setRotation(0);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y+50 ));
		}
	}
	sprite_hand->runAction(RepeatForever::create(Sequence::createWithTwoActions(TintTo::create(0.5, 255, 255, 255), TintTo::create(0.5, 180, 180, 180))));
}
void UIGuideTarven::updateCoin(const int64_t nCoin, const int64_t nGold)
{
	std::string s_coin = numSegment(StringUtils::format("%lld", nCoin));
	std::string s_gold = numSegment(StringUtils::format("%lld", nGold));
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_silver_num"))->setString(s_coin);
	dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_gold_num"))->setString(s_gold);
}
void UIGuideTarven::showBarGirlChat(float time)
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto bargirl_dialog = view->getChildByName<Widget*>("panel_bargirl_dialog");
	auto i_panel_tavern = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_tavern"));
	i_panel_tavern->setVisible(true);
	bargirl_dialog->setOpacity(255);
	bargirl_dialog->setVisible(true);
	bargirl_dialog->setPosition(Vec2(0, 0));
	auto i_dialog_bg = bargirl_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_bargirl = i_dialog_bg->getChildByName<Text*>("label_bargirl");
	auto i_bargirl = bargirl_dialog->getChildByName<ImageView*>("image_bargirl");
	auto image_f = dynamic_cast<ImageView*>(Helper::seekWidgetByName(i_bargirl,"image_f"));
	image_f->setVisible(false);
	i_bargirl->ignoreContentAdaptWithSize(false);
	if (m_dialog_step == BARGIRL_STEP_1)
	{
		bargirl_dialog->setTouchEnabled(false);
		i_bargirl->loadTexture(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_BAR_GIRL));
	}
	else if (m_dialog_step == BARGIRL_STEP_2)
	{
		if (SINGLE_HERO->m_iGender == 1)
		{
			t_bargirl->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_FEMAIE_AIDE"]);
		}
		else
		{
			t_bargirl->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAIN_MAIE_AIDE"]);
		}
		m_BarGirlContentLabel->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_CHEIF_ACCEPT_DRINK"]);
		bargirl_dialog->setLocalZOrder(100);
		i_bargirl->loadTexture(getCheifIconPath());
		bargirl_dialog->setTouchEnabled(true);
	}
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(i_dialog_bg, "label_content"));
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(0);

	if (i_bargirl->getPositionX() < 0)
	{
		m_BarGirlContentLabel->setOpacity(0);
		i_dialog_bg->setOpacity(0);
		t_bargirl->setOpacity(0);
		i_bargirl->setPositionX(-i_bargirl->getContentSize().width / 2);
		i_bargirl->runAction(MoveBy::create(0.5, Vec2(i_bargirl->getBoundingBox().size.width, 0)));
		i_dialog_bg->setPositionX(0.63*i_dialog_bg->getBoundingBox().size.width);
		i_dialog_bg->runAction(Sequence::create(DelayTime::create(0.1), FadeIn::create(0.5), nullptr));
		t_bargirl->runAction(Sequence::create(DelayTime::create(0.1), FadeIn::create(0.5), nullptr));
		m_BarGirlContentLabel->runAction(Sequence::create(DelayTime::create(0.1), FadeIn::create(0.5), CallFunc::create([=]{
			if (m_dialog_step == BARGIRL_STEP_1)
			{
				m_bDialogActionEnd = true;
			}
			 }), nullptr));
		if (m_dialog_step == BARGIRL_STEP_1)
		{
			this->scheduleOnce(schedule_selector(UIGuideTarven::mainButtonMoveToLeft), 0);
		}
		else if (m_dialog_step == BARGIRL_STEP_2)
		{
			this->scheduleOnce(schedule_selector(UIGuideTarven::anchorAction), 1);
		}
	}
}

//对话的点击提示图标运动
void UIGuideTarven::anchorAction(const float fTime)
{
	auto guard_dialog = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB])->getChildByName("panel_bargirl_dialog");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_dialog_bg")->getChildByName<ImageView*>("image_anchor");
	i_anchor->stopAllActions();
	i_anchor->setPosition(Vec2(642, 39));
	i_anchor->setOpacity(255);
	i_anchor->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5, Vec2(0, 10))), EaseBackOut::create(MoveBy::create(0.5, Vec2(0, -10))))));
}
void UIGuideTarven::mainButtonMoveToLeft(const float fTime)
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto p_tavern = view->getChildByName<Widget*>("panel_tavern");
	p_tavern->setPosition(STARTPOS);
	auto b_chat = p_tavern->getChildByName<Button*>("button_chat");
	auto b_order = p_tavern->getChildByName<Button*>("button_order");
	auto b_sailor = p_tavern->getChildByName<Button*>("button_recruit_sailor");
	auto b_captain = p_tavern->getChildByName<Button*>("button_hire_captain");
	auto b_bounty = p_tavern->getChildByName<Button*>("button_bounty_board");
	auto b_pos_x = b_chat->getPositionX();
	auto winsize = Director::getInstance()->getWinSize();
	b_chat->setPositionX(winsize.width + b_chat->getBoundingBox().size.width / 2);
	b_order->setPositionX(winsize.width + b_order->getBoundingBox().size.width / 2);
	b_sailor->setPositionX(winsize.width + b_sailor->getBoundingBox().size.width / 2);
	b_captain->setPositionX(winsize.width + b_captain->getBoundingBox().size.width / 2);
	b_bounty->setPositionX(winsize.width + b_bounty->getBoundingBox().size.width / 2);
	b_chat->addTouchEventListener(CC_CALLBACK_2(UIGuideTarven::menuCall_func,this));
	b_order->addTouchEventListener(CC_CALLBACK_2(UIGuideTarven::menuCall_func,this));
	b_sailor->addTouchEventListener(CC_CALLBACK_2(UIGuideTarven::menuCall_func,this));
	b_captain->addTouchEventListener(CC_CALLBACK_2(UIGuideTarven::menuCall_func,this));
	b_bounty->addTouchEventListener(CC_CALLBACK_2(UIGuideTarven::menuCall_func,this));
	b_chat->runAction(Sequence::create(MoveBy::create(0.2, Vec2(-b_chat->getBoundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	b_order->runAction(Sequence::create(DelayTime::create(0.05), MoveBy::create(0.2, Vec2(-b_order->getBoundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	b_sailor->runAction(Sequence::create(DelayTime::create(0.1), MoveBy::create(0.2, Vec2(-b_sailor->getBoundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	b_captain->runAction(Sequence::create(DelayTime::create(0.15), MoveBy::create(0.2, Vec2(-b_captain->getBoundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	b_bounty->runAction(Sequence::create(DelayTime::create(0.2), MoveBy::create(0.2, Vec2(-b_bounty->getBoundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), CallFunc::create([=]{handpicFocusOnButton(b_sailor); }), nullptr));
	setButtonsDisable(view, "button_recruit_sailor");
}
void UIGuideTarven::mainButtonMoveToRight()
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB]);
	auto p_tavern = view->getChildByName<Widget*>("panel_tavern");
	p_tavern->setPosition(STARTPOS);
	auto b_chat = p_tavern->getChildByName<Button*>("button_chat");
	auto b_order = p_tavern->getChildByName<Button*>("button_order");
	auto b_sailor = p_tavern->getChildByName<Button*>("button_recruit_sailor");
	auto b_captain = p_tavern->getChildByName<Button*>("button_hire_captain");
	auto b_bounty = p_tavern->getChildByName<Button*>("button_bounty_board");
	auto winsize = Director::getInstance()->getWinSize();
	b_chat->runAction(Sequence::create(MoveTo::create(0.2, Vec2(b_chat->getBoundingBox().size.width / 2 + winsize.width, b_chat->getPositionY())), nullptr));
	b_order->runAction(Sequence::create(DelayTime::create(0.05), MoveTo::create(0.2, Vec2(b_order->getBoundingBox().size.width / 2 + winsize.width, b_order->getPositionY())), nullptr));
	b_sailor->runAction(Sequence::create(DelayTime::create(0.1), MoveTo::create(0.2, Vec2(b_sailor->getBoundingBox().size.width / 2 + winsize.width, b_sailor->getPositionY())), nullptr));
	b_captain->runAction(Sequence::create(DelayTime::create(0.15), MoveTo::create(0.2, Vec2(b_captain->getBoundingBox().size.width / 2 + winsize.width, b_captain->getPositionY())), nullptr));
	b_bounty->runAction(Sequence::create(DelayTime::create(0.2), MoveTo::create(0.2, Vec2(b_bounty->getBoundingBox().size.width / 2 + winsize.width, b_bounty->getPositionY())), nullptr));
}
void UIGuideTarven::openOtherMainButton(const float time)
{
	auto p_tavern2 = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	p_tavern2->setVisible(true);
	auto otherButton = p_tavern2->getChildByName<Widget*>("panel_two_butter");
	otherButton->setVisible(true);
	otherButton->setOpacity(255);
	auto b_chat2 = otherButton->getChildByName<Button*>("button_chat");
	auto b_order2 = otherButton->getChildByName<Button*>("button_order");
	auto b_sailor2 = otherButton->getChildByName<Button*>("button_recruit_sailor");
	auto b_captain2 = otherButton->getChildByName<Button*>("button_hire_captain");
	auto b_bounty2 = otherButton->getChildByName<Button*>("button_bounty_board");
	openSailorDialog(b_sailor2);

	
}
void UIGuideTarven::openSailorDialog(Widget* button)
{
	openView(TARVEN_COCOS_RES[TARVEN_RECRUIT_CSB]);
	auto sailors = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_CSB]);
	moveTo(sailors, STARTPOS);
	initSailor();
	
	auto viewTwo = getViewRoot(TARVEN_COCOS_RES[TARVEN_TWO_CSB]);
	auto button_recruit_sailor = dynamic_cast<Button*>(Helper::seekWidgetByName(viewTwo, "button_recruit_sailor"));
	if (m_sailor_step == SAILOR_RECRUIT_STEP_1)
	{
		button_recruit_sailor->runAction(MoveBy::create(0.2, Vec2(-17, 0)));
		button_recruit_sailor->setBright(false);
		button_recruit_sailor->setTouchEnabled(false);
		setButtonsDisable(sailors, "button_chat_1");
	}
	else if (m_sailor_step == SAILOR_RECRUIT_STEP_2)
	{
		setButtonsDisable(sailors, "button_chat_2");
	}
	//setButtonsDisable(viewTwo,"button_backcity");
	
}
//交互框移动
bool UIGuideTarven::moveTo(Widget* target, const Point& pos)
{
	closeBarGirlChat();
	auto button_chat_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(target, "button_chat_1"));
	auto button_chat_2 = dynamic_cast<Button*>(Helper::seekWidgetByName(target, "button_chat_2"));
	float time = 0;
	target->setPosition(ENDPOS3);
	if (m_sailor_step == SAILOR_RECRUIT_STEP_1)
	{
		target->runAction(Sequence::create(DelayTime::create(time), MoveTo::create(0.4f, pos), CallFunc::create([=]{handpicFocusOnButton(button_chat_1); }), nullptr));
	}
	else if (m_sailor_step == SAILOR_RECRUIT_STEP_2)
	{
		target->runAction(Sequence::create(DelayTime::create(time), MoveTo::create(0.4f, pos), CallFunc::create([=]{handpicFocusOnButton(button_chat_2); }), nullptr));
	}
	return true;
}
/*
*关闭女郎对话
*/
void UIGuideTarven::closeBarGirlChat()
{
	auto bargirl_dialog = getViewRoot(TARVEN_COCOS_RES[TARVEN_CSB])->getChildByName<Widget*>("panel_bargirl_dialog");
	bargirl_dialog->setTouchEnabled(true);
	auto i_dialog_bg = bargirl_dialog->getChildByName<ImageView*>("image_dialog_bg");
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(i_dialog_bg, "label_content"));
	auto t_batgirl = i_dialog_bg->getChildByName<Text*>("label_bargirl");
	auto i_bargirl = bargirl_dialog->getChildByName<ImageView*>("image_bargirl");
	auto i_anchor = i_dialog_bg->getChildByName<ImageView*>("image_anchor");
	i_anchor->stopAllActions();
	i_anchor->setOpacity(0);
	bargirl_dialog->stopAllActions();
	
	t_content->runAction(FadeOut::create(0.1));
	i_dialog_bg->runAction(FadeOut::create(0.1));
	t_batgirl->runAction(FadeOut::create(0.1));
	i_bargirl->runAction(Sequence::createWithTwoActions(DelayTime::create(0), MoveBy::create(0.3, Vec2(-i_bargirl->getBoundingBox().size.width, 0))));
	bargirl_dialog->runAction(Sequence::createWithTwoActions(DelayTime::create(0), MoveTo::create(0, ENDPOS)));

}
//初始化水手交互框
void UIGuideTarven::initSailor()
{
	auto sailor = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_CSB]);
	sailor->setTouchEnabled(false);
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(sailor, "label_title"));
	auto t_content = sailor->getChildByName<Text*>("label_content");
	auto b_chat1 = dynamic_cast<Button*>(Helper::seekWidgetByName(sailor, "button_chat_1"));
	auto b_chat2 = dynamic_cast<Button*>(Helper::seekWidgetByName(sailor, "button_chat_2"));
	auto b_chat3 = dynamic_cast<Button*>(Helper::seekWidgetByName(sailor, "button_chat_3"));
	auto t_chat1 = b_chat1->getChildByName<Text*>("label_1");
	auto t_content1 = b_chat1->getChildByName<Text*>("label_content");
	auto i_silver = b_chat1->getChildByName<Text*>("image_silver");
	auto t_silver = b_chat1->getChildByName<Text*>("label_silver_num");
	auto t_silver_0 = b_chat1->getChildByName<Text*>("label_silver_num_0");
	auto t_chat2 = b_chat2->getChildByName<Text*>("label_2");
	auto t_content2 = b_chat2->getChildByName<Text*>("label_content");
	auto t_chat3 = b_chat3->getChildByName<Text*>("label_3");
	auto t_content3 = b_chat3->getChildByName<Text*>("label_content");

	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_TITLE"]);
	//int n = 30 * (ceil(FAME_NUM_TO_LEVEL(m_pBarInfoResult->fame) / 10.0) + 1);
	std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT"];
	std::string new_vaule = StringUtils::format("%d", 180);
	std::string old_vaule = "[num]";
	repalce_all_ditinct(content, old_vaule, new_vaule);
	t_content->setString(content);
	t_chat1->setString("1.");
	t_chat2->setString("2.");
	t_chat3->setString("3.");

	t_content1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT2"]);
	t_silver->setString(numSegment(StringUtils::format("%d",3000)));
	auto i_bargirl = sailor->getChildByName<ImageView*>("image_bargirl");
	i_bargirl->ignoreContentAdaptWithSize(false);
	i_bargirl->loadTexture(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_BAR_GIRL));

	if (m_sailor_step == SAILOR_RECRUIT_STEP_2)
	{
		i_silver->setVisible(false);
		t_silver->setVisible(false);
		t_silver_0->setVisible(true);
		t_content2->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT3"]);
	}
	else if (m_sailor_step == SAILOR_RECRUIT_STEP_1)
	{
		i_silver->setVisible(true);
		t_silver->setVisible(true);
		t_silver_0->setVisible(false);
		t_content2->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT1"]);
	}
	t_content3->setString(SINGLE_SHOP->getTipsInfo()["TIP_TARVEN_FIRE_SAILORS_TITLE"]);
}
//雇佣水手界面
void UIGuideTarven::openSailorConfirm()
{
	openView(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB],20);
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB]);
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_content"));
	//initSailorDialog();
	
	std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PUP_HIRE_SAILORS_CONTENT"];
	std::string new_vaule = StringUtils::format("%d", 180);
	std::string old_vaule = "[num]";
	repalce_all_ditinct(content, old_vaule, new_vaule);
	t_content->setString(content);

	auto t_selnum = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_count"));
	auto t_cost = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_income_num"));
	auto liew_sailor = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_ship_sailor_num"));
	auto t_sailor1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_1"));
	auto t_sailor2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_2"));
	auto liew_days = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_maxday"));
	auto t_days = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_maxday_num"));
	auto liew_required = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_required"));
	auto t_required = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_required_num"));
	auto l_sailor = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view, "progressbar_durable"));
	auto l_sailor_add = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view, "progressbar_durable_add"));
	auto b_caveat = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_caveat"));
	b_caveat->setVisible(false);
	auto sliders = dynamic_cast<Slider*>(Helper::seekWidgetByName(view, "slider_goods_num"));
	sliders->setTouchEnabled(false);
	sliders->setPercent(100);

	t_selnum->setString(StringUtils::format("%d/%d", 35,35));
	t_cost->setString(numSegment(StringUtils::format("%d", 4000)));
	t_sailor1->setString(StringUtils::format("%d",35));
	t_sailor2->setString(StringUtils::format("/%d", 35));
	t_days->setString(StringUtils::format("%d", 0));
	t_required->setString(StringUtils::format("%d",16));
	l_sailor->setPercent(100);
	l_sailor_add->setPercent(100);


	auto b_ok = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_ok"));
	b_ok->addTouchEventListener(CC_CALLBACK_2(UIGuideTarven::menuCall_func,this));
	setButtonsDisable(view, "button_ok");
	handpicFocusOnButton(b_ok);
	liew_sailor->refreshView();
	liew_days->refreshView();
	liew_required->refreshView();
}
void UIGuideTarven::crewSailorsAccount()
{
	auto tips = SINGLE_SHOP->getTipsInfo();
	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB], 21);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	view->getChildByName<Text*>("label_title")->setString(tips["TIP_PUP_SAILOR_RECRUIT_SUCCESS"]);
	auto p_result = view->getChildByName<Widget*>("panel_result");
	auto l_result = view->getChildByName<ListView*>("listview_result");
	auto image_div = p_result->getChildByName<ImageView*>("image_div_1");
	auto p_item = p_result->getChildByName<Widget*>("panel_silver_2");
	auto p_sailor = p_item->clone();
	p_sailor->getChildByName<Text*>("label_force_relation")->setString(tips["TIP_PUP_SAILOR_RECRUIT_TITLE"]);
	p_sailor->getChildByName<Text*>("label_buy_num")->setString(StringUtils::format("+%d", 35));
	p_sailor->getChildByName<Text*>("label_buy_num")->setTextColor(Color4B(40, 25, 13, 255));
	p_sailor->getChildByName<ImageView*>("image_silver")->loadTexture("cocosstudio/login_ui/common/sailor_2.png");
	p_sailor->getChildByName<ImageView*>("image_silver")->setPositionX(p_sailor->getChildByName<Text*>("label_buy_num")->getPositionX() - p_sailor->getChildByName<Text*>("label_buy_num")->getBoundingBox().size.width
		- p_sailor->getChildByName<ImageView*>("image_silver")->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_sailor);
	auto image_div_1 = image_div->clone();
	l_result->pushBackCustomItem(image_div_1);
	auto p_cost = p_item->clone();
	p_cost->getChildByName<Text*>("label_force_relation")->setString(tips["TIP_PUP_SAILOR_RECRUIT_FEE"]);
	p_cost->getChildByName<Text*>("label_buy_num")->setString(numSegment(StringUtils::format("%d", 500)));
	p_cost->getChildByName<Text*>("label_buy_num")->setTextColor(Color4B(40, 25, 13, 255));
	p_cost->getChildByName<ImageView*>("image_silver")->loadTexture("cocosstudio/login_ui/common/silver.png");
	p_cost->getChildByName<ImageView*>("image_silver")->setPositionX(p_cost->getChildByName<Text*>("label_buy_num")->getPositionX() - p_cost->getChildByName<Text*>("label_buy_num")->getBoundingBox().size.width
		- p_cost->getChildByName<ImageView*>("image_silver")->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_cost);
	auto image_div_2 = image_div->clone();
	l_result->pushBackCustomItem(image_div_2);
	auto p_fee = p_item->clone();
	p_fee->getChildByName<Text*>("label_force_relation")->setString(tips["TIP_PUP_SAILOR_RECRUIT_COST"]);
	p_fee->getChildByName<Text*>("label_buy_num")->setString(numSegment(StringUtils::format("%d", 4000)));
	p_fee->getChildByName<Text*>("label_buy_num")->setTextColor(Color4B(40, 25, 13, 255));
	p_fee->getChildByName<ImageView*>("image_silver")->loadTexture("cocosstudio/login_ui/common/silver.png");
	p_fee->getChildByName<ImageView*>("image_silver")->setPositionX(p_fee->getChildByName<Text*>("label_buy_num")->getPositionX() - p_fee->getChildByName<Text*>("label_buy_num")->getBoundingBox().size.width
		- p_fee->getChildByName<ImageView*>("image_silver")->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_fee);
	auto image_div_3 = image_div->clone();
	l_result->pushBackCustomItem(image_div_3);
	//银币
	SINGLE_HERO->m_iSilver -= 4000;
	updateCoin(SINGLE_HERO->m_iSilver, SINGLE_HERO->m_iVp);
	view->getChildByName<Button*>("button_result_yes")->addTouchEventListener(CC_CALLBACK_2(UIGuideTarven::menuCall_func,this));
	auto button_result_yes = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_result_yes"));
	handpicFocusOnButton(button_result_yes);
	setButtonsDisable(view, "button_result_yes");
}
