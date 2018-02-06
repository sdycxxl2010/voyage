/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月25日
 *  Author:Sen
 *  global enum 
 *
 */

#ifndef __ENUM__LIST__H__
#define __ENUM__LIST__H__

#include <string>

enum ITEM_TYPE
{
	ITEM_TYPE_GOOD,			 //货物
	ITEM_TYPE_SHIP,			 //船只
	ITEM_TYPE_SHIP_EQUIP,	 //船只装备
	ITEM_TYPE_PORP,			 //道具
	ITEM_TYPE_ROLE_EQUIP,    //人物装备
	ITEM_TYPE_DRAWING,		 //图纸
	ITEM_TYPE_SPECIAL,       //特殊
};

/*
*1：船首像；2：撞角；3：船尾炮（废弃）；4：船帆；5：（废弃）；6：装甲；7：火炮；
*/
enum SHIP_SUB_TYPE
{
	SUB_TYPE_SHIP_FIGUREHEAD = 1,
	SUB_TYPE_SHIP_EMBOLON,
	SUB_TYPE_SHIP_STERN_GUN,
	SUB_TYPE_SHIP_SAIL,
	SUB_TYPE_SHIP_ABANDON,
	SUB_TYPE_SHIP_ARMOUR,
	SUB_TYPE_SHIP_GUN,
};

/*
*0：使用类道具; 1：制造素材； 2：船只强化计划； 3：装备强化计划 ; 4：送给酒吧 ; 5 : 故事碎片
*/
enum PROP_SUB_TYPE
{
	SUB_TYPE_USE,
	SUB_TYPE_MATERIAL,
	SUB_TYPE_SHIP_PLAN,
	SUB_TYPE_EQUIP_PLAN,
	SUB_TYPE_GIRL_GIFT,
	SUB_TYPE_STORY_DEBRIS,
};

/*
*使用类道具 p4 = 1:战斗道具; p4 = 2:航海道具; p4 = 3:伪装道具; p4 = 4:特殊道具。
*/
enum USE_SUB_SUB_TYPE
{
	SUB_SUB_TYPE_COMBAT_PROP = 1,
	SUB_SUB_TYPE_SAILING_PROP,
	SUB_SUB_TYPE_DISGUISE_PROP,
	SUB_SUB_TYPE_SPECIAL_PROP,
};

/*
*装备强化计划（p4 = 1:火炮；p4 = 2：船帆；p4 = 3：装甲；）
*/
enum EQUIP_PLAN_SUB_SUB_TYPE
{
	SUB_SUB_TYPE_EQUIP_GUN_PLAN = 1,
	SUB_SUB_TYPE_EQUIP_SAIL_PLAN,
	SUB_SUB_TYPE_EQUIP_ARM_PLAN,
};
/*
*1:帽子;2:衣服;3:武器;4:饰品;5: 鞋子
*/
enum ROLE_EQUIP_SUB_TYPE
{
	SUB_TYPE_ROLE_EQUIP_HAT = 1,
	SUB_TYPE_ROLE_EQUIP_CLOTHES,
	SUB_TYPE_ROLE_EQUIP_ARM,
	SUB_TYPE_ROLE_EQUIP_ACCESSORY,
	SUB_TYPE_ROLE_EQUIP_SHOE
};

enum CHARACTERINFO
{
	//攻击力
	CHARACTER_ATTACK = 0,
	//防御
	CHARACTER_DEFENCE,
	//血量
	CHARACTER_HP,
};

enum IMAGE_ICON_TYPE
{
	IMAGE_ICON_OUTLINE = 1, //带描边图片 128
	IMAGE_ICON_SHADE,	//带阴影图片  256
};

enum FIGUR_FALG     //人物NPC的类型
{
	FLAG_BAR_GIRL,		//酒吧女郎
	FLAG_CHIEF_OFFICER, //副官
	FLAG_SOLDIER,		//士兵
	FLAG_SUPPLY_OFFICER,//供给官员
	FLAG_UNCLE_TRADE,	//交易所大叔
	FLAG_SHIP_BOSS,      //船坞老板
};
//城市中不同位置
enum CITY_AREA_FALG   
{
	//船坞
	FLAG_DOCK_AREA,
	//酒馆
	FLAG_PUP_AREA,	
	//王宫
	FLAG_PALACE_AREA,
	//港口
	FLAG_WHARF_AREA,
};

enum NPC_TYPE     //海上NPC的类型
{
	NPC_TYPE_CARAVAN = 1,	//商队
	NPC_TYPE_CORVETTE,  //护卫舰
	NPC_TYPE_NAVY,		//海军
	NPC_TYPE_PIRATE,    //海盗
};
//技能的类型(NPC,船长,玩家,小伙伴特殊技能，小伙伴特殊技能)
enum SKILL_TYPE
{
	SKILL_TYPE_NPC = 0,
	SKILL_TYPE_CAPTAIN = 1,
	SKILL_TYPE_PLAYER = 2,
	SKILL_TYPE_COMPANION_NORMAL = 3,
	SKILL_TYPE_PARTNER_SPECIAL = 4,
};

