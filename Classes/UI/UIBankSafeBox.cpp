#include "UIBankSafeBox.h"

UIBankSafeBox::UIBankSafeBox():
	m_Callback(nullptr),
	m_pSellResult(nullptr)
{
	m_iGoodsId = 0;
	m_nPersonalBank = 0;
}

UIBankSafeBox::~UIBankSafeBox()
{
	SpriteFrameCache::getInstance()->purgeSharedSpriteFrameCache();
	if (m_pSellResult)
	{
		get_items_to_sell_result__free_unpacked(m_pSellResult,0);
	}
}

void UIBankSafeBox::onEnter()
{
	UIBasicLayer::onEnter();// final
}
void UIBankSafeBox::onExit()
{
	UIBasicLayer::onExit();
}

UIBankSafeBox* UIBankSafeBox::createWarehouseLayer(Widget *vRoot,bool isPersonal)
{
	UIBankSafeBox* whl = new UIBankSafeBox;
	if (isPersonal){
		whl->m_nPersonalBank = 0;
	}else{
		whl->m_nPersonalBank = 0;
	}
	if (whl && whl->init(vRoot))
	{
		whl->autorelease();
		return whl;
	}
	CC_SAFE_DELETE(whl);
	return nullptr;
}


bool UIBankSafeBox::init(Widget *vRoot)
{
	bool pRet = false;
	do 
	{
		UIBasicLayer::init();
	
		auto p_deposit = vRoot->getChildByName<Widget*>("panel_select_item_sell");
		auto panelBuy=p_deposit->getChildByName<Widget*>("panel_buy");
		auto twoBuf = panelBuy->getChildByName<Widget*>("panel_two_butter");
		auto listview_two_butter=twoBuf->getChildByName<Widget*>("listview_two_butter");
		dynamic_cast<ListView*>(listview_two_butter)->setDirection(cocos2d::ui::ScrollView::Direction::NONE);

		auto b_shipEq = listview_two_butter->getChildByName<Button*>("button_goods");//船装备
		auto b_human_equ = listview_two_butter->getChildByName<Button*>("button_ships");//人装备
		auto b_draw = listview_two_butter->getChildByName<Button*>("button_ship_equipment");//图纸
		auto b_miscs  = listview_two_butter->getChildByName<Button*>("button_human_equipment");//道具
		b_shipEq->setBright(false);

		//文本
		auto label_shipequipment=b_shipEq->getChildByName<Text*>("label_goods");	
		auto label_human_equipment=b_human_equ->getChildByName<Text*>("label_ships");
		auto label_drawings=b_draw->getChildByName<Text*>("label_shipequipment");
		auto label_miscs=b_miscs->getChildByName<Text*>("label_human_equipment");
	
		label_shipequipment->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_CSB_DEPOSIT_ITEM_SHIP_EQUIP_TITLE"]);
		label_human_equipment->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_CSB_DEPOSIT_ITEM_HUMAN_EQUIP_TITLE"]);
		label_drawings->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_CSB_DEPOSIT_ITEM_DRAW_TITLE"]);
		label_miscs->setString(SINGLE_SHOP->getTipsInfo()["TIP_BANK_CSB_DEPOSIT_ITEM_MISC_TITLE"]);

		//删除用不到
	    auto  b_special = listview_two_butter->getChildByName<Button*>("button_special");
		auto button_drawings = listview_two_butter->getChildByName<Button*>("button_drawings");
		auto button_miscs = listview_two_butter->getChildByName<Button*>("button_miscs");
		auto b_Bg_8= listview_two_butter->getChildByName<ImageView*>("image_div_butter_8");
		auto b_Bg_6= listview_two_butter->getChildByName<ImageView*>("image_div_butter_6");
		auto b_Bg_7= listview_two_butter->getChildByName<ImageView*>("image_div_butter_7");
		
		b_special->removeFromParent();
		button_drawings->removeFromParent();
		button_miscs->removeFromParent();
		b_Bg_6->removeFromParent();
		b_Bg_7->removeFromParent();
		b_Bg_8->removeFromParent();
		pRet = true;
	} while (0);
	return pRet;
}

