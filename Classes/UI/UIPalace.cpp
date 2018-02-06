#include "UIPalace.h"
#include "UIStore.h"
#include "UICommon.h"
#include "UIInform.h"

#include "MainTaskManager.h"
#include "CompanionTaskManager.h"
#include "UIChatHint.h"
#include "UISocial.h"
#include "UIVoyageSlider.h"

#include "UINationWarRanking.h"
#include "UINationWarEntrance.h"

#define BUTTON_OFF "cocosstudio/login_ui/palace_720/palace_btn_off_black.png"
#define BUTTON_OFF1 "cocosstudio/login_ui/palace_720/palace_btn_1_off.png"

UIPalace::UIPalace() :
m_pPalaceResult(nullptr),
m_pInvestResult(nullptr),
m_pRankResult(nullptr),
m_pDevGoodsInfo(nullptr),
m_pDevGoodCount(nullptr),
m_pProvideGoodReward(nullptr),
m_pPalceSlider(nullptr),
m_bIsPrestigeUp(false),
m_bIsofficePalace(false),
m_bDialogOut(false),
m_btnMoved(false),
m_bussinessInfoMove(false),
m_viewruancomplete(true)
{
	m_eUIType = UI_PALACE;
	m_nInvestCoinNum=0;
	m_nProvideIndex=0;
	m_nProvideItemNum=0;
	m_nDevelopWay=0;
	m_nInviteGuildTag=0;
	m_preMainButton = nullptr;
	m_bNumpadFlag = true;
	m_operateWidgetTag = 0;
	m_operateTag = 0;
	m_confirmType = CONFIRM_INDEX_ACCEPT_ADDFRIEND;
	m_alreadyOpenView = false;
	m_curView = nullptr;
	m_InfoViewalreadyOpen = false;
	m_State = NO_SELECTED;
	m_pChooseNation = nullptr;
	m_pWarPrepareResult = nullptr;
};

UIPalace::~UIPalace()
{
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
	if (m_pPalaceResult)
	{
		get_palace_info_result__free_unpacked(m_pPalaceResult,0);
		m_pPalaceResult=nullptr;
	}
	if (m_pRankResult)
	{
		get_rank_list_result__free_unpacked(m_pRankResult,0);
		m_pRankResult=nullptr;
	}
	m_pPalceSlider=nullptr;
	m_pInvestResult=nullptr;
	m_pDevGoodsInfo=nullptr;
	m_pDevGoodCount=nullptr;
	m_pProvideGoodReward=nullptr;
	m_loadingLayer = nullptr;
	this->unschedule(schedule_selector(UIPalace::timeShow));
	NotificationCenter::getInstance()->removeAllObservers(this);
}

UIPalace* UIPalace::createPalace()
{
	auto bk = new UIPalace;
	if (bk && bk->init())
	{
		bk->autorelease();
		return bk;
	}
	CC_SAFE_DELETE(bk);
	return nullptr;
}

bool UIPalace::init()
{
	bool pRet = false;
	do 
	{
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIPalace::onServerEvent,this),this);
		ProtocolThread::GetInstance()->getPalaceInfo(UILoadingIndicator::create(this,m_eUIType));
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(UIPalace::palaceSliderEvent), "button_Slider_yes", NULL);
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(UIPalace::palaceSliderClose), "close_palace_Slider", NULL);
		m_loadingLayer = UILoadingIndicator::create(this, 0);
		m_loadingLayer->showSelf();
		pRet = true;
	} while (0);
	return pRet;
}

void UIPalace::onEnter()
{
	UIBasicLayer::onEnter();
}

void UIPalace::onExit()
{
	UIBasicLayer::onExit();
}

void UIPalace::showPalaceInfo(const GetPalaceInfoResult*result)
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

	auto viewCity = getViewRoot(PALACE_COCOS_RES[VIEW_CITYINFO_CSB]);
	viewCity->setTouchEnabled(false);
	auto listviewContent=viewCity->getChildByName("listview_content");
	auto image_titleBg=viewCity->getChildByName("image_title_bg");
	auto imageCityCountry=image_titleBg->getChildByName<ImageView*>("image_country");
	auto labelCity=image_titleBg->getChildByName<Text*>("label_city");
	if (result->nationidx > 0)
	{
		//City icon
		imageCityCountry->loadTexture(getCountryIconPath(result->nationidx));
	}
	else
	{
		//City icon
		imageCityCountry->loadTexture(getCountryIconPath(SINGLE_HERO->m_iCurCityNation));
	}
	
	labelCity->setString(SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].name);//City name
	auto panelContent=listviewContent->getChildByName("panel_content");
	auto panelCity=panelContent->getChildByName("panel_city");
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
	std::string new_value1 = SINGLE_SHOP->getNationInfo()[result->nationidx];
	repalce_all_ditinct(s_port_city, old_value1, new_value1);
	text_port_city->setString(s_port_city);
	text_port_city->setPositionX(text_port->getContentSize().width + text_port->getPositionX() + (text_port_city->getContentSize().width) / 2);
	text_port_city->setTouchEnabled(true);
	text_port_city->setTag(1);
	text_port_city->addTouchEventListener(CC_CALLBACK_2(UIPalace::textEvent,this));

	auto imageCity_population=panelCity->getChildByName("image_city_population");
	auto labelPopulation_num=imageCity_population->getChildByName<Text*>("label_population_num");
	labelPopulation_num->setString(StringUtils::format("%d",result->population));
	
	auto imageCity_trade=panelCity->getChildByName("image_city_trade");
	auto labelTrade_num=imageCity_trade->getChildByName<Text*>("label_trade_num");
	labelTrade_num->setString(StringUtils::format("%d",result->trade));

	auto imageCity_manufacture=panelCity->getChildByName("image_city_manufacture");
	auto labelManufacture_num=imageCity_manufacture->getChildByName<Text*>("label_manufacture_num");
	labelManufacture_num->setString(StringUtils::format("%d",result->manufacture));
	auto panCurrent=panelContent->getChildByName("panel_current");
	auto imageCountry=panCurrent->getChildByName<ImageView*>("image_country");

	auto label_bonus_2 = viewCity->getChildByName<Text*>("label_bonus_!");
	auto label_detaila = viewCity->getChildByName<Text*>("label_detaila");
	label_detaila->setPositionX(label_bonus_2->getPositionX() + label_bonus_2->getContentSize().width + 7);
	std::string s_detaila;
	s_detaila += html_color_begin;
	s_detaila += text_port_city->getString();
	s_detaila += html_color_end;
	std::string old_value2 = "[ Details ]";
	std::string new_value2 = SINGLE_SHOP->getNationInfo()[result->nationidx];
	repalce_all_ditinct(s_port_city, old_value1, new_value1);
	label_detaila->setString(s_detaila);
	label_detaila->setTouchEnabled(true);
	label_detaila->setTag(2);
	label_detaila->addTouchEventListener(CC_CALLBACK_2(UIPalace::textEvent, this));
	auto image_city_population_0 = panelCity->getChildByName<ImageView*>("image_city_population_0");
	image_city_population_0->loadTexture("cocosstudio/login_ui/palace_720/city_manufacture.png");
	auto listviewNameLv=panCurrent->getChildByName<ListView*>("listview_name_lv");

	auto currMayorName=dynamic_cast<Text*>(listviewNameLv->getItem(0));

	auto mayorImage=panCurrent->getChildByName<ImageView*>("image_country");
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
	auto currMayorLv=dynamic_cast<Text*>(listviewNameLv->getItem(1));
	currMayorLv->setVisible(false);
	currMayorLv->setString(StringUtils::format("Lv. %d",result->mayorlevel));

	auto imageCitytrade=panCurrent->getChildByName<ImageView*>("image_city_trade");
	auto labelPolice=imageCitytrade->getChildByName<Text*>("label_trade");
	listviewNameLv->refreshView();
//develop way
	std::string  s_direct = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_TITLE"];
	//新发展方向，如果是0，显示无
	int dirNum = result->newdevdirection;
	
	/*if (result->newdevdirection!=0)
	{
	dirNum=result->newdevdirection;
	}*/
	if (result->mayorname==nullptr)
	{
		//dirNum=4;
	}
	if (dirNum==1)
	{
		imageCitytrade->loadTexture("res/palace_developway/city_population.png");
		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_POPULATION"];
		imageCitytrade->setTouchEnabled(true);
		imageCitytrade->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func, this));
		imageCitytrade->setTag(IMAGE_ICON_POPULATION + IMAGE_INDEX2);
	}
	else if (dirNum==2)
	{
		imageCitytrade->loadTexture("res/palace_developway/city_trade.png");
		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_TRADE"];
		imageCitytrade->setTouchEnabled(true);
		imageCitytrade->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func, this));
		imageCitytrade->setTag(IMAGE_ICON_TRADE + IMAGE_INDEX2);
	}
	else if (dirNum==3)
	{
		imageCitytrade->loadTexture("res/palace_developway/city_manufacture.png");
		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_MAKING"];
		imageCitytrade->setTouchEnabled(true);
		imageCitytrade->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func, this));
		imageCitytrade->setTag(IMAGE_ICON_MANUFACTURE + IMAGE_INDEX2);
	}
	else
	{
		imageCitytrade->loadTexture("res/palace_developway/city_no.png");
		s_direct +=SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NOT_RANK"];
	}
	labelPolice->setString(s_direct);
	auto image_pulldown = viewCity->getChildByName<ImageView*>("image_pulldown");
	image_pulldown->setScale9Enabled(false);
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar((ListView*)listviewContent,image_pulldown);
	//是否有许可证
	showBussinessliceView();
	//流行品信息
	showPalacePopularGood();
	auto i_pop = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewCity, "image_city_population"));
	i_pop->setTouchEnabled(true);
	i_pop->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func, this));
	i_pop->setTag(IMAGE_ICON_POPULATION + IMAGE_INDEX2);
	auto i_trade = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewCity, "image_city_trade"));
	i_trade->setTouchEnabled(true);
	i_trade->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func, this));
	i_trade->setTag(IMAGE_ICON_TRADE + IMAGE_INDEX2);
	auto i_manu = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewCity, "image_city_manufacture"));
	i_manu->setTouchEnabled(true);
	i_manu->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func, this));
	i_manu->setTag(IMAGE_ICON_MANUFACTURE + IMAGE_INDEX1);
}

/*
* 当剧情或者对话动作结束时调用该函数
* actionIndex, 当前动作的index
*/
void UIPalace::notifyCompleted(int actionIndex)
{
	//扫尾工作
	switch (actionIndex){
	case MAIN_STORY:
		break;
	case COMPANION_STORY:
		break;
	case SMALL_STORY:
		break;
	default:
		break;
	}
	doNextJob(actionIndex);
}

/*
* 执行下一个对话或者剧情操作
* actionIndex, 当前动作的index
*/
void UIPalace::doNextJob(int actionIndex)
{
	actionIndex++;
	auto viewCsb = getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB]);
	auto panel_palace = viewCsb->getChildByName<Widget*>("panel_palace");
	auto panel_actionbar = viewCsb->getChildByName<Widget*>("panel_actionbar");
	auto panel_two_butter = viewCsb->getChildByName<Widget*>("panel_two_butter");
	switch (actionIndex)
	{
	case MAIN_STORY:
		//剧情（主线）
		//主线任务完成对话
		if (MainTaskManager::GetInstance()->checkTaskAction())
		{
			addNeedControlWidgetForHideOrShow(nullptr, true);
			allTaskInfo(false, MAIN_STORY);
			MainTaskManager::GetInstance()->notifyUIPartDelayTime(0.5);
		}
		else
		{
			notifyCompleted(MAIN_STORY);
		}
		break;
	case COMPANION_STORY:
		//小伙伴剧情
		if (CompanionTaskManager::GetInstance()->checkTaskAction())
		{
			addNeedControlWidgetForHideOrShow(panel_palace, true);
			addNeedControlWidgetForHideOrShow(panel_actionbar, false);
			addNeedControlWidgetForHideOrShow(panel_two_butter, false);
			allTaskInfo(false, COMPANION_STORY, 0);
			CompanionTaskManager::GetInstance()->notifyUIPartDelayTime(0);
		}
		else
		{
			notifyCompleted(COMPANION_STORY);
		}
		break;
	case SMALL_STORY:
		//小剧情
		if (SINGLE_HERO->m_nTriggerId)
		{
			addNeedControlWidgetForHideOrShow(nullptr, true);
			allTaskInfo(false, SMALL_STORY);
			notifyUIPartDelayTime(0.5);
		}
		else
		{
			notifyCompleted(SMALL_STORY);
		}
		break;
	default:
		break;
	}
}

void UIPalace::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		auto target = dynamic_cast<Widget*>(pSender);
		buttonEventByName(target,target->getName());
	}
}

