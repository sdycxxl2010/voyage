#include "UIGuideExchange.h"
#include "UICommon.h"
#include "UIInform.h"

#include "TVSceneLoader.h"
#include "UIVoyageSlider.h"

#define FEE 10000
float getAvePrice(PriceDataDefine **price, int64_t num, int64_t buy)
{
	if (price == nullptr || num == 0 || buy == 0)
	{
		return 0;
	}

	int64_t total = 0;
	int64_t temp = buy;
	int64_t diff = 0;
	for (int i = 0; i < num; ++i)
	{
		int64_t p = price[i]->price;
		int64_t c = price[i]->count;
		diff = temp - c;
		if (diff <= 0)
		{
			total += (temp * p);
			break;
		}
		else
		{
			temp = diff;
			total += (c * p);
		}
	}
	return (1.0 * total / buy);
}

int getMaxNum(PriceDataDefine **price, int num, int64_t coins, float tax)
{
	if (price == nullptr || num == 0 || coins == 0)
	{
		return 0;
	}

	int64_t total = 0;
	int	temp = 0;
	int maxNum = 0;
	for (auto i = 0; i < num; ++i)
	{
		total += ceil(1.0*(price[i]->count * price[i]->price)*(1 + tax));//税
		maxNum += price[i]->count;
	}
	if (total < coins)
	{
		return maxNum;
	}
	else
	{
		total = 0;
		maxNum = 0;
		for (int i = 0; i < num; i++)
		{
			if (price[i]->price > 0)
			{
				temp = 1.0 * (coins / (price[i]->price * (1 + tax)));
			}
			else
			{
				temp = 0;
			}

			if (temp > price[i]->count)
			{
				coins -= ceil(1.0 * price[i]->price * price[i]->count * (1 + tax));
			}
			else
			{
				break;
			}
			maxNum += price[i]->count;

		}
		return  maxNum + temp;
	}
}

UIGuideExchange::UIGuideExchange()
{
	m_vRoot = nullptr;
	m_pResult = nullptr;
	m_pLastPriceResult = nullptr;
	m_nItemIndex = 0;
    m_eUIType = UI_EXCHANGE;
	m_nPageIndex = -1;
	m_pMainButton = nullptr;
	m_pMinorButton = nullptr;
	m_pTempButton = nullptr;

	m_guideStage = 0;
	m_guideCount = 0;
	m_guideGoodType = -1;
	m_guideKinds = 0;
	m_guideId = -1;
	m_guidePrice = 0;
	m_guidePopular = -1;
	m_guideWeight = 0;
	m_costCoin = 0;
	m_confirmViewIndex = 0;
}

UIGuideExchange::~UIGuideExchange()
{
	unregisterCallBack();
	
}

UIGuideExchange* UIGuideExchange::createExchange()
{
	UIGuideExchange* ex = new UIGuideExchange;
	if (ex && ex->init())
	{
		ex->autorelease();
		return ex;
	}
	CC_SAFE_DELETE(ex);
	return nullptr;
}

bool UIGuideExchange::init()
{
	bool pRet = false;
	do
	{
		CC_BREAK_IF(!UIBasicLayer::init());
		registerCallBack();

		openView(MARKET_COCOS_RES[MARKET_CSB]);
		openView(MARKET_COCOS_RES[MARKET_BUY_CSB]);
		m_nPageIndex = PAGE_BUY_INDEX;
//		int stagelevel = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str());		
		//chengyuan++
		sprite_hand = Sprite::create();
		sprite_hand->setTexture("cocosstudio/login_ui/start_720/hand_icon.png");
		auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
		auto image_pulldown = Helper::seekWidgetByName(view, "image_pulldown");
		image_pulldown->setVisible(false);
		sprite_hand->setVisible(false);
		this->addChild(sprite_hand, 100);
		initLocalData();
		int stagelevel = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str());
		if (stagelevel < 4)
		{
			openGuideBuyView();
		}
		else
		{
			ProtocolThread::GetInstance()->getItemsBeingSold(ITEM_TYPE_GOOD, UILoadingIndicator::create(this));
		}
		changeMainButtonState(nullptr);
		changeMinorButtonState(nullptr);

		pRet = true;
	} while (0);


	

	return pRet;
}

