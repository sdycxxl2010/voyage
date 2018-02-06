#ifndef _H_SDBARGIRLGOSSIPDIALOG_
#define _H_SDBARGIRLGOSSIPDIALOG_
#include "TVStaticDataBase.h"
class SDBargirlGossipDialog : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDBargirlGossipDialog";
    }
private:
      int id;
      int gossip_id;
      string dialog_ids;
public:
    int getid()
    {
      return id;
    }
    int getgossip_id()
    {
      return gossip_id;
    }
    string& getdialog_ids()
    {
      return dialog_ids;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            gossip_id = TVStaticDataHelper::GetIntValue(root, "gossip_id");
            dialog_ids = TVStaticDataHelper::GetStringValue(root, "dialog_ids");
     }

     static SDBargirlGossipDialog* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDBargirlGossipDialog*>(base);
     }

     static SDBargirlGossipDialog* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDBargirlGossipDialog*>(base);
     }

};
#endif