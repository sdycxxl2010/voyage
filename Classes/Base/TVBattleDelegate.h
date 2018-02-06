/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月30日
 *  Author:Sen
 *  battle delegate
 *
 */

#ifndef __BATTLE__DELEGATE__H__
#define __BATTLE__DELEGATE__H__

#include "cocos2d.h"
#include "EffectManage.h"
#include "AStar.h"
class TVBattleShareObj;
class SkillManage;
class UIBattleHUD;

class TVBattleDelegate
{
public:
	TVBattleDelegate(){};
	virtual ~TVBattleDelegate(){};
	virtual void specialButtonEvent(cocos2d::Node* target,std::string name,float varValue = 0) = 0; //按钮相应
	virtual void callbackEventByIndex(int tag){};				//AI模式切换
	virtual void usedSkills(int tag){};							//使用技能
	virtual void usedProps(int tag){};							//使用道具
	virtual void changeShipInfo(int skill_id,int tag = 0){};	//改变船只数据
	virtual CMap* getMap(){ return nullptr;};					//获取Map
	virtual TVBattleShareObj* getShareObj() { return nullptr;};	//战斗
	virtual EffectManage* getEffective(){ return nullptr;};		//战斗特效管理
	virtual SkillManage* getSkillManage() { return nullptr;};	//获取技能管理
	virtual UIBattleHUD* getBattleUI(){ return nullptr;};			//获取BattleUI
	virtual void startBattle(float dt){};							//战斗开始
	virtual void stopBattle(){};							//战斗暂停
	//攻击海盗基地时保存船只坐标
	virtual Vec2 getShipPos() { return nullptr; };
	virtual bool getContinueLastFight(){ return false; }
};

#endif 
