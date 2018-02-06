#include "WriterLog.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "cocostudio/DictionaryHelper.h"
#include "TVBattleShareObj.h"
#include "TVBasicShip.h"
#include "zlib.h"
#include "env.h"
#include "ServerManager.h"

USING_NS_CC;
using namespace cocostudio;

static WriterLog* writerlog = nullptr;

#define ENABLE_BATTLE_LOG 0

WriterLog::WriterLog()
	:m_pShareObj(nullptr)
{
	m_Root.SetArray();
	m_CurActionIndex = 0;
	m_battleLog = 0;
}

WriterLog::~WriterLog()
{
	for (int i = 0; i < m_deleteObj.size(); i++)
	{
		delete []m_deleteObj[i];
	}
	if(m_battleLog) battle_log__free_unpacked(m_battleLog,0);
}

WriterLog* WriterLog::getInstance()
{
	if (!writerlog)
	{
		writerlog = new WriterLog;
	}
	return writerlog;
}

void WriterLog::release()
{
	delete writerlog;
	writerlog = nullptr;
}

void WriterLog::setDelegate(TVBattleShareObj* shareObj)
{
	m_pShareObj = shareObj;
}

void WriterLog::readerJsonByName(std::string fileName)
{
	std::string jsonpath;
 	jsonpath = cocos2d::FileUtils::getInstance()->fullPathForFilename(fileName);
 	
 	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
 	m_Root.Parse<0>(contentStr.c_str());
	
 	if (m_Root.HasParseError())
 	{
 		CCLOG("GetParseError %s \n",m_Root.GetParseError());
 	}
}

void WriterLog::saveToJson(rapidjson::Document value,std::string fileName)
{
	////using namespace rapidjson;
	//StringBuffer buffer;
	//Writer<StringBuffer> w(buffer);
	//value.Accept(w);
	//CCLOG("--%s",buffer.GetString());
}

char *copyString(const char*p)
{
	int len = strlen(p);
	char *res = new char[len+1];
	strcpy(res,p);
	return res;
}

