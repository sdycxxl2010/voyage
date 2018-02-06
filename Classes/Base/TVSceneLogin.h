/************************************************************************/
/* @author zjl                                                                     */
/************************************************************************/
#ifndef _H_TVLOGIN_SCENE_
#define _H_TVLOGIN_SCENE_
#include "TVScene.h"
class TVSceneLogin: public TVScene
{
public:
	TVSceneLogin();
	~TVSceneLogin();

public:
	void OnEnterScene() override;

	void OnExitScene() override;

	void OnTickScene() override;

	TVSceneType curSceneType() override
	{
		return TVSceneType::_SceneLogin;
	}

private:

};

#endif