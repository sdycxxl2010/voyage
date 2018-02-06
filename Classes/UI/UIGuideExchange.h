/*
*
*  CopyRight (c) ....
*  Created on: 2015年06月22日
*  Author:Sen
*  The game exchange implementation class
*
*/

#ifndef __EXCHANGEGUIDE__LAYER__
#define __EXCHANGEGUIDE__LAYER__

#include "UIBasicLayer.h"

class UIGuideExchange : public UIBasicLayer
{
public:
	//买卖订单左侧button索引
	enum EX_BUTTON_INDEX
	{
		//买
		BUY_GOOD_INDEX = 632,
		//卖
		SELL_GOOD_INDEX = 783,
		//订单
		ORDER_BUY_INDEX = 818,
	};

	//The index of the Button (buy sell order)
	enum EX_PAGE
	{
		//购买界面
		PAGE_BUY_INDEX,
		//卖出界面
		PAGE_SELL_INDEX,
		//订单界面
		PAGE_ORDER_INDEX,
	};


	//引导进度
	enum EXCHANGEGUIDE
	{
		//交易所任务开始
		SELL_TASK_BEGIN =1,
		//点击交易按钮
		CLICK_BTN_SELL,
		//任务需卖掉的物品
		FOUC_SELL_GOODS,
		//确认卖出
		CONFIRM_SELL_GOODS,
		//返回城市
		BACK_TO_CITY,

	};
	//购买物品引导进度
	enum EXCHANGE_BUY_GUIDE
	{
// 		//交易所任务开始
// 		BUY_TASK_BEGIN ,
		//点击交易按钮
		CLICK_BTN_BUY = 1,
		//任务需买的物品
		FOUC_BUY_GOODS ,
		//确认卖出
		CONFIRM_BUY_GOODS,
		//返回城市
        BUY_BACK_TO_CITY,
	};
	enum EXCHANGE_CONFIRM_VIEW
	{
		//确认买的结算界面
		EXCHAGE_CONFIRM_BUY_VIEW = 1,
		//确认卖的结算界面
		EXCHAGE_CONFIRM_SELL_VIEW,
	};
public:
	UIGuideExchange();
	~UIGuideExchange();
	static UIGuideExchange* createExchange();
	bool init();

	//默认的button相应 
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;  

	//卖的物品栏相应
	void sellBtnEvent(Ref *pSender, Widget::TouchEventType TouchType); 
	//买物品界面的点击事件
	void buyBtnEvent(Ref *pSender, Widget::TouchEventType TouchType);

	//关闭买卖成功的交互框相应
	void closeBuySellSuccessView(Ref *pSender, Widget::TouchEventType TouchType); 

	//接收服务器信息
	void onServerEvent(struct ProtobufCMessage* message, int eMsgType);
	
	/**
	*任务点击按钮提示
	*@parm pSender 任务下一步需要点击的按钮
	*/
	void focusOnButton(Node * pSender);


	/**
	*设置需要开启点击事件的按钮
	*@param node 按钮父节点
	*@param btn_name 按钮名
	*/
	void setButtonsDisable(Node * node, std::string btn_name = "");
	
private:
	/*
	*更新买的本地数据
	*result:交易所卖出的物品信息
	*/
	void updateBuyView(const GetItemsBeingSoldResult *result);  
	/*
	*更新卖的本地数据
	*result:可以交易的物品信息
	*/
	void updateSellView(const GetPersonalItemResult *result);   
	//延迟更新买界面
	void delayUpdateBuyView(const float fTime);

	template<class T>
	//更新界面
	void updateView(const T *result);   

	//确认对话框
	void confirmEvent();	

	//Open the Sell interface
	void openSellDialog();				
	
	/*
	*买物品的界面
	*/
	void openBuyDialog();
	/*
	* 卖界面滑动条实现
	* nNum:卖出的物品个数 
	*/
	void sellingSliderEvent(const int64_t nNum);  

	/*
	*买界面滑动条实现
	*参数  nNum:个数
	*/
	void buyingSliderEvent(const int64_t nNum);
	/*
	*更新银币和v票并刷新界面
	*coin : v票数量
	*silver : 银币数量
	*/
	void updateCoinNum(const int64_t coin, const int64_t silver);  
	/*
	*更改上方按键的状态
	*target : 被选中的上方按钮
	*/
	void changeMainButtonState(Widget *target);   
	/*
	*更改左侧按键的状态
	*target  : 被选中的左侧按钮
	*/
	void changeMinorButtonState(Widget *target);   

	/**
	*新手引导
	*/
	void guide();

	/**
	*新添加引导
	*/
	void partOneGuide(int guidestage);

	/**
	*初始化本地数据
	*/
	void initLocalData();

	/**
	*新手引导购买物品界面
	*/
	void openGuideBuyView();
private:

	//物品索引
	int						m_nItemIndex;	
	//当前主界面索引（买、卖、订单）
	int						m_nPageIndex;		
	//主界面索引按钮
	Widget                  *m_pMainButton;  
	//主界面索引按钮
	Widget                  *m_pMinorButton;         
	//信息
	ProtobufCMessage		*m_pResult;
	//价格信息
	GetPriceDataResult		*m_pLastPriceResult;   
	//选择物品界面索引
	Widget                 *m_pTempButton;
	//创建订单物品索引
	int                     m_nOrderIndex;
	//购买时的物品的平均价格
	float                   m_nAverageCost;    
	// 任务指引小手
	Sprite                 *sprite_hand;
	//任务进度
	int                    m_guideStage;
	//引导下需要卖出道具数量
	int                    m_guideCount;
	//道具类型
	int                    m_guideGoodType;
	//道具种类
	int                    m_guideKinds; 
	//道具ID
	int                    m_guideId;
	//道具价格
	float                  m_guidePrice;
	//是不是流行道具
	int                    m_guidePopular;
	//上个城市的道具价格
	float                  m_lastCityPrice;
	//税率
	float                  m_tax;
	//利润
	int                    m_receiveCoin;
	//重量
	int                  m_guideWeight;
	//花费的银币
	int                  m_costCoin;
	//当前确认界面
	int                  m_confirmViewIndex;
};
#endif
