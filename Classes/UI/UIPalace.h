/*
*  CopyRight (c) 
*  Created on: 2014年10月12日
*  Author: xie 
*  description: palace or city hall
*/
#ifndef __PALACE_SCENE_H__
#define __PALACE_SCENE_H__

#include "UIBasicLayer.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;
class UIVoyageSlider;

class UIPalace : public UIBasicLayer, TableViewDataSource, TableViewDelegate
{
public:
	enum CONFIRM_INDEX_PALACELAYER
	{
		//接受好友邀请
		CONFIRM_INDEX_ACCEPT_ADDFRIEND,
		//拒绝
		CONFIRM_INDEX_REFUSAL_ADDFRIEND,
		//加好友
		CONFIRM_INDEX_STRANGER_ADDFRIEND,
		//加入黑名单
		CONFIRM_INDEX_BLOCK_ADDFRIEND,
		//移除好友
		CONFIRM_INDEX_REMOVEFRIEND,
		//好友拉黑
		CONFIRM_INDEX_FRIEND_BLOCK,
		//陌生人拉黑
		CONFIRM_INDEX_STRANGER_BLOCK,
		//好友不在线
		CONFIRM_INDEX_FRIEND_NOT_ONLINE,
		//邀请好友到公会
		CONFIRM_INDEX_SOCIAL_INVITE_GUILD,
		//移除黑名单
		CONFIRM_INDEX_BLOCK_REMOVE,
	};

public:
	UIPalace();
	~UIPalace();
	//初始化数据
	bool init();

	void notifyCompleted(int index) override;
	void doNextJob(int actionIndex);

	static UIPalace* createPalace();
	void onEnter();
	void onExit();
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	//点击事件
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType) override;
	void buttonEventByName(Widget* target, std::string name);

	//点击主button改变状态
	void changeMainButtonState(Widget*target);

	//卫兵动画及城市基本信息
	void initCityView();

	//箭头动画
	void anchorAction(float time);

	//点击列表中个人信息
	void ranklistInfo(Ref *pSender, Widget::TouchEventType TouchType);

	//office物品细信息
	void provideItemDetail(Ref *pSender, Widget::TouchEventType TouchType);

	//友好度信息点击
	void palacefriendValueFlag(Ref *pSender, Widget::TouchEventType TouchType);

	//键盘事件
	void investNumpadEvent(Ref *pSender, Widget::TouchEventType TouchType);

	//邀请好友到公会的点击事件
	void rankListPerInfoGUild(Ref *pSender, Widget::TouchEventType TouchType);

	//城市基本信息  result：王宫的接口数据
	void showPalaceInfo(const GetPalaceInfoResult* result);

	//办公室初始化  result：王宫的接口数据
	void officeChoose(const GetPalaceInfoResult *result);

	//办公室第一天的初始化  result：王宫的接口数据
	void firstDayChosse(const GetPalaceInfoResult *result);

	//选择发展方向  result：王宫的接口数据
	void firstDaybuttonChange(const GetPalaceInfoResult *result);

	//友好度显示  result：友好度信息
	void cityFriendshipShow(const GetFriendValueResult*result);

	//投资排行 result：排行榜数据
	void onRankView(const GetRankListResult *result);

	//投资界面初始化 result：投资信息
	void showInvestView(const GetCurrentInvestDataResult *result);

	//初始化最高投资者  result：投资信息
	void showCurrInvestTop(const GetCurrentInvestDataResult *result);

	//更新银币和V票
	void showMainCoin();

	//非市长的办公室界面  result：发展需要的物品
	void showAnnouncement(const GetDevGoodsInfoResult*result);

	//市长的办公室界面  result：发展需要的物品
	void showOfficeAnnouncement(const GetDevGoodsInfoResult*result);

	//滑动条事件
    void showSlider(const GetDevGoodsCountResult*result);
	void palaceSliderEvent(Ref*);
	void palaceSliderClose(Ref*);

	//办公室物品   result：提供物品的信息
	void showProvideRewards(const ProvideDevGoodsResult*result);

	//滑动条事件  num：选择的物品数量
	void  onPalaceSliderNumbers(const int num);
	//周期倒计时
	void timeShow(float t);

	//动画退出
	void palaceoutofscreen();
	
	//排行榜个人信息
	void showRanklistPerInfo(const int rankTag);

	//提供物品信息
	void showItemDetail(const int imageTag);
	//发展方向
	void showDeveloptext(const GetDevGoodsInfoResult *result);
	
	//商业许可证
	void showBussinessliceView();
	
	//办公室为空  panelDemand：存储发展物品的容器
	void showNogoodPanel(const Widget*panelDemand);

	//投资银币结果
	void showInvestCoinResult(const InvestCityResult*result);
	
	//流行品信息
	void showPalacePopularGood();
	//出现数字键盘
	void showPalaceNumPad();
	void moveEnd(){ m_bNumpadFlag = true; };

	//输入框展位字符透明度变化
	void update(float delta);

	/**
	*宫殿右侧按钮动画进场
	*/
	void buttonAction(float dt);
	/**
	*宫殿按钮向右移动动画
	*/
	void buttonMoveRight();
	/**
	*打开的界面动画
	*/
	void viewAction(Node * pSender,std::string viewPath);

	/**
	*界面关闭动画
	*/
	void viewCloseAction();
	/**
	*国战界面
	*/
	void showWar();
	/**
	*关于国战的按钮响应
	*/
	void warCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	/**
	*上一次战斗时的排行
	*/
	void showLastWarRanking();
	/**
	*发起国战
	*/
	void showStartWar(const GetEnemyNationResult *pResult);
	/**
	*国战的奖励和规则
	*/
	void showWarRuses();
	//投资列表

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
	/**
	*获取港口类型或者港口信息
	*int port 获取的内容 0港口类型  1港口信息 2港口类型对应json文件的key值
	*/
	std::string getPortTypeOrInfo(int port = 0);
	/*
	*text点击
	*/
	void textEvent(Ref* target, Widget::TouchEventType type);
	/*
	*首都城市港口加成的详细信息
	*/
	void openManufactureView(GetForceCityResult  *result);
