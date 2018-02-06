#ifndef _H_TVGAME_SCENE_OBJECT_
#define _H_TVGAME_SCENE_OBJECT_
#include "TVBasic.h"

class TVGameObject
{
public:
	TVGameObject();
	~TVGameObject();

public:
	virtual void Init()
	{

	}

	virtual void Tick(float dt)
	{

	}

	virtual void Destroy()
	{

	}

};



#endif // !_H_TVGAME_SCENE_OBJECT_
