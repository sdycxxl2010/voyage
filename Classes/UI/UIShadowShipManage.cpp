/*
 * NearbyShipManager.cpp
 *
 *  Created on: Dec 16, 2015
 *      Author: xijing
 */


#include "UIShadowShipManage.h"
#include "UISailManage.h"
#include "UIShadowShip.h"
#include "TVSceneLoader.h"
#include "Utils.h"
#include "AppHelper.h"

UIShadowShipManage::UIShadowShipManage(UISailManage*mapLayer){
	m_mapLayer = mapLayer;
	m_lastUpdateMilliSec = 0;
	//m_lastGetPostionMilliSec = 0;
}

UIShadowShipManage::~UIShadowShipManage(){

}

void UIShadowShipManage::update(float f){
	long a = Utils::gettime();
	timeval timeStruc;
	gettimeofday(&timeStruc,nullptr);
	int64_t sec1 = ((int64_t)timeStruc.tv_sec) * 1000;
	int64_t sec2 = timeStruc.tv_usec / 1000;
	int64_t curTime = sec1 + sec2;
	m_lastUpdateMilliSec = curTime;
	auto map = Director::getInstance()->getRunningScene()->getChildByTag(MAP_TAG + 100);
	if (m_ghostShips.size() > 0)
	{
		for (auto ship : m_ghostShips)
		{
			Point map_pos = m_mapLayer->getPosition();
			Point ship_pos = ship->getPosition();
// 			//x direct
// 			if (true)//std::abs(x) > std::abs(x_speed) - 0.1)
// 			{
// 
// 				if (ship->getIsCenterX())
// 				{
// 					map_pos.x -= x_speed;// * (x/std::abs(x));
// 				}
// 				else
// 				{
// 					float diff = ship_pos.x + map_pos.x;
// 					if (diff < m_WinSize.width / 2 && std::abs(map_pos.x) > 100.0)
// 					{
// 						map_pos.x += (m_WinSize.width / 2 - diff);
// 						m_bIsCenterX = true;
// 					}
// 					if (diff > m_WinSize.width / 2 && std::abs(map_pos.x) < 10.0)
// 					{
// 						map_pos.x -= (diff - m_WinSize.width / 2);
// 						m_bIsCenterX = true;
// 					}
// 				}
// 				ship_pos.x += x_speed;//* (x/std::abs(x));
// 			}
// 			//y direct
// 			if (true)//std::abs(y) > std::abs(y_speed) - 0.1)
// 			{
// 
// 				if (m_bIsCenterY)
// 				{
// 					map_pos.y -= y_speed;//* (y/std::abs(y));
// 				}
// 				else
// 				{
// 					float diff = ship_pos.y + map_pos.y;
// 					if (diff < m_WinSize.height / 2 && std::abs(map_pos.y) > 100.0)
// 					{
// 						map_pos.y += (m_WinSize.height / 2 - diff);
// 						m_bIsCenterY = true;
// 					}
// 					if (diff > m_WinSize.height / 2 && std::abs(map_pos.y) < 10.0)
// 					{
// 						map_pos.y -= (diff - m_WinSize.height / 2);
// 						m_bIsCenterY = true;
// 					}
// 				}
// 				ship_pos.y += y_speed;// * (y/std::abs(y));
// 			}
// 			else
// 			{
// 				ship_pos.y = m_TargetPos.y;
// 				if (m_bIsCenterY)
// 				{
// 					//map_pos.y -= y;
// 				}
// 			}
// 			if (ship_pos.x < 0.1 || ship_pos.y < 0.1 || ship_pos.x > MAP_HEIGH * MAP_CELL_WIDTH - 1 || ship_pos.y > MAP_WIDTH * MAP_CELL_HEIGH - 1)
// 			{
// 				m_IsShipMoving = false;
// 				return;
// 			}
			bool hitText = m_mapLayer->getHistCheck()->landHitTest(ship_pos.x, MAP_WIDTH * MAP_CELL_HEIGH - ship_pos.y, 20);
			ship->setIsShipHitLand(hitText);
		}
		int i = 0;
		for (int i = 0; i < m_ghostShips.size();i++)
		{
			auto ship = m_ghostShips.at(i);
			if (ship->getIsShipHitLand())
			{
				ship->removeShip();
				m_ghostShips.erase(m_ghostShips.begin() + i);
				i -= 1;
			}
		}
	}
	long b = Utils::gettime();
	if (AppHelper::frameTime > 0.045)
	{
		log("UIShadowShipManage:%d, frameTime:%f, frameCount:%d", b - a, AppHelper::frameTime, AppHelper::frameCount);
	}
}


