#include "UICenterCharactorRole.h"
#include "UICommon.h"
#include "UIInform.h"

UICenterCharactorRole::UICenterCharactorRole()
	:m_pParent(nullptr),
	m_pInputText(nullptr),
	bIsEnter(false),
	m_pResult(nullptr)
{
	 
}

UICenterCharactorRole::~UICenterCharactorRole()
{

}

UICenterCharactorRole* UICenterCharactorRole::createRole(UIBasicLayer *parent)
{
	UICenterCharactorRole* csl = new UICenterCharactorRole;
	if (csl)
	{
		csl->m_pParent = parent;
		csl->init();
		csl->autorelease();
		return csl;
	}
	CC_SAFE_DELETE(csl);
	return nullptr;
}

bool UICenterCharactorRole::init()
{
	bool pRet = false;
	do 
	{
		m_pInputText = cocos2d::ui::EditBox::create(Size(100, 100), "input.png");
		m_pInputText->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
		m_pInputText->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
		m_pInputText->setTag(200);
		m_pInputText->setPosition(Vec2(-200,-200));
		m_pInputText->setDelegate(this);
		m_pParent->addChild(m_pInputText,100);

		pRet = true;
	} while (0);
	return pRet;

}

void UICenterCharactorRole::openCenterRole()
{
	ProtocolThread::GetInstance()->getPersonalInfo(UILoadingIndicator::create(m_pParent));
}

void UICenterCharactorRole::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* target = dynamic_cast<Widget*>(pSender);
		std::string name = target->getName();
		buttonEvent(target,name);
	}
}

void UICenterCharactorRole::buttonEvent(Widget *pSender,std::string name)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//个人信息界面编辑按钮
	if (isButton(button_editor))
	{
		if (m_pInputText)
		{
			auto parent = pSender->getParent();
			auto w_parent = dynamic_cast<Widget*>(parent);
			auto t_inputName = dynamic_cast<Text*>(w_parent->getChildByName<Widget*>("text_signature"));
			m_pInputText->setText((t_inputName->getString()).c_str());
			m_pInputText->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
		}
		return;
	}
	//获取个人信息失败提示框确认按钮
	if (isButton(button_error_yes))
	{
		return;
	}
	if (isButton(image_reputation) || isButton(image_silver_2) || isButton(image_silver_1) || isButton(image_portugal) || isButton(image_country)|| isButton(image_friendliness_status))
    {		
		UICommon::getInstance()->openCommonView(m_pParent);
		UICommon::getInstance()->flushImageDetail(pSender);
		return;
   }
	if (isButton(image_spain) || isButton(image_england) || isButton(image_venice) || isButton(image_holland) || isButton(image_sweden) || isButton(image_genoa) || isButton(image_bl))
	{
		UICommon::getInstance()->openCommonView(m_pParent);
		UICommon::getInstance()->flushImageDetail(pSender);
		return;
	}
	if (isButton(button_bl_details))
	{
		openBussinessView(m_pResult);
		return;
	}

	if (isButton(button_close))
	{
		m_pParent->closeView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_BL_DETAIL_CSB]);
		return;
	}


}


void UICenterCharactorRole::onServerEvent(struct ProtobufCMessage *message,int msgType)
{
	switch (msgType)
	{
	case PROTO_TYPE_GetPersonalInfoResult:
		{
			auto result = (GetPersonalInfoResult*)(message);
			if (result->failed == 0)
			{
				flushRoleInfomation(result);
			}else
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openConfirmYes("TIP_CENTER_GET_ROLE_FAIL");
			}
			break;
		}
	case PROTO_TYPE_SetBioInfoResult:
		{
			SetBioInfoResult *bioInfoResult = (SetBioInfoResult*)(message);
			if (bioInfoResult->failed == 0)
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_CHANGE_ROLL_SUCCESS");
			}
			break;
		}
	default:
		break;
	}
}

