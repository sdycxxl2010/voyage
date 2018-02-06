#include "UIExchange.h"
#include "UIChatHint.h"
#include "UIVoyageSlider.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UIStore.h"

#include "Utils.h"

#define FEE 10000
/*
*平均价格
*/
float getAveragePrice(PriceDataDefine **price,int64_t num,int64_t buy)
{
	if(price == nullptr || num == 0 || buy == 0)
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
		}else
		{
			temp = diff;
			total += (c * p);
		}
	}
	if(diff <= 0){
		return (1.0 * total / buy);
	}else{
		return total*1.0/(buy-diff);
	}
}
/*
*最大购买数
*/
int getMaxNumber(PriceDataDefine **price,int num,int64_t coins, float tax)
{
	if(price == nullptr || num == 0 || coins == 0)
	{
		return 0;
	}

	int64_t total = 0;
	int	temp = 0;
	int maxNum = 0;
	for (auto i = 0; i < num; ++i)
	{
		total += ceil(1.0*(price[i]->count * price[i]->price)*(1+ tax));//税
		maxNum += price[i]->count;
	}
	if (total < coins)
	{
		return maxNum;
	}else
	{
		total = 0;
		maxNum = 0;
		for (int i = 0; i < num; i++)
		{
			if (price[i]->price > 0)
			{
				temp = 1.0 * (coins / (price[i]->price * (1 + tax)));
			}else
			{
				temp = 0;
			}
			
			if (temp > price[i]->count)
			{
				coins -= ceil(1.0 * price[i]->price * price[i]->count * (1 + tax));
			}else
			{
				break;
			}
			maxNum += price[i]->count;
			
		}
		return  maxNum + temp;
	}
}

UIExchange::UIExchange()
{
	m_nItemIndex = 0;
	m_nBuyNumber = 0;
	m_nUnparadox = 0;
	m_fTotalCap = 0;
	m_bMoveActionFlag = true;
	m_eUIType = UI_EXCHANGE;
	m_bIsSellPopular = false;
	m_bIsLevelUp = false;
	m_bIsPrestigeUp = false;
	m_bIsCaptainUp = false;
	m_nPageIndex = PAGE_BUY_INDEX;
	m_pMainButton = nullptr;
	m_pMinorButton = nullptr;
	m_pResult = nullptr;
	m_pLastPriceResult = nullptr;
	m_bIsSellOrder = false;
	m_bIsRecentSellOrder = false;
	m_pTempButton = nullptr;
	m_eType = ITEM_TYPE_GOOD;
	m_nOrderIndex = -1;
	m_nOrderAmount = -1;
	m_nOrderPrice = -1;
	m_nOrderTax = -1;
	m_nOrderFee = -1;
	m_nOrderDay = -1;
	m_nOrderSkillLv = -1;
	m_pOrderlistResult = nullptr;
	m_pRecentOrderResult = nullptr;
	m_nAverageCost = -1;
	m_pSellItemResult = nullptr;
	m_pBuyListImagePulldown = nullptr;
	m_pSellListImagePulldown = nullptr;
	m_TableViewItem = nullptr;
	m_pGetItemToSell = nullptr;
	m_bNumpadFlag = true;
	m_vItemId.clear();
	m_vSkillDefine.clear();
}

UIExchange::~UIExchange()
{
	unregisterCallBack();
}

UIExchange* UIExchange::createExchange()
{
	UIExchange* ex = new UIExchange;
	if (ex && ex->init())
	{
		ex->autorelease();
		return ex;
	}
	CC_SAFE_DELETE(ex);
	return nullptr;
}

bool UIExchange::init()
{
	bool pRet = false;
	do 
	{
		CC_BREAK_IF(!UIBasicLayer::init());
		registerCallBack();

		openView(MARKET_COCOS_RES[MARKET_CSB]);
		openView(MARKET_COCOS_RES[MARKET_BUY_CSB]);
		m_nPageIndex = PAGE_BUY_INDEX;
		ProtocolThread::GetInstance()->getItemsBeingSold(ITEM_TYPE_GOOD,UILoadingIndicator::create(this));
		changeMainButtonState(nullptr);
		changeMinorButtonState(nullptr);
		auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
		auto  l_button = view->getChildByName("panel_buy_two_butter")->getChildByName<cocos2d::ui::ScrollView*>("listview_two_butter");
		l_button->addEventListener(CC_CALLBACK_2(UIExchange::scrollButtonEvent, this));
		auto b_more = view->getChildByName<Button*>("button_more");
		b_more->setVisible(true);
		/*
		auto  list_buy = view->getChildByName("panel_buy_two_butter")->getChildByName<ListView*>("listview_two_butter");
		list_buy->addEventListenerListView(this, listvieweventselector(ExchangeLayer::touchButtonListview));
		auto  i_pulldown = view->getChildByName("panel_buy_two_butter")->getChildByName<ImageView*>("image_pulldown");
		addListViewBar(list_buy, i_pulldown);
		auto btn_pulldown = i_pulldown->getChildByName("button_pulldown");
		btn_pulldown->setAnchorPoint(Vec2(0.5, 0));
		m_pBuyListImagePulldown = i_pulldown;
		m_pBuyListImagePulldown->setVisible(false);
		*/
		//聊天
		auto ch = UIChatHint::createHint();
		this->addChild(ch, 10);
		pRet = true;
	} while (0);
	return pRet;
}

void UIExchange::confirmEvent()
{
	switch (m_nPageIndex)
	{
	case PAGE_BUY_INDEX:
		{
			sendBuyResultToServer();
			break;
		}
	case PAGE_SELL_INDEX:
		{
			GetPersonalItemResult *result = (GetPersonalItemResult *)m_pResult;
			if(m_nBuyNumber <= 0)
			{
				CCLOG("buy count is null...");
				return;
			}
			int64_t price1 = ceil(getAveragePrice(m_pLastPriceResult->npconlyprices,m_pLastPriceResult->n_npconlyprices,m_nBuyNumber));
			int64_t price2 = ceil(getAveragePrice(m_pLastPriceResult->prices,m_pLastPriceResult->n_prices,m_nBuyNumber));
			int itemId = 0;
			int optioanalNum = 0;
			int uniqueId = 0;
			if (result->itemtype == 0)
			{
				itemId = result->goods[m_nItemIndex]->goodsid;
			}else if (result->itemtype == 1)
			{
				itemId = result->ships[m_nItemIndex]->sid;
				optioanalNum = result->ships[m_nItemIndex]->optionalnum;
				uniqueId = result->ships[m_nItemIndex]->id;
			}else if (result->itemtype == 5)
			{
				itemId = result->drawings[m_nItemIndex]->iid;
				uniqueId = result->drawings[m_nItemIndex]->uniqueid;
			}else if (result->itemtype < 6)
			{
				itemId = result->equipments[m_nItemIndex]->equipmentid;
				optioanalNum = result->equipments[m_nItemIndex]->optionalnum;
				uniqueId = result->equipments[m_nItemIndex]->uniqueid;
			}else if (result->itemtype == 6)
			{
				itemId =  result->specials[m_nItemIndex]->itemid;
			}
			if (m_bIsSellPopular)
			{
				m_bIsSellPopular = false;
				ProtocolThread::GetInstance()->sellItem(result->itemtype,itemId,
				m_nBuyNumber,optioanalNum,uniqueId,price1,1,UILoadingIndicator::create(this));
			}else
			{
				ProtocolThread::GetInstance()->sellItem(result->itemtype,itemId,
				m_nBuyNumber,optioanalNum,uniqueId,price2,0,UILoadingIndicator::create(this));
			}
			break;
		}
	default:
		break;
	}
}

void UIExchange::sendBuyResultToServer()
{
	if(m_nBuyNumber <= 0)
	{
		CCLOG("buy count is null...");
		return;
	}
	
	int goodsForShips = m_nBuyNumber;
	int *shipIds = nullptr;
	int	*counts = nullptr;
	if (m_pLastPriceResult->n_shipinfos > 0)
	{
		shipIds = new int[m_pLastPriceResult->n_shipinfos];
		counts = new int[m_pLastPriceResult->n_shipinfos];
	}
	int index = m_pMinorButton->getTag() - BUY_GOOD_INDEX;
	switch (index)
	{
	case ITEM_TYPE_SHIP:
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_PORP:
	case ITEM_TYPE_ROLE_EQUIP:
	case ITEM_TYPE_DRAWING: 
		{
			break;
		}
	case ITEM_TYPE_GOOD:	//buy view ,fist button
		{
			for (int i = 0; i < m_pLastPriceResult->n_shipinfos; ++i)
			{
				UIVoyageSlider *ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(i + 2));
				counts[i] = ss->getCurrentNumber();
				shipIds[i] = m_pLastPriceResult->shipinfos[i]->shipid;
				goodsForShips -= counts[i];
			}
			if (goodsForShips != 0)
			{
				cocos2d::log("push goods in ships,have a error,please check!!");
				return;
			}
			break;
		}
	default:
		break;
	}
	GetItemsBeingSoldResult *result = (GetItemsBeingSoldResult*)m_pResult;
	int64_t price = ceil(getAveragePrice(m_pLastPriceResult->prices,m_pLastPriceResult->n_prices,m_nBuyNumber));
	float dat = (1.0 + m_pLastPriceResult->tax * 1.0 / FEE);
	price = ceil(price * dat);
	if (index == ITEM_TYPE_GOOD)
	{
		ProtocolThread::GetInstance()->buyItem(m_pLastPriceResult->itemtype,m_pLastPriceResult->itemid,
			price,shipIds,counts,m_pLastPriceResult->n_shipinfos,result->items[m_nItemIndex]->optionalnum,
			result->items[m_nItemIndex]->charcteritemid, UILoadingIndicator::create(this));
	}else
	{
		ProtocolThread::GetInstance()->buyItem(m_pLastPriceResult->itemtype,m_pLastPriceResult->itemid,
			price,nullptr,nullptr,m_nBuyNumber,result->items[m_nItemIndex]->optionalnum,
			result->items[m_nItemIndex]->charcteritemid, UILoadingIndicator::create(this));
	}
	
	if (shipIds != nullptr)
	{
		delete []shipIds;
		delete []counts;
		shipIds = nullptr;
		counts = nullptr;
	}
}
float* UIExchange::getShipsCap()
{
	float* shipsCap = new float[m_pLastPriceResult->n_shipinfos];
	for (auto i = 0; i < m_pLastPriceResult->n_shipinfos; ++i)
	{
		shipsCap[i] = (m_pLastPriceResult->shipinfos[i]->maxsize - m_pLastPriceResult->shipinfos[i]->usedsize) / m_pLastPriceResult->weight;
		if (m_pLastPriceResult->shipinfos[i]->maxsize < m_pLastPriceResult->shipinfos[i]->usedsize)
		{
			shipsCap[i] = 0;
		}
	}
	return shipsCap;
}

void UIExchange::buyingSliderEvent(const int64_t nNum)
{
	m_nBuyNumber = nNum;
	Widget *panel = nullptr;
	int64_t price = ceil(getAveragePrice(m_pLastPriceResult->prices,m_pLastPriceResult->n_prices,nNum));
	int buttonTag = m_pMinorButton->getTag() - BUY_GOOD_INDEX;
	if(buttonTag == ITEM_TYPE_SHIP || buttonTag == ITEM_TYPE_DRAWING || buttonTag == ITEM_TYPE_SPECIAL)
	{
		panel = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_VIEWSHIP_CSB]);
		auto panel_buy_goods = panel->getChildByName<Widget*>("panel_buy_goods");
		auto image_subRoot = panel_buy_goods->getChildByName("image_select_num_bg");
		auto panel_slider = image_subRoot->getChildByName<Widget*>("panel_select_num");
		auto text_count = panel_slider->getChildByName<Text*>("label_goods_num");
		std::string st_count = StringUtils::format("x %lld",nNum);
		float dat = (1.0 + m_pLastPriceResult->tax * 1.0 / FEE);
		std::string st_cost = numSegment( StringUtils::format("%lld",int64_t(ceil(nNum * price * dat))));
		
		auto text_price = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_buy_goods,"label_price_num"));
		auto panel_tax  = image_subRoot->getChildByName<Widget*>("panel_tax_num");
		auto text_tax  = panel_tax->getChildByName<Text*>("label_price_num");
		auto panel_cost = image_subRoot->getChildByName<Widget*>("panel_cost_num");
		auto text_cost = panel_cost->getChildByName<Text*>("label_cost_num");
		text_tax->setString(numSegment(StringUtils::format("%.0f",ceil(1.0 * nNum * price * m_pLastPriceResult->tax / FEE))));
		text_count->setString(st_count);
		text_cost->setString(st_cost);
		text_price->setString(numSegment(StringUtils::format("%lld",price)));
		dynamic_cast<Layout*>(panel_tax)->requestDoLayout();
		dynamic_cast<Layout*>(panel_cost)->requestDoLayout();
//chengyuan++
		auto i_silver = panel_cost->getChildByName<ImageView*>("image_silver");
		text_cost->ignoreContentAdaptWithSize(true);
		i_silver->setPositionX(text_cost->getPositionX() - text_cost->getContentSize().width - i_silver->getContentSize().width/2);
	}else
	{
		panel = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_VIEWEQUIP_CSB]);
		auto panel_buy_goods = panel->getChildByName<Widget*>("panel_buy_goods");
		auto image_subRoot = panel_buy_goods->getChildByName("image_select_num_bg");
		auto panel_slider = image_subRoot->getChildByName<Widget*>("panel_select_num");
		auto text_count = panel_slider->getChildByName<Text*>("label_goods_num");
		auto panel_weight = dynamic_cast<Widget*>(Helper::seekWidgetByName(panel_buy_goods,"panel_ship_weight"));
		auto image_weight = panel_weight->getChildByName("image_progressbar_ship_weight");
		auto loadingbar_wight = image_weight->getChildByName<LoadingBar*>("progressbar_weight");
		auto loadingbar_wight_add = image_weight->getChildByName<LoadingBar*>("progressbar_weight_add");
		auto panel_bagcapacity = panel_weight->getChildByName<Layout*>("panel_shipcapacity_status_num");
		auto text_bagcapacity_num_1 = panel_bagcapacity->getChildByName<Text*>("label_shipcapacity_status_num_1");
		auto text_bagcapacity_num_3 = panel_bagcapacity->getChildByName<Text*>("label_shipcapacity_status_num_3");
		auto panel_tax  = image_subRoot->getChildByName<Widget*>("panel_tax_num");
		auto text_tax  = panel_tax->getChildByName<Text*>("label_price_num");
		auto panel_cost = image_subRoot->getChildByName<Widget*>("panel_cost_num");
		auto text_cost = panel_cost->getChildByName<Text*>("label_cost_num");
		auto text_price = panel_buy_goods->getChildByName<Text*>("label_price_num");
		
		auto i_weigh = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel, "image_weight"));
		i_weigh->setTouchEnabled(true);
		i_weigh->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
		i_weigh->setTag(IMAGE_ICON_WEIGHT + IMAGE_INDEX2);
		auto i_coins = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel, "image_goldcoin_1"));
		i_coins->setTouchEnabled(true);
		i_coins->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
		i_coins->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);
		auto i_icon_ship = panel->getChildByName<ImageView*>("panel_buying_props");
		auto i_icon_ship1 = i_icon_ship->getChildByName<ImageView*>("panel_buy_goods");
		auto i_icon_ship2 = i_icon_ship1->getChildByName<ImageView*>("image_select_num_bg");
		auto i_icon_ship3 = i_icon_ship2->getChildByName<ImageView*>("panel_ship_weight");
		auto i_icon_ship4 = i_icon_ship3->getChildByName<ImageView*>("image_ship_weight");
		i_icon_ship4->setTouchEnabled(true);
		i_icon_ship4->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
		i_icon_ship4->setTag(IMAGE_ICON_PACKAGE + IMAGE_INDEX2);
		auto i_icons1 = i_icon_ship2->getChildByName<ImageView*>("panel_tax_num");
		auto i_icons2 = i_icons1->getChildByName<ImageView*>("image_goldcoin_1");
		i_icons2->setTouchEnabled(true);
		i_icons2->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
		i_icons2->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);
		auto i_icons3 = i_icon_ship2->getChildByName<ImageView*>("panel_cost_num");
		auto i_icons4 = i_icons3->getChildByName<ImageView*>("image_silver");
		i_icons4->setTouchEnabled(true);
		i_icons4->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
		i_icons4->setTag(IMAGE_ICON_COINS + IMAGE_INDEX1);

		text_price->setString(numSegment(StringUtils::format("%lld",price)));
		float cost_num = nNum * price + ceil(1.0 * nNum * price * m_pLastPriceResult->tax / FEE);
		std::string st_cost = numSegment(StringUtils::format("%.0f",cost_num));
		std::string st_count = StringUtils::format("x %d",nNum);
		text_count->setString(st_count);
		text_cost->setString(st_cost);
		text_tax->setString(numSegment(StringUtils::format("%lld",int64_t(ceil(1.0 * nNum * price *  m_pLastPriceResult->tax / FEE)))));
		loadingbar_wight->setPercent(100 * (m_pLastPriceResult->curpackagesize)/m_pLastPriceResult->maxpackagesize);
		loadingbar_wight_add->setPercent(100 * (m_pLastPriceResult->curpackagesize + nNum * m_pLastPriceResult->weight)/m_pLastPriceResult->maxpackagesize);
		text_bagcapacity_num_1->setString(String::createWithFormat("%0.2f",(1.0 * m_pLastPriceResult->curpackagesize / 100) + (1.0 * nNum * m_pLastPriceResult->weight / 100))->_string);
		text_bagcapacity_num_3->setString(String::createWithFormat("/%0.2f",1.0 * (m_pLastPriceResult->maxpackagesize) / 100)->_string);
		panel_bagcapacity->setLayoutType(Layout::Type::HORIZONTAL);
		panel_bagcapacity->requestDoLayout();
		dynamic_cast<Layout*>(panel_cost)->requestDoLayout();
		dynamic_cast<Layout*>(panel_tax)->requestDoLayout();
//chengyuan++
		auto i_silver = panel_cost->getChildByName<ImageView*>("image_silver");
		text_cost->ignoreContentAdaptWithSize(true);
		i_silver->setPositionX(text_cost->getPositionX() - text_cost->getContentSize().width - i_silver->getContentSize().width / 2);
	}
}

void UIExchange::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	if (!m_bMoveActionFlag)
	{
		return;
	}
	auto button = (Widget*)pSender;
	int tag = button->getTag();
	std::string name = button->getName();
	//玩家升级
	if (isButton(panel_levelup))
	{
		if (m_bIsPrestigeUp)
		{
			m_bIsPrestigeUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
			return;
		}
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->getCaptainAddexp();
			UICommon::getInstance()->flushCaptainLevelUp(m_pSellItemResult->n_captains, m_pSellItemResult->captains);
			return;
		}
		return;
	}
	//声望升级
	if (isButton(panel_r_levelup))
	{
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->getCaptainAddexp();
			UICommon::getInstance()->flushCaptainLevelUp(m_pSellItemResult->n_captains, m_pSellItemResult->captains);
			return;
		}
		return;
	}
	//船长升级
	if (isButton(panel_captain_addexp))
	{
		return;
	}
	//V票
	if (isButton(button_ok))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		closeView();
		confirmEvent();
		return;
	}
	else
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//打开商城
		if (isButton(button_silver))
		{
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 1);
			return;
		}
		if (isButton(button_gold))
		{
			UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
			return;
		}
		//物品详情
		if (isButton(button_good_bg) && m_nPageIndex != PAGE_ORDER_INDEX)//&& m_vItemId.empty())
		{
			auto par = dynamic_cast<Widget*>(button->getParent());
			if (par->getTag() < 200)
			{
				m_nItemIndex = par->getTag();
			}
			getCurrentItemInfo();
			return;
		}
		//取消订单
		if (isButton(button_cancel_order))
		{
			closeView();
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYesOrNO("TIP_EXCANCEL_ORDER_CONFIRM_TITLE", "TIP_EXCANCEL_ORDER_CONFIRM");
			return;
		}

		//关闭
		if (isButton(button_close) || isButton(button_no) || isButton(button_cancel))
		{
			closeView();
			return;
		}
		//买 卖 订单 按钮
		if (isButton(button_buy))
		{
			closeView();
			openView(MARKET_COCOS_RES[MARKET_BUY_CSB]);
			m_pMinorButton = nullptr;
			changeMainButtonState(button);
			changeMinorButtonState(nullptr);
			m_nPageIndex = PAGE_BUY_INDEX;
			//滑动条
			//m_pSellListImagePulldown = nullptr;
			auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
			/*
			auto  list_buy = view->getChildByName("panel_buy_two_butter")->getChildByName<ListView*>("listview_two_butter");
			list_buy->addEventListenerListView(this, listvieweventselector(ExchangeLayer::touchButtonListview));
			auto  i_pulldown = view->getChildByName("panel_buy_two_butter")->getChildByName<ImageView*>("image_pulldown");
			addListViewBar(list_buy, i_pulldown);
			auto btn_pulldown = i_pulldown->getChildByName("button_pulldown");
			btn_pulldown->setAnchorPoint(Vec2(0.5, 0));
			m_pBuyListImagePulldown = i_pulldown;
			m_pBuyListImagePulldown->setVisible(false);
			*/
			auto  l_button = view->getChildByName("panel_buy_two_butter")->getChildByName<cocos2d::ui::ScrollView*>("listview_two_butter");
			l_button->addEventListener(CC_CALLBACK_2(UIExchange::scrollButtonEvent, this));
			auto b_more = view->getChildByName<Button*>("button_more");
			b_more->setVisible(true);
			ProtocolThread::GetInstance()->getItemsBeingSold(ITEM_TYPE_GOOD, UILoadingIndicator::create(this));
			return;
		}
		//卖 按钮
		if (isButton(button_sell))
		{
			closeView();
			openView(MARKET_COCOS_RES[MARKET_SELL_CSB]);
			m_pMinorButton = nullptr;
			changeMainButtonState(button);
			changeMinorButtonState(nullptr);
			m_nPageIndex = PAGE_SELL_INDEX;
			//滑动条
			//m_pBuyListImagePulldown = nullptr;
			auto view = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]);
			/*
			auto  list_buy = view->getChildByName("panel_buy_two_butter")->getChildByName<ListView*>("listview_two_butter");
			list_buy->addEventListenerListView(this, listvieweventselector(ExchangeLayer::touchButtonListview));
			auto  i_pulldown = view->getChildByName("panel_buy_two_butter")->getChildByName<ImageView*>("image_pulldown");
			addListViewBar(list_buy, i_pulldown);
			auto btn_pulldown = i_pulldown->getChildByName("button_pulldown");
			btn_pulldown->setAnchorPoint(Vec2(0.5, 0));
			m_pSellListImagePulldown = i_pulldown;
			m_pSellListImagePulldown->setVisible(false);
			*/
			auto  l_button = view->getChildByName("panel_buy_two_butter")->getChildByName<cocos2d::ui::ScrollView*>("listview_two_butter");
			l_button->addEventListener(CC_CALLBACK_2(UIExchange::scrollButtonEvent, this));
			ProtocolThread::GetInstance()->getPersonalItems(ITEM_TYPE_GOOD, 1, UILoadingIndicator::create(this));
			auto b_more = view->getChildByName<Button*>("button_more");
			b_more->setVisible(true);
			return;
		}
		//订单 按钮
		if (isButton(button_myorder))
		{
			closeView();
			openView(MARKET_COCOS_RES[MARKET_ORDER_CSB]);
			m_pMinorButton = nullptr;
			changeMainButtonState(button);
			changeMinorButtonState(nullptr);
			m_nPageIndex = PAGE_ORDER_INDEX;
			m_bIsSellOrder = false;
			ProtocolThread::GetInstance()->getOrderList(0, UILoadingIndicator::create(this));
			return;
		}
		//回到主城
		if (isButton(button_backmaincity))
		{
			button_callBack();
			return;
		}
		//创建订单
		if (isButton(button_create_order))
		{
			if (SINGLE_HERO->m_iLevel <= 5)//等级不足时提示
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCHANGE_ORDER_NOT_LEVEL");
			}
			else if (SINGLE_HERO->m_iHaslisence)
			{
				if (SINGLE_HERO->m_iEffectivelisence == 0)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_PALACE_LICENSE_DO");
				}
				else
				{
					if (m_bMoveActionFlag)
					{
						m_eType = ITEM_TYPE_GOOD;
						if (m_pMinorButton->getTag() == ORDER_BUY_INDEX)//买单
						{
							updateBuyOrderSelectedItemView();
						}
						else//卖单
						{
							ProtocolThread::GetInstance()->getItemsToSell(m_eType, 1, UILoadingIndicator::create(this));
						}
					}
				}
			}
			else//没有许可证提示
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCHANGE_NOT_LICENSE");
			}
			return;
		}
		//技能详情
		if (isButton(image_skill_bg))
		{
			UICommon::getInstance()->openCommonView(this);
			int index = button->getTag();
			UICommon::getInstance()->flushSkillView(m_vSkillDefine.at(index));
			return;
		}
		flushEvent(button, name);
	}
	if (isButton(image_goldcoin_1))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(button);
		return;
	}
	if (isButton(image_ship_weight))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(button);
		return;
	}
	if (isButton(image_silver))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(button);
		return;
	}
	if (isButton(image_weight))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(button);
		return;
	}
	if (isButton(image_ship_weight))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushImageDetail(button);
		return;
	}
	if (isButton(button_del))
	{
		std::string str = m_inputNum->getString();
		if (!str.empty())
		{
			str.erase(str.end() - 1);
			m_inputNum->setString(str);
		}
		std::string value = m_inputNum->getString();
		int a = 0;
		std::string str1 = value;
		std::string str2 = "";
		a = str1.find_first_of(",");
		if (a == -1)
		{
			m_nOrderPrice = atoi(value.c_str());
			if (m_nOrderPrice <= 0)
			{
				m_nOrderPrice = 0;
			}
			m_inputNum->setString(numSegment(StringUtils::format("%lld", m_nOrderPrice)));
		}
		else
		{
			do
			{
				a = str1.find(",");
				if (a != -1)
				{
					str2 += str1.substr(0, a);
					str1 = str1.substr(a + 1, std::string::npos);
				}
				else
				{
					str2 += str1;
				}

			} while (a != -1);

			m_nOrderPrice = atoi(str2.c_str());
			m_inputNum->setString(numSegment(StringUtils::format("%lld", m_nOrderPrice)));
		}
		goodsNumberChangeEvent(m_nOrderAmount);
		return;
	}
	if (isButton(button_yes) || isButton(panel_numpad))
	{
		if (m_bNumpadFlag)
		{
			m_bNumpadFlag = false;
			auto numpad = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
			numpad->setTouchEnabled(false);
			auto panel_silver = numpad->getChildByName<Widget*>("panel_silver");
			panel_silver->runAction(Sequence::createWithTwoActions(DelayTime::create(0.4f), Place::create(ENDPOS)));
			auto num_root = numpad->getChildByName<Widget*>("panel_numpad");
			num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, ENDPOS4),CallFunc::create(this, callfunc_selector(UIExchange::movedEnd))));
			numpad->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Place::create(ENDPOS)));
			num_root->getChildByName<Button*>("button_yes")->setTouchEnabled(false);
			std::string str = m_inputNum->getString();
			if (str.empty())
			{
				m_inputNum->setString("0");
			}
		}
		return;
	}
}


void UIExchange::buyBtnEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		//sound effect
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (SINGLE_HERO->m_iHaslisence)
		{
			if (SINGLE_HERO->m_iEffectivelisence == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PALACE_LICENSE_DO");
			}
			else
			{
				auto target = dynamic_cast<Widget*>(pSender);
				m_nItemIndex = target->getTag();
				GetItemsBeingSoldResult *result = (GetItemsBeingSoldResult*)m_pResult;
				ProtocolThread::GetInstance()->getPriceData(result->itemtype, result->items[m_nItemIndex]->itemid, result->items[m_nItemIndex]->charcteritemid, 50, 0, UILoadingIndicator::create(this));

			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_EXCHANGE_NOT_LICENSE");
		}
	}
}

void UIExchange::sellBtnEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		//sound effect
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto target = dynamic_cast<Widget*>(pSender);
		m_nItemIndex = target->getTag();
		GetPersonalItemResult *result = (GetPersonalItemResult*)m_pResult;
		int itemid = 0;
		int amount = 0;
		int characterItemId = 0;
		if (result->itemtype == ITEM_TYPE_GOOD)
		{
			itemid = result->goods[m_nItemIndex]->goodsid;
			amount = result->goods[m_nItemIndex]->amount;
		}else if (ITEM_TYPE_SHIP == result->itemtype)
		{
			itemid = result->ships[m_nItemIndex]->sid;
			characterItemId = result->ships[m_nItemIndex]->id;
			amount = 1;
		}else if (ITEM_TYPE_DRAWING == result->itemtype)
		{
			itemid = result->drawings[m_nItemIndex]->iid;
			amount = result->drawings[m_nItemIndex]->amount;
		}else if (ITEM_TYPE_SPECIAL == result->itemtype )
		{
			itemid = result->specials[m_nItemIndex]->itemid;
			amount = result->specials[m_nItemIndex]->amount;
		}else
		{
			itemid = result->equipments[m_nItemIndex]->equipmentid;
			amount = result->equipments[m_nItemIndex]->amount;
			characterItemId = result->equipments[m_nItemIndex]->uniqueid;
		}
		if (SINGLE_HERO->m_iHaslisence)
		{
			if (SINGLE_HERO->m_iEffectivelisence == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PALACE_LICENSE_DO");
			}
			else
			{
				ProtocolThread::GetInstance()->getPriceData(result->itemtype, itemid, 0, amount, 1, UILoadingIndicator::create(this));
			}
		}else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_EXCHANGE_NOT_LICENSE");
		}
	}
}

