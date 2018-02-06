/*
*Created on: 2015年05月12日
*  Author: xie xingli
*  Descripion: 每日签到
*/
#ifndef __EVERYDAYSIGN_LAYER_H__
#define __EVERYDAYSIGN_LAYER_H__
//签到周期
#define ALLDAYS 30
//今日可签
#define CAN_SIGN 1
//不可签
#define CAN_NOT_SIGN
 //已经签
#define HAVE_SIGNED 3
#include "UIBasicLayer.h"
#include "login.pb-c.h"
class UIEveryDaySign:public  UIBasicLayer
{
public:
	
	UIEveryDaySign();
	~UIEveryDaySign();
	bool init();
	void initStatic(float f);
	static UIEveryDaySign* createEveryDaysign(int tag);
	//显示签到面板
	void updateDaysPanel();
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType) override;
	//物品详情
	void goodItem_callfunc(Ref *pSender,Widget::TouchEventType TouchType);
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	//签到的动画
	void todayAnimation(float f);
private:
	//签到的tag值
	int m_nBtnTag;
	int  signYesOrNoFlag[30];//保存每一天的状态值，是否可签
	int m_nTotalDays;//签到的总天数
	int m_nMainCityOrSeaTag;//标识值，是出现在海上还是主城
	Button*buttonClose;//关闭按钮
};
#endif
