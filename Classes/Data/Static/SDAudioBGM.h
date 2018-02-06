#ifndef _H_SDAUDIOBGM_
#define _H_SDAUDIOBGM_
#include "TVStaticDataBase.h"
class SDAudioBGM : public TVStaticDataBase 
{
public: 
    static const char* ConfigName()
    {
       return "SDAudioBGM";
    }
private:
      int ID;
      string AudioBGMPath;
      float Volume;
public:
    int getID()
    {
      return ID;
    }
    string& getAudioBGMPath()
    {
      return AudioBGMPath;
    }
    float getVolume()
    {
      return Volume;
    }

    void Load(const rapidjson::Value& root) 
    {
            ID = TVStaticDataHelper::GetIntValue(root, "ID");
            AudioBGMPath = TVStaticDataHelper::GetStringValue(root, "AudioBGMPath");
            Volume = TVStaticDataHelper::GetFloatValue(root, "Volume");
     }

     static SDAudioBGM* GetElement(string& strElement)
     {
           TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByStrId(ConfigName(), strElement.c_str());
           return dynamic_cast<SDAudioBGM*>(base);
     }

     static SDAudioBGM* GetElement(int nElement)
     {
         TVStaticDataBase* base = TVStaticDataManager::getInstance()->GetElementByIntId(ConfigName(), nElement);
         return dynamic_cast<SDAudioBGM*>(base);
     }

};
#endif