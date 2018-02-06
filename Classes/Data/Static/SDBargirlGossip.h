#ifndef _H_SDBARGIRLGOSSIP_
#define _H_SDBARGIRLGOSSIP_
#include "TVStaticDataBase.h"
class SDBargirlGossip : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDBargirlGossip";
    }
private:
      int id;
      string title_cn;
      string title_en;
      int favorability_require;
      int available;
public:
    int getid()
    {
      return id;
    }
    string& gettitle_cn()
    {
      return title_cn;
    }
    string& gettitle_en()
    {
      return title_en;
    }
    int getfavorability_require()
    {
      return favorability_require;
    }
    int getavailable()
    {
      return available;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            title_cn = TVStaticDataHelper::GetStringValue(root, "title_cn");
            title_en = TVStaticDataHelper::GetStringValue(root, "title_en");
            favorability_require = TVStaticDataHelper::GetIntValue(root, "favorability_require");
            available = TVStaticDataHelper::GetIntValue(root, "available");
     }

     static SDBargirlGossip* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDBargirlGossip*>(base);
     }

     static SDBargirlGossip* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDBargirlGossip*>(base);
     }

};
#endif