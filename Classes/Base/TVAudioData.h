/*
*  CopyRight (c) 
*  Created on: 2014年11月18日
*  Author:  
*  description: music resource
*/
#ifndef __AUDIO_DATA_H__
#define __AUDIO_DATA_H__

//主城背景音乐，一些城市有独立的音乐，属于苏丹势力的播放08，其他播放欧洲小城市音乐07
static const char* audio_port[]={
	"audio/nation_bgMusic/01_Lisbon.mp3", //0 Portugal
	"audio/nation_bgMusic/02_Sevilla.mp3", //Spain
	"audio/nation_bgMusic/03_London.mp3", //England
	"audio/nation_bgMusic/04_Netherlands.mp3", 
	"audio/nation_bgMusic/05_Genova.mp3",   
	"audio/nation_bgMusic/06_Venice.mp3",  //5
	"audio/nation_bgMusic/07_Europe.mp3", 
	"audio/nation_bgMusic/08_Sudan.mp3", 
	"audio/nation_bgMusic/07_Europe.mp3", 
	"audio/nation_bgMusic/07_Europe.mp3",
	"audio/nation_bgMusic/07_Europe.mp3", //10
	"audio/nation_bgMusic/07_Europe.mp3",
	"audio/nation_bgMusic/07_Europe.mp3"
};

//开始的背景音乐
static const char* audio_start[] = {
	"audio/nation_bgMusic/start.mp3",
};
//酒馆背景音乐
static const char* audio_bar[]={
	"audio/bar_bgMusic/07_Europe.mp3", 
	"audio/bar_bgMusic/08_Sudan.mp3"
};
//王宫背景音乐
static const char* audio_palace[]={
	"audio/palace_bgMusic/07_Europe.mp3",
	"audio/palace_bgMusic/08_Sudan.mp3"
};
static const char* audio_sail[]= {
	"audio/sail_bgMusic/01_seiling.mp3",//海上航行时背景音乐
	"audio/sail_bgMusic/02_goBattle.mp3",//海上开始战斗，暂时不用
	"audio/sail_bgMusic/03_battleing.mp3",//战斗时的背景音乐
	"audio/sail_bgMusic/05_afterbattle_bgm.mp3",//战斗胜利后缅怀烈士的背景音乐
	"audio/sail_bgMusic/06_preliminaryBattle.mp3",//海盗预战斗北京音乐
};

static const char* audio_effect[]= {
	"audio/bgEffect/01_button.wav",          //按钮音效
	"audio/bgEffect/02_storm.wav",           //暴风雨音效
	"audio/bgEffect/03_aground.wav",         //触礁音效
	"audio/bgEffect/04_tussle.wav",			 //打斗音效
	"audio/bgEffect/05_mew.wav",			 //海鸥音效(2s)
	"audio/bgEffect/06_mew.wav",			 //海鸥音效(5s)
	"audio/bgEffect/07_fire_cannon.wav",     //发射火炮音效(随机)
	"audio/bgEffect/08_fire_cannon.wav",	 //发射火炮音效(随机)
	"audio/bgEffect/09_fire_cannon.wav",     //发射火炮音效(随机)
	"audio/bgEffect/10_fire_cannon.wav",     //发射火炮音效(随机)
	"audio/bgEffect/11_connon_hit.wav",      //火炮击中音效(随机)
	"audio/bgEffect/12_connon_hit.wav",		 //火炮击中音效(随机)
	"audio/bgEffect/13_connon_hit.wav",		 //火炮击中音效(随机)
	"audio/bgEffect/14_connon_hit.wav",		 //火炮击中音效(随机)
	"audio/bgEffect/16_gold.wav",            //花费V票银币音效
	"audio/bgEffect/17_in_combat.wav",		 //进入战斗音效
	"audio/bgEffect/18_crash.wav",			 //碰撞
	"audio/bgEffect/19_float.wav",           //漂浮物音效
	"audio/bgEffect/20_over_night.wav",		 //时间跨过一个晚上音效
	"audio/bgEffect/21_used_prop.wav",       //使用道具音效（战斗时）
	"audio/bgEffect/22_emergency.wav",       //突发事件音效
	"audio/bgEffect/23_palyer_upgrade.wav",	 //玩家升级音效
	"audio/bgEffect/24_porp_reinforce.wav",  //强化道具音效（强化中使用）
	"audio/bgEffect/25_loof.wav",			 //转舵音效
	"audio/bgEffect/26_loof.wav",			 //转舵音效
	"audio/bgEffect/27_win.mp3",			 //战斗胜利
	"audio/bgEffect/28_fail.mp3",			 //战斗失败
	"audio/bgEffect/29_cannon_hit.wav",		 //炮弹击中效果1
	"audio/bgEffect/30_cannon_hit.wav",		 //炮弹击中效果2
	"audio/bgEffect/31_cannon_hit.wav",		 //炮弹击中效果3
	"audio/bgEffect/32_open_door.wav",		 //酒馆开门音效
	"audio/bgEffect/33_close_door.wav",      //酒馆关门音效
	"audio/bgEffect/34_come_footstep.wav",   //脚步走近音效
	"audio/bgEffect/35_go_footstep.wav",     //脚步走远音效
	"audio/bgEffect/36_judge_catelina.mp3",	 //西班牙审判卡特琳娜音效
	//持续回血-水泡
	"audio/bgEffect/37_bloodreturn.wav",	
	//BUFF类技能-军号
	"audio/bgEffect/38_buff_bugle.wav",	 
	//炮击类技能-引线
	"audio/bgEffect/39_cannon_lead.wav",
	//打捞失败
	"audio/bgEffect/40_salvage_fail.wav",
	//打捞出特殊物品  包含rarity>2 的物品
	"audio/bgEffect/41_salvage_special.wav",
	//打捞成功   打捞出银币,补给，一般物品
	"audio/bgEffect/42_salvage_succ.wav",
};

