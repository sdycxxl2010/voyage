/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月03日
 *  Author:Sen
 *  宏文件
 *
 */

#ifndef __SYSTEM__VAR__H__
#define __SYSTEM__VAR__H__

#include "VoyageConfig.h"
#include <string>

#ifndef START_INDEX
#define START_INDEX 50
#endif
//位置Vec
#define STARTPOS Vec2(0,0)
#define ENDPOS	Vec2(0,1000)
#define ENDPOS2	Vec2(0,-1000)
#define ENDPOS3	Vec2(-1300,0)
#define ENDPOS4 Vec2(0,-160)
//描边数据
#define OUTLINE_MIN 4
#define OUTLINE_MAX 6
#define OUTLINE_COLOR Color4B(67,50,14,255)
//酒吧女郎好感度
#define FAVOUR_MIN 50
#define FAVOUR_MID 180
#define FAVOUR_SIDE_TASK 200
#define FAVOUR_MAX 300
//声望等级
#define PRESTIGE_MIN 20
#define PRESTIGE_MID 50
#define PRESTIGE_MAX 75
//描边
#define LEFT_BUTTON_TEXT_COLOR_NORMAL Color4B(46,29,14,255)
#define LEFT_BUTTON_TEXT_COLOR_PASSED Color4B(241,200,129,255)
#define TOP_BUTTON_TEXT_COLOR_NORMAL Color4B(231,192,125,255)
#define TOP_BUTTON_TEXT_COLOR_PASSED Color4B(255,233,195,255)
//不满足条件时文字变红rgb
#define TEXT_RED Color4B(183,28,28,255)
#define TEXT_RED_2 Color3B(183,28,28)
//满足条件的绿
#define TEXT_GREEN Color4B(0,255,108,255)
//地图显示文字颜色 描边
#define MAP_FRIEND_TEXT_COLOR Color4B(254,255, 214, 255)
#define MAP_FRIEND_TEXT_OUTLINE Color4B(71,33,14,255)
#define MAP_ENEMY_TEXR_CLOLR Color4B(255,231,231,255)
#define MAP_ENEMY_TEXT_OUTLINE  Color4B(150,0,0,255)
#define MAP_FONT_TTF "fonts/LBRITEDI.TTF"
#define MAP_FONT_SIZE 24
#define MAP_OUTLINE_SIZE 4

//海上供给警告
#define CRITICAL_DAY 1
#define WARNING_DAY 2
#define CARW_ONE_DAY_SUPPLY 1
#define CARW_ONE_DAY_DIE 3

enum WARNING_LV
{
WARNING_NONE,
	SUPPLY_WARNING,
	SUPPLY_URGENT_WARNING,
	SAILOR_WARNING,
	SAILOR_URGENT_WARNING,
	SAILOR_NOT_ENOUGH_EFFECT_SPEED,
};
//Unreasonable_code_10;纯数字应该配置化
//等级界限
#define LEVEL_MAX_NUM 99
#define PRESTIGE_MAX_NUM 99
#define TAG_LAODING 1234
//声音
//1 open,2 close
#define BG_MUSIC_KEY  "Bg_music" 
#define SOUND_EFFECT_KEY "Sound_efffect"
#define OPEN_AUDIO 1
#define CLOSE_AUDIO 2
//语言保存
#define LANGUAGE_KEY "language"
//技能点提示
#define MAIN_CITY_NEW_POINT "main_city_new_point"
#define SKILL_NEW_POINT "skill_new_point"
//主线任务是否完成
#define MAIN_TASK_FINISH "main_task_finish"

//第几艘船装备有损坏
#define SHIP_POSTION_EQUIP_BROKEN "ship_postion_equip_broken_%d"

//服务器选择本地存储
#define SERVER_LOCAL_INDEX "server_local_index"
//用户协议已接受
#define HAVE_ACCPET_EULA "have_accpet_eula"
//开始界面已经打开过更新日志
#define HAVE_OPEN_CHANGE_LOG "have_open_change_log"

#define PAGE_ITEM_NUM 4
#define SHIP_EQUIP_BROKEN 5/*装备破损的定义值*/
#define SHIP_EQUIP_BROKEN_PERCENT 0.1/*装备破损的比例10%*/
#define VILLAGE_START_ID 34/*目前村庄的最小ID*/

//前三名排名图标
const std::string RANK_GOLD = "res/rank/rank_gold.png";
const std::string RANK_SILVER = "res/rank/rank_silver.png";
const std::string RANK_COPPER = "res/rank/rank_copper.png";