void UIPalace::buttonEventByName(Widget* target, std::string name)
{
	if (target == m_preMainButton)
	{
		return;
	}
	if (isButton(panel_r_levelup))//声望升级
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_HIRE_RESULT_CSB]);
		UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);
		return;
	}
	if (isButton(button_silver))//银币
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 1);
		return;
	}
	if (isButton(button_gold))//V票
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
		return;
	}
	if (isButton(button_city_info))//城市信息
	{
		if (!m_viewruancomplete)
		{
			return;
		}
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bDialogOut)
		{
			viewCloseAction();
			closeView(PALACE_COCOS_RES[VIEW_INVESTMENT_CSB]);//只有数字键盘消失时
		}
		else
		{
			palaceoutofscreen();
		}
		m_State = CITY_INFO;
		openView(PALACE_COCOS_RES[VIEW_CITYINFO_CSB], 0);
		viewAction(getViewRoot(PALACE_COCOS_RES[VIEW_CITYINFO_CSB]), PALACE_COCOS_RES[VIEW_CITYINFO_CSB]);
		m_bIsofficePalace = false;
		ProtocolThread::GetInstance()->getPalaceInfo(UILoadingIndicator::create(this));
		if (!m_btnMoved)
		{
			buttonMoveRight();
		}
		changeMainButtonState(target);
		return;
	}
	if (isButton(button_hall_of_honor))//荣誉排行榜
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (!m_viewruancomplete)
		{
			return;
		}
		m_alreadyOpenView = false;
		if (m_bDialogOut)
		{
			viewCloseAction();
		}
		else
		{
			palaceoutofscreen();
		}
		m_State = HALL_OF_HONOR;
		ProtocolThread::GetInstance()->getRankList(UILoadingIndicator::create(this));
		if (!m_btnMoved)
		{
			buttonMoveRight();
		}
		changeMainButtonState(target);
		return;
	}
	if (isButton(button_investment))//投资模块
	{
		if (!m_viewruancomplete)
		{
			return;
		}
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bDialogOut)
		{
			viewCloseAction();
		}
		else
		{
			palaceoutofscreen();
		}
		m_State = INVESTMENT;
		ProtocolThread::GetInstance()->getCurrentInvestData(UILoadingIndicator::create(this));
		openView(PALACE_COCOS_RES[VIEW_INVESTMENT_CSB]);
		viewAction(getViewRoot(PALACE_COCOS_RES[VIEW_INVESTMENT_CSB]), PALACE_COCOS_RES[VIEW_INVESTMENT_CSB]);
		if (!m_btnMoved)
		{
			buttonMoveRight();
		}
		changeMainButtonState(target);
		return;
	}
	if (isButton(button_office))//办公室
	{
		if (!m_viewruancomplete)
		{
			return;
		}
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bDialogOut)
		{
			viewCloseAction();
		}
		else
		{
			palaceoutofscreen();
		}
		if (!m_btnMoved)
		{
			buttonMoveRight();
		}
		changeMainButtonState(target);
		m_State = OFFICE;
		m_bIsofficePalace = true;
		ProtocolThread::GetInstance()->getPalaceInfo(UILoadingIndicator::create(this));
		return;
	}
	//国战
	if (isButton(button_war))
	{
		if (!m_viewruancomplete)
		{
			return;
		}
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (target->isBright())
		{
			if (m_bDialogOut)
			{
				viewCloseAction();
			}
			else
			{
				palaceoutofscreen();
			}
			m_State = WAR;

			ProtocolThread::GetInstance()->getCityPrepareStatus(UILoadingIndicator::create(this));
			if (!m_btnMoved)
			{
				buttonMoveRight();
			}
			changeMainButtonState(target);
		}
		else
		{
			openSuccessOrFailDialog("TIP_COUNTRY_WAR_NOT_CUR_NATION");
		}
		return;
	}

	if (isButton(button_invest_to))//确定投资
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openView(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB], 3);
		auto investDialog = getViewRoot(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB]);

		auto imagePut = investDialog->getChildByName("image_input_bg");
		auto inputCurrency_num = imagePut->getChildByName<Text*>("label_currency_num");
		inputCurrency_num->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func, this));
		inputCurrency_num->setString("0");
		this->scheduleUpdate();
		Text * t_coin = investDialog->getChildByName<Text*>("label_silver_num");
		Text * t_gold = investDialog->getChildByName<Text*>("label_gold_num");
		t_coin->setText(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iCoin)));
		t_gold->setText(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iGold)));
		t_coin->setContentSize(Size( 220,34));
		t_gold->setContentSize(Size(220, 34));
		setTextFontSize(t_coin);
		setTextFontSize(t_gold);
		if (m_bNumpadFlag)
		{
			showPalaceNumPad();
		}
		return;
	}
	if (isButton(button_yes))//是否确定投资
	{

		auto imagePut = getViewRoot(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB])->getChildByName("image_input_bg");
		auto inputCurrency_num = imagePut->getChildByName<Text*>("label_currency_num");
		auto s_num = m_padStirng;
		m_padStirng.clear();
		if (isNumber(s_num))
		{
			m_nInvestCoinNum = atoll(s_num.c_str());
			this->unscheduleUpdate();
			if (m_nInvestCoinNum >= 10000 && m_nInvestCoinNum <= SINGLE_HERO->m_iCoin)
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
				ProtocolThread::GetInstance()->investCity(m_nInvestCoinNum, UILoadingIndicator::create(this));
				inputCurrency_num->setString("0");
				closeView();
			}
			else
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
				if (m_nInvestCoinNum>SINGLE_HERO->m_iCoin)
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
	if (isButton(button_captain_yes))//投资结算框的那个按钮
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_HIRE_RESULT_CSB]);
		UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);
		ProtocolThread::GetInstance()->getCurrentInvestData();
		if (m_bIsPrestigeUp)
		{
			m_bIsPrestigeUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
		}

		//主线任务完成对话
		addNeedControlWidgetForHideOrShow(nullptr, true);
		if (MainTaskManager::GetInstance()->checkTaskAction())
		{
			allTaskInfo(false, MAIN_STORY);
			MainTaskManager::GetInstance()->notifyUIPartDelayTime(0.5);
		}
		return;
	}
	if (isButton(button_change))//改变发展方向
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		firstDaybuttonChange(m_pPalaceResult);
		return;
	}
	if (isButton(button_city_population))//选择发展方向界面，人口
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pImageLight_effectPopulation->setVisible(true);
		m_pImageLight_effectTrade->setVisible(false);
		m_pImageLight_effectManuf->setVisible(false);
		m_nDevelopWay = 1;
		return;
	}
	if (isButton(button_city_trade))//交易
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pImageLight_effectPopulation->setVisible(false);
		m_pImageLight_effectTrade->setVisible(true);
		m_pImageLight_effectManuf->setVisible(false);
		m_nDevelopWay = 2;
		return;
	}
	if (isButton(button_city_manufacture))//制造
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pImageLight_effectPopulation->setVisible(false);
		m_pImageLight_effectTrade->setVisible(false);
		m_pImageLight_effectManuf->setVisible(true);
		m_nDevelopWay = 3;
		return;
	}
	if (isButton(button_Ok_change))//确定发展方向
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_nDevelopWay == 1)
		{
			ProtocolThread::GetInstance()->changeDevDir(1, UILoadingIndicator::create(this));
			closeView();
		}
		else if (m_nDevelopWay == 2)
		{
			ProtocolThread::GetInstance()->changeDevDir(2, UILoadingIndicator::create(this));
			closeView();
		}
		else if (m_nDevelopWay == 3)
		{
			ProtocolThread::GetInstance()->changeDevDir(3, UILoadingIndicator::create(this));
			closeView();
		}
		else
		{
			openSuccessOrFailDialog("TIP_PALACE_CHOOSE_WAY_ASK");
		}
		return;
	}

	if (isButton(button_result_yes))//投资结算确定按钮
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);
		closeView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
		if (m_bIsPrestigeUp)
		{
			m_bIsPrestigeUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
		}
		return;
	}

	if (isButton(button_provide))//office界面提供物品
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nProvideIndex = target->getTag();
		if (m_nProvideIndex)
		{
			ProtocolThread::GetInstance()->getDevGoodsCount(m_nProvideIndex, 0);
			return;
		}
		return;
	}
	//info
	if (isButton(button_firstday_office_info))//市长的第一阶段信息解释
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_OFFICE_INFO_TITLE", "TIP_PALACE_OFFICE_INFO_CONTENT");
		return;
	}

	if (isButton(button_investinfo))//投资解释
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_INVEST_INFO_TITLE", "TIP_PALACE_INVEST_INFO_CONTENT");
		return;
	}
	if (isButton(button_citydevelop_info))//发展方向解释
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_CITYDEVELOP_INFO_TITLE", "TIP_PALACE_CITYDEVELOP_INFO_CONTENT");
		return;
	}
	if (isButton(button_Current_info))//货币解释
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_CURRENT_INFO_TITLE", "TIP_PALACE_CURRENT_INFO_CONTENT");
		return;
	}
	if (isButton(button_invest_Rank_info))//排行榜解释
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_INVESTRANK_INFO_TITLE", "TIP_PALACE_INVESTRANK_INFO_CONTENT");
		return;
	}

	if (isButton(button_license_info))//许可证信息
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_LICENSE_INFO_TITLE", "TIP_PALACE_LICENSE_INFO_CONTENT");
		return;
	}
	if (isButton(button_license2_info))//购买许可证的解释信息
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_LICENSE_INFO_TITLE", "TIP_PALACE_BUY_LICENCE_BEFORE_CONTENT");
		return;
	}
	if (isButton(button_port_info))//港口解释信息
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView(getPortTypeOrInfo(2).c_str(),getPortTypeOrInfo(1));
		return;
	}

	if (isButton(button_buy))//购买许可证
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (SINGLE_HERO->m_iHaslisence)
		{
			openSuccessOrFailDialog("TIP_PALACE_HAS_LICENSE");
		}
		else if (SINGLE_HERO->m_iPrestigeLv < m_pPalaceResult->licenserequiredfamelv)
		{
			openSuccessOrFailDialog("TIP_PALACE_NOT_PRESTIGE");
		}
		else if (SINGLE_HERO->m_iCoin < m_pPalaceResult->licenseprice)
		{
			openSuccessOrFailDialog("TIP_COIN_NOT");
		}
		else
		{
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushSilverConfirmView("TIP_PALACE_BUY_LICENSE_TITLE", "TIP_PALACE_BUY_LICENSE_CONTENT", m_pPalaceResult->licenseprice);
		}
		return;
	}

	if (isButton(button_backcity))//返回主城
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		this->button_callBack();
		return;
	}
	if (isButton(button_close))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
	if (isButton(button_no))//关闭
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB]);
		closeView(PALACE_COCOS_RES[VIEW_CHANGE_POLICY_CSB]);
		this->unscheduleUpdate();
		UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
		m_padStirng.clear();
		return;
	}
	if (isButton(button_person_close))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		return;
	}
	if (isButton(button_item_close))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		return;
	}
	if (isButton(button_confirm_yes))//排行榜里邀请好友到公会
	{
		if (m_confirmType == UISocial::CONFIRM_INDEX_ACCEPT_ADDFRIEND)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			ProtocolThread::GetInstance()->inviteUserToGuild(m_nInviteGuildTag, UILoadingIndicator::create(this));
		}
		else
		{
			//拉黑
			if (m_confirmType == CONFIRM_INDEX_FRIEND_BLOCK || m_confirmType == CONFIRM_INDEX_STRANGER_BLOCK)
			{
				ProtocolThread::GetInstance()->friendsOperation(m_operateWidgetTag, 2, UILoadingIndicator::create(this));
				return;
			}
			//删除好友
			else if (m_confirmType == CONFIRM_INDEX_REMOVEFRIEND)
			{
				ProtocolThread::GetInstance()->friendsOperation(m_operateWidgetTag, 1, UILoadingIndicator::create(this));
				return;
			}
			else if (m_confirmType == CONFIRM_INDEX_BLOCK_ADDFRIEND || m_confirmType == CONFIRM_INDEX_STRANGER_ADDFRIEND)
			{
				ProtocolThread::GetInstance()->friendsOperation(m_operateWidgetTag, 0, UILoadingIndicator::create(this));
				return;
			}
			//移除黑名单
			else if (m_confirmType == CONFIRM_INDEX_BLOCK_REMOVE)
			{
				ProtocolThread::GetInstance()->friendsOperation(m_operateWidgetTag, 5, UILoadingIndicator::create(this));
				return;
			}
		}
		return;
	}
	if (isButton(button_s_yes))//确定购买许可证
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		ProtocolThread::GetInstance()->buyCityLicense(UILoadingIndicator::create(this));
		return;
	}
	if (isButton(label_currency_num))//点击输入框，出现数字键盘
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bNumpadFlag)
		{
			showPalaceNumPad();
		}
		return;
	}
	if (isButton(image_skill_bg_))//技能详情
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto t_skill_lv = target->getChildByName<Text*>("text_item_skill_lv");
		SKILL_DEFINE skillDefine;
		skillDefine.id = target->getTag();
		skillDefine.lv = atoi(t_skill_lv->getString().data());
		skillDefine.skill_type = SKILL_TYPE_PLAYER;
		skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushSkillView(skillDefine);
		return;
	}
	if (isButton(button_good_bg))  //物品详情
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		int popularItemId = target->getTag();
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushGoodsDetail(nullptr, popularItemId, true);
		return;
	}
	if (isButton(button_numpad_yes) || isButton(panel_numpad_mask)) //退出数字键盘
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bNumpadFlag)
		{
			m_bNumpadFlag = false;
			auto view = getViewRoot(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB]);
			auto p_numpad_mark = view->getChildByName<Widget*>("panel_numpad_mask");
			p_numpad_mark->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4f), Place::create(ENDPOS)));
			auto num_root = view->getChildByName<Widget*>("panel_numpad");
			num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, ENDPOS4),
				CallFunc::create(this, callfunc_selector(UIPalace::moveEnd))));
			auto label_currency_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_currency_num"));
			std::string str = label_currency_num->getString();
			if (str.empty())
			{
				label_currency_num->setString("0");
			}
		}
		return;
	}

	if (isButton(button_invite))
	{
		m_nInviteGuildTag = target->getTag();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_pRankResult->isadmin)
		{
			if (SINGLE_HERO->m_InvitedToGuildNation == SINGLE_HERO->m_iNation)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYesOrNO("TIP_GULID_BUTTON_INVITATIONS", "TIP_GUILD_INVINT_GUILD");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_INVITED_FAILED_NOT_SAME_COUNTRY_CONTENT");
			}
		}
		else
		{
			openSuccessOrFailDialog("TIP_PALACE_INVITEGUILD_FAIL_NO_ADMIN");
		}
		return;
	}
	if (isButton(button_error_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
		return;
	}
	if (isButton(button_add_friend))
	{
		m_confirmType = CONFIRM_INDEX_STRANGER_ADDFRIEND;
		if (target->getParent()->getTag() == SINGLE_HERO->m_iID)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_NOT_ADD_FRIENDED");
		}
		else if (target->getTag() == 0)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_AGAIN_ADD_FRIENDS");
		}
		else if (target->getTag() == 1)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_ALREADY_GOOD_FRIEND");
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_ADD_FRIEND_TITLE", "TIP_SOCIAL_ADD_FRIEND");
		}
		return;
	}

	if (isButton(button_friend_delete))
	{
		m_confirmType = CONFIRM_INDEX_REMOVEFRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_DELETE_FRIEND_TITLE", "TIP_SOCIAL_DELETE_FRIEND");
		return;
	}

	if (isButton(button_friend_block))
	{
		m_confirmType = CONFIRM_INDEX_FRIEND_BLOCK;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_FRIEND");
		return;
	}
	if (isButton(button_stranger_block))
	{
		m_confirmType = CONFIRM_INDEX_STRANGER_BLOCK;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_STRANGER");
		return;
	}

	if (isButton(button_stranger_remove))
	{
		m_confirmType = CONFIRM_INDEX_BLOCK_REMOVE;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_REMOVE_BLOCK_TITLE", "TIP_REMOVE_BLOCK");
		return;
	}
	if (isButton(image_city_population))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_city_trade))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
	if (isButton(image_city_manufacture) || isButton(image_friendliness_status))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(target);
		return;
	}
}

void UIPalace::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetPalaceInfoResult:
		{
			GetPalaceInfoResult *result = (GetPalaceInfoResult *)message;
			if (result->failed == 0)
			{
				m_pPalaceResult= result;				
				switch (m_State)
				{
				case NO_SELECTED:
					initCityView();
					break;
				case CITY_INFO:
					showPalaceInfo(m_pPalaceResult);
					//showBussinessliceView();
					ProtocolThread::GetInstance()->getFriendValue(1, UILoadingIndicator::create(this));
					break;
				case OFFICE:
					if (m_bIsofficePalace)
					{
						officeChoose(result);
						viewAction(m_curView, m_viewPath);
					}
					break;
				case WAR:
					ProtocolThread::GetInstance()->getCityPrepareStatus(UILoadingIndicator::create(this));
				    break;
				default:
					if (m_bIsofficePalace)
					{
						officeChoose(result);
						viewAction(m_curView, m_viewPath);
					}
					break;
				}
			}
			else
			{
				openSuccessOrFailDialog("TIP_PALACE_GET_DATA_FAIL");
				button_callBack();
			}
			break;
		}
	case PROTO_TYPE_InvestCityResult:
		{
			InvestCityResult *result = (InvestCityResult *)message;
			if (result->failed == 0)
			{
				SINGLE_HERO->m_iCoin = result->coin;
				SINGLE_HERO->m_iGold = result->gold;
				SINGLE_HERO->m_iLevel = EXP_NUM_TO_LEVEL(result->exp);
				SINGLE_HERO->m_iPrestigeLv = FAME_NUM_TO_LEVEL(result->fame);
				showMainCoin();
				showInvestCoinResult(result);
				showInvestView(m_pInvestResult);
				m_bIsofficePalace=false;
				if (result->new_fame_level)
				{
					m_bIsPrestigeUp = true;
					SINGLE_HERO->m_iPrestigeLv = result->new_fame_level;
				}
			}else if(result->failed == 2){
				openSuccessOrFailDialog("TIP_PALACE_INVEST_ENEMY");
			}else
			{
				openSuccessOrFailDialog("TIP_PALACE_INVEST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetCurrentInvestDataResult:
		{
			GetCurrentInvestDataResult *result = (GetCurrentInvestDataResult *)message;
			m_pInvestResult=result;
			if (result->failed == 0)
			{
				showInvestView(result);
				showCurrInvestTop(result);
				m_bIsofficePalace=false;
				
			}else
			{
				openSuccessOrFailDialog("TIP_PALACE_INVEST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetRankListResult:
		{
			GetRankListResult *result = (GetRankListResult *)message;
			if (result->failed == 0)
			{
				if (m_pRankResult)
				{
					get_rank_list_result__free_unpacked(m_pRankResult,0);
				}

				m_pRankResult = result;
				if (!m_alreadyOpenView)
				{
					m_alreadyOpenView = true;
					onRankView(result);
				}			
			}else
			{
				openSuccessOrFailDialog("TIP_PALACE_RANK_FAIL");
			}
			break;
		}
	case PROTO_TYPE_ChangeDevDirResult:
		{
			ChangeDevDirResult *result = (ChangeDevDirResult *)message;
			if (result->failed == 0)
			{	
				ProtocolThread::GetInstance()->getPalaceInfo();	
				openSuccessOrFailDialog("TIP_PALACE_CHANGE_DIRECT_SUCCESS");
			}else
			{
				openSuccessOrFailDialog("TIP_PALACE_CHANGE_DIRECT_FAIL");
			}
			break;
		}
	case  PROTO_TYPE_GetDevGoodsInfoResult:
		{
			GetDevGoodsInfoResult*result=(GetDevGoodsInfoResult*)message;
			if (result->failed==0)
			{
				m_pDevGoodsInfo=result;
				if (getViewRoot(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB]))//csb文件存在时才打开
				{
					showAnnouncement(result);
				}			
				if (getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_DECIDED_CSB]))
				{
					showOfficeAnnouncement(result);
				}							
			}
			else
			{
				openSuccessOrFailDialog("TIP_PALACE_GETDEVGOODSINFORESULT_FALL");
			}
			break;
		}
	case  PROTO_TYPE_ProvideDevGoodsResult:
		{
			ProvideDevGoodsResult*result=(ProvideDevGoodsResult*)message;
			if (result->failed==0)
			{
				m_pProvideGoodReward=result;
				
				ProtocolThread::GetInstance()->getDevGoodsInfo();
				ProtocolThread::GetInstance()->getPalaceInfo();
			   showProvideRewards(m_pProvideGoodReward);
				SINGLE_HERO->m_iCoin=result->coins;
				showMainCoin();
				if (result->new_fame_level)
				{
					m_bIsPrestigeUp = true;
					SINGLE_HERO->m_iPrestigeLv = result->new_fame_level;
				}
			}
			else
			{
				openSuccessOrFailDialog("TIP_PALACE_PROVIDEDEVGOODSRESULT_FALL");
			}
			break;
		}
	case PROTO_TYPE_GetDevGoodsCountResult:
		{
			GetDevGoodsCountResult*result=(GetDevGoodsCountResult*)message;
			if (result->failed==0)
			{
				m_pDevGoodCount=result;
				
				if (result->count==0)
				{
					openSuccessOrFailDialog("TIP_PALACE_NOTHAVETHISGOODS");
				}
				else
				{
					showSlider(result);
				}	
			}else if(result->failed == 3){
				openSuccessOrFailDialog("TIP_PALACE_GETDEVGOODSCOUNTRESULT_ENOUGH");
			}
			else
			{
				openSuccessOrFailDialog("TIP_PALACE_GETDEVGOODSCOUNTRESULT_FALL");
			}

			break;
		}
	case PROTO_TYPE_GetFriendValueResult:
		{
			GetFriendValueResult*result=(GetFriendValueResult*)message;
			
			if (result->failed==0)
			{
				if (result->n_nation!=0)
				{
					cityFriendshipShow(result);
				}			
			}
			else
			{
				openSuccessOrFailDialog("TIP_PALACE_GETFRIENDVALUERESULT");
			}
			break;
		}
	case PROTO_TYPE_InviteUserToGuildResult:
		{
			InviteUserToGuildResult *result = (InviteUserToGuildResult *)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_GULID_INVINT_SUCCESS");
			}
			else if (result->failed == 11)
			{
				// 邀请的对象不是同一势力
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_GUILD_INVITED_FAILED_NOT_SAME_COUNTRY_CONTENT");
			}
			else
			{
				openSuccessOrFailDialog("TIP_GULID_INVINT_FAIL",UICommon::getInstance(),100000);
			}
			break;
		}
	case PROTO_TYPE_BuyCityLicenseResult:
		{
			BuyCityLicenseResult *result = (BuyCityLicenseResult *)message;
			if (result->failed == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_PALACE_BUY_LICENSE_SUCCESS");
				SINGLE_HERO->m_iHaslisence = 1;
				SINGLE_HERO->m_iCoin-=m_pPalaceResult->licenseprice;
				showBussinessliceView();
				showMainCoin();
			}else if (result->failed == COIN_NOT_FAIL)
			{
				openSuccessOrFailDialog("TIP_COIN_NOT");
			}else
			{
				openSuccessOrFailDialog("TIP_CENTER_OPERATE_FAIL");
			}
			break;
		}
	case PROTO_TYPE_FriendsOperationResult:
	{
			FriendsOperationResult *pFriendsResult = (FriendsOperationResult*)message;
			if (pFriendsResult->failed == 0)
			{
				m_confirmType = CONFIRM_INDEX_ACCEPT_ADDFRIEND;
				//添加好友
				if (pFriendsResult->actioncode == 3)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADDED_FRIEND_SUCCESS");
				}
				//添加黑名单里的玩家为好友
				else if (pFriendsResult->actioncode == 2)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_BLACK_LIST_SUCCESS");
				}
				//删除好友
				else if (pFriendsResult->actioncode == 1)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_DETELE_FRIEND_SUCCESS");
				}
				//加为好友
				else if (pFriendsResult->actioncode == 0) 
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_SOCIAL_ADD_FRIEND_SUCCESS");
				}
				//移除拉黑玩家
				else if (pFriendsResult->actioncode == 5)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_REMOVE_BLOCK_SUCCESS");
				}
				UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_FRIEND_DETAIL_CSB]);