void UIBankSafeBox::getFirstResult()
{
	
	std::string name("button_goods");//开始显示船装备
	auto listview_two_butter =dynamic_cast<Widget*>( Helper::seekWidgetByName(m_vRoot, "listview_two_butter"));
	auto b_shipEq = listview_two_butter->getChildByName<Button*>("button_goods");
	auto label_shipequipment = b_shipEq->getChildByName<Text*>("label_goods");
	label_shipequipment->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
	b_shipEq->setBright(false);
	Layer *layer = dynamic_cast<Layer*>(m_vRoot->getParent()->getParent());
	ProtocolThread::GetInstance()->getItemsToSell(2, m_nPersonalBank, UILoadingIndicator::create(layer));
	//callEventByName(name,nullptr);
}
void UIBankSafeBox::setCallBack(std::function<void ()> callback)
{
	m_Callback = callback;
}
void UIBankSafeBox::menuCall_func(Ref *pSender,Widget::TouchEventType touchType)
{
	if (Widget::TouchEventType::ENDED != touchType)
	{
		return;
	}
	Widget* widget = dynamic_cast<Widget*>(pSender);
	std::string name(widget->getName());
	
 	if (isButton(button_last_widget))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		cocos2d::NotificationCenter::getInstance()->postNotification("button_backSafe_notify",this);
		return;
	}
	callEventByName(name,widget);
}

void UIBankSafeBox::buttonClikEvent(Ref *pSender,Widget::TouchEventType touchType)
{
	if (Widget::TouchEventType::ENDED != touchType)
	{
		return;
	}
	Widget *btn = dynamic_cast<Widget*>(pSender);
	auto name = btn->getName();
	m_iGoodsId = btn->getTag() ;
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	if (m_Callback)
	{
		m_Callback();
	}
}
ToSellItemsDefine* UIBankSafeBox::getSeletedItemInfo()
{
	if (!m_pSellResult)
	{
		return nullptr;
	}
	return m_pSellResult->items[m_iGoodsId];
}

void UIBankSafeBox::callEventByName(std::string& name,Widget* node)
{
	auto panelBuy=Helper::seekWidgetByName(m_vRoot,"panel_buy");
	auto twoBuf=Helper::seekWidgetByName(panelBuy,"panel_two_butter");
	auto listview_two_butter=Helper::seekWidgetByName(twoBuf,"listview_two_butter");

	auto b_shipEq = listview_two_butter->getChildByName<Button*>("button_goods");//船装备(公共UI导致的名字错乱)
	auto b_human_equ = listview_two_butter->getChildByName<Button*>("button_ships");//人装备
	auto b_draw = listview_two_butter->getChildByName<Button*>("button_ship_equipment");//图纸
	auto b_miscs  = listview_two_butter->getChildByName<Button*>("button_human_equipment");//道具
	auto  b_special = listview_two_butter->getChildByName<Button*>("button_special");


	auto label_shipequipment=b_shipEq->getChildByName<Text*>("label_goods");

	auto label_human_equipment=b_human_equ->getChildByName<Text*>("label_ships");
	auto label_drawings=b_draw->getChildByName<Text*>("label_shipequipment");
	auto label_miscs=b_miscs->getChildByName<Text*>("label_human_equipment");
	node = Helper::seekWidgetByName(listview_two_butter,name.c_str());

	Layer *layer = dynamic_cast<Layer*>(m_vRoot->getParent()->getParent());
	//sound effect
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	int itemType=0;
	if (name.compare("button_goods")==0)
	{
		itemType = ITEM_TYPE_SHIP_EQUIP;//船装备
		b_shipEq->setBright(false);
		b_human_equ->setBright(true);
		b_draw->setBright(true);
		b_miscs->setBright(true);
		label_shipequipment->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
		label_human_equipment->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		label_drawings->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		label_miscs->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}else if (name.compare("button_ships")==0)
	{
		itemType = ITEM_TYPE_ROLE_EQUIP;//人装备
		b_shipEq->setBright(true);
		b_human_equ->setBright(false);
		b_draw->setBright(true);
		b_miscs->setBright(true);
		label_shipequipment->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		label_human_equipment->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
		label_drawings->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		label_miscs->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}else if(name.compare("button_ship_equipment")==0)
	{
		itemType = ITEM_TYPE_DRAWING;//图纸
		b_shipEq->setBright(true);
		b_human_equ->setBright(true);
		b_draw->setBright(false);
		b_miscs->setBright(true);
		label_shipequipment->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		label_human_equipment->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		label_drawings->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
		label_miscs->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}else if (name.compare("button_human_equipment")==0)
	{
		itemType = ITEM_TYPE_PORP;//道具
		b_shipEq->setBright(true);
		b_human_equ->setBright(true);
		b_draw->setBright(true);
		b_miscs->setBright(false);
		label_shipequipment->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		label_human_equipment->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		label_drawings->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
		label_miscs->setTextColor(LEFT_BUTTON_TEXT_COLOR_PASSED);
	}
	else
	{
		return;
	}
       ProtocolThread::GetInstance()->getItemsToSell(itemType,m_nPersonalBank,UILoadingIndicator::create(layer));
}
void	UIBankSafeBox::onServerEvent(struct ProtobufCMessage *message,int msgType)
{
	
}
void UIBankSafeBox::updateView(GetItemsToSellResult* result,bool isFail /* = false */)
{
 	if(m_pSellResult)
 	{
 		get_items_to_sell_result__free_unpacked(m_pSellResult,0);
 	}
	m_pSellResult = result;
	updateViewSell(result,isFail);
}

