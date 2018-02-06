#include "TVGoodsType.h"
#include "TVSingle.h"
#include "xxhash.h"

const char *ship_3d_table[] = {
	"ship/ship_1/Two_Masts_caravel_1.c3b",
	"ship/ship_1/Two_Masts_caravel_1.c3b",
	"ship/ship_1/Two_Masts_caravel_2.c3b",
	"ship/ship_1/Two_Masts_caravel_2.c3b",
	"ship/ship_1/Two_Masts_caravel_2.c3b",
	"ship/ship_1/Two_Masts_caravel_3.c3b",
	"ship/ship_1/Two_Masts_caravel_3.c3b",
	"ship/ship_1/Two_Masts_caravel_3.c3b",

	"ship/ship_2/cog_boat_1.c3b",
	"ship/ship_2/cog_boat_1.c3b",
	"ship/ship_2/cog_boat_2.c3b",
	"ship/ship_2/cog_boat_2.c3b",
	"ship/ship_2/cog_boat_2.c3b",
	"ship/ship_2/cog_boat_3.c3b",
	"ship/ship_2/cog_boat_3.c3b",
	"ship/ship_2/cog_boat_3.c3b",

	"ship/ship_3/Small_Dhow_boat_1.c3b",
	"ship/ship_3/Small_Dhow_boat_1.c3b",
	"ship/ship_3/Small_Dhow_boat_2.c3b",
	"ship/ship_3/Small_Dhow_boat_2.c3b",
	"ship/ship_3/Small_Dhow_boat_2.c3b",
	"ship/ship_3/Small_Dhow_boat_3.c3b",
	"ship/ship_3/Small_Dhow_boat_3.c3b",
	"ship/ship_3/Small_Dhow_boat_3.c3b",

	"ship/ship_4/ThreeMastsCaravel_1.c3b",
	"ship/ship_4/ThreeMastsCaravel_1.c3b",
	"ship/ship_4/ThreeMastsCaravel_2.c3b",
	"ship/ship_4/ThreeMastsCaravel_2.c3b",
	"ship/ship_4/ThreeMastsCaravel_2.c3b",
	"ship/ship_4/ThreeMastsCaravel_3.c3b",
	"ship/ship_4/ThreeMastsCaravel_3.c3b",
	"ship/ship_4/ThreeMastsCaravel_3.c3b",

	"ship/ship_5/Three_Masts_Carrack_boat_1.c3b",
	"ship/ship_5/Three_Masts_Carrack_boat_1.c3b",
	"ship/ship_5/Three_Masts_Carrack_boat_2.c3b",
	"ship/ship_5/Three_Masts_Carrack_boat_2.c3b",
	"ship/ship_5/Three_Masts_Carrack_boat_2.c3b",
	"ship/ship_5/Three_Masts_Carrack_boat_3.c3b",
	"ship/ship_5/Three_Masts_Carrack_boat_3.c3b",
	"ship/ship_5/Three_Masts_Carrack_boat_3.c3b",

	"ship/ship_6/British_Galleon_boat_1.c3b",
	"ship/ship_6/British_Galleon_boat_1.c3b",
	"ship/ship_6/British_Galleon_boat_2.c3b",
	"ship/ship_6/British_Galleon_boat_2.c3b",
	"ship/ship_6/British_Galleon_boat_2.c3b",
	"ship/ship_6/British_Galleon_boat_3.c3b",
	"ship/ship_6/British_Galleon_boat_3.c3b",
	"ship/ship_6/British_Galleon_boat_3.c3b",

	"ship/ship_7/Spanish_Galleon_boat_1.c3b",
	"ship/ship_7/Spanish_Galleon_boat_1.c3b",
	"ship/ship_7/Spanish_Galleon_boat_2.c3b",
	"ship/ship_7/Spanish_Galleon_boat_2.c3b",
	"ship/ship_7/Spanish_Galleon_boat_2.c3b",
	"ship/ship_7/Spanish_Galleon_boat_3.c3b",
	"ship/ship_7/Spanish_Galleon_boat_3.c3b",
	"ship/ship_7/Spanish_Galleon_boat_3.c3b",

	"ship/ship_8/Dhow_boat_1.c3b",
	"ship/ship_8/Dhow_boat_1.c3b",
	"ship/ship_8/Dhow_boat_2.c3b",
	"ship/ship_8/Dhow_boat_2.c3b",
	"ship/ship_8/Dhow_boat_2.c3b",
	"ship/ship_8/Dhow_boat_3.c3b",
	"ship/ship_8/Dhow_boat_3.c3b",
	"ship/ship_8/Dhow_boat_3.c3b",

	"ship/ship_9/Fourmasts_Carrack_boat_1.c3b",
	"ship/ship_9/Fourmasts_Carrack_boat_1.c3b",
	"ship/ship_9/Fourmasts_Carrack_boat_2.c3b",
	"ship/ship_9/Fourmasts_Carrack_boat_2.c3b",
	"ship/ship_9/Fourmasts_Carrack_boat_2.c3b",
	"ship/ship_9/Fourmasts_Carrack_boat_3.c3b",
	"ship/ship_9/Fourmasts_Carrack_boat_3.c3b",
	"ship/ship_9/Fourmasts_Carrack_boat_3.c3b",

	"ship/ship_10/Large_Galleon_boat_1.c3b",
	"ship/ship_10/Large_Galleon_boat_1.c3b",
	"ship/ship_10/Large_Galleon_boat_2.c3b",
	"ship/ship_10/Large_Galleon_boat_2.c3b",
	"ship/ship_10/Large_Galleon_boat_2.c3b",
	"ship/ship_10/Large_Galleon_boat_3.c3b",
	"ship/ship_10/Large_Galleon_boat_3.c3b",
	"ship/ship_10/Large_Galleon_boat_3.c3b",

	"ship/ship_11/extra_boat_1.c3b",
	"ship/ship_11/extra_boat_1.c3b",
	"ship/ship_11/extra_boat_2.c3b",
	"ship/ship_11/extra_boat_2.c3b",
	"ship/ship_11/extra_boat_2.c3b",
	"ship/ship_11/extra_boat_3.c3b",
	"ship/ship_11/extra_boat_3.c3b",
	"ship/ship_11/extra_boat_3.c3b",

	"ship/ship_12/LargeArabianGalley_boat_1.c3b",
	"ship/ship_12/LargeArabianGalley_boat_1.c3b",
	"ship/ship_12/LargeArabianGalley_boat_2.c3b",
	"ship/ship_12/LargeArabianGalley_boat_2.c3b",
	"ship/ship_12/LargeArabianGalley_boat_2.c3b",
	"ship/ship_12/LargeArabianGalley_boat_3.c3b",
	"ship/ship_12/LargeArabianGalley_boat_3.c3b",
	"ship/ship_12/LargeArabianGalley_boat_3.c3b",

	"ship/ship_13/AgileSambukboat_1.c3b",
	"ship/ship_13/AgileSambukboat_1.c3b",
	"ship/ship_13/AgileSambukboat_2.c3b",
	"ship/ship_13/AgileSambukboat_2.c3b",
	"ship/ship_13/AgileSambukboat_2.c3b",
	"ship/ship_13/AgileSambukboat_3.c3b",
	"ship/ship_13/AgileSambukboat_3.c3b",
	"ship/ship_13/AgileSambukboat_3.c3b",

	"ship/ship_14/ArabianGalley_boat_1.c3b",
	"ship/ship_14/ArabianGalley_boat_1.c3b",
	"ship/ship_14/ArabianGalley_boat_2.c3b",
	"ship/ship_14/ArabianGalley_boat_2.c3b",
	"ship/ship_14/ArabianGalley_boat_2.c3b",
	"ship/ship_14/ArabianGalley_boat_3.c3b",
	"ship/ship_14/ArabianGalley_boat_3.c3b",
	"ship/ship_14/ArabianGalley_boat_3.c3b"
};
const std::string rootPath = "Sprite3DTest/";
const std::string ship_3d_test_table[] = 
{
	"twomastscaravel.c3b",
	"cog.c3b",
	"smalldhow.c3b",
	"threemastscaravel.c3b",
	"threemastscarrack.c3b",
	"britishgalleon.c3b",
	"britishgalleon.c3b",
	"dhow.c3b",
	"fourmastscarrack.c3b",
	"largegalleon.c3b",
	"venetiangalley.c3b",
	"arabiangalley.c3b",
	"agilesambuk.c3b",
	"arabiangalley.c3b",
};
std::string  getGoodsName(const int iid)
{
	return SINGLE_SHOP->getGoodsData()[iid].name; 
}