//				closeView();
//				ProtocolThread::GetInstance()->getRankList(LoadingLayer::create(this));
			}
			else if (pFriendsResult->failed == 105)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_REQUEST_ADDFRIENDED");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_ADD_FRIENDED_FAIL");
			}
			break;
	}
	case PROTO_TYPE_GetUserInfoByIdResult:
	{
				GetUserInfoByIdResult *result = (GetUserInfoByIdResult *)message;
				if (result->failed == 0)
				{
					SINGLE_HERO->m_InvitedToGuildNation = result->nation;
					UICommon::getInstance()->openCommonView(this);
					if (m_operateWidgetTag == SINGLE_HERO->m_iID)
					{
						UICommon::getInstance()->flushFriendDetail(result, true);
					}
					else
					{
						UICommon::getInstance()->flushFriendDetail(result, false);
					}
				}
				else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_SOCIAL_PLAYER_INFO_FAIL");
				}
				break;
	}
	case PROTO_TYPE_GetCityPrepareStatusResult:
		{
			GetCityPrepareStatusResult *result = (GetCityPrepareStatusResult *)message;
			if (result->failed == 0)
			{
				m_pWarPrepareResult = result;
				m_pWarPrepareResult->current_hp = (m_pWarPrepareResult->current_hp <= 0 ? 0 : m_pWarPrepareResult->current_hp);
				showWar();
			}
			break;
		}
	case PROTO_TYPE_GetEnemyNationResult:
		{
			GetEnemyNationResult *result = (GetEnemyNationResult *)message;
			if (result->failed == 0)
			{
				showStartWar(result);
			}
			break;
		}
	case PROTO_TYPE_ApplyStateWarResult:
		{
			ApplyStateWarResult *result = (ApplyStateWarResult *)message;
			if (result->failed == 0)
			{
				m_pChooseNation = nullptr;
				closeView(PALACE_COCOS_RES[VIEW_LAUNCHWAR_CSB]);
				openView(PALACE_COCOS_RES[VIEW_LAUNCHWAR_RESULT_CSB], 11);
				auto view = getViewRoot(PALACE_COCOS_RES[VIEW_LAUNCHWAR_RESULT_CSB]);
				auto b_yes = view->getChildByName<Button*>("button_result_yes");
				b_yes->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));
				auto p_content = view->getChildByName<Widget*>("panel_content");
				auto t_vs = p_content->getChildByName<Text*>("label_content_0");
				std::string st_vs = SINGLE_SHOP->getNationInfo().at(result->my_nation_id);
				st_vs += " VS ";
				st_vs += SINGLE_SHOP->getNationInfo().at(result->enemy_nation_id);
				t_vs->setString(st_vs);
				auto i_nation_1 = p_content->getChildByName<ImageView*>("image_country");
				auto i_nation_2 = p_content->getChildByName<ImageView*>("image_country_0");
				i_nation_1->loadTexture(getCountryIconPath(result->my_nation_id));
				i_nation_2->loadTexture(getCountryIconPath(result->enemy_nation_id));
				ProtocolThread::GetInstance()->getCityPrepareStatus(UILoadingIndicator::create(this));
			}
			else if (result->failed == 5)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PALACE_COUNTRY_WAR_APPLY_FAIL");
			}
			else if (result->failed == 6)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PALACE_COUNTRY_WAR_APPLY_FAIL2");
			}
			//市长竞选前不可以发动国战
			else if (result->failed == 8)
			{
				//前面已经做了限制
				//InformView::getInstance()->openInformView(this);
				//InformView::getInstance()->openConfirmYes("TIP_PALACE_COUNTRY_WAR_APPLY_FAIL3");
			}
			break;
		}
	case PROTO_TYPE_GetForceCityResult:
	{
										  GetForceCityResult  *result = (GetForceCityResult*)message;
										  if (result->failed == 0)
										  {
											  if (m_operateTag == 1)
											  {
												  UICommon::getInstance()->openCommonView(this);
												  UICommon::getInstance()->openCestatusView(result);
											  }
											  else
											  {
												  UICommon::getInstance()->openCommonView(this);
												  UICommon::getInstance()->openManufactureView(result);
											  }
										  }
										  break;
	}
	default:
		break;
	}
}

void UIPalace::palaceSliderEvent(Ref*)
{
	closeView(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
	ProtocolThread::GetInstance()->provideDevGoods(m_nProvideIndex,m_nProvideItemNum,0);
}

void UIPalace::palaceSliderClose(Ref*)
{
	closeView(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
}

void UIPalace::timeShow(float t)
{	
	if(!m_pPalaceResult) return;
	m_pPalaceResult->nextcycletime--;
	
	int64_t totalDays = m_pPalaceResult->totalinvestcycleseconds / (3600 * 24);//周期总天数
	int64_t firstDayTimeLeft = m_pPalaceResult->nextcycletime - (m_pPalaceResult->totalinvestcycleseconds / 7 * 6);//第一阶段剩下的时间(共7个阶段)
	if (m_pPalaceResult->nextcycletime<1 || firstDayTimeLeft == -1)//7 days a cycle
	{
		if (m_State != NO_SELECTED)
		{
			ProtocolThread::GetInstance()->getPalaceInfo(UILoadingIndicator::create(this));
		}
	}
	
	//char timeBuf[80];
	int64_t Days = m_pPalaceResult->nextcycletime/(3600*24);
	int64_t Hours = (m_pPalaceResult->nextcycletime)/3600%24;
	int64_t Mins = (m_pPalaceResult->nextcycletime/60)%60;
	int64_t Seconds = m_pPalaceResult->nextcycletime%60;
	
	std::string timesleft = "";

	if (Days>=1)
	{		
		timesleft = StringUtils::format("%lld %s %lld %s/%lld %s", Days, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_DAY_2"].c_str(),
			Hours, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_HOURS_2"].c_str(),
			totalDays, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_DAY_2"].c_str());
	}
	else if(Hours>=1)
	{
		timesleft = StringUtils::format("%lld %s %lld %s/%lld %s", Hours, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_HOURS_2"].c_str(),
			Mins, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_MINS_2"].c_str(),
			totalDays, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_DAY_2"].c_str());

	}
	else if(Mins>=1)
	{
		timesleft = StringUtils::format("%lld %s %lld %s/%lld %s", Mins, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_MINS_2"].c_str(),
			Seconds, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_SECONDS_2"].c_str(),
			totalDays, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_DAY_2"].c_str());
	}
	else
	{
		timesleft = StringUtils::format("%lld %s/%lld %s", Seconds, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_SECONDS_2"].c_str(),
			totalDays, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_DAY_2"].c_str());
	}
	//investment
	if (getViewRoot(PALACE_COCOS_RES[VIEW_INVESTMENT_CSB]))
	{
		auto labelTime=getViewRoot(PALACE_COCOS_RES[VIEW_INVESTMENT_CSB])->getChildByName<Text*>("label_time");
		labelTime->setString(timesleft);
	}
	//announcement
	if (getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_DECIDED_CSB]))
	{
		auto viewofficeAnn=getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_DECIDED_CSB]);
		auto panelUnreard=viewofficeAnn->getChildByName("panel_unreward");
		auto labelTimeOff=panelUnreard->getChildByName<Text*>("label_time");
		labelTimeOff->setString(timesleft);
	}
	
	//weekly
	if (getViewRoot(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB]))
	{
		auto viewoffic_Announce=getViewRoot(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB]);
		auto label_timeAnn=viewoffic_Announce->getChildByName<Text*>("label_time");
		label_timeAnn->setString(timesleft);
	}
	//office
	if (getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_UNDECIDED_CSB]))
	{
		auto viewoffic_UN=getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_UNDECIDED_CSB]);
		auto labelTimeChange=viewoffic_UN->getChildByName<Text*>("label_time_0");
		auto labelTimeUN=viewoffic_UN->getChildByName<Text*>("label_time");
		labelTimeUN->setString(timesleft);
		if (m_pPalaceResult->isfirstday)
		{
		
			int64_t firstDayHours = (firstDayTimeLeft) / 3600 % 24;
			int64_t firstDayMins = (firstDayTimeLeft / 60) % 60;
			int64_t firstDaySeconds = firstDayTimeLeft % 60;
			std::string strHours = StringUtils::format("%02lld", firstDayHours);
			std::string strMins = StringUtils::format("%02lld", firstDayMins);
			std::string strSec = StringUtils::format("%02lld", firstDaySeconds);
		
			if (firstDayTimeLeft<=0)
			{
				labelTimeChange->setVisible(false);
			}
			else
			{
				labelTimeChange->setVisible(true);
			}
			std::string timeAll = strHours + ":" + strMins + ":" + strSec;
			labelTimeChange->setFontSize(25);
			labelTimeChange->setString(timeAll);
		}
		else
		{
			labelTimeChange->setVisible(false);
		}
	}

	//国战开始或者准备的倒计时
	auto view1 = getViewRoot(PALACE_COCOS_RES[VIEW_WAR_CSB]);
	if (view1 && m_pWarPrepareResult)
	{
		auto l_warContent = view1->getChildByName<ListView*>("listview_content");
		for (size_t i = 0; i < m_pWarPrepareResult->n_fight_nations; i++)
		{
			auto item = l_warContent->getItem(i);
			auto t_war1 = item->getChildByName<Text*>("labe_1");
			auto t_war2 = item->getChildByName<Text*>("labe_1_0");
			auto t_war_state = item->getChildByName<Text*>("labe_war_start");
			auto t_war_time = item->getChildByName<Text*>("label_time");
			
			m_pWarPrepareResult->fight_nations[i]->dis_to_end--;
			int h = m_pWarPrepareResult->fight_nations[i]->dis_to_end / 3600;
			int m = m_pWarPrepareResult->fight_nations[i]->dis_to_end % 3600 / 60;
			int s = m_pWarPrepareResult->fight_nations[i]->dis_to_end % 3600 % 60;
			if (m_pWarPrepareResult->fight_nations[i]->time_sign == 0)
			{
				t_war1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_PREPARE_CONTENT1"]);
				std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_PREPARE_CONTENT2"];
				std::string new_vaule = SINGLE_SHOP->getNationInfo().find(m_pWarPrepareResult->fight_nations[i]->nation1)->second;
				std::string old_vaule = "[nation_name1]";
				repalce_all_ditinct(content, old_vaule, new_vaule);
				new_vaule = SINGLE_SHOP->getNationInfo().find(m_pWarPrepareResult->fight_nations[i]->nation2)->second;
				old_vaule = "[nation_name2]";
				repalce_all_ditinct(content, old_vaule, new_vaule);
				t_war2->setString(content);
				t_war_state->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_PREPARE_TIME"]);
				
				t_war_time->setString(StringUtils::format("%02d:%02d:%02d", h, m, s));
				if (m_pWarPrepareResult->fight_nations[i]->dis_to_end <= 0)
				{
					m_pWarPrepareResult = nullptr;
					ProtocolThread::GetInstance()->getCityPrepareStatus(UILoadingIndicator::create(this));
					break;
				}
			}
			else
			{
				t_war1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_CONTENT1"]);
				std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_CONTENT2"];
				std::string new_vaule = SINGLE_SHOP->getNationInfo().find(m_pWarPrepareResult->fight_nations[i]->nation1)->second;
				std::string old_vaule = "[nation_name1]";
				repalce_all_ditinct(content, old_vaule, new_vaule);
				new_vaule = SINGLE_SHOP->getNationInfo().find(m_pWarPrepareResult->fight_nations[i]->nation2)->second;
				old_vaule = "[nation_name2]";
				repalce_all_ditinct(content, old_vaule, new_vaule);
				t_war2->setString(content);
				t_war_state->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_TIME"]);
				t_war_time->setString(StringUtils::format("%02d:%02d:%02d", h, m, s));
				if (m_pWarPrepareResult->fight_nations[i]->dis_to_end <= 0)
				{
					m_pWarPrepareResult = nullptr;
					ProtocolThread::GetInstance()->getCityPrepareStatus(UILoadingIndicator::create(this));
					break;
				}
			}
		}

		if (m_pWarPrepareResult && m_pWarPrepareResult->dis_to_apply_war > 0)
		{
			m_pWarPrepareResult->dis_to_apply_war--;

			auto view = getViewRoot(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
			if (view)
			{
				int h = m_pWarPrepareResult->dis_to_apply_war / 3600;
				int m = m_pWarPrepareResult->dis_to_apply_war % 3600 / 60;
				int s = m_pWarPrepareResult->dis_to_apply_war % 3600 % 60;

				std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_ARMISTICE_DAY"];
				std::string new_vaule = StringUtils::format("%02d:%02d:%02d", h, m, s);
				std::string old_vaule = "[time]";
				repalce_all_ditinct(content, old_vaule, new_vaule);

				auto t_content = dynamic_cast<Text*>(view->getChildByName("label_dropitem_tiptext"));
				t_content->setString(content);
			}
		
			if (m_pWarPrepareResult->dis_to_apply_war == 0)
			{
				ProtocolThread::GetInstance()->getCityPrepareStatus(UILoadingIndicator::create(this));
				closeView(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
			}
		}
	}
}

void UIPalace::showSlider(const GetDevGoodsCountResult*result)
{
	if (!result)
	{
		return;
	}
	openView(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB],3);
	auto chooseItemNum=getViewRoot(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
	m_pPalceSlider=UIVoyageSlider::create(chooseItemNum,10,0,true);
	m_pPalceSlider->addSliderScrollEvent_1(CC_CALLBACK_1(UIPalace::onPalaceSliderNumbers,this));
	m_pPalceSlider->retain();
	auto imageItemBg=chooseItemNum->getChildByName("image_item_bg");
	auto imageItem=imageItemBg->getChildByName<ImageView*>("image_item");
	imageItem->loadTexture(getGoodsIconPath(result->goodsid));
	m_pPalceSlider->initSliderNum(result->count,0);
}

void UIPalace::showProvideRewards(const ProvideDevGoodsResult*result)
{
	
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushPlayerAddExpOrFrame(result->exp,result->fame,result->addedexp,result->addedfame);
	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB],3);
	auto view=getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	//标题
	auto label_title = view->getChildByName<Text*>("label_title");
	label_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_COMMONCSB_SAILNG_RESULT"]);
	//按钮
	auto button_result_yes=view->getChildByName<Button*>("button_result_yes");
	auto panel_result = view->getChildByName("panel_result");
	//列表
	ListView* listview_result = panel_result->getChildByName<ListView*>("listview_result");
	//分割线
	auto image_div_1=panel_result->getChildByName<ImageView*>("image_div_1");
	auto image_div_1_clone=image_div_1->clone();
	auto image_div_2_clone=image_div_1->clone();
	//显示物品
	Widget*  panel_buy_3=panel_result->getChildByName<Widget*>("panel_buy_3");
	Widget* panel_buy_3_clone = panel_buy_3->clone();
	auto label_buy=panel_buy_3_clone->getChildByName<Text*>("label_buy");
	label_buy->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_PROVIDE_GOOD"]);

	auto label_items_name=panel_buy_3_clone->getChildByName<Text*>("label_items_name");
	auto label_buy_num=panel_buy_3_clone->getChildByName<Text*>("label_buy_num");

	label_items_name->setString(getGoodsName(result->goodsid));//名字
	label_buy_num->setString(StringUtils::format("x %d",result->addedcount));//数量
	label_items_name->setPositionX(label_buy_num->getPositionX() - label_buy_num->getContentSize().width - label_items_name->getContentSize().width / 2 - 6);
	//花费金币
	Widget* panel_silver=panel_result->getChildByName<Widget*>("panel_silver_2");
	Widget* panel_silver_clone = panel_silver->clone();

	auto label_force_relation=panel_silver_clone->getChildByName<Text*>("label_force_relation");
	label_force_relation->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_PROVIDE_GOOD_REWARD"]);
	auto income_num=panel_silver_clone->getChildByName<Text*>("label_buy_num");
	income_num->setString(numSegment(StringUtils::format("%d",result->addedcoins)));//收入银币数量
	auto image_silver = panel_silver_clone->getChildByName<ImageView*>("image_silver");
	image_silver->loadTexture("cocosstudio/login_ui/common/silver.png");
	image_silver->setPositionX(income_num->getPositionX() - income_num->getBoundingBox().size.width - income_num->getBoundingBox().size.width / 2 - 5);

	listview_result->pushBackCustomItem(panel_buy_3_clone);
	listview_result->pushBackCustomItem(image_div_1_clone);
	listview_result->pushBackCustomItem(panel_silver_clone);
	listview_result->pushBackCustomItem(image_div_2_clone);
	listview_result->refreshView();
}

