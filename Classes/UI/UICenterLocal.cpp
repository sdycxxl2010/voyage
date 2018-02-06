#include "UICenterLocal.h"
#include "UICommon.h"
#include "UIInform.h"
#include "UIVoyageSlider.h"

#include "Utils.h"

UICenterLocal::UICenterLocal():
	m_pParent(nullptr),
	m_pMinorButton(nullptr),
	m_isFirst(true),
	m_eOperationIndex(LOCAL_INDEX_DROP),
	m_Result(nullptr),
	m_nItemIndex(-1)
{

}

UICenterLocal::~UICenterLocal()
{
	if (m_Result)
	{
		get_warehouse_items_result__free_unpacked(m_Result, 0);
	}
}

UICenterLocal* UICenterLocal::createLocal(UIBasicLayer* parent)
{
	UICenterLocal* csl = new UICenterLocal;
	if (csl && csl->init())
	{
		csl->m_pParent = parent;
		csl->autorelease();
		return csl;
	}
	CC_SAFE_DELETE(csl);
	return nullptr;
}


void UICenterLocal::openCenterLocal()
{
	m_isFirst = true;
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_WAREHOUSE_CSB]);
	ProtocolThread::GetInstance()->getWarehouseItems(UILoadingIndicator::create(m_pParent));

	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_WAREHOUSE_CSB]);
	auto p_button = view->getChildByName<Widget*>("panel_warehouse_two_butter");
	auto l_button = p_button->getChildByName<cocos2d::ui::ScrollView*>("listview_citybutter");
	l_button->addEventListener(CC_CALLBACK_2(UICenterLocal::scrollButtonEvent, this));
}

void UICenterLocal::flushCityButton()
{
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_WAREHOUSE_CSB]);
	auto b_city  = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_city"));
	auto listView = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_citybutter"));
	
	auto item = listView->getItem(0);
	item->retain();
	listView->removeAllChildrenWithCleanup(true);
	listView->pushBackCustomItem(item);
	item->release();
	int count = 0;
	if (m_Result->curcitygoods != NULL)
	{
		auto f_item = b_city->clone();
		f_item->setTag(0);
		auto t_city = f_item->getChildByName<Text*>("label_goods");
		t_city->setString(SINGLE_SHOP->getCitiesInfo()[m_Result->curcitygoods->cityid].name);
		listView->pushBackCustomItem(f_item);
		listView->pushBackCustomItem(listView->getItem(0)->clone());
		m_pMinorButton = f_item;
		count++;
	}
	
	for (int i = 0; i < m_Result->n_othergoods; i++)
	{
		auto b_item = b_city->clone();
		auto t_city = b_item->getChildByName<Text*>("label_goods");
		t_city->setString(SINGLE_SHOP->getCitiesInfo()[m_Result->othergoods[i]->cityid].name);
		b_item->setTag(i+1);
		listView->pushBackCustomItem(b_item);
		listView->pushBackCustomItem(listView->getItem(0)->clone());
		count++;
	}
	changeMinorButtonState(m_pMinorButton);

	auto b_more = view->getChildByName<Button*>("button_more");
	if (count > 4)
	{
		b_more->setVisible(true);
	}
	else
	{
		b_more->setVisible(false);
	}
}


void UICenterLocal::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}
	std::string name = ((Button*)pSender)->getName();
	buttonEvent((Button*)pSender,name);
}