int getGoodsAveragePrice(const int iid)
{
	return SINGLE_SHOP->getGoodsData()[iid].price;
}

std::string getGoodsIconPath(const int iid,const IMAGE_ICON_TYPE type)
{
	std::string path;
	if (type == IMAGE_ICON_OUTLINE)
	{
		path += StringUtils::format("res/goods_128/goods_%d.png",iid);
	}else if (type == IMAGE_ICON_SHADE)
	{
		path += StringUtils::format("res/goods_256/goods_%d.png",iid);
	}else
	{
		path += StringUtils::format("res/goods_128/goods_%d.png",iid);
	}
	return path;
}

std::string getShipName(const int sid)
{
	return SINGLE_SHOP->getShipData()[sid].name; 
}
//Unreasonable_code_14;太多的资源路径，最好配置化

std::string getShipIconPath(const int sid,const IMAGE_ICON_TYPE type)
{
	int icon_id = SINGLE_SHOP->getShipData()[sid].icon_id;
	std::string path;
	if (type == IMAGE_ICON_OUTLINE)
	{
		path += StringUtils::format("res/ships_128/ships_%d.png", icon_id);
	}else if (type == IMAGE_ICON_SHADE)
	{
		path += StringUtils::format("res/ships_256/ships_%d.png", icon_id);
	}else
	{
		path += StringUtils::format("res/ships_128/ships_%d.png", icon_id);
	}
	return path;
}