void UIPalace::initCityView()
{
	auto Winsize = Director::getInstance()->getWinSize();
	openView(PALACE_COCOS_RES[VIEW_PLACE_CSB]);//打开csb文件
	auto viewCsb=getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB]);
	auto image_bg = viewCsb->getChildByName<ImageView*>("image_bg");
	auto panel_palace=viewCsb->getChildByName("panel_palace");//对话
	auto panel_actionbar=viewCsb->getChildByName("panel_actionbar");//显示金银币
	auto panel_two_butter=viewCsb->getChildByName<Widget*>("panel_two_butter");//四个按钮
	auto panelDialog=panel_palace->getChildByName("panel_dialog");
	auto roleGuard=panelDialog->getChildByName<ImageView*>("image_soldier");;
	auto image_dialog_bg=panelDialog->getChildByName<ImageView*>("image_dialog_bg");
	auto labelguard=panelDialog->getChildByName("label_guard");
	auto guardText=panelDialog->getChildByName<Text*>("label_content");
	auto i_anchor = panelDialog->getChildByName<ImageView*>("image_anchor");
	image_dialog_bg->setOpacity(0);
	labelguard->setOpacity(0);
	guardText->setOpacity(0);
	i_anchor->setOpacity(0);
	image_dialog_bg->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), FadeIn::create(0.5)));
	labelguard->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), FadeIn::create(0.5)));
	guardText->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), FadeIn::create(0.5)));
	roleGuard->runAction(MoveTo::create(0.5f,Vec2(0,0)));
	roleGuard->ignoreContentAdaptWithSize(false);
	roleGuard->loadTexture(getNpcPath(SINGLE_HERO->m_iCityID,FLAG_SOLDIER));
	panel_two_butter->removeFromParent();
	this->addChild(panel_two_butter, 2);
	if (m_loadingLayer)
	{
		m_loadingLayer->removeSelf();
		m_loadingLayer = nullptr;
	}

	scheduleOnce(schedule_selector(UIPalace::buttonAction),1);
	auto button_officeSuDan = panel_two_butter->getChildByName<Button*>("button_office");
	auto t_palace = panel_actionbar->getChildByName<Text*>("label_palace");
	if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].palace_type == 1)
	{
		t_palace->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TITLE_PALACE"]);
	}
	else
	{
		t_palace->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TITLE_CITYHALL"]);
	}
	//背景图片
	image_bg->loadTexture(getCityAreaBgPath(SINGLE_HERO->m_iCityID, FLAG_PALACE_AREA));
	
	auto tips = SINGLE_SHOP->getTipsInfo();
	std::string palace_type[] = {"","PALACE","CITYHALL","MANSION"};
	std::string prestige[] = {"","MIN","MID","MAX","LAR"};
	int type = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].palace_type;
	int p = 1;
	if (SINGLE_HERO->m_iPrestigeLv < PRESTIGE_MIN)
	{
		p = 1;
	}else if (SINGLE_HERO->m_iPrestigeLv < PRESTIGE_MID)
	{
		p = 2;
	}else if (SINGLE_HERO->m_iPrestigeLv < PRESTIGE_MAX)
	{
		p = 3;
	}else
	{
		p = 4;
	}		
	std::string content = "";
	//是市长
	if (m_pPalaceResult->ismayor)
	{
		content = tips["TIP_CITYHLL_DIALOG_CONTENT_FOR_MAYOR"];
	}
	else
	{
		std::string tip_index = StringUtils::format("TIP_PALACE_%s_%s", palace_type[type].c_str(), prestige[p].c_str());
		content = tips[tip_index];
		std::string new_vaule = SINGLE_HERO->m_sName;
		std::string old_vaule = "[heroname]";
		repalce_all_ditinct(content, old_vaule, new_vaule);
	}
	guardText->setString(content);
	float m_hight = getLabelHight(content,guardText->getContentSize().width,guardText->getFontName(),guardText->getFontSize());
	guardText->setContentSize(Size(guardText->getContentSize().width, m_hight));

	auto viewBar=panel_actionbar;
	viewBar->setLocalZOrder(100);
	viewBar->setVisible(true);
	auto bntSliver=viewBar->getChildByName<Button*>("button_silver");
	auto label_silvernum=bntSliver->getChildByName<Text*>("label_silver_num");
	auto bntGold=viewBar->getChildByName<Button*>("button_gold");
	auto label_goldnum=bntGold->getChildByName<Text*>("label_gold_num");
	label_silvernum->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iCoin)));
	label_goldnum->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iGold)));
	this->scheduleOnce(schedule_selector(UIPalace::anchorAction),1);
	setTextFontSize(label_silvernum);
	setTextFontSize(label_goldnum);
	notifyCompleted(NONE);

	//聊天
	auto ch = UIChatHint::createHint();
	this->addChild(ch, 10);

	this->schedule(schedule_selector(UIPalace::timeShow), 1.f);
}

void UIPalace::anchorAction(float time)
{
	auto guard_dialog = getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB])->getChildByName("panel_palace")->getChildByName("panel_dialog");
	auto i_anchor = guard_dialog->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(255);
	i_anchor->setVisible(false);
}

void UIPalace::officeChoose(const GetPalaceInfoResult *result)
{
	if (result->isfirstday)//first day
	{
		if (result->ismayor)
		{
			//到时间时，关闭可能没关闭的csb
			closeView(PALACE_COCOS_RES[VIEW_OFFICE_DECIDED_CSB]);
			closeView(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB]);
			closeView(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
			closeView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
			UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);

			openView(PALACE_COCOS_RES[VIEW_OFFICE_UNDECIDED_CSB]);
			m_curView = getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_UNDECIDED_CSB]);
			m_viewPath = PALACE_COCOS_RES[VIEW_OFFICE_UNDECIDED_CSB];
			auto viewoffic_UN = getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_UNDECIDED_CSB]);
			firstDayChosse(result);
		}
		else
		{
			//not mayor
			//到时间时，关闭可能没关闭的csb
			closeView(PALACE_COCOS_RES[VIEW_OFFICE_DECIDED_CSB]);
			closeView(PALACE_COCOS_RES[VIEW_OFFICE_UNDECIDED_CSB]);
			closeView(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
			closeView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
			UICommon::getInstance()->closeView(COMMOM_COCOS_RES[C_VIEW_ADD_EXP_CSB]);

			openView(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB]);
			m_curView = getViewRoot(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB]);
			m_viewPath = PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB];
			auto panelDemand = getViewRoot(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB])->getChildByName<Widget*>("panel_demand");
			auto label_population = getViewRoot(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB])->getChildByName<Text*>("label_population");

			panelDemand->setVisible(true);
			label_population->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WEEKLYANNOUNCEMENT_FIRSTDAY_TEXT"]);
			showNogoodPanel(panelDemand);
		}
	}
	else//2-7 days
	{
		if (result->ismayor)
		{
			closeView(PALACE_COCOS_RES[VIEW_CHANGE_POLICY_CSB]);
			closeView(PALACE_COCOS_RES[VIEW_OFFICE_UNDECIDED_CSB]);
			openView(PALACE_COCOS_RES[VIEW_OFFICE_DECIDED_CSB]);
			m_curView = getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_DECIDED_CSB]);
			m_viewPath = PALACE_COCOS_RES[VIEW_OFFICE_DECIDED_CSB];
		}
		else
		{
			openView(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB]);
			m_curView = getViewRoot(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB]);
			m_viewPath = PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB];
		}
		ProtocolThread::GetInstance()->getDevGoodsInfo();
	}
}

void UIPalace::firstDaybuttonChange(const GetPalaceInfoResult *result)
{
	auto viewoffic_UN=getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_UNDECIDED_CSB]);
	auto buttonChange=viewoffic_UN->getChildByName<Button*>("button_change");
	//一开始三个灯都不亮
	openView(PALACE_COCOS_RES[VIEW_CHANGE_POLICY_CSB],3);
	auto chossePolicePanel=getViewRoot(PALACE_COCOS_RES[VIEW_CHANGE_POLICY_CSB]);
	
	auto buttonPopulation = chossePolicePanel->getChildByName<Button*>("button_city_population");
	m_pImageLight_effectPopulation = buttonPopulation->getChildByName<ImageView*>("image_light_effect_2");
	auto buttonTrade = chossePolicePanel->getChildByName<Button*>("button_city_trade");
	m_pImageLight_effectTrade = buttonTrade->getChildByName<ImageView*>("image_light_effect_2");
	auto buttonManuf = chossePolicePanel->getChildByName<Button*>("button_city_manufacture");
	m_pImageLight_effectManuf = buttonManuf->getChildByName<ImageView*>("image_light_effect_2");
	m_pImageLight_effectPopulation->setVisible(false);
	m_pImageLight_effectTrade->setVisible(false);
	m_pImageLight_effectManuf->setVisible(false);
	auto label_content = chossePolicePanel->getChildByName<Text*>("label_content");
	label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CHANGE_POLICY_TEXT_MORE_TIMES"]);
	buttonChange->setTouchEnabled(true);
	buttonChange->setBright(true);
}

void UIPalace::cityFriendshipShow(const GetFriendValueResult*result)
{
	//friendvalue
	auto viewCity=getViewRoot(PALACE_COCOS_RES[VIEW_CITYINFO_CSB]);
	if (!viewCity) return;
	auto listviewContent=viewCity->getChildByName("listview_content");
	auto panelContent=listviewContent->getChildByName("panel_content");
	auto imageAllcountry = panelContent->getChildByName("image_personal_friendliness");
	for (int i = 1; i <  result->n_nation; i++)
	{
		for (int j = 0; j < result->n_nation-i; j++)
		{
			if (result->nation[j]->nationid > result->nation[j+1]->nationid)
			{
				auto temp = result->nation[j];
				result->nation[j] = result->nation[j+1];
				result->nation[j+1] = temp;
			}
		}
	}

	Widget *p_friend = nullptr;
	int p_friend_num = 0;
	for (int i=1;i<=7;++i)
	{
		switch(i)
		{
		case 1:
			p_friend = imageAllcountry->getChildByName<Widget *>("image_portugal");
			break;
		case 2:
			p_friend=imageAllcountry->getChildByName<Widget *>("image_spain");
			break;
		case 3:
			p_friend=imageAllcountry->getChildByName<Widget *>("image_england");
			break;
		case 4:
			p_friend=imageAllcountry->getChildByName<Widget *>("image_venice");
			break;
		case 5:
			p_friend=imageAllcountry->getChildByName<Widget *>("image_sweden");
			break;
		case 6:
			p_friend=imageAllcountry->getChildByName<Widget *>("image_genoa");
			break;
		case 7:
			p_friend = imageAllcountry->getChildByName<Widget *>("image_holland");
			break;
		default:
			break;
		}
		p_friend->setTag(IMAGE_ICON_CLOCK+IMAGE_INDEX2+result->nation[i-1]->nationid);
		if (p_friend!=nullptr)
		{

			if (result->nation[i - 1]->value < 0 && result->nation[i - 1]->value >= -100000)
			{
				p_friend_num = -1;
			}
			else if (result->nation[i - 1]->value < -100000)
			{
				p_friend_num = result->nation[i - 1]->value / 100000.0 - 0.5;
			}
			else
			{
				p_friend_num = result->nation[i - 1]->value / 100000.0 + 0.5;
			}
			((ImageView*)p_friend)->loadTexture(getCountryIconPath(result->nation[i-1]->nationid));
			auto i_friend=p_friend->getChildByName<ImageView*>("image_friendliness_status");
			p_friend->setTouchEnabled(true);
			p_friend->addTouchEventListener(CC_CALLBACK_2(UIPalace::palacefriendValueFlag, this));

			if (p_friend_num >= 0)
			{
				i_friend->loadTexture(ICON_FRIEND_NPC);
				i_friend->setTouchEnabled(true);
				i_friend->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func, this));
				i_friend->setTag(IMAGE_ICON_FRIEND + IMAGE_INDEX2);
			}
			else
			{
				i_friend->loadTexture(ICON_ENEMY_NPC);
				i_friend->loadTexture(ICON_ENEMY_NPC);
				i_friend->setTouchEnabled(true);
				i_friend->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func, this));
				i_friend->setTag(IMAGE_ICON_ENEMY + IMAGE_INDEX2);
			}
			Text *t_num = p_friend->getChildByName<Text*>("label_num");
			t_num->setString(StringUtils::format("%d",p_friend_num));
		}
	}
}

void UIPalace::showAnnouncement(const GetDevGoodsInfoResult*result) //not mayor announcement
{

	auto viewoffic_Announce=getViewRoot(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB]);
	auto panelDemand=viewoffic_Announce->getChildByName<Widget*>("panel_demand");
	panelDemand->setVisible(true);
	//skill
	auto image_skill_bg_1=viewoffic_Announce->getChildByName<ImageView*>("image_skill_bg_1");
	if (result->skill_administration)
	{
		image_skill_bg_1->setVisible(true);
		image_skill_bg_1->ignoreContentAdaptWithSize(false);
		image_skill_bg_1->loadTexture(getSkillIconPath(SKILL_ADMINISTRATION, SKILL_TYPE_PLAYER));
		image_skill_bg_1->setTag(SKILL_ADMINISTRATION);
		image_skill_bg_1->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func,this));
		auto skill_lv = image_skill_bg_1->getChildByName<Text*>("text_item_skill_lv");
		setTextSizeAndOutline(skill_lv,result->skill_administration);
	}else
	{
		image_skill_bg_1->setVisible(false);
	}
	
	if (1 || result->mayorid)
	{
		for (int i = 0; i < 3; i++)
		{
			std::string panelIndex = StringUtils::format("panel_demand_%d",i + 1);
			auto panelChildDemad=panelDemand->getChildByName(panelIndex);

			auto imageBg=panelChildDemad->getChildByName("image_items_bg");
			auto firstgood=imageBg->getChildByName<ImageView*>("image_item");
			firstgood->ignoreContentAdaptWithSize(false);	
			firstgood->loadTexture(getGoodsIconPath(result->goods[i]->itemid));

			firstgood->setVisible(true);
			firstgood->setTouchEnabled(true);
			firstgood->setTag(i+1);
			firstgood->addTouchEventListener(CC_CALLBACK_2(UIPalace::provideItemDetail,this));
			auto labelgoodNum=panelChildDemad->getChildByName<Text*>("label_items_num");
			labelgoodNum->setString(StringUtils::format("%lld/%lld",result->goods[i]->currentcount,result->goods[i]->requiredcount));
			auto progressBarBg=panelChildDemad->getChildByName("image_progressbar");
			auto progressBar=progressBarBg->getChildByName<LoadingBar*>("progressbar_durable");	
			progressBar->setPercent(100.0*result->goods[i]->currentcount/result->goods[i]->requiredcount);

			auto provideButton=panelChildDemad->getChildByName<Button*>("button_provide");
			provideButton->setTag(i+1);
			provideButton->setTouchEnabled(true);
			provideButton->setBright(true);
		}
	}
	else
	{
		showNogoodPanel(panelDemand);
	}
	showDeveloptext(result);
}