void WriterLog::addStartStates(EngageInFightResult* result)
{
	m_CurActionIndex = 0;
	m_key = result->key;
	m_name = result->filename;
#if !ENABLE_BATTLE_LOG
	return;
#endif
	auto& allocc = m_Root.GetAllocator();
	rapidjson::Value startResult(rapidjson::kObjectType);

	rapidjson::Value v(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	startResult.AddMember("time",v,allocc);

	rapidjson::Value cannons_1(rapidjson::kArrayType);
	rapidjson::Value cannons_2(rapidjson::kArrayType);
	
	for (int i = 0; i < result->n_myships;i++)
	{
		rapidjson::Value oneShipResult(rapidjson::kObjectType);
		FightShipInfo* shipInfo = result->myships[i];
	
		const char* skey = String::createWithFormat("%d",i + 1)->getCString();
		oneShipResult.AddMember("tag",i + 1,allocc);
		oneShipResult.AddMember("id",shipInfo->sid,allocc);
		oneShipResult.AddMember("curhp",shipInfo->current_hp,allocc);
		oneShipResult.AddMember("maxhp",shipInfo->max_hp,allocc);
		oneShipResult.AddMember("sailors",shipInfo->sailornum,allocc);
		oneShipResult.AddMember("defense",shipInfo->defense,allocc);
		oneShipResult.AddMember("speed",shipInfo->speed,allocc);
		oneShipResult.AddMember("steer",shipInfo->steer_speed,allocc);
		skey = copyString(skey);
		m_deleteObj.push_back(skey);
		startResult.AddMember(skey,oneShipResult,allocc);
		
		rapidjson::Value cannons_1_1(rapidjson::kArrayType);
		for (int j = 0; j < shipInfo->n_cannons;j++)
		{
			rapidjson::Value cannons_1_1_1(rapidjson::kObjectType);
			cannons_1_1_1.AddMember("id",shipInfo->cannons[j]->cannonid,allocc);
			cannons_1_1_1.AddMember("attack",shipInfo->cannons[j]->attack,allocc);
			cannons_1_1_1.AddMember("range",shipInfo->cannons[j]->range,allocc);
			cannons_1_1.PushBack(cannons_1_1_1,allocc);
		}
		cannons_1.PushBack(cannons_1_1,allocc);
	}

	
	for (int i = 0; i < result->n_enemyships;i++)
	{
		FightShipInfo* shipInfo = result->enemyships[i];
		rapidjson::Value oneShipResult(rapidjson::kObjectType);
		
		const char* skey = String::createWithFormat("%d",i + 6)->getCString();
		oneShipResult.AddMember("tag",i + 6,allocc);
		oneShipResult.AddMember("id",shipInfo->sid,allocc);
		oneShipResult.AddMember("curhp",shipInfo->current_hp,allocc);
		oneShipResult.AddMember("maxhp",shipInfo->max_hp,allocc);
		oneShipResult.AddMember("sailors",shipInfo->sailornum,allocc);
		oneShipResult.AddMember("defense",shipInfo->defense,allocc);
		oneShipResult.AddMember("speed",shipInfo->speed,allocc);
		oneShipResult.AddMember("steer",shipInfo->steer_speed,allocc);
		skey = copyString(skey);
		m_deleteObj.push_back(skey);
		startResult.AddMember(skey,oneShipResult,allocc);
	
		rapidjson::Value cannons_2_2(rapidjson::kArrayType);
		for (int j = 0; j < shipInfo->n_cannons;j++)
		{
			rapidjson::Value cannons_2_2_2(rapidjson::kObjectType);
			cannons_2_2_2.AddMember("id",shipInfo->cannons[j]->cannonid,allocc);
			cannons_2_2_2.AddMember("attack",shipInfo->cannons[j]->attack,allocc);
			cannons_2_2_2.AddMember("range",shipInfo->cannons[j]->range,allocc);
			cannons_2_2.PushBack(cannons_2_2_2,allocc);
		}
		cannons_2.PushBack(cannons_2_2,allocc);
	}

	startResult.AddMember("mycannon",cannons_1,allocc);
	startResult.AddMember("enemycannon",cannons_2,allocc);
	m_Root.PushBack(startResult,allocc);

}

void WriterLog::addGameOverStates(EndFightResult* result)
{
#if !ENABLE_BATTLE_LOG
	return;
#endif
	if (!m_pShareObj)
	{
		return;
	}
	rapidjson::Value overResult(rapidjson::kObjectType);
	auto& allocc = m_Root.GetAllocator();
	auto& enemys = m_pShareObj->m_vEnemys;
	auto& friends = m_pShareObj->m_vMyFriends;
	rapidjson::Value v(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	overResult.AddMember("time",v,allocc);
	overResult.AddMember("supply",result->addedsaildays,allocc);
	//overResult.AddMember("iswin",true,allocc);
	overResult.AddMember("lootcoins",result->lootcoins,allocc);
	overResult.AddMember("prestige",result->lootfame,allocc);
#if 1
	for(int i=0;i<result->n_lootitemsarray;i++){
		rapidjson::Value goodInfo(rapidjson::kObjectType);
		goodInfo.AddMember("id",result->lootitemsarray[i]->itemid,allocc);
		goodInfo.AddMember("count",result->lootitemsarray[i]->count,allocc);
		goodInfo.AddMember("type",result->lootitemsarray[i]->itemtype,allocc);
		overResult.AddMember("items",goodInfo,allocc);
	}
#else
	if (result->lootgoods)
	{
		rapidjson::Value goodInfo(rapidjson::kObjectType);
		goodInfo.AddMember("id",result->lootgoods->itemid,allocc);
		goodInfo.AddMember("count",result->lootgoods->count,allocc);
		goodInfo.AddMember("type",result->lootgoods->itemtype,allocc);
		overResult.AddMember("good",goodInfo,allocc);
	}
	if (result->lootitems)
	{
		rapidjson::Value itemInfo(rapidjson::kObjectType);
		itemInfo.AddMember("id",result->lootitems->itemid,allocc);
		itemInfo.AddMember("count",result->lootitems->count,allocc);
		itemInfo.AddMember("type",result->lootitems->itemtype,allocc);
		overResult.AddMember("items",itemInfo,allocc);
	}
	if (result->lootprops)
	{
		rapidjson::Value propsInfo(rapidjson::kObjectType);
		propsInfo.AddMember("id",result->lootprops->itemid,allocc);
		propsInfo.AddMember("count",result->lootprops->count,allocc);
		propsInfo.AddMember("type",result->lootprops->itemtype,allocc);
		overResult.AddMember("items",propsInfo,allocc);
	}
#endif
	for (int j = 0; j < friends.size();j++)
	{
		TVBasicShip* ship = friends[j];
		auto* info = ship->m_Info;
		const char* shipInfo = String::createWithFormat("tag:%d,hp:%d,sailor:%d",info->_tag,ship->getBlood(),info->_sailornum)->getCString();
		const char* skey = String::createWithFormat("%d",j + 1)->getCString();
		overResult.AddMember(skey,shipInfo,allocc);
	}
	for (int i = 0; i < enemys.size(); i++)
	{
		TVBasicShip* ship = enemys[i];
		auto* info = ship->m_Info;
		const char* shipInfo = String::createWithFormat("tag:%d,hp:%d,sailor:%d",info->_tag,ship->getBlood(),info->_sailornum)->getCString();
		const char* skey = String::createWithFormat("%d",i + 6)->getCString();
		overResult.AddMember(skey,shipInfo,allocc);
	}
	m_Root.PushBack(overResult,allocc);
}

void WriterLog::addActionLog(ShipAction_FLAG& headinfo)
{
#if !ENABLE_BATTLE_LOG
	return;
#endif
	int type = headinfo.action_type;
	auto& allocc = m_Root.GetAllocator();
	m_CurActionIndex++;
	rapidjson::Value target(kObjectType);
	target.AddMember("type",type,allocc);
	target.AddMember("id",headinfo.curId,allocc);
	rapidjson::Value v(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	target.AddMember("time",v,allocc);
	switch (type)
	{
	case ACT_CHANGE_TARGET:
		{
			target.AddMember("targetid",headinfo.targetId,allocc);
			break;
		}
	case ACT_CHANGE_POS:
		{
			target.AddMember("x",headinfo.pos.x,allocc);
			target.AddMember("y",headinfo.pos.y,allocc);
			break;
		}
	case ACT_ADD_BUFFER:
		{
			target.AddMember("buffer",headinfo.propsId,allocc);
			break;
		}
	case ACT_REMOVE_BUFFER:
		{
			target.AddMember("removeBuffer",headinfo.propsId,allocc);
			break;
		}
	case ACT_USE_PROPS:
		{
			target.AddMember("props",headinfo.propsId,allocc);
			break;
		}
	case ACT_LOCK:
		{
			target.AddMember("lock",headinfo.shipid,allocc);
			break;
		}
	case ACT_UNLOCK:
		{
			target.AddMember("unlock",headinfo.shipid,allocc);
			break;
		}
	case ACT_COLD_BATTLE:
		{
			target.AddMember("cold",headinfo.targetId,allocc);
			break;
		}
	case ACT_DIED:
		{
			target.AddMember("died","died",allocc);
			break;
		}
	default:
		break;
	}
//	std::string s_key = StringUtils::format("%d",m_CurActionIndex);
	m_Root.PushBack(target,allocc);
}

void WriterLog::addActionLog_Main(ShipAction_FLAG& headinfo)
{
#if !ENABLE_BATTLE_LOG
	return;
#endif
	auto& allocc = m_Root.GetAllocator();
	rapidjson::Value target(kObjectType);
	m_CurActionIndex++;
	int type = headinfo.action_type;

	switch (type)
	{
	case ACT_CHANGE_TARGET:
		{
			target.AddMember("targetid",headinfo.targetId,allocc);
			break;
		}
	case ACT_CHANGE_POS:
		{
			target.AddMember("x",headinfo.pos.x,allocc);
			target.AddMember("y",headinfo.pos.y,allocc);
			break;
		}
	case ACT_ADD_BUFFER:
		{
			target.AddMember("buffer",headinfo.propsId,allocc);
			break;
		}
	case ACT_REMOVE_BUFFER:
		{
			target.AddMember("removeBuffer",headinfo.propsId,allocc);
			break;
		}
	case ACT_USE_PROPS:
		{
			target.AddMember("props",headinfo.propsId,allocc);
			break;
		}
	case ACT_LOCK:
		{
			break;
		}
	case ACT_COLD_BATTLE:
		{
			target.AddMember("cold",headinfo.targetId,allocc);
			break;
		}
	case ACT_DIED:
		{
			target.AddMember("died","died",allocc);
			break;
		}
	default:
		break;
	}

//	std::string s_key = StringUtils::format("%d",m_CurActionIndex);
	m_Root.PushBack(target,allocc);
}

void WriterLog::sendDataToServer()
{
#if !ENABLE_BATTLE_LOG
	int enableCompress = 0;

	const char* content = "[]";
#ifdef WIN32
	std::string s_content = content;
	log("data:%s",s_content.substr(0,100).c_str());
	//CCLOG("data:%s",content);
#endif

	cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
	string basicUrl = ServerManager::CurSelectedServer()->GetServerAddr();
	std::string urlpath = StringUtils::format("%s/battle.php?key=%s&filename=%s&env_type=%d",basicUrl.c_str(), m_key.c_str(),m_name.c_str(),ENVIRONMENT);
	log("upload: url %s , len:%d",urlpath.c_str(), strlen(content));
	request->setUrl(urlpath.c_str());
	request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
	request->setResponseCallback(m_callbackNetwork);
	//CC_CALLBACK_2(WriterLog::onHttpRequestCompleted,this));

	request->setRequestData(content, strlen(content));

	request->setTag("log_test");

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
#else
	int enableCompress = 0;
	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writer(buffer);
	m_Root.Accept(writer);
	const char* content = buffer.GetString();
#ifdef WIN32
	std::string s_content = content;
	log("data:%s",s_content.substr(0,100).c_str());
	//CCLOG("data:%s",content);
#endif 
	int len = buffer.Size();
	if (len < 512)
	{
		len = len + 9;
	}
	Bytef* destStr;
	if (enableCompress)
	{
		destStr = new Bytef[len];
		unsigned long destLen;
		compress(destStr,&destLen,(Bytef*)content,buffer.Size());
	}
	
	cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
	string basicUrl = ServerManager::CurSelectedServer()->GetServerAddr();
	std::string urlpath = StringUtils::format("%s/battle.php?key=%s&filename=%s&env_type=%d",basicUrl.c_str(), m_key.c_str(),m_name.c_str(),ENVIRONMENT);
	log("upload: url %s , len:%d",urlpath.c_str(), strlen(content));
	request->setUrl(urlpath.c_str());
	request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
	request->setResponseCallback(m_callbackNetwork);
	//CC_CALLBACK_2(WriterLog::onHttpRequestCompleted,this));

	// write the post data
	if (enableCompress)
	{
		request->setRequestData((const char*)destStr, strlen((const char*)destStr));
	}else
	{
		request->setRequestData(content, strlen(content));
	}
	request->setTag("log_test");

	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();
	if (enableCompress)
	{
		delete [] destStr;
	}
#endif
}

void WriterLog::onHttpRequestCompleted(network::HttpClient* client, network::HttpResponse* response)
{
	auto data = response->getResponseData();
	std::string s_data(data->begin(),data->end());
//	log("upload ret:%s",s_data.substr(0,100).c_str());
}

int64_t	WriterLog::getCurrentTimeUsev()
{
	timeval timeStruc;
	gettimeofday(&timeStruc,nullptr);
	int64_t sec1 = (int64_t)timeStruc.tv_sec;
	return sec1;
}

void WriterLog::setCallbackHttp(const network::ccHttpRequestCallback& callback)
{
	m_callbackNetwork = callback;
}

//火炮造成的伤害，参数依次为：己方shipid ，敌人shipid，伤害值，造成伤害的火炮iid，己方ship位置，敌人ship位置
void WriterLog::addCannonBattleLogItem(int myShipId,int enemyShipId,int hurt,int cannonItemid,Vec2 myShipPos, Vec2 enemyShipPos,bool isSelfVictim)
{
	BattleLogItem*item = (BattleLogItem*)malloc(sizeof(BattleLogItem));
	battle_log_item__init(item);
	item->idx = battleLogs.size();
	item->type = BATTLE_LOG_CANNON;
	if(isSelfVictim){
		item->type += 100;
	}
	item->myshipid = myShipId;
	item->n_enemyshipids = 1;
	item->enemyshipids = (int32_t*)malloc(sizeof(int32_t)*item->n_enemyshipids);
	item->enemyshipids[0] = enemyShipId;
	item->cannonitemid = cannonItemid;
	item->hurt = hurt;
	item->myshipx = (int32_t)myShipPos.x;
	item->myshipy = (int32_t)myShipPos.y;
	item->enemyshipx = (int32_t)enemyShipPos.x;
	item->enemyshipy = (int32_t)enemyShipPos.y;
	battleLogs.push_back(item);
}
//
//白刃战造成的水手死亡，参数依次为：己方shipid ，敌人shipid，敌人死亡的水手，己方死亡的水手
void WriterLog::addSailorBattlelogItem(int myShipId,int enemyShipId,int killedEnemySailorNum,int killedSelfSailorNum)
{
	BattleLogItem*item = (BattleLogItem*)malloc(sizeof(BattleLogItem));
		battle_log_item__init(item);
		item->idx = battleLogs.size();
		item->type = BATTLE_LOG_SAILOR;

		item->myshipid = myShipId;
		item->n_enemyshipids = 1;
		item->enemyshipids = (int32_t*)malloc(sizeof(int32_t)*item->n_enemyshipids);
		item->enemyshipids[0] = enemyShipId;
		item->killedsailornum = killedEnemySailorNum;
		item->addedsailornum = -killedSelfSailorNum;
		battleLogs.push_back(item);
}
//isSelfVictim 如果是敌人对己方造成的伤害，设置为true
//放技能造成的伤害，int skillId 技能id,int skillOwnerId 放技能的captainid或者玩家，victimShipIds 受伤的ship id 数组
void WriterLog::addSkillBattleLogItem(int skillId,int skillOwnerId,int hurt,int*victimShipIds, int n_ship,bool isSelfVictim)
{
	BattleLogItem*item = (BattleLogItem*)malloc(sizeof(BattleLogItem));
		battle_log_item__init(item);
		item->idx = battleLogs.size();
		item->type = BATTLE_LOG_SKILL;
		if(isSelfVictim){
			item->type += 100;
		}

		item->n_enemyshipids = n_ship;
		item->enemyshipids = (int32_t*)malloc(sizeof(int32_t)*item->n_enemyshipids);
		for(int i=0;i<n_ship;i++){
			item->enemyshipids[i] = victimShipIds[i];
		}
		item->skillid = skillId;
		item->skillownerid = skillOwnerId;
		item->hurt = hurt;
		battleLogs.push_back(item);
}


//单挑取胜，isSelfVictim 敌人取胜则设置1
void WriterLog::addCaptainVsBattleLogItem(int myShipId,int enemyShipId,int myCaptainId,bool isSelfVictim)
{
	BattleLogItem*item = (BattleLogItem*)malloc(sizeof(BattleLogItem));
		battle_log_item__init(item);
		item->idx = battleLogs.size();
		item->type = BATTLE_LOG_CAPTAIN_VS;
		if(isSelfVictim){
			item->type += 100;
		}
		item->myshipid = myShipId;
		item->n_enemyshipids = 1;
		item->enemyshipids = (int32_t*)malloc(sizeof(int32_t)*item->n_enemyshipids);
		item->enemyshipids[0] = enemyShipId;

		battleLogs.push_back(item);
}

void WriterLog::addShipCrashBattleLogItem(int shipid,int isMyShip,int reason){

	BattleLogItem*item = (BattleLogItem*)malloc(sizeof(BattleLogItem));
	battle_log_item__init(item);
	item->idx = battleLogs.size();
	item->type = BATTLE_LOG_SHIP_CRASH;
	if(!isMyShip){
		item->type += 100;

	}else{

	}
	item->myshipid = shipid;
	item->myshipx = reason;

	battleLogs.push_back(item);
}

BattleLog*WriterLog::getBattleLogItems()
{
	if(battleLogs.size() > 0){
		if(m_battleLog) battle_log__free_unpacked(m_battleLog,0);
		m_battleLog = (BattleLog*)malloc(sizeof(BattleLog));
		battle_log__init(m_battleLog);
		m_battleLog->n_logs = battleLogs.size();
		m_battleLog->logs = (BattleLogItem**)malloc(sizeof(BattleLogItem*)*battleLogs.size());
		for(int i=0;i<battleLogs.size();i++){
			m_battleLog->logs[i] = battleLogs.at(i);
		}
		battleLogs.clear();
		return m_battleLog;
	}
	return 0;
}