void UIExchange::sellBtnEventPopular(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		//sound effect
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto panel_sell_dialog  = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODSPOPULAR_CSB]);
		auto panel_sell_goods = panel_sell_dialog->getChildByName("panel_sell_goods");
		auto target = dynamic_cast<Button*>(pSender);
		std::string name = target->getName();

		auto btn_price_1 = panel_sell_goods->getChildByName<Button*>("button_price_r");
		auto btn_price_2 = panel_sell_goods->getChildByName<Button*>("button_price");
		if (isButton(button_price_r))
		{
			m_bIsSellPopular = true;
			target->setBright(false);
			btn_price_2->setBright(true);
		}else
		{
			m_bIsSellPopular = false;
			target->setBright(false);
			btn_price_1->setBright(true);
		}
		auto ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(100));
		int num = ss->getCurrentNumber();
		sellingSliderEvent(num);
	}
}

void UIExchange::orderBtnEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		//sound effect
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto target = dynamic_cast<Widget*>(pSender);
		m_nItemIndex = target->getTag();
		openOrderOperate();
	}
}

void UIExchange::itemInfoBtnEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		//sound effect
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		Button* target = dynamic_cast<Button*>(pSender);
		m_nItemIndex = target->getTag();
		getCurrentItemInfo();
	}
}

bool UIExchange::moveTo(Node* target,const Point& pos)
{
	if (m_bMoveActionFlag)
	{
		m_bMoveActionFlag = false;
		target->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f,pos),
			CallFunc::create(this,callfunc_selector(UIExchange::moveEnd))));
		return true;
	}else
	{
		return false;
	}
}

void UIExchange::flushEvent(Widget *target,std::string name)
{
	switch (m_nPageIndex)
	{
	case PAGE_BUY_INDEX:
		pageview_buttonEvent_1(name,target);
		break;
	case PAGE_SELL_INDEX:
		pageview_buttonEvent_2(name,target);
		break;
	case PAGE_ORDER_INDEX:
		pageview_buttonEvent_3(name,target);
		break;
	default:
		break;
	};
}

void UIExchange::openOrderOperate()
{
	openView(MARKET_COCOS_RES[MARKET_ORDER_VIEW_CSB]);
	auto order_info = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_VIEW_CSB]);
	auto listview_info = order_info->getChildByName<ListView*>("listview_result");
	auto text_name = order_info->getChildByName<Text*>("label_goods_name");
	auto text_price = order_info->getChildByName<Text*>("label_amount");//price
	auto text_amount = order_info->getChildByName<Text*>("label_price_num");//amount
	auto panel_remain_time = listview_info->getChildByName("panel_time_remain");
	auto text_days = panel_remain_time->getChildByName<Text*>("label_day");
	auto panel_location = listview_info->getChildByName("panel_location");
	auto text_city = panel_location->getChildByName<Text*>("label_city");
	auto panel_total = listview_info->getChildByName("panel_total");
	auto text_cost = panel_total->getChildByName<Text*>("label_total_num");
	auto button_good = order_info->getChildByName<Button*>("button_good_bg");
	auto image_good = dynamic_cast<ImageView*>(button_good->getChildByName<Widget*>("image_goods"));
	button_good->setTouchEnabled(true);
	auto b_cancel_order = order_info->getChildByName<Button*>("button_cancel_order");
	b_cancel_order->setVisible(true);
	b_cancel_order->setTouchEnabled(true);

	OrderDefine *order = m_pOrderlistResult->orders[m_nItemIndex];
	time_t now_time = time(nullptr);
	now_time = order->exptime - now_time;
	if(now_time > 0)
	{
		tm *tm_days = gmtime(&now_time);
		std::string st_time = "";
		if (tm_days->tm_mday - 1>0)
		{
			    st_time = StringUtils::format("%d %s %d %s %d %s", tm_days->tm_mday - 1, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_DAY_2"].c_str(),
				tm_days->tm_hour, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_HOURS_2"].c_str(),
				tm_days->tm_min, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_MINS_2"].c_str());
		}
		else if (tm_days->tm_hour>0)
		{
			st_time = StringUtils::format("%d %s %d %s", tm_days->tm_hour, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_HOURS_2"].c_str(),
				tm_days->tm_min, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_MINS_2"].c_str());
		}
		else
		{
			st_time = StringUtils::format("%d %s",tm_days->tm_min, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_MINS_2"].c_str());
		}
		text_days->setString(st_time);
	}else
	{
		text_days->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_ORDER_OUT_TIME"]);
	}
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(m_pOrderlistResult->orders[m_nItemIndex]->itemtype,m_pOrderlistResult->orders[m_nItemIndex]->itemid,itemName,itemPath);
	
	if (m_pOrderlistResult->orders[m_nItemIndex]->item_name && std::strcmp(m_pOrderlistResult->orders[m_nItemIndex]->item_name, "") != 0)
	{
		itemName = m_pOrderlistResult->orders[m_nItemIndex]->item_name;
	}
	auto nMaxStringNum = 0;
	auto languageTypeNum = 0;
	if (isChineseCharacterIn(itemName.c_str()))
	{
		languageTypeNum = 1;
	}
	if (languageTypeNum)
	{
		nMaxStringNum = 12;
	}
	else
	{
		nMaxStringNum = 25;
	}
	text_name->setString(apostrophe(itemName, nMaxStringNum));
	image_good->loadTexture(itemPath);
	button_good->setTag(m_pOrderlistResult->orders[m_nItemIndex]->itemid + (m_pOrderlistResult->orders[m_nItemIndex]->itemtype + 1 )* 10000);
	text_price->setString(numSegment(StringUtils::format("%lld",order->price)));
	text_amount->setString((StringUtils::format("x %lld",order->number)));
	text_city->setString(SINGLE_SHOP->getCitiesInfo()[order->cityid].name);
	text_cost->setString(numSegment(StringUtils::format("%lld",order->number * order->price)));

	order_info->setTag(m_nItemIndex);

	auto i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(button_good,m_pOrderlistResult->orders[m_nItemIndex]->itemid,m_pOrderlistResult->orders[m_nItemIndex]->itemtype);
	setBgImageColorFormIdAndType(i_bgImage,m_pOrderlistResult->orders[m_nItemIndex]->itemid,m_pOrderlistResult->orders[m_nItemIndex]->itemtype);
	setTextColorFormIdAndType(text_name,m_pOrderlistResult->orders[m_nItemIndex]->itemid,m_pOrderlistResult->orders[m_nItemIndex]->itemtype);
//chengyuan++
	text_cost->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	auto i_silver = order_info->getChildByName("image_silver_1");
	text_cost->ignoreContentAdaptWithSize(true);
	i_silver->setPositionX(text_cost->getPositionX() - text_cost->getContentSize().width - i_silver->getContentSize().width / 2);
//
}
void UIExchange::openRecentOrderOperate()
{
	openView(MARKET_COCOS_RES[MARKET_ORDER_VIEW_CSB]);
	auto order_info = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_VIEW_CSB]);
	auto listview_info = order_info->getChildByName<ListView*>("listview_result");
	auto text_name = order_info->getChildByName<Text*>("label_goods_name");
	auto text_price = order_info->getChildByName<Text*>("label_amount");//price
	auto text_amount = order_info->getChildByName<Text*>("label_price_num");//amount
	auto panel_remain_time = listview_info->getChildByName("panel_time_remain");
	auto text_days = panel_remain_time->getChildByName<Text*>("label_day");
	auto panel_location = listview_info->getChildByName("panel_location");
	auto text_city = panel_location->getChildByName<Text*>("label_city");
	auto panel_total = listview_info->getChildByName("panel_total");
	auto text_cost = panel_total->getChildByName<Text*>("label_total_num");
	auto button_good = order_info->getChildByName<Button*>("button_good_bg");
	auto image_good = dynamic_cast<ImageView*>(button_good->getChildByName<Widget*>("image_goods"));
	button_good->setTouchEnabled(true);
	auto b_cancel_order = order_info->getChildByName<Button*>("button_cancel_order");
	b_cancel_order->setVisible(false);
	b_cancel_order->setTouchEnabled(false);

	RecentOrderDefine *order = m_pRecentOrderResult->orders[m_nItemIndex];
	time_t now_time = time(nullptr);
	now_time = order->exptime - now_time;
	if (now_time > 0)
	{
		tm *tm_days = gmtime(&now_time);
		std::string st_time = "";
		if (tm_days->tm_mday - 1>0)
		{
			st_time = StringUtils::format("%d %s %d %s %d %s", tm_days->tm_mday - 1, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_DAY_2"].c_str(),
				tm_days->tm_hour, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_HOURS_2"].c_str(),
				tm_days->tm_min, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_MINS_2"].c_str());
		}
		else if (tm_days->tm_hour>0)
		{
			st_time = StringUtils::format("%d %s %d %s", tm_days->tm_hour, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_HOURS_2"].c_str(),
				tm_days->tm_min, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_MINS_2"].c_str());
		}
		else
		{
			st_time = StringUtils::format("%d %s", tm_days->tm_min, SINGLE_SHOP->getTipsInfo()["TIP_PALACE_TIME_DOWN_MINS_2"].c_str());
		}
		text_days->setString(st_time);
	}
	else
	{
		text_days->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_ORDER_OUT_TIME"]);
	}
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(m_pRecentOrderResult->orders[m_nItemIndex]->itemtype, m_pRecentOrderResult->orders[m_nItemIndex]->itemid, itemName, itemPath);
	auto nMaxStringNum = 0;
	auto languageTypeNum = 0;
	if (isChineseCharacterIn(itemName.c_str()))
	{
		languageTypeNum = 1;
	}
	if (languageTypeNum)
	{
		nMaxStringNum = 12;
	}
	else
	{
		nMaxStringNum = 25;
	}
	text_name->setString(apostrophe(itemName, nMaxStringNum));
	image_good->loadTexture(itemPath);
	button_good->setTag(m_pRecentOrderResult->orders[m_nItemIndex]->itemid + (m_pRecentOrderResult->orders[m_nItemIndex]->itemtype + 1) * 10000);
	text_price->setString(numSegment(StringUtils::format("%lld", order->price)));
	text_amount->setString((StringUtils::format("x %lld", order->number)));
	text_city->setString(SINGLE_SHOP->getCitiesInfo()[order->cityid].name);
	text_cost->setString(numSegment(StringUtils::format("%lld", order->number * order->price)));

	order_info->setTag(m_nItemIndex);

	auto i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(button_good, m_pRecentOrderResult->orders[m_nItemIndex]->itemid, m_pRecentOrderResult->orders[m_nItemIndex]->itemtype);
	setBgImageColorFormIdAndType(i_bgImage, m_pRecentOrderResult->orders[m_nItemIndex]->itemid, m_pRecentOrderResult->orders[m_nItemIndex]->itemtype);
	setTextColorFormIdAndType(text_name, m_pRecentOrderResult->orders[m_nItemIndex]->itemid, m_pRecentOrderResult->orders[m_nItemIndex]->itemtype);
	//chengyuan++
	text_cost->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	auto i_silver = order_info->getChildByName("image_silver_1");
	text_cost->ignoreContentAdaptWithSize(true);
	i_silver->setPositionX(text_cost->getPositionX() - text_cost->getContentSize().width - i_silver->getContentSize().width / 2);
	//
}

void UIExchange::getCurrentItemInfo()
{
	int iid = 0;
	int type = 0;
	int id = 0;
	switch (m_nPageIndex)
	{
	case PAGE_BUY_INDEX:
		{
			GetItemsBeingSoldResult *result = (GetItemsBeingSoldResult*)m_pResult;
			iid = result->items[m_nItemIndex]->itemid;
			type = m_pMinorButton->getTag() - BUY_GOOD_INDEX;
			id = result->items[m_nItemIndex]->charcteritemid;
			break;
		}
	case PAGE_SELL_INDEX:
		{
			GetPersonalItemResult *result = (GetPersonalItemResult*)m_pResult;
			type = m_pMinorButton->getTag() - SELL_GOOD_INDEX;
			switch (type)
			{
			case ITEM_TYPE_GOOD:
				{
					iid = result->goods[m_nItemIndex]->goodsid;
					id = 0;
					break;
				}
			case ITEM_TYPE_SHIP:
				{
					iid = result->ships[m_nItemIndex]->sid;
					id = result->ships[m_nItemIndex]->id;
					break;
				}
			case ITEM_TYPE_SHIP_EQUIP:
				{
					iid = result->equipments[m_nItemIndex]->equipmentid;
					id = result->equipments[m_nItemIndex]->uniqueid;
					break;
				}
			case ITEM_TYPE_PORP:
				{
					iid = result->equipments[m_nItemIndex]->equipmentid;
					id = result->equipments[m_nItemIndex]->uniqueid;
					break;
				}
			case ITEM_TYPE_ROLE_EQUIP:
				{
					iid = result->equipments[m_nItemIndex]->equipmentid;
					id = result->equipments[m_nItemIndex]->uniqueid;
					break;
				}
			case ITEM_TYPE_DRAWING:
				{
					iid = result->drawings[m_nItemIndex]->iid;
					id = result->drawings[m_nItemIndex]->uniqueid;
					break;
				}
			case ITEM_TYPE_SPECIAL:
				{
					iid = result->specials[m_nItemIndex]->itemid;
					id = 0;
					break;
				}
			default:
				{
					break;
				}
			}
			break;
		}
	case PAGE_ORDER_INDEX:
		{
			if (m_pMinorButton->getName() == "button_order_marketbuy" || m_pMinorButton->getName() == "button_order_marketsell")
			{
				iid = m_pRecentOrderResult->orders[m_nItemIndex]->itemid;
				type = m_pRecentOrderResult->orders[m_nItemIndex]->itemtype;
				id = m_pRecentOrderResult->orders[m_nItemIndex]->uniqueid;
			}
			else
			{
				GetOrderListResult *result = (GetOrderListResult*)m_pResult;
				if (!result->orders)
				{
					result = m_pOrderlistResult;
				}

				iid = result->orders[m_nItemIndex]->itemid;
				type = result->orders[m_nItemIndex]->itemtype;
				id = result->orders[m_nItemIndex]->uniqueid;
			}
			break;
		}
	default:
		{
			break;
		}
	}

	ProtocolThread::GetInstance()->getItemsDetailInfo(iid,type,id,UILoadingIndicator::create(this));
}

void UIExchange::updateBuyOrderSelectedItemView(Widget *goodItem, int id)
{
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(m_eType, id, itemName, itemPath);
	auto name = goodItem->getChildByName<Text*>("label_goods_name");
	auto item_bg = goodItem->getChildByName<Button*>("button_good_bg");
	auto i_icon = item_bg->getChildByName<ImageView*>("image_goods");
	name->setPositionY(goodItem->getBoundingBox().size.height / 2);
	i_icon->ignoreContentAdaptWithSize(false);
	i_icon->loadTexture(itemPath);
	auto nMaxStringNum = 0;
	auto languageTypeNum = 0;
	if (isChineseCharacterIn(itemName.c_str()))
	{
		languageTypeNum = 1;
	}
	if (languageTypeNum)
	{
		nMaxStringNum = 25;
	}
	else
	{
		nMaxStringNum = 45;
	}
	name->setString(apostrophe(itemName, nMaxStringNum));
	item_bg->setTag(id);
	goodItem->setTag(id);
	item_bg->setTouchEnabled(true);
	item_bg->addTouchEventListener(CC_CALLBACK_2(UIExchange::openGoodsBtnEvent, this));
	setBgButtonFormIdAndType(item_bg, id, m_eType);
	setTextColorFormIdAndType(name, id, m_eType);
}

void UIExchange::updateBuyOrderSelectedItemView()
{
	m_vItemId.clear();
	int n_items = 0;
	auto items = SINGLE_SHOP->getItemData();
	std::map <int, ITEM_RES>::iterator m1_Iter;
	switch (m_eType)
	{
	case ITEM_TYPE_GOOD:
		n_items = SINGLE_SHOP->getGoodsData().size();
		for (size_t i = 0; i < n_items; i++)
		{
			m_vItemId.push_back(i + 1);
		}
		break;
	case ITEM_TYPE_SHIP:
		n_items = SINGLE_SHOP->getShipData().size();
		for (size_t i = 0; i < n_items; i++)
		{
			//暂时屏蔽掉require_level>50的物品
			if (SINGLE_SHOP->getShipData()[i + 1].no_trade == 0 && SINGLE_SHOP->getShipData()[i + 1].require_level <= 50)
			{
				m_vItemId.push_back(i + 1);
			}
		}
		break;
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_PORP:
	case ITEM_TYPE_ROLE_EQUIP:
	case ITEM_TYPE_DRAWING:
		for (m1_Iter = items.begin(); m1_Iter != items.end(); m1_Iter++)
		{
			auto& item = m1_Iter->second;
			//暂时屏蔽掉require_level>50的物品
			if (item.type == m_eType && item.no_trade == 0 && item.required_lv <= 50)
			{
				n_items++;
				m_vItemId.push_back(item.id);
			}
		}
		break;
	case ITEM_TYPE_SPECIAL:
		m_vItemId.push_back(10000);
		break;
	default:
		break;
	}

	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
	if (!view)
	{
		openView(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
		view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
		auto listview_two_butter = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_two_butter"));
		listview_two_butter->removeLastItem();
		listview_two_butter->removeLastItem();
		view->setPosition(ENDPOS);
		moveTo(view,STARTPOS);
		m_pTempButton = nullptr;
		changeOrderButtonState(nullptr);
		auto b_more = view->getChildByName<Button*>("button_more");
		b_more->setVisible(true);
	}
	auto panel_buy = view->getChildByName<Widget*>("panel_buy");
	auto goods_list = panel_buy->getChildByName<ListView*>("listview_item");
	auto size = goods_list->getContentSize();
	auto p_item = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_itemdetails"));
	goods_list->removeAllChildrenWithCleanup(true);
	goods_list->pushBackCustomItem(p_item->clone());
	m_TableViewItem = goods_list->getItem(0);
	m_TableViewItem->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title_create_sell_order"));
	auto t_subtitle = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_title_select_item"));
	auto tips = SINGLE_SHOP->getTipsInfo();
	t_title->setString(tips["TIP_EXCHANGE_CREATE_BUY_ORDER_TITLE"]);
	t_subtitle->setString(tips["TIP_PUP_CSB_GIFT_SUBTITLE"]);

	goods_list->setVisible(false);
	m_TableViewItem->setTouchEnabled(false);
	m_TableViewItem->setVisible(true);
	auto list_parent = goods_list->getParent();
	auto children = list_parent->getChildren();
	TableView *tableView = nullptr;
	for (auto& child : children){
		tableView = dynamic_cast<TableView*>(child);
		if (tableView){
			break;
		}
	}

	if (!tableView){
		tableView = TableView::create(this, size);
		tableView->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
		tableView->setDelegate(this);
		tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	
		list_parent->addChild(tableView, 9999);
		auto pos = goods_list->getPosition();
		tableView->setPosition(pos);
		tableView->setAnchorPoint(goods_list->getAnchorPoint());
		tableView->setTouchEnabled(true);
	}
	auto image_pulldown = panel_buy->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2);
	tableView->setUserObject(button_pulldown);
	button_pulldown->setVisible(false);
	button_pulldown->getParent()->setVisible(false);
	tableView->reloadData();

	auto p_button = panel_buy->getChildByName<Widget*>("panel_two_butter");
	auto l_button = p_button->getChildByName<cocos2d::ui::ScrollView*>("listview_two_butter");
	l_button->addEventListener(CC_CALLBACK_2(UIExchange::scrollButtonEvent, this));
}

void UIExchange::updateSellOrderSelectedItemView(GetItemsToSellResult *result)
{
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
	if (!view)
	{
		openView(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
		view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
		auto listview_two_butter = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listview_two_butter"));
		listview_two_butter->removeLastItem();
		listview_two_butter->removeLastItem();
		view->setPosition(ENDPOS);
		moveTo(view,STARTPOS);
		m_pTempButton = nullptr;
		changeOrderButtonState(nullptr);
		auto b_more = view->getChildByName<Button*>("button_more");
		b_more->setVisible(true);
	}

	auto panel_buy = view->getChildByName<Widget*>("panel_buy");
	auto l_item = panel_buy->getChildByName<ListView*>("listview_item");	
	l_item->removeAllChildrenWithCleanup(true);
	auto p_item = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_itemdetails"));
	p_item->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func,this));
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_title_create_sell_order"));	
	auto t_subtitle = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_title_select_item"));
	auto tips = SINGLE_SHOP->getTipsInfo();
	t_title->setString(tips["TIP_EXCHANGE_CREATE_SELL_ORDER_TITLE"]);
	t_subtitle->setString(tips["TIP_PUP_CSB_GIFT_SUBTITLE"]);

	auto image_pulldown = panel_buy->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");

	auto p_no = view->getChildByName<ListView*>("panel_no");
	p_no->setVisible(false);
	if (result->n_items == 0)
	{
		auto scrollView = Helper::seekWidgetByName(view, "button_pulldown");
		scrollView->setVisible(false);
		p_no->setVisible(true);
		image_pulldown->setVisible(false);
		return;
	}

	std::string itemName;
	std::string itemPath;
	for (int i = 0; i < result->n_items; i++)
	{
		ToSellItemsDefine *sellDefine = result->items[i];
		auto item = p_item->clone();
		auto t_name = item->getChildByName<Text*>("label_goods_name");
		auto item_bg = item->getChildByName<Button*>("button_good_bg");
		auto i_icon = item_bg->getChildByName<ImageView*>("image_goods");
		auto t_num = item->getChildByName<Text*>("label_goods_num");
		getItemNameAndPath(m_eType,sellDefine->itemid,itemName,itemPath);
		if (sellDefine->user_define_name && std::strcmp(sellDefine->user_define_name, "") != 0)
		{
			itemName = sellDefine->user_define_name;
		}
		auto nMaxStringNum = 0;
		auto languageTypeNum = 0;
		if (isChineseCharacterIn(itemName.c_str()))
		{
			languageTypeNum = 1;
		}
		if (languageTypeNum)
		{
			nMaxStringNum = 25;
		}
		else
		{
			nMaxStringNum = 45;
		}
		t_name->setString(apostrophe(itemName, nMaxStringNum));
		t_name->setFontName("");
		t_name->setFontSize(28);
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(itemPath);
		t_num->setString("x " + numSegment(StringUtils::format("%d",sellDefine->amount)));
		item->setTag(i);
		item_bg->setName("item_bg");
		item_bg->setTag(sellDefine->itemid);
		i_icon->setTag(sellDefine->characteritemid);
		item_bg->setTouchEnabled(true);
		item_bg->addTouchEventListener(CC_CALLBACK_2(UIExchange::openGoodsBtnEvent,this));
		l_item->pushBackCustomItem(item);
		if (sellDefine->currentdurability != sellDefine->maxdurability)
		{
			auto i_broken = item->getChildByName<ImageView*>("image_broken");
			i_broken->setVisible(true);
		}
		if (sellDefine->optionalitemnum > 0)
		{
			addStrengtheningIcon(item_bg);
		}
		if (result->itemtype == ITEM_TYPE_SHIP_EQUIP || result->itemtype == ITEM_TYPE_ROLE_EQUIP)//船装备或人装备损毁提示
		{
			if (sellDefine->currentdurability <= SHIP_EQUIP_BROKEN || sellDefine->currentdurability <= sellDefine->maxdurability*SHIP_EQUIP_BROKEN_PERCENT)
			{
				addequipBrokenIcon(item_bg);
				setBrokenEquipRed(i_icon);
			}
		}
		auto i_bgImage = item_bg->getChildByName<ImageView*>("image_item_bg_lv");
		setBgButtonFormIdAndType(item_bg,sellDefine->itemid,m_eType);
		setBgImageColorFormIdAndType(i_bgImage,sellDefine->itemid,m_eType);
		setTextColorFormIdAndType(t_name,sellDefine->itemid,m_eType);
	}
	
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2 );
	addListViewBar(l_item,image_pulldown);

	auto p_button = panel_buy->getChildByName<Widget*>("panel_two_butter");
	auto l_button = p_button->getChildByName<cocos2d::ui::ScrollView*>("listview_two_butter");
	l_button->addEventListener(CC_CALLBACK_2(UIExchange::scrollButtonEvent, this));
}

