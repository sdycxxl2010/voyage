
#include "UIVillage.h"
#include "UICommon.h"
#include "UIMain.h"
#include "UIInform.h"
#include "UIVoyageSlider.h"

#include "TVSceneLoader.h"

UIVillage::UIVillage()
{
	viilageUiFlag = -1;
	hireSailorTag = -1;
	m_pBarInfoResult = nullptr;
	m_pAvailCrewNumResult = nullptr;
	m_bIsBanquet = false;
	m_bIsHint = false;
}

UIVillage::~UIVillage()
{
	m_pBarInfoResult = nullptr;
	m_pAvailCrewNumResult = nullptr;
}
UIVillage* UIVillage::createVillage(int tag)
{
	UIVillage* villagelayer = new UIVillage;
	villagelayer->viilageUiFlag = tag;
	CC_RETURN_IF(villagelayer);
	if (villagelayer->init())
	{
		villagelayer->autorelease();
		return villagelayer;
	}
	CC_SAFE_RELEASE(villagelayer);
	return nullptr;
}
bool UIVillage::init()
{
	if (UIBasicLayer::init())
	{
		do
		{
			CC_BREAK_IF(!UIBasicLayer::init());
			ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIVillage::onServerEvent, this), this);

			if (viilageUiFlag ==VILLAGE_UI_PALACE)
			{
				flushVillageOffice();
			}
			else if (viilageUiFlag == VILLAGE_UI_TARVEN)
			{
				ProtocolThread::GetInstance()->getBarInfo(0, UILoadingIndicator::create(this));
			}

		} while (0);
		return true;
	}
	return false;
}

