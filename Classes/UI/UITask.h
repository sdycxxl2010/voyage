#ifndef __TASK_LAYER_H__
#define __TASK_LAYER_H__
//签到周期
#define ALLDAYS 30
//今日可签
#define CAN_SIGN 1
//不可签
#define CAN_NOT_SIGN
//已经签
#define HAVE_SIGNED 3
#include "UIBasicLayer.h"
#include "login.pb-c.h"

enum CONFIRM_INDEX_TASKLAYER
{

};
enum MAINLAYER_TASK
{
	MAIN_TASK,
	PARTNER_TASK,
	SIDE_TASK,
	NOMAL_TASK
};
enum NOMAL_TASK_TYPE
{
	NOMAL_ROB_TASK = 1,
	NOMAL_SIDE_TASK
};

class UITask : public UIBasicLayer
{
public:
	UITask();
	~UITask();
	bool init();
	void onExit();
	void onEnter();
	 
	static UITask*  getInstance()
	{
		if (m_taskLayer == nullptr)
		{
			m_taskLayer = new UITask;
		}
		return m_taskLayer;
	}
	void openTaskLayer(int index = 0,Node *parent = nullptr, int cameramask = 1);
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);
	void onServerEvent(struct ProtobufCMessage* message,int msgType);

	void flushLeftTaskButton();
	void flushTaskInfo(int index);
	int getCityIdxFromTaskId(const int nTaskId);
	//刷新界面
	void everySecondReflush(float fTime);
	//放弃任务交互框
	void openTaskConfirmDialog(const int nIndex, const bool bIsAccept = true); 
	//领取奖励界面
	void openGetRewardDialog(const HandleTaskResult *result); 
	void openMianTaskGetRewardDialog(const GetMainTaskRewardResult *taskReward);
	void openNomalGetRewardDialog(const GetDailyActivitiesRewardResult *result);
	void getNpcPresentItem(const GetPretaskItemsResult *preTask);
	//信仰之力获取的物品
	void openPrayDialog(const GetPrayResult *pPrayResult);
	void goodCall_func(Ref *pSender, Widget::TouchEventType TouchType);

	void openSublineRewardDialog();
private:
	static UITask     *m_taskLayer;
	bool                  m_mainTask;
	Widget*               m_pTempButton;
	GetUserTasksResult   *m_userTask; 
	TaskDefine		*m_pCurTaskDefine;
	CompanionTaskDefine **m_pCompanionsTaskDefine;
	HandleTaskResult	   *m_pHandleTaskResult;
	GetMainTaskRewardResult*m_pMainTaskResult; 

	GetNpcScoreResult   *m_pNpcScoreResult;

	MAINLAYER_TASK	m_task;
	MAINTASKSTORYINFO m_story; 

	std::vector<int>mainId;
	std::vector<int>sideId;
	std::vector<int>nomalId;

	int              m_nTaskFalg;
	
	//当前主线任务id 
	int			     m_nCurrentTaskId;
	//当前主线任务状态 
	int			     m_nCurrentTaskStatus;
	//当前主线任务是否全部完成 
	int				 m_nCompletedAllTasks;
	//玩家等级是否升级 
	bool			 m_bIsLevelUp;
	//玩家声望是否升级 
	bool			 m_bIsPrestigeUp;
	//玩家船上船长是否升级 
	bool			 m_bIsCaptainUp;
	//打开的那个任务
	int              m_nTaskIndex;
	//是否可以祈祷
	bool             m_bIsPrayer;

	int              m_cameraMask;
	//完成赏金榜任务的id
	int m_nCompleteBoundId;
	//日常任务的两个活动（打劫和完成支线任务活动,1 为打劫，2为支线任务活动）
	NOMAL_TASK_TYPE nomalTaskIndex;
	//保存每一天的状态值，是否可签
	int  signYesOrNoFlag[30];
	//签到的总天数
	int m_nTotalDays;

	int m_nSublineId;
	bool m_bSublineFinish;
};

#endif
