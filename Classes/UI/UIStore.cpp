#include "UIStore.h"
#include "env.h"
#include "Utils.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UIVoyageSlider.h"

#include "UIMain.h"
#include "UIBank.h"
#include "UIExchange.h"
#include "UITavern.h"
#include "UIShipyard.h"
#include "UIPalace.h"
#include "UIPort.h"
#include "TVSceneLoader.h"
#include "UISailManage.h"
#include "UISailHUD.h"

#if ANDROID
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif

UIStore*UIStore::m_ticketStoreLayer = nullptr;

UIStore* UIStore::m_pInstance=0;
UIStore* UIStore::GetInstance(){
	return m_pInstance;
}

UIStore::UIStore() :
m_nBtngoodTag(0),
m_nRandomGoodsBtnTag(0),
m_nRandomGoodsType(0),
m_nRandomShipNum(0),
m_nRandomEquipNum(0),
m_nRandomItemNum(0)
{
	m_pInstance = this;
    m_nPackagesize = -1;
    m_nMaxpropNums = -1;
    m_nBuyPropNum = -1;
    m_pSlider = nullptr;
    m_pLabelNum = nullptr;
    m_pGetItemResult = nullptr;
    m_bPrimarySerIsCooling = false;
    m_bIsRandomGoods = false;
	m_pButton = nullptr;
	m_pMarketActivitiesResult = nullptr;
	m_nConfirmIndex = 0;
	//对话走字
	lenNum = 0;

	chatContent = "";
	anchPic = nullptr;
	chatTxt = nullptr;
	m_bDiaolgOver = false;
	m_PTimeLabel = nullptr;
	m_pButtonTag = -1;
	m_vThingBlack.clear();
	m_bTouchClassification = false;
	m_vRandomRefreshItems.clear();
	m_nBlackMarketSortTag = 0;
	m_button_Priescort = nullptr;
	m_tableview_tag = TABLEVIEW_NONE;
	m_nBlackStoreChange = -1;
}

UIStore::~UIStore()
{
    m_ticketStoreLayer = nullptr;
    m_pSlider = nullptr;
    m_pLabelNum = nullptr;
    m_pGetItemResult = nullptr;
	m_vThingBlack.clear();
    //--zhengqiang
    for (std::vector<BlackMarketRandomShipAndEquipDefine*>::iterator it = m_vRandomItems.begin(); it != m_vRandomItems.end(); ++it)
    {
        if (*it != nullptr)
        {
            delete *it;
            *it = nullptr;
        }
    }
    m_vRandomItems.clear();
    //--
}

void UIStore::onEnter()
{
    UIBasicLayer::onEnter();
    ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIStore::onServerEvent, this), this);
}

void UIStore::onExit()
{
    ProtocolThread::GetInstance()->unregisterMessageCallback(this);
    UIBasicLayer::onExit();

}

void UIStore::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
    if (TouchType != Widget::TouchEventType::ENDED)
    {
        return;
    }
    auto buttongood = dynamic_cast<Widget*>(pSender);
    std::string name = buttongood->getName();

    if (isButton(button_v_ticket))//V票
    {
        SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nBlackMarketSortTag = 0;
		m_bTouchClassification = false;
		ProtocolThread::GetInstance()->getVTicketMarketItems(VITICKET_TYPE_V_TICKET, UILoadingIndicator::create(this));	
		auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
		auto panel_content_no = paneStore->getChildByName("panel_centent_no");
		panel_content_no->setVisible(false);
		removeAllTableViews();
        return;
    }
     if (isButton(button_v_coin))//银币
    {
        SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nBlackMarketSortTag = 0;
		m_bTouchClassification = false;
		ProtocolThread::GetInstance()->getVTicketMarketItems(VITICKET_TYPE_COIN, UILoadingIndicator::create(this));
		auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
		auto panel_content_no = paneStore->getChildByName("panel_centent_no");
		panel_content_no->setVisible(false);
		removeAllTableViews();
        return;
    }
    if (isButton(button_v_black))//黑市
    {
        SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nBlackMarketSortTag = 0;
		m_bTouchClassification = false;
		ProtocolThread::GetInstance()->getVTicketMarketItems(VITICKET_TYPE_V_PROP, UILoadingIndicator::create(this));
		auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
		auto panel_content_no = paneStore->getChildByName("panel_centent_no");
		panel_content_no->setVisible(false);
        return;
    }
	//水手币
	if (isButton(button_v_sailor))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nBlackMarketSortTag = 0;
		m_bTouchClassification = false;
		ProtocolThread::GetInstance()->getVTicketMarketActivities(VITICKET_TYPE_SAILOR_HOUSE, UILoadingIndicator::create(this));
		auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
		auto panel_content_no = paneStore->getChildByName("panel_centent_no");
		panel_content_no->setVisible(false);
		removeAllTableViews();
		return;
	}
	//宝箱
	if (isButton(button_v_lucky))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

		m_nBlackMarketSortTag = 0;
		m_bTouchClassification = false;
		ProtocolThread::GetInstance()->getVTicketMarketActivities(VITICKET_TYPE_LUCKY_CHEST, UILoadingIndicator::create(this));
		auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
		auto panel_content_no = paneStore->getChildByName("panel_centent_no");
		panel_content_no->setVisible(false);
		removeAllTableViews();
		return;
	}
	if (isButton(button_close_1))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
		return;
	}
    if (isButton(button_close))//商城的关闭按钮
    {
        SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);   
		//关掉定时器
		if (isScheduled(schedule_selector(UIStore::vticketTimeSecondFresh)))
		{
		this->unschedule(schedule_selector(UIStore::vticketTimeSecondFresh));
		}
		closeView(STORE_RES[VIEW_STORE_MAIN_PANEL]);

		if (this->getChildByTag(INFORMVIEWTAG))
		{
			auto view = this->getChildByTag(INFORMVIEWTAG);
			dynamic_cast<UIInform*>(view)->closeAutoInform(0);
		}
		this->removeFromParentAndCleanup(true);
        ProtocolThread::GetInstance()->unregisterMessageCallback(this);
		m_pButton = nullptr;
		m_nBlackMarketSortTag = 0;
		m_button_Priescort = nullptr;
		m_tableview_tag = TABLEVIEW_NONE;
		m_nBlackStoreChange = -1;
		SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_uicommon0.plist");
		SpriteFrameCache::getInstance()->removeSpriteFrameByName("eff_plist/fx_uicommon1.plist");
        return;
    }
    if (isButton(button_escort_close))//护送服务信息解释的关闭按钮
    {
        SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
        closeView();
        return;
    }
    if (isButton(button_v_yes))//结算框的确定购买按钮
    {
        SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_GOLD_16);
		closeView();
	
		if (m_pButton->getTag() == VITICKET_TYPE_COIN)
        {
			ProtocolThread::GetInstance()->buyVTicketMarketItem(m_pGetItemResult->type, m_pGetItemResult->items[m_nBtngoodTag]->uniqueid, 1, UILoadingIndicator::create(this));
        }
		else if (m_pButton->getTag() == VITICKET_TYPE_V_PROP)
        {
            if (m_bIsRandomGoods)//黑市里的随机物品
            {
                ProtocolThread::GetInstance()->buyVTicketMarketItem(VITICKET_TYPE_V_PROP, m_vRandomItems[m_nRandomGoodsBtnTag]->uniqueid, m_nBuyPropNum, UILoadingIndicator::create(this));
                m_bIsRandomGoods = false;
            }
            else//黑市里的固定物品
            {
                ProtocolThread::GetInstance()->buyVTicketMarketItem(m_pGetItemResult->type, m_pGetItemResult->items[m_nBtngoodTag]->uniqueid, m_nBuyPropNum, UILoadingIndicator::create(this));
            }
		}
		else
		{
			int item_uniqueid = 0;
			if (m_nBtngoodTag < m_pMarketActivitiesResult->n_items)
			{
				item_uniqueid = m_pMarketActivitiesResult->items[m_nBtngoodTag]->uniqueid;
			}
			else
			{
				item_uniqueid = m_pMarketActivitiesResult->randomitems[m_nBtngoodTag - m_pMarketActivitiesResult->n_items]->uniqueid;
			}
			ProtocolThread::GetInstance()->buyVTicketMarketActivities(VITICKET_TYPE_SAILOR_HOUSE, item_uniqueid, m_nBuyPropNum, UILoadingIndicator::create(this));
		}
        return;
    }
    if (isButton(button_v_no))//结算框取消购买按钮
    {
        SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		closeView();
        m_bIsRandomGoods = false;
        return;
    }
    //slider
    if (isButton(button_plus))//购买道具选择数量界面的加号
    {
        SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
        if (m_nBuyPropNum >= m_nMaxpropNums)
        {
            return;
        }
        m_nBuyPropNum++;
        m_pSlider->setPercent(m_nBuyPropNum*100.0 / m_nMaxpropNums);
        m_pLabelNum->setString(StringUtils::format("x %d", m_nBuyPropNum));
		changeCost();
        return;
    }
    if (isButton(button_minus))//购买道具选择数量界面的减号
    {
        SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
        if (m_nBuyPropNum <= 1)
        {
            return;
        }
        m_nBuyPropNum--;
        m_pSlider->setPercent(m_nBuyPropNum*100.0 / m_nMaxpropNums);
        m_pLabelNum->setString(StringUtils::format("x %d", m_nBuyPropNum));
		changeCost();
        return;
    }

	//水手之家--上面问号解释
	if (isButton(button_bank_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_STORE_SAILOR_HOUSE_TITLE", "TIP_STORE_SAILOR_HOUSE_CONTENT");
		return;
	}
	//幸运宝箱--打开宝箱
	if (isButton(button_open))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//ProtocolThread::GetInstance()->buyVTicketMarketActivities(VITICKET_TYPE_LUCKY_CHEST, -1, -1, LoadingLayer::create(this));
		return;
	}
	//水手之家--物品详情
	if (isButton(panel_1) || isButton(Button_1))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		openGoodInfo(nullptr, buttongood->getParent()->getTag(), buttongood->getTag());
		return;
	}
	//护送Panel
	if (isButton(Panel_faildialog_root))
	{
		if (m_bDiaolgOver)
		{		
			m_bDiaolgOver = false;
			auto currentScene = Director::getInstance()->getRunningScene();
			auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
			auto cityLayer = mainlayer->getChildren().at(0);//Panel_12
			cityLayer->runAction(Sequence::create(CallFunc::create(this, callfunc_selector(UIStore::comebackHome)), DelayTime::create(1.0),
			CallFunc::create(this, callfunc_selector(UIStore::comebackHomeEcsort)), nullptr));
		}
		else
		{
			if (chatTxt)
			{
				chatTxt->setString(chatContent);
				m_bDiaolgOver = true;	
			}	
			if (isScheduled(schedule_selector(UIStore::escortContentDialog)))
			{
				this->unschedule(schedule_selector(UIStore::escortContentDialog));
			}	
			chatTxt = nullptr;
			anchPic->setVisible(true);
		}
		return;
	}
	//买东西花V票不足时跳转到V票页面
	if (isButton(button_confirm_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nBlackMarketSortTag = 0;
		removeAllTableViews();
		ProtocolThread::GetInstance()->getVTicketMarketItems(VITICKET_TYPE_V_TICKET, UILoadingIndicator::create(this));
		return;
	}
	if (isButton(button_ship_parts))
	{
		if (m_button_Priescort)
		{
			m_button_Priescort = nullptr;
		}
		openView(STORE_RES[VIEW_STORE_CATEGORY_CSB]);
		auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_CATEGORY_CSB]);
		auto panel_category = paneStore->getChildByName<Layout*>("panel_category");
		auto listview_1 = panel_category->getChildByName<ListView*>("listview_1");
		//所有
		auto button_option_1 = dynamic_cast<Button*>(Helper::seekWidgetByName(paneStore, "button_option_1"));
		button_option_1->setTag(0);
		//特殊
		Button* button_clone_6 = dynamic_cast<Button*>(button_option_1->clone());
		listview_1->pushBackCustomItem(button_clone_6);
		std::string button_clone6 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SPECIAL"];
		button_clone_6->setTitleText(button_clone6);
		button_clone_6->setTag(1);
		//材料
		Button* button_clone = dynamic_cast<Button*>(button_option_1->clone());
		listview_1->pushBackCustomItem(button_clone);
		std::string button_clone1 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_MATERIAL"];
		button_clone->setTitleText(button_clone1);
		button_clone->setTag(2);
		//消耗品
		Button* button_clone_1 = dynamic_cast<Button*>(button_option_1->clone());
		listview_1->pushBackCustomItem(button_clone_1);
		std::string button_clone2 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_CONSUMABLES"];
		button_clone_1->setTitleText(button_clone2);
		button_clone_1->setTag(3);
		//卷轴
		Button* button_clone_2 = dynamic_cast<Button*>(button_option_1->clone());
		listview_1->pushBackCustomItem(button_clone_2);
		std::string button_clone3 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SCOLLS"];
		button_clone_2->setTitleText(button_clone3);
		button_clone_2->setTag(4);
		//船只
		Button* button_clone_3 = dynamic_cast<Button*>(button_option_1->clone());
		listview_1->pushBackCustomItem(button_clone_3);
		std::string button_clone4 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SHIPS"];
		button_clone_3->setTitleText(button_clone4);
		button_clone_3->setTag(5);
		//船装备
		Button* button_clone_4 = dynamic_cast<Button*>(button_option_1->clone());
		listview_1->pushBackCustomItem(button_clone_4);
		std::string button_clone5 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SHIPS_PARTS"];
		button_clone_4->setTitleText(button_clone5);
		button_clone_4->setTag(6);

		//强化方案
		Button* button_clone_7 = dynamic_cast<Button*>(button_option_1->clone());
		listview_1->pushBackCustomItem(button_clone_7);
		std::string button_clone7 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_ENHANCE"];
		button_clone_7->setTitleText(button_clone7);
		button_clone_7->setTag(7);

		//船图纸
		Button* button_clone_8 = dynamic_cast<Button*>(button_option_1->clone());
		listview_1->pushBackCustomItem(button_clone_8);
		std::string button_clone8 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SHIP_BLUEPTINTS"];
		button_clone_8->setTitleText(button_clone8);
		button_clone_8->setTag(8);

		//装备图纸
		Button* button_clone_9 = dynamic_cast<Button*>(button_option_1->clone());
		listview_1->pushBackCustomItem(button_clone_9);
		std::string button_clone9 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_EQUIP_BLUEPTINTS"];
		button_clone_9->setTitleText(button_clone9);
		button_clone_9->setTag(9);

		listview_1->refreshView();
		auto image_pulldown = paneStore->getChildByName<ImageView*>("image_pulldown");
		auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
		button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
		addListViewBar(listview_1, image_pulldown);
		return;
	} 
	if (isButton(button_option_1))
	{
	auto sort = buttongood->getTag();
	closeView(STORE_RES[VIEW_STORE_CATEGORY_CSB]);
	auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto t_title = dynamic_cast<Text*>(Helper::seekWidgetByName(paneStore, "label_title"));
	auto button_ship_parts = dynamic_cast<Button*>(Helper::seekWidgetByName(paneStore, "button_ship_parts"));
	button_ship_parts->setContentSize(Size(140, 40));
	button_ship_parts->setPositionX(t_title->getPositionX() + t_title->getContentSize().width + button_ship_parts->getContentSize().width / 2 + 40);
	if (sort == 0)
	{
		updateStoreView(m_pGetItemResult);
		m_bTouchClassification = false;
		auto panel_content_no = paneStore->getChildByName("panel_centent_no");
		panel_content_no->setVisible(false);
	}
	else
	{
		if (sort == 1)
		{
			std::string button_clone7 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SPECIAL"];
			button_ship_parts->setTitleText(button_clone7);
		}
		else if (sort == 2)
		{
			std::string button_clone1 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_MATERIAL"];
			button_ship_parts->setTitleText(button_clone1);
		}
		else if (sort == 3)
		{
			std::string button_clone2 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_CONSUMABLES"];
			button_ship_parts->setTitleText(button_clone2.c_str());
		}
		else if (sort == 4)
		{
			std::string button_clone3 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SCOLLS"];
			button_ship_parts->setTitleText(button_clone3);
		}
		else if (sort == 5)
		{
			std::string button_clone4 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SHIPS"];
			button_ship_parts->setTitleText(button_clone4);
		}
		else if (sort == 6)
		{
			std::string button_clone5 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SHIPS_PARTS"];
			button_ship_parts->setTitleText(button_clone5);
		}
		else if (sort == 7)
		{
			std::string button_clone7 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_ENHANCE"];
			button_ship_parts->setTitleText(button_clone7);
		}
		else if (sort == 8)
		{
			std::string button_clone8 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SHIP_BLUEPTINTS"];
			button_ship_parts->setTitleText(button_clone8);
			button_ship_parts->setContentSize(Size(280, 40));
			button_ship_parts->setPositionX(t_title->getPositionX() + t_title->getContentSize().width + button_ship_parts->getContentSize().width / 2 + 40);
		}
		else if (sort == 9)
		{
			std::string button_clone9 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_EQUIP_BLUEPTINTS"];
			button_ship_parts->setTitleText(button_clone9);
			button_ship_parts->setContentSize(Size(280, 40));
			button_ship_parts->setPositionX(t_title->getPositionX() + t_title->getContentSize().width + button_ship_parts->getContentSize().width / 2 + 40);
		}
		m_nBlackMarketSortTag = sort;
		RefreshItems(sort);
		m_bTouchClassification = true;
	}
	
	m_nBlackMarketSortTag = sort;
	auto panel_content_1 = paneStore->getChildByName("panel_content_1");
	auto listview_sale = panel_content_1->getChildByName<ListView*>("listview_sale");
	auto image_pulldown = panel_content_1->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(listview_sale, image_pulldown);
	return;

	}
}
void UIStore::goodItem_callfunc(Ref *pSender, Widget::TouchEventType TouchType)
{
    if (TouchType != Widget::TouchEventType::ENDED)
    {
        return;
    }
    auto saleGood = dynamic_cast<Widget*>(pSender);
    std::string name = saleGood->getName();
    m_nBtngoodTag = saleGood->getTag();
    if (isButton(button_buy_price))
    {
        SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		//购买V票付费（直接跳转到付费界面）
        if (m_pGetItemResult->type == VITICKET_TYPE_V_TICKET)
        {
#if ANDROID
			if (m_pGetItemResult->items[m_nBtngoodTag]->iabstring) 
			{
				JniMethodInfo jniInfo;
				bool getObbPathSucc = false;
				bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo, "com/piigames/voyage/AppActivity", "BuyIABItem", "(Ljava/lang/String;Ljava/lang/String;)I");
				//log("BuyIABItem  id:%d", id);
				if (id)
				{
					std::string uuid= m_pGetItemResult->payload;//
					std::ostringstream stmCid ;
					stmCid << time(nullptr) ;
					uuid = uuid + stmCid.str();
					Utils::onChargeRequest(uuid, m_pGetItemResult->items[m_nBtngoodTag]->iabstring, m_pGetItemResult->items[m_nBtngoodTag]->price/100.0, m_pGetItemResult->items[m_nBtngoodTag]->count);
					jstring itemName = jniInfo.env->NewStringUTF(m_pGetItemResult->items[m_nBtngoodTag]->iabstring);
					jstring payload = jniInfo.env->NewStringUTF(m_pGetItemResult->payload);
					int res = jniInfo.env->CallStaticIntMethod(jniInfo.classID, jniInfo.methodID, itemName, payload);
					jniInfo.env->DeleteLocalRef(itemName);
					jniInfo.env->DeleteLocalRef(payload);
				}
			}
			else 
			{
				ProtocolThread::GetInstance()->buyVTicketMarketItem(m_pGetItemResult->type, m_pGetItemResult->items[m_nBtngoodTag]->uniqueid, 0, UILoadingIndicator::createWithMask(this, 4));
			}
#else
			if (m_pGetItemResult->items[m_nBtngoodTag]->iabstring)
			{
				ProtocolThread::GetInstance()->getIABItem(m_pGetItemResult->items[m_nBtngoodTag]->iabstring, UILoadingIndicator::createWithMask(this, 4));
			}
			else
			{
				//类型，数据库里的id，数量
				ProtocolThread::GetInstance()->buyVTicketMarketItem(m_pGetItemResult->type, m_pGetItemResult->items[m_nBtngoodTag]->uniqueid, 0, UILoadingIndicator::createWithMask(this, 4));
			}
#endif
        }
        else
        {
			//银币
            if (m_pGetItemResult->type == VITICKET_TYPE_COIN)
            {
                if (m_pGetItemResult->golds < (m_pGetItemResult->items[m_nBtngoodTag]->price) / 100.0)
                {

					UIInform::getInstance()->openInformView(this);
					UIInform::getInstance()->openGoldNotEnoughYesOrNo(m_pGetItemResult->items[m_nBtngoodTag]->price / 100);
                    return;
                }
                else
                {
                    showYoucost(m_pGetItemResult, m_nBtngoodTag);
                }
            }
			//黑市
			else if (m_pGetItemResult->type == VITICKET_TYPE_V_PROP)
            {
	
				int couldFillCounts = 0;
				if (SINGLE_SHOP->getItemData()[m_pGetItemResult->items[m_nBtngoodTag]->itemid].weight>0)
				{
					couldFillCounts = m_nPackagesize / ((SINGLE_SHOP->getItemData()[m_pGetItemResult->items[m_nBtngoodTag]->itemid].weight * m_pGetItemResult->items[m_nBtngoodTag]->count) / 100);
				}
				else
				{
					couldFillCounts = m_nPackagesize;
				}
                int couldBuyCounts = m_pGetItemResult->golds/((m_pGetItemResult->items[m_nBtngoodTag]->price) / 100.0);
                int counts = couldBuyCounts < couldFillCounts ? couldBuyCounts : couldFillCounts;
				m_nMaxpropNums = counts < ONCE_BUYITEM_MAX_NUM ? counts : ONCE_BUYITEM_MAX_NUM;
				//高级护送Id
                if (m_pGetItemResult->items[m_nBtngoodTag]->itemid == VITICKET_ESCORT_SENIOR_ID)
                {
                    if (SINGLE_HERO->m_iBornCity == SINGLE_HERO->m_iCityID)
                    {
                        UIInform::getInstance()->openInformView(this);
                        UIInform::getInstance()->openViewAutoClose("TIP_ESCORT_HAVE_BEEN_INCITY");//已在主城
                    }
                    else
                    {
						if (m_pGetItemResult->golds<m_pGetItemResult->items[m_nBtngoodTag]->price/100.0)
						{
							UIInform::getInstance()->openInformView(this);
							UIInform::getInstance()->openGoldNotEnoughYesOrNo(m_pGetItemResult->items[m_nBtngoodTag]->price / 100);

						}
						else
						{
							m_nMaxpropNums = 1;
							showYoucost(m_pGetItemResult, m_nBtngoodTag);
						}                  
                    }
                }
				//初级护送Id
                else if (m_pGetItemResult->items[m_nBtngoodTag]->itemid == VITICKET_ESCORT_PRIMARY_ID)
                {
                    if (SINGLE_HERO->m_iBornCity == SINGLE_HERO->m_iCityID)
                    {
                        UIInform::getInstance()->openInformView(this);
                        UIInform::getInstance()->openViewAutoClose("TIP_ESCORT_HAVE_BEEN_INCITY");
                    }
                    else
                    {
                        if (m_bPrimarySerIsCooling)
                        {
                            UIInform::getInstance()->openInformView(this);
                            UIInform::getInstance()->openViewAutoClose("TIP_PRIMARY_ESCORT_IS_COOLING");
                        }
                        else
                        {
							if (SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
                            {
                                UIInform::getInstance()->openInformView(this);
                                UIInform::getInstance()->openViewAutoClose("TIP_VILLAGE_PRIMARY_ESCORT_CANNOT_USE");
                            }
                            else
                            {
								if (m_pGetItemResult->coins< m_pGetItemResult->items[m_nBtngoodTag]->price/100.0)
								{
									openSuccessOrFailDialog("TIP_COIN_NOT");
								}
								else
								{
									m_nMaxpropNums = 1;
									showYoucost(m_pGetItemResult, m_nBtngoodTag);
								}                     
                            }

                        }
                    }
				}
				else if (m_pGetItemResult->items[m_nBtngoodTag]->itemid == VITICKET_LOW_DROPS_ID || m_pGetItemResult->items[m_nBtngoodTag]->itemid == VITICKET_SENIOR_DROPS_ID)
				{
					if (couldBuyCounts <= 0)
					{
						UIInform::getInstance()->openInformView(this);
						UIInform::getInstance()->openGoldNotEnoughYesOrNo((int)m_pGetItemResult->items[m_nBtngoodTag]->price / 100);
					}
					else
					{
						ProtocolThread::GetInstance()->getDropIncreaseRateInfo(UILoadingIndicator::create(this));
					}
				}
                else
                {
                    if (m_nMaxpropNums < 1)
                    {
						if (couldFillCounts<=0)
						{
							openSuccessOrFailDialog("TIP_VTICKETSHOP_BAG_FULL");
						}
						else if (couldBuyCounts<=0)
						{
							/*  std::string info = SINGLE_SHOP->getTipsInfo()["TIP_GOLD_NOT_CONTENT"];
							  std::string old_value = "[num]";
							  std::string new_value = StringUtils::format("%d", (int)m_pGetItemResult->items[m_nBtngoodTag]->price / 100);
							  repalce_all_ditinct(info, old_value, new_value);
							  openSuccessOrFailDialogWithString(info);*/
							UIInform::getInstance()->openInformView(this);
							UIInform::getInstance()->openGoldNotEnoughYesOrNo((int)m_pGetItemResult->items[m_nBtngoodTag]->price / 100);

						}           
					}
					else
					{
						initPropslider(m_nMaxpropNums);
					}
                }
            }
        }
        return;
    }
}