enum AUDIO_EFFECT
{
	AUDIO_EFFECT_BUTTON_01,					 //按钮音效
	AUDIO_EFFECT_STORM_02,					 //暴风雨音效
	AUDIO_EFFECT_AGROUND_03,				 //触礁音效
	AUDIO_EFFECT_TUSSLE_04,					 //打斗音效
	AUDIO_EFFECT_MEW_05,					 //海鸥音效(2s)
	AUDIO_EFFECT_MEW_06,					 //海鸥音效(5s)
	AUDIO_EFFECT_FIRE_CANNON_07,			 //发射火炮音效(随机)
	AUDIO_EFFECT_FIRE_CANNON_08,			 //发射火炮音效(随机)
	AUDIO_EFFECT_FIRE_CANNON_09,			 //发射火炮音效(随机)
	AUDIO_EFFECT_FIRE_CANNON_10,			 //发射火炮音效(随机)
	AUDIO_EFFECT_CONNON_HIT_11,				 //火炮击中音效(随机)
	AUDIO_EFFECT_CONNON_HIT_12,				 //火炮击中音效(随机)
	AUDIO_EFFECT_CONNON_HIT_13,				 //火炮击中音效(随机)
	AUDIO_EFFECT_CONNON_HIT_14,				 //火炮击中音效(随机)
	AUDIO_EFFECT_GOLD_16,					 //花费V票银币音效
	AUDIO_EFFECT_IN_COMBAT_17,				 //进入战斗音效
	AUDIO_EFFECT_CRASH_18,					 //碰撞
	AUDIO_EFFECT_FLOAT_19,					 //漂浮物音效
	AUDIO_EFFECT_OVER_NIGHT_20,				 //时间跨过一个晚上音效
	AUDIO_EFFECT_USED_PROP_21,				 //使用道具音效（战斗时）
	AUDIO_EFFECT_EMERGENCY_22,				 //突发事件音效
	AUDIO_EFFECT_PALYER_UPGRAD_23,			 //玩家升级音效
	AUDIO_EFFECT_PORP_REINFORCE_24,			 //强化道具音效（强化中使用）
	AUDIO_EFFECT_LOOF_25,					 //转舵音效
	AUDIO_EFFECT_LOOF_26,					 //转舵音效
	AUDIO_EFFECT_BATTLE_WIN_27,				 //战斗胜利
	AUDIO_EFFECT_BATTLE_FAIL_28,			 //战斗失败
	AUDIO_EFFECT_CANNON_HIT_29,				 //炮弹击中效果1
	AUDIO_EFFECT_CANNON_HIT_30,				 //炮弹击中效果2
	AUDIO_EFFECT_CANNON_HIT_31,				 //炮弹击中效果3
	AUDIO_EFFECT_OPEN_DOOR_32,				 //酒馆开门音效
	AUDIO_EFFECT_CLOSE_DOOR_33,				 //酒馆关门音效
	AUDIO_EFFECT_FOOTSTEPS_COME_34,			 //脚步走近音效
	AUDIO_EFFECT_FOOTSTEPS_GO_35,			 //脚步走远音效
	AUDIO_JUDGE_CATE_36,					 //西班牙审判卡特琳娜音效
	//持续回血-水泡
	AUDIO_EFFECT_BLOOD_RETURN,
	//BUFF类技能-军号
	AUDIO_EFFECT_BUFF_BUGLE,
	//炮击类技能-引线
	AUDIO_EFFECT_CONNON_LEAD,
	//打捞失败
	AUDIO_EFFECT_SALVAGE_FAIL_40,
	//打捞出特殊物品  包含rarity>2 的物品
	AUDIO_EFFECT_SALVAGE_SPECIAL_41,
	//打捞成功   打捞出银币,补给，一般物品
	AUDIO_EFFECT_SALVAGE_SUCCEED_42,

};

#endif



