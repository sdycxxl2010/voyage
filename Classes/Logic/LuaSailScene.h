#ifndef __LUA_SAIL_SCENE_H__
#define __LUA_SAIL_SCENE_H__
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "login.pb-c.h"
#include "SailRoad.h"
#include "UIBasicLayer.h"
USING_NS_CC;
class LuaSailScene : public Scene
{
public:
	LuaSailScene();
	~LuaSailScene();
	static LuaSailScene* create(int id);
	bool init();
	void onServerEvent(struct ProtobufCMessage *message,int msgType);
	

	void initStaticData(float f);
private:
	SailRoad* m_SailRoad;
	int m_TargetId;
	UIBasicLayer*		m_pMapUISail;
};


#endif
