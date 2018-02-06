#include "ShopData.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "cocostudio/DictionaryHelper.h"
#include "cocostudio/TextResUtils.h"
#include "TVSingle.h"
#include "ProtocolThread.h"
#include "Utils.h"
#include "zlib.h"


#if ANDROID
#include <dirent.h>
#include "voyage/GPGSManager.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <unistd.h>
#endif

//#include <openssl/md5.h>
#define LOCAL_JSON_VERSION_CODE "LOCAL_JSON_VERSION_CODE"

#define CONFIG_PARSE_HEAD(json_name) \
	std::string jsonpath;\
	rapidjson::Document root;\
	jsonpath = findJsonFile(json_name);\
	std::size_t found = jsonpath.find_last_of("/\\");\
	std::string jsonFileName = jsonpath.substr(found+1);\
	if(!shouldLoadJson(jsonFileName)){\
		return;\
	}\
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);\
	root.Parse<0>(contentStr.c_str());\
	if(root.HasParseError()){\
		return;\
	}\
	updateConfigJsonMd5(jsonFileName,(unsigned char*)contentStr.c_str(),contentStr.length());

USING_NS_CC;
using namespace cocostudio;

static void delete_json_in_dir(const char*dirPath){
#if ANDROID
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(dirPath)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type != DT_DIR) {
				int len = strlen(ent->d_name);
				char*p = ent->d_name;
				if (len > 4) {
					int i = len - 1;
					while (i > 0) {
						if (*(p + i) == '.') {
							if (strcmp(p + i, ".json") == 0) {
								char file_path[2048];
								strcpy(file_path,dirPath);
								strcat(file_path,ent->d_name);
								log("delete downloaded json file:%s",file_path);
								FileUtils::getInstance()->removeFile(file_path);
							}
							break;
						}
						i--;
					}
					//voyage_log(loop,"read config json:%s\n", ent->d_name);
				}
			}
		}
		closedir(dir);
	}
#else
	FileUtils::getInstance()->removeDirectory(dirPath);
#endif
}

unsigned char* Md5Sum(unsigned char*inBuffer,int inLen,int*outLen)
{
#if 0
	int n;
	MD5_CTX c;
	unsigned char* out = new unsigned char[MD5_DIGEST_LENGTH];

	MD5_Init(&c);
	MD5_Update(&c, inBuffer, inLen);
	MD5_Final(out, &c);
	*outLen = MD5_DIGEST_LENGTH;
	return out;
#else
	unsigned int hash = 5381;
	int i=0;
	while (i<inLen){
		unsigned int c = *(inBuffer+i);
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		i++;
	}
	unsigned char* out = new unsigned char[4];
	memcpy(out,&hash,4);
	*outLen = 4;
	return out;
#endif
}

ShopData::ShopData()
{
	//图片语言（不同语言不同的图片）
	LanguageType nType = LanguageType(Utils::getLanguage());
	switch (nType)
	{
	case cocos2d::LanguageType::CHINESE:
		L_TYPE = 1;
		break;
	case cocos2d::LanguageType::TRADITIONAL_CHINESE:
		L_TYPE = 2;
		break;
	default:
		L_TYPE = 0;
		break;
	}

	Loading_bg = cocos2d::random(1, 2);
	m_RemoteConfigDir = FileUtils::getInstance()->getWritablePath();
	if(m_RemoteConfigDir.at(m_RemoteConfigDir.length()-1) != '/' && m_RemoteConfigDir.at(m_RemoteConfigDir.length()-1) != '\\'){
		m_RemoteConfigDir = m_RemoteConfigDir + "/";
	}
	memset(m_RemoteJsonConfigData,0,MAX_REMOTE_CONFIG_FILE_NUM*sizeof(struct _ConfigJsonMd5*));
	m_CurrentRemoteJsonConfigNum = 0;

#if ANDROID
	//getVersionCode
	JniMethodInfo jniInfo;
	bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","GetVersionCode","()I");
	if(id)
	{
		int m_myClinetVersion = jniInfo.env->CallStaticIntMethod(jniInfo.classID, jniInfo.methodID);
		int jsonVCode = cocos2d::CCUserDefault::getInstance()->getIntegerForKey(LOCAL_JSON_VERSION_CODE,0);
		log("----------------get local json version:%d apk version:%d",jsonVCode,m_myClinetVersion);
		if(jsonVCode > 0 && jsonVCode < m_myClinetVersion){
			log("----------------delete local json files");
			//FileUtils::getInstance()->removeDirectory(m_RemoteConfigDir + "config");
			std::string fPath = m_RemoteConfigDir + "config/";
			delete_json_in_dir(fPath.c_str());
			cocos2d::CCUserDefault::getInstance()->setIntegerForKey(LOCAL_JSON_VERSION_CODE,0);
		}
	}
#else
	std::string fPath = m_RemoteConfigDir + "config/";
	delete_json_in_dir(fPath.c_str());
#endif
}

void ShopData::updateRemoteJson(struct _ConfigJsonResult**json, int num) {
	int createDir = 0;
	if (!cocos2d::CCFileUtils::getInstance()->isDirectoryExist(m_RemoteConfigDir + "config")) {
		createDir = 1;
		cocos2d::CCFileUtils::getInstance()->createDirectory(m_RemoteConfigDir + "config");
	}
	int trigger_num = 0;

	for (int i = 0; i < num; i++) {
		ConfigJsonResult*item = json[i];
		if (item && item->filename) {
			int deleted = 0;
			std::string fullRemotePath = m_RemoteConfigDir + "config/" + item->filename;
			std::string fullResPath = "config/";
			unsigned int uncomprLen = item->fileorgsize;
			unsigned char*uncompr = (unsigned char*)malloc(uncomprLen);
			int err = uncompress(uncompr, (uLongf*)&uncomprLen, item->filecontent.data, item->filecontent.len);
			if(err != Z_OK){
				free(uncompr);
				continue;
			}

			if (!createDir && cocos2d::CCFileUtils::getInstance()->isFileExist(fullResPath+item->filename) && cocos2d::CCFileUtils::getInstance()->isFileExist(fullRemotePath)) {
				//这个分支处理老用户升级了游戏版本，如果服务器和apk里得一样，就可以删除从服务器下载得json
				std::string jsonpath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(fullResPath+item->filename);
				std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
				int len = 0;
				unsigned char*md5 = Md5Sum((unsigned char*) contentStr.c_str(), contentStr.length(), &len);
				if (len == item->md5.len && memcmp(md5, item->md5.data, len) == 0) {
					for (int i = 0; i < m_CurrentRemoteJsonConfigNum; i++) {
						if (m_RemoteJsonConfigData[i]) {
							if (strcmp(m_RemoteJsonConfigData[i]->filename, item->filename) == 0) {
								if (m_RemoteJsonConfigData[i]->md5.data)
									free(m_RemoteJsonConfigData[i]->md5.data);
								m_RemoteJsonConfigData[i]->md5.data = 0;
								m_RemoteJsonConfigData[i]->md5.len = 0;
								trigger_num++;
								break;
							}
						}
					}
					deleted = 1;
					cocos2d::CCFileUtils::getInstance()->removeFile(fullRemotePath);
				}
				if (md5)
					free(md5);
			}
			if (!deleted) {
				//
				FILE*fp = fopen(fullRemotePath.c_str(), "w");
				log("ShopData::updateRemoteJson write remote json %s,fp:%d", fullRemotePath.c_str(), fp);
				if (fp) {
					int w = fwrite(uncompr, 1, uncomprLen, fp);
					fclose(fp);
					if (w == uncomprLen) {

						for (int i = 0; i < m_CurrentRemoteJsonConfigNum; i++) {
							if (m_RemoteJsonConfigData[i]) {
								if (strcmp(m_RemoteJsonConfigData[i]->filename, item->filename) == 0) {
									if (m_RemoteJsonConfigData[i]->md5.data)
										free(m_RemoteJsonConfigData[i]->md5.data);
									m_RemoteJsonConfigData[i]->md5.data = 0;
									m_RemoteJsonConfigData[i]->md5.len = 0;
									log("trigger reload json:%s",item->filename);
									trigger_num++;
									break;
								}
							}
						}
					}
				}

			}
			free(uncompr);
		}
	}

	if(trigger_num > 0){
#if ANDROID
		//getVersionCode
		JniMethodInfo jniInfo;
		bool id = cocos2d::JniHelper::getStaticMethodInfo(jniInfo,"com/piigames/voyage/AppActivity","GetVersionCode","()I");
		if(id)
		{
			int m_myClinetVersion = jniInfo.env->CallStaticIntMethod(jniInfo.classID, jniInfo.methodID);
			cocos2d::CCUserDefault::getInstance()->setIntegerForKey(LOCAL_JSON_VERSION_CODE,m_myClinetVersion);
		}
#endif
		loadShopData();
	}
}

std::string ShopData::getRemoteJson(const std::string &filename){
	if(cocos2d::CCFileUtils::getInstance()->isFileExist(m_RemoteConfigDir+filename)){
		return m_RemoteConfigDir+filename;
	}
	return "";
}

void ShopData::setJsonDirty(std::string &filename){
	if(filename.length() == 0) return ;
	for(int i=0;i<m_CurrentRemoteJsonConfigNum;i++){
		if(m_RemoteJsonConfigData[i]){
			if(strcmp(m_RemoteJsonConfigData[i]->filename,filename.c_str()) == 0){
				if(m_RemoteJsonConfigData[i]->md5.data)free(m_RemoteJsonConfigData[i]->md5.data);
				m_RemoteJsonConfigData[i]->md5.data = 0;
				m_RemoteJsonConfigData[i]->md5.len = 0;
				return;
			}
		}
	}
}
//如果从服务器load到了json，会把本地md5 clear (setJsonDirty)
int ShopData::shouldLoadJson(std::string &filename){
	if(filename.length() == 0) return 0;
	for(int i=0;i<m_CurrentRemoteJsonConfigNum;i++){
		if(m_RemoteJsonConfigData[i]){
			if(strcmp(m_RemoteJsonConfigData[i]->filename,filename.c_str()) == 0){
				if(m_RemoteJsonConfigData[i]->md5.len > 0){
					return 0;
				}else{
					return 1;
				}
			}
		}
	}
	return 1;
}

struct _ConfigJsonMd5**ShopData::getJsonSigure(int*size){
	*size = m_CurrentRemoteJsonConfigNum;
	return m_RemoteJsonConfigData;
}

int ShopData::updateConfigJsonMd5(std::string &filename,unsigned char*buffer,int buffLen){
	int len=0;
	unsigned char*md5 = Md5Sum(buffer,buffLen,&len);
	if(len <= 0 || !md5 || filename.length() == 0) return 0;
	for(int i=0;i<m_CurrentRemoteJsonConfigNum;i++){
		if(m_RemoteJsonConfigData[i]){
			if(strcmp(m_RemoteJsonConfigData[i]->filename,filename.c_str()) == 0){
				if(m_RemoteJsonConfigData[i]->md5.data)free(m_RemoteJsonConfigData[i]->md5.data);
				m_RemoteJsonConfigData[i]->md5.data = md5;
				m_RemoteJsonConfigData[i]->md5.len = len;
				return 1;
			}
		}
	}
	ConfigJsonMd5*config = (ConfigJsonMd5*)malloc(sizeof(ConfigJsonMd5));
	config_json_md5__init(config);
	config->md5.data = md5;
	config->md5.len = len;
	config->filename = (char*)malloc(filename.length() + 1);
	strcpy(config->filename,filename.c_str());
	m_RemoteJsonConfigData[m_CurrentRemoteJsonConfigNum++] = config;
	return 2;
}