void UIExchange::openBuyDialog()
{
	int64_t buy_num = getMaxNumber(m_pLastPriceResult->prices,m_pLastPriceResult->n_prices,SINGLE_HERO->m_iCoin,m_pLastPriceResult->tax*1.0 / FEE);
	if(buy_num < 1)
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
		return;
	}
	openView(MARKET_COCOS_RES[MARKET_BUY_VIEWGOODS_CSB]);
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_VIEWGOODS_CSB]);
	
	auto i_gol1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_goldcoin_1"));
	i_gol1->setTouchEnabled(true);
	i_gol1->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
	i_gol1->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);
	auto i_gol2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_weight"));
	i_gol2->setTouchEnabled(true);
	i_gol2->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
	i_gol2->setTag(IMAGE_ICON_WEIGHT + IMAGE_INDEX2);
	auto i_gol3 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_ship_weight"));
	i_gol3->setTouchEnabled(true);
	i_gol3->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
	i_gol3->setTag(IMAGE_ICON_CARGO + IMAGE_INDEX2);
	auto i_icon_bg = view->getChildByName<ImageView*>("panel_buying_goods");
	auto i_icon_bg1 = i_icon_bg->getChildByName<ImageView*>("listview_buy_goods");
	auto i_icon_bg2 = i_icon_bg1->getChildByName<ImageView*>("image_select_num_bg");
	auto i_icon_bg3 = i_icon_bg2->getChildByName<ImageView*>("panel_tax_num");
	auto i_icon_bg4 = i_icon_bg3->getChildByName<ImageView*>("image_goldcoin_1");
	i_icon_bg4->setTouchEnabled(true);
	i_icon_bg4->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
	i_icon_bg4->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);
	auto i_icon_bg5 = i_icon_bg2->getChildByName<ImageView*>("panel_cost_num");
	auto i_icon_bg6 = i_icon_bg5->getChildByName<ImageView*>("image_silver");
	i_icon_bg6->setTouchEnabled(true);
	i_icon_bg6->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
	i_icon_bg6->setTag(IMAGE_ICON_COINS + IMAGE_INDEX1);
	auto listview_goods = dynamic_cast<ListView*>(view->getChildByName<Widget*>("listview_buy_goods"));
	auto goodsInfo = listview_goods->getItem(0);
	auto panel_selected_bg = goodsInfo->getChildByName<Widget*>("image_select_num_bg");
	auto panel_ship_weight = dynamic_cast<Widget*>(Helper::seekWidgetByName((Widget*)goodsInfo,"panel_ship_weight"));
	auto panel_slider = panel_selected_bg->getChildByName<Widget*>("panel_select_num");
	auto text_name = dynamic_cast<Text*>(goodsInfo->getChildByName("label_goods_name"));
	auto text_weght = dynamic_cast<Text *>(goodsInfo->getChildByName("label_weight_num"));
	auto button_good = goodsInfo->getChildByName<Button*>("button_good_bg");
	auto image_goods = dynamic_cast<ImageView*>(button_good->getChildByName("image_goods")); 
	auto text_price = dynamic_cast<Text*>(goodsInfo->getChildByName("label_price_num"));
	auto text_cost = dynamic_cast<Text*>(panel_selected_bg->getChildByName("label_cost_num"));
	auto text_tax = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_selected_bg,"label_tax"));
	std::string st_tax = StringUtils::format("%s(%.2f%%):",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(),m_pLastPriceResult->tax * 100.0 / FEE);
	text_tax->setString(st_tax);
	
	m_vSkillDefine.clear();
	int id = 0;
	int lv = 0;
	for (int i = 0; i < m_pLastPriceResult->n_skills; i++)
	{
		id = m_pLastPriceResult->skills[i]->id;
		if (id == SKILL_TAX_PROTOCOL || id == SKILL_TECHNIQUE_OF_CARGO)
		{
			lv = m_pLastPriceResult->skills[i]->level;
			if (lv > 0)
			{
				SKILL_DEFINE skillDefine;
				skillDefine.id = id;
				skillDefine.lv = m_pLastPriceResult->skills[i]->level;
				skillDefine.skill_type = SKILL_TYPE_PLAYER;
				skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
				m_vSkillDefine.push_back(skillDefine);
			}
		}
	}
	if (m_pLastPriceResult->captain_special_skill_about_decrease_tax_level > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = m_pLastPriceResult->captain_special_skill_about_decrease_tax_id;
		skillDefine.lv = m_pLastPriceResult->captain_special_skill_about_decrease_tax_level;
		skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
		skillDefine.icon_id = m_pLastPriceResult->captain_special_skill_about_decrease_tax_captain_id;
		m_vSkillDefine.push_back(skillDefine);
	}
	auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_skill_bg"));
	image_skill->setVisible(false);
	for (size_t i = 0; i < m_vSkillDefine.size(); i++)
	{
		if (i == 0)
		{
			image_skill->ignoreContentAdaptWithSize(false);
			image_skill->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill->setVisible(true);
			image_skill->setTag(i);
			image_skill->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func,this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
			{
				text_skill_lv->setVisible(false);
			}
			else
			{
				text_skill_lv->setVisible(true);
			}
		}
		else
		{
			auto image_skill_clone = dynamic_cast<ImageView*>(image_skill->clone());
			image_skill_clone->ignoreContentAdaptWithSize(false);
			image_skill_clone->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill_clone->setVisible(true);
			image_skill_clone->setTag(i);
			image_skill_clone->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func,this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill_clone, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			image_skill_clone->setPositionX(image_skill->getPositionX() - i*1.1*image_skill->getBoundingBox().size.width);
			panel_selected_bg->addChild(image_skill_clone);
			if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
			{
				text_skill_lv->setVisible(false);
			}
			else
			{
				text_skill_lv->setVisible(true);
			}
		}
	}

	text_name->setString(getGoodsName(m_pLastPriceResult->itemid));
	image_goods->ignoreContentAdaptWithSize(false);
	image_goods->loadTexture(getGoodsIconPath(m_pLastPriceResult->itemid));
	text_weght->setString(String::createWithFormat("%0.2f",1.0 * m_pLastPriceResult->weight/100)->_string);

	auto i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(button_good,m_pLastPriceResult->itemid,ITEM_TYPE_GOOD);
	setBgImageColorFormIdAndType(i_bgImage,m_pLastPriceResult->itemid,ITEM_TYPE_GOOD);
	setTextColorFormIdAndType(text_name,m_pLastPriceResult->itemid,ITEM_TYPE_GOOD);

	float* shipsCap = getShipsCap();
	m_fTotalGoods = 0; 
	//int64_t averagePrice = ceil(getAveragePrice(m_pLastPriceResult->prices,m_pLastPriceResult->n_prices,m_fTotalGoods));
	m_fTotalCap = 0;
	[&,shipsCap,this](){
		for (auto i = 0; i < m_pLastPriceResult->n_shipinfos; ++i)
		{
			m_fTotalGoods += shipsCap[i];
			m_fTotalCap += m_pLastPriceResult->shipinfos[i]->maxsize / m_pLastPriceResult->weight;
			cocos2d::log("m_fTotalGoods:%.f,m_fTotalCap:%.f",m_fTotalGoods,m_fTotalCap);
		}
	}();

	if(buy_num > m_fTotalGoods)  // judge coins
	{
		buy_num = m_fTotalGoods;
	}
	
	
	auto panel_shipcapacity = panel_ship_weight->getChildByName("panel_shipcapacity_status_num");
	auto text_capacity_num_1 = panel_shipcapacity->getChildByName<Text*>("label_shipcapacity_status_num_1");
	auto text_capacity_num_3 = panel_shipcapacity->getChildByName<Text*>("label_shipcapacity_status_num_3");

	auto panel_shipCap = panel_ship_weight->getChildByName("image_progressbar_ship_weight");
	auto loadingbar_weight = dynamic_cast<LoadingBar*>(panel_shipCap->getChildByName("progressbar_weight"));
	auto loadingbar_weight_add = dynamic_cast<LoadingBar*>(panel_shipCap->getChildByName("progressbar_weight_add"));

	auto ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
	if (ss)
	{
		ss->removeFromParentAndCleanup(true);
	}

	ss = UIVoyageSlider::create(panel_slider,buy_num,0,true);
	int index1 = 0;
	for (int i = 1; i < m_pLastPriceResult->n_prices; i++)
	{
		if (m_pLastPriceResult->prices[index1]->price > m_pLastPriceResult->prices[i]->price)
		{
			index1 = i;
		}
	}
	ss->setCurrentNumber(m_pLastPriceResult->prices[index1]->count);
	ss->setTag(1);
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIExchange::updateShipForNumber,this));

	int total_goods_cap = 0;
	int had_goods_cap = 0;
	for (int i = 0; i < m_pLastPriceResult->n_shipinfos; i++)
	{
		total_goods_cap += m_pLastPriceResult->shipinfos[i]->maxsize;
		had_goods_cap += m_pLastPriceResult->shipinfos[i]->usedsize;
	}
	text_capacity_num_1->setString(StringUtils::format("%0.2f", (had_goods_cap + m_pLastPriceResult->weight * buy_num) / 100.0));
	text_capacity_num_3->setString(StringUtils::format("/%0.2f", total_goods_cap / 100.0));
	dynamic_cast<Layout*>(panel_shipcapacity)->forceDoLayout();
	loadingbar_weight->setPercent(100 *(m_fTotalCap - m_fTotalGoods)/m_fTotalCap);
	loadingbar_weight_add->setPercent(100 *(m_fTotalCap - m_fTotalGoods + buy_num)/m_fTotalCap);
	
	for (int i = m_pLastPriceResult->n_shipinfos; i <= 5; ++i)
	{
		std::string s_shipName = StringUtils::format("image_shiplist_bg_%d",i);
		Node* item = goodsInfo->getChildByName(s_shipName);
		item->setVisible(false);
	}
	
	for (auto i = 1; i <= m_pLastPriceResult->n_shipinfos; ++i)
	{
		std::string st_shipName = StringUtils::format("image_shiplist_bg_%d",i);
		std::string st_capacity = StringUtils::format("listview_name_capacity_%d",i);
		std::string st_shipIconName = StringUtils::format("image_ship_bg_%d",i);
		auto item = goodsInfo->getChildByName(st_shipName);
		auto listview_capacity = dynamic_cast<ListView*>(item->getChildByName(st_capacity));
		auto text_shipCap = dynamic_cast<Text*>(listview_capacity->getItem(1));

		auto panel_ship_bg = item->getChildByName(st_shipIconName);
		auto image_ship = dynamic_cast<ImageView*>(panel_ship_bg->getChildByName("image_ship"));
		auto image_num = dynamic_cast<ImageView*>(panel_ship_bg->getChildByName("image_num"));
		auto panel_slider = item->getChildByName<Widget*>("panel_select_num");

		auto ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(i + 1));
		if (ss)
		{
			ss->removeFromParentAndCleanup(true);
		}
		auto shipSlider = UIVoyageSlider::create(panel_slider,shipsCap[i-1],0,true);
		
		float current_size = (float)m_pLastPriceResult->shipinfos[i-1]->usedsize / 100;
		float cargo_size = (float)m_pLastPriceResult->shipinfos[i-1]->maxsize / 100;

		text_shipCap->setString(String::createWithFormat("(%.2f/%.2f)",current_size,cargo_size)->_string);
		image_ship->loadTexture(::getShipIconPath(m_pLastPriceResult->shipinfos[i-1]->sid));
		image_num->loadTexture(::getPositionIconPath(i));
		listview_capacity->requestRefreshView();
		item->setVisible(true);
		auto button_minus = panel_slider->getChildByName<Widget*>("button_minus");
		auto button_plus = panel_slider->getChildByName<Widget*>("button_plus");
		auto silder_goods = dynamic_cast<Slider*>(panel_slider->getChildByName<Widget*>("slider_goods_num"));
		if (current_size == cargo_size)
		{
			button_minus->setTouchEnabled(false);
			button_plus->setTouchEnabled(false);
			silder_goods->setTouchEnabled(false);
			silder_goods->setPercent(0);
			silder_goods->setVisible(false);
			button_minus->setBright(false);
			button_plus->setBright(false);
		}else
		{
			button_minus->setTouchEnabled(true);
			button_plus->setTouchEnabled(true);
			silder_goods->setTouchEnabled(true);
			silder_goods->setVisible(true);
			button_minus->setBright(true);
			button_plus->setBright(true);
		}
		shipSlider->setEnableZero(true);
		shipSlider->setTag(i+ 1);
		this->addChild(shipSlider);
		shipSlider->addSliderScrollEvent_2(CC_CALLBACK_2(UIExchange::shipSliderEvent,this));
	}
	this->addChild(ss);
	listview_goods->refreshView();
	listview_goods->jumpToTop();

//chengyuan++
	auto p_goodsInfo = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_VIEWGOODS_CSB])->getChildByName("panel_buy_goods");
	auto l_price_view = p_goodsInfo->getChildByName<ListView*>("listView_price");
	auto t_price_num = dynamic_cast<Text*>(l_price_view->getItem(1));
	auto b_price_info = l_price_view->getItem(2);
	auto i_weight = p_goodsInfo->getChildByName<ImageView*>("image_weight");
	auto t_weight_num = p_goodsInfo->getChildByName<Text*>("label_weight_num");
	t_price_num->ignoreContentAdaptWithSize(true);
	 
	auto list_buygoods = view->getChildByName<ListView*>("listview_buy_goods");
	auto pull_down = view->getChildByName<ImageView*>("image_pulldown");
	addListViewBar(list_buygoods, pull_down);
	auto btn_pull_down = pull_down->getChildByName("button_pulldown");
	btn_pull_down->setPositionX(btn_pull_down->getPositionX() - btn_pull_down->getContentSize().width / 2);

}

void UIExchange::openBuyPropsDialog()
{
	int64_t goodsNum = getMaxNumber(m_pLastPriceResult->prices,m_pLastPriceResult->n_prices,SINGLE_HERO->m_iCoin,m_pLastPriceResult->tax*1.0 / FEE);
	if (goodsNum <= 0)
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
		return;
	}

	GetItemsBeingSoldResult	*result = (GetItemsBeingSoldResult *)m_pResult;
	int64_t avergePrice = ceil(getAveragePrice(m_pLastPriceResult->prices, m_pLastPriceResult->n_prices, 1));
	if (goodsNum > 9999)
	{
		goodsNum = 9999;
	}

	int64_t nbag = (m_pLastPriceResult->maxpackagesize - m_pLastPriceResult->curpackagesize) / m_pLastPriceResult->weight;

	if (nbag < 1)
	{
		auto bag_info = SINGLE_SHOP->getBagExpandFeeInfo();
		size_t i = 0;
		for (; i < bag_info.size(); i++)
		{
			if (bag_info[i].capacity == m_pLastPriceResult->maxpackagesize)
			{
				break;
			}
		}
		if (i == bag_info.size() - 1)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_BAG_FULL");
		}
		else
		{
			int cost_num = bag_info[i + 1].fee;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushSilverConfirmView("TIP_CENTER_ITEM_BAG_EXPAND_TITLE",
				"TIP_EXCHANGE_ITEM_BAG_EXPAND_CONTENT", cost_num);
		}	
		return;
	}

	openView(MARKET_COCOS_RES[MARKET_BUY_VIEWEQUIP_CSB]);
	auto panel_buy = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_VIEWEQUIP_CSB]);
	auto panel_buy_goods = panel_buy->getChildByName("panel_buy_goods");
	auto panel_subRoot = panel_buy_goods->getChildByName("image_select_num_bg");
	auto panel_weight = panel_buy_goods->getChildByName("panel_personal_weight");
	auto p_name = panel_buy_goods->getChildByName<Layout*>("panel_name");
	auto text_name = p_name->getChildByName<Text*>("label_name");
	auto text_price = panel_buy_goods->getChildByName<Text*>("label_price_num");
	auto panel_slider = panel_subRoot->getChildByName<Widget*>("panel_select_num");
	auto text_weght = panel_buy_goods->getChildByName<Text*>("label_weight_num");
	auto button_icon_bg = panel_buy_goods->getChildByName<Button*>("button_good_bg");
	auto image_item = dynamic_cast<ImageView*>(button_icon_bg->getChildByName<Widget*>("image_goods"));
	auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName((Widget*)panel_buy,"image_skill_bg"));
	image_skill->setVisible(false);

	m_vSkillDefine.clear();
	for (int i = 0; i < m_pLastPriceResult->n_skills; i++)
	{
		if (m_pLastPriceResult->skills[i]->id == SKILL_TAX_PROTOCOL)
		{
			SKILL_DEFINE skillDefine;
			skillDefine.id = SKILL_TAX_PROTOCOL;
			skillDefine.lv = m_pLastPriceResult->skills[i]->level;
			skillDefine.skill_type = SKILL_TYPE_PLAYER;
			skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
			m_vSkillDefine.push_back(skillDefine);
			break;
		}
	}

	if (m_pLastPriceResult->captain_special_skill_about_decrease_tax_level > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = m_pLastPriceResult->captain_special_skill_about_decrease_tax_id;
		skillDefine.lv = m_pLastPriceResult->captain_special_skill_about_decrease_tax_level;
		skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
		skillDefine.icon_id = m_pLastPriceResult->captain_special_skill_about_decrease_tax_captain_id;
		m_vSkillDefine.push_back(skillDefine);
	}

	for (size_t i = 0; i < m_vSkillDefine.size(); i++)
	{
		if (i == 0)
		{
			image_skill->ignoreContentAdaptWithSize(false);
			image_skill->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill->setVisible(true);
			image_skill->setTag(i);
			image_skill->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
			{
				text_skill_lv->setVisible(false);
			}
			else
			{
				text_skill_lv->setVisible(true);
			}
		}
		else
		{
			auto image_skill_clone = dynamic_cast<ImageView*>(image_skill->clone());
			image_skill_clone->ignoreContentAdaptWithSize(false);
			image_skill_clone->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill_clone->setVisible(true);
			image_skill_clone->setTag(i);
			image_skill_clone->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill_clone, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			image_skill_clone->setPositionX(image_skill->getPositionX() - i*1.1*image_skill->getBoundingBox().size.width);
			panel_subRoot->addChild(image_skill_clone);
			if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
			{
				text_skill_lv->setVisible(false);
			}
			else
			{
				text_skill_lv->setVisible(true);
			}
		}
	}

	auto text_tax = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)panel_buy_goods,"label_tax"));
	std::string st_tax = StringUtils::format("%s(%.2f%%):",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(),m_pLastPriceResult->tax * 100.0 / FEE);
	text_tax->setString(st_tax);

	if (nbag < goodsNum)
	{
		goodsNum = nbag;
	}
	
	text_name->setString(getItemName(m_pLastPriceResult->itemid));
	image_item->loadTexture(getItemIconPath(m_pLastPriceResult->itemid));
	text_price->setString(numSegment(String::createWithFormat("%lld",avergePrice)->_string));
	text_weght->setString(String::createWithFormat("%0.2f",1.0*m_pLastPriceResult->weight/100)->_string);

	auto i_bgImage = button_icon_bg->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(button_icon_bg,m_pLastPriceResult->itemid,m_pLastPriceResult->itemtype);
	setBgImageColorFormIdAndType(i_bgImage,m_pLastPriceResult->itemid,m_pLastPriceResult->itemtype);
	setTextColorFormIdAndType(text_name,m_pLastPriceResult->itemid,m_pLastPriceResult->itemtype);

	p_name->setLayoutType(Layout::Type::HORIZONTAL);
	p_name->requestDoLayout();

	UIVoyageSlider* ss = UIVoyageSlider::create(panel_slider,goodsNum,0,true);
	ss->setCurrentNumber(1);
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIExchange::buyingSliderEvent,this));
	ss->setTag(1);
	this->addChild(ss);
//chengyuan++
	text_price->ignoreContentAdaptWithSize(true);
	auto  b_btninfo = panel_buy_goods->getChildByName("button_price_info");
	b_btninfo->setPositionX(text_price->getPositionX() + text_price->getContentSize().width + b_btninfo->getContentSize().width/2 + 10);
}

void UIExchange::openSellDialog()
{
	GetPersonalItemResult *result = (GetPersonalItemResult*)m_pResult;

	Node *panel_sellDialog = nullptr;
	bool ispopular = false;
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(result->itemtype,m_pLastPriceResult->itemid,itemName,itemPath);	

	if (result->itemtype == ITEM_TYPE_GOOD)
	{
		ispopular = result->goods[m_nItemIndex]->ispopular;
	}
	
	if (ispopular)
	{
		openView(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODSPOPULAR_CSB]);
		panel_sellDialog = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODSPOPULAR_CSB]);
		auto panel_sellGoods = panel_sellDialog->getChildByName("panel_sell_goods");
		auto button_good_bg = panel_sellGoods->getChildByName<Button*>("button_good_bg");
		auto image_goods = dynamic_cast<ImageView*>(button_good_bg->getChildByName<Widget*>("image_goods"));
		auto text_name = dynamic_cast<Text*>(panel_sellGoods->getChildByName<Widget*>("label_name"));
		auto image_select_bg = panel_sellGoods->getChildByName<ImageView*>("image_select_num_bg");
		auto panel_select_bg = image_select_bg->getChildByName<Widget*>("panel_select_num");
		auto text_count = panel_select_bg->getChildByName<Text*>("label_count");

		image_goods->loadTexture(itemPath);
		text_name->setString(itemName);

		auto i_bgImage = button_good_bg->getChildByName<ImageView*>("image_item_bg_lv");
		setBgButtonFormIdAndType(button_good_bg,m_pLastPriceResult->itemid,result->itemtype);
		setBgImageColorFormIdAndType(i_bgImage,m_pLastPriceResult->itemid,result->itemtype);
		setTextColorFormIdAndType(text_name,m_pLastPriceResult->itemid,result->itemtype);

		auto btn_price_1 = panel_sellGoods->getChildByName<Button*>("button_price_r");
		auto btn_price_2 = panel_sellGoods->getChildByName<Button*>("button_price");
		auto text_price_1 = btn_price_1->getChildByName<Text*>("label_price_num");
		auto text_price_2 = btn_price_2->getChildByName<Text*>("label_price_num");

		btn_price_1->setBright(false);
		btn_price_2->setBright(true);
		m_bIsSellPopular = true;
		btn_price_1->addTouchEventListener(CC_CALLBACK_2(UIExchange::sellBtnEventPopular,this));
		btn_price_2->addTouchEventListener(CC_CALLBACK_2(UIExchange::sellBtnEventPopular,this));
		btn_price_1->getChildByName<Button*>("button_price_info_1")->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func,this));
		btn_price_2->getChildByName<Button*>("button_price_info_2")->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func,this));

		UIVoyageSlider* ss1 = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(100));
		if (ss1)
		{
			ss1->removeFromParentAndCleanup(true);
		}
		UIVoyageSlider* ss = UIVoyageSlider::create(panel_select_bg,result->goods[m_nItemIndex]->amount,0,true);
		int index = 0;
		for (int i = 1; i < m_pLastPriceResult->n_npconlyprices; i++)
		{
			if(m_pLastPriceResult->npconlyprices[i]->price > m_pLastPriceResult->npconlyprices[index]->price)
			{
				index = i;
			}
		}
		
		ss->setCurrentNumber(m_pLastPriceResult->npconlyprices[index]->count);
		ss->setTag(100);
		ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIExchange::sellingSliderEvent,this));
		this->addChild(ss);

		m_vSkillDefine.clear();
		if (m_pLastPriceResult->skill_tax_protocol > 0)
		{
			SKILL_DEFINE skillDefine;
			skillDefine.id = SKILL_TAX_PROTOCOL;
			skillDefine.lv = m_pLastPriceResult->skill_tax_protocol;
			skillDefine.skill_type = SKILL_TYPE_PLAYER;
			skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
			m_vSkillDefine.push_back(skillDefine);
		}

		if (m_pLastPriceResult->skill_expert_sellor > 0)
		{
			SKILL_DEFINE skillDefine;
			skillDefine.id = SKILL_EXPERT_SELLOR;
			skillDefine.lv = m_pLastPriceResult->skill_expert_sellor;
			skillDefine.skill_type = SKILL_TYPE_PLAYER;
			skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
			m_vSkillDefine.push_back(skillDefine);
		}

		if (m_pLastPriceResult->captain_skill_expert_sellor > 0)
		{
			SKILL_DEFINE skillDefine;
			skillDefine.id = SKILL_CAPTAIN_BEST_SELLOR;
			skillDefine.lv = m_pLastPriceResult->captain_skill_expert_sellor;
			if (m_pLastPriceResult->captain_skill_expert_sellor_captain_type == 1)
			{
				skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
			}
			else
			{
				skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
			}
			skillDefine.icon_id = m_pLastPriceResult->captain_skill_expert_sellor_captain_id;
			m_vSkillDefine.push_back(skillDefine);
		}

		if (m_pLastPriceResult->caption_skill_good_reputation > 0)
		{
			SKILL_DEFINE skillDefine;
			skillDefine.id = SKILL_CAPTAIN_TRANSACTION_REPUTAION;
			skillDefine.lv = m_pLastPriceResult->caption_skill_good_reputation;
			if (m_pLastPriceResult->captain_skill_good_reputation_captain_type == 1)
			{
				skillDefine.skill_type = SKILL_TYPE_CAPTAIN;
			}
			else
			{
				skillDefine.skill_type = SKILL_TYPE_COMPANION_NORMAL;
			}
			skillDefine.icon_id = m_pLastPriceResult->caption_skill_good_reputation_captain_id;
			m_vSkillDefine.push_back(skillDefine);
		}

		if (m_pLastPriceResult->captain_special_skill_about_decrease_tax_level > 0)
		{
			SKILL_DEFINE skillDefine;
			skillDefine.id = m_pLastPriceResult->captain_special_skill_about_decrease_tax_id;
			skillDefine.lv = m_pLastPriceResult->captain_special_skill_about_decrease_tax_level;
			skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
			skillDefine.icon_id = m_pLastPriceResult->captain_special_skill_about_decrease_tax_captain_id;
			m_vSkillDefine.push_back(skillDefine);
		}

		auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(image_select_bg, "image_skill_bg"));
		image_skill->setVisible(false);
		for (size_t i = 0; i < m_vSkillDefine.size(); i++)
		{
			if (i == 0)
			{
				image_skill->ignoreContentAdaptWithSize(false);
				image_skill->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
				image_skill->setVisible(true);
				image_skill->setTag(i);
				image_skill->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func,this));
				auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill, "text_item_skill_lv"));
				setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
				if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
				{
					text_skill_lv->setVisible(false);
				}
				else
				{
					text_skill_lv->setVisible(true);
				}
			}
			else
			{
				auto image_skill_clone = dynamic_cast<ImageView*>(image_skill->clone());
				image_skill_clone->ignoreContentAdaptWithSize(false);
				image_skill_clone->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
				image_skill_clone->setVisible(true);
				image_skill_clone->setTag(i);
				image_skill_clone->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func,this));
				auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill_clone, "text_item_skill_lv"));
				setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
				image_skill_clone->setPositionX(image_skill->getPositionX() - i*1.1*image_skill->getBoundingBox().size.width);
				image_select_bg->addChild(image_skill_clone);
				if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
				{
					text_skill_lv->setVisible(false);
				}
				else
				{
					text_skill_lv->setVisible(true);
				}
			}
		}

		auto text_tax = dynamic_cast<Text*>(Helper::seekWidgetByName((Widget*)image_select_bg,"label_tax"));
		std::string st_tax = StringUtils::format("%s(%.2f%%):",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(),m_pLastPriceResult->tax * 100.0 / FEE);
		text_tax->setString(st_tax);

	}else
	{
		openView(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODS_CSB]);
		panel_sellDialog = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODS_CSB]);
		auto i_icon = panel_sellDialog->getChildByName<ImageView*>("panel_sell_goods");
		auto i_icon1 = i_icon->getChildByName<ImageView*>("panel_buy_goods");
		auto i_iconglod = i_icon->getChildByName<ImageView*>("image_goldcoin_1");
		i_iconglod->setTouchEnabled(true);
		i_iconglod->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this)); 
		i_iconglod->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);
		auto i_icon2 = i_icon1->getChildByName<ImageView*>("image_select_num_bg");
		auto i_icon3 = i_icon2->getChildByName<ImageView*>("panel_cost_num");
		auto i_icon4 = i_icon3->getChildByName<ImageView*>("image_silver");
		i_icon4->setTouchEnabled(true);
		i_icon4->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
		i_icon4->setTag(IMAGE_ICON_COINS + IMAGE_INDEX1);
		auto i_icon5 = i_icon2->getChildByName<ImageView*>("panel_profit_num");
		auto i_icon6 = i_icon5->getChildByName<ImageView*>("image_goldcoin_1");
		auto i_icon7 = i_icon2->getChildByName<ImageView*>("panel_tax_num");
		auto i_icon8 = i_icon7->getChildByName<ImageView*>("image_goldcoin_1");
		i_icon6->setTouchEnabled(true);
		i_icon6->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
		i_icon6->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);
		i_icon8->setTouchEnabled(true);
		i_icon8->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
		i_icon8->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);

		auto panel_sellGoods = panel_sellDialog->getChildByName("panel_buy_goods");
		auto button_good = panel_sellGoods->getChildByName<Button*>("button_good_bg");
		auto image_goods = dynamic_cast<ImageView*>(button_good->getChildByName<Widget*>("image_goods"));
		auto text_name = dynamic_cast<Text*>(panel_sellGoods->getChildByName<Widget*>("label_name"));
		auto image_select_bg = panel_sellGoods->getChildByName("image_select_num_bg");
		auto text_price = panel_sellGoods->getChildByName<Text*>("label_price_num");

		image_goods->loadTexture(itemPath);
		
		if (result->itemtype == ITEM_TYPE_SHIP)
		{
			if (result->ships[m_nItemIndex]->user_define_name && std::strcmp(result->ships[m_nItemIndex]->user_define_name, "") != 0)
			{
				itemName = result->ships[m_nItemIndex]->user_define_name;
			}
		}

		text_name->setString(itemName);
		
		auto i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
		setBgButtonFormIdAndType(button_good,m_pLastPriceResult->itemid,result->itemtype);
		setBgImageColorFormIdAndType(i_bgImage,m_pLastPriceResult->itemid,result->itemtype);
		setTextColorFormIdAndType(text_name,m_pLastPriceResult->itemid,result->itemtype);

		auto panel_select_num = image_select_bg->getChildByName<Widget*>("panel_select_num");
		int count = 0;
		if (result->itemtype == ITEM_TYPE_GOOD)
		{
			count = result->goods[m_nItemIndex]->amount;
		}else if (result->itemtype == ITEM_TYPE_SHIP)
		{
			count = 1;
		}else if (result->itemtype == ITEM_TYPE_DRAWING)
		{
			count = result->drawings[m_nItemIndex]->amount;
		}else if (result->itemtype < ITEM_TYPE_SPECIAL)
		{
			count = result->equipments[m_nItemIndex]->amount;
		}else if(result->itemtype == ITEM_TYPE_SPECIAL)
		{
			count = result->gold;
		}

		int index = 0;
		for (int i = 1; i < m_pLastPriceResult->n_prices; i++)
		{
			if(m_pLastPriceResult->prices[i]->price > m_pLastPriceResult->prices[index]->price)
			{
				index = i;
			}
		}
		
		UIVoyageSlider* ss1 = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(101));
		if (ss1)
		{
			ss1->removeFromParentAndCleanup(true);
		}
		UIVoyageSlider* ss = UIVoyageSlider::create(panel_select_num,count,0,true);
		ss->setCurrentNumber(m_pLastPriceResult->prices[index]->count);
		ss->setTag(101);
		ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIExchange::sellingSliderEvent,this));
		this->addChild(ss);

		auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName((Widget*)panel_sellDialog,"image_skill_bg"));
		image_skill->setVisible(false);

		m_vSkillDefine.clear();
		for (int i = 0; i < m_pLastPriceResult->n_skills; i++)
		{
			if (m_pLastPriceResult->skills[i]->id == SKILL_TAX_PROTOCOL)
			{
				SKILL_DEFINE skillDefine;
				skillDefine.id = SKILL_TAX_PROTOCOL;
				skillDefine.lv = m_pLastPriceResult->skills[i]->level;
				skillDefine.skill_type = SKILL_TYPE_PLAYER;
				skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
				m_vSkillDefine.push_back(skillDefine);
				break;
			}
		}

		if (m_pLastPriceResult->captain_special_skill_about_decrease_tax_level > 0)
		{
			SKILL_DEFINE skillDefine;
			skillDefine.id = m_pLastPriceResult->captain_special_skill_about_decrease_tax_id;
			skillDefine.lv = m_pLastPriceResult->captain_special_skill_about_decrease_tax_level;
			skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
			skillDefine.icon_id = m_pLastPriceResult->captain_special_skill_about_decrease_tax_captain_id;
			m_vSkillDefine.push_back(skillDefine);
		}

		for (size_t i = 0; i < m_vSkillDefine.size(); i++)
		{
			if (i == 0)
			{
				image_skill->ignoreContentAdaptWithSize(false);
				image_skill->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
				image_skill->setVisible(true);
				image_skill->setTag(i);
				image_skill->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
				auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill, "text_item_skill_lv"));
				setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
				if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
				{
					text_skill_lv->setVisible(false);
				}
				else
				{
					text_skill_lv->setVisible(true);
				}
			}
			else
			{
				auto image_skill_clone = dynamic_cast<ImageView*>(image_skill->clone());
				image_skill_clone->ignoreContentAdaptWithSize(false);
				image_skill_clone->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
				image_skill_clone->setVisible(true);
				image_skill_clone->setTag(i);
				image_skill_clone->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
				auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill_clone, "text_item_skill_lv"));
				setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
				image_skill_clone->setPositionX(image_skill->getPositionX() - i*1.1*image_skill->getBoundingBox().size.width);
				image_select_bg->addChild(image_skill_clone);
				if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
				{
					text_skill_lv->setVisible(false);
				}
				else
				{
					text_skill_lv->setVisible(true);
				}
			}
		}
	}


}

