#ifndef _H_SDLOADINGANIMATION_
#define _H_SDLOADINGANIMATION_
#include "TVStaticDataBase.h"
class SDLoadingAnimation : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDLoadingAnimation";
    }
private:
      int id;
      int scene_id;
      string name_cn;
      string path;
public:
    int getid()
    {
      return id;
    }
    int getscene_id()
    {
      return scene_id;
    }
    string& getname_cn()
    {
      return name_cn;
    }
    string& getpath()
    {
      return path;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            scene_id = TVStaticDataHelper::GetIntValue(root, "scene_id");
            name_cn = TVStaticDataHelper::GetStringValue(root, "name_cn");
            path = TVStaticDataHelper::GetStringValue(root, "path");
     }

     static SDLoadingAnimation* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDLoadingAnimation*>(base);
     }

     static SDLoadingAnimation* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDLoadingAnimation*>(base);
     }

};
#endif