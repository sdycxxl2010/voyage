/*
*  CopyRight (c)
*  Created on: 2016年4月6日
*  description: 国战
*/

#ifndef __COUNTRYWAR_LAYER_H__
#define __COUNTRYWAR_LAYER_H__
#include "UIBasicLayer.h"
class UINationWarEntrance : public UIBasicLayer
{
public:
	//正在使用数字键的tag
	enum NUM_PAD_TAG
	{
		M_USINGPAD_NONE,
		//雇佣军
		M_INVEST_COIN,
		//建造仓库
		M_INVEST_VTICKET,
		//准备阶段升级耐久
		M_UPGRADE_DURABLE,
		//准备阶段升级火力
		M_UPGRADE_GUN,
		//战斗阶段银币修复
		M_ONWARING_REPAIR_DURABLE_USE_COIN,
		//战斗阶段V快速修复
		M_ONWARING_REPAIR_DURABLE_USE_V,
		//战斗阶段增强火力
		M_ONWARING_SUPER_FIRE,
	};
	//点击大按钮的Tag
	enum MAIN_BUTTON_INDEX
	{
		N_BUTTON_NONE,
		N_BUTTON_DEFENSE,
		N_BUTTON_STATUE,
		N_BUTTON_RULES,
	};
public:
	UINationWarEntrance();
	~UINationWarEntrance();
	bool init();
	void onEnter();
	void onExit();
	static UINationWarEntrance* createCountryWarLayer();
	//初始化数据
	void initStatic(float f);
	/**
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage *message, int msgType);
	/**
	*按钮点击事件
	*/
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	//键盘事件
	void numPadEvent(Ref *pSender, Widget::TouchEventType TouchType);
	//战斗阶段防守界面点击事件
	void onWaringEvent(Ref *pSender, Widget::TouchEventType TouchType);

	//女郎进场
	void guardEnterScene(const float fTime);
	//女郎退场
	void guardExitScene(const float fTime);
	//大按钮状态
	void mainButtonChangeStatus(Widget *target);

   //准备阶段防守界面
	void openPrepareDefendPanel();
	//战斗阶段防守界面
	void openOnGoingWarPanel();
	//打开规则界面
	void openRulesPanel();
	

	//主界面移动
	void mainPageMoveEnd(Widget*view);
	//关闭运动完毕的界面
	void attackMoveEnd(){ m_bAttackMoveActionFlag = true; closeOtherView(0); };
	//关闭运动完毕的界面
	void closeOtherView(const float fTime);
	//更新V票银币
	void updateWarCoin(int64_t golds, int64_t coins);
	//显示数字键盘
	void showNumpad(Text*m_usingText);

	//一些倒计时刷新
	void everySecondflush(const float fTime);


	//准备阶段刷新耐久
	void flushPrepareDurable(AddCityRepairPoolResult*result);
	//准备阶段刷新攻击
	void flushPrepareGun(AddCityAttackPoolResult*result);
	//战斗阶段刷新耐久
	void flushOnWarDurable(RepairCityDefenseInWarResult *result);
	//战斗阶段刷新超级火力
	void flushOnWarGun(EnhanceAttackResult *result);


	//使用V票修复耐久
	void repairWallByVticket();
	//刷新界面
	void flushDefendPanelSeconds(const float fTime);

	//实时刷新玩家的分数、国战损失和战利品 
	void openWarstatus(GetNationWarCityLostResult* result);
	//更新右上角时间
	void flushPrepareOrWarTime(GetCityPrepareStatusResult *result);
	//花费确认框 0 花费银币，1花费V票
	void costResultPanle(const int64_t nums, int costType = 0);


private:
	/**
	*用于记录选择按钮(操作更改按钮的状态)
	*/
	Widget *m_pMainButton;
	/**
	*用于记录打开的界面(操作界面)
	*/
	Widget *m_pView;
	//界面是否运动完毕
	bool m_bAttackMoveActionFlag;
	//键盘输入字符串
	std::string         m_padStirng;
	//使用数字键盘的tag
	NUM_PAD_TAG m_usingNumPad;
	//处于准备阶段还是战斗阶段
	bool isOnWaring;
	//正在使用的数字键盘的label
	Text* m_UsingNumPadText;
	GetCityPrepareStatusResult*m_PrepareResult;
	//倒计时时间
	int64_t m_Times;
	//战斗阶段修复倒计时
	int m_repairAllTime;
	//战斗阶段增强活力倒计时
	int m_addGunAllTime;
	//大按钮
	MAIN_BUTTON_INDEX m_nMainbuttonTag;
	//准备状态切换成战斗状态
	bool m_bChangePrepareToWar;
	//加载层
	UILoadingIndicator*m_loadingLayer;

	std::vector<Widget*> m_vWigetInfo;

};
#endif
