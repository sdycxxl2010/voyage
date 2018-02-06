#include "UIBank.h"
#include "UIStore.h"
#include "UICommon.h"
#include "UIInform.h"
#include "Utils.h"

#include "UIVoyageSlider.h"
#include "UIChatHint.h"
#include "UIBankSafeBox.h"
#include "MainTaskManager.h"
#include "TimeUtil.h"

UIBank::UIBank():
	m_bIsMyBank(false),
	m_bIsChangeBank(true),
	m_bIsDeposit(false)
{
	m_bIsBankItem = false;
	m_bIsBankLog = false;
	m_pBankResult = nullptr;
	m_nMoneyCount = 0;
	m_pItemCenter = nullptr;
	m_pSlider = nullptr;
	m_pBankItem = new BankItem;
	m_pBankItem->type = 0;
	m_pBankItem->cid = 0;
	m_pBankItem->iid = 0;
	m_pBankItem->num = 0;
	m_pBankItem->attack = 0;
	m_pBankItem->defense = 0;
	m_pBankItem->durability = 0;
	m_pBankItem->m_speed = 0;
	m_pBankItem->max_durability = 0;
	m_pBankItem->steer_speed = 0;
	m_pBankItem->optional_item = 0;
	m_pBankItem->optional_num = 0;
	m_pBankItem->optional_value = nullptr;
	m_vBankItems.clear();
	m_eUIType = UI_BANK;
	m_bNumpadFlag = true;
	m_ratio_times.clear();
	m_insurance_item.clear();
	n_chooseRatiotime = 0;
	n_chooseRatio = 0;
	n_protectDelegate = 0;
	n_insurance_coinCost = 0;
	n_insurance_VticketCost = 0;
	m_textField_insure = nullptr;
	m_myInsurance = new InsuranceInfor;
	m_myInsurance->leftamount = 0;
	m_myInsurance->lefttimeinseconds = 0;
	m_myInsurance->percent = 0;
	m_myInsurance->totalamount = 0;
	m_myInsurance->protectdelegate = 1;
	m_bChangeInsurePlan = false;
	m_buyInsurenceResult = nullptr;
	m_pEquipDetail = nullptr;
	m_bLeftButtonTag = BUTTON_TAG_NONE;
};

UIBank::~UIBank()
{
	if (m_pBankResult)
	{
		get_bank_info_result__free_unpacked(m_pBankResult,0);
		m_pBankResult=nullptr;
	}
	if (m_pSlider)
	{
		m_pSlider->release();
	}
 	
	if (m_pItemCenter)
	{
		m_pItemCenter=nullptr;
	}
	delete m_pBankItem;
	delete m_myInsurance;
	m_vBankItems.clear();
	m_ratio_times.clear();
	m_insurance_item.clear();
	delete m_pEquipDetail;
 	NotificationCenter::getInstance()->removeAllObservers(this);
}

UIBank* UIBank::createBank()
{
	auto bk = new UIBank;
	if (bk && bk->init())
	{
		bk->autorelease();
		return bk;
	}
	CC_SAFE_DELETE(bk);
	return nullptr;
}

bool UIBank::init()
{
	bool pRet = false;
	do 
	{
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIBank::onServerEvent,this),this);
		//加主界面
		initStatic(0);
		pRet = true;
	} while (0);
	//接受通知
	NotificationCenter::getInstance()->addObserver(this,callfuncO_selector(UIBank::SliderEvent),"button_Slider_yes",NULL);
	NotificationCenter::getInstance()->addObserver(this,callfuncO_selector(UIBank::button_safebackEvent),"button_backSafe_notify",NULL);
	NotificationCenter::getInstance()->addObserver(this,callfuncO_selector(UIBank::bankSliderClose),"close_palace_Slider",NULL);
	//Began is personal bank
	m_bIsMyBank=true;
	ProtocolThread::GetInstance()->getBankInfo(m_bIsMyBank,UILoadingIndicator::create(this,m_eUIType));
	return pRet;
}

void UIBank::initStatic(float f)
{

	openView(BANK_COCOS_RES[VIEW_BANK_MAIN_CSB]);
	auto bankViewMain = getViewRoot(BANK_COCOS_RES[VIEW_BANK_MAIN_CSB]);

	auto view_Butter = bankViewMain->getChildByName("image_butter_bg");
	auto ListView_butter = view_Butter->getChildByName("listview_butter");
	auto button_Personal = ListView_butter->getChildByName<Button*>("button_personal");
	button_Personal->setBright(false);
	
	auto viewCharaButter = bankViewMain->getChildByName("panel_charactor_two_butter");
	auto listview_two_butter = dynamic_cast<ListView*>(bankViewMain->getChildByName("listview_two_butter"));
	auto image_div_butter_personal = dynamic_cast<ImageView*>(Helper::seekWidgetByName(bankViewMain, "image_div_butter_personal"));
	auto button_coins_personal = dynamic_cast<Button*>(Helper::seekWidgetByName(bankViewMain, "button_coins_personal"));

	auto image_div_butter_personal_clone = image_div_butter_personal->clone();
	image_div_butter_personal_clone->setTag(1024);

	auto button_coins_personal_clone = button_coins_personal->clone();
	button_coins_personal_clone->setTag(1025);
	auto label_personal_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(button_coins_personal_clone, "label_personal_coin"));
	button_coins_personal_clone->setBright(false);
	label_personal_coin->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_LEFT_LISTVIEW_BUTTON_CURRENCY"]);
	label_personal_coin->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
	listview_two_butter->insertCustomItem(button_coins_personal_clone, 0);
	listview_two_butter->insertCustomItem(image_div_butter_personal_clone, 0);
	listview_two_butter->setDirection(cocos2d::ui::ScrollView::Direction::NONE);
	listview_two_butter->refreshView();
	m_bLeftButtonTag = BUTTON_TAG_PERSONAL_COIN;

	//保险周期1，3，7
	if (m_ratio_times.size()<=0)
	{
		m_ratio_times.push_back(FIRST_PERIOD);
		m_ratio_times.push_back(SECOND_PERIOD);
		m_ratio_times.push_back(THIRD_PERIOD);
	}
	//聊天
	auto ch = UIChatHint::createHint();
	this->addChild(ch, 10);
}
void UIBank::onEnter()
{	
	UIBasicLayer::onEnter();
}
void UIBank::onExit()
{
	//stopAudio();
	UIBasicLayer::onExit();
}
void UIBank::flushMyCoinGold()
{
	openView(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
	auto currency = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);

	auto panel_content = dynamic_cast<Widget*>(Helper::seekWidgetByName(currency,"panel_content"));
	panel_content->setVisible(true);

	auto label_input_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_content, "label_input_1"));
	auto label_input_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_content, "label_input_0"));
	label_input_0->setContentSize(Size(230, 40));
	if (m_bIsMyBank)
	{
		label_input_1->setString("5%");
	}
	else
	{
		label_input_1->setString("20%");
	}
	std::string s_coin = numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iCoin));
	std::string s_gold = numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iGold));
	auto currencyOwn=currency->getChildByName("panel_currency_own");
	auto buttonSilver=currencyOwn->getChildByName("button_silver");
	auto sliverNum=buttonSilver->getChildByName<Text*>("label_silver_num");
	sliverNum->setString(s_coin);
	auto buttonGold=currencyOwn->getChildByName("button_gold");
	auto goldNum=buttonGold->getChildByName<Text*>("label_gold_num");
	goldNum->setString(s_gold);
	setTextFontSize(sliverNum);
	setTextFontSize(goldNum);
	//输入框触摸事件
	auto panelCobtent =currency->getChildByName("panel_content");
	auto imageInPut=panelCobtent->getChildByName("image_input_bg");
	auto tf_num = imageInPut->getChildByName<Text*>("label_currency_num");
	tf_num->addTouchEventListener(CC_CALLBACK_2(UIBank::menuCall_func,this));
	tf_num->setString("0");
	//数字键盘银币
	auto p_mask_numpad = currency->getChildByName<Widget*>("panel_mask_numpad");
	auto t_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_mask_numpad, "label_silver_num"));
	auto t_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_mask_numpad, "label_gold_num"));
	t_silver_num->setString(StringUtils::format("%lld", SINGLE_HERO->m_iCoin));
	t_gold_num->setString(StringUtils::format("%lld", SINGLE_HERO->m_iGold));
	this->scheduleUpdate();
}

void UIBank::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		auto target = dynamic_cast<Widget*>(pSender);
		buttonEventByName(target,target->getName());
	}
}

