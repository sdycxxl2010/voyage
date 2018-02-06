#include "UIOffLine.h"
#include "UISailHUD.h"
#include "TVSceneLoader.h"
#include "UIInform.h"

#include "TVBattleManager.h"
#include "UIWorldNotify.h"
#define  ITEM_NUM 2;
UIOffLine::UIOffLine()
{
	m_lootTimes = 0;
	m_cameraMask = 1;
	m_nPageIndex = 0;
	m_pParent = nullptr;
	m_delegateResult = nullptr;
	m_emailResultl = nullptr;
	m_revengePlayerInfo = nullptr;
	m_lootPlayerInfo = nullptr;
}
UIOffLine::~UIOffLine()
{
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
	for (auto loot : m_vLootList)
	{
		delete loot;
		loot = nullptr;
	}
	m_vLootList.clear();
}


bool UIOffLine::init(int cameraMask)
{
	if (!UIBasicLayer::init())
	{
		return false;
	}
	this->m_cameraMask = cameraMask;
	m_emailResultl = ProtocolThread::GetInstance()->getLastLoginResult();
	auto scene = _director->getRunningScene();
	if (cameraMask == 4)
	{
		auto node = scene->getChildByTag(MAP_TAG + 100);
		if(node){
			Node * mapUINode = node->getChildByName("MAPUI");
			UISailHUD*mapUI =  dynamic_cast<UISailHUD*>(mapUINode);
			if(mapUI){
				mapUI->shipPause(true);
			}
		}
	}
	registerCallBack();
	showdelegateResult();
	return true;
}
UIOffLine * UIOffLine::create(int cameraMask)
{
	auto layer = new UIOffLine;
	if (layer && layer->init(cameraMask))
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}
void UIOffLine::openOffLineDelegateLayer(UIBasicLayer * parent, int cameramask)
{
	auto layer = UIOffLine::create(cameramask);
	layer->setLocalZOrder(10000);
	layer->setCameraMask(cameramask);
	if (parent!= nullptr)
	{
		parent->addChild(layer);
		layer->m_pParent = parent;
	}else
	{
		Director::getInstance()->getRunningScene()->addChild(layer);
	}
}