//技能
#define SKILL_DER_MIN 10.0
#define SKILL_DER_MAX 1000.0
enum SKILL_ID
{
	SKILL_POWWEFUL_SHOOT = 1,   //强力炮击    (主动技能)
	SKILL_SALVO_SHOOT,			//齐射           (主动技能)
	SKILL_INCENDIARY_SHOOT,     //燃烧弹     (主动技能)
	SKILL_HAMMER_SHOOT,			//链球炮弹(主动技能)
	SKILL_FAST_RELOAD,			//快速装填
	SKILL_WEAKNESS_ATTACK,		//弱点攻击
	SKILL_ASSAULT_NAVIGATION,	//突击航行  (主动技能)
	SKILL_SNEAK_ATTACK,         //奇袭             (主动技能)
	SKILL_EMBOLON_ATTACK,		//撞角攻势    (主动技能)
	SKILL_ATTACKING_HORN,		//攻击号角    (主动技能)
	SKILL_INDOMITABLE_WILL,     //不屈意志
	SKILL_ROAR_OF_VICTORY,      //胜利呐喊
	SKILL_PROGRESSIVE_REPAIR,   //缓慢修理		(主动技能)
	SKILL_EMERGENT_REPAIR,		//应急修理		(主动技能)
	SKILL_FORTRESS_BATTLESHIP,  //海上堡垒		(主动技能)
	SKILL_FLEET_REPAIR,			//舰队修复			(主动技能)
	SKILL_ARMOUR_OPTIMIZATION,  //装甲优化
	SKILL_REFORM_OF_HULL,		//舰体改良
	SKILL_TAX_PROTOCOL,         //税务协定
	SKILL_EXPERT_SELLOR,        //售卖专家
	SKILL_PUBLIC_SPEAKING,      //演说技巧
	SKILL_ADMINISTRATION,       //市政厅常客
	SKILL_TECHNIQUE_OF_BUSINESS, //商贸协定
	SKILL_TECHNIQUE_OF_CARGO,   //货船技巧
	SKILL_MANUFACTURING_EXPERT, //制造专家
	SKILL_SHIPBUILDING_TECHNOLOGY,//造船工艺
	SKILL_OCEAN_EXPLORATION,	//海洋探索
	SKILL_GOOD_COOK,			//烹饪技巧
	SKILL_MISSION_TERMINATOR,   //任务达人
	SKILL_LATE_SUBMISSION,      //延时提交
	//严阵以待
	SKILL_COMBAT_READY,
	//劝降
	SKILL_INDUCE_TO_CAPITULATE,
	//掠夺高手
	SKILL_PREDATORY_MASTE,
	//黑吃黑
	SKILL_BLACK_EAT_BLACK,
	//食物掠夺
	SKILL_FOOD_LOOTING,
	//船体牵引
	SKILL_HULL_TRACTION,
	//神射手
	SKILL_GREAT_SHOOTER,
};

enum CAPTAIN_SKILL_ID
{
	//火炮学
	SKILL_CAPTAIN_GUN_EXPERT = 1,
	//冲锋
	SKILL_CAPTAIN_RAM_EXPERT,
	//链球炮
	SKILL_CAPTAIN_HAMMER_SHOOT,
	//要害射击学
	SKILL_CAPTAIN_WEAKNESS_ATTACK,
	//装甲学
	SKILL_CAPTAIN_ARMOR_DEFENSE,
	//斗志		
	SKILL_CAPTAIN_UNBEATABLE_WILL,
	//激励
	SKILL_CAPTAIN_ROAR_OF_VICTORY,
	//销售学
	SKILL_CAPTAIN_BEST_SELLOR,
	//烹饪
	SKILL_CAPTAIN_COOK_SKILL,
	//演讲
	SKILL_CAPTAIN_TRANSACTION_REPUTAION,
	//任务学
	SKILL_CAPTAIN_MISSION_EXPERT,
	//进阶任务学	
	SKILL_CAPTAIN_SECRETARY,
	//瞄准	
	SKILL_CAPTAIN_ACCURACY,
	//操帆
	SKILL_CAPTAIN_SAIL_OPERATION,
	//瞭望
	SKILL_CAPTAIN_OUTLOOK,
	//仓储
	SKILL_CAPTAIN_WAREHOUSING,
	//后勤
	SKILL_CAPTAIN_LOGISTICS,
};

enum COMPANION_NORMAL_SKILL_ID
{
	//火炮学
	SKILL_COMPANION_NORMAL_GUN_EXPERT = 1,
	//冲锋
	SKILL_COMPANION_NORMAL_RAM_EXPERT,
	//链球炮
	SKILL_COMPANION_NORMAL_HAMMER_SHOOT,
	//要害射击学
	SKILL_COMPANION_NORMAL_WEAKNESS_ATTACK,
	//装甲学
	SKILL_COMPANION_NORMAL_ARMOR_DEFENSE,
	//斗志		
	SKILL_COMPANION_NORMAL_UNBEATABLE_WILL,
	//激励
	SKILL_COMPANION_NORMAL_ROAR_OF_VICTORY,
	//销售学
	SKILL_COMPANION_NORMAL_BEST_SELLOR,
	//烹饪
	SKILL_COMPANION_NORMAL_COOK_SKILL,
	//演讲
	SKILL_COMPANION_NORMAL_TRANSACTION_REPUTAION,
	//任务学
	SKILL_COMPANION_NORMAL_MISSION_EXPERT,
	//进阶任务学	
	SKILL_COMPANION_NORMAL_SECRETARY,
	//瞄准	
	SKILL_COMPANION_NORMAL_ACCURACY,
	//操帆
	SKILL_COMPANION_NORMAL_SAIL_OPERATION,
	//瞭望
	SKILL_COMPANION_NORMAL_OUTLOOK,
	//仓储
	SKILL_COMPANION_NORMAL_WAREHOUSING,
	//后勤
	SKILL_COMPANION_NORMAL_LOGISTICS,
};

enum UI_VIEW_TYPE
{
	UI_NONE,
	UI_PORT,
	UI_BANK,
	UI_WHARF,
	UI_DOCK,
	UI_EXCHANGE,
	UI_PUB,
	UI_CABIN,
	UI_MAIL,
	UI_CENTER,
	UI_MAP,
	UI_PALACE,
	UI_BATTLE,
	UI_START,
	UI_ATTACK,//海盗场景中用到
	//国战
	UI_COUNTRY_WAR,
};

enum MUSIC_TYPE  //音乐类型
{
	MUSIC_CITY,
	MUSIC_PALACE,
	MUSIC_TAVERN,
	MUSIC_SAILING,
	MUSIC_BATTLE,
	MUSIC_AFTER_BATTLE,
	MUSIC_START,
	MUSIC_BEFORE_BATTLE,
};

enum BATTLE_RESULT   //战斗结果
{
	BATTLE_WIN = 1,	//胜利
	BATTLE_DEFEATED,//失败
	BATTLE_DRAW,	//平局
	BATTLE_FLEE		//道具逃跑
};