void UIBank::buttonEventByName(Widget* target,std::string name)
{
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_BANK_MAIN_CSB])->getChildByName("image_butter_bg");
	auto ListView_butter=view->getChildByName("listview_butter");
	auto button_Personal=ListView_butter->getChildByName<Button*>("button_personal");
	auto button_Guild=ListView_butter->getChildByName<Button*>("button_guild");
	auto button_GuildDark=ListView_butter->getChildByName<Button*>("button_guild_no");

	auto subButton =  getViewRoot(BANK_COCOS_RES[VIEW_BANK_MAIN_CSB])->getChildByName("panel_charactor_two_butter");
	auto listview_two_butter=dynamic_cast<ListView*>(subButton->getChildByName("listview_two_butter"));
	
	
	auto  button_Safe =   dynamic_cast<Button*>(Helper::seekWidgetByName(listview_two_butter, "button_safe"));
	auto label_safe=button_Safe->getChildByName<Text*>("label_safe");

	auto button_Sccess_log = dynamic_cast<Button*>(Helper::seekWidgetByName(listview_two_butter, "button_sccess_log"));
	auto label_sccess_log=button_Sccess_log->getChildByName<Text*>("label_sccess_log");

	//保险
	auto  button_insurance = dynamic_cast<Button*>(Helper::seekWidgetByName(listview_two_butter, "button_insurance"));
	auto label_insurance = button_insurance->getChildByName<Text*>("label_insurance");
	//个人银币
	auto image_div_butter_personal_clone = dynamic_cast<ImageView*>(listview_two_butter->getChildByTag(1024));
	auto button_coins_personal_clone = dynamic_cast<Button*>(listview_two_butter->getChildByTag(1025));

	if (isButton(button_silver))//银币
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType,1);
		return;
	}
	if (isButton(button_gold))//V票
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType,0);
		return;
	}
	//公会银行标题问号解释
	if (isButton(button_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_BANK_GUILD_TITME_INFO_TITLE", "TIP_BANK_GUILD_TITME_INFO_CONTENT");
		return;
	}

	if (isButton(button_bank_info))//解释银行信息的问号按钮
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		if (m_bIsMyBank)
		{
			UICommon::getInstance()->flushInfoBtnView("TIP_BANK_MONENY_INFO_TITLE", "TIP_BANK_MONENY_INFO_CONTENT");
		}
		else
		{
			UICommon::getInstance()->flushInfoBtnView("TIP_BANK_MONENY_INFO_TITLE", "TIP_BANK_SAVE_COIN_INFO");
		}

		return;
	}
	if (isButton(button_safe_info))//保险箱信息解释
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		if (m_bIsMyBank)
		{
			UICommon::getInstance()->flushInfoBtnView("TIP_BANK_SAFEINFO_TITLE", "TIP_BANK_SAFEINFO_CONTENT");
		}
		else
		{
			UICommon::getInstance()->flushInfoBtnView("TIP_BANK_SAFEINFO_TITLE", "TIP_BANK_SAVE_GOOD_INFO");
		}

		return;
	}
	if (isButton(button_personal))//个人银行
	{
		
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto viewMain = getViewRoot(BANK_COCOS_RES[VIEW_BANK_MAIN_CSB]);
		auto viewButter = viewMain->getChildByName("panel_charactor_two_butter");
		viewButter->setVisible(true);

		auto image_bg_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewMain, "image_bg_1"));
		auto image_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewMain, "image_bg"));
		image_bg_1->setVisible(false);
		image_bg->setVisible(true);
	

		dynamic_cast<Text*>(button_Guild->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_NORMAL);
		dynamic_cast<Text*>(button_Personal->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_PASSED);
		m_bIsMyBank = true;
		m_bIsChangeBank = true;
		m_bLeftButtonTag = BUTTON_TAG_PERSONAL_COIN;
		button_Personal->setBright(false);
		button_Guild->setBright(true);
		//个人银币
		auto image_div_butter_personal = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewMain, "image_div_butter_personal"));
		auto button_coins_personal = dynamic_cast<Button*>(Helper::seekWidgetByName(viewMain, "button_coins_personal"));
		
		if (!button_coins_personal_clone)
		{
			button_coins_personal_clone = dynamic_cast<Button*>(button_coins_personal->clone());
			button_coins_personal_clone->setTag(1025);
			listview_two_butter->insertCustomItem(button_coins_personal_clone, 0);
		}
		if (!image_div_butter_personal_clone)
		{
			image_div_butter_personal_clone = dynamic_cast<ImageView*>(image_div_butter_personal->clone());
			image_div_butter_personal_clone->setTag(1024);

			listview_two_butter->insertCustomItem(image_div_butter_personal_clone, 0);

		}
		listview_two_butter->refreshView();
		auto label_personal_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(button_coins_personal_clone, "label_personal_coin"));
		button_coins_personal_clone->setBright(false);
		label_personal_coin->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_LEFT_LISTVIEW_BUTTON_CURRENCY"]);
		label_personal_coin->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);

		label_insurance->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_LEFT_LISTVIEW_BUTTON_INSURANCE"]);

		opposedCitySafeBtn();
		button_insurance->setBright(true);
		label_insurance->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		button_Sccess_log->setBright(true);
		label_sccess_log->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);

		m_nMoneyCount = 0;
		m_padString.clear();

	    //关闭其他的，打开自己的
		closeView(BANK_COCOS_RES[VIEW_LOG_GUILD_CSB]);
		closeView(BANK_COCOS_RES[VIEW_SAFE_CSB]);
		closeView(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
		closeView(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]);
		openView(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
		auto viewPer = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
		auto panel_centent_guild_no = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewPer,"panel_centent_guild_no"));
		panel_centent_guild_no->setVisible(false);
		ProtocolThread::GetInstance()->getBankInfo(m_bIsMyBank,UILoadingIndicator::create(this));

	}
	if (isButton(button_guild))//公会银行
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		button_Personal->setBright(true);
		button_Guild->setBright(false);


		m_bIsMyBank = false;
		m_bIsChangeBank = true;

		dynamic_cast<Text*>(button_Personal->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_NORMAL);
		dynamic_cast<Text*>(button_Guild->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_PASSED);
		if (image_div_butter_personal_clone)
		{
			image_div_butter_personal_clone->removeFromParentAndCleanup(true);
			image_div_butter_personal_clone = nullptr;
		}
		if (button_coins_personal_clone)
		{
			button_coins_personal_clone->removeFromParentAndCleanup(true);
			button_coins_personal_clone = nullptr;
		}
		listview_two_butter->refreshView();
		button_insurance->setBright(false);
		label_insurance->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_LEFT_LISTVIEW_BUTTON_CURRENCY"]);
		label_insurance->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
		m_bLeftButtonTag = BUTTON_TAG_GUILD_COIN;
		opposedCitySafeBtn();
		button_Sccess_log->setBright(true);
		label_sccess_log->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);

		m_nMoneyCount = 0;
		m_padString.clear();
	   //关闭其他的，打开自己的
		closeView(BANK_COCOS_RES[VIEW_LOG_GUILD_CSB]);
		closeView(BANK_COCOS_RES[VIEW_SAFE_CSB]);
		closeView(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]);
		closeView(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
		openView(BANK_COCOS_RES[VIEW_CURRENY_CSB]);

		ProtocolThread::GetInstance()->getBankInfo(m_bIsMyBank,UILoadingIndicator::create(this));
		
	}
	if (isButton(button_back))//返回主城
	{	
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		if (isScheduled(schedule_selector(UIBank::insuranceLeftTime)))
		{
			this->unschedule(schedule_selector(UIBank::insuranceLeftTime));
		}
		this->button_callBack();
		
	}
	
	if (isButton(button_safe))//保险箱
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bLeftButtonTag = BUTTON_TAG_GUILD_SAFE;
		if (m_bIsCanGotoSafe)
		{
			if (button_coins_personal_clone)
			{
				button_coins_personal_clone->setBright(true);
				auto label_personal_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(button_coins_personal_clone, "label_personal_coin"));
				label_personal_coin->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
			}
			button_Safe->setBright(false);
			label_safe->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
			button_Sccess_log->setBright(true);
			label_sccess_log->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
			button_insurance->setBright(true);
			label_insurance->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
			
			if (m_bIsBankItem)
			{
				//关闭其他的，打开自己的
				closeView(BANK_COCOS_RES[VIEW_LOG_GUILD_CSB]);
				closeView(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
				closeView(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]);
				openView(BANK_COCOS_RES[VIEW_SAFE_CSB]);
				updateItemView();
			}
			else
			{
				ProtocolThread::GetInstance()->getBankInfo(m_bIsMyBank, UILoadingIndicator::create(this));
			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_BANK_OPPOSEDCITY_CAN_NOT_ENTER_SAFE");
		}
		
	}
	if (isButton(button_coins_personal))//个人银币
	{

	   SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	   m_bLeftButtonTag = BUTTON_TAG_PERSONAL_COIN;
	
	   if (button_coins_personal_clone)
	   {
		   button_coins_personal_clone->setBright(false);
		   auto label_personal_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(button_coins_personal_clone, "label_personal_coin"));
		   label_personal_coin->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
	   }
		button_insurance->setBright(true);
		label_insurance->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		opposedCitySafeBtn();
		button_Sccess_log->setBright(true);
		label_sccess_log->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		m_nMoneyCount = 0;
		m_padString.clear();
		//关闭其他的，打开自己的
		closeView(BANK_COCOS_RES[VIEW_LOG_GUILD_CSB]);
		closeView(BANK_COCOS_RES[VIEW_SAFE_CSB]);
		closeView(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]);
		ProtocolThread::GetInstance()->getBankInfo(m_bIsMyBank,UILoadingIndicator::create(this));
	
	}
	if (isButton(button_sccess_log))//记录
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bLeftButtonTag = BUTTON_TAG_GUILD_LOG;
		if (button_coins_personal_clone)
		{
			button_coins_personal_clone->setBright(true);
			auto label_personal_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(button_coins_personal_clone, "label_personal_coin"));
			label_personal_coin->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		}
		button_insurance->setBright(true);
		label_insurance->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		opposedCitySafeBtn();
		button_Sccess_log->setBright(false);
		label_sccess_log->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
		closeView(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
		closeView(BANK_COCOS_RES[VIEW_SAFE_CSB]);
		closeView(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]);
		openView(BANK_COCOS_RES[VIEW_LOG_GUILD_CSB]);
		ProtocolThread::GetInstance()->getBankLog(m_bIsMyBank,UILoadingIndicator::create(this));
	}
	//保险和公会银币
	if (isButton(button_insurance))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bLeftButtonTag = BUTTON_TAG_PERSONAL_INSURANCE;
		if (button_coins_personal_clone)
		{
			button_coins_personal_clone->setBright(true);
			auto label_personal_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(button_coins_personal_clone, "label_personal_coin"));
			label_personal_coin->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		}
		button_insurance->setBright(false);
		label_insurance->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
		opposedCitySafeBtn();
		button_Sccess_log->setBright(true);
		label_sccess_log->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		m_nMoneyCount = 0;
		m_padString.clear();
		closeView();
		if (m_bIsMyBank)
		{
			enterInsuranceModule();
		}
		else
		{
			ProtocolThread::GetInstance()->getBankInfo(m_bIsMyBank, UILoadingIndicator::create(this));
		}
	}
	subButtonEventByName(target,name);
}
void UIBank::withdrawItem(const int index)
{
	m_pBankItem->type = m_vBankItems[index]->itemtype;
	m_pBankItem->cid = m_vBankItems[index]->characteritemid;
	m_pBankItem->num = m_vBankItems[index]->num;
	m_pBankItem->iid = m_vBankItems[index]->iid;
	m_pBankItem->attack = m_vBankItems[index]->attack;
	m_pBankItem->defense = m_vBankItems[index]->defense;
	m_pBankItem->max_durability = m_vBankItems[index]->max_durability;
	m_pBankItem->durability = m_vBankItems[index]->durability;
	m_pBankItem->m_speed = m_vBankItems[index]->speed;
	m_pBankItem->steer_speed = m_vBankItems[index]->steer_speed;
	m_pBankItem->optional_num = m_vBankItems[index]->optional_num;
	m_pBankItem->optional_item = m_vBankItems[index]->optional_item;
	m_pBankItem->optional_value = m_vBankItems[index]->optional_value;

	
}
void UIBank::subButtonEventByName(Widget* target,std::string name)
{
	if (isButton(label_currency_num))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bNumpadFlag)
		{
			showBankNumPad();
		}
		return;
	}
	if (isButton(label_insure_num))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bNumpadFlag)
		{
			showMyInsuranceNumPad();
		}
	}
	if (isButton(button_numpad_yes) || isButton(panel_mask_numpad)) //退出数字键盘
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_bNumpadFlag)
		{
			m_bNumpadFlag = false;
			auto view = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
			auto viewInsure = getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_BUY_CSB]);
			if (view)
			{
				auto panel_currency_own = view->getChildByName<Widget*>("panel_currency_own");
				panel_currency_own->setVisible(true);
				auto p_mask_numpad = view->getChildByName<Widget*>("panel_mask_numpad");
				p_mask_numpad->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4f), Place::create(Vec2(1200,-90))));
				auto num_root = view->getChildByName<Widget*>("panel_numpad");
				num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, Vec2(-335, -270)),
					CallFunc::create(this, callfunc_selector(UIBank::moveEnd))));
				auto label_currency_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_currency_num"));
				std::string str = label_currency_num->getString();
				if (str.empty())
				{
					label_currency_num->setString("0");
				}
			}
			if (viewInsure)
			{
				auto p_mask_numpad = viewInsure->getChildByName<Widget*>("panel_mask_numpad");
				p_mask_numpad->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4f), Place::create(Vec2(1330, 100))));
				auto num_root = viewInsure->getChildByName<Widget*>("panel_numpad");
				num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, Vec2(0, -270)),
					CallFunc::create(this, callfunc_selector(UIBank::moveEnd))));
				auto label_insure_num = dynamic_cast<Text*>(Helper::seekWidgetByName(viewInsure, "label_insure_num"));
				std::string str = label_insure_num->getString();
				if (str.empty())
				{
					label_insure_num->setString("0");
				}
				changeCostByRatioAndTime();
			}	
		}
		return;
	}
	if (isButton(button_item_nothing) || isButton(button_goodsafe_deposit))//存储物品按钮
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_vBankItems.size() + 1> m_pBankResult->maxitemnum)
		{
			openSuccessOrFailDialog("TIP_BANK_CAPTAIN_FULL");
			return;
		}
		else
		{
			//创建存物品
			openView(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
			auto viewChooseItem = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
			auto whl = UIBankSafeBox::createWarehouseLayer(viewChooseItem,m_bIsMyBank);
			whl->retain();
			m_pItemCenter = whl;
			whl->setRootWidget((Widget*)viewChooseItem);
			whl->setCallBack(CC_CALLBACK_0(UIBank::depositItem,this));
		
			dynamic_cast<UIBankSafeBox*>(m_pItemCenter)->getFirstResult();	
			auto image_titile_bg_creat=viewChooseItem->getChildByName("image_titile_bg_creat");
			
			if (!m_bIsMyBank&&!m_pBankResult->isadmin)
			{
				auto label_title_deposit = image_titile_bg_creat->getChildByName<Text*>("label_title_select_item_2");
				label_title_deposit->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_NOT_GUILDADM_BECARE"]);
				label_title_deposit->setVisible(true);
				auto b_info = image_titile_bg_creat->getChildByName<Button*>("button_info");
				b_info->addTouchEventListener(CC_CALLBACK_2(UIBank::menuCall_func,this));
				b_info->setVisible(false);
			}
			else
			{		
				auto label_title_deposit = image_titile_bg_creat->getChildByName<Text*>("label_title_create_sell_order");
				label_title_deposit->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_DEPOSIT_GOODS"]);
			}
			m_bIsDeposit = true;
		}
			return;
	}
	if (isButton(button_money_deposit))//存储银币
	{
		auto view= getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
		auto panelCobtent =view->getChildByName("panel_content");
		auto imageInPut=panelCobtent->getChildByName("image_input_bg");
	   auto tf_num = imageInPut->getChildByName<Text*>("label_currency_num");
		std::string s_num = tf_num->getString();
		tf_num->setString("0");
		int64_t moneyNum = atoll(m_padString.c_str());
		m_padString.clear();
		m_nMoneyCount = moneyNum;
		if (moneyNum > SINGLE_HERO->m_iCoin)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			openSuccessOrFailDialog("TIP_BANK_DEPOSIT_INPUT_TOO_MUCH");
		}
		else if (moneyNum < 1)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			openSuccessOrFailDialog("TIP_BANK_DEPOSIT_INPUT_NOT_ZERO");
		}
		else
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
			ProtocolThread::GetInstance()->depositMoney(moneyNum, m_bIsMyBank, UILoadingIndicator::create(this));
		}
		tf_num->setString("0");

		if (m_bNumpadFlag)
		{
			m_bNumpadFlag = false;
			auto view = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
			auto panel_currency_own = view->getChildByName<Widget*>("panel_currency_own");
			panel_currency_own->setVisible(true);
			auto p_mask_numpad = view->getChildByName<Widget*>("panel_mask_numpad");
			p_mask_numpad->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4f), Place::create(Vec2(1100, -82))));
			auto num_root = view->getChildByName<Widget*>("panel_numpad");
			num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, Vec2(-335, -270)),
				CallFunc::create(this, callfunc_selector(UIBank::moveEnd))));
			auto label_currency_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_currency_num"));
			std::string str = label_currency_num->getString();
			if (str.empty())
			{
				label_currency_num->setString("0");
			}
		}
		return;
	}
	if (isButton(button_money_take))//取出银币
	{
		auto view= getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
		auto panelCobtent =view->getChildByName("panel_content");
		auto imageInPut=panelCobtent->getChildByName("image_input_bg");
		auto tf_num = imageInPut->getChildByName<Text*>("label_currency_num");
		std::string s_num = tf_num->getString();
		tf_num->setString("0");
		
		int64_t moneyNum = atoll(m_padString.c_str());
		m_padString.clear();
		m_nMoneyCount = moneyNum;
		m_bIsDeposit = false;
		if (moneyNum > m_pBankResult->coin)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			openSuccessOrFailDialog("TIP_BANK_TAKE_INPUT_TOO_MUCH");
		}
		else if (moneyNum < 1)
		{
			SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
			openSuccessOrFailDialog("TIP_BANK_GET_INPUT_NOT_ZERO");
		}
		else
		{
			//不是公会管理员不能从银行取钱
			if (!m_bIsMyBank&&!m_pBankResult->withdraw_cash)
			{		
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
				openSuccessOrFailDialog("TIP_BANK_CANNOT_TAKE_MONEY");
			}
			else
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
				ProtocolThread::GetInstance()->withdrawMoney(m_nMoneyCount, m_bIsMyBank, UILoadingIndicator::create(this));
			}
		}
		tf_num->setString("0");

		if (m_bNumpadFlag)
		{
			m_bNumpadFlag = false;
			auto view = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
			auto panel_currency_own = view->getChildByName<Widget*>("panel_currency_own");
			panel_currency_own->setVisible(true);
			auto p_mask_numpad = view->getChildByName<Widget*>("panel_mask_numpad");
			p_mask_numpad->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4f), Place::create(Vec2(1100, -82))));
			auto num_root = view->getChildByName<Widget*>("panel_numpad");
			num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, Vec2(-335, -270)),
				CallFunc::create(this, callfunc_selector(UIBank::moveEnd))));
			auto label_currency_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_currency_num"));
			std::string str = label_currency_num->getString();
			if (str.empty())
			{
				label_currency_num->setString("0");
			}
		}
		return;
	}
	if (isButton(button_item_lock_un))//物品栏里的格子
	{
		return;
	}
	if (isButton(button_item_bg))//点击物品
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openView(BANK_COCOS_RES[VIEW_SLOT_DETAIL_CSB]);
		auto viewSlot = getViewRoot(BANK_COCOS_RES[VIEW_SLOT_DETAIL_CSB]);

		auto buttonTake =viewSlot->getChildByName<Button*>("button_take");
		if (!m_bIsMyBank&&!m_pBankResult->withdraw_item)
		{
			buttonTake->setBright(false);
		}
		else
		{
			buttonTake->setBright(true);
		}

		auto labelname = viewSlot->getChildByName<Text*>("label_item_name");
		auto image_item_bg = viewSlot->getChildByName<ImageView*>("image_item_bg");
		image_item_bg->setTouchEnabled(true);
		image_item_bg->addTouchEventListener(CC_CALLBACK_2(UIBank::menuCall_func,this));
		
		auto tags = m_pBankItem->iid;
		auto imageitem=image_item_bg->getChildByName<ImageView*>("image_item");
		auto labelNum=image_item_bg->getChildByName<Text*>("text_item_num");
		auto image_item_bg_lv = image_item_bg->getChildByName<ImageView*>("image_item_bg_lv");
		withdrawItem(target->getTag() - START_INDEX);
		if (m_pBankItem->type == ITEM_TYPE_SHIP_EQUIP)
		{
			image_item_bg->setTag(m_pBankItem->iid + SAFE_EQUIP_TAG);	
		
			if (!m_pEquipDetail)
			{
				m_pEquipDetail = new EquipmentDefine;
			}		
			m_pEquipDetail->equipmentid = m_pBankItem->iid;
			m_pEquipDetail->attack = m_pBankItem->attack;
			m_pEquipDetail->maxdurable = m_pBankItem->max_durability;
			m_pEquipDetail->durable = m_pBankItem->durability;
			m_pEquipDetail->defense = m_pBankItem->defense;
			m_pEquipDetail->speed = m_pBankItem->m_speed;
			m_pEquipDetail->steer_speed = m_pBankItem->steer_speed;
			m_pEquipDetail->optionalnum = m_pBankItem->optional_num;
			m_pEquipDetail->optionalid = m_pBankItem->optional_item;
			m_pEquipDetail->optionalvalue = m_pBankItem->optional_value;

		}
		else if (m_pBankItem->type == ITEM_TYPE_DRAWING)
		{
			image_item_bg->setTag(m_pBankItem->iid + SAFE_ITEM_DRAWING);
		}
		else if (m_pBankItem->type == ITEM_TYPE_PORP)
		{
			image_item_bg->setTag(m_pBankItem->iid + SAFE_ITEM_PROP);
		}
		else
		{
			image_item_bg->setTag(m_pBankItem->iid);
		}
		
		m_bIsDeposit = false;
		std::string iconPath = getItemIconPath(m_pBankItem->iid);
		std::string lllabelname = getItemName(m_pBankItem->iid);
		imageitem->loadTexture(iconPath);
		setTextSizeAndOutline(labelNum, m_pBankItem->num);
		labelname->setTextVerticalAlignment(TextVAlignment::TOP);
		labelname->setFontSize(30);
		labelname->setString(lllabelname);
		setBgButtonFormIdAndType(image_item_bg, m_pBankItem->iid, m_pBankItem->type);
		setTextColorFormIdAndType(labelname, m_pBankItem->iid, m_pBankItem->type);
		setBgImageColorFormIdAndType(image_item_bg_lv, m_pBankItem->iid, m_pBankItem->type);
		return;
	}
	if (isButton(button_add_slot) || isButton(button_item_lock_light))//扩展格子
	{
		
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	
		int64_t coin = 0;
		int32_t ask_next = m_pBankResult->maxitemnum+1;
		if (m_pBankResult->ispersonal == 1)
		{
			coin = 10000 * (ask_next - 3);//个人默认为3个
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushSilverConfirmView("TIP_BANK_EXPAND_TITLE", "TIP_BANK_EXPAND_CONTENT", coin);
		}else
		{
			if (!m_pBankResult->isadmin)
			{
				openSuccessOrFailDialog("TIP_GUILD_NOT_ACCESS");
				return;
			}
			coin = 50000 * (ask_next - 2);//工会默认为2个
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushSilverConfirmView("TIP_BANK_EXPAND_TITLE", "TIP_BANK_EXPAND_CONTENT", coin);
		}		
		return;
	}
	if (isButton(button_s_yes))//扩展格子确定按钮
	{
		ProtocolThread::GetInstance()->expandBank(m_bIsMyBank,UILoadingIndicator::create(this));
		return;
		
	}
	if (isButton(button_close))//取消按钮
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView(BANK_COCOS_RES[VIEW_SLOT_DETAIL_CSB]);
		return;
	}
	if (isButton(button_ok))//存储或取出银币提示界面的确定按钮
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		if (m_bIsDeposit)
		{
			ProtocolThread::GetInstance()->depositMoney(m_nMoneyCount,m_bIsMyBank,UILoadingIndicator::create(this));
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_BANK_DESPOIT_MONEY_SUCCESS");
		}else
		{
			ProtocolThread::GetInstance()->withdrawMoney(m_nMoneyCount,m_bIsMyBank,UILoadingIndicator::create(this));
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_BANK_WITHDRAW_MONEY_SUCCESS");
		}
        closeView();
		return;
	}
	if (isButton(button_cancel))//存储或取出银币提示界面的取消按钮
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
	if (isButton(button_yes))//扩展格子确定按钮
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		ProtocolThread::GetInstance()->expandBank(m_bIsMyBank,UILoadingIndicator::create(this));
		closeView();
		return;
	}
	
	if (isButton(button_confirm_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//确定覆盖投保
		if (m_bChangeInsurePlan)
		{
			m_bChangeInsurePlan = false;
			openInsuranceInsure();
			return;
		}
		//提示页面，确定存储或取出物品
		if (m_bIsDeposit)
		{
			ProtocolThread::GetInstance()->depositItem(m_pBankItem->cid,m_pBankItem->iid,m_pBankItem->type,m_pBankItem->num,m_bIsMyBank,UILoadingIndicator::create(this));
		}
		else
		{
			ProtocolThread::GetInstance()->withdrawItem(m_pBankItem->cid,m_pBankItem->type,m_pBankItem->num,m_bIsMyBank,UILoadingIndicator::create(this));
		}
		return;
	}
	//取消覆盖投保
	if (isButton(button_confirm_no))
	{
		m_bChangeInsurePlan = false;
		return;
	}
	if (isButton(button_take))//取出物品
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		
		closeView(BANK_COCOS_RES[VIEW_SLOT_DETAIL_CSB]);
		if (!m_bIsMyBank&&!m_pBankResult->withdraw_item)
		{		
			openSuccessOrFailDialog("TIP_BANK_CANNOT_TAKE_GOODS");
		}
		else
		{
			updateItemSelect(false);
		}
		
	}
	if (isButton(button_fee_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_BANK_HOSTILE_STATES_COST_FEE_TITLE", "TIP_BANK_HOSTILE_STATES_COST_FEE");

		return;
	}
	//打开投保界面
	if (isButton(button_buy))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openInsuranceInsure();
		return;
	}
	//确定投保
	if (isButton(button_myinsuer_ok))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (isNumber(m_padString))
		{
			int64_t num = atoll(m_padString.c_str());
			if (num<=0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_BANK_INSURANCE_TEXTFIELD_CONTENT_NOT_NUM");
				return;
			}
			else if (num<10000)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_INSURANCE_COIN_TOO_LITTLE");
				return;
			}
			else if ((n_insurance_coinCost>SINGLE_HERO->m_iCoin) && m_insurance_item[n_chooseRatio]->insurepercent==70)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_BANK_INSURANCE_COIN_NOT_ENOUGH");
				return;
			}
			else if (n_insurance_VticketCost>SINGLE_HERO->m_iGold)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_BANK_INSURANCE_V_NOT_ENOUGH");
				return;
			}
			else
			{
				closeView();
				ProtocolThread::GetInstance()->buyInsurance(num,m_ratio_times[n_chooseRatiotime],m_insurance_item[n_chooseRatio]->insurepercent,n_protectDelegate,UILoadingIndicator::create(this));
			}
		}
 		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_BANK_INSURANCE_TEXTFIELD_CONTENT_NOT_NUM");
			return;
		}
		return;
	}
	//关闭投保界面
	if (isButton(button_myinsure_cancel))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
	//改变投保
	if (isButton(button_plane_change))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_bChangeInsurePlan = true;
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_BANK_CHANGE_INSURE_ASK_TITLE", "TIP_BANK_CHANGE_INSURE_ASK_CONTENT");
		return;
	}
	//投保的问号按钮
	if (isButton(button_forInsure_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_BANK_INSURANCE_QUESTION_TITLE", "TIP_BANK_INSURANCE_QUESTION_CONTENT");
		return;
	}
	//safe界面的物品详情
	if (isButton(image_item_bg))
	{
		UICommon::getInstance()->openCommonView(this);
		auto propTag = target->getTag();
		//装备
		if (propTag >= SAFE_EQUIP_TAG&&propTag<SAFE_ITEM_DRAWING)
		{
				UICommon::getInstance()->flushEquipsDetail(m_pEquipDetail, propTag - SAFE_EQUIP_TAG, true,true);
		}
		else if (propTag >= SAFE_ITEM_DRAWING&&propTag<SAFE_ITEM_PROP)
		{
			UICommon::getInstance()->flushDrawingDetail(nullptr, propTag - SAFE_ITEM_DRAWING, true);
		}
		else if (propTag>SAFE_ITEM_PROP)
		{
			UICommon::getInstance()->flushItemsDetail(nullptr, propTag - SAFE_ITEM_PROP, true);
		}
		else 
		{
			UICommon::getInstance()->flushItemsDetail(nullptr, propTag, true);
		}
		
		return;
	}
	//保险委托未开启
	if (isButton(panel_cover))
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openViewAutoClose("TIP_INSURANCE_CAN_NOT_CHOOSE_DELEGATION");
		return;
	}
}
void UIBank::SliderEvent(Ref*)
{
	if (m_bIsDeposit)
	{
		//存物品
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_BANK_SAFEINFO_TITLE","TIP_BANK_DEPOSIT_ITEM_SURE");
		
	}else
	{
		//取物品
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_BANK_WITHDRAW_ITEM_SURE","TIP_BANK_WITHDRAW_ITEM_SURE");
	}
	closeView(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
}
void UIBank::button_safebackEvent(Ref*)
{
	closeView(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
	if (m_bIsBankItem)
	{
		updateItemView();
	}else
	{
		ProtocolThread::GetInstance()->getBankInfo(m_bIsMyBank,UILoadingIndicator::create(this));
	}
}
void UIBank::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetBankInfoResult:
		{
			GetBankInfoResult* result = (GetBankInfoResult*)message;
			if (result->failed == 0)
			{			
				if (m_pBankResult)
				{
					get_bank_info_result__free_unpacked(m_pBankResult,0);
				}
				m_pBankResult = result;
				m_insurance_item.clear();
				if (m_insurance_item.size() <= 0)
				{
					for (int i = 0; i < result->n_insurane; i++)
					{
						m_insurance_item.push_back(result->insurane[i]);
					}
				}
				if (result->myinsurance)
				{
					m_myInsurance->leftamount = result->myinsurance->leftamount;
					m_myInsurance->lefttimeinseconds = result->myinsurance->lefttimeinseconds;
					m_myInsurance->percent = result->myinsurance->percent;
					m_myInsurance->totalamount = result->myinsurance->totalamount;
					m_myInsurance->protectdelegate = result->myinsurance->protectdelegate;
				}
				if (!isScheduled(schedule_selector(UIBank::insuranceLeftTime)))
				{
					this->schedule(schedule_selector(UIBank::insuranceLeftTime), 1.0f);
				}
				if (m_bIsMyBank)
				{
					
					switch (m_bLeftButtonTag)
					{
					case UIBank::BUTTON_TAG_NONE:
					break;
					case UIBank::BUTTON_TAG_PERSONAL_COIN:
					{
						flushMyCoinGold();
						updateMoneyView();
						break;
					}
					
					case UIBank::BUTTON_TAG_PERSONAL_INSURANCE:
					{
							enterInsuranceModule();
							break;
					}
					case UIBank::BUTTON_TAG_GUILD_COIN:
					{
						break;
					}
					
					case UIBank::BUTTON_TAG_GUILD_SAFE:
					break;
					case UIBank::BUTTON_TAG_GUILD_LOG:
					break;
					default:
					break;
					}
				}
				else
				{
					flushMyCoinGold();
					hideTakeButton();
					updateMoneyView();				
				}		
				opposedCity();
				m_bIsBankItem = true;			
				pushItems_vector();
				
				if (m_bIsChangeBank)
				{
					m_bIsChangeBank = false;
				}
			}
			else if (result->failed ==1)//没有公会
			{	
				if (m_bIsChangeBank)
				{
					m_bIsMyBank = !m_bIsMyBank;
				}
				showHaveNoGuild();
				get_bank_info_result__free_unpacked(result,0);
			}
			break;
		}
	case PROTO_TYPE_GetBankLogResult:
		{
			GetBankLogResult* result = (GetBankLogResult*)message;
			if (result->failed == 0)
			{
				updateLogView(result);
				get_bank_log_result__free_unpacked(result,0);
			}else
			{
				openSuccessOrFailDialog("TIP_BANK_LOG_FAIL");
			}
			break;
		}
	case PROTO_TYPE_WithdrawMoneyResult:
		{
			WithdrawMoneyResult* result = (WithdrawMoneyResult*)message;
			if (result->failed == 0)
			{
				m_bIsBankLog = false;
				SINGLE_HERO->m_iCoin = result->coin;
				SINGLE_HERO->m_iGold = result->gold;
				m_pBankResult->coin -= m_nMoneyCount;
				updateMoneyView();
				flushMyCoinGold();

				//主线任务完成对话
				addNeedControlWidgetForHideOrShow(nullptr, true);
				if (MainTaskManager::GetInstance()->checkTaskAction())
				{
					allTaskInfo(false, MAIN_STORY);
					MainTaskManager::GetInstance()->notifyUIPartDelayTime(0.5);
				}
				else
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openViewAutoClose("TIP_BANK_WITHDRAW_MONEY_SUCCESS");
				}
				
			}else
			{
				openSuccessOrFailDialog("TIP_BANK_GET_MONEY_FAIL");

			}
			break;
		}
	case PROTO_TYPE_DepositMoneyResult:
		{
			DepositMoneyResult* result = (DepositMoneyResult*)message;
			if (result->failed == 0)
			{
				m_bIsBankLog = false;
				SINGLE_HERO->m_iCoin = result->coin;
				SINGLE_HERO->m_iGold = result->gold;
				m_pBankResult->coin += m_nMoneyCount;
				updateMoneyView();
				flushMyCoinGold();
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_BANK_DESPOIT_MONEY_SUCCESS");
			}else
			{
				openSuccessOrFailDialog("TIP_CONNECT_FAIL");
			}
			break;
		}
	case PROTO_TYPE_ExpandBankResult:
		{
			ExpandBankResult* result = (ExpandBankResult*)message;
			if (result->failed == 0)
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
				m_pBankResult->maxitemnum = result->bankcapacity;
				SINGLE_HERO->m_iCoin=result->coin;
				SINGLE_HERO->m_iGold=result->gold;
				updateItemView();
			}
			else if (result->failed == 4)
			{
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
				openSuccessOrFailDialog("TIP_EXBANK_BAG_COIN_NOT_FULL");
			}
			else
			{	
				SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
				openSuccessOrFailDialog("TIP_BANK_EXPAND_BOX_FAIL");
			}
			break;
		}
	case PROTO_TYPE_WithdrawItemResult:
		{
			WithdrawItemResult* result = (WithdrawItemResult*)message;
			if (result->failed == 0)
			{
				m_bIsBankLog = false;
				std::shared_ptr<BankItem>withDrawItem(new BankItem);
				withDrawItem->type = result->type;
				withDrawItem->cid = m_pBankItem->cid;
				withDrawItem->iid = m_pBankItem->iid;
				withDrawItem->num = m_pBankItem->num;
				withDrawItem->durability = m_pBankItem->durability;
				withDrawItem->max_durability = m_pBankItem->max_durability;
				withDrawItem->m_speed = m_pBankItem->m_speed;
				withDrawItem->attack = m_pBankItem->attack;
				withDrawItem->defense = m_pBankItem->defense;
				withDrawItem->steer_speed = m_pBankItem->steer_speed;
				withDrawItem->optional_item = m_pBankItem->optional_item;
				withDrawItem->optional_num = m_pBankItem->optional_num;
				withDrawItem->optional_value = m_pBankItem->optional_value;

				deleteItem_vector(withDrawItem);
				updateItemView();
				//InformView::getInstance()->openInformView(this);
				//InformView::getInstance()->openViewAutoClose("TIP_BANK_GET_ITEM_SUCCESS");
			} else if (result->failed == 25)
			{
				openSuccessOrFailDialog("TIP_EXCREATE_BAG_FULL");
			}
			else
			{
				openSuccessOrFailDialog("TIP_BANK_GET_ITEM_FAIL");
			}
			break;
		}
	case PROTO_TYPE_DepositItemResult:
		{
			DepositItemResult* result = (DepositItemResult*)message;
			if (result->failed == 0)
			{
				m_bIsBankLog = false;  
				std::shared_ptr<BankItem>bankItem(new BankItem);
				bankItem->type = result->type;
				bankItem->cid = result->id;
				bankItem->iid = result->deposititem->iid;
				bankItem->num = result->deposititem->num;
				bankItem->durability = result->deposititem->durability;
				bankItem->max_durability = result->deposititem->max_durability;
				bankItem->m_speed = result->deposititem->speed;
				bankItem->attack = result->deposititem->attack;
				bankItem->defense = result->deposititem->defense;
				bankItem->steer_speed = result->deposititem->steer_speed;
				bankItem->optional_item = result->deposititem->optional_item;
				bankItem->optional_num = result->deposititem->optional_num;
				bankItem->optional_value = result->deposititem->optional_value;
				inserItem_vector(bankItem);
				updateItemView();
				//存储物品后返回safe界面,要删除存储物品板
				closeView(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
				//InformView::getInstance()->openInformView(this);
				//InformView::getInstance()->openViewAutoClose("TIP_BANK_DEPOSIT_ITEM_SUCCESS");
			}else if(result->failed==1){
				openSuccessOrFailDialog("TIP_BANK_DEPOSIT_GUILD_LIMIT");
			}else
			{
				openSuccessOrFailDialog("TIP_BANK_DEPOSIT_ITEM_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetItemsToSellResult:
		{
			GetItemsToSellResult* result = ( GetItemsToSellResult*)message;
			if (result->failed == 0)
			{
				dynamic_cast<UIBankSafeBox*>(m_pItemCenter)->updateView(result,false);
			}else
			{		
				openSuccessOrFailDialog("TIP_BANK_SELECK_ITEM_FAIL");
			}
			break;
		}
	case PROTO_TYPE_BuyInsuranceResult:
	{
			BuyInsuranceResult*result = (BuyInsuranceResult*)message;
			if (result->failed ==0)
			{
				m_buyInsurenceResult = result;
				m_myInsurance->leftamount = result->count;
				m_myInsurance->lefttimeinseconds = result->lefttimeinseconds;
				m_myInsurance->percent = result->percent;
				m_myInsurance->totalamount = result->count;
				m_myInsurance->protectdelegate = result->protectdelegate;
				SINGLE_HERO->m_iCoin = result->coin;
				SINGLE_HERO->m_iGold = result->gold;
				updateOnlyCoinGold();
				showMyInsuranceInfo();
				if (isScheduled(schedule_selector(UIBank::insuranceLeftTime)))
				{
				
				}
				else
				{
					this->schedule(schedule_selector(UIBank::insuranceLeftTime), 1.0f);
				}
				if (result->costvticket > 0)
				{
					Utils::consumeVTicket("11", 1, result->costvticket);
				}
			}
			else
			{
				openSuccessOrFailDialog("TIP_CENTER_OPERATE_FAIL");
			}
			break;
	}
	default:
		break;
	}
}

void UIBank::pushItems_vector()
{
	if (m_pBankResult)
	{
		m_vBankItems.clear();
		for (auto i = 0; i < m_pBankResult->n_bankitem; i++)
		{
			m_vBankItems.push_back(m_pBankResult->bankitem[i]);
		}
	}
}
void UIBank::inserItem_vector(std::shared_ptr<BankItem>bankItem)
{
	if (m_pBankResult)
	{
		bool isFind = false;
		for (auto item = m_vBankItems.begin(); item != m_vBankItems.end(); item++)
		{
			if((*item)->characteritemid == bankItem->cid)
			{
				(*item)->num++;
				isFind = true;
				break;
			}
		}
		if (!isFind)
		{
			BankItemDefine* item = new BankItemDefine;
			item->base.n_unknown_fields = -1; // custom new for delete
			item->itemtype = bankItem->type;
			item->characteritemid = bankItem->cid;
			item->iid = bankItem->iid;
			item->num = bankItem->num;
			item->durability = bankItem->durability;
			item->max_durability = bankItem->max_durability;
			item->speed = bankItem->m_speed;
			item->attack = bankItem->attack;
			item->defense = bankItem->defense;
			item->steer_speed = bankItem->steer_speed;
			item->optional_item = bankItem->optional_item;
			item->optional_num = bankItem->optional_num;
			item->optional_value = bankItem->optional_value;
			m_vBankItems.push_back(item);
		}
	}
}
void UIBank::deleteItem_vector(std::shared_ptr<BankItem>withDrawItem)
{
	if (m_pBankResult)
	{	
		for (auto item = m_vBankItems.begin();item != m_vBankItems.end();item++)
		{
			BankItemDefine* b_item = *item;
			if (b_item->characteritemid == withDrawItem->cid && b_item->itemtype == withDrawItem->type && b_item->iid == withDrawItem->iid)
			{
				b_item->num -= withDrawItem->num;
				if (b_item->num < 1)
				{
					m_vBankItems.erase(item);
					if (b_item->base.n_unknown_fields == -1)  // custom new 
					{
						delete b_item;
						b_item = nullptr;
					}
				}
				break;
			}
		}
	}
}
void UIBank::updateItemView()
{
	openView(BANK_COCOS_RES[VIEW_SAFE_CSB]);
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_SAFE_CSB]);
	std::string s_coin = numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iCoin));
	std::string s_gold = numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iGold));
	auto buttonsiliver=view->getChildByName<Button*>("button_silver");
	auto silvernum= buttonsiliver->getChildByName<Text*>("label_silver_num");
	silvernum->setString(s_coin);
	auto buttongold=view->getChildByName<Button*>("button_gold");
	auto goldnum= buttongold->getChildByName<Text*>("label_gold_num");
	goldnum->setString(s_gold);
	auto listview_safe=view->getChildByName("listview_safe_slot");
	auto t_safe_num = listview_safe->getChildByName<Text*>("label_safe_slot_num");
	std::string safe_num = StringUtils::format("%d/%d",m_vBankItems.size(),m_pBankResult->maxitemnum);
	t_safe_num->setString(safe_num);
	setTextFontSize(silvernum);
	setTextFontSize(goldnum);
	auto lv_items = view->getChildByName<ListView*>("listview_house");
	        lv_items->setDirection(ListView::Direction::VERTICAL);
			lv_items->setBounceEnabled(true);
			lv_items->removeAllChildrenWithCleanup(true);
	auto image_pulldown_item = view->getChildByName<ImageView*>("image_pulldown_item");
	auto button_pulldown = image_pulldown_item->getChildByName<Button*>("button_pulldown_item");
	button_pulldown->setPositionX(image_pulldown_item->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);

	 auto imBg= view->getChildByName<ImageView*>("image_item_bg");
			 imBg->setVisible(false);
	
	auto btnHave =view->getChildByName<Widget*>("button_item_bg");
	auto btnNormal=view->getChildByName<Widget*>("button_item_nothing");
	auto btnLockLight=view->getChildByName<Widget*>("button_item_lock_light");
	auto btnLockDuck=view->getChildByName<Widget*>("button_item_lock_un");
	
	btnHave->setVisible(true);
	btnNormal->setVisible(true);
	btnLockLight->setVisible(true);
	btnLockDuck->setVisible(true);

	//safe panel
	int numAll=m_pBankResult->maxitemnum+1;
	int num =m_vBankItems.size();
	int list_line;
	if (numAll%8==0)
	{
		list_line=numAll/8;
	}
	else
	{
		list_line=(numAll/8)+1;
	}
	for (int i=0;i<list_line;++i)
	{
		ListView *item_layer = ListView::create();
		item_layer->setDirection(ListView::Direction::HORIZONTAL);
		item_layer->setItemsMargin(3);
		item_layer->setContentSize(Size(812,100));
	   
		for (int j=0;j<8;j++)
		{
			if(8*i+j<m_vBankItems.size())
			{
			Widget*btn=(Button*)btnHave->clone();
			btn->setSwallowTouches(false);
			itemInfoForItemView(btn,8*i+j);
			item_layer->pushBackCustomItem(btn);
			}
			else
			{
				if (8*i+j<m_pBankResult->maxitemnum)
				{
					Widget*btnN=(Button*)btnNormal->clone();
					btnN->setSwallowTouches(false);
					item_layer->pushBackCustomItem(btnN);
				}
				else 
				{
					if (8 * i + j==m_pBankResult->maxitemnum)
					{
						Widget*btnLoDoLight = (Button*)btnLockLight->clone();
						btnLoDoLight->setSwallowTouches(false);
						item_layer->pushBackCustomItem(btnLoDoLight);
					}
					else
					{
						Widget*btnLoDo = (Button*)btnLockDuck->clone();
						btnLoDo->setSwallowTouches(false);
						item_layer->pushBackCustomItem(btnLoDo);
					}
					
				}
			}
		}
		lv_items->pushBackCustomItem(item_layer);
	}
	btnHave->setVisible(false);
	btnNormal->setVisible(false);
	btnLockLight->setVisible(false);
	btnLockDuck->setVisible(false);
	addListViewBar(lv_items,image_pulldown_item);
}