void UIShadowShipManage::reportShipPosition(struct _NearbyPlayer**array,int number){
//	timeval timeStruc;
//	gettimeofday(&timeStruc,nullptr);
//	int64_t sec1 = ((int64_t)timeStruc.tv_sec) * 1000;
//	int64_t sec2 = timeStruc.tv_usec / 1000;
//	int64_t curTime = sec1 + sec2;
//	//m_lastUpdateMilliSec = curTime;
//	//return;
//	int64_t roadTime = 0;
	std::vector<struct NearByPlayerItem> newList;
	if(m_lastPosition.size() > 0){
		for (auto item = m_lastPosition.begin(); item != m_lastPosition.end(); item++)
		{
			for(int newIdx = 0;newIdx <number;newIdx ++){
				if((*item).cid == array[newIdx]->cid){
					NearByPlayerItem player;
					player.cid = array[newIdx]->cid;
					player.name = array[newIdx]->name;
					player.sid = array[newIdx]->sid;
					//player.start = Vec2((*item).x,(*item).y);
					//player.end = Vec2(array[newIdx]->x,array[newIdx]->y);

					auto rd = array[newIdx]->shiproutine;
					player.roadTime = rd->starttime;
					if(rd->n_roads > 1){
						for(auto i =1;i<rd->n_roads;i++){
							struct ShipRoadLines item;
							item.start = Vec2(rd->roads[i-1]->x,rd->roads[i-1]->y);
							item.end = Vec2(rd->roads[i]->x,rd->roads[i]->y);
							item.millisec = rd->roads[i]->millisec;// - rd->roads[i-1]->millisec;
							player.points.push_back(item);
							log("-------------------------------------------------------------------------------nearby ship (%f,%f) --------> (%f,%f) (time:%d, %d-%d)  ",item.start.x,item.start.y,item.end.x,item.end.y,item.millisec,rd->roads[i]->millisec , rd->roads[i-1]->millisec);
						}
					}else if(rd->n_roads == 1){
						struct ShipRoadLines item;
						item.start = Vec2(rd->roads[0]->x,rd->roads[0]->y);
						item.end = Vec2(rd->roads[0]->x,rd->roads[0]->y);
						item.millisec = 100;//rd->roads[0]->millisec;
						player.points.push_back(item);
						log("-------------------------------------------------------------------------------nearby ship (%f,%f) --------> (%f,%f)   ",item.start.x,item.start.y,item.end.x,item.end.y);
					}

					player.lv = array[newIdx]->lv;
					player.nation = array[newIdx]->nation;
					if(player.points.size() > 0){
						newList.push_back(player);
					}
				}
			}
		}
	}

	m_movingShips.clear();
	for(auto i=newList.begin();i!=newList.end();i++){
		m_movingShips.push_back(*i);
	}

	if (m_ghostShips.size() == 0 && m_movingShips.size() > 0)
	{
		for (NearByPlayerItem player : m_movingShips)
		{
			auto ship = UIShadowShip::create(&player);
			m_ghostShips.push_back(ship);
			m_mapLayer->getChildByTag(55)->addChild(ship, 4);
			ship->start();
//			ship->setPosition(Vec2(player.points[0].start.x, MAP_WIDTH * MAP_CELL_WIDTH - player.points[0].start.y));
//			m_ghostShips.push_back(ship);
//			m_mapLayer->getChildByTag(55)->addChild(ship, 4);
//			ship->shipMove(Vec2(player.points[0].end.x, MAP_WIDTH * MAP_CELL_WIDTH - player.points[0].end.y),player.points[0].millisec/1000.0);
		}
	}
	else if (m_ghostShips.size() > 0)
	{
		std::vector<UIShadowShip*> newShipList;
		for (auto i = 0; i < m_ghostShips.size(); i++)
		{
			auto ship = m_ghostShips.at(i);
			int idx = isShipInVector(m_movingShips, ship->getCID());
			if(idx >= 0){
				auto player = m_movingShips[idx];
				//ship->shipMove(Vec2(player.points[0].end.x, MAP_WIDTH * MAP_CELL_WIDTH - player.points[0].end.y),player.points[0].millisec/1000.0);
				ship->resetRoad(&player);
				newShipList.push_back(ship);
			}else{
				ship->removeShip();
			}
		}
		m_ghostShips.clear();
		for(auto i=newShipList.begin();i!=newShipList.end();i++){
			m_ghostShips.push_back(*i);
		}

		for(auto i=0;i<m_movingShips.size();i++){
			int found = 0;
			auto & player = m_movingShips[i];
			for(auto j=0;j<m_ghostShips.size();j++){
				if(m_ghostShips[j]->getCID() == player.cid){
					found = 1;
					break;
				}
			}
			if(!found){
				auto ship = UIShadowShip::create(&player);
				m_mapLayer->getChildByTag(55)->addChild(ship, 4);
				m_ghostShips.push_back(ship);
				ship->start();
//				ship->setPosition(Vec2(player.points[0].start.x, MAP_WIDTH * MAP_CELL_WIDTH - player.points[0].start.y));
//				m_ghostShips.push_back(ship);
//				m_mapLayer->getChildByTag(55)->addChild(ship, 4);
//				ship->shipMove(Vec2(player.points[0].end.x, MAP_WIDTH * MAP_CELL_WIDTH - player.points[0].end.y),player.points[0].millisec/1000.0);
			}
		}
	}

//	for(auto j=0;j<m_ghostShips.size();j++){
//		m_ghostShips[j]->start();
//	}

	m_lastPosition.clear();
	for(int i=0;i<number;i++){
		m_lastPosition.push_back(*array[i]);
	}
	newList.clear();
	//m_lastGetPostionMilliSec = curTime;

	//if(m_movingShips.size() > 0)
	//	log("-------------------------------------------------------------------------------nearby ship num:%d   ",m_movingShips.size());
}

int UIShadowShipManage::isShipInVector(std::vector<struct NearByPlayerItem> items , int cid)
{
	for (int i=0;i<items.size();i++)
	{
		if (items[i].cid  == cid)
		{
			return i;
		}
	}
	return -1;
}
