#include "LuaSailScene.h"
#include "SystemVar.h"
#include "ProtocolThread.h"
//#include "Maps.h"
#include "UIGuideSailManage.h"
#include "TVSingle.h"
#include "UIStoryLine.h"
LuaSailScene::LuaSailScene()
{
	m_SailRoad = new SailRoad;
}
LuaSailScene::~LuaSailScene()
{
	//delete m_SailRoad;
}

LuaSailScene* LuaSailScene::create(int id)
{
	auto ls = new LuaSailScene;
	if (ls && ls->initWithPhysics())
	{
		ls->m_TargetId = id;
		ls->init();
		ls->autorelease();
		return ls;
	}
	CC_SAFE_DELETE(ls);
	return nullptr;
}

bool LuaSailScene::init()
{
	bool pRet = false;
	do 
	{
		Scene::init();
		this->scheduleOnce(schedule_selector(LuaSailScene::initStaticData), 0);
		pRet = true;
	} while (0);
	
	return pRet;
}

void LuaSailScene::initStaticData(float f)
{
	SINGLE_HERO->shipGuideEnemy = true;//加入引导中的海盗船
	UIGuideSailManage*m_pMapUISail = UIGuideSailManage::create();
	this->addChild(m_pMapUISail);
	
 	/*m_SailRoad->initSailRoadData();
 	m_SailRoad->getSailRoad(SINGLE_HERO->m_iCityID, 17);
 	m_pMapUISail->setAutoSailRoad(m_SailRoad);*/
	
}
void LuaSailScene::onServerEvent(struct ProtobufCMessage *message,int msgType)
{

}
