#include "UIGuidePalace.h"

#include "UICommon.h"
#include "UIInform.h"

UIGuidePalace::UIGuidePalace() :
m_pPalaceResult(nullptr),
m_bDialogOut(false),
sprite_hand(nullptr),
m_Haslisence(0),
m_guideStage(0),
m_btnMoved(false),
m_viewruancomplete(true)
{
	m_eUIType = UI_PALACE;
	m_lisenceprice = 100000;
	m_curView = nullptr;
	m_InfoViewalreadyOpen = false;
	m_preMainButton = nullptr;
	isShowPopularGoods = false;
};

UIGuidePalace::~UIGuidePalace()
{
	if (m_pPalaceResult)
	{
		get_palace_info_result__free_unpacked(m_pPalaceResult, 0);
		m_pPalaceResult = nullptr;
	}
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}

UIGuidePalace* UIGuidePalace::createPalace()
{
	auto bk = new UIGuidePalace;
	if (bk && bk->init())
	{
		bk->autorelease();
		return bk;
	}
	CC_SAFE_DELETE(bk);

	return nullptr;
}

bool UIGuidePalace::init()
{
	bool pRet = false;
	do
	{
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIGuidePalace::onServerEvent, this), this);
		this->scheduleOnce(schedule_selector(UIGuidePalace::initStatic), 0);
		ProtocolThread::GetInstance()->getPalaceInfo(UILoadingIndicator::create(this, m_eUIType));
		pRet = true;
	} while (0);
	return pRet;
}


void UIGuidePalace::onEnter()
{
	UIBasicLayer::onEnter();
}
void UIGuidePalace::onExit()
{
	UIBasicLayer::onExit();
}
void UIGuidePalace::initStatic(float f)
{
	initCityView();
}
void UIGuidePalace::showPalaceInfo(const GetPalaceInfoResult*result)
{
	std::string html_color_begin;
	std::string html_color_end;
#if WIN32
	html_color_begin = "";
	html_color_end = "";
#else
	//下划线
	html_color_begin = "<html><u >";
	html_color_end = "</u></html>";
#endif
	openView(PALACE_COCOS_RES[VIEW_CITYINFO_CSB]);
	auto viewCity = getViewRoot(PALACE_COCOS_RES[VIEW_CITYINFO_CSB]);
	viewCity->setTouchEnabled(false);
	auto listviewContent = viewCity->getChildByName("listview_content");
	auto image_titleBg = viewCity->getChildByName("image_title_bg");
	auto imageCityCountry = image_titleBg->getChildByName<ImageView*>("image_country");
	auto labelCity = image_titleBg->getChildByName<Text*>("label_city");
	if (result->nationidx > 0)
	{
		//City icon
		imageCityCountry->loadTexture(getCountryIconPath(result->nationidx));
	}
	else
	{
		//City icon
		imageCityCountry->loadTexture(getCountryIconPath(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].nation));
	}
	labelCity->setString(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].name);
	auto panelContent = listviewContent->getChildByName("panel_content");
	auto panelCity = panelContent->getChildByName("panel_city");
	auto text_port = dynamic_cast<Text*>(Helper::seekWidgetByName(viewCity, "text_port"));
	std::string s_port = text_port->getString();
	std::string old_value = "[NeutralPort]";
	std::string new_value = SINGLE_SHOP->getTipsInfo()[getPortTypeOrInfo(2).c_str()];
	repalce_all_ditinct(s_port, old_value, new_value);
	text_port->setString(s_port);
	auto text_port_city = dynamic_cast<Text*>(Helper::seekWidgetByName(viewCity, "text_port_city"));
	std::string s_port_city;
	s_port_city += html_color_begin;
	s_port_city += text_port_city->getString();
	s_port_city += html_color_end;
	std::string old_value1 = "[England]";
	std::string new_value1 = SINGLE_SHOP->getNationInfo()[SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].nation];
	repalce_all_ditinct(s_port_city, old_value1, new_value1);
	text_port_city->setString(s_port_city);
	text_port_city->setPositionX(text_port->getContentSize().width + text_port->getPositionX() + (text_port_city->getContentSize().width) / 2);
	text_port_city->setTouchEnabled(false);
	auto imageCity_population = panelCity->getChildByName("image_city_population");
	auto labelPopulation_num = imageCity_population->getChildByName<Text*>("label_population_num");
	labelPopulation_num->setString(StringUtils::format("%d", result->population));

	auto imageCity_trade = panelCity->getChildByName("image_city_trade");
	auto labelTrade_num = imageCity_trade->getChildByName<Text*>("label_trade_num");
	labelTrade_num->setString(StringUtils::format("%d", result->trade));

	auto imageCity_manufacture = panelCity->getChildByName("image_city_manufacture");
	auto labelManufacture_num = imageCity_manufacture->getChildByName<Text*>("label_manufacture_num");
	labelManufacture_num->setString(StringUtils::format("%d", result->manufacture));
	auto panCurrent = panelContent->getChildByName("panel_current");
	auto imageCountry = panCurrent->getChildByName<ImageView*>("image_country");

	auto listviewNameLv = panCurrent->getChildByName<ListView*>("listview_name_lv");

	auto currMayorName = dynamic_cast<Text*>(listviewNameLv->getItem(0));

	auto mayorImage = panCurrent->getChildByName<ImageView*>("image_country");
	mayorImage->loadTexture(getCountryIconPath(result->mayornation));
	if (result->mayorname)
	{
		currMayorName->setString(result->mayorname);
		mayorImage->setVisible(true);
	}
	else
	{
		currMayorName->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NOT_RANK"]);
		mayorImage->setVisible(false);
	}
	auto currMayorLv = dynamic_cast<Text*>(listviewNameLv->getItem(1));
	currMayorLv->setVisible(false);
	currMayorLv->setString(StringUtils::format("Lv. %d", result->mayorlevel));

	auto imageCitytrade = panCurrent->getChildByName<ImageView*>("image_city_trade");
	auto labelPolice = imageCitytrade->getChildByName<Text*>("label_trade");
	listviewNameLv->refreshView();
	//develop way
	std::string  s_direct = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_TITLE"];
	int dirNum = result->olddevdirection;
	if (result->newdevdirection != 0)
	{
		dirNum = result->newdevdirection;
	}
	if (result->mayorname == nullptr)
	{
		dirNum = 4;
	}
	if (dirNum == 1)
	{
		imageCitytrade->loadTexture("cocosstudio/login_ui/palace_720/city_population.png");
		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_POPULATION"];
	}
	else if (dirNum == 2)
	{
		imageCitytrade->loadTexture("cocosstudio/login_ui/palace_720/city_trade.png");
		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_TRADE"];
	}
	else if (dirNum == 3)
	{
		imageCitytrade->loadTexture("cocosstudio/login_ui/palace_720/city_manufacture.png");
		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_MAKING"];
	}
	else
	{
		imageCitytrade->loadTexture(" ");
		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NOT_RANK"];
	}
	labelPolice->setString(s_direct);
	auto image_pulldown = viewCity->getChildByName<ImageView*>("image_pulldown");
	image_pulldown->setScale9Enabled(false);
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar((ListView*)listviewContent, image_pulldown);
	
}