void UICenterLocal::buttonEvent(Widget* pSender,std::string name)
{
	//城市之间切换
	if (isButton(button_city)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		changeMinorButtonState(pSender);
		updateCityGoods();
		return;
	}
	//物品详情
	if (isButton(button_items_goods_bg)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_nItemIndex = pSender->getTag();
		openGoodInfo(m_nItemIndex);
		return;
	}
	//全部取回
	if (isButton(button_takeall)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_eOperationIndex = LOCAL_INDEX_TAKEALL;
		UIInform::getInstance()->openInformView(m_pParent);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_CENTER_TAKEALL_ITEM_TITLE","TIP_CENTER_TAKEALL_ITEM");
		return;
	}
	//部分取回
	if (isButton(button_take)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pParent->closeView();
		m_eOperationIndex = LOCAL_INDEX_TAKE;
		UIInform::getInstance()->openInformView(m_pParent);
		UIInform::getInstance()->openConfirmYesOrNO("TIP_CENTER_TAKE_ITEM_TITLE","TIP_CENTER_TAKE_ITEM");
		return;
	}
	//丢弃
	if(isButton(button_drop))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_pMinorButton)
		{
			if (m_pMinorButton->getTag()==0)
			{
				m_eOperationIndex = LOCAL_INDEX_DROP;
			}
			else
			{
				m_eOperationIndex = NOT_LOCAL_INDEX_DROP;;
			}
		}
		
		m_pParent->closeView();
		showDropItem();
		return;
	}
	//关闭提示界面
	if (isButton(button_close)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pParent->closeView();
		return;
	}
	//问号解释--费用
	if (isButton(button_doubt)) 
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		showHelpInfomation();
		return;
	}
	//提示界面取消按钮点击事件
	if (isButton(button_no))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pParent->closeView();
		return;
	}
	//取回操作--确认
	if (isButton(button_confirm_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_eOperationIndex == LOCAL_INDEX_TAKEALL)
		{
			ProtocolThread::GetInstance()->fetchAllFromWarehouse(UILoadingIndicator::create(m_pParent));
		}
		
		if (m_eOperationIndex == LOCAL_INDEX_TAKE)
		{
			ProtocolThread::GetInstance()->fetchItemFromWarehouse(m_Result->curcitygoods->goods[m_nItemIndex]->id,UILoadingIndicator::create(m_pParent));
		}
		return;
	}
	//丢弃-确认
	if (isButton(button_yes))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		m_pParent->closeView();
		if (m_eOperationIndex == LOCAL_INDEX_DROP)
		{
			UIVoyageSlider*	ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(100));
			ProtocolThread::GetInstance()->deleteItemFromWarehouse(m_Result->curcitygoods->goods[m_nItemIndex]->id,ss->getCurrentNumber(),UILoadingIndicator::create(m_pParent));
		}
		else if (m_eOperationIndex ==NOT_LOCAL_INDEX_DROP)
		{
			UIVoyageSlider*	ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(100));
			int index = m_pMinorButton->getTag() - 1;//城市id
			auto nums = ss->getCurrentNumber();
			if ((m_Result->othergoods[index]->n_goods == 1 )&& (nums == m_Result->othergoods[index]->goods[m_nItemIndex]->amount))
			{
				m_pMinorButton = nullptr;
				m_isFirst = true;
			}
			ProtocolThread::GetInstance()->deleteItemFromWarehouse(m_Result->othergoods[index]->goods[m_nItemIndex]->id, ss->getCurrentNumber(), UILoadingIndicator::create(m_pParent));
			
		}
		return;
	}
	//物品详情
	if (isButton(image_item_bg))
	{
		auto m_icon = pSender->getChildByName<ImageView*>("image_item");
		auto m_num = pSender->getChildByName<Text*>("text_item_num");
		ProtocolThread::GetInstance()->getItemsDetailInfo(m_icon->getTag(), pSender->getTag(), m_num->getTag(), UILoadingIndicator::create(m_pParent));
		return;
	}
}