void UIOffLine::onServerEvent(struct ProtobufCMessage* message, int msgType)
{
	UIBasicLayer::onServerEvent(message, msgType);
	switch (msgType)
	{
	case PROTO_TYPE_GetLootPlayerLogResult:
	{
		GetLootPlayerLogResult * result = (GetLootPlayerLogResult*)message;
		if (result->failed == 0)
		{
			m_delegateResult = result;
			m_vLootList.clear();
			for (int i = 0; i < result->n_log; i++)
			{
				m_vLootList.push_back(m_delegateResult->log[i]);
			}
			showRevengeResult(m_delegateResult);
		}
		else
		{

		}

	}
		break;
	case PROTO_TYPE_FindLootPlayerResult :
	{
		FindLootPlayerResult * result = (FindLootPlayerResult *)message;
		if (result->failed == 0)
		{
			m_lootPlayerInfo = result;
			if (result->info->lootforbid == 1)
			{
				UIInform::getInstance()->openInformView(this);
				UIInform::getInstance()->openConfirmYes("TIP_CANNOT_LOOT_PLAYER_INFO");
			}
			else
			{
				_userData = m_lootPlayerInfo->battledata;
				openFightEvent();
			}

		}
		//当前没有可打劫的玩家提示信息
		else if (result->failed == 2)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_LOOT_PLAYER_INFO");
		}
		//当前玩家不可以被复仇提示信息
		else if (result->failed == 3)
		{
			UIInform::getInstance()->openInformView(this);
			UIInform::getInstance()->openConfirmYes("TIP_CANNOT_LOOT_PLAYER_INFO");
		}
	}
		default:
			break;
	}
}
void UIOffLine::menuCall_func(Ref *pSender, Widget::TouchEventType TouchType)
{
	if (Widget::TouchEventType::ENDED != TouchType)
	{
		return;
	}
	Node * sender = dynamic_cast<Node*>(pSender);
	std::string nodeName = sender->getName();
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
	callEventByName(nodeName, sender);
}
void UIOffLine::callEventByName(std::string& name, Node* node)
{
	//结算界面继续按钮
	if (isButton(button_continue_1))
	{
		if (m_lootTimes != 0)
		{
			ProtocolThread::GetInstance()->getLootPlayerLog(0, 0, UILoadingIndicator::createWithMask(this, m_cameraMask));
		}
		else
		{
			closeView(MAPUI_COCOS_RES[INDEX_UI_ROB_RESULT_1_CSB]);
			if (m_cameraMask == 4)
			{
				auto scene = _director->getRunningScene();
				Node * m_MapUI = scene->getChildByTag(MAP_TAG + 100)->getChildByName("MAPUI");
				dynamic_cast<UISailHUD*>(m_MapUI)->shipPause(false);
			}
			if (m_pParent)
			{
				m_pParent->notifyCompleted(OFFLINE_DELEGATE);
			}
			this->removeFromParentAndCleanup(true);
		}
		return;
	}
	//报复
	if (isButton(button_revenge))
	{
		int cid = node->getTag();
		m_revengePlayerInfo = m_delegateResult->log[cid];
		ProtocolThread::GetInstance()->startRevengeOnLootPlayer(m_revengePlayerInfo->id,UILoadingIndicator::createWithMask(this,m_cameraMask));
		return;
	}
	//报复列表确认按钮
	if (isButton(button_yes))
	{
	    closeView(MAPUI_COCOS_RES[INDEX_UI_ROB_RESULT_2_CSB]);
		closeView(MAPUI_COCOS_RES[INDEX_UI_ROB_RESULT_1_CSB]);
		if (m_cameraMask == 4)
		{
			auto scene = _director->getRunningScene();
			Node * m_MapUI = scene->getChildByTag(MAP_TAG + 100)->getChildByName("MAPUI");		
			dynamic_cast<UISailHUD*>(m_MapUI)->shipPause(false);
		}
		if (m_pParent)
		{
			m_pParent->notifyCompleted(OFFLINE_DELEGATE);
		}
		this->removeFromParentAndCleanup(true);
		return;
	}
	//说明
	if (isButton(button_bank_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_OUTLINEDELEGATE_TITLE", "TIP_OUTLINEDELEGATE_CONTENT");
	}

	//打劫开始战斗--问号解释
	if (isButton(button_rob_info))
	{
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_SAILING_BATTLE_NOT_LOSE_TITLE", "TIP_SAILING_BATTLE_NOT_LOSE_CONTENT");
		return;
	}
}
void UIOffLine::showdelegateResult()
{
	openView(MAPUI_COCOS_RES[INDEX_UI_ROB_RESULT_1_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_ROB_RESULT_1_CSB]);
	m_nPageIndex = PAGE_LOOT_LOG_INDEX;
	view->setOpacity(255 / 2);
	view->runAction(FadeIn::create(0.5));
	auto l_listview = view->getChildByName<ListView*>("listview_content");	
	TableView* mTableView = dynamic_cast<TableView*>(l_listview->getParent()->getChildByTag(999));
	if (!mTableView){
		mTableView = TableView::create(this, Size(l_listview->getContentSize().width, l_listview->getContentSize().height));
		l_listview->getParent()->addChild(mTableView,100);
	}
	mTableView->setVisible(true);
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(l_listview->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(999);
	mTableView->setTouchEnabled(true);
	auto cell = mTableView->dequeueCell();
	auto children = mTableView->getChildren();
	this->setCameraMask(_cameraMask, true);
}
void UIOffLine::showRevengeResult(GetLootPlayerLogResult * result)
{
	openView(MAPUI_COCOS_RES[INDEX_UI_ROB_RESULT_2_CSB]);
	auto view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_ROB_RESULT_2_CSB]);
	m_nPageIndex = PAGE_LOOT_RESULT_LIST_INDEX;
	auto item = view->getChildByName<Widget*>("image_log_1");
	auto l_listview = view->getChildByName<ListView*>("listview_content");
	auto image_pulldown  = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName("button_pulldown");
	view->setCameraMask(m_cameraMask, true);
	TableView* mTableView = dynamic_cast<TableView*>(l_listview->getParent()->getChildByTag(999));
	log("table: %f %f", l_listview->getContentSize().width, l_listview->getContentSize().height);
	if (!mTableView){
		mTableView = TableView::create(this, CCSizeMake(l_listview->getContentSize().width, l_listview->getContentSize().height));
		l_listview->getParent()->addChild(mTableView, 100);
	}
	mTableView->setVisible(true);
	mTableView->setDirection(TableView::Direction::VERTICAL);
	mTableView->setDelegate(this);
	mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	mTableView->setPosition(l_listview->getPosition());
	mTableView->setVisible(true);
	mTableView->setTag(999);
	mTableView->setTouchEnabled(true);
	image_pulldown->setLocalZOrder(10001);
 	mTableView->setUserObject(button_pulldown);
	mTableView->reloadData();
	this->setCameraMask(_cameraMask, true);

}
void UIOffLine::openBattleView(EngageInFightResult *battledata)
{
	_userData = battledata;
	openView(MAPUI_COCOS_RES[INDEX_UI_ROB_UI_CSB]);
	Widget* view = getViewRoot(MAPUI_COCOS_RES[INDEX_UI_ROB_UI_CSB]);

	view->setCameraMask(m_cameraMask, true);
	Widget* panel_title_left = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_title_left"));
	ImageView* image_flag_left = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_left, "image_flag_left"));
	Text* label_name_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_left, "label_name_left"));
	Text* label_lv_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_left, "label_lv_left"));
	ImageView* image_moral_left = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_left, "image_moral_left"));

	std::string m_country = StringUtils::format("res/country_icon/flag_%d.png", SINGLE_HERO->m_iNation);
	image_flag_left->loadTexture(m_country);
	label_name_left->setString(SINGLE_HERO->m_sName);
	std::string m_level = StringUtils::format("Lv. %d", battledata->userlv);
	label_lv_left->setString(m_level);
	flushMoral(image_moral_left, battledata->morale);

	Widget* panel_title_right = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_title_right"));
	ImageView* image_flag_right = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_right, "image_flag_right"));
	Text* label_name_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_right, "label_name_right"));
	Text* label_lv_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_title_right, "label_lv_right"));
	ImageView* image_moral_right = dynamic_cast<ImageView*>(Helper::seekWidgetByName(panel_title_right, "image_moral_right"));

	std::string e_country = StringUtils::format("res/country_icon/flag_%d.png", m_revengePlayerInfo->peerinfo->nation);
	image_flag_right->loadTexture(e_country);
	std::string e_strName = m_revengePlayerInfo->peerinfo->heroname;
	label_name_right->setString(e_strName);
	std::string e_level = StringUtils::format("Lv. %d", battledata->enemyinfo->level);
	label_lv_right->setString(e_level);
	flushMoral(image_moral_right, battledata->npcmorale);

	Widget* panel_combat_left = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_combat_left"));
	Text* label_combat_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_combat_left"));
	Text* label_ship_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_ship_left"));
	Text* label_sailor_left = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_left, "label_sailor_left"));
	std::string m_combat = StringUtils::format("%d", battledata->myinfo->power);
	label_combat_left->setString(m_combat);
	std::string m_shipNum = StringUtils::format("%d", battledata->myinfo->shipnum);
	label_ship_left->setString(m_shipNum);
	std::string m_sailNum = StringUtils::format("%d", battledata->myinfo->sailnum);
	label_sailor_left->setString(m_sailNum);

	Widget* panel_combat_right = dynamic_cast<Widget*>(Helper::seekWidgetByName(view, "panel_combat_right"));
	Text* label_win_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_win_num"));
	Text* label_combat_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_combat_right"));
	Text* label_ship_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_ship_right"));
	Text* label_sailor_right = dynamic_cast<Text*>(Helper::seekWidgetByName(panel_combat_right, "label_sailor_right"));
	Text* label_rob_coin = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_speed_num"));
	std::string e_combat = StringUtils::format("%d", battledata->enemyinfo->power);
	label_combat_right->setString(e_combat);
	std::string e_shipNum = StringUtils::format("%d", battledata->enemyinfo->shipnum);
	label_ship_right->setString(e_shipNum);
	std::string e_sailNum = StringUtils::format("%d", battledata->enemyinfo->sailnum);
	label_sailor_right->setString(e_sailNum);
	std::string str_win_num = StringUtils::format("%d%%", battledata->victorypercent);
	label_win_num->setString(str_win_num);
	if (battledata->victorypercent < 30)
	{
		label_win_num->setTextColor(cocos2d::Color4B::RED);
	}
	else if (battledata->victorypercent < 60)
	{
		label_win_num->setTextColor(cocos2d::Color4B::ORANGE);
	}
	else
	{
		label_win_num->setTextColor(Color4B(40, 25, 13, 255));
	}

	auto b_info = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_rob_info"));
	auto t_ranson = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "labe_ranson"));
	t_ranson->setString(SINGLE_SHOP->getTipsInfo()["TIP_SAILING_BATTLE_NOT_LOSE"]);
	t_ranson->setPositionX(view->getBoundingBox().size.width / 2 - b_info->getBoundingBox().size.width);
	b_info->setPositionX(t_ranson->getPositionX() + t_ranson->getBoundingBox().size.width / 2 + b_info->getBoundingBox().size.width);
}
void UIOffLine::flushMoral(Widget* view, int num)
{
		for (int i = 1; i <= 5; i++)
		{
			std::string str_moral = StringUtils::format("image_moral_%d", i);
			ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(view, str_moral));
			moral_image->loadTexture(getMoralIconPath(3));
		}
		int num_morale = num;
		int n1000 = (num_morale / 1000) % 6;
		int n100 = num_morale - n1000 * 1000;
		for (int i = 1; i <= n1000; i++){

			std::string str_moral = StringUtils::format("image_moral_%d", i);
			ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(view, str_moral));
			moral_image->loadTexture(getMoralIconPath(1));

		}
		if (n1000 < 5 && n100 >0){
			std::string str_moral = StringUtils::format("image_moral_%d", n1000 + 1);
			ImageView* moral_image = dynamic_cast<ImageView *>(Helper::seekWidgetByName(view, str_moral));
			moral_image->loadTexture(getMoralIconPath(2));
		}

}