void UIGuidePalace::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		auto target = dynamic_cast<Widget*>(pSender);
		buttonEventByName(target, target->getName());
	}
}

void UIGuidePalace::buttonEventByName(Widget* target, std::string name)
{
	auto winsize = Director::getInstance()->getWinSize();

	auto  twoButter = this->getChildByName("panel_two_butter");
	auto Butter_button_city = twoButter->getChildByName<Button*>("button_city_info");
	auto Butter_button_hall = twoButter->getChildByName<Button*>("button_hall_of_honor");
	auto Butter_button_invest = twoButter->getChildByName<Button*>("button_investment");
	auto Butter_button_office = twoButter->getChildByName<Button*>("button_office");

	if (isButton(button_city_info))//城市信息
	{
		if (!m_viewruancomplete)
		{
			return;
		}
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//只有数字键盘消失时
		if (m_bDialogOut)
		{
			viewCloseAction();
			closeView(PALACE_COCOS_RES[VIEW_INVESTMENT_CSB]);
		}
		else
		{
			palaceoutofscreen();
		}

		showPalaceInfo(m_pPalaceResult);
		openView(PALACE_COCOS_RES[VIEW_CITYINFO_CSB], 0);
		viewAction(getViewRoot(PALACE_COCOS_RES[VIEW_CITYINFO_CSB]), PALACE_COCOS_RES[VIEW_CITYINFO_CSB]);
		ProtocolThread::GetInstance()->getPalaceInfo(UILoadingIndicator::create(this));
		if (!m_btnMoved)
		{
			buttonMoveRight();
		}
		changeMainButtonState(target);
		guide();
		return;
	}
	//购买交易许可证
	if (isButton(button_buy))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushSilverConfirmView("TIP_PALACE_BUY_LICENSE_TITLE", "TIP_PALACE_BUY_LICENSE_CONTENT", m_lisenceprice);
		SINGLE_HERO->m_iSilver -= m_lisenceprice;
		guide();
		return;
	}
	//返回主城
	if (isButton(button_backcity))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		SINGLE_HERO->m_iStage = 4;
		this->button_callBack();
		return;
	}
	//确认购买
	if (isButton(button_s_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openViewAutoClose("TIP_PALACE_BUY_LICENSE_SUCCESS");
		this->removeChildByTag(101);
		m_Haslisence = 1;
		showMainCoin();
		showBussinessliceView();
		guide();
		return;
	}

}

