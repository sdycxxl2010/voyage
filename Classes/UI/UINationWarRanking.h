/*
*  Created on: 2016年04月19日
*  Author: zhangting
*  Descripion: 国战排行榜界面
*/
#ifndef __COUNTRYWAR_RANKING_H__
#define __COUNTRYWAR_RANKING_H__

#include "UIBasicLayer.h"
#include "login.pb-c.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;

class UINationWarRanking :public  UIBasicLayer, TableViewDataSource, TableViewDelegate
{
public:
	
	UINationWarRanking();
	~UINationWarRanking();
	bool init();
	void initStatic();
	static UINationWarRanking* createCountryWarRanking();

	/* 必须实现的函数 */
	// 当滑动tableview时触发该方法 参数为当前的tableview对象
	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view);
	// 当tableview被放大或缩小时触发该方法  参数为当前tableview对象
	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {};
	// 当cell被点击时调用该方法 参数为当前的tableview对象与被点击的cell对象
	virtual void tableCellTouched(TableView* table, TableViewCell* cell);
	// 设置tableview的Cell大小
	virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);
	// 获取编号为idx的cell
	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
	// 设置tableview中cell的个数
	virtual ssize_t numberOfCellsInTableView(TableView *table);
private:

	GetContributionRankResult	*m_pContributionRankResult;

	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void buttonEvent(Widget* target, std::string name, bool isRefresh = false);

	/*
	* 每隔3秒刷新排行榜数据
	*/
	void updateRanking(float f);

	void initRanking();
};
#endif
