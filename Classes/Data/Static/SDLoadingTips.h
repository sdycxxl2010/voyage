#ifndef _H_SDLOADINGTIPS_
#define _H_SDLOADINGTIPS_
#include "TVStaticDataBase.h"
class SDLoadingTips : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDLoadingTips";
    }
private:
      int id;
      int scene_id;
      string tip_en;
      string tip_cn;
public:
    int getid()
    {
      return id;
    }
    int getscene_id()
    {
      return scene_id;
    }
    string& gettip_en()
    {
      return tip_en;
    }
    string& gettip_cn()
    {
      return tip_cn;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            scene_id = TVStaticDataHelper::GetIntValue(root, "scene_id");
            tip_en = TVStaticDataHelper::GetStringValue(root, "tip_en");
            tip_cn = TVStaticDataHelper::GetStringValue(root, "tip_cn");
     }

     static SDLoadingTips* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDLoadingTips*>(base);
     }

     static SDLoadingTips* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDLoadingTips*>(base);
     }

};
#endif