//邮件的csb文件
static const std::string MAIL_COCOS_RES[] = 
{
	"cocosstudio/mail/voyageUI_mail.csb",
	"cocosstudio/mail/voyageUI_mail_choosefriend.csb",
	"cocosstudio/mail/voyageUI_mail_viewplayer.csb",
	"cocosstudio/mail/voyageUI_mail_viewsystem.csb",
	"cocosstudio/mail/voyageUI_mail_write.csb",
};
enum MAIL_LIST
{
	MAIL_CSB,
	MAIL_CHOOSE_FRIEND_CSB,
	MAIL_VIEW_PLAYER_CSB,
	MAIL_VIEW_SYSTEM_CSB,
	MAIL_WRITE_CSB,
};
//公用窗口 CommonView
static const std::string COMMOM_COCOS_RES[]=
{
	"cocosstudio/common/voyageUI_common_confirm.csb",
	"cocosstudio/common/voyageUI_common_confirmcoin.csb",
	"cocosstudio/common/voyageUI_common_confirmticket.csb",
	"cocosstudio/common/voyageUI_common_dialogescort.csb",
	"cocosstudio/common/voyageUI_common_dialoghelp.csb",
	"cocosstudio/common/voyageUI_common_dialogprovide.csb",
	"cocosstudio/common/voyageUI_common_dialogskill.csb",
	"cocosstudio/common/voyageUI_common_dialogwarning.csb",
	"cocosstudio/common/voyageUI_common_dialoginfobtn_pic.csb",
	"cocosstudio/common/voyageUI_common_exp.csb",
	"cocosstudio/common/voyageUI_common_expcaptian.csb",
	"cocosstudio/common/voyageUI_common_getitems.csb",
	"cocosstudio/common/voyageUI_common_legend.csb",
	"cocosstudio/common/voyageUI_common_leveldown_rep.csb",
	"cocosstudio/common/voyageUI_common_levelup_exp.csb",
	"cocosstudio/common/voyageUI_common_levelup_rep.csb",
	"cocosstudio/common/voyageUI_common_numpad.csb",
	"cocosstudio/common/voyageUI_common_result.csb",
	"cocosstudio/common/voyageUI_common_resultlist.csb",
	"cocosstudio/common/voyageUI_common_resulttext.csb",
	"cocosstudio/common/voyageUI_common_selectitem.csb",
	"cocosstudio/common/voyageUI_common_viewblueprint.csb",
	"cocosstudio/common/voyageUI_common_viewequip.csb",
	"cocosstudio/common/voyageUI_common_viewgoods.csb",
	"cocosstudio/common/voyageUI_common_viewguild.csb",
	"cocosstudio/common/voyageUI_common_viewnoweight.csb",
	"cocosstudio/common/voyageUI_common_viewplayer.csb",
	"cocosstudio/common/voyageUI_common_viewship.csb",
	"cocosstudio/common/voyageUI_common_viewenhance.csb",
	"cocosstudio/common/voyageUI_common_broken.csb",
	"cocosstudio/common/voyageUI_common_pause.csb",
	"cocosstudio/common/voyageUI_common_confirm_textbtn.csb",
	"cocosstudio/common/voyageUI_common_confirmcoin_concise.csb",
	"cocosstudio/common/voyageUI_common_result_salvage.csb",
	"cocosstudio/common/voyageUI_common_confirmstore.csb",
	"cocosstudio/common/voyageUI_common_resultreward.csb",
	"cocosstudio/common/voyageUI_common_maintain.csb",
	"cocosstudio/common/voyageUI_common_dialogwarning_2.csb",
	"cocosstudio/common/voyageUI_common_confirmrepair.csb",
	"cocosstudio/common/voyageUI_common_importantnotify.csb",
	"cocosstudio/common/voyageUI_common_confirm_textbtn_2.csb",
	"cocosstudio/common/voyageUI_common_viewship_cabins.csb",
	"cocosstudio/common/voyageUI_common_viewship_parts.csb",
	"cocosstudio/common/voyageUI_common_dialogloading.csb", 
	"cocosstudio/common/voyageUI_common_confirm_multiplelines.csb",
	"cocosstudio/common/voyageUI_common_viewplayer_equip.csb",
	"cocosstudio/common/voyageUI_common_viewplayer_fleet.csb",
	"cocosstudio/common/voyageUI_common_viewforcestatus.csb",
	"cocosstudio/common/voyageUI_common_dialog_cityinfobonus.csb",
};
enum COMMON_LIST
{         
	C_VIEW_ITEM_ESCORT_CSB,				//	带确定取消的提示框
	C_VIEW_COST_SILVER_COM_CSB,				//消费银币提示框
	C_VIEW_COST_V_COM_CSB,						//花费V票
	C_VIEW_ESCORT_INFO_CSB,						//护送服务解释
	C_VIEW_INFOBTN_CSB,								//解释信息框
	C_VIEW_PROVIDE_SELECT_CSB,					//选择条
	C_VIEW_SKILL_DES_CSB,								//技能详情描述
	C_VIEW_WARNING_CSB,							//警告提示框
	//酒吧女郎状态说明按钮打开界面
	C_VIEW_DIALOG_INFO_CSB,
	C_VIEW_ADD_EXP_CSB,								//显示增加经验和声望，带人物头像
	C_VIEW_CAPTAIN_ADDEXP_CSB,				//船长加经验
	C_VIEW_GETITEM_CSB,								//获得物品
	C_VIEW_ATTRIBUTE_CSB,							//一些图片的详情信息
	C_VIEW_R_LEVEL_DOWN_CSB,					//声望降低
	C_VIEW_LEVEL_UP_CSB,								//经验升级
	C_VIEW_R_LEVEL_UP_CSB,							//声望升级
	C_VIEW_NUMPAD_CSB,								//数字版
	C_VIEW_HIRE_RESULT_CSB,						//雇用成功提示框,投资成功提示框
	C_VIEW_SAILLING_ACCIDENT_CSB,			//海上事件结算提示框
	C_VIEW_RESULTTEXT_CSB,							//带标题的,只有一个确定按钮的提示框
	C_VIEW_ITEM_CHOOSE_CSB,						//	选择物品
	C_VIEW_DRAWING_DETAIL_CSB,				//图纸详情
	C_VIEW_EQUIP_DETAIL_CSB,						//装备基本信息
	C_VIEW_ITEM_DETAIL_CSB,						//物品详情
	C_VIEW_GUILD_CSB,									//邀请到公会
	C_VIEW_SPECIAL_DETAIL_CSB,					//特殊物品
	C_VIEW_FRIEND_DETAIL_CSB,					//好友信息
	C_VIEW_SHIP_DETAIL_CSB,							//船的基本信息
	C_VIEW_ENHANCE_DETAIL_CSB,                      //强化道具的详细信息   
	C_VIEW_EQUIPMENT_BROKEN_CSB,                    //装备损坏
	C_VIEW_PAUSE_GAME,                    //暂停页面
	C_VIEW_EXIT_RECONNECT, //退出游戏或重连服务器
	//海上打捞花费确认框
	C_VIEW_SALVAGE_COIN,
	//海上打捞结果
	C_VIEW_SALVAGE_RESULT,
	//商城花费确认框
	C_VIEW_CONFIRM_STORE,
	//祈祷获得物品确认框
	C_VIEW_GET_REWARD,
	//服务器维护框
	C_VIEW_SERVER_MAINTAIN,
	//warning,带滚动条，显示长文本
	C_VIEW_WARNING_LONGTEXT_CSB,
	//装备修理对话框
	COMMON_CONFIRM_REPAIR,
	//服务器通知
	C_VIEW_SERVER_NOTIFICATION,
	//游戏升级界面
	C_VIEW_UPDATE_GAME,
	//船只详细信息的舱室界面
	C_VIEW_SHIP_CABIN,
	//船只详细信息的装备界面
	C_VIEW_SHIP_PART,
	//国战loading
	C_VIEW_NATION_WAR_LOADING,
	//删除角色提示框
	C_VIEW_MULTIPLELINES_CSB,
	//显示人物模型界面
	C_VIEW_PLAYER_EQUIP_CSB,
	//显示船只3d模型界面
	C_VIEW_PLAYER_FLEET_CSB,
	//显示所有国家的所有城市
	C_VIEW_CESTATUS_CSB,
	//显示非首都城市对首都城市的制造加成
	C_VIEW_DIALOG_CITYINFOBOUNS_CSB,
};
//公用inform
static const std::string INFORM_COCOS_RES[]=
{
	"cocosstudio/inform/voyageUI_inform_error_confirm.csb",
	"cocosstudio/inform/voyageUI_inform_error.csb",
	"cocosstudio/inform/voyageUI_inform_toast.csb",

};
enum INFORM_LIST
{
	C_VIEW_CONFIRM_CSB,//操作成功提示框,两个按钮
	C_VIEW_ERROR_CONFIRM_CSB,//普通操作提示框，一个按钮
	C_VIEW_OTHER_TOAST_CSB,//操作成功提示框,自动关闭

};