void UIExchange::openBuyShipDialog()
{
	int nGoodsNumber = 100;
	nGoodsNumber = getMaxNumber(m_pLastPriceResult->prices,m_pLastPriceResult->n_prices,SINGLE_HERO->m_iCoin,m_pLastPriceResult->tax*1.0  / FEE);
	if (nGoodsNumber < 1)
	{
		UIInform::getInstance()->openInformView(this);
		UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
		return;
	}

	openView(MARKET_COCOS_RES[MARKET_BUY_VIEWSHIP_CSB]);
	auto panel_buy = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_VIEWSHIP_CSB]);
	auto i_icon_bg = panel_buy->getChildByName<ImageView*>("listview_price");
	auto i_gol1 = i_icon_bg->getChildByName<ImageView*>("image_goldcoin_1");
	i_gol1->setTouchEnabled(true); 
	i_gol1->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
	i_gol1->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);

	auto i_icon_bg1= panel_buy->getChildByName<ImageView*>("image_select_num_bg");
	auto i_goll = i_icon_bg1->getChildByName<ImageView*>("panel_tax_num");
	auto i_gol2 = i_goll->getChildByName<ImageView*>("image_goldcoin_1");
	i_gol2->setTouchEnabled(true); 
	i_gol2->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
	i_gol2->setTag(IMAGE_ICON_COINS + IMAGE_INDEX2);
	auto i_gol3 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_buy, "image_silver"));
	i_gol3->setTouchEnabled(true);
	i_gol3->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
	i_gol3->setTag(IMAGE_ICON_COINS + IMAGE_INDEX1);
	auto panel_buy_goods = panel_buy->getChildByName("panel_buy_goods");
	auto image_subRoot = panel_buy_goods->getChildByName("image_select_num_bg");
	auto panel_slider = image_subRoot->getChildByName<Widget*>("panel_select_num");
	auto listview_price = dynamic_cast<ListView*>(panel_buy_goods->getChildByName("listview_price"));
	auto p_name = panel_buy_goods->getChildByName<Layout*>("panel_name");
	auto text_name = p_name->getChildByName<Text*>("label_name");
	auto text_price = dynamic_cast<Text*>(listview_price->getItem(1));
	auto button_good_bg = panel_buy_goods->getChildByName<Button*>("button_good_bg");
	auto image_goods = button_good_bg->getChildByName<ImageView*>("image_goods");
	auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_buy,"image_skill_bg"));
	image_skill->setVisible(false);
	auto text_tax = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_buy,"label_tax"));
	std::string st_tax = StringUtils::format("%s(%.2f%%):",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(),m_pLastPriceResult->tax * 100.0 / FEE);
	text_tax->setString(st_tax);

	auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_buy,"text_item_skill_lv"));
	int num = 0;
	for (int i = 0; i < m_pLastPriceResult->n_skills; i++)
	{
		if (m_pLastPriceResult->skills[i]->id == SKILL_TAX_PROTOCOL)
		{
			num = m_pLastPriceResult->skills[i]->level;
			m_vSkillDefine.clear();
			SKILL_DEFINE skillDefine;
			skillDefine.id = SKILL_TAX_PROTOCOL;
			skillDefine.lv = num;
			skillDefine.skill_type = SKILL_TYPE_PLAYER;
			skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
			m_vSkillDefine.push_back(skillDefine);
			break;
		}
	}

	if (m_pLastPriceResult->captain_special_skill_about_decrease_tax_level > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = m_pLastPriceResult->captain_special_skill_about_decrease_tax_id;
		skillDefine.lv = m_pLastPriceResult->captain_special_skill_about_decrease_tax_level;
		skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
		skillDefine.icon_id = m_pLastPriceResult->captain_special_skill_about_decrease_tax_captain_id;
		m_vSkillDefine.push_back(skillDefine);
	}

	for (size_t i = 0; i < m_vSkillDefine.size(); i++)
	{
		if (i == 0)
		{
			image_skill->ignoreContentAdaptWithSize(false);
			image_skill->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill->setVisible(true);
			image_skill->setTag(i);
			image_skill->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
			{
				text_skill_lv->setVisible(false);
			}
			else
			{
				text_skill_lv->setVisible(true);
			}
		}
		else
		{
			auto image_skill_clone = dynamic_cast<ImageView*>(image_skill->clone());
			image_skill_clone->ignoreContentAdaptWithSize(false);
			image_skill_clone->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill_clone->setVisible(true);
			image_skill_clone->setTag(i);
			image_skill_clone->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill_clone, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			image_skill_clone->setPositionX(image_skill->getPositionX() - i*1.1*image_skill->getBoundingBox().size.width);
			image_subRoot->addChild(image_skill_clone);
			if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
			{
				text_skill_lv->setVisible(false);
			}
			else
			{
				text_skill_lv->setVisible(true);
			}
		}
	}
		
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(m_pLastPriceResult->itemtype,m_pLastPriceResult->itemid,itemName,itemPath);
	int64_t nPrice = ceil(getAveragePrice(m_pLastPriceResult->prices,m_pLastPriceResult->n_prices,1));

	if (nGoodsNumber > 99)
	{
		nGoodsNumber = 99;
	}
	auto ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
	if (ss)
	{
		ss->removeFromParentAndCleanup(true);
	}
	ss = UIVoyageSlider::create(panel_slider,nGoodsNumber,0,true);
	ss->setCurrentNumber(1);
	text_name->setString(itemName);

	auto i_bgImage = button_good_bg->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(button_good_bg,m_pLastPriceResult->itemid,m_pLastPriceResult->itemtype);
	setBgImageColorFormIdAndType(i_bgImage,m_pLastPriceResult->itemid,m_pLastPriceResult->itemtype);
	setTextColorFormIdAndType(text_name,m_pLastPriceResult->itemid,m_pLastPriceResult->itemtype);

	p_name->setLayoutType(Layout::Type::HORIZONTAL);
	p_name->requestDoLayout();
	image_goods->loadTexture(itemPath);
	text_price->setString(numSegment(StringUtils::format("%lld",nPrice)));
	
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIExchange::buyingSliderEvent,this));
	ss->setTag(1);
	this->addChild(ss);
	listview_price->requestRefreshView();

//chengyuan++
	auto item_1 = (Text*)listview_price->getItem(1);
	item_1->ignoreContentAdaptWithSize(true);
}

static void quick_sort_price(BeingSoldItemsDefine**items,int start,int end)
{
	if (start < end) {
		int i = start;
		int j = end;
		int64_t x = items[i]->lastprice;
		BeingSoldItemsDefine*itemx = items[i];
		while (i < j) {
			while (i < j && items[j]->lastprice > x)
				j--;
			if (i < j) {
				items[i] = items[j];
				i++;
			}
			while (i < j && items[i]->lastprice < x)
				i++;
			if (i < j) {
				items[j] = items[i];
				j--;
			}
		}
		items[i] = itemx;
		quick_sort_price(items, start, i - 1);
		quick_sort_price(items, i + 1, end);
	}
}

void UIExchange::onServerEvent(struct ProtobufCMessage* message,int eMsgType)
{
	UIBasicLayer::onServerEvent(message,eMsgType);
	switch (eMsgType)
	{
	case PROTO_TYPE_CancelOrderResult:
		{
			CancelOrderResult *result = ( CancelOrderResult*)message;
			if(result->failed == 0)
			{
				if (m_bIsSellOrder)
				{
					ProtocolThread::GetInstance()->getOrderList(1,UILoadingIndicator::create(this));
				}else
				{
					ProtocolThread::GetInstance()->getOrderList(0,UILoadingIndicator::create(this));
				}
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_EXCANCEL_ORDER_SUCCESS");
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCANCEL_ORDER_FAIL");
			}
			cancel_order_result__free_unpacked(result,0);
			break;
		}           
	case PROTO_TYPE_BuyItemResult:
		{
			BuyItemResult *result = ( _BuyItemResult*)message;
			if(result->failed == 0)
			{
				updateCoinNum(result->usergolds,result->usercoins);
				buyItemSuccess(result);
				ProtocolThread::GetInstance()->getItemsBeingSold(m_pMinorButton->getTag() - BUY_GOOD_INDEX,UILoadingIndicator::create(this));
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCANCEL_BUY_FAIL");
			}
			buy_item_result__free_unpacked(result,0);
			break;
		}
	case PROTO_TYPE_SellItemResult:
		{
			SellItemResult *result = (SellItemResult*)message;
			if(result->failed == 0)
			{
				if (result->count != result->reqcount)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_EXCHANGE_SELL_NOT_WHOLE");	
				}else
				{
					m_pSellItemResult = result;
					if (result->newlevel)
					{
						m_bIsLevelUp = true;
						SINGLE_HERO->m_iLevel = result->newlevel;
					}
					if (result->new_fame_level)
					{
						m_bIsPrestigeUp = true;
						SINGLE_HERO->m_iPrestigeLv = result->new_fame_level;
					}
					if (result->n_captains > 0)
					{
						for (int i = 0; i< result->n_captains; i++)//有船长增加的经验大于0是才显示界面
						{
							if (result->captains[i]->lootexp > 0)
							{
								m_bIsCaptainUp = true;
								break;
							}
							else
							{
								m_bIsCaptainUp = false;
							}
						}
					}
				
					updateCoinNum(result->usergolds,result->usercoins);
					sellItemSuccess(result);
					int index = m_pMinorButton->getTag() - SELL_GOOD_INDEX;
					ProtocolThread::GetInstance()->getPersonalItems(index,1,UILoadingIndicator::create(this));
				}	
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCANCEL_SELL_FAIL");
			}
			break;
		}
	case PROTO_TYPE_CreateOrderResult:
		{
			CreateOrderResult	*result = (CreateOrderResult *)message;
			if (result->failed == 0)
			{
				closeView();
				closeView();
				if (m_bIsSellOrder)
				{
					createOrderSellSuccess();
					ProtocolThread::GetInstance()->getOrderList(1,UILoadingIndicator::create(this));
				}else
				{
					createOrderBuySuccess();
					ProtocolThread::GetInstance()->getOrderList(0,UILoadingIndicator::create(this));

				}
			}else if (result->failed == 101||result->failed == 14)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_ORDER_COIN_NOT_FULL");
			}
			else if (result->failed == 25)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_ORDER_DURABILITY_EMPTY");
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_ORDER_FAIL");
			}
			create_order_result__free_unpacked(result,0);
			break;
		}
	case PROTO_TYPE_GetItemsBeingSoldResult:
		{
			GetItemsBeingSoldResult *result = (GetItemsBeingSoldResult*)message;
			if(result->failed == 0)
			{		
				m_pResult = message;
				SINGLE_HERO->m_iGold = result->gold;
				SINGLE_HERO->m_iCoin = result->coin;
				if(result->itemtype != 0 && result->n_items > 1){
					std::vector<BeingSoldItemsDefine*> items(result->items,&result->items[result->n_items]);
//					for(auto iter=items.begin();iter != items.end();iter++){
//						//log("BeingSoldItemsDefine vector size:%d --%d---value:%lld",items.size(),result->n_items,(*iter)->lastprice);
//					}

					std::sort(items.begin(),items.end(),[](BeingSoldItemsDefine* A, BeingSoldItemsDefine*B){ return A->lastprice < B->lastprice;});
					//std::copy(items.begin(), items.end(), result->items);
					int i = 0;
					for(auto iter=items.begin();iter != items.end();iter++){
						result->items[i++] = *iter;
					}
				}
				//this->scheduleOnce(schedule_selector(ExchangeLayer::delayUpdateBuyView),0.02f);
				delayUpdateBuyView(0);
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_BUY_LIST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetOrderListResult:
		{
			GetOrderListResult* result = (GetOrderListResult*)message;
			if (result->failed == 0)
			{
				m_pOrderlistResult = result;
				m_pResult = message;
				updateCoinNum(result->gold,result->coin);
				updateOrderView((GetOrderListResult*)m_pResult);
			}else if (result->failed == 10)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_ORDER_LIST_FULL");
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_ORDER_LIST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetRecentOrderResult:
		{
			GetRecentOrderResult* result = (GetRecentOrderResult*)message;
			if (result->failed == 0)
			{
				m_pRecentOrderResult = result;
				m_pResult = message;
				updateRecentOrderView((GetRecentOrderResult*)m_pResult);
			}
			else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_ORDER_LIST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetItemsToSellResult:
		{
			GetItemsToSellResult *result = (GetItemsToSellResult*)message;
			if (result->failed == 0)
			{
				if (m_nPageIndex == PAGE_ORDER_INDEX)
				{
					updateSellOrderSelectedItemView(result);
				}
				m_pGetItemToSell = result;
				m_pResult = message;
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_SELL_LIST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetPriceDataResult:
		{
			GetPriceDataResult *result = (GetPriceDataResult*)message;
			if(result->failed != 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_GET_PRICE_FAIL");
				return;
			}
			m_pLastPriceResult = result;
			if(m_nPageIndex == PAGE_BUY_INDEX)
			{
				if (result->count < 1)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_EXCHANE_NOT_BUY");
					return;
				}
				distributeBuyEvent();
			}else
			{
				if (result->count < 1)
				{
					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_NOT_BUY_ORDER");
					return;
				}
				openSellDialog();
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
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_SELL_LIST_FAIL");
			}
			break;
		}
	case PROTO_TYPE_GetItemsDetailInfoResult:
	{
		GetItemsDetailInfoResult *result = (GetItemsDetailInfoResult*)message;
		if(result->failed == 0)
		{
			openGoodInfo(result,result->itemtype,result->itemid);
		}else
		{
			//InformView::getInstance()->openInformView(this);
			//InformView::getInstance()->openConfirmYes("TIP_EXCANCEL_ORDER_FAIL");
		}
		break;
	} 
	case PROTO_TYPE_GetCityProducesResult:
	{
		GetCityProducesResult *result = (GetCityProducesResult*)message;
		if(result->failed == 0)
		{
			cityBuyInfo(result);
		}else
		{
			//InformView::getInstance()->openInformView(this);
			//InformView::getInstance()->openConfirmYes("TIP_EXCANCEL_ORDER_FAIL");
		}
		break;
	} 
	case PROTO_TYPE_GetCityDemandsResult:
	{
		GetCityDemandsResult *result = (GetCityDemandsResult*)message;
		if(result->failed == 0)
		{
			citySellInfo(result);
		}else
		{
			//InformView::getInstance()->openInformView(this);
			//InformView::getInstance()->openConfirmYes("TIP_EXCANCEL_ORDER_FAIL");
		}
		break;
	} 
	case PROTO_TYPE_ExpandPackageSizeResult:
	{
		ExpandPackageSizeResult *result = (ExpandPackageSizeResult*)message;
		if (result->failed == 0)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_CENTER_ITEM_BAG_EXPAND_RESULT");
			updateCoinNum(result->gold, result->coin);
		}
		else if (result->failed == COIN_NOT_FAIL)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
		}
		break;
	}
	default:
		break;
	}
}

void UIExchange::buyItemSuccess(const BuyItemResult *result)
{
	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto t_title = view->getChildByName<Text*>("label_title");
	auto p_result = view->getChildByName<Widget*>("panel_result");
	auto l_result = p_result->getChildByName<ListView*>("listview_result");
	auto p_buy = p_result->getChildByName<Widget*>("panel_buy_3");
	auto p_silver = p_result->getChildByName<Widget*>("panel_silver_2");
	auto i_div = p_result->getChildByName<ImageView*>("image_div_1");
	
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_RESULT_TITLE"]);
	auto p_buy_clone = p_buy->clone();
	auto t_buy = p_buy_clone->getChildByName<Text*>("label_buy");
	auto t_item = p_buy_clone->getChildByName<Text*>("label_items_name");
	auto t_buyNum = p_buy_clone->getChildByName<Text*>("label_buy_num");
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(result->itemtype,result->itemid,itemName,itemPath);
	int amount = result->count;
	t_buy->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_BUY_RESULT"]);
	auto nMaxStringNum = 0;
	auto languageTypeNum = 0;
	if (isChineseCharacterIn(itemName.c_str()))
	{
		languageTypeNum = 1;
	}
	if (languageTypeNum)
	{
		nMaxStringNum = 16;
	}
	else
	{
		nMaxStringNum = 30;
	}
	t_item->setString(apostrophe(itemName, nMaxStringNum));
		t_item->setString(vticItemName(itemName));
	t_buyNum->setString(StringUtils::format("x %d",amount));
	l_result->pushBackCustomItem(p_buy_clone);
	auto i_div1 = i_div->clone();
	l_result->pushBackCustomItem(i_div1);

	auto p_tax = p_silver->clone();
	auto t_tax = p_tax->getChildByName<Text*>("label_force_relation");
	auto t_taxNum = p_tax->getChildByName<Text*>("label_buy_num");
	auto i_silver_tax = p_tax->getChildByName<ImageView*>("image_silver");
	float tax = float(m_pLastPriceResult->tax * 100.0 / FEE);
	t_tax->setString(StringUtils::format("%s(%.2f%%):",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(),tax));
	t_taxNum->setString(numSegment(StringUtils::format("%lld",result->taxamount)));
	i_silver_tax->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_tax->setPositionX(t_taxNum->getPositionX() - t_taxNum->getBoundingBox().size.width - i_silver_tax->getBoundingBox().size.width / 2 - 5);

	l_result->pushBackCustomItem(p_tax);
	auto i_div2 = i_div->clone();
	l_result->pushBackCustomItem(i_div2);

	auto p_cost = p_silver->clone();
	auto t_cost = p_cost->getChildByName<Text*>("label_force_relation");
	auto t_costNum = p_cost->getChildByName<Text*>("label_buy_num");
	auto i_silver_cost = p_cost->getChildByName<ImageView*>("image_silver");
	t_cost->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILOR_RECRUIT_COST"]);
	t_costNum->setString(numSegment(StringUtils::format("%lld",result->totalprice)));
	i_silver_cost->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_cost->setPositionX(t_costNum->getPositionX() - t_costNum->getBoundingBox().size.width - i_silver_cost->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_cost);
	auto i_div3 = i_div->clone();
	l_result->pushBackCustomItem(i_div3);

	auto b_yes = view->getChildByName<Button*>("button_result_yes");
	b_yes->addTouchEventListener(CC_CALLBACK_2(UIExchange::closeBuySellSuccessView,this));

	l_result->setPositionY(l_result->getPositionY() - p_silver->getBoundingBox().size.height/2);

// chengyuan++
	for (auto i : l_result->getItems())
	{
		if (i->getName().find("panel_buy_3") == 0)
		{
			auto t_iname = (Text*)i->getChildByName("label_items_name");
			auto t_inum = (Text*)i->getChildByName("label_buy_num");
			t_iname->setPositionX(t_inum->getPositionX() - t_inum->getContentSize().width-t_iname->getContentSize().width/2-10);
		}
	}
}

void UIExchange::sellItemSuccess(const SellItemResult *result)
{
	UICommon::getInstance()->openCommonView(this);
	UICommon::getInstance()->flushPlayerAddExpOrFrame(result->exp,result->fame,result->expadded,result->fameadded);

	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto t_title = view->getChildByName<Text*>("label_title");
	auto p_result = view->getChildByName<Widget*>("panel_result");
	auto l_result = p_result->getChildByName<ListView*>("listview_result");
	auto p_buy = p_result->getChildByName<Widget*>("panel_buy_3");
	auto p_silver = p_result->getChildByName<Widget*>("panel_silver_2");
	auto i_div = p_result->getChildByName<ImageView*>("image_div_1");

	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_RESULT_TITLE"]);
	auto p_buy_clone = p_buy->clone();
	auto t_buy = p_buy_clone->getChildByName<Text*>("label_buy");
	auto t_item = p_buy_clone->getChildByName<Text*>("label_items_name");
	auto t_buyNum = p_buy_clone->getChildByName<Text*>("label_buy_num");
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(result->itemtype,result->itemid,itemName,itemPath);
	int amount = result->count;
	t_buy->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_SELL_RESULT"]);
	auto nMaxStringNum = 0;
	auto languageTypeNum = 0;
	if (isChineseCharacterIn(itemName.c_str()))
	{
		languageTypeNum = 1;
	}
	if (languageTypeNum)
	{
		nMaxStringNum = 16;
	}
	else
	{
		nMaxStringNum = 30;
	}
	t_item->setString(apostrophe(itemName, nMaxStringNum));
	t_item->setString(vticItemName(itemName));
	t_buyNum->setString(StringUtils::format("x %d",amount));
	l_result->pushBackCustomItem(p_buy_clone);
	auto i_div1 = i_div->clone();
	l_result->pushBackCustomItem(i_div1);

	auto p_tax = p_silver->clone();
	auto t_tax = p_tax->getChildByName<Text*>("label_force_relation");
	auto t_taxNum = p_tax->getChildByName<Text*>("label_buy_num");
	auto i_silver_tax = p_tax->getChildByName<ImageView*>("image_silver");
	float tax = float(m_pLastPriceResult->tax * 100.0 / FEE);
	t_tax->setString(StringUtils::format("%s(%.2f%%):",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(),tax));
	t_taxNum->setString(numSegment(StringUtils::format("%lld",result->taxamount)));
	i_silver_tax->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_tax->setPositionX(t_taxNum->getPositionX() - t_taxNum->getBoundingBox().size.width - i_silver_tax->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_tax);
	auto i_div2 = i_div->clone();
	l_result->pushBackCustomItem(i_div2);

	auto p_income = p_silver->clone();
	auto t_income = p_income->getChildByName<Text*>("label_force_relation");
	auto t_incomeNum = p_income->getChildByName<Text*>("label_buy_num");
	auto i_silver_income = p_income->getChildByName<ImageView*>("image_silver");
	t_income->setString(StringUtils::format("%s:",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_SELL_COST_MARK_TITLE"].data()));
	t_incomeNum->setString(numSegment(StringUtils::format("%lld",result->totalprice)));
	i_silver_income->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_income->setPositionX(t_incomeNum->getPositionX() - t_incomeNum->getBoundingBox().size.width - i_silver_income->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_income);
	auto i_div3 = i_div->clone();
	l_result->pushBackCustomItem(i_div3);

	auto p_profit = p_silver->clone();
	auto t_profit = p_profit->getChildByName<Text*>("label_force_relation");
	auto t_profitNum = p_profit->getChildByName<Text*>("label_buy_num");
	auto i_silver_profit = p_profit->getChildByName<ImageView*>("image_silver");
	t_profit->setString(StringUtils::format("%s:",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_PROFIT"].data()));
	if (result->earnings > 0)
	{
		t_profitNum->setString(numSegment(StringUtils::format("%lld", result->earnings)));
		t_profitNum->setTextColor(Color4B(40, 25, 13, 255));
	}
	else
	{
		auto str = numSegment(StringUtils::format("%lld",-result->earnings));
		t_profitNum->setString(StringUtils::format("-%s",str.c_str()));
		t_profitNum ->setTextColor(Color4B(183, 28, 28, 255));
	}
	i_silver_profit->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_profit->setPositionX(t_profitNum->getPositionX() - t_profitNum->getBoundingBox().size.width - i_silver_profit->getBoundingBox().size.width / 2 - 5);

	l_result->pushBackCustomItem(p_profit);
	auto i_div4 = i_div->clone();
	l_result->pushBackCustomItem(i_div4);

	auto b_yes = view->getChildByName<Button*>("button_result_yes");
	b_yes->addTouchEventListener(CC_CALLBACK_2(UIExchange::closeBuySellSuccessView, this));
	
//chengyuan ++

	for (auto i : l_result->getItems())
	{
		if (i->getName().find("panel_buy_3") == 0)
		{
			auto t_iname = (Text*)i->getChildByName("label_items_name");
			auto t_inum = (Text*)i->getChildByName("label_buy_num");
			t_iname->setPositionX(t_inum->getPositionX() - t_inum->getContentSize().width - t_iname->getContentSize().width/2-10);
		}
	}
}

void UIExchange::createOrderBuySuccess()
{
	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto t_title = view->getChildByName<Text*>("label_title");
	auto p_result = view->getChildByName<Widget*>("panel_result");
	auto l_result = p_result->getChildByName<ListView*>("listview_result");
	auto p_buy = p_result->getChildByName<Widget*>("panel_buy_3");
	auto p_silver = p_result->getChildByName<Widget*>("panel_silver_2");
	auto i_div = p_result->getChildByName<ImageView*>("image_div_1");
	
	auto p_buy_clone = p_buy->clone();
	auto t_buy = p_buy_clone->getChildByName<Text*>("label_buy");
	auto t_item = p_buy_clone->getChildByName<Text*>("label_items_name");
	auto t_buyNum = p_buy_clone->getChildByName<Text*>("label_buy_num");
	std::string itemName;
	std::string itemPath;
	getItemNameAndPath(m_eType,m_nOrderIndex,itemName,itemPath);
	t_buy->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_ORDER_HOLD"]);
	auto nMaxStringNum = 0;
	auto languageTypeNum = 0;
	if (isChineseCharacterIn(itemName.c_str()))
	{
		languageTypeNum = 1;
	}
	if (languageTypeNum)
	{
		nMaxStringNum = 16;
	}
	else
	{
		nMaxStringNum = 30;
	}
	t_item->setString(apostrophe(itemName, nMaxStringNum));
	t_item->setString(vticItemName(itemName));
	t_buyNum->setString(StringUtils::format("x %d",m_nOrderAmount));
	dynamic_cast<Layout*>(p_buy_clone)->requestDoLayout();
	l_result->pushBackCustomItem(p_buy_clone);
	auto i_div1 = i_div->clone();
	l_result->pushBackCustomItem(i_div1);

	auto p_tax = p_silver->clone();
	auto t_tax = p_tax->getChildByName<Text*>("label_force_relation");
	auto t_taxNum = p_tax->getChildByName<Text*>("label_buy_num");
	auto i_silver_tax = p_tax->getChildByName<ImageView*>("image_silver");
	t_tax->setString(StringUtils::format("%s(%.2f%%):", SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(), m_nOrderTax / 100.0));
	t_taxNum->setString(numSegment(StringUtils::format("%lld",int64_t(ceil(m_nOrderPrice * m_nOrderAmount *  m_nOrderTax / 10000.0)))));
	dynamic_cast<Layout*>(p_tax)->requestDoLayout();
	i_silver_tax->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_tax->setPositionX(t_taxNum->getPositionX() - t_taxNum->getBoundingBox().size.width - i_silver_tax->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_tax);
	auto i_div2 = i_div->clone();
	l_result->pushBackCustomItem(i_div2);

	auto p_fee = p_silver->clone();
	auto t_fee = p_fee->getChildByName<Text*>("label_force_relation");
	auto t_feeNum = p_fee->getChildByName<Text*>("label_buy_num");
	auto i_silver_fee = p_fee->getChildByName<ImageView*>("image_silver");
	t_fee->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILOR_RECRUIT_FEE"]);
	t_feeNum->setString(StringUtils::format("%lld",m_nOrderFee));
	dynamic_cast<Layout*>(p_fee)->requestDoLayout();
	i_silver_fee->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_fee->setPositionX(t_feeNum->getPositionX() - t_feeNum->getBoundingBox().size.width - i_silver_fee->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_fee);
	auto i_div3 = i_div->clone();
	l_result->pushBackCustomItem(i_div3);

	auto p_cost = p_silver->clone();
	auto t_cost = p_cost->getChildByName<Text*>("label_force_relation");
	auto t_costNum = p_cost->getChildByName<Text*>("label_buy_num");
	auto i_silver_cost = p_cost->getChildByName<ImageView*>("image_silver");
	t_cost->setString(SINGLE_SHOP->getTipsInfo()["TIP_PUP_SAILOR_RECRUIT_COST"]);
	t_costNum->setString(numSegment(StringUtils::format("%lld",int64_t((m_nOrderPrice  * m_nOrderAmount * (1.0 + m_nOrderTax / 10000.0) + m_nOrderFee)))));
	dynamic_cast<Layout*>(p_cost)->requestDoLayout();
	i_silver_cost->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_cost->setPositionX(t_costNum->getPositionX() - t_costNum->getBoundingBox().size.width - i_silver_cost->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_cost);
	auto i_div4 = i_div->clone();
	l_result->pushBackCustomItem(i_div4);

	for (auto i : l_result->getItems())
	{
		if (i->getName().find("panel_buy_3") == 0)
		{
			auto t_iname = (Text*)i->getChildByName("label_items_name");
			auto t_inum = (Text*)i->getChildByName("label_buy_num");
			t_iname->setPositionX(t_inum->getPositionX() - t_inum->getContentSize().width - t_iname->getContentSize().width / 2 - 10);
		}
	}
}

void UIExchange::closeBuySellSuccessView(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		UICommon::getInstance()->closeView();
		if (m_bIsLevelUp)
		{
			m_bIsLevelUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPlayerLevelUp();
			return;
		}
		if (m_bIsPrestigeUp)
		{
			m_bIsPrestigeUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->flushPrestigeLevelUp();
			return;
		}
		if (m_bIsCaptainUp)
		{
			m_bIsCaptainUp = false;
			UICommon::getInstance()->openCommonView(this);
			UICommon::getInstance()->getCaptainAddexp();
			UICommon::getInstance()->flushCaptainLevelUp(m_pSellItemResult->n_captains, m_pSellItemResult->captains);
			return;
		}
	}
}

void UIExchange::distributeBuyEvent()
{

	int index = m_pMinorButton->getTag() - BUY_GOOD_INDEX;
	switch (index)
	{

	case ITEM_TYPE_DRAWING:   //.....,sixth button
	case ITEM_TYPE_SHIP:   //buy view ,second button
	case ITEM_TYPE_SPECIAL://.....,sixth button
		openBuyShipDialog();
		break;
	case ITEM_TYPE_PORP:  //.....,fourth button
	case ITEM_TYPE_SHIP_EQUIP:  //.....,third button
	case ITEM_TYPE_ROLE_EQUIP:   //.....,fifth button
		openBuyPropsDialog();
		break;
	case ITEM_TYPE_GOOD:   // buy view ,fist button
		{
			int64_t totalNum = 0;
			for (auto i = 0; i < m_pLastPriceResult->n_shipinfos; ++i)
			{	
				if (m_pLastPriceResult->shipinfos[i]->maxsize < m_pLastPriceResult->shipinfos[i]->usedsize)
				{
					m_pLastPriceResult->shipinfos[i]->maxsize = m_pLastPriceResult->shipinfos[i]->usedsize;
				}
				totalNum += (m_pLastPriceResult->shipinfos[i]->maxsize - m_pLastPriceResult->shipinfos[i]->usedsize)/m_pLastPriceResult->weight;
			}
			if (totalNum > 0)
			{
				openBuyDialog();
			}else if (m_pLastPriceResult->n_shipinfos < 1)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCHNAGE_NOT_SHIP");
			}else
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_SHIP_CAP_FULL");
			}
			break;
		}
	default:
		break;
	}

}

void UIExchange::pageview_buttonEvent_1(std::string& name,Widget *target)
{
	//物品价值的解释
	if (isButton(button_cost_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_EXCHANGE_BUY_COST_MARK_TITLE","TIP_EXCHANGE_BUY_COST_MARK_CONTENT");
		return;
	}
	//物品价格的解释
	if (isButton(button_price_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_EXCHANGE_BUY_PRICE_MARK_TITLE","TIP_EXCHANGE_BUY_PRICE_MARK_CONTENT");
		return;
	}
	
	//物品之间切换
	if (isButton(button_goods) || isButton(button_ships) || isButton(button_ship_equipment) || isButton(button_human_equipment)
		|| isButton(button_drawings) || isButton(button_miscs) || isButton(button_special))
	{
		changeMinorButtonState(target);
		int index = target->getTag() - BUY_GOOD_INDEX;
		auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
		auto b_info = view->getChildByName<Button*>("button_info");
		if (index == ITEM_TYPE_GOOD)
		{
			b_info->setVisible(true);
			b_info->addTouchEventListener(CC_CALLBACK_2(UIExchange::buyAndSellInfoBtnEvent, this));
		}else
		{
			b_info->setVisible(false);
		}
		ProtocolThread::GetInstance()->getItemsBeingSold(index,UILoadingIndicator::create(this));
		return;
	}
	//扩展个人背包
	if (isButton(button_s_yes))
	{
		ProtocolThread::GetInstance()->expandPackageSize(UILoadingIndicator::create(this));
		return;
	}
}

void UIExchange::pageview_buttonEvent_2(std::string& name,Widget *target)
{
	if (isButton(button_confirm_yes))
	{
		return;
	}
	//物品价值的解释
	if (isButton(button_cost_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_EXCHANGE_SELL_COST_MARK_TITLE","TIP_EXCHANGE_SELL_COST_MARK_CONTENT");
		return;
	}
	//流行品价格的解释（货物）
	if (isButton(button_price_info_1))
	{	
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_EXCHANGE_SELL_PRICE_MARK_TITLE1","TIP_EXCHANGE_SELL_PRICE_MARK_CONTENT1");
		return;
	}
	//普通物品价格的解释（货物）
	if (isButton(button_price_info_2))
	{	
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_EXCHANGE_SELL_PRICE_MARK_TITLE2","TIP_EXCHANGE_SELL_PRICE_MARK_CONTENT2");
		return;
	}
	//其他物品价格的解释
	if (isButton(button_price_info))
	{	
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_EXCHANGE_SELL_PRICE_MARK_TITLE3","TIP_EXCHANGE_SELL_PRICE_MARK_CONTENT3");
		return;
	}

	//物品之间切换
	if (isButton(button_goods) || isButton(button_ships) || isButton(button_ship_equipment) || isButton(button_human_equipment)
		|| isButton(button_drawings) || isButton(button_miscs) || isButton(button_special))
	{
		changeMinorButtonState(target);
		int index = target->getTag() - SELL_GOOD_INDEX;
		auto view = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]);
		auto b_info = view->getChildByName<Button*>("button_info");
		if (index == ITEM_TYPE_GOOD)
		{
			b_info->setVisible(true);
			b_info->addTouchEventListener(CC_CALLBACK_2(UIExchange::buyAndSellInfoBtnEvent, this));
		}else
		{
			b_info->setVisible(false);
		}
		ProtocolThread::GetInstance()->getPersonalItems(index,1,UILoadingIndicator::create(this));
	}
}

void UIExchange::pageview_buttonEvent_3(std::string& name,Widget *target)
{
	if (isButton(button_confirm_yes)) //取消订单 yes button
	{
		GetOrderListResult *result = (GetOrderListResult*)m_pResult;
		if (!result->orders)
		{
			result = m_pOrderlistResult;
		}
		ProtocolThread::GetInstance()->cancelOrder(result->orders[m_nItemIndex]->itemtype,result->orders[m_nItemIndex]->itemid,
			result->orders[m_nItemIndex]->orderid,UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_confirm_no)) //取消订单 no button
	{
		return;
	}
	//物品价值的解释
	if (isButton(button_cost_info))
	{
		UICommon::getInstance()->openCommonView(this);
		if (m_pMinorButton->getTag() == ORDER_BUY_INDEX)
		{
			UICommon::getInstance()->flushInfoBtnView("TIP_EXCHANGE_BUY_ORDER_COST_MARK_TITLE","TIP_EXCHANGE_BUY_ORDER_COST_MARK_CONTENT");
		}else
		{
		
			UICommon::getInstance()->flushInfoBtnView("TIP_EXCHANGE_SELL_ORDER_COST_MARK_TITLE","TIP_EXCHANGE_SELL_ORDER_COST_MARK_CONTENT");
		}
		return;
	}
	//返回交易所
	if (isButton(button_last_widget))
	{
		closeView();
		return;
	}
	//货物
	if (isButton(button_goods))
	{
		m_eType = ITEM_TYPE_GOOD;
		if (m_bIsSellOrder)
		{
			ProtocolThread::GetInstance()->getItemsToSell(m_eType,0,UILoadingIndicator::create(this));
		}else
		{
			updateBuyOrderSelectedItemView();
		}
		changeOrderButtonState(target);
		return;
	}
	//船只
	if (isButton(button_ships))
	{
		m_eType = ITEM_TYPE_SHIP;
		if (m_bIsSellOrder)
		{
			ProtocolThread::GetInstance()->getItemsToSell(m_eType,0,UILoadingIndicator::create(this));
		}else
		{
			updateBuyOrderSelectedItemView();
		}
		changeOrderButtonState(target);
		return;
	}
	//船只装备
	if (isButton(button_ship_equipment))
	{
		m_eType = ITEM_TYPE_SHIP_EQUIP;
		if (m_bIsSellOrder)
		{
			ProtocolThread::GetInstance()->getItemsToSell(m_eType,0,UILoadingIndicator::create(this));
		}else
		{
			updateBuyOrderSelectedItemView();
		}
		changeOrderButtonState(target);
		return;
	}
	//人物装备
	if (isButton(button_human_equipment))
	{
		m_eType = ITEM_TYPE_ROLE_EQUIP;
		if (m_bIsSellOrder)
		{
			ProtocolThread::GetInstance()->getItemsToSell(m_eType,0,UILoadingIndicator::create(this));
		}else
		{
			updateBuyOrderSelectedItemView();
		}
		changeOrderButtonState(target);
		return;
	}
	//图纸
	if (isButton(button_drawings))
	{
		m_eType = ITEM_TYPE_DRAWING;
		if (m_bIsSellOrder)
		{
			ProtocolThread::GetInstance()->getItemsToSell(m_eType,0,UILoadingIndicator::create(this));
		}else
		{
			updateBuyOrderSelectedItemView();
		}
		changeOrderButtonState(target);
		return;
	}
	//道具
	if (isButton(button_miscs))
	{
		m_eType = ITEM_TYPE_PORP;
		if (m_bIsSellOrder)
		{
			ProtocolThread::GetInstance()->getItemsToSell(m_eType,0,UILoadingIndicator::create(this));
		}else
		{
			updateBuyOrderSelectedItemView();
		}
		changeOrderButtonState(target);
		return;
	}
	//特殊
	if (isButton(button_special))
	{
		m_eType = ITEM_TYPE_SPECIAL;
		if (m_bIsSellOrder)
		{
			ProtocolThread::GetInstance()->getItemsToSell(m_eType,0,UILoadingIndicator::create(this));
		}else
		{
			updateBuyOrderSelectedItemView();
		}
		changeOrderButtonState(target);
		return;
	}
	//点击物品创建订单
	if (isButton(panel_itemdetails))
	{
		int itemId = 0;
		m_nOrderIndex = target->getTag();
		if (m_bIsSellOrder)
		{
			itemId = m_pGetItemToSell->items[m_nOrderIndex]->itemid;
		}
		else
		{
			itemId = m_nOrderIndex;
		}
		auto i_broken = target->getChildByName<ImageView*>("image_broken");
		if (i_broken->isVisible())
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_EXCHANGE_SELL_ORDER_REMIND");
			return;
		}
		int isCanTrade = 0;//是否可以交易,0可以，1不可以
		switch (m_eType)
		{
		case ITEM_TYPE_GOOD:
		isCanTrade = 0;
		break;
		case ITEM_TYPE_SHIP:
		isCanTrade = SINGLE_SHOP->getShipData()[itemId].no_trade;
		break;
		case ITEM_TYPE_SHIP_EQUIP:
		case ITEM_TYPE_PORP:
		case ITEM_TYPE_ROLE_EQUIP:
		case ITEM_TYPE_DRAWING:
		isCanTrade = SINGLE_SHOP->getItemData()[itemId].no_trade;
		break;
		case ITEM_TYPE_SPECIAL:
		isCanTrade = 0;
		break;
		default:
		break;
		}
		if (isCanTrade ==1)
		{
			openSuccessOrFailDialog("TIP_MARKET_NO_TRADE_GOOD");
			return;
		}
		if (m_bIsSellOrder)
		{
			openSellOrder();
		}else
		{
			openBuyOrder();
		}
		return;
	}
	//创建订单 减少天数
	if (isButton(button_left))
	{
		m_nOrderDay--;
		if (m_nOrderDay == 1)
		{
			target->setBright(false);
			target->setTouchEnabled(false);
		}
		Widget *view = nullptr;
		if (m_bIsSellOrder)
		{
			view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_SELL_CSB]);
		}else
		{
			view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_BUY_CSB]);
		}
		auto t_day = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_expired_time_1"));
		t_day->setString(StringUtils::format("%d %s",m_nOrderDay,SINGLE_SHOP->getTipsInfo()["TIP_DAY"].data()));
		auto b_right = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_right"));
		b_right->setBright(true);
		b_right->setTouchEnabled(true);
		return;
	}
	//创建订单 增加天数
	if (isButton(button_right))
	{
		m_nOrderDay++;
		if (m_nOrderDay == 7)
		{
			target->setBright(false);
			target->setTouchEnabled(false);
		}
		Widget *view = nullptr;
		if (m_bIsSellOrder)
		{
			view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_SELL_CSB]);
		}else
		{
			view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_BUY_CSB]);
		}
		auto t_day = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_expired_time_1"));
		t_day->setString(StringUtils::format("%d %s",m_nOrderDay,SINGLE_SHOP->getTipsInfo()["TIP_DAY"].data()));
		auto b_left = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_left"));
		b_left->setBright(true);
		b_left->setTouchEnabled(true);
		return;
	}
	//创建订单 减少价格
	if (isButton(button_price_minus))
	{		
		Widget *view = nullptr;
		if (m_bIsSellOrder)
		{
			view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_SELL_CSB]);
		}else
		{
			view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_BUY_CSB]);
		}	

		auto t_price = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_price_num"));
		Text* text_price = dynamic_cast<Text*>(t_price->getParent()->getChildByTag(1));
		if (m_nOrderPrice <=1)
		{

			//m_nOrderPrice = 1;
			m_nOrderPrice = 0;
			text_price->setString(numSegment(StringUtils::format("%d", m_nOrderPrice)));
			goodsNumberChangeEvent(m_nOrderAmount);
			return;
		}	
		m_nOrderPrice--;
	
		text_price->setString(numSegment(StringUtils::format("%d", m_nOrderPrice)));
		goodsNumberChangeEvent(m_nOrderAmount);
		return;
	}
	//创建订单 增加价格
	if (isButton(button_price_plus))
	{
		Widget *view = nullptr;
		if (m_bIsSellOrder)
		{
			view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_SELL_CSB]);
		}
		else
		{
			view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_BUY_CSB]);
		}
		auto t_price = dynamic_cast<TextField*>(Helper::seekWidgetByName(view, "textfield_price_num"));
		Text* text_price = dynamic_cast<Text*>(t_price->getParent()->getChildByTag(1));

		if (m_nOrderPrice >= 1000000000)
		{
			m_nOrderPrice = 1000000000;
			text_price->setString(numSegment(StringUtils::format("%d", m_nOrderPrice)));
			return;
		}
		m_nOrderPrice++;
		text_price->setString(numSegment(StringUtils::format("%d", m_nOrderPrice)));
		goodsNumberChangeEvent(m_nOrderAmount);
		return;
	}

	if(isButton(button_result_yes))
	{
		closeView();
		return;
	}
	//物品详情
	/*if (isButton(button_good_bg))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (target->getTag() < 10000)
		{
			return;
		}		
		int type = target->getTag() / 10000;
		openGoodInfo(nullptr,type -1,target->getTag() - type * 10000);
		return;
	}*/

	//物品之间切换
	if (isButton(button_order_buy) || isButton(button_order_sell))
	{
		auto view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_CSB]);
		auto b_create_order = view->getChildByName<Button*>("button_create_order");
		b_create_order->setVisible(true);
		b_create_order->setTouchEnabled(true);

		changeMinorButtonState(target);
		int index = target->getTag() - ORDER_BUY_INDEX;
		if (index == 0)
		{
			m_bIsSellOrder = false;
		}else
		{
			m_bIsSellOrder = true;
		}
		ProtocolThread::GetInstance()->getOrderList(index,UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_order_marketbuy) || isButton(button_order_marketsell))
	{
		auto view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_CSB]);
		auto b_create_order = view->getChildByName<Button*>("button_create_order");
		b_create_order->setVisible(false);
		b_create_order->setTouchEnabled(false);

		changeMinorButtonState(target);
		int index = target->getTag() - RECENT_ORDER_BUY_INDEX;
		auto type = 0;
		if (index == 0)
		{
			type = 1;
			m_bIsRecentSellOrder = false;
		}
		else
		{
			m_bIsRecentSellOrder = true;
		}
		ProtocolThread::GetInstance()->getRecentOrder(type, UILoadingIndicator::create(this));
		return;
	}
}

