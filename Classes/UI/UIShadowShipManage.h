/*
 * NearbyShipManager.h
 *
 *  Created on: Dec 16, 2015
 *      Author: xijing
 */

#ifndef NEARBYSHIPMANAGER_H_
#define NEARBYSHIPMANAGER_H_

#include "cocos2d.h"
#include "ProtocolThread.h"

USING_NS_CC;
struct _NearbyPlayer;
class UISailManage;
class UIShadowShip;

struct ShipRoadLines{
	Vec2 start;
	Vec2 end;
	int millisec;
};

struct NearByPlayerItem{
	int sid;
	int cid;
	int lv;
	int nation;
	std::string name;
	std::vector<struct ShipRoadLines> points;
	int64_t roadTime;
};

class UIShadowShipManage{
public:
	UIShadowShipManage(UISailManage*mapLayer);
	~UIShadowShipManage();
	void update(float f);
	void reportShipPosition(struct _NearbyPlayer**array,int number);
	std::vector<UIShadowShip*> getGhostShips(){ return m_ghostShips;};

	int isShipInVector(std::vector<struct NearByPlayerItem> items, int cid);
private:
	std::vector<struct NearByPlayerItem> m_movingShips;
	std::vector<NearbyPlayer> m_lastPosition;
	UISailManage* m_mapLayer;
	int64_t m_lastUpdateMilliSec;
	//int64_t m_lastGetPostionMilliSec;
	std::vector<UIShadowShip*> m_ghostShips;
};

#endif /* NEARBYSHIPMANAGER_H_ */
