#ifndef _H_SDAUDIOEFFECT_
#define _H_SDAUDIOEFFECT_
#include "TVStaticDataBase.h"
class SDAudioEffect : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDAudioEffect";
    }
private:
      int ID;
      string AudioEffectPath;
      float Volume;
public:
    int getID()
    {
      return ID;
    }
    string& getAudioEffectPath()
    {
      return AudioEffectPath;
    }
    float getVolume()
    {
      return Volume;
    }

    void Load(const rapidjson::Value& root) 
    {
            ID = TVStaticDataHelper::GetIntValue(root, "ID");
            AudioEffectPath = TVStaticDataHelper::GetStringValue(root, "AudioEffectPath");
            Volume = TVStaticDataHelper::GetFloatValue(root, "Volume");
     }

     static SDAudioEffect* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDAudioEffect*>(base);
     }

     static SDAudioEffect* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDAudioEffect*>(base);
     }

};
#endif