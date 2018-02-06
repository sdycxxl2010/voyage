/*
*  CopyRight (c) 
*  Created on: 2014年11月15日
*  Author: 
*  description: 海上事件管理类
*/
#ifndef __SEA_EVENT_MANAGE_H__
#define __SEA_EVENT_MANAGE_H__

#include "cocos2d.h"
#include "ProtocolThread.h"
#include "TVSeaEvent.h"

class UISailManage;
class TVSeaEventManage : public cocos2d::Node
{
	friend class UISailManage;
public:
	TVSeaEventManage();
	~TVSeaEventManage();
	
	static TVSeaEventManage* createManage(cocos2d::Node* map);
	bool geneEvents(SeaEventDefine** events,int n_events);					//存储数据
	void setCurrentDays_pos(int usedDays,cocos2d::Vec2 pos);				//保存航行天数和船位置
	int getseaEventHalfRange(){ return n_r; };
	void setseaEventHalfRange(int range){ n_r = range; };
	void update(float f);
	bool isHappenEvent(TVSeaEvent* evts);					//选择海上事件的类型
	void startSeaEvent();					//触发海上事件
	void pauseSeaEvent(){ m_bIsStart = false; };//暂停
	void releaseEvents();					//释放资源
	std::vector<TVSeaEvent*> getEvents(){ return m_vEvents; };

	bool getSeaEventFLoat(){ return m_bIsFloat; };
	TVSeaEvent *SeaFloart(){ return m_flortevts; };
private:
	int n_r;
	TVSeaEvent*m_flortevts;
	bool				m_bIsStart;					//海上事件触发的标识
	int				m_nUsedDays;					//海上事件发生的时间
	int				m_nLastPestisDay;				//最近发生鼠疫的时间
	int				m_nLastHomesickDay;		//最近发生思乡的时间
	int				m_nLastSepsisDay;			//最近发生败血病的时间

	float				m_fNpcShip_flag;				//npc
	float				m_fStorm_flag;					//暴风雨
	float				m_fRock_flag;					//触礁
	cocos2d::Vec2 m_vShipPos;				//船的位置
	cocos2d::Node*	m_pMap;				//Maps类的对象
	std::vector<TVSeaEvent*>m_vEvents;	//存储海上事件
	//漂浮物的标识
	bool m_bIsFloat;
};


#endif
