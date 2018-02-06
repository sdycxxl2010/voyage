#ifndef _H_SDGUIDETASKSREWARD_
#define _H_SDGUIDETASKSREWARD_
#include "TVStaticDataBase.h"
class SDGuideTasksReward : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDGuideTasksReward";
    }
private:
      int id;
      int task_id;
      int coins;
      int item1type;
      int item1id;
      int item1amount;
      int item2type;
      int item2id;
      int item2amount;
      int v_tickets;
      int v_badges;
public:
    int getid()
    {
      return id;
    }
    int gettask_id()
    {
      return task_id;
    }
    int getcoins()
    {
      return coins;
    }
    int getitem1type()
    {
      return item1type;
    }
    int getitem1id()
    {
      return item1id;
    }
    int getitem1amount()
    {
      return item1amount;
    }
    int getitem2type()
    {
      return item2type;
    }
    int getitem2id()
    {
      return item2id;
    }
    int getitem2amount()
    {
      return item2amount;
    }
    int getv_tickets()
    {
      return v_tickets;
    }
    int getv_badges()
    {
      return v_badges;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            task_id = TVStaticDataHelper::GetIntValue(root, "task_id");
            coins = TVStaticDataHelper::GetIntValue(root, "coins");
            item1type = TVStaticDataHelper::GetIntValue(root, "item1type");
            item1id = TVStaticDataHelper::GetIntValue(root, "item1id");
            item1amount = TVStaticDataHelper::GetIntValue(root, "item1amount");
            item2type = TVStaticDataHelper::GetIntValue(root, "item2type");
            item2id = TVStaticDataHelper::GetIntValue(root, "item2id");
            item2amount = TVStaticDataHelper::GetIntValue(root, "item2amount");
            v_tickets = TVStaticDataHelper::GetIntValue(root, "v_tickets");
            v_badges = TVStaticDataHelper::GetIntValue(root, "v_badges");
     }

     static SDGuideTasksReward* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDGuideTasksReward*>(base);
     }

     static SDGuideTasksReward* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDGuideTasksReward*>(base);
     }

};
#endif