void UIGuidePalace::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetPalaceInfoResult:
	{
										   GetPalaceInfoResult *result = (GetPalaceInfoResult *)message;
										   if (result->failed == 0)
										   {
											   m_pPalaceResult = result;

											   if (CCUserDefault::getInstance()->getIntegerForKey(BG_MUSIC_KEY, OPEN_AUDIO) == OPEN_AUDIO)
											   {
												   playAudio();
											   }
										   }
										   else
										   {
											   openSuccessOrFailDialog("TIP_PALACE_GET_DATA_FAIL");
										   }
										   break;
	}
	default:
		break;
	}
}

void UIGuidePalace::initCityView()
{
	auto Winsize = Director::getInstance()->getWinSize();
	//打开csb文件
	openView(PALACE_COCOS_RES[VIEW_PLACE_CSB]);
	auto viewCsb = getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB]);
	//对话
	auto panel_palace = viewCsb->getChildByName("panel_palace");
	//显示金银币
	auto panel_actionbar = viewCsb->getChildByName("panel_actionbar");
	//四个按钮
	auto panel_two_butter = viewCsb->getChildByName<Widget*>("panel_two_butter");
	auto panelDialog = panel_palace->getChildByName("panel_dialog");
	auto roleGuard = panelDialog->getChildByName<ImageView*>("image_soldier");;
	auto image_dialog_bg = panelDialog->getChildByName<ImageView*>("image_dialog_bg");
	auto labelguard = panelDialog->getChildByName("label_guard");
	auto guardText = panelDialog->getChildByName<Text*>("label_content");
	auto i_anchor = panelDialog->getChildByName<ImageView*>("image_anchor");
	image_dialog_bg->setOpacity(0);
	labelguard->setOpacity(0);
	guardText->setOpacity(0);
	i_anchor->setOpacity(0);
	image_dialog_bg->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), FadeIn::create(0.5)));
	labelguard->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), FadeIn::create(0.5)));
	guardText->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), FadeIn::create(0.5)));
	roleGuard->runAction(MoveTo::create(0.5f, Vec2(0, 0)));
	roleGuard->ignoreContentAdaptWithSize(false);
	roleGuard->loadTexture(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_SOLDIER));
	auto button_officeSuDan = panel_two_butter->getChildByName<Button*>("button_office");

	panel_two_butter->setLocalZOrder(99);
	panel_two_butter->setPosition(panel_two_butter->getWorldPosition());
	panel_two_butter->removeFromParent();
	this->addChild(panel_two_butter);

	setButtonsDisable(panel_two_butter);

	//背景图片
	auto image_bg = viewCsb->getChildByName<ImageView*>("image_bg");
	image_bg->loadTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_PALACE_AREA));

 	auto tips = SINGLE_SHOP->getTipsInfo();
 	std::string palace_type[] = { "", "PALACE", "CITYHALL", "CITYHALL" };
 	std::string prestige[] = { "", "MIN", "MID", "MAX", "LAR" };
 	int type = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].palace_type;
 	int p = 1;
	//宫殿声望4个等级对应NPC的提示语
	if (SINGLE_HERO->m_iPrestigeLv < PRESTIGE_MIN)
	{
		p = 1;
	}
	else if (SINGLE_HERO->m_iPrestigeLv < PRESTIGE_MID)
	{
		p = 2;
	}
	else if (SINGLE_HERO->m_iPrestigeLv < PRESTIGE_MAX)
	{
		p = 3;
	}
	else
	{
		p = 4;
	}
 	std::string tip_index = StringUtils::format("TIP_PALACE_%s_%s", palace_type[type].c_str(), prestige[p].c_str());
 	std::string content = tips[tip_index];
 	std::string new_vaule = SINGLE_HERO->m_sName;
	std::string old_vaule = "[heroname]";
 	repalce_all_ditinct(content, old_vaule, new_vaule);
	guardText->setString(content);
	auto t_palace = panel_actionbar->getChildByName<Text*>("label_palace");

	if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].palace_type == 1)
	{
		t_palace->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TITLE_PALACE"]);
	}
	else
	{
		t_palace->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TITLE_CITYHALL"]);
	}

	sprite_hand = Sprite::create();
	sprite_hand->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
	sprite_hand->setVisible(false);
	this->addChild(sprite_hand, 100);
	showMainCoin();
	scheduleOnce(schedule_selector(UIGuidePalace::buttonAction), 1);

}