void UIGuideExchange::confirmEvent()
{
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushPlayerAddExpOrFrame(SINGLE_HERO->m_iExp, SINGLE_HERO->m_iRexp, 0, 0);

	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto t_title = view->getChildByName<Text*>("label_title");
	auto p_result = view->getChildByName<Widget*>("panel_result");
	auto l_result = p_result->getChildByName<ListView*>("listview_result");
	auto p_buy = p_result->getChildByName<Widget*>("panel_buy_3");
	auto p_silver = p_result->getChildByName<Widget*>("panel_silver");
	auto i_div = p_result->getChildByName<ImageView*>("image_div_1");
	auto p_silver_1 = p_result->getChildByName<ImageView*>("panel_silver_2");
	auto p_buy_clone = p_buy->clone();
	auto t_buy = p_buy_clone->getChildByName<Text*>("label_buy");
	auto t_item = p_buy_clone->getChildByName<Text*>("label_items_name");
	auto t_buyNum = p_buy_clone->getChildByName<Text*>("label_buy_num");
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(ITEM_TYPE_GOOD, m_guideId, itemName, itemPath);
	int amount = m_guideCount;
	if (m_confirmViewIndex == EXCHAGE_CONFIRM_SELL_VIEW)
	{
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_RESULT_TITLE"]);
		t_buy->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_SELL_RESULT"]);
		t_item->setString(itemName);
		t_buyNum->setString(StringUtils::format("x %d", amount));
		t_item->setPositionX(t_buyNum->getPositionX() - t_buyNum->getContentSize().width - t_item->getContentSize().width / 2 - 6);
		l_result->pushBackCustomItem(p_buy_clone);
		auto i_div1 = i_div->clone();
		l_result->pushBackCustomItem(i_div1);
		//税
		float taxNum = m_guidePrice * m_tax * m_guideCount;
		//税后得到的银币
		float cost = m_guidePrice * m_guideCount - taxNum;
		auto p_tax = p_silver_1->clone();
		auto t_tax = p_tax->getChildByName<Text*>("label_force_relation");
		auto t_taxNum = p_tax->getChildByName<Text*>("label_buy_num");
		auto i_silver = p_tax->getChildByName<ImageView*>("image_silver");
		float m_taxshow = m_tax * 100;
		t_tax->setString(StringUtils::format("%s(%.2f%%):", SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(), m_taxshow));
		t_taxNum->setString(numSegment(StringUtils::format("%.f", taxNum)));
		i_silver->setPositionX(t_taxNum->getPositionX() - t_taxNum->getContentSize().width - i_silver->getContentSize().width / 2 - 6);
		l_result->pushBackCustomItem(p_tax);
		auto i_div2 = i_div->clone();
		l_result->pushBackCustomItem(i_div2);

		auto p_income = p_silver_1->clone();
		auto t_income = p_income->getChildByName<Text*>("label_force_relation");
		auto t_incomeNum = p_income->getChildByName<Text*>("label_buy_num");
		auto i_silver_1 = p_income->getChildByName<ImageView*>("image_silver");
		t_income->setString(StringUtils::format("%s:", SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_SELL_COST_MARK_TITLE"].data()));
		t_incomeNum->setString(numSegment(StringUtils::format("%.f", cost)));
		i_silver_1->setPositionX(t_incomeNum->getPositionX() - t_incomeNum->getContentSize().width - i_silver_1->getContentSize().width / 2 - 6);
		l_result->pushBackCustomItem(p_income);
		auto i_div3 = i_div->clone();
		l_result->pushBackCustomItem(i_div3);
		auto p_profit = p_silver_1->clone();
		auto t_profit = p_profit->getChildByName<Text*>("label_force_relation");
		auto t_profitNum = p_profit->getChildByName<Text*>("label_buy_num");
		auto i_silver_2 = p_profit->getChildByName<ImageView*>("image_silver");
		//收益
		float profitNum = cost - m_lastCityPrice *m_guideCount;
		t_profit->setString(StringUtils::format("%s:", SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_PROFIT"].data()));
		t_profitNum->setString(numSegment(StringUtils::format("%.f", profitNum)));
		i_silver_2->setPositionX(t_profitNum->getPositionX() - t_profitNum->getContentSize().width - i_silver_2->getContentSize().width / 2 - 6);
		l_result->pushBackCustomItem(p_profit);
		auto i_div4 = i_div->clone();
		l_result->pushBackCustomItem(i_div4);
	}
	else
	{
		t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_RESULT_TITLE"]);
		t_buy->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_BUY_RESULT"]);
		t_item->setString(itemName);
		t_buyNum->setString(StringUtils::format("x %d", amount));
		t_item->setPositionX(t_buyNum->getPositionX() - t_buyNum->getContentSize().width - t_item->getContentSize().width / 2 - 6);
		l_result->pushBackCustomItem(p_buy_clone);
		auto i_div1 = i_div->clone();
		l_result->pushBackCustomItem(i_div1);

		//税
		float taxNum = m_lastCityPrice * m_tax * m_guideCount;
		//税后得到的银币
		float cost = m_lastCityPrice * m_guideCount  + taxNum;
		auto p_tax = p_silver_1->clone();
		auto t_tax = p_tax->getChildByName<Text*>("label_force_relation");
		auto t_taxNum = p_tax->getChildByName<Text*>("label_buy_num");
		auto i_silver = p_tax->getChildByName<ImageView*>("image_silver");
		float m_taxshow = m_tax * 100;
		t_tax->setString(StringUtils::format("%s(%.2f%%):", SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(), m_taxshow));
		t_taxNum->setString(numSegment(StringUtils::format("%.f", taxNum)));
		i_silver->setPositionX(t_taxNum->getPositionX() - t_taxNum->getContentSize().width - i_silver->getContentSize().width / 2 - 6);
		l_result->pushBackCustomItem(p_tax);
		auto i_div2 = i_div->clone();
		l_result->pushBackCustomItem(i_div2);

		auto p_income = p_silver_1->clone();
		auto t_income = p_income->getChildByName<Text*>("label_force_relation");
		auto t_incomeNum = p_income->getChildByName<Text*>("label_buy_num");
		auto i_silver_1 = p_income->getChildByName<ImageView*>("image_silver");
		t_income->setString(StringUtils::format("%s:", SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_BUY_COST_MARK_TITLE"].data()));
		t_incomeNum->setString(numSegment(StringUtils::format("%.f", cost)));
		i_silver_1->setPositionX(t_incomeNum->getPositionX() - t_incomeNum->getContentSize().width - i_silver_1->getContentSize().width / 2 - 6);
		l_result->pushBackCustomItem(p_income);
		auto i_div3 = i_div->clone();
		l_result->pushBackCustomItem(i_div3);
	}





	auto b_yes = view->getChildByName<Button*>("button_result_yes");
	b_yes->addTouchEventListener(CC_CALLBACK_2(UIGuideExchange::closeBuySellSuccessView,this));

	guide();
}


void UIGuideExchange::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = (Widget*)pSender;
	int tag = button->getTag();
	std::string name = button->getName();
	if (m_pMainButton == button || m_pMinorButton == button || m_pTempButton == button)
	{
		return;
	}
	//确认卖出物品
	if (isButton(button_ok))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		closeView();
		confirmEvent();
//chengyuan++
		return;
	}
	else
	{

		if (isButton(button_sell))//交易所卖出物品
		{
			closeView();
			openView(MARKET_COCOS_RES[MARKET_SELL_CSB]);
			auto view = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]);
			auto image_pulldown = Helper::seekWidgetByName(view, "image_pulldown");
			image_pulldown->setVisible(false);
			m_pMinorButton = nullptr;
			changeMainButtonState(button);
			changeMinorButtonState(nullptr);
			m_nPageIndex = PAGE_SELL_INDEX;
			ProtocolThread::GetInstance()->getPersonalItems(ITEM_TYPE_GOOD, 1, UILoadingIndicator::create(this));
			guide();
			return;
		}

		//返回主城
		if (isButton(button_backmaincity))
		{
			CHANGETO(SCENE_TAG::MAINGUID_TAG);
			return;
		}
	}
}

void UIGuideExchange::sellBtnEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		//sound effect
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto target = dynamic_cast<Widget*>(pSender);
		sprite_hand->setVisible(false);
		openSellDialog();
		this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), CallFunc::create([=]{ guide();  })));
	}	
}

