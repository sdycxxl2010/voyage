/************************************************************************/
/* @author zjl                                                                     */
/************************************************************************/
#ifndef _H_TV_SCENE_PORTAL_
#define _H_TV_SCENE_PORTAL_
#include "TVScene.h"

class TVScenePortal : public TVScene
{
public:
	TVScenePortal();
	~TVScenePortal();

public:
	void OnEnterScene() override;

	void OnExitScene() override;

	void OnTickScene() override;

	TVSceneType curSceneType() override
	{
		return TVSceneType::_ScenePortal;
	}




};


#endif