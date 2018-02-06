#ifndef _H_SDGUIDETASKSCONDITION_
#define _H_SDGUIDETASKSCONDITION_
#include "TVStaticDataBase.h"
class SDGuideTasksCondition : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDGuideTasksCondition";
    }
private:
      int id;
      int task_id;
      string condition_desc;
      int type;
      int p1;
      int p2;
      int p3;
public:
    int getid()
    {
      return id;
    }
    int gettask_id()
    {
      return task_id;
    }
    string& getcondition_desc()
    {
      return condition_desc;
    }
    int gettype()
    {
      return type;
    }
    int getp1()
    {
      return p1;
    }
    int getp2()
    {
      return p2;
    }
    int getp3()
    {
      return p3;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            task_id = TVStaticDataHelper::GetIntValue(root, "task_id");
            condition_desc = TVStaticDataHelper::GetStringValue(root, "condition_desc");
            type = TVStaticDataHelper::GetIntValue(root, "type");
            p1 = TVStaticDataHelper::GetIntValue(root, "p1");
            p2 = TVStaticDataHelper::GetIntValue(root, "p2");
            p3 = TVStaticDataHelper::GetIntValue(root, "p3");
     }

     static SDGuideTasksCondition* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDGuideTasksCondition*>(base);
     }

     static SDGuideTasksCondition* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDGuideTasksCondition*>(base);
     }

};
#endif