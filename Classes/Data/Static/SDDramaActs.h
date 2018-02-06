#ifndef _H_SDDRAMAACTS_
#define _H_SDDRAMAACTS_
#include "TVStaticDataBase.h"
class SDDramaActs : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDDramaActs";
    }
private:
      int id;
      string note;
      string relate_id;
      string relate_num;
      string content_en;
      string content_cn;
public:
    int getid()
    {
      return id;
    }
    string& getnote()
    {
      return note;
    }
    string& getrelate_id()
    {
      return relate_id;
    }
    string& getrelate_num()
    {
      return relate_num;
    }
    string& getcontent_en()
    {
      return content_en;
    }
    string& getcontent_cn()
    {
      return content_cn;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            note = TVStaticDataHelper::GetStringValue(root, "note");
            relate_id = TVStaticDataHelper::GetStringValue(root, "relate_id");
            relate_num = TVStaticDataHelper::GetStringValue(root, "relate_num");
            content_en = TVStaticDataHelper::GetStringValue(root, "content_en");
            content_cn = TVStaticDataHelper::GetStringValue(root, "content_cn");
     }

     static SDDramaActs* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDDramaActs*>(base);
     }

     static SDDramaActs* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDDramaActs*>(base);
     }

};
#endif