void UIBank::itemInfoForItemView(Widget* item,int index)
{
	auto bankItem = m_vBankItems[index];
	item->setTag(index + START_INDEX);
	auto i_item_icon = item->getChildByName<ImageView*>("image_item");
	auto image_item_bg = item->getChildByName<ImageView*>("image_item_bg");
	auto image_item_bg_lv = item->getChildByName<ImageView*>("image_item_bg_lv");

	auto item_num = item->getChildByName<Text*>("text_item_num");
	std::string iconPath;
	std::string name;
	if (bankItem->itemtype == ITEM_TYPE_SHIP)
	{
		name = getShipName(bankItem->iid);
		iconPath = getShipIconPath(bankItem->iid);
	}
	else if (bankItem->itemtype == ITEM_TYPE_SHIP_EQUIP)
	{
		if (bankItem->optional_item> 0)
		{
			addStrengtheningIcon(item);
		}
		if (bankItem->durability <= SHIP_EQUIP_BROKEN || bankItem->durability <= bankItem->max_durability *SHIP_EQUIP_BROKEN_PERCENT)
		{
			addequipBrokenIcon(item);
			setBrokenEquipRed(i_item_icon);
		}
		name = getItemName(bankItem->iid);
		iconPath = getItemIconPath(bankItem->iid);
	}
	else
	{
		name = getItemName(bankItem->iid);
		iconPath = getItemIconPath(bankItem->iid);
	}
	i_item_icon->loadTexture(iconPath);
	setBgButtonFormIdAndType(image_item_bg, bankItem->iid, bankItem->itemtype);
	setBgImageColorFormIdAndType(image_item_bg_lv, bankItem->iid, bankItem->itemtype);
	setTextSizeAndOutline(item_num, bankItem->num);
}
void UIBank::updateMoneyView()
{
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
	auto panel_content=view->getChildByName("panel_content");
	auto label_silver_num_have=panel_content->getChildByName<Text*>("label_silver_num_have");
	label_silver_num_have->setLocalZOrder(10);
	label_silver_num_have->setVisible(true);
	std::string s_coin = numSegment(StringUtils::format("%lld",m_pBankResult->coin));
	label_silver_num_have->setString(s_coin);	
}
void UIBank::updateLogView(const GetBankLogResult* result)
{
	openView(BANK_COCOS_RES[VIEW_LOG_GUILD_CSB]);
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_LOG_GUILD_CSB]);
	if (!view){ return; }	
	auto panel_log_no =view ->getChildByName("panel_log_no");
	panel_log_no->setVisible(false);
	
	//公会的log
	auto lv_log = view->getChildByName<ListView*>("listview_log_guild");
	//个人的log
	auto personal_log = view->getChildByName<ListView*>("listview_log_personal");
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	Widget*w_item = nullptr;
	ListView*i_listViewLog = nullptr;
	if (m_bIsMyBank)
	{
		lv_log->setVisible(false);
		personal_log->setVisible(true);
		w_item = personal_log->getItem(0);
		w_item->retain();
		personal_log->removeAllChildrenWithCleanup(true);
		i_listViewLog = personal_log;
	}
	else
	{
		lv_log->setVisible(true);
		personal_log->setVisible(false);
		w_item = lv_log->getItem(0);
		w_item->retain();
		lv_log->removeAllChildrenWithCleanup(true);
		i_listViewLog = lv_log;
	}
	w_item->setVisible(true);
	
	auto label1=Label::create(" "," ",38);
	label1->setColor(Color3B(46,29,14));
	view->addChild(label1,1,1);
	
	label1->setPosition(Vec2(view->getContentSize().width/2,view->getContentSize().height/2));
	label1->setVisible(false);
	int num = result->n_log;
	if (num < 1)
	{
		w_item->setVisible(false);
		i_listViewLog->pushBackCustomItem(w_item);
		label1->setVisible(true);
		label1->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_NOT_RANK"]);
		image_pulldown->setVisible(false);
		return;
	}
	else
	{
		image_pulldown->setVisible(true);
		view->removeChildByTag(1);
	}
	for (int i = 0; i < num; i++)
	{
		auto item = w_item->clone();
		item->setTag(START_INDEX + i);
		itemInfoForLogView(item,result->log[i]);
		i_listViewLog->pushBackCustomItem(item);
	}
	w_item->release();
	
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2+3);
	addListViewBar(i_listViewLog, image_pulldown);
}