std::string getShipModePath_3D(const int sid)
{
	int id = sid;
	if (id > 112)
	{
		id = 112;
	}
	return ship_3d_table[id - 1];
}
std::string getShipTestModePath_3D(const int modeid)
{
	return rootPath + ship_3d_test_table[modeid-1];
}

std::string getShipTopIconPath(const int sid)
{
	int model_index = SINGLE_SHOP->getShipData()[sid].model_id;
	std::string path = StringUtils::format("ship/shipTopView/%d.png", model_index);
	return  path;
}

std::string getShipSkewIconPath(const int sid, const int Rotation)
{
	int model_index = SINGLE_SHOP->getShipData()[sid].model_id;
	std::string path = StringUtils::format("ship/shipskewview/%d/d_%d.png", model_index, Rotation);
	return  path;
}

int getBattleShipType(const int sid)
{
	int offset[] = { 1, 1, 2, 2, 2, 3, 3, 3 };
	int model_index = 0;
	if (sid % 8 == 0)
	{
		model_index = sid / 8 * 3;
	}else 
	{
		model_index = sid / 8 * 3 + offset[sid % 8 - 1];
	}
	return model_index;
}

std::string getItemName(const int iid)
{
	/*10000:v 票， 10001：水手币 10002:银币*/
	if (iid == 10000 || iid == 10001 || iid == 10002)
	{
		return SINGLE_SHOP->getSpecialsInfo()[iid].name;
	}
	return SINGLE_SHOP->getItemData()[iid].name;
}

 std::string getItemIconPath(const int iid,const IMAGE_ICON_TYPE type)
{
	std::string path;
	
	/*if (type == IMAGE_ICON_OUTLINE)
	{
	path = StringUtils::format("res/items_128/items_%d.png", iid);
	}else if (type == IMAGE_ICON_SHADE)
	{
	path = StringUtils::format("res/items_256/items_%d.png", iid);
	}else
	{
	path = StringUtils::format("res/items_128/items_%d.png",iid);
	}*/
	
	auto shopData = SINGLE_SHOP->getItemData();
	std::map<int, ITEM_RES>::iterator m1_Iter;
	int pathId = 0;
	if (iid == 10000 || iid == 10001 || iid == 10002 || iid == 10003)
	{
		pathId = iid;
	}
	else
	{
		for (m1_Iter = shopData.begin(); m1_Iter != shopData.end(); m1_Iter++)
		{
			if (m1_Iter->second.id == iid)
			{
				pathId = m1_Iter->second.icon_id;
				break;
			}
		}
	}
	
	if (type == IMAGE_ICON_OUTLINE)
	{
		path = StringUtils::format("res/items_icon_128/items_%d.png", pathId);
	}else if (type == IMAGE_ICON_SHADE)
	{
		path = StringUtils::format("res/items_icon_256/items_%d.png", pathId);
	}else
	{
		path = StringUtils::format("res/items_icon_128/items_%d.png", pathId);
	}
	return path;
}

