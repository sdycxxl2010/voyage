/************************************************************************/
/* @author zjl                                                                     */
/************************************************************************/
#ifndef _H_TVGAME_SCENE_MGR_
#define _H_TVGAME_SCENE_MGR_
#include "TVBasic.h"
#include "TVScene.h"
#include "TVGameObject.h"


class TVSceneManager : public SingletonPtr<TVSceneManager>
{
	friend class SingletonPtr<TVSceneManager>;
public:
	TVSceneManager();
	~TVSceneManager();

protected:
	void     Init();
	void	 Tick(float dt);
	void	 Destroy();

protected:
	void	 TickScenes(float dt);
	void     TickObjects(float dt);

public:
	void update(float dt);

public:
	TVScene*    curGameScene()
	{
		return m_pCurScene;
	}

	void        setNextScene(TVSceneType type);

private:
	TVScene* m_pCurScene;
	TVScene* m_pNextScene;
	TVScene* m_pPreScene;

	map<TVSceneType, TVScene*> m_mSceneMap;

	vector<TVGameObject*> m_vGameObjects;
};



#endif