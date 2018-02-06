/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月02日
 *  Author:Sen
 *  json文件管理
 *
 */

#ifndef __SHOP__DATA__
#define __SHOP__DATA__

#include "cocos2d.h"
#include "json/document.h"
#include "SystemVar.h"
#include <unordered_map>
#include "xxhash.h"

using namespace rapidjson;
//船只数据
struct SHIP_RES
{
	int type;
	std::string name;
	int price;
	std::string desc;
	int require_level;
	int enchant_slots;
	int hp_max;
	int supply_max;
	int crew_max;
	int crew_require;
	int defense;
	int speed;
	int steer_speed;
	int bowgun_num;
	int rig_num;
	int armor_num;
	int spinnnaker_num;
	int cannon_num;
	int bowicon_num;
	int cargo_kinds;
	int cargo_size;
	int rarity;
	int icon_id;
	int model_id;
	int no_trade;//是否可以交易 0可以 1不可以
	int collision_a;
	int collision_b;
	int capture_lv;
	int capture_rate;
	std::string room_define;
	int nation_limit;
	int proficiency_need;
};
//货物数据
struct GOODS_RES
{
	int type;
	std::string name;
	int price;
	float weight;
	std::string desc;
};
//物品数据
struct ITEM_RES
{
	int id;
	int type;
	int sub_type;
	int property1;
	int rarity;
	std::string name;
	std::string property2;
	int price;
	std::string desc;
	float weight;
	int property3;
	int required_lv;
	int max_durability;
	int no_trade;//是否可以交易 0可以 1不可以
	int property4;
	int property5;
	std::string shortdesc;
	int poficiency;
	int icon_id; //图片资源
};
//技能数据
struct SKILL_TREE
{
	int cdreduce_per_lv;
	int efffect_skill_id;
	int skill_layer;
	int base_rate;
	int points_per_level;
	int cd;
	int base_skill_require_lv;
	int rate_per_lv;
	int ap_type;
	int type;
	int sub_type;
	int base_effect;
	int effect_per_lv;
	int base_skill_id;
	int base_duration;
	int max_level;
	int duration_per_lv;
	int require_level;
	int cur_skill_level;
	std::string name;
	std::string desc;
	std::string next_desc;
	int icon_id;
	int max_level_special;
	int room_type;
	int effect_type;
};
//cocos UI数据
struct TEXT_RES
{
	std::string name;
	std::string text;
	std::string path;
	std::string font;
	int type;
};
//船长数据
struct CAPTAIN_RES
{
	int type;
	std::string gender;
	std::string name;
};
//登陆点数据
struct LANDINfO
{
	int left_down_x;
	int left_down_y;
	int right_up_x;
	int right_up_y;
	int dock_x;
	int dock_y;
	std::string name;
};
//城市数据
struct CITYINFO
{
	int x;
	int y;
	int id;
	int dock_x;
	int dock_y;
	int leave_x;
	int leave_y;
	std::string name;
	int nation;
	std::string bargirl;
	int palace_type;
	int port_type;
};
//特殊商品数据
struct SPECIALSINFO
{
	int type;
	int price;
	std::string name;
	std::string desc;
};
//支线任务数据
struct SIDETASKINFO
{
	std::string title;
	std::string desc;
	int level;
	int time;
	int type;
//	int rewards_coin;
//	int rewards_gold;
//	int rewards_item1_id;
//	int rewards_item1_type;
//	int rewards_item1_amount;
//	int rewards_item2_id;
//	int rewards_item2_type;
//	int rewards_item2_amount;
//	int rewards_exp;
//	int rewards_reputation;
//	int rewards_force_id;
//	int rewards_force_affect;
//	int finish_target_npcId;
//	int finish_number;
//	int finish_goods_id;
//	int finish_goods_amount;
//	int finish_target_cityId;
};
//任务数据
struct SIDECORETASKINFO
{
	int task_core_id;
	int type;
	int level;
	int time;
	int rewards_coin;
	int rewards_gold;
	int rewards_item1_id;
	int rewards_item1_type;
	int rewards_item1_amount;
	int rewards_item2_id;
	int rewards_item2_type;
	int rewards_item2_amount;
	int rewards_exp;
	int rewards_reputation;
	int rewards_force_id;
	int rewards_force_affect;
	int* finish_target_npcId;
	int* zone;
	int npc_number;
	int finish_number;

	int* finish_goods_id;
	int* finish_goods_amount;
	int* finish_target_cityId;
	int goods_id_number;
	int bonus_v_ticket;
};