//公会的csb文件
static const std::string GUILD_COCOS_RES[] = 
{
	"cocosstudio/guild/voyageUI_guild.csb",
	"cocosstudio/guild/voyageUI_guild_applications.csb",
	"cocosstudio/guild/voyageUI_guild_creat.csb",
	"cocosstudio/guild/voyageUI_guild_creat_failed.csb",
	"cocosstudio/guild/voyageUI_guild_invitations.csb",
	"cocosstudio/guild/voyageUI_guild_list.csb",
	"cocosstudio/guild/voyageUI_guild_manager.csb",
	"cocosstudio/guild/voyageUI_guild_myguild.csb",
	"cocosstudio/guild/voyageUI_guild_search.csb",
	"cocosstudio/guild/voyageUI_guild_search_result.csb",
};
enum GUILD_LIST
{
	GUILD_CSB,
	GUILD_APPLICATIONS_CSB,
	GUILD_CREAT_CSB,
	GUILD_CREAT_FAILED_CSB,
	GUILD_INVITATIONS_CSB,
	GUILD_LIST_CSB,
	GUILD_MANAGER_CSB,
	GUILD_MYGUILD_CSB,
	GUILD_SEARCH_CSB,
	GUILD_SEARCH_RESULT_CSB,
};


//船坞的csb文件
static const std::string SHIPYARD_COCOS_RES[] = 
{
	"cocosstudio/shipyard/voyageUI_shipyard.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet_formation.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet_formation_choosecaptain.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet_formation_postion.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet_gear.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet_gear_change.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_repair.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_workshop.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_workshop_build.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_workshop_build_dialog.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_workshop_build_slot.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_workshop_enhance.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_workshop_enhance_choose.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_workshop_enhance_result.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_workshop_enhance_slot.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_workshop_build_result.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_dialog.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet_gear_cabin.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet_gear_cabin_choose.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet_gear_cabin_l.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet_gear_cabin_m.csb",
	"cocosstudio/shipyard/voyageUI_shipyard_fleet_gear_cabin_s.csb",
};
enum SHIPYARD_LIST
{
	SHIPYARD_CSB,
	SHIPYARD_FLEET_CSB,
	SHIPYARD_FLEET_FORMATION_CSB,
	SHIPYARD_FLEET_FORMATION_CHOOSECAPTAIN_CSB,
	SHIPYARD_FLEET_FORMATION_POSITION_CSB,
	SHIPYARD_FLEET_FEAR_CSB,
	SHIPYARD_FLEET_FEAT_CHANGE_CSB,
	SHIPYARD_REPAIR_CSB,
	SHIPYARD_WORKSHOP_CSB,
	SHIPYARD_WORKSHOP_BUILD_CSB,
	SHIPYARD_WORKSHOP_BUILD_DIALOG_CSB,
	SHIPYARD_WORKSHOP_BUILD_SLOT_CSB,
	SHIPYARD_WORKSHOP_ENHANCE_CSB,
	SHIPYARD_WORKSHOP_ENHANCE_CHOOSE_CSB,
	SHIPYARD_WORKSHOP_ENHANCE_RESULT_CSB,
	SHIPYARD_WORKSHOP_SLOT_CSB,
	SHIPYARD_WORKSHOP_BUILD_RESULT_CSB,
	SHIPYARD_DIALOG_CSB,
	SHIPYARD_FLEET_GEAR_CABIN_CSB,
	SHIPYARD_FLEET_GEAR_CABIN_CHOOSE_CSB,
	SHIPYARD_FLEET_GEAR_CABIN_L_CSB,
	SHIPYARD_FLEET_GEAR_CABIN_M_CSB,
	SHIPYARD_FLEET_GEAR_CABIN_S_CSB,
};
//王宫的csb文件
static const std::string PALACE_COCOS_RES[] = 
{
	"cocosstudio/palace/voyageUI_palace.csb",
	"cocosstudio/palace/voyageUI_palace_hall.csb",
	"cocosstudio/palace/voyageUI_palace_info.csb",
	"cocosstudio/palace/voyageUI_palace_invest.csb",
	"cocosstudio/palace/voyageUI_palace_invest_dialog.csb",
	"cocosstudio/palace/voyageUI_palace_office.csb",
	"cocosstudio/palace/voyageUI_palace_office_changepolicy.csb",
	"cocosstudio/palace/voyageUI_palace_office_officeundecided.csb",
	"cocosstudio/palace/voyageUI_palace_officenotmayor.csb",
	"cocosstudio/palace/voyageUI_palace_war.csb",
	"cocosstudio/palace/voyageUI_palace_war_launchwar.csb",
	"cocosstudio/palace/voyageUI_palace_war_launchwar_result.csb",
	"cocosstudio/palace/voyageUI_palace_war_ranking.csb",
	"cocosstudio/palace/voyageUI_palace_war_rules.csb",
};
enum PALACE_LIST
{
	VIEW_PLACE_CSB,//首页，带对话
	VIEW_HALL_OF_HONOR_CSB,//投资排行
	VIEW_CITYINFO_CSB,//城市信息
	VIEW_INVESTMENT_CSB,//投资首页
	VIEW_INVEST_DIALOG_CSB,//投资输入框
	VIEW_OFFICE_DECIDED_CSB,//office显示
	VIEW_CHANGE_POLICY_CSB,//选择发展方向
	VIEW_OFFICE_UNDECIDED_CSB,//第一天office 
	VIEW_ANNOUNCEMENT_CSB,//非市长
	//国战
	VIEW_WAR_CSB,
	//战书
	VIEW_LAUNCHWAR_CSB,
	//战书发送结果
	VIEW_LAUNCHWAR_RESULT_CSB,
	//国战排行
	VIEW_WAR_RANKING_CSB,
	//奖励和规则
	VIEW_WAR_RULES,
};


