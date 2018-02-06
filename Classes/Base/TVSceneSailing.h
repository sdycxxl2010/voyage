/************************************************************************/
/* @author zjl                                                                     */
/************************************************************************/
#ifndef _H_TV_SAILING_SCENE_
#define _H_TV_SAILING_SCENE_
#include "TVScene.h"
class TVSceneSailing: public TVScene
{
public:
	TVSceneSailing();
	~TVSceneSailing();

public:
	void OnEnterScene() override;

	void OnExitScene() override;

	void OnTickScene() override;

	TVSceneType curSceneType() override
	{
		return TVSceneType::_SceneSailing;
	}

private:

};

#endif