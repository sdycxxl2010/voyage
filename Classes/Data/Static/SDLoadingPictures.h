#ifndef _H_SDLOADINGPICTURES_
#define _H_SDLOADINGPICTURES_
#include "TVStaticDataBase.h"
class SDLoadingPictures : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDLoadingPictures";
    }
private:
      int id;
      int scene_id;
      string name_cn;
      string path;
      int use_tips;
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
    int getuse_tips()
    {
      return use_tips;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            scene_id = TVStaticDataHelper::GetIntValue(root, "scene_id");
            name_cn = TVStaticDataHelper::GetStringValue(root, "name_cn");
            path = TVStaticDataHelper::GetStringValue(root, "path");
            use_tips = TVStaticDataHelper::GetIntValue(root, "use_tips");
     }

     static SDLoadingPictures* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDLoadingPictures*>(base);
     }

     static SDLoadingPictures* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDLoadingPictures*>(base);
     }

};
#endif