#ifndef WIN32
static time_t my_timegm(struct tm * tm)
{
    time_t ret;
    char *tz;

    tz=getenv("TZ");
    setenv("TZ", "", 1);
    tzset();
    ret = mktime(tm);
    if(tz){
    	setenv("TZ", tz, 1);
    }else{
    	unsetenv("TZ");
    }
    tzset();
    return ret;
}
#endif



void UIBank::itemInfoForLogView(Widget* parent,std::string context)
{
	auto time_1 = parent->getChildByName<Text*>("label_time_1");
	auto time_2 = parent->getChildByName<Text*>("label_time_2");
	auto iconParet=parent->getChildByName<Button*>("button_good_bg");
	auto icon=iconParet->getChildByName<ImageView*>("image_goods");
	auto labelgoods_name=parent->getChildByName<Text*>("label_goods_name");
	auto label_goods_num=parent->getChildByName<Text*>("label_goods_num");
	auto labeltakeor_deposit=parent->getChildByName<Text*>("label_take_or_deposit");
	icon->setVisible(true);
	rapidjson::Document root;
	if(root.Parse<0>(context.c_str()).HasParseError()){
		return;
	}
	if(root.IsNull())
		return;
	auto& logObj = DictionaryHelper::getInstance()->getSubDictionary_json(root,"log");

	auto timeStr = DictionaryHelper::getInstance()->getStringValue_json(root,"time");
	std::string::size_type flag_1 = context.find(",");
	std::string s_time=" ";
	std::string s_info =" ";

//convert to local timezone on android and ios.
#ifndef WIN32
		time_t t = (time_t)atol(timeStr);
		struct tm *new_time = localtime(&t);
		char buf[80];
		bzero(buf, 80);
		std::strftime(buf, 80, "%F %R", new_time);
		s_time = std::string(buf);
#endif
		
	if(logObj.IsNull())
		return;

	auto type = DictionaryHelper::getInstance()->getIntValue_json(logObj,"type");

	long long  count = DictionaryHelper::getInstance()->getFloatValue_json(logObj, "count");

	if(type == 2 || type == 4)
	{
		int iid = DictionaryHelper::getInstance()->getIntValue_json(logObj,"itemid");
		if(type == 2)
		{
			//withdraw
			labelgoods_name->setString(logItemName(getItemName(iid)));
			label_goods_num->setString(String::createWithFormat("x %lld",count)->_string);
			labeltakeor_deposit->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_WITHDRAW"].data());
			icon->ignoreContentAdaptWithSize(false);
			icon->loadTexture(getItemIconPath(iid));
		}else{		
			labelgoods_name->setString(logItemName(getItemName(iid)));
			label_goods_num->setString(String::createWithFormat("x %lld",count)->_string);
			labeltakeor_deposit->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_DEPOSIT"]);
			icon->ignoreContentAdaptWithSize(false);
			icon->loadTexture(getItemIconPath(iid));
		}
		if (!m_bIsMyBank)
		{
			auto palyerName = parent->getChildByName<Text*>("label_name");
			std::string name = DictionaryHelper::getInstance()->getStringValue_json(logObj,"name");
			s_info += "[";
			s_info += name;
			s_info += "]";
			palyerName->setString(s_info);
		}
	}
	else if(type == 1 || type == 3)
	{
		 
		if(type == 1){//withdraw
			
			icon->ignoreContentAdaptWithSize(false);
			icon->loadTexture(getItemIconPath(10002));
			labelgoods_name->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_SILVER"]);
			label_goods_num->setString("x " + (numSegment( String::createWithFormat("%lld",count)->_string)));
			labeltakeor_deposit->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_WITHDRAW"]);

		}else{
			
			icon->ignoreContentAdaptWithSize(false);
			icon->loadTexture(getItemIconPath(10002));
			labelgoods_name->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_SILVER"]);
			label_goods_num->setString("x " + (numSegment(String::createWithFormat("%lld", count)->_string)));
			labeltakeor_deposit->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_DEPOSIT"]);
		}
		if (!m_bIsMyBank)
		{
			auto palyerName = parent->getChildByName<Text*>("label_name");
			std::string name = DictionaryHelper::getInstance()->getStringValue_json(logObj,"name");
			s_info += "[";
			s_info += name;
			s_info += "]";
			palyerName->setString(s_info);
		}
	}
	time_1->setString(timeUtil(atoll(timeStr), TIME_UTIL::_TU_YEAR_MON_DAY, TIME_TYPE::_TT_LOCAL));
	time_2->setString(timeUtil(atoll(timeStr), TIME_UTIL::_TU_HOUR_MIN_SEC, TIME_TYPE::_TT_LOCAL));
	setTextFontSize(time_1);
	setTextFontSize(time_2);
}

