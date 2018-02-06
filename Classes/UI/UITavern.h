/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月25日
 *  Author:Sen
 *  The game Tavern implementation class
 *
 */

#ifndef __PUP__LAYER__
#define __PUP__LAYER__

#include "UIBasicLayer.h"
#include "UIChatHint.h"
#include "UICommon.h"
class EffectManage;

class UITavern : public UIBasicLayer
{
public:
	//确定交互索引
	enum PUP_CONFIRM_INDEX
	{
		CONFIRM_INDEX_BARGIRL_DRINK,
		CONFIRM_INDEX_FOOD,
		CONFIRM_INDEX_DRINK,
		CONFIRM_INDEX_NOT_DRINK,
		CONFIRM_INDEX_SAILOR_DRINK,
		CONFIRM_INDEX_CAPTAIN_FRESH,
		CONFIRM_INDEX_TASK_FRESH,
		CONFIRM_INDEX_CHIEF_CHAT,
	};

	//酒吧女郎对话索引
	enum PUP_CONTENT_INDEX
	{
		CONTENT_WELCOME,
		CONTENT_ACCEPT_DRINK,
		CONTENT_REFUSE_DRINK,
		CONTENT_ACCEPT_GIFT,
		CONTENT_INFO_RMATION,
		CONTENT_BARGIRL_CHAT,
		CONTENT_NO_INTELLIGENCE,
	};
	enum PUP_UI_INDEX
	{
		UI_CHAT = 1,
		UI_ORDER,
		UI_RECRUIT_SAILOR,
		UI_HIRE_CAPTAIN,
		UI_BOUNTY_BOARD,
	};
public:
	UITavern();
	~UITavern();
	void onEnter();
	void onExit();
	static UITavern* createPup();
	bool init();
	void initResult();

	void notifyCompleted(int index) override;
	void doNextJob(int actionIndex);

	//初始化酒吧女郎聊天交互框
	void initBarGirlChat();   
	//初始化酒水交互框
	void initFoodAndDrink();  
	//初始化水手交互框
	void initSailor();        
	//初始化船长交互框
	void initCaptain();		  
	//初始化赏金榜交互框
	void initTask();          
	//初始化购买水手的滑动条
	void initSailorDialog();  
	
	//默认的button相应 
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType) override;  
	void buttonEvent(Widget *target,std::string name);
	//右侧大主题的按钮相应
	void mainButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);        
	//聊天交互里的按钮相应
	void barGirlButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);     
	//酒水交互里的按钮相应
	void foodAndDrinkButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);
	//水手交互里的按钮相应
	void sailorButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);		
	//船长交互里的按钮相应
	void captainButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);		
	//赏金榜交互里的按钮相应
	void taskButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);		
	//赏金榜任务列表的任务切换
	void taskEvent(Ref *pSender,Widget::TouchEventType TouchType);				

	//接收服务器信息
	void onServerEvent(struct ProtobufCMessage* message,int msgType);

	//酒吧女郎入场
	void showBarGirlChat(const float fTime);      
	//副官入场
	void showBarChiefChat(const float fTime);     
	//对话的点击提示图标运动
	void anchorAction(const float fTime);    
	//关闭女郎对话    
	void closeBarGirlChat();               
	//关闭副官对话
	void closeBarChiefChat();			   
	//酒馆主界面tavern_csb所有按钮向左移动
	void mainButtonMoveToLeft(const float fTime); 
	//酒馆主界面tavern_csb所有按钮向右移动
	void mainButtonMoveToRight();          
	//酒馆主界面显示tavern_2_csb
	void openOtherMainButton(const float fTime);  
	//酒馆主界面tavern_2_csb所有按钮隐藏
	void mainButtonTavern2CsbHide(const float fTime);
	//按钮选中的状态改变
	void mainButtonChangeStatus(Widget *target); 
	//动作结束 更改动作判定状态
	void actionRunningEnd(const float fTime);

	//打开聊天交互
	void openBarGirlDialog(Widget *button);         
	//打开酒水交互
	void openFoodAndDrinkDialog(Widget *button);    
	//打开水手交互
	void openSailorDialog(Widget *button);			
	//打开船长交互
	void openCaptainDialog(Widget *button);			
	//打开赏金榜交互
	void openTaskDialog(Widget *button);	
	//关闭当前view		
	void closeDialog();                   
      
	//显示礼物界面
	void openGiftView();                   
	//解析情报 
	void parseIntelligence(std::string &text);  

	//雇佣水手界面
	void openSailorConfirm(const GetAvailCrewNumResult *result);  
	//雇佣水手时滑动条变动更新界面显示 
	void sliderSailor(const int nNum);                          
	
	//刷新我的船长界面
	void flushMyCaptains();                         
	//更新我的船长数据
	void addMyCaptains(const GetCaptainResult *result);   
	//更新我的船长数据
	void deleteMyCaptain(const FireCaptainResult *result);
	//刷新商店里船长界面
	void flushShopCaptains();						
	//解雇船长交互框
	void openFireCaptainsConfirm(const int nIndex);        

	//刷新任务列表
	void flushTaskList();                      
	//任务的详情
	void showTaskInfo(const int nTaskId,const int nCoreTaskId,const int nNpcIdx);  
	int getCoreTaskIdFromTaskId(const int nTaskId);                  
	int getNpcIdxFromTaskId(const int nTaskId);
	int getCityIdxFromTaskId(const int nTaskId);
	//放弃任务交互框
	void openTaskConfirmDialog(const int nIndex,const bool bIsAccept = true); 
	//领取奖励界面
	void openGetRewardDialog(const HandleTaskResult *result); 

	//更新银币和V票
	void updateCoin(const int64_t nCoin,const int64_t nGold); 
	//刷新界面    
	void everySecondReflush(const float fTime);    
	//交互框移动
	bool moveTo(Node *target,const Point& pos);        

	void scrollEvent(Ref *node, ScrollviewEventType type);
	/*酒吧女郎聊天按钮选中的状态改变*/
	void minorButtonChangeStatus(Widget *target);
	/*酒吧女郎聊天button左移动*/
	void minorButtonMoveToLeft(const float fTime); 
	/*酒吧女郎聊天button右移动*/
	void minorButtonMoveToRight(const float fTime);
	/*酒吧标题的显示隐藏*/
	void tarvenShowOrHide(const float fTime, const bool isShow);
	/*改变酒吧女郎聊天对话内容*/
	void changeBarGirlChat(const float fTime);
	//逐字显示文本的方法
	void showDialogWordText(float t);
	//逐字显示文本的定时器
	void showDialogWordSchedule();
	//好感度动作
	void showFavorDegreeAction();
	//进入聊天界面时 好感度状态等级显示
	void showFavorStatus();
	//解雇水手
	void initDismissSailors();
	//船长交互里的按钮相应
	void dismissSailorButtonEvent(Ref *pSender, Widget::TouchEventType TouchType);
	//选择条事件
	void dismissSliderChange(Ref* obj, cocos2d::ui::Slider::EventType type);
	//酒馆女郎好感度状态特效总sprite
	Sprite				   *m_pSpriteForFavorAnimate;
	//好感度// two button show by it
	int					   m_nFavour; 
	int                    m_nViewIndex;
	UIChatHint		   *m_pCh;
