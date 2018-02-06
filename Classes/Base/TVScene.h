/************************************************************************/
/* @author zjl                                                                     */
/************************************************************************/
#ifndef _H_TVGAME_SCENE_
#define _H_TVGAME_SCENE_
#include "TVBasic.h"

enum TVSceneType
{
	_SceneUnkown = 0,
	_SceneLogin,
	_ScenePortal,
	_SceneSailing,
	_SceneBattle,
	_SceneSolo,
};


class TVScene
{
public:
	TVScene();
	~TVScene();

public:
	virtual void OnEnterScene();

	virtual void OnExitScene();

	virtual void OnTickScene();

public:
	virtual TVSceneType curSceneType() = 0;

public:
	Scene* rootNode();

public:
	static Node* curSceneRootNode();

private:
	Scene* m_pRootScene;

};



#endif