struct MAINTASKSTORYINFO
{
	std::string name;
	std::string title;
	std::string desc;
	int id;
	int nation;
	int chapter_idx;
};
//图纸所需材料种类及个数
struct DRAWINGMATERIALINFO
{
	int requried_item;
	int requried_item_num;
};
//图纸数据
struct DRAWINGSDESIGNINFO
{
	int required_time;
	int manufacture_req;
	int coin;
	std::string city_id_list;
	int required_item1;
	int requried_item1_num;
	int required_item2;
	int requried_item2_num;
	int type;
	int define_id;
	std::vector<DRAWINGMATERIALINFO> required_items;
};
//登陆奖励数据
struct DAILYREWARDINFO
{
	int date;
	int items_id;
	int items_num;
	std::string ships_id;
	int ships_num;
	int coins;
	int v_tickets;
};
//战斗的NPC数据
struct BATTLENPCINFO
{
	std::string name;
	int type;
	int force_id;
};
//主线任务对话数据
struct TASKDIALOGINFO
{
	std::string name;
	std::string dialog;
};
//新手引导对话数据
struct GUIDETASKDIALOGINFO
{
	std::string name;
	std::string dialog;
};

//主线任务
struct ITEMINFO
{
	int item_type;
	int item_id;
	int amount;
};
struct REWARDSJSON
{
	int coin;
	int exp;
	int reputation;
	int vticket;
	int force_id;
	int force_affect;
	std::vector<ITEMINFO>items;
};

struct PARTSINFO
{
	int sea;
	int city;
	int cityarea;
	int x;
	int y;
	int r;
	int action;
	int part;
	int type;
	bool has_position;
	bool has_condition;
	std::vector<ITEMINFO>items;
	int target_id;
	int target_amount;
	int target_city;
	bool has_npc;
	int npc_id;
	bool has_branch;
	int branch;
	bool has_next_part;
	int next_part;
};
struct TRIGGERJSON
{
	int part_num;
	std::vector<PARTSINFO>parts;
};

struct FINISHJSON
{
	int sea;
	int city;
	int cityarea;
	int x;
	int y;
	int r;
	int type;
	std::vector<ITEMINFO>items;
};

struct PRETASKJSON
{
	int coin;
	int bank;
	std::vector<ITEMINFO>items;
};

struct MAINTASKINFO
{
	std::string title;
	std::string name;
	std::string desc;
	std::string tip;
	int id;
	int nation;
	int chapter_idx;
	int start_flag;
	int next_id;
	TRIGGERJSON trigger;
	REWARDSJSON rewards;
	FINISHJSON finish;
	PRETASKJSON pretask;
};

struct BAGEXPANDFEEINFO
{
	int id;
	int fee;
	int capacity;
};

//触发剧情对话数据
struct TRIGGERDIALOGINFO
{
	std::string name;
	std::string dialog;
};

struct TRIGGERCONDITION
{
	int type;
	int task_type;
	int character_number;
	int amount;
	int compare;
};

//触发剧情对话判断数据
struct TRIGGERDIALOGJUDGEINFO
{
	int id;
	int action;
	int type;
	int need_pre_cond;
	int probability;
	std::string name;
	REWARDSJSON rewards;
	FINISHJSON position;
	TRIGGERCONDITION condition;
};
//小伙伴红胡子剧情对话数据
struct PARTNERREDBEARDDIALOGINFO
{
	std::string name;
	std::string dialog;
};
//小伙伴红胡子剧情任务判定
struct PARTNERDIALOGJUDGEINFO
{
	std::string title;
	std::string name;
	std::string desc;
	std::string tip;
	int id;
	int chapter_idx;
	int start_flag;
	int next_id;
	int companion_id;
	TRIGGERJSON trigger;
	REWARDSJSON rewards;
	FINISHJSON finish;
	PRETASKJSON pretask;
};
//boss奖励列表用
struct BOSS_AWARDS
{
	int coin;
	int v_ticket;
	int badge;
};
//小伙伴数据
struct COMPANION_INFO
{
	int id;
	std::string name;
	int ap;
	int dp;
	int hp;
	int ap_lv;
	int dp_lv;
	int hp_lv;
	int skill1;
	int skill2;
	int skill3;
	int specialty1;
	int specialty2;
	int specialty3;
	int item1;
	int item1_amount;
	int item2;
	int item2_amount;
	int item3;
	int item3_amount;
	std::string desc;
	int icon_id;
	int card_id;
	int model_id;
};
//小伙伴工作室数据
struct JOB_POSITION_INFO
{
	int id;
	std::string room;
	std::string job;
	std::string desc;
};