std::string getCompanionName(const int iid, const bool isCaptain)
{
	std::string name;
	if (isCaptain)
	{
		name = SINGLE_SHOP->getCaptainData()[iid].name;
	}
	else
	{
		name = SINGLE_SHOP->getCompanionInfo()[iid].name;
	}
	return name;
}

std::string getCompanionIconPath(const int iid, const bool isCaptain)
{
	std::string path;
	if (isCaptain)
	{
		if (iid < 101)
		{
			int idMan = iid % 6 + 1;
			return cocos2d::StringUtils::format("res/captains_icon/captainMan%d.png", idMan);
		}
		else
		{
			int idWom = iid % 6 + 1;
			return cocos2d::StringUtils::format("res/captains_icon/captainWom%d.png", idWom);
		}
	}
	else
	{
		path = StringUtils::format("res/companion_icon/companion_%d.png", SINGLE_SHOP->getCompanionInfo()[iid].icon_id);
	}
	return path;
}

std::string getCompanionCardPath(const int iid, const bool isCaptain)
{
	std::string path;
	if (isCaptain)
	{
		if (iid < 101)
		{
			int idMan = iid % 6 + 1;
			path = StringUtils::format("res/companion_card/captain_man_%d.jpg", idMan);
		}
		else
		{
			int idWom = iid % 6 + 1;
			path = StringUtils::format("res/companion_card/captain_wom_%d.jpg", idWom);
		}
	}
	else
	{
		path = StringUtils::format("res/companion_card/companion_%d.jpg", SINGLE_SHOP->getCompanionInfo()[iid].card_id);
	}
	return path;
}

std::string getCompanionRoom(const int shipposition, const int shipId)
{
	auto shipType = SINGLE_SHOP->getShipData().at(shipId).type;
	auto temp_shipPosition = 0;
	switch (shipType)
	{
	case 1:
		temp_shipPosition = shipposition;
		break;
	case 2:
		temp_shipPosition = shipposition + 16;
		break;
	case 3:
		temp_shipPosition = shipposition + 16 + 24;
		break;
	default:
		break;
	}
	auto room_type = SINGLE_SHOP->getShipRoom().at(temp_shipPosition).room_type;
	auto room = SINGLE_SHOP->getJobPositionInfo()[room_type].room;
	return room;
}

std::string getCompanionHeadPath(const int iid)
{
	std::string path;
	path = StringUtils::format("res/companion_head/companion_%d.jpg", SINGLE_SHOP->getCompanionInfo()[iid].card_id);
	return path;
}
std::string getPositionIconPath(const int iid)
{
	return cocos2d::StringUtils::format("res/position_icon/num_%d.png",iid);
}

std::string getMoralIconPath(const int iid)
{
	return cocos2d::StringUtils::format("res/position_icon/moral_%d.png",iid);
}
std::string getPlayerIconPath(const int iid)
{
	return cocos2d::StringUtils::format("res/player_icon/icon_%d.png",iid);
}

std::string getCountryIconPath(const int iid)
{
	 return cocos2d::StringUtils::format("res/country_icon/flag_%d.png",iid);
}