//酒馆的csb文件
static const std::string TARVEN_COCOS_RES[] = 
{
	"cocosstudio/tarven/voyageUI_tarven.csb",
	"cocosstudio/tarven/voyageUI_tarven_2.csb",
	"cocosstudio/tarven/voyageUI_tarven_chat.csb",
	"cocosstudio/tarven/voyageUI_tarven_chat_gift.csb",
	"cocosstudio/tarven/voyageUI_tarven_chat_result.csb",
	"cocosstudio/tarven/voyageUI_tarven_hire.csb",
	"cocosstudio/tarven/voyageUI_tarven_hire_confirmfire.csb",
	"cocosstudio/tarven/voyageUI_tarven_hire_dialog.csb",
	"cocosstudio/tarven/voyageUI_tarven_order.csb",
	"cocosstudio/tarven/voyageUI_tarven_order_result.csb",
	"cocosstudio/tarven/voyageUI_tarven_quest.csb",
	"cocosstudio/tarven/voyageUI_tarven_recruit.csb",
	"cocosstudio/tarven/voyageUI_tarven_recruit_dialog.csb",
	"cocosstudio/tarven/voyageUI_tarven_quest_accept.csb",
};
enum TARVEN_LIST
{
	TARVEN_CSB,
	TARVEN_TWO_CSB,
	TARVEN_CHAT_CSB,
	TARVEN_CHAT_GIFT_CSB,
	TARVEN_CHAT_RESULT_CSB,
	TARVEN_HIRE_CSB,
	TARVEN_HIRE_CONFIRMFIRE_CSB,
	TARVEN_HIRE_DIALOG_CSB,
	TARVEN_ORDER_CSB,
	TARVEN_ORDER_RESULT_CSB,
	TARVEN_QUEST_CSB,
	TARVEN_RECRUIT_CSB,
	TARVEN_RECRUIT_DIALOG_CSB,
	TARVEN_QUEST_ACCEPT_CSB,
};
//港口csb文件
static const std::string DOCK_COCOS_RES[] = 
{
	"cocosstudio/dock/voyageUI_dock.csb",
	"cocosstudio/dock/voyageUI_dock_map.csb",
	"cocosstudio/dock/voyageUI_dock_supplies.csb",
	"cocosstudio/dock/voyageUI_dock_autopilot.csb",
};
enum DOCK_LIST
{
	VIEW_CHIEF_DIALOG_CSB,//人物对话,VIEW_ACTIONBAR,//金银币
	VIEW_DOCK_MAP_CSB,//地图
	VIEW_BUY_SUPPLY_CSB,//购买补给品
	VIEW_AUTOPILOT_CSB, //自动航行选择界面
};
//银行csb文件
static const std::string BANK_COCOS_RES[] =
{
	"cocosstudio/bank/voyageUI_bank.csb",
	"cocosstudio/bank/voyageUI_bank_currency.csb",
	"cocosstudio/bank/voyageUI_bank_logguild.csb",
	"cocosstudio/bank/voyageUI_bank_safe.csb",
	"cocosstudio/bank/voyageUI_bank_safe_slot.csb",
	"cocosstudio/bank/voyageUI_bank_insurance.csb",
	"cocosstudio/bank/voyageUI_bank_insurance_buy.csb",
};
enum BANK_LIST
{
	VIEW_BANK_MAIN_CSB,//银币，物品，记录
	VIEW_CURRENY_CSB,//存钱取钱,VIEW_CONTENT_GUILD_NO,
	VIEW_LOG_GUILD_CSB,//VIEW_LOG_NO,//没有记录
	VIEW_SAFE_CSB,//存储物品面板
	VIEW_SLOT_DETAIL_CSB,//取物品的时候先出现这个对话框
	//保险界面
	VIEW_BANK_INSURANCE_CSB,
	//保险界面选择
	VIEW_BANK_INSURANCE_BUY_CSB,
};
//设置csb文件
static const std::string SETTING_COCOS_RES[] =
{
	"cocosstudio/settings/voyageUI_settings_account.csb",
	"cocosstudio/settings/voyageUI_settings.csb",
	"cocosstudio/settings/voyageUI_settings_help.csb",
	"cocosstudio/settings/voyageUI_settings_help_legend.csb",
	"cocosstudio/settings/voyageUI_settings_info.csb",
	"cocosstudio/settings/voyageUI_settings_info_credits.csb",
	"cocosstudio/settings/voyageUI_settings_help_customservice.csb",
	"cocosstudio/settings/voyageUI_settings_help_customservice_write.csb",
	"cocosstudio/settings/voyageUI_settings_info_eula.csb",
	"cocosstudio/settings/voyageUI_settings_language.csb",
	"cocosstudio/settings/voyageUI_settings_account_password.csb",
	"cocosstudio/settings/voyageUI_settings_account_mail.csb",

};