void UIGuideExchange::buyBtnEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		//sound effect
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto target = dynamic_cast<Widget*>(pSender);
		sprite_hand->setVisible(false);
		openBuyDialog();
		this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), CallFunc::create([=]{ guide();  })));
	}
}
void UIGuideExchange::openSellDialog()
{
	GetPersonalItemResult *result = (GetPersonalItemResult*)m_pResult;

	Node *panel_sellDialog = nullptr;
	bool ispopular = false;
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(result->itemtype, m_guideId, itemName, itemPath);

	if (result->itemtype == ITEM_TYPE_GOOD)
	{
		ispopular = m_guidePopular;
	}

	if (ispopular)
	{

	}
	else
	{
		openView(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODS_CSB]);
		panel_sellDialog = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODS_CSB]);
		auto panel_sellGoods = panel_sellDialog->getChildByName("panel_buy_goods");
		auto button_good = panel_sellGoods->getChildByName<Button*>("button_good_bg");
		auto image_goods = dynamic_cast<ImageView*>(button_good->getChildByName<Widget*>("image_goods"));
		auto text_name = dynamic_cast<Text*>(panel_sellGoods->getChildByName<Widget*>("label_name"));
		auto image_select_bg = panel_sellGoods->getChildByName("image_select_num_bg");
		auto text_price = panel_sellGoods->getChildByName<Text*>("label_price_num");

		image_goods->loadTexture(itemPath);
		text_name->setString(itemName);

		auto i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
		setBgButtonFormIdAndType(button_good, m_guideId, m_guideGoodType);
		setBgImageColorFormIdAndType(i_bgImage, m_guideId, m_guideGoodType);
		setTextColorFormIdAndType(text_name, m_guideId, m_guideGoodType);

		auto panel_select_num = image_select_bg->getChildByName<Widget*>("panel_select_num");
		int count = 0;
		if (m_guideGoodType== ITEM_TYPE_GOOD)
		{
			count = m_guideCount;
		}


		int index = 0;
		UIVoyageSlider* ss1 = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(101));
		if (ss1)
		{
			ss1->removeFromParentAndCleanup(true);
		}
		UIVoyageSlider* ss = UIVoyageSlider::create(panel_select_num, count, 0, true);
		ss->setCurrentNumber(m_guideCount);
		ss->setTag(101);
		ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIGuideExchange::sellingSliderEvent, this));
		this->addChild(ss);

		auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName((Widget*)panel_sellDialog, "image_skill_bg"));
		image_skill->ignoreContentAdaptWithSize(false);
		image_skill->loadTexture(getSkillIconPath(SKILL_TAX_PROTOCOL, SKILL_TYPE_PLAYER));
		image_skill->setVisible(true);
		image_skill->addTouchEventListener(CC_CALLBACK_2(UIGuideExchange::menuCall_func,this));
		int num = 0;
		image_skill->setVisible(false);

	}
}
void UIGuideExchange::openBuyDialog()
{
	GetPersonalItemResult *result = (GetPersonalItemResult*)m_pResult;

	Node *panel_sellDialog = nullptr;
	bool ispopular = false;
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(m_guideGoodType, m_guideId, itemName, itemPath);
	if (ispopular)
	{

	}
	else
	{
		openView(MARKET_COCOS_RES[MARKET_BUY_VIEWGOODS_CSB]);
		panel_sellDialog = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_VIEWGOODS_CSB]);
		auto panel_sellGoods = panel_sellDialog->getChildByName("panel_buy_goods");
		auto button_good = panel_sellGoods->getChildByName<Button*>("button_good_bg");
		auto image_goods = dynamic_cast<ImageView*>(button_good->getChildByName<Widget*>("image_goods"));
		auto text_name = dynamic_cast<Text*>(panel_sellGoods->getChildByName<Widget*>("label_goods_name"));
		auto image_select_bg = panel_sellGoods->getChildByName("image_select_num_bg");
		auto text_price = panel_sellGoods->getChildByName<Text*>("label_price_num");

		image_goods->loadTexture(itemPath);
		text_name->setString(itemName);

		auto i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
		setBgButtonFormIdAndType(button_good, m_guideId, m_guideGoodType);
		setBgImageColorFormIdAndType(i_bgImage, m_guideId, m_guideGoodType);
		setTextColorFormIdAndType(text_name, m_guideId, m_guideGoodType);

		auto panel_select_num = image_select_bg->getChildByName<Widget*>("panel_select_num");
		int count = 0;
		if (m_guideGoodType == ITEM_TYPE_GOOD)
		{
			count = m_guideCount;
		}


		int index = 0;
		UIVoyageSlider* ss1 = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(101));
		if (ss1)
		{
			ss1->removeFromParentAndCleanup(true);
		}
		UIVoyageSlider* ss = UIVoyageSlider::create(panel_select_num, count, 0, true);
		ss->setCurrentNumber(m_guideCount);
		ss->setTag(101);
		ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIGuideExchange::buyingSliderEvent, this));
		this->addChild(ss);

		auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName((Widget*)panel_sellDialog, "image_skill_bg"));
		image_skill->ignoreContentAdaptWithSize(false);
		image_skill->loadTexture(getSkillIconPath(SKILL_TAX_PROTOCOL, SKILL_TYPE_PLAYER));
		image_skill->setVisible(true);
		image_skill->addTouchEventListener(CC_CALLBACK_2(UIGuideExchange::menuCall_func,this));
		int num = 0;
		image_skill->setVisible(false);

	}
}
void UIGuideExchange::onServerEvent(struct ProtobufCMessage* message, int eMsgType)
{
	UIBasicLayer::onServerEvent(message, eMsgType);
	switch (eMsgType)
	{
	case PROTO_TYPE_GetItemsBeingSoldResult:
	{
											   GetItemsBeingSoldResult *result = (GetItemsBeingSoldResult*)message;
											   if (result->failed == 0)
											   {
												   m_pResult = message;
												   this->scheduleOnce(schedule_selector(UIGuideExchange::delayUpdateBuyView), 0.02f);
											   }
											   else
											   {
												   UIInform::getInstance()->openInformView(this);
												   UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_BUY_LIST_FAIL");
											   }
											   break;
	}
	case PROTO_TYPE_GetPersonalItemResult:
	{
											 GetPersonalItemResult* result = (GetPersonalItemResult*)message;
											 if (result->failed == 0)
											 {
												 m_pResult = message;
												 updateSellView((GetPersonalItemResult*)m_pResult);
											 }
											 else
											 {
												 UIInform::getInstance()->openInformView(this);
												 UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_SELL_LIST_FAIL");
											 }
											 break;
	}

	default:
		break;
	}
}