ShopData::~ShopData()
{
	m_ShipData.clear();
	m_ItemData.clear();
	m_GoodsData.clear();
	m_CityInfo.clear();
	m_LandData.clear();
	m_TextData.clear();
	m_CaptainData.clear();
	m_FoodAndDrinkInfo.clear();
	m_SpecialInfo.clear();
	m_SideTaskInfo.clear();
	m_MainTaskStoryInfo.clear();
	m_BattleNpcInfo.clear();
	m_DrawingsDesignInfo.clear();
	m_Nation.clear();
	m_DailyReward.clear();
	m_CaptainSkillInfo.clear();
	m_TipsInfo.clear();
	m_TaskDialogInfo.clear();
	m_zoneInfo.clear();
	m_MainTaskInfo.clear();
	m_BagExpandFeeInfo.clear();
	m_bossAwards.clear();
	m_companionInfo.clear();
	m_companionSkillInfo.clear();
	m_jobPositionInfo.clear();
	m_companionSpecialiesSkillInfo.clear();
	m_shipRoomInfo.clear();
	m_MainTaskGuideInfo.clear();
	m_proficiencyInfo.clear();
	m_cityWarPositionInfo.clear();
	m_shipWarPositionInfo.clear();
}

void ShopData::loadShopData()
{
	loadItemData();
	loadGoodsData();
	loadShipData();
	loadSkillTrees();
	//moved to AppDelegate.cpp
	//loadTextData();
	loadCaptainData();
	loadCityInfoData();
	loadFoodADrinkData();
	loadSpecialsData();
	loadSideTaskData();
	loadMainTaskStoryData();
	loadBattleNpcData();
	loadDrawingsDesignData();
	loadNationData();
	loadDailyRewardData();
	loadCoreSideTaskData();
	loadCaptainSkillData();
	loadNPCSkillData();
	loadTipsInfo();
	loadZoneData();
	loadMainTaskData();
	loadBagExpandFeeData();
	loadTriggerDialogData();
    loadTriggerDialogJudgeData();
	loadBossAwards();

	loadPartnerRedBeardDialogData();
	loadPartnerDialogJudgeData();

	loadCompanionData();
	loadCompanionSkillData();
	loadJobPositionData();
	loadSpecialiesSkillData();
	loadShipRoomData();
	loadTextData();
	loadLandData();
	loadMaintaskGuideData();
	loadProficiencyData();
	loadCityWarPositionData();
	loadShipWarPositionData();

	LoadCityAreaResourceData();
	loadSettingLengend();
}

void ShopData::loadShipData()
{
//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(SHIP_PATH_JSON);
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(SHIP_PATH_JSON);
	m_ShipData.clear();
	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts,"type");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
			auto price = DictionaryHelper::getInstance()->getIntValue_json(counts,"price");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts,"desc");
			auto require_level = DictionaryHelper::getInstance()->getIntValue_json(counts,"require_level");
			auto enchant_slots = DictionaryHelper::getInstance()->getIntValue_json(counts,"enchant_slots");
			auto hp_max = DictionaryHelper::getInstance()->getIntValue_json(counts,"hp_max");
			auto supply_max = DictionaryHelper::getInstance()->getIntValue_json(counts,"supply_max");
			auto crew_max = DictionaryHelper::getInstance()->getIntValue_json(counts,"crew_max");
			auto crew_require = DictionaryHelper::getInstance()->getIntValue_json(counts,"crew_require");
			auto defense = DictionaryHelper::getInstance()->getIntValue_json(counts,"defense");
			auto speed = DictionaryHelper::getInstance()->getIntValue_json(counts,"speed");
			auto steer_speed = DictionaryHelper::getInstance()->getIntValue_json(counts,"steer_speed");
			auto bowgun_num = DictionaryHelper::getInstance()->getIntValue_json(counts,"bowgun_num");
			auto rig_num = DictionaryHelper::getInstance()->getIntValue_json(counts,"rig_num");
			auto armor_num = DictionaryHelper::getInstance()->getIntValue_json(counts,"armor_num");
			auto spinnnaker_num = DictionaryHelper::getInstance()->getIntValue_json(counts,"spinnnaker_num");
			auto cannon_num = DictionaryHelper::getInstance()->getIntValue_json(counts,"cannon_num");
			auto bowicon_num = DictionaryHelper::getInstance()->getIntValue_json(counts,"bowicon_num");
			auto cargo_kinds = DictionaryHelper::getInstance()->getIntValue_json(counts,"cargo_kinds");
			auto cargo_size = DictionaryHelper::getInstance()->getIntValue_json(counts,"cargo_size");
			auto rarity = DictionaryHelper::getInstance()->getIntValue_json(counts,"rarity");
			auto icon_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "icon_id");
			auto model_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "model_id");
			auto no_trade = DictionaryHelper::getInstance()->getIntValue_json(counts, "no_trade");
			auto collision_a = DictionaryHelper::getInstance()->getIntValue_json(counts, "collision_a");
			auto collision_b = DictionaryHelper::getInstance()->getIntValue_json(counts, "collision_b");
			auto capture_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "capture_lv");
			auto capture_rate = DictionaryHelper::getInstance()->getIntValue_json(counts, "capture_rate");
			auto room_define = DictionaryHelper::getInstance()->getStringValue_json(counts, "room_define");
			auto nation_limit = DictionaryHelper::getInstance()->getIntValue_json(counts, "nation_limit");
			auto n_poficiency = DictionaryHelper::getInstance()->getIntValue_json(counts, "proficiency_need");
			SHIP_RES ship = { type, name, price, desc, require_level, enchant_slots, hp_max, supply_max, crew_max,
				crew_require, defense, speed, steer_speed, bowgun_num, rig_num, armor_num, spinnnaker_num,
				cannon_num, bowicon_num, cargo_kinds, cargo_size, rarity, icon_id, model_id, no_trade, collision_a,
				collision_b, capture_lv, capture_rate, room_define, nation_limit,n_poficiency};
			m_ShipData.insert(std::pair<int,SHIP_RES>(id,ship));
		}else
		{
			break;
		}
	}
	log("ship num in json:%d",m_ShipData.size());
}


void ShopData::loadItemData()
{
	std::string jsonpath;
	rapidjson::Document root;
	jsonpath = findJsonFile(ITEM_PATH_JSON);
	std::size_t found = jsonpath.find_last_of("/\\");
	std::string jsonFileName = jsonpath.substr(found+1);
	if(!shouldLoadJson(jsonFileName)){
		return;
	}
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
	root.Parse<0>(contentStr.c_str());
	if(root.HasParseError()){
		return;
	}
	updateConfigJsonMd5(jsonFileName,(unsigned char*)contentStr.c_str(),contentStr.length());
	m_ItemData.clear();
	char *defaultStr = "";
	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			auto sub_type = DictionaryHelper::getInstance()->getIntValue_json(counts,"sub_type");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts,"type");
			auto range = DictionaryHelper::getInstance()->getIntValue_json(counts,"property1");
			auto rarity = DictionaryHelper::getInstance()->getIntValue_json(counts,"rarity");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name",defaultStr);
			auto propertys = DictionaryHelper::getInstance()->getStringValue_json(counts,"property2",defaultStr);
			auto price = DictionaryHelper::getInstance()->getIntValue_json(counts,"price");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts,"desc");
			auto weight = DictionaryHelper::getInstance()->getFloatValue_json(counts,"weight");
			auto property3 = DictionaryHelper::getInstance()->getIntValue_json(counts,"property3");
			auto required_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"required_lv");
			auto max_durability = DictionaryHelper::getInstance()->getIntValue_json(counts,"max_durability");
			auto no_trade = DictionaryHelper::getInstance()->getIntValue_json(counts, "no_trade");
			auto property4 = DictionaryHelper::getInstance()->getIntValue_json(counts, "property4");
			auto property5 = DictionaryHelper::getInstance()->getIntValue_json(counts, "property5");
			auto shortdesc = DictionaryHelper::getInstance()->getStringValue_json(counts, "shortdesc", defaultStr);
			auto poficiency = DictionaryHelper::getInstance()->getIntValue_json(counts, "proficiency_need");
			auto icon_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "icon_id");
			ITEM_RES item = { id, type, sub_type, range, rarity, name, propertys, price, desc, weight, property3, required_lv, max_durability, no_trade, property4, property5, shortdesc, poficiency, icon_id };
			m_ItemData.insert(std::pair<int,ITEM_RES>(id,item));
		}else
		{
			break;
		}
	}
}

void ShopData::loadGoodsData()
{
//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(GOODS_PATH_JSON);
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(GOODS_PATH_JSON);
	m_GoodsData.clear();
	int i=0;
	for (i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);

			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts,"type");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
			auto price = DictionaryHelper::getInstance()->getIntValue_json(counts,"price");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts,"desc");
			auto weight = DictionaryHelper::getInstance()->getFloatValue_json(counts,"weight");
			GOODS_RES goods = {type,name,price,weight,desc};
			m_GoodsData.insert(std::pair<int,GOODS_RES>(id,goods));
		}else
		{
			break;
		}

	}
}

void ShopData::loadSkillTrees()
{
//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(SKILL_PATH_JSON);
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(SKILL_PATH_JSON);
	m_SkillTrees.clear();
	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			auto cdreduce_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"cd_reduce_per_level");
		    auto efffect_skill_id = DictionaryHelper::getInstance()->getIntValue_json(counts,"efffect_skill_id");
			auto skill_layer = DictionaryHelper::getInstance()->getIntValue_json(counts,"skill_layer");
			auto base_rate = DictionaryHelper::getInstance()->getIntValue_json(counts,"base_rate");
			auto points_per_level = DictionaryHelper::getInstance()->getIntValue_json(counts,"points_per_level");
			auto cd = DictionaryHelper::getInstance()->getIntValue_json(counts,"cd");
			auto base_skill_require_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"base_skill_require_lv");
			auto rate_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"rate_per_lv");
			auto ap_type = DictionaryHelper::getInstance()->getIntValue_json(counts,"ap_type");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts,"type");
			auto sub_type = DictionaryHelper::getInstance()->getIntValue_json(counts,"sub_type");
			auto base_effect = DictionaryHelper::getInstance()->getIntValue_json(counts,"base_effect");
			auto effect_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"effect_per_lv");
			auto base_skill_id = DictionaryHelper::getInstance()->getIntValue_json(counts,"base_skill_id");
			auto base_duration = DictionaryHelper::getInstance()->getIntValue_json(counts,"base_duration");
			auto max_level = DictionaryHelper::getInstance()->getIntValue_json(counts,"max_level");
			auto duration_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"duration_per_lv");
			auto require_level = DictionaryHelper::getInstance()->getIntValue_json(counts,"require_level");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts,"descr");
			auto next_desc = DictionaryHelper::getInstance()->getStringValue_json(counts,"next_desc");
			auto icon_id = id;
			auto cur_skill_level = 0;
			auto max_level_special = 0;
			auto room_type = 0;
			auto effect_type = 0;
			SKILL_TREE skill = {cdreduce_per_lv,efffect_skill_id,skill_layer,base_rate,points_per_level,cd,
				base_skill_require_lv,rate_per_lv,ap_type,type,sub_type,base_effect,effect_per_lv,base_skill_id,
				base_duration,max_level,duration_per_lv,require_level,cur_skill_level,name,desc,next_desc,icon_id,
				max_level_special, room_type, effect_type };
			m_SkillTrees.insert(std::pair<int,SKILL_TREE>(id,skill));
		}else
		{
			break;
		}
	}
}

