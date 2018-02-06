/*
 * MainTaskManager.h
 *
 *  Created on: Aug 15, 2015
 *      Author: xijing
 */

#ifndef MAINTASKMANAGER_H_
#define MAINTASKMANAGER_H_
#include <functional>
#include "cocos2d.h"
#include "TaskVar.h"

struct PARTSINFO;

typedef std::function<bool (int,int)> ActionCallBack;//dialogId, flag

class MainTaskManager : public cocos2d::Ref 
{
public:

	static MainTaskManager*   GetInstance();
	PARTSINFO* m_PartsInfo;
	virtual ~MainTaskManager();
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	bool matchTaskPosition();
	bool matchTaskConidition();
    
    void reportSeaXY(int x,int y);
    void reportSeaXY(int x,int y,int sailDays);
    void reportCityArea(int cityId,CityAreaType area);
    void reportCityArea(CityAreaType area);

	void notifyUIPartDelayTime(float time);
    void notifyUIPartCompleted(float t=0);
	void completeMainTask();
    void reset();
	int checkTaskAction();

	void registerActionCallback(const ActionCallBack&callback){m_actionCallback = callback;}
	void setShowChat(bool showChat) { m_bShowChat = showChat; };
	bool getShowChat(){ return m_bShowChat; };
	void setIsTriggering(bool value) { m_isTriggering = value; };
	bool getIsTriggering(){ return m_isTriggering; };
private:
	int m_currentTaskId;
	int m_currentPartId;
	int m_currentCityId;
	int m_seaX;
	int m_seaY;
	int notifyDialogId;
	int m_sailDays;
	int64_t m_lastSendXYTime;
    
    //已经触发过
    bool m_isTriggering;
	//任务完成并且条件也满足
	bool m_isCurrentTaskCompleted;
	//显示对话
	bool m_bShowChat;

	static MainTaskManager* m_pInstance;
	
	//display content
	ActionCallBack m_actionCallback;
	CityAreaType m_currentCityArea;
	//其它参数
	TargetCity m_targetData; 

	std::vector<BoughtItem> m_boughtItems;

	MainTaskManager();
	PARTSINFO* getPartInfo(int taskId,int partId);
	bool matchCityTaskPosition(int cityId,int cityArea);
    bool matchSeaTaskPosition(int x,int y);
};

#endif /* MAINTASKMANAGER_H_ */
