/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月01日
 *  Author:Sen
 *  battle main ship (旗舰实现 玩家控制的船只)
 *
 */

#ifndef __BATTLE__MAIN__SHIP__
#define __BATTLE__MAIN__SHIP__

#include "TVBasicShip.h"
#include "AStar.h"

class TVBattleMainShip : public TVBasicShip
{
public:
	TVBattleMainShip();
	~TVBattleMainShip();

	static TVBattleMainShip* createShip(TVBattleManager*manager,FightShipInfo* shipInfo, int sid, bool isFriend, int s = 1, int fightType = 0, int index = 0);
	void onEnter() override;
	void onExit() override;
	bool init();

	/*  Battle delegate class */
	void callbackEventByIndex(int tag) override;
	void specialButtonEvent(Node* target,std::string name,float varValue) override;
	
	void hurt(Node* bullt);    //炮弹击中船只伤害计算
	void changeStrategy();	   //更改进攻AI模式	
	void moveToTaraget(Vec2 targetPos);  //追击攻击目标船只
	void findTargetAgain();              //目标船只
	void touchForTarget(Vec2 targetPos); 
	void update(float delate) override;
	void shipDied() override;		//船只死亡
	void deleteBuffer(SkillData& skilldata);
	void startBattle();			//开始战斗
	void addLogAction(int type,float arg_1 = 0,int arg_2 = 0,Vec2 pos = Vec2(0,0)); //写入log
//	float getShipMoveSpeed();  //船只的速度
//	void checkExpediteSpeed(float delt); //加速度
	void battleFree();
	void battleSalvoShoot(float num,int tag);   //技能：齐射
	void moveAndRotation(Vec2 targetPos) override;  //移动或转弯
	void shipMove(Vec2 targetPos);			//船只移动
	void shipRotation(Vec2 targetPos);		//船只转弯
	void zeroSpeedRotaion();
	void zeroRotationEnd();				  
	void checkAttackDistanceIcon();		 //攻击范围
	void shipDiedAnimationEnd();		//船只死亡后将船只从界面上删除
	void shipMianDieAnimation();//玩家控制船的死亡动画
	int         slovId;
private:
	Vec2		m_TargetPos;
	Sprite		*m_pAttackDistanceIcon; //攻击范围
	Sprite		*m_pGreenCircle;  //攻击范围的绿圈

	bool m_bIsMoveEnd;		 //移动是否结束
	bool m_bIsRotationEnd;	//转弯是否结束
	bool m_bZeroSpeedRotation;
	bool m_bIsCenterX;      //旗舰是否在中心
	bool m_bIsCenterY;	 
	Point m_shipStartPostion; //旗舰开始的位置

	//是否有舰体改良技能
	bool  m_bIsMainShipSkill;
	//旗舰血量是否低于一半
	bool  m_bIsMainShipHPHalf;
};

#endif