void UIStore::sailorHouse_callfunc(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (TouchType != Widget::TouchEventType::ENDED)
	{
		return;
	}
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	auto saleGood = dynamic_cast<Widget*>(pSender);
	std::string name = saleGood->getName();
	m_nBtngoodTag = saleGood->getTag();
	int item_id = 0;
	int item_price = 0;
	int item_type = ITEM_TYPE_PORP;
	if (m_nBtngoodTag < m_pMarketActivitiesResult->n_items)
	{
		item_id = m_pMarketActivitiesResult->items[m_nBtngoodTag]->itemid;
		item_price = m_pMarketActivitiesResult->items[m_nBtngoodTag]->price;
	}
	else
	{
		item_id = m_pMarketActivitiesResult->randomitems[m_nBtngoodTag - m_pMarketActivitiesResult->n_items]->itemid;
		item_price = m_pMarketActivitiesResult->randomitems[m_nBtngoodTag - m_pMarketActivitiesResult->n_items]->price;
		item_type = m_pMarketActivitiesResult->randomitems[m_nBtngoodTag - m_pMarketActivitiesResult->n_items]->itemtype;
	}

	int couldFillCounts = 10;
	if (item_type != ITEM_TYPE_SHIP)
	{
		if (SINGLE_SHOP->getItemData()[item_id].weight>0)
		{
			couldFillCounts = m_nPackagesize / (SINGLE_SHOP->getItemData()[item_id].weight / 100);
		}
		else
		{
			couldFillCounts = m_nPackagesize;
		}
	}
	int couldBuyCounts = m_pMarketActivitiesResult->sailorcoins / item_price;
	int counts = couldBuyCounts < couldFillCounts ? couldBuyCounts : couldFillCounts;
	m_nMaxpropNums = counts < ONCE_BUYITEM_MAX_NUM ? counts : ONCE_BUYITEM_MAX_NUM;
	if (m_nMaxpropNums < 1)
	{
		if (couldFillCounts <= 0)
		{
			openSuccessOrFailDialog("TIP_VTICKETSHOP_BAG_FULL");
		}
		else if (couldBuyCounts <= 0)
		{
			openSuccessOrFailDialog("TIP_SAILOR_COIN_NOT");
		}
	}
	else
	{
		initPropslider(m_nMaxpropNums);
	}
}