void UICenterLocal::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	switch (msgType)
	{
	case PROTO_TYPE_GetWarehouseItemsResult:
		{
			GetWarehouseItemsResult *result = (GetWarehouseItemsResult *)message;
			if (result->failed == 0)
			{
				m_Result = result;
				if (m_isFirst)
				{
					m_isFirst = false;
					flushCityButton();
				}
				updateCityGoods();	
			}else
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_GET_LOCAL_DATA_FAIL");	
			}
			break;
		}
	case PROTO_TYPE_WarehouseOprationResult:
		{
			WarehouseOprationResult *result = (WarehouseOprationResult*)message; 		
			if (result->failed == 0 && result->count != 0)
			{ 			
				UIInform::getInstance()->openInformView(m_pParent);
				// fetch goods 	0:取回 1：删除 2：全部取回
				if (result->actioncode == 0 || result->actioncode == 2) 		
				{ 		
					auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_WAREHOUSE_CSB]);
					auto b_takeAll = Helper::seekWidgetByName(view,"button_takeall");
					if (m_eOperationIndex == LOCAL_INDEX_TAKEALL)
					{
						if (b_takeAll)
						{
							auto b_takeall = dynamic_cast<Button*>(b_takeAll);
							b_takeall->setTouchEnabled(false);
							b_takeall->setBright(false);
						}
					}else
					{
						if (b_takeAll && m_Result->curcitygoods->n_goods < 2)
						{
							auto b_takeall = dynamic_cast<Button*>(b_takeAll);
							b_takeall->setTouchEnabled(false);
							b_takeall->setBright(false);
						}
					}
					UIInform::getInstance()->openViewAutoClose("TIP_CENTER_FETCH_SUCCESS");	
				}
				else 			
				{ 			
					UIInform::getInstance()->openViewAutoClose("TIP_CENTER_DETELE_ITEM_SUCCESS");	
				} 				
				ProtocolThread::GetInstance()->getWarehouseItems(UILoadingIndicator::create(m_pParent)); 		
			}
			else if (result->failed == 9)
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_SHIP_CAP_FULL");
			}
			else if (result->failed == 104)
			{
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openConfirmYes("TIP_EXCREATE_BAG_FULL");
			}
			else 		
			{ 				
				UIInform::getInstance()->openInformView(m_pParent);
				UIInform::getInstance()->openViewAutoClose("TIP_CENTER_OPERATE_FAIL");	
			} 			
			break; 
		}
	case PROTO_TYPE_GetItemsDetailInfoResult:
		{
			GetItemsDetailInfoResult *result = (GetItemsDetailInfoResult*)message;
			if (result->failed == 0)
			{
				UICommon::getInstance()->openCommonView(m_pParent);
				switch (result->itemtype)
				{
				case ITEM_TYPE_GOOD://goods
					UICommon::getInstance()->flushGoodsDetail(result->goods, result->itemid, true);
					break;
				case ITEM_TYPE_SHIP://ship
					UICommon::getInstance()->flushShipDetail(result->ship, result->itemid, true);
					break;
				case ITEM_TYPE_SHIP_EQUIP://equip
					UICommon::getInstance()->flushEquipsDetail(result->equipment, result->itemid, true);
					break;
				case ITEM_TYPE_PORP://prop
				case ITEM_TYPE_ROLE_EQUIP:// role equip
					UICommon::getInstance()->flushItemsDetail(result->equipment, result->itemid, true);
					break;
				case ITEM_TYPE_DRAWING:// drawing
					UICommon::getInstance()->flushDrawingDetail(result->drawing, result->itemid, true);
					break;
				case ITEM_TYPE_SPECIAL:// special
					UICommon::getInstance()->flushSpecialDetail(result->itemid, true);
					break;
				default:
					break;
				}
			}
			break;
		}
	default:
		break;
	}
}