void UIBankSafeBox::updateViewSell(const GetItemsToSellResult *result,bool isFail)
{
	auto lvParent = m_vRoot->getChildByName<Layout*>("panel_buy");
	ListView* lv=dynamic_cast<ListView*>(Helper::seekWidgetByName(lvParent,"listview_item"));
	Widget* buttonitem = dynamic_cast<Widget*>(Helper::seekWidgetByName(lvParent, "panel_itemdetails"));
	if (!buttonitem){ return; }
	
	lv->removeAllChildrenWithCleanup(true);
	if (isFail){ return; };
	
	for (int i = 0; i <result->n_items; ++i)
	{		
		std::string itemName;
		std::string itemPath;
		getItemNamePath(result->itemtype,result->items[i]->itemid,itemName,itemPath);
		Widget*goodBtn=(Widget*)buttonitem->clone();
		auto  buttomBg=(Button*)goodBtn->getChildByName("button_good_bg");
		goodBtn->addTouchEventListener(CC_CALLBACK_2(UIBankSafeBox::buttonClikEvent,this));
		goodBtn->setTag(i);
		auto imageBg = buttomBg->getChildByName<ImageView*>("image_goods");
		imageBg->ignoreContentAdaptWithSize(false);
		imageBg->loadTexture(itemPath);
		auto labelName=goodBtn->getChildByName<Text*>("label_goods_name");
		auto labelNum=goodBtn->getChildByName<Text*>("label_goods_num");
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
		labelName->setString(apostrophe(itemName, nMaxStringNum));
		labelNum->setString(String::createWithFormat("x %lld",result->items[i]->amount)->_string);
		if (result->itemtype == ITEM_TYPE_SHIP_EQUIP || result->itemtype == ITEM_TYPE_ROLE_EQUIP)
		{
			if (result->items[i]->optionalitemnum > 0)
			{
				addStrengtheningIcon(buttomBg);
			}
			if (result->items[i]->currentdurability <= SHIP_EQUIP_BROKEN ||result->items[i]->currentdurability <= result->items[i]->maxdurability *SHIP_EQUIP_BROKEN_PERCENT)
			{
				addequipBrokenIcon(buttomBg);
				setBrokenEquipRed(imageBg);
			}
		}
		lv->pushBackCustomItem(goodBtn);

		setBgButtonFormIdAndType(buttomBg, result->items[i]->itemid, result->itemtype);
		setTextColorFormIdAndType(labelName, result->items[i]->itemid, result->itemtype);
	}	
	auto image_pulldown = lvParent->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width/2 - button_pulldown->getBoundingBox().size.width/2);
	addListViewBar(lv,image_pulldown);
}

void UIBankSafeBox::setRootWidget(Widget *root)
{
	m_vRoot = root;
	addEventForChildren(m_vRoot);
}

void UIBankSafeBox::getItemNamePath(int itemtype,int itemid,std::string& name,std::string& path)
{
	if (itemtype == ITEM_TYPE_GOOD)
	{
		name = getGoodsName(itemid);
		path = getGoodsIconPath(itemid,IMAGE_ICON_OUTLINE);
	}else if(itemtype == ITEM_TYPE_SHIP)
	{
		name = getShipName(itemid);
		path = getShipIconPath(itemid,IMAGE_ICON_OUTLINE);
	}else
	{
		name = getItemName(itemid);
		path = getItemIconPath(itemid,IMAGE_ICON_OUTLINE);
	}
}

int UIBankSafeBox::getGoodsType()
{
	return m_pSellResult->itemtype;
}