//小伙伴特殊技能数据
struct SPECIALIES_SKILL_INFO
{
	int id;
	std::string name;
	int require_level;
	int effect_type;
	int room_type;
	std::string desc;
};
//船只剖面图数据
struct SHIP_ROOM_INFO
{
	int id;
	int ship_type;
	int room_id;
	int room_type;
};
//新手引导
struct MAINTASKGUIDEINFO
{
	int id;
	int nation;
	int chapter_idx;
	int next_id;
	int start_flag;
	std::string tip;
	std::string name;
	std::string title;
	std::string desc;
	REWARDSJSON rewards;
};
struct PROFICIENCY_INFO
{
	int id;
	std::string name;
	int visible;
	int type;
	int icon_id;
	int item_need;
};

//国战战斗船只的位置
struct POSITION_EXAMPLE
{
	int x;
	int y;
	int r;
};

//国战战斗船只的位置
struct CITY_WAR_POSITION
{
	int id;
	std::vector<POSITION_EXAMPLE> position;
};

//国战船只的发炮点
struct SHIP_POSITION
{
	int x;
	int y;
};
struct SHIP_WAR_POSITION
{
	int id;
	int model_id;
	int direction;
	std::vector<SHIP_POSITION> position;
};

struct CITY_AREA_RESOURCE
{
	int id;
	int background_id;
	int background_village;
	int shipyard_id;
	int bank_id;
	int dock_id;
	int tavern_id;
	int cityhall_id;

	int supplyofficer_id;
	int soldier_id;
	int villageboss_id;
	int chiefofficer_id;
	int merchant_id;
	//港口图标
	int icon_id;
	int icon_village;
	int market_id;
	int shipboss_id;
	int bargirl_id;
};
struct SETTING_LENGEND
{
	int id;
	int type;
	std::string name;
	std::string path;
};
struct _ConfigJsonMd5;
struct _ConfigJsonResult;

#define MAX_REMOTE_CONFIG_FILE_NUM 256

typedef std::unordered_map<std::string,TEXT_RES> hash_map;
class ShopData
{

public:
	ShopData();
	~ShopData();

	
	std::map<int,SHIP_RES>			getShipData(){ return m_ShipData; };	//船只信息
	std::map<int,ITEM_RES>			getItemData(){ return m_ItemData; };	//物品信息
	std::map<int,GOODS_RES>			getGoodsData(){ return m_GoodsData; };	//货物信息
	std::map<int,LANDINfO>&			getLandInfo(){ return m_LandData; };	//登陆单信息
	std::map<int,CITYINFO>&			getCitiesInfo() { return m_CityInfo; };	//城市信息
	std::map<int,SKILL_TREE>&		getSkillTrees(){ return m_SkillTrees; };//技能信息
	std::map<int,CAPTAIN_RES>		getCaptainData(){ return m_CaptainData; };//船长信息

	std::unordered_map<std::string, TEXT_RES> getTextData(){return m_TextData;}; //cocos ui
	std::unordered_map<std::string,std::string>&		getTipsInfo(){ return m_TipsInfo; };//提示信息
	std::map<int,std::string>&		getFoodDrinkData() { return m_FoodAndDrinkInfo; }; //酒水信息
	std::map<int,SPECIALSINFO>&		getSpecialsInfo() { return m_SpecialInfo; };		//特殊物品信息
	std::map<int,SIDETASKINFO>&     getSideTaskInfo() {return m_SideTaskInfo; };		//支线任务信息
	std::map<int,BATTLENPCINFO>&      getBattleNpcInfo() {return m_BattleNpcInfo; };	//NPC信息
	std::map<int,DRAWINGSDESIGNINFO>& getDrawingsDesignInfo() {return m_DrawingsDesignInfo; };//图纸信息
	std::map<int,std::string>&		getNationInfo() {return m_Nation; };					//国家的信息
	std::map<int,DAILYREWARDINFO> &		getDailyRewardInfo(){return m_DailyReward; };		//登陆奖励信息
	std::map<int,SKILL_TREE>&     getCaptainSkillInfo(){return m_CaptainSkillInfo; };		//船长的技能
	std::map<int,SKILL_TREE>&     getNPCSkillInfo(){return m_NPCSkillInfo; };				//NPC的技能
	//任务NPC所在的海域
	std::map<int, std::string>&     getZoneInfo(){ return m_zoneInfo; }		
	std::map<int, BAGEXPANDFEEINFO>&     getBagExpandFeeInfo() { return m_BagExpandFeeInfo; };