void UICenterCharactorRole::flushRoleInfomation(const GetPersonalInfoResult *pResult)
{
	m_pResult = pResult;
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_STASTICS_CSB]);
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_STASTICS_CSB]);
	auto i_icon_bg  =   view->getChildByName<ImageView*>("image_head_player_bg");
	auto i_icon_bg1 = view->getChildByName<ImageView*>("panel_daily_expenses");
	auto i_icon_bg2 = view->getChildByName<ImageView*>("panel_gold");
	auto i_icon_bg3 = view->getChildByName<ImageView*>("panel_bussiness_license");
	auto i_icon_bg4 = view->getChildByName<ImageView*>("panel_bank");

	auto i_icon     =   i_icon_bg->getChildByName<ImageView*>("image_head");
	auto p_exp      =   i_icon_bg->getChildByName<Widget*>("panel_exp");
	auto p_rep      =   i_icon_bg->getChildByName<Widget*>("panel_reputation"); 
	auto i_head_lv  =   i_icon_bg->getChildByName<ImageView*>("image_head_lv");
	auto t_lv       =   i_head_lv->getChildByName<Text*>("label_lv");
	auto t_name     =   dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_name"));
	auto i_country  =   dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_country"));
	auto t_exp      =   dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_exp_num"));
	auto t_r_lv     =   dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_lv_r"));
	auto t_prestige =   dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_reputation_num"));
	auto t_silver   =   dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_silver_num"));
	auto t_daily    =   dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_daily_expenses_num"));
	auto t_gold     =   dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_gold_num"));
	auto t_bank     =   dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_bank_num"));
	auto t_bl       =   dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_bussiness_license_num"));
	auto t_guild    =   dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_guild_name"));
	auto t_time     =   dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_time"));
	auto i_guild    =   dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"Image_61"));
	auto i_rep      =   dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_reputation"));
	auto i_sil = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_silver_2"));
	auto b_details = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_bl_details"));
	auto i_sil1 = i_icon_bg1->getChildByName<ImageView*>("image_silver_1");
	auto i_sil2 = i_icon_bg2->getChildByName<ImageView*>("image_silver_1");
	auto i_sil3 = i_icon_bg3->getChildByName<ImageView*>("image_bl");
	auto i_sil4 = i_icon_bg4->getChildByName<ImageView*>("image_silver_1");
	i_icon->ignoreContentAdaptWithSize(false);
	i_icon->loadTexture(getPlayerIconPath(pResult->iconid));
	t_name->setString(pResult->heroname);
	t_name->enableOutline(Color4B::BLACK,OUTLINE_MAX);
	t_lv->setString(StringUtils::format("%lld",pResult->level));
	b_details->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func, this));
	float temp_exp = 0;
	if (pResult->level < LEVEL_MAX_NUM)
	{
		temp_exp = (pResult->exp - LEVEL_TO_EXP_NUM(pResult->level))*1.0/(LEVEL_TO_EXP_NUM(pResult->level+1)-LEVEL_TO_EXP_NUM(pResult->level));
	}
	float temp_rep = 0;
	if (pResult->famelevel < LEVEL_MAX_NUM)
	{
		temp_rep = (pResult->fame - LEVEL_TO_FAME_NUM(pResult->famelevel))*1.0/(LEVEL_TO_FAME_NUM(pResult->famelevel+1)-LEVEL_TO_FAME_NUM(pResult->famelevel));
	}
	t_exp->setString(numSegment(StringUtils::format("%lld", pResult->exp)) + "/" + numSegment(StringUtils::format("%lld",LEVEL_TO_EXP_NUM(pResult->level + 1))));
	p_exp->setContentSize(Size(p_exp->getSize().width, 178 * temp_exp));
	t_prestige->setString(StringUtils::format("%lld/%lld", pResult->fame, LEVEL_TO_FAME_NUM(pResult->famelevel + 1)));
	t_prestige->setString(numSegment(StringUtils::format("%lld", pResult->fame)) + "/" + numSegment(StringUtils::format("%lld", LEVEL_TO_FAME_NUM(pResult->famelevel + 1))));

	i_rep->setTouchEnabled(true);
	i_rep->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func, this));
	i_rep->setTag(IMAGE_ICON_PRESTIGE + IMAGE_INDEX2);

	i_sil->setTouchEnabled(true);
	i_sil->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func, this));
	i_sil->setTag(IMAGE_ICON_COINS + IMAGE_INDEX1);

	i_sil1->setTouchEnabled(true);
	i_sil1->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func, this));
	i_sil1->setTag(IMAGE_ICON_COINS + IMAGE_INDEX1);

	i_sil2->setTouchEnabled(true);
	i_sil2->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func, this));
	i_sil2->setTag(IMAGE_ICON_VTICKET + IMAGE_INDEX1);

	i_sil3->setTouchEnabled(true);
	i_sil3->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func, this));
	i_sil3->setTag(IMAGE_ICON_LICENSE + IMAGE_INDEX1);

	i_sil4->setTouchEnabled(true);
	i_sil4->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func, this));
	i_sil4->setTag(IMAGE_ICON_COINS + IMAGE_INDEX1);

	i_country->ignoreContentAdaptWithSize(false);
	i_country->loadTexture(getCountryIconPath(pResult->nationid));
	i_country->setTouchEnabled(true);
	i_country->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func,this));
	i_country->setTag(pResult->nationid + IMAGE_ICON_CLOCK + IMAGE_INDEX2);
	t_guild->setString(pResult->guildname);
	t_r_lv->setString(StringUtils::format("%lld",pResult->famelevel));
	t_time->setString(StringUtils::format("%.1f %s", pResult->regtime / 3600.0, SINGLE_SHOP->getTipsInfo()["TIP_HOUR"].data()));
	t_daily->setString(numSegment(StringUtils::format("%lld",pResult->dailycost)));
	t_gold->setString(numSegment(StringUtils::format("%lld",pResult->golds)));
	t_bank->setString(numSegment(StringUtils::format("%lld", pResult->bankcoins)));
	t_bl->setString(numSegment(StringUtils::format("%d", pResult->n_ownlicensecities)));
	t_silver->setString(numSegment(StringUtils::format("%lld",pResult->coins)));
	if (pResult->guildid)
	{
		i_guild->setVisible(true);
		i_guild->ignoreContentAdaptWithSize(false);
		i_guild->loadTexture(getGuildIconPath(pResult->guildicon));
	}else
	{
		i_guild->setVisible(false);
	}

	for (int i = 1; i <  pResult->n_personal; i++)
	{
		for (int j = 0; j < pResult->n_personal-i; j++)
		{
			if (pResult->personal[j]->nationid > pResult->personal[j+1]->nationid)
			{
				auto temp = pResult->personal[j];
				pResult->personal[j] = pResult->personal[j+1];
				pResult->personal[j+1] = temp;
			}
		}
	}
	auto personal_friend = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"image_personal_friendliness"));
	ImageView *p_friend = nullptr;
	int p_friend_num = 0;
	for(int i = 1; i <= 7; i++)
	{
		switch(i)
		{
		case 1:
			p_friend = personal_friend->getChildByName<ImageView *>("image_portugal");
			p_friend->setTag(IMAGE_ICON_PORTUGAL+IMAGE_INDEX2);
			break;
		case 2:
			p_friend = personal_friend->getChildByName<ImageView *>("image_spain");
			p_friend->setTag(IMAGE_ICON_SPAIN+IMAGE_INDEX2);
			break;
		case 3:
			p_friend = personal_friend->getChildByName<ImageView *>("image_england");
			p_friend->setTag(IMAGE_ICON_ENGLAND+IMAGE_INDEX2);
			break;
		case 4:
			p_friend = personal_friend->getChildByName<ImageView *>("image_venice");
			p_friend->setTag(IMAGE_ICON_NETHERLANDS+IMAGE_INDEX2);
			break;
		case 5:
			p_friend = personal_friend->getChildByName<ImageView *>("image_holland");
			p_friend->setTag(IMAGE_ICON_GENOVA+IMAGE_INDEX2);
			break;
		case 6:
			p_friend = personal_friend->getChildByName<ImageView *>("image_sweden");
			p_friend->setTag(IMAGE_ICON_VENICE+IMAGE_INDEX1);
			break;
		case 7:
			p_friend = personal_friend->getChildByName<ImageView *>("image_genoa");
			p_friend->setTag(IMAGE_ICON_SWEDEN+IMAGE_INDEX1);
			break;
		default:
			break;
		}
		p_friend->loadTexture(getCountryIconPath(i));
		p_friend->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func,this));
		p_friend->setTouchEnabled(true);
		if (p_friend != nullptr)
		{
			if (pResult->personal[i - 1]->value < 0 && pResult->personal[i - 1]->value >= -100000)
			{
				p_friend_num = -1;
			}
			else if (pResult->personal[i - 1]->value < -100000)
			{
				p_friend_num = pResult->personal[i - 1]->value / 100000.0 - 0.5;
			}else
			{
				p_friend_num = pResult->personal[i - 1]->value / 100000.0 + 0.5;
			}

			p_friend->loadTexture(getCountryIconPath(pResult->personal[i-1]->nationid));
			auto  i_friend = p_friend->getChildByName<ImageView*>("image_friendliness_status");
			if (p_friend_num >= 0)
			{
				i_friend->loadTexture(ICON_FRIEND_NPC);
				i_friend->setTouchEnabled(true);
				i_friend->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func, this));
				i_friend->setTag(IMAGE_ICON_FRIEND + IMAGE_INDEX1);
			}else
			{
				i_friend->loadTexture(ICON_ENEMY_NPC);
				i_friend->setTouchEnabled(true);
				i_friend->addTouchEventListener(CC_CALLBACK_2(UICenterCharactorRole::menuCall_func, this));
				i_friend->setTag(IMAGE_ICON_ENEMY+ IMAGE_INDEX1);
			}
			Text *t_num = p_friend->getChildByName<Text*>("label_num");
			t_num->setString(String::createWithFormat("%d",p_friend_num)->_string);
		}
	}
	auto l_signature = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"text_signature"));
	if (pResult->bio)
	{
		if (strcmp(pResult->bio, "") != 0)
		{
			l_signature->setString(pResult->bio);
		}
		else
		{
			m_stringbio = l_signature->getString();
		}
	}

	auto listView = view->getChildByName<ListView*>("listview_infomation");
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(listView,image_pulldown);
//chengyuan++
	auto p_CInfo = listView->getItem(0);
	auto p_static = p_CInfo->getChildByName("panel_stastics");

	for (auto p_item : p_static->getChildren())
	{
		ImageView * i_silver = ImageView::create();
		Text * t_label = Text::create();
		if (p_item->getName().find("panel_") == 0)
		{
			for(Node*i : p_item->getChildren())
			{
				
				if (i->getName().find("image_") == 0 || i->getName().find("Image_") == 0)
				{
					i_silver = (ImageView*)i;
				}
				else if (i->getName().find("label_") == 0)
				{
				     if (t_label->getName().size() < i->getName().size())
				     {
						 t_label = (Text*)i;
				     }
				}
			}
			if (p_item->getName().find("panel_reputation") == 0 )
			{
				t_label = p_item->getChildByName<Text*>("label_reputation_num");
				t_label->ignoreContentAdaptWithSize(true);
				t_label->setTextHorizontalAlignment(TextHAlignment::RIGHT);
				auto t_r_lv = p_item->getChildByName<Text*>("label_lv_r");
				auto i_bg = p_item->getChildByName("image_lv_bg_r");
				auto i_re = p_item->getChildByName("image_reputation");
				t_r_lv->setString(StringUtils::format("Lv. %lld", pResult->famelevel));		
				t_label->setPositionX(i_bg->getPositionX() - i_bg->getContentSize().width / 2 - 20);
				i_re->setPositionX(t_label->getPositionX() - t_label->getContentSize().width - i_re->getContentSize().width / 2 - 20);

			}
			else if (p_item->getName().find("panel_exp") == 0)
			{				
				t_label->ignoreContentAdaptWithSize(true);
				t_label->setTextHorizontalAlignment(TextHAlignment::RIGHT);
				auto t_lv = p_item->getChildByName<Text*>("label_lv");
				auto i_bg = p_item->getChildByName("image_lv_bg");
				t_lv->setString(StringUtils::format("Lv. %lld", pResult->level));
				t_label->setPositionX(i_bg->getPositionX() - i_bg->getContentSize().width / 2 - 20);
			}
			else
			{
				t_label->ignoreContentAdaptWithSize(true);
				t_label->setTextHorizontalAlignment(TextHAlignment::RIGHT);
				if ((i_silver->getName().find("image_") == 0 || i_silver->getName().find("Image_") == 0 )&& t_label->getName().find("label") == 0)
				{
					std::string name = t_label->getName();
					i_silver->setPositionX(t_label->getPositionX() - t_label->getContentSize().width - i_silver->getContentSize().width);
				}
		
			}
		}
	}

	UserDefault::getInstance()->setBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(MAIN_CITY_NEW_POINT).c_str(), false);
	UserDefault::getInstance()->flush();
	auto view1 = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_CSB]);
	if (view1)
	{
		auto i_notify = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view1,"image_notify"));
		if (UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(SKILL_NEW_POINT).c_str()))
		{
			i_notify->setVisible(true);
		}
		else
		{
			i_notify->setVisible(false);
		}
	}
}