enum SETTING_LIST
{
	//账户
	VIEW_ACCOUNT_CSB,
	//主界面
	VIEW_SET_CSB,
	//帮助
	VIEW_HELP_CSB,
	//图鉴
	VIEW_LEGEND_CSB,
	//信息
	VIEW_INFO_CSB,
	//制作
	VIEW_CREDITS_CSB,
	//客服
	VIEW_CUSTOMERSERVICE,
	//反映问题
	VIEW_CUSTOMERSERVICE_WRITE,
	//协议许可
	VIEW_INFO_EULA,
	//语言管理
	VIEW_LANGUAGE,
	/*修改密码*/
	VIEW_SET_PASSWORD,
	//验证邮箱
	VIEW_ACCOUNT_EMAIL,
	
};


//交易所的csb文件
static const std::string MARKET_COCOS_RES[] = 
{
	"cocosstudio/market/voyageUI_market.csb",
	"cocosstudio/market/voyageUI_market_buy.csb",
	"cocosstudio/market/voyageUI_market_buy_info.csb",
	"cocosstudio/market/voyageUI_market_buy_viewequip.csb",
	"cocosstudio/market/voyageUI_market_buy_viewgoods.csb",
	"cocosstudio/market/voyageUI_market_buy_viewship.csb",
	"cocosstudio/market/voyageUI_market_order.csb",
	"cocosstudio/market/voyageUI_market_order_buy.csb",
	"cocosstudio/market/voyageUI_market_order_sell.csb",
	"cocosstudio/market/voyageUI_market_order_view.csb",
	"cocosstudio/market/voyageUI_market_sell.csb",
	"cocosstudio/market/voyageUI_market_sell_info.csb",
	"cocosstudio/market/voyageUI_market_sell_viewgoods.csb",
	"cocosstudio/market/voyageUI_market_sell_viewgoodspopular.csb",
};
enum MARKET_LIST
{
	MARKET_CSB,
	MARKET_BUY_CSB,
	MARKET_BUY_INFO_CSB,
	MARKET_BUY_VIEWEQUIP_CSB,
	MARKET_BUY_VIEWGOODS_CSB,
	MARKET_BUY_VIEWSHIP_CSB,
	MARKET_ORDER_CSB,
	MARKET_ORDER_BUY_CSB,
	MARKET_ORDER_SELL_CSB,
	MARKET_ORDER_VIEW_CSB,
	MARKET_SELL_CSB,
	MARKET_SELL_INFO_CSB,
	MARKET_SELL_VIEWGOODS_CSB,
	MARKET_SELL_VIEWGOODSPOPULAR_CSB,
};


//个人中心的csb文件
static const std::string PLAYER_COCOS_RES[] = 
{
	"cocosstudio/player/voyageUI_player.csb",
	"cocosstudio/player/voyageUI_player_charactor.csb",
	"cocosstudio/player/voyageUI_player_charactorequip.csb",
	"cocosstudio/player/voyageUI_player_charactorequip_change.csb",
	"cocosstudio/player/voyageUI_player_charactorskill.csb",
	"cocosstudio/player/voyageUI_player_charactorskill_slotchange.csb",
	"cocosstudio/player/voyageUI_player_charactorskill_tree.csb",
	"cocosstudio/player/voyageUI_player_charactorskill_tree_viewskill.csb",
	"cocosstudio/player/voyageUI_player_charactorstastics.csb",
	"cocosstudio/player/voyageUI_player_dropitem.csb",
	"cocosstudio/player/voyageUI_player_inventory.csb",
	"cocosstudio/player/voyageUI_player_warehouse.csb",
	"cocosstudio/player/voyageUI_player_warehouse_viewitem.csb",
	"cocosstudio/player/voyageUI_player_charactorshipp.csb",
	"cocosstudio/player/voyageUI_player_charactorshipp_learn.csb",
	"cocosstudio/player/voyageUI_player_charactorstastics_bl_detail.csb",
};
enum PLAYER_LIST
{
	PLAYER_CSB,
	PLAYER_CHARACTOR_CSB,
	PLAYER_CHARACTOR_EQUIP_CSB,
	PLAYER_CHARACTOR_EQUIP_CHANGE_CSB,
	PLAYER_CHARACTOR_SKILL_CSB,
	PLAYER_CHARACTOR_SKILL_SLOTCHANGE_CSB,
	PLAYER_CHARACTOR_SKILL_TREE_CSB,
	PLAYER_CHARACTOR_SKILL_TREE_VIEWSKILL_CSB,
	PLAYER_CHARACTOR_STASTICS_CSB,
	PLAYER_DROP_ITEM_CSB,
	PLAYER_INVENTORY_CSB,
	PLAYER_WAREHOUSE_CSB,
	PLAYER_WAREHOUSE_VIEWITEM_CSB,
	PLAYER_CHARACTOR_SHIP_PRO_CSB,
	PLAYER_CHARACTOR_SHIP_PRO_LEARN_CSB,
	PLAYER_CHARACTOR_SHIP_PRO_BL_DETAIL_CSB,
};