//初级护送
const std::string ESCORT_PRIMARY = "res/Vticket_coin/escort_primary.png";
//高级护送
const std::string ESCORT_SENIOR = "res/Vticket_coin/escort_senior.png";
//低级掉落服务
const std::string LOW_DROPS = "res/Vticket_coin/low_dorp.png";
//高级掉落服务
const std::string SENIOR_DROPS = "res/Vticket_coin/senior_dorp.png";
//低级掉落服务
const std::string LOW_DROPS_IOCN = "res/Vticket_coin/low_dorp_buff.png";
//高级掉落服务
const std::string SENIOR_DROPS_IOCN = "res/Vticket_coin/senior_dorp_buff.png";
//个人背包超重图标
const std::string BAG_OVER_WEIGHT = "res/Vticket_coin/hints_weight.png";
//单个的v票，银币图标
const std::string SINGLE_V_PIC = "res/Vticket_coin/v_ticket.png";
const std::string SINGLE_COIN_PIC = "res/Vticket_coin/silver.png";
//config 路径
const std::string GOODS_PATH_JSON = "config/goods";
const std::string SHIP_PATH_JSON = "config/ships";
const std::string ITEM_PATH_JSON = "config/items";
const std::string SKILL_PATH_JSON = "config/skills";
const std::string TEXT_PATH_JSON = "config/texts";
const std::string CAPTAIN_PATH_JSON = "config/names";
const std::string LAND_PATH_JSON = "config/oceans";
const std::string CITY_PATH_JSON = "config/cities";
const std::string TIPSINFO_JSON = "config/tipsInfo";
const std::string FOOD_DRINK_JSON = "config/food_drink";
const std::string SPECIALS_PATH_JSON = "config/specials";
const std::string SIDETASK_PATH_JSON = "config/side_tasks";
const std::string CORESIDETASK_PATH_JSON = "config/side_task_numbers";
const std::string BATTLENPC_PATH_JSON = "config/battle_npc";
const std::string DRAWINGSDESIGN_PATH_JSON = "config/drawings_design.json";
const std::string NATION_PATH_JSON = "config/forces";
const std::string DAILYREWARD_PATH_JSON ="config/daily_reward";
const std::string CAPTAINSKILL_PATH_JSON = "config/captain_skill_list";
const std::string NPCSKILL_PATH_JSON = "config/npc_skills_list";
const std::string GUIDETASKDIALOG_PATH_JSON = "config/guide_dialog";
const std::string TASKDIALOG_PATH_JSON = "config/task_dialog";
const std::string TRIGGERDIALOG_PATH_JSON = "config/trigger_dialog";
const std::string TRIGGERDIALOG_JUDGE_PATH_JSON = "config/trigger_dialog_judge";
const std::string PARTNERREDBEARDDIALOG_PATH_JSON = "config/partner_red_Beard";
const std::string PARTNERREDBEARDDIALOG_JUDGE_PATH_JSON = "config/companion_tasks";
const std::string ZONE_PATH_JSON = "config/zone_name";

const std::string MAINTASK_PATH_JSON = "config/main_tasks";
const std::string MAINTASK_STORY_PATH_JSON = "config/main_tasks_story";
const std::string BAG_EXPAND_FEE_PATH_JSON = "config/bag_expand_fee";
const std::string BOSS_AWARDS_JSON = "config/boss_award";
const std::string COMPANION_JSON = "config/companions";
const std::string COMPANION_SKILL_JSON = "config/companion_skill_list";
const std::string JOB_POSITION_JSON = "config/flagship_jobs";
const std::string COMPANION_SPECIALIES_JSON = "config/companion_specialties_list";
const std::string SHIP_ROOM_JSON = "config/flagship_rooms_define";
const std::string MAIN_TASK_GUIDE_JSON = "config/main_tasks_guide";
const std::string PROFICNENCY_INFO_JSON = "config/proficiency";
const std::string WARPOSITION_INFO_JSON = "config/city_war_positions";
const std::string SHIPPOSITION_INFO_JSON = "config/ship_war_positions";
const std::string CITIES_RESOURCES_INFO_JSON = "config/cities_resources";
const std::string SETTING_LENGEND_INFO_JSON = "config/setting";

static std::string CITY_ICON_PATH = "res/wharfRes/city_1.png";
static std::string SHIP_ICON_PATH = "res/wharfRes/ship.png"; 
static std::string EU_ENEMY_S_PATH = "res/wharfRes/eu_enemy_s.png"; 
static std::string OTTTOMAN_S_PATH = "res/wharfRes/otttoman_s.png"; 

static std::string ERROR_DIALOG_BG = "dia_bg.png";
static std::string DIALOG_BUTTON_IMG[] = {"npc.png", "npc_no.png"};

//字体
const std::string CUSTOM_FONT_NAME_1 = "fonts/Roboto-Regular.ttf";
const std::string CUSTOM_FONT_NAME_1_1 = "fonts/RobotoCondensed-Regular.ttf";
const std::string CUSTOM_FONT_NAME_2 = "fonts/LBRITEI.TTF";
const std::string CUSTOM_FONT_NAME_3 = "fonts/LBRITEDI.TTF";
const std::string CUSTOM_FONT_NAME_4 = "fonts/RobotoCondensed-Bold.ttf";