void UICenterCharactorRole::textFiledEvent(Ref *target, bool type)
{
	std::string value = m_pInputText->getText();
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_STASTICS_CSB]);
	auto i_signature = view->getChildByName<ImageView*>("image_signature_bg");
	auto l_signature = i_signature->getChildByName<Text*>("text_signature");
	if (illegal_character_check(value))
	{
		value = "";
		m_pInputText->setText(value.c_str());
		UIInform::getInstance()->openInformView(m_pParent);
		UIInform::getInstance()->openConfirmYes("TIP_INPUT_ILLEGAL_CHARACTER");
		l_signature->setString(value);
		return;
	}
	auto Signheight = getLabelHight(value, l_signature->getBoundingBox().size.width, l_signature->getFontName());
	if (type)
	{
		bIsEnter = false;
		if (Signheight>66)
		{
			l_signature->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_ROLE_PERSONAL_SIGN_CONTENT"]);
			UIInform::getInstance()->openInformView(m_pParent);
			UIInform::getInstance()->openViewAutoClose("TIP_CENTER_ROLE_PERSONAL_SIGN_TOO_LONG");
		}
		else
		{

			l_signature->setString(value);
			char *str = (char *)value.c_str();
			if (value == m_stringbio)
			{
				return;
			}
			ProtocolThread::GetInstance()->setBioInfo(str, UILoadingIndicator::create(m_pParent));
		}
	}
}
void UICenterCharactorRole::editBoxReturn(ui::EditBox * editBox)
{
	if (editBox->getTag() == m_pInputText->getTag())
	{
		bIsEnter = true;
		this->textFiledEvent(editBox, bIsEnter);
	}
}
void UICenterCharactorRole::openBussinessView(const GetPersonalInfoResult *pResult)
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_BL_DETAIL_CSB]);
	auto panel_detail = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_CHARACTOR_SHIP_PRO_BL_DETAIL_CSB]);
	auto listview_detail = panel_detail->getChildByName<ListView*>("listview_content");
	//已拥有的
	auto panel_content_0 = dynamic_cast<Layout*>(Helper::seekWidgetByName(panel_detail, "panel_content_1"));
	int num1 = pResult->n_ownlicensecities;
	int* ids1 = pResult->ownlicensecities;
	for (int i = 0; i < num1; i++)
	{
		auto panel = dynamic_cast<Layout*>(panel_content_0->clone());
		auto label_trade = panel->getChildByName<Text*>("label_trade");
		auto image_country = panel->getChildByName<ImageView*>("image_country");
		auto image_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel, "image_silver"));
		auto label_trade_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel, "label_trade_0"));
		image_silver->setPositionX(label_trade_0->getPositionX() - label_trade_0->getContentSize().width - image_silver->getContentSize().width / 2 - 12);
		for (int j = 0; j < pResult->n_cityinfo; j++)
		{
			if (pResult->cityinfo[j]->city_id == ids1[i])
			{
				image_country->loadTexture(getCountryIconPath(pResult->cityinfo[j]->nation));
			}
		}
		label_trade->setString(SINGLE_SHOP->getCitiesInfo()[ids1[i]].name);
		listview_detail->insertCustomItem(panel, i);
		panel->setVisible(true);
	}

	//已失效的
	auto panel_content_1_0 = dynamic_cast<Layout*>(Helper::seekWidgetByName(panel_detail, "panel_content_1_0"));
	int num2 = pResult->n_loselicensecities;
	int* ids2 = pResult->loselicensecities;
	for (int i = 0; i < num2; i++)
	{
		auto panel = dynamic_cast<Layout*>(panel_content_1_0->clone());
		auto label_trade1 = panel->getChildByName<Text*>("label_trade");
		auto image_country1 = panel->getChildByName<ImageView*>("image_country");
		auto image_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel, "image_silver"));
		auto label_trade_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel, "label_trade_0"));
		image_silver->setPositionX(label_trade_0->getPositionX() - label_trade_0->getContentSize().width - image_silver->getContentSize().width / 2 - 12);
		for (int j = 0; j < pResult->n_cityinfo; j++)
		{
			if (pResult->cityinfo[j]->city_id == ids2[i])
			{
				image_country1->loadTexture(getCountryIconPath(pResult->cityinfo[j]->nation));
			}
		}
		label_trade1->setString(SINGLE_SHOP->getCitiesInfo()[ids2[i]].name);
		listview_detail->insertCustomItem(panel, num1 + i);
		setGLProgramState(image_silver, true);
		panel->setVisible(true);
	}

	//未拥有的
	auto panel_content_2_1 = dynamic_cast<Layout*>(Helper::seekWidgetByName(panel_detail, "panel_content_1_1"));
	int num3 = pResult->n_withoutlicensecities;
	int* ids3 = pResult->withoutlicensecities;
	for (int i = 0; i < num3; i++)
	{
		auto panel = dynamic_cast<Layout*>(panel_content_2_1->clone());
		auto label_trade2 = panel->getChildByName<Text*>("label_trade");
		auto image_country2 = panel->getChildByName<ImageView*>("image_country");
		for (int j = 0; j < pResult->n_cityinfo; j++)
		{
			if (pResult->cityinfo[j]->city_id == ids3[i])
			{
				image_country2->loadTexture(getCountryIconPath(pResult->cityinfo[j]->nation));
			}
		}
		label_trade2->setString(SINGLE_SHOP->getCitiesInfo()[ids3[i]].name);
		listview_detail->insertCustomItem(panel, num1 + num2 + i);
		panel->setVisible(true);
	}
	auto image_pulldown = panel_detail->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(listview_detail, image_pulldown);
	listview_detail->removeLastItem();
	listview_detail->removeLastItem();
	listview_detail->removeLastItem();
	listview_detail->refreshView();
}