void UIStore::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
    UIBasicLayer::onServerEvent(message, msgType);
    switch (msgType)
    {
    case PROTO_TYPE_GetIABItemResult:
    {
    	GetIABItemResult*result = (GetIABItemResult*)message;
    	if(result->failed == 0){
    		updateGold(result->gold);
    		ProtocolThread::GetInstance()->getVTicketMarketItems(VITICKET_TYPE_V_TICKET,UILoadingIndicator::create(this));
    	}
		break;
    }
    case PROTO_TYPE_GetVTicketMarketItemsResult:
    {
        GetVTicketMarketItemsResult*result = (GetVTicketMarketItemsResult*)message;
		if (result->failed == 0)
		{
			m_pGetItemResult = result;
			buttonBright(result->type);
			m_nPackagesize = (result->maxpackagesize - result->currentpackagesize) / 100.0;
			if (result->deliverservicecd)
			{
				m_bPrimarySerIsCooling = true;
			}
			else
			{
				m_bPrimarySerIsCooling = false;
			}
			//保存随机物品
			if (result->type == VITICKET_TYPE_V_PROP)
			{
				std::vector<BlackMarketRandomShipAndEquipDefine*> randomItems;

				for (int i = 0; i < result->n_randomship; ++i)
				{
					randomItems.push_back(result->randomship[i]);
				}
				for (int i = 0; i < result->n_randomequip; ++i)
				{
					randomItems.push_back(result->randomequip[i]);
				}
				for (int i = 0; i < result->n_randomitem; ++i)
				{
					randomItems.push_back(result->randomitem[i]);
				}
				m_vRandomItems.clear();
				m_vRandomItems = randomItems;
				m_nRandomShipNum = result->n_randomship;
				m_nRandomEquipNum = result->n_randomequip;
				m_nRandomItemNum = result->n_randomitem;
			}
			if (m_nBlackMarketSortTag)
			{
				RefreshItems(m_nBlackMarketSortTag);
			}
			else
			{
				updateStoreView(result);
			}
		}
        else
        {
            openSuccessOrFailDialog("TIP_VTICKET_GETGOODINFO_FAIL");
        }
        break;
    }
    case PROTO_TYPE_BuyVTicketMarketItemResult:
    {
        BuyVTicketMarketItemResult*result = (BuyVTicketMarketItemResult*)message;
        if (result->failed == 0)
        {
            updateCoinGold(result);
            //TODO: 消费追踪,如何判断type?

            //TODO: 重构
            if (result->type == VITICKET_TYPE_V_PROP && (result->itemid == VITICKET_ESCORT_SENIOR_ID || result->itemid == VITICKET_ESCORT_PRIMARY_ID))
            {
                SINGLE_HERO->m_escortCitySucces = true;
                auto currentScene = Director::getInstance()->getRunningScene();
                auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
				if (SINGLE_HERO->m_iCityID == 3)
				{
					mainlayer->closeRainEffect();
				}
                if (mainlayer)
                {
					//关掉定时器
					if (isScheduled(schedule_selector(UIStore::vticketTimeSecondFresh)))
					{
						this->unschedule(schedule_selector(UIStore::vticketTimeSecondFresh));
					}
                    auto cityLayer = mainlayer->getChildren().at(0);
					cityLayer->setVisible(false);

					//护送期间改成海上的声音
					m_eUIType = UI_MAP;
					playAudio();
					escortDialog(result->itemid);
                }
				if (result->itemid == VITICKET_ESCORT_SENIOR_ID)
				{
					Utils::consumeVTicket("2", result->count, result->price);
				}
            }
			else if (result->type != VITICKET_TYPE_V_TICKET)
			{
				if (m_pGetItemResult->items[m_nBtngoodTag]->itemid == VITICKET_LOW_DROPS_ID || m_pGetItemResult->items[m_nBtngoodTag]->itemid == VITICKET_SENIOR_DROPS_ID)
				{
					auto currentScene = Director::getInstance()->getRunningScene();
					auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
					if (mainlayer)
					{
						mainlayer->updataDorpserviceTime(result->drop_rate_increase_ramian_time, result->drop_rate_increase_type);
					}
				}
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openViewAutoClose("TIP_SAILOR_HOURE_BUG_ITME_SUCCESS");
			    ProtocolThread::GetInstance()->getVTicketMarketItems(result->type);
				//统计花费v票
				if (result->type == VITICKET_TYPE_COIN)
				{
					Utils::consumeVTicket("1", result->count, result->price);
				}
				else if (result->type == VITICKET_TYPE_V_PROP)
				{
					Utils::consumeVTicket("2", result->count, result->price);
				}				
			}else
            {
                ProtocolThread::GetInstance()->getVTicketMarketItems(result->type);
            }
        }
        else if (result->failed == COIN_NOT_FAIL)
        {
            UIInform::getInstance()->openInformView(this);
            UIInform::getInstance()->openConfirmYes("TIP_COIN_NOT");
        }
        else if (result->failed == GOLD_NOT_FAIL)
        {
            UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openGoldNotEnoughYesOrNo(result->price);
        }
        else
        {
            //openSuccessOrFailDialog("TIP_VTICKET_BUYGOOD_FAIL");
			//BuyVTicketMarketItemResult* widget = static_cast<GetVTicketMarketItemsResult*>(result);
			//updateStoreView(result);
			updateCoinGold(result);
			buttonBright(result->type);
        }
        break;
    }
    case PROTO_TYPE_GetLeaderboardResult:
    {
        GetLeaderboardResult*result = (GetLeaderboardResult*)message;
        if (result->failed == 0)
        {
            // 					CommonView::getInstance()->openCommonView(this);
            // 					CommonView::getInstance()->openView(CommonView::C_VIEW_RARK);
            // 					CommonView::getInstance()->flushRank(result);			
        }
        break;
    }
	case PROTO_TYPE_GetVTicketMarketActivitiesResult:
	{
		GetVTicketMarketActivitiesResult *result = (GetVTicketMarketActivitiesResult*)message;
		if (result->failed == 0)
		{
			m_pMarketActivitiesResult = result;
			/*if (isScheduled(schedule_selector(VTicketStoreLayer::primaryEscortTime)))
			{
				this->unschedule(schedule_selector(VTicketStoreLayer::primaryEscortTime));
			}*/
			if (result->type == VITICKET_TYPE_SAILOR_HOUSE)
			{
				buttonBright(VITICKET_TYPE_SAILOR_HOUSE);
				updateSailorHouse();
			}
			else
			{
				buttonBright(VITICKET_TYPE_LUCKY_CHEST);
				updateLuckyChest();
			}
		}
		break;
	}
	case PROTO_TYPE_BuyVTicketMarketActivitiesResult:
	{
		BuyVTicketMarketActivitiesResult *result = (BuyVTicketMarketActivitiesResult*)message;
		if (result->failed == 0)
		{
			UIInform::getInstance()->openInformView(this);
			if (result->type == VITICKET_TYPE_SAILOR_HOUSE)
			{
				m_pMarketActivitiesResult->sailorcoins = result->sailorcoins;
				if (result->itemid == 62 && result->count > 0)
				 {
					auto currentScene = _director->getRunningScene();
					auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
					mainlayer->flushSudanMaskNum(result->count);
				 }
				
				buttonBright(VITICKET_TYPE_SAILOR_HOUSE);
				UIInform::getInstance()->openViewAutoClose("TIP_SAILOR_HOURE_BUG_ITME_SUCCESS");
			}
			else
			{
				m_pMarketActivitiesResult->treasure = result->itemid;
				m_pMarketActivitiesResult->treasurekey = result->itemid;
				updateLuckyChest();
			}
		}
		//水手币不足
		else if (result->failed == 102)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_SAILOR_HOURE_SAILOR_COIN_SHORTAGE");
		}
		//个人背包不足
		else if (result->failed == 104)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_BAG_FULL");
		}
		break;
	} 
	case PROTO_TYPE_GetDropIncreaseRateInfoResult:
	{
		GetDropIncreaseRateInfoResult*result = (GetDropIncreaseRateInfoResult*)message;
		if (result->failed == 0)
		{
			m_nMaxpropNums = 1;
			//到时间或者没有掉落服务时
			if (result->remain_time <= 0)
			{
				showYoucost(m_pGetItemResult, m_nBtngoodTag);
			}
			//当前时低级，购低级
			else if (result->service_type == 1 && m_pGetItemResult->items[m_nBtngoodTag]->itemid == VITICKET_LOW_DROPS_ID)
			{
				showYoucost(m_pGetItemResult, m_nBtngoodTag, 1);
			}
			//当前时低级，购高级
			else if (result->service_type == 1 && m_pGetItemResult->items[m_nBtngoodTag]->itemid == VITICKET_SENIOR_DROPS_ID)
			{
				showYoucost(m_pGetItemResult, m_nBtngoodTag, 2);
			}
			//当前时高级，购低级
			else if (result->service_type == 2 && m_pGetItemResult->items[m_nBtngoodTag]->itemid == VITICKET_LOW_DROPS_ID)
			{
				showYoucost(m_pGetItemResult, m_nBtngoodTag, 3);
			}
			//当前时高级，购高级
			else if (result->service_type == 2 && m_pGetItemResult->items[m_nBtngoodTag]->itemid == VITICKET_SENIOR_DROPS_ID)
			{
				showYoucost(m_pGetItemResult, m_nBtngoodTag, 4);
			}
		}
		break;
	}
    default:
        break;
    }
}
void UIStore::updateStoreView(const GetVTicketMarketItemsResult*result)
{
    if (!result) return;
	auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
    auto panel_content_1 = paneStore->getChildByName("panel_content_1");
    auto button_gold = panel_content_1->getChildByName("button_gold");
    auto label_gold_num = button_gold->getChildByName<Text*>("label_gold_num");
	label_gold_num->setString(numSegment(StringUtils::format("%lld", result->golds)));
	auto button_ship_parts = dynamic_cast<Button*>(Helper::seekWidgetByName(paneStore, "button_ship_parts"));
	std::string button_clone8 = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_ALL"];
	button_ship_parts->setTitleText(button_clone8);
	button_ship_parts->setGlobalZOrder(1000);
    auto listview_sale = panel_content_1->getChildByName<ListView*>("listview_sale");
    listview_sale->scheduleUpdate();
    listview_sale->setDirection(ListView::Direction::VERTICAL);
    listview_sale->removeAllChildrenWithCleanup(true);
	listview_sale->setVisible(true);
	auto panel_category = dynamic_cast<Layout*>(Helper::seekWidgetByName(paneStore, "panel_category"));
    if (result->type == VITICKET_TYPE_V_PROP )
	{
		panel_category->setVisible(true);
		auto button_ship_parts = dynamic_cast<Button*>(Helper::seekWidgetByName(paneStore, "button_ship_parts"));
		button_ship_parts->setTouchEnabled(true);
		button_ship_parts->addTouchEventListener(CC_CALLBACK_2(UIStore::menuCall_func, this));

		//黑市刷新时间暂时隐藏
		/*
		auto panel_time = panel_content_1->getChildByName<Widget*>("panel_time");
		auto panel_time_clone = panel_time->clone();
		auto label_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_time_clone, "label_time"));
		auto time_title = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_time_clone, "label_title"));
		m_PTimeLabel = label_time;
		time_title->setVisible(true);
		m_PTimeLabel->setVisible(true);
		time_title->setPositionX(time_title->getContentSize().width / 2 + panel_time->getContentSize().width * 0.4);
		m_PTimeLabel->setPositionX(time_title->getContentSize().width + panel_time->getContentSize().width * 0.4);
		listview_sale->pushBackCustomItem(panel_time_clone);
		*/
		//tableView
		int num = result->n_items;
		if (num < 1)
		{
			return;
		}
		tableViewJustForBlackAllItem();
		return;
	}
	else
	{
		panel_category->setVisible(false);
	}
	
    //discount
    auto  image_onsale_bg = panel_content_1->getChildByName<Widget*>("image_onsale_bg");
    //V 
    auto  image_sale_v_1 = panel_content_1->getChildByName<Widget*>("image_sale_v_1");
    auto image_sale_v_2 = panel_content_1->getChildByName<Widget*>("image_sale_v_2");
    // coin 
    auto image_sale_ic_1 = panel_content_1->getChildByName<Widget*>("image_sale_ic_1");
    auto image_sale_ic_2 = panel_content_1->getChildByName<Widget*>("image_sale_ic_2");
    //black market
    auto imageBlackmarket_sale_1 = panel_content_1->getChildByName<Widget*>("image_blackmarket_sale_1");

    int discountGoodNum = 0;
    for (int num = 0; num < result->n_items; ++num)
    {
        if (result->items[num]->discount&&result->items[num]->promotion)
            discountGoodNum++;
    }
    //打折的
    for (int j = 0; j < discountGoodNum; ++j)
    {
        ListView *item_layer = ListView::create();
        item_layer->setDirection(ListView::Direction::HORIZONTAL);
        item_layer->setItemsMargin(2);
        item_layer->setContentSize(Size(846, 130));
        Widget*image_onsale = (Widget*)image_onsale_bg->clone();
        image_onsale->setVisible(true);
        item_layer->pushBackCustomItem(image_onsale);
        auto label_off = image_onsale->getChildByName<Text*>("label_off");
        //中英文的打折表达方式是不同的
		LanguageType nType = LanguageType(Utils::getLanguage());
		switch (nType)
		{
		case cocos2d::LanguageType::TRADITIONAL_CHINESE:
		case cocos2d::LanguageType::CHINESE:
		label_off->setString(StringUtils::format("%d", ((100-result->items[j]->discount))/10) + " " + SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_ON_SALE_OFF"]);
		break;
		default:
		label_off->setString(StringUtils::format("%d%%", (result->items[j]->discount)) + " " + SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_ON_SALE_OFF"]);
		break;
		}
      

        auto image_sale_item = image_onsale->getChildByName<ImageView*>("image_sale_item");
        image_sale_item->ignoreContentAdaptWithSize(false);
        image_sale_item->loadTexture(getVticketOrCoinPath(result->items[j]->itemid, 3));
		
		auto sp_effect = Sprite::create();
		sp_effect->setScale(1.1);
	
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon0.plist");
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eff_plist/fx_uicommon1.plist");
		Vector<SpriteFrame *> frames;
		for (int i = 0; i <= 23; i++)
		{
			std::string name = StringUtils::format("blinkstar/eff_blinkstar_%02d.png", i);
			SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
			frames.pushBack(frame);
		}
		auto animation = Animation::createWithSpriteFrames(frames, 1.0 / 20);

		//image_sale_item->runAction(RepeatForever::create(animate));
		sp_effect->runAction(RepeatForever::create(Animate::create(animation)));
		sp_effect->setPosition(image_sale_item->getContentSize() / 2);
		image_sale_item->addChild(sp_effect);

		auto button_price = image_onsale->getChildByName<Button*>("button_buy_price");
        auto label_amount = image_onsale->getChildByName<Text*>("label_amount");
		label_amount->setTextHorizontalAlignment(TextHAlignment::LEFT);
		label_amount->setTextVerticalAlignment(TextVAlignment::CENTER);
		label_amount->setAnchorPoint(Vec2(0, 0.5));
		label_amount->ignoreContentAdaptWithSize(false);
		label_amount->setContentSize(Size((button_price->getPositionX() - label_amount->getPositionX() - button_price->getBoundingBox().size.width/2 - 5),label_amount->getFontSize()));
        label_amount->setString("x" + numSegment(StringUtils::format("%lld", result->items[j]->count)));
		setTextFontSize(label_amount);
		label_amount->setPosition(Vec2(image_sale_item->getPositionX() + image_sale_item->getBoundingBox().size.width/2 + 10,image_sale_item->getPositionY() + label_amount->getBoundingBox().size.height/2));
        
        auto label_v_num = button_price->getChildByName<Text*>("label_v_num");
        auto image_v_discount = button_price->getChildByName<ImageView*>("image_v");
        if (result->type == VITICKET_TYPE_V_TICKET)
        {
            image_v_discount->setVisible(false);
            label_v_num->setContentSize(Size(130, label_v_num->getContentSize().height));
            label_v_num->setTextHorizontalAlignment(TextHAlignment::CENTER);
            label_v_num->setString(StringUtils::format("$%.2f", (result->items[j]->price) / 100.0));
        }
        else
        {
            image_v_discount->setVisible(true);
            label_v_num->setString(numSegment(StringUtils::format("%.0f", (result->items[j]->price) / 100.0)));
        }
		setTextFontSize(label_v_num);
        // tag
        button_price->setTag(j);
        button_price->addTouchEventListener(CC_CALLBACK_2(UIStore::goodItem_callfunc,this));
        listview_sale->pushBackCustomItem(item_layer);
    }
	int num = result->n_items;
	num -= discountGoodNum + 1;
	if (num < 1)
	{
		return;
	}
    //不打折的
    //sort
    VTicketMarketItemDefine*temp;
	for (int k = 0; k < num; k++)
    {
        for (int i = 0; i < result->n_items - discountGoodNum - 1 - k; i++)
        {
            if (result->items[discountGoodNum + i]->count>result->items[discountGoodNum + i + 1]->count)
            {
                temp = result->items[discountGoodNum + i + 1];
                result->items[discountGoodNum + i + 1] = result->items[discountGoodNum + i];
                result->items[discountGoodNum + i] = temp;
            }
        }
    }
    int line = ((result->n_items - discountGoodNum) % 3 == 0) ? ((result->n_items - discountGoodNum) / 3) : ((result->n_items - discountGoodNum) / 3 + 1);
    for (int k = 0; k < line; ++k)
    {
        ListView *item_layer_1 = ListView::create();
        item_layer_1->setDirection(ListView::Direction::HORIZONTAL);
        item_layer_1->setItemsMargin(2);

        if (result->type == VITICKET_TYPE_V_PROP)
        {
            item_layer_1->setContentSize(Size(846, 330));
        }
        else
        {
            item_layer_1->setContentSize(Size(846, 253));
        }
        Widget*btn_sale;
        for (int j = 0; j < 3; ++j)
        {
            if (3 * k + j < result->n_items - discountGoodNum)
            {
				switch (result->type)
				{
					case VITICKET_TYPE_V_TICKET:
					{
							if (result->items[discountGoodNum + 3 * k + j]->discount)
							{
								btn_sale = (Widget*)image_sale_v_2->clone();
							}
							else
							{
								btn_sale = (Widget*)image_sale_v_1->clone();
							}
							break;
					}
					case VITICKET_TYPE_COIN:
					{
							if (result->items[discountGoodNum + 3 * k + j]->discount)
							{
								btn_sale = (Widget*)image_sale_ic_2->clone();
							}
							else
							{
								btn_sale = (Widget*)image_sale_ic_1->clone();
							}
							break;
					}
					case VITICKET_TYPE_V_PROP:
					{
							btn_sale = (Widget*)imageBlackmarket_sale_1->clone();
							break;
					}
					default:
					break;
				}
                btn_sale->setVisible(true);
                auto image_sale_item = btn_sale->getChildByName<ImageView*>("image_sale_item");
                auto label_amount = btn_sale->getChildByName<Text*>("label_amount");
				auto t_group = btn_sale->getChildByName<Text*>("label_amount_1");
				

                auto button_buyprice = btn_sale->getChildByName<Button*>("button_buy_price");
                auto label_v_num = button_buyprice->getChildByName<Text*>("label_v_num");
                auto image_v = button_buyprice->getChildByName<ImageView*>("image_v");
				image_sale_item->ignoreContentAdaptWithSize(false);
				if (result->type == VITICKET_TYPE_COIN)
				{
					image_sale_item->ignoreContentAdaptWithSize(true);
				}
              
                // 不同的名字
                if (result->type == VITICKET_TYPE_V_PROP)
                {
                    auto info_btn = btn_sale->getChildByName<Button*>("Button_1");
                    auto panel_1 = btn_sale->getChildByName<Widget*>("panel_1");
					panel_1->setTag(result->items[discountGoodNum + 3 * k + j]->itemid); 
                    panel_1->setSwallowTouches(false);
                    panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc,this));
                    info_btn->setTag(result->items[discountGoodNum + 3 * k + j]->itemid);
					info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc,this));
                    //护送服务,10002高级护送，10003初级护送
                    if (result->items[discountGoodNum + 3 * k + j]->itemid == VITICKET_ESCORT_SENIOR_ID || result->items[discountGoodNum + 3 * k + j]->itemid == VITICKET_ESCORT_PRIMARY_ID)
                    {
                        if (result->items[discountGoodNum + 3 * k + j]->itemid == VITICKET_ESCORT_SENIOR_ID)
                        {
                            label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_SENIOR_ESCORT"]);
                            image_sale_item->loadTexture(ESCORT_SENIOR);
                        }
                        else
                        {
                            m_button_Priescort = btn_sale;
                            label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_PRIMARY_ESCORT"]);
                            image_sale_item->loadTexture(ESCORT_PRIMARY);
                        }
					}
					//掉落服务, 10004低级掉落，10005高级掉落
					else if (result->items[discountGoodNum + 3 * k + j]->itemid == VITICKET_LOW_DROPS_ID || result->items[discountGoodNum + 3 * k + j]->itemid == VITICKET_SENIOR_DROPS_ID)
					{
						if (result->items[discountGoodNum + 3 * k + j]->itemid == VITICKET_LOW_DROPS_ID)
						{
							label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKET_LOW_DROPS"]);
							image_sale_item->loadTexture(LOW_DROPS);
						}
						else
						{
							m_button_Priescort = btn_sale;
							label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKE_SENIOR_DROPS"]);
							image_sale_item->loadTexture(SENIOR_DROPS);
						}
					}
                    else
                    {
                        label_amount->setString(SINGLE_SHOP->getItemData()[result->items[discountGoodNum + 3 * k + j]->itemid].name);
                        image_sale_item->loadTexture(getItemIconPath(result->items[discountGoodNum + 3 * k + j]->itemid));
                    }
					if (result->items[discountGoodNum + 3 * k + j]->count > 1)
					{
						t_group->enableOutline(Color4B(54, 31 ,8, 255), 4);
						t_group->setVisible(true);
						t_group->setString(StringUtils::format("x%d", result->items[discountGoodNum + 3 * k + j]->count));
					}
					else
					{
						t_group->setVisible(false);
					}
                }
                else
                {
                    if (result->items[discountGoodNum + 3 * k + j]->discount)
                    {
                        auto label_amount_2 = btn_sale->getChildByName<Text*>("label_amount_2");
                        label_amount_2->setString((StringUtils::format("+%d%%", result->items[discountGoodNum + 3 * k + j]->discount)));
                    }
                    label_amount->setString("x" + numSegment(StringUtils::format("%lld", result->items[discountGoodNum + 3 * k + j]->count)));
                    image_sale_item->setTouchEnabled(false);
                    image_sale_item->loadTexture(getVticketOrCoinPath(result->items[discountGoodNum + 3 * k + j]->itemid, 3 * k + j + 1));
                }

                //tag
                button_buyprice->setTag(discountGoodNum + 3 * k + j);
                button_buyprice->addTouchEventListener(CC_CALLBACK_2(UIStore::goodItem_callfunc,this));
                //按钮上的价格显示
                if (result->type == VITICKET_TYPE_V_TICKET)
                {
                    image_v->setVisible(false);
                    label_v_num->setContentSize(Size(130, label_v_num->getContentSize().height));
                    label_v_num->setTextHorizontalAlignment(TextHAlignment::CENTER);
					label_v_num->setString(StringUtils::format("$%.2f", (result->items[discountGoodNum + 3 * k + j]->price) / 100.0));
					setTextFontSize(label_v_num);
				}
                else
                {
                    if (result->items[discountGoodNum + 3 * k + j]->itemid == VITICKET_ESCORT_PRIMARY_ID)
                    {
                        if (m_bPrimarySerIsCooling)
                        {
                            button_buyprice->setBright(false);
                            label_v_num->setVisible(false);
                            image_v->setVisible(false);
                        }
                        else
                        {
							if (SINGLE_HERO->m_iBornCity == SINGLE_HERO->m_iCityID || SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
                            {
                                button_buyprice->setBright(false);
                            }
                            else
                            {
                                button_buyprice->setBright(true);
                            }
                            label_v_num->setVisible(true);
                            image_v->setVisible(true);
                        }
                        label_v_num->setString(numSegment(StringUtils::format("%.0f", (result->items[discountGoodNum + 3 * k + j]->price) / 100.0)));
						setTextFontSize(label_v_num);
						image_v->ignoreContentAdaptWithSize(false);
                        image_v->loadTexture(SINGLE_COIN_PIC);
                        //暂时隐藏
                        if (V_Ui_type != UI_PORT)
                        {
                            button_buyprice->setBright(false);
                            button_buyprice->setTouchEnabled(false);
                        }
                    }
                    else
                    {
                        if (result->items[discountGoodNum + 3 * k + j]->itemid == VITICKET_ESCORT_SENIOR_ID)
                        {
                            if (SINGLE_HERO->m_iBornCity == SINGLE_HERO->m_iCityID)
                            {
                                button_buyprice->setBright(false);
                            }
                            else
                            {
                                button_buyprice->setBright(true);
                            }
                            //暂时隐藏
                            if (V_Ui_type != UI_PORT)
                            {
                                button_buyprice->setBright(false);
                                button_buyprice->setTouchEnabled(false);
                            }
                        }
                        label_v_num->setString(numSegment(StringUtils::format("%.0f", (result->items[discountGoodNum + 3 * k + j]->price) / 100.0)));
						setTextFontSize(label_v_num);
					}

                }

                item_layer_1->pushBackCustomItem(btn_sale);
            }
        }
        listview_sale->pushBackCustomItem(item_layer_1);
    }

    auto item_layer = dynamic_cast<ListView*>(listview_sale->getItem(listview_sale->getChildrenCount()-1));
    auto item_count = item_layer->getChildrenCount();

    //--zhengqiang
    if (result->type == VITICKET_TYPE_V_PROP)
    {
        std::vector<BlackMarketRandomShipAndEquipDefine*> randomItems;

        for (int i = 0; i < result->n_randomship; ++i)
        {
            randomItems.push_back(result->randomship[i]);
        }
        for (int i = 0; i < result->n_randomequip; ++i)
        {
            randomItems.push_back(result->randomequip[i]);
        }
        for (int i = 0; i < result->n_randomitem; ++i)
        {
            randomItems.push_back(result->randomitem[i]);
        }

        m_vRandomItems = randomItems;
        m_nRandomShipNum = result->n_randomship;
        m_nRandomEquipNum = result->n_randomequip;
        m_nRandomItemNum = result->n_randomitem;
        int randomCount = result->n_randomship + result->n_randomequip + result->n_randomitem;
        int num = 0;

        ListView *randomItemsOneLine = ListView::create();
        randomItemsOneLine->setDirection(ListView::Direction::HORIZONTAL);
        randomItemsOneLine->setItemsMargin(2);
        randomItemsOneLine->setContentSize(Size(846, 330));

        ListView* itemsOneLine;
        Widget* btn_sale_random;

        for (int i = 0; i < randomCount; ++i)
        {
            btn_sale_random = (Widget*)imageBlackmarket_sale_1->clone();
            btn_sale_random->setVisible(true);
            auto image_sale_item = btn_sale_random->getChildByName<ImageView*>("image_sale_item");
            auto label_amount = btn_sale_random->getChildByName<Text*>("label_amount");
            auto button_buyprice = btn_sale_random->getChildByName<Button*>("button_buy_price");
            auto label_v_num = button_buyprice->getChildByName<Text*>("label_v_num");
            auto image_v = button_buyprice->getChildByName<ImageView*>("image_v");
            image_sale_item->ignoreContentAdaptWithSize(false);
            auto info_btn = btn_sale_random->getChildByName<Button*>("Button_1");
            auto panel_1 = btn_sale_random->getChildByName<Widget*>("panel_1");
            panel_1->setSwallowTouches(false);
         
          
            if (i<m_nRandomShipNum)
            {
                image_sale_item->loadTexture(getShipIconPath(randomItems[i]->itemid));
				label_amount->setString(SINGLE_SHOP->getShipData()[randomItems[i]->itemid].name);
				panel_1->setTag(randomItems[i]->itemid+SHIP_TAG);
				panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc,this));
				info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc,this));
				info_btn->setTag(randomItems[i]->itemid + SHIP_TAG);
            } else{
                image_sale_item->loadTexture(getItemIconPath(randomItems[i]->itemid));
				label_amount->setString(SINGLE_SHOP->getItemData()[randomItems[i]->itemid].name);
				
				panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc,this));
				info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc,this));
				if (i>=m_nRandomShipNum + m_nRandomEquipNum)
				{
					//分类没有规则 现在临时处理
					if (randomItems[i]->sort == 8 || randomItems[i]->sort == 9)
					{
						info_btn->setTag(randomItems[i]->itemid + BLUEPTINTS_TAG);
						panel_1->setTag(randomItems[i]->itemid + BLUEPTINTS_TAG);
					}
					else
					{
						info_btn->setTag(randomItems[i]->itemid);
						panel_1->setTag(randomItems[i]->itemid);
					}
				}
				else
				{
					//装备
					info_btn->setTag(randomItems[i]->itemid+EQUIP_TAG);
					panel_1->setTag(randomItems[i]->itemid + EQUIP_TAG);
				}
				
            }
            int id = randomItems[i]->itemid;
            int price = randomItems[i]->price;
			label_v_num->setString(numSegment(StringUtils::format("%d", randomItems[i]->price)));
			setTextFontSize(label_v_num);
            button_buyprice->setTag(i);
            button_buyprice->addTouchEventListener(CC_CALLBACK_2(UIStore::randomGoodsItem_callfunc,this));

            if (i < 3 - item_count)
            {
                item_layer->pushBackCustomItem(btn_sale_random);
            }
            else
            {
                if (num % 3 == 0)
                {
                    itemsOneLine = (ListView*)randomItemsOneLine->clone();
                    listview_sale->pushBackCustomItem(itemsOneLine);
                }
                itemsOneLine->pushBackCustomItem(btn_sale_random);
                num++;

            }
        }
    }
    //--
    auto image_pulldown = panel_content_1->getChildByName<ImageView*>("image_pulldown");
    auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
    button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
    addListViewBar(listview_sale, image_pulldown);

	if (SINGLE_HERO->m_heroIsOnsea)
	{
		paneStore->setCameraMask(4);
	}
	else
	{
		paneStore->setCameraMask(1);
	}
}

