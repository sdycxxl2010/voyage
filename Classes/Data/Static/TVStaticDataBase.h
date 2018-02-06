#ifndef _H_STATIC_DATA_BASE_H
#define _H_STATIC_DATA_BASE_H
#include "TVBasic.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "cocostudio/DictionaryHelper.h"
#include "cocostudio/TextResUtils.h"
using namespace cocostudio;

class TVStaticDataBase
{
public:
	TVStaticDataBase();
	~TVStaticDataBase();
public:
	virtual void Load(const rapidjson::Value& root) = 0;

};

class TVStaticDataHelper
{
public:
	static int GetIntValue(const rapidjson::Value& root, const char* key, int def = 0)
	{
		int val = DictionaryHelper::getInstance()->getIntValue_json(root, key);
		return val;
	}

	static float GetFloatValue(const rapidjson::Value& root, const char* key, float def = 0.0f)
	{
		float val = DictionaryHelper::getInstance()->getFloatValue_json(root, key);
		return val;
	}

	static const char* GetStringValue(const rapidjson::Value& root, const char* key, const char* def = "")
	{
		const char* val = DictionaryHelper::getInstance()->getStringValue_json(root, key);
		return val;
	}
};

#endif