void UIGuideExchange::closeBuySellSuccessView(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		std::string name = dynamic_cast<Node*>(pSender)->getName();
		closeView();
		UICommon::getInstance()->closeView();
		if (isButton(button_result_yes))
		{
			guide();
			return;
		}
		
	}
}

void UIGuideExchange::sellingSliderEvent(const int64_t nNum)
{
      std::string itemName;
      std::string itemPath;
      getItemNameAndPath(m_guideGoodType, m_guideId, itemName, itemPath);

       auto w_goodsInfo = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODS_CSB]);
	   //滑动条显示当前物品所有数量
	   auto t_goonum = w_goodsInfo->getChildByName("image_select_num_bg")->getChildByName<Text*>("label_goods_num");
	   //税
	   auto t_pricenum = w_goodsInfo->getChildByName<Text*>("panel_tax_num")->getChildByName<Text*>("label_price_num");
	   //税率
	   auto t_tax = w_goodsInfo->getChildByName<Text*>("label_tax");
	   //本金
	   auto t_costnum= w_goodsInfo->getChildByName("panel_cost_num")->getChildByName<Text*>("label_cost_num");
	   auto t_incom = w_goodsInfo->getChildByName("image_select_num_bg")->getChildByName<Text*>("label_cost"); 
	   //收益
	   auto t_profitnum = w_goodsInfo->getChildByName("panel_profit_num")->getChildByName<Text*>("label_price_num");
	   //一共卖出多少钱
	   auto t_cost = w_goodsInfo->getChildByName("panel_cost_num")->getChildByName<Text*>("label_cost_num");  
	   auto h_income = w_goodsInfo->getChildByName("image_select_num_bg")->getChildByName<Text*>("label_goods_num");
	   auto t_profit = w_goodsInfo->getChildByName("panel_profit_num")->getChildByName<Text*>("label_price_num");
	   auto lv_price = w_goodsInfo->getChildByName<ListView*>("listview_price");
	   lv_price->removeAllChildrenWithCleanup(true);
	   auto    i_down = dynamic_cast<Widget*>(w_goodsInfo->getChildByName("panel_down"));
	   auto	i_up = dynamic_cast<Widget*>(w_goodsInfo->getChildByName("panel_up"));
	   auto	i_draw = dynamic_cast<Widget*>(w_goodsInfo->getChildByName("panel_draw"));
	   auto	i_goldcoin = dynamic_cast<ImageView*>(w_goodsInfo->getChildByName("image_goldcoin_1"));
	   auto	t_price = dynamic_cast<Text*>(w_goodsInfo->getChildByName("label_price_num"));
	   auto	b_price = dynamic_cast<Button*>(w_goodsInfo->getChildByName("button_price_info"));

	   auto i_coin_clone = i_goldcoin->clone();
	   lv_price->pushBackCustomItem(i_coin_clone);
	   auto t_price_clone = t_price->clone();
	   ((Text*)t_price_clone)->ignoreContentAdaptWithSize(true);
	   ((Text*)t_price_clone)->setString(StringUtils::format("%.0f", m_guidePrice));
	   lv_price->pushBackCustomItem(t_price_clone);

	   //税
	   float tax = m_guidePrice * m_tax * m_guideCount;
	   //税后得到的银币
	   float cost = m_guidePrice * m_guideCount - tax;
	   t_goonum->setText(StringUtils::format("%d",m_guideCount));
	   t_pricenum->setText(numSegment(StringUtils::format("%.f",tax)));
	   t_costnum->setText(numSegment(StringUtils::format("%.f",cost)));
	   auto image_silver = Helper::seekWidgetByName(w_goodsInfo, "image_silver");
	   t_profitnum->setText(numSegment(StringUtils::format("%.f", cost - (m_lastCityPrice*m_guideCount))));
	   image_silver->setPositionX(t_costnum->getPositionX() - t_costnum->getContentSize().width - image_silver->getContentSize().width / 2 - 5);
	   int  riseRate =int( ((m_guidePrice - m_lastCityPrice) / m_lastCityPrice) *100);
	   auto i_down_clone = i_up->clone();
	   std::string s_price = StringUtils::format("%d%%", int(riseRate));
	   Text *t_text = i_down_clone->getChildByName<Text*>("label_down_num");
	   t_text->setString(s_price);
	   lv_price->pushBackCustomItem(i_down_clone);
	   //税率
	   float m_taxshow = m_tax * 100;
	   std::string  s_tax_title = StringUtils::format("%s(%.2f%%):", SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(), m_taxshow);
	   t_tax->setText(s_tax_title);
	   m_receiveCoin = (int)(cost);
	   m_confirmViewIndex = EXCHAGE_CONFIRM_SELL_VIEW;
}

