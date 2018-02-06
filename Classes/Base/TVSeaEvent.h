/*
*  CopyRight (c) 
*  Created on: 2014年11月15日
*  Author: 
*  description: 海上事件
*/
#ifndef __SEA_EVENT_H__
#define __SEA_EVENT_H__
#include "ProtocolThread.h"

enum SEA_EVENT_TYPE
{
	EV_SEA_NPC = 1,//npc
	EV_SEA_STORM,  //暴风雨
	EV_SEA_FLOAT,  //漂浮物
	EV_SEA_ROCK,  //触礁
	EV_SEA_BIG_FOG, // 大雾
	EV_SEA_BOSS,	//大boss
	EV_SEA_PESTIS, //鼠疫
	EV_SEA_HOMESICK,//思乡
	EV_SEA_SEPSIS, //败血病
};
class UINPCShip;

class TVSeaEvent 
{
public:
	TVSeaEvent();
	~TVSeaEvent();

	//bool m_IsCan;//海上npc是否可以触发战斗
	int m_Event_id;//海上事件id
	int m_Type;//海上事件的类型
	int m_Range;//海上事件触发范围
	int m_Rate;//海上事件触发概率
	int m_Days;//航行天数
	int m_X;//海上事件坐标
	int m_Y;
	UINPCShip*m_NPC;
	int m_NPCId;
	bool m_Wanted;
	bool m_IsFriend;
	int m_NPCLevel;
	bool shouldTriggerBattle();
	static TVSeaEvent* generateEvent(SeaEventDefine* se);//构造
	bool initEvent(SeaEventDefine* se);//根据海上事件类型初始化
};


#endif