void UIGuidePalace::anchorAction(float time)
{
	auto guard_dialog = getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB])->getChildByName("panel_palace")->getChildByName("panel_dialog");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(255);
	i_anchor->setVisible(false);
}

void UIGuidePalace::showMainCoin()
{
	auto viewBar = getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB])->getChildByName("panel_actionbar");
	auto bntSliver = viewBar->getChildByName<Button*>("button_silver");
	auto label_silvernum = bntSliver->getChildByName<Text*>("label_silver_num");
	auto bntGold = viewBar->getChildByName<Button*>("button_gold");
	auto label_goldnum = bntGold->getChildByName<Text*>("label_gold_num");
	label_silvernum->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iSilver)));
	label_goldnum->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iVp)));
	setTextFontSize(label_silvernum);
	setTextFontSize(label_goldnum);

}

void UIGuidePalace::palaceoutofscreen()
{
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB])->getChildByName("panel_palace");
	auto panelDialog = view->getChildByName("panel_dialog");
	auto roleGuard = panelDialog->getChildByName<ImageView*>("image_soldier");
	auto image_dialog_bg = panelDialog->getChildByName<ImageView*>("image_dialog_bg");
	auto labelguard = panelDialog->getChildByName("label_guard");
	auto guardText = panelDialog->getChildByName<Text*>("label_content");

	image_dialog_bg->runAction(FadeOut::create(0.1));
	labelguard->runAction(FadeOut::create(0.1));
	guardText->runAction(FadeOut::create(0.1));
	roleGuard->runAction(MoveTo::create(0.5f, Vec2(-600, 0)));

	auto i_anchor = panelDialog->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(0);
	i_anchor->stopAllActions();
	m_bDialogOut = true;//对话已退出
}

