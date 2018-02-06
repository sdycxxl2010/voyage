/*
 * TaskManager.h
 *
 *  Created on: Feb 16, 2016
 *      Author: xijing
 */

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_
#include "cocos2d.h"

struct PARTSINFO;

enum ConditionType
{
	TYPE__UNKNOWN = 0,
	TYPE__MAKE_SHIP = 1,
	TYPE__HIRE_CAPTAIN = 2,
	TYPE__ASSIGN_CAPTAIN = 3,
	TYPE__REINFORCE_SHIP = 4,
	TYPE__WITHDRAW_MONEY = 5,
	TYPE__INVEST_PALACE = 6,
	TYPE__BUY_ITEM = 7,
	TYPE__HAS_SHIP = 8,
	TYPE__BEAT_NPC = 11,
};

enum CityAreaType
{
	AREA__MAINCITY = 0,
	AREA__MARKET = 1,
	AREA__SHIPYARD_FLEET = 2,
	AREA__TARVEN = 3,
	AREA__PALACE = 4,
	AREA__BANK = 5,
	AREA__DOCK = 6,
	AREA__SHIPYARD_BOSS = 7,
	AREA__SHIPYARD_REPAIR = 8,
	AREA__SHIPYARD_WORKSHOP = 9,

	AREA__NOWHERE = 255,
};

struct BoughtItem
{
	int m_itemtype;
	int m_itemid;
	int64_t m_itemamount;
};
struct TargetCity
{
	TargetCity()
	{
		m_targetId = 0;
		m_targetCity = 0;
		m_targetAmount = 0;
	}
	int m_targetId;
	int m_targetCity;
	int64_t m_targetAmount;
};

typedef std::function<bool (int,int)> ActionCallBack;//dialogId, flag

typedef enum _TASK_TYPE
{
	_TASK_NONE,
	_TASK_BUILD_SHIP,
	_TASK_HIRE_CAPTAIN,
	_TASK_CONFIG_CAPTAIN,
	_TASK_STRENGTHEN_SHIP,
	_TASK_BUY_INSURANCE,
	_TASK_INVESTMENT,
	_TASK_DELIVER_GOODS,
	_TASK_HAS_SHIP,
	_TASK_ACCEPT_SIDE_TASK,
	_TASK_PLAYER_CNT,
	_TASK_KILL_NPC,
	_TASK_FELLT_HAS_SHIP,
	_TASK_SALVAGE,
	_TASK_EXP,
	_TASK_ROB,
	_TASK_CITY_LICENSE,
	_TASK_COIN,
	_TASK_FINISH_SIDE_TASK,
	_TASK_FIND_CITY,
	_TASK_RARE,
	_TASK_PIRATE_ATASKACK_CITY,
	_TASK_REP,
	_TASK_HAS_COPANION,
	_TASK_INVESTMENT_COIN,
	_TASK_PIRATE_BASES,
	_TASK_MAYOR,

}TASK_TYPE;




#endif /* TASKMANAGER_H_ */
