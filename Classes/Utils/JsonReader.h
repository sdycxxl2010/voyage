#ifndef __JSON_READER_H__
#define __JSON_READER_H__
#include "cocos2d.h"
#include "json/document.h"
using namespace rapidjson;
class CJsonReader
{
public:
	CJsonReader();
	~CJsonReader();
	static void readerJsonByName(std::string fileName);
	static void saveToJson(rapidjson::Document value,std::string fileName);
private:

};





#endif  //json reader 