private:
	GetBarInfoResult	   *m_pBarInfoResult;
	GetIntelligenceResult  *m_pIntelligenceResult;
	Widget				   *m_pMainButton; 
	Widget                 *m_pCaptainButton;
	GetAvailCrewNumResult  *m_pAvailCrewNumResult;
	GetPersonalItemResult  *m_pGiftResult;
	HandleTaskResult	   *m_pHandleTaskResult;
	TaskDefine			   *m_pCurTaskDefine;
	Widget				   *m_pTaskButton;
	GetNpcScoreResult      *m_pNpcScoreResult;
	/*酒吧女郎聊天*/
	Widget				   *m_pBarGirlButton;
	/*酒吧女郎聊天礼物*/
	Widget				   *m_pGiftButton;
	Label				   *m_BarGirlContentLabel;

	Label				   *chatTxt;
	ImageView			   *anchPic;
	
	//我的船长信息
	std::vector<BarCaptainDefine*> m_vMyCaptains;  
	//商店的船长信息
	std::vector<BarCaptainDefine*> m_vShopCaptains;
	//任务
	std::vector<int> m_vTaskIds;
	std::vector<int> m_vCoreTaskIds;
	std::vector<int> m_vNpcIdx;
	std::vector<int> m_vCityIdx;
	std::unordered_map<int, int> m_BonusMap;

	std::string chatContent;

	//船长刷新
	int64_t				   m_nCaptainFlushTime;  
	//任务刷新
	int64_t				   m_nTaskFlushTime; 
	
	//确认对话索引
	int					   m_nConfirmIndex;    
	//酒水
	int                    m_nBarIndex;         
	//酒吧女郎对话索引
	int					   m_nChatContentIndex;  
	//0:not task 1:not finish task 2:finish task
	int					   m_nTaskFalg;
	//当前显示的任务id
	int					   m_nTaskId;
	//请酒吧女郎喝酒次数
	int					   m_nCheers;
	//当前水手数
	int					   m_nCurSailorNum;
	int					   lenNum;
	int					   lenAfter;
	int					   plusNum;
	int					   maxLen;
	//对话的总页数
	int					   m_nToTalDialogPage;
	//当前对话页
	int					   m_nCurDialogPage;
	//记录anchor的初始y坐标
	int					   bargirlPositionY;
	int					   triggerDialogId;
	//是否收到的合心意礼物 0为不合心意 1为合心意
	int					   m_nlikeRecievedGift;
	    
	//雇水手宴请
	bool				   m_bIsBanquet;    
	//雇水手时是否请喝酒了
	bool				   m_bIsHint;                
	//是否是友好城市
	bool				   m_bIsFriend;  
	//玩家等级是否升级
	bool				   m_bIsLevelUp;		
	//玩家声望是否升级
	bool				   m_bIsPrestigeUp;	
	//玩家船上船长是否升级
	bool				   m_bIsCaptainUp;	
	//聊天界面返回按钮点击状态 true可以点击
	bool				   m_bChatButtonBackClick;
	//是否有运行中的动作
	bool				   m_bActionRunning;
	bool				   m_bChiefClick;
	//是否进入的村庄
	bool				   m_IsVillage;	
	bool				   wordLenBoo;
	//逐字显示是否结束
	bool				   guardDiaolgOver;
	//副官是否可以点击
	bool				   bargirlClickBoo;
	//好感度动画是否播放过
	bool				   favorDegreeActionPlayed;
	
	//好感度状态特效--星星动画
	void createStarForBargirlFavor(int count);

	/*
	*技能图标的表达(每个界面显示时用到要清空)
	*/
	std::vector<SKILL_DEFINE> m_vSkillDefine;
	//最后完成的taskid
	int m_lastTaskid;
	//解雇的水手数
	int m_ndissmissSailors;
	GLProgram* BarGLProgram;
};
#endif