void UIGuideExchange:: buyingSliderEvent(const int64_t nNum)
{
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(m_guideGoodType, m_guideId, itemName, itemPath);

 	auto w_goodsInfo = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_VIEWGOODS_CSB]);
	auto p_buyinggoods = w_goodsInfo->getChildByName<Widget*>("panel_buying_goods");
	auto l_buygoods = w_goodsInfo->getChildByName<ListView*>("listview_buy_goods");
	auto p_buygoods = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_buyinggoods, "panel_buy_goods"));
	auto lv_price = dynamic_cast<ListView*>(Helper::seekWidgetByName(p_buygoods, "listView_price"));
	auto	t_price = dynamic_cast<Text*>(p_buygoods->getChildByName("label_price_num"));
	//单价
	auto    t_goodPrice = lv_price->getChildByName("label_price_num");
	//重量
	auto    t_weight = lv_price->getChildByName("label_weight_num");
	auto i_costBg = p_buygoods->getChildByName<Widget*>("label_weight_num");
	auto p_shipweight = dynamic_cast<Widget*>(Helper::seekWidgetByName(p_buyinggoods, "panel_ship_weight"));
	auto t_shipnum1 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_shipweight, "label_shipcapacity_status_num_1"));
	auto t_shipnum2 = dynamic_cast<Text*>(Helper::seekWidgetByName(p_shipweight, "label_shipcapacity_status_num_3"));
	auto p_bar = Helper::seekWidgetByName(p_shipweight, "progressbar_weight");
	auto p_baradd = Helper::seekWidgetByName(p_shipweight, "progressbar_weight_add");

	auto t_tax2 = Helper::seekWidgetByName(p_buygoods, "label_tax_2");
	auto t_goodsnum = Helper::seekWidgetByName(p_buygoods, "label_goods_num");
	auto t_labeltax = Helper::seekWidgetByName(p_buygoods, "label_tax");
	auto p_taxnum = Helper::seekWidgetByName(p_buygoods, "panel_tax_num");
	auto t_labeltaxnum = Helper::seekWidgetByName(p_taxnum, "label_price_num");
	auto t_costnum = Helper::seekWidgetByName(p_buygoods, "label_cost_num");
	auto i_pulldown = Helper::seekWidgetByName(p_buyinggoods, "image_pulldown");
	dynamic_cast<Text*>(t_goodPrice)->setString(StringUtils::format("%.f",m_lastCityPrice));
	dynamic_cast<Text*>(t_weight)->setString(StringUtils::format("%d", m_guideWeight));
	t_tax2->setVisible(false);
	float m_taxshow = m_tax * 100;
	float n_tax =  m_lastCityPrice * m_tax * m_guideCount;
	std::string  s_tax_title = StringUtils::format("%s(%.2f%%):", SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(), m_taxshow);
	t_shipnum1->setString("50 ");
	t_shipnum2->setString("/ 200");
	t_shipnum1->setPositionX(t_shipnum1->getPositionX() + 20);
	dynamic_cast<Text*>(t_labeltax)->setString(s_tax_title);
	dynamic_cast<Text*>(t_labeltaxnum)->setString(StringUtils::format("%.f",n_tax));
	dynamic_cast<Text*>(t_goodsnum)->setString(StringUtils::format("X%d", m_guideCount));
	float n_costnum = m_guideCount * m_lastCityPrice + n_tax;
	dynamic_cast<Text*>(t_costnum)->setString(numSegment(StringUtils::format("%.f", n_costnum)));
	dynamic_cast<LoadingBar*>(p_bar)->setVisible(false);
	dynamic_cast<LoadingBar*>(p_baradd)->setPercent(25);
	i_pulldown->setVisible(false);
	m_costCoin = int(n_costnum);
	m_confirmViewIndex = EXCHAGE_CONFIRM_BUY_VIEW;
}

template<class T>
void UIGuideExchange::updateView(const T *result)
{
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
	auto goods_list = view->getChildByName<ListView*>("listview_goods");
	auto item_0 = goods_list->getItem(0);
	auto item = item_0->clone();
	goods_list->removeAllItems();
	item->setTouchEnabled(true);
	item->setVisible(true);
	updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	if (result->n_items == 0)
	{
		item->setVisible(false);
		item->setTouchEnabled(false);
		goods_list->pushBackCustomItem(item);
		button_pulldown->setVisible(false);
		return;
	}
	for (int i = 0; i < result->n_items; ++i)
	{
		auto	goodsItem = item->clone();
		auto    button_good = goodsItem->getChildByName<Button*>("button_good_bg");
		auto    i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
		auto	img_icon = button_good->getChildByName<ImageView*>("image_goods");
		auto	price = goodsItem->getChildByName<Text*>("label_goods_price");
		auto	t_name = goodsItem->getChildByName<Text*>("label_goods_name");
		auto	t_amount = goodsItem->getChildByName<Text*>("label_goods_amount");
		auto	t_weight = goodsItem->getChildByName<Text*>("label_goods_weight");
		auto    i_weight = goodsItem->getChildByName<Widget*>("image_weight");
		auto    i_div = goodsItem->getChildByName<ImageView*>("image_div_2");
		auto    i_lvBg = goodsItem->getChildByName<ImageView*>("image_lv_bg");
		i_lvBg->setVisible(false);
		std::string itemName;
		std::string itemPath;
		getItemNameAndPath(result->itemtype, result->items[i]->itemid, itemName, itemPath);
		img_icon->ignoreContentAdaptWithSize(false);
		img_icon->loadTexture(itemPath);

		goodsItem->setName(String::createWithFormat("button_item_%d", i)->getCString());
		goodsItem->setTag(i);

		setBgButtonFormIdAndType(button_good, result->items[i]->itemid, result->itemtype);
		setBgImageColorFormIdAndType(i_bgImage, result->items[i]->itemid, result->itemtype);
		setTextColorFormIdAndType(t_name, result->items[i]->itemid, result->itemtype);
		button_good->addTouchEventListener(CC_CALLBACK_2(UIGuideExchange::menuCall_func,this));
		price->setString(StringUtils::format("%lld", result->items[i]->lastprice));
		t_name->setString(itemName);
		if (result->itemtype == ITEM_TYPE_SHIP || result->itemtype == ITEM_TYPE_DRAWING)
		{
			i_weight->setVisible(false);
			t_weight->setVisible(false);
			i_div->setVisible(false);
		}
		else
		{
			GetItemsBeingSoldResult *buyResult = (GetItemsBeingSoldResult *)m_pResult;
			i_weight->setVisible(true);
			t_weight->setVisible(true);
			t_weight->setString(StringUtils::format("%.2f", 1.0 * buyResult->items[i]->weight / 100));
			i_div->setVisible(true);
		}
		goods_list->pushBackCustomItem(goodsItem);
	}
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(goods_list, image_pulldown);
	guide();
}

void UIGuideExchange::updateCoinNum(const int64_t coin, const int64_t silver)
{
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_CSB]);
	auto i_bg = view->getChildByName("panel_currency");
	auto btn_coin = i_bg->getChildByName("button_silver");
	auto btn_gold = i_bg->getChildByName("button_gold");
	auto t_coin = dynamic_cast<Text*>(btn_gold->getChildByName<Widget*>("label_gold_num"));
	auto t_silver = dynamic_cast<Text*>(btn_coin->getChildByName<Widget*>("label_silver_num"));
	t_coin->setString(numSegment(StringUtils::format("%lld", coin)));
	t_silver->setString(numSegment(StringUtils::format("%lld", silver)));
}

