/*
 * CompanionTaskManager.h
 *
 *  Created on: Feb 16, 2016
 *      Author: xijing
 */

#ifndef COMPANIONTASKMANAGER_H_
#define COMPANIONTASKMANAGER_H_

#include "TaskVar.h"

class CompanionTaskManager : public cocos2d::Ref
{
public:

	static CompanionTaskManager*   GetInstance();
	PARTSINFO* m_PartsInfo;
	virtual ~CompanionTaskManager();
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	bool matchTaskPosition();
	bool matchTaskConidition();

    void reportSeaXY(int x,int y);
    void reportSeaXY(int x,int y,int sailDays);
    void reportCityArea(int cityId,CityAreaType area);
    void reportCityArea(CityAreaType area);

	void notifyUIPartDelayTime(float time);
    void notifyUIPartCompleted(float t=0);
	void completeCompanionTask();
    void reset();
	int checkTaskAction();

	void registerActionCallback(const ActionCallBack&callback){m_actionCallback = callback;}

	void setIsTriggering(bool value) { m_isTriggering = value; };
	bool getIsTriggering(){ return m_isTriggering; };
	int getCurrentTaskId(){ return m_currentTaskId; };
	int getCurrentPartId(){ return m_currentPartId; };
private:
	int m_currentTaskId;
	int m_currentPartId;
	int m_currentCityId;
	int m_seaX;
	int m_seaY;
	int notifyDialogId;
	int m_sailDays;
	int64_t m_lastSendXYTime;
	int missionFailed;
    //已经触发过
    bool m_isTriggering;
	//任务完成并且条件也满足
	bool m_isCurrentTaskCompleted;

	static CompanionTaskManager* m_pInstance;

	//display content
	ActionCallBack m_actionCallback;
	CityAreaType m_currentCityArea;
	//其它参数
	TargetCity m_targetData;

	std::vector<BoughtItem> m_boughtItems;

	CompanionTaskManager();
	PARTSINFO* getPartInfo(int taskId,int partId);
	bool matchCityTaskPosition(int cityId,int cityArea);
    bool matchSeaTaskPosition(int x,int y);
};


#endif /* COMPANIONTASKMANAGER_H_ */