void ShopData::loadTextData()
{
	std::string jsonpath;
	rapidjson::Document root;
	jsonpath = findJsonFile(TEXT_PATH_JSON);
	std::size_t found = jsonpath.find_last_of("/\\");
	std::string jsonFileName = jsonpath.substr(found+1);
	if(!shouldLoadJson(jsonFileName)){
		return;
	}
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
	root.Parse<0>(contentStr.c_str());
	if(root.HasParseError()){
		return;
	}
	updateConfigJsonMd5(jsonFileName,(unsigned char*)contentStr.c_str(),contentStr.length());

	TextResUtils::getInstance()->loadData(jsonpath);
}

void ShopData::loadCaptainData()
{
//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(CAPTAIN_PATH_JSON);
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(CAPTAIN_PATH_JSON);
	m_CaptainData.clear();
	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			auto gender = DictionaryHelper::getInstance()->getStringValue_json(counts,"gender");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");

			CAPTAIN_RES captain = {id,gender,name};
			m_CaptainData.insert(std::pair<int,CAPTAIN_RES>(id,captain));
		}else
		{
			break;
		}
	}
}

void ShopData::loadLandData()
{
	CONFIG_PARSE_HEAD(LAND_PATH_JSON)
	m_LandData.clear();
//	if (m_LandData.size() > 4)
//	{
//		return;
//	}
//	m_LandData.clear();
//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(LAND_PATH_JSON);
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			int id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			int left_down_x = DictionaryHelper::getInstance()->getIntValue_json(counts, "left_down_x");
			int left_down_y = DictionaryHelper::getInstance()->getIntValue_json(counts, "left_down_y");
			int right_up_x = DictionaryHelper::getInstance()->getIntValue_json(counts, "right_up_x");
			int right_up_y = DictionaryHelper::getInstance()->getIntValue_json(counts, "right_up_y");
			std::string name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");

			LANDINfO captain = { left_down_x, left_down_y, right_up_x, right_up_y,0, 0, name };
			m_LandData.insert(std::pair<int,LANDINfO>(id,captain));
		}else
		{
			break;
		}
	}
}

void ShopData::loadCityInfoData()
{
//	if (m_CityInfo.size() > 0)
//	{
//		return;
//	}
//
//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(CITY_PATH_JSON);
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(CITY_PATH_JSON);
	m_CityInfo.clear();
	int i=0;
	for (i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			int id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			int x = DictionaryHelper::getInstance()->getIntValue_json(counts,"x");
			int y = DictionaryHelper::getInstance()->getIntValue_json(counts,"y");
			int dock_x = DictionaryHelper::getInstance()->getIntValue_json(counts,"dock_x");
			int dock_y = DictionaryHelper::getInstance()->getIntValue_json(counts,"dock_y");
			int leave_x = DictionaryHelper::getInstance()->getIntValue_json(counts,"leave_x");
			int leave_y = DictionaryHelper::getInstance()->getIntValue_json(counts,"leave_y");
			std::string name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
			int nation = DictionaryHelper::getInstance()->getIntValue_json(counts,"nation");
			std::string bargirl = DictionaryHelper::getInstance()->getStringValue_json(counts,"bargirl");
			int palace_type = DictionaryHelper::getInstance()->getIntValue_json(counts,"palace_type");
			int port_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "type");
			CITYINFO captain = { x, y, id, dock_x, dock_y, leave_x, leave_y, name, nation, bargirl, palace_type, port_type };
			m_CityInfo.insert(std::pair<int,CITYINFO>(id,captain));
		}else
		{
			break;
		}
	}
//	log("loadData: city %d from %s", i, jsonpath.c_str());
}

void ShopData::loadTipsInfo()
{
//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(TIPSINFO_JSON);
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(TIPSINFO_JSON);
	m_TipsInfo.clear();
	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto tip = DictionaryHelper::getInstance()->getStringValue_json(counts,"tip");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
			m_TipsInfo.insert(std::pair<std::string,std::string>(name,tip));
		}else
		{
			break;
		}
	}
}

void ShopData::loadFoodADrinkData()
{
//	std::string jsonpath = findJsonFile(FOOD_DRINK_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(FOOD_DRINK_JSON);
	m_FoodAndDrinkInfo.clear();
	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			int id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			std::string name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
			m_FoodAndDrinkInfo.insert(std::pair<int,std::string>(id,name));
		}else
		{
			break;
		}
	}
}

void ShopData::loadSpecialsData()
{
//	std::string jsonpath = findJsonFile(SPECIALS_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(SPECIALS_PATH_JSON);
	m_SpecialInfo.clear();

	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts,"type");
			auto price = DictionaryHelper::getInstance()->getIntValue_json(counts,"price");
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts,"desc");
			SPECIALSINFO specials = {type,price,name,desc};
			m_SpecialInfo.insert(std::pair<int,SPECIALSINFO>(id,specials));
		}else
		{
			break;
		}
	}
}

MAINTASKSTORYINFO& ShopData::getMainTaskStoryByNationAndChapter(int nation, int chapter)
{
	auto it = m_MainTaskStoryInfo.end();
	for (auto i = m_MainTaskStoryInfo.begin(); i != m_MainTaskStoryInfo.end(); i++)
	{
		auto item = *i;
		if (item->nation == nation && chapter == item->chapter_idx){
			//log("title = %s desc = %s", item->title.c_str(), item->desc.c_str());
			return *item;
		}
	}
	return **it;
}

SIDECORETASKINFO& ShopData::getCoreTaskById(int coreTaskId){
	std::vector<SIDECORETASKINFO*>::iterator it = m_CoreTaskInfo.end();
	for (auto i = m_CoreTaskInfo.begin(); i != m_CoreTaskInfo.end(); i++)
	{
		auto item = *i;
		if(item->task_core_id == coreTaskId){
			return *item;
		}
	}
	return **it;
}
////SINGLE_HERO
//SIDECORETASKINFO& ShopData::getCoreTaskByLevelAndType(int level,int type){
//	std::vector<SIDECORETASKINFO*>::iterator it = m_CoreTaskInfo.end();
//	for (auto i = m_CoreTaskInfo.begin(); i != m_CoreTaskInfo.end(); i++)
//	{
//		auto item = *i;
//		if(item->level == level && item->type == type){
//			return *item;
//		}
//	}
//	return **it;
//}
//
//SIDECORETASKINFO& ShopData::getCoreTaskByTaskInfo(SIDETASKINFO&taskInfo){
//	std::vector<SIDECORETASKINFO*>::iterator it = m_CoreTaskInfo.end();
//	int level = EXP_NUM_TO_LEVEL(SINGLE_HERO->m_iTotalExp);
//	int type = taskInfo.type;
//	for (auto i = m_CoreTaskInfo.begin(); i != m_CoreTaskInfo.end(); i++)
//	{
//		auto item = *i;
//		if(item->level == level && item->type == type){
//			return *item;
//		}
//	}
//	return **it;
//}

static int *get_int_array_from_string(char*p,char s_char,int*size){
	if(p&&p[0]){
		int len = 100;
		int*ids = new int[len];
		int i=0;
		while(*p){
			long value = atoi(p);

			ids[i++] = value;
			while(*p != s_char && *p != 0) p++;
			if(!*p) break;
			p++;
		}
		if(i == 0){
			if(size) *size = 0;
			delete[] ids;
			return 0;
		}
		if(size) *size = i;
		return ids;
	}
	if(size) *size = 0;
	return 0;
}

void ShopData::loadCoreSideTaskData()
{
//
//	std::string jsonpath = findJsonFile(CORESIDETASK_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(CORESIDETASK_PATH_JSON);
	m_CoreTaskInfo.clear();

	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts,"type");
			auto level = DictionaryHelper::getInstance()->getIntValue_json(counts,"level");
			auto time = DictionaryHelper::getInstance()->getIntValue_json(counts,"task_time");

			auto rewards_coin  = 0;
			auto rewards_gold  = 0;
			auto rewards_item1_id = 0;
			auto rewards_item1_type = 0;
			auto rewards_item1_amount = 0;
			auto rewards_item2_id = 0;
			auto rewards_item2_type = 0;
			auto rewards_item2_amount = 0;
			auto rewards_exp = 0;
			auto rewards_reputation = 0;
			auto rewards_force_id = 0;
			auto rewards_force_affect = 0;
			int* finish_target_npcIds = 0;
			int npc_id_count = 0;
			int*zone_ids = 0;

			auto finish_number = 0;
			int* finish_goods_id = 0;
			int* finish_goods_amount = 0;
			int* finish_target_cityId = 0;
			int goods_id_num = 0;
			int bonus_v_ticket = 0;

			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts,"rewards_json"))
			{
				auto reward_counts = DictionaryHelper::getInstance()->getStringValue_json(counts,"rewards_json");
				rapidjson::Document root_rewards;
				root_rewards.Parse<0>(reward_counts);
				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"coin"))
				{
					rewards_coin = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"coin");
				}
				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"gold"))
				{
					rewards_gold = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"gold");
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"items"))
				{
					auto count = DictionaryHelper::getInstance()->getArrayCount_json(root_rewards,"items");
					for (int i = 0; i < count;i++)
					{
						auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root_rewards,"items",i);
						if (i == 0)
						{
							rewards_item1_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"item_id");
							rewards_item1_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"item_type");
							rewards_item1_amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"amount");
						}else
						{
							rewards_item2_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"item_id");
							rewards_item2_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"item_type");
							rewards_item2_amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"amount");
						}
					}
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"exp"))
				{
					rewards_exp = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"exp");
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"reputation"))
				{
					rewards_reputation = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"reputation");
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"reputaion"))
				{
					rewards_reputation = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"reputaion");
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"force_id"))
				{
					rewards_force_id = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"force_id");
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"force_affect"))
				{
					rewards_force_affect = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"force_affect");
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"bonus"))
				{
					bonus_v_ticket = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"bonus");
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(counts,"finish_json"))
				{
					auto finish_counts = DictionaryHelper::getInstance()->getStringValue_json(counts,"finish_json");
					rapidjson::Document root_finish;
					root_finish.Parse<0>(finish_counts);
					if (DictionaryHelper::getInstance()->checkObjectExist_json(root_finish,"target_npc"))
					{
						const char*npcIds = DictionaryHelper::getInstance()->getStringValue_json(root_finish,"target_npc");
						finish_target_npcIds = get_int_array_from_string((char*)npcIds,',',&npc_id_count);
					}
					if(DictionaryHelper::getInstance()->checkObjectExist_json(root_finish,"zone"))
					{
						const char*zone = DictionaryHelper::getInstance()->getStringValue_json(root_finish,"zone");
						zone_ids = get_int_array_from_string((char*)zone,',',&npc_id_count);
					}
					if (DictionaryHelper::getInstance()->checkObjectExist_json(root_finish, "number"))
					{
						/*
						const char* number_str = DictionaryHelper::getInstance()->getStringValue_json(root_finish,"number");
						finish_number = atoi(number_str);
						*/
						finish_number = DictionaryHelper::getInstance()->getIntValue_json(root_finish, "number");
					}
					if (DictionaryHelper::getInstance()->checkObjectExist_json(root_finish,"goods_id"))
					{
						auto str = DictionaryHelper::getInstance()->getStringValue_json(root_finish,"goods_id");
						finish_goods_id = get_int_array_from_string((char*)str,',',&goods_id_num);
					}
					if (DictionaryHelper::getInstance()->checkObjectExist_json(root_finish,"goods_amount"))
					{
						auto str = DictionaryHelper::getInstance()->getStringValue_json(root_finish,"goods_amount");
						finish_goods_amount = get_int_array_from_string((char*)str,',',&goods_id_num);
					}
					if (DictionaryHelper::getInstance()->checkObjectExist_json(root_finish,"target_city"))
					{
						auto str = DictionaryHelper::getInstance()->getStringValue_json(root_finish,"target_city");
						finish_target_cityId = get_int_array_from_string((char*)str,',',&goods_id_num);
					}
				}
			}
			SIDECORETASKINFO sideTask1 = {id,type,level,time,rewards_coin,rewards_gold,rewards_item1_id,rewards_item1_type,
									rewards_item1_amount,rewards_item2_id,rewards_item2_type,rewards_item2_amount,
									rewards_exp,rewards_reputation,rewards_force_id,rewards_force_affect,finish_target_npcIds,zone_ids,npc_id_count,
									finish_number,finish_goods_id,finish_goods_amount,finish_target_cityId,goods_id_num, bonus_v_ticket} ;
			SIDECORETASKINFO* sideTask = new SIDECORETASKINFO;
			memcpy(sideTask,&sideTask1,sizeof(SIDECORETASKINFO));
			m_CoreTaskInfo.push_back(sideTask);
			//m_CoreSideTaskInfo.insert(std::pair<int,SIDETASKINFO>(id,sideTask));
		}else
		{
			break;
		}
	}
}

