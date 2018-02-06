/*
*  CopyRight (c)
*
*  Author: xie
*  description: 酒馆新手引导
*/
#ifndef __PUPGUIDE_SCENE_H__
#define __PUPGUIDE_SCENE_H__
#include "UIBasicLayer.h"
#include "UINoviceStoryLine.h"
class UIGuideTarven : public UIBasicLayer
{
public:
	//对话出现的时机
	enum BARGIRLSTEP
	{
		BARGIRL_STEP_1 =1,//初始的进入
		BARGIRL_STEP_2,//喝酒招待以后
	};
	//招募水手出现的时机
	enum SAILORLSTEP
	{
		SAILOR_RECRUIT_STEP_1 = 1,//未喝酒时
		SAILOR_RECRUIT_STEP_2,//喝酒以后
	};
public:
	UIGuideTarven();
	~UIGuideTarven();
	bool init();
	void initStatic();
	void onEnter();
	void onExit();
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	static UIGuideTarven* createPupLayerGuide();
	//按钮不可点击
	void setButtonsDisable(Node * node, std::string btn_name = "");
	//小手位置
	void handpicFocusOnButton(Node * psender);
	//刷新V票银币
	void updateCoin(const int64_t nCoin, const int64_t nGold);
	//女郎入场
	void showBarGirlChat(float time);
	void mainButtonMoveToLeft(const float fTime);
	void mainButtonMoveToRight();
	//其他按钮状态处理
	void openOtherMainButton(const float fTime);
	//打开是否喝酒界面
	void openSailorDialog(Widget* button);
	//动作
	bool moveTo(Widget* target, const Point& pos);
	//关闭对话
	void closeBarGirlChat();
	void initSailor();
	//招募水手界面
	void openSailorConfirm();
	//结算界面
	void crewSailorsAccount();
private:
	//小手
	Sprite*sprite_hand;
	//第几步
	int m_guideStep;
	//对话文本
	Label*m_BarGirlContentLabel;
	//对话出现的时机
	int m_dialog_step;
	//招募水手出现的时机
	int m_sailor_step;
	//是否结束动作
	bool m_bDialogActionEnd;
	//对话层
	UINoviceStoryLine*m_guideDialogLayer;

	//对话的点击提示图标运动
	void anchorAction(const float fTime);
};
#endif

