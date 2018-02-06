#ifndef _H_SDBARGIRLINTELLIGENCE_
#define _H_SDBARGIRLINTELLIGENCE_
#include "TVStaticDataBase.h"
class SDBargirlIntelligence : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDBargirlIntelligence";
    }
private:
      int id;
      string title_cn;
      string title_en;
      int price;
      int available;
public:
    int getid()
    {
      return id;
    }
    string& gettitle_cn()
    {
      return title_cn;
    }
    string& gettitle_en()
    {
      return title_en;
    }
    int getprice()
    {
      return price;
    }
    int getavailable()
    {
      return available;
    }

    void Load(const rapidjson::Value& root) 
    {
            id = TVStaticDataHelper::GetIntValue(root, "id");
            title_cn = TVStaticDataHelper::GetStringValue(root, "title_cn");
            title_en = TVStaticDataHelper::GetStringValue(root, "title_en");
            price = TVStaticDataHelper::GetIntValue(root, "price");
            available = TVStaticDataHelper::GetIntValue(root, "available");
     }

     static SDBargirlIntelligence* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDBargirlIntelligence*>(base);
     }

     static SDBargirlIntelligence* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDBargirlIntelligence*>(base);
     }

};
#endif