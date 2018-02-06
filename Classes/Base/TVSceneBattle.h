/************************************************************************/
/* @author zjl                                                                     */
/************************************************************************/
#ifndef _H_TV_BATTLE_SCENE_
#define _H_TV_BATTLE_SCENE_
#include "TVScene.h"
class TVSceneBattle: public TVScene
{
public:
	TVSceneBattle();
	~TVSceneBattle();

public:
	void OnEnterScene() override;

	void OnExitScene() override;

	void OnTickScene() override;

	TVSceneType curSceneType() override
	{
		return TVSceneType::_SceneBattle;
	}

private:

};

#endif