void UICenterLocal::changeMinorButtonState(Widget *target)
{
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

void UICenterLocal::openGoodInfo(int index)
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_WAREHOUSE_VIEWITEM_CSB]);
	auto w_panel = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_WAREHOUSE_VIEWITEM_CSB]);
	auto m_name = w_panel->getChildByName<Text*>("label_item_name");
	auto m_city = w_panel->getChildByName<Text*>("label_city");
	auto m_bg   = w_panel->getChildByName<ImageView*>("image_item_bg");
	auto m_icon = m_bg->getChildByName<ImageView*>("image_item");
	auto m_num  = m_bg->getChildByName<Text*>("text_item_num");
	auto b_drop = w_panel->getChildByName<Button*>("button_drop");
	auto b_take = w_panel->getChildByName<Button*>("button_take");
	int iid;
	int type;
	int id;
	int optionalitemnum = 0;
	std::string name;
	std::string path;
	if (m_pMinorButton->getTag() == 0)
	{
		iid = m_Result->curcitygoods->goods[index]->itemid;
		type = m_Result->curcitygoods->goods[index]->itemtype;
		id = m_Result->curcitygoods->goods[index]->destitemid;
		m_city->setString(SINGLE_SHOP->getCitiesInfo()[m_Result->curcitygoods->cityid].name);
		setTextSizeAndOutline(m_num,m_Result->curcitygoods->goods[index]->amount);
		b_drop->setVisible(true);
		b_drop->setTouchEnabled(true);
		b_take->setVisible(true);
		b_take->setTouchEnabled(true);
		optionalitemnum = m_Result->curcitygoods->goods[index]->optionalitemnum;
	}else
	{
		iid = m_Result->othergoods[m_pMinorButton->getTag() - 1]->goods[index]->itemid;
		type = m_Result->othergoods[m_pMinorButton->getTag() - 1]->goods[index]->itemtype;
		id = m_Result->othergoods[m_pMinorButton->getTag() - 1]->goods[index]->destitemid;
		m_city->setString(SINGLE_SHOP->getCitiesInfo()[m_Result->othergoods[m_pMinorButton->getTag()-1]->cityid].name);
		setTextSizeAndOutline(m_num, m_Result->othergoods[m_pMinorButton->getTag() - 1]->goods[index]->amount);
		auto pos1 = b_drop->getPositionX();
		auto pos2 = b_take->getPositionX();
		b_drop->setVisible(true);
		b_drop->setTouchEnabled(true);
		b_drop->setPositionX((pos1 + pos2) / 2);
		b_take->setVisible(false);
		b_take->setTouchEnabled(false);
		optionalitemnum = m_Result->othergoods[m_pMinorButton->getTag() - 1]->goods[index]->optionalitemnum;
	}

	if (optionalitemnum > 0)
	{
		addStrengtheningIcon(m_bg);
	}
	m_bg->setTag(type);
	m_icon->setTag(iid);

	m_num->setTag(id);
	getItemNameAndPath(type,iid, name, path);
	m_name->setString(changeItemName(name));
	m_icon->loadTexture(path);
	setBgButtonFormIdAndType(m_bg, iid, type);
	setTextColorFormIdAndType(m_name, iid, type);

	m_bg->setTouchEnabled(true);
	m_bg->addTouchEventListener(CC_CALLBACK_2(UICenterLocal::menuCall_func, this));
}

