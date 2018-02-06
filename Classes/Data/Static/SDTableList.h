#ifndef _H_SDTABLELIST_
#define _H_SDTABLELIST_
#include "TVStaticDataBase.h"
class SDTableList : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDTableList";
    }
private:
      string TableName;
      int TableIdType;
      int InitLoad;
      int SelfParse;
public:
    string& getTableName()
    {
      return TableName;
    }
    int getTableIdType()
    {
      return TableIdType;
    }
    int getInitLoad()
    {
      return InitLoad;
    }
    int getSelfParse()
    {
      return SelfParse;
    }

    void Load(const rapidjson::Value& root) 
    {
            TableName = TVStaticDataHelper::GetStringValue(root, "TableName");
            TableIdType = TVStaticDataHelper::GetIntValue(root, "TableIdType");
            InitLoad = TVStaticDataHelper::GetIntValue(root, "InitLoad");
            SelfParse = TVStaticDataHelper::GetIntValue(root, "SelfParse");
     }

     static SDTableList* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDTableList*>(base);
     }

     static SDTableList* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDTableList*>(base);
     }

};
#endif