#include "TVStaticDataManager.h"
#include "ConverterHelper.h"

#include "json/stringbuffer.h"
#include "json/writer.h"
#include "cocostudio/DictionaryHelper.h"
#include "cocostudio/TextResUtils.h"
#include "TVSDHeaders.h"

using namespace rapidjson;


TVStaticDataManager::TVStaticDataManager()
{

	strKeyTableDB.clear();
	intKeyTableDB.clear();


	InitLoadTableList();
}

TVStaticDataManager::~TVStaticDataManager()
{
}

void TVStaticDataManager::LoadStrTableData(const char* fileName)
{
	map<string, TVStaticDataBase*>* tableAsset = 0;
	string fileKey = fileName;
	if (strKeyTableDB.find(fileName) != strKeyTableDB.end())
	{
		tableAsset = strKeyTableDB[fileKey];
	}
	else
	{
		tableAsset = new map<string, TVStaticDataBase*>();
		strKeyTableDB[fileKey] = tableAsset;
	}

	std::string jsonpath("StaticData/");
	jsonpath.append(fileName);
	jsonpath.append(".json");

	rapidjson::Document root;
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
	root.Parse<0>(contentStr.c_str());
	if (root.HasParseError())
	{
		return;
	}

	auto it = root.MemberonBegin();
	while(it != root.MemberonEnd())
	{
		TVStaticDataBase* ins = TVSDHeaderHelper::CreateTemplateIns(fileName);
		const char*  key = it->name.GetString();
		ins->Load(it->value);
		tableAsset->insert(make_pair(key, ins));
		it++;
	}
}


void TVStaticDataManager::LoadIntTableData(const char* fileName)
{
	map<int, TVStaticDataBase*>* tableAsset = 0;
	string fileKey = fileName;
	if (intKeyTableDB.find(fileName) != intKeyTableDB.end())
	{
		tableAsset = intKeyTableDB[fileKey];
	}
	else
	{
		tableAsset = new map<int, TVStaticDataBase*>();
		intKeyTableDB[fileKey] = tableAsset;
	}

	std::string jsonpath("StaticData/");
	jsonpath.append(fileName);
	jsonpath.append(".json");

	rapidjson::Document root;
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
	root.Parse<0>(contentStr.c_str());
	if (root.HasParseError())
	{
		return;
	}

	auto it = root.MemberonBegin();
	while (it != root.MemberonEnd())
	{
		TVStaticDataBase* ins = TVSDHeaderHelper::CreateTemplateIns(fileName);
		ins->Load(it->value);
		int k = ConvertHelper::atoint(it->name.GetString());
		tableAsset->insert(make_pair(k, ins));
		it++;
	}
}

void TVStaticDataManager::InitLoadTableList()
{
	LoadStrTableData(SDTableList::ConfigName());

	map<string, TVStaticDataBase*>* map = GetStrTable(SDTableList::ConfigName());
	for (auto it = map->begin(); it != map->end(); it ++)
	{
		if (it->first != SDTableList::ConfigName())
		{
			SDTableList* item = dynamic_cast<SDTableList*>(it->second);
			assert(item != NULL);
			if (item->getTableIdType() == 0)
			{
				LoadIntTableData(item->getTableName().c_str());
			}else if (item->getTableIdType() == 1)
			{
				LoadStrTableData(item->getTableName().c_str());
			}
		}
	}
}


