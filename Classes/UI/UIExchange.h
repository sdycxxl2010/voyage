/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月22日
 *  Author:Sen
 *  The game exchange implementation class
 *
 */

#ifndef __EXCHANGE__LAYER__
#define __EXCHANGE__LAYER__

#include "UICommon.h"
#include "UIBasicLayer.h"
#include "GUI/CCScrollView/CCTableView.h"
USING_NS_CC_EXT;

class UIExchange : public UIBasicLayer, public cocos2d::extension::TableViewDelegate, public cocos2d::extension::TableViewDataSource
{
public:
	UIExchange();
	~UIExchange();
	static UIExchange* createExchange(); 
	bool init();
	/*
	*更新银币和v票并刷新界面
	*参数 gold:v票个数   silver:银币个数
	*/
	void updateCoinNum(const int64_t gold, const int64_t silver);
private:
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	/*
	*按钮点击事件(买、卖、订单里的事件)
	*/
	void flushEvent(Widget *target,std::string name);
	/*
	*按钮点击事件(买里的事件)
	*/
	void pageview_buttonEvent_1(std::string& name, Widget *target);
	/*
	*按钮点击事件(卖里的事件)
	*/
	void pageview_buttonEvent_2(std::string& name, Widget *target);
	/*
	*按钮点击事件(订单里的事件)
	*/
	void pageview_buttonEvent_3(std::string& name, Widget *target);
	/*
	*买的物品栏相应
	*/
	void buyBtnEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*卖的物品栏相应
	*/
	void sellBtnEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*订单物品的详情相应
	*/
	void itemInfoBtnEvent(Ref *pSender,Widget::TouchEventType TouchType); 
	/*
	*流行品价格切换相应
	*/
	void sellBtnEventPopular(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*订单物品栏相应
	*/
	void orderBtnEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*该城市中买和卖的货物相应
	*/
	void buyAndSellInfoBtnEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*物品的详情相应  
	*/
	void openGoodsBtnEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*关闭买卖成功的交互框相应
	*/
	void closeBuySellSuccessView(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage* message,int eMsgType);
	/*
	*更新买的本地数据
	*参数  result:买的数据
	*/
	void updateBuyView(const GetItemsBeingSoldResult *result);
	/*
	*更新卖的本地数据
	*参数  result:卖的数据
	*/
	void updateSellView(const GetPersonalItemResult *result);
	/*
	*更新订单的本地数据
	*参数  result:订单的数据
	*/
	void updateOrderView(const GetOrderListResult *result);
	void updateRecentOrderView(const GetRecentOrderResult *result);
	/*
	*延迟更新买界面
	*/
	void delayUpdateBuyView(const float fTime);
	/*
	*更新买界面
	*/
	template<class T>
	void updateView(const T *result);
	/*
	*物品详情
	*/
	void getCurrentItemInfo();
	/*
	*确认对话框
	*/
	void confirmEvent();
	/*
	*买货物时获取船只的容量
	*/
	float* getShipsCap();
	/*
	*订单物品的详情
	*/
	void openOrderOperate();
	void openRecentOrderOperate();
	/*
	*卖界面
	*/
	void openSellDialog();
	/*
	*买界面交互
	*/
	void distributeBuyEvent();
	/*
	*买物品的界面
	*/
	void openBuyDialog();
	/*
	*买船只、图纸和特殊物品界面
	*/
	void openBuyShipDialog();
	/*
	*买船只装备、人装备和道具界面
	*/
	void openBuyPropsDialog();
	/*
	*买货物时,更新上方的滑动条界面显示
	*参数  nNumber:个数
	*/
	void updateShipForNumber(int64_t nNumber);
	/*
	*买界面滑动条实现
	*参数  nNum:个数
	*/
	void buyingSliderEvent(const int64_t nNum);
	/*
	*买货物时，船只货物滑动条
	*参数 nTag:船只位置 nNum:个数
	*/
	void shipSliderEvent(const int nTag,const int nNum);
	/*
	*卖界面滑动条实现
	*参数  nNum:个数
	*/
	void sellingSliderEvent(const int64_t nNum);
	/*
	*提交买的物品数据
	*/
	void sendBuyResultToServer();
	/*
	*买物品成功界面
	*参数  result:买物品的数据
	*/
	void buyItemSuccess(const BuyItemResult *result);
	/*
	*卖物品成功界面
	*参数  result:买物品的数据
	*/
	void sellItemSuccess(const SellItemResult *result);
	/*
	*买单创建成功界面
	*/
	void createOrderBuySuccess();
	/*
	*卖单创建成功界面
	*/
	void createOrderSellSuccess();
	/*
	*本城市生产的货物
	*参数  result:生产货物的数据
	*/
	void cityBuyInfo(const GetCityProducesResult *result);
	/*
	*本城市求购的货物
	*参数  result:求购的货物的数据
	*/
	void citySellInfo(const GetCityDemandsResult *result);
	/*
	*更改上方按键的状态(买、卖、订单按钮)
	*参数 target:当前点击的按钮
	*/
	void changeMainButtonState(Widget *target);
	/*
	*更改左侧按键的状态
	*参数 target:当前点击的按钮
	*/
	void changeMinorButtonState(Widget *target);
	/*
	*界面滑动方法
	*/
	bool moveTo(Node *target,const Point &pos);
	void moveEnd(){ m_bMoveActionFlag = true;};
	/*
	*创建买订单选择界面
	*/
	void updateBuyOrderSelectedItemView();
	/*
	*创建卖订单选择界面
	*参数 result:个人物品数据
	*/
	void updateSellOrderSelectedItemView(GetItemsToSellResult *result);
	/*
	*更改选择界面左侧按钮的状态
	*参数 target:当前点击的按钮
	*/
	void changeOrderButtonState(Widget *target);
	/*
	*创建买订单
	*/
	void openBuyOrder();
	/*
	*创建卖订单
	*/
	void openSellOrder();
	/*
	*创建订单更改单价以及数量时内容更新
	*参数 num:个数
	*/
	void goodsNumberChangeEvent(int num);
	/*
	*提交订单相应
	*/
	void orderOkBtnEvent(Ref *pSender,Widget::TouchEventType TouchType);
	/*
	*订单输入相应
	*/
	void textEvent(Ref* target);
	/*
	*左侧按钮辅助条相应
	*/
	void touchButtonListview(Ref *pSender, ListViewEventType TouchType);
	/*
	*键盘判断
	*/
	/*
	*table view 的实现
	*/
	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(TableView *table);
	virtual void tableCellTouched(TableView* table, TableViewCell* cell);
	virtual void tableCellWillRecycle(TableView* table,TableViewCell* cell);
	virtual Size cellSizeForTable(TableView *table);
	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view);
	virtual void scrollEvent(Ref* target,ScrollviewEventType type);