void UIGuideExchange::delayUpdateBuyView(const float fTime)
{
	
	updateBuyView((GetItemsBeingSoldResult*)m_pResult);
}

void UIGuideExchange::updateBuyView(const GetItemsBeingSoldResult* result)
{
	if (m_vRoot == nullptr)
	{
		//initResult(0);
	}
	updateView<GetItemsBeingSoldResult>(result);
}

void UIGuideExchange::updateSellView(const GetPersonalItemResult* result)
{
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]);
	auto goods_list = view->getChildByName<ListView*>("listview_goods");
	auto item_0 = goods_list->getItem(0);
	auto item = item_0->clone();
	goods_list->removeAllItems();
	item->setTouchEnabled(true);
	item->setVisible(true);
	updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);
	int n_items = 0;
	switch (m_guideGoodType)
	{
	case ITEM_TYPE_GOOD:
	{
						   n_items = m_guideKinds;
						   break;
	}
	default:
		break;
	}

	if (n_items == 0)
	{
		item->setVisible(false);
		item->setTouchEnabled(false);
		goods_list->pushBackCustomItem(item);
		auto scrollView = Helper::seekWidgetByName(view, "button_pulldown");
		return;
	}

	for (int i = 0; i < n_items; ++i)
	{
		auto	goodsItem = item->clone();
		auto	button_good = goodsItem->getChildByName<Button*>("button_good_bg");
		auto	i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
		auto	img_icon = button_good->getChildByName<ImageView*>("image_goods");
		auto	t_price = goodsItem->getChildByName<Text*>("label_goods_price");
		auto	t_name = goodsItem->getChildByName<Text*>("label_goods_name");
		auto	t_amount = goodsItem->getChildByName<Text*>("label_goods_amount");
		auto	t_weight = goodsItem->getChildByName<Text*>("label_goods_weight");

		std::string itemName;
		std::string itemPath;
		int itemid = 0;
		int64_t price = 0;
		int64_t amount = 0;

//chengyuan++
		
		auto v = goodsItem->getChildByName<Widget*>("image_r");
		if (m_guideGoodType == ITEM_TYPE_GOOD)
		{
			itemid = m_guideId;
			itemName = getGoodsName(itemid);
			itemPath = getGoodsIconPath(itemid, IMAGE_ICON_OUTLINE);
			price = m_guidePrice;
			amount = m_guideCount;
			if (m_guidePopular == 0)
			{
				v->setVisible(false);
			}
			else
			{
				v->setVisible(true);
			}
		}
		img_icon->ignoreContentAdaptWithSize(false);
		img_icon->loadTexture(itemPath);

		goodsItem->setName(String::createWithFormat("button_item_%d", i)->getCString());
		goodsItem->setTag(i);

		setBgButtonFormIdAndType(button_good, itemid, result->itemtype);
		setBgImageColorFormIdAndType(i_bgImage, itemid, result->itemtype);
		setTextColorFormIdAndType(t_name, itemid, result->itemtype);

		button_good->addTouchEventListener(CC_CALLBACK_2(UIGuideExchange::menuCall_func,this));
		t_price->setString(String::createWithFormat("%lld", price)->getCString());
		t_name->setString(itemName);
		goods_list->pushBackCustomItem(goodsItem);

		t_amount->setString(String::createWithFormat("x %lld", amount)->_string);
		goodsItem->addTouchEventListener(CC_CALLBACK_2(UIGuideExchange::sellBtnEvent,this));

	}
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	addListViewBar(goods_list, image_pulldown);
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2 + 3);
}

void UIGuideExchange::changeMainButtonState(Widget *target)
{
	if (!target)
	{
		auto view = getViewRoot(MARKET_COCOS_RES[MARKET_CSB]);
		target = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "button_buy"));
	}

	if (m_pMainButton)
	{
		m_pMainButton->setBright(true);
		dynamic_cast<Text*>(m_pMainButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_NORMAL);
	}
	m_pMainButton = target;
	m_pMainButton->setBright(false);
	dynamic_cast<Text*>(m_pMainButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_PASSED);
}