void UIStore::scrollEvent(Ref* target,ScrollviewEventType type)
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

void UIStore::randomGoodsItem_callfunc(Ref *pSender, Widget::TouchEventType TouchType)
{
    if (TouchType != Widget::TouchEventType::ENDED)
    {
        return;
    }
    auto randomGoodsBtn = dynamic_cast<Widget*>(pSender);
    m_nRandomGoodsBtnTag = randomGoodsBtn->getTag();
    if (m_nRandomGoodsBtnTag < m_nRandomShipNum)
    {
        m_nRandomGoodsType = ITEM_TYPE_SHIP;
    }
    else if (m_nRandomGoodsBtnTag < (m_nRandomEquipNum + m_nRandomShipNum))
    {
        m_nRandomGoodsType = ITEM_TYPE_SHIP_EQUIP;
    }
    else
    {
        m_nRandomGoodsType = ITEM_TYPE_PORP;
    }
    SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

    int couldFillCounts = 0, couldBuyCounts = 0, counts = 0; 
   
    if (m_nRandomGoodsType == ITEM_TYPE_SHIP_EQUIP || m_nRandomGoodsType == ITEM_TYPE_PORP)
    {    
		if (SINGLE_SHOP->getItemData()[m_vRandomItems[m_nRandomGoodsBtnTag]->itemid].weight>0)
		{
			couldFillCounts = m_nPackagesize / (SINGLE_SHOP->getItemData()[m_vRandomItems[m_nRandomGoodsBtnTag]->itemid].weight / 100);
		}
		else
		{
			couldFillCounts = m_nPackagesize;
		}
        couldBuyCounts = m_pGetItemResult->golds / (m_vRandomItems[m_nRandomGoodsBtnTag]->price);
        counts = couldBuyCounts < couldFillCounts ? couldBuyCounts : couldFillCounts;
		m_nMaxpropNums = counts < ONCE_BUYITEM_MAX_NUM ? counts : ONCE_BUYITEM_MAX_NUM;
    }
    else
    {
		couldFillCounts = 10;
        couldBuyCounts = m_pGetItemResult->golds / (m_vRandomItems[m_nRandomGoodsBtnTag]->price);
		m_nMaxpropNums = couldBuyCounts < ONCE_BUYITEM_MAX_NUM ? couldBuyCounts : ONCE_BUYITEM_MAX_NUM;
    }

    if (m_nMaxpropNums < 1)
    {
		if (couldFillCounts<=0)
		{
			openSuccessOrFailDialog("TIP_VTICKETSHOP_BAG_FULL");
		}
		else if (couldBuyCounts<=0)
		{

			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openGoldNotEnoughYesOrNo(m_vRandomItems[m_nRandomGoodsBtnTag]->price);

		}
        
        return;
    }
    initPropsliderForRandomGoods2(m_nMaxpropNums);

}
void UIStore::openVticketStoreLayer(const int Ui_type, const int index)
{
    if (ENVIRONMENT == INTERNAL_TEST){
        ProtocolThread::GetInstance()->unregisterMessageCallback(this);
        ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UIStore::onServerEvent, this), this);
		ProtocolThread::GetInstance()->getLeaderboard(0, UILoadingIndicator::createWithMask(this, 4));
        return;
    }
    V_Ui_type = Ui_type;
    auto currentScene = Director::getInstance()->getRunningScene();
    this->removeFromParentAndCleanup(true);
    currentScene->addChild(this, 10);
	buttonBright(-1);
	vticketTimeSecondFresh(0);
	this->schedule(schedule_selector(UIStore::vticketTimeSecondFresh), 1.0f);

	ProtocolThread::GetInstance()->getVTicketMarketItems(index, UILoadingIndicator::createWithMask(this, 4));
}

void UIStore::showYoucost(const GetVTicketMarketItemsResult*result, const int btngoodTag, const int tipIndex)
{
    if (!result) return;
    openView(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);			//打开csb文件
    auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_COST_V_COM_CSB]);
    auto label_repair_content = view->getChildByName<Text*>("label_repair_content");
    auto listview_cost = view->getChildByName<ListView*>("listview_cost");
    auto panel_cost = listview_cost->getItem(1);

    auto image_silver_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_silver_1"));
    image_silver_1->ignoreContentAdaptWithSize(false);
    std::string content = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_SHOP_YOUWILL_BUY"];
    std::string repairContent =" x " + numSegment(StringUtils::format("%d", result->items[btngoodTag]->count));
    std::string itemName;

    auto label_advanced_repair = view->getChildByName<Text*>("label_advanced_repair");
    label_advanced_repair->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_V_OR_COIN"]);
    if (result->items[btngoodTag]->itemid == 10000)
    {
        itemName = SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_GOLD"];
    }
    else if (result->items[btngoodTag]->itemid == 10001)
    {
        itemName = SINGLE_SHOP->getTipsInfo()["TIP_BANK_SILVER"];
    }
    else
    {
        itemName = SINGLE_SHOP->getItemData()[result->items[btngoodTag]->itemid].name;
        repairContent = StringUtils::format(" x%d", m_nBuyPropNum);
    }
    content = content + " " + itemName + repairContent;
    auto label_cost_num = panel_cost->getChildByName<Text*>("label_cost_num");
    auto  image_silver_cost = panel_cost->getChildByName<ImageView*>("image_silver_1");
	switch (result->type)
	{
	case VITICKET_TYPE_V_TICKET:
	{
			image_silver_cost->setVisible(false);
			label_cost_num->setTextHorizontalAlignment(TextHAlignment::CENTER);
			label_cost_num->setString(StringUtils::format("$%.2f", result->items[btngoodTag]->price / 100.0));
			break;
	}
	case VITICKET_TYPE_COIN:
	{
			label_cost_num->setString(numSegment(StringUtils::format("%.0f", result->items[btngoodTag]->price / 100.0)));
			break;
	}
	case VITICKET_TYPE_V_PROP:
	{
								
			if (result->items[btngoodTag]->itemid == VITICKET_ESCORT_SENIOR_ID)//高级护送花V票
			{
				m_nBuyPropNum = 1;
				std::string new_vaule = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iBornCity].name;
				std::string old_vaule = "[cityname]";
				content = SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_ESCORT_CONTENT"];
				repalce_all_ditinct(content, old_vaule, new_vaule);

				label_advanced_repair->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_SENIOR_ESCORT"]);
				label_cost_num->setString(numSegment(StringUtils::format("%.0f", result->items[btngoodTag]->price / 100.0*m_nBuyPropNum)));
			}
			else if (result->items[btngoodTag]->itemid == VITICKET_ESCORT_PRIMARY_ID)//初级护送花银币
			{
				m_nBuyPropNum = 1;
				std::string new_vaule = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].name;
				std::string old_vaule = "[cityname]";
				content = SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_PRIMARY_ESCORT_CONTENT"];
				repalce_all_ditinct(content, old_vaule, new_vaule);

				label_advanced_repair->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_PRIMARY_ESCORT"]);
				label_cost_num->setString(numSegment(StringUtils::format("%.0f", result->items[btngoodTag]->price / 100.0*m_nBuyPropNum)));
				image_silver_1->loadTexture(SINGLE_COIN_PIC);
			}
			else if (result->items[btngoodTag]->itemid == VITICKET_LOW_DROPS_ID || result->items[btngoodTag]->itemid == VITICKET_SENIOR_DROPS_ID)
			{
				m_nBuyPropNum = 1;
				switch (tipIndex)
				{
				case 1:
					content = SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_LOW_LOW"];
					break;
				case 2:
					content = SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_LOW_SENIOR"];
					break;
				case 3:
					content = SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_SENIOR_LOW"];
					break;
				case 4:
					content = SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_SENIOR_SENIOR"];
					break;
				default:
					break;
				}
				if (result->items[btngoodTag]->itemid == VITICKET_LOW_DROPS_ID)
				{
					label_advanced_repair->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKET_LOW_DROPS"]);
					if (tipIndex == 0)
					{
						content = SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_LOW_DROPS"];
					}
				}
				else
				{
					label_advanced_repair->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKE_SENIOR_DROPS"]);
					if (tipIndex == 0)
					{
						content = SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_SENIOR_DROPS"];
					}
				}
				label_cost_num->setString(numSegment(StringUtils::format("%.0f", result->items[btngoodTag]->price / 100.0*m_nBuyPropNum)));
			}
			else
			{
				label_cost_num->setString(numSegment(StringUtils::format("%.0f", result->items[btngoodTag]->price / 100.0*m_nBuyPropNum)));
			}
			break;
	}
	default:
	break;
	}

    label_repair_content->setString(content);
    listview_cost->refreshView();
}