std::string getSkillName(const int iid, const int skill_type)
{
	std::string name = "NOT FOUND NAME";
	switch (skill_type)
	{
	case SKILL_TYPE_NPC:
		name = SINGLE_SHOP->getNPCSkillInfo()[iid].name;
		break;
	case SKILL_TYPE_CAPTAIN:
		name = SINGLE_SHOP->getCaptainSkillInfo()[iid].name;
		break;
	case SKILL_TYPE_PLAYER:
		name = SINGLE_SHOP->getSkillTrees()[iid].name;
		break;
	case SKILL_TYPE_COMPANION_NORMAL:
		name = SINGLE_SHOP->getCompanionNormalSkillInfo()[iid].name;
		break;
	case SKILL_TYPE_PARTNER_SPECIAL:
		name = SINGLE_SHOP->getCompanionSpecialiesSkillInfo()[iid].name;
		break;
	default:
		break;
	}
	return name;
}

std::string getSkillIconPath(const int iid, const int skill_type)
{
	int icon;
	switch (skill_type)
	{
	case SKILL_TYPE_NPC:
		icon = SINGLE_SHOP->getNPCSkillInfo()[iid].icon_id;
		break;
	case SKILL_TYPE_CAPTAIN:
		icon = SINGLE_SHOP->getCaptainSkillInfo()[iid].icon_id;
		break;
	case SKILL_TYPE_PLAYER:
		icon = SINGLE_SHOP->getSkillTrees()[iid].icon_id;
		break;
	case SKILL_TYPE_COMPANION_NORMAL:
		icon = SINGLE_SHOP->getCompanionNormalSkillInfo()[iid].icon_id;
		break;
	case SKILL_TYPE_PARTNER_SPECIAL:
		return cocos2d::StringUtils::format("res/skill_icon/special_skill_%d.png", iid);
		break;
	default:
		break;
	}
	return cocos2d::StringUtils::format("res/skill_icon/skill_%d.png", icon);
}

std::string getGuildIconPath(const int iid)
{
	 return cocos2d::StringUtils::format("res/guild_icon/guild_%d.png",iid);
}

std::string getVticketOrCoinPath(const int iid,const int order)
{
	std::string VticketCoinPath;
	if (iid == 10000)
	{
		 VticketCoinPath= cocos2d::StringUtils::format("res/Vticket_coin/v_%d.png",order);
		 if (order > 7)
		 {
			 VticketCoinPath= cocos2d::StringUtils::format("res/Vticket_coin/v_%d.png",7);
		 }
	}else if (iid == 10001)
	{
		VticketCoinPath =cocos2d::StringUtils::format("res/Vticket_coin/c_%d.png",order);
		if (order > 7)
		{
			VticketCoinPath= cocos2d::StringUtils::format("res/Vticket_coin/c_%d.png",7);
		}
	}
	return VticketCoinPath;
}

std::string getCheifIconPath()
{
	return StringUtils::format("res/npc/chief/npc_%d_%d.png",SINGLE_HERO->m_iNation,SINGLE_HERO->m_iGender%2+1);
}

std::string getNpcIconPath(int type)
{
	return StringUtils::format("res/npc_icon/npc_%d.png", type);
}

std::string getNpcPath(const int city_id,const FIGUR_FALG figure_falg) //this chief_officer not used,use upper getCheifIconPath
{
	std::string path;
	switch (figure_falg)
	{
	case FLAG_BAR_GIRL:
	{
		if (SINGLE_SHOP->getCitiesInfo()[city_id].port_type == 5)
		{
			path = StringUtils::format("cities_resources/villageboss/villageboss_%d.png", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].villageboss_id);
		}
		else
		{
			path = StringUtils::format("cities_resources/bargirl/bargirl_%d.png", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].bargirl_id);
		}
		break;
	}
	case FLAG_CHIEF_OFFICER:
	{
		path = StringUtils::format("cities_resources/chiefofficer/chiefofficer_%d.png", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].chiefofficer_id);
		break;
	}
	case FLAG_SOLDIER:
	{
		path = StringUtils::format("cities_resources/soldier/soldier_%d.png", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].soldier_id);
		break;
	}
	case FLAG_SUPPLY_OFFICER:
	{
		path = StringUtils::format("cities_resources/supplyofficer/supplyofficer_%d.png", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].supplyofficer_id);
		break;
	}
	case FLAG_UNCLE_TRADE:
	{
		path = StringUtils::format("cities_resources/merchant/merchant_%d.png", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].merchant_id);
		break;
	}
	case FLAG_SHIP_BOSS:
	{
		path = StringUtils::format("cities_resources/shipboss/shipboss_%d.png", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].shipboss_id);
		break;
	}
	default:
		break;
	}

	return path;
}