static const std::string IMAGE_FONT[] =
{
	"fonts/time_total.png",
	"fonts/damage_total.png",
	"fonts/crit_total.png",
	"fonts/add_total.png",
};

//UI
#define COCOSTDIO_RES_DIR "cocosstudio/"
static const std::string COCOSTUDIO_RES[] = 
{
	"voyage_UI_signup",
	"voyage_UI_login",
	"voyageUI_create",
	"voyageUI_maincity",
	"voyageUI_hatch",//5
	"voyageUI_change",
	"voyageUI_mail",
	"voyageUI_exchange",
	"voyageUI_personal",
	"voyageUI_bar",//10
	"voyageUI_dock",
	"voyageUI_wharf",
	"voyageUI_sailing",
	"voyageUI_palace",
	"voyageUI_bank",//15
	"voyageUI_fight",
	"voyageUI_guild",
	"voyageUI_social",
	"voyageUI_map",
	"voyageUI_v_tickets",//20
	"voyageUI_load",
	"voyageUI_setting",
	"voyageUI_dailyreward",
	"voyageUI_start",
	"voyageUI_quest"
};

//海上NPC警告
static const std::string NPC_SHIPICON[] = 
{
	"ship/ship.png",
	"ship/warning.png",
	"ship/attack-range_s.png",
	"ship/attack-range_m.png",
	"ship/attack-range_l.png",
};
//城市
static const char* cites_name_table[] = 
{
	"Lisbon", //0
	"Seville",
	"London",
	"Amsterdam",
	"Hamburg",
	"Genoa",
	"Venice",
	"Marseilles",
	"Naples",
	"Athens",
	"Constantinople",//11
	"Alexandria",
	"Stockholm",
	"Algiers",
	"Barcelona",
	"Porto",
	"Faro",
	"Gijon",
	"Nantes",
	"Bruges",
	"Southampton",//21
	"Bristol",
	"Rotterdam",
	"Lubec",
	"Oslo",
	"Copenhagen",
	"Pisa",
	"Palermo",
	"Siracusa",
	"Tunisian",
	"Ceuta",  //31
	"Beirut",
	"Madeira",
	"Arguin",
	"Grand Cayman",
	"Havana",
	"Nassau",
	"San Juan",
	"Santo Domingo",//39
	"Jamaica",
};

//新手引导
static const std::string GUIDE_WAR[] =
{
	"guide/guide_battle.png",
	"guide/guide_battle.zh_CN.png",
	"guide/guide_battle.zh_CN.png"
};

static const std::string GUIDE_SAILING[] =
{
	"guide/guide_sailing.png",
	"guide/guide_sailing.zh_CN.png",
	"guide/guide_sailing.zh_CN.png"
};

//图片中英文
static const std::string WAR_VICTORY[] = 
{
	"image_language/title_victory.png",
	"image_language/title_victory.zh_cn.png",
	"image_language/title_victory.zh_hk.png",
};

static const std::string WAR_DRAW[] =
{
	"image_language/title_draw.png",
	"image_language/title_draw.zh_cn.png",
	"image_language/title_draw.zh_hk.png",
};

static const std::string WAR_FLEE[] =
{
	"image_language/title_flee.png",
	"image_language/title_flee.zh_cn.png",
	"image_language/title_flee.zh_hk.png",
};
static const std::string WAR_DEFEATED[] =
{
	"image_language/title_defeated.png",
	"image_language/title_defeated.zh_cn.png",
	"image_language/title_defeated.zh_hk.png",
};
//打劫界面
static const std::string ROB_WAR_VICTORY[] =
{
	"image_language/rob_title/title_victory.png",
	"image_language/rob_title/title_victory_cn.png",
	"image_language/rob_title/title_victory_hk.png",
};

static const std::string ROB_WAR_DRAW[] =
{
	"image_language/rob_title/title_draw.png",
	"image_language/rob_title/title_draw_cn.png",
	"image_language/rob_title/title_draw_hk.png",
};

static const std::string ROB_WAR_FLEE[] =
{
	"image_language/rob_title/title_flee.png",
	"image_language/rob_title/title_flee_cn.png",
	"image_language/rob_title/title_flee_hk.png",
};