void UIPalace::showOfficeAnnouncement(const GetDevGoodsInfoResult*result)// mayor announcement
{
	auto viewofficeAnn=getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_DECIDED_CSB]);
	auto panelDemand=viewofficeAnn->getChildByName<Widget*>("panel_demand");
	if (0 && !result->mayorid)
	{
		panelDemand->setVisible(true);
		showNogoodPanel(panelDemand);
	}
	else
	{
		panelDemand->setVisible(true);
		for (int i = 0; i < 3; i++)
		{
			std::string panelIndex = StringUtils::format("panel_demand_%d",i + 1);
			auto panelChildDemad=panelDemand->getChildByName(panelIndex);

			auto imageBg=panelChildDemad->getChildByName("image_items_bg");
			auto firstgood=imageBg->getChildByName<ImageView*>("image_item");
			firstgood->ignoreContentAdaptWithSize(false);
			firstgood->loadTexture(getGoodsIconPath(result->goods[i]->itemid));

			firstgood->setVisible(true);
			firstgood->setTag(i+1);
			firstgood->setTouchEnabled(true);
			firstgood->addTouchEventListener(CC_CALLBACK_2(UIPalace::provideItemDetail,this));
			auto labelgoodNum=panelChildDemad->getChildByName<Text*>("label_items_num");
			labelgoodNum->setString(StringUtils::format("%lld/%lld",result->goods[i]->currentcount,result->goods[i]->requiredcount));
			auto progressBarBg=panelChildDemad->getChildByName("image_progressbar");
			auto progressBar=progressBarBg->getChildByName<LoadingBar*>("progressbar_durable");	
			progressBar->setPercent(100.0*result->goods[i]->currentcount/result->goods[i]->requiredcount);
			auto provideButton=panelChildDemad->getChildByName<Button*>("button_provide");
			provideButton->setTag(i+1);
		}
	}
	//skill
	auto image_skill_bg_1=viewofficeAnn->getChildByName<ImageView*>("image_skill_bg_1");
	if (result->skill_administration)
	{
		image_skill_bg_1->setVisible(true);
		image_skill_bg_1->ignoreContentAdaptWithSize(false);
		image_skill_bg_1->loadTexture(getSkillIconPath(SKILL_ADMINISTRATION, SKILL_TYPE_PLAYER));
		image_skill_bg_1->setTag(SKILL_ADMINISTRATION);
		image_skill_bg_1->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func,this));

		auto skill_lv = image_skill_bg_1->getChildByName<Text*>("text_item_skill_lv");
		setTextSizeAndOutline(skill_lv,result->skill_administration);
	}else
	{
		image_skill_bg_1->setVisible(false);
	}
	showDeveloptext(result);
	
	//reward and reputation
	auto panelUnreard=viewofficeAnn->getChildByName("panel_unreward");
	auto panelReward=viewofficeAnn->getChildByName("panel_reward");
	auto labelReward=panelReward->getChildByName<Text*>("label_reward");
	labelReward->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_OFFICE_GOALACHIEVED_TEXT"]);

	panelUnreard->setVisible(true);
	panelReward->setVisible(false);
	auto labelSilverNum=panelUnreard->getChildByName<Text*>("label_silver_num");
	labelSilverNum->setString(numSegment(StringUtils::format("%d",result->rewardcoins)));
	auto labelReputation=panelUnreard->getChildByName<Text*>("label_r_num");
	labelReputation->setString(StringUtils::format("%d",result->rewardfame));
	if ((result->goods[0]->currentcount==result->goods[0]->requiredcount)&&(result->goods[1]->currentcount==result->goods[1]->requiredcount)
		&&(result->goods[2]->currentcount==result->goods[2]->requiredcount))
	{
		panelUnreard->setVisible(false);
		panelReward->setVisible(true);
	}
	else
	{
		panelUnreard->setVisible(true);
		panelReward->setVisible(false);
	}
}

void UIPalace::showInvestView(const GetCurrentInvestDataResult *result)
{
	if (!result) return;
	auto viewInvestment = getViewRoot(PALACE_COCOS_RES[VIEW_INVESTMENT_CSB]);
	if (!viewInvestment)
	{
		return;
	}
	auto  lastSessionlabel = viewInvestment->getChildByName<Text*>("label_last_session");
	auto label_content = viewInvestment->getChildByName<Text*>("label_content");
	if (SINGLE_HERO->m_iCurCityNation == 8){
		label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_INVESTMENT_TEXT_SUDAN"]);
	}else{
		label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_INVESTMENT_TEXT"]);
	}
	auto lastSession=viewInvestment->getChildByName<Text*>("label_last_session_num");
	lastSession->setString(numSegment(StringUtils::format("%lld",result->lastsessiontopinvest)));
	auto youinvestlabel=viewInvestment->getChildByName<Text*>("label_you_total_invest");
	auto youinvest=viewInvestment->getChildByName<Text*>("label_you_total_invest_num");
	youinvest->setString(numSegment(StringUtils::format("%lld",result->myinvest)));
	//skill
	auto image_skill_bg_1=viewInvestment->getChildByName<ImageView*>("image_skill_bg_1");

	if (result->skill_public_speaking)
	{
		image_skill_bg_1->setVisible(true);
		image_skill_bg_1->ignoreContentAdaptWithSize(false);
		image_skill_bg_1->loadTexture(getSkillIconPath(SKILL_PUBLIC_SPEAKING, SKILL_TYPE_PLAYER));
		image_skill_bg_1->setTag(SKILL_PUBLIC_SPEAKING);
		image_skill_bg_1->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func,this));

		auto skill_lv = image_skill_bg_1->getChildByName<Text*>("text_item_skill_lv");
		setTextSizeAndOutline(skill_lv,result->skill_public_speaking);
	}else
	{
		image_skill_bg_1->setVisible(false);
	}
	this->scheduleUpdate();
}

void UIPalace::showMainCoin()
{
	auto viewBar=getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB])->getChildByName("panel_actionbar");
	auto bntSliver=viewBar->getChildByName<Button*>("button_silver");
	auto label_silvernum=bntSliver->getChildByName<Text*>("label_silver_num");
	auto bntGold=viewBar->getChildByName<Button*>("button_gold");
	auto label_goldnum=bntGold->getChildByName<Text*>("label_gold_num");
	label_silvernum->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iCoin)));
	label_goldnum->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iGold)));
	setTextFontSize(label_silvernum);
	setTextFontSize(label_goldnum);
}

void UIPalace::onRankView(const GetRankListResult *result)
{	
	if (!result) return;
	openView(PALACE_COCOS_RES[VIEW_HALL_OF_HONOR_CSB]);
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_HALL_OF_HONOR_CSB]);
	viewAction(view, PALACE_COCOS_RES[VIEW_HALL_OF_HONOR_CSB]);
	if (!view) return;
	auto WinSize = Director::getInstance()->getWinSize();
	auto label1 = Label::create(" ", " ", 38);
	label1->setColor(Color3B(46, 29, 14));
	auto imageBg = view->getChildByName("image_bg");
	label1->setVisible(false);
	label1->setTag(1);
	imageBg->addChild(label1);

	label1->setPosition(Vec2(imageBg->getContentSize().width / 2, imageBg->getContentSize().height / 2 + 50));

	//头像
	auto image_head_bg = view->getChildByName<ImageView*>("image_head_bg");
	auto playHeroIcon = image_head_bg->getChildByName<ImageView*>("image_head_16");
	playHeroIcon->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));
	auto listRanking = view->getChildByName<ListView*>("listview_ranking");
	auto listviewYouRank = view->getChildByName<ListView*>("listview_you_rank");
	auto labelYou_rank_num = dynamic_cast<Text*>(listviewYouRank->getItem(1));
	auto listviewYouTotalInvest = view->getChildByName<ListView*>("listview_you_total_invest");
	auto labelYou_total_invest_num = dynamic_cast<Text*>(listviewYouTotalInvest->getItem(2));

	std::string s_rank;
	if (result->myrank == 0)
	{
		s_rank = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NOT_RANK"];
	}
	else
	{
		s_rank = StringUtils::format("NO.%d", result->myrank);
	}
	labelYou_rank_num->setString(s_rank);
	labelYou_total_invest_num->setString(numSegment(StringUtils::format("%lld", result->myinvest)));
	listviewYouRank->refreshView();
	listviewYouTotalInvest->refreshView();

	auto w_item = listRanking->getItem(0);
	listRanking->setVisible(false);

	auto i_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = i_pulldown->getChildByName<Button*>("button_pulldown");
	if (result->n_rankinfo<1)
	{
		w_item->setVisible(false);
		w_item->autorelease();
		label1->setVisible(true);
		label1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_RANKVIEW_HAVENONE_INVESTOR"]);
		i_pulldown->setVisible(false);
		return;
	}
	else
	{
		i_pulldown->setVisible(true);
		imageBg->removeChildByTag(1);
	}
	button_pulldown->setPositionX(i_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2 + 12);
	auto t_title = view->getChildByName<Text*>("label_forces_relationship");
	auto btn_info = view->getChildByName("button_invest_Rank_info");
	t_title->ignoreContentAdaptWithSize(true);
	btn_info->setPositionX(t_title->getPositionX() + t_title->getContentSize().width / 2 + btn_info->getContentSize().width / 2);
	//添加tableView
	TableView* mTableView = dynamic_cast<TableView*>(view->getChildByTag(1024));
	if (mTableView)
	{
		mTableView->reloadData();
		return;
	}
	if (!mTableView){
		mTableView = TableView::create(this, CCSizeMake(listRanking->getContentSize().width, listRanking->getContentSize().height));
		view->addChild(mTableView, 100);
	}
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(listRanking->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(1024);
	mTableView->setTouchEnabled(true);
	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	mTableView->setBounceable(true);
	this->setCameraMask(_cameraMask, true);
	//for (auto i=0;i<num;i++)
	//{
	//	auto item=w_item->clone();
	//	item->setTag(i + START_INDEX);
	//	item->setVisible(true);
	//	listRanking->pushBackCustomItem(item);
	//	auto rankItem=result->rankinfo[i];
	//	auto rank_Image=item->getChildByName<ImageView*>("image_trophy");//奖杯图片
	//	rank_Image->setVisible(false);
	//	rank_Image->ignoreContentAdaptWithSize(false);

	//	auto label_rank=item->getChildByName<Text*>("label_rank");
	//	
	//	auto imagCounty=item->getChildByName<ImageView*>("image_country");
	//	Widget* listview_name_lv=item->getChildByName<Widget*>("listview_name_lv");
	//	auto label_name=listview_name_lv->getChildByName<Text*>("label_name");
	//	int rankNum = 0;
	//	if (i==0)
	//	{
	//		label_rank->setString(StringUtils::format("%d", i+1));
	//		rankNum = i+1;
	//	}
	//	else if (i > 0)
	//	{
	//		for (int j = 0; j < i; j++)
	//		{
	//			if (result->rankinfo[j]->investment == result->rankinfo[i]->investment)
	//			{
	//				rankNum = j + 1;
	//				break;
	//			}
	//			else
	//			{
	//				rankNum = j+2;
	//			}
	//		}

	//		label_rank->setString(StringUtils::format("%d", rankNum));
	//	}
	//	if (rankNum == 1)
	//	{
	//		rank_Image->setVisible(true);
	//		rank_Image->loadTexture(RANK_GOLD);
	//		label_rank->setColor(Color3B::WHITE);
	//	}
	//	else if (rankNum == 2)
	//	{
	//		rank_Image->setVisible(true);
	//		rank_Image->loadTexture(RANK_SILVER);
	//		label_rank->setColor(Color3B::WHITE);
	//	}
	//	else if (rankNum == 3)
	//	{
	//		rank_Image->setVisible(true);
	//		rank_Image->loadTexture(RANK_COPPER);
	//		label_rank->setColor(Color3B::WHITE);
	//	}
	//	else
	//	{
	//		label_rank->setColor(Color3B(46, 29, 14));
	//	}
	//
	//	item->addTouchEventListener(CC_CALLBACK_2(PalaceLayer::ranklistInfo,this));
	//	item->setTag(i+1);
	//	auto label_lv=listview_name_lv->getChildByName<Text*>("label_lv");
	//	auto labelSilver_num=item->getChildByName<Text*>("label_silver_num");
	//	
	//
	//	if (SINGLE_HERO->m_iID==rankItem->characterid)
	//	{
	//		rankItem->level=SINGLE_HERO->m_iLevel;
	//	}
	//	imagCounty->loadTexture(getCountryIconPath(rankItem->country));
	//	label_name->setString(StringUtils::format("%s",rankItem->name));
	//	
	//	label_lv->setString(StringUtils::format("Lv. %d",rankItem->level));
	//	labelSilver_num->setString(numSegment(StringUtils::format("%lld",rankItem->investment)));
	//}
	//
	//button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 + 3);
	//addListViewBar(listRanking,image_pulldown);
	//chengyuan++
	/*int j = 0;
	for (auto item : listRanking->getItems())
	{
	j++;
	auto i_silver = item->getChildByName("image_silver");
	auto t_silver_num = item->getChildByName<Text*>("label_silver_num");
	t_silver_num->ignoreContentAdaptWithSize(true);
	i_silver->setPositionX(t_silver_num->getPositionX() - t_silver_num->getContentSize().width - i_silver->getContentSize().width / 2 - 6);
	if (j <= 3)
	{
	continue;
	}
	else
	{
	auto t_rank = item->getChildByName<Text*>("label_rank");
	}
	}*/
}

void UIPalace::showCurrInvestTop(const GetCurrentInvestDataResult *result)
{
	if (!getViewRoot(PALACE_COCOS_RES[VIEW_INVESTMENT_CSB])) return;
	Widget* imageTopBg=getViewRoot(PALACE_COCOS_RES[VIEW_INVESTMENT_CSB])->getChildByName<Widget*>("image_top_bg");
	
	auto  listviewtop=imageTopBg->getChildByName<ListView*>("listview_name_lv");
	
	auto labelname =dynamic_cast<Text*>(listviewtop->getItem(0));
	auto labelLv=dynamic_cast<Text*>(listviewtop->getItem(1));
	auto countryBg=imageTopBg->getChildByName<ImageView*>("image_country");
	auto labelSilverNum=imageTopBg->getChildByName<Text*>("label_silver_num");
	if (!result->thissessiontop)
	{
		labelname->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NOT_RANK"]);
		labelLv->setVisible(false);
		countryBg->setVisible(false);
		labelSilverNum->setString(" 0");
	}
	else
	{
		if (result->thissessiontop->characterid==SINGLE_HERO->m_iID)
		{
			result->thissessiontop->level=SINGLE_HERO->m_iLevel;
		}
		labelname->setString(result->thissessiontop->name);
		labelLv->setVisible(true);
		labelLv->setString(StringUtils::format("Lv. %d",result->thissessiontop->level));
		countryBg->setVisible(true);
		countryBg->loadTexture(getCountryIconPath(result->thissessiontop->country));
		labelSilverNum->setString(numSegment(StringUtils::format("%lld",result->thissessiontop->investment)));
	}
	listviewtop->refreshView();
//chengyuan++
	float font_size_b = 20;
	float font_size_s = 12;
	auto i_silver = imageTopBg->getChildByName("image_silver");	
	i_silver->setPositionX(labelSilverNum->getPositionX() - labelSilverNum->getContentSize().width - i_silver->getContentSize().width / 2);
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_INVESTMENT_CSB]);
	auto lastSession =  view->getChildByName<Text*>("label_last_session_num");
	auto youinvest = view->getChildByName<Text*>("label_you_total_invest_num");
	auto i_silver_2 = view->getChildByName("image_silver_1");
	auto i_silver_3 = view->getChildByName("image_silver_2");
	int str_len_1 = labelSilverNum->getString().size();
	int str_len_2 = lastSession->getString().size();
	int str_len_3 = youinvest->getString().size();
	i_silver->setPositionX(labelSilverNum->getPositionX() - font_size_b * str_len_1 - i_silver->getContentSize().width / 2 - 6);
	i_silver_2->setPositionX(lastSession->getPositionX() -  font_size_s * str_len_2 - i_silver_2->getContentSize().width / 2 - 6);
	i_silver_3->setPositionX(youinvest->getPositionX() - font_size_s * str_len_3- i_silver_3->getContentSize().width / 2  - 6);
//
}

void UIPalace::firstDayChosse(const GetPalaceInfoResult *result)
{
	auto viewoffic_UN = getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_UNDECIDED_CSB]);
	auto officeDevWayBg=viewoffic_UN->getChildByName<ImageView*>("image_city_trade");
	auto labelPlice=officeDevWayBg->getChildByName<Text*>("label_trade");
	auto label_content_1=viewoffic_UN->getChildByName<Text*>("label_content_1");
	auto label_content_2=viewoffic_UN->getChildByName<Text*>("label_content_2");
	label_content_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_OFFICE_FIRSTDAY_1_TEXT"]);
	label_content_2->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_OFFICE_FIRSTDAY_2_TEXT"]);

 	std::string  s_direct = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_TITLE"];

	int dirNum = result->newdevdirection;
 	if (dirNum==1)
 	{
 		officeDevWayBg->loadTexture("res/palace_developway/city_population.png");
 		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_POPULATION"];
 	}
 	else if (dirNum==2)
 	{
 		officeDevWayBg->loadTexture("res/palace_developway/city_trade.png");
 		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_TRADE"];
 	}
 	else if (dirNum==3)
 	{
 		officeDevWayBg->loadTexture("res/palace_developway/city_manufacture.png");
 		s_direct += SINGLE_SHOP->getTipsInfo()["TIP_PALACE_CITY_DIRECTION_MAKING"];
 	}
	else
 	{
 		officeDevWayBg->loadTexture("res/palace_developway/city_no.png");
 		s_direct +=SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NOT_RANK"];
 	}
 	labelPlice->setString(s_direct);
}

