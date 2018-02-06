#ifndef _H_SDLANGDESC_
#define _H_SDLANGDESC_
#include "TVStaticDataBase.h"
class SDLangDesc : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDLangDesc";
    }
private:
      string ID;
      string tip_en_US;
      string tip_zh_CN;
      string tips_zh_HK;
public:
    string& getID()
    {
      return ID;
    }
    string& gettip_en_US()
    {
      return tip_en_US;
    }
    string& gettip_zh_CN()
    {
      return tip_zh_CN;
    }
    string& gettips_zh_HK()
    {
      return tips_zh_HK;
    }

    void Load(const rapidjson::Value& root) 
    {
            ID = TVStaticDataHelper::GetStringValue(root, "ID");
            tip_en_US = TVStaticDataHelper::GetStringValue(root, "tip_en_US");
            tip_zh_CN = TVStaticDataHelper::GetStringValue(root, "tip_zh_CN");
            tips_zh_HK = TVStaticDataHelper::GetStringValue(root, "tips_zh_HK");
     }

     static SDLangDesc* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDLangDesc*>(base);
     }

     static SDLangDesc* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDLangDesc*>(base);
     }

};
#endif