//海上csb文件
static const std::string MAPUI_COCOS_RES[] =
{
	"cocosstudio/sailing/voyageUI_sailing.csb",
	"cocosstudio/sailing/voyageUI_sailing_battleresult1.csb",
	"cocosstudio/sailing/voyageUI_sailing_battleresult2.csb",
	"cocosstudio/sailing/voyageUI_sailing_discover.csb",
	"cocosstudio/sailing/voyageUI_sailing_event.csb",
	"cocosstudio/sailing/voyageUI_sailing_fleetinfo.csb",
	"cocosstudio/sailing/voyageUI_sailing_fleetinfo_ship.csb",
	"cocosstudio/sailing/voyageUI_sailing_land.csb",
	"cocosstudio/sailing/voyageUI_sailing_land_result.csb",
	"cocosstudio/sailing/voyageUI_sailing_npcinfo.csb",
	"cocosstudio/sailing/voyageUI_sailing_startbattle.csb",
	"cocosstudio/sailing/voyageUI_sailling_die_result.csb",

	"cocosstudio/sailing/voyageUI_sailing_battlefield.csb",
	"cocosstudio/sailing/voyageUI_sailing_battlefield_skillfx.csb",
	"cocosstudio/sailing/voyageUI_sailling_defeat.csb",
	"cocosstudio/sailing/voyageUI_sailing_battleresult_scene_pirata.csb",
	"cocosstudio/sailing/voyageUI_sailing_battleresult_scene_siege.csb",
	"cocosstudio/sailing/voyageUI_sailing_startrob.csb",
	"cocosstudio/sailing/voyageUI_sailing_offlineresult1.csb",
	"cocosstudio/sailing/voyageUI_sailing_offlineresult2.csb",
	"cocosstudio/sailing/voyageUI_sailing_battleresult1_m.csb",
	"cocosstudio/sailing/voyageUI_sailing_battleresult1_robbery.csb",

	"cocosstudio/sailing/voyageUI_sailling_land_war.csb",
	"cocosstudio/sailing/voyageUI_sailling_land_war_depot.csb",
	"cocosstudio/sailing/voyageUI_sailling_land_war_depot_repair.csb",
	"cocosstudio/sailing/voyageUI_sailing_battleonmap.csb",
	"cocosstudio/sailing/voyageUI_sailing_battleboard.csb",
	"cocosstudio/sailing/voyageUI_sailing_battleboard_win.csb",
};
enum MAPUI_LIST
{
	INDEX_UI_SAILLING_MAIN_CSB,//海上主界面，panel_sailing
	INDEX_UI_BUTTLE_RESULT_1_CSB,//战斗结果，主角结算
	INDEX_UI_BUTTLE_RESULT_2_CSB,//战斗结果，船队结算
	INDEX_UI_FOUND_NEWCITY_CSB,//发现新城市
	INDEX_UI_FLOAT_CSB,//海上事件
	INDEX_UI_FLEETINFO_CSB,//舰队信息第一个面板
	INDEX_UI_SHIPDETAILS_CSB,//舰队每一条船的信息
	INDEX_UI_LOAD_CSB,//登录城市第一个结算
	INDEX_UI_BACK_CITY_CSB,//登录城市主角结算框
	INDEX_UI_FLEETINFO_NPC_CSB,//海上点击npc船只
	INDEX_UI_BATTLE_VS_CSB,//进入战斗，对战
	INDEX_UI_DIE_RESULT_CSB,//海上死亡结算框

	INDEX_UI_FIGHT_UI_MAIN_CSB, //战斗中的主场景
	INDEX_UI_FIGHT_UI_SKILL_EFFECT_CSB,//战斗中使用技能效果
	INDEX_UI_FIGHT_DEFEATED_DIALOG_CSB,//战败或饿死的对话
	//海盗攻城活动
	INDEX_UI_BATTLERRESULT_SCENE_PIRATA_CSB,
	INDEX_UI_BATTLERRESULT_SCENE_SIEGE_CSB,
	//打劫界面
	INDEX_UI_ROB_UI_CSB,
	//登录离线委托结算
	INDEX_UI_ROB_RESULT_1_CSB,
	INDEX_UI_ROB_RESULT_2_CSB,
	//战斗结算船长经验增加界面
	INDEX_UI_BATTLE_RESULT_M_CSB,
	//打劫结算界面
	INDEX_UI_ROB_BATTLE_RESULT_CSB,

	//国战海上登陆、修船、雇佣水手
	COUNTRY_WAR_SEA_LAND_CSB,
	COUNTRY_WAR_SEA_LAND_DEPOT_CSB,
	COUNTRY_WAR_SEA_LAND_DEPOT_REPAIR_CSB,

	//国战场景UI层
	COUNTRY_WAR_UI_CSB,
	//单挑场景
	INDEX_UI_BATTLE_BOARD,
    //单挑胜利
	INDEX_UI_BATTLE_BOARD_WIN,
};
static const std::string	VILLAGE_COCOS_RES[] =
{
	"cocosstudio/village/voyageUI_village_governor.csb",
	"cocosstudio/village/voyageUI_village_tarven.csb",

};
enum VILLAGE_LIST
{
	VIEW_VILLAGE_GOVERNOR_CSB,//村庄王宫
	VIEW_VILLAGE_TARVEN_CSB,//村庄酒馆
};


