#ifndef _H_SDBARGIRGOSSIPPROBABILITY_
#define _H_SDBARGIRGOSSIPPROBABILITY_
#include "TVStaticDataBase.h"
class SDBargirGossipProbability : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDBargirGossipProbability";
    }
private:
      int id;
      string range;
      int gossip_1;
      int gossip_2;
      int gossip_3;
      int gossip_4;
public:
    int getid()
    {
      return id;
    }
    string& getrange()
    {
      return range;
    }
    int getgossip_1()
    {
      return gossip_1;
    }
    int getgossip_2()
    {
      return gossip_2;
    }
    int getgossip_3()
    {
      return gossip_3;
    }
    int getgossip_4()
    {
      return gossip_4;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            range = TVStaticDataHelper::GetStringValue(root, "range");
            gossip_1 = TVStaticDataHelper::GetIntValue(root, "gossip_1");
            gossip_2 = TVStaticDataHelper::GetIntValue(root, "gossip_2");
            gossip_3 = TVStaticDataHelper::GetIntValue(root, "gossip_3");
            gossip_4 = TVStaticDataHelper::GetIntValue(root, "gossip_4");
     }

     static SDBargirGossipProbability* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDBargirGossipProbability*>(base);
     }

     static SDBargirGossipProbability* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDBargirGossipProbability*>(base);
     }

};
#endif