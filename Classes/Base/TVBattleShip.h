/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月01日
 *  Author:Sen
 *  battle ship
 *
 */

#ifndef __BATTLE__SHIP__
#define __BATTLE__SHIP__

#include "TVBasicShip.h"

class TVBattleShip : public TVBasicShip
{
public:
	TVBattleShip();
	~TVBattleShip();
	static TVBattleShip* createShip(TVBattleManager*manager,FightShipInfo* shipInfo,int sid,bool isFriend,int s = 1, int fightType = 0, int index = 0);
	/* node class */
	bool init();
	virtual void onEnter() override;
	virtual void onExit() override;

	void startBattle();  //开始战斗
	void update(float delate) override;

	/* battleDelegate class */
	void callbackEventByIndex(int tag);
	void specialButtonEvent(Node* target,std::string name,float varValue);
	
	 
	void lockShip(bool isLock); //锁定船只
	void changeStrategy();	//更改进攻AI模式	
	void flowShipDispense(); 
	void hurt(Node* bullt);	//炮弹击中船只伤害计算 
	void getEnemyHurtPoints(int points[5]);  //敌舰的位置
	void clearHateByTag(int tag);  //清除敌舰的位置
	void findTargetAgain();	
	void addLogAction(int type,float arg_1 = 0,int arg_2 = 0,Vec2 pos = Vec2(0,0)); //战斗log
//	float getShipMoveSpeed(); //船只的速度

	void npcMove();		//npc 移动
	void npcRotation(); //npc 转向

	void battleFree();		//AI模式：自由战斗
	void battleFocus();		//AI模式：集中火力
	void battleProtected();	//AI模式：保护旗舰
	void shipDied();		//船只死亡
	void shipDiedAnimationEnd();//船只死亡后，从界面上删除

	void moveToFirePosition(Vec2 targetVec); // 计算坐标，移动到适合开火的位置
	void moveAndRotation(Vec2 targetPos) override; //船只移动与转弯
	void checkSelected();							//旗舰锁定目标的icon
	void onTouchEnded(Touch *touch, Event *unused_event); //锁定按钮

	void shipDieAnimation();//船死亡动画
	int getEnemySailors(); // 获得敌方所有船只水手
	int getFriendSailors(); //获得友方所有船只水手
	/*
	*设置主动白刃战参数
	*/
	void setChargeInitiative(bool isCharge);
private:
	int			m_gHurtPoints[5];
	Vec2		m_TargetPos;
	TVBasicShip	*m_pPreTarget; //上一次锁定的目标
	
	Sprite	*m_pEnable_touch;  //锁定目标（在攻击范围内）
	Sprite	*m_pDisable_touch; //锁定目标(没有在攻击范围内)
	//bool	m_bIsMoveEnd;	   //移动结束
	long last_cal_time; //上一次路径计算的时间
	/*
	*主动白刃战参数
	*/
	bool m_bIsChargeInitiative;
};

#endif