void UIOffLine::openFightEvent()
{
	_parent->setVisible(false);
	auto map = _parent;
	map->retain();
	ProtocolThread::GetInstance()->unregisterMessageCallback(_parent);
	EngageInFightResult *result = (EngageInFightResult*)(_userData);
	SINGLE_HERO->m_pBattelData = result;
	SINGLE_HERO->shippos = Vec2(0, 0);
	ProtocolThread::GetInstance()->forceFightStatus();
	CHANGETO(SCENE_TAG::BATTLE_TAG);
}
void UIOffLine::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
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
	if (contentSize.height <= viewSize.height){
		button_pulldown->setVisible(false);
		button_pulldown->getParent()->setVisible(false);
	}
	else{
		button_pulldown->setVisible(true);
		button_pulldown->getParent()->setVisible(true);
		auto factor = (currentLen / totalLen);
		if (factor > 1.0) factor = 1.0;
		if (factor < 0) factor = 0;
		button_pulldown->setPositionY(len * factor + allowHeight * 2 + allowHeight);
	}

}

Size UIOffLine::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	if (m_nPageIndex == PAGE_LOOT_LOG_INDEX)
	{
		auto old_item = m_pRoot->getChildByName<Widget*>("panel_coins");
		Size s = old_item->getContentSize();
		return s;
	}
	if (m_nPageIndex == PAGE_LOOT_RESULT_LIST_INDEX)
	{
		if (m_vLootList.size() > 0)
		{
			auto old_item = m_pRoot->getChildByName<Widget*>("image_log_1");
			Size s = old_item->getContentSize();
			return s;
		}
	}
	return Size(0, 0);
}
TableViewCell* UIOffLine::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell *cell = new TableViewCell();//table->dequeueCell();
	cell->autorelease();
	Widget* cell_item;	
	if (m_nPageIndex == PAGE_LOOT_LOG_INDEX)
	{
         //判断结算界面Item
		if (idx == 0)
		{
			auto old_item = m_pRoot->getChildByName<Widget*>("panel_coins");
			cell_item = old_item->clone();
		}
		else if (idx == 1)
		{
			auto old_item = m_pRoot->getChildByName<Widget*>("panel_attacked");
			cell_item = old_item->clone();
		}
		else if (idx == 2)
		{
			auto old_item = m_pRoot->getChildByName<Widget*>("panel_exp");
			cell_item = old_item->clone();
		}
		else if (idx == 3)
		{
			auto old_item = m_pRoot->getChildByName<Widget*>("panel_r");
			cell_item = old_item->clone();
		}	
	}
	if (m_nPageIndex == PAGE_LOOT_RESULT_LIST_INDEX)
	{
		auto old_item = m_pRoot->getChildByName<Widget*>("image_log_1");
		cell_item = old_item->clone();
	}
	cell->addChild(cell_item, 1);
	cell_item->setPosition(Vec2(15, 0));
	cell->setTag(idx);
	cell->setVisible(true);
	cell_item->setSwallowTouches(false);
	if (m_nPageIndex == PAGE_LOOT_LOG_INDEX)
	{
		//离线委托的到的银币
		if (idx == 0)
		{
			auto i_coin = Helper::seekWidgetByName(cell_item, "image_silver_4");
			auto t_coin = cell_item->getChildByName<Text*>("label_hosted_num");
			auto str = numSegment(StringUtils::format("%lld", m_emailResultl->delegatecoins));
			if (m_emailResultl->delegatecoins > 0)
			{
				t_coin->setString("+" + str);
			}
			else
			{
				t_coin->setString(str);
			}
			i_coin->setPositionX(t_coin->getPositionX() - t_coin->getContentSize().width - i_coin->getContentSize().width / 2 - 6);
		}
		//离线委托被打劫次数
		if (idx == 1)
		{
			auto t_times = cell_item->getChildByName<Text*>("label_hosted_num_0");
			auto t_times_label = cell_item->getChildByName<Text*>("label_hosted");
			m_lootTimes = m_emailResultl->attackedtimes;
			t_times_label->setString(SINGLE_SHOP->getTipsInfo()["TIP_OUTLINE_DELEGATE_REPORT_TIMES_LABEL"]);
			auto timeStr = StringUtils::format("%d", m_emailResultl->attackedtimes);
			t_times->setString(timeStr);
		}
	}
	
	if (m_nPageIndex == PAGE_LOOT_RESULT_LIST_INDEX)
	{
		cell_item->setCameraMask(m_cameraMask);
		cell_item->setAnchorPoint(Vec2(0, 0));
		LootLog * logdata = m_vLootList.at(idx);
		cell_item->setTag(idx);
		auto t_name = cell_item->getChildByName<Text*>("label_name");
		auto t_lv = cell_item->getChildByName<Text*>("label_lv");
		auto i_coin = cell_item->getChildByName("image_silver");
		auto t_coin = cell_item->getChildByName<Text*>("label_looted_num");
		auto b_replay = cell_item->getChildByName("button_replay");
		auto i_flag = cell_item->getChildByName<ImageView*>("image_flag");
		auto t_lose = cell_item->getChildByName<Text*>("label_lose");
		auto t_time = cell_item->getChildByName<Text*>("label_time");
		auto t_time1 = cell_item->getChildByName<Text*>("label_time");
		auto t_win = cell_item->getChildByName<Text*>("label_won");
//		auto t_draw = cell_item->getChildByName<Text*>("label_draw");
		auto b_revenge = cell_item->getChildByName<Button*>("button_revenge");
		b_replay->setVisible(false);
		t_name->setString(logdata->peerinfo->heroname);
		t_lv->setString(StringUtils::format("Lv.%d", logdata->peerinfo->level));
		i_flag->loadTexture(StringUtils::format("res/country_icon/flag_%d.png", logdata->peerinfo->nation));
		auto coinNum = numSegment(StringUtils::format("%lld", logdata->coins));
		t_coin->setString(coinNum);
		t_coin->setPositionX(i_coin->getPositionX() + i_coin->getContentSize().width / 2 + 6);

		//失败
		if (logdata->result == 1)
		{
			t_lose->setVisible(true);
			t_win->setVisible(false);
//			t_draw->setVisible(false);
			b_revenge->setTag(idx);
		}
		//胜利
		else if (logdata->result == 2)
		{
			t_lose->setVisible(false);
			t_win->setVisible(true);
//			t_draw->setVisible(false);
			b_revenge->setVisible(false);
		}
		//平局
		else if (logdata->result == 3)
		{
			t_lose->setVisible(false);
			t_win->setVisible(true);
//			t_draw->setVisible(false);
			b_revenge->setVisible(false);
		}
		//敌人逃跑
		else if (logdata->result == 4)
		{
			t_lose->setVisible(false);
			t_win->setVisible(true);
//			t_draw->setVisible(false);
			b_revenge->setVisible(false);
		}

		if (logdata->peerinfo->lootforbid)
		{
			b_revenge->setTouchEnabled(false);
			b_revenge->setBright(false);
		}

		std::string content;
		int time = logdata->secondsago / (60.0 * 60.0 * 24);
		if (time > 0)
		{
			content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_BATTLE_LOG_DATS_AGO"];
		}
		else
		{
			time = logdata->secondsago / (60.0 * 60.0);
			if (time > 0)
			{
				content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_BATTLE_LOG_HOURS_AGO"];
			}
			else
			{
				time = ceil(logdata->secondsago / 60.0);
				content = SINGLE_SHOP->getTipsInfo()["TIP_EMAIL_BATTLE_LOG_MINS_AGO"];
			}
		}

		std::string new_value = StringUtils::format("%d", time);
		std::string old_value = "[time]";
		repalce_all_ditinct(content, old_value, new_value);
		t_time->setString(content);
	}
	cell->setCameraMask(m_cameraMask);
	return cell;
}
ssize_t UIOffLine::numberOfCellsInTableView(TableView *table)
{

    if (m_nPageIndex == PAGE_LOOT_LOG_INDEX)
    {
		//现在没有经验和声望项 只显示银币和被打劫次数
		return RESULT_LOG_ITEM_NUM;
    }

	if (m_nPageIndex == PAGE_LOOT_RESULT_LIST_INDEX)
	{
		if (m_vLootList.size() > 0)
		{
			return m_vLootList.size();
		}
	}
	return 0;
}