void UICenterLocal::updateCityGoods()
{
	auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_WAREHOUSE_CSB]);
	auto l_cityGoods = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_warehouse"));
	l_cityGoods->setDirection(ListView::Direction::VERTICAL);
	auto b_cityGoods = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_items_goods_bg"));
	auto i_cityGoods = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view,"image_items_goods_bg_nt"));
	l_cityGoods->removeAllChildrenWithCleanup(true);

	auto b_takeAll = Helper::seekWidgetByName(view,"button_takeall");
	int index = m_pMinorButton->getTag();
	CityGoods*	citygoods = nullptr;
	if (index == 0)
	{
		citygoods = m_Result->curcitygoods;
		if (citygoods->n_goods < 1)
		{
			b_takeAll->setTouchEnabled(false);
			b_takeAll->setBright(false);
		}else
		{
			b_takeAll->setTouchEnabled(true);
			b_takeAll->setBright(true);
		}	
	}else
	{
		citygoods = m_Result->othergoods[index - 1];
		b_takeAll->setTouchEnabled(false);
		b_takeAll->setBright(false);
	}
	if (citygoods == nullptr)
	{
		return;
	}
	int n_grid = 10; //skill
	int n_line = (n_grid/8)+1;
	if (citygoods->n_goods > n_grid)
	{
		n_line = (citygoods->n_goods/8)+1;
	}
	
	for (int i = 0; i < n_line; ++i)
	{
		ListView *item_layer = ListView::create();
		item_layer->setDirection(ListView::Direction::HORIZONTAL);
		item_layer->setItemsMargin(3);
		item_layer->setContentSize(Size(812,98));
		for (int j = 0; j < 8; ++j)
		{
			if (8 * i + j < citygoods->n_goods)
			{
				Button *item = (Button*)b_cityGoods->clone();
				item->setTag(8 * i + j);
				item->setSwallowTouches(false);
				auto i_icon = item->getChildByName<ImageView*>("image_goods");
				auto t_text = item->getChildByName<Text*>("text_item_num");

				std::string name;
				std::string path;
				getItemNameAndPath(citygoods->goods[8 * i + j]->itemtype,citygoods->goods[8 * i + j]->itemid,name,path);
				i_icon->loadTexture(path);
				setTextSizeAndOutline(t_text,citygoods->goods[8 * i + j]->amount);
				item_layer->pushBackCustomItem(item);
				auto i_item_bg = item->getChildByName<ImageView*>("image_goods_bg");
				auto i_item_lv = item->getChildByName<ImageView*>("image_item_bg_lv");
				setBgButtonFormIdAndType(i_item_bg, citygoods->goods[8 * i + j]->itemid, citygoods->goods[8 * i + j]->itemtype);
				setBgImageColorFormIdAndType(i_item_lv, citygoods->goods[8 * i + j]->itemid, citygoods->goods[8 * i + j]->itemtype);
				
				if (citygoods->goods[8 * i + j]->optionalitemnum > 0)
				{
					addStrengtheningIcon(item);
				}
			}else if (8 * i + j < n_grid)
			{
				ImageView *item = (ImageView*)i_cityGoods->clone();
				item_layer->pushBackCustomItem(item);
			}
			
		}
		l_cityGoods->pushBackCustomItem(item_layer);	
	}
	int good_need_fee_count = 0;
// 	if (m_Result->curcitygoods->n_goods>10)
// 	{
// 		good_need_fee_count = m_Result->curcitygoods->n_goods - 10;
// 	}
// 
// 	for (int i = 0; i < m_Result->n_othergoods; i++)
// 	{
// 		if (m_Result->othergoods[i]->n_goods>10)
// 		{
// 			good_need_fee_count += (m_Result->othergoods[i]->n_goods-10);
// 		}
// 	}
	//仓库格子计算修改
	int counts = m_Result->curcitygoods->n_goods;
	for (int i = 0; i < m_Result->n_othergoods;i++)
	{
		counts += m_Result->othergoods[i]->n_goods;
	}
	good_need_fee_count = counts - 10;
	if (good_need_fee_count<=0)
	{
		good_need_fee_count = 0;
	}
	Text *t_cust = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_total_custodian_num"));
	t_cust->setString(numSegment(StringUtils::format("%d",500 * good_need_fee_count))+ "/" +(SINGLE_SHOP->getTipsInfo()["TIP_DAY"].data()));
	auto l_fees = dynamic_cast<ListView*>(Helper::seekWidgetByName(view,"listview_fees"));
	l_fees->refreshView();

	auto image_pulldown_item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_pulldown_item"));
	auto button_pulldown = image_pulldown_item->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown_item->getBoundingBox().size.width / 2 - image_pulldown_item->getBoundingBox().size.width / 2);
	addListViewBar(l_cityGoods, image_pulldown_item);
}

