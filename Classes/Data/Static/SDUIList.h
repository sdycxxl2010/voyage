#ifndef _H_SDUILIST_
#define _H_SDUILIST_
#include "TVStaticDataBase.h"
class SDUIList : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDUIList";
    }
private:
      int ID;
      string IDStr;
      string UIResPath;
      int Zorder;
      int IsPop;
      int BGM;
public:
    int getID()
    {
      return ID;
    }
    string& getIDStr()
    {
      return IDStr;
    }
    string& getUIResPath()
    {
      return UIResPath;
    }
    int getZorder()
    {
      return Zorder;
    }
    int getIsPop()
    {
      return IsPop;
    }
    int getBGM()
    {
      return BGM;
    }

    void Load(const rapidjson::Value& root) 
    {
            ID = TVStaticDataHelper::GetIntValue(root, "ID");
            IDStr = TVStaticDataHelper::GetStringValue(root, "IDStr");
            UIResPath = TVStaticDataHelper::GetStringValue(root, "UIResPath");
            Zorder = TVStaticDataHelper::GetIntValue(root, "Zorder");
            IsPop = TVStaticDataHelper::GetIntValue(root, "IsPop");
            BGM = TVStaticDataHelper::GetIntValue(root, "BGM");
     }

     static SDUIList* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDUIList*>(base);
     }

     static SDUIList* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDUIList*>(base);
     }

};
#endif