void UIVillage::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();

	if (isButton(button_invest_to))//投资框
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openView(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB]);
		auto investDialog = getViewRoot(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB]);
		investDialog->setTouchEnabled(true);
		auto imagePut = investDialog->getChildByName("image_input_bg");
		auto inputCurrency_num = imagePut->getChildByName<Text*>("label_currency_num");
		inputCurrency_num->addTouchEventListener(this, toucheventselector(UIVillage::menuCall_func));
		inputCurrency_num->setString("0");

		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->showNumpad(inputCurrency_num);
		return;
	}
	if (isButton(button_yes))//确定投资
	{
		auto imagePut = getViewRoot(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB])->getChildByName("image_input_bg");
		auto inputCurrency_num = imagePut->getChildByName<Text*>("label_currency_num");
		auto s_num = inputCurrency_num->getString();
		int64_t investCoinNum = 0;
		if (isNumber(s_num))
		{
			investCoinNum = atoi(s_num.c_str());
			if (investCoinNum >= 10000 && investCoinNum <= SINGLE_HERO->m_iCoin)
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
				inputCurrency_num->setString("0");
				closeView();
			}
			else
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
				if (investCoinNum > SINGLE_HERO->m_iCoin)
				{
					openSuccessOrFailDialog("TIP_PALACE_INPUT_TOO_MUCH");
					inputCurrency_num->setString("0");
				}
				else
				{
					openSuccessOrFailDialog("TIP_PALACE_INPUT_MIN");
					inputCurrency_num->setString("0");
				}
			}
		}
		return;
	}
	if (isButton(label_currency_num))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->showNumpad((Text*)button);
		return;
	}
	//if (isButton(button_no))//关闭村庄投资
	//{
	//	closeView(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB]);
	//	return;
	//}
	if (isButton(button_normal_hire))//普通雇佣水手
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		hireSailorTag = CONFIRM_INDEX_NORMAL_HIRE;
		ProtocolThread::GetInstance()->getAvailCrewNum(UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_host_hire))//喝酒雇佣水手
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		hireSailorTag = CONFIRM_INDEX_HOST_DRINK_HIRE;
		ProtocolThread::GetInstance()->getAvailCrewNum(UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_ok))//雇佣水手确认
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		UIVoyageSlider *ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
		ProtocolThread::GetInstance()->getCrew(ss->getCurrentNumber(), UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_result_yes))//雇佣水手结果界面按键
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		if (m_pAvailCrewNumResult->currentcrewnum == m_pAvailCrewNumResult->maxcrewnum)
		{
			openSuccessOrFailDialog("TIP_PUP_SHIP_NOT_CAP");
			return;
		}
		ProtocolThread::GetInstance()->getAvailCrewNum(UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_s_yes))//确定宴请
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		if (hireSailorTag == CONFIRM_INDEX_HOST_DRINK_HIRE)
		{
			ProtocolThread::GetInstance()->barConsume(8, 3, UILoadingIndicator::create(this));
		}
		return;
	}
	if (isButton(button_info))//村庄王宫里的问号解释
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_BANK_MONENY_INFO_TITLE", "TIP_BANK_MONENY_INFO_CONTENT");
		return;
	}
	if (isButton(button_caveat))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushWarning(SAILOR_URGENT_WARNING);
		return;
	}
	if (isButton(button_income_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PUP_SAILORS_INCOME_TITLE", "TIP_PUP_SAILORS_INCOME_CONTENT");
		return;
	}
	if (isButton(button_maxday_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PUP_SAILORS_MAXDAY_TITLE", "TIP_PUP_SAILORS_MAXDAY_CONTENT");
		return;
	}
	if (isButton(button_cancel))//雇佣水手取消
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
	if (isButton(button_village_close))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bIsHint)
		{
			m_bIsHint = false;
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_PUP_EXIT_PUP");
			return;
		}
		else
		{
			closeView();
			ProtocolThread::GetInstance()->unregisterMessageCallback(this);
			this->removeFromParentAndCleanup(true);
		}
		return;
	}
	if (isButton(button_error_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		return;
	}
}
//村庄王宫
void UIVillage::flushVillageOffice()
{
	openView(VILLAGE_COCOS_RES[VIEW_VILLAGE_GOVERNOR_CSB]);
	auto viewVillage = getViewRoot(VILLAGE_COCOS_RES[VIEW_VILLAGE_GOVERNOR_CSB]);
	auto label_title = dynamic_cast<Text*>(Helper::seekWidgetByName(viewVillage, "label_title"));
	label_title->setString(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].name);//村庄名字
	label_title->enableOutline(OUTLINE_COLOR, OUTLINE_MAX);

	auto listview_content = viewVillage->getChildByName<ListView*>("listview_content");
	listview_content->setBounceEnabled(true);
	auto loadingbar_1 = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(viewVillage, "loadingbar_1"));
	loadingbar_1->setPercent(30);//进度条
	auto label_invest_percent = dynamic_cast<Text*>(Helper::seekWidgetByName(viewVillage, "label_invest_percent"));
	label_invest_percent->setString(StringUtils::format("%.2f%%", 123 * 1.0 / 999 * 100));//进度条数字
	auto label_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(viewVillage, "label_silver_num"));//已投资的银币
	label_silver_num->setString(StringUtils::format("%d/%d", 30, 100));

	auto image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewVillage, "image_head"));
	image_head->ignoreContentAdaptWithSize(false);
	//image_head->loadTexture(getPlayerIconPath(m_pCityDataResult->data->iconidx));//头像

	auto your_contryRank = dynamic_cast<Text*>(Helper::seekWidgetByName(viewVillage, "label_you_rank_num"));
	your_contryRank->setString(StringUtils::format("No.%d", 3));//国家排名

	auto you_total_invest_num = dynamic_cast<Text*>(Helper::seekWidgetByName(viewVillage, "label_you_total_invest_num"));
	you_total_invest_num->setString(StringUtils::format("%d", 300000));//我的投资银币数
	flushVillageRank();

}
void UIVillage::flushVillageRank()
{
	auto viewVillage = getViewRoot(VILLAGE_COCOS_RES[VIEW_VILLAGE_GOVERNOR_CSB]);
	auto listview_3 = dynamic_cast<ListView*>(Helper::seekWidgetByName(viewVillage, "listview_3"));//排行表
	auto panel_list_bg_1 = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewVillage, "panel_list_bg_1"));//排行表
	for (int i = 0; i < 25; i++)
	{
		auto item = panel_list_bg_1->clone();
		item->setTag(i + START_INDEX);
		item->setVisible(true);
		listview_3->pushBackCustomItem(item);

		auto label_rank = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_rank"));//排名
		auto image_country = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_country"));//势力
		auto label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_name"));//势力名字
		auto label_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(item, "label_silver_num"));//势力银币

		auto rank_Image = dynamic_cast<ImageView*>(Helper::seekWidgetByName(item, "image_trophy"));//奖杯
		rank_Image->setVisible(false);
		if (i == 0)
		{
			rank_Image->setVisible(true);
			rank_Image->loadTexture(RANK_GOLD);
			label_rank->setColor(Color3B::WHITE);
		}
		else if (i == 1)
		{
			rank_Image->setVisible(true);
			rank_Image->loadTexture(RANK_SILVER);
			label_rank->setColor(Color3B::WHITE);
		}
		else if (i == 2)
		{
			rank_Image->setVisible(true);
			rank_Image->loadTexture(RANK_COPPER);
			label_rank->setColor(Color3B::WHITE);
		}

	}
	listview_3->refreshView();
}
void UIVillage::flushVillagePup()
{
	openView(VILLAGE_COCOS_RES[VIEW_VILLAGE_TARVEN_CSB]);
	auto sailor = getViewRoot(VILLAGE_COCOS_RES[VIEW_VILLAGE_TARVEN_CSB]);
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(sailor, "label_title"));
	auto t_content = sailor->getChildByName<Text*>("label_content");
	auto b_chat1 = dynamic_cast<Button*>(Helper::seekWidgetByName(sailor, "button_host_hire"));
	auto b_chat2 = dynamic_cast<Button*>(Helper::seekWidgetByName(sailor, "button_normal_hire"));
	auto t_chat1 = b_chat1->getChildByName<Text*>("label_1");
	auto t_content1 = b_chat1->getChildByName<Text*>("label_content");
	auto i_silver = b_chat1->getChildByName<Text*>("image_silver");
	auto t_silver = b_chat1->getChildByName<Text*>("label_silver_num");
	auto t_silver_0 = b_chat1->getChildByName<Text*>("label_hosted");
	auto t_chat2 = b_chat2->getChildByName<Text*>("label_2");
	auto t_content2 = b_chat2->getChildByName<Text*>("label_content");

	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_TITLE"]);
	int n = 30 * (ceil(FAME_NUM_TO_LEVEL(m_pBarInfoResult->fame) / 10.0) + 1);
	std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT"];
	std::string new_vaule = StringUtils::format("%d", n);
	std::string old_vaule = "[num]";
	repalce_all_ditinct(content, old_vaule, new_vaule);
	t_content->setString(content);
	t_chat1->setString("1.");
	t_chat2->setString("2.");
	t_content2->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT1"]);
	t_content1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILORS_CONTENT2"]);
	t_silver->setString(StringUtils::format("%d", m_pBarInfoResult->treatforgetcrewcost));
	b_chat1->addTouchEventListener(this,toucheventselector(UIVillage::menuCall_func));
	b_chat2->addTouchEventListener(this, toucheventselector(UIVillage::menuCall_func));

	auto i_bargirl = sailor->getChildByName<ImageView*>("image_bargirl");
	i_bargirl->ignoreContentAdaptWithSize(false);
	i_bargirl->loadTexture(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_BAR_GIRL));
	if (m_bIsBanquet)
	{
		i_silver->setVisible(false);
		t_silver->setVisible(false);
		t_silver_0->setVisible(true);
	}
	else
	{
		i_silver->setVisible(true);
		t_silver->setVisible(true);
		t_silver_0->setVisible(false);
	}
}
void UIVillage::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetBarInfoResult:
	{
		GetBarInfoResult* result = (GetBarInfoResult *)message;
		m_pBarInfoResult = result;
		m_nCurSailorNum = m_pBarInfoResult->totalcrewnum;//当前水手数
	   if (m_pBarInfoResult->failed == 0)
	   {
		   flushVillagePup();
	   }
	   else
	   {
		   ProtocolThread::GetInstance()->getBarInfo(0, UILoadingIndicator::create(this));
	   }
		break;
    }
	case PROTO_TYPE_GetAvailCrewNumResult:
	{
		GetAvailCrewNumResult* result = (GetAvailCrewNumResult *)message;
		if (result->failed == 0)
		{
			if (result->currentcrewnum == result->maxcrewnum)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PUP_CREW_FULL");
			}
			else
			{
				//普通雇佣
				if (hireSailorTag == CONFIRM_INDEX_NORMAL_HIRE)
				{
					m_pAvailCrewNumResult = result;
					if (m_pAvailCrewNumResult->maxcrewnum <= m_pAvailCrewNumResult->currentcrewnum)
					{
						//crew full.
						UIInform::getInstance()->openInformView(this);
						UIInform::getInstance()->openConfirmYes("TIP_PUP_SHIP_NOT_CAP");
					}
					else
					{
						openSailorConfirm(result);
					}
				}
				else//高级招募
				{
					m_pAvailCrewNumResult = result;
					if (m_pAvailCrewNumResult->maxcrewnum <= m_pAvailCrewNumResult->currentcrewnum)
					{
						//crew full.
						UIInform::getInstance()->openInformView(this);
						UIInform::getInstance()->openConfirmYes("TIP_PUP_SHIP_NOT_CAP");
					}
					else
					{
						if (!m_bIsBanquet)
						{
							UICommon::getInstance()->openCommonView(this);
							UICommon::getInstance()->flushSilverConfirmView("TIP_PUP_FOODANDDRINK_TITLE", "TIP_PUP_FOODANDDRINK_CONTENT", m_pBarInfoResult->treatforgetcrewcost);
						}
						else
						{
							openSailorConfirm(result);
						}
					}
				}
			}
		}
		else if (result->failed == COIN_NOT_FAIL)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
		}
		else if (result->failed == 3)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_EXCHNAGE_NOT_SHIP");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_PUP_CREW_FULL");
		}
		break;
	}
	case PROTO_TYPE_GetCrewResult:
	{
		GetCrewResult *result = (GetCrewResult *)message;
		if (result->failed == 0)
		{
			m_nCurSailorNum += result->crewnum;
			int sailorNum = 0;
			for (int i = 0; i < result->n_shipcrew; i++)
			{
				sailorNum += result->shipcrew[i]->crewmax;
			}

			if (m_nCurSailorNum == sailorNum)
			{
				m_bIsHint = false;
			}
			closeView();
			auto &tips = SINGLE_SHOP->getTipsInfo();
			openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB], 11);
			auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
			view->getChildByName<Text*>("label_title")->setString(tips["TIP_PUP_SAILOR_RECRUIT_SUCCESS"]);
			auto p_result = view->getChildByName<Widget*>("panel_result");
			auto l_result = view->getChildByName<ListView*>("listview_result");
			auto image_div = p_result->getChildByName<ImageView*>("image_div_1");
			auto p_item = p_result->getChildByName<Widget*>("panel_silver");
			auto p_sailor = p_item->clone();
			p_sailor->getChildByName<Text*>("label_force_relation")->setString(tips["TIP_PUP_SAILOR_RECRUIT_TITLE"]);
			auto i_sailor = p_sailor->getChildByName<ImageView*>("image_silver");
			i_sailor->loadTexture("res/shipAttIcon/att_6.png");
			i_sailor->setColor(Color3B(58, 32, 2));
			p_sailor->getChildByName<Text*>("label_buy_num")->setString(StringUtils::format("+%d", result->crewnum));
			l_result->pushBackCustomItem(p_sailor);
			auto image_div_1 = image_div->clone();
			l_result->pushBackCustomItem(image_div_1);

			auto p_cost = p_item->clone();
			p_cost->getChildByName<Text*>("label_force_relation")->setString(tips["TIP_PUP_SAILOR_RECRUIT_FEE"]);
			p_cost->getChildByName<Text*>("label_buy_num")->setString(StringUtils::format("%d", m_pBarInfoResult->hirecrewfee));
			l_result->pushBackCustomItem(p_cost);
			auto image_div_2 = image_div->clone();
			l_result->pushBackCustomItem(image_div_2);

			auto p_fee = p_item->clone();
			p_fee->getChildByName<Text*>("label_force_relation")->setString(tips["TIP_PUP_SAILOR_RECRUIT_COST"]);
			p_fee->getChildByName<Text*>("label_buy_num")->setString(StringUtils::format("%d", result->cost));
			l_result->pushBackCustomItem(p_fee);
			auto image_div_3 = image_div->clone();
			l_result->pushBackCustomItem(image_div_3);

			updateMainCityCoin(result->coin, result->gold);
		}
		else if (result->failed == 1)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_PUP_SHIP_NOT_CAP");
		}
		else if (result->failed == COIN_NOT_FAIL)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_PUP_BUY_CREW_FAIL");
		}
		break;
	}
	case PROTO_TYPE_BarConsumeResult:
	{
	BarConsumeResult* result = (BarConsumeResult *)message;
	if (result->failed == 0)
	{
		updateMainCityCoin(result->coin, result->gold);
		if (result->actioncode == 3)
		{
			auto view = getViewRoot(VILLAGE_COCOS_RES[VIEW_VILLAGE_TARVEN_CSB]);
			m_bIsBanquet = true;
			m_bIsHint = true;
			Helper::seekWidgetByName(view, "image_silver")->setVisible(false);
			Helper::seekWidgetByName(view, "label_silver_num")->setVisible(false);
			Helper::seekWidgetByName(view, "label_hosted")->setVisible(true);
		
			ProtocolThread::GetInstance()->getAvailCrewNum(UILoadingIndicator::create(this));
		}
	}
	else if (result->failed == COIN_NOT_FAIL)
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
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
void UIVillage::openSailorConfirm(const GetAvailCrewNumResult* result)
{
	openView(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB], 11);
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB]);
	auto t_content = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_content"));
	initSailorDialog();
	int nNum = 30 * (ceil(FAME_NUM_TO_LEVEL(m_pBarInfoResult->fame) / 10.0) + 1);
	if (m_bIsBanquet)
	{
		nNum *= 3;
	}
	std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PUP_HIRE_SAILORS_CONTENT"];
	std::string new_vaule = StringUtils::format("%d", nNum);
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

	UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
	if (m_pAvailCrewNumResult->availcrewnum == 0)
	{
		dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_plus"))->setTouchEnabled(false);
		dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_minus"))->setTouchEnabled(false);
		dynamic_cast<Slider*>(Helper::seekWidgetByName(view, "slider_goods_num"))->setTouchEnabled(false);
	}
	ss->setMaxPercent(m_pAvailCrewNumResult->availcrewnum);
	ss->setCurrentNumber(m_pAvailCrewNumResult->availcrewnum);
	t_selnum->setString(StringUtils::format("%d/%d", m_pAvailCrewNumResult->availcrewnum, m_pAvailCrewNumResult->availcrewnum));
	t_cost->setString(StringUtils::format("%d", m_pAvailCrewNumResult->crewprice * ss->getCurrentNumber() + m_pBarInfoResult->hirecrewfee));
	t_sailor1->setString(StringUtils::format("%d", m_pAvailCrewNumResult->currentcrewnum + m_pAvailCrewNumResult->availcrewnum));
	t_sailor2->setString(StringUtils::format("/%d", m_pAvailCrewNumResult->maxcrewnum));
	t_days->setString(StringUtils::format("%d", m_pAvailCrewNumResult->totalsupply / (m_pAvailCrewNumResult->onecrewsupply *
		(m_pAvailCrewNumResult->currentcrewnum + m_pAvailCrewNumResult->availcrewnum))));
	t_required->setString(StringUtils::format("%d", m_pAvailCrewNumResult->totalrequired));
	l_sailor->setPercent(100.0 * (m_pAvailCrewNumResult->currentcrewnum) / m_pAvailCrewNumResult->maxcrewnum);
	l_sailor_add->setPercent(100.0 * (m_pAvailCrewNumResult->currentcrewnum + m_pAvailCrewNumResult->availcrewnum) / m_pAvailCrewNumResult->maxcrewnum);
	b_caveat->addTouchEventListener(this, toucheventselector(UIVillage::menuCall_func));
	if (m_pAvailCrewNumResult->totalrequired <= m_pAvailCrewNumResult->currentcrewnum + m_pAvailCrewNumResult->availcrewnum)
	{
		t_sailor1->setTextColor(Color4B(40, 25, 13, 255));
		b_caveat->setVisible(false);
	}
	else
	{
		t_sailor1->setTextColor(Color4B(255, 0, 0, 255));
		b_caveat->setVisible(true);
	}

	auto b_cancel = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_cancel"));
	auto b_ok = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_ok"));

	auto b_income = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_income_info"));
	auto b_maxday = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_maxday_info"));
	liew_sailor->refreshView();
	liew_days->refreshView();
	liew_required->refreshView();
}
void UIVillage::initSailorDialog()
{
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB]);;
	auto w_slider = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_select_num"));
	auto ss = dynamic_cast<UIVoyageSlider*>(getChildByTag(1));
	if (ss)
	{
		ss->removeFromParentAndCleanup(true);
	}
	ss = UIVoyageSlider::create(w_slider, 100, 0, true);
	this->addChild(ss, 1, 1);
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIVillage::sliderSailor, this));
}
void UIVillage::sliderSailor(const int nNum)
{
	UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
	auto view = getViewRoot(TARVEN_COCOS_RES[TARVEN_RECRUIT_DIALOG_CSB]);
	auto t_selnum = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_count"));
	auto t_cost = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_income_num"));
	auto liew_sailor = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_ship_sailor_num"));
	auto t_sailor1 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_1"));
	auto t_sailor2 = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_ship_durable_num_2"));
	auto liew_days = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_maxday"));
	auto t_days = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_maxday_num"));
	auto l_sailor = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view, "progressbar_durable"));
	auto l_sailor_add = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(view, "progressbar_durable_add"));
	auto b_caveat = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_caveat"));

	t_selnum->setString(StringUtils::format("%d/%d", ss->getCurrentNumber(), m_pAvailCrewNumResult->availcrewnum));
	t_cost->setString(StringUtils::format("%d", m_pAvailCrewNumResult->crewprice * ss->getCurrentNumber() + m_pBarInfoResult->hirecrewfee));
	t_sailor1->setString(StringUtils::format("%d", m_pAvailCrewNumResult->currentcrewnum + ss->getCurrentNumber()));
	t_sailor2->setString(StringUtils::format("/%d", m_pAvailCrewNumResult->maxcrewnum));
	t_days->setString(StringUtils::format("%d", m_pAvailCrewNumResult->totalsupply / (m_pAvailCrewNumResult->onecrewsupply *
		(m_pAvailCrewNumResult->currentcrewnum + ss->getCurrentNumber()))));
	l_sailor->setPercent(100.0 * (m_pAvailCrewNumResult->currentcrewnum) / m_pAvailCrewNumResult->maxcrewnum);
	l_sailor_add->setPercent(100.0 * (m_pAvailCrewNumResult->currentcrewnum + ss->getCurrentNumber()) / m_pAvailCrewNumResult->maxcrewnum);
	if (m_pAvailCrewNumResult->totalrequired <= m_pAvailCrewNumResult->currentcrewnum + ss->getCurrentNumber())
	{
		t_sailor1->setTextColor(Color4B(40, 25, 13, 255));
		b_caveat->setVisible(false);
	}
	else
	{
		t_sailor1->setTextColor(Color4B(255, 0, 0, 255));
		b_caveat->setVisible(true);
	}
	liew_sailor->refreshView();
	liew_days->refreshView();
}
void UIVillage::updateMainCityCoin(const int64_t nCoin, const int64_t nGold)
{
	SINGLE_HERO->m_iCoin = nCoin;
	SINGLE_HERO->m_iGold = nGold;
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
}