void ShopData::loadSideTaskData()
{
//	std::string jsonpath = findJsonFile(SIDETASK_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(SIDETASK_PATH_JSON);
	m_SideTaskInfo.clear();
	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id"); 
			auto title = DictionaryHelper::getInstance()->getStringValue_json(counts,"title");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts,"descr");
			auto level = DictionaryHelper::getInstance()->getIntValue_json(counts,"level");
			auto time = DictionaryHelper::getInstance()->getIntValue_json(counts,"task_time");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts,"type");

			auto rewards_coin  = 0;
			auto rewards_gold  = 0;
			auto rewards_item1_id = 0;
			auto rewards_item1_type = 0;
			auto rewards_item1_amount = 0;
			auto rewards_item2_id = 0;
			auto rewards_item2_type = 0;
			auto rewards_item2_amount = 0;
			auto rewards_exp = 0;
			auto rewards_reputation = 0;
			auto rewards_force_id = 0;
			auto rewards_force_affect = 0;
			auto finish_target_npcId = 0;
			auto finish_number = 0;
			auto finish_goods_id = 0;
			auto finish_goods_amount = 0;
			auto finish_target_cityId = 0;
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts,"rewards_json"))
			{
				auto reward_counts = DictionaryHelper::getInstance()->getStringValue_json(counts,"rewards_json");		
				rapidjson::Document root_rewards;
				root_rewards.Parse<0>(reward_counts);
				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"coin"))
				{
					rewards_coin = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"coin");
				}
				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"gold"))
				{
					rewards_gold = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"gold");
				}
				
				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"items"))
				{
					auto count = DictionaryHelper::getInstance()->getArrayCount_json(root_rewards,"items");
					for (int i = 0; i < count;i++)
					{
						auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root_rewards,"items",i);
						if (i == 0)
						{
							rewards_item1_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"item_id");
							rewards_item1_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"item_type");
							rewards_item1_amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"amount");
						}else
						{
							rewards_item2_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"item_id");
							rewards_item2_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"item_type");
							rewards_item2_amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts,"amount");
						}
					}
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"exp"))
				{
					rewards_exp = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"exp");
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"reputation"))
				{
					rewards_reputation = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"reputation");
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"force_id"))
				{
					rewards_force_id = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"force_id");
				}

				if (DictionaryHelper::getInstance()->checkObjectExist_json(root_rewards,"force_affect"))
				{
					rewards_force_affect = DictionaryHelper::getInstance()->getIntValue_json(root_rewards,"force_affect");
				}
			
				if (DictionaryHelper::getInstance()->checkObjectExist_json(counts,"finish_json"))
				{
					auto finish_counts = DictionaryHelper::getInstance()->getStringValue_json(counts,"finish_json");		
					rapidjson::Document root_finish;
					root_finish.Parse<0>(finish_counts);
					if (DictionaryHelper::getInstance()->checkObjectExist_json(root_finish,"target_npc"))
					{
						finish_target_npcId = DictionaryHelper::getInstance()->getIntValue_json(root_finish,"target_npc");
					}
					if (DictionaryHelper::getInstance()->checkObjectExist_json(root_finish,"number"))
					{
						finish_number = DictionaryHelper::getInstance()->getIntValue_json(root_finish,"number");
					}
					if (DictionaryHelper::getInstance()->checkObjectExist_json(root_finish,"goods_id"))
					{
						finish_goods_id = DictionaryHelper::getInstance()->getIntValue_json(root_finish,"goods_id");
					}
					if (DictionaryHelper::getInstance()->checkObjectExist_json(root_finish,"goods_amount"))
					{
						finish_goods_amount = DictionaryHelper::getInstance()->getIntValue_json(root_finish,"goods_amount");
					}
					if (DictionaryHelper::getInstance()->checkObjectExist_json(root_finish,"target_city"))
					{
						finish_target_cityId = DictionaryHelper::getInstance()->getIntValue_json(root_finish,"target_city");
					}
				}
			}
			SIDETASKINFO sideTask = {title,desc,level,time,type} ;
			m_SideTaskInfo.insert(std::pair<int,SIDETASKINFO>(id,sideTask));
		}else
		{
			break;
		}
	}
}

void ShopData::loadMainTaskStoryData()
{
//	std::string jsonpath = findJsonFile(MAINTASK_STORY_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(MAINTASK_STORY_PATH_JSON);
	m_MainTaskStoryInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			auto title = DictionaryHelper::getInstance()->getStringValue_json(counts, "title");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts, "desc");

			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto nation = DictionaryHelper::getInstance()->getIntValue_json(counts, "nation");
			auto chapter_idx = DictionaryHelper::getInstance()->getIntValue_json(counts, "chapter_idx");
			//log("name = %s title = %s",name,title);
			//MAINTASKSTORYINFO mainstory1 = { name, title, desc, id, nation, chapter_idx };
			MAINTASKSTORYINFO* mainstory = new MAINTASKSTORYINFO;
			mainstory->name = name;
			mainstory->title = title;
			mainstory->desc = desc;
			mainstory->id = id;
			mainstory->nation = nation;
			mainstory->chapter_idx = chapter_idx;
			//memcpy(mainstory, &mainstory1, sizeof(MAINTASKSTORYINFO));
			m_MainTaskStoryInfo.push_back(mainstory);
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadBattleNpcData()
{
//	std::string jsonpath = findJsonFile(BATTLENPC_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(BATTLENPC_PATH_JSON);
	m_BattleNpcInfo.clear();

	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts,"type");
			auto force_id=DictionaryHelper::getInstance()->getIntValue_json(counts,"force_id");
			BATTLENPCINFO npc = {name,type,force_id};
			m_BattleNpcInfo.insert(std::pair<int,BATTLENPCINFO>(id,npc));
		}else
		{
			break;
		}
	}
}

void ShopData::loadDrawingsDesignData()
{
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(DRAWINGSDESIGN_PATH_JSON);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(DRAWINGSDESIGN_PATH_JSON)
	m_DrawingsDesignInfo.clear();
	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			std::vector<DRAWINGMATERIALINFO> items;
			auto iid = DictionaryHelper::getInstance()->getIntValue_json(counts,"iid");
			auto required_time = DictionaryHelper::getInstance()->getIntValue_json(counts,"required_time");
			auto manufacture_req = DictionaryHelper::getInstance()->getIntValue_json(counts,"manufacture_req");
			auto coin = DictionaryHelper::getInstance()->getIntValue_json(counts,"coin");
			auto required_item1 = DictionaryHelper::getInstance()->getIntValue_json(counts,"required_item1");
			auto required_item1_num = DictionaryHelper::getInstance()->getIntValue_json(counts,"required_item1_num");
			DRAWINGMATERIALINFO itemMaterual_1;
			itemMaterual_1.requried_item = required_item1;
			itemMaterual_1.requried_item_num = required_item1_num;
			items.push_back(itemMaterual_1);
			auto required_item2 = DictionaryHelper::getInstance()->getIntValue_json(counts, "required_item2");
			auto irequried_item2_num = DictionaryHelper::getInstance()->getIntValue_json(counts, "requried_item2_num");
			DRAWINGMATERIALINFO itemMaterual_2;
			itemMaterual_2.requried_item = required_item2;
			itemMaterual_2.requried_item_num = irequried_item2_num;
			items.push_back(itemMaterual_2);
			auto required_item3 = DictionaryHelper::getInstance()->getIntValue_json(counts, "required_item3");
			auto irequried_item3_num = DictionaryHelper::getInstance()->getIntValue_json(counts, "requried_item3_num");
			DRAWINGMATERIALINFO itemMaterual_3;
			itemMaterual_3.requried_item = required_item3;
			itemMaterual_3.requried_item_num = irequried_item3_num;
			items.push_back(itemMaterual_3);
			auto required_item4 = DictionaryHelper::getInstance()->getIntValue_json(counts, "required_item4");
			auto irequried_item4_num = DictionaryHelper::getInstance()->getIntValue_json(counts, "required_item4_num");
			DRAWINGMATERIALINFO itemMaterual_4;
			itemMaterual_4.requried_item = required_item4;
			itemMaterual_4.requried_item_num = irequried_item4_num;
			items.push_back(itemMaterual_4);
			auto required_item5 = DictionaryHelper::getInstance()->getIntValue_json(counts, "required_item5");
			auto irequried_item5_num = DictionaryHelper::getInstance()->getIntValue_json(counts, "required_item5_num");
			DRAWINGMATERIALINFO itemMaterual_5;
			itemMaterual_5.requried_item = required_item5;
			itemMaterual_5.requried_item_num = irequried_item5_num;
			items.push_back(itemMaterual_5);
			auto city_id_list = DictionaryHelper::getInstance()->getStringValue_json(counts,"city_id_list");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts,"type");
			auto define_id = DictionaryHelper::getInstance()->getIntValue_json(counts,"define_id");
			DRAWINGSDESIGNINFO drawings = {required_time,manufacture_req,coin,city_id_list,
				required_item1,required_item1_num,required_item2,irequried_item2_num,type,define_id,items};
			m_DrawingsDesignInfo.insert(std::pair<int,DRAWINGSDESIGNINFO>(iid,drawings));
		}else
		{
			break;
		}
	}
}

