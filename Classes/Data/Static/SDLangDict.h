#ifndef _H_SDLANGDICT_
#define _H_SDLANGDICT_
#include "TVStaticDataBase.h"
class SDLangDict : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDLangDict";
    }
private:
      string ID;
      string text_en_US;
      string text_zh_CN;
      string text_zh_HK;
      int type;
      string name;
public:
    string& getID()
    {
      return ID;
    }
    string& gettext_en_US()
    {
      return text_en_US;
    }
    string& gettext_zh_CN()
    {
      return text_zh_CN;
    }
    string& gettext_zh_HK()
    {
      return text_zh_HK;
    }
    int gettype()
    {
      return type;
    }
    string& getname()
    {
      return name;
    }

    void Load(const rapidjson::Value& root) 
    {
            ID = TVStaticDataHelper::GetStringValue(root, "ID");
            text_en_US = TVStaticDataHelper::GetStringValue(root, "text_en_US");
            text_zh_CN = TVStaticDataHelper::GetStringValue(root, "text_zh_CN");
            text_zh_HK = TVStaticDataHelper::GetStringValue(root, "text_zh_HK");
            type = TVStaticDataHelper::GetIntValue(root, "type");
            name = TVStaticDataHelper::GetStringValue(root, "name");
     }

     static SDLangDict* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDLangDict*>(base);
     }

     static SDLangDict* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDLangDict*>(base);
     }

};
#endif