//好友的csb文件
static const std::string SOCIAL_COCOS_RES[] =
{
	"cocosstudio/social/voyageUI_social.csb",
	"cocosstudio/social/voyageUI_social_search.csb",
	"cocosstudio/social/voyageUI_social_search_result.csb",
	"cocosstudio/voyageUI_chat.csb",
};
enum SOCIAL_LIST
{
	SOCIAL_CSB,
	SOCIAL_SEARCH_CSB,
	SOCIAL_SEARCH_RESULT_CSB,
	SOCIAL_CHAT_CSB,
};
static const std::string CITYINFO_RES[] =
{
	"cocosstudio/voyageUI_localinfo.csb",
};
enum CITYINFO_LIST
{
	CITYINFO_CSB,
};
//等级限时礼包和抽奖活动及首充v活动,国战海报
static const std::string GIFTPACK_RES[] =
{
	"cocosstudio/event/voyageUI_promotion.csb",
	"cocosstudio/event/voyageUI_event_lottery.csb",
	"cocosstudio/event/voyageUI_event_bonus.csb",
	"cocosstudio/event/voyageUI_event_war_prepare.csb",
};
enum GIFTPACK_LIST
{
	GIFTPACK_CSB,
	LOTTERY_CSB,
	BONUS_CSB,
	COUNTRY_WAR_PREPARE_POSTER,
};
/*创建角色csb*/
static const std::string CREATE_ROLE_RES[] =
{
	"cocosstudio/voyageUI_createrole_step1.csb",
	"cocosstudio/voyageUI_createrole_step2.csb",
	"cocosstudio/voyageUI_createrole_step3.csb",
};
enum CREAT_ROLE_LIST
{
	CREATE_ROLE_STEP_ONE,
	CREATE_ROLE_STEP_TWO,
	CREATE_ROLE_STEP_THREE,
};
//账户验证
static const std::string VERIFIY_ACCOUNT_RES[] =
{
	"cocosstudio/event/voyageUI_event_verifyaccount.csb",
	"cocosstudio/event/voyageUI_event_invited.csb",
	"cocosstudio/event/voyageUI_event_invited_reward.csb",
};
enum VERIFIY_ACCOUNT_LIST
{
	VIEW_VERIFIY_ACCOUNT,
	//被邀请得到奖励
	VIEW_FACEBOOK_GETREWARD,
	//邀请其他玩家
	VIEW_INVITE_OTHERS,
};
//账户锁定,更新日志
static const std::string ABNORMAL_ACCOUNT_RES[] =
{
	"cocosstudio/voyageUI_start_abnormalaccount.csb",
	"cocosstudio/voyageUI_start_update.csb",
	
};
enum ABNORMAL_ACCOUNT_LIST
{
	VIEW_ABNOMAL_ACCOUNT,
	VIEW_CHANGE_LOG,
};

//活动
static const std::string ACTIVITY_RES[] =
{
	"cocosstudio/activity/voyageUI_activity_attackpirates.csb",
	"cocosstudio/activity/voyageUI_activity_attackpirates_2.csb",
	"cocosstudio/activity/voyageUI_activity_attackpirates_attack.csb",
	"cocosstudio/activity/voyageUI_activity_attackpirates_gloryscore.csb",
	"cocosstudio/activity/voyageUI_activity_attackpirates_rules.csb",
	"cocosstudio/activity/voyageUI_activity_siegeofpirates_boss.csb",
	"cocosstudio/activity/voyageUI_activity_siegeofpirates_ranking.csb",
	"cocosstudio/activity/voyageUI_activity_siegeofpirates_rules.csb"
};
enum ACTUVITY_LIST
{
	ACTIVITY_ATTACK_PIRATES_CSB,
	ACTIVITY_ATTACK_PIRATES_TWO_CSB,
	ACTIVITY_ATTACK_PIRATES_ATTACK_CSB,
	ACTIVITY_ATTACK_PIRATES_SCORE_CSB,
	ACTIVITY_ATTACK_PIRATES_RULES_CSB,
	ACTIVITY_PIRATES_ATTACK_BOSS_CSB,
	ACTIVITY_PIRATES_ATTACK_RANKING_CSB,
	ACTIVITY_PIRATES_ATTACK_RULES_CSB,
};
//商城主界面
static const std::string STORE_RES[] =
{
	"cocosstudio/voyageUI_store.csb",
	"cocosstudio/voyageUI_store_category.csb",
};
enum STORE_LIST
{
	VIEW_STORE_MAIN_PANEL,
	VIEW_STORE_CATEGORY_CSB,
};
//每日签到
static const std::string EVERYDAYSIGN_RES[] =
{
	"cocosstudio/voyageUI_dailyreward.csb",
};
enum EVERYDAYSIGN_LIST
{
	VIEW_EVERYDAY_SIGN_PANEL,
};

/*
*小伙伴界面
*/
static const std::string COMPANION_RES[] =
{
	"cocosstudio/companions/voyageUI_companions.csb",
	"cocosstudio/companions/voyageUI_companions_details.csb",
	"cocosstudio/companions/voyageUI_companions_details_viewcard.csb",
	"cocosstudio/companions/voyageUI_handbook.csb",
	"cocosstudio/companions/voyageUI_handbook_insufficient.csb",
};
enum COMPANION_LIST
{
	COMPANIONS_CSB,
	COMPANIONS_DETAILS_CSB,
	DETAILS_VIEWCARD_CSB,
	COMPANIONS_HANDBOOK_CSB,
	COMPANIONS_INSUFFICIENT,
};
/*
*国战界面
*/
static const std::string COUNTRY_WAR_RES[] =
{
	"cocosstudio/war/voyageUI_events_war.csb",
	"cocosstudio/war/voyageUI_events_war_2.csb",
	"cocosstudio/war/voyageUI_events_war_defend.csb",
	"cocosstudio/war/voyageUI_events_war_prepare.csb",
	"cocosstudio/war/voyageUI_events_war_prepare_defense_upgrade_amount.csb",
	
	"cocosstudio/war/voyageUI_events_war_rules.csb",
	"cocosstudio/war/voyageUI_events_war_report.csb",
	"cocosstudio/war/voyageUI_palace_war_status.csb",
};
enum COUNTRY_WAR_LIST
{
	COUNTRY_WAR_CSB,
	COUNTRY_WAR_TWO_CSB,
	COUNTRY_WAR_ON_WARRING_DEFEND_CSB,
	COUNTRY_WAR_PREPARE_DEFENSE_CSB,
	COUNTRY_WAR_ON_WARRING_REPAIR_BY_V_CSB,
	COUNTRY_WAR_RULES_CSB,
	COUNTRY_WAR_REPORT_CSB,
	COUNTRY_WAR_STATUS_CSB,
};
#endif