void UIGuideExchange::changeMinorButtonState(Widget *target)
{
	if (!target)
	{
		auto view1 = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
		if (view1)
		{
			target = dynamic_cast<Widget*>(Helper::seekWidgetByName(view1, "button_goods"));
			auto b_info = view1->getChildByName<Button*>("button_info");
			auto listview_two_butter = dynamic_cast<ListView*>(Helper::seekWidgetByName(view1, "listview_two_butter"));
			listview_two_butter->removeLastItem();
			listview_two_butter->removeLastItem();
		}
		auto view2 = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]);
		if (view2)
		{
			target = dynamic_cast<Widget*>(Helper::seekWidgetByName(view2, "button_goods"));
			auto b_info = view2->getChildByName<Button*>("button_info");
			auto listview_two_butter = dynamic_cast<ListView*>(Helper::seekWidgetByName(view2, "listview_two_butter"));
			listview_two_butter->removeLastItem();
			listview_two_butter->removeLastItem();
		}
		auto view3 = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_CSB]);
		if (view3)
		{
			target = dynamic_cast<Widget*>(Helper::seekWidgetByName(view3, "button_order_buy"));
		}
	}

	if (m_pMinorButton)
	{
		m_pMinorButton->setBright(true);
		dynamic_cast<Text*>(m_pMinorButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}
	m_pMinorButton = target;
	m_pMinorButton->setBright(false);
	dynamic_cast<Text*>(m_pMinorButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
}

void UIGuideExchange::guide()
{
	m_guideStage++;
	int stagelevel = UserDefault::getInstance()->getIntegerForKey(ProtocolThread::GetInstance()->getFullKeyName("STAGELEVEL").c_str());

	if (stagelevel < 4)
	{
		partOneGuide(m_guideStage);
		return;
	}
	switch (m_guideStage)
	{
	case SELL_TASK_BEGIN:
	{

							auto view = getViewRoot(MARKET_COCOS_RES[MARKET_CSB]);
							auto btn_sell = view->getChildByName("button_sell");
							focusOnButton(btn_sell);
							for (auto root : m_vRoots)
							{
								setButtonsDisable(root, "button_sell");
								if (root->getChildren().at(0)->getName().find("panel_buy") == 0)
								{
									root->getChildren().at(0)->getChildByName<ListView*>("listview_two_butter")->setDirection(SCROLLVIEW_DIR_NONE);
									root->getChildren().at(0)->getChildByName<ListView*>("listview_goods")->setDirection(SCROLLVIEW_DIR_NONE);
								}

							}


	}
		break;
	case CLICK_BTN_SELL:
	{
						   for (auto root : m_vRoots)
						   {

							   if (root->getChildren().at(0)->getName().find("panel_sell") == 0)
							   {
								   root->getChildren().at(0)->getChildByName<ListView*>("listview_two_butter")->setDirection(SCROLLVIEW_DIR_NONE);
								   root->getChildren().at(0)->getChildByName<ListView*>("listview_goods")->setDirection(SCROLLVIEW_DIR_NONE);
								   setButtonsDisable(root);
								   auto btn = root->getChildren().at(0)->getChildByName<ListView*>("listview_goods")->getItem(0);
								   dynamic_cast<Widget*>(btn)->setTouchEnabled(true);
								   focusOnButton(btn);
							   }
							   else
							   {
								   setButtonsDisable(root);
							   }

						   }
						   updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);
	}

		break;
	case FOUC_SELL_GOODS:
	{
							for (auto root : m_vRoots)
							{

								if (root->getChildren().at(0)->getName().find("panel_sell_goods") == 0)
								{
									root->getChildren().at(0)->getChildByName<ListView*>("listview_price")->setDirection(SCROLLVIEW_DIR_NONE);
									setButtonsDisable(root);
									auto btn = root->getChildren().at(0)->getChildByName("button_ok");
									dynamic_cast<Widget*>(btn)->setTouchEnabled(true);
									auto s_slidergood = root->getChildren().at(0)->getChildByName<Slider*>("slider_goods_num");
									s_slidergood->setTouchEnabled(false);
									s_slidergood->setPercent(100);
									focusOnButton(btn);
								}
								else
								{
									setButtonsDisable(root);
								}

							}
							updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);
	}
		break;
	case CONFIRM_SELL_GOODS:
	{
							   auto view = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]);
							   auto l_sell_view = view->getChildByName<ListView*>("listview_goods");
							   l_sell_view->removeAllChildrenWithCleanup(true);
							   for (auto root : m_vRoots)
							   {
								   setButtonsDisable(root, "button_result_yes");
								   if (root->getChildren().at(0)->getName().find("panel_float_result") == 0)
								   {
									   auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
									   auto btn = Helper::seekWidgetByName(view, "button_result_yes");
									   focusOnButton(btn);
								   }
							   }
							   updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);
	}
		break;
	case BACK_TO_CITY:
	{

						 for (auto root : m_vRoots)
						 {
							 setButtonsDisable(root, "button_backmaincity");
							 if (root->getChildren().at(0)->getName().find("panel_buy") == 0)
							 {
								 root->getChildren().at(0)->getChildByName<ListView*>("listview_two_butter")->setDirection(SCROLLVIEW_DIR_NONE);
								 root->getChildren().at(0)->getChildByName<ListView*>("listview_goods")->setDirection(SCROLLVIEW_DIR_NONE);
							 }
						 }
						 auto root = getViewRoot(MARKET_COCOS_RES[MARKET_CSB]);
						 auto btn_back = root->getChildByName("button_backmaincity");
						 SINGLE_HERO->m_iSilver += m_receiveCoin;
						 updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);
						 focusOnButton(btn_back);
						 SINGLE_HERO->m_iStage = 5;
	}
	default:
		break;
	}
}
void UIGuideExchange::partOneGuide(int guidestage)
{
	switch (guidestage)
	{
	case CLICK_BTN_BUY:
	{
						  for (auto root : m_vRoots)
						  {

							  if (root->getChildren().at(0)->getName().find("panel_buy") == 0)
							  {
								  root->getChildren().at(0)->getChildByName<ListView*>("listview_two_butter")->setDirection(SCROLLVIEW_DIR_NONE);
								  root->getChildren().at(0)->getChildByName<ListView*>("listview_goods")->setDirection(SCROLLVIEW_DIR_NONE);
								  setButtonsDisable(root);
								  auto btn = root->getChildren().at(0)->getChildByName<ListView*>("listview_goods")->getItem(0);
								  dynamic_cast<Widget*>(btn)->setTouchEnabled(true);
								  focusOnButton(btn);
							  }
							  else
							  {
								  setButtonsDisable(root);
							  }

						  }
						  updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);
	}
		break;
	case  FOUC_BUY_GOODS:
	{
							for (auto root : m_vRoots)
							{

								if (root->getChildren().at(0)->getName().find("panel_buying_goods") == 0)
								{
									root->getChildren().at(0)->getChildByName<ListView*>("listview_buy_goods")->setDirection(SCROLLVIEW_DIR_NONE);
									setButtonsDisable(root);
									auto btn = root->getChildren().at(0)->getChildByName("button_ok");
									dynamic_cast<Widget*>(btn)->setTouchEnabled(true);
									auto s_slidergood = root->getChildren().at(0)->getChildByName<Slider*>("slider_goods_num");
									s_slidergood->setTouchEnabled(false);
									s_slidergood->setPercent(100);
									focusOnButton(btn);
								}
								else
								{
									setButtonsDisable(root);
								}

							}
							updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);
	}
		break;
	case  CONFIRM_BUY_GOODS:
	{
							   auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
// 							   auto l_sell_view = view->getChildByName<ListView*>("listview_goods");
// 							   l_sell_view->removeAllChildrenWithCleanup(true);
							   for (auto root : m_vRoots)
							   {
								   setButtonsDisable(root, "button_result_yes");
								   if (root->getChildren().at(0)->getName().find("panel_float_result") == 0)
								   {
									   auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
									   auto btn = Helper::seekWidgetByName(view, "button_result_yes");
									   focusOnButton(btn);
								   }
							   }
							   updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);
	}
		break;
	case BUY_BACK_TO_CITY:
	{
							 for (auto root : m_vRoots)
							 {
								 setButtonsDisable(root, "button_backmaincity");
								 if (root->getChildren().at(0)->getName().find("panel_buy") == 0)
								 {
									 root->getChildren().at(0)->getChildByName<ListView*>("listview_two_butter")->setDirection(SCROLLVIEW_DIR_NONE);
									 root->getChildren().at(0)->getChildByName<ListView*>("listview_goods")->setDirection(SCROLLVIEW_DIR_NONE);
								 }
							 }
							 auto root = getViewRoot(MARKET_COCOS_RES[MARKET_CSB]);
							 auto btn_back = root->getChildByName("button_backmaincity");
							 SINGLE_HERO->m_iSilver -= m_costCoin;
							 updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);
							 focusOnButton(btn_back);					 
							 SINGLE_HERO->m_iStage = 3;
	}
		break;
	default:
		break;
	}
}


