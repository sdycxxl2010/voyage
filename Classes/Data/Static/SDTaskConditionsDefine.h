#ifndef _H_SDTASKCONDITIONSDEFINE_
#define _H_SDTASKCONDITIONSDEFINE_
#include "TVStaticDataBase.h"
class SDTaskConditionsDefine : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDTaskConditionsDefine";
    }
private:
      int id;
      int type;
      string condition_desc;
      string p1;
      string p2;
      string p3;
public:
    int getid()
    {
      return id;
    }
    int gettype()
    {
      return type;
    }
    string& getcondition_desc()
    {
      return condition_desc;
    }
    string& getp1()
    {
      return p1;
    }
    string& getp2()
    {
      return p2;
    }
    string& getp3()
    {
      return p3;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            type = TVStaticDataHelper::GetIntValue(root, "type");
            condition_desc = TVStaticDataHelper::GetStringValue(root, "condition_desc");
            p1 = TVStaticDataHelper::GetStringValue(root, "p1");
            p2 = TVStaticDataHelper::GetStringValue(root, "p2");
            p3 = TVStaticDataHelper::GetStringValue(root, "p3");
     }

     static SDTaskConditionsDefine* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDTaskConditionsDefine*>(base);
     }

     static SDTaskConditionsDefine* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDTaskConditionsDefine*>(base);
     }

};
#endif