void  UIPalace::onPalaceSliderNumbers(int num)
{
	auto view=getViewRoot(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
	if (!view) return;
	m_pDevGoodCount->count=num;
	auto label_dropitem_num=view->getChildByName<Text*>("label_dropitem_num");
	label_dropitem_num->setString(StringUtils::format("x %d",num));
	std::string dropitemstring=label_dropitem_num->getString();
	std::string dropitemNum=dropitemstring.substr(2,dropitemstring.length()-2);
	m_nProvideItemNum=atoi(dropitemNum.c_str());
}

void UIPalace::palaceoutofscreen()
{
	auto view=getViewRoot(PALACE_COCOS_RES[VIEW_PLACE_CSB])->getChildByName("panel_palace");
	auto panelDialog= view->getChildByName("panel_dialog");
	auto roleGuard=panelDialog->getChildByName<ImageView*>("image_soldier");
	auto image_dialog_bg=panelDialog->getChildByName<ImageView*>("image_dialog_bg");
	auto labelguard=panelDialog->getChildByName("label_guard");
	auto guardText=panelDialog->getChildByName<Text*>("label_content");
	
	image_dialog_bg->runAction(FadeOut::create(0.1));
	labelguard->runAction(FadeOut::create(0.1));
	guardText->runAction(FadeOut::create(0.1));
	roleGuard->runAction(MoveTo::create(0.5f,Vec2(-600,0)));

	auto i_anchor = panelDialog->getChildByName<ImageView*>("image_anchor");
	i_anchor->setOpacity(0);
	i_anchor->stopAllActions();
	m_bDialogOut = true;//对话已退出
}

void UIPalace::ranklistInfo(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType!=Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget*widget=dynamic_cast<Widget*>(pSender);
	int rankTag=widget->getTag()-1;
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//showRanklistPerInfo(rankTag);
	m_operateWidgetTag = m_pRankResult->rankinfo[rankTag]->characterid;
	ProtocolThread::GetInstance()->getUserInfoById(m_operateWidgetTag, UILoadingIndicator::create(this));
}

void UIPalace::showRanklistPerInfo(int rankTag)
{
	if (!m_pRankResult) 
	{
		return;
	}
	openView(COMMOM_COCOS_RES[C_VIEW_FRIEND_DETAIL_CSB]);
	auto viewPerson=getViewRoot(COMMOM_COCOS_RES[C_VIEW_FRIEND_DETAIL_CSB]);
	auto singlePerson=m_pRankResult->rankinfo[rankTag];
	//UI
	auto panel_bio_info=viewPerson->getChildByName("panel_bio_info");
	auto image_head_bg=panel_bio_info->getChildByName("image_head_bg");
	auto image_head=image_head_bg->getChildByName<ImageView*>("image_head");
	image_head->loadTexture(getPlayerIconPath(singlePerson->iconidx));

	auto image_head_lv=image_head_bg->getChildByName("image_head_lv");
	auto label_lv=image_head_lv->getChildByName<Text*>("label_lv");
	label_lv->setString(StringUtils::format("%d",singlePerson->level));

	auto label_r_lv = image_head_bg->getChildByName<Text*>("label_r_lv");
	label_r_lv->setString(StringUtils::format("%d",singlePerson->famelv));

	auto image_flag=panel_bio_info->getChildByName<ImageView*>("image_flag");
	image_flag->loadTexture(getCountryIconPath(singlePerson->country));

	auto label_player_name=panel_bio_info->getChildByName<Text*>("label_player_name");
	label_player_name->setString(StringUtils::format("%s",singlePerson->name));
	
	auto label_bio_content=panel_bio_info->getChildByName<Text*>("label_bio_content");
	if (singlePerson->signature)
	{
		if (strcmp(singlePerson->signature, "") != 0)
		{
			label_bio_content->setString(StringUtils::format("%s", singlePerson->signature));
		}
	}

	auto listview_content=viewPerson->getChildByName<ListView*>("listview_content");
	auto button_guild=listview_content->getItem(1);

	auto panel_guild_right=button_guild->getChildByName("panel_guild_right");
	auto panel_guild_no=button_guild->getChildByName("panel_guild_no");
 	if (singlePerson->guildid)
 	{
		panel_guild_right->setVisible(true);
		panel_guild_no->setVisible(false);
		auto label_guild_name=panel_guild_right->getChildByName<Text*>("label_guild_name");
		auto image_guild_icon=panel_guild_right->getChildByName<ImageView*>("image_guild_icon");
		label_guild_name->setString(StringUtils::format("%s",singlePerson->guild));
		image_guild_icon->loadTexture(getGuildIconPath(singlePerson->guildicon));//guild icon
		image_guild_icon->setPositionX(label_guild_name->getPositionX() - label_guild_name->getContentSize().width - image_guild_icon->getContentSize().width/2 - 5);
	}else
	{
		panel_guild_no->setVisible(true);
		panel_guild_right->setVisible(false);
		auto label_guildName = panel_guild_no->getChildByName<Text*>("label_guild_name");
		label_guildName->setAnchorPoint(Vec2(1,0.5));
		auto button_guildInvite=panel_guild_no->getChildByName<Button*>("button_invite");
		button_guildInvite->addTouchEventListener(CC_CALLBACK_2(UIPalace::rankListPerInfoGUild,this));
		button_guildInvite->setTag(singlePerson->characterid);//tag;
		if (singlePerson->characterid == SINGLE_HERO->m_iID || !m_pRankResult->isadmin)
		{
			button_guildInvite->setVisible(false);
			label_guildName->setPositionX(button_guildInvite->getPositionX() + button_guildInvite->getContentSize().width / 2);
		}else
		{
			button_guildInvite->setVisible(true);
		}
		label_guildName->setTextHorizontalAlignment(TextHAlignment::RIGHT);
 	}

	auto button_played=listview_content->getItem(2);
	auto label_played_time=button_played->getChildByName<Text*>("label_played_time");
	float perHours = (singlePerson->onlinesec)/3600.0;
	label_played_time->setString(StringUtils::format("%0.2f %s",perHours,SINGLE_SHOP->getTipsInfo()["TIP_HOUR"].data()));//player time;

	if (singlePerson->characterid == SINGLE_HERO->m_iID)
	{
		log("It is me !");
	}
	else{
		//朋友
		if (singlePerson->relationwithme == 1)
		{
			auto p_btn = viewPerson->getChildByName<Layout*>("panel_btn_friend");
			p_btn->setVisible(true);
			p_btn->setTouchEnabled(true);
			auto btn_delete = p_btn->getChildByName<Button*>("button_friend_delete");
			auto btn_block = p_btn->getChildByName<Button*>("button_friend_block");
			btn_delete->setTag(singlePerson->characterid);
			btn_block->setTag(singlePerson->characterid);
		    btn_delete->addTouchEventListener(CC_CALLBACK_2(UIPalace::rankListPerInfoGUild,this));
			btn_block->addTouchEventListener(CC_CALLBACK_2(UIPalace::rankListPerInfoGUild,this));
		}
		//拉黑玩家
		else if (singlePerson->relationwithme == 2)
		{
			auto p_btn = viewPerson->getChildByName<Layout*>("panel_btn_block");
			p_btn->setVisible(true);
			p_btn->setTouchEnabled(true);
			auto btn_remvoe= p_btn->getChildByName<Button*>("button_stranger_remove");
			auto btn_add = p_btn->getChildByName<Button*>("button_add_friend");
			btn_remvoe->setTag(singlePerson->characterid);
			btn_add->setTag(singlePerson->characterid);
			btn_remvoe->addTouchEventListener(CC_CALLBACK_2(UIPalace::rankListPerInfoGUild,this));
			btn_add->addTouchEventListener(CC_CALLBACK_2(UIPalace::rankListPerInfoGUild,this));
		}else
		{
			auto p_btn = viewPerson->getChildByName<Layout*>("panel_btn_stranger");
			p_btn->setVisible(true);
			p_btn->setTouchEnabled(true);
			auto btn_block = p_btn->getChildByName<Button*>("button_stranger_block");
			auto btn_add = p_btn->getChildByName<Button*>("button_add_friend");
			btn_block->setTag(singlePerson->characterid);
			btn_add->setTag(singlePerson->characterid);
			btn_block->addTouchEventListener(CC_CALLBACK_2(UIPalace::rankListPerInfoGUild,this));
			btn_add->addTouchEventListener(CC_CALLBACK_2(UIPalace::rankListPerInfoGUild,this));
		}
	}

	listview_content->refreshView();
}

void UIPalace::rankListPerInfoGUild(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType!=Widget::TouchEventType::ENDED)
	{
		return;
	}
	Widget*widget=dynamic_cast<Widget*>(pSender);
	auto name = widget->getName();
	if (isButton(button_invite))
	{
		m_nInviteGuildTag = widget->getTag();
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_pRankResult->isadmin)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_GULID_BUTTON_INVITATIONS", "TIP_GUILD_INVINT_GUILD");
		}
		else
		{
			openSuccessOrFailDialog("TIP_PALACE_INVITEGUILD_FAIL_NO_ADMIN");
		}
		return;
	}

	if (isButton(button_add_friend))
	{
		m_operateWidgetTag = widget->getTag();
		m_confirmType = CONFIRM_INDEX_STRANGER_ADDFRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_ADD_FRIEND_TITLE", "TIP_SOCIAL_ADD_FRIEND");
		return;
	}
	
	if (isButton(button_friend_delete))
	{
		m_operateWidgetTag = widget->getTag();
		m_confirmType = CONFIRM_INDEX_REMOVEFRIEND;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_DELETE_FRIEND_TITLE", "TIP_SOCIAL_DELETE_FRIEND");
		return;
	}

	if (isButton(button_friend_block))
	{
		m_operateWidgetTag = widget->getTag();
		m_confirmType = CONFIRM_INDEX_FRIEND_BLOCK;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_FRIEND");
		return;
	}


	if (isButton(button_stranger_block))
	{
		m_operateWidgetTag = widget->getTag();
		m_confirmType = CONFIRM_INDEX_STRANGER_BLOCK;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_SOCIAL_BLOCK_TITLE", "TIP_SOCIAL_BLOCK_STRANGER");
		return;
	}

	if (isButton(button_stranger_remove))
	{
		m_operateWidgetTag = widget->getTag();
		m_confirmType = CONFIRM_INDEX_BLOCK_REMOVE;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_REMOVE_BLOCK_TITLE", "TIP_REMOVE_BLOCK");
		return;
	}
}

void UIPalace::provideItemDetail(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType!=Widget::TouchEventType::ENDED)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	auto imageName=(Widget*)pSender;
	std ::string name=imageName->getName();
	int imageTag=imageName->getTag()-1;
	showItemDetail(imageTag);
}

void UIPalace::showItemDetail(int imageTag)
{
	if (!m_pDevGoodsInfo) return;
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushGoodsDetail(nullptr,m_pDevGoodsInfo->goods[imageTag]->itemid,true);
}

void UIPalace::showDeveloptext(const GetDevGoodsInfoResult*result)
{
	// not mayor
	auto viewoffic_Announce=getViewRoot(PALACE_COCOS_RES[VIEW_ANNOUNCEMENT_CSB]);
	auto viewofficeAnn=getViewRoot(PALACE_COCOS_RES[VIEW_OFFICE_DECIDED_CSB]);//市长
	Text* label_population=nullptr;
	Text*label_content=nullptr;
	if (viewoffic_Announce)
	{
		label_population=viewoffic_Announce->getChildByName<Text*>("label_population");
		label_content=label_population;
	}
	if(viewofficeAnn)
	{
		 label_content=viewofficeAnn->getChildByName<Text*>("label_content");
		 label_population=label_content;
	}
	
	int dirNum = result->newdevdirection;

	if (dirNum==1)
	{
		label_population->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_DEVELOP_POPULATION"]);
		label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_DEVELOP_POPULATION"]);
	}
	else if (dirNum==2)
	{
		label_population->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_DEVELOP_TRADE"]);
		label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_DEVELOP_TRADE"]);
	}
	else if (dirNum==3)
	{
		label_population->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_DEVELOP_MAKE"]);
		label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_DEVELOP_MAKE"]);
	}
	else
	{
		label_population->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NO_DEVELOP_WAY"]);
		label_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NO_DEVELOP_WAY"]);
	}
}

void UIPalace::showBussinessliceView()
{
	//business license
	auto viewCity=getViewRoot(PALACE_COCOS_RES[VIEW_CITYINFO_CSB]);
	auto listviewContent=viewCity->getChildByName("listview_content");
	auto panelContent=listviewContent->getChildByName("panel_content");

	auto panel_license_1 = dynamic_cast<Layout*>(Helper::seekWidgetByName((Widget*)panelContent,"panel_license_1"));
	auto moveHight1=panel_license_1->getContentSize().height;
	auto image_div_3= dynamic_cast<ImageView*>(Helper::seekWidgetByName((Widget*)panelContent,"image_div_3"));
	auto moveHight2=image_div_3->getContentSize().height;
	auto image_license = dynamic_cast<ImageView*>(Helper::seekWidgetByName((Widget*)panelContent, "image_license"));
	auto label_require=dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent,"label_require"));
	auto image_r = dynamic_cast<ImageView*>(Helper::seekWidgetByName((Widget*)panelContent,"image_r"));
	auto image_r_2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName((Widget*)panelContent, "image_r_2"));
	auto label_require_num=dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent,"label_r_lv_0"));
	auto label_require_0 = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent, "label_require_0"));
	auto button_yesorno=dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)panelContent,"button_yesorno"));
	auto b_buy = dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)panelContent,"button_buy"));
	auto right_yesorno=dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)panelContent,"right_yesorno"));
	if (SINGLE_HERO->m_iHaslisence)//own
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

		if (!m_InfoViewalreadyOpen)
		{
			Vector<Node*>children;
			children = panelContent->getChildren();
			for (auto item : children)
			{
				std::string str = item->getName();
				if (str.compare("panel_pp") == 0 || str.compare("image_div_1") == 0 || str.compare("panel_license_1") == 0 || str.compare("image_div_3") == 0)
				{
					continue;
				}
				else
				{
					item->setPositionY(item->getPositionY() + (moveHight1 / 2 + moveHight2)*2);
				}

			}
			m_InfoViewalreadyOpen = true;
			if (SINGLE_HERO->m_iEffectivelisence == 0)
			{
				panel_license_1->setVisible(false);
				image_div_3->setVisible(false);
				image_r->setVisible(false);
				image_r_2->setVisible(true);
				label_require_num->setVisible(false);
				button_yesorno->setVisible(false);
				right_yesorno->setVisible(false);
				b_buy->setVisible(false);
				b_buy->setBright(false);
				setGLProgramState(image_license,true);
				label_require->setString(SINGLE_SHOP->getTipsInfo()["TIP_LOADING_TEXT_16"]);
				label_require->setColor(Color3B(46, 125, 50));
				label_require_0->setString(numSegment(StringUtils::format("%d", m_pPalaceResult->fames)) + "/" + numSegment(StringUtils::format("%d", m_pPalaceResult->licenserequiredfame)));
				label_require_0->setPositionX(image_r->getPositionX() + label_require_0->getContentSize().width/2 + 20);
				label_require_0->setTextColor(Color4B::RED);
				label_require->setVisible(true);
				label_require_0->setVisible(true);
			}
		}
	}
	else
	{
		panel_license_1->setVisible(true);
		image_div_3->setVisible(true);
		image_r->setVisible(true);
		label_require_num->setVisible(true);
		button_yesorno->setVisible(true);
		b_buy->setVisible(true);
		b_buy->setBright(true);
		label_require->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE_REQUIRE"]);
		label_require->setColor(Color3B(46,29,14));
		if (SINGLE_HERO->m_iCoin < m_pPalaceResult->licenseprice)
		{
			right_yesorno->setVisible(false);
		}
		else
		{
			right_yesorno->setVisible(false);
		}
	}
	auto t_require = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent,"label_r_lv_0"));
	t_require->setString(StringUtils::format("%d",m_pPalaceResult->licenserequiredfamelv));
	auto b_require = dynamic_cast<Button*>(Helper::seekWidgetByName((Widget*)panelContent,"button_yesorno"));

	if (SINGLE_HERO->m_iPrestigeLv >= m_pPalaceResult->licenserequiredfamelv)
	{
		b_require->setBright(true);
	}else
	{
		b_require->setBright(false);
	}
	auto t_silver = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent,"Label_33"));
	t_silver->setString(numSegment(StringUtils::format("%d",m_pPalaceResult->licenseprice)));
	setTextFontSize(t_silver);

	auto label_license_1=dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent,"label_license_1"));
	label_license_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE_NO"]);
	if (SINGLE_HERO->m_iEffectivelisence == 0)
	{
		auto label_license = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent, "label_license"));
		label_license->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE_DISABLED"]);
		auto a = label_license->getString();
	}
	else
	{
		auto label_license = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panelContent, "label_license"));
		label_license->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE"]);
		auto a = label_license->getString();
	}
}

void UIPalace::palacefriendValueFlag(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType!=Widget::TouchEventType::ENDED)
	{
		return;
	}
	int tag = dynamic_cast<Widget*>(pSender)->getTag();
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushImageDetail(dynamic_cast<Widget*>(pSender));
}