void UIGuideExchange::focusOnButton(Node * pSender)
{
	
	sprite_hand->setVisible(true);
	//	sprite_hand->setAnchorPoint(Vec2(0, 1));
	auto c_psender = dynamic_cast<Widget*>(pSender);
	Size win = _director->getWinSize();
	Vec2 pos = c_psender->getWorldPosition();
	Size cut_hand = sprite_hand->getContentSize() / 2;
	Size cut_psendet = pSender->getContentSize() / 2;
	std::string name = dynamic_cast<Widget*>(pSender)->getName();
	if (pos.x < win.width / 2 && pos.y > win.height / 2)
	{
		if (name.compare("button_backmaincity")==0)
		{
			sprite_hand->setRotation(-180);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
		}
		else
		{
			sprite_hand->setRotation(-180);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + c_psender->getContentSize().width/2,c_psender->getWorldPosition().y));
		}
	}
	else if (pos.x > win.width / 2 && pos.y > win.height / 2)
	{
		sprite_hand->setRotation(-180);
		sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
			c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
	}
	else if (pos.x < win.width / 2 && pos.y < win.height / 2)
	{
		if (name.compare("button_result_yes") == 0)
		{
			sprite_hand->setRotation(-180);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y - c_psender->getContentSize().height / 2 * 0.6 - sprite_hand->getContentSize().height / 2 * 0.6));
		}
		else
		{
			sprite_hand->setRotation(70);
			sprite_hand->setPosition(Vec2(c_psender->getWorldPosition().x + sprite_hand->getContentSize().width / 2,
				c_psender->getWorldPosition().y + c_psender->getContentSize().height / 2 * 0.6 + sprite_hand->getContentSize().height / 2 * 0.6));
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

void UIGuideExchange::setButtonsDisable(Node * node, std::string btn_name)
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

void UIGuideExchange::initLocalData()
{
	m_guideCount = 10;
	m_guideGoodType = 0;
	m_guideKinds = 1;
	m_guidePrice = 200;
	m_guidePopular = 0;
	m_lastCityPrice = 100;
	m_tax = 0.05;
	m_guideWeight = 5;
	int nation = SINGLE_HERO->m_iNation;
	switch (nation)
	{
	case 1:
		m_guideId = 2;
		break;
	case 2:
		m_guideId = 76;
		break;
	case 3:
		m_guideId = 5;
		break;
	case 4:
		m_guideId = 79;
		break;
	case 5:
		m_guideId = 13;
		break;
	default:
		break;
	}
}
void UIGuideExchange::openGuideBuyView()
{
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
	auto goods_list = view->getChildByName<ListView*>("listview_goods");
	auto item_0 = goods_list->getItem(0);
	auto item = item_0->clone();
	goods_list->removeAllItems();
	item->setTouchEnabled(true);
	item->setVisible(true);
	updateCoinNum(SINGLE_HERO->m_iVp, SINGLE_HERO->m_iSilver);

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	int n_items = 1;
// 	if (result->n_items == 0)
// 	{
// 		item->setVisible(false);
// 		item->setTouchEnabled(false);
// 		goods_list->pushBackCustomItem(item);
// 		button_pulldown->setVisible(false);
// 		return;
// 	}
	for (int i = 0; i < n_items; ++i)
	{
		auto	goodsItem = item->clone();
		auto    button_good = goodsItem->getChildByName<Button*>("button_good_bg");
		auto    i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
		auto	img_icon = button_good->getChildByName<ImageView*>("image_goods");
		auto	price = goodsItem->getChildByName<Text*>("label_goods_price");
		auto	t_name = goodsItem->getChildByName<Text*>("label_goods_name");
		auto	t_amount = goodsItem->getChildByName<Text*>("label_goods_amount");
		auto	t_weight = goodsItem->getChildByName<Text*>("label_goods_weight");
		auto    i_weight = goodsItem->getChildByName<Widget*>("image_weight");
		auto    i_div = goodsItem->getChildByName<ImageView*>("image_div_2");
		auto    i_lvBg = goodsItem->getChildByName<ImageView*>("image_lv_bg");
		i_lvBg->setVisible(false);
		std::string itemName;
		std::string itemPath;
		getItemNameAndPath(m_guideGoodType, m_guideId, itemName, itemPath);
		img_icon->ignoreContentAdaptWithSize(false);
		img_icon->loadTexture(itemPath);

		goodsItem->setName(String::createWithFormat("button_item_%d", i)->getCString());
		goodsItem->setTag(i);

		setBgButtonFormIdAndType(button_good, m_guideId, m_guideGoodType);
		setBgImageColorFormIdAndType(i_bgImage, m_guideId, m_guideGoodType);
		setTextColorFormIdAndType(t_name, m_guideId, m_guideGoodType);
		button_good->addTouchEventListener(CC_CALLBACK_2(UIGuideExchange::menuCall_func,this));
		price->setString(StringUtils::format("%.f", m_lastCityPrice));
		t_name->setString(itemName);
		if (m_guideGoodType == ITEM_TYPE_SHIP || m_guideGoodType == ITEM_TYPE_DRAWING)
		{
			i_weight->setVisible(false);
			t_weight->setVisible(false);
			i_div->setVisible(false);
		}
		else
		{
			//GetItemsBeingSoldResult *buyResult = (GetItemsBeingSoldResult *)m_pResult;
			i_weight->setVisible(true);
			t_weight->setVisible(true);
			t_weight->setString(StringUtils::format("%d", m_guideWeight));

			i_div->setVisible(true);
		}
		goods_list->pushBackCustomItem(goodsItem);
		goodsItem->addTouchEventListener(CC_CALLBACK_2(UIGuideExchange::buyBtnEvent,this));
	}
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(goods_list, image_pulldown);
	guide();
}