/*------后期的村庄功能实现后需要修改-------*/
std::string getCityAreaBgPath(const int city_id, const CITY_AREA_FALG area_falg)
{
	std::string path;
	switch (area_falg)
	{
	case FLAG_DOCK_AREA:
	{
		path = StringUtils::format("cities_resources/shipyard/shipyard_%d.jpg", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].shipyard_id);
		break;
	}
	case FLAG_PUP_AREA:
	{
		path = StringUtils::format("cities_resources/tavern/tavern_%d.jpg", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].tavern_id);
		break;
	}
	case FLAG_PALACE_AREA:
	{
		//王宫
		path = StringUtils::format("cities_resources/cityhall/cityhall_%d.jpg", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].cityhall_id);
		break;
	}
	case FLAG_WHARF_AREA:
	{
		path = StringUtils::format("cities_resources/dock/dock_%d.jpg", SINGLE_SHOP->getCityAreaResourceInfo()[city_id].dock_id);
		break;
	}
	default:
		break;
	}
	return path;
}

void repalce_all_ditinct(std::string& str,const std::string& old_value,const std::string& new_value)
{
	for(std::string::size_type pos(0);pos!=std::string::npos;pos+=new_value.length())
	{
		if((pos=str.find(old_value,pos))!=std::string::npos)
			str.replace(pos,old_value.length(),new_value);
		else
			break;
	}
}

std::string getTaskType(const int type)
{
	return StringUtils::format("res/shipAttIcon/quest_type_%d.png", type);
}


