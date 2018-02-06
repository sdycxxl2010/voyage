#ifndef _H_STATIC_TABLE_LIST_H
#define _H_STATIC_TABLE_LIST_H
#include "TVBasic.h"
#include "TVStaticDataManager.h"

#include "SDUIList.h"
#include "SDTableList.h"
#include "SDAudioBGM.h"
#include "SDAudioEffect.h"
#include "SDLangDict.h"
#include "SDLangDesc.h"
#include "SDBargirlGossip.h"
#include "SDBargirGossipProbability.h"
#include "SDBargirlGossipDialog.h"
#include "SDBargirlIntelligence.h"
#include "SDBargirlIntelligenceDialog.h"
#include "SDDramaActs.h"
#include "SDDramaNpc.h"
#include "SDDramaBackground.h"
#include "SDDramaBgm.h"
#include "SDDramaSound.h"
#include "SDGuideTasks.h"
#include "SDGuideTasksCondition.h"
#include "SDGuideTasksReward.h"
#include "SDLoadingScenes.h"
#include "SDLoadingPictures.h"
#include "SDLoadingTips.h"
#include "SDLoadingAnimation.h"
#include "SDTaskConditionsDefine.h"

class  TVSDHeaderHelper
{
public:
	static TVStaticDataBase* CreateTemplateIns(const char* tStr)
	{
      if (strcmp(tStr, SDTableList::ConfigName()) == 0)
      {
        return new SDTableList();
      }
      else if (strcmp(tStr, SDUIList::ConfigName()) == 0)
      {
       return new SDUIList();
      }      else if (strcmp(tStr, SDTableList::ConfigName()) == 0)
      {
       return new SDTableList();
      }      else if (strcmp(tStr, SDAudioBGM::ConfigName()) == 0)
      {
       return new SDAudioBGM();
      }      else if (strcmp(tStr, SDAudioEffect::ConfigName()) == 0)
      {
       return new SDAudioEffect();
      }      else if (strcmp(tStr, SDLangDict::ConfigName()) == 0)
      {
       return new SDLangDict();
      }      else if (strcmp(tStr, SDLangDesc::ConfigName()) == 0)
      {
       return new SDLangDesc();
      }      else if (strcmp(tStr, SDBargirlGossip::ConfigName()) == 0)
      {
       return new SDBargirlGossip();
      }      else if (strcmp(tStr, SDBargirGossipProbability::ConfigName()) == 0)
      {
       return new SDBargirGossipProbability();
      }      else if (strcmp(tStr, SDBargirlGossipDialog::ConfigName()) == 0)
      {
       return new SDBargirlGossipDialog();
      }      else if (strcmp(tStr, SDBargirlIntelligence::ConfigName()) == 0)
      {
       return new SDBargirlIntelligence();
      }      else if (strcmp(tStr, SDBargirlIntelligenceDialog::ConfigName()) == 0)
      {
       return new SDBargirlIntelligenceDialog();
      }      else if (strcmp(tStr, SDDramaActs::ConfigName()) == 0)
      {
       return new SDDramaActs();
      }      else if (strcmp(tStr, SDDramaNpc::ConfigName()) == 0)
      {
       return new SDDramaNpc();
      }      else if (strcmp(tStr, SDDramaBackground::ConfigName()) == 0)
      {
       return new SDDramaBackground();
      }      else if (strcmp(tStr, SDDramaBgm::ConfigName()) == 0)
      {
       return new SDDramaBgm();
      }      else if (strcmp(tStr, SDDramaSound::ConfigName()) == 0)
      {
       return new SDDramaSound();
      }      else if (strcmp(tStr, SDGuideTasks::ConfigName()) == 0)
      {
       return new SDGuideTasks();
      }      else if (strcmp(tStr, SDGuideTasksCondition::ConfigName()) == 0)
      {
       return new SDGuideTasksCondition();
      }      else if (strcmp(tStr, SDGuideTasksReward::ConfigName()) == 0)
      {
       return new SDGuideTasksReward();
      }      else if (strcmp(tStr, SDLoadingScenes::ConfigName()) == 0)
      {
       return new SDLoadingScenes();
      }      else if (strcmp(tStr, SDLoadingPictures::ConfigName()) == 0)
      {
       return new SDLoadingPictures();
      }      else if (strcmp(tStr, SDLoadingTips::ConfigName()) == 0)
      {
       return new SDLoadingTips();
      }      else if (strcmp(tStr, SDLoadingAnimation::ConfigName()) == 0)
      {
       return new SDLoadingAnimation();
      }      else if (strcmp(tStr, SDTaskConditionsDefine::ConfigName()) == 0)
      {
       return new SDTaskConditionsDefine();
      }
      return 0;
    }
};
#endif
    