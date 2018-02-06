/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月02日
 *  Author:Sen
 *  Goods type (icon and name)
 *
 */

#ifndef __GOODS__TYPE__
#define __GOODS__TYPE__

#include "EnumList.h"
#include "ShopData.h"
//船舱信息
enum CARBININDEX
{
	//厨房
	CARBIN_KITCHEN = 1,
	//船帆
	CARBIN_SAIL,
	//甲板
	CARBIN_DECK,
	//瞭望室
	CARBIN_WATCHTIWER,
	//火炮室
	CARBIN_ARTILLERY,
	//会计室
	CARBIN_ACCOUNTINHG,
	//参谋室
	CARBIN_STAFF,
	//仓库
	CARBIN_STORE,
	//水手室
	CARBIN_MARINERS,
	//工作间
	CARBIN_WORKSHOP,
	//舰长室
	CARBIN_CAPTAIN = 20,
};
std::string getGoodsName(const int iid);		//获取货物的名字
int getGoodsAveragePrice(const int iid);
std::string getGoodsIconPath(const int iid,const IMAGE_ICON_TYPE type = IMAGE_ICON_OUTLINE); //获取货物的图片路径
std::string getShipName(const int sid);      //获取船只的名字
std::string getShipIconPath(const int sid,const IMAGE_ICON_TYPE type = IMAGE_ICON_OUTLINE); //获取船只的图片路径
std::string getShipModePath_3D(const int sid);//获取3D模型
//获取新的3D船只模型路径
std::string getShipTestModePath_3D(const int modeid);
std::string getShipTopIconPath(const int sid); //获取船只的顶视图
/*
获取国战时船只的斜视图
参数 sid:船只的id   Rotation:角度
*/
std::string getShipSkewIconPath(const int sid, const int Rotation);
int getBattleShipType(const int sid);   //船只类型：小中大
std::string getItemName(const int iid);  //获取物品的名字
std::string getItemIconPath(const int iid,const IMAGE_ICON_TYPE type = IMAGE_ICON_OUTLINE); //获取物品的图片路径

std::string getPositionIconPath(const int iid);//获取位置图片
std::string getMoralIconPath(const int iid);	//获取士气的图片
/*
获取小伙伴的名字
参数 iid:小伙伴的id   isCaptain：是否是船长
*/
std::string getCompanionName(const int iid, const bool isCaptain);
/*
获取小伙伴的icon路径
参数 iid:小伙伴的id   isCaptain：是否是船长
*/
std::string getCompanionIconPath(const int iid, const bool isCaptain);
/*
获取小伙伴的card路径
参数 iid:小伙伴的id   isCaptain：是否是船长
*/
std::string getCompanionCardPath(const int iid, const bool isCaptain);

/*
获取小伙伴的工作地点
参数 shipposition:小伙伴的在船上的位置   shipId：船只的id
*/
std::string getCompanionRoom(const int shipposition, const int shipId);

/**
*获取小伙伴头像
*参数 iid:小伙伴的id
*/
std::string getCompanionHeadPath(const int iid);

std::string getPlayerIconPath(const int iid);	//获取玩家的图片路径
std::string getCountryIconPath(const int iid);  //获取国家的图片路径
//获取技能的图片路径
std::string getSkillName(const int iid,const int skill_type);
//获取技能的图片路径
std::string getSkillIconPath(const int iid,const int skill_type);
std::string getGuildIconPath(const int iid);	//获取公会的图片路径
std::string getVticketOrCoinPath(const int iid,const int order); //获取商城图片
std::string getCheifIconPath();  //获取副官的图片路径
/*
*获取npc的icon路径
*参数 type:npc的类型
*/
std::string getNpcIconPath(int type);
/*
*获取对话时的图片路径
*参数 city_id:城市id  figure_falg:npc的类型
*/
std::string getNpcPath(const int city_id,const FIGUR_FALG figure_falg);
std::string getTaskType(const int type);//获得任务类型图片路径
/*获取城市中不同位置的背景*/
std::string getCityAreaBgPath(const int city_id, const CITY_AREA_FALG area_falg);

void repalce_all_ditinct(std::string& str,const std::string& old_value,const std::string& new_value);  //替换字符串
bool illegal_character_check(std::string& str);  //非法字符
// 过滤所有表情
bool illegal_Emoji_check(std::string& str);
//去左边空格 
void TrimLeft(std::string &str);
//去右边空格
void TrimRight(std::string &str);
//去所有空格
void TrimAll(std::string &str);

//强化时解析用到
long my_atoi(const char *str);
int get_ship_enhance_value_array_from_string(char*p, int(*old1)[7], int(*old2)[7], int(*index1)[2], int(*index2)[2]);
//获取人物模型
std::string getCharacterModePath(int gender);
//获取舱室素材路径
std::string getCarbinTexturePath(int roomType);
#endif
