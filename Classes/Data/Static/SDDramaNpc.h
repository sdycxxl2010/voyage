#ifndef _H_SDDRAMANPC_
#define _H_SDDRAMANPC_
#include "TVStaticDataBase.h"
class SDDramaNpc : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDDramaNpc";
    }
private:
      int id;
      string name_en;
      string name_cn;
      string path_image;
      string path_icon;
      string path_sound;
public:
    int getid()
    {
      return id;
    }
    string& getname_en()
    {
      return name_en;
    }
    string& getname_cn()
    {
      return name_cn;
    }
    string& getpath_image()
    {
      return path_image;
    }
    string& getpath_icon()
    {
      return path_icon;
    }
    string& getpath_sound()
    {
      return path_sound;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            name_en = TVStaticDataHelper::GetStringValue(root, "name_en");
            name_cn = TVStaticDataHelper::GetStringValue(root, "name_cn");
            path_image = TVStaticDataHelper::GetStringValue(root, "path_image");
            path_icon = TVStaticDataHelper::GetStringValue(root, "path_icon");
            path_sound = TVStaticDataHelper::GetStringValue(root, "path_sound");
     }

     static SDDramaNpc* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDDramaNpc*>(base);
     }

     static SDDramaNpc* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDDramaNpc*>(base);
     }

};
#endif