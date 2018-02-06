/*
*  Created on: 2016年04月12日
*  Author: zhangting
*  Descripion: 国战的海上登陆城市界面
*/
#ifndef __COUNTRYWAR_LAND_H__
#define __COUNTRYWAR_LAND_H__

#include "UIBasicLayer.h"
#include "login.pb-c.h"

class UINationWarLand:public  UIBasicLayer
{
public:
	
	UINationWarLand();
	~UINationWarLand();
	bool init();
	void initStatic(float f);
	static UINationWarLand* createCountryWarLand(GetCityStatusResult* result);
private:
	/*
	*修理
	*/
	enum REPAIRSHIP
	{
		NONE = 0,
		REPAIR_GOLD = 1,  //金币修理 一个船只
		REPAIR_GOLDS, //金币修理 所有船只
		REPAIR_COIN,  //银币修理
		REPAIR_NOT_GOLDS, //v票不足
	};
	//登陆时获取城市状态
	GetCityStatusResult		*m_pResultCityStatus;
	GetNationWarEntranceResult	*m_pResultNationWarEntrance;
	//获取舰队和船坞中船只船长装备的信息
	GetFleetAndDockShipsResult	*m_pResult_fleet;
	//快速招募水手的结果
	CalFastHireCrewResult *m_pFastHireCrewResult;

	//修理索引
	REPAIRSHIP     m_nShipRepairIndex;
	//按钮索引
	Widget	*m_pTempButton2;
	//城市沦陷
	Widget  *m_pFallen;
	//需要修理的船只数
	int		m_nNomalRepairNum;
	//快速招募水手的数量
	int m_nFastHireCrewNum;
	//国战倒计时
	int m_nWarTime;
	//初始化执行次数限制一次
	bool m_bIsFirstInit;

	int m_nConfirm;

	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void buttonEvent(Widget* target, std::string name, bool isRefresh = false);

	//刷新界面    
	void everySecondReflush(const float fTime);
	//检查更新国战耐久
	void updateWarNumber(const float fTime);						 

	//初始国战相关信息
	void initWarMessage();

	//实时刷新国战耐久
	void flushWarMessage();

	/*
	*打开补给界面
	*/
	void initDepotView();
	/*
	*打开修理界面
	*/
	void initRepairView();
	/*
	*打开雇佣水手界面
	*/
	void initHireSalorView();

	/*
	*更新银币和v票并刷新界面
	*参数 gold:v票个数   coin:银币个数
	*/
	void flushCoinInfo(int64_t coin, int64_t gold);

	/*
	*国战修理时舰队信息显示
	*result
	*/
	void flushWarPanelFleetInfo(GetFleetAndDockShipsResult* result);

	/*
	*初始化国战期间登陆时修理界面舰队每条船的信息
	*item:需要加载船只信息的按钮
	*shipDefine:船只信息
	*tag:船的编号
	*/
	void initWarFleetShipInfo(Widget* item, ShipDefine* shipDefine, int tag);

	/*
	*修理花费界面提示
	*/
	void updateRepairShipDialog();

	//水手快速补给滑动条事件
	void fastHireCrewSliderChange(Ref* obj, cocos2d::ui::SliderEventType type);
};
#endif