void UIStore::updateGold(int64_t gold)
{
	auto view = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	    auto panel_content_1 = view->getChildByName("panel_content_1");
	    auto button_gold = panel_content_1->getChildByName("button_gold");
	    auto labelGoldNum = button_gold->getChildByName<Text*>("label_gold_num");
	    labelGoldNum->setString(numSegment(StringUtils::format("%lld", gold)));

	    SINGLE_HERO->m_iGold = gold;

	    auto currentScene = Director::getInstance()->getRunningScene();
	    switch (V_Ui_type)
	    {
	    case UI_NONE:
	    {

	        break;
	    }
	    case UI_PORT:
	    {
	        auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	        mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
			//充值成功 首充活动消失
			mainlayer->flushVActivityBonus();
	        break;
	    }
	    case UI_BANK:
	    {
	        auto bankLayer = (UIBank*)(currentScene->getChildByTag(SCENE_TAG::BANK_TAG + 100));
	        bankLayer->updateOnlyCoinGold();
	        break;
	    }
	    case UI_WHARF:
	    {
	        auto wharfLayer = (UIPort*)(currentScene->getChildByTag(SCENE_TAG::WHARF_TAG + 100));
	        wharfLayer->updateRoleCoin();
	        break;
	    }
	    case UI_DOCK:
	    {
	        auto dockLayer = (UIShipyard*)(currentScene->getChildByTag(SCENE_TAG::DOCK_TAG + 100));
	        dockLayer->flushCoinInfo(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
	        break;
	    }
	    case UI_EXCHANGE:
	    {
	        auto exechangeLayer = (UIExchange*)(currentScene->getChildByTag(SCENE_TAG::EXCHANG_TAG + 100));
	        exechangeLayer->updateCoinNum(SINGLE_HERO->m_iGold, SINGLE_HERO->m_iCoin);
	        break;
	    }
	    case UI_PUB:
	    {
	        auto pupLayer = (UITavern*)(currentScene->getChildByTag(SCENE_TAG::PUPL_TAG + 100));
	        pupLayer->updateCoin(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
	        break;
	    }
	    case UI_CABIN:
	    {
	        break;
	    }
	    case UI_MAIL:
	    {
	        break;
	    }
	    case UI_CENTER:
	    {
	        break;
	    }
	    case UI_MAP:
	    {
			auto mapsLayer = (UISailManage*)(currentScene->getChildByTag(SCENE_TAG::MAP_TAG + 100));
			auto mapUI = mapsLayer->getChildByName<UISailHUD*>("MAPUI");
			if (mapUI->getDisableCancelFightFlag() == 3)
			{
				if (mapUI->getCancelFightCost() <= SINGLE_HERO->m_iGold)
				{
					mapUI->setDisableCancelFightFlag(2);
				}
			}
			else
			{
				if (mapUI->getSaveShipCost() <= SINGLE_HERO->m_iGold)
				{
					mapUI->setDisableCancelFightFlag(4);
				}
			}
	        break;
	    }
	    case UI_PALACE:
	    {
	        auto palaceLayer = (UIPalace*)(currentScene->getChildByTag(SCENE_TAG::PALACE_TAG + 100));
	        palaceLayer->showMainCoin();
	        break;
	    }
	    case UI_BATTLE:
	    {
	        break;
	    }
	    default:
	        break;
	    }
}


void UIStore::updateCoinGold(const BuyVTicketMarketItemResult*result)
{

	auto view = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
    auto panel_content_1 = view->getChildByName("panel_content_1");
    auto button_gold = panel_content_1->getChildByName("button_gold");
    auto labelGoldNum = button_gold->getChildByName<Text*>("label_gold_num");
    labelGoldNum->setString(numSegment(StringUtils::format("%lld", result->golds)));
    SINGLE_HERO->m_iCoin = result->coins;
    SINGLE_HERO->m_iGold = result->golds;

    auto currentScene = Director::getInstance()->getRunningScene();
    switch (V_Ui_type)
    {
    case UI_NONE:
    {

        break;
    }
    case UI_PORT:
    {
        auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
        mainlayer->flushCionAndGold(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
        mainlayer->flushSudanMaskNum(result->count);
        break;
    }
    case UI_BANK:
    {
        auto bankLayer = (UIBank*)(currentScene->getChildByTag(SCENE_TAG::BANK_TAG + 100));
        bankLayer->updateOnlyCoinGold();
        break;
    }
    case UI_WHARF:
    {
        auto wharfLayer = (UIPort*)(currentScene->getChildByTag(SCENE_TAG::WHARF_TAG + 100));
        wharfLayer->updateRoleCoin();
        break;
    }
    case UI_DOCK:
    {
        auto dockLayer = (UIShipyard*)(currentScene->getChildByTag(SCENE_TAG::DOCK_TAG + 100));
        dockLayer->flushCoinInfo(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
        break;
    }
    case UI_EXCHANGE:
    {
        auto exechangeLayer = (UIExchange*)(currentScene->getChildByTag(SCENE_TAG::EXCHANG_TAG + 100));
        exechangeLayer->updateCoinNum(SINGLE_HERO->m_iGold, SINGLE_HERO->m_iCoin);
        break;
    }
    case UI_PUB:
    {
        auto pupLayer = (UITavern*)(currentScene->getChildByTag(SCENE_TAG::PUPL_TAG + 100));
        pupLayer->updateCoin(SINGLE_HERO->m_iCoin, SINGLE_HERO->m_iGold);
        break;
    }
    case UI_CABIN:
    {
        break;
    }
    case UI_MAIL:
    {
        break;
    }
    case UI_CENTER:
    {
        break;
    }
    case UI_MAP:
    {
        break;
    }
    case UI_PALACE:
    {
        auto palaceLayer = (UIPalace*)(currentScene->getChildByTag(SCENE_TAG::PALACE_TAG + 100));
        palaceLayer->showMainCoin();
        break;
    }
    case UI_BATTLE:
    {
        break;
    }
    default:
        break;
    }
}

void UIStore::buttonBright(const int brightTag)
{
	openView(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto listview_left = paneStore->getChildByName<ListView*>("listview_left");
	auto panel_content_1 = paneStore->getChildByName("panel_content_1");
	auto panel_lucky = paneStore->getChildByName("panel_lucky");
	auto panel_time = panel_content_1->getChildByName<Widget*>("panel_time");
	auto time_title = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_time, "label_title"));
	time_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKET_TIME_REFRESH_TITLE"]);
	auto label_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_time, "label_time"));
	m_PTimeLabel = label_time;
	//初始化所有按钮
	m_pButtonTag = brightTag;
	if (brightTag == -1)
	{
		for (size_t i = 0; i < 5; i++)
		{
			m_pButton = listview_left->getItem(2 * i);
			if (SINGLE_HERO->m_heroIsOnsea)
			{
				if (i != 0)
				{
					m_pButton->setVisible(false);
				}
				
				if (i < 4)
				{
					listview_left->getItem(2 * i + 1)->setVisible(false);
				}
			}
			m_pButton->setBright(false);
			m_pButton->getChildByName("image_on")->setVisible(false);
			m_pButton->getChildByName("label_on")->setVisible(false);
			m_pButton->getChildByName("label_off")->setVisible(true);
			m_pButton->setTouchEnabled(true);
		}
		return;
	}

	if (m_pButton)
	{
		m_pButton->setBright(false);
		m_pButton->getChildByName("image_on")->setVisible(false);
		m_pButton->getChildByName("label_on")->setVisible(false);
		m_pButton->getChildByName("label_off")->setVisible(true);
		m_pButton->setTouchEnabled(true);
	}
	
	m_pButton = listview_left->getItem(2 * brightTag);
	m_pButton->setBright(true);
	m_pButton->getChildByName("image_on")->setVisible(true);
	m_pButton->getChildByName("label_on")->setVisible(true);
	m_pButton->getChildByName("label_off")->setVisible(false);
	m_pButton->setTouchEnabled(false);
	

	auto label_title_v = panel_content_1->getChildByName<Text*>("label_title_v");
	auto button_bank_info = panel_content_1->getChildByName<Button*>("button_bank_info");
	auto button_gold = panel_content_1->getChildByName<Button*>("button_gold");
	auto image_gold = button_gold->getChildByName<ImageView*>("image_gold");
	auto label_gold_num = button_gold->getChildByName<Text*>("label_gold_num");
	auto str_title = StringUtils::format("TIP_STORE_TITLE_%d", brightTag + 1);
	label_title_v->setString(SINGLE_SHOP->getTipsInfo()[str_title]);
	image_gold->ignoreContentAdaptWithSize(false);
	image_gold->setContentSize(Size(48,35));
	auto postiony1 = 573;
	auto postiony2 = 555;
	switch (brightTag)
	{
	case VITICKET_TYPE_V_TICKET:
	{
		image_gold->loadTexture(getVticketOrCoinPath(10000, 1));
		label_gold_num->setString(numSegment(StringUtils::format("%lld", m_pGetItemResult->golds)));
		panel_content_1->setVisible(true);
		panel_lucky->setVisible(false);
		button_bank_info->setVisible(false);
		time_title->setVisible(false);
		m_PTimeLabel->setVisible(false);
		label_title_v->setPositionY(postiony2);
		break;
	}
	case VITICKET_TYPE_COIN:
	{
		image_gold->loadTexture(getVticketOrCoinPath(10000, 1));
		label_gold_num->setString(numSegment(StringUtils::format("%lld", m_pGetItemResult->golds)));
		panel_content_1->setVisible(true);
		panel_lucky->setVisible(false);
		button_bank_info->setVisible(false);
		time_title->setVisible(false);
		m_PTimeLabel->setVisible(false);
		label_title_v->setPositionY(postiony2);
		break;
	}
	case VITICKET_TYPE_V_PROP:
	{
		image_gold->loadTexture(getVticketOrCoinPath(10000, 1));
		label_gold_num->setString(numSegment(StringUtils::format("%lld", m_pGetItemResult->golds)));
		panel_content_1->setVisible(true);
		panel_lucky->setVisible(false);
		button_bank_info->setVisible(false);
		time_title->setVisible(false);
		m_PTimeLabel->setVisible(false);
		label_title_v->setPositionY(postiony1);
		break;
	}
	case VITICKET_TYPE_SAILOR_HOUSE:
	{
		image_gold->setContentSize(Size(48, 48));
		image_gold->loadTexture(getItemIconPath(10001));
		label_gold_num->setString(numSegment(StringUtils::format("%lld", m_pMarketActivitiesResult->sailorcoins)));
		panel_content_1->setVisible(true);
		panel_lucky->setVisible(false);
		button_bank_info->setVisible(true);
		button_bank_info->setPositionX(label_title_v->getPositionX() + label_title_v->getBoundingBox().size.width + button_bank_info->getBoundingBox().size.width / 2 + 10);
		time_title->setVisible(true);
		m_PTimeLabel->setVisible(true);	
		time_title->setPositionX(time_title->getContentSize().width / 2);
		m_PTimeLabel->setPositionX(time_title->getContentSize().width);
		label_title_v->setPositionY(postiony1);
		break;
	}
	case VITICKET_TYPE_LUCKY_CHEST:
	{
		panel_content_1->setVisible(false);
		panel_lucky->setVisible(true);
		time_title->setVisible(false);
		m_PTimeLabel->setVisible(false);
		label_title_v->setPositionY(postiony2);
		break;
	}
	default:
		break;
	}
}

void UIStore::initPropsliderForRandomGoods2(const int maxpropNum)
{
	openView(COMMOM_COCOS_RES[C_VIEW_CONFIRM_STORE]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_CONFIRM_STORE]);
	auto image_items_bg = view->getChildByName<ImageView*>("image_items_bg");
	auto image_item = image_items_bg->getChildByName<ImageView*>("image_item");
	auto label_item_name = view->getChildByName<Text*>("label_item_name");
	auto label_dropitem_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_dropitem_num"));
	auto slider_dropitem = dynamic_cast<Slider*>(Helper::seekWidgetByName(view, "slider_goods_num"));
	auto label_cost_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cost_num"));
	
    if (maxpropNum <= 1)
    {
        slider_dropitem->setTouchEnabled(false);
    }
    else
    {
        slider_dropitem->setTouchEnabled(true);
    }
    slider_dropitem->setPercent(1.0 / maxpropNum * 100);
    m_pSlider = slider_dropitem;
    m_pLabelNum = label_dropitem_num;
   
    slider_dropitem->addEventListener(CC_CALLBACK_2(UIStore::propSliderChange,this));
    label_dropitem_num->setString(StringUtils::format("x %d", 1));//默认为1
    m_nBuyPropNum = 1;
    if (m_nRandomGoodsType == ITEM_TYPE_SHIP)
    {
        image_item->loadTexture(getShipIconPath(m_vRandomItems[m_nRandomGoodsBtnTag]->itemid));
		label_item_name->setString(getShipName(m_vRandomItems[m_nRandomGoodsBtnTag]->itemid));
    }
    else
    {
        image_item->loadTexture(getItemIconPath(m_vRandomItems[m_nRandomGoodsBtnTag]->itemid));
		label_item_name->setString(getItemName(m_vRandomItems[m_nRandomGoodsBtnTag]->itemid));
    }
	setBgButtonFormIdAndType(image_items_bg, m_vRandomItems[m_nRandomGoodsBtnTag]->itemid, m_nRandomGoodsType);
    m_bIsRandomGoods = true;
	label_cost_num->setString(StringUtils::format("%d", m_nBuyPropNum * m_vRandomItems[m_nRandomGoodsBtnTag]->price));
}
void UIStore::initPropslider(const int maxpropNum)
{
	openView(COMMOM_COCOS_RES[C_VIEW_CONFIRM_STORE]);
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_CONFIRM_STORE]);
	auto image_items_bg = view->getChildByName<ImageView*>("image_items_bg");
	auto image_item = image_items_bg->getChildByName<ImageView*>("image_item");
	auto label_item_name = view->getChildByName<Text*>("label_item_name");
	auto label_dropitem_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_dropitem_num"));
	auto slider_dropitem = dynamic_cast<Slider*>(Helper::seekWidgetByName(view, "slider_goods_num"));
	auto label_cost_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cost_num"));
	
    if (maxpropNum <= 1)
    {
        slider_dropitem->setTouchEnabled(false);
    }
    else
    {
        slider_dropitem->setTouchEnabled(true);
    }
    slider_dropitem->setPercent(1.0 / maxpropNum * 100);
    m_pSlider = slider_dropitem;
    m_pLabelNum = label_dropitem_num;

	int item_id = 0;
	int item_price = 0;
	int item_type = ITEM_TYPE_PORP;
	int item_group = 1;
	if (m_pButton->getTag() == VITICKET_TYPE_V_PROP)
	{
		item_id = m_pGetItemResult->items[m_nBtngoodTag]->itemid;
		item_price = m_pGetItemResult->items[m_nBtngoodTag]->price / 100;
		item_group = m_pGetItemResult->items[m_nBtngoodTag]->count;
	}
	else
	{
		if (m_nBtngoodTag < m_pMarketActivitiesResult->n_items)
		{
			item_id = m_pMarketActivitiesResult->items[m_nBtngoodTag]->itemid;
			item_price = m_pMarketActivitiesResult->items[m_nBtngoodTag]->price;
		}
		else
		{
			item_id = m_pMarketActivitiesResult->randomitems[m_nBtngoodTag - m_pMarketActivitiesResult->n_items]->itemid;
			item_price = m_pMarketActivitiesResult->randomitems[m_nBtngoodTag - m_pMarketActivitiesResult->n_items]->price;
			item_type = m_pMarketActivitiesResult->randomitems[m_nBtngoodTag - m_pMarketActivitiesResult->n_items]->itemtype;
		}

		auto image_silver_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_silver_1"));
		image_silver_1->loadTexture(getItemIconPath(10001));
	}
	std::string name;
	std::string path;
	getItemNameAndPath(item_type, item_id, name, path);
	if (item_group > 1)
	{
		name += StringUtils::format(" x%d", item_group);
	}
	label_item_name->setString(name);
	image_item->loadTexture(path);
    slider_dropitem->addEventListener(CC_CALLBACK_2(UIStore::propSliderChange,this));
    label_dropitem_num->setString(StringUtils::format("x %d", 1));//默认为1
    m_nBuyPropNum = 1;
	setBgButtonFormIdAndType(image_items_bg, item_id, item_type);
	label_cost_num->setString(StringUtils::format("%d", m_nBuyPropNum * item_price));
}
void UIStore::propSliderChange(Ref* obj, cocos2d::ui::Slider::EventType type)
{
	if (type != cocos2d::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
    {
        return;
    }
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_CONFIRM_STORE]);
	auto label_dropitem_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_dropitem_num"));
	auto slider_dropitem = dynamic_cast<Slider*>(Helper::seekWidgetByName(view, "slider_goods_num"));
	auto label_cost_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cost_num"));

	label_dropitem_num->setString(StringUtils::format("x %d", m_nMaxpropNums*slider_dropitem->getPercent() / 100));
    m_nBuyPropNum = m_nMaxpropNums*slider_dropitem->getPercent() / 100.0;
    if (m_nBuyPropNum <= 1)//最少数量为1
    {
		label_dropitem_num->setString("x 1");
        m_nBuyPropNum = 1;
    }

	int item_price = 0;
	if (m_pButton->getTag() == VITICKET_TYPE_V_PROP)
	{
		if (m_bIsRandomGoods)
		{
			item_price = m_vRandomItems[m_nRandomGoodsBtnTag]->price;
		}
		else
		{
			item_price = m_pGetItemResult->items[m_nBtngoodTag]->price / 100;
		}
	}
	else
	{
		if (m_nBtngoodTag < m_pMarketActivitiesResult->n_items)
		{
			item_price = m_pMarketActivitiesResult->items[m_nBtngoodTag]->price;
		}
		else
		{
			item_price = m_pMarketActivitiesResult->randomitems[m_nBtngoodTag - m_pMarketActivitiesResult->n_items]->price;
		}
	}
	label_cost_num->setString(StringUtils::format("%d", m_nBuyPropNum * item_price));
}

void UIStore::changeCost()
{
	auto view = getViewRoot(COMMOM_COCOS_RES[C_VIEW_CONFIRM_STORE]);
	auto label_cost_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_cost_num"));

	int item_price = 0;
	if (m_pButton->getTag() == VITICKET_TYPE_V_PROP)
	{
		if (m_bIsRandomGoods)
		{
			item_price = m_vRandomItems[m_nRandomGoodsBtnTag]->price;
		}
		else
		{
			item_price = m_pGetItemResult->items[m_nBtngoodTag]->price / 100;
		}
	}
	else
	{
		if (m_nBtngoodTag < m_pMarketActivitiesResult->n_items)
		{
			item_price = m_pMarketActivitiesResult->items[m_nBtngoodTag]->price;
		}
		else
		{
			item_price = m_pMarketActivitiesResult->randomitems[m_nBtngoodTag - m_pMarketActivitiesResult->n_items]->price;
		}
	}
	label_cost_num->setString(StringUtils::format("%d", m_nBuyPropNum * item_price));
}

