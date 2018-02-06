#include "JsonReader.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "cocostudio/DictionaryHelper.h"

USING_NS_CC;
using namespace cocostudio;
CJsonReader::CJsonReader()
{
}

CJsonReader::~CJsonReader()
{
}

void CJsonReader::readerJsonByName(std::string fileName)
{
	std::string jsonpath;
 	rapidjson::Document root;
 	jsonpath = cocos2d::CCFileUtils::getInstance()->fullPathForFilename(fileName);
 	//size_t pos = jsonpath.find_last_of('/');
 	//std::string strFilePath = jsonpath.substr(0,pos+1);
 	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonpath);
 	root.Parse<0>(contentStr.c_str());

 	if (root.HasParseError())
 	{
 		CCLOG("GetParseError %s \n",root.GetParseError());
 	}else
 	{
 		CCLOG("--%s",contentStr.c_str());
 	}
	StringBuffer buffer;
	Writer<StringBuffer> w(buffer);
	root.Accept(w);
	CCLOG("--==%s",buffer.GetString());
	//saveToJson(root,"");
	
}

void CJsonReader::saveToJson(rapidjson::Document value,std::string fileName)
{
	////using namespace rapidjson;
	//StringBuffer buffer;
	//Writer<StringBuffer> w(buffer);
	//value.Accept(w);
	//CCLOG("--%s",buffer.GetString());
}