#include "UIPackageDeals.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UIStore.h"
#include "TVSceneLoader.h"
#include "UIMain.h"
#include "Utils.h"

UIPackageDeals::UIPackageDeals()
{
	myPageView = nullptr;
	pagesCount = 0;
	m_pOnsaleItemResults = nullptr;
	shipSale.clear();
	equipSale.clear();
	itemsale.clear();
	m_pBuyItemResults = nullptr;
	m_bBuyTimeLimitGood = false;
	m_eUIType = UI_PORT;
}
UIPackageDeals::~UIPackageDeals()
{
	shipSale.clear();
	equipSale.clear();
	itemsale.clear();
}
UIPackageDeals*UIPackageDeals::createGiftLayer()
{
	UIPackageDeals*giftLayer = new UIPackageDeals;
	CC_RETURN_IF(giftLayer);
	if (giftLayer->init())
	{
		giftLayer->autorelease();
		return giftLayer;
	}
	CC_SAFE_RELEASE(giftLayer);
	return nullptr;
}
bool UIPackageDeals::init()
{
	bool pRet = false;
	do 
	{
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIPackageDeals::onServerEvent, this), this);
		this->scheduleOnce(schedule_selector(UIPackageDeals::initStaticData), 0);
		ProtocolThread::GetInstance()->getOnsaleItems(UILoadingIndicator::create(this));
		pRet = true;

	} while (0);
	return pRet;

}
void UIPackageDeals::initStaticData(float f)
{
	openView(GIFTPACK_RES[GIFTPACK_CSB]);
	auto view = getViewRoot(GIFTPACK_RES[GIFTPACK_CSB]);
	auto pageview_giftpack = view->getChildByName<PageView*>("pageview_giftpack");
	auto btnLeft = view->getChildByName<Button*>("button_left");
	auto btnRight = view->getChildByName<Button*>("button_right");
	btnRight->setVisible(false);//开始第一页，右键不显示
	btnLeft->setVisible(false);
	myPageView = pageview_giftpack;
	myPageView->setTouchEnabled(false);
	buyGoodsBtn = view->getChildByName<Button*>("button_buy_price");
	
}
void UIPackageDeals::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto button = static_cast<Widget*>(pSender);
	std::string name = button->getName();
	auto viewGift = getViewRoot(GIFTPACK_RES[GIFTPACK_CSB]);
	auto btnLeft = viewGift->getChildByName<Button*>("button_left");
	auto btnRight = viewGift->getChildByName<Button*>("button_right");
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	//关闭页面
	if (isButton(button_close))
	{
		if (isScheduled(schedule_selector(UIPackageDeals::showLimitTime)))
		{
			this->unschedule(schedule_selector(UIPackageDeals::showLimitTime));
		}
		closeView();
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		return;
	}
	//左箭头
	if (isButton(button_left))
	{
		auto pageNow = myPageView->getCurPageIndex();
		myPageView->scrollToPage(++pageNow);
		
			if (m_pOnsaleItemResults->remaintime&&m_pOnsaleItemResults->timelevelgood)
			{
				buyGoodsBtn->setTitleText(StringUtils::format("%d", m_pOnsaleItemResults->goods[pageNow-1]->vticket));
			}
			else
			{
				buyGoodsBtn->setTitleText(StringUtils::format("%d", m_pOnsaleItemResults->goods[pageNow]->vticket));
			}

		if (myPageView->getCurPageIndex() ==pagesCount-1)
		{
			btnLeft->setVisible(false);
		}
		if (pageNow > 0 && pageNow <= pagesCount - 1)
		{
			btnRight->setVisible(true);
		}	
		return;
	}
	//右箭头
	if (isButton(button_right))
	{
		auto pageNow = myPageView->getCurPageIndex();
		myPageView->scrollToPage(--pageNow);
		
			if (m_pOnsaleItemResults->remaintime &&m_pOnsaleItemResults->timelevelgood)
			{			
				if (myPageView->getCurPageIndex()==0)
				{
					buyGoodsBtn->setTitleText(StringUtils::format("%d", m_pOnsaleItemResults->timelevelgood->vticket));
				}
				else
				{
					buyGoodsBtn->setTitleText(StringUtils::format("%d", m_pOnsaleItemResults->goods[pageNow]->vticket));
				}
			}
			else
			{
				buyGoodsBtn->setTitleText(StringUtils::format("%d", m_pOnsaleItemResults->goods[pageNow]->vticket));
			}
		if (pageNow ==0)
		{
			btnRight->setVisible(false);
			btnLeft->setVisible(true);
		}
		if (pageNow > 0 && pageNow <= pagesCount - 1)
		{
			btnLeft->setVisible(true);
		}
		
		return;
	}
	//购买物品
	if (isButton(button_buy_price))
	{
		
		int costV = 0;
		auto pageNow = myPageView->getCurPageIndex();
		if (pageNow == 0)
		{
			if (m_pOnsaleItemResults->remaintime&&m_pOnsaleItemResults->timelevelgood)
			{
				costV = m_pOnsaleItemResults->timelevelgood->vticket;
			}
			else
			{
				costV = m_pOnsaleItemResults->goods[0]->vticket;
			}
		}
		else
		{
			if (m_pOnsaleItemResults->remaintime&&m_pOnsaleItemResults->timelevelgood)
			{
				costV = m_pOnsaleItemResults->goods[pageNow - 1]->vticket;
			}
			else
			{
				costV = m_pOnsaleItemResults->goods[pageNow]->vticket;
			}
		}
		if (costV <= SINGLE_HERO->m_iGold)
		{
			showOnsaleCost();
		}
		else
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openGoldNotEnoughYesOrNo(costV);
		}

		return;
	}
	//V票不足打开商城
	if (isButton(button_confirm_yes))
	{
		UIStore::getInstance()->openVticketStoreLayer(m_eUIType, 0);
	}
	//确定购买物品
	if (isButton(button_v_yes))
	{
		auto pageNow = myPageView->getCurPageIndex();
		if (pageNow == 0)
		{
			if (m_pOnsaleItemResults->remaintime&&m_pOnsaleItemResults->timelevelgood)
			{
				m_bBuyTimeLimitGood = true;
				ProtocolThread::GetInstance()->buyOnsaleItems(m_pOnsaleItemResults->timelevelgood->uniqueid, m_pOnsaleItemResults->timelevelgood->type, UILoadingIndicator::create(this));
			}
			else
			{
				ProtocolThread::GetInstance()->buyOnsaleItems(m_pOnsaleItemResults->goods[0]->uniqueid, m_pOnsaleItemResults->goods[pageNow]->type, UILoadingIndicator::create(this));
			}
		}
		else
		{
			if (m_pOnsaleItemResults->remaintime&&m_pOnsaleItemResults->timelevelgood)
			{
				ProtocolThread::GetInstance()->buyOnsaleItems(m_pOnsaleItemResults->goods[pageNow - 1]->uniqueid, m_pOnsaleItemResults->goods[pageNow - 1]->type, UILoadingIndicator::create(this));
			}
			else
			{
				ProtocolThread::GetInstance()->buyOnsaleItems(m_pOnsaleItemResults->goods[pageNow]->uniqueid, m_pOnsaleItemResults->goods[pageNow]->type, UILoadingIndicator::create(this));
			}

		}
		closeView();
		return;
	}
	//取消购买
	if (isButton(button_v_no))
	{
		closeView();
		return;
	}
}
void UIPackageDeals::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetOnsaleItemsResult:
	{
		GetOnsaleItemsResult* result = (GetOnsaleItemsResult*)message;	
		if (result->failed==0)
		{
			m_pOnsaleItemResults = result;
			//fix 第一版只显示限时礼包
			m_pOnsaleItemResults->n_goods = 0;
			result->n_goods = 0;
			if (!result->timelevelgood||!result->remaintime)
			{
				pagesCount = result->n_goods;//普通物品组数
			}
			else
			{
				pagesCount = result->n_goods+1;//add限时物品
			}
			updateGiftPage();
		}
		else
		{
			
		}
		break;
	}
	case PROTO_TYPE_BuyOnsaleItemsResult:
	{
		BuyOnsaleItemsResult*result = (BuyOnsaleItemsResult*)message;
		if (result->failed == 0)
		{
			m_pBuyItemResults = result;
			updateMaincityCoin();
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openViewAutoClose("TIP_MIANCITY_GIFT_PACKAGE_BUYGOODS_SUCC");
			if (m_bBuyTimeLimitGood)
			{
				m_bBuyTimeLimitGood = false;
				auto currentScene = Director::getInstance()->getRunningScene();
				auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
				if (mainlayer)
				{
					mainlayer->showGiftPackBg(false);
					//fix 第一版只显示限时礼包
					//................begin..............
					if (isScheduled(schedule_selector(UIPackageDeals::showLimitTime)))
					{
						this->unschedule(schedule_selector(UIPackageDeals::showLimitTime));
					}
					closeView(GIFTPACK_RES[GIFTPACK_CSB]);
					ProtocolThread::GetInstance()->unregisterMessageCallback(this);
					//.................end..................
				}
			}
			Utils::consumeVTicket("3", 1, result->price);
			//ProtocolThread::GetInstance()->getOnsaleItems(LoadingLayer::create(this));
		}
		else if (result->failed ==104)
		{
			openSuccessOrFailDialog("TIP_VTICKETSHOP_BAG_FULL");
		}
		else if (result->failed == 112)//V票不够
		{
             std::string info = SINGLE_SHOP->getTipsInfo()["TIP_GOLD_NOT_CONTENT"];
			std::string old_value = "[num]";
			std::string new_value = StringUtils::format("%d", m_pBuyItemResults->golds);
			repalce_all_ditinct(info, old_value, new_value);
			openSuccessOrFailDialogWithString(info);
			
		}
		break;
	}
	default:
	break;
	}
}
void UIPackageDeals::updateGiftPage()
{
	shipSale.clear();
	equipSale.clear();
	itemsale.clear();
	auto view = getViewRoot(GIFTPACK_RES[GIFTPACK_CSB]);
	auto pageview_giftpack = view->getChildByName<PageView*>("pageview_giftpack");
	auto btnLeft = view->getChildByName<Button*>("button_left");
	auto btnRight = view->getChildByName<Button*>("button_right");
	auto panel_pp = view->getChildByName<Widget*>("panel_pp");
	panel_pp->retain();
	panel_pp->removeFromParentAndCleanup(true);
	pageview_giftpack->removeAllPages();
	pageview_giftpack->addWidgetToPage(panel_pp, 0, true);
	if (pagesCount ==1)
	{
		btnLeft->setVisible(false);
		btnRight->setVisible(false);
	}
	else
	{
		//第一版本只显示限时礼包
		btnLeft->setVisible(false);
		btnRight->setVisible(false);
		/*if (pageview_giftpack->getCurPageIndex()==0)
		{
			btnLeft->setVisible(true);
		}
		else
		{
			btnLeft->setVisible(false);
		}*/
		
	}
	for (int i = 0;i < pagesCount-1;i++)
	{
		auto panelGift_1 = panel_pp->clone();
		pageview_giftpack->addWidgetToPage(panelGift_1, i + 1, true);
	}
	//显示限时物品
	if (m_pOnsaleItemResults->timelevelgood)
	{
		showLimitTime(0);
		showLimitPageGoods();
		buyGoodsBtn->setTitleText(StringUtils::format("%d", m_pOnsaleItemResults->timelevelgood->vticket));
		
		this->schedule(schedule_selector(UIPackageDeals::showLimitTime), 1.0f);
	}
	else
	{
		buyGoodsBtn->setTitleText(StringUtils::format("%d", m_pOnsaleItemResults->goods[0]->vticket));
	}
	
	//显示非限时物品
	for (int i = 0; i < m_pOnsaleItemResults->n_goods; i++)//多少组非限时物品
	{
		for (int j= 0; j < m_pOnsaleItemResults->goods[i]->n_onsaleship;j++)
		{
			shipSale.push_back(m_pOnsaleItemResults->goods[i]->onsaleship[j]);//存进的每条船
		}
		for (int k = 0; k < m_pOnsaleItemResults->goods[i]->n_onsaleequip; k++)
		{
			equipSale.push_back(m_pOnsaleItemResults->goods[i]->onsaleequip[k]);//存进的每个装备
		}
		for (int m = 0; m < m_pOnsaleItemResults->goods[i]->n_onsaleitem; m++)
		{
			itemsale.push_back(m_pOnsaleItemResults->goods[i]->onsaleitem[m]);//存进的每个物品
		}
		if (!m_pOnsaleItemResults->timelevelgood)
		{
			auto pageShow = myPageView->getPage(i);
			showCommonPageGoods(pageShow);
		}
		else
		{
			 auto pageShow = myPageView->getPage(i+1);
			 showCommonPageGoods(pageShow);

		}
	}

}
void UIPackageDeals::showCommonPageGoods(Layout*pageShow)
{
	auto label_content_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(pageShow, "label_content_1"));
	auto label_content_time_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(pageShow, "label_content_time_1"));
	auto label_content_time_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(pageShow, "label_content_time_2"));
	label_content_1->setVisible(true);
	label_content_time_1->setVisible(false);
	label_content_time_2->setVisible(false);
	label_content_1->setString(SINGLE_SHOP->getTipsInfo()["TIP_MIANCITY_GIFT_PACKAGE_NOTIME_LIMIT_TITLE"]);
	if (shipSale.size() != 0)
	{
		for (int index = 0; index < shipSale.size(); index++)
		{
			auto panelItem = dynamic_cast<Widget*>(Helper::seekWidgetByName(pageShow, StringUtils::format("panel_item_%d", index + 1)));
			auto image_goods = panelItem->getChildByName<ImageView*>("image_goods");
			auto button_1 = panelItem->getChildByName<Button*>("button_1");
			button_1->setLocalZOrder(100);
			button_1->setTag(shipSale[index]->itemid + ONSALE_SHIP_TAG);
			button_1->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			image_goods->setVisible(true);
			image_goods->setTag(shipSale[index]->itemid + ONSALE_SHIP_TAG);
			image_goods->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			auto label_goods_name = panelItem->getChildByName<Text*>("label_goods_name");
			auto label_num = panelItem->getChildByName<Text*>("label_num");
			image_goods->loadTexture(getShipIconPath(shipSale[index]->itemid, IMAGE_ICON_SHADE));
			label_goods_name->setString(getShipName(shipSale[index]->itemid));
			label_num->setString(StringUtils::format("x %d", shipSale[index]->amount));
		}
	}
	if (equipSale.size() != 0)
	{
		for (int indexequipSale = 0; indexequipSale < equipSale.size(); indexequipSale++)
		{
			auto panelItem = dynamic_cast<Widget*>(Helper::seekWidgetByName(pageShow, StringUtils::format("panel_item_%d", shipSale.size()+indexequipSale + 1)));
			auto image_goods = panelItem->getChildByName<ImageView*>("image_goods");
			auto button_1 = panelItem->getChildByName<Button*>("button_1");
			button_1->setLocalZOrder(100);
			button_1->setTag(equipSale[indexequipSale]->itemid + ONSALE_EQUIP_TAG);
			button_1->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			image_goods->setVisible(true);
			image_goods->setTag(equipSale[indexequipSale]->itemid + ONSALE_EQUIP_TAG);
			image_goods->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			auto label_goods_name = panelItem->getChildByName<Text*>("label_goods_name");
			auto label_num = panelItem->getChildByName<Text*>("label_num");
			image_goods->loadTexture(getItemIconPath(equipSale[indexequipSale]->itemid, IMAGE_ICON_SHADE));
			label_goods_name->setString(getItemName(equipSale[indexequipSale]->itemid));
			label_num->setString(StringUtils::format("x %d", equipSale[indexequipSale]->amount));
		}
	}
	if (itemsale.size() != 0)
	{
		for (int indexitemsale = 0; indexitemsale < itemsale.size(); indexitemsale++)
		{
			auto panelItem = dynamic_cast<Widget*>(Helper::seekWidgetByName(pageShow, StringUtils::format("panel_item_%d", shipSale.size()+equipSale.size() + indexitemsale + 1)));
			auto image_goods = panelItem->getChildByName<ImageView*>("image_goods");
			auto button_1 = panelItem->getChildByName<Button*>("button_1");
			button_1->setLocalZOrder(100);
			button_1->setTag(itemsale[indexitemsale]->itemid + ONSALE_ITEM_TAG);
			button_1->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			image_goods->setVisible(true);
			image_goods->setTag(itemsale[indexitemsale]->itemid + ONSALE_ITEM_TAG);
			image_goods->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			auto label_goods_name = panelItem->getChildByName<Text*>("label_goods_name");
			auto label_num = panelItem->getChildByName<Text*>("label_num");
			image_goods->loadTexture(getItemIconPath(itemsale[indexitemsale]->itemid, IMAGE_ICON_SHADE));
			label_goods_name->setString(getItemName(itemsale[indexitemsale]->itemid));
			label_num->setString(StringUtils::format("x %d", itemsale[indexitemsale]->amount));
		}
	}
}
void UIPackageDeals::showLimitPageGoods()
{
	std::vector<OnsaleItemDefine*>limitShipSale;//存储船
	std::vector<OnsaleItemDefine*>limitEquipSale;//存储设备
	std::vector<OnsaleItemDefine*>limitItemsale;//存储物品
	//limitShipSale
	auto pageShow = myPageView->getPage(0);//限时的物品放在首页
	auto label_content_time_1 = pageShow->getChildByName<Text*>("label_content_time_1");
	auto label_content_1 = pageShow->getChildByName<Text*>("label_content_1");
	label_content_1->setVisible(false);
	label_content_time_1->setVisible(true);
	for (int j = 0; j < m_pOnsaleItemResults->timelevelgood->n_onsaleship; j++)
	{
		limitShipSale.push_back(m_pOnsaleItemResults->timelevelgood->onsaleship[j]);//存进的每条船
	}
	for (int k = 0; k < m_pOnsaleItemResults->timelevelgood->n_onsaleequip; k++)
	{
		limitEquipSale.push_back(m_pOnsaleItemResults->timelevelgood->onsaleequip[k]);//存进的每个装备
	}
	for (int m = 0; m < m_pOnsaleItemResults->timelevelgood->n_onsaleitem; m++)
	{
		limitItemsale.push_back(m_pOnsaleItemResults->timelevelgood->onsaleitem[m]);//存进的每个物品
	}
	
	if (limitShipSale.size() != 0)
	{
		
		for (int index = 0; index < limitShipSale.size(); index++)
		{
			auto panelItem = dynamic_cast<Widget*>(Helper::seekWidgetByName(pageShow, StringUtils::format("panel_item_%d", index + 1)));
			auto image_goods = panelItem->getChildByName<ImageView*>("image_goods");
			auto button_1 = panelItem->getChildByName<Button*>("button_1");
			button_1->setLocalZOrder(100);
			button_1->setTag(limitShipSale[index]->itemid + ONSALE_SHIP_TAG);
			button_1->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			image_goods->setVisible(true);
			image_goods->setTag(limitShipSale[index]->itemid + ONSALE_SHIP_TAG);
			image_goods->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			auto label_goods_name = panelItem->getChildByName<Text*>("label_goods_name");
			auto label_num = panelItem->getChildByName<Text*>("label_num");
			image_goods->loadTexture(getShipIconPath(limitShipSale[index]->itemid, IMAGE_ICON_SHADE));
			label_goods_name->setString(getShipName(limitShipSale[index]->itemid));
			label_num->setString(StringUtils::format("x %d", limitShipSale[index]->amount));
		}
	}
	if (limitEquipSale.size() != 0)
	{
		for (int indexEquip = 0; indexEquip < limitEquipSale.size(); indexEquip++)
		{
			auto panelItem = dynamic_cast<Widget*>(Helper::seekWidgetByName(pageShow, StringUtils::format("panel_item_%d", limitShipSale.size() + indexEquip + 1)));
			auto image_goods = panelItem->getChildByName<ImageView*>("image_goods");
			auto button_1 = panelItem->getChildByName<Button*>("button_1");
			button_1->setLocalZOrder(100);
			button_1->setTag(limitEquipSale[indexEquip]->itemid + ONSALE_EQUIP_TAG);
			button_1->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			image_goods->setVisible(true);
			image_goods->setTag(limitEquipSale[indexEquip]->itemid + ONSALE_EQUIP_TAG);
			image_goods->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			auto label_goods_name = panelItem->getChildByName<Text*>("label_goods_name");
			auto label_num = panelItem->getChildByName<Text*>("label_num");
			image_goods->loadTexture(getItemIconPath(limitEquipSale[indexEquip]->itemid, IMAGE_ICON_SHADE));
			label_goods_name->setString(getItemName(limitEquipSale[indexEquip]->itemid));
			label_num->setString(StringUtils::format("x %d", limitEquipSale[indexEquip]->amount));
		}
	}
	if (limitItemsale.size() != 0)
	{
		for (int indexItem = 0; indexItem < limitItemsale.size(); indexItem++)
		{
			auto panelItem = dynamic_cast<Widget*>(Helper::seekWidgetByName(pageShow, StringUtils::format("panel_item_%d", limitEquipSale.size()+limitShipSale.size()+indexItem+1)));
			auto image_goods = panelItem->getChildByName<ImageView*>("image_goods");
			auto button_1 = panelItem->getChildByName<Button*>("button_1");
			button_1->setLocalZOrder(100);
			button_1->setTag(limitItemsale[indexItem]->itemid + ONSALE_ITEM_TAG);
			button_1->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			image_goods->setVisible(true);		
			image_goods->setTag(limitItemsale[indexItem]->itemid + ONSALE_ITEM_TAG);	
			image_goods->addTouchEventListener(CC_CALLBACK_2(UIPackageDeals::imageOnsaleItem_callfunc,this));
			auto label_goods_name = panelItem->getChildByName<Text*>("label_goods_name");
			auto label_num = panelItem->getChildByName<Text*>("label_num");
			image_goods->loadTexture(getItemIconPath(limitItemsale[indexItem]->itemid, IMAGE_ICON_SHADE));
			label_goods_name->setString(getItemName(limitItemsale[indexItem]->itemid));
			label_num->setString(StringUtils::format("x %d", limitItemsale[indexItem]->amount));
		}
	}
}
void UIPackageDeals::showLimitTime(float f)
{
	auto pageShow = myPageView->getPage(0);//限时的物品放在首页
	auto label_content_time_2 = pageShow->getChildByName<Text*>("label_content_time_2");
	m_pOnsaleItemResults->remaintime--;

	int64_t Hours = (m_pOnsaleItemResults->remaintime) / 3600 % 24;
	int64_t Mins = (m_pOnsaleItemResults->remaintime / 60) % 60;
	int64_t Seconds = m_pOnsaleItemResults->remaintime % 60;
	std::string strMins = StringUtils::format("%02lld", Mins);
	std::string strSec = StringUtils::format("%02lld", Seconds);
	
	std::string  leftTime = StringUtils::format("0%lld", Hours) + ":" + strMins + ":" + strSec;
	label_content_time_2->setString(leftTime);
	if (!m_pOnsaleItemResults->remaintime)
	{
		if (isScheduled(schedule_selector(UIPackageDeals::showLimitTime)))
		{
			this->unschedule(schedule_selector(UIPackageDeals::showLimitTime));
		}
		label_content_time_2->setVisible(false);
		//fix 第一版只显示限时礼包
		//................begin..............
		closeView(GIFTPACK_RES[GIFTPACK_CSB]);
		ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		//...............end.................
		//ProtocolThread::GetInstance()->getOnsaleItems(LoadingLayer::create(this));
	}
}
void UIPackageDeals::showOnsaleCost()
{
	openView(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
	auto listview_cost = view->getChildByName<ListView*>("listview_cost");
	auto panel_cost = listview_cost->getItem(1);

	auto label_advanced_repair = view->getChildByName<Text*>("label_advanced_repair");
	label_advanced_repair->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_V_OR_COIN"]);

	int costV = 0;//花费
	auto pageNow = myPageView->getCurPageIndex();
	auto panel_ppClone = myPageView->getPage(pageNow)->getChildByName("panel_pp");
	std::string panelItem_goodName_1 = panel_ppClone->getChildByName<Widget*>("panel_item_1")->getChildByName<Text*>("label_goods_name")->getString();
	std::string panelItem_goodName_2 = panel_ppClone->getChildByName<Widget*>("panel_item_2")->getChildByName<Text*>("label_goods_name")->getString();
	std::string panelItem_goodName_3 = panel_ppClone->getChildByName<Widget*>("panel_item_3")->getChildByName<Text*>("label_goods_name")->getString();
	std::string content = panelItem_goodName_1 + "," + panelItem_goodName_2 + "," + panelItem_goodName_3;
	if (pageNow == 0)
	{
		if (m_pOnsaleItemResults->remaintime&&m_pOnsaleItemResults->timelevelgood)
		{
			costV = m_pOnsaleItemResults->timelevelgood->vticket;
		}
		else
		{
			costV = m_pOnsaleItemResults->goods[0]->vticket;
		}
	}
	else
	{
		if (m_pOnsaleItemResults->remaintime&&m_pOnsaleItemResults->timelevelgood)
		{
			costV = m_pOnsaleItemResults->goods[pageNow - 1]->vticket;
		}
		else
		{
			costV = m_pOnsaleItemResults->goods[pageNow]->vticket;
		}
	}
	
	auto label_repair_content = view->getChildByName<Text*>("label_repair_content");
	label_repair_content->setString(content);
	auto label_cost_num = panel_cost->getChildByName<Text*>("label_cost_num");
	label_cost_num->setString(StringUtils::format("%d", costV));
}
void UIPackageDeals::updateMaincityCoin()
{
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	if (mainlayer)
	{
		mainlayer->flushCionAndGold(m_pBuyItemResults->coins, m_pBuyItemResults->golds);
	}
	
}
void UIPackageDeals::imageOnsaleItem_callfunc(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	auto saleGood = dynamic_cast<Widget*>(pSender);
	std::string name = saleGood->getName();
	int onSaleItemTag = saleGood->getTag();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	UICommon::getInstance()->openCommonView(this);
	if (onSaleItemTag<ONSALE_EQUIP_TAG)
	{
		UICommon::getInstance()->flushShipDetail(nullptr, onSaleItemTag - ONSALE_SHIP_TAG, true);
	}
	else if (onSaleItemTag<ONSALE_ITEM_TAG)
	{
		UICommon::getInstance()->flushEquipsDetail(nullptr, onSaleItemTag - ONSALE_EQUIP_TAG, true);
	}
	else
	{
		UICommon::getInstance()->flushItemsDetail(nullptr, onSaleItemTag - ONSALE_ITEM_TAG, true);
	}
}