void UICenterLocal::showHelpInfomation()
{
	m_pParent->openView(COMMOM_COCOS_RES[C_VIEW_COST_SILVER_COM_CSB]);
	auto w_panel = m_pParent->getViewRoot(COMMOM_COCOS_RES[C_VIEW_COST_SILVER_COM_CSB]);
	w_panel->getChildByName("button_s_no")->setVisible(false);
	w_panel->getChildByName("button_s_yes")->setVisible(false);

	auto b_close = Button::create();
	b_close->setName("button_close");
	b_close->loadTextureNormal("res/close.png");
	b_close->addTouchEventListener(CC_CALLBACK_2(UICenterLocal::menuCall_func,this));
	b_close->setPosition(Vec2(w_panel->getBoundingBox().size.width * 0.74,w_panel->getBoundingBox().size.height * 0.68));
	w_panel->addChild(b_close);

	auto t_title = w_panel->getChildByName<Text*>("label_repair");
	t_title->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_LOCAL_HELP_TITLE"]);
	auto t_content = w_panel->getChildByName<Text*>("label_repair_content");
	t_content->setContentSize(t_content->getContentSize() + Size(0, t_content->getFontSize()));
	t_content->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_LOCAL_HELP_CONTENT"]);

	auto t_costTitle = dynamic_cast<Text*>(Helper::seekWidgetByName(w_panel,"label_cost"));
	t_costTitle->setString(SINGLE_SHOP->getTipsInfo()["TIP_CENTER_LOCAL_HELP_COST"]);
	auto i_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(w_panel, "image_silver_1"));
	auto t_cost = dynamic_cast<Text*>(Helper::seekWidgetByName(w_panel,"label_cost_num"));
	int goods_total = 0;
	/*if (m_Result->curcitygoods->n_goods>10)
	{
	goods_total += (m_Result->curcitygoods->n_goods - 10);
	}

	for (int i = 0; i < m_Result->n_othergoods; i++)
	{
	if (m_Result->othergoods[i]->n_goods>10)
	{
	goods_total += (m_Result->othergoods[i]->n_goods-10);
	}
	}*/
	//仓库格子计算修改
	int counts = m_Result->curcitygoods->n_goods;
	for (int i = 0; i < m_Result->n_othergoods; i++)
	{
		counts += m_Result->othergoods[i]->n_goods;
	}
	goods_total = counts - 10;
	if (goods_total <= 0)
	{
		goods_total = 0;
	}
	t_cost->setString(numSegment(StringUtils::format("%d",500 * goods_total)));
	i_silver->setPositionX(t_cost->getPositionX() - t_cost->getBoundingBox().size.width - i_silver->getContentSize().width/2 - 5);
}