private:
	/*
	 * 王宫状态
	 */
	enum				{
		NO_SELECTED,
		CITY_INFO,
		HALL_OF_HONOR,
		INVESTMENT,
		OFFICE,
		WAR,
	};
	int					m_State; //王宫状态
	bool					m_bIsofficePalace;//办公室标识
	bool					m_bIsPrestigeUp;//声望升级
	bool					m_bDialogOut;//对话退出
	bool					m_bNumpadFlag;//键盘控制
	int					m_nProvideIndex;//办公室物品的标识
	int					m_nProvideItemNum;//提供物品的数量
	int					m_nDevelopWay;//发展方向
	int					m_nInviteGuildTag;//排行榜邀请个人进工会
	int64_t					m_nInvestCoinNum;//投资数目
	std::string         m_padStirng;//键盘输入字符串

	GetPalaceInfoResult*				m_pPalaceResult;//存储王宫信息
	GetCurrentInvestDataResult*	m_pInvestResult;//存储投资结果
	GetRankListResult*					m_pRankResult;//存储排行
	GetDevGoodsInfoResult*		m_pDevGoodsInfo;//办公室物品信息
	GetDevGoodsCountResult*	m_pDevGoodCount;//提供物品数量
	ProvideDevGoodsResult*		m_pProvideGoodReward;//提供物品得奖励
	UIVoyageSlider*								m_pPalceSlider;//滑动条
	ImageView*m_pImageLight_effectPopulation;//人口发展方向图片
	ImageView*m_pImageLight_effectTrade;//交易发展方向图片
	ImageView*m_pImageLight_effectManuf;//制造发展方向图片
	Widget* m_preMainButton;//左侧当前按下的按钮
	UIBasicLayer  *m_parent;

	//当前好友操作的类型
	CONFIRM_INDEX_PALACELAYER m_confirmType;
	//当前点击的控件Tag值 用于添加/删除好友操作
	int                 m_operateWidgetTag;
	//当前点击的控件Tag值 用于势力详情/制造详情操作
	int                 m_operateTag;
	//排行榜界面是否开启
	bool                m_alreadyOpenView;
	//按钮是否做完动画
	bool                m_btnMoved;
	//当前显示的View;
	Node *              m_curView;
	bool                m_bussinessInfoMove;
	//界面动画时候做完
	bool                m_viewruancomplete;
	//主城信息界面时候打开
	bool                m_InfoViewalreadyOpen;
	//资源路径
	std::string         m_viewPath;
	/*
	*选择国战时记录选择哪个国家
	*/
	Widget			  * m_pChooseNation;
	//加载层
	UILoadingIndicator*m_loadingLayer;
	/*
	*国战准备阶段数据
	*/
	GetCityPrepareStatusResult *m_pWarPrepareResult;
};


#endif