void ShopData::loadNationData()
{
//	std::string jsonpath = findJsonFile(NATION_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(NATION_PATH_JSON);
	m_Nation.clear();
	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
			m_Nation.insert(std::pair<int,std::string>(id,name));
		}else
		{
			break;
		}
	}
}
void ShopData::loadDailyRewardData()
{
//	std::string jsonpath=findJsonFile(DAILYREWARD_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(DAILYREWARD_PATH_JSON);
	m_DailyReward.clear();

	for (int i=0; ;i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto &counts=DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto date=DictionaryHelper::getInstance()->getIntValue_json(counts,"date");
			auto items_id=DictionaryHelper::getInstance()->getIntValue_json(counts,"items_id");
			auto items_num=DictionaryHelper::getInstance()->getIntValue_json(counts,"items_num");
			auto ships_id=DictionaryHelper::getInstance()->getStringValue_json(counts,"ships_id");
			auto ships_num=DictionaryHelper::getInstance()->getIntValue_json(counts,"ships_num");
			auto coins=DictionaryHelper::getInstance()->getIntValue_json(counts,"coins");
			auto v_tickets=DictionaryHelper::getInstance()->getIntValue_json(counts,"v_tickets");
			DAILYREWARDINFO dailyReward={date,items_id,items_num,ships_id,ships_num,coins,v_tickets};
			m_DailyReward.insert(std::pair<int,DAILYREWARDINFO>(date,dailyReward));
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadNPCSkillData() {
//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(NPCSKILL_PATH_JSON);
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(NPCSKILL_PATH_JSON);
	m_NPCSkillInfo.clear();

	for (int i = 0;; i++) {
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i)) {
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto cdreduce_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "cdreduce_per_lv");
			auto efffect_skill_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "efffect_skill_id");
			auto skill_layer = DictionaryHelper::getInstance()->getIntValue_json(counts, "skill_layer");
			auto base_rate = DictionaryHelper::getInstance()->getIntValue_json(counts, "base_rate");
			auto points_per_level = DictionaryHelper::getInstance()->getIntValue_json(counts, "points_per_lv");
			auto cd = DictionaryHelper::getInstance()->getIntValue_json(counts, "cd");
			auto base_skill_require_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "base_skill_require_lv");
			auto rate_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "rate_per_lv");
			auto ap_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "ap_type");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts, "type");
			auto sub_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "sub_type");
			auto base_effect = DictionaryHelper::getInstance()->getIntValue_json(counts, "base_effect");
			auto effect_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "effect_per_lv");
			auto base_skill_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "base_skill_id");
			auto base_duration = DictionaryHelper::getInstance()->getIntValue_json(counts, "base_duration");
			auto max_level = DictionaryHelper::getInstance()->getIntValue_json(counts, "max_level");
			auto duration_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "duration_per_lv");
			auto require_level = DictionaryHelper::getInstance()->getIntValue_json(counts, "require_level");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			
			auto icon_id = id;
			auto cur_skill_level = 0;
			auto max_level_special = 0;
			auto room_type = 0;
			auto effect_type = 0;
			SKILL_TREE skill = { cdreduce_per_lv, efffect_skill_id, skill_layer, base_rate, points_per_level, cd, base_skill_require_lv, rate_per_lv, ap_type, type, sub_type, base_effect,
				effect_per_lv, base_skill_id, base_duration, max_level, duration_per_lv, require_level, cur_skill_level, name, "", "", icon_id, max_level_special, room_type, effect_type };
			m_NPCSkillInfo.insert(std::pair<int, SKILL_TREE>(id, skill));
		} else {
			break;
		}
	}
}

void ShopData::loadGuideTaskDialogData()
{
	std::string st_path = StringUtils::format("%s_%d_%d", GUIDETASKDIALOG_PATH_JSON.data(), SINGLE_HERO->m_iNation, SINGLE_HERO->m_iGender);
	std::string jsonpath = findJsonFile(st_path);
	rapidjson::Document root;
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
	root.Parse<0>(contentStr.c_str());
	m_GuideTaskDialogInfo.clear();
	int count = DictionaryHelper::getInstance()->getArrayCount_json(root, "guide_dialog");
	for (int i = 0; i < count; i++)
	{
		auto &counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root, "guide_dialog", i);
		auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
		auto dialog = DictionaryHelper::getInstance()->getStringValue_json(counts, "dialog");

		GUIDETASKDIALOGINFO taskInfo = { name, dialog };
		m_GuideTaskDialogInfo.insert(std::pair<int, GUIDETASKDIALOGINFO>(i, taskInfo));
	}
}

void ShopData::loadTaskDialogData()
{
	std::string st_path = StringUtils::format("%s_%d_%d", TASKDIALOG_PATH_JSON.data(), SINGLE_HERO->m_iNation, SINGLE_HERO->m_iGender);
	std::string jsonpath = findJsonFile(st_path);
	rapidjson::Document root;
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
	root.Parse<0>(contentStr.c_str());
	m_TaskDialogInfo.clear();
	int count = DictionaryHelper::getInstance()->getArrayCount_json(root,"task_dialog");
	for (int i = 0; i < count;i++)
	{
		auto &counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root,"task_dialog",i);
		auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
		auto dialog = DictionaryHelper::getInstance()->getStringValue_json(counts,"dialog");

		TASKDIALOGINFO taskInfo = {name,dialog};
		m_TaskDialogInfo.insert(std::pair<int,TASKDIALOGINFO>(i,taskInfo));
	}
}

