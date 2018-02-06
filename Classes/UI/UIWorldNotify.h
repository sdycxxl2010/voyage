/*
*
*  CopyRight (c) ....
*  Created on: 2016年03月30日
*  Author:Chengyuan
*  游戏全局通知界面
*
*/
#ifndef __SYSINFOLAYER_H
#define __SYSINFOLAYER_H
#include "UIBasicLayer.h"
USING_NS_CC;
class UIWorldNotify : public UIBasicLayer
{
public:
	UIWorldNotify();
	~UIWorldNotify();
	bool init();
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	CREATE_FUNC(UIWorldNotify);
	void openSysInfoView(int time);
	void timeOff(float dt);
private:
	int m_nSysIndex;
};

#endif