void UIStore::propItem_callfunc(Ref *pSender, Widget::TouchEventType TouchType)
{
    if (TouchType != Widget::TouchEventType::ENDED)
    {
        return;
    }
    SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
    auto propItem = dynamic_cast<Widget*>(pSender);
    auto propTag = propItem->getTag();
	if (propTag == VITICKET_ESCORT_SENIOR_ID || propTag == VITICKET_ESCORT_PRIMARY_ID || propTag == VITICKET_LOW_DROPS_ID || propTag == VITICKET_SENIOR_DROPS_ID)
    {
        escortExplain(propTag);
    }
    else
    {
		UICommon::getInstance()->openCommonView(this);

		if (propTag >= BLUEPTINTS_TAG)
		{
			UICommon::getInstance()->flushDrawingDetail(nullptr, propTag - BLUEPTINTS_TAG, true);
		}
		else if (propTag >= SHIP_TAG)//船的Tag
		{
			UICommon::getInstance()->flushShipDetail(nullptr, propTag - SHIP_TAG, true);
		}
		else if (propTag >= EQUIP_TAG)
		{
			UICommon::getInstance()->flushEquipsDetail(nullptr, propTag - EQUIP_TAG, true);
		}
		else
		{
			UICommon::getInstance()->flushItemsDetail(nullptr, propTag, true);
		}
    }

}
void UIStore::escortExplain(int escortId)
{
    openView(COMMOM_COCOS_RES[C_VIEW_ESCORT_INFO_CSB]);
    auto viewEscort = getViewRoot(COMMOM_COCOS_RES[C_VIEW_ESCORT_INFO_CSB]);
    auto i_image = viewEscort->getChildByName<ImageView*>("image_1");
    auto label_goods_name = viewEscort->getChildByName<Text*>("label_goods_name");
    auto label_info = dynamic_cast<Text*>(Helper::seekWidgetByName(viewEscort, "label_info"));
    std::string st_str;
    if (escortId == VITICKET_ESCORT_SENIOR_ID)
    {
        i_image->loadTexture(ESCORT_SENIOR);
        label_goods_name->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_SENIOR_ESCORT"]);
        st_str = SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_ESCORT_CONTENT"];
		std::string new_vaule = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iBornCity].name;
		std::string old_vaule = "[cityname]";
		repalce_all_ditinct(st_str, old_vaule, new_vaule);
    }
	else if (escortId == VITICKET_ESCORT_PRIMARY_ID)
    {
        i_image->loadTexture(ESCORT_PRIMARY);
        label_goods_name->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_PRIMARY_ESCORT"]);      
		std::string new_vaule = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].name;
		std::string old_vaule = "[cityname]";
		st_str = SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_PRIMARY_ESCORT_CONTENT"];
		repalce_all_ditinct(st_str, old_vaule, new_vaule);

	}
	else if (escortId == VITICKET_LOW_DROPS_ID)
	{
		i_image->loadTexture(LOW_DROPS);
		label_goods_name->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKET_LOW_DROPS"]);
		st_str = SINGLE_SHOP->getTipsInfo()["TIP_VITICKET_LOW_DROPS_CONTENT"];
	}
	else
	{
		i_image->loadTexture(SENIOR_DROPS);
		label_goods_name->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKE_SENIOR_DROPS"]);
		st_str = SINGLE_SHOP->getTipsInfo()["TIP_VITICKE_SENIOR_DROPS_CONTENT"];
	}
    label_info->setString(st_str);
    auto l_info = viewEscort->getChildByName<ListView*>("list_info");
    label_info->setContentSize(Size(label_info->getContentSize().width, getLabelHight(st_str, label_info->getContentSize().width, label_info->getFontName(), label_info->getFontSize())));
    auto image_pulldown = viewEscort->getChildByName<ImageView*>("image_pulldown");
    auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
    button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
    addListViewBar(l_info, image_pulldown);
}
void UIStore::vticketTimeSecondFresh(float t)
{
	if (m_pButtonTag == VITICKET_TYPE_V_PROP)
	{
		if (m_pGetItemResult)
		{
			//黑市刷新
			m_pGetItemResult->blackmarketlefttime--;
			int64_t Hours = (m_pGetItemResult->blackmarketlefttime) / 3600 % 24;
			int64_t Mins = (m_pGetItemResult->blackmarketlefttime / 60) % 60;
			int64_t Seconds = m_pGetItemResult->blackmarketlefttime % 60;
			std::string  blackTime = StringUtils::format("%02lld", Hours) + ":" + StringUtils::format("%02lld", Mins) + ":" + StringUtils::format("%02lld", Seconds);
			if (!m_pGetItemResult->blackmarketlefttime)
			{
				ProtocolThread::GetInstance()->getVTicketMarketItems(VITICKET_TYPE_V_PROP, UILoadingIndicator::create(this));
			}
			if (m_PTimeLabel)
			{
				m_PTimeLabel->setString(blackTime);
			}
			
			//初级护送
			if (m_button_Priescort&&m_pGetItemResult->deliverservicecd > 0 && (m_nRandomGoodsBtnTag == 0 || m_nRandomGoodsBtnTag == 1))
			{
				--m_pGetItemResult->deliverservicecd;
				int64_t serviceHours = (m_pGetItemResult->deliverservicecd) / 3600 % 24;
				int64_t serviceMins = (m_pGetItemResult->deliverservicecd / 60) % 60;
				int64_t serviceSeconds = m_pGetItemResult->deliverservicecd % 60;
				std::string  esscortTime = StringUtils::format("%02lld", serviceHours) + ":" + StringUtils::format("%02lld", serviceMins) + ":" + StringUtils::format("%02lld", serviceSeconds);
				auto image_sale_item = m_button_Priescort->getChildByName<ImageView*>("image_sale_item");
				auto label_amount = m_button_Priescort->getChildByName<Text*>("label_amount");
				auto button_buyprice = m_button_Priescort->getChildByName<Button*>("button_buy_price");
				auto label_v_num = button_buyprice->getChildByName<Text*>("label_v_num");
				auto image_v = button_buyprice->getChildByName<ImageView*>("image_v");
				button_buyprice->setTitleText(esscortTime);
				if (!m_pGetItemResult->deliverservicecd)
				{
					button_buyprice->setTouchEnabled(true);
					button_buyprice->setBright(true);
					label_v_num->setVisible(true);
					image_v->setVisible(true);
					button_buyprice->setTitleText("");
					m_bPrimarySerIsCooling = false;
				}
			}
		}
	}
	if (m_pButtonTag == VITICKET_TYPE_SAILOR_HOUSE)
	{
		//水手之家
		if (m_pMarketActivitiesResult)
		{
			m_pMarketActivitiesResult->activitieslefttime--;
			int64_t Hours = (m_pMarketActivitiesResult->activitieslefttime) / 3600 % 24;
			int64_t Mins = (m_pMarketActivitiesResult->activitieslefttime / 60) % 60;
			int64_t Seconds = m_pMarketActivitiesResult->activitieslefttime % 60;

			std::string  blackTime = StringUtils::format("%02lld", Hours) + ":" + StringUtils::format("%02lld", Mins) + ":" + StringUtils::format("%02lld", Seconds);
			if (!m_pMarketActivitiesResult->activitieslefttime)
			{
				ProtocolThread::GetInstance()->getVTicketMarketActivities(VITICKET_TYPE_SAILOR_HOUSE, UILoadingIndicator::create(this));
			}
			if (m_PTimeLabel)
			{
				m_PTimeLabel->setString(blackTime);
			}
		}
	}
}
void UIStore::comebackHome()
{
	//护送服务相当于重新进入主城 重新检测相关信息
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	mainlayer->setUsePropBackToCity(true);
    ProtocolThread::GetInstance()->getCurrentCityData();
}
void UIStore::comebackHomeEcsort()
{
    auto currentScene = Director::getInstance()->getRunningScene();
    auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	auto cityLayer = mainlayer->getChildren().at(0);
	cityLayer->setVisible(true);
	closeView(MAPUI_COCOS_RES[INDEX_UI_FIGHT_DEFEATED_DIALOG_CSB]);
	this->removeFromParentAndCleanup(true);
}
void UIStore::addEscortBlinkImage()
{
	m_eUIType = UI_MAP;
	playAudio();
	auto currentScene = Director::getInstance()->getRunningScene();
	auto mainlayer = (UIMain*)(currentScene->getChildByTag(SCENE_TAG::MAIN_TAG + 100));
	auto cityLayer = mainlayer->getChildren().at(0);
	cityLayer->setVisible(false);
	/*auto imageEscort = ImageView::create("res_lua/CeutaBattle.jpg");
	imageEscort->setAnchorPoint(Vec2(0, 0));
	mainlayer->addChild(imageEscort);
	imageEscort->runAction(Sequence::create(Repeat::create(Sequence::create(FadeOut::create(1.0f), FadeIn::create(1.0f), nullptr),4),RemoveSelf::create(),nullptr));*/
}

void UIStore::updateSailorHouse()
{
	auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto panel_category = dynamic_cast<Layout*>(Helper::seekWidgetByName(paneStore, "panel_category"));
	panel_category->setVisible(false);
	auto panel_content_1 = paneStore->getChildByName("panel_content_1");

	auto listview_sale = panel_content_1->getChildByName<ListView*>("listview_sale");
	listview_sale->setDirection(ListView::Direction::VERTICAL);
	listview_sale->removeAllChildrenWithCleanup(true);

	auto item = panel_content_1->getChildByName<ImageView*>("image_blackmarket_sale_1");
	int n_grid = m_pMarketActivitiesResult->n_items + m_pMarketActivitiesResult->n_randomitems;
	int n_line = ceil(n_grid / 3.0);
	for (size_t i = 0; i < n_line; i++)
	{
		auto item_layer = ListView::create();
		item_layer->setDirection(ListView::Direction::HORIZONTAL);
		item_layer->setItemsMargin(2);
		item_layer->setContentSize(Size(846,326));
		for (size_t j = 0; j < 3; j++)
		{
			if (3 * i + j < n_grid)
			{
				auto item_clone = item->clone();
				auto image_item = item_clone->getChildByName<ImageView*>("image_sale_item");
				auto item_name = item_clone->getChildByName<Text*>("label_amount");
				auto b_buy = item_clone->getChildByName<Button*>("button_buy_price");
				auto t_cost = b_buy->getChildByName<Text*>("label_v_num");
				auto i_sailor = b_buy->getChildByName<ImageView*>("image_v");
				auto info_btn = item_clone->getChildByName<Button*>("Button_1");
				auto panel_1 = item_clone->getChildByName<Button*>("panel_1");
				b_buy->addTouchEventListener(CC_CALLBACK_2(UIStore::sailorHouse_callfunc,this));
				b_buy->setTag(3 * i + j);
				if (3 * i + j < m_pMarketActivitiesResult->n_items)
				{
					image_item->loadTexture(getItemIconPath(m_pMarketActivitiesResult->items[3 * i + j]->itemid));
					auto names = getItemName(m_pMarketActivitiesResult->items[3 * i + j]->itemid);
					item_name->setString(getItemName(m_pMarketActivitiesResult->items[3 * i + j]->itemid));
					t_cost->setString(numSegment(StringUtils::format("%d", m_pMarketActivitiesResult->items[3 * i + j]->price)));
					panel_1->setTag(m_pMarketActivitiesResult->items[3 * i + j]->itemid);
					info_btn->setTag(m_pMarketActivitiesResult->items[3 * i + j]->itemid);
					item_clone->setTag(SINGLE_SHOP->getItemData()[m_pMarketActivitiesResult->items[3 * i + j]->itemid].type);	
				}
				else
				{
					std::string name;
					std::string path;
					getItemNameAndPath(m_pMarketActivitiesResult->randomitems[3 * i + j - m_pMarketActivitiesResult->n_items]->itemtype, m_pMarketActivitiesResult->randomitems[3 * i + j - m_pMarketActivitiesResult->n_items]->itemid, name, path);
					image_item->loadTexture(path);
					item_name->setString(vticItemName(name));
					t_cost->setString(numSegment(StringUtils::format("%d", m_pMarketActivitiesResult->randomitems[3 * i + j - m_pMarketActivitiesResult->n_items]->price)));
					panel_1->setTag(m_pMarketActivitiesResult->randomitems[3 * i + j - m_pMarketActivitiesResult->n_items]->itemid);
					info_btn->setTag(m_pMarketActivitiesResult->randomitems[3 * i + j - m_pMarketActivitiesResult->n_items]->itemid);
					item_clone->setTag(m_pMarketActivitiesResult->randomitems[3 * i + j - m_pMarketActivitiesResult->n_items]->itemtype);
				}
				setTextFontSize(t_cost);
				i_sailor->loadTexture(getItemIconPath(10001));
				panel_1->setSwallowTouches(false);
				panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::menuCall_func,this));
				info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::menuCall_func,this));
				item_layer->pushBackCustomItem(item_clone);
			}
			else
			{
				break;
			}
		}
		listview_sale->pushBackCustomItem(item_layer);
	}

	auto image_pulldown = panel_content_1->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 - button_pulldown->getBoundingBox().size.width / 2);
	addListViewBar(listview_sale, image_pulldown);
}

