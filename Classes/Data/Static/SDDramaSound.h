#ifndef _H_SDDRAMASOUND_
#define _H_SDDRAMASOUND_
#include "TVStaticDataBase.h"
class SDDramaSound : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDDramaSound";
    }
private:
      int id;
      string name_cn;
      string path;
public:
    int getid()
    {
      return id;
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
            name_cn = TVStaticDataHelper::GetStringValue(root, "name_cn");
            path = TVStaticDataHelper::GetStringValue(root, "path");
     }

     static SDDramaSound* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDDramaSound*>(base);
     }

     static SDDramaSound* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDDramaSound*>(base);
     }

};
#endif