void ShopData::loadTriggerDialogData()
{
//	std::string jsonpath = findJsonFile(TRIGGERDIALOG_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(TRIGGERDIALOG_PATH_JSON);
	m_TriggerDialogInfo.clear();

	int count = DictionaryHelper::getInstance()->getArrayCount_json(root, "trigger_dialog");
	for (int i = 0; i < count; i++)
	{
		auto &counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root, "trigger_dialog", i);
		auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
		auto dialog = DictionaryHelper::getInstance()->getStringValue_json(counts, "dialog");

		TRIGGERDIALOGINFO triggerInfo = { name, dialog };
		m_TriggerDialogInfo.insert(std::pair<int, TRIGGERDIALOGINFO>(i, triggerInfo));
	}
}
void ShopData::loadTriggerDialogJudgeData()
{
//	std::string jsonpath = findJsonFile(TRIGGERDIALOG_JUDGE_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(TRIGGERDIALOG_JUDGE_PATH_JSON);
	m_TriggerDialogJudgeInfo.clear();

	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);

			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto action = DictionaryHelper::getInstance()->getIntValue_json(counts, "action");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts, "type");
			auto need_pre_cond = DictionaryHelper::getInstance()->getIntValue_json(counts, "need_pre_cond");
			auto probability = DictionaryHelper::getInstance()->getIntValue_json(counts, "probability");

			REWARDSJSON rewards;
			rewards.coin = 0;
			rewards.exp = 0;
			rewards.reputation = 0;
			rewards.vticket = 0;
			rewards.force_id = 0;
			rewards.force_affect = 0;
			rewards.items.clear();
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "reward_json"))
			{
				auto rewards_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "reward_json");
				if (rewards_counts)
				{
					rapidjson::Document reward_json;
					reward_json.Parse<0>(rewards_counts);
					rewards.coin = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "coin");
					rewards.exp = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "exp");
					rewards.reputation = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "reputation");
					rewards.vticket = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "vticket");
					rewards.force_id = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "force_id");
					rewards.force_affect = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "force_affect");

					auto count = DictionaryHelper::getInstance()->getArrayCount_json(reward_json, "items");
					for (int i = 0; i < count; i++)
					{
						auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(reward_json, "items", i);
						ITEMINFO item;
						item.item_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_type");
						item.item_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
						item.amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "amount");
						rewards.items.push_back(item);
					}
				}
			}


			FINISHJSON position;
			position.sea = 0;
			position.city = 0;
			position.cityarea = 0;
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "position_json"))
			{
				auto position_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "position_json");
				if (position_counts)
				{
					rapidjson::Document position_json;
					position_json.Parse<0>(position_counts);
					position.sea = DictionaryHelper::getInstance()->getIntValue_json(position_json, "sea");
					position.city = DictionaryHelper::getInstance()->getIntValue_json(position_json, "city");
					position.cityarea = DictionaryHelper::getInstance()->getIntValue_json(position_json, "cityarea");
				}
			}


			TRIGGERCONDITION condition;
			condition.type = 0;
			condition.task_type = 0;
			condition.character_number = 0;
			condition.amount = 0;
			condition.compare = 0;
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "condition_json"))
			{
				auto condition_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "condition_json");
				if (condition_counts)
				{
					rapidjson::Document condition_json;
					condition_json.Parse<0>(condition_counts);
					condition.type = DictionaryHelper::getInstance()->getIntValue_json(condition_json, "type");
					condition.task_type = DictionaryHelper::getInstance()->getIntValue_json(condition_json, "task_type");
					condition.character_number = DictionaryHelper::getInstance()->getIntValue_json(condition_json, "character_number");
					condition.amount = DictionaryHelper::getInstance()->getIntValue_json(condition_json, "amount");
					condition.compare = DictionaryHelper::getInstance()->getIntValue_json(condition_json, "compare");
				}
			}

			TRIGGERDIALOGJUDGEINFO triggerJudgeInfo = { id, action, type, need_pre_cond, probability, name, rewards, position, condition };
			m_TriggerDialogJudgeInfo.insert(std::pair<int, TRIGGERDIALOGJUDGEINFO>(id, triggerJudgeInfo));
		}
		else
		{
			break;
		}
	}
}
//小伙伴红胡子剧情对话信息
void ShopData::loadPartnerRedBeardDialogData()
{
//	std::string jsonpath = findJsonFile(PARTNERREDBEARDDIALOG_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(PARTNERREDBEARDDIALOG_PATH_JSON);
	m_partnerRedBeardDialogInfo.clear();

	int count = DictionaryHelper::getInstance()->getArrayCount_json(root, "partner_dialog");
	for (int i = 0; i < count; i++)
	{
		auto &counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(root, "partner_dialog", i);
		auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
		auto dialog = DictionaryHelper::getInstance()->getStringValue_json(counts, "dialog");

		PARTNERREDBEARDDIALOGINFO partnerRedBeardDialogInfo = { name, dialog };
		m_partnerRedBeardDialogInfo.insert(std::pair<int, PARTNERREDBEARDDIALOGINFO>(i, partnerRedBeardDialogInfo));
	}
}
//小伙伴红胡子剧情任务判定
void ShopData::loadPartnerDialogJudgeData()
{
//	std::string jsonpath = findJsonFile(PARTNERREDBEARDDIALOG_JUDGE_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(PARTNERREDBEARDDIALOG_JUDGE_PATH_JSON);
	m_partnerDialogJudgeInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);

			auto title = DictionaryHelper::getInstance()->getStringValue_json(counts, "title");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts, "desc");
			auto tip = DictionaryHelper::getInstance()->getStringValue_json(counts, "tip");
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto chapter_idx = DictionaryHelper::getInstance()->getIntValue_json(counts, "chapter_idx");
			auto start_flag = DictionaryHelper::getInstance()->getIntValue_json(counts, "start_flag");
			auto next_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "next_id");
			auto companion_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "companion_id");

			TRIGGERJSON trigger;
			trigger.part_num = 0;
			trigger.parts.clear();
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "trigger_json"))
			{
				auto trigger_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "trigger_json");
				if (trigger_counts)
				{
					rapidjson::Document trigger_json;
					trigger_json.Parse<0>(trigger_counts);

					if (DictionaryHelper::getInstance()->checkObjectExist_json(trigger_json, "part_num"))
					{
						trigger.part_num = DictionaryHelper::getInstance()->getIntValue_json(trigger_json, "part_num");
					}

					auto count = DictionaryHelper::getInstance()->getArrayCount_json(trigger_json, "parts");
					for (int i = 0; i < count; i++)
					{
						auto&part_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(trigger_json, "parts", i);
						PARTSINFO part;
						part.sea = 0;
						part.city = 0;
						part.cityarea = 0;
						part.x = 0;
						part.y = 0;
						part.r = 0;
						part.type = 0;
						part.action = 0;
						part.part = 0;
						part.items.clear();
						part.has_position = false;
						part.has_condition = false;
						part.target_id = 0;
						part.target_amount = 0;
						part.target_city = 0;
						part.has_npc = false;
						part.npc_id = 0;
						part.has_branch = false;
						part.branch = 0;
						part.has_next_part = false;
						part.next_part = 0;
						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "action"))
						{
							part.action = DictionaryHelper::getInstance()->getIntValue_json(part_counts, "action");
						}
						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "part"))
						{
							part.part = DictionaryHelper::getInstance()->getIntValue_json(part_counts, "part");
						}

						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "npc_id"))
						{
							part.has_npc = true;
							part.npc_id = DictionaryHelper::getInstance()->getIntValue_json(part_counts, "npc_id");
						}

						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "nextpart"))
						{
							part.has_next_part = true;
							part.next_part = DictionaryHelper::getInstance()->getIntValue_json(part_counts, "nextpart");
						}

						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "position"))
						{
							part.has_position = true;
							auto &position_counts = DictionaryHelper::getInstance()->getSubDictionary_json(part_counts, "position");

							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "sea"))
							{
								part.sea = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "sea");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "city"))
							{
								part.city = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "city");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "cityarea"))
							{
								part.cityarea = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "cityarea");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "x"))
							{
								part.x = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "x");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "y"))
							{
								part.y = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "y");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "r"))
							{
								part.r = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "r");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "branch"))
							{
								part.has_branch = true;
								part.branch = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "branch");
							}
						}

						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "condition"))
						{
							part.has_condition = true;
							auto &condition_counts = DictionaryHelper::getInstance()->getSubDictionary_json(part_counts, "condition");

							if (DictionaryHelper::getInstance()->checkObjectExist_json(condition_counts, "type"))
							{
								part.type = DictionaryHelper::getInstance()->getIntValue_json(condition_counts, "type");
							}

							if(DictionaryHelper::getInstance()->checkObjectExist_json(condition_counts, "target_items"))
							{
								auto count = DictionaryHelper::getInstance()->getArrayCount_json(condition_counts, "target_items");
								for (int i = 0; i < count; i++)
								{
									auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(condition_counts, "target_items", i);
									if (DictionaryHelper::getInstance()->checkObjectExist_json(items_counts, "item_type"))
									{
										ITEMINFO item;
										item.item_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_type");
										item.item_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
										item.amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_amount");
										part.items.push_back(item);
									}
									else
									{
										part.target_city = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "city");
										part.target_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
										part.target_amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_amount");
									}
								}
							}
							else if(DictionaryHelper::getInstance()->checkObjectExist_json(condition_counts, "target_npc"))
							{
								auto count = DictionaryHelper::getInstance()->getArrayCount_json(condition_counts, "target_npc");
								if(count > 0)
								{
									auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(condition_counts, "target_npc", 0);
									if(DictionaryHelper::getInstance()->checkObjectExist_json(items_counts))
									{
										part.target_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "npc_id");
										part.target_amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "npc_amount");
									}

								}
							}
						}
						trigger.parts.push_back(part);
					}
				}
			}


			REWARDSJSON rewards;
			rewards.coin = 0;
			rewards.exp = 0;
			rewards.reputation = 0;
			rewards.vticket = 0;
			rewards.force_id = 0;
			rewards.force_affect = 0;
			rewards.items.clear();
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "rewards_json"))
			{
				auto rewards_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "rewards_json");
				if (rewards_counts)
				{
					rapidjson::Document reward_json;
					reward_json.Parse<0>(rewards_counts);
					rewards.coin = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "coin");
					rewards.exp = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "exp");
					rewards.reputation = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "reputation");
					rewards.vticket = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "vticket");
					rewards.force_id = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "force_id");
					rewards.force_affect = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "force_affect");

					auto count = DictionaryHelper::getInstance()->getArrayCount_json(reward_json, "items");
					for (int i = 0; i < count; i++)
					{
						auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(reward_json, "items", i);
						ITEMINFO item;
						item.item_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_type");
						item.item_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
						item.amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "amount");
						rewards.items.push_back(item);
					}
				}
			}


			FINISHJSON finish;
			finish.sea = 0;
			finish.city = 0;
			finish.items.clear();
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "finish_json"))
			{
				auto finish_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "finish_json");
				if (finish_counts)
				{
					rapidjson::Document finish_json;
					finish_json.Parse<0>(finish_counts);

					if (DictionaryHelper::getInstance()->checkObjectExist_json(finish_json, "position"))
					{
						auto &position_counts = DictionaryHelper::getInstance()->getSubDictionary_json(finish_json, "position");

						if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "sea"))
						{
							finish.sea = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "sea");
						}
						if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "city"))
						{
							finish.city = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "city");
						}
					}
				}
			}


			PRETASKJSON pretask;
			pretask.coin = 0;
			pretask.bank = 0;
			pretask.items.clear();
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "pretask_json"))
			{
				auto pretask_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "pretask_json");
				if (pretask_counts)
				{
					rapidjson::Document pretask_json;
					pretask_json.Parse<0>(pretask_counts);
					if (DictionaryHelper::getInstance()->checkObjectExist_json(pretask_json, "coin"))
					{
						pretask.coin = DictionaryHelper::getInstance()->getIntValue_json(pretask_json, "coin");
					}

					if (DictionaryHelper::getInstance()->checkObjectExist_json(pretask_json, "bank"))
					{
						pretask.bank = DictionaryHelper::getInstance()->getIntValue_json(pretask_json, "bank");
					}

					auto count = DictionaryHelper::getInstance()->getArrayCount_json(pretask_json, "items");
					for (int i = 0; i < count; i++)
					{
						auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(pretask_json, "items", i);
						ITEMINFO item;
						item.item_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_type");
						item.item_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
						item.amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "amount");
						pretask.items.push_back(item);
					}
				}
			}

			PARTNERDIALOGJUDGEINFO partnerRedBeardDialogJudgeInfo = { title, name, desc, tip, id, chapter_idx, start_flag, next_id, companion_id, trigger, rewards, finish, pretask };
			m_partnerDialogJudgeInfo.insert(std::pair<int, PARTNERDIALOGJUDGEINFO>(id, partnerRedBeardDialogJudgeInfo));
		}
		else
		{
			break;
		}
	}
}
void ShopData::loadCaptainSkillData()
{
//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(CAPTAINSKILL_PATH_JSON);
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(CAPTAINSKILL_PATH_JSON)
	m_CaptainSkillInfo.clear();

	for (int i = 0; ;i++)
	{
		if(DictionaryHelper::getInstance()->checkObjectExist_json(root,i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root,i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts,"id");
			auto cdreduce_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"cdreduce_per_lv");
		    auto efffect_skill_id = DictionaryHelper::getInstance()->getIntValue_json(counts,"efffect_skill_id");
			auto skill_layer = DictionaryHelper::getInstance()->getIntValue_json(counts,"skill_layer");
			auto base_rate = DictionaryHelper::getInstance()->getIntValue_json(counts,"base_rate");
			auto points_per_level = DictionaryHelper::getInstance()->getIntValue_json(counts,"points_per_lv");
			auto cd = DictionaryHelper::getInstance()->getIntValue_json(counts,"cd");
			auto base_skill_require_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"base_skill_require_lv");
			auto rate_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"rate_per_lv");
			auto ap_type = DictionaryHelper::getInstance()->getIntValue_json(counts,"ap_type");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts,"type");
			auto sub_type = DictionaryHelper::getInstance()->getIntValue_json(counts,"sub_type");
			auto base_effect = DictionaryHelper::getInstance()->getIntValue_json(counts,"base_effect");
			auto effect_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"effect_per_lv");
			auto base_skill_id = DictionaryHelper::getInstance()->getIntValue_json(counts,"base_skill_id");
			auto base_duration = DictionaryHelper::getInstance()->getIntValue_json(counts,"base_duration");
			auto max_level = DictionaryHelper::getInstance()->getIntValue_json(counts,"max_level");
			auto duration_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts,"duration_per_lv");
			auto require_level = DictionaryHelper::getInstance()->getIntValue_json(counts,"require_level");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts,"name");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts,"descr");
			auto next_desc = DictionaryHelper::getInstance()->getStringValue_json(counts,"nextdesc");
			auto icon_id = DictionaryHelper::getInstance()->getIntValue_json(counts,"icon_id");
			auto cur_skill_level = 0;
			auto max_level_special = DictionaryHelper::getInstance()->getIntValue_json(counts, "max_level_special");
			auto room_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "room_type");
			auto effect_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "effect_type");
			SKILL_TREE skill = {cdreduce_per_lv,efffect_skill_id,skill_layer,base_rate,points_per_level,cd,
				base_skill_require_lv,rate_per_lv,ap_type,type,sub_type,base_effect,effect_per_lv,base_skill_id,
				base_duration, max_level, duration_per_lv, require_level, cur_skill_level, name, desc, next_desc, icon_id,
				max_level_special, room_type, effect_type};
			m_CaptainSkillInfo.insert(std::pair<int,SKILL_TREE>(id,skill));
		}else
		{
			break;
		}
	}
}

