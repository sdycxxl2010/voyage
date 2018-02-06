#ifndef _H_SDDRAMABGM_
#define _H_SDDRAMABGM_
#include "TVStaticDataBase.h"
class SDDramaBgm : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDDramaBgm";
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

     static SDDramaBgm* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDDramaBgm*>(base);
     }

     static SDDramaBgm* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDDramaBgm*>(base);
     }

};
#endif