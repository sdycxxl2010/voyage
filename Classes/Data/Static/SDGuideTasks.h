#ifndef _H_SDGUIDETASKS_
#define _H_SDGUIDETASKS_
#include "TVStaticDataBase.h"
class SDGuideTasks : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDGuideTasks";
    }
private:
      int id;
      string name_ids;
      string title_ids;
      string tip_ids;
      int start_flag;
      int next_id;
      int visible;
public:
    int getid()
    {
      return id;
    }
    string& getname_ids()
    {
      return name_ids;
    }
    string& gettitle_ids()
    {
      return title_ids;
    }
    string& gettip_ids()
    {
      return tip_ids;
    }
    int getstart_flag()
    {
      return start_flag;
    }
    int getnext_id()
    {
      return next_id;
    }
    int getvisible()
    {
      return visible;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            name_ids = TVStaticDataHelper::GetStringValue(root, "name_ids");
            title_ids = TVStaticDataHelper::GetStringValue(root, "title_ids");
            tip_ids = TVStaticDataHelper::GetStringValue(root, "tip_ids");
            start_flag = TVStaticDataHelper::GetIntValue(root, "start_flag");
            next_id = TVStaticDataHelper::GetIntValue(root, "next_id");
            visible = TVStaticDataHelper::GetIntValue(root, "visible");
     }

     static SDGuideTasks* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDGuideTasks*>(base);
     }

     static SDGuideTasks* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDGuideTasks*>(base);
     }

};
#endif