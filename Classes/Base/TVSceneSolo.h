/************************************************************************/
/* @author zjl                                                                     */
/************************************************************************/
#ifndef _H_TV_SOLO_SCENE_
#define _H_TV_SOLO_SCENE_
#include "TVScene.h"
class TVSceneSolo: public TVScene
{
public:
	TVSceneSolo();
	~TVSceneSolo();

public:
	void OnEnterScene() override;

	void OnExitScene() override;

	void OnTickScene() override;

	TVSceneType curSceneType() override
	{
		return TVSceneType::_SceneSolo;
	}

private:

};

#endif