	//新手引导剧情
	std::map<int, GUIDETASKDIALOGINFO>&     getGuideTaskDialogInfo(){ return m_GuideTaskDialogInfo; }
	//主线任务剧情
	std::map<int,TASKDIALOGINFO>&     getTaskDialogInfo(){return m_TaskDialogInfo; }	
	//主线任务判定
	std::map<int, MAINTASKINFO>&     getMainTaskInfo() { return m_MainTaskInfo; };		
	//触发剧情对话信息
	std::map<int, TRIGGERDIALOGINFO>&     getTriggerDialogInfo(){ return m_TriggerDialogInfo; }
	//触发剧情对话判定
	std::map<int, TRIGGERDIALOGJUDGEINFO>&     getTriggerDialogJudgeInfo(){ return m_TriggerDialogJudgeInfo; }

	//小伙伴红胡子剧情对话信息
	std::map<int, PARTNERREDBEARDDIALOGINFO>&     getPartnerRedBeardDialogInfo(){ return m_partnerRedBeardDialogInfo; }
	//小伙伴红胡子剧情任务判定
	std::map<int, PARTNERDIALOGJUDGEINFO>&     getPartnerDialogJudgeInfo(){ return m_partnerDialogJudgeInfo; }

	std::map<int, BOSS_AWARDS>&     getBossAwards(){ return m_bossAwards; };
	/*
	*小伙伴数据
	*/
	std::map<int, COMPANION_INFO>&     getCompanionInfo(){ return m_companionInfo; };
	/*
	*小伙伴正常技能数据
	*/
	std::map<int, SKILL_TREE>&     getCompanionNormalSkillInfo(){ return m_companionSkillInfo; };
	/*
	*小伙伴工作位置数据
	*/
	std::map<int, JOB_POSITION_INFO>&     getJobPositionInfo(){ return m_jobPositionInfo; };
	/*
	*小伙伴特殊技能数据
	*/
	std::map<int, SPECIALIES_SKILL_INFO>&     getCompanionSpecialiesSkillInfo(){ return m_companionSpecialiesSkillInfo; };
	/*
	*船只剖面图数据
	*/
	std::map<int, SHIP_ROOM_INFO>&     getShipRoom(){ return m_shipRoomInfo; };
	//新手引导task奖励
	std::map<int, MAINTASKGUIDEINFO>&   getMaintaskGuide(){ return m_MainTaskGuideInfo; };
	//熟练度信息
	std::map<int, PROFICIENCY_INFO>&  getProficiencyInfo(){ return m_proficiencyInfo; };

	//国战战斗船只的位置
	std::map<int, CITY_WAR_POSITION>&  getCityWarPositionInfo(){ return m_cityWarPositionInfo; };
	
	//国战船只的发炮点
	std::map<int, SHIP_WAR_POSITION>&  getShipWarPositionInfo(){ return m_shipWarPositionInfo; };

	//获取每个城市的各个功能背景图
	std::map<int, CITY_AREA_RESOURCE>&  getCityAreaResourceInfo(){ return m_cityAreaResourceInfo; };
	//设置里的图鉴
	std::map<int, SETTING_LENGEND>&  getSettingLengend(){ return m_settingLengendInfo; };

	void loadShopData();
	void loadLandData();
	void loadTextData();
	void loadItemData();
	void loadShipData();
	void loadGoodsData();
	void loadCityInfoData();
	void loadSkillTrees();
	void loadTipsInfo();
	void loadCaptainData();
	void loadSailIngData();
	void loadFoodADrinkData();
	void loadSpecialsData();
	void loadSideTaskData();
	void loadCoreSideTaskData();
	void loadBattleNpcData();
	void loadDrawingsDesignData();
	void loadNationData();
	void loadDailyRewardData();
	void loadCaptainSkillData();
	void loadNPCSkillData();

	void loadGuideTaskDialogData();
	void loadTaskDialogData();

	void loadMainTaskStoryData();

	void loadZoneData();

	void loadMainTaskData();
	void loadBagExpandFeeData();

	void loadTriggerDialogData();
	void loadTriggerDialogJudgeData();

	void loadPartnerRedBeardDialogData();
	void loadPartnerDialogJudgeData();

	void loadBossAwards();
	void loadCompanionData();
	void loadCompanionSkillData();
	void loadJobPositionData();
	void loadSpecialiesSkillData();
	void loadShipRoomData();
	//新手引导task
	void loadMaintaskGuideData();
	//熟练度信息
	void loadProficiencyData();
	//加载国战战斗船只的位置
	void loadCityWarPositionData();
	//加载国战船只的发炮点
	void loadShipWarPositionData();