void UIStore::updateLuckyChest()
{
	auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto panel_lucky = paneStore->getChildByName<Widget*>("panel_lucky");
	auto image_chest = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_lucky, "image_sailor_currency_0"));
	auto image_key = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_lucky, "image_sailor_currency"));
	auto t_chest = image_chest->getChildByName<Text*>("text_num");
	auto t_key = image_key->getChildByName<Text*>("text_num");
	auto b_open = panel_lucky->getChildByName<Button*>("button_open");
	t_chest->setString(StringUtils::format("%lld", m_pMarketActivitiesResult->treasure));
	t_key->setString(StringUtils::format("%lld", m_pMarketActivitiesResult->treasurekey));
	if (m_pMarketActivitiesResult->treasurekey < 1 || m_pMarketActivitiesResult->treasure < 1)
	{
		b_open->setBright(false);
		b_open->setTouchEnabled(false);
	}
	else
	{
		b_open->setBright(true);
		b_open->setTouchEnabled(true);
	}
}
//护送走字
void UIStore::escortDialog(int type)
{
	openView(MAPUI_COCOS_RES[INDEX_UI_FIGHT_DEFEATED_DIALOG_CSB]);
	auto viewfieldDiaog = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_FIGHT_DEFEATED_DIALOG_CSB]);
	viewfieldDiaog->addTouchEventListener(CC_CALLBACK_2(UIStore::menuCall_func, this));
	auto panel_defeat = dynamic_cast<Widget*>(Helper::seekWidgetByName(viewfieldDiaog, "panel_defeat"));
	panel_defeat->setVisible(true);
	auto i_text_content = panel_defeat->getChildByName<Text*>("text_content");
	auto text_title = panel_defeat->getChildByName<Text*>("text_title");
	text_title->setVisible(false);
	if (type ==VITICKET_ESCORT_PRIMARY_ID)
	{
		chatContent = SINGLE_SHOP->getTipsInfo()["TIP_V_STORE_ESCORT_CONTENT_COIN"];
	}
	else if (type == VITICKET_ESCORT_SENIOR_ID)
	{
		chatContent = SINGLE_SHOP->getTipsInfo()["TIP_V_STORE_ESCORT_CONTENT_V"];
	}
	
	anchPic = dynamic_cast<ImageView*>(Helper::seekWidgetByName(viewfieldDiaog, "image_anchor"));
	anchPic->runAction(RepeatForever::create(Sequence::createWithTwoActions(EaseBackOut::create(MoveBy::create(0.5, Vec2(0, 10))), EaseBackOut::create(MoveBy::create(0.5, Vec2(0, -10))))));
	anchPic->setVisible(false);
	std::string new_vaule = SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iBornCity].name;
	std::string old_vaule = "[city]";
	repalce_all_ditinct(chatContent, old_vaule, new_vaule);
	chatTxt = i_text_content;
	this->schedule(schedule_selector(UIStore::escortContentDialog), 0.1 / 3);
}
void UIStore::escortContentDialog(float t)
{
	std::string	 text = chatTxt->getString();
	int lenAfter = lenNum + text.length();
	std::string showT = "";
	int plusNum = chatGetUtf8CharLen(chatContent.c_str() + lenAfter);
	lenAfter = lenAfter + plusNum;
	int maxLen = chatContent.length() + 1;
	showT = chatContent.substr(lenNum, lenAfter - lenNum);
	chatTxt->setString(showT);
	if (lenAfter >= maxLen - 1)
	{
		m_bDiaolgOver = true;
		anchPic->setVisible(true);
		chatTxt = nullptr;
		this->unschedule(schedule_selector(UIStore::escortContentDialog));
	}
	else
	{
		m_bDiaolgOver = false;
	}
}
std::string UIStore::vticItemName(std::string itemName)
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
void UIStore::RefreshItems(int sort)
{
	if (!m_pGetItemResult) return;
	m_vThingBlack.clear();
	m_vRandomRefreshItems.clear();
	for (int i = 0; i < m_vRandomItems.size(); i++)
	{
		if (m_vRandomItems.at(i)->sort == sort)
		{
			m_vRandomRefreshItems.push_back(m_vRandomItems.at(i));
		}
	}
	for (int i = 0; i < m_pGetItemResult->n_items; i++)
	{
 		if (m_pGetItemResult->items[i]->sort == sort)
		{
			m_vThingBlack.push_back(m_pGetItemResult->items[i]);
		}
	}
	if (sort==7)
	{
		
		VTicketMarketItemDefine*temp;
		for (int k = 0; k < m_vThingBlack.size(); k++)
		{
			for (int i = 0; i < m_vThingBlack.size() - 1 - k; i++)
			{
				if (m_vThingBlack.at(i)->price>m_vThingBlack.at(i + 1)->price)
				{
					temp = m_vThingBlack.at(i + 1);
					m_vThingBlack.at(i + 1) = m_vThingBlack.at(i);
					m_vThingBlack.at(i) = temp;
				}
			}
		}
		BlackMarketRandomShipAndEquipDefine*tempRandom;
		for (int k = 0; k < m_vRandomRefreshItems.size(); k++)
		{
			for (int i = 0; i < m_vRandomRefreshItems.size() - 1 - k; i++)
			{
				if (m_vRandomRefreshItems.at(i)->price>m_vRandomRefreshItems.at(i + 1)->price)
				{
					tempRandom = m_vRandomRefreshItems.at(i + 1);
					m_vRandomRefreshItems.at(i + 1) = m_vRandomRefreshItems.at(i);
					m_vRandomRefreshItems.at(i) = tempRandom;
				}
			}
		}

	}
#if 1
	openView(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto panel_content_no = paneStore->getChildByName("panel_centent_no");
	int allNums = m_vRandomRefreshItems.size() + m_vThingBlack.size();
	int lines = (allNums % 3 == 0) ? (allNums / 3) : (allNums / 3 + 1);
	if (lines < 1)
	{
		panel_content_no->setVisible(true);
		m_PTimeLabel = nullptr;
	}
	else
	{
		panel_content_no->setVisible(false);
	}
	m_tableview_tag = TABLEVIEW_FOR_CATEGORY;
	tableViewForcategory();
#else
	openView(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto panel_content_1 = paneStore->getChildByName("panel_content_1");
	auto listview_sale = panel_content_1->getChildByName<ListView*>("listview_sale");
	listview_sale->setDirection(ListView::Direction::VERTICAL);
	listview_sale->removeAllChildrenWithCleanup(true);

	auto imageBlackmarket_sale_1 = panel_content_1->getChildByName<Widget*>("image_blackmarket_sale_1");

	int allNum = m_vRandomRefreshItems.size() + m_vThingBlack.size();
	int line = (allNum % 3 == 0) ? (allNum / 3) : (allNum / 3 + 1);

	auto panel_content_no = paneStore->getChildByName("panel_centent_no");
	if (line < 1)
	{
		panel_content_no->setVisible(true);
		m_PTimeLabel = nullptr;
	}
	else
	{
		//黑市刷新时间暂时隐藏
		/*
		auto panel_time = panel_content_1->getChildByName<Widget*>("panel_time");
		auto panel_time_clone = panel_time->clone();
		auto label_time = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_time_clone, "label_time"));
		auto time_title = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_time_clone, "label_title"));
		m_PTimeLabel = label_time;
		time_title->setVisible(true);
		m_PTimeLabel->setVisible(true);
		time_title->setPositionX(time_title->getContentSize().width / 2 + panel_time->getContentSize().width * 0.4);
		m_PTimeLabel->setPositionX(time_title->getContentSize().width + panel_time->getContentSize().width * 0.4);
		listview_sale->pushBackCustomItem(panel_time_clone);
		*/
		panel_content_no->setVisible(false);
	}

	for (int k = 0; k < line; ++k)
	{
		ListView *item_layer_1 = ListView::create();
		item_layer_1->setDirection(ListView::Direction::HORIZONTAL);
		item_layer_1->setItemsMargin(2);
		item_layer_1->setContentSize(Size(846, 330));
		for (int j = 0; j < 3; ++j)
		{
			if (3 * k + j < m_vThingBlack.size())
			{
				Widget*btn_sale;
				btn_sale = (Widget*)imageBlackmarket_sale_1->clone();
				btn_sale->setVisible(true);
				auto image_sale_item = btn_sale->getChildByName<ImageView*>("image_sale_item");
				auto t_group = btn_sale->getChildByName<Text*>("label_amount_1");
				//简介名称
				Text* label_amount = dynamic_cast<Text*>(Helper::seekWidgetByName(btn_sale, "label_amount"));
				auto names = SINGLE_SHOP->getItemData()[m_vThingBlack.at(3 * k + j)->itemid].name;
				label_amount->setString(names);
				auto button_buyprice = btn_sale->getChildByName<Button*>("button_buy_price");
				auto label_v_num = button_buyprice->getChildByName<Text*>("label_v_num");
				auto image_v = button_buyprice->getChildByName<ImageView*>("image_v");
				//数量
				if (m_vThingBlack.at(3 * k + j)->count > 1)
				{
					t_group->enableOutline(Color4B(54, 31, 8, 255), 4);
					t_group->setVisible(true);
					t_group->setString(StringUtils::format("x%d", m_vThingBlack.at(3 * k + j)->count));
				}
				else
				{
					t_group->setVisible(false);
				}
				//图片 详情
				auto info_btn = btn_sale->getChildByName<Button*>("Button_1");
				auto panel_1 = btn_sale->getChildByName<Widget*>("panel_1");
				panel_1->setTag(m_vThingBlack.at(3 * k + j)->itemid);
				panel_1->setSwallowTouches(false);
				panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
				info_btn->setTag(m_vThingBlack.at(3 * k + j)->itemid);
				info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
				//价格
				Text* label_v_numm = dynamic_cast<Text*>(Helper::seekWidgetByName(paneStore, "label_v_num"));
				auto image_v_discount = dynamic_cast<ImageView*>(Helper::seekWidgetByName(paneStore, "image_v"));
				image_v->setVisible(true);

				image_v_discount->setVisible(true);
				label_v_num->setString(numSegment(StringUtils::format("%.0f", (m_vThingBlack.at(3 * k + j)->price) / 100.0)));

				//购买
				for (int index = 0; index < m_pGetItemResult->n_items; index++)
				{
					if (m_pGetItemResult->items[index]->itemid == m_vThingBlack.at(3 * k + j)->itemid)
					{
						button_buyprice->setTag(index);
					}

				}
				button_buyprice->addTouchEventListener(CC_CALLBACK_2(UIStore::goodItem_callfunc, this));
				//护送服务,10002高级护送，10003初级护送
				if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_ESCORT_SENIOR_ID || m_vThingBlack.at(3 * k + j)->itemid == VITICKET_ESCORT_PRIMARY_ID)
				{
					if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_ESCORT_SENIOR_ID)
					{
						label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_SENIOR_ESCORT"]);
						image_sale_item->loadTexture(ESCORT_SENIOR);
					}
					else
					{
						m_button_Priescort = btn_sale;
						label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_PRIMARY_ESCORT"]);
						image_sale_item->loadTexture(ESCORT_PRIMARY);
					}
					//服务服务按钮的特殊处理..........begin
					if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_ESCORT_PRIMARY_ID)
					{
						if (m_bPrimarySerIsCooling)
						{
							button_buyprice->setBright(false);
							label_v_num->setVisible(false);
							image_v->setVisible(false);
						}
						else
						{
							if (SINGLE_HERO->m_iBornCity == SINGLE_HERO->m_iCityID || SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
							{
								button_buyprice->setBright(false);
							}
							else
							{
								button_buyprice->setBright(true);
							}
							label_v_num->setVisible(true);
							image_v->setVisible(true);
						}
						setTextFontSize(label_v_num);
						image_v->ignoreContentAdaptWithSize(false);
						image_v->loadTexture(SINGLE_COIN_PIC);
						if (V_Ui_type != UI_PORT)
						{
							button_buyprice->setBright(false);
							button_buyprice->setTouchEnabled(false);
						}
					}
					else
					{
						if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_ESCORT_SENIOR_ID)
						{
							if (SINGLE_HERO->m_iBornCity == SINGLE_HERO->m_iCityID)
							{
								button_buyprice->setBright(false);
							}
							else
							{
								button_buyprice->setBright(true);
							}
							if (V_Ui_type != UI_PORT)
							{
								button_buyprice->setBright(false);
								button_buyprice->setTouchEnabled(false);
							}
						}
						setTextFontSize(label_v_num);
					}
					//.........end
				}
				//掉落服务, 10004低级掉落，10005高级掉落
				else if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_LOW_DROPS_ID || m_vThingBlack.at(3 * k + j)->itemid == VITICKET_SENIOR_DROPS_ID)
				{
					if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_LOW_DROPS_ID)
					{
						label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKET_LOW_DROPS"]);
						image_sale_item->loadTexture(LOW_DROPS);
					}
					else
					{
						m_button_Priescort = btn_sale;
						label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKE_SENIOR_DROPS"]);
						image_sale_item->loadTexture(SENIOR_DROPS);
					}
				}
				else
				{
					image_sale_item->loadTexture(getItemIconPath(m_vThingBlack.at(3 * k + j)->itemid));
				}
				item_layer_1->pushBackCustomItem(btn_sale);

			}
			else if (3 * k + j >= m_vThingBlack.size() && 3 * k + j<allNum)
			{
				Widget*btn_sale;
				btn_sale = (Widget*)imageBlackmarket_sale_1->clone();
				btn_sale->setVisible(true);

				auto image_sale_item = btn_sale->getChildByName<ImageView*>("image_sale_item");
				//image_sale_item->loadTexture(getItemIconPath(m_vRandomRefreshItems.at(3 * k + j)->itemid));
				auto t_group = btn_sale->getChildByName<Text*>("label_amount_1");
				//简介名称
				Text* label_amount = dynamic_cast<Text*>(Helper::seekWidgetByName(btn_sale, "label_amount"));
				auto button_buyprice = btn_sale->getChildByName<Button*>("button_buy_price");
				auto label_v_num = button_buyprice->getChildByName<Text*>("label_v_num");
				auto image_v = button_buyprice->getChildByName<ImageView*>("image_v");
				//图片 详情
				auto info_btn = btn_sale->getChildByName<Button*>("Button_1");
				auto panel_1 = btn_sale->getChildByName<Widget*>("panel_1");
				//价格
				Text* label_v_numm = dynamic_cast<Text*>(Helper::seekWidgetByName(paneStore, "label_v_num"));
				auto image_v_discount = dynamic_cast<ImageView*>(Helper::seekWidgetByName(paneStore, "image_v"));
				image_v->setVisible(true);
				image_v_discount->setVisible(true);
				
				if (sort == 5)
				{
					image_sale_item->loadTexture(getShipIconPath(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid));
					label_amount->setString(SINGLE_SHOP->getShipData()[m_vRandomRefreshItems.at(3 * k + j)->itemid].name);
					panel_1->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + SHIP_TAG);
					info_btn->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + SHIP_TAG);
				}
				else if(sort == 6)
				{
					image_sale_item->loadTexture(getItemIconPath(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid));
					label_amount->setString(SINGLE_SHOP->getItemData()[m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid].name);
					info_btn->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid +EQUIP_TAG);
					panel_1->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + EQUIP_TAG);
				}
				else if (sort == 8 || sort == 9)
				{
					image_sale_item->loadTexture(getItemIconPath(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid));
					label_amount->setString(SINGLE_SHOP->getItemData()[m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid].name);
					info_btn->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + BLUEPTINTS_TAG);
					panel_1->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + BLUEPTINTS_TAG);
				
				}
				else
				{

					image_sale_item->loadTexture(getItemIconPath(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid));
					label_amount->setString(SINGLE_SHOP->getItemData()[m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid].name);
					info_btn->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid);
					panel_1->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid);
				}

				panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
				info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
				int id = m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid;
				int price = m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->price;
				label_v_num->setString(numSegment(StringUtils::format("%d", m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->price)));
				setTextFontSize(label_v_num);

				for (int index = 0; index < m_vRandomItems.size(); index++)
				{
					if (id == m_vRandomItems.at(index)->itemid)
					{
						button_buyprice->setTag(index);
					}
				}
				button_buyprice->addTouchEventListener(CC_CALLBACK_2(UIStore::randomGoodsItem_callfunc, this));
				item_layer_1->pushBackCustomItem(btn_sale);
			}
			
		}
		listview_sale->pushBackCustomItem(item_layer_1);
	}