void UIGuidePalace::showBussinessliceView()
{
	//business license
	if (!isShowPopularGoods)
	{
		showPalacePopularGood();
	}
	auto viewCity = getViewRoot(PALACE_COCOS_RES[VIEW_CITYINFO_CSB]);
	auto listviewContent = viewCity->getChildByName("listview_content");
	auto panelContent = listviewContent->getChildByName("panel_content");
	auto panel_license_1 = dynamic_cast<Layout*>(Helper::seekWidgetByName((Widget*)panelContent, "panel_license_1"));
	auto image_div_3 = dynamic_cast<ImageView*>(Helper::seekWidgetByName((Widget*)panelContent, "image_div_3"));
	auto moveHight1 = panel_license_1->getContentSize().height;
	auto moveHight2 = image_div_3->getContentSize().height;

	auto label_require = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent, "label_require"));
	auto image_r = dynamic_cast<ImageView*>(Helper::seekWidgetByName((Widget*)panelContent, "image_r"));
	auto label_require_num = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent, "label_r_lv_0"));
	auto button_yesorno = dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)panelContent, "button_yesorno"));
	auto b_buy = dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)panelContent, "button_buy"));
	auto right_yesorno = dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)panelContent, "right_yesorno"));
	auto panel_pp = Helper::seekWidgetByName((Widget*)panelContent, "panel_pp");
	//own
	if (m_Haslisence)
	{
		panel_license_1->setVisible(false);
		image_div_3->setVisible(false);
		image_r->setVisible(false);
		label_require_num->setVisible(false);
		button_yesorno->setVisible(false);
		right_yesorno->setVisible(true);
		b_buy->setVisible(false);
		b_buy->setBright(false);
		label_require->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE_QWNED"]);
		label_require->setColor(Color3B(46, 125, 50));
		Vector<Node*>children;
		children = panelContent->getChildren();
		//购买完交易许可证后界面调整
		for (auto item : children)
		{
			std::string str = item->getName();
			if (str.compare("panel_pp") == 0 || str.compare("image_div_1") == 0 || str.compare("panel_license_1") == 0 || str.compare("image_div_3") == 0)
			{
				continue;
			}
			else
			{
				item->setPositionY(item->getPositionY() + (moveHight1 + moveHight2));
			}
		}
	}
	else
	{
		panelContent->setPosition(Vec2(0, 0));
		panel_license_1->setVisible(true);
		image_div_3->setVisible(true);
		image_r->setVisible(true);
		label_require_num->setVisible(true);
		button_yesorno->setVisible(true);
		b_buy->setVisible(true);
		b_buy->setBright(true);
		label_require->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE_REQUIRE"]);
		label_require->setColor(Color3B(46, 29, 14));
		if (SINGLE_HERO->m_iCoin < m_pPalaceResult->licenseprice)
		{
			right_yesorno->setVisible(false);
		}
		else
		{
			right_yesorno->setVisible(true);
		}
	}
	auto t_require = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent, "label_r_lv_0"));
	t_require->setString(StringUtils::format("%d", m_pPalaceResult->licenserequiredfamelv));
	auto b_require = dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)panelContent, "button_yesorno"));

	if (SINGLE_HERO->m_iPrestigeLv >= m_pPalaceResult->licenserequiredfamelv)
	{
		b_require->setBright(true);
	}
	else
	{
		b_require->setBright(false);
	}
	auto t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent, "Label_33"));
	t_silver->setString(numSegment(StringUtils::format("%d", m_lisenceprice)));
	t_silver->setTextHorizontalAlignment(TextHAlignment::LEFT);
	t_silver->ignoreContentAdaptWithSize(true);
	auto label_license_1 = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent, "label_license_1"));
	label_license_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE_NO"] + SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].name);
	auto label_license = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent, "label_license"));
	label_license->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE"] + SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].name);
}

void UIGuidePalace::guide()
{
	m_guideStage++;
	switch (m_guideStage)
	{
	case SELECT_CITYINFO:
	{
							setButtonsDisable(m_pRoot, "button_city_info");
							setButtonsDisable(this->getChildByName("panel_two_butter"), "button_city_info");
							auto b_cityInfo = m_pRoot->getChildByName<Widget*>("button_city_info");
							auto callfun = CallFunc::create([=]{focusOnButton(b_cityInfo); sprite_hand->setPositionX(sprite_hand->getPositionX()- 30);  });
							auto se = callfun;
							this->runAction(se);
							

	}
		break;
	case BUY_LIENCE:
	{
  					   auto l_view =  m_pRoot->getChildByName<ListView*>("listview_content");
					   l_view->setDirection(SCROLLVIEW_DIR_NONE);
					   sprite_hand->setVisible(false);
					   for (auto m_cpRoot:m_vRoots)
					   {
						   setButtonsDisable(m_cpRoot, "button_buy");					  
					   }
					   setButtonsDisable(this->getChildByName("panel_two_butter"));
					   auto b_buy = m_pRoot->getChildByName<Widget*>("button_buy");

					   b_buy->setVisible(true);
					   m_Haslisence = 0;
					   showBussinessliceView();
					   auto right_yesorno = dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)m_pRoot, "right_yesorno"));
					   right_yesorno->setVisible(false);
					   this->runAction(Sequence::createWithTwoActions(DelayTime::create(1), CallFunc::create([=]{focusOnButton(b_buy); })));
	}
		break;
	case BUY_CONFIRM:
		 	 {
						auto view = UICommon::getInstance()->getViewRoot(COMMOM_COCOS_RES[C_VIEW_COST_SILVER_COM_CSB]);
						CC_SAFE_RETAIN(view);
						view->removeFromParent();
						view->setLocalZOrder(10);
						view->setTag(101);
						this->addChild(view);
						setButtonsDisable(view,"button_s_yes");
						focusOnButton(view->getChildByName("button_s_yes"));
		 	 }
				 break;
	case BACKTO_CITY:
	{
						setButtonsDisable(getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB]), "button_backcity");
						auto b_back = getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB])->getChildByName<Widget*>("button_backcity");
						
						focusOnButton(b_back);
						b_back->setTouchEnabled(true);
	}
		break;
	default:
		break;
	}
}