void UIExchange::sellingSliderEvent(const int64_t nNum)
{
	m_nBuyNumber = m_pLastPriceResult->count;
	GetPersonalItemResult *result = (GetPersonalItemResult*)m_pResult;
	m_nBuyNumber = m_nBuyNumber > nNum ? nNum:m_nBuyNumber;
	//专家售卖技能(个人，船长)只对流行品有作用
	float skill_num = 0;
	if (result->itemtype==ITEM_TYPE_GOOD)
	{
		if (result->goods[m_nItemIndex]->ispopular)
		{
			skill_num = m_pLastPriceResult->captain_skill_expert_sellor * 0.005 + m_pLastPriceResult->skill_expert_sellor * 0.01;
		}
		else
		{
			//skill_num =m_pLastPriceResult->skill_expert_sellor * 0.01;
		}
	}
	//float fPrice1_orginal = (1 + skill_num)* ceil(getAveragePrice(m_pLastPriceResult->npconlyprices, m_pLastPriceResult->n_npconlyprices, m_nBuyNumber));
	int index = 0;
	for (int i = 1; i < m_pLastPriceResult->n_npconlyprices; i++)
	{
		if (m_pLastPriceResult->npconlyprices[i]->price > m_pLastPriceResult->npconlyprices[index]->price)
		{
			index = i;
		}
	}
	double fPrice1_orginal = 0;
	if (m_pLastPriceResult->n_npconlyprices > 0)
	{
		fPrice1_orginal = (1 + skill_num)* m_pLastPriceResult->npconlyprices[index]->price;
	}
	
	double fPrice1 = fPrice1_orginal * (1 - m_pLastPriceResult->tax * 1.0 / FEE);
	double tax1 = ceil(m_nBuyNumber * fPrice1_orginal * m_pLastPriceResult->tax * 1.0 / FEE);
	int64_t nCostCoin1 = floor(1.0 * fPrice1_orginal * m_nBuyNumber) - tax1;
	//float fPrice2_orginal = getAveragePrice(m_pLastPriceResult->prices,m_pLastPriceResult->n_prices,m_nBuyNumber);
	index = 0;
	for (int i = 1; i < m_pLastPriceResult->n_prices; i++)
	{
		if (m_pLastPriceResult->prices[i]->price > m_pLastPriceResult->prices[index]->price)
		{
			index = i;
		}
	}
	double fPrice2_orginal = 0;
	if (m_pLastPriceResult->n_prices > 0)
	{
		fPrice2_orginal = (1 + skill_num)* m_pLastPriceResult->prices[index]->price;
	}
	
	double fPrice2 = fPrice2_orginal * (1 - m_pLastPriceResult->tax * 1.0 / FEE);
	double tax2 = ceil(m_nBuyNumber * fPrice2_orginal * m_pLastPriceResult->tax * 1.0 / FEE);
	int64_t nCostCoin2 = floor(1.0 * fPrice2_orginal * m_nBuyNumber) - tax2;
	Node *w_goodsInfo = nullptr;
	if (result->itemtype == ITEM_TYPE_GOOD && result->n_goods >m_nItemIndex && result->goods[m_nItemIndex]->ispopular)
	{
		w_goodsInfo = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODSPOPULAR_CSB]);
		Node*	w_sellGoods = w_goodsInfo->getChildByName("panel_sell_goods");
		Button* btn_price_1 = w_sellGoods->getChildByName<Button*>("button_price_r");
		Button* btn_price_2 = w_sellGoods->getChildByName<Button*>("button_price");
		Text* t_price_1 = btn_price_1->getChildByName<Text*>("label_price_num");
		Text* t_price_2 = btn_price_2->getChildByName<Text*>("label_price_num");
		auto t_name = w_goodsInfo->getChildByName("label_name");
		auto p_up = w_goodsInfo->getChildByName("panel_up");
		std::string s_price_1 =numSegment(StringUtils::format("%.0f",fPrice1_orginal));
		std::string s_price_2 =numSegment(StringUtils::format("%.0f",fPrice2_orginal));
		t_price_1->setString(s_price_1);
		t_price_2->setString(s_price_2);
		if (fPrice1_orginal > fPrice2_orginal)
		{
			btn_price_2->setVisible(false);
			btn_price_1->setPositionY(t_name->getPositionY());
			p_up->setPositionY(t_name->getPositionY() - p_up->getContentSize().height/2);
		}
		
		auto p_price1 = w_sellGoods->getChildByName<Widget*>("panel_up");
		auto p_price2 = w_sellGoods->getChildByName<Widget*>("panel_down");
		auto i_p1 = p_price1->getChildByName<ImageView*>("image_up");
		auto t_p1 = p_price1->getChildByName<Text*>("label_down_num");
		auto t_d1 = p_price1->getChildByName<Text*>("label_draw");
		auto i_p2 = p_price2->getChildByName<ImageView*>("image_down");
		auto t_p2 = p_price2->getChildByName<Text*>("label_down_num");
		auto t_d2 = p_price2->getChildByName<Text*>("label_draw");
		if (m_pLastPriceResult->averagecost < fPrice1)
		{
			i_p1->setVisible(true);
			t_p1->setVisible(true);
			t_d1->setVisible(false);
			i_p1->loadTexture("cocosstudio/login_ui/market_720/up.png");
			t_p1->setTextColor(Color4B(53,134,0,255));
			if (m_pLastPriceResult->averagecost == 0)
			{
				t_p1->setString("100%");
			}else
			{
				t_p1->setString(StringUtils::format("%.0f%%", (fPrice1 - m_pLastPriceResult->averagecost) * 100.0 / m_pLastPriceResult->averagecost));
			}
		}else if (m_pLastPriceResult->averagecost > fPrice1)
		{
			i_p1->setVisible(true);
			t_p1->setVisible(true);
			t_d1->setVisible(false);
			i_p1->loadTexture("cocosstudio/login_ui/market_720/down.png");
			t_p1->setTextColor(Color4B(255,0,0,255));
			t_p1->setString(StringUtils::format("%.0f%%", (m_pLastPriceResult->averagecost - fPrice1) * 100.0 / m_pLastPriceResult->averagecost));
		}else
		{
			i_p1->setVisible(false);
			t_p1->setVisible(false);
			t_d1->setVisible(true);
			t_d1->setString("-- 0%");
		}

		if (m_pLastPriceResult->averagecost < fPrice2)
		{
			i_p2->setVisible(true);
			t_p2->setVisible(true);
			t_d2->setVisible(false);
			i_p2->loadTexture("cocosstudio/login_ui/market_720/up.png");
			t_p2->setTextColor(Color4B(53,134,0,255));
			if (m_pLastPriceResult->averagecost == 0)
			{
				t_p2->setString("100%");
			}else
			{
				t_p2->setString(StringUtils::format("%.0f%%", (fPrice2 - m_pLastPriceResult->averagecost) * 100.0 / m_pLastPriceResult->averagecost));
			}
			
		}else if (m_pLastPriceResult->averagecost > fPrice2)
		{
			i_p2->setVisible(true);
			t_p2->setVisible(true);
			t_d2->setVisible(false);
			i_p2->loadTexture("cocosstudio/login_ui/market_720/down.png");
			t_p2->setTextColor(Color4B(255,0,0,255));
			t_p2->setString(StringUtils::format("%.0f%%", (m_pLastPriceResult->averagecost - fPrice2) * 100.0 / m_pLastPriceResult->averagecost));
		}else
		{
			i_p2->setVisible(false);
			t_p2->setVisible(false);
			t_d2->setVisible(true);
			t_d2->setString("-- 0%");
		}

		auto    w_selected_bg = w_sellGoods->getChildByName("image_select_num_bg");
		auto	w_num_bg = w_selected_bg->getChildByName("panel_select_num");
		auto    p_cost = w_selected_bg->getChildByName<Widget*>("panel_cost_num");
		auto	t_cost_total = p_cost->getChildByName<Text*>("label_cost_num");
		auto    p_profit = w_selected_bg->getChildByName<Widget*>("panel_profit_num");
		auto	t_profit = p_profit->getChildByName<Text*>("label_price_num");
		auto    p_tax = w_selected_bg->getChildByName<Widget*>("panel_tax_num");
		auto	t_tax = p_tax->getChildByName<Text*>("label_price_num");
		
		Text	*t_count = w_num_bg->getChildByName<Text*>("label_goods_num");
		std::string s_count = StringUtils::format("x %lld",m_nBuyNumber);
		std::string s_cost_total;

		if (result->goods[m_nItemIndex]->ispopular)
		{
			s_cost_total = StringUtils::format("%lld",nCostCoin1);
			t_profit->setString(numSegment(StringUtils::format("%d",int(nCostCoin1 - m_nBuyNumber * m_pLastPriceResult->averagecost))));
			t_tax->setString(numSegment(StringUtils::format("%.0f", tax1)));
		}else
		{
			s_cost_total = StringUtils::format("%lld",nCostCoin2);
			t_profit->setString(numSegment(StringUtils::format("%d",int(nCostCoin2 - m_nBuyNumber * m_pLastPriceResult->averagecost))));
			t_tax->setString(numSegment(StringUtils::format("%.0f", tax2)));
		}
		t_cost_total->setString(numSegment(s_cost_total));
		t_count->setString(s_count);
		dynamic_cast<Layout*>(p_cost)->requestDoLayout();
		dynamic_cast<Layout*>(p_profit)->requestDoLayout();
		dynamic_cast<Layout*>(p_tax)->requestDoLayout();

		UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(100));
		ss->setCurrentNumber(m_nBuyNumber);

		if (fPrice2 == 0 || m_pLastPriceResult->averagecost == 0)
		{
			btn_price_2->setTouchEnabled(false);
			i_p2->setVisible(false);
			t_p2->setVisible(false);
			t_d2->setVisible(false);
		}else
		{
			btn_price_2->setTouchEnabled(true);
		}
//chengyuan++
		auto sellview = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODSPOPULAR_CSB]);
		auto select_num_view = sellview->getChildByName("image_select_num_bg");
		auto p_cost_num = select_num_view->getChildByName("panel_cost_num");
		auto i_silver = p_cost_num->getChildByName("image_silver");
		auto t_cost_num = p_cost_num->getChildByName<Text*>("label_cost_num");
		t_cost_num->setTextHorizontalAlignment(TextHAlignment::RIGHT);
		t_cost_num->ignoreContentAdaptWithSize(true);
		int s_num = t_cost_num->getString().size();
		i_silver->setPositionX(t_cost_num->getPositionX() - t_cost_num->getContentSize().width - 30);
//
	}else
	{
		w_goodsInfo = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODS_CSB]);
		auto	w_sellGoods = w_goodsInfo->getChildByName("panel_buy_goods");
		auto lv_price = w_sellGoods->getChildByName<ListView*>("listview_price");
		lv_price->removeAllChildrenWithCleanup(true);
		auto    i_down = dynamic_cast<Widget*>(w_sellGoods->getChildByName("panel_down"));
		auto	i_up = dynamic_cast<Widget*>(w_sellGoods->getChildByName("panel_up"));
		auto	i_draw = dynamic_cast<Widget*>(w_sellGoods->getChildByName("panel_draw"));
		auto	i_goldcoin = dynamic_cast<ImageView*>(w_sellGoods->getChildByName("image_goldcoin_1"));
		auto	t_price = dynamic_cast<Text*>(w_sellGoods->getChildByName("label_price_num"));
		auto	b_price = dynamic_cast<Button*>(w_sellGoods->getChildByName("button_price_info"));

		auto i_coin_clone = i_goldcoin->clone();
		lv_price->pushBackCustomItem(i_coin_clone);
		auto t_price_clone = t_price->clone();
		
		((Text*)t_price_clone)->setString(numSegment(StringUtils::format("%.0f",fPrice2_orginal)));
//chengyuan++

		float t_size = 16;
		auto t_num = ((Text*)t_price_clone)->getString().size();
		t_price_clone->setContentSize(Size(t_size *t_num + 12, t_price_clone->getContentSize().height));
//	
		lv_price->pushBackCustomItem(t_price_clone);

		if (result->itemtype == ITEM_TYPE_GOOD)
		{
			if (fPrice2 < result->goods[m_nItemIndex]->averagecost)
			{
				auto i_down_clone = i_down->clone();
				Text *t_text = i_down_clone->getChildByName<Text*>("label_down_num");	
				int price_per = 100;
				if (result->goods[m_nItemIndex]->averagecost > 0)
				{
					price_per = 100 * (result->goods[m_nItemIndex]->averagecost - fPrice2) / result->goods[m_nItemIndex]->averagecost;
				}
				std::string s_price = StringUtils::format("%d%%",price_per);
				t_text->setString(s_price);
				lv_price->pushBackCustomItem(i_down_clone);

			}else if(fPrice2 == result->goods[m_nItemIndex]->averagecost)
			{
				auto i_draw_clone = i_draw->clone();
				lv_price->pushBackCustomItem(i_draw_clone);
			
			}else
			{
				auto i_up_clone = i_up->clone();
				Text *t_text = i_up_clone->getChildByName<Text*>("label_down_num");

				int price_per = 100;
				if (result->goods[m_nItemIndex]->averagecost > 0)
				{
					price_per = 100 * (fPrice2 - result->goods[m_nItemIndex]->averagecost)/result->goods[m_nItemIndex]->averagecost;
				}
				std::string s_price = StringUtils::format("%d%%",price_per);
				t_text->setString(s_price);
				lv_price->pushBackCustomItem(i_up_clone);
			}
		}
		auto b_price_clone = b_price->clone();
		lv_price->pushBackCustomItem(b_price_clone);
		Text	*t_mount = dynamic_cast<Text*>(w_sellGoods->getChildByName<Widget*>("label_goods_num"));

		auto   w_selected_bg = w_sellGoods->getChildByName("image_select_num_bg");
		auto   w_num_bg = w_selected_bg->getChildByName<Widget*>("panel_select_num");
		auto   t_count = dynamic_cast<Text*>(Helper::seekWidgetByName(w_num_bg,"label_goods_num"));
		std::string s_count = StringUtils::format("x %lld",m_nBuyNumber);
		auto   p_cost = w_selected_bg->getChildByName<Widget*>("panel_cost_num");
		auto   t_cost = p_cost->getChildByName<Text*>("label_cost_num");
		std::string s_cost = StringUtils::format("%lld",nCostCoin2);

		t_cost->setString(numSegment(s_cost));
		t_count->setString(s_count);

		UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(101));
		ss->setCurrentNumber(m_nBuyNumber);

		auto t_cost_tax = w_selected_bg->getChildByName<Text*>("label_tax");
		std::string st_tax_title;
		st_tax_title += StringUtils::format("%s(%.2f%%):",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(),m_pLastPriceResult->tax * 100.0 / FEE);
		auto   p_tax = w_selected_bg->getChildByName<Widget*>("panel_tax_num");
		auto   t_tax = p_tax->getChildByName<Text*>("label_price_num");
		auto   l_profit = w_selected_bg->getChildByName<Text*>("label_profit");
		auto   p_profit = w_selected_bg->getChildByName<Widget*>("panel_profit_num");
		auto   t_profit = p_profit->getChildByName<Text*>("label_price_num");
		auto   i_div = w_selected_bg->getChildByName<ImageView*>("image_div_0");
		
		if (result->itemtype == ITEM_TYPE_GOOD)
		{
			p_tax->setVisible(true);
			t_cost_tax->setVisible(true);
			t_cost_tax->setString(st_tax_title);
			t_tax->setString(numSegment(StringUtils::format("%.0f", ceil(m_nBuyNumber * fPrice2_orginal * m_pLastPriceResult->tax * 1.0 / FEE))));
			dynamic_cast<Layout*>(p_tax)->requestDoLayout();
			l_profit->setString(StringUtils::format("%s:",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_PROFIT"].data()));
			if( (nCostCoin2 - m_nBuyNumber*m_pLastPriceResult->averagecost) > 0)
			{
				t_profit->setString(numSegment(StringUtils::format("%d", int(nCostCoin2 - m_nBuyNumber*m_pLastPriceResult->averagecost))));
			}
			else
			{
				t_profit->setString(numSegment(StringUtils::format("%d", int(nCostCoin2 - m_nBuyNumber*m_pLastPriceResult->averagecost))));
			}

			;
			i_div->setVisible(true);
		}else
		{
			p_tax->setVisible(false);
			t_cost_tax->setVisible(false);
			//显示税
			t_profit->setString(numSegment(StringUtils::format("%.0f", ceil(m_nBuyNumber * fPrice2_orginal * m_pLastPriceResult->tax * 1.0 / FEE))));
			l_profit->setString(st_tax_title);
			i_div->setVisible(false);
		}
		dynamic_cast<Layout*>(p_profit)->requestDoLayout();
//chengyuan++
		auto sellview = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_VIEWGOODS_CSB]);
		auto select_num_view = sellview->getChildByName("image_select_num_bg");
		auto p_cost_num = select_num_view->getChildByName("panel_cost_num");
		auto i_silver = p_cost_num->getChildByName("image_silver");
		auto t_cost_num = p_cost_num->getChildByName<Text*>("label_cost_num");
		t_cost_num->setTextHorizontalAlignment(TextHAlignment::RIGHT);
		t_cost_num->ignoreContentAdaptWithSize(true);
		i_silver->setPositionX(t_cost_num->getPositionX() - t_cost_num->getContentSize().width - 30);

		auto p_goodInfo = sellview->getChildByName("panel_buy_goods");
		auto l_goodPrice = p_goodInfo->getChildByName<ListView*>("listview_price");
		auto t_goodname = p_goodInfo->getChildByName("label_name");
		l_goodPrice->setPositionX(t_goodname->getPositionX());