void ShopData::loadZoneData()
{
//	std::string jsonpath = findJsonFile(ZONE_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(ZONE_PATH_JSON);
	m_zoneInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto &counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");

			m_zoneInfo.insert(std::pair<int, std::string>(id, name));
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadMainTaskData()
{
//	std::string jsonpath = findJsonFile(MAINTASK_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(MAINTASK_PATH_JSON);
	m_MainTaskInfo.clear();

	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);

			auto title = DictionaryHelper::getInstance()->getStringValue_json(counts, "title");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts, "desc");
			auto tip = DictionaryHelper::getInstance()->getStringValue_json(counts, "tip");
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto nation = DictionaryHelper::getInstance()->getIntValue_json(counts, "nation");
			auto chapter_idx = DictionaryHelper::getInstance()->getIntValue_json(counts, "chapter_idx");
			auto start_flag = DictionaryHelper::getInstance()->getIntValue_json(counts, "start_flag");
			auto next_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "next_id");

			TRIGGERJSON trigger;
			trigger.part_num = 0;
			trigger.parts.clear();
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "trigger_json"))
			{
				auto trigger_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "trigger_json");
				if (trigger_counts)
				{
					rapidjson::Document trigger_json;
					trigger_json.Parse<0>(trigger_counts);

					if (DictionaryHelper::getInstance()->checkObjectExist_json(trigger_json, "part_num"))
					{
						trigger.part_num = DictionaryHelper::getInstance()->getIntValue_json(trigger_json, "part_num");
					}

					auto count = DictionaryHelper::getInstance()->getArrayCount_json(trigger_json, "parts");
					for (int i = 0; i < count; i++)
					{
						auto&part_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(trigger_json, "parts", i);
						PARTSINFO part;
						part.sea = 0;
						part.city = 0;
						part.cityarea = 0;
						part.x = 0;
						part.y = 0;
						part.r = 0;
						part.type = 0;
						part.action = 0;
						part.part = 0;
						part.items.clear();
						part.has_position = false;
						part.has_condition = false;
						part.target_id = 0;
						part.target_amount = 0;
						part.target_city = 0;
						part.has_npc = false; 
						part.npc_id = 0;
						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "action"))
						{
							part.action = DictionaryHelper::getInstance()->getIntValue_json(part_counts, "action");
						}
						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "part"))
						{
							part.part = DictionaryHelper::getInstance()->getIntValue_json(part_counts, "part");
						}

						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "npc_id"))
						{
							part.has_npc = true;
							part.npc_id = DictionaryHelper::getInstance()->getIntValue_json(part_counts, "npc_id");
						}

						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "position"))
						{
							part.has_position = true;
							auto &position_counts = DictionaryHelper::getInstance()->getSubDictionary_json(part_counts, "position");

							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "sea"))
							{
								part.sea = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "sea");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "city"))
							{
								part.city = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "city");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "cityarea"))
							{
								part.cityarea = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "cityarea");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "x"))
							{
								part.x = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "x");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "y"))
							{
								part.y = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "y");
							}
							if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "r"))
							{
								part.r = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "r");
							}
						}

						if (DictionaryHelper::getInstance()->checkObjectExist_json(part_counts, "condition"))
						{
							part.has_condition = true;
							auto &condition_counts = DictionaryHelper::getInstance()->getSubDictionary_json(part_counts, "condition");

							if (DictionaryHelper::getInstance()->checkObjectExist_json(condition_counts, "type"))
							{
								part.type = DictionaryHelper::getInstance()->getIntValue_json(condition_counts, "type");
							}

							auto count = DictionaryHelper::getInstance()->getArrayCount_json(condition_counts, "target_items");
							for (int i = 0; i < count; i++)
							{
								auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(condition_counts, "target_items", i);
								if (DictionaryHelper::getInstance()->checkObjectExist_json(items_counts, "item_type"))
								{
									ITEMINFO item;
									item.item_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_type");
									item.item_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
									item.amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_amount");
									part.items.push_back(item);
								}
								else
								{
									part.target_city = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "city");
									part.target_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
									part.target_amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_amount");
								}
							}
						}
						trigger.parts.push_back(part);
					}
				}
			}


			REWARDSJSON rewards;
			rewards.coin = 0;
			rewards.exp = 0;
			rewards.reputation = 0;
			rewards.vticket = 0;
			rewards.force_id = 0;
			rewards.force_affect = 0;
			rewards.items.clear();
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "rewards_json"))
			{
				auto rewards_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "rewards_json");
				if (rewards_counts)
				{
					rapidjson::Document reward_json;
					reward_json.Parse<0>(rewards_counts);
					rewards.coin = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "coin");
					rewards.exp = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "exp");
					rewards.reputation = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "reputation");
					rewards.vticket = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "vticket");
					rewards.force_id = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "force_id");
					rewards.force_affect = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "force_affect");

					auto count = DictionaryHelper::getInstance()->getArrayCount_json(reward_json, "items");
					for (int i = 0; i < count; i++)
					{
						auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(reward_json, "items", i);
						ITEMINFO item;
						item.item_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_type");
						item.item_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
						item.amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "amount");
						rewards.items.push_back(item);
					}
				}	
			}


			FINISHJSON finish;
			finish.sea = 0;
			finish.city = 0;
			finish.cityarea = 0;
			finish.type = 0;
			finish.x = 0;
			finish.y = 0;
			finish.r = 0;
			finish.items.clear();
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "finish_json"))
			{
				auto finish_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "finish_json");
				if (finish_counts)
				{
					rapidjson::Document finish_json;
					finish_json.Parse<0>(finish_counts);

					if (DictionaryHelper::getInstance()->checkObjectExist_json(finish_json, "position"))
					{
						auto &position_counts = DictionaryHelper::getInstance()->getSubDictionary_json(finish_json, "position");

						if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "sea"))
						{
							finish.sea = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "sea");
						}
						if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "city"))
						{
							finish.city = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "city");
						}
						if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "cityarea"))
						{
							finish.cityarea = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "cityarea");
						}
						if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "x"))
						{
							finish.x = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "x");
						}
						if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "y"))
						{
							finish.y = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "y");
						}
						if (DictionaryHelper::getInstance()->checkObjectExist_json(position_counts, "r"))
						{
							finish.r = DictionaryHelper::getInstance()->getIntValue_json(position_counts, "r");
						}
					}

					if (DictionaryHelper::getInstance()->checkObjectExist_json(finish_json, "condition"))
					{
						auto &condition_counts = DictionaryHelper::getInstance()->getSubDictionary_json(finish_json, "condition");

						if (DictionaryHelper::getInstance()->checkObjectExist_json(condition_counts, "type"))
						{
							finish.type = DictionaryHelper::getInstance()->getIntValue_json(condition_counts, "type");
						}

						auto count = DictionaryHelper::getInstance()->getArrayCount_json(condition_counts, "target_items");
						for (int i = 0; i < count; i++)
						{
							auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(condition_counts, "target_items", i);
							ITEMINFO item;
							item.item_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_type");
							item.item_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
							item.amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_amount");
							finish.items.push_back(item);
						}
					}
				}
			}


			PRETASKJSON pretask;
			pretask.coin = 0;
			pretask.bank = 0;
			pretask.items.clear();
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "pretask_json"))
			{
				auto pretask_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "pretask_json");
				if (pretask_counts)
				{
					rapidjson::Document pretask_json;
					pretask_json.Parse<0>(pretask_counts);
					if (DictionaryHelper::getInstance()->checkObjectExist_json(pretask_json, "coin"))
					{
						pretask.coin = DictionaryHelper::getInstance()->getIntValue_json(pretask_json, "coin");
					}

					if (DictionaryHelper::getInstance()->checkObjectExist_json(pretask_json, "bank"))
					{
						pretask.bank = DictionaryHelper::getInstance()->getIntValue_json(pretask_json, "bank");
					}

					auto count = DictionaryHelper::getInstance()->getArrayCount_json(pretask_json, "items");
					for (int i = 0; i < count; i++)
					{
						auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(pretask_json, "items", i);
						ITEMINFO item;
						item.item_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_type");
						item.item_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
						item.amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "amount");
						pretask.items.push_back(item);
					}
				}	
			}
	
			MAINTASKINFO mainTask = {title, name, desc, tip, id, nation, chapter_idx, start_flag, next_id, trigger, rewards, finish, pretask};
			m_MainTaskInfo.insert(std::pair<int, MAINTASKINFO>(id, mainTask));
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadBagExpandFeeData()
{
//	std::string jsonpath = findJsonFile(BAG_EXPAND_FEE_PATH_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(BAG_EXPAND_FEE_PATH_JSON)
	m_BagExpandFeeInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto fee = DictionaryHelper::getInstance()->getIntValue_json(counts, "fee");
			auto capacity = DictionaryHelper::getInstance()->getIntValue_json(counts, "capacity");
			BAGEXPANDFEEINFO bagInfo = { id, fee, capacity };
			m_BagExpandFeeInfo.insert(std::pair<int, BAGEXPANDFEEINFO>(id, bagInfo));
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadBossAwards()
{
//	std::string jsonpath = findJsonFile(BOSS_AWARDS_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(BOSS_AWARDS_JSON);
	m_bossAwards.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto coin = DictionaryHelper::getInstance()->getIntValue_json(counts, "coin");
			auto v_ticket = DictionaryHelper::getInstance()->getIntValue_json(counts, "v_ticket");
			auto badge = DictionaryHelper::getInstance()->getIntValue_json(counts, "badge");
			BOSS_AWARDS bossInfo = { coin, v_ticket, badge };
			m_bossAwards.insert(std::pair<int, BOSS_AWARDS>(id, bossInfo));
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadCompanionData()
{
//	std::string jsonpath = findJsonFile(COMPANION_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(COMPANION_JSON);
	m_companionInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			auto ap = DictionaryHelper::getInstance()->getIntValue_json(counts, "ap");
			auto dp = DictionaryHelper::getInstance()->getIntValue_json(counts, "dp");
			auto hp = DictionaryHelper::getInstance()->getIntValue_json(counts, "hp");
			auto ap_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "ap_lv");
			auto dp_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "dp_lv");
			auto hp_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "hp_lv");
			auto skill1 = DictionaryHelper::getInstance()->getIntValue_json(counts, "skill1");
			auto skill2 = DictionaryHelper::getInstance()->getIntValue_json(counts, "skill2");
			auto skill3 = DictionaryHelper::getInstance()->getIntValue_json(counts, "skill3");
			auto specialty1 = DictionaryHelper::getInstance()->getIntValue_json(counts, "specialty1");
			auto specialty2 = DictionaryHelper::getInstance()->getIntValue_json(counts, "specialty2");
			auto specialty3 = DictionaryHelper::getInstance()->getIntValue_json(counts, "specialty3");
			auto item1 = DictionaryHelper::getInstance()->getIntValue_json(counts, "item1");
			auto item1_amount = DictionaryHelper::getInstance()->getIntValue_json(counts, "item1_amount");
			auto item2 = DictionaryHelper::getInstance()->getIntValue_json(counts, "item2");
			auto item2_amount = DictionaryHelper::getInstance()->getIntValue_json(counts, "item2_amount");
			auto item3 = DictionaryHelper::getInstance()->getIntValue_json(counts, "item3");
			auto item3_amount = DictionaryHelper::getInstance()->getIntValue_json(counts, "item3_amount");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts, "desc");
			auto icon_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "icon_id");
			auto card_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "card_id");
			auto model_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "model_id");

			COMPANION_INFO campanionInfo = {id, name, ap, dp, hp, ap_lv, dp_lv, hp_lv, skill1, skill2, skill3, specialty1, specialty2,
				specialty3,item1, item1_amount, item2, item2_amount, item3, item3_amount, desc, icon_id, card_id, model_id,};
			m_companionInfo.insert(std::pair<int, COMPANION_INFO>(id, campanionInfo));
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadCompanionSkillData()
{
//	std::string jsonpath;
//	rapidjson::Document root;
//	jsonpath = findJsonFile(COMPANION_SKILL_JSON);
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(COMPANION_SKILL_JSON);
	m_companionSkillInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto cdreduce_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "cdreduce_per_lv");
			auto efffect_skill_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "efffect_skill_id");
			auto skill_layer = DictionaryHelper::getInstance()->getIntValue_json(counts, "skill_layer");
			auto base_rate = DictionaryHelper::getInstance()->getIntValue_json(counts, "base_rate");
			auto points_per_level = DictionaryHelper::getInstance()->getIntValue_json(counts, "points_per_lv");
			auto cd = DictionaryHelper::getInstance()->getIntValue_json(counts, "cd");
			auto base_skill_require_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "base_skill_require_lv");
			auto rate_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "rate_per_lv");
			auto ap_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "ap_type");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts, "type");
			auto sub_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "sub_type");
			auto base_effect = DictionaryHelper::getInstance()->getIntValue_json(counts, "base_effect");
			auto effect_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "effect_per_lv");
			auto base_skill_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "base_skill_id");
			auto base_duration = DictionaryHelper::getInstance()->getIntValue_json(counts, "base_duration");
			auto max_level = DictionaryHelper::getInstance()->getIntValue_json(counts, "max_level");
			auto duration_per_lv = DictionaryHelper::getInstance()->getIntValue_json(counts, "duration_per_lv");
			auto require_level = DictionaryHelper::getInstance()->getIntValue_json(counts, "require_level");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts, "descr");
			auto next_desc = DictionaryHelper::getInstance()->getStringValue_json(counts, "nextdesc");
			auto icon_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "icon_id");
			auto cur_skill_level = 0;
			auto max_level_special = DictionaryHelper::getInstance()->getIntValue_json(counts, "max_level_special");
			auto room_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "room_type");
			auto effect_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "effect_type");
			SKILL_TREE skill = { cdreduce_per_lv, efffect_skill_id, skill_layer, base_rate, points_per_level, cd,
				base_skill_require_lv, rate_per_lv, ap_type, type, sub_type, base_effect, effect_per_lv, base_skill_id,
				base_duration, max_level, duration_per_lv, require_level, cur_skill_level, name, desc, next_desc, icon_id,
				max_level_special, room_type, effect_type };
			m_companionSkillInfo.insert(std::pair<int, SKILL_TREE>(id, skill));
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadJobPositionData()
{
//	std::string jsonpath = findJsonFile(JOB_POSITION_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(JOB_POSITION_JSON);
	m_jobPositionInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto room = DictionaryHelper::getInstance()->getStringValue_json(counts, "room");
			auto job = DictionaryHelper::getInstance()->getStringValue_json(counts, "job");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts, "desc");

			JOB_POSITION_INFO jobInfo = { id, room, job, desc };
			m_jobPositionInfo.insert(std::pair<int, JOB_POSITION_INFO>(id, jobInfo));
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadSpecialiesSkillData()
{
//	std::string jsonpath = findJsonFile(COMPANION_SPECIALIES_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(COMPANION_SPECIALIES_JSON);
	m_companionSpecialiesSkillInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			auto require_level = DictionaryHelper::getInstance()->getIntValue_json(counts, "required_level");
			auto effect_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "effect_type");
			auto room_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "room_type");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts, "desc");

			SPECIALIES_SKILL_INFO skillInfo = { id, name, require_level, effect_type, room_type, desc};
			m_companionSpecialiesSkillInfo.insert(std::pair<int, SPECIALIES_SKILL_INFO>(id, skillInfo));
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadShipRoomData()
{
//	std::string jsonpath = findJsonFile(SHIP_ROOM_JSON);
//	rapidjson::Document root;
//	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
//	root.Parse<0>(contentStr.c_str());
	CONFIG_PARSE_HEAD(SHIP_ROOM_JSON);
	m_shipRoomInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto ship_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "ship_type");
			auto room_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "room_id");
			auto room_type = DictionaryHelper::getInstance()->getIntValue_json(counts, "room_type");

			SHIP_ROOM_INFO shipInfo = { id, ship_type, room_id, room_type};
			m_shipRoomInfo.insert(std::pair<int, SHIP_ROOM_INFO>(id, shipInfo));
		}
		else
		{
			break;
		}
	}
}
void ShopData::loadMaintaskGuideData()
{
	CONFIG_PARSE_HEAD(MAIN_TASK_GUIDE_JSON);
	m_MainTaskGuideInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);

			auto title = DictionaryHelper::getInstance()->getStringValue_json(counts, "title");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			auto desc = DictionaryHelper::getInstance()->getStringValue_json(counts, "desc");
			auto tip = DictionaryHelper::getInstance()->getStringValue_json(counts, "tip");
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto nation = DictionaryHelper::getInstance()->getIntValue_json(counts, "nation");
			auto chapter_idx = DictionaryHelper::getInstance()->getIntValue_json(counts, "chapter_idx");
			auto start_flag = DictionaryHelper::getInstance()->getIntValue_json(counts, "start_flag");
			auto next_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "next_id");

			REWARDSJSON rewards;
			rewards.coin = 0;
			rewards.exp = 0;
			rewards.reputation = 0;
			rewards.vticket = 0;
			rewards.force_id = 0;
			rewards.force_affect = 0;
			rewards.items.clear();
			if (DictionaryHelper::getInstance()->checkObjectExist_json(counts, "rewards_json"))
			{
				auto rewards_counts = DictionaryHelper::getInstance()->getStringValue_json(counts, "rewards_json");
				if (rewards_counts)
				{
					rapidjson::Document reward_json;
					reward_json.Parse<0>(rewards_counts);
					rewards.coin = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "coin");
					rewards.exp = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "exp");
					rewards.reputation = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "reputation");
					rewards.vticket = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "vticket");
					rewards.force_id = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "force_id");
					rewards.force_affect = DictionaryHelper::getInstance()->getIntValue_json(reward_json, "force_affect");

					auto count = DictionaryHelper::getInstance()->getArrayCount_json(reward_json, "items");
					for (int i = 0; i < count; i++)
					{
						auto&items_counts = DictionaryHelper::getInstance()->getDictionaryFromArray_json(reward_json, "items", i);
						ITEMINFO item;
						item.item_type = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_type");
						item.item_id = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "item_id");
						item.amount = DictionaryHelper::getInstance()->getIntValue_json(items_counts, "amount");
						rewards.items.push_back(item);
					}
				}
			}	
			MAINTASKGUIDEINFO mainTaskGuide = { id, nation, chapter_idx, next_id, start_flag, tip, name, title, desc, rewards };
			m_MainTaskGuideInfo.insert(std::pair<int, MAINTASKGUIDEINFO>(id, mainTaskGuide));
		}
		else
		{
			break;
		}
	}

}
void ShopData::loadProficiencyData()
{
	CONFIG_PARSE_HEAD(PROFICNENCY_INFO_JSON);
	m_proficiencyInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts, "type");
			auto visible = DictionaryHelper::getInstance()->getIntValue_json(counts, "visible");
			auto iconid = DictionaryHelper::getInstance()->getIntValue_json(counts, "icon_id");
			auto item = DictionaryHelper::getInstance()->getIntValue_json(counts, "item_need");
			PROFICIENCY_INFO proficiencyInfo = { id, name, visible, type, iconid, item };
			m_proficiencyInfo.insert(std::pair<int, PROFICIENCY_INFO>(id, proficiencyInfo));
		}
		else
		{
			break;
		}
	}


}