#endif
}
void UIStore::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
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
		button_pulldown->setPositionY(len * factor + allowHeight);
	}
}
void UIStore::tableCellTouched(TableView* table, TableViewCell* cell)
{
}
Size UIStore::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	auto view = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto listView_for_table = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listView_for_table"));
	auto contensize = listView_for_table->getContentSize();
	return contensize;
	//return Size(846,330);
}
TableViewCell * UIStore::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell *cell = table->dequeueCell();
	ListView* cell_item;
	
	cell = new TableViewCell();
	cell->autorelease();
	if (m_pGetItemResult&&m_pGetItemResult->type == VITICKET_TYPE_V_PROP)
	{
		auto view = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
		auto listView_for_table = dynamic_cast<ListView*>(Helper::seekWidgetByName(view, "listView_for_table"));
		listView_for_table->setDirection(ListView::Direction::HORIZONTAL);
		listView_for_table->setItemsMargin(2);
		auto imageBlackmarket_sale_1 = view->getChildByName<Widget*>("image_blackmarket_sale_1");
		cell_item = dynamic_cast<ListView*>( listView_for_table->clone());

		cell->addChild(cell_item);
		cell_item->setPosition(Vec2(0, 0));
		cell_item->setCameraMask(_cameraMask);
		cell->setVisible(true);
		cell->setTag(idx);

		cell_item->setTouchEnabled(false);
		cell_item->setVisible(true);
		cell_item->setSwallowTouches(false);
		//每三个组成一条
	
		switch (m_tableview_tag)
		{
		case UIStore::TABLEVIEW_NONE:
		break;
		case UIStore::TABLEVIEW_FOR_ALL:
		{
										   for (int j = 0; j < 3; ++j)
										   {
											   Widget*btn_sale;
											   btn_sale = (Widget*)imageBlackmarket_sale_1->clone();
											   btn_sale->setVisible(true);
											   auto image_sale_item = btn_sale->getChildByName<ImageView*>("image_sale_item");
											   auto label_amount = btn_sale->getChildByName<Text*>("label_amount");
											   auto t_group = btn_sale->getChildByName<Text*>("label_amount_1");


											   auto button_buyprice = btn_sale->getChildByName<Button*>("button_buy_price");
											   auto label_v_num = button_buyprice->getChildByName<Text*>("label_v_num");
											   auto image_v = button_buyprice->getChildByName<ImageView*>("image_v");
											   image_sale_item->ignoreContentAdaptWithSize(false);

											   auto info_btn = btn_sale->getChildByName<Button*>("Button_1");
											   auto panel_1 = btn_sale->getChildByName<Widget*>("panel_1");
											   //常驻物品
											   if (3 * idx + j < m_pGetItemResult->n_items)
											   {
												   panel_1->setTag(m_pGetItemResult->items[3 * idx + j]->itemid);
												   panel_1->setSwallowTouches(false);
												   panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
												   info_btn->setTag(m_pGetItemResult->items[3 * idx + j]->itemid);
												   info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
												   //护送服务,10002高级护送，10003初级护送
												   if (m_pGetItemResult->items[3 * idx + j]->itemid == VITICKET_ESCORT_SENIOR_ID || m_pGetItemResult->items[3 * idx + j]->itemid == VITICKET_ESCORT_PRIMARY_ID)
												   {
													   if (m_pGetItemResult->items[3 * idx + j]->itemid == VITICKET_ESCORT_SENIOR_ID)
													   {
														   label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_SENIOR_ESCORT"]);
														   image_sale_item->loadTexture(ESCORT_SENIOR);
													   }
													   else
													   {
														   m_button_Priescort = btn_sale;
														   label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_PRIMARY_ESCORT"]);
														   image_sale_item->loadTexture(ESCORT_PRIMARY);
													   }
												   }
												   //掉落服务, 10004低级掉落，10005高级掉落
												   else if (m_pGetItemResult->items[3 * idx + j]->itemid == VITICKET_LOW_DROPS_ID || m_pGetItemResult->items[3 * idx + j]->itemid == VITICKET_SENIOR_DROPS_ID)
												   {
													   if (m_pGetItemResult->items[3 * idx + j]->itemid == VITICKET_LOW_DROPS_ID)
													   {
														   label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKET_LOW_DROPS"]);
														   image_sale_item->loadTexture(LOW_DROPS);
													   }
													   else
													   {
														   m_button_Priescort = btn_sale;
														   label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKE_SENIOR_DROPS"]);
														   image_sale_item->loadTexture(SENIOR_DROPS);
													   }
												   }
												   else
												   {
													   label_amount->setString(SINGLE_SHOP->getItemData()[m_pGetItemResult->items[3 * idx + j]->itemid].name);
													   image_sale_item->loadTexture(getItemIconPath(m_pGetItemResult->items[3 * idx + j]->itemid));
												   }
												   if (m_pGetItemResult->items[3 * idx + j]->count > 1)
												   {
													   t_group->enableOutline(Color4B(54, 31, 8, 255), 4);
													   t_group->setVisible(true);
													   t_group->setString(StringUtils::format("x%d", m_pGetItemResult->items[3 * idx + j]->count));
												   }
												   else
												   {
													   t_group->setVisible(false);
												   }

												   //tag
												   button_buyprice->setTag(3 * idx + j);
												   button_buyprice->addTouchEventListener(CC_CALLBACK_2(UIStore::goodItem_callfunc, this));

												   //价格显示
												   if (m_pGetItemResult->items[3 * idx + j]->itemid == VITICKET_ESCORT_PRIMARY_ID)
												   {
													   if (m_bPrimarySerIsCooling)
													   {
														   button_buyprice->setBright(false);
														   label_v_num->setVisible(false);
														   image_v->setVisible(false);
													   }
													   else
													   {
														   if (SINGLE_HERO->m_iBornCity == SINGLE_HERO->m_iCityID || SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
														   {
															   button_buyprice->setBright(false);
														   }
														   else
														   {
															   button_buyprice->setBright(true);
														   }
														   label_v_num->setVisible(true);
														   image_v->setVisible(true);
													   }
													   label_v_num->setString(numSegment(StringUtils::format("%.0f", (m_pGetItemResult->items[3 * idx + j]->price) / 100.0)));
													   setTextFontSize(label_v_num);
													   image_v->ignoreContentAdaptWithSize(false);
													   image_v->loadTexture(SINGLE_COIN_PIC);
													   //暂时隐藏
													   if (V_Ui_type != UI_PORT)
													   {
														   button_buyprice->setBright(false);
														   button_buyprice->setTouchEnabled(false);
													   }
												   }
												   else
												   {
													   if (m_pGetItemResult->items[3 * idx + j]->itemid == VITICKET_ESCORT_SENIOR_ID)
													   {
														   if (SINGLE_HERO->m_iBornCity == SINGLE_HERO->m_iCityID)
														   {
															   button_buyprice->setBright(false);
														   }
														   else
														   {
															   button_buyprice->setBright(true);
														   }
														   //暂时隐藏
														   if (V_Ui_type != UI_PORT)
														   {
															   button_buyprice->setBright(false);
															   button_buyprice->setTouchEnabled(false);
														   }
													   }
													   label_v_num->setString(numSegment(StringUtils::format("%.0f", (m_pGetItemResult->items[3 * idx + j]->price) / 100.0)));
													   setTextFontSize(label_v_num);
												   }
												   cell_item->pushBackCustomItem(btn_sale);
											   }
											   else if (3 * idx + j >= m_pGetItemResult->n_items && 3 * idx + j < m_pGetItemResult->n_items + m_vRandomItems.size())
											   {
												   int i = 3 * idx + j - m_pGetItemResult->n_items;
												   if (i < m_nRandomShipNum)
												   {
													   image_sale_item->loadTexture(getShipIconPath(m_vRandomItems[i]->itemid));
													   label_amount->setString(SINGLE_SHOP->getShipData()[m_vRandomItems[i]->itemid].name);
													   panel_1->setTag(m_vRandomItems[i]->itemid + SHIP_TAG);
													   panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
													   info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
													   info_btn->setTag(m_vRandomItems[i]->itemid + SHIP_TAG);
												   }
												   else{
													   image_sale_item->loadTexture(getItemIconPath(m_vRandomItems[i]->itemid));
													   label_amount->setString(SINGLE_SHOP->getItemData()[m_vRandomItems[i]->itemid].name);

													   panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
													   info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
													   if (i >= m_nRandomShipNum + m_nRandomEquipNum)
													   {
														   //分类没有规则 现在临时处理
														   if (m_vRandomItems[i]->sort == 8 || m_vRandomItems[i]->sort == 9)
														   {
															   info_btn->setTag(m_vRandomItems[i]->itemid + BLUEPTINTS_TAG);
															   panel_1->setTag(m_vRandomItems[i]->itemid + BLUEPTINTS_TAG);
														   }
														   else
														   {
															   info_btn->setTag(m_vRandomItems[i]->itemid);
															   panel_1->setTag(m_vRandomItems[i]->itemid);
														   }
													   }
													   else
													   {
														   //装备
														   info_btn->setTag(m_vRandomItems[i]->itemid + EQUIP_TAG);
														   panel_1->setTag(m_vRandomItems[i]->itemid + EQUIP_TAG);
													   }

												   }
												   int id = m_vRandomItems[i]->itemid;
												   int price = m_vRandomItems[i]->price;
												   label_v_num->setString(numSegment(StringUtils::format("%d", m_vRandomItems[i]->price)));
												   setTextFontSize(label_v_num);
												   button_buyprice->setTag(i);
												   button_buyprice->addTouchEventListener(CC_CALLBACK_2(UIStore::randomGoodsItem_callfunc, this));
                                                   cell_item->pushBackCustomItem(btn_sale);
											   }
											   
										   }
										   break;
		}
		case UIStore::TABLEVIEW_FOR_CATEGORY:
		{
												for (int j = 0; j < 3; ++j)
												{
													int k = idx;
													if (3 * k + j < m_vThingBlack.size())
													{
														Widget*btn_sale;
														btn_sale = (Widget*)imageBlackmarket_sale_1->clone();
														btn_sale->setVisible(true);
														auto image_sale_item = btn_sale->getChildByName<ImageView*>("image_sale_item");
														auto t_group = btn_sale->getChildByName<Text*>("label_amount_1");
														//简介名称
														Text* label_amount = dynamic_cast<Text*>(Helper::seekWidgetByName(btn_sale, "label_amount"));
														auto names = SINGLE_SHOP->getItemData()[m_vThingBlack.at(3 * k + j)->itemid].name;
														label_amount->setString(names);
														auto button_buyprice = btn_sale->getChildByName<Button*>("button_buy_price");
														auto label_v_num = button_buyprice->getChildByName<Text*>("label_v_num");
														auto image_v = button_buyprice->getChildByName<ImageView*>("image_v");
														//数量
														if (m_vThingBlack.at(3 * k + j)->count > 1)
														{
															t_group->enableOutline(Color4B(54, 31, 8, 255), 4);
															t_group->setVisible(true);
															t_group->setString(StringUtils::format("x%d", m_vThingBlack.at(3 * k + j)->count));
														}
														else
														{
															t_group->setVisible(false);
														}
														//图片 详情
														auto info_btn = btn_sale->getChildByName<Button*>("Button_1");
														auto panel_1 = btn_sale->getChildByName<Widget*>("panel_1");
														panel_1->setTag(m_vThingBlack.at(3 * k + j)->itemid);
														panel_1->setSwallowTouches(false);
														panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
														info_btn->setTag(m_vThingBlack.at(3 * k + j)->itemid);
														info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
														//价格
														Text* label_v_numm = dynamic_cast<Text*>(Helper::seekWidgetByName(btn_sale, "label_v_num"));
														auto image_v_discount = dynamic_cast<ImageView*>(Helper::seekWidgetByName(btn_sale, "image_v"));
														image_v->setVisible(true);

														image_v_discount->setVisible(true);
														label_v_num->setString(numSegment(StringUtils::format("%.0f", (m_vThingBlack.at(3 * k + j)->price) / 100.0)));

														//购买
														for (int index = 0; index < m_pGetItemResult->n_items; index++)
														{
															if (m_pGetItemResult->items[index]->itemid == m_vThingBlack.at(3 * k + j)->itemid)
															{
																button_buyprice->setTag(index);
															}

														}
														button_buyprice->addTouchEventListener(CC_CALLBACK_2(UIStore::goodItem_callfunc, this));
														//护送服务,10002高级护送，10003初级护送
														if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_ESCORT_SENIOR_ID || m_vThingBlack.at(3 * k + j)->itemid == VITICKET_ESCORT_PRIMARY_ID)
														{
															if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_ESCORT_SENIOR_ID)
															{
																label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_SENIOR_ESCORT"]);
																image_sale_item->loadTexture(ESCORT_SENIOR);
															}
															else
															{
																m_button_Priescort = btn_sale;
																label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VTICKETSHOP_BUY_PRIMARY_ESCORT"]);
																image_sale_item->loadTexture(ESCORT_PRIMARY);
															}
															//服务服务按钮的特殊处理..........begin
															if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_ESCORT_PRIMARY_ID)
															{
																if (m_bPrimarySerIsCooling)
																{
																	button_buyprice->setBright(false);
																	label_v_num->setVisible(false);
																	image_v->setVisible(false);
																}
																else
																{
																	if (SINGLE_HERO->m_iBornCity == SINGLE_HERO->m_iCityID || SINGLE_SHOP->getCitiesInfo()[SINGLE_HERO->m_iCityID].port_type == 5)
																	{
																		button_buyprice->setBright(false);
																	}
																	else
																	{
																		button_buyprice->setBright(true);
																	}
																	label_v_num->setVisible(true);
																	image_v->setVisible(true);
																}
																setTextFontSize(label_v_num);
																image_v->ignoreContentAdaptWithSize(false);
																image_v->loadTexture(SINGLE_COIN_PIC);
																if (V_Ui_type != UI_PORT)
																{
																	button_buyprice->setBright(false);
																	button_buyprice->setTouchEnabled(false);
																}
															}
															else
															{
																if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_ESCORT_SENIOR_ID)
																{
																	if (SINGLE_HERO->m_iBornCity == SINGLE_HERO->m_iCityID)
																	{
																		button_buyprice->setBright(false);
																	}
																	else
																	{
																		button_buyprice->setBright(true);
																	}
																	if (V_Ui_type != UI_PORT)
																	{
																		button_buyprice->setBright(false);
																		button_buyprice->setTouchEnabled(false);
																	}
																}
																setTextFontSize(label_v_num);
															}
															//.........end
														}
														//掉落服务, 10004低级掉落，10005高级掉落
														else if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_LOW_DROPS_ID || m_vThingBlack.at(3 * k + j)->itemid == VITICKET_SENIOR_DROPS_ID)
														{
															if (m_vThingBlack.at(3 * k + j)->itemid == VITICKET_LOW_DROPS_ID)
															{
																label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKET_LOW_DROPS"]);
																image_sale_item->loadTexture(LOW_DROPS);
															}
															else
															{
																m_button_Priescort = btn_sale;
																label_amount->setString(SINGLE_SHOP->getTipsInfo()["TIP_VITICKE_SENIOR_DROPS"]);
																image_sale_item->loadTexture(SENIOR_DROPS);
															}
														}
														else
														{
															image_sale_item->loadTexture(getItemIconPath(m_vThingBlack.at(3 * k + j)->itemid));
														}
														cell_item->pushBackCustomItem(btn_sale);

													}
													else if (3 * k + j >= m_vThingBlack.size() && 3 * k + j < m_vRandomRefreshItems.size() + m_vThingBlack.size())
													{
														Widget*btn_sale;
														btn_sale = (Widget*)imageBlackmarket_sale_1->clone();
														btn_sale->setVisible(true);

														auto image_sale_item = btn_sale->getChildByName<ImageView*>("image_sale_item");
														//image_sale_item->loadTexture(getItemIconPath(m_vRandomRefreshItems.at(3 * k + j)->itemid));
														auto t_group = btn_sale->getChildByName<Text*>("label_amount_1");
														//简介名称
														Text* label_amount = dynamic_cast<Text*>(Helper::seekWidgetByName(btn_sale, "label_amount"));
														auto button_buyprice = btn_sale->getChildByName<Button*>("button_buy_price");
														auto label_v_num = button_buyprice->getChildByName<Text*>("label_v_num");
														auto image_v = button_buyprice->getChildByName<ImageView*>("image_v");
														//图片 详情
														auto info_btn = btn_sale->getChildByName<Button*>("Button_1");
														auto panel_1 = btn_sale->getChildByName<Widget*>("panel_1");
														//价格
														Text* label_v_numm = dynamic_cast<Text*>(Helper::seekWidgetByName(btn_sale, "label_v_num"));
														auto image_v_discount = dynamic_cast<ImageView*>(Helper::seekWidgetByName(btn_sale, "image_v"));
														image_v->setVisible(true);
														image_v_discount->setVisible(true);

														if (m_nBlackMarketSortTag == 5)
														{
															image_sale_item->loadTexture(getShipIconPath(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid));
															label_amount->setString(SINGLE_SHOP->getShipData()[m_vRandomRefreshItems.at(3 * k + j)->itemid].name);
															panel_1->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + SHIP_TAG);
															info_btn->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + SHIP_TAG);
														}
														else if (m_nBlackMarketSortTag == 6)
														{
															image_sale_item->loadTexture(getItemIconPath(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid));
															label_amount->setString(SINGLE_SHOP->getItemData()[m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid].name);
															info_btn->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + EQUIP_TAG);
															panel_1->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + EQUIP_TAG);
														}
														else if (m_nBlackMarketSortTag == 8 || m_nBlackMarketSortTag == 9)
														{
															image_sale_item->loadTexture(getItemIconPath(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid));
															label_amount->setString(SINGLE_SHOP->getItemData()[m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid].name);
															info_btn->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + BLUEPTINTS_TAG);
															panel_1->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid + BLUEPTINTS_TAG);

														}
														else
														{

															image_sale_item->loadTexture(getItemIconPath(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid));
															label_amount->setString(SINGLE_SHOP->getItemData()[m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid].name);
															info_btn->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid);
															panel_1->setTag(m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid);
														}

														panel_1->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
														info_btn->addTouchEventListener(CC_CALLBACK_2(UIStore::propItem_callfunc, this));
														int id = m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->itemid;
														int price = m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->price;
														label_v_num->setString(numSegment(StringUtils::format("%d", m_vRandomRefreshItems.at(3 * k + j - m_vThingBlack.size())->price)));
														setTextFontSize(label_v_num);

														for (int index = 0; index < m_vRandomItems.size(); index++)
														{
															if (id == m_vRandomItems.at(index)->itemid)
															{
																button_buyprice->setTag(index);
															}
														}
														button_buyprice->addTouchEventListener(CC_CALLBACK_2(UIStore::randomGoodsItem_callfunc, this));
														cell_item->pushBackCustomItem(btn_sale);
													}

												}
											break;
		}
	
		default:
		break;
		}
		
	}
	return cell;
}
ssize_t UIStore::numberOfCellsInTableView(TableView *table)
{
	int lines = 0;
	switch (m_tableview_tag)
	{
	case UIStore::TABLEVIEW_NONE:
	break;
	case UIStore::TABLEVIEW_FOR_ALL:
	{
		
			if (m_pGetItemResult)
			{
				int allBlackItemNums = m_pGetItemResult->n_items + m_vRandomItems.size();
				lines = (allBlackItemNums % 3 == 0) ? (allBlackItemNums / 3) : (allBlackItemNums / 3 + 1);
			}
             break;
	}
	
	case UIStore::TABLEVIEW_FOR_CATEGORY:
	{
			int allBlackItemNums = m_vThingBlack.size() + m_vRandomRefreshItems.size();
			lines = (allBlackItemNums % 3 == 0) ? (allBlackItemNums / 3) : (allBlackItemNums / 3 + 1);
			break;
	}
	
	default:
	break;
	}
	return lines;
}
void UIStore::tableViewJustForBlackAllItem()
{
	m_tableview_tag = TABLEVIEW_FOR_ALL;
	auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto panel_content_1 = paneStore->getChildByName("panel_content_1");
	auto listview_sale = panel_content_1->getChildByName<ListView*>("listview_sale");
	listview_sale->removeAllChildrenWithCleanup(true);
	listview_sale->setVisible(false);
	auto image_pulldown = panel_content_1->getChildByName<ImageView*>("image_pulldown");
    auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");

	TableView* mTableViewCatergory = dynamic_cast<TableView*>(panel_content_1->getChildByTag(10241 + TABLEVIEW_FOR_CATEGORY));
	if (mTableViewCatergory){
		mTableViewCatergory->setVisible(false);
	}
	//添加tableView
	TableView* mTableView = dynamic_cast<TableView*>(panel_content_1->getChildByTag(10241+TABLEVIEW_FOR_ALL));
	if (!mTableView){
		mTableView = TableView::create(this, Size(listview_sale->getContentSize().width, listview_sale->getContentSize().height));
		mTableView = TableView::create(this, Size(846,486));
		panel_content_1->addChild(mTableView);
		mTableView->reloadData();
	}
	mTableView->setTag(10241+TABLEVIEW_FOR_ALL);
	mTableView->setAnchorPoint(listview_sale->getAnchorPoint());
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(listview_sale->getPosition());
	mTableView->setVisible(true);
	mTableView->setTouchEnabled(true);
	mTableView->setUserObject(button_pulldown);

	mTableView->setBounceable(true);
	mTableView->setClippingToBounds(true);
	this->setCameraMask(_cameraMask, true);
	
}
 void UIStore::tableViewForcategory()
{
	 m_tableview_tag = TABLEVIEW_FOR_CATEGORY;
	auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	auto panel_content_1 = paneStore->getChildByName("panel_content_1");
	auto listview_sale = panel_content_1->getChildByName<ListView*>("listview_sale");
	listview_sale->removeAllChildrenWithCleanup(true);
	listview_sale->setVisible(false);
	auto image_pulldown = panel_content_1->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");

	TableView* mTableViewAll = dynamic_cast<TableView*>(panel_content_1->getChildByTag(10241 + TABLEVIEW_FOR_ALL));
	if (mTableViewAll){
		mTableViewAll->setVisible(false);
	}
	//添加tableView
	TableView* mTableView = dynamic_cast<TableView*>(panel_content_1->getChildByTag(10241 + TABLEVIEW_FOR_CATEGORY));
	if (!mTableView){
		mTableView = TableView::create(this, Size(listview_sale->getContentSize().width, listview_sale->getContentSize().height));
		panel_content_1->addChild(mTableView);
	}
	mTableView->setTag(10241 + TABLEVIEW_FOR_CATEGORY);
	mTableView->setAnchorPoint(Vec2(0, 0));
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(listview_sale->getPosition());
	mTableView->setVisible(true);
	mTableView->setTouchEnabled(true);
	mTableView->setUserObject(button_pulldown);
	
	mTableView->setBounceable(true);
	mTableView->setClippingToBounds(true);
	if (m_nBlackStoreChange!=m_nBlackMarketSortTag)
	{
		m_nBlackStoreChange = m_nBlackMarketSortTag;
		mTableView->reloadData();
	}
	
	this->setCameraMask(_cameraMask, true);
}
 void UIStore::removeAllTableViews()
 {
	 auto paneStore = getViewRoot(STORE_RES[VIEW_STORE_MAIN_PANEL]);
	 auto panel_content_1 = paneStore->getChildByName("panel_content_1");
	 auto listview_sale = panel_content_1->getChildByName<ListView*>("listview_sale");
	 listview_sale->removeAllChildrenWithCleanup(true);
	 listview_sale->setVisible(true);
	 TableView* mTableViewAll = dynamic_cast<TableView*>(panel_content_1->getChildByTag(10241 + TABLEVIEW_FOR_ALL));
	 if (mTableViewAll){
		 mTableViewAll->removeFromParentAndCleanup(true);
		 mTableViewAll = nullptr;
	 }
	 TableView* mTableViewCatergory = dynamic_cast<TableView*>(panel_content_1->getChildByTag(10241 + TABLEVIEW_FOR_CATEGORY));
	 if (mTableViewCatergory){
		 mTableViewCatergory->removeFromParentAndCleanup(true);
		 mTableViewCatergory = nullptr;
	 }
 }