static const std::string ROB_WAR_DEFEATED[] =
{
	"image_language/rob_title/title_defeated.png",
	"image_language/rob_title/title_defeated_cn.png",
	"image_language/rob_title/title_defeated_hk.png",
};
//王宫背景
static const std::string PALACE_BG[] =
{
	"res/palace_bg/europe_palace.jpg",
	"res/palace_bg/europe_cityhall.jpg",
	"res/palace_bg/sudan_palace.jpg",
};
//酒馆背景
static const std::string TAVERN_BG[] =
{
	"res/tavern_bg/europe_tavern.jpg",
	"res/tavern_bg/sudan_tavern.jpg",
	"res/tavern_bg/village_tavern_africa.jpg",
	"res/tavern_bg/village_tavern_caribbean.jpg",
	"res/tavern_bg/village_tavern_europe.jpg",
};
//港口背景
static const std::string WHARF_BG[] =
{
	"res/wharf_bg/europe_wharf.jpg",
	"res/wharf_bg/sudan_wharf.jpg",
};

//城市下方icon
static const std::string BANK_ENABLE[] =
{
    "cocosstudio/login_ui/city_720/bank.png",
	"cocosstudio/login_ui/city_720/bank_zh_cn.png",
    "cocosstudio/login_ui/city_720/bank_zh_hk.png",//zh_HK 
};
static const std::string BANK_DISABLE[] =
{
    "cocosstudio/login_ui/city_720/bank2.png",
	"cocosstudio/login_ui/city_720/bank2_zh_cn.png",
    "cocosstudio/login_ui/city_720/bank2_zh_hk.png",
};
static const std::string CITYHALL_ENABLE[] =
{
    "cocosstudio/login_ui/city_720/cityhall.png",
	"cocosstudio/login_ui/city_720/cityhall_zh_cn.png",
    "cocosstudio/login_ui/city_720/cityhall_zh_hk.png",
};
static const std::string CITYHALL_DISABLE[] =
{
    "cocosstudio/login_ui/city_720/cityhall2.png",
	"cocosstudio/login_ui/city_720/cityhall2_zh_cn.png",
    "cocosstudio/login_ui/city_720/cityhall2_zh_hk.png",
};
static const std::string DOCK[] =
{
    "cocosstudio/login_ui/city_720/dock.png",
	"cocosstudio/login_ui/city_720/dock_zh_cn.png",
    "cocosstudio/login_ui/city_720/dock_zh_hk.png",
};
static const std::string MARKET_ENABLE[] =
{
    "cocosstudio/login_ui/city_720/market.png",
	"cocosstudio/login_ui/city_720/market_zh_cn.png",
    "cocosstudio/login_ui/city_720/market_zh_hk.png",
};
static const std::string MARKET_DISABLE[] =
{
    "cocosstudio/login_ui/city_720/market2.png",
	"cocosstudio/login_ui/city_720/market2_zh_cn.png",
    "cocosstudio/login_ui/city_720/market2_zh_hk.png",
};
static const std::string PALACE_ENABLE[] =
{
    "cocosstudio/login_ui/city_720/palace.png",
	"cocosstudio/login_ui/city_720/palace_zh_cn.png",
    "cocosstudio/login_ui/city_720/palace_zh_hk.png",
};
static const std::string PALACE_DISABLE[] =
{
    "cocosstudio/login_ui/city_720/palace2.png",
	"cocosstudio/login_ui/city_720/palace2_zh_cn.png",
    "cocosstudio/login_ui/city_720/palace2_zh_hk.png",
};
static const std::string SHIPYARD_ENABLE[] =
{
    "cocosstudio/login_ui/city_720/shipyard.png",
	"cocosstudio/login_ui/city_720/shipyard_zh_cn.png",
    "cocosstudio/login_ui/city_720/shipyard_zh_hk.png",
};
static const std::string SHIPYARD_DISABLE[] =
{
    "cocosstudio/login_ui/city_720/shipyard2.png",
	"cocosstudio/login_ui/city_720/shipyard2_zh_cn.png",
    "cocosstudio/login_ui/city_720/shipyard2_zh_hk.png",
};
static const std::string TAVERN_ENABLE[] =
{
    "cocosstudio/login_ui/city_720/tavern.png",
	"cocosstudio/login_ui/city_720/tavern_zh_cn.png",
    "cocosstudio/login_ui/city_720/tavern_zh_hk.png",
};
static const std::string TAVERN_DISABLE[] =
{
    "cocosstudio/login_ui/city_720/tavern2.png",
	"cocosstudio/login_ui/city_720/tavern2_zh_cn.png",
    "cocosstudio/login_ui/city_720/tavern2_zh_hk.png",
};
//升级文字图片
static const std::string LEVEL_UP[] =
{
	"image_language/level_up.png",
	"image_language/level_up.zh_cn.png",
	"image_language/level_up.zh_hk.png",
};
//降级文字图片
static const std::string LEVEL_DOWN[] =
{
	"image_language/level_dowm.png",
	"image_language/level_dowm.zh_cn.png",
	"image_language/level_dowm.zh_hk.png",
};
#endif