void UIPalace::showNogoodPanel(const Widget*panelDemand)
{
	for (int i=0;i<3;i++)
	{
		std::string panelIndex = StringUtils::format("panel_demand_%d",i + 1);
		auto panelChildDemad=panelDemand->getChildByName(panelIndex);

		auto imageBg=panelChildDemad->getChildByName("image_items_bg");
		auto firstgood=imageBg->getChildByName<ImageView*>("image_item");
		firstgood->ignoreContentAdaptWithSize(false);
		firstgood->setVisible(false);
		auto labelgoodNum=panelChildDemad->getChildByName<Text*>("label_items_num");
		labelgoodNum->setString("--/--");
		auto progressBarBg=panelChildDemad->getChildByName("image_progressbar");
		auto progressBar=progressBarBg->getChildByName<LoadingBar*>("progressbar_durable");	
		progressBar->setPercent(0);
		auto provideButton=panelChildDemad->getChildByName<Button*>("button_provide");
		provideButton->setTag(i+1);
		provideButton->setTouchEnabled(false);
		provideButton->setBright(false);
	}
}

void UIPalace::showInvestCoinResult(const InvestCityResult*result)
{
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushCostOnlyCoin(m_nInvestCoinNum,"TIP_PALACE_INVEST_INFO_TITLE","TIP_COMMON_CSB_PALACE_INVEST_RESULT","TIP_PALACE_INVEST_INFO_TITLE");
	UICommon::getInstance()->flushPlayerAddExpOrFrame(result->exp,result->fame,0,result->add_fame);
}

void UIPalace::changeMainButtonState(Widget*target)
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

void UIPalace::showPalacePopularGood()
{
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_CITYINFO_CSB]);
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
	}
	else
	{
		i_popular_product_no->setVisible(false);
		i_title->setVisible(true);
		if (goodCount ==1)
		{
			panel_item_1->setVisible(true);
			panel_item_1->setPositionX(ppWidth*1.0 / 2.5);
		}
		else if (goodCount ==2)
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

void UIPalace::showPalaceNumPad()
{
	m_bNumpadFlag = false;
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB]);
	auto p_numpad_mask = view->getChildByName<Widget*>("panel_numpad_mask");
	p_numpad_mask->setPosition(STARTPOS);
	auto num_root = view->getChildByName<Widget*>("panel_numpad");
	num_root->setPosition(ENDPOS4);
	num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, STARTPOS),
		CallFunc::create(this, callfunc_selector(UIPalace::moveEnd))));
	auto b_yes = num_root->getChildByName<Button*>("button_numpad_yes");
	b_yes->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func,this));
	auto b_del = num_root->getChildByName<Button*>("button_del");
	b_del->addTouchEventListener(CC_CALLBACK_2(UIPalace::investNumpadEvent, this));
	p_numpad_mask->addTouchEventListener(CC_CALLBACK_2(UIPalace::menuCall_func,this));
	for (size_t i = 0; i < 10; i++)
	{
		auto b_num = dynamic_cast<Button*>(Helper::seekWidgetByName(num_root, StringUtils::format("button_%d", i)));
		b_num->setTag(i);
		b_num->addTouchEventListener(CC_CALLBACK_2(UIPalace::investNumpadEvent, this));
	}
}

void UIPalace::investNumpadEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	auto target = dynamic_cast<Widget*>(pSender);
	std::string name = target->getName();
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB]);
	auto label_currency_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_currency_num"));
//	std::string str = label_currency_num->getString();
	if (isButton(button_del))
	{
		if (!m_padStirng.empty())
		{
			m_padStirng.erase(m_padStirng.end() - 1);
		}
	}
	else
	{
		int tag = target->getTag();

		if (m_padStirng.size() < 2 && m_padStirng == "0")
		{
			m_padStirng.erase(m_padStirng.end() - 1);
		}
		m_padStirng += StringUtils::format("%d", tag);
	}

	std::string str = numSegment(m_padStirng);
	label_currency_num->setString(str);
}

void UIPalace::update(float dt)
{
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_INVEST_DIALOG_CSB]);
	if (view)
	{
		auto t_num = view->getChildByName<Text*>("label_currency_num");
		if (t_num->getString().compare("0") == 0)
		{
			t_num->setOpacity(127);
		}
		else
		{
			t_num->setOpacity(255);
		}
	}
}

void UIPalace::buttonAction(float dt)
{
	auto  twoButter = this->getChildByName("panel_two_butter");
	auto btn_cityinfo = twoButter->getChildByName<Button*>("button_city_info");
	auto btn_hallofhonor = twoButter->getChildByName<Button*>("button_hall_of_honor");
	auto btn_investment = twoButter->getChildByName<Button*>("button_investment");
	auto btn_office = twoButter->getChildByName<Button*>("button_office");
	auto btn_war = twoButter->getChildByName<Button*>("button_war");
	int offPosX = twoButter->getPositionX() - _director->getWinSize().width;
	btn_cityinfo->setPositionX(btn_cityinfo->getPositionX() - offPosX);
	btn_hallofhonor->setPositionX(btn_hallofhonor->getPositionX() - offPosX);
	btn_investment->setPositionX(btn_investment->getPositionX() - offPosX);
	btn_office->setPositionX(btn_office->getPositionX() - offPosX);
	btn_war->setPositionX(btn_war->getPositionX() - offPosX);
	
	btn_cityinfo->runAction(Sequence::create(MoveBy::create(0.2, Vec2(-btn_cityinfo->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	btn_hallofhonor->runAction(Sequence::create(DelayTime::create(0.05), MoveBy::create(0.2, Vec2(-btn_hallofhonor->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	btn_investment->runAction(Sequence::create(DelayTime::create(0.1), MoveBy::create(0.2, Vec2(-btn_investment->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	btn_office->runAction(Sequence::create(DelayTime::create(0.15), MoveBy::create(0.2, Vec2(-btn_office->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));
	btn_war->runAction(Sequence::create(DelayTime::create(0.15), MoveBy::create(0.2, Vec2(-btn_war->boundingBox().size.width - 100, 0)), MoveBy::create(0.1, Vec2(50, 0)), nullptr));

	//不可以发动国战的 -- 隐藏国战按钮
	if (SINGLE_HERO->m_iCurCityNation > 5)
	{
		btn_war->setVisible(false);
	}
}

void UIPalace::buttonMoveRight()
{
	auto  twoButter = this->getChildByName("panel_two_butter");
	auto btn_cityinfo = twoButter->getChildByName<Button*>("button_city_info");
	auto btn_hallofhonor = twoButter->getChildByName<Button*>("button_hall_of_honor");
	auto btn_investment = twoButter->getChildByName<Button*>("button_investment");
	auto btn_office = twoButter->getChildByName<Button*>("button_office");
	auto btn_war = twoButter->getChildByName<Button*>("button_war");
	btn_cityinfo->runAction(Sequence::create(DelayTime::create(0.05), MoveBy::create(0.2, Vec2(btn_cityinfo->boundingBox().size.width / 3 + 20, 0)), nullptr));
	btn_hallofhonor->runAction(Sequence::create(DelayTime::create(0.1), MoveBy::create(0.2, Vec2(btn_cityinfo->boundingBox().size.width / 3 + 20, 0)), nullptr));
	btn_investment->runAction(Sequence::create(DelayTime::create(0.15), MoveBy::create(0.2, Vec2(btn_cityinfo->boundingBox().size.width / 3 + 20, 0)), nullptr));
	btn_office->runAction(Sequence::create(DelayTime::create(0.2), MoveBy::create(0.2, Vec2(btn_cityinfo->boundingBox().size.width / 3 + 20, 0)), nullptr));
	btn_war->runAction(Sequence::create(DelayTime::create(0.2), MoveBy::create(0.2, Vec2(btn_war->boundingBox().size.width / 3 + 20, 0)), nullptr));
	m_btnMoved = true;
	//不可以发动国战的 -- 隐藏国战按钮
	if (SINGLE_HERO->m_iCurCityNation > 5)
	{
		btn_war->setVisible(false);
	}
}

void UIPalace::viewAction(Node * pSender,std::string viewPath)
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

void UIPalace::viewCloseAction()
{
	if (m_curView)
	{	
		std::string str = m_viewPath.substr(0, std::string::npos);
		m_curView->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4, ENDPOS3), CallFunc::create([=]{closeView(str); })));
	}
}

void UIPalace::showWar()
{
	bool is_action = true;
	if (getViewRoot(PALACE_COCOS_RES[VIEW_WAR_CSB]))
	{
		closeView(PALACE_COCOS_RES[VIEW_WAR_CSB]);
		is_action = false;
	}
	
	openView(PALACE_COCOS_RES[VIEW_WAR_CSB]);
	if (is_action)
	{
		viewAction(getViewRoot(PALACE_COCOS_RES[VIEW_WAR_CSB]), PALACE_COCOS_RES[VIEW_WAR_CSB]);
	}
	else
	{
		m_curView = getViewRoot(PALACE_COCOS_RES[VIEW_WAR_CSB]);
	}
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_WAR_CSB]);
	auto l_warContent = view->getChildByName<ListView*>("listview_content");
	l_warContent->setClippingEnabled(true);
	auto item = l_warContent->getItem(0);
	for (size_t i = 0; i < m_pWarPrepareResult->n_fight_nations; i++)
	{
		auto pFightNationInfo = m_pWarPrepareResult->fight_nations[i];
		Widget *p_war;
		if (i == 0)
		{
			p_war = item;
		}
		else
		{
			p_war = item->clone();
			l_warContent->insertCustomItem(p_war, i);
		}

		auto b_war_state = p_war->getChildByName<Button*>("button_horn");
		b_war_state->setTouchEnabled(false);
		auto t_war1 = p_war->getChildByName<Text*>("labe_1");
		auto t_war2 = p_war->getChildByName<Text*>("labe_1_0");
		auto t_war_state = p_war->getChildByName<Text*>("labe_war_start");
		auto t_war_time = p_war->getChildByName<Text*>("label_time");
		int h = m_pWarPrepareResult->fight_nations[i]->dis_to_end / 3600;
		int m = m_pWarPrepareResult->fight_nations[i]->dis_to_end % 3600 / 60;
		int s = m_pWarPrepareResult->fight_nations[i]->dis_to_end % 3600 % 60;
		//准备
		if (pFightNationInfo->time_sign == 0)
		{
			t_war1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_PREPARE_CONTENT1"]);
			std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_PREPARE_CONTENT2"];
			std::string new_vaule = SINGLE_SHOP->getNationInfo().find(pFightNationInfo->nation1)->second;
			std::string old_vaule = "[nation_name1]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = SINGLE_SHOP->getNationInfo().find(pFightNationInfo->nation2)->second;
			old_vaule = "[nation_name2]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			t_war2->setString(content);
			t_war_state->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_PREPARE_TIME"]);
			t_war_time->setString(StringUtils::format("%02d:%02d:%02d", h, m, s));
		}
		else
		{
			t_war1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_CONTENT1"]);
			std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_CONTENT2"];
			std::string new_vaule = SINGLE_SHOP->getNationInfo().find(pFightNationInfo->nation1)->second;
			std::string old_vaule = "[nation_name1]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			new_vaule = SINGLE_SHOP->getNationInfo().find(pFightNationInfo->nation2)->second;
			old_vaule = "[nation_name2]";
			repalce_all_ditinct(content, old_vaule, new_vaule);
			t_war2->setString(content);
			t_war_state->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_TIME"]);
			t_war_time->setString(StringUtils::format("%02d:%02d:%02d", h, m, s));
		}
		auto i_nation1 = p_war->getChildByName<ImageView*>("image_flag_city_1");
		auto i_nation2 = p_war->getChildByName<ImageView*>("image_flag_city_2");
		i_nation1->loadTexture(getCountryIconPath(pFightNationInfo->nation1));
		i_nation2->loadTexture(getCountryIconPath(pFightNationInfo->nation2));
	}

	if (m_pWarPrepareResult->n_fight_nations < 1)
	{
		l_warContent->removeItem(0);
	}

	//城市的攻击和血量
	auto i_city_property = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_cityfort"));
	auto t_city_att = dynamic_cast<Text*>(Helper::seekWidgetByName(i_city_property, "label_gun_number"));
	auto t_city_derable = dynamic_cast<Text*>(Helper::seekWidgetByName(i_city_property, "label_derable_num"));
	auto l_city_derable = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(i_city_property, "progressbar_durable"));
	t_city_att->setString(StringUtils::format("%lld", m_pWarPrepareResult->attack));
	//非战斗状态用当前血量
	if (m_pWarPrepareResult->is_in_war == 0)
	{
		m_pWarPrepareResult->final_max_hp = m_pWarPrepareResult->current_hp;
	}
	else if(m_pWarPrepareResult->is_in_war == 2)
	{
		m_pWarPrepareResult->final_max_hp = m_pWarPrepareResult->max_hp;
	}
	t_city_derable->setString(StringUtils::format("%lld/%lld", m_pWarPrepareResult->current_hp, m_pWarPrepareResult->final_max_hp));
	l_city_derable->setPercent(100.0 * m_pWarPrepareResult->current_hp / m_pWarPrepareResult->final_max_hp);
	setTextFontSize(t_city_att);
	setTextFontSize(t_city_derable);

	//暂时处理（上一次国战详情）
	auto p_last = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_last_result"));
	auto i_div_2 = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "image_div_2"));
	p_last->removeFromParentAndCleanup(true);
	i_div_2->removeFromParentAndCleanup(true);

	auto t_times = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_chance_0"));
	std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_WAR_COUNTRY_TIMES"];
	t_times->setString(content);
	auto b_start = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_start"));
	b_start->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));

	//非首都的市政厅
	if (SINGLE_SHOP->getCitiesInfo().find(SINGLE_HERO->m_iCityID)->second.palace_type == 1)
	{
		//玩家本身国家 与该城市所在国家 一致
		if (SINGLE_HERO->m_iCurCityNation == SINGLE_HERO->m_iNation)
		{
			//玩家本身国家 与该城市所在国家一致  是不是市长
			if (m_pWarPrepareResult->is_city_mayor == 1)
			{
				//该城市所在国家已经处于战争准备状态或者战争进行中状态
				if (m_pWarPrepareResult->is_in_war == 0)
				{
					//是否是休战日
					if (m_pWarPrepareResult->dis_to_apply_war > 0)
					{
						b_start->setBright(false);
					}
					else
					{
						b_start->setBright(true);
					}
				}
				else
				{
					b_start->setBright(false);
				}
			}
			else
			{
				b_start->setBright(false);
			}
		}
		else
		{
			b_start->setBright(false);
		}
	}
	else
	{
		b_start->setBright(false);
	}

	auto b_prepare = view->getChildByName<Button*>("button_enter");
	//不是本国
	if (SINGLE_HERO->m_iCurCityNation != SINGLE_HERO->m_iNation)
	{
		b_prepare->setBright(false);
	}
	else
	{
		//没有战争
		if (m_pWarPrepareResult->is_in_war == 0)
		{
			b_prepare->setBright(false);
		}
		else
		{
			b_prepare->setBright(true);
		}
	}
	auto b_rules = view->getChildByName<Button*>("button_enter_0");
	b_prepare->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));
	b_rules->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_warContent, image_pulldown);
}

void UIPalace::warCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto target = dynamic_cast<Widget*>(pSender);
	auto name = target->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//准备国战或者开始国战
	if (isButton(button_prepare))
	{
		UINationWarEntrance*pirateLayer = UINationWarEntrance::createCountryWarLayer();
		Scene* scene = Scene::createWithPhysics();
		scene->addChild(pirateLayer);
		Director::getInstance()->replaceScene(scene);
		return;
	}
	//国战排行
	if (isButton(button_ranking))
	{
		//showLastWarRanking();
		auto layer = UINationWarRanking::createCountryWarRanking();
		this->addChild(layer,3);
		return;
	}
	//规则和奖励
	if (isButton(button_enter_0))
	{
		showWarRuses();
		return;
	}

	//进入战斗准备
	if (isButton(button_enter))
	{
		//不是本国
		if (SINGLE_HERO->m_iCurCityNation != SINGLE_HERO->m_iNation)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_PALACE_COUNTRY_DEFENSE_NOT_ENTER1");
		}
		else
		{
			//没有战争
			if (m_pWarPrepareResult->is_in_war == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PALACE_COUNTRY_DEFENSE_NOT_ENTER2");
			}
			else
			{
				UINationWarEntrance*pirateLayer = UINationWarEntrance::createCountryWarLayer();
				Scene* scene = Scene::createWithPhysics();
				scene->addChild(pirateLayer);
				Director::getInstance()->replaceScene(scene);
			}
		}
		return;
	}

	//发起国战
	if (isButton(button_start))
	{
		//首都
		if (SINGLE_SHOP->getCitiesInfo().find(SINGLE_HERO->m_iCityID)->second.palace_type == 1)
		{
			//玩家本身国家 与该城市所在国家 一致
			if (SINGLE_HERO->m_iCurCityNation == SINGLE_HERO->m_iNation)
			{
				//玩家本身国家 与该城市所在国家一致  是不是市长
				if (m_pWarPrepareResult->is_city_mayor == 1)
				{
					//该城市所在国家已经处于战争准备状态或者战争进行中状态
					if (m_pWarPrepareResult->is_in_war == 0)
					{
						if (m_pWarPrepareResult->dis_to_apply_war > 0)
						{

							int h = m_pWarPrepareResult->dis_to_apply_war / 3600;
							int m = m_pWarPrepareResult->dis_to_apply_war % 3600 / 60;
							int s = m_pWarPrepareResult->dis_to_apply_war % 3600 % 60;
					
							std::string content = SINGLE_SHOP->getTipsInfo()["TIP_PALACE_COUNTRY_ARMISTICE_DAY"];
							std::string new_vaule = StringUtils::format("%02d:%02d:%02d", h, m, s);
							std::string old_vaule = "[time]";
							repalce_all_ditinct(content, old_vaule, new_vaule);

							openView(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
							auto view = getViewRoot(INFORM_COCOS_RES[C_VIEW_ERROR_CONFIRM_CSB]);
							auto t_content = dynamic_cast<Text*>(view->getChildByName("label_dropitem_tiptext"));
							t_content->setString(content);
						}
						else
						{
							ProtocolThread::GetInstance()->getEnemyNation(UILoadingIndicator::create(this));
						}
					}
					else
					{
						UIInform::getInstance()->openInformView(this);
						UIInform::getInstance()->openConfirmYes("TIP_PALACE_COUNTRY_YOU_IN_WAR");
					}
				}
				else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_COUNTRY_WAR_NOT_MAYOR");
				}
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PALACE_COUNTRY_NOT_YOU_NATION");
			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COUNTRY_WAR_NOT_CAPITAL");
		}
		return;
	}
	//确认发起国战
	if (isButton(button_yes))
	{
		log("nationId = %d", m_pChooseNation->getTag());
		ProtocolThread::GetInstance()->applyStateWar(m_pChooseNation->getTag(), UILoadingIndicator::create(this));
		return;
	}
	//取消国战
	if (isButton(button_no))
	{
		closeView(PALACE_COCOS_RES[VIEW_LAUNCHWAR_CSB]);
		m_pChooseNation = nullptr;
		return;
	}

	//确认发起国战
	if (isButton(button_result_yes))
	{
		closeView(PALACE_COCOS_RES[VIEW_LAUNCHWAR_RESULT_CSB]);
		return;
	}
	//关闭国战排行或者奖励规则
	if (isButton(button_close))
	{
		closeView(PALACE_COCOS_RES[VIEW_WAR_RANKING_CSB]);
		closeView(PALACE_COCOS_RES[VIEW_WAR_RULES]);
		return;
	}
	//解释说明-国战排行
	if (isButton(button_rank_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_WAR_RANKING_TITLE", "TIP_PALACE_WAR_RANKING_CONTENET");
		return;
	}
	//解释说明-发送战书
	if (isButton(button_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_WAR_LAUNCHWAR_TITLE", "TIP_PALACE_WAR_LAUNCHWAR_CONTENT");
		return;
	}
	//选择国家
	if (isButton(panel_content))
	{
		if (m_pChooseNation)
		{
			m_pChooseNation->getChildByName<ImageView*>("image_press")->setVisible(false);
		}
		m_pChooseNation = target;
		m_pChooseNation->getChildByName<ImageView*>("image_press")->setVisible(true);
		auto view = getViewRoot(PALACE_COCOS_RES[VIEW_LAUNCHWAR_CSB]);
		auto b_yes = view->getChildByName<Button*>("button_yes");
		b_yes->setBright(true);
		b_yes->setTouchEnabled(true);
		return;
	}
}

void UIPalace::showLastWarRanking()
{
	openView(PALACE_COCOS_RES[VIEW_WAR_RANKING_CSB], 11);
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_WAR_RANKING_CSB]);
	auto b_close = view->getChildByName<Button*>("button_close");
	b_close->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));

	auto panel_title = view->getChildByName<Widget*>("panel_title");
	auto t_ranking = panel_title->getChildByName<Text*>("label_last_war");
	auto b_info = view->getChildByName<Button*>("button_rank_info");
	b_info->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));
	t_ranking->setPositionX((panel_title->getBoundingBox().size.width - t_ranking->getBoundingBox().size.width) / 2 + t_ranking->getBoundingBox().size.width / 2);
	b_info->setPositionX(t_ranking->getPositionX() + t_ranking->getBoundingBox().size.width / 2 + b_info->getBoundingBox().size.width);

	auto panel_list = view->getChildByName<Widget*>("panel_list");
	auto l_ranking = view->getChildByName<ListView*>("listview_ranking");
	l_ranking->removeAllChildrenWithCleanup(true);

	
	for (size_t i = 0; i < 10; i++)
	{
		auto item = panel_list->clone();
		auto t_rank = item->getChildByName<Text*>("label_rank");
		auto i_trophy = item->getChildByName<ImageView*>("image_trophy");
		auto i_nation = item->getChildByName<ImageView*>("image_country");
		auto t_name = item->getChildByName<Text*>("label_name");
		auto t_lv = item->getChildByName<Text*>("label_lv");
		auto t_admage = item->getChildByName<Text*>("label_silver_num");
		t_rank->setString(StringUtils::format("%d", i + 1));
		if (i < 3)
		{
			if (i + 1 == 1)
			{
				i_trophy->loadTexture(RANK_GOLD);
			}
			else if (i + 1 == 2)
			{
				i_trophy->loadTexture(RANK_SILVER);
			}
			else
			{
				i_trophy->loadTexture(RANK_COPPER);
			}
		}
		else
		{
			i_trophy->setVisible(false);
			t_rank->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		}
		i_nation->loadTexture(getCountryIconPath(1));
		t_name->setString("Name");
		t_lv->setString(StringUtils::format("Lv. %d", 1));
		t_admage->setString(StringUtils::format("%d", 100));
		l_ranking->pushBackCustomItem(item);
	}

	auto image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_head_16"));
	auto t_rank_title = view->getChildByName<Text*>("label_you_rank");
	auto t_rank_num = view->getChildByName<Text*>("label_you_rank_num");
	auto t_damage_title = view->getChildByName<Text*>("label_you_score");
	auto t_damage_num = view->getChildByName<Text*>("label_you_score_num");
	if (true)
	{
		t_rank_num->setString(StringUtils::format("%d", 1));
	}
	else
	{
		t_rank_num->setString("N/A");
	}

	t_rank_num->setPositionX(t_rank_title->getBoundingBox().size.width + t_rank_title->getPositionX() + 10);
	t_damage_num->setString(StringUtils::format("%d", 1));
	t_damage_num->setPositionX(t_damage_title->getBoundingBox().size.width + t_damage_title->getPositionX() + 10);
	image_head->loadTexture(getPlayerIconPath(1));
	auto  i_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto b_pulldown = i_pulldown->getChildByName<Button*>("button_pulldown");
	b_pulldown->setPositionX(i_pulldown->getBoundingBox().size.width / 2 - b_pulldown->getBoundingBox().size.width / 2 + 3);
	addListViewBar(l_ranking, i_pulldown);
	
	auto p_no = view->getChildByName<Widget*>("panel_centent_no");
	if (true)
	{
		p_no->setVisible(false);
	}
	else
	{
		p_no->setVisible(true);
	}
}

