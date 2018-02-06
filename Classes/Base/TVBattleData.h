/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月30日
 *  Author:Sen
 *  battle data 
 *
 */

#ifndef __BATTLE__DATA__H__
#define __BATTLE__DATA__H__

#include "TVBasic.h"

#define  BATTLETIP_UP "BATTLETIP_UP"
enum SHIP_TYPE //舰队类型
{
	MAIN_SHIP, //旗舰
	FLOW_SHIP, //其它舰
	NPC_SHIP,  //NPC舰 
};

enum SHIP_ACTION_MODEL
{
	MODEL_FREE = 1,
	MODEL_PROTECT,
	MODEL_FOCUS,
};

enum PROP_TAG
{
	PROP_FLEE_SMOKE = 39,
	PROP_NIORD_HORN = 40,
	PROP_WAR_DRUM = 41,
	PROP_CROSS = 42,
};

enum BUFF_STATE
{
	STATE_NONE = 0,
	STATE_FEAR,
	STATE_SLOWSPEED,
	STATE_FIRE,
	STATE_FREE,  
	STATE_DISORDER, 
	STATE_INSPIRE,  
	STATE_CRIT, 
	STATE_REPAIR = 13,
	STATE_DIED_REPAIR = 14,
	STATE_BATTLE_REPAIR = 15,
};

enum SKILL_STATE
{
	SKILL_NONE = 0,
	SKILL_CRIT = 1,
	SKILL_REPAIR = 13,
};
//战斗类型
enum FIGHT_TYPE
{
	//正常战斗
	FIGHT_TYPE_NORMAL = 0,
	//主线、小伙伴任务战斗
	FIGHT_TYPE_TASK = 1,
	//海盗攻城战斗
	FIGHT_TYPE_PIRATE_ATTACK = 2,
	//攻击海盗基地战斗
	FIGHT_TYPE_ATTACK_PIRATE = 3,
	//打劫或者复仇战斗
	FIGHT_TYPE_ROB = 4,
};

#define NPC_ID "NPC_ID"
#define MY_SHIP_SAILOR "MY_SHIP_SAILOR_%d"
#define ENEMY_SHIP_SAILOR "ENEMY_SHIP_SAILOR_%d"
#define MY_SHIP_BLOOD "MY_SHIP_BLOOD_%d"
#define ENEMY_SHIP_BLOOD "ENEMY_SHIP_BLOOD_%d"
#define FIGHT_TIME "FIGHT_TIME"
#define MY_SHIP_MORALE "MY_SHIP_MORALE"
#define ENEMY_SHIP_MORALE "ENEMY_SHIP_MORALE"
#define PROP_NUM "PROP_NUM_%d"
#define SKILL_NUM "SKILL_NUM_%d"
#define CAPTURINGSHIPID  "CAPTURING_SHIP_ID_%d"
#define FOODPLUNDERSHIPID  "FOOD_PLUNDER_SHIP_ID_%d"

struct Ship_info
{
	int _sid;	//id
	int _type;	//船只大中小
	int _tag;    //位置
	int _index;   //索引值
	int _attack;  //基础攻击
	int _defense;	//基础防御
	int _change_attack; //变化的攻击(技能或者其它的影响)
	int _change_defense;//变化的防御(技能或者其它的影响)
	int _sailornum; //水手数
	int _sailBeginNum;  // 白刃战时捕获的对手数量
	bool _isFriend;    //是否是友方
	int _extAttack;   //士气增加的攻击
	int _activeCritSize;   //主动技能增加的攻击
	int _critSize;         //弱点攻击概率
	int _isSlowSpeedTime;  //skill_id:4
	int _isFireEfficeharm; //skill_id:3
	int _attack_accuracy; //火炮射击精准修正发生的概率
	int _captureNum;  // 白刃战时捕获的对手数量
	int _bowgun_attack; //撞角的攻击

};

enum FIGHTING_CENTERL_TIP_TYPE
{
	//船只沉没
	TIP_SHIP_SUNK,
	//技能(现在没有这个)
	TIP_SKILL,
	//单挑开始
	TIP_SINGLED_START,
	//单挑结果
	TIP_SINGLED_END,
};

struct FIGHTING_CENTER_TIP_DATA
{
	int succSid;
	int succPos;
	int failSid;
	int failPos;
	int skillId;
	//胜利--单挑小伙伴的icon id
	int succ_proto_id;
	//胜利--单挑小伙伴是否是船长 0:不是船长 1:是船长 2:玩家icon 3:海盗
	int succ_is_captain;
	//失败--单挑小伙伴的icon id
	int fail_proto_id;
	//失败--单挑小伙伴是否是船长 0:不是船长 1:是船长 2:玩家icon 3:海盗
	int fail_is_captain;
	FIGHTING_CENTERL_TIP_TYPE fighting_type;
};

const cocos2d::Vec2 BATTLE_MAP_SIZE = cocos2d::Vec2(2400,1440);
const int MAX_Z_ORDER = 5000;
const int BATTLE_SCALE_RATE = 3;

const std::string ENABLE_NOT_ICON = "ship/selected.png";
const std::string ENABLE_ATK_ICON = "ship/target_atk.png";

const std::string FRIEND_SHIP_POS_FLAG = "ship/radar_a_allied.png";
const std::string ENEMY_SHIP_POS_FLAG = "ship/radar_a_enemy.png";
const std::string ENEMY_SHIP_POS_FLAG_BRI = "ship/radar_enemy.png";
const std::string FRIEND_SHIP_POS_FLAG_BRI = "ship/radar_allied.png";
const std::string MAINE_POS_FLAG = "ship/radar_mainship.png";
const std::string ATTACK_RANGE_MAX = "ship/range_3.png";
const std::string ATTACK_RANGE_MID = "ship/range_2.png";
const std::string ATTACK_RANGE_MIN = "ship/range_1.png";
const std::string ATTACK_RANGE_GREEN = "ship/green-circle.png";

const std::string BUFFER_4 = "ship/buffer/buff_%d.png";
const std::string PROPS_IMG_5[] = {"ship/buffer/item_retreat.png","ship/buffer/item_slow.png","ship/buffer/item_moral.png",
	"ship/buffer/item_terrify.png","ship/buffer/item_confuse.png"};

//修理组件id
#define REPAIR_MODULE_ID  301

#endif 
