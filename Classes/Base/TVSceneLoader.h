/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月03日
 *  Author:Sen
 *  场景切换管理
 *
 */

#ifndef __SCENE__CHANGE__
#define __SCENE__CHANGE__

#define  CHANGETO(TAG) TVSceneLoader::getInstance()->changeToSceneByTag(TAG)

enum SCENE_TAG
{
	REGISTER_TAG,    //注册
	LOGIN_TAG,	     //登陆
	CREATE_ROLE_TAG, //创建角色
	MAIN_TAG,	     //主城
	EXCHANG_TAG,	 //交易所
	DOCK_TAG,		 //船坞
	PUPL_TAG,		 //酒吧
	PALACE_TAG,		 //王宫
	BANK_TAG,		 //银行
	WHARF_TAG,		 //港口
	CENTER_TAG,	     //个人中心
	EMAIL_TAG,		 //邮箱
	SOCIAL_TAG,	     //好友
	GUILD_TAG,       //公会
	TASK_TAG,		 //任务
	MAP_TAG,		 //海上
	SHOP_TAG,		 //商城
	SET_TAG,		 //设置
	MAINGUID_TAG,    //新手引导下的主城
	PALACEGUID_TAG,  //新手引导下的宫殿
	EXCHANGEGUID_TAG,//新手引导下的商店
	SKILLGUIDE_TAG,//新手引导下的个人中心
	MAP_SAILING_TAG, //已经在海上了，一般是从战斗返回
	WHARF_GUIDE_TAG, //新手引导下的港口
	DOCK_GUIDE_TAG, //新手引导下的船坞
	PUP_GUIDE_TAG, //新手引导下的酒馆
	//国战场景
	COUNTRY_WAR_TAG,
	//进入战斗
	BATTLE_TAG,
};

class TVSceneLoader 
{
public:
	TVSceneLoader();
	~TVSceneLoader();
	
	static TVSceneLoader* getInstance();
	void release();
	void changeToSceneByTag(SCENE_TAG tag);
};

#endif
