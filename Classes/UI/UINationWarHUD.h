/*
*
*  CopyRight (c) ....
*  Created on: 2016年04月07日
*  Author:Sen
*  国战UI层
*
*/

#ifndef __WAR__UI__
#define __WAR__UI__

#include "UIBasicLayer.h"

class UINationWarHUD: public UIBasicLayer
{
public:
	UINationWarHUD();
	~UINationWarHUD();
	bool init(StartAttackCityResult  *pResult, UIBasicLayer* parent);
	static UINationWarHUD* createWarUI(UIBasicLayer* parent, StartAttackCityResult  *pResult);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);

	/*
	*退出国战倒计时
	*/
	void updateBySecond(const float fTime);
	/*
	*城市攻击舰队中的船只
	*参数  attack:攻击 position:船只在舰队中的位置(0-4)
	*/
	void cityAttackShip(int attack, int position);
	/*
	*设置舰队的船只耐久(血量)并更新UI
	*参数  position:船只在舰队中的位置(0-4)
	*/
	void updateShipDuable(int position);

	/*
	*舰队中的船只攻击城市
	*参数  attack:攻击
	*/
	void shipAttackCity(int attack);
	/*
	*设置主城的耐久(血量)并更新UI
	*参数
	*/
	void updateCityDuable(bool isAtt);

	/*
	*跑马灯实现
	*/
	void flushMainChat();

	/*
	设置城市和玩家自己的舰队的数据
	*/
	void setCityAndShipFleetInfo(CityStatus *pCityInfo, NationWarCharacterStatus *pShipFleetInfo);

	/*
	*国战结束
	*/
	void countryWarStop();

	/*
	*国家被攻破
	*/
	void cityDied();

	/*
	*个人信息界面
	*/
	void openUserView(GetUserInfoByIdResult *result);

	/*
	*国战提示
	*/
	void openTipsView(int index);

	/*
	*城市信息
	*/
	CityStatus *getCityStatus();

	/*
	*国战准备界面
	*/
	void changeLabel();

private:
	UIBasicLayer *m_pParent;
	/*
	*国战的时间
	*/
	int  m_nCountryWarTime;

	RichText   *m_pMainChat;
	/*
	*跑马灯显示
	*/
	bool  m_bIsFirstChat;  
	/*
	*跑马灯时间
	*/
	int   m_nShowChatTime;
	/*
	*m_mainChat中Item个数
	*/
	int   m_nChatItem;

	/*
	*刚进入战斗时的数据
	*/
	StartAttackCityResult  *m_pResult;
	/*
	*城市数据
	*/
	CityStatus *m_pCityInfo;
	/*
	*玩家自己的舰队数据
	*/
	NationWarCharacterStatus *m_pLastShipFleetInfo;
	NationWarCharacterStatus *m_pShipFleetInfo;

	/*
	*个人cid
	*/
	int				  m_nUserCid;

	/*
	*记录显示那一条
	*/
	int m_nLoadingNum;


	/*
	*确认交互框索引
	*/
	enum CONFIRM_INDEX_WAR
	{
		//初始化
		CONFIRM_INDEX_NONE,
		//玩家死亡
		CONFIRM_INDEX_DIED,
		//城市被攻破
		CONFIRM_INDEX_CITY_BREACHED,
		//国战结束
		CONFIRM_INDEX_WAR_END,
		//玩家自己主动退出
		CONFIRM_INDEX_QUIT,
		//玩家自己主动退出倒计时没有结束
		CONFIRM_INDEX_NOT_QUIT,
		//回合超时
		CONFIRM_INDEX_OVERTIME,
		//删除好友
		CONFIRM_INDEX_REMOVEFRIEND,
		//拉黑
		CONFIRM_INDEX_FRIEND_BLOCK,
		//拉黑陌生人
		CONFIRM_INDEX_STRANGER_BLOCK,
		//公会邀请
		CONFIRM_INDEX_SOCIAL_INVITE_GUILD,
		//加为好友
		CONFIRM_INDEX_STRANGER_ADDFRIEND,
		//移除黑名单
		CONFIRM_INDEX_BLOCK_REMOVE,
	};

	CONFIRM_INDEX_WAR  m_eConfirm;
};

#endif