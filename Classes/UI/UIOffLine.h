/*
*
*  CopyRight (c) ....
*  Created on: 2015年12月10日
*  Author:Chengyuan
*  离线委托界面
*
*/
#ifndef __OFFLINEDELEGATE_H
#define __OFFLINEDELEGATE_H

#include "UIBasicLayer.h"
#include"cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

//离线委托信息显示的Item个数
#define  RESULT_LOG_ITEM_NUM 2;
class UIOffLine : public UIBasicLayer,TableViewDataSource, TableViewDelegate
{
public:
	enum LOOT_PAGE
	{
		//离线委托信息
		PAGE_LOOT_LOG_INDEX= 1,
		//报复列表
		PAGE_LOOT_RESULT_LIST_INDEX,
	};
public:
	UIOffLine();
	~UIOffLine();
	bool init(int cameraMask);

	/*
	*打开打劫界面
	*parent: 父节点
	*cameramask:当前场景的cameramask
	*/
	static void openOffLineDelegateLayer(UIBasicLayer * parent,int cameramask = 1);
	static UIOffLine * create(int cameraMask);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	void callEventByName(std::string& name, Node* node);
	void onServerEvent(struct ProtobufCMessage* message, int msgType);

private:
	//显示离线委托信息
	void showdelegateResult();

	/*显示报复列表
	*result: 报复列表数据
	*/
	void showRevengeResult(GetLootPlayerLogResult * result );


	/*打开战斗界面
	*battledata : 战斗数据
	*/
	void openBattleView(EngageInFightResult *battledata);

	/*添加士气
	*view: 士气显示控件所在的父节点
	*num: 士气值
	*/
	void flushMoral(Widget* view, int num);

	//开始战斗
	void openFightEvent();

	/* 必须实现的函数 */
	// 当滑动tableview时触发该方法 参数为当前的tableview对象
	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view);

	// 当tableview被放大或缩小时触发该方法  参数为当前tableview对象
	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {};

	// 当cell被点击时调用该方法 参数为当前的tableview对象与被点击的cell对象
	virtual void tableCellTouched(TableView* table, TableViewCell* cell){};

	/* 设置tableview的Cell大小
	*table : 当前需要设置大小的tableview
	*idx : 当前设置的tableview的编号
	*/
	virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);

	/* 获取编号为idx的cell
	*table : 当前需要操作的tableView
	*idx : 当前初始化的tablecell的编号
	*/
	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);

	// 设置tableview中cell的个数
	virtual ssize_t numberOfCellsInTableView(TableView *table);

private:
	//打劫信息
	GetLootPlayerLogResult * m_delegateResult;
	//玩家登录信息
	EmailLoginResult* m_emailResultl;
	//当前复仇玩家信息
	LootLog * m_revengePlayerInfo;
	//打劫的战斗数据
	FindLootPlayerResult * m_lootPlayerInfo;
	//camera
	int m_cameraMask;
	//被打劫次数
	int m_lootTimes;
	//打劫列表数据
	std::vector<LootLog *> m_vLootList;
	//当前打开的界面
	int m_nPageIndex;
	//调用的layer
	UIBasicLayer *m_pParent;
};

#endif