bool illegal_character_check(std::string& str)
{
	std::string str1 = "<html>";  //非法字符
	for (std::string::size_type pos(0); pos != std::string::npos; pos++)
	{
		if ((pos = str.find(str1, pos)) != std::string::npos)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool illegal_Emoji_check(std::string& str)
{
	int len = str.length();
	for (int i = 0; i < len; i++) 
	{
		char codePoint = str.at(i);
		if ((codePoint == 0x0) ||
			(codePoint == 0x9) ||
			(codePoint == 0xA) ||
			(codePoint == 0xD) ||
			((codePoint >= 0x20) && (codePoint <= 0xD7FF)) ||
			((codePoint >= 0xE000) && (codePoint <= 0xFFFD)) ||
			((codePoint >= 0x10000) && (codePoint <= 0x10FFFF))) 
		{
			return false;
		}
	}
	return true;
}

void TrimLeft(std::string &str)
{
	size_t index;
	index = str.find_first_not_of(" ");
	if (index != std::string::npos)
	{
		str.erase(0, index);
	}
}

void TrimRight(std::string &str)
{
	size_t index;
	index = str.find_last_not_of(" ");
	if (index != std::string::npos)
	{
		str = str.substr(0, index + 1);
	}
}

void TrimAll(std::string &str)
{
	TrimLeft(str);
	TrimRight(str);
}

long my_atoi(const char *str)
{
	long result = 0;
	int signal = 1;
	int step = 10;

	if (!str)
		return 0;
	if (!str[0])
		return 0;
	while (*str && (*str == ' ' || *str == '\t')){
		str++;
	}
	if (!str[0])
		return 0;
	if (*str == '-')
	{
		signal = -1;
		str++;
	}
	else if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X'))
	{
		step = 16;
		str += 2;
	}

	if (step == 10)
	{
		while (*str >= '0'&&*str <= '9')
			result = result * 10 + (*str++ - '0');
	}
	else if (step == 16)
	{
		/*
		while((*str>='0'&&*str<='9') || (*str >= 'a'&&*str <='f') || )
		{
		result = result*10+(*str++ -'0');
		}*/
		while (*str)
		{
			if (*str >= '0'&&*str <= '9')
			{
				result = result * 16 + (*str++ - '0');
			}
			else if (*str >= 'a'&&*str <= 'f')
			{
				result = result * 16 + (*str++ - 'a' + 10);
			}
			else if (*str >= 'A'&&*str <= 'F')
			{
				result = result * 16 + (*str++ - 'A' + 10);
			}
			else{
				break;
			}
		}
	}

	return signal*result;
}

int get_ship_enhance_value_array_from_string(char*p, int(*old1)[7], int(*old2)[7], int(*index1)[2], int(*index2)[2]) {

	for (size_t i = 0; i < 7; i++)
	{
		(*old1)[i] = 0;
		(*old2)[i] = 0;
	}
	for (size_t i = 0; i < 2; i++)
	{
		(*index1)[i] = -1;
		(*index2)[i] = -1;
	}

	char*p_value_string = p;
	if (p_value_string) {
		int p_idx = 0;
		int p_index = 0;
		while (p_idx < 7 && *p_value_string/* && *p_value_string != '-'*/) {
			(*old1)[p_idx] = my_atoi(p_value_string);
			if ((*old1)[p_idx] < 0){
				p_value_string++;
			}
			if ((*old1)[p_idx] != 0){
				(*index1)[p_index] = p_idx;
				p_index++;
			}

			while ((*p_value_string != ',' && *p_value_string != '-') && *p_value_string != 0)
				p_value_string++;
			if (*p_value_string == '-') {
				p_value_string++;
				break;
			}
			if (*p_value_string)
				p_value_string++;

			p_idx++;
		}

		p_idx = 0;
		p_index = 0;
		while (p_idx < 7 && *p_value_string) {
			(*old2)[p_idx] = my_atoi(p_value_string);
			if ((*old2)[p_idx] < 0){
				p_value_string++;
			}
			if ((*old2)[p_idx] != 0){
				(*index2)[p_index] = p_idx;
				p_index++;
			}

			while ((*p_value_string != ',' && *p_value_string != '-') && *p_value_string != 0)
				p_value_string++;

			if (*p_value_string)
				p_value_string++;
			p_idx++;
		}
		return 1;
	}
	return 0;
}

std::string getCharacterModePath(int gender)
{
	//男性角色
	if (gender == 1)
	{
		auto str = "Character3D/man.c3b";
		return str;
	}
	//女性角色
	else if (gender == 2)
	{
		auto str = "Character3D/woman.c3b";
		return str;
	}
	return "";
}
std::string getCarbinTexturePath(int roomType)
{
	std::string path = "cocosstudio/login_ui/cabin_720/";
	switch (roomType)
	{
	case CARBIN_KITCHEN:
		path += "cabin_kitchen.png";
		break;
			//船帆
	case CARBIN_SAIL:
		path += "cabin_operation_sail.png";
		break;
			//甲板
	case CARBIN_DECK:
		path += "cabin_deck.png";
		break;
			//瞭望室
	case CARBIN_WATCHTIWER:
		path += "cabin_outlook.png";
		break;
			//火炮室
	case CARBIN_ARTILLERY:
		path += "cabin_gunpowder.png";
		break;
			//会计室
	case CARBIN_ACCOUNTINHG:
		path += "cabin_accounting.png";
		break;
			//参谋室
	case CARBIN_STAFF:
		path += "cabin_staff_officer.png";
		break;
			//仓库
	case CARBIN_STORE:
		path += "cabin_warehouse.png";
		break;
			//水手室
	case CARBIN_MARINERS:
		path += "cabin_sailor.png";
		break;
			//工作间
	case CARBIN_WORKSHOP:
		path += "cabin_workshop.png";
		break;
			//舰长室
	case CARBIN_CAPTAIN:
		path += "cabin_mercenary.png";
		break;
	default:
		path += "cabin_no.png";
		break;
	}
	return path;
}