//
	}
}

void UIExchange::createOrderSellSuccess()
{
	openView(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_SAILLING_ACCIDENT_CSB]);
	auto t_title = view->getChildByName<Text*>("label_title");
	auto p_result = view->getChildByName<Widget*>("panel_result");
	auto l_result = p_result->getChildByName<ListView*>("listview_result");
	auto p_silver = p_result->getChildByName<Widget*>("panel_silver_2");
	auto i_div = p_result->getChildByName<ImageView*>("image_div_1");

	auto &tips = SINGLE_SHOP->getTipsInfo();
	t_title->setString(tips["TIP_EXCHANGE_ORDER_TITLE"]);
	auto p_tax = p_silver->clone();
	auto t_tax = p_tax->getChildByName<Text*>("label_force_relation");
	auto t_taxNum = p_tax->getChildByName<Text*>("label_buy_num");
	auto i_silver_tax = p_tax->getChildByName<ImageView*>("image_silver");
	t_tax->setString(StringUtils::format("%s(%.2f%%):",tips["TIP_EXCHANGE_TEX"].data(),m_nOrderTax / 100.0));
	t_taxNum->setString(numSegment(StringUtils::format("%lld",int64_t(ceil(m_nOrderPrice * m_nOrderAmount *  m_nOrderTax / 10000.0)))));
	dynamic_cast<Layout*>(p_tax)->requestDoLayout();
	i_silver_tax->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_tax->setPositionX(t_taxNum->getPositionX() - t_taxNum->getBoundingBox().size.width - i_silver_tax->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_tax);
	auto i_div1 = i_div->clone();
	l_result->pushBackCustomItem(i_div1);

	auto p_fee = p_silver->clone();
	auto t_fee = p_fee->getChildByName<Text*>("label_force_relation");
	auto t_feeNum = p_fee->getChildByName<Text*>("label_buy_num");
	auto i_silver_fee = p_fee->getChildByName<ImageView*>("image_silver");
	t_fee->setString(tips["TIP_PUP_SAILOR_RECRUIT_FEE"]);
	t_feeNum->setString(StringUtils::format("%lld",m_nOrderFee));
	dynamic_cast<Layout*>(p_fee)->requestDoLayout();
	i_silver_fee->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_fee->setPositionX(t_feeNum->getPositionX() - t_feeNum->getBoundingBox().size.width - i_silver_fee->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_fee);
	auto i_div2 = i_div->clone();
	l_result->pushBackCustomItem(i_div2);

	auto p_cost = p_silver->clone();
	auto t_cost = p_cost->getChildByName<Text*>("label_force_relation");
	auto t_costNum = p_cost->getChildByName<Text*>("label_buy_num");
	auto i_silver_cost = p_cost->getChildByName<ImageView*>("image_silver");
	t_cost->setString(tips["TIP_EXCHANGE_ORDER_INCOME"]);
	t_costNum->setString(numSegment(StringUtils::format("%lld",int64_t(((m_nOrderPrice - m_nAverageCost) * m_nOrderAmount) * (1.0 - m_nOrderTax / 10000.0) - m_nOrderFee))));
	dynamic_cast<Layout*>(p_cost)->requestDoLayout();
	i_silver_cost->loadTexture("cocosstudio/login_ui/common/silver.png");
	i_silver_cost->setPositionX(t_costNum->getPositionX() - t_costNum->getBoundingBox().size.width - i_silver_cost->getBoundingBox().size.width / 2 - 5);
	l_result->pushBackCustomItem(p_cost);
	auto i_div3 = i_div->clone();
	l_result->pushBackCustomItem(i_div3);

	l_result->setPositionY(l_result->getPositionY() - p_silver->getBoundingBox().size.height/2);
}

void UIExchange::shipSliderEvent(const int nTag,const int nNum)
{	
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_VIEWGOODS_CSB]);
	auto shipList = dynamic_cast<ListView*>(view->getChildByName("listview_buy_goods"));
	
	Node	*goodsInfo = shipList->getItem(0);
	std::string s_shipName = StringUtils::format("image_shiplist_bg_%d",nTag - 1);
	Node	*item = goodsInfo->getChildByName(s_shipName);
	Node	*w_num_bg = item->getChildByName("panel_select_num");
	Text	*t_shipNum = dynamic_cast<Text*>(w_num_bg->getChildByName("label_ship_num"));
	t_shipNum->setString(String::createWithFormat("x %d",nNum)->_string);

	if (m_nUnparadox < 1)
	{
		++m_nUnparadox;
		int totalNum = 0;
		for (auto i = 0; i < m_pLastPriceResult->n_shipinfos; ++i)
		{
			UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(i+2));
			if (ss != nullptr)
			{
				totalNum += ss->getCurrentNumber();
			}
			
		}
		
		UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));

		if(ss != nullptr)
		{
			int diffNum = totalNum - ss->getMaxPercent();
			if (diffNum > 0 )
			{
				UIVoyageSlider* ss_current = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(nTag));
				ss_current->setCurrentNumber(nNum - diffNum);
				t_shipNum->setString(String::createWithFormat("x %d",nNum - diffNum)->_string);
				totalNum = ss->getMaxPercent();
			}	
			ss->setCurrentNumber(totalNum);
		}
		updateShipForNumber(totalNum);	
	}
}

TableViewCell* UIExchange::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell *cell = table->dequeueCell();
	Widget*goodsItem;
	if (!cell){
		goodsItem = m_TableViewItem->clone();
		cell = new TableViewCell();
		cell->autorelease();
		cell->addChild(goodsItem,1);
		goodsItem->setPosition(Vec2(0, 0));
	}
	else{
		goodsItem = dynamic_cast<Widget*>(cell->getChildren().at(0));
	}
	cell->setVisible(true);
	auto i_intensify = Helper::seekWidgetByName((Widget*)cell, "goods_intensify");
	if (i_intensify)
	{
		i_intensify->removeFromParentAndCleanup(true);
	}

	auto equip_broken_icon = Helper::seekWidgetByName(goodsItem, "equip_broken_icon");//装备损坏
	if (equip_broken_icon)
	{
		equip_broken_icon->removeFromParentAndCleanup(true);
	}

	auto  i_button_good = goodsItem->getChildByName<Button*>("button_good_bg");
	auto	i_img_icon = i_button_good->getChildByName<ImageView*>("image_goods");
	setBrokenEquipRed(i_img_icon, false);

	int i = idx;

	if(m_nPageIndex == PAGE_BUY_INDEX)
	{
		GetItemsBeingSoldResult *result = (GetItemsBeingSoldResult *)m_pResult;
		while (i >= result->n_items){
			i -= result->n_items;
		}
		auto    button_good = goodsItem->getChildByName<Button*>("button_good_bg");
		auto	img_icon = button_good->getChildByName<ImageView*>("image_goods");
		auto	price = goodsItem->getChildByName<Text*>("label_goods_price");
		auto	t_name = goodsItem->getChildByName<Text*>("label_goods_name");
		auto	t_amount = goodsItem->getChildByName<Text*>("label_goods_amount");
		auto	t_weight = goodsItem->getChildByName<Text*>("label_goods_weight");
		auto    i_weight = goodsItem->getChildByName<Widget*>("image_weight");
		auto    i_div = goodsItem->getChildByName<ImageView*>("image_div_2");
		auto    i_broken = goodsItem->getChildByName<ImageView*>("image_broken");
		auto image_lv_bg = goodsItem->getChildByName<ImageView*>("image_lv_bg");
		image_lv_bg->setVisible(false);
		auto t_required = image_lv_bg->getChildByName<Text*>("label_lv");
		image_lv_bg->setPositionX(680);

		std::string itemName;
		std::string itemPath;
		getItemNameAndPath(result->itemtype, result->items[i]->itemid, itemName, itemPath);
		img_icon->ignoreContentAdaptWithSize(false);
		img_icon->loadTexture(itemPath);

		goodsItem->setName(String::createWithFormat("button_item_%d", i)->getCString());
		goodsItem->setTag(i);

		setBgButtonFormIdAndType(button_good, result->items[i]->itemid, result->itemtype);
		setTextColorFormIdAndType(t_name, result->items[i]->itemid, result->itemtype);
		button_good->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
		price->setString(numSegment(StringUtils::format("%lld", result->items[i]->lastprice)));
		auto nMaxStringNum = 0;
		auto languageTypeNum = 0;
		if (isChineseCharacterIn(itemName.c_str()))
		{
			languageTypeNum = 1;
		}
		if (languageTypeNum)
		{
			nMaxStringNum = 20;
		}
		else
		{
			nMaxStringNum = 37;
		}
		t_name->setString(apostrophe(itemName, nMaxStringNum));
		goodsItem->addTouchEventListener(CC_CALLBACK_2(UIExchange::buyBtnEvent, this));

		if (result->itemtype == ITEM_TYPE_SHIP || result->itemtype == ITEM_TYPE_DRAWING || result->itemtype == ITEM_TYPE_SPECIAL)
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
		i_broken->setVisible(false);
		int required_lv = 0;

		if (result->itemtype == ITEM_TYPE_SHIP)
		{
			image_lv_bg->setVisible(true);
			required_lv = SINGLE_SHOP->getShipData()[result->items[i]->itemid].require_level;
			image_lv_bg->setPositionX(810);
		}
		else if (result->itemtype == ITEM_TYPE_SHIP_EQUIP)
		{
			image_lv_bg->setVisible(true);
			required_lv = SINGLE_SHOP->getItemData()[result->items[i]->itemid].required_lv;
			image_lv_bg->setPositionX(680);
		}
		else if (result->itemtype == ITEM_TYPE_PORP || result->itemtype == ITEM_TYPE_ROLE_EQUIP || result->itemtype == ITEM_TYPE_DRAWING)
		{
			image_lv_bg->setVisible(false);
			required_lv = SINGLE_SHOP->getItemData()[result->items[i]->itemid].required_lv;
		}
		else
		{
			image_lv_bg->setVisible(false);
			required_lv = 0;
		}
		if (required_lv > SINGLE_HERO->m_iLevel)
		{
			image_lv_bg->loadTexture("login_ui/market_720/required_lv_bg_2.png");
			t_required->setTextColor(Color4B::WHITE);
			t_required->setString(StringUtils::format("Lv. %d", required_lv));
		}
		else
		{
			image_lv_bg->loadTexture("login_ui/market_720/required_lv_bg_1.png");
			t_required->setTextColor(TEXT_GREEN);
			t_required->setString(StringUtils::format("Lv. %d", required_lv));
		}

		if (result->items[i]->optionalitemnum > 0)
		{
			addStrengtheningIcon(button_good);
		}
	}
	else if(m_nPageIndex == PAGE_SELL_INDEX)
	{
		GetPersonalItemResult *result = (GetPersonalItemResult *)m_pResult;
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
		auto i_broken = goodsItem->getChildByName<ImageView*>("image_broken");
		auto v = goodsItem->getChildByName<Widget*>("image_r");
		i_broken->setVisible(false);
		if (result->itemtype == ITEM_TYPE_GOOD)
		{
			itemid = result->goods[i]->goodsid;
			itemName = getGoodsName(itemid);
			itemPath = getGoodsIconPath(itemid,IMAGE_ICON_OUTLINE);
			price = result->goods[i]->refprice;
			amount = result->goods[i]->amount;

			if (result->goods[i]->ispopular == 0)
			{
				//price = result->goods[i]->refprice;
				v->setVisible(false);
			}else
			{
				//price = result->goods[i]->refprice*(1 + result->captain_skill_expert_sellor * 0.005 + result->skill_expert_sellor * 0.01);
				v->setVisible(true);
			}
		}else
		{
			v->setVisible(false);
			if (result->itemtype == ITEM_TYPE_DRAWING)
			{
				itemid = result->drawings[i]->iid;
				itemName = getItemName(itemid);
				itemPath = getItemIconPath(itemid,IMAGE_ICON_OUTLINE);
				price = result->drawings[i]->refprice;
				amount = result->drawings[i]->amount;
			}else if (result->itemtype == ITEM_TYPE_SHIP)
			{
				itemid = result->ships[i]->sid;
				if (result->ships[i]->user_define_name && std::strcmp(result->ships[i]->user_define_name,"") != 0)
				{
					itemName = result->ships[i]->user_define_name;
				}else
				{
					itemName = getShipName(itemid);
				}
				itemPath = getShipIconPath(itemid,IMAGE_ICON_OUTLINE);
				price = result->ships[i]->price;
				amount = 1;
				if (result->ships[i]->hp < result->ships[i]->hp_max)
				{
					i_broken->setVisible(true);
				}
				if (result->ships[i]->optionid1 > 0 || result->ships[i]->optionid2 > 0)
				{
					addStrengtheningIcon(button_good);
				}
			}else if (result->itemtype == ITEM_TYPE_SPECIAL)
			{
				itemid = result->specials[i]->itemid;
				amount = result->specials[i]->amount;
				price = result->specials[i]->price;
				itemName = getItemName(itemid);
				itemPath = getItemIconPath(itemid,IMAGE_ICON_OUTLINE);
			}else
			{
				itemid = result->equipments[i]->equipmentid;
				itemName = getItemName(itemid);
				itemPath = getItemIconPath(itemid,IMAGE_ICON_OUTLINE);
				price = result->equipments[i]->refprice;
				amount = result->equipments[i]->amount;
				if (result->itemtype == ITEM_TYPE_SHIP_EQUIP)
				{
					if (result->equipments[i]->durable < result->equipments[i]->maxdurable)
					{
						i_broken->setVisible(true);
					}
				}

				if (result->equipments[i]->optionalid > 0)
				{
					addStrengtheningIcon(button_good);
				}
				if (result->equipments[i]->durable <= SHIP_EQUIP_BROKEN || result->equipments[i]->durable <= result->equipments[i]->maxdurable*SHIP_EQUIP_BROKEN_PERCENT)
				{
					addequipBrokenIcon(button_good);
					setBrokenEquipRed(img_icon);
				}
				
			}
		}

		img_icon->ignoreContentAdaptWithSize(false);
		img_icon->loadTexture(itemPath);

		goodsItem->setName(String::createWithFormat("button_item_%d",i)->getCString());
		goodsItem->setTag(i);

		setBgButtonFormIdAndType(button_good,itemid,result->itemtype);
		setBgImageColorFormIdAndType(i_bgImage,itemid,result->itemtype);
		setTextColorFormIdAndType(t_name,itemid,result->itemtype);

		button_good->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func,this));
		t_price->setString(numSegment(String::createWithFormat("%lld",price)->getCString()));
		auto nMaxStringNum = 0;
		auto languageTypeNum = 0;
		if (isChineseCharacterIn(itemName.c_str()))
		{
			languageTypeNum = 1;
		}
		if (languageTypeNum)
		{
			nMaxStringNum = 20;
		}
		else
		{
			nMaxStringNum = 37;
		}
		t_name->setString(apostrophe(itemName, nMaxStringNum));
		t_name->setFontName("");
		t_name->setFontSize(28);
		if (amount / 100000 != 0)
		{
			t_amount->setFontSize(24);
		}
		else
		{
			t_amount->setFontSize(32);
		}

		t_amount->setString(String::createWithFormat("x %lld",amount)->_string);
	}
	//买订单
	else if (m_nPageIndex == PAGE_ORDER_INDEX)
	{	
		if ((m_pMinorButton->getTag() - ORDER_BUY_INDEX) == 0)
		{
			updateBuyOrderSelectedItemView(goodsItem, m_vItemId[i]);
		}
		else if ((m_pMinorButton->getTag() - RECENT_ORDER_BUY_INDEX) == 0 || m_bIsRecentSellOrder)
		{
			while (i >= m_pRecentOrderResult->n_orders){
				i -= m_pRecentOrderResult->n_orders;
			}
			RecentOrderDefine* ord = m_pRecentOrderResult->orders[i];
			goodsItem->setName(String::createWithFormat("button_item_%d", i)->getCString());
			goodsItem->setTag(i);
			goodsItem->addTouchEventListener(CC_CALLBACK_2(UIExchange::orderBtnEvent, this));

			auto button_good = goodsItem->getChildByName<Button*>("button_good_bg");
			button_good->addTouchEventListener(CC_CALLBACK_2(UIExchange::itemInfoBtnEvent, this));
			button_good->setTag(i);

			auto price = dynamic_cast<Text*>(goodsItem->getChildByName<Widget*>("label_price_num"));
			price->setString(numSegment(StringUtils::format("%lld", m_pRecentOrderResult->orders[i]->price)));
			auto t_city = dynamic_cast<Text*>(goodsItem->getChildByName<Widget*>("label_city"));
			t_city->setString(SINGLE_SHOP->getCitiesInfo()[m_pRecentOrderResult->orders[i]->cityid].name);
			time_t t_sendTime = m_pRecentOrderResult->orders[i]->exptime;
			time_t now_time = time(NULL);
			now_time = t_sendTime - now_time - 3;

			auto t_day = dynamic_cast<Text*>(goodsItem->getChildByName<Widget*>("label_days_num"));
			std::string itemName;
			std::string itemPath;
			getItemNameAndPath(m_pRecentOrderResult->orders[i]->itemtype, ord->itemid, itemName, itemPath);

			if (now_time > 0)
			{
				tm *tm_days = gmtime(&now_time);
				if (now_time >= 86400)
				{
					std::string days = SINGLE_SHOP->getTipsInfo()["TIP_DAY"];
					std::string dataS = StringUtils::format("> %d %s", tm_days->tm_mday - 1, days.c_str());
					t_day->setString(dataS);
				}
				else if (now_time >= 3600)
				{
					std::string hours = SINGLE_SHOP->getTipsInfo()["TIP_HOUR"];
					std::string dataS = StringUtils::format("< %d %s", tm_days->tm_hour + 1, hours.c_str());
					t_day->setString(dataS);
				}
				else{
					std::string mins = SINGLE_SHOP->getTipsInfo()["TIP_MINUTE"];
					std::string dataS = StringUtils::format("< %d %s", tm_days->tm_min + 1, mins.c_str());
					t_day->setString(dataS);
				}
			}
			else
			{
				t_day->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_ORDER_OUT_TIME"]);
			}
			auto i_icon = dynamic_cast<ImageView*>(button_good->getChildByName<Widget*>("image_goods"));
			auto t_name = dynamic_cast<Text*>(goodsItem->getChildByName<Widget*>("label_goods_name"));
			auto t_number = dynamic_cast<Text*>(goodsItem->getChildByName<Widget*>("label_goods_num"));
			i_icon->ignoreContentAdaptWithSize(false);
			i_icon->loadTexture(itemPath);

			auto i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
			setBgButtonFormIdAndType(button_good, ord->itemid, m_pRecentOrderResult->orders[i]->itemtype);
			setBgImageColorFormIdAndType(i_bgImage, ord->itemid, m_pRecentOrderResult->orders[i]->itemtype);
			setTextColorFormIdAndType(t_name, ord->itemid, m_pRecentOrderResult->orders[i]->itemtype);
			
			if (ord->number / 100000 > 0)
			{
				t_number->setFontSize(24);
			}
			else
			{
				t_number->setFontSize(32);
			}

			t_number->setString("x " + numSegment(StringUtils::format("%lld", ord->number)));
			auto nMaxStringNum = 0;
			auto languageTypeNum = 0;
			if (isChineseCharacterIn(itemName.c_str()))
			{
				languageTypeNum = 1;
			}
			if (languageTypeNum)
			{
				nMaxStringNum = 16;
			}
			else
			{
				nMaxStringNum = 30;
			}
			t_name->setString(apostrophe(itemName, nMaxStringNum));
		}
	}
	return cell;
}
/**
* Returns number of cells in a given table view.
*
* @return number of cells
*/
ssize_t UIExchange::numberOfCellsInTableView(TableView *table)
{
	if(m_nPageIndex == PAGE_BUY_INDEX){
		GetItemsBeingSoldResult *result = (GetItemsBeingSoldResult *)m_pResult;
		return result->n_items;
	}else if(m_nPageIndex == PAGE_SELL_INDEX){
		GetPersonalItemResult *result = (GetPersonalItemResult *)m_pResult;
		int n_items = 0;
		switch (result->itemtype)
		{
		case ITEM_TYPE_GOOD:
			{
				n_items = result->n_goods;
				break;
			}
		case ITEM_TYPE_SHIP:
			{
				n_items = result->n_ships;
				break;
			}
		case ITEM_TYPE_SHIP_EQUIP:
		case ITEM_TYPE_PORP:
		case ITEM_TYPE_ROLE_EQUIP:
			{
				n_items = result->n_equipments;
				break;
			}
		case ITEM_TYPE_DRAWING:
			{
				n_items = result->n_drawings;
			}
			break;
		case ITEM_TYPE_SPECIAL:
			{
				n_items = result->n_specials;
				if (result->specials[0]->amount < 1)
				{
					n_items = 0;
				}
			}
			break;
		default:
			break;
		}
		return n_items;
	}
	//创建买单
	else if (m_nPageIndex == PAGE_ORDER_INDEX)
	{
		if ((m_pMinorButton->getTag() - ORDER_BUY_INDEX) == 0)
		{
			return m_vItemId.size();
		}
		else if ((m_pMinorButton->getTag() - RECENT_ORDER_BUY_INDEX) == 0 || m_bIsRecentSellOrder)
		{
			return m_pRecentOrderResult->n_orders;
		}
	}
	return 0;
}

void UIExchange::tableCellTouched(TableView* table, TableViewCell* cell)
{
	if(m_nPageIndex == PAGE_BUY_INDEX){
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (SINGLE_HERO->m_iHaslisence)
		{
			if (SINGLE_HERO->m_iEffectivelisence == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PALACE_LICENSE_DO");
			}
			else{
				m_nItemIndex = cell->getIdx();
				GetItemsBeingSoldResult *result = (GetItemsBeingSoldResult*)m_pResult;
				ProtocolThread::GetInstance()->getPriceData(result->itemtype, result->items[m_nItemIndex]->itemid, result->items[m_nItemIndex]->charcteritemid, 50, 0, UILoadingIndicator::create(this));

			}
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_EXCHANGE_NOT_LICENSE");
		}
	}else if(m_nPageIndex == PAGE_SELL_INDEX){
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nItemIndex = cell->getIdx();
		GetPersonalItemResult *result = (GetPersonalItemResult*)m_pResult;
		int itemid = 0;
		int amount = 0;
		int characterItemId = 0;
		if (result->itemtype == ITEM_TYPE_GOOD)
		{
			itemid = result->goods[m_nItemIndex]->goodsid;
			amount = result->goods[m_nItemIndex]->amount;
		}else if (ITEM_TYPE_SHIP == result->itemtype)
		{
			itemid = result->ships[m_nItemIndex]->sid;
			characterItemId = result->ships[m_nItemIndex]->id;
			amount = 1;
		}else if (ITEM_TYPE_DRAWING == result->itemtype)
		{
			itemid = result->drawings[m_nItemIndex]->iid;
			amount = result->drawings[m_nItemIndex]->amount;
		}else if (ITEM_TYPE_SPECIAL == result->itemtype )
		{
			itemid = result->specials[m_nItemIndex]->itemid;
			amount = result->specials[m_nItemIndex]->amount;
		}else
		{
			itemid = result->equipments[m_nItemIndex]->equipmentid;
			amount = result->equipments[m_nItemIndex]->amount;
			characterItemId = result->equipments[m_nItemIndex]->uniqueid;
		}
		if (SINGLE_HERO->m_iHaslisence)
		{
			if (SINGLE_HERO->m_iEffectivelisence == 0)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_PALACE_LICENSE_DO");
			}
			else{
				ProtocolThread::GetInstance()->getPriceData(result->itemtype, itemid, 0, amount, 1, UILoadingIndicator::create(this));
			}
		}else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_EXCHANGE_NOT_LICENSE");
		}
	}
	else if (m_nPageIndex == PAGE_ORDER_INDEX)
	{
		if ((m_pMinorButton->getTag() - ORDER_BUY_INDEX) == 0)
		{
			m_nOrderIndex = m_vItemId[cell->getIdx()];
			openBuyOrder();
		}
		else if ((m_pMinorButton->getTag() - RECENT_ORDER_BUY_INDEX) == 0 || m_bIsRecentSellOrder)
		{
			//m_nItemIndex = cell->getIdx();
			//openRecentOrderOperate();
		}
	}
}

void UIExchange::tableCellWillRecycle(TableView* table, TableViewCell* cell)
{

}

Size UIExchange::cellSizeForTable(TableView *table) {
	/*if (m_nPageIndex == PAGE_ORDER_INDEX)
	{
		if ((m_pMinorButton->getTag() - ORDER_BUY_INDEX) == 0)
		{
		}
		else if ((m_pMinorButton->getTag() - RECENT_ORDER_BUY_INDEX) == 0 || m_bIsRecentSellOrder)
		{
			return m_TableViewItem->getContentSize();
		}
	}*/
	auto size = m_TableViewItem->getContentSize();
	log("width = %f height = %f", size.width, size.height);
	return m_TableViewItem->getContentSize();
};

void UIExchange::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
{
	auto pos = view->getContentOffset();
	auto viewSize = view->getViewSize();
	auto contentSize = view->getContainer()->getContentSize();

	float bottom = viewSize.height-contentSize.height;
	float top = 0;
	float totalLen = top - bottom;
	float currentLen = top - pos.y;

	Button*button_pulldown = dynamic_cast<Button*>(view->getUserObject());
	if (!button_pulldown)
		return;
	float allowHeight = 16;
	float len = dynamic_cast<Widget*>(button_pulldown->getParent())->getSize().height - button_pulldown->getSize().height - allowHeight * 2;
	if (contentSize.height <= viewSize.height){
		button_pulldown->setVisible(false);
		button_pulldown->getParent()->setVisible(false);
	}else{
		button_pulldown->setVisible(true);
		button_pulldown->getParent()->setVisible(true);
		auto factor = (currentLen / totalLen);
		if (factor > 1.0) factor = 1.0;
		if (factor < 0) factor = 0;
		//log("ExchangeLayer::scrollViewDidScroll,(%f)", currentLen / totalLen);
		button_pulldown->setPositionY(len * factor + allowHeight*2 + allowHeight);
	}	
}

template<class T>
void UIExchange::updateView(const T *result)
{
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
	auto goods_list = view->getChildByName<ListView*>("listview_goods");
	auto size = goods_list->getContentSize();
	m_TableViewItem = goods_list->getItem(0);
	goods_list->setVisible(false);
	m_TableViewItem->setTouchEnabled(false);
	m_TableViewItem->setVisible(true);
	auto list_parent = goods_list->getParent();
	auto children = list_parent->getChildren();

	updateCoinNum(SINGLE_HERO->m_iGold, SINGLE_HERO->m_iCoin);

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setVisible(false);
	button_pulldown->getParent()->setVisible(false);

	auto p_no = view->getChildByName<ListView*>("panel_no");
	p_no->setVisible(false);
	if (result->n_items == 0)
	{
		auto scrollView = Helper::seekWidgetByName(view, "button_pulldown");
		scrollView->setVisible(false);
		p_no->setVisible(true);
		image_pulldown->setVisible(false);

		TableView *tableView = nullptr;
		for (auto& child : children){
			tableView = dynamic_cast<TableView*>(child);
			if (tableView){
				break;
			}
		}
		if (tableView)
		{
			tableView->removeFromParentAndCleanup(true);
		}
		return;
	}

	TableView *tableView = nullptr;
	for (auto& child : children){
		tableView = dynamic_cast<TableView*>(child);
		if (tableView){
			break;
		}
	}
	if (!tableView){
		tableView = TableView::create(this, size);
		tableView->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
		// tableView->setAnchorPoint(ccp(0, 0));  
		tableView->setDelegate(this);
		tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
		//tableView->initWithViewSize(size);
		
		list_parent->addChild(tableView, 9999);
		auto pos = goods_list->getPosition();
		tableView->setPosition(pos);
		tableView->setAnchorPoint(goods_list->getAnchorPoint());
		tableView->setTouchEnabled(true);
	}	
	tableView->setUserObject(button_pulldown);
	tableView->reloadData();
}

void UIExchange::updateCoinNum(const int64_t gold,const int64_t silver)
{
	SINGLE_HERO->m_iCoin = silver;
	SINGLE_HERO->m_iGold = gold;

	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_CSB]);
	auto i_bg = view->getChildByName("panel_currency");
	auto btn_coin = i_bg->getChildByName("button_silver");
	auto btn_gold = i_bg->getChildByName("button_gold");
	auto t_coin = dynamic_cast<Text*>(btn_gold->getChildByName<Widget*>("label_gold_num"));
	auto t_silver = dynamic_cast<Text*>(btn_coin->getChildByName<Widget*>("label_silver_num"));
    t_coin->setString(numSegment(StringUtils::format("%lld", gold)));
	t_silver->setString(numSegment(StringUtils::format("%lld",silver)));
	setTextFontSize(t_coin);
	setTextFontSize(t_silver);
	auto viewPad = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	if (viewPad)
	{
		auto panel_silver = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewPad, "panel_silver"));
		auto label_silver_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_silver, "label_silver_num"));
		auto label_gold_num = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_silver, "label_gold_num"));
		label_silver_num->setString(numSegment(StringUtils::format("%lld", silver)));
		label_gold_num->setString(numSegment(StringUtils::format("%lld", gold)));
		setTextFontSize(label_silver_num);
		setTextFontSize(label_gold_num);
	}
}

