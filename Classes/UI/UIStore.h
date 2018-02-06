/*
*  CopyRight (c) 
*  Created on: 2015年3月25日
*  Author: xie 
*  description: something about shop ,include v,coin ,prop,transmit server
*/
#ifndef __VTICKET_STORE_H__
#define __VTICKET_STORE_H__
#include "UIBasicLayer.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;


class UIStore : public UIBasicLayer, TableViewDataSource, TableViewDelegate
{
public:
	enum VITICKET_TYPE
	{
		//V票
		VITICKET_TYPE_V_TICKET,
		//银币
		VITICKET_TYPE_COIN,
		//道具
		VITICKET_TYPE_V_PROP,
		//水手之家
		VITICKET_TYPE_SAILOR_HOUSE,
		//幸运宝箱
		VITICKET_TYPE_LUCKY_CHEST,
	};
	enum VITICKET_TYPE_1
	{
		VITICKET_ESCORT_SENIOR_ID = 10002,//高级护送Id
		VITICKET_ESCORT_PRIMARY_ID,//初级护送Id
		//低级掉落服务
		VITICKET_LOW_DROPS_ID,
		//高级掉落服务
		VITICKET_SENIOR_DROPS_ID,
	};
	enum VITICKET_SOME_TAG
	{
		//一次购买的最大数量
		ONCE_BUYITEM_MAX_NUM = 10,
		//在随机物品中，区分船装备的id
		EQUIP_TAG = 10000,
		//在随机物品中，用于区分船id,和物品id
		SHIP_TAG = 30000,
		BLUEPTINTS_TAG = 50000,
	};
	enum BLACK_MARKET_TABLEVIEW_TAG
	{
		TABLEVIEW_NONE,
		//全部物品的tableview
		TABLEVIEW_FOR_ALL,
		//分类物品的tableview
		TABLEVIEW_FOR_CATEGORY,
	};

public:
	static UIStore* GetInstance();
	UIStore();
	~UIStore();
	void onExit();
	void onEnter();
	static UIStore*  getInstance()
	{
		if (m_ticketStoreLayer == nullptr)
		{
			m_ticketStoreLayer = new UIStore;
		}
		return m_ticketStoreLayer;
	}
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);
    
	//购买按钮的点击事件
	void goodItem_callfunc(Ref *pSender,Widget::TouchEventType TouchType);

	//水手之家按钮事件
	void sailorHouse_callfunc(Ref *pSender, Widget::TouchEventType TouchType);

	//不同的场景打开商城  Ui_type：哪个场景，index：银币还是V票
	void openVticketStoreLayer(const int Ui_type,const int index);

	//不同场景下的商城 result：购买之后的接口数据
	void updateCoinGold(const BuyVTicketMarketItemResult*result);

	//更新物品面板  result：商城信息
	void updateStoreView(const GetVTicketMarketItemsResult*result);

	//购买结算  result：商城信息，btngoodTag：所购买的物品的index, tipIndex:掉落时提示用
	void showYoucost(const GetVTicketMarketItemsResult*result, const int btngoodTag, const int tipIndex = 0);

	//V票的更新 gold：当前的V票
	void updateGold(int64_t gold);

	//按钮改变 brightTag：当前的点击按钮
	void buttonBright(const int brightTag);

	//初始化固定物品的进度条 maxpropNum：购买的最大数量
	void initPropslider(const int maxpropNum);

	//初始化随机物品的选择条 maxpropNum：购买的最大数量
    void initPropsliderForRandomGoods(const int maxpropNum);

	//选择条事件
	void propSliderChange(Ref* obj, cocos2d::ui::Slider::EventType type);

	//道具详情
	void propItem_callfunc(Ref *pSender, Widget::TouchEventType TouchType);

	//护送服务的信息解释  escortId：初级护送还是高级护送
	void escortExplain(int escortId);

	//周期倒计时
	void vticketTimeSecondFresh(float t);

	//护送服务
	void comebackHome();

	//护送服务提示界面
	void comebackHomeEcsort();

    //--zhengqiang
	//黑市随机物品按钮的点击事件
	void randomGoodsItem_callfunc(Ref *pSender, Widget::TouchEventType TouchType); 

	//初始化随机物品选择条  maxpropNum：购买的最大数量
    void initPropsliderForRandomGoods2(const int maxpropNum);

    virtual void scrollEvent(Ref* target,ScrollviewEventType type);
    //--
	//护送动画
	void addEscortBlinkImage();

	//水手之家
	void updateSailorHouse();
	//幸运宝箱
	void updateLuckyChest();
	void changeCost();
	//护送 type：高级护送还是初级护送
	void escortDialog(int type);
	//走字
	void escortContentDialog(float t);

	//item名字过长时，超出部分加“...”
	std::string vticItemName(std::string itemName);
	//分类刷新黑市物品
	void RefreshItems(int sort);

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

	//黑市全部物品创建tableview
	void tableViewJustForBlackAllItem();
	//分类物品创建tableview
	void tableViewForcategory();
	//删除所有tableview
	void removeAllTableViews();
private:
	static UIStore* m_pInstance;
	int			V_Ui_type;//不同场景下的标识
	int			m_nBtngoodTag;//物品按钮的tag
	float			m_nPackagesize;//背包大小
	int			m_nMaxpropNums;//购买道具的最大量
	int			m_nBuyPropNum;//购买的数量
	Slider*		m_pSlider;//购买的滑动条
	Text*		m_pLabelNum;//数量显示
	static UIStore*	m_ticketStoreLayer;//单例对象
	GetVTicketMarketItemsResult*m_pGetItemResult;//保存服务器返回的商品信息
	Widget*m_button_Priescort;//护送服务的按钮
	bool m_bPrimarySerIsCooling;//初级服务是否在冷却
    //--zhengqiang
    int         m_nRandomGoodsBtnTag;//随机物品的btnTag
    int         m_nRandomGoodsType;//船只,船只装备,道具中的一种
    int         m_nRandomShipNum;//随机物品的船的数量
    int         m_nRandomEquipNum;//随机物品的装备数量
    int         m_nRandomItemNum;//随机物品的物品数量
    bool        m_bIsRandomGoods; //该物品是不是黑市里的随机物品
	bool        m_bTouchClassification;//点击分类按钮
    std::vector<BlackMarketRandomShipAndEquipDefine*> m_vRandomItems; //存储随机物品
	std::vector<VTicketMarketItemDefine*> m_vThingBlack;//分类存储物品
	std::vector<BlackMarketRandomShipAndEquipDefine*> m_vRandomRefreshItems; //存储分类随机物品
    //--
	Widget    *m_pButton;//当前按下的按钮
	GetVTicketMarketActivitiesResult *m_pMarketActivitiesResult;//活动接口数据

	int lenNum;//当前显示的长度
	bool m_bDiaolgOver;//对话是否播完
	Text		*chatTxt;//对话文本框
	std::string chatContent;//对话文本内容
	ImageView*anchPic;//对话箭头
	//刷新时间的文本
	Text * m_PTimeLabel;
	//大按钮的tag
	int m_pButtonTag;
	//关闭按钮的标识
	int   m_nConfirmIndex;
	//记录点击分类按钮索引
	int m_nBlackMarketSortTag;//0表示不操作任何操作
	//保存分类切换
	int m_nBlackStoreChange;
	BLACK_MARKET_TABLEVIEW_TAG m_tableview_tag;
};
#endif
