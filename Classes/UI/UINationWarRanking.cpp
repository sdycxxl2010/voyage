#include "UINationWarRanking.h"
#include "SystemVar.h"
#include "ProtocolThread.h"
#include "UITips.h"
#include "TVSingle.h"

#include "UIInform.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "TVSceneLoader.h"
#include "UICommon.h"
#include "Utils.h"
#include "UISailManage.h"

USING_NS_CC;
using namespace rapidjson;
using namespace cocostudio;
UINationWarRanking::UINationWarRanking()
{
	m_pContributionRankResult = nullptr;
};
UINationWarRanking::~UINationWarRanking()
{
	ProtocolThread::GetInstance()->unregisterMessageCallback(this);
}

UINationWarRanking*UINationWarRanking::createCountryWarRanking()
{
	auto warRanking = new UINationWarRanking;
	if (warRanking && warRanking->init())
	{
		warRanking->autorelease();
		return warRanking;
	}
	CC_SAFE_DELETE(warRanking);
	return nullptr;
}
bool UINationWarRanking::init()
{
	bool pRet = false;
	do 
	{
		ProtocolThread::GetInstance()->registerMessageCallback(CC_CALLBACK_2(UINationWarRanking::onServerEvent, this), this);
		initStatic();
		pRet = true;
	} while (0);
	return pRet;
}
void UINationWarRanking::initStatic()
{
	openView(PALACE_COCOS_RES[VIEW_WAR_RANKING_CSB], 11);
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_WAR_RANKING_CSB]);
	auto b_close = view->getChildByName<Button*>("button_close");
	b_close->addTouchEventListener(CC_CALLBACK_2(UINationWarRanking::menuCall_func, this));

	auto panel_title = view->getChildByName<Widget*>("panel_title");
	auto t_ranking = panel_title->getChildByName<Text*>("label_last_war");
	auto b_info = view->getChildByName<Button*>("button_rank_info");
	b_info->addTouchEventListener(CC_CALLBACK_2(UINationWarRanking::menuCall_func, this));
	t_ranking->setPositionX((panel_title->getBoundingBox().size.width - t_ranking->getBoundingBox().size.width) / 2 + t_ranking->getBoundingBox().size.width / 2);
	b_info->setPositionX(t_ranking->getPositionX() + t_ranking->getBoundingBox().size.width / 2 + b_info->getBoundingBox().size.width);

	auto panel_list = view->getChildByName<Widget*>("panel_list");
	auto l_ranking = view->getChildByName<ListView*>("listview_ranking");
	l_ranking->removeAllChildrenWithCleanup(true);

	auto image_head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(view, "image_head_16"));
	auto t_rank_title = view->getChildByName<Text*>("label_you_rank");
	auto t_rank_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_you_rank_num"));
	auto t_damage_title = view->getChildByName<Text*>("label_you_score");
	auto t_exploit_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_you_score_num"));
	t_rank_num->setString("N/A");

	t_rank_num->setPositionX(t_rank_title->getBoundingBox().size.width + t_rank_title->getPositionX() + 10);
	t_exploit_num->setString("0");
	t_exploit_num->setPositionX(t_damage_title->getBoundingBox().size.width + t_damage_title->getPositionX() + 10);

	image_head->loadTexture(getPlayerIconPath(SINGLE_HERO->m_iIconidx));

	auto p_no = view->getChildByName<Widget*>("panel_centent_no");
	p_no->setVisible(true);

	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");
	button_pulldown->setPositionX(image_pulldown->getBoundingBox().size.width / 2 + 3);
	image_pulldown->setVisible(false);
	ProtocolThread::GetInstance()->getContributionRank();
	this->schedule(schedule_selector(UINationWarRanking::updateRanking), 3);
}
void UINationWarRanking::initRanking()
{
	auto view = getViewRoot(PALACE_COCOS_RES[VIEW_WAR_RANKING_CSB]);
	auto l_ranking = view->getChildByName<ListView*>("listview_ranking");
	auto p_no = view->getChildByName<Widget*>("panel_centent_no");
	auto image_pulldown = view->getChildByName<ImageView*>("image_pulldown");
	auto button_pulldown = image_pulldown->getChildByName<Button*>("button_pulldown");

	TableView* mTableView = dynamic_cast<TableView*>(l_ranking->getParent()->getChildByTag(1000));
	if (m_pContributionRankResult->n_rank_info <= 0)
	{
		p_no->setVisible(true);
		if (mTableView)
		{
			mTableView->reloadData();
		}
		return;
	}
	else
	{
		p_no->setVisible(false);
	}

	if (!mTableView)
	{
		mTableView = TableView::create(this, CCSizeMake(l_ranking->getContentSize().width, l_ranking->getContentSize().height));
		l_ranking->getParent()->addChild(mTableView,1000);
		mTableView->setVisible(true);
		mTableView->setDirection(TableView::Direction::VERTICAL);
		mTableView->setDelegate(this);
		mTableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
		mTableView->setPosition(l_ranking->getPosition());
		mTableView->setVisible(true);
		mTableView->setTag(1000);
		mTableView->setTouchEnabled(true);
		mTableView->setPosition(l_ranking->getPosition());
		mTableView->setUserObject(button_pulldown);
		mTableView->reloadData();
		auto cameras = Director::getInstance()->getRunningScene()->getCameras();
		if (cameras.size() >= 2){
			auto flag = (unsigned short)cameras[cameras.size() - 1]->getCameraFlag();
			mTableView->setCameraMask((unsigned short)cameras[cameras.size() - 1]->getCameraFlag(), true);
		}
	}
}

