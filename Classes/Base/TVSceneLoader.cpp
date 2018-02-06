#include "TVSceneLoader.h"
#include "UISceneLoading.h"
#include "PicResources.h"

static TVSceneLoader* _instance = nullptr;

TVSceneLoader::TVSceneLoader()
{

}

TVSceneLoader::~TVSceneLoader()
{

}

TVSceneLoader* TVSceneLoader::getInstance()
{
	if (!_instance)
	{
		_instance = new TVSceneLoader();
	}
	return _instance;
}

void TVSceneLoader::changeToSceneByTag(SCENE_TAG tag)
{
	Scene* scene = Scene::createWithPhysics();
	if (tag == MAP_TAG)
	{
#if VOAGE_TEST_1
		scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#else
		scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
#endif
	}

	if (scene)
	{
		vector<string> *path;
		switch(tag)
		{
		case BANK_TAG:
			path = &res_bank_720;
			break;
		case WHARF_TAG:
		case WHARF_GUIDE_TAG:
			path = &res_wharf_720;
			break;
		case DOCK_GUIDE_TAG:
		case DOCK_TAG:
			path = &res_shipyard_720;
			break;
		case EXCHANG_TAG:
			path = &res_exchange_720;
			break;
		case PUP_GUIDE_TAG:
		case PUPL_TAG:
			path = &res_bar_720;
			break;
		case CENTER_TAG:
			path = &res_player_720;
			break;
		case MAP_SAILING_TAG:
		case MAP_TAG:
			path = &res_sailing_720;
			break;
		case PALACE_TAG:
			path = &res_palace_720;
			break;
		case MAIN_TAG:
			path = &res_city;
			break;
		case MAINGUID_TAG:
			path = &res_city;
			break;
		case PALACEGUID_TAG:
			path = &res_palace_720;
			break;
		case EXCHANGEGUID_TAG:
			path = &res_exchange_720;
			break;
		case SKILLGUIDE_TAG:
			path = &res_player_720;
			break;
		case COUNTRY_WAR_TAG:
			path = &res_city;
			break;
		case BATTLE_TAG:
			path = &res_city;
			break;
		default:
			vector<string> def ={};
			path = &def;
			break;
		}
		/* loading common is not a good idea yet, too many png pictures.*/
		/*
		vector<string> total;
		total.reserve(path->size() + res_common.size());
		total.insert(total.end(), path->begin(), path->end());
		total.insert(total.end(), res_common.begin(), res_common.end());
		*/
		UISceneLoading *layer = UISceneLoading::create(*path, tag);
		scene->addChild(layer, 1);
		Director::getInstance()->replaceScene(scene);
	}
}

void TVSceneLoader::release()
{
	delete _instance;
}