void UIExchange::delayUpdateBuyView(const float fTime)
{
	updateBuyView((GetItemsBeingSoldResult*)m_pResult);
}

void UIExchange::updateBuyView(const GetItemsBeingSoldResult* result)
{
	if (result)
	{
		updateView<GetItemsBeingSoldResult>(result);
	}
}

void UIExchange::updateOrderView(const GetOrderListResult* result)
{
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_CSB]);
	auto p_orderdetails = view->getChildByName<Widget*>("panel_orderdetails");
	auto goods_list = view->getChildByName<ListView*>("listview_goods");
	auto t_buy = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_buy"));
	auto t_sell = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_ships"));
	auto b_order_market_buy = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_order_marketbuy"));
	auto t_market_buy = dynamic_cast<Text*>(Helper::seekWidgetByName(b_order_market_buy, "label_ships"));
	auto b_order_market_sell = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_order_marketsell"));
	auto t_market_sell = dynamic_cast<Text*>(Helper::seekWidgetByName(b_order_market_sell, "label_ships"));
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	image_pulldown->setVisible(true);
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2 + 3);
	
	t_market_buy->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_BUY_ORDER_MARKET"]);
	t_market_sell->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_SELL_ORDER_MARKET"]);
	std::string buy = SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_BUY_ORDER"];
	std::string sell = SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_SELL_ORDER"];
	
	int index = m_pMinorButton->getTag() - ORDER_BUY_INDEX;
	if (index == 0)
	{
		buy += StringUtils::format("(%d",result->n_orders);
		sell += StringUtils::format("(%d",result->currentordernum2);
	}
	else
	{
		buy += StringUtils::format("(%d",result->currentordernum2);
		sell += StringUtils::format("(%d",result->n_orders);
	}

	buy += StringUtils::format("/%d)",result->maxordernum);
	sell += StringUtils::format("/%d)",result->maxordernum);
	t_buy->setString(buy);
	t_sell->setString(sell);
	m_nOrderTax = result->tax;
	m_nOrderFee = result->fee;
	m_nOrderSkillLv = 0;
	m_vSkillDefine.clear();
	for (int i = 0; i < result->n_skills; i++)
	{
		if (result->skills[i]->id == SKILL_TAX_PROTOCOL)
		{
			m_nOrderSkillLv = result->skills[i]->level;
			SKILL_DEFINE skillDefine;
			skillDefine.id = SKILL_TAX_PROTOCOL;
			skillDefine.lv = m_nOrderSkillLv;
			skillDefine.skill_type = SKILL_TYPE_PLAYER;
			skillDefine.icon_id = SINGLE_HERO->m_iIconidx;
			m_vSkillDefine.push_back(skillDefine);
			break;
		}
	}
	if (result->captain_special_skill_about_decrease_tax_level > 0)
	{
		SKILL_DEFINE skillDefine;
		skillDefine.id = result->captain_special_skill_about_decrease_tax_id;
		skillDefine.lv = result->captain_special_skill_about_decrease_tax_level;
		skillDefine.skill_type = SKILL_TYPE_PARTNER_SPECIAL;
		skillDefine.icon_id = result->captain_special_skill_about_decrease_tax_captain_id;
		m_vSkillDefine.push_back(skillDefine);
	}
	auto list_parent = goods_list->getParent();
	auto children = list_parent->getChildren();
	TableView *tableView = nullptr;
	for (auto& child : children){
		tableView = dynamic_cast<TableView*>(child);
		if (tableView){
			break;
		}
	}
	if (tableView)
	{
		tableView->removeFromParentAndCleanup(true);
	}
	
	goods_list->removeAllItems();

	auto p_no = view->getChildByName<ListView*>("panel_no");
	p_no->setVisible(false);
	if(result->n_orders == 0)
	{
		auto scrollView = Helper::seekWidgetByName(view, "button_pulldown");
		scrollView->setVisible(false);
		p_no->setVisible(true);
		image_pulldown->setVisible(false);
		return;
	}
	auto item = p_orderdetails->clone();
	goods_list->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
	auto i_intensify = Helper::seekWidgetByName(item, "goods_intensify");
	if (i_intensify)
	{
		i_intensify->removeFromParentAndCleanup(true);
	}
	for(int i = 0; i < result->n_orders; ++i)
	{
		OrderDefine* ord = result->orders[i];
		auto goodsItem = item->clone();
		goodsItem->setName(String::createWithFormat("button_item_%d",i)->getCString());
		goodsItem->setTag(i);
		goodsItem->addTouchEventListener(CC_CALLBACK_2(UIExchange::orderBtnEvent, this));
		goodsItem->setTouchEnabled(true);
		auto button_good = goodsItem->getChildByName<Button*>("button_good_bg");
		button_good->addTouchEventListener(CC_CALLBACK_2(UIExchange::itemInfoBtnEvent, this));
		button_good->setTag(i);

		auto price = dynamic_cast<Text*>(goodsItem->getChildByName<Widget*>("label_price_num"));
		price->setString(numSegment(StringUtils::format("%lld", result->orders[i]->price)));
		auto t_city = dynamic_cast<Text*>(goodsItem->getChildByName<Widget*>("label_city"));
		t_city->setString(SINGLE_SHOP->getCitiesInfo()[result->orders[i]->cityid].name);
		time_t t_sendTime = result->orders[i]->exptime;
		time_t now_time = time(NULL);
		now_time = t_sendTime - now_time-3;
		
		auto t_day = dynamic_cast<Text*>(goodsItem->getChildByName<Widget*>("label_days_num"));
		std::string itemName;
		std::string itemPath;
		getItemNameAndPath(result->orders[i]->itemtype,ord->itemid,itemName,itemPath);
	
		if(now_time > 0)
		{
			tm *tm_days = gmtime(&now_time);
			if(now_time >= 86400)
			{
			std::string days = SINGLE_SHOP->getTipsInfo()["TIP_DAY"];
			std::string dataS = StringUtils::format("> %d %s",tm_days->tm_mday-1,days.c_str());
			t_day->setString(dataS);
			}
			else if(now_time >= 3600)
			{
				std::string hours = SINGLE_SHOP->getTipsInfo()["TIP_HOUR"];
				std::string dataS = StringUtils::format("< %d %s",tm_days->tm_hour+1,hours.c_str());
				t_day->setString(dataS);
			}else{
				std::string mins = SINGLE_SHOP->getTipsInfo()["TIP_MINUTE"];
				std::string dataS = StringUtils::format("< %d %s",tm_days->tm_min+1,mins.c_str());
				t_day->setString(dataS);
			}
		}else
		{
			t_day->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_ORDER_OUT_TIME"]);
		}
		auto i_icon = dynamic_cast<ImageView*>(button_good->getChildByName<Widget*>("image_goods"));
		auto t_name = dynamic_cast<Text*>(goodsItem->getChildByName<Widget*>("label_goods_name"));
		auto t_number = dynamic_cast<Text*>(goodsItem->getChildByName<Widget*>("label_goods_num"));
		i_icon->ignoreContentAdaptWithSize(false);
		i_icon->loadTexture(itemPath);
		
		auto i_bgImage = button_good->getChildByName<ImageView*>("image_item_bg_lv");
		setBgButtonFormIdAndType(button_good,ord->itemid,result->orders[i]->itemtype);
		setBgImageColorFormIdAndType(i_bgImage,ord->itemid,result->orders[i]->itemtype);
		setTextColorFormIdAndType(t_name,ord->itemid,result->orders[i]->itemtype);
		if (result->orders[i]->optionalitemnum > 0)
		{
			addStrengtheningIcon(button_good);
		}
		if (ord->number / 100000 > 0)
		{
			t_number->setFontSize(24);
		}
		else
		{
			t_number->setFontSize(32);
		}

		t_number->setString("x " + numSegment(StringUtils::format("%lld", ord->number)));
		if (ord->item_name && std::strcmp(ord->item_name, "") != 0)
		{
			itemName = ord->item_name;
		}
		auto nMaxStringNum = 0;
		auto languageTypeNum = 0;
		if (isChineseCharacterIn(itemName.c_str()))
		{
			languageTypeNum = 1;
		}
		if (languageTypeNum)
		{
			nMaxStringNum = 16;
		}
		else
		{
			nMaxStringNum = 30;
		}
		t_name->setString(apostrophe(itemName, nMaxStringNum));
		t_name->setFontName("");
		t_name->setFontSize(28);
		goods_list->pushBackCustomItem(goodsItem);
	}
	addListViewBar(goods_list, image_pulldown);
}
void UIExchange::updateRecentOrderView(const GetRecentOrderResult *result)
{
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_CSB]);
	auto p_orderdetails = view->getChildByName<Widget*>("panel_orderdetails");
	auto goods_list = view->getChildByName<ListView*>("listview_goods");
	auto size = Size(goods_list->getContentSize().width,417.0);
	auto t_text = view->getChildByName<Text*>("text_title");
	auto i_div_1 = view->getChildByName<ImageView*>("image_div_1");
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	image_pulldown->setVisible(true);
	if (m_bIsRecentSellOrder)
	{
		t_text->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_SELL_ORDER_MARKET_TITLE"]);
	}
	else
	{
		t_text->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_BUY_ORDER_MARKET_TITLE"]);
	}
	goods_list->removeAllItems();
	auto p_no = view->getChildByName<ListView*>("panel_no");
	p_no->setVisible(false);
	auto t_title = t_text->clone();
	auto i_div = i_div_1->clone();
	if (result->n_orders == 0)
	{
		goods_list->insertCustomItem(t_title, 0);
		goods_list->insertCustomItem(i_div, 1);
		auto scrollView = Helper::seekWidgetByName(view, "button_pulldown");
		scrollView->setVisible(false);
		p_no->setVisible(true);
		image_pulldown->setVisible(false);

		auto list_parent = goods_list->getParent();
		auto children = list_parent->getChildren();
		TableView *tableView = nullptr;
		for (auto& child : children){
			tableView = dynamic_cast<TableView*>(child);
			if (tableView){
				break;
			}
		}
		if (tableView)
		{
			tableView->removeFromParentAndCleanup(true);
		}
		return;
	}
	goods_list->insertCustomItem(t_title, 0);
	goods_list->insertCustomItem(i_div, 1);

	m_TableViewItem = p_orderdetails;
	m_TableViewItem->setTouchEnabled(false);
	m_TableViewItem->setVisible(true);
	auto list_parent = goods_list->getParent();
	auto children = list_parent->getChildren();
	TableView *tableView = nullptr;
	for (auto& child : children){
		tableView = dynamic_cast<TableView*>(child);
		if (tableView){
			break;
		}
	}
	if (!tableView){
		tableView = TableView::create(this, size);
		tableView->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
		tableView->setDelegate(this);
		tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);

		list_parent->addChild(tableView, 9999);
		auto pos = goods_list->getPosition();
		tableView->setPosition(pos);
		tableView->setAnchorPoint(goods_list->getAnchorPoint());
		tableView->setTouchEnabled(true);
	}
	
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 + 3);
	button_pulldown->setAnchorPoint(Point(0.5,0.5));
	tableView->setUserObject(button_pulldown);
	button_pulldown->setVisible(false);
	button_pulldown->getParent()->setVisible(false);
	tableView->reloadData();
	updateCoinNum(SINGLE_HERO->m_iGold, SINGLE_HERO->m_iCoin);
}

void UIExchange::updateSellView(const GetPersonalItemResult* result)
{
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]);
	auto goods_list = view->getChildByName<ListView*>("listview_goods");
	auto size = goods_list->getContentSize();
	m_TableViewItem = goods_list->getItem(0);

	updateCoinNum(SINGLE_HERO->m_iGold,SINGLE_HERO->m_iCoin);


	goods_list->setVisible(false);
	m_TableViewItem->setTouchEnabled(false);
	m_TableViewItem->setVisible(true);

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2);
	button_pulldown->setVisible(false);
	button_pulldown->getParent()->setVisible(false);
	auto p_twoButter = Helper::seekWidgetByName(view, "panel_buy_two_butter");
	auto i_pulldown = Helper::seekWidgetByName(p_twoButter, "image_pulldown");
	i_pulldown->setVisible(false);

	auto list_parent = goods_list->getParent();
	auto children = list_parent->getChildren();

	int n_items = 0;
	switch (result->itemtype)
	{
	case ITEM_TYPE_GOOD:
	{
						   n_items = result->n_goods;
						   break;
	}
	case ITEM_TYPE_SHIP:
	{
						   n_items = result->n_ships;
						   break;
	}
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_PORP:
	case ITEM_TYPE_ROLE_EQUIP:
	{
								 n_items = result->n_equipments;
								 break;
	}
	case ITEM_TYPE_DRAWING:
	{
							  n_items = result->n_drawings;
	}
		break;
	case ITEM_TYPE_SPECIAL:
	{
							  n_items = result->n_specials;
							  if (result->specials[0]->amount < 1)
							  {
								  n_items = 0;
							  }
	}
		break;
	default:
		break;
	}
	auto p_no = view->getChildByName<ListView*>("panel_no");
	p_no->setVisible(false);
	if (n_items == 0)
	{
		auto scrollView = Helper::seekWidgetByName(view, "button_pulldown");
		scrollView->setVisible(false);
		p_no->setVisible(true);
		image_pulldown->setVisible(false);

		TableView *tableView = nullptr;
		for (auto& child : children){
			tableView = dynamic_cast<TableView*>(child);
			if (tableView){
				break;
			}
		}
		if (tableView)
		{
			tableView->removeFromParentAndCleanup(true);
		}
		return;
	}

	
	TableView *tableView = nullptr;
	for (auto& child : children){
		tableView = dynamic_cast<TableView*>(child);
		if (tableView){
			break;
		}
	}

	if (!tableView){
		tableView = TableView::create(this, size);
		tableView->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
		// tableView->setAnchorPoint(ccp(0, 0));
		tableView->setDelegate(this);
		tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
		//tableView->initWithViewSize(size);

		list_parent->addChild(tableView, 9999);
		auto pos = goods_list->getPosition();
		tableView->setPosition(pos);
		tableView->setAnchorPoint(goods_list->getAnchorPoint());
		tableView->setTouchEnabled(true);
	}
	tableView->setUserObject(button_pulldown);
	tableView->reloadData();
}

void UIExchange::updateShipForNumber(int64_t nNumber)
{
	++m_nUnparadox;
	m_nBuyNumber = nNumber;

	auto panel_buy = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_VIEWGOODS_CSB]);
	auto shipList = dynamic_cast<ListView*>(panel_buy->getChildByName<Widget*>("listview_buy_goods"));
	auto w_item = shipList->getItem(0);
	auto button_ok = dynamic_cast<Button*>(panel_buy->getChildByName("button_ok"));

	if (m_nBuyNumber < 1)
	{
		button_ok->setTouchEnabled(false);
		button_ok->setBright(false);
	}else
	{
		button_ok->setTouchEnabled(true);
		button_ok->setBright(true);
	}

	auto    w_selected_bg = w_item->getChildByName<Widget*>("image_select_num_bg");
	auto    p_cost = dynamic_cast<Layout*>(Helper::seekWidgetByName(w_selected_bg,"panel_cost_num"));
	auto	t_totalPrice = dynamic_cast<Text*>(Helper::seekWidgetByName(w_selected_bg,"label_cost_num"));
	auto	t_tax = dynamic_cast<Text*>(Helper::seekWidgetByName(w_selected_bg,"label_price_num"));
	auto	w_selected_num_bg = w_selected_bg->getChildByName("panel_select_num");
	auto	t_number = dynamic_cast<Text*>(w_selected_num_bg->getChildByName("label_goods_num"));
	auto    lv_price = dynamic_cast<ListView*>(w_item->getChildByName("listView_price"));
	auto	t_price = dynamic_cast<Text*>(lv_price->getChildByName("label_price_num"));
	
	auto	w_weight_bg = dynamic_cast<Widget*>(Helper::seekWidgetByName(w_item,"panel_ship_weight"));
	auto	w_shipcapacity_bg = w_weight_bg->getChildByName("panel_shipcapacity_status_num");
	auto	t_capacity_num_1 = w_shipcapacity_bg->getChildByName<Text*>("label_shipcapacity_status_num_1");
	auto	t_capacity_num_3 = w_shipcapacity_bg->getChildByName<Text*>("label_shipcapacity_status_num_3");
	auto	w_shipCap_bg = w_weight_bg->getChildByName("image_progressbar_ship_weight");
	auto	ship_state_bar = dynamic_cast<LoadingBar*>(w_shipCap_bg->getChildByName("progressbar_weight"));
	auto	ship_state_bar_add = dynamic_cast<LoadingBar*>(w_shipCap_bg->getChildByName("progressbar_weight_add"));
	auto	ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(1));
	if (ss != nullptr)
	{
		int total_goods_cap = 0;
		int had_goods_cap = 0;
		for (int i = 0; i < m_pLastPriceResult->n_shipinfos; i++){
			total_goods_cap += m_pLastPriceResult->shipinfos[i]->maxsize;
			had_goods_cap += m_pLastPriceResult->shipinfos[i]->usedsize;
		}
		t_capacity_num_1->setString(StringUtils::format("%0.2f", (had_goods_cap + m_pLastPriceResult->weight*m_nBuyNumber)/100.0));
		t_capacity_num_3->setString(StringUtils::format("/%0.2f", total_goods_cap/100.0));
		dynamic_cast<Layout*>(w_shipcapacity_bg)->setLayoutType(Layout::Type::HORIZONTAL);
		dynamic_cast<Layout*>(w_shipcapacity_bg)->requestDoLayout();
	}
	
	ship_state_bar_add->setPercent(100 *(m_fTotalCap - m_fTotalGoods)/m_fTotalCap);
	ship_state_bar_add->setPercent(100 *(m_fTotalCap - m_fTotalGoods + nNumber)/m_fTotalCap);
	float i_price = getAveragePrice(m_pLastPriceResult->prices,m_pLastPriceResult->n_prices,nNumber);
	int64_t costCoin = ceil(1.0*i_price * nNumber * (1.0 + m_pLastPriceResult->tax*1.0/FEE));
	int64_t costCoin1 = i_price * nNumber;
	int64_t costTax = ceil(1.0*i_price * nNumber * m_pLastPriceResult->tax*1.0/FEE);

	t_price->setString(numSegment(String::createWithFormat("%.0f",i_price)->_string));
	t_totalPrice->setString(numSegment(String::createWithFormat("%lld",costCoin)->_string));
	t_number->setString(String::createWithFormat("x %lld",nNumber)->_string);
	t_tax->setString(numSegment(StringUtils::format("%lld",costTax)));
	auto priceAboveNum = i_price - getGoodsAveragePrice(m_pLastPriceResult->itemid);
	//log("%f %d", priceAboveNum,getGoodsAveragePrice(m_pLastPriceResult->itemid));
	if (priceAboveNum > 0 && getGoodsAveragePrice(m_pLastPriceResult->itemid) > 0) {
		float priceAboveRate = priceAboveNum
				/ getGoodsAveragePrice(m_pLastPriceResult->itemid);
		if (priceAboveRate >= 0.01) {
			auto	price_warning = dynamic_cast<Text*>(lv_price->getChildByName("price_warning"));
			if(!price_warning){
				price_warning = Text::create();
				price_warning->setName("price_warning");
				price_warning->setFontSize(24);
				price_warning->setTextColor(Color4B(183, 28, 28, 255));
				price_warning->setTextVerticalAlignment(TextVAlignment::BOTTOM);
				lv_price->insertCustomItem(price_warning, 2);
			}
			std::string warningText = SINGLE_SHOP->getTipsInfo()["TIP_MARKET_BUY_GOODS_PRICE_WARNING"].c_str();
			std::string old_value = "[ratenum]";
			std::string new_value = StringUtils::format("%0.0f%%", ceil(priceAboveRate * 100));
			repalce_all_ditinct(warningText, old_value, new_value);
			price_warning->setText(warningText);
		}
	}
	else
	{
		auto	price_warning = dynamic_cast<Text*>(lv_price->getChildByName("price_warning"));
		if (price_warning)
		{
			price_warning->removeFromParentAndCleanup(true);
		}
	}
	lv_price->setContentSize(Size(850,60));
	lv_price->requestRefreshView();
	if (m_nUnparadox >= 2)  // two event yet!
	{
		m_nUnparadox = 0;
		return;
	}
	//auto push goods
	for (auto i = 0; i < m_pLastPriceResult->n_shipinfos; ++i)
	{
		UIVoyageSlider* ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(i+2));
		if (nNumber > ss->getMaxPercent())
		{
			nNumber -= ss->getMaxPercent();
			shipSliderEvent(i + 2,ss->getMaxPercent());
			ss->setCurrentNumber(ss->getMaxPercent());
		}else
		{
			shipSliderEvent(i+2,nNumber);
			ss->setCurrentNumber(nNumber);
			nNumber = 0;
		}
	}
	m_nUnparadox = 0;

//chengyuan++
	auto    i_silver = w_selected_bg->getChildByName("image_silver");
	t_totalPrice->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	float x = t_totalPrice->getPositionX() - t_totalPrice->getContentSize().width - 30;
	i_silver->setPositionX(x);
}

void UIExchange::buyAndSellInfoBtnEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		//sound effect
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_nPageIndex == PAGE_BUY_INDEX)
		{
			ProtocolThread::GetInstance()->getCityProduces(UILoadingIndicator::create(this));
		}else
		{
			ProtocolThread::GetInstance()->getCityDemands(UILoadingIndicator::create(this));
		}
	}
}

void UIExchange::openGoodsBtnEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		//sound effect
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		switch (m_nPageIndex)
		{
		case PAGE_BUY_INDEX:
		case PAGE_SELL_INDEX:
			openGoodInfo(nullptr,0,((Widget*)pSender)->getTag());
			break;
		case PAGE_ORDER_INDEX:
			if (m_bIsSellOrder)
			{
				ProtocolThread::GetInstance()->getItemsDetailInfo(((Widget*)pSender)->getTag(), m_eType, ((Widget*)pSender)->getChildByName<ImageView*>("image_goods")->getTag(), UILoadingIndicator::create(this));
			}
			else
			{
				openGoodInfo(nullptr, m_eType, ((Widget*)pSender)->getTag());
			}	
			break;
		default:
			break;
		}
	}
}

void UIExchange::cityBuyInfo(const GetCityProducesResult *result)
{
	openView(MARKET_COCOS_RES[MARKET_BUY_INFO_CSB]);
	auto buy_info = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_INFO_CSB]);
	auto supply_officer = buy_info->getChildByName<ImageView*>("image_165");
	supply_officer->ignoreContentAdaptWithSize(false);
	supply_officer->loadTexture(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_UNCLE_TRADE));
	auto l_item = buy_info->getChildByName<ListView*>("listview_item");
	auto item1 = l_item->getItem(0);
	auto item2 = l_item->getItem(1);
	item1->retain();
	item2->retain();
	l_item->removeAllChildrenWithCleanup(true);
	l_item->pushBackCustomItem(item1);
	l_item->pushBackCustomItem(item2);
	item1->release();
	item2->release();
	auto b_itemdetails = buy_info->getChildByName<Widget*>("panel_itemdetails");
	for (int i = 0; i < result->n_produces; i++)
	{
		auto b_item = b_itemdetails->clone();
		auto b_goods = b_item->getChildByName<Button*>("button_good_bg");
		b_goods->setName("LocalGoods");
		auto i_good_bg = b_item->getChildByName<ImageView*>("image_item_bg_lv");
		auto i_good  = b_item->getChildByName<ImageView*>("image_goods");
		auto i_black = b_item->getChildByName<ImageView*>("image_black");
		auto i_lock  = b_item->getChildByName<ImageView*>("image_lock");
		auto t_name  = b_item->getChildByName<Text*>("label_goods_name");
		auto t_name1 = b_item->getChildByName<Text*>("label_goods_name_0");
		auto p_require = b_item->getChildByName<Widget*>("panel_require");
		auto t_reputation = p_require->getChildByName<Text*>("label_reputation_num");
		auto label_require = p_require->getChildByName<Text*>("label_require");
		label_require->setString(SINGLE_SHOP->getTipsInfo()["TIP_PALACE_LICENSE_REQUIRE"]);
		b_goods->addTouchEventListener(CC_CALLBACK_2(UIExchange::openGoodsBtnEvent, this));
		b_goods->setTag(result->produces[i]->goodsid);
		i_good->ignoreContentAdaptWithSize(false);
		i_good->loadTexture(getGoodsIconPath(result->produces[i]->goodsid,IMAGE_ICON_OUTLINE));
		t_name->setString(getGoodsName(result->produces[i]->goodsid));
		t_name1->setString(getGoodsName(result->produces[i]->goodsid));
		if (result->produces[i]->requiredtradelevel <= result->currenttradelevel)
		{
			i_black->setVisible(false);
			i_lock->setVisible(false);
			t_name->setVisible(false);
			t_name1->setVisible(true);
			p_require->setVisible(false);
		}else
		{
			i_black->setVisible(true);
			i_lock->setVisible(true);
			t_name->setVisible(true);
			t_name1->setVisible(false);
			p_require->setVisible(true);
			t_reputation->setString(StringUtils::format(">%d",result->produces[i]->requiredtradelevel));
		}
		
		setBgButtonFormIdAndType(b_goods, result->produces[i]->goodsid, ITEM_TYPE_GOOD);
		setBgImageColorFormIdAndType(i_good_bg, result->produces[i]->goodsid, ITEM_TYPE_GOOD);
		l_item->pushBackCustomItem(b_item);
	}
	auto image_pulldown = buy_info->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(l_item,image_pulldown);
}

void UIExchange::citySellInfo(const GetCityDemandsResult *result)
{
	openView(MARKET_COCOS_RES[MARKET_SELL_INFO_CSB]);
	auto sell_info = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_INFO_CSB]);
	auto supply_officer = sell_info->getChildByName<ImageView*>("image_165");
	supply_officer->ignoreContentAdaptWithSize(false);
	supply_officer->loadTexture(getNpcPath(SINGLE_HERO->m_iCityID, FLAG_UNCLE_TRADE));
	auto l_item = sell_info->getChildByName<ListView*>("listview_item");
	auto item1 = l_item->getItem(0);
	auto item2 = l_item->getItem(1);
	item1->retain();
	item2->retain();
	l_item->removeAllChildrenWithCleanup(true);
	l_item->pushBackCustomItem(item1);
	l_item->pushBackCustomItem(item2);
	item1->release();
	item2->release();
	auto b_itemdetails = sell_info->getChildByName<Widget*>("panel_itemdetails");
	for (int i = 0; i < result->n_prices; i++)
	{
		auto b_item = b_itemdetails->clone();
		auto b_goods = b_item->getChildByName<Button*>("button_good_bg");
		b_goods->setName("LocalGoods");
		auto i_good_bg = b_item->getChildByName<ImageView*>("image_item_bg_lv");
		auto i_good  = b_goods->getChildByName<ImageView*>("image_goods");
		auto t_name  = b_item->getChildByName<Text*>("label_goods_name");
		auto p_require = b_item->getChildByName<Widget*>("panel_require");
		auto t_reputation = p_require->getChildByName<Text*>("label_reputation_num");
		auto image_r = b_item->getChildByName<ImageView*>("image_r");
		b_goods->addTouchEventListener(CC_CALLBACK_2(UIExchange::openGoodsBtnEvent, this));
		b_goods->setTag(result->prices[i]->goodsid);
		i_good->ignoreContentAdaptWithSize(false);
		i_good->loadTexture(getGoodsIconPath(result->prices[i]->goodsid,IMAGE_ICON_OUTLINE));
		t_name->setString(getGoodsName(result->prices[i]->goodsid));
		t_reputation->setAnchorPoint(Vec2(0,0.5));
		t_reputation->setTextHorizontalAlignment(TextHAlignment::LEFT);
		t_reputation->setString(numSegment(StringUtils::format("%d",result->prices[i]->price)));
		if (result->prices[i]->ispopular)
		{
			image_r->setVisible(true);
		}else
		{
			image_r->setVisible(false);
		}
		setBgButtonFormIdAndType(b_goods, result->prices[i]->goodsid, ITEM_TYPE_GOOD);
		setBgImageColorFormIdAndType(i_good_bg, result->prices[i]->goodsid, ITEM_TYPE_GOOD);
		l_item->pushBackCustomItem(b_item);
	}
	auto image_pulldown = sell_info->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(l_item,image_pulldown);
}

void UIExchange::changeMainButtonState(Widget *target)
{
	if (!target)
	{
		auto view = getViewRoot(MARKET_COCOS_RES[MARKET_CSB]);
		target = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"button_buy"));
	}
	
	if (m_pMainButton)
	{
		m_pMainButton->setBright(true);
		m_pMainButton->setTouchEnabled(true);
		dynamic_cast<Text*>(m_pMainButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_NORMAL);
	}
	m_pMainButton = target;
	m_pMainButton->setBright(false);
	m_pMainButton->setTouchEnabled(false);
	dynamic_cast<Text*>(m_pMainButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_PASSED);
}