void UINationWarRanking::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		auto button = dynamic_cast<Widget*>(pSender);
		std::string name = button->getName();
		buttonEvent(button, name);
	}
}
void UINationWarRanking::buttonEvent(Widget* target, std::string name, bool isRefresh)
{
	SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);

	//国战排行榜的关闭
	if (isButton(button_close))
	{
		closeView();
		this->removeFromParentAndCleanup(true);
		return;
	}

	//解释说明-国战排行
	if (isButton(button_rank_info))
	{
		UICommon::getInstance()->openCommonView(this);
		UICommon::getInstance()->flushInfoBtnView("TIP_PALACE_WAR_RANKING_TITLE", "TIP_PALACE_WAR_RANKING_CONTENET");
		return;
	}

}
void UINationWarRanking::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	switch (msgType)
	{
	case PROTO_TYPE_GetContributionRankResult:
	{
		GetContributionRankResult *result = (GetContributionRankResult *)message;
		m_pContributionRankResult = result;
		if (result->failed == 0)
		{
			auto view = getViewRoot(PALACE_COCOS_RES[VIEW_WAR_RANKING_CSB]);
			auto t_rank_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_you_rank_num"));
			auto t_exploit_num = dynamic_cast<Text*>(Helper::seekWidgetByName(view, "label_you_score_num"));
			auto t_damage_title = view->getChildByName<Text*>("label_you_score");

			if (result->my_rank > 0)
			{
				if (result->my_rank > 10000)
				{
					t_rank_num->setString("No. 10000 +");
				}
				else
				{
					t_rank_num->setString(StringUtils::format("No. %d", result->my_rank));
				}
			}
			if (result->my_rank_info[0].score > 0)
			{
				t_exploit_num->setString(StringUtils::format("%d", result->my_rank_info[0].score));
				t_exploit_num->setPositionX(t_damage_title->getBoundingBox().size.width + t_damage_title->getPositionX() + 3);
			}
			initRanking();
		}
		else
		{
			
		}

		break;
	}
	default:
		break;
	}
}

/*
* 每隔3秒刷新排行榜数据
*/
void UINationWarRanking::updateRanking(float f)
{
	ProtocolThread::GetInstance()->getContributionRank();
}

void UINationWarRanking::scrollViewDidScroll(cocos2d::extension::ScrollView* view){
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

void UINationWarRanking::tableCellTouched(TableView* table, TableViewCell* cell)
{
	
}

Size UINationWarRanking::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	if (m_pContributionRankResult && m_pContributionRankResult->n_rank_info)
	{
		auto old_item = m_pRoot->getChildByName<Widget*>("panel_list");
		Size s = old_item->getContentSize();
		return s;
	}
	return Size(0, 0);
}

//返回cell的个数
ssize_t UINationWarRanking::numberOfCellsInTableView(TableView *table)
{
	if (m_pContributionRankResult && m_pContributionRankResult->n_rank_info)
	{
		return m_pContributionRankResult->n_rank_info;
	}
	return 0;
}

TableViewCell * UINationWarRanking::tableCellAtIndex(TableView *table, ssize_t idx)
{
	// 在table中取一个cell，判断是否为空，为空则创建新的。
	TableViewCell *cell = table->dequeueCell();
	Widget* cell_item;
	if (!cell)
	{
		cell = new TableViewCell();
		cell->autorelease();
		auto old_item = m_pRoot->getChildByName<Widget*>("panel_list");
		cell_item = old_item->clone();
		cell->addChild(cell_item, 1);
		cell_item->setPosition(Vec2(0, 0));
		auto cameras = Director::getInstance()->getRunningScene()->getCameras();
		if (cameras.size() >= 2){
			auto flag = (unsigned short)cameras[cameras.size() - 1]->getCameraFlag();
			cell_item->setCameraMask((unsigned short)cameras[cameras.size() - 1]->getCameraFlag(), true);
		}
	}
	else
	{
		cell_item = dynamic_cast<Widget*>(cell->getChildren().at(0));
	}
	cell->setTag(idx);
	cell->setVisible(true);
	cell_item->setSwallowTouches(false);

	auto t_rank = cell_item->getChildByName<Text*>("label_rank");
	auto i_trophy = cell_item->getChildByName<ImageView*>("image_trophy");
	auto i_nation = cell_item->getChildByName<ImageView*>("image_country");
	auto t_name = dynamic_cast<Text*>(Helper::seekWidgetByName(cell_item,"label_name"));
	auto t_lv = dynamic_cast<Text*>(Helper::seekWidgetByName(cell_item, "label_lv"));
	auto t_exploit_num = cell_item->getChildByName<Text*>("label_silver_num");
	i_trophy->setVisible(true);
	t_rank->setTextColor(Color4B(255, 255, 255, 255));
	t_rank->setString(StringUtils::format("%d", idx + 1));
	if (idx == 0)
	{
		i_trophy->loadTexture(RANK_GOLD);
	}
	else if (idx == 1)
	{
		i_trophy->loadTexture(RANK_SILVER);
	}
	else if (idx == 2)
	{
		i_trophy->loadTexture(RANK_COPPER);
	}
	else
	{
		i_trophy->setVisible(false);
		t_rank->setTextColor(LEFT_BUTTON_TEXT_COLOR_NORMAL);
	}
	//玩家国籍
	i_nation->loadTexture(getCountryIconPath(m_pContributionRankResult->rank_info[idx]->nation));
	//玩家名字	
	t_name->setString(m_pContributionRankResult->rank_info[idx]->name);
	//玩家等级	
	t_lv->setString(StringUtils::format("Lv.%d", m_pContributionRankResult->rank_info[idx]->level));
	//玩家功勋值
	t_exploit_num->setString(StringUtils::format("%d", m_pContributionRankResult->rank_info[idx]->score));

	return cell;
}