void UIGuidePalace::setButtonsDisable(Node * node, std::string btn_name)
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

void UIGuidePalace::focusOnButton(Node * psender)
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
		if (name.compare("button_city_info") == 0)
		{
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2 * 0.6,
				c_psender->getWorldPosition().y - 8));
		}
		else
		{
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x - sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
		}
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
				sprite_hand->setRotation(70);
				sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
					c_psender->getWorldPosition().y + c_psender->getContentSize().height / 2 * 0.6 + sprite_hand->getContentSize().height / 2 * 0.6));
			}

		}

	}
	else if (pos.x > win.width / 2 && pos.y < win.height / 2)
	{
		if (name.compare("button_s_yes") == 0 || name.compare("button_buy") == 0)
		{
			sprite_hand->setRotation(-180);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
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

void UIGuidePalace::showPalacePopularGood()
{
		auto view = getViewRoot(PALACE_COCOS_RES[VIEW_CITYINFO_CSB]);
		isShowPopularGoods = true;
		if (!view) return;
		auto panel_pp = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_pp"));
		auto ppWidth = panel_pp->getContentSize().width;
		auto i_title = panel_pp->getChildByName<Text*>("label_popular_product");
		auto i_popular_product_no = panel_pp->getChildByName<Text*>("label_popular_product_no");
		i_popular_product_no->setString(SINGLE_SHOP->getTipsInfo()["TIP_MAINCITY_HAVE_NO_POPULAR_PRODUCT"]);
		auto panel_item_1 = panel_pp->getChildByName("panel_item_1");
		auto panel_item_2 = panel_pp->getChildByName("panel_item_2");
		auto panel_item_3 = panel_pp->getChildByName("panel_item_3");
		panel_item_1->setVisible(false);
		panel_item_2->setVisible(false);
		panel_item_3->setVisible(false);
		int  goodCount = ProtocolThread::GetInstance()->getPopularGoodsIds().size();
		if (!goodCount)
		{
			i_popular_product_no->setVisible(true);
			i_title->setVisible(false);
			auto listviewContent = view->getChildByName("listview_content");
			auto panelContent = listviewContent->getChildByName("panel_content");
			auto children = panelContent->getChildren();
			//没有流行品时调整宫殿信息界面布局
			for (auto item : children)
			{
				std::string str = item->getName();
				if (str.compare("panel_pp") == 0 || str.compare("image_div_1") == 0)
				{
					item->setVisible(false);
					continue;
				}
				else
				{
					item->setPositionY(item->getPositionY() + panel_pp->getContentSize().height);
				}
			}
		}
		else
		{
			i_popular_product_no->setVisible(false);
			i_title->setVisible(true);
			//调整显示的流行品位置 流行品个数 1,2,3
			if (goodCount == 1)
			{
				panel_item_1->setVisible(true);
				panel_item_1->setPositionX(ppWidth*1.0 / 2.5);
			}
			else if (goodCount == 2)
			{
				panel_item_1->setVisible(true);
				panel_item_2->setVisible(true);
				panel_item_1->setPositionX(ppWidth*1.0 / 4);
				panel_item_2->setPositionX(ppWidth*1.0 / 3 * 2);
			}
			else
			{
				panel_item_1->setVisible(true);
				panel_item_2->setVisible(true);
				panel_item_3->setVisible(true);
			}
			for (int i = 0; i < goodCount; i++)
			{
				auto goodId = ProtocolThread::GetInstance()->getPopularGoodsIds()[i];
				std::string panelIndex = StringUtils::format("panel_item_%d", i + 1);
				auto panelItem = panel_pp->getChildByName(panelIndex);
				auto button_good_bg = panelItem->getChildByName<Button*>("button_good_bg");
				button_good_bg->setTouchEnabled(true);
				button_good_bg->setTag(goodId);
				auto image_goods = button_good_bg->getChildByName<ImageView*>("image_goods");
				image_goods->ignoreContentAdaptWithSize(false);
				image_goods->loadTexture(getGoodsIconPath(goodId));
				auto label_goods_name = panelItem->getChildByName<Text*>("label_goods_name");
				label_goods_name->setString(SINGLE_SHOP->getGoodsData()[goodId].name);
			}
		}
}
void UIGuidePalace::buttonAction(float dt)
{
	auto  twoButter = this->getChildByName("panel_two_butter");
	auto btn_cityinfo = twoButter->getChildByName<Button*>("button_city_info");
	auto btn_hallofhonor = twoButter->getChildByName<Button*>("button_hall_of_honor");
	auto btn_investment = twoButter->getChildByName<Button*>("button_investment");
	auto btn_office = twoButter->getChildByName<Button*>("button_office");
	int offPosX = twoButter->getPositionX() - _director->getWinSize().width;
	btn_cityinfo->setPositionX(btn_cityinfo->getPositionX() - offPosX);
	btn_hallofhonor->setPositionX(btn_hallofhonor->getPositionX() - offPosX);
	btn_investment->setPositionX(btn_investment->getPositionX() - offPosX);
	btn_office->setPositionX(btn_office->getPositionX() - offPosX);
	btn_cityinfo->runAction(Sequence::create(MoveBy::create(0.2, Vec2(-btn_cityinfo->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	btn_hallofhonor->runAction(Sequence::create(DelayTime::create(0.05), MoveBy::create(0.2, Vec2(-btn_hallofhonor->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	btn_investment->runAction(Sequence::create(DelayTime::create(0.1), MoveBy::create(0.2, Vec2(-btn_investment->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	btn_office->runAction(Sequence::create(DelayTime::create(0.15), MoveBy::create(0.2, Vec2(-btn_office->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(1), CallFunc::create([=]{guide(); })));
}
void UIGuidePalace::buttonMoveRight()
{
	auto  twoButter = this->getChildByName("panel_two_butter");
	auto btn_cityinfo = twoButter->getChildByName<Button*>("button_city_info");
	auto btn_hallofhonor = twoButter->getChildByName<Button*>("button_hall_of_honor");
	auto btn_investment = twoButter->getChildByName<Button*>("button_investment");
	auto btn_office = twoButter->getChildByName<Button*>("button_office");
	btn_cityinfo->runAction(Sequence::create(DelayTime::create(0.05), MoveBy::create(0.2, Vec2(btn_cityinfo->boundingBox().size.width / 3 + 20, 0)), nullptr));
	btn_hallofhonor->runAction(Sequence::create(DelayTime::create(0.1), MoveBy::create(0.2, Vec2(btn_cityinfo->boundingBox().size.width / 3 + 20, 0)), nullptr));
	btn_investment->runAction(Sequence::create(DelayTime::create(0.15), MoveBy::create(0.2, Vec2(btn_cityinfo->boundingBox().size.width / 3 + 20, 0)), nullptr));
	btn_office->runAction(Sequence::create(DelayTime::create(0.2), MoveBy::create(0.2, Vec2(btn_cityinfo->boundingBox().size.width / 3 + 20, 0)), nullptr));
	m_btnMoved = true;
}
void UIGuidePalace::viewAction(Node * pSender, std::string viewPath)
{
	if (pSender)
	{
		m_curView = nullptr;
		m_viewruancomplete = false;
		m_InfoViewalreadyOpen = false;
		m_viewPath = viewPath;
		pSender->setLocalZOrder(10);
		pSender->setPosition(ENDPOS3);
		pSender->runAction(Sequence::create(DelayTime::create(0.4), MoveTo::create(0.4f, STARTPOS), CallFunc::create([=]{m_viewruancomplete = true; }), nullptr));
		m_curView = pSender;
	}

}
void UIGuidePalace::viewCloseAction()
{
	if (m_curView)
	{
		std::string str = m_viewPath.substr(0, std::string::npos);
		m_curView->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4, ENDPOS3), CallFunc::create([=]{closeView(str); })));
	}
}
void UIGuidePalace::changeMainButtonState(Widget*target)
{
	auto winsize = Director::getInstance()->getWinSize();
	if (!target) return;
	if (m_preMainButton)
	{
		m_preMainButton->runAction(MoveBy::create(0.2, Vec2(20, 0)));
		m_preMainButton->setBright(true);
		m_preMainButton->setTouchEnabled(true);
	}
	m_preMainButton = target;
	if (m_preMainButton)
	{
		m_preMainButton->runAction(MoveBy::create(0.2, Vec2(-20, 0)));
		m_preMainButton->setBright(false);
		m_preMainButton->setTouchEnabled(false);
	}
}
std::string UIGuidePalace::getPortTypeOrInfo(int port)
{
	int portType = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type;
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