/*
*
*  CopyRight (c) ....
*  Created on: 2016年04月07日
*  Author:Sen
*  国战场景管理(包含WarMapLayer、WarShip、WarUI)
*
*/

#ifndef __WAR__MANAGE__
#define __WAR__MANAGE__

#include "UIBasicLayer.h"
class UINationWarMap;
class UINationWarHUD;
class UINationWarShip;
class EffectManage;

//国战舰队的类型
enum SHIP_FLEET_TYPE 
{
	//初始化类型
	NO_SHIP_FLEET,
	//我的舰队
	MY_SHIP_FLEET,
	//其他玩家的舰队
	GHOST_SHIP_FLEET,
};

class WarManage : public UIBasicLayer
{
public:
	WarManage();
	~WarManage();
	bool init();
	static WarManage* createWarManage();
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void initf();

	UINationWarMap *getWarMapLayer();
	UINationWarHUD		*getWarUI();
	UINationWarShip		*getWarShip(int tag);
	EffectManage *getEffectManage();

	std::vector<UINationWarShip*>  m_vWarShip;

	/*
	*国战结束
	*/
	void countryWarStop();

	Layer* getLayer();

	/*
	*回合倒计时
	*/
	void updateBySecond(const float fTime);
	/*
	*获取回合数据
	*/
	NationWarBattleTurnResult * getBattleTurnResult();
private:
	UINationWarMap *m_pWarMap;
	UINationWarHUD		*m_pWarUI;
	UINationWarShip		*m_pWarShip;
	EffectManage *m_pEffectManage;
	/*
	*进入国战的初始化数据
	*/
	StartAttackCityResult  *m_pResult;
	/*
	*战斗是否结束
	*/
	bool					m_bIsWarStop;
	/*
	*每一回合的战斗数据
	*/
	NationWarBattleTurnResult * m_pBattleTurnResult;

	Layer						*m_pLayer;
	
	/*
	*每一回合计时
	*/
	int					m_nBattleTureTime;

};

#endif