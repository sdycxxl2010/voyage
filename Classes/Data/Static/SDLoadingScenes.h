#ifndef _H_SDLOADINGSCENES_
#define _H_SDLOADINGSCENES_
#include "TVStaticDataBase.h"
class SDLoadingScenes : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDLoadingScenes";
    }
private:
      int id;
      string name_cn;
      int type;
public:
    int getid()
    {
      return id;
    }
    string& getname_cn()
    {
      return name_cn;
    }
    int gettype()
    {
      return type;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            name_cn = TVStaticDataHelper::GetStringValue(root, "name_cn");
            type = TVStaticDataHelper::GetIntValue(root, "type");
     }

     static SDLoadingScenes* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDLoadingScenes*>(base);
     }

     static SDLoadingScenes* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDLoadingScenes*>(base);
     }

};
#endif