	//加载每个城市的各个功能背景图
	void LoadCityAreaResourceData();
	//设置里的图鉴资源
	void loadSettingLengend();

	void releaseLandData();
	std::string findJsonFile(std::string name);
	SIDECORETASKINFO& getCoreTaskById(int coreTaskId);
	MAINTASKSTORYINFO& getMainTaskStoryByNationAndChapter(int nation, int chapter);

	//带config。  filename = config/xxxjson
	std::string getRemoteJson(const std::string &filename);
	//set reload得标志
	void setJsonDirty(std::string &filename);
	int shouldLoadJson(std::string &filename);
	int updateConfigJsonMd5(std::string &filename,unsigned char*buffer,int buffLen);
	struct _ConfigJsonMd5**getJsonSigure(int*size);
	void updateRemoteJson(struct _ConfigJsonResult**json,int num);
	//图片资源语言本地化
	int L_TYPE;
	int Loading_bg;
private:
	//存放服务器返回得更新json，存储在这个目录
	std::string m_RemoteConfigDir;
	//保存正在使用得json文件得 md5
	struct _ConfigJsonMd5*m_RemoteJsonConfigData[MAX_REMOTE_CONFIG_FILE_NUM];
	//m_RemoteJsonConfigData数组，得有效数据个数
	int m_CurrentRemoteJsonConfigNum;

	std::map<int,SHIP_RES> m_ShipData;
	std::map<int,ITEM_RES> m_ItemData;
	std::map<int,GOODS_RES> m_GoodsData;

	std::map<int,SKILL_TREE> m_SkillTrees;

	std::unordered_map<std::string, TEXT_RES> m_TextData;
	std::unordered_map<std::string, std::string> m_TipsInfo;
	
	std::map<int,CAPTAIN_RES> m_CaptainData;
	std::map<int,LANDINfO> m_LandData;
	std::map<int,CITYINFO> m_CityInfo;
	std::map<int,std::string> m_FoodAndDrinkInfo;
	std::map<int,SPECIALSINFO> m_SpecialInfo;
	std::map<int,SIDETASKINFO> m_SideTaskInfo;
	std::map<int,BATTLENPCINFO> m_BattleNpcInfo;
	std::map<int,DRAWINGSDESIGNINFO> m_DrawingsDesignInfo;
	std::map<int,std::string> m_Nation;
	std::map<int,DAILYREWARDINFO>m_DailyReward;
	std::vector<SIDECORETASKINFO*> m_CoreTaskInfo;
	std::map<int,SKILL_TREE> m_CaptainSkillInfo;
	std::map<int,SKILL_TREE> m_NPCSkillInfo;

	std::map<int, GUIDETASKDIALOGINFO> m_GuideTaskDialogInfo;
	std::map<int,TASKDIALOGINFO> m_TaskDialogInfo;

	std::vector<MAINTASKSTORYINFO*> m_MainTaskStoryInfo;

	std::map<int,std::string> m_zoneInfo;
	std::map<int, MAINTASKINFO> m_MainTaskInfo;
	std::map<int, BAGEXPANDFEEINFO> m_BagExpandFeeInfo;

	std::map<int, TRIGGERDIALOGINFO> m_TriggerDialogInfo;
	std::map<int, TRIGGERDIALOGJUDGEINFO> m_TriggerDialogJudgeInfo;
	std::map<int, PARTNERREDBEARDDIALOGINFO> m_partnerRedBeardDialogInfo;
	std::map<int, PARTNERDIALOGJUDGEINFO> m_partnerDialogJudgeInfo;
	std::map<int, BOSS_AWARDS> m_bossAwards;
	std::map<int, COMPANION_INFO> m_companionInfo;
	std::map<int, SKILL_TREE> m_companionSkillInfo;
	std::map<int, JOB_POSITION_INFO> m_jobPositionInfo;
	std::map<int, SPECIALIES_SKILL_INFO> m_companionSpecialiesSkillInfo;
	std::map<int, SHIP_ROOM_INFO>m_shipRoomInfo;
	std::map<int, MAINTASKGUIDEINFO>m_MainTaskGuideInfo;
	std::map<int, PROFICIENCY_INFO>m_proficiencyInfo;
	std::map<int, CITY_WAR_POSITION>m_cityWarPositionInfo;
	std::map<int, SHIP_WAR_POSITION>m_shipWarPositionInfo;

	std::map<int, CITY_AREA_RESOURCE>m_cityAreaResourceInfo;

	std::map<int, SETTING_LENGEND>m_settingLengendInfo;
};

#endif