void UIExchange::changeMinorButtonState(Widget *target)
{
	if (!target)
	{
		auto view1 = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
		if (view1)
		{
			target = dynamic_cast<Widget*>(Helper::seekWidgetByName(view1,"button_goods"));
			auto listview_two_butter = dynamic_cast<ListView*>(Helper::seekWidgetByName(view1, "listview_two_butter"));
			listview_two_butter->removeLastItem();
			listview_two_butter->removeLastItem();
			auto b_info = view1->getChildByName<Button*>("button_info");
			b_info->addTouchEventListener(CC_CALLBACK_2(UIExchange::buyAndSellInfoBtnEvent, this));
		}
		auto view2 = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]);
		if (view2)
		{
			auto listview_two_butter = dynamic_cast<ListView*>(Helper::seekWidgetByName(view2, "listview_two_butter"));
			listview_two_butter->removeLastItem();
			listview_two_butter->removeLastItem();
			target = dynamic_cast<Widget*>(Helper::seekWidgetByName(view2,"button_goods"));
			auto b_info = view2->getChildByName<Button*>("button_info");
			b_info->addTouchEventListener(CC_CALLBACK_2(UIExchange::buyAndSellInfoBtnEvent, this));
		}
		auto view3 = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_CSB]);
		if (view3)
		{
			target = dynamic_cast<Widget*>(Helper::seekWidgetByName(view3,"button_order_buy"));
		}
	}
	
	if (m_pMinorButton)
	{
		m_pMinorButton->setBright(true);
		m_pMinorButton->setTouchEnabled(true);
		dynamic_cast<Text*>(m_pMinorButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}
	m_pMinorButton = target;
	m_pMinorButton->setBright(false);
	m_pMinorButton->setTouchEnabled(false);
	dynamic_cast<Text*>(m_pMinorButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
}

void UIExchange::changeOrderButtonState(Widget *target)
{
	if (!target)
	{
		auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
		target = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"button_goods"));
	}
	if (m_pTempButton)
	{
		m_pTempButton->setBright(true);
		m_pTempButton->setTouchEnabled(true);
		dynamic_cast<Text*>(m_pTempButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}
	m_pTempButton = target;
	m_pTempButton->setBright(false);
	m_pTempButton->setTouchEnabled(false);
	dynamic_cast<Text*>(m_pTempButton->getChildren().at(0))->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
}

void UIExchange::openBuyOrder()
{
	openView(MARKET_COCOS_RES[MARKET_ORDER_BUY_CSB]);
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_BUY_CSB]);
	auto p_buy = view->getChildByName<Widget*>("panel_buy_goods");
	auto p_select = p_buy->getChildByName<Widget*>("image_select_num_bg");
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_title_order_buy"));
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHSNGE_BUY_ORDER_TITLE"]);
	
	auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_select, "image_skill_bg"));
	image_skill->setVisible(false);
	for (size_t i = 0; i < m_vSkillDefine.size(); i++)
	{
		if (i == 0)
		{
			image_skill->ignoreContentAdaptWithSize(false);
			image_skill->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill->setVisible(true);
			image_skill->setTag(i);
			image_skill->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
			{
				text_skill_lv->setVisible(false);
			}
			else
			{
				text_skill_lv->setVisible(true);
			}
		}
		else
		{
			auto image_skill_clone = dynamic_cast<ImageView*>(image_skill->clone());
			image_skill_clone->ignoreContentAdaptWithSize(false);
			image_skill_clone->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill_clone->setVisible(true);
			image_skill_clone->setTag(i);
			image_skill_clone->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill_clone, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			image_skill_clone->setPositionX(image_skill->getPositionX() - i*1.1*image_skill->getBoundingBox().size.width);
			p_select->addChild(image_skill_clone);
			if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
			{
				text_skill_lv->setVisible(false);
			}
			else
			{
				text_skill_lv->setVisible(true);
			}
		}
	}

	auto	w_icon_bg = p_buy->getChildByName<Button*>("button_good_bg");
	w_icon_bg->setName("w_icon_bg");
	w_icon_bg->setTag(m_nOrderIndex);
	w_icon_bg->setTouchEnabled(true);
	w_icon_bg->addTouchEventListener(CC_CALLBACK_2(UIExchange::openGoodsBtnEvent, this));
	auto	i_icon = dynamic_cast<ImageView*>(w_icon_bg->getChildByName("image_goods"));
	auto	t_name = dynamic_cast<Text*>(p_buy->getChildByName("label_name"));
	auto	i_price_bg = p_buy->getChildByName("image_input_bg");
	auto    t_price = dynamic_cast<TextField*>(i_price_bg->getChildByName("textfield_price_num"));
	//t_price->setMaxLength(9);
	auto   t_day = p_buy->getChildByName<Text*>("label_expired_time_1");

	auto   p_cost = p_buy->getChildByName<Widget*>("panel_cost_num");
	auto   t_cost = p_cost->getChildByName<Text*>("label_cost_num");

	auto tipInfo = SINGLE_SHOP->getTipsInfo();
	m_nOrderDay = 3;
	std::string s_day = StringUtils::format("%d %s",m_nOrderDay,tipInfo["TIP_DAY"].c_str());
	t_day->setString(s_day);
	std::string itemName;
	std::string itemPath;
	
	getItemNameAndPath(m_eType,m_nOrderIndex,itemName,itemPath);
	switch (m_eType)
	{
	case ITEM_TYPE_GOOD:
		m_nOrderPrice = SINGLE_SHOP->getGoodsData()[m_nOrderIndex].price;
		m_nOrderAmount = 9999;
		break;
	case ITEM_TYPE_SHIP:
		m_nOrderPrice = SINGLE_SHOP->getShipData()[m_nOrderIndex].price;
		m_nOrderAmount = 100;
		break;
	case ITEM_TYPE_SHIP_EQUIP:
	case ITEM_TYPE_PORP:
	case ITEM_TYPE_ROLE_EQUIP:
	case ITEM_TYPE_DRAWING:
		m_nOrderPrice = SINGLE_SHOP->getItemData()[m_nOrderIndex].price;
		m_nOrderAmount = 100;
		break;
	case ITEM_TYPE_SPECIAL:
		m_nOrderPrice = SINGLE_SHOP->getSpecialsInfo()[m_nOrderIndex].price;
		m_nOrderAmount = 100;
		break;
	default:
		m_nOrderAmount = 100;
		break;
	}
		

	auto w_slider_bg = p_select->getChildByName<Slider*>("panel_select_num");
	auto t_amount = dynamic_cast<Text*>(w_slider_bg->getChildByName("label_goods_num"));
	t_amount->setString(StringUtils::format("x %lld",m_nOrderAmount));
	auto nMaxStringNum = 0;
	auto languageTypeNum = 0;
	if (isChineseCharacterIn(itemName.c_str()))
	{
		languageTypeNum = 1;
	}
	if (languageTypeNum)
	{
		nMaxStringNum = 20;
	}
	else
	{
		nMaxStringNum = 37;
	}
	t_name->setString(apostrophe(itemName, nMaxStringNum));
	i_icon->loadTexture(itemPath);

	auto i_bgImage = w_icon_bg->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(w_icon_bg,m_nOrderIndex,m_eType);
	setBgImageColorFormIdAndType(i_bgImage,m_nOrderIndex,m_eType);
	setTextColorFormIdAndType(t_name,m_nOrderIndex,m_eType);

	t_price->setVisible(false);
	Text* text_price = nullptr;
	if (!text_price)
	{
		text_price = Text::create();
		text_price->setString(numSegment(StringUtils::format("%lld", m_nOrderPrice)));
		text_price->setPosition(t_price->getPosition());
		text_price->setFontSize(26);
		t_price->getParent()->addChild(text_price);
		text_price->setAnchorPoint(Vec2(1.0, 0.5));
		text_price->setTag(1);
		text_price->setTouchEnabled(true);
		text_price->addTouchEventListener(CC_CALLBACK_1(UIExchange::textEvent, this));
		text_price->setFontName(CUSTOM_FONT_NAME_1_1);
		text_price->setTextColor(Color4B(40, 25, 13, 255));
		setTextFontSize(text_price);
		text_price->setTextHorizontalAlignment(TextHAlignment::RIGHT);
		text_price->setTextVerticalAlignment(TextVAlignment::CENTER);
		text_price->setContentSize(t_price->getContentSize());
		text_price->ignoreContentAdaptWithSize(false);
	}
	std::string  s_tax_title = StringUtils::format("%s(%.2f%%):",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(),m_nOrderTax/100.0);
	auto   l_tax = p_select->getChildByName<Text*>("label_tax");
	auto   p_tax = p_select->getChildByName<Widget*>("panel_tax_num");
	auto   t_tax = p_tax->getChildByName<Text*>("label_price_num");
	
	std::string  s_fee_title =  StringUtils::format("%s:",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_FEE"].data());
	auto   l_fee = p_select->getChildByName<Text*>("label_fee");
	auto   p_fee = p_select->getChildByName<Widget*>("panel_fee_num");
	auto   t_fee = p_fee->getChildByName<Text*>("label_price_num");

	l_tax->setString(s_tax_title);
	t_tax->setString(StringUtils::format("%lld",int64_t(ceil((m_nOrderPrice * m_nOrderAmount)* m_nOrderTax / 10000.0))));
	t_cost->setString(String::createWithFormat("%lld",int64_t((m_nOrderPrice * m_nOrderAmount)*(1.0 + m_nOrderTax / 10000.0) + m_nOrderFee))->_string);
	l_fee->setString(s_fee_title);
	t_fee->setString(StringUtils::format("%lld",m_nOrderFee));
	dynamic_cast<Layout*>(p_tax)->requestDoLayout();
	dynamic_cast<Layout*>(p_fee)->requestDoLayout();
	dynamic_cast<Layout*>(p_cost)->requestDoLayout();
	auto ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(200));
	if (ss)
	{
		ss->removeFromParentAndCleanup(true);
	}
	ss = UIVoyageSlider::create(w_slider_bg,m_nOrderAmount,0,true);
	ss->setTag(200);
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIExchange::goodsNumberChangeEvent,this));;
	this->addChild(ss);

	auto b_ok = view->getChildByName<Widget*>("button_ok");
	b_ok->addTouchEventListener(CC_CALLBACK_2(UIExchange::orderOkBtnEvent, this));
}

void UIExchange::openSellOrder()
{
	openView(MARKET_COCOS_RES[MARKET_ORDER_SELL_CSB]);
	auto view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_SELL_CSB]);
	auto p_buy = view->getChildByName<Widget*>("panel_buy_goods");
	auto p_select = p_buy->getChildByName<Widget*>("image_select_num_bg");
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_title_order_buy"));
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_EXCHSNGE_SELL_ORDER_TITLE"]);

	auto image_skill = dynamic_cast<ImageView*>(Helper::seekWidgetByName(p_select, "image_skill_bg"));
	image_skill->setVisible(false);
	for (size_t i = 0; i < m_vSkillDefine.size(); i++)
	{
		if (i == 0)
		{
			image_skill->ignoreContentAdaptWithSize(false);
			image_skill->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill->setVisible(true);
			image_skill->setTag(i);
			image_skill->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
			{
				text_skill_lv->setVisible(false);
			}
			else
			{
				text_skill_lv->setVisible(true);
			}
		}
		else
		{
			auto image_skill_clone = dynamic_cast<ImageView*>(image_skill->clone());
			image_skill_clone->ignoreContentAdaptWithSize(false);
			image_skill_clone->loadTexture(getSkillIconPath(m_vSkillDefine.at(i).id, m_vSkillDefine.at(i).skill_type));
			image_skill_clone->setVisible(true);
			image_skill_clone->setTag(i);
			image_skill_clone->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
			auto text_skill_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(image_skill_clone, "text_item_skill_lv"));
			setTextSizeAndOutline(text_skill_lv, m_vSkillDefine.at(i).lv);
			image_skill_clone->setPositionX(image_skill->getPositionX() - i*1.1*image_skill->getBoundingBox().size.width);
			p_select->addChild(image_skill_clone);
			if (m_vSkillDefine.at(i).skill_type == SKILL_TYPE_PARTNER_SPECIAL)
			{
				text_skill_lv->setVisible(false);
			}
			else
			{
				text_skill_lv->setVisible(true);
			}
		}
	}
	
	auto	w_icon_bg = p_buy->getChildByName<Button*>("button_good_bg");
	auto	i_icon = dynamic_cast<ImageView*>(w_icon_bg->getChildByName("image_goods"));
	auto	t_name = dynamic_cast<Text*>(p_buy->getChildByName("label_name"));
	auto	i_price_bg = p_buy->getChildByName("image_input_bg");
	auto    t_price = dynamic_cast<TextField*>(i_price_bg->getChildByName("textfield_price_num"));
	//t_price->setMaxLength(9);
	auto   t_day = p_buy->getChildByName<Text*>("label_expired_time_1");

	auto   p_cost = p_buy->getChildByName<Widget*>("panel_cost_num");
	auto   t_cost = p_cost->getChildByName<Text*>("label_cost_num");

	auto tipInfo = SINGLE_SHOP->getTipsInfo();
	m_nOrderDay = 3;
	std::string s_day = StringUtils::format("%d %s",m_nOrderDay,tipInfo["TIP_DAY"].c_str());
	t_day->setString(s_day);

	std::string itemName;
	std::string itemPath;
	GetItemsToSellResult *result = (GetItemsToSellResult*)m_pResult;
	m_nOrderPrice = result->items[m_nOrderIndex]->lastprice;
	m_nOrderAmount = result->items[m_nOrderIndex]->amount;
	float lastPrice = result->items[m_nOrderIndex]->averagecost;
	getItemNameAndPath(m_eType,result->items[m_nOrderIndex]->itemid,itemName,itemPath);
	if (result->items[m_nOrderIndex]->user_define_name && std::strcmp(result->items[m_nOrderIndex]->user_define_name, "") != 0)
	{
		itemName = result->items[m_nOrderIndex]->user_define_name;
	}
	t_name->setString(itemName);
	

	auto w_slider_bg = p_select->getChildByName<Slider*>("panel_select_num");
	auto t_amount = dynamic_cast<Text*>(w_slider_bg->getChildByName("label_goods_num"));
	t_amount->setString(StringUtils::format("x %lld",m_nOrderAmount));
	auto nMaxStringNum = 0;
	auto languageTypeNum = 0;
	if (isChineseCharacterIn(itemName.c_str()))
	{
		languageTypeNum = 1;
	}
	if (languageTypeNum)
	{
		nMaxStringNum = 20;
	}
	else
	{
		nMaxStringNum = 37;
	}
	t_name->setString(apostrophe(itemName, nMaxStringNum));
	t_name->setFontName("");
	t_name->setFontSize(32);
	i_icon->loadTexture(itemPath);
	i_icon->setTag(result->items[m_nOrderIndex]->characteritemid);

	auto i_bgImage = w_icon_bg->getChildByName<ImageView*>("image_item_bg_lv");
	setBgButtonFormIdAndType(w_icon_bg,result->items[m_nOrderIndex]->itemid,m_eType);
	setBgImageColorFormIdAndType(i_bgImage,result->items[m_nOrderIndex]->itemid,m_eType);
	setTextColorFormIdAndType(t_name,result->items[m_nOrderIndex]->itemid,m_eType);
	if (result->items[m_nOrderIndex]->optionalitemnum)
	{
		addStrengtheningIcon(w_icon_bg);
	}
	t_price->setVisible(false);
	Text* text_price = nullptr;
	if (!text_price)
	{
		text_price = Text::create();
		text_price->setString(numSegment(StringUtils::format("%lld", m_nOrderPrice)));
		text_price->setPosition(t_price->getPosition());
		text_price->setFontSize(26);
		t_price->getParent()->addChild(text_price);
		text_price->setAnchorPoint(Vec2(1.0, 0.5));
		text_price->setTag(1);
		text_price->setTouchEnabled(true);
		text_price->addTouchEventListener(CC_CALLBACK_1(UIExchange::textEvent, this));
		text_price->setFontName(CUSTOM_FONT_NAME_1_1);
		text_price->setTextColor(Color4B(46, 29, 14, 255));
		setTextFontSize(text_price);
		text_price->setTextHorizontalAlignment(TextHAlignment::RIGHT);
		text_price->setTextVerticalAlignment(TextVAlignment::CENTER);
		text_price->setContentSize(t_price->getContentSize());
		text_price->ignoreContentAdaptWithSize(false);
	}
	w_icon_bg->setName("w_icon_bg");
	w_icon_bg->setTag(result->items[m_nOrderIndex]->itemid);
	w_icon_bg->setTouchEnabled(true);
	w_icon_bg->addTouchEventListener(CC_CALLBACK_2(UIExchange::openGoodsBtnEvent, this));

	std::string  s_tax_title = StringUtils::format("%s(%.2f%%):",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_TEX"].data(),m_nOrderTax/100.0);
	auto   l_tax = p_select->getChildByName<Text*>("label_tax");
	auto   p_tax = p_select->getChildByName<Widget*>("panel_tax_num");
	auto   t_tax = p_tax->getChildByName<Text*>("label_price_num");
	
	std::string  s_fee_title =  StringUtils::format("%s:",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_FEE"].data());
	auto   l_fee = p_select->getChildByName<Text*>("label_fee");
	auto   p_fee = p_select->getChildByName<Widget*>("panel_fee_num");
	auto   t_fee = p_fee->getChildByName<Text*>("label_price_num");

	std::string  s_profit_title =  StringUtils::format("%s:",SINGLE_SHOP->getTipsInfo()["TIP_EXCHANGE_PROFIT"].data());
	auto   l_profit = p_select->getChildByName<Text*>("label_profit");
	auto   p_profit  = p_select->getChildByName<Widget*>("panel_profit_num");
	auto   t_profit  = p_profit->getChildByName<Text*>("label_price_num");

	l_tax->setString(s_tax_title);
	t_tax->setString(numSegment(StringUtils::format("%lld",int64_t(ceil((m_nOrderPrice * m_nOrderAmount)* m_nOrderTax / 10000.0)))));
	t_cost->setString(numSegment(StringUtils::format("%lld",int64_t((m_nOrderPrice * m_nOrderAmount)*(1.0 - m_nOrderTax / 10000.0) - m_nOrderFee))));
	t_fee->setString(StringUtils::format("%lld",m_nOrderFee));
	l_profit->setString(s_profit_title);
	t_profit->setString(numSegment(StringUtils::format("%lld",int64_t(((m_nOrderPrice - lastPrice) * m_nOrderAmount) * (1.0 - m_nOrderTax / 10000.0) - m_nOrderFee))));
	dynamic_cast<Layout*>(p_tax)->requestDoLayout();
	dynamic_cast<Layout*>(p_fee)->requestDoLayout();
	dynamic_cast<Layout*>(p_cost)->requestDoLayout();
	dynamic_cast<Layout*>(p_profit)->requestDoLayout();
	auto ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(200));
	if (ss)
	{
		ss->removeFromParentAndCleanup(true);
	}
	ss = UIVoyageSlider::create(w_slider_bg,m_nOrderAmount,0,true);
	ss->setTag(200);
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UIExchange::goodsNumberChangeEvent,this));;
	this->addChild(ss);

	auto b_ok = view->getChildByName<Widget*>("button_ok");
	b_ok->addTouchEventListener(CC_CALLBACK_2(UIExchange::orderOkBtnEvent, this));
}

void UIExchange::goodsNumberChangeEvent(int num)
{
	Widget *view = nullptr;
	if (m_bIsSellOrder)
	{
		view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_SELL_CSB]);
	}else
	{
		view = getViewRoot(MARKET_COCOS_RES[MARKET_ORDER_BUY_CSB]);
	}

	m_nOrderAmount = num;
	auto t_amount = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_goods_num"));
	t_amount->setString(StringUtils::format("x %d",m_nOrderAmount));
	
	auto p_tax = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_tax_num"));
	auto t_tax = p_tax->getChildByName<Text*>("label_price_num");
	t_tax->setString(numSegment(StringUtils::format("%lld",int64_t(ceil(m_nOrderPrice * m_nOrderAmount *  m_nOrderTax / 10000.0)))));
	dynamic_cast<Layout*>(p_tax)->requestDoLayout();

	auto   p_cost = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_cost_num"));
	auto   t_cost = p_cost->getChildByName<Text*>("label_cost_num");
	if (m_bIsSellOrder)
	{
		t_cost->setString(numSegment(StringUtils::format("%lld",int64_t(m_nOrderPrice * m_nOrderAmount * (1.0 - m_nOrderTax / 10000.0) - m_nOrderFee))));
		auto p_profit = dynamic_cast<Widget*>(Helper::seekWidgetByName(view,"panel_profit_num"));
		auto  t_profit = p_profit->getChildByName<Text*>("label_price_num");
		GetItemsToSellResult *result = (GetItemsToSellResult*)m_pResult;
		m_nAverageCost = result->items[m_nOrderIndex]->averagecost;
		t_profit->setString(numSegment(StringUtils::format("%lld",int64_t(((m_nOrderPrice - m_nAverageCost) * m_nOrderAmount) * (1.0 - m_nOrderTax / 10000.0) - m_nOrderFee))));
		dynamic_cast<Layout*>(p_profit)->requestDoLayout();
	}else
	{
		t_cost->setString(numSegment(StringUtils::format("%lld",int64_t(ceil(m_nOrderPrice * m_nOrderAmount *  (1.0 + m_nOrderTax / 10000.0) + m_nOrderFee)))));
	}
	dynamic_cast<Layout*>(p_cost)->requestDoLayout();
//chengyuan++
	auto i_silver = view->getChildByName("image_silver");
	t_cost->ignoreContentAdaptWithSize(true);
	t_cost->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	i_silver->setPositionX(t_cost->getPositionX() - t_cost->getContentSize().width - i_silver->getContentSize().width / 2);
//
}

void UIExchange::orderOkBtnEvent(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
	if (m_nOrderPrice < 1)
	{
		openSuccessOrFailDialog("TIP_EXCHANG_ORDER_MIN_PRICE");
	}
	else
	{
		if (m_bIsSellOrder)
		{
			auto result = (GetItemsToSellResult*)m_pResult;
			ProtocolThread::GetInstance()->createOrder(m_eType, result->items[m_nOrderIndex]->itemid, m_nOrderAmount,
				m_nOrderPrice, m_nOrderDay, result->items[m_nOrderIndex]->optionalnum, result->items[m_nOrderIndex]->characteritemid,
				SINGLE_HERO->m_iCityID, 0, UILoadingIndicator::create(this));
		}
		else
		{
			ProtocolThread::GetInstance()->createOrder(m_eType, m_nOrderIndex,
				m_nOrderAmount, m_nOrderPrice, m_nOrderDay, 0, 0, SINGLE_HERO->m_iCityID, 1, UILoadingIndicator::create(this));
		}
		closeView(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	}
}

void UIExchange::textEvent(Ref* target)
{
	openView(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
 	if (m_bNumpadFlag)
	{
		Text* temp = dynamic_cast<Text*>(target);
		showNumpad(temp);
	}
}
void UIExchange::touchButtonListview(Ref *pSender, ListViewEventType TouchType)
{
	switch (TouchType)
	{
	case LISTVIEW_ONSELECTEDITEM_START:
	{
		 if (m_pBuyListImagePulldown)
		 {
			 m_pBuyListImagePulldown->setVisible(true);
		 }
		 if (m_pSellListImagePulldown)
		{
			 m_pSellListImagePulldown->setVisible(true);
		}
		
		break;
	}
	case LISTVIEW_ONSELECTEDITEM_END:
	{
		 if (m_pBuyListImagePulldown)
		 {
			 m_pBuyListImagePulldown->setVisible(false);
		 }
		 if (m_pSellListImagePulldown)
		 {
			 m_pSellListImagePulldown->setVisible(false);
		 }
		break;
	}
	default:
	break;
	}
}

void UIExchange::scrollEvent(Ref* target,ScrollviewEventType type)
{

	if (type == ScrollviewEventType::SCROLLVIEW_EVENT_SCROLLING) {
		ListView *lv = dynamic_cast<ListView*>(target);
		Widget *sroll = dynamic_cast<Widget*>(lv->getUserObject());
		Size innerContainer = lv->getInnerContainerSize();
		Size lv_size = lv->getContentSize();
		if (lv->getUserObject() == nullptr) {
			lv->unscheduleAllSelectors();
			return;
		}

		Point innerCon_pos = lv->getInnerContainer()->getPosition();
		Point lv_pos = lv->getPosition();
		if (innerContainer.height < lv_size.height + 10) {
			lv->unscheduleAllSelectors();
			return;
		}
		if (!sroll->isVisible()) {
			sroll->setVisible(true);
			return;
		}
		Size img_size = sroll->getSize();
		auto len = dynamic_cast<Widget*>(sroll->getParent())->getSize().height - img_size.height - 17 * 2;
		auto factor = std::fabs(innerCon_pos.y * 1.0 / (innerContainer.height - lv_size.height));
		//float per = std::fabs(innerCon_pos.y/(innerContainer.height - lv_size.height + 0.01));
		sroll->setPositionY(len * factor + 17);
	} else if (type == ScrollviewEventType::SCROLLVIEW_EVENT_SCROLL_TO_TOP) {

	}
}
std::string UIExchange::vticItemName(std::string itemName)
{
	auto nameLen = itemName.length();
	LanguageType nType = LanguageType(Utils::getLanguage());
	switch (nType)
	{
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
	case cocos2d::LanguageType::CHINESE:
	{
										   if (nameLen > 3 * 8)//utf8大多数一个汉字3字节
										   {
											   itemName = itemName.substr(0, 3 * 8) + "...";
										   }
										   break;
	}
	default:
	{
			   if (nameLen > 32)
			   {
				   itemName = itemName.substr(0, 25) + "...";
			   }
			   break;
	}
	}
	return itemName;
}

void UIExchange::scrollButtonEvent(Ref *pSender, cocos2d::ui::ScrollView::EventType type)
{
	if (type == cocos2d::ui::ScrollView::EventType::SCROLL_TO_TOP)
	{
		if (m_nPageIndex == PAGE_BUY_INDEX)
		{
			auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
			auto b_more = view->getChildByName<Button*>("button_more");
			b_more->setBright(true);
		}
		else if (m_nPageIndex == PAGE_SELL_INDEX)
		{
			auto view = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]);
			auto b_more = view->getChildByName<Button*>("button_more");
			b_more->setBright(true);
		}else
		{
			auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
			auto b_more = view->getChildByName<Button*>("button_more");
			b_more->setBright(true);
		}
	}
	else if (type == cocos2d::ui::ScrollView::EventType::SCROLL_TO_BOTTOM)
	{
		if (m_nPageIndex == PAGE_BUY_INDEX)
		{
			auto view = getViewRoot(MARKET_COCOS_RES[MARKET_BUY_CSB]);
			auto b_more = view->getChildByName<Button*>("button_more");
			b_more->setBright(false);
		}
		else if (m_nPageIndex == PAGE_SELL_INDEX)
		{
			auto view = getViewRoot(MARKET_COCOS_RES[MARKET_SELL_CSB]);
			auto b_more = view->getChildByName<Button*>("button_more");
			b_more->setBright(false);
		}
		else
		{
			auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ITEM_CHOOSE_CSB]);
			auto b_more = view->getChildByName<Button*>("button_more");
			b_more->setBright(false);
		}
	}
}
void UIExchange::showNumpad(Text* inputNum)
{
	    m_bNumpadFlag = false;
	    auto numpad = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
		numpad->setPosition(STARTPOS);
		if (numpad)
		{
			auto num_root = numpad->getChildByName<Widget*>("panel_numpad");
			num_root->setPosition(ENDPOS4);
			auto panel_silver = numpad->getChildByName<Widget*>("panel_silver");
			panel_silver->setPosition(Vec2(0,586));
			num_root->runAction(Sequence::createWithTwoActions(MoveTo::create(0.4f, STARTPOS), CallFunc::create(this, callfunc_selector(UIExchange::movedEnd))));
			num_root->getChildByName<Button*>("button_yes")->setTouchEnabled(true);
			numpad->addTouchEventListener(CC_CALLBACK_2(UIExchange::menuCall_func, this));
			numpad->setTouchEnabled(true);
			updateCoinNum(SINGLE_HERO->m_iGold, SINGLE_HERO->m_iCoin);
			m_inputNum = inputNum;
			for (int i = 0; i < 10; i++)
			{
				auto b_num = num_root->getChildByName<Button*>(StringUtils::format("button_%d", i));
				b_num->addTouchEventListener(CC_CALLBACK_2(UIExchange::buttonNumpadEvent, this));
			}
		}
}
void UIExchange::buttonNumpadEvent(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED == TouchType)
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto button = dynamic_cast<Button*>(pSender);
		int tag = button->getTag();
		std::string str = m_inputNum->getString();
		if (str.size() < 2 && str == "0")
		{
			str.erase(str.end() - 1);
		}
		str += StringUtils::format("%d", tag);
		m_inputNum->setString(str);

		std::string value = m_inputNum->getString();
		int a = 0;
		std::string str1 = value;
		std::string str2 = "";
		a = str1.find_first_of(",");
		if (a == -1)
		{
			m_nOrderPrice = atoi(value.c_str());
			if (m_nOrderPrice <= 0)
			{
				m_nOrderPrice = 0;
			}
			if (m_nOrderPrice >= 1000000000)
			{
				m_nOrderPrice = 1000000000;
			}

			m_inputNum->setString(numSegment(StringUtils::format("%lld", m_nOrderPrice)));
		}
		else
		{
			do
			{
				a = str1.find(",");
				if (a != -1)
				{
					str2 += str1.substr(0, a);
					str1 = str1.substr(a + 1, std::string::npos);
				}
				else
				{
					str2 += str1;
				}

			} while (a != -1);

			m_nOrderPrice = atoi(str2.c_str());
			if (m_nOrderPrice >= 1000000000)
			{
				m_nOrderPrice = 1000000000;
			}
			m_inputNum->setString(numSegment(StringUtils::format("%lld", m_nOrderPrice)));
		}
		goodsNumberChangeEvent(m_nOrderAmount);
	}
}
void UIExchange::closeNumpad()
{
	auto numpad = getViewRoot(COMMOM_COCOS_RES[C_VIEW_NUMPAD_CSB]);
	if (numpad)
	{
		numpad->setTouchEnabled(false);
		auto num_root = numpad->getChildByName<Widget*>("panel_numpad");
		num_root->getChildByName<Button*>("button_yes")->setTouchEnabled(false);
		num_root->runAction(MoveTo::create(0.5, ENDPOS4));
		numpad->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5), Place::create(ENDPOS)));
		m_nOrderPrice = atoi(m_inputNum->getString().c_str());
	}
}