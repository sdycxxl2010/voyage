/*
*
*  CopyRight (c) ....
*  Created on: 2015年12月17日
*  Author:Chengyuan
*  海上影子船只
*
*/
#ifndef __GHOSTSHIP_H__
#define __GHOSTSHIP_H__
#include "cocos2d.h"
#include "SailRoad.h"
#include "TVSailDefineVar.h"
#include "UIShadowShipManage.h"

USING_NS_CC;
class UIShadowShip : public Node 
{
public:
	//船头朝向
	enum SHIP_DIRECTION
	{
		//初始
		DIR_NONE,
		//向上
		UP,
		//向下
		DOWN,
		//向左
		LEFT,
		//向右
		RIGHT,
		//左上
		L_UP,
		//左下
		L_DOWN,
		//右上
		R_UP,
		//右下
		R_DOWN,
	};
public:
	UIShadowShip();
	~UIShadowShip();
	bool init(struct NearByPlayerItem*playerInfo);
	
	/*
	*更新船只路径
	*playerInfo: 影子船只数据
	*/
	bool resetRoad(struct NearByPlayerItem*playerInfo);
	static UIShadowShip * create(struct NearByPlayerItem * playerInfo);

	// 开始船只动画,无参数，方便外部调用
	void start();
	
	/*
	*船只移动
	*pos:移动到的点
	*sec:移动的时间
	*/
	void shipMove(Vec2 pos,double sec);

	//删除船只
	void removeShip();

	//的到玩家的cid
	int getCID(){ return m_cid; };

	//得到当前船只的运动状态
	bool getMoved(){ return m_isMoved; };
	/*
	*船只是否撞到陆地
	*b_hit : 撞到/没撞到
	*/
	void setIsShipHitLand(bool b_hit){ m_isHitLand = b_hit; };

	bool getIsShipHitLand(){ return m_isHitLand; };
private:
	/*
	*船只转向
	*dir : 船头朝向
	*/
	void changeDirection(SHIP_DIRECTION dir);

	//加载船只信息
	void loadShip();

	/**
	*判断船只下次移动的方向
	*@param 船只要移动到的点
	*/
	SHIP_DIRECTION confirmShipDir(Vec2 pos);

	//船只移动完成的回调
	void moveEnd();

	void update(float dt);

	//船只运动轨迹
	std::queue<ShipRoadLines>m_qSailRoads;
private:
	//当前船的朝向
	SHIP_DIRECTION m_currentDir;
	//船只信息
	NearByPlayerItem * m_playerInfo;
	//船只贴图
	Sprite * m_shipImage;
	//国家Icon
	ImageView * m_countryIcon;
	//船只名
	Text* m_tname;
	//船只等级
	Text* m_tlevel;
	//当前船只是否完成运动
	bool m_isMoved;
	//当前船只玩家cid
	int m_cid;
	//是否撞到陆地
	bool m_isHitLand;
	//船只是否运动完成
	bool m_stopped;
	//上一次移动花费的时间
	int64_t m_lastRoadTIme;
	/*
	*影子的玩家旗舰的id
	*/
	int  m_nShipId;
};


#endif
