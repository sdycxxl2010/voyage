#ifndef _H_SDBARGIRLINTELLIGENCEDIALOG_
#define _H_SDBARGIRLINTELLIGENCEDIALOG_
#include "TVStaticDataBase.h"
class SDBargirlIntelligenceDialog : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDBargirlIntelligenceDialog";
    }
private:
      int id;
      int intelligence_id;
      int status;
      string dialog_ids;
public:
    int getid()
    {
      return id;
    }
    int getintelligence_id()
    {
      return intelligence_id;
    }
    int getstatus()
    {
      return status;
    }
    string& getdialog_ids()
    {
      return dialog_ids;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            intelligence_id = TVStaticDataHelper::GetIntValue(root, "intelligence_id");
            status = TVStaticDataHelper::GetIntValue(root, "status");
            dialog_ids = TVStaticDataHelper::GetStringValue(root, "dialog_ids");
     }

     static SDBargirlIntelligenceDialog* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDBargirlIntelligenceDialog*>(base);
     }

     static SDBargirlIntelligenceDialog* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDBargirlIntelligenceDialog*>(base);
     }

};
#endif