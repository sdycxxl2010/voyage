/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月30日
 *  Author:Sen
 *  writer log
 *
 */

#ifndef __WRITER__LOG__
#define __WRITER__LOG__

#include "cocos2d.h"
#include "json/document.h"
#include "login.pb-c.h"
#include "network/HttpClient.h"

USING_NS_CC;
using namespace rapidjson;

class TVBattleShareObj;

//type ,id,pos,curid,tarid,propsid,sailors
struct ShipAction_FLAG
{
	int action_type;
	int shipid;
	Vec2 pos;
	int curId;
	int targetId;
	int propsId;
	int sailors;
};

enum ACTION_TYPE
{
	ACT_CHANGE_TARGET = 0,
	ACT_CHANGE_POS,
	ACT_ADD_BUFFER,
	ACT_REMOVE_BUFFER,
	ACT_USE_PROPS,
	ACT_CHANGE_STRATEGY,
	ACT_LOCK,
	ACT_UNLOCK,
	ACT_COLD_BATTLE,
	ACT_DIED,
};

//如果是敌人对自己，在此值的基础上加 100 eg. BATTLE_LOG_CANNON_ENEMY = 100
enum BATTLE_LOG_TYPE
{
	BATTLE_LOG_CANNON = 0,
	BATTLE_LOG_SAILOR = 1,
	BATTLE_LOG_SKILL = 2,
	BATTLE_LOG_CAPTAIN_VS = 3,
	BATTLE_LOG_SHIP_CRASH = 4,
};

class WriterLog
{
public:
	WriterLog();
	~WriterLog();
	static WriterLog* getInstance();
	/*
	*释放内存
	*/
	void release();
	/*
	*设置代理
	*/
	void setDelegate(TVBattleShareObj* shareObj);
	/*开始战斗的船只状态log
	*
	*/
	void addStartStates(EngageInFightResult* result);
	/*结束战斗的船只状态log
	*
	*/
	void addGameOverStates(EndFightResult* result);
	/*战斗的过程的log
	*
	*/
	void addActionLog(ShipAction_FLAG& headinfo);
	/*战斗的过程的log（占时没有用到）
	*
	*/
	void addActionLog_Main(ShipAction_FLAG& headinfo);
	
	//（占时没有用到）
	void readerJsonByName(std::string fileName);
	void saveToJson(rapidjson::Document value,std::string fileName);
	void onHttpRequestCompleted(network::HttpClient* client, network::HttpResponse* response);
	void setCallbackHttp(const network::ccHttpRequestCallback& callback);
	
	/*
	*提交数据
	*/
	void sendDataToServer();
	int64_t	getCurrentTimeUsev();
	//isSelfVictim 如果是敌人对己方造成的伤害，设置为true
	//火炮造成的伤害，参数依次为：己方shipid ，敌人shipid，伤害值，造成伤害的火炮iid，己方ship位置，敌人ship位置
	void addCannonBattleLogItem(int myShipId,int enemyShipId,int hurt,int cannonItemid,Vec2 myShipPos, Vec2 enemyShipPos,bool isSelfVictim);
	//
	//白刃战造成的水手死亡，参数依次为：己方shipid ，敌人shipid，敌人死亡的水手，己方死亡的水手
	void addSailorBattlelogItem(int myShipId,int enemyShipId,int killedEnemySailorNum,int killedSelfSailorNum);
	//isSelfVictim 如果是敌人对己方造成的伤害，设置为true
	//放技能造成的伤害，int skillId 技能id,int skillOwnerId 放技能的captainid或者玩家，victimShipIds 受伤的ship id 数组
	void addSkillBattleLogItem(int skillId,int skillOwnerId,int hurt,int*victimShipIds, int n_ship,bool isSelfVictim);
	//单挑取胜，isSelfVictim 敌人取胜则设置1
	void addCaptainVsBattleLogItem(int myShipId,int enemyShipId,int myCaptainId,bool isSelfVictim);

	//船沉了，参数reason：1 炮打沉，2水手死光，3，单挑，
	//isMyShip,如果是自己的船设置为1
	void addShipCrashBattleLogItem(int shipid,int isMyShip,int reason);

	BattleLog*getBattleLogItems();
private:
	network::ccHttpRequestCallback m_callbackNetwork;
	std::string m_key;
	std::string m_name;
	std::vector<const char*> m_deleteObj;
	int m_CurActionIndex;
	std::string m_Content;
	rapidjson::Document m_Root;
	std::vector<std::string> m_LogContent;
	TVBattleShareObj* m_pShareObj;
	std::vector<BattleLogItem*> battleLogs;
	BattleLog*m_battleLog;
};

#endif