void ShopData::loadCityWarPositionData()
{
	CONFIG_PARSE_HEAD(WARPOSITION_INFO_JSON);
	m_cityWarPositionInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");

			std::vector<POSITION_EXAMPLE>v_example;
			for (int j = 0; j < 5; j++)
			{
				auto x = DictionaryHelper::getInstance()->getIntValue_json(counts, StringUtils::format("x%d", j + 1).c_str());
				auto y = DictionaryHelper::getInstance()->getIntValue_json(counts, StringUtils::format("y%d", j + 1).c_str());
				auto r = DictionaryHelper::getInstance()->getIntValue_json(counts, StringUtils::format("r%d", j + 1).c_str());
				POSITION_EXAMPLE p_example = { x, y, r };
				v_example.push_back(p_example);
			}
			CITY_WAR_POSITION war_positin = { id, v_example };
			m_cityWarPositionInfo.insert(std::pair<int, CITY_WAR_POSITION>(id, war_positin));
		}
		else
		{
			break;
		}
	}
}

void ShopData::loadShipWarPositionData()
{
	CONFIG_PARSE_HEAD(SHIPPOSITION_INFO_JSON);
	m_shipWarPositionInfo.clear();
	std::string st_X[] = { "A_x", "B_x", "C_x", "D_x", "E_x", "F_x" };
	std::string st_Y[] = { "A_y", "B_y", "C_y", "D_y", "E_y", "F_y" };
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto id = i + 1;
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto model_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "model_id");
			auto direction = DictionaryHelper::getInstance()->getIntValue_json(counts, "direction");

			std::vector<SHIP_POSITION>v_ship_position;
			for (int j = 0; j < 6; j++)
			{
				auto x = DictionaryHelper::getInstance()->getIntValue_json(counts, st_X[j].c_str());
				auto y = DictionaryHelper::getInstance()->getIntValue_json(counts, st_Y[j].c_str());
				SHIP_POSITION p_example = { x, y };
				v_ship_position.push_back(p_example);
			}
			SHIP_WAR_POSITION ship_positin = { id, model_id, direction, v_ship_position };
			m_shipWarPositionInfo.insert(std::pair<int, SHIP_WAR_POSITION>(id, ship_positin));
		}
		else
		{
			break;
		}
	}
}

void ShopData::LoadCityAreaResourceData()
{
	CONFIG_PARSE_HEAD(CITIES_RESOURCES_INFO_JSON);
	m_cityAreaResourceInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto background_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "background_id");
			auto background_village = DictionaryHelper::getInstance()->getIntValue_json(counts, "background_village");
			auto shipyard_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "shipyard_id");
			auto bank_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "bank_id");
			auto dock_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "dock_id");
			auto tavern_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "tavern_id");
			auto cityhall_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "cityhall_id");
			auto supplyofficer_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "supplyofficer_id");
			auto soldier_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "soldier_id");
			auto villageboss_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "villageboss_id");
			auto chiefofficer_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "chiefofficer_id");
			auto merchant_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "merchant_id");
			auto icon_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "icon_id");
			auto icon_village = DictionaryHelper::getInstance()->getIntValue_json(counts, "icon_village");
			auto market_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "market_id");
			auto shipboss_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "shipboss_id");
			auto bargirl_id = DictionaryHelper::getInstance()->getIntValue_json(counts, "bargirl_id");

			CITY_AREA_RESOURCE cityAreaResourceInfo = { id, background_id, background_village, shipyard_id, bank_id, dock_id, tavern_id, cityhall_id, supplyofficer_id, soldier_id, villageboss_id,
				chiefofficer_id, merchant_id, icon_id, icon_village,market_id, shipboss_id, bargirl_id, };
			m_cityAreaResourceInfo.insert(std::pair<int, CITY_AREA_RESOURCE>(id, cityAreaResourceInfo));
		}
		else
		{
			break;
		}
	}
}
void ShopData::loadSettingLengend()
{
	CONFIG_PARSE_HEAD(SETTING_LENGEND_INFO_JSON);
	m_settingLengendInfo.clear();
	for (int i = 0;; i++)
	{
		if (DictionaryHelper::getInstance()->checkObjectExist_json(root, i))
		{
			auto& counts = DictionaryHelper::getInstance()->getSubDictionary_json(root, i);
			auto id = DictionaryHelper::getInstance()->getIntValue_json(counts, "id");
			auto type = DictionaryHelper::getInstance()->getIntValue_json(counts, "type");
			auto name = DictionaryHelper::getInstance()->getStringValue_json(counts, "name");
			auto path = DictionaryHelper::getInstance()->getStringValue_json(counts, "path");
			SETTING_LENGEND settingInfo ={id, type, name, path,};
			m_settingLengendInfo.insert(std::pair<int, SETTING_LENGEND>(id, settingInfo));
		}
		else
		{
			break;
		}
	}

}
void ShopData::releaseLandData()
{
	m_LandData.clear();
}

std::string ShopData::findJsonFile(std::string name) {
	std::size_t found = name.find_last_of(".");
	if (found != std::string::npos) {
		std::string nameExt = name.substr(found + 1);
		if (nameExt.compare("json") == 0) {
			std::string path = getRemoteJson(name);
			if (path.length() > 0) {
				return path;
			} else {
				std::string fullPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name);
				return fullPath;
			}
		}
	}

	std::string jsonPath;
	LanguageType nType = LanguageType(Utils::getLanguage());
	switch (nType) {
	case cocos2d::LanguageType::CHINESE: {
		std::string remoteJson = getRemoteJson(name + ".zh_CN" + ".json");
		if (remoteJson.length() > 0) {
			jsonPath = remoteJson;
		} else if (cocos2d::CCFileUtils::getInstance()->isFileExist(name + ".zh_CN" + ".json")) {
			jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".zh_CN" + ".json");

		} else {
			jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".json");
		}
	}
		break;
	case cocos2d::LanguageType::TRADITIONAL_CHINESE: {
		log("language: traditional chinese");
		std::string remoteJson = getRemoteJson(name + ".zh_HK" + ".json");
		if (remoteJson.length() > 0) {
			jsonPath = remoteJson;
		} else if (cocos2d::CCFileUtils::getInstance()->isFileExist(name + ".zh_HK" + ".json")) {
			jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".zh_HK" + ".json");

		} else {
			jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".json");
		}
	}
		break;
	default: {
		log("language: english");
		std::string remoteJson = getRemoteJson(name + ".json");
		if (remoteJson.length() > 0) {
			jsonPath = remoteJson;
		} else {
			jsonPath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(name + ".json");
		}
	}
		break;
	}
	return jsonPath;
}