void UIBank::depositItem()
{
	updateItemSelect(true);
    
}

void UIBank::updateItemSelect(const bool isDeposit)
{
	//选择条
	openView(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
	auto viewSlider = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
	Widget* sliderView = dynamic_cast<Widget*>(viewSlider);
	m_pSlider = UIVoyageSlider::create(sliderView, 10, 0, true);
	m_pSlider->addSliderScrollEvent_1(CC_CALLBACK_1(UIBank::onSliderNumbers, this));
	m_pSlider->retain();

	auto t_title = viewSlider->getChildByName<Text*>("label_title_deposit_or_take");
	auto i_icon_bg = viewSlider->getChildByName<Widget*>("image_item_bg");
	
	i_icon_bg->addTouchEventListener(CC_CALLBACK_2(UIBank::menuCall_func,this));
	auto i_icon = i_icon_bg->getChildByName<ImageView*>("image_item");
	auto image_item_bg_lv = i_icon_bg->getChildByName<ImageView*>("image_item_bg_lv");
	
	if (!isDeposit)//取出
	{
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_TAKE_TITLE"]);
		i_icon_bg->setTouchEnabled(true);
		m_pSlider->initSliderNum(m_pBankItem->num,0);
		if (m_pBankItem->type != ITEM_TYPE_SHIP)
		{	
			std::string iconPath = getItemIconPath(m_pBankItem->iid);
			i_icon->loadTexture(iconPath);
		}else
		{
			std::string iconPath = getShipIconPath(m_pBankItem->iid);
			i_icon->loadTexture(iconPath);
		}
		setBgButtonFormIdAndType(i_icon_bg, m_pBankItem->iid, m_pBankItem->type);
		setBgImageColorFormIdAndType(image_item_bg_lv, m_pBankItem->iid, m_pBankItem->type);
		//tag
		if (m_pBankItem->type == ITEM_TYPE_SHIP_EQUIP)
		{
			i_icon_bg->setTag(m_pBankItem->iid + SAFE_EQUIP_TAG);
		}
		else if (m_pBankItem->type == ITEM_TYPE_DRAWING)
		{
			i_icon_bg->setTag(m_pBankItem->iid + SAFE_ITEM_DRAWING);
		}
		else if (m_pBankItem->type == ITEM_TYPE_PORP)
		{
			i_icon_bg->setTag(m_pBankItem->iid + SAFE_ITEM_PROP);
		}
		else
		{
			i_icon_bg->setTag(m_pBankItem->iid);
		}
	}
	else//存储
	{
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_DEPOSIT_TITLE"]);
		i_icon_bg->setTouchEnabled(true);
		auto itemInfo = dynamic_cast<UIBankSafeBox*>(m_pItemCenter)->getSeletedItemInfo();
		int itemType = dynamic_cast<UIBankSafeBox*>(m_pItemCenter)->getGoodsType();
		m_pBankItem->type = itemType;
		m_pBankItem->cid = itemInfo->characteritemid;
		m_pBankItem->num = itemInfo->amount;
		m_pSlider->initSliderNum(m_pBankItem->num, 0);
		if (itemType == ITEM_TYPE_SHIP)
		{
			m_pBankItem->iid = itemInfo->itemid;
			std::string iconPath = getShipIconPath(m_pBankItem->iid, IMAGE_ICON_OUTLINE);
			i_icon->loadTexture(iconPath);

		}
		else
		{
			m_pBankItem->iid = itemInfo->itemid;
			std::string iconPath = getItemIconPath(m_pBankItem->iid, IMAGE_ICON_OUTLINE);
			i_icon->loadTexture(iconPath);
		}

		setBgButtonFormIdAndType(i_icon_bg, m_pBankItem->iid, m_pBankItem->type);
		setBgImageColorFormIdAndType(image_item_bg_lv, m_pBankItem->iid, m_pBankItem->type);
      //tag
		if (m_pBankItem->type == ITEM_TYPE_SHIP_EQUIP)
		{
			i_icon_bg->setTag(m_pBankItem->iid + SAFE_EQUIP_TAG);
			ITEM_RES item = SINGLE_SHOP->getItemData()[itemInfo->itemid];
			if (!m_pEquipDetail)
			{
				m_pEquipDetail = new EquipmentDefine;
			}
			int att[5];
			std::string property2 = item.property2;
			property2 += ",";
			int startPos = 0;
			for (int i = 0;; i++)
			{
				if (property2.empty())
				{
					break;
				}
				std::string::size_type index = property2.find(',', startPos);
				if (index != std::string::npos)
				{
					std::string subStr = property2.substr(startPos, index - startPos);
					int num = atoi(subStr.c_str());
					att[i] = num;
					startPos = index + 1;
				}
				else
				{
					break;
				}
			}
			if (m_pEquipDetail)
			{
				m_pEquipDetail->attack = att[0];
				m_pEquipDetail->defense = att[1];
				m_pEquipDetail->durable = itemInfo->currentdurability;
				m_pEquipDetail->speed = att[3];
				m_pEquipDetail->steer_speed = att[4];
				m_pEquipDetail->optionalvalue = itemInfo->optional_value;
				m_pEquipDetail->optionalid = itemInfo->optionalitemnum;
				m_pEquipDetail->optionalnum = itemInfo->optionalnum;
			}
		}
		else if (m_pBankItem->type == ITEM_TYPE_DRAWING)
		{
			i_icon_bg->setTag(m_pBankItem->iid + SAFE_ITEM_DRAWING);
		}
		else if (m_pBankItem->type == ITEM_TYPE_PORP)
		{
			i_icon_bg->setTag(m_pBankItem->iid + SAFE_ITEM_PROP);
		}
		else
		{
			i_icon_bg->setTag(m_pBankItem->iid);
		}
	}
	

}
void UIBank::onSliderNumbers(const int num)
{
	m_pBankItem->num = num;
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
	auto t_num = view->getChildByName<Text*>("label_dropitem_num");
	std::string s_num = StringUtils::format("x %d",num);
	t_num->setString(s_num);
}
void UIBank::hideTakeButton()
{
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
	auto panel_content=view->getChildByName("panel_content");
	auto buttonMoneyTake=panel_content->getChildByName<Button*>("button_money_take");
	auto buttonMoneyDeposit=panel_content->getChildByName<Button*>("button_money_deposit");
	auto label_content_take_no=panel_content->getChildByName<Text*>("label_content_take_no");
	int  takePosX=buttonMoneyTake->getPositionX();
	if (!m_bIsMyBank&&!m_pBankResult->withdraw_cash)
	{
		buttonMoneyTake->setVisible(false);
		buttonMoneyDeposit->setPositionX(takePosX-111);
		label_content_take_no->setVisible(true);
		label_content_take_no->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_CANNOT_TAKE_MONEY"]);
	}
	else
	{
		buttonMoneyTake->setVisible(true);
		buttonMoneyDeposit->setPositionX(takePosX-111*2);
		label_content_take_no->setVisible(false);
	}
}
void UIBank::showHaveNoGuild()
{
	auto viewMain = getViewRoot(BANK_COCOS_RES[VIEW_BANK_MAIN_CSB]);
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
	auto image_bg_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewMain, "image_bg_1"));
	auto image_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewMain, "image_bg"));
	image_bg_1->setVisible(true);
	image_bg->setVisible(false);
	auto image_div_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_div_silver"));
	auto image_div_silver_leng = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_div_silver_leng"));
	image_div_silver->setVisible(false);
	image_div_silver_leng->setVisible(true);
	
	auto panel_content=view->getChildByName("panel_content");
	panel_content->setVisible(false);
	auto panelguild_no = view->getChildByName("panel_centent_guild_no");
	panelguild_no->setVisible(true);
	
	auto label_no_found_1_1=panelguild_no->getChildByName<Text*>("label_no_found_1_1");
	auto label_no_found_2_1=panelguild_no->getChildByName<Text*>("label_no_found_2_1");
	label_no_found_1_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_TO_BANK_FAILED"]);
	label_no_found_2_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_JOIN_OR_CREATE_GUILD"]);

	std::string s_coin = numSegment( StringUtils::format("%lld",SINGLE_HERO->m_iCoin));
	std::string s_gold = numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iGold));
	auto currencyOwn=view->getChildByName("panel_currency_own");
	auto sizeWidth = Director::getInstance()->getWinSize().width;
	currencyOwn->setPositionX(950 - sizeWidth);
	auto buttonSilver=currencyOwn->getChildByName("button_silver");
	auto sliverNum=buttonSilver->getChildByName<Text*>("label_silver_num");
	sliverNum->setString(s_coin);
	auto buttonGold=currencyOwn->getChildByName("button_gold");
	auto goldNum=buttonGold->getChildByName<Text*>("label_gold_num");
	goldNum->setString(s_gold);

	auto viewButter = getViewRoot(BANK_COCOS_RES[VIEW_BANK_MAIN_CSB])->getChildByName("panel_charactor_two_butter");
	viewButter->setVisible(false);

}