void UIPalace::showStartWar(const GetEnemyNationResult *pResult)
{
	openView(PALACE_COCOS_RES[VIEW_LAUNCHWAR_CSB], 11);
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_LAUNCHWAR_CSB]);
	auto b_no = view->getChildByName<Button*>("button_no");
	auto b_yes = view->getChildByName<Button*>("button_yes");
	auto b_info = view->getChildByName<Button*>("button_info");
	b_no->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));
	b_yes->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));
	b_info->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));
	b_yes->setBright(false);
	b_yes->setTouchEnabled(false);

	auto l_content = view->getChildByName<ListView*>("listview_content");
	auto p_nation = view->getChildByName<Widget*>("panel_content");
	p_nation->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));
	for (size_t i = 0; i < pResult->n_nations; i++)
	{
		auto item = p_nation->clone();
		auto i_nation = item->getChildByName<ImageView*>("image_country");
		auto t_nation = item->getChildByName<Text*>("label_trade");
		auto i_silver = item->getChildByName<ImageView*>("image_silver");
		auto t_silver = item->getChildByName<Text*>("label_silver_num");
		item->setTag(pResult->nations[i]->nations_id);
		i_nation->loadTexture(getCountryIconPath(pResult->nations[i]->nations_id));
		t_nation->setString(SINGLE_SHOP->getNationInfo().at(pResult->nations[i]->nations_id));
		t_silver->setString(StringUtils::format("%lld", pResult->nations[i]->can_robbed_coins));
		i_silver->setPositionX(t_silver->getPositionX() - t_silver->getBoundingBox().size.width - i_silver->getBoundingBox().size.width);
		l_content->pushBackCustomItem(item);
	}

	auto  i_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto b_pulldown = i_pulldown->getChildByName<Button*>("button_pulldown");
	b_pulldown->setPositionX(i_pulldown->getBoundingBox().size.width / 2 - b_pulldown->getBoundingBox().size.width / 2 + 3);
	addListViewBar(l_content, i_pulldown);
	m_pChooseNation = nullptr;
}

void UIPalace::showWarRuses()
{
	openView(PALACE_COCOS_RES[VIEW_WAR_RULES], 11);
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_WAR_RULES]);

	auto l_rules = view->getChildByName<ListView*>("listview_rules");

	auto t_reward = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "text_rewards_0"));
	auto str_reward = SINGLE_SHOP->getTipsInfo()["TIP_COUNTRY_WAR_REWARD"];
	auto size = Size(t_reward->getContentSize().width, getLabelHight(str_reward, t_reward->getContentSize().width, t_reward->getFontName(), t_reward->getFontSize()) + 10);
	t_reward->setString(str_reward);
	t_reward->setContentSize(size);
	t_reward->setPositionY(0);
	
	auto t_rule = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "text_rules_war"));
	for (size_t i = 0; i < 3; i++)
	{
		Text *t_rule_clone;
		if (i == 0)
		{
			t_rule_clone = t_rule;
		}
		else
		{
			t_rule_clone = dynamic_cast<Text*>(t_rule->clone());
			l_rules->pushBackCustomItem(t_rule_clone);
		}
		auto str_content = SINGLE_SHOP->getTipsInfo()[StringUtils::format("TIP_PALACE_WAR_RULE_%d", i + 1)];
		auto size1 = Size(t_rule->getContentSize().width, getLabelHight(str_content, t_rule->getContentSize().width, t_rule->getFontName(), t_rule->getFontSize()) + 10);
		t_rule_clone->setString(str_content);
		t_rule_clone->setContentSize(size1);
		t_rule->setPositionY(0);
		
		auto b_close = view->getChildByName<Button*>("button_close");
		b_close->addTouchEventListener(CC_CALLBACK_2(UIPalace::warCall_func, this));
	}

	auto  i_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto b_pulldown = i_pulldown->getChildByName<Button*>("button_pulldown");
	b_pulldown->setPositionX(i_pulldown->getBoundingBox().size.width / 2 - b_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(l_rules, i_pulldown);
}

void UIPalace::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
{
	auto pos = view->getContentOffset();
	auto viewSize = view->getViewSize();
	auto contentSize = view->getContainer()->getContentSize();

	float bottom = viewSize.height - contentSize.height;
	float top = 0;
	float totalLen = top - bottom;
	float currentLen = top - pos.y;

	Button*button_pulldown = dynamic_cast<Button*>(view->getUserObject());
	if (!button_pulldown)
		return;
	float allowHeight = 16;
	float len = dynamic_cast<Widget*>(button_pulldown->getParent())->getSize().height - button_pulldown->getSize().height - allowHeight * 2;
	if (contentSize.height <= viewSize.height)
	{
		button_pulldown->setVisible(false);
		button_pulldown->getParent()->setVisible(false);
	}
	else
	{
		button_pulldown->setVisible(true);
		button_pulldown->getParent()->setVisible(true);
		auto factor = (currentLen / totalLen);
		if (factor > 1.0) factor = 1.0;
		if (factor < 0) factor = 0;
		button_pulldown->setPositionY(len * factor + allowHeight * 2 + allowHeight);
	}
}
void UIPalace::tableCellTouched(TableView* table, TableViewCell* cell)
{
	int rankTag = cell->getChildren().at(0)->getTag() - 1;
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	m_operateWidgetTag = m_pRankResult->rankinfo[rankTag]->characterid;
	ProtocolThread::GetInstance()->getUserInfoById(m_operateWidgetTag, UILoadingIndicator::create(this));
}
Size UIPalace::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_HALL_OF_HONOR_CSB]);
	auto listRanking = view->getChildByName<ListView*>("listview_ranking");
	auto w_item = listRanking->getItem(0);
	if (w_item)
	{
		return w_item->getContentSize();
	}
	return Size(0, 0);
}
TableViewCell * UIPalace::tableCellAtIndex(TableView *table, ssize_t idx)
{
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_HALL_OF_HONOR_CSB]);
	auto listRanking = view->getChildByName<ListView*>("listview_ranking");
	auto w_item = listRanking->getItem(0);


	TableViewCell *cell = table->dequeueCell();
	Widget* cell_item;
	if (!cell)
	{
		cell = new TableViewCell();
		cell->autorelease();
		cell_item = w_item->clone();


		cell->addChild(cell_item, 1);
		cell_item->setPosition(Vec2(0, 0));
		cell_item->setCameraMask(_cameraMask);
	}
	else
	{
		cell_item = dynamic_cast<Widget*>(cell->getChildren().at(0));
	}

	cell->setTag(idx);
	cell->setVisible(true);
	cell_item->setSwallowTouches(false);

	cell_item->setTouchEnabled(true);
	cell_item->setVisible(true);
	auto rankItem = m_pRankResult->rankinfo[idx];
	auto rank_Image = cell_item->getChildByName<ImageView*>("image_trophy");//奖杯图片
	rank_Image->setVisible(false);
	rank_Image->ignoreContentAdaptWithSize(false);

	auto label_rank = cell_item->getChildByName<Text*>("label_rank");

	auto imagCounty = cell_item->getChildByName<ImageView*>("image_country");
	Widget* listview_name_lv = cell_item->getChildByName<Widget*>("listview_name_lv");
	auto label_name = listview_name_lv->getChildByName<Text*>("label_name");
	int rankNum = 0;
	if (idx == 0)
	{
		label_rank->setString(StringUtils::format("%d", idx + 1));
		rankNum = idx + 1;
	}
	else if (idx > 0)
	{
		for (int j = 0; j < idx; j++)
		{
			if (m_pRankResult->rankinfo[j]->investment == m_pRankResult->rankinfo[idx]->investment)
			{
				rankNum = j + 1;
				break;
			}
			else
			{
				rankNum = j + 2;
			}
		}

		label_rank->setString(StringUtils::format("%d", rankNum));
	}
	if (rankNum == 1)
	{
		rank_Image->setVisible(true);
		rank_Image->loadTexture(RANK_GOLD);
		label_rank->setColor(Color3B::WHITE);
	}
	else if (rankNum == 2)
	{
		rank_Image->setVisible(true);
		rank_Image->loadTexture(RANK_SILVER);
		label_rank->setColor(Color3B::WHITE);
	}
	else if (rankNum == 3)
	{
		rank_Image->setVisible(true);
		rank_Image->loadTexture(RANK_COPPER);
		label_rank->setColor(Color3B::WHITE);
	}
	else
	{
		label_rank->setColor(Color3B(46, 29, 14));
	}

	//cell_item->addTouchEventListener(CC_CALLBACK_2(PalaceLayer::ranklistInfo, this));
	cell_item->setTag(idx + 1);
	auto label_lv = listview_name_lv->getChildByName<Text*>("label_lv");
	auto labelSilver_num = cell_item->getChildByName<Text*>("label_silver_num");


	if (SINGLE_HERO->m_iID == rankItem->characterid)
	{
		rankItem->level = SINGLE_HERO->m_iLevel;
	}
	imagCounty->loadTexture(getCountryIconPath(rankItem->country));
	label_name->setString(StringUtils::format("%s", rankItem->name));

	label_lv->setString(StringUtils::format("Lv. %d", rankItem->level));
	labelSilver_num->setString(numSegment(StringUtils::format("%lld", rankItem->investment)));
	auto i_silver = cell_item->getChildByName("image_silver");
	auto t_silver_num = cell_item->getChildByName<Text*>("label_silver_num");
	t_silver_num->ignoreContentAdaptWithSize(true);
	i_silver->setPositionX(t_silver_num->getPositionX() - t_silver_num->getContentSize().width - i_silver->getContentSize().width / 2 - 6);

	return cell;
}
ssize_t UIPalace::numberOfCellsInTableView(TableView *table)
{
	if (m_pRankResult)
	{
		return m_pRankResult->n_rankinfo;
	}
	return 0;
}

std::string UIPalace::getPortTypeOrInfo(int port)
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
void UIPalace::textEvent(Ref* target, Widget::TouchEventType type)
{
	auto text = dynamic_cast<Text*>(target);
	if (Widget::TouchEventType::BEGAN == type)
	{
		text ->setOpacity(120);
		m_operateTag = text->getTag();
	}
	if (Widget::TouchEventType::ENDED == type)
	{
		text->setOpacity(255);
		ProtocolThread::GetInstance()->getForceCity(SINGLE_HERO->m_iCurCityNation);
	}
}

