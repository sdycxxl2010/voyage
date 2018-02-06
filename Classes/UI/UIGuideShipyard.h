/*
*  CopyRight (c)
* 
*  Author: xie
*  description: 船坞新手引导
*/
#ifndef __DOCKGUIDE_SCENE_H__
#define __DOCKGUIDE_SCENE_H__
#include "UIBasicLayer.h"


class UIGuideShipyard : public UIBasicLayer
{
public:
	enum DOCKGUIDE_STAGE
	{
		//安装甲板
		DOCK_STEP_1_ARMOR = 1,
		//选择甲板
		DOCK_STEP_2_CHOOSE_ARMOR,
		//确定选择甲板
		DOCK_STEP_3_SURE_CHOOSE_ARMOR,
		//安装火炮1
		DOCK_STEP_4_GUN_1,
		//选择火炮1
		DOCK_STEP_5_CHOOSE_GUN_1,
		//确定安装火炮1
		DOCK_STEP_6_SURE_CHOOSE_GUN_1,
		//安装火炮2
		DOCK_STEP_7_GUN_2,
		//选择火炮2
		DOCK_STEP_8_CHOOSE_GUN_2,
		//确定安装火炮2
		DOCK_STEP_9_SURE_CHOOSE_GUN_2,
		//返回
		DOCK_STEP_10_BACK,

	};
public:
	UIGuideShipyard();
	~UIGuideShipyard();
	bool init();
	//初始化
	void initStatic();
	void onEnter();
	void onExit();
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	static UIGuideShipyard* createDockLayerGuide();
	//按钮不可点击
	void setButtonsDisable(Node * node, std::string btn_name ="");
	//小手的位置
	void handpicFocusOnButton(Node * psender);
	//打开准备界面
	void openEquipedView();
	//刷新fleet界面
	void updateFleetView();
private:
	int  equipNum;
	//攻击力、防御、方向、船速数值设置  方向船速暂定
	int  atkNum;
	int defNum;
	int speedNum;
	int steeringNum;
	//小手
	Sprite*sprite_hand;
	//第几步
	int m_guideStep;
	UILoadingIndicator*m_loadingNode;
};
#endif