void UIBank::updateOnlyCoinGold()
{
	if (getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]))
	{
		auto view = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
		auto panel_currency_own= view->getChildByName("panel_currency_own");
		auto buttonsilver=panel_currency_own->getChildByName("button_silver");
		auto label_silver_num=buttonsilver->getChildByName<Text*>("label_silver_num");
		label_silver_num->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iCoin)));

		auto buttongold=panel_currency_own->getChildByName("button_gold");
		auto label_gold_num=buttongold->getChildByName<Text*>("label_gold_num");
		label_gold_num->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iGold)));
		setTextFontSize(label_silver_num);
		setTextFontSize(label_gold_num);
	}
	if (getViewRoot(BANK_COCOS_RES[VIEW_SAFE_CSB]))
	{
		auto viewPanelSafe = getViewRoot(BANK_COCOS_RES[VIEW_SAFE_CSB]);
		auto buttonsilverSafe=viewPanelSafe->getChildByName("button_silver");
		auto label_silver_numsafe=buttonsilverSafe->getChildByName<Text*>("label_silver_num");
		label_silver_numsafe->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iCoin)));

		auto buttongoldSafe=viewPanelSafe->getChildByName("button_gold");
		auto label_gold_numSafe=buttongoldSafe->getChildByName<Text*>("label_gold_num");
		label_gold_numSafe->setString(numSegment(StringUtils::format("%lld",SINGLE_HERO->m_iGold)));
		setTextFontSize(label_silver_numsafe);
		setTextFontSize(label_gold_numSafe);
	}
	if (getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]))
	{
		auto insurance_panel = getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]);
		auto panel_currency_own = insurance_panel->getChildByName("panel_currency_own");
		auto buttonsilver = panel_currency_own->getChildByName("button_silver");
		auto label_silver_num = buttonsilver->getChildByName<Text*>("label_silver_num");
		label_silver_num->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iCoin)));

		auto buttongold = panel_currency_own->getChildByName("button_gold");
		auto label_gold_num = buttongold->getChildByName<Text*>("label_gold_num");
		label_gold_num->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iGold)));
		setTextFontSize(label_silver_num);
		setTextFontSize(label_gold_num);
	}
}
void UIBank::bankSliderClose(Ref*)
{
	closeView(COMMOM_COCOS_RES[C_VIEW_PROVIDE_SELECT_CSB]);
}
void UIBank::showBankNumPad()
{
	m_bNumpadFlag = false;
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
	auto panel_currency_own = view->getChildByName<Widget*>("panel_currency_own");
	panel_currency_own->setVisible(false);
	auto p_mask_numpad = view->getChildByName<Widget*>("panel_mask_numpad");
	p_mask_numpad->setPosition(Vec2(-335, -90));
	//显示银币V票
	auto t_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_mask_numpad, "label_silver_num"));
	auto t_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_mask_numpad, "label_gold_num"));
	t_silver_num->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iCoin)));
	t_gold_num->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iGold)));
	t_gold_num->setContentSize(Size(220, 34));
	t_silver_num->setContentSize(Size(220, 34));
	setTextFontSize(t_silver_num);
	setTextFontSize(t_gold_num);
	auto num_root = view->getChildByName<Widget*>("panel_numpad");
	num_root->setPosition(Vec2(-335, -270));
	num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, Vec2(-335, -90)),
		CallFunc::create(this, callfunc_selector(UIBank::moveEnd))));//键盘应该到的位置Vec2(-335, -90)
	auto b_yes = num_root->getChildByName<Button*>("button_numpad_yes");
	b_yes->addTouchEventListener(CC_CALLBACK_2(UIBank::menuCall_func,this));
	auto b_del = num_root->getChildByName<Button*>("button_del");
	b_del->addTouchEventListener(CC_CALLBACK_2(UIBank::bankNumpadEvent,this));
	p_mask_numpad->addTouchEventListener(CC_CALLBACK_2(UIBank::menuCall_func,this));
	for (size_t i = 0; i < 10; i++)
	{
		auto b_num = dynamic_cast<Button*>(Helper::seekWidgetByName(num_root, StringUtils::format("button_%d", i)));
		b_num->setTag(i);
		b_num->addTouchEventListener(CC_CALLBACK_2(UIBank::bankNumpadEvent,this));
	}
}
void UIBank::bankNumpadEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	auto target = dynamic_cast<Widget*>(pSender);
	std::string name = target->getName();
	
	if (isButton(button_del))//删除键
	{
		if (!m_padString.empty())
		{
			m_padString.erase(m_padString.end() - 1);
		}
	}
	else
	{
		int tag = target->getTag();

		if (m_padString.size() < 2 && m_padString == "0")
		{
			m_padString.erase(m_padString.end() - 1);
		}
		m_padString += StringUtils::format("%d", tag);
	}
	std::string str = numSegment(m_padString);
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
	auto viewInsure = getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_BUY_CSB]);
	if (view)
	{
		auto label_currency_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_currency_num"));
		label_currency_num->setString(str);
	}
	if (viewInsure)
	{
		auto label_insure_num = dynamic_cast<Text*>(Helper::seekWidgetByName(viewInsure, "label_insure_num"));
		label_insure_num->setString(str);
	}
	
}
void UIBank::update(float dt)
{
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
	if (view)
	{
		auto t_num_bg = view->getChildByName("image_input_bg");
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

void UIBank::opposedCity()
{
	auto viewMain = getViewRoot(BANK_COCOS_RES[VIEW_BANK_MAIN_CSB]);
	auto viewCurrcy = getViewRoot(BANK_COCOS_RES[VIEW_CURRENY_CSB]);
	if (viewMain)
	{
		auto subButton = viewMain->getChildByName("panel_charactor_two_butter");
		auto listview_two_butter = subButton->getChildByName("listview_two_butter");
		auto button_safe = listview_two_butter->getChildByName<Button*>("button_safe");
		auto label_safe = dynamic_cast<Text*>(Helper::seekWidgetByName(button_safe, "label_safe"));
		//苏丹国家银行处理
		if (SINGLE_HERO->m_iCurCityNation == 8)
		{
			if (m_pBankResult->usesudanprop)
			{
				m_bIsCanGotoSafe = true;
				label_safe->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
			}
			else
			{
				m_bIsCanGotoSafe = false;
				label_safe->setTextColor(Color4B::GRAY);
			}
		}
		else if (m_pBankResult->friendvalue < 0)
		{
			m_bIsCanGotoSafe = false;
			label_safe->setTextColor(Color4B::GRAY);
		}
		else
		{
			m_bIsCanGotoSafe = true;
			label_safe->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		}
	}
	if (viewCurrcy)
	{
		auto button_fee_info = dynamic_cast<Button*>(Helper::seekWidgetByName(viewCurrcy, "button_fee_info"));
		//问号按钮
		if (SINGLE_HERO->m_iCurCityNation == 8)
		{
			if (m_pBankResult->usesudanprop)
			{
				button_fee_info->setVisible(false);
			}
			else
			{
				button_fee_info->setVisible(false);
			}
		}
		else if (m_pBankResult->friendvalue < 0)
		{
			button_fee_info->setVisible(false);
		}
		else
		{
			button_fee_info->setVisible(false);
		}
	}
	
}
void UIBank::opposedCitySafeBtn()
{
	auto viewMain = getViewRoot(BANK_COCOS_RES[VIEW_BANK_MAIN_CSB]);
	auto subButton = viewMain->getChildByName("panel_charactor_two_butter");
	auto listview_two_butter = subButton->getChildByName("listview_two_butter");
	auto button_safe = listview_two_butter->getChildByName<Button*>("button_safe");
	auto label_safe = dynamic_cast<Text*>(Helper::seekWidgetByName(button_safe, "label_safe"));
	//苏丹国家银行处理
	if (SINGLE_HERO->m_iCurCityNation == 8)
	{
		if (m_pBankResult->usesudanprop)
		{
			button_safe->setBright(true);
			label_safe->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		}
		else
		{
			label_safe->setTextColor(Color4B::GRAY);
		}
	}
	else if (m_pBankResult->friendvalue < 0)
	{
		label_safe->setTextColor(Color4B::GRAY);
	}
	else
	{
		button_safe->setBright(true);
		label_safe->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}
}

void UIBank::enterInsuranceModule()
{
	openView(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]);
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]);
	//金银币
	updateOnlyCoinGold();

	auto panel_introduction = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_introduction"));
	auto panel_myinsurance = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_myinsurance"));
	//保险介绍的内容
	auto label_insurance_content = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_introduction, "label_insurance_content"));
	label_insurance_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_INSURANCE_INTRODUCTION_CONTENT"]);
	auto button_buy = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_introduction, "button_buy"));
	//如果已经投保且还没到期
	if (m_pBankResult->myinsurance || m_buyInsurenceResult)
	{
		panel_introduction->setVisible(false);
		panel_myinsurance->setVisible(true);
		showMyInsuranceInfo();
	}
	else
	{
		panel_introduction->setVisible(true);
		panel_myinsurance->setVisible(false);
		button_buy->setAnchorPoint(Vec2(0.5, 1));
		button_buy->setPositionY(label_insurance_content->getPositionY() - getLabelHight(label_insurance_content->getString(),
		label_insurance_content->getBoundingBox().size.width, label_insurance_content->getFontName())-20);
	}
}
void UIBank::openInsuranceInsure()
{
	m_padString.clear();
	openView(BANK_COCOS_RES[VIEW_BANK_INSURANCE_BUY_CSB]);
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_BUY_CSB]);

	auto listview_content = view->getChildByName<ListView*>("listview_content");
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - image_pulldown->getBoundingBox().size.width / 2+5);
	addListViewBar(listview_content, image_pulldown);

	auto panel_buy_goods = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_buy_goods"));
	//周期，比率,委托，默认选择第一个
	n_chooseRatio = 0;
	n_chooseRatiotime = 0;
	n_protectDelegate = 0;

	//offline
	auto panel_cover = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods, "panel_cover"));
	panel_cover->addTouchEventListener(CC_CALLBACK_2(UIBank::menuCall_func,this));
	auto checkbox_1 = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(panel_cover, "checkbox_1"));
	checkbox_1->addEventListenerCheckBox(this, checkboxselectedeventselector(UIBank::checkBoxCall_func));
	checkbox_1->setSelected(false);
	if (m_pBankResult->delegateactived)
	{
		checkbox_1->setTouchEnabled(true);
		checkbox_1->setBright(true);
		panel_cover->setTouchEnabled(false);
	}
	else
	{
		checkbox_1->setTouchEnabled(false);
		checkbox_1->setBright(false);
		panel_cover->setTouchEnabled(true);
	}
	//Max compensation
	auto panel_maxc = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods, "panel_maxc"));

	auto label_insure_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_maxc, "label_insure_num"));
	label_insure_num->setTouchEnabled(true);
	label_insure_num->addTouchEventListener(CC_CALLBACK_2(UIBank::menuCall_func,this));
	int64_t coins = ceil(SINGLE_HERO->m_iCoin*1.0 / 10000)*10000;
	if (coins<0)
	{
		coins = 0;
	}
	label_insure_num->setString(numSegment(StringUtils::format("%lld", coins)));
	//label_insure_num->setString(StringUtils::format("%lld", coins));
	m_padString = StringUtils::format("%lld", coins);

	//比率
	auto panel_ratio = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods, "panel_ratio"));
	auto button_ratio_left = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_ratio, "button_ratio_left"));
	auto button_ratio_right = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_ratio, "button_ratio_right"));
	button_ratio_left->addTouchEventListener(CC_CALLBACK_2(UIBank::insureBtn_func,this));
	button_ratio_right->addTouchEventListener(CC_CALLBACK_2(UIBank::insureBtn_func,this));
	button_ratio_left->setVisible(false);

	auto i_ratio = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ratio, "label_maxc_num"));
	
	std::string ratioContent = StringUtils::format("%d%%", m_insurance_item.at(n_chooseRatio)->insurepercent);
	i_ratio->setString(ratioContent);

	//周期
	auto panel_time = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods, "panel_time"));
	auto button_time_left = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_time, "button_time_left"));
	auto button_time_right = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_time, "button_time_right"));
	button_time_left->addTouchEventListener(CC_CALLBACK_2(UIBank::insureBtn_func,this));
	button_time_right->addTouchEventListener(CC_CALLBACK_2(UIBank::insureBtn_func,this));
	button_time_left->setVisible(false);
	auto i_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_time, "label_maxc_num"));
	std::string timeContent = StringUtils::format("%d", m_ratio_times[n_chooseRatiotime]) + " " + SINGLE_SHOP->getTipsInfo()["TIP_DAY"];
	i_time->setString(timeContent);
	//花费
	auto panel_cost = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_cost"));
	auto label_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_cost, "label_gold_num"));
	auto label_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_cost, "label_silver_num"));
	auto image_gold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_cost, "image_gold"));
	auto image_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_cost, "image_silver"));


	int64_t  insurance_count = atoll(m_padString.c_str());
	//按1亿为基础算的（系数/100000000*金额*天数）,以现在1天为基本计算单位N，3天为N1.2,7天为N1.5
	int64_t silver = 0;
	int64_t v_ticket = 0;
	//附加险
	int64_t delegateCost = 0;
	if (m_insurance_item[n_chooseRatio]->usevticket)
	{
		//100%的比率
		delegateCost = (m_insurance_item[n_chooseRatio]->protectdelegateprice*1.0 / 100000000)* insurance_count*m_ratio_times[n_chooseRatiotime];
		if (!n_protectDelegate)
		{
			delegateCost = 0;
		}
		silver = 0;
		v_ticket = ceil((m_insurance_item[n_chooseRatio]->feefactor*1.0 / 100000000)* insurance_count*m_ratio_times[n_chooseRatiotime] + delegateCost);

	}
	else
	{
		//70%的比率
		delegateCost = ceil((m_insurance_item[n_chooseRatio]->protectdelegateprice*1.0 / 100000000)* insurance_count*m_ratio_times[n_chooseRatiotime]);
		if (!n_protectDelegate)
		{
			delegateCost = 0;
		}
		silver = ceil((m_insurance_item[n_chooseRatio]->feefactor*1.0 / 100000000)* insurance_count*m_ratio_times[n_chooseRatiotime]);
		v_ticket = delegateCost;
	}
	
	label_gold_num->setString(StringUtils::format("%lld", v_ticket));
	label_silver_num->setString(StringUtils::format("%lld", silver));
	//保费
	n_insurance_coinCost =silver;
	n_insurance_VticketCost = v_ticket;
	//调整位置
	image_silver->setPositionX(label_silver_num->getPositionX() - label_silver_num->getContentSize().width - image_silver->getContentSize().width / 2 - 12);
	label_gold_num->setPositionX(image_silver->getPositionX() - 80);
	image_gold->setPositionX(label_gold_num->getPositionX() - label_gold_num->getContentSize().width - image_gold->getContentSize().width / 2 - 12);

}
void UIBank::showMyInsuranceInfo()
{
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]);

	if (view)
	{
		auto panel_myinsurance = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_myinsurance"));
		auto panel_introduction = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_introduction"));
		panel_myinsurance->setVisible(true);
		panel_introduction->setVisible(false);

		auto image_time_start = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_myinsurance, "image_time_start"));
		auto label_event_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(image_time_start, "label_event_0"));

		insuranceLeftTime(0);
		if (m_myInsurance->lefttimeinseconds <=0)
		{		
			std::string  leftTime = StringUtils::format("%d", 0) + SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_DAY_2"]
				+ " " + "00" + ":" + "00" + ":" + "00";
			label_event_0->setString(leftTime);		
		}

		auto panel_cover = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_myinsurance, "panel_cover"));
		auto checkbox_coverage = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(panel_cover, "checkbox_coverage"));
		checkbox_coverage->setTouchEnabled(false);
		if (m_myInsurance->protectdelegate)
		{
			checkbox_coverage->setSelected(true);
		}
		else
		{
			checkbox_coverage->setSelected(false);
		}

		auto panel_maxc = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_myinsurance, "panel_maxc"));
		auto image_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_maxc, "image_silver"));

		auto label_maxc_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_myinsurance, "label_maxc_num"));
		label_maxc_num->setString(numSegment(StringUtils::format("%lld", m_myInsurance->totalamount)));
		image_silver->setPositionX(label_maxc_num->getPositionX() - label_maxc_num->getContentSize().width-image_silver->getContentSize().width/2- 12);

		auto panel_compensated = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_myinsurance, "panel_compensated"));
		auto image_silver_compensated = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_compensated, "image_silver"));
		auto label_c_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_myinsurance, "label_c_num"));
		label_c_num->setString(numSegment(StringUtils::format("%lld", m_myInsurance->totalamount - m_myInsurance->leftamount)));
		image_silver_compensated->setPositionX(label_c_num->getPositionX() - label_c_num->getContentSize().width - image_silver_compensated ->getContentSize().width/2- 12);

		auto panel_each = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_myinsurance, "panel_each"));
		auto label_each_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_myinsurance, "label_each_num"));
		label_each_num->setString(StringUtils::format("%d%%", m_myInsurance->percent));
		
	}
}
void UIBank::insureBtn_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	auto target = dynamic_cast<Widget*>(pSender);
	auto name =target->getName();

	auto viewInsure = getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_BUY_CSB]);
	auto panel_buy_goods = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewInsure, "panel_buy_goods"));

	auto panel_ratio = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods, "panel_ratio"));
	auto button_ratio_left = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_ratio, "button_ratio_left"));
	auto button_ratio_right = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_ratio, "button_ratio_right"));

	auto panel_time = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods, "panel_time"));
	auto button_time_left = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_time, "button_time_left"));
	auto button_time_right = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_time, "button_time_right"));
	//周期左按钮
	if (isButton(button_ratio_left))
	{
		if (n_chooseRatio<=0)
		{
			return;
		}
		else
		{
			n_chooseRatio--;
			if (n_chooseRatio ==0)
			{
				button_ratio_left->setVisible(false);
			}
			button_ratio_right->setVisible(true);
			changeCostByRatioAndTime();
		}
		return;
	}
	//周期右按钮
	if (isButton(button_ratio_right))
	{
		if (n_chooseRatio >= m_insurance_item.size()-1)
		{
			return;
		}
		else
		{
			n_chooseRatio++;
			if (n_chooseRatio == m_insurance_item.size() - 1)
			{
				button_ratio_right->setVisible(false);
			}
			button_ratio_left->setVisible(true);
			changeCostByRatioAndTime();
		}
		return;
	}
	//时间左按钮
	if (isButton(button_time_left))
	{
		if (n_chooseRatiotime<=0)
		{
			return;
		}
		else
		{
			n_chooseRatiotime--;
			if (n_chooseRatiotime ==0)
			{
				button_time_left->setVisible(false);
			}
			button_time_right->setVisible(true);
			changeCostByRatioAndTime();
		}
		return;
	}
	//时间右按钮
	if (isButton(button_time_right))
	{
		if (n_chooseRatiotime>=m_ratio_times.size()-1)
		{
			return;
		}
		else
		{
			n_chooseRatiotime++;
			if (n_chooseRatiotime == m_ratio_times.size() - 1)
			{
				button_time_right->setVisible(false);
			}
			button_time_left->setVisible(true);
			changeCostByRatioAndTime();
		}
		return;
	}

}
void UIBank::checkBoxCall_func(Ref* target, CheckBoxEventType type)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (type == CheckBoxEventType::CHECKBOX_STATE_EVENT_SELECTED)
	{
		n_protectDelegate = 1;
	}
	else
	{
		n_protectDelegate = 0;
	}
	changeCostByRatioAndTime();
}
void UIBank::changeCostByRatioAndTime()
{
	
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_BUY_CSB]);
	auto panel_buy_goods = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_buy_goods"));
	
	auto panel_cover = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods, "panel_cover"));
	auto checkbox_1 = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(panel_cover, "checkbox_1"));

	//花费
	auto panel_cost = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_cost"));
	auto label_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_cost, "label_gold_num")) ;
	auto label_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_cost, "label_silver_num"));
	setTextFontSize(label_gold_num);
	setTextFontSize(label_silver_num);
	auto image_gold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_cost, "image_gold"));
	auto image_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_cost, "image_silver"));
	
	auto panel_maxc = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods, "panel_maxc"));

	auto label_insure_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_maxc, "label_insure_num"));
	
	int64_t insurance_count = atoll(m_padString.c_str());
	//按1亿为基础算的（系数/100000000*金额*天数）以现在1天为基本计算单位N，3天为N1.2,7天为N1.5
	int64_t silver = 0;
	int64_t v_ticket = 0;		
	//附加险
	int64_t delegateCost = 0;
	float multiple = 0;
	switch (m_ratio_times[n_chooseRatiotime])
	{
	case FIRST_PERIOD:
	multiple = 1.0;
	break;
	case SECOND_PERIOD:
	multiple = 1.2;
	break;
	case THIRD_PERIOD:
	multiple = 1.5;
	break;
	default:
	break;
	}
	if (m_insurance_item[n_chooseRatio]->usevticket)
	{
		//100%的比率
		delegateCost = (m_insurance_item[n_chooseRatio]->protectdelegateprice*1.0 / 100000000)* insurance_count*multiple;
		if (!n_protectDelegate)
		{
			delegateCost = 0;
		}
		silver = 0; 
		v_ticket = ceil((m_insurance_item[n_chooseRatio]->feefactor*1.0 / 100000000)* insurance_count*multiple + delegateCost);
	}
	else
	{
		//70%的比率
		delegateCost = ceil((m_insurance_item[n_chooseRatio]->protectdelegateprice*1.0 / 100000000)* insurance_count*multiple);
		if (!n_protectDelegate)
		{
			delegateCost = 0;
		}
		silver = ceil((m_insurance_item[n_chooseRatio]->feefactor*1.0 / 100000000)* insurance_count*multiple);
		v_ticket = delegateCost;
	}
	label_gold_num->setString(numSegment(StringUtils::format("%lld", v_ticket)));
	label_silver_num->setString(numSegment(StringUtils::format("%lld", silver)));
	label_silver_num->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	
	//保费
	n_insurance_coinCost = silver;
	n_insurance_VticketCost = v_ticket;
	//比率
	auto panel_ratio = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods, "panel_ratio"));
	auto button_ratio_left = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_ratio, "button_ratio_left"));
	auto button_ratio_right = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_ratio, "button_ratio_right"));

	auto i_ratio = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_ratio, "label_maxc_num"));
	std::string ratioContent = StringUtils::format("%d%%", m_insurance_item[n_chooseRatio]->insurepercent);
	i_ratio->setString(ratioContent);

	//周期
	auto panel_time = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods, "panel_time"));
	auto button_time_left = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_time, "button_time_left"));
	auto button_time_right = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_time, "button_time_right"));
	
	auto i_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_time, "label_maxc_num"));
	std::string timeContent = StringUtils::format("%d", m_ratio_times[n_chooseRatiotime]) + " " + SINGLE_SHOP->getTipsInfo()["TIP_DAY"];
	i_time->setString(timeContent);

	//调整位置
	image_silver->setPositionX(label_silver_num->getPositionX() - label_silver_num->getContentSize().width - image_silver->getContentSize().width / 2 - 12);
	label_gold_num->setPositionX(image_silver->getPositionX() - 80);
	image_gold->setPositionX(label_gold_num->getPositionX() - label_gold_num->getContentSize().width - image_gold->getContentSize().width / 2 - 12);
}
void UIBank::insuranceLeftTime(float t)
{
	//m_pBankResult->myinsurance->lefttimeinseconds--;
	m_myInsurance->lefttimeinseconds--;
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_CSB]);

	if (view)
	{
		auto panel_myinsurance = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_myinsurance"));
		auto panel_introduction = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_introduction"));
		auto image_time_start = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_myinsurance, "image_time_start"));
		auto label_event_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(image_time_start, "label_event_0"));
		auto button_plane_change = dynamic_cast<Button*>(Helper::seekWidgetByName(panel_myinsurance,"button_plane_change"));
		auto label_bank_savings = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_myinsurance, "label_bank_savings"));
		button_plane_change->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_BANK_INSURANCE_CHANGE_PLAN_TITLE_1"]);
		label_bank_savings->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_MY_INSURANCE_PANEL_TITLE_1"]);
		label_bank_savings->setTextColor(Color4B(46,125,50,255));
		
		if (m_myInsurance->lefttimeinseconds <0)
		{
			if (isScheduled(schedule_selector(UIBank::insuranceLeftTime)))
			{
				this->unschedule(schedule_selector(UIBank::insuranceLeftTime));
			}
			button_plane_change->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_BANK_INSURANCE_CHANGE_PLAN_TITLE_2"]);
			label_bank_savings->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_MY_INSURANCE_PANEL_TITLE_2"]);
			label_bank_savings->setTextColor(Color4B(183, 28, 28, 255));

			return;
		}
		else
		{
			if (!m_myInsurance->leftamount)
			{
				if (isScheduled(schedule_selector(UIBank::insuranceLeftTime)))
				{
					this->unschedule(schedule_selector(UIBank::insuranceLeftTime));
				}
				button_plane_change->setTitleText(SINGLE_SHOP->getTipsInfo()["TIP_BANK_INSURANCE_CHANGE_PLAN_TITLE_2"]);
				label_bank_savings->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_MY_INSURANCE_PANEL_TITLE_3"]);
				label_bank_savings->setTextColor(Color4B(183, 28, 28, 255));
				return;
			}
		}	
		//倒计时
		int64_t Days = m_myInsurance->lefttimeinseconds / (3600 * 24);
		int64_t Hours = (m_myInsurance->lefttimeinseconds) / 3600 % 24;
		int64_t Mins = (m_myInsurance->lefttimeinseconds / 60) % 60;
		int64_t Seconds = m_myInsurance->lefttimeinseconds % 60;

		std::string strHours = StringUtils::format("%02lld", Hours);;
		std::string strMins = StringUtils::format("%02lld", Mins);;
		std::string strSec = StringUtils::format("%02lld", Seconds);
	
		std::string  leftTime = StringUtils::format("%lld",Days)+SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_DAY_2"]
			+ " " + strHours + ":" + strMins + ":" + strSec;
		label_event_0->setString(leftTime);

		//本地时间
		auto panel_time = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_myinsurance, "panel_time"));
		auto label_time_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_time, "label_time_num"));
		std::string insuranceTime = "";
		struct tm *tm;
		time_t timep;
		time(&timep);
		tm = localtime(&timep);
		
		std::string time1 = StringUtils::format("%d-%d%d-%d%d", tm->tm_year + 1900, (tm->tm_mon + 1) / 10, (tm->tm_mon + 1) % 10, tm->tm_mday / 10, tm->tm_mday % 10);
		std::string time2 = StringUtils::format("%d%d:%d%d:%d%d", tm->tm_hour / 10, tm->tm_hour % 10, tm->tm_min / 10, tm->tm_min % 10, tm->tm_sec / 10, tm->tm_sec % 10);
	
		insuranceTime = time1 + "  " + time2;
		label_time_num->setString(insuranceTime);
	}
}
void UIBank::showMyInsuranceNumPad()
{
	m_bNumpadFlag = false;
	auto view = getViewRoot(BANK_COCOS_RES[VIEW_BANK_INSURANCE_BUY_CSB]);

	auto p_mask_numpad = view->getChildByName<Widget*>("panel_mask_numpad");
	p_mask_numpad->setPosition(Vec2(0, 0));
	//显示银币V票
	auto t_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_mask_numpad, "label_silver_num"));
	auto t_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(p_mask_numpad, "label_gold_num"));
	t_silver_num->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iCoin)));
	t_gold_num->setString(numSegment(StringUtils::format("%lld", SINGLE_HERO->m_iGold)));
	t_silver_num->setContentSize(Size(220, 34));
	t_gold_num->setContentSize(Size(220, 34));
	setTextFontSize(t_silver_num);
	setTextFontSize(t_gold_num);
	auto num_root = view->getChildByName<Widget*>("panel_numpad");
	num_root->setPosition(Vec2(0, -270));
	num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, Vec2(0, 0)),
		CallFunc::create(this, callfunc_selector(UIBank::moveEnd))));//键盘应该到的位置Vec2(-335, -90)
	auto b_yes = num_root->getChildByName<Button*>("button_numpad_yes");
	b_yes->addTouchEventListener(CC_CALLBACK_2(UIBank::menuCall_func,this));
	auto b_del = num_root->getChildByName<Button*>("button_del");
	b_del->addTouchEventListener(CC_CALLBACK_2(UIBank::bankNumpadEvent,this));
	p_mask_numpad->addTouchEventListener(CC_CALLBACK_2(UIBank::menuCall_func,this));
	for (size_t i = 0; i < 10; i++)
	{
		auto b_num = dynamic_cast<Button*>(Helper::seekWidgetByName(num_root, StringUtils::format("button_%d", i)));
		b_num->setTag(i);
		b_num->addTouchEventListener(CC_CALLBACK_2(UIBank::bankNumpadEvent,this));
	}
}
std::string UIBank::logItemName(std::string itemName)
{
	auto nameLen = itemName.length();
	LanguageType nType = LanguageType(Utils::getLanguage());
	switch (nType)
	{
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
	case cocos2d::LanguageType::CHINESE:
	{
		if (nameLen > 3 * 10)//utf8大多数一个汉字3字节
		{
			itemName = itemName.substr(0, 3 * 10) + "...";
		}
		break;
	}
	default:
	{
		if (!m_bIsMyBank)
		{
			if (nameLen >15)
			{
				itemName = itemName.substr(0, 15) + "...";
			}
		}
		else
		{
			if (nameLen > 30)
			{
				itemName = itemName.substr(0, 30) + "...";
			}
		}	
		break;
	}
 }
	return itemName;
}
