/*
*  CopyRight (c)
*  Created on: 2014年11月15日
*  Author:
*  description: 新手引导海上事件管理类
*/
#ifndef __SEA_EVENT_MANAGEGUIDE_H__
#define __SEA_EVENT_MANAGEGUIDE_H__

#include "cocos2d.h"
#include "ProtocolThread.h"
#include "TVSeaEvent.h"

class TVGuideSeaEventManage : public cocos2d::Node
{
public:
	TVGuideSeaEventManage();
	~TVGuideSeaEventManage();

	static TVGuideSeaEventManage* createManage(cocos2d::Node* map);
	bool init();
	int getGuideSeaEventHalfRange(){ return n_r; };
	void setGuideSeaEventHalfRange(int range){ n_r = range; };
	bool geneEvents(SeaEventDefine** events, int n_events);					//存储数据
	void setCurrentDays_pos(int usedDays, cocos2d::Vec2 pos);				//保存航行天数和船位置
	void update(float f);
	bool isHappenEvent(TVSeaEvent* evts);					//选择海上事件的类型
	void startSeaEvent();					//触发海上事件
	void pauseSeaEvent(){ m_bIsStart = false; };//暂停
	void releaseEvents();					//释放资源
	std::vector<TVSeaEvent*> getEvents(){ return m_vEvents; };
private:
	int n_r;
	bool				m_bIsStart;					//海上事件触发的标识
	int				m_nUsedDays;					//海上事件发生的时间

	float				m_fNpcShip_flag;				//npc
	cocos2d::Vec2 m_vShipPos;				//船的位置
	cocos2d::Node*	m_pMap;				//Maps类的对象
	std::vector<TVSeaEvent*>m_vEvents;	//存储海上事件
};


#endif