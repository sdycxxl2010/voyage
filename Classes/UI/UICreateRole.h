/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月03日
 *  Author:Sen
 *  create role 
 *
 */


#ifndef __CREATE__ROLE__
#define __CREATE__ROLE__

#include "UIBasicLayer.h"

class UICreateRole : public UIBasicLayer 
{
public:
	enum FIRSTMATE
	{
		//战斗女
		WOMAN_BATTEL =1,
		//经济女
		WOMAN_BUSSNIESS,
		//战斗男
		MAN_BATTEL,
		//经济男
		MAN_BUSSNIESS,
	};
	enum HERO_SEX
	{
		//男
		ISMAN=1,
		//女
		ISWOMAN,
	};
public:
	UICreateRole();
	~UICreateRole();
	bool init();
	static UICreateRole* create();
	static Scene* createScene();

	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);
	void nationBtnEvent(Ref *pSender,Widget::TouchEventType TouchType);   //选择国家的相应
	void nationLeftAndRigetBtnEvent(Ref *pSender,Widget::TouchEventType TouchType); //出事地左右按钮的相应
   //初始化选择选择小伙伴
	void initFirstmateChoose();
	//小伙伴偏好选择按钮相应
	void myFirstmateChooseBtn(Ref *pSender, Widget::TouchEventType TouchType);
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	
	void changePlayer(bool isAdd); //选择人物头像
	void exitGame();	//退出游戏
	void reConnect();	//重新连接

	//chengyuan ++
	//初始化选择国家界面
	void initSelectNation();
	
	/*
	*输入框操作函数
	*target: 当前正在操作的textfield
	*type:   当前的操作类型
	*/
	void textfileEvent(Ref* target, TextFiledEventType type); 
private:
	//国家
	int m_nation;
	//性别
	int m_sex;
	//图标
	int m_icon;
	//名字
	std::string m_heroName;
	//小伙伴偏好，战斗或贸易
	int m_firstmate_hobby;

	bool m_bFirstStep;
	bool m_bSecondStep;
};

#endif