	/*
	*买单
	*/
	void updateBuyOrderSelectedItemView(Widget *goodItem, int id);
	//item名字过长时，超出部分加“...”
	std::string vticItemName(std::string itemName);

	/*
	*左侧滚动提示
	*/
	void scrollButtonEvent(Ref *pSender, cocos2d::ui::ScrollView::EventType type);
	/*
	*弹出数字键盘
	*/
	void showNumpad(Text* inputNum);
	void movedEnd(){ m_bNumpadFlag = true; };
	/*
	*数字键盘调用的方法
	*/
	void buttonNumpadEvent(Ref *pSender, Widget::TouchEventType TouchType);
	/*
	*关闭键盘
	*/
	void closeNumpad();
private:	
	/*
	*买卖订单左侧button索引
	*/
	enum EX_BUTTON_INDEX
	{
		BUY_GOOD_INDEX = 632,
		SELL_GOOD_INDEX = 783,
		ORDER_BUY_INDEX = 818,
		RECENT_ORDER_BUY_INDEX = 122,
	};
	/*
	*The index of the Button (buy sell order)
	*/
	enum EX_PAGE
	{
		PAGE_BUY_INDEX,
		PAGE_SELL_INDEX,
		PAGE_ORDER_INDEX,
	};
	Text * m_inputNum;
	/*
	*购买物品的个数
	*/
	int64_t					m_nBuyNumber;
	/*
	*物品索引
	*/
	int						m_nItemIndex;
	/*
	*buy goods ,number control flag
	*/
	int64_t					m_nUnparadox;
	/*
	*判断界面是否在action
	*/
	bool					m_bMoveActionFlag;
	/*
	*总的船舱的容量
	*/
	float					m_fTotalCap;
	/*
	*总的货物的重量
	*/
	float					m_fTotalGoods;
	/*
	*是否是流行品
	*/
	bool					m_bIsSellPopular;
	/*
	*玩家等级是否升级
	*/
	bool					m_bIsLevelUp;
	/*
	*玩家声望是否升级
	*/
	bool					m_bIsPrestigeUp;
	/*
	*玩家船上船长是否升级
	*/
	bool					m_bIsCaptainUp;
	/*
	*当前主界面索引（买、卖、订单）
	*/
	EX_PAGE					m_nPageIndex;
	/*
	*记录主界面索引按钮(买、卖、订单)
	*/
	Widget                  *m_pMainButton;
	/*
	*记录主界面左侧索引按钮
	*/
	Widget                  *m_pMinorButton;
	/*
	*数据信息
	*/
	ProtobufCMessage		*m_pResult;
	/*
	*价格信息
	*/
	GetPriceDataResult		*m_pLastPriceResult;
	/*
	*是否是卖订单
	*/
	bool                    m_bIsSellOrder;
	/*
	*是否是市场的卖订单
	*/
	bool                    m_bIsRecentSellOrder;
	/*
	*选择物品界面索引
	*/
	Widget                 *m_pTempButton;
	/*
	*订定物品类型
	*/
	ITEM_TYPE               m_eType;
	/*
	*创建订单物品索引
	*/
	int                     m_nOrderIndex;
	/*
	*创建订单的个数
	*/
	int64_t                 m_nOrderAmount;
	/*
	*创建订单的价格
	*/
	int64_t                 m_nOrderPrice;
	/*
	*创建订单的税
	*/
	int64_t                 m_nOrderTax;
	/*
	*创建订单的消费300
	*/
	int64_t                 m_nOrderFee;
	/*
	*创建订单的天数
	*/
	int                     m_nOrderDay;
	/*
	*订单技能等级
	*/
	int                     m_nOrderSkillLv;
	/*
	*创建的订单数据
	*/
	GetOrderListResult     *m_pOrderlistResult;

	GetRecentOrderResult     *m_pRecentOrderResult;
	/*
	*购买时的物品的平均价格
	*/
	float                   m_nAverageCost;
	/*
	*卖物品时的个人物品数据
	*/
	SellItemResult		   *m_pSellItemResult;
	/*
	*买界面的滑动条
	*/
	ImageView				*m_pBuyListImagePulldown;
	/*
	*卖界面的滑动条
	*/
	ImageView				*m_pSellListImagePulldown;
	/*
	*TableView Cell
	*/
	Widget*					m_TableViewItem;
	/*
	*创建卖单时的个人物品数据
	*/
	GetItemsToSellResult	*m_pGetItemToSell;
	/*
	*创建买界面的id
	*/
	std::vector<int> m_vItemId;
	/*
	*技能图标的表达(每个界面显示时用到要清空)
	*/
	std::vector<SKILL_DEFINE> m_vSkillDefine;
	/*
	*键盘控制
	*/
	bool					m_bNumpadFlag;
};
#endif
