/*
*  Created on: 2016年04月12日
*  Author: zhangting
*  Descripion: 国战的战争状态界面
*/
#ifndef __COUNTRYWAR_STATUS_H__
#define __COUNTRYWAR_STATUS_H__

#include "UIBasicLayer.h"
#include "login.pb-c.h"

class UINationWarStatus:public  UIBasicLayer
{
public:
	
	UINationWarStatus();
	~UINationWarStatus();
	bool init();
	void initStatic(float f);
	static UINationWarStatus* createCountryWarStatus();
private:

	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void buttonEvent(Widget* target, std::string name, bool isRefresh = false);

	//检查更新
	void updateAll(const float fTime);						 

	//实时刷新玩家的分数、国战损失和战利品 
	void flushWarScoreAndLossAndLooted(GetNationWarCityLostResult* result);

	std::vector<Widget*> m_vWigetInfo;
};
#endif