void UICenterLocal::showDropItem()
{
	m_pParent->openView(PLAYER_COCOS_RES[PLAYER_DROP_ITEM_CSB]);
	auto w_dropItemInfo = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_DROP_ITEM_CSB]);
	auto i_item_bg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(w_dropItemInfo, "image_item_bg"));
	auto i_item_lv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(w_dropItemInfo, "image_item_bg_lv"));
	auto i_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(w_dropItemInfo,"image_item"));
	auto t_drop = dynamic_cast<Text*>(Helper::seekWidgetByName(w_dropItemInfo,"label_dropitem_num"));
	auto b_yes= dynamic_cast<Button*>(Helper::seekWidgetByName(w_dropItemInfo,"button_yes"));
	auto b_no= dynamic_cast<Button*>(Helper::seekWidgetByName(w_dropItemInfo,"button_no"));
	std::string name;
	std::string path;
	auto index = m_pMinorButton->getTag();
	int amount = 0;
	if (index==0)
	{
		getItemNameAndPath(m_Result->curcitygoods->goods[m_nItemIndex]->itemtype, m_Result->curcitygoods->goods[m_nItemIndex]->itemid, name, path);
		amount = m_Result->curcitygoods->goods[m_nItemIndex]->amount;
		setBgButtonFormIdAndType(i_item_bg, m_Result->curcitygoods->goods[m_nItemIndex]->itemid, m_Result->curcitygoods->goods[m_nItemIndex]->itemtype);
		setBgImageColorFormIdAndType(i_item_lv, m_Result->curcitygoods->goods[m_nItemIndex]->itemid, m_Result->curcitygoods->goods[m_nItemIndex]->itemtype);

		if (m_Result->curcitygoods->goods[m_nItemIndex]->optionalitemnum > 0)
		{
			addStrengtheningIcon(i_item_bg);
		}

	}
	else
	{
		getItemNameAndPath(m_Result->othergoods[index - 1]->goods[m_nItemIndex]->itemtype, m_Result->othergoods[index - 1]->goods[m_nItemIndex]->itemid, name, path);
		amount = m_Result->othergoods[index - 1]->goods[m_nItemIndex]->amount;
		setBgButtonFormIdAndType(i_item_bg, m_Result->othergoods[index - 1]->goods[m_nItemIndex]->itemid, m_Result->othergoods[index - 1]->goods[m_nItemIndex]->itemtype);
		setBgImageColorFormIdAndType(i_item_lv, m_Result->othergoods[index - 1]->goods[m_nItemIndex]->itemid, m_Result->othergoods[index - 1]->goods[m_nItemIndex]->itemtype);

		if (m_Result->othergoods[index - 1]->goods[m_nItemIndex]->optionalitemnum > 0)
		{
			addStrengtheningIcon(i_item_bg);
		}
	}

	i_icon->ignoreContentAdaptWithSize(false);
	i_icon->loadTexture(path);
	t_drop->setString(StringUtils::format("x %d",amount));
	b_yes->addTouchEventListener(CC_CALLBACK_2(UICenterLocal::menuCall_func,this));
	b_no->addTouchEventListener(CC_CALLBACK_2(UICenterLocal::menuCall_func,this));
	
	

	auto sslider = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(100));
	if (sslider)
	{
		sslider->removeFromParentAndCleanup(true);
	}
	auto w_slider = dynamic_cast<Widget*>(Helper::seekWidgetByName(w_dropItemInfo,"panel_slider"));
	UIVoyageSlider *ss = UIVoyageSlider::create(w_slider,amount,0,true);
	this->addChild(ss,1,100);
	ss->addSliderScrollEvent_1(CC_CALLBACK_1(UICenterLocal::updataDropNum,this));
	ss->setMaxPercent(amount);
	ss->setCurrentNumber(amount);
}

void UICenterLocal::updataDropNum(int num)
{
	auto w_dropItemInfo = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_DROP_ITEM_CSB]);
	auto t_drop = dynamic_cast<Text*>(Helper::seekWidgetByName(w_dropItemInfo,"label_dropitem_num"));
	t_drop->setString(StringUtils::format("x %d",num));
	UIVoyageSlider*	ss = dynamic_cast<UIVoyageSlider*>(this->getChildByTag(100));
	ss->setCurrentNumber(num);
}

std::string UICenterLocal::changeItemName(std::string itemName)
{
	auto nameLen = itemName.length();
	LanguageType nType = LanguageType(Utils::getLanguage());
	switch (nType)
	{
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
	case cocos2d::LanguageType::CHINESE:
	{
										   if (nameLen >3 * 10)//utf8大多数一个汉字3字节
										   {
											   itemName = itemName.substr(0, 3 * 10) + "...";
										   }
										   break;
	}
	default:
	{
			   if (nameLen > 20)
			   {
				   itemName = itemName.substr(0, 20) + "...";
			   }
			   break;
	}
	}
	return itemName;
}

void UICenterLocal::scrollButtonEvent(Ref *pSender, cocos2d::ui::ScrollView::EventType type)
{
	if (type == cocos2d::ui::ScrollView::EventType::SCROLL_TO_TOP)
	{
		auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_WAREHOUSE_CSB]);
		auto b_more = view->getChildByName<Button*>("button_more");
		b_more->setBright(true);
	}
	else if (type == cocos2d::ui::ScrollView::EventType::SCROLL_TO_BOTTOM)
	{
		auto view = m_pParent->getViewRoot(PLAYER_COCOS_RES[PLAYER_WAREHOUSE_CSB]);
		auto b_more = view->getChildByName<Button*>("button_more");
		b_more->setBright(false);
	}
}