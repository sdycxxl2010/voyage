/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月03日
 *  Author:Sen
 *  登陆界面
 *
 */

#ifndef __LOGIN__LAYER__
#define __LOGIN__LAYER__

#include "UIBasicLayer.h"

class UILogin : public UIBasicLayer,ui::EditBoxDelegate
{
public:
	UILogin();
	~UILogin();
	bool init();

	static UILogin* create();
	static Scene* createScene();
    void onServerEvent(struct ProtobufCMessage* message,int msgType);
	void update(float dt);   //判断输入框是否为输入状态并修改输入框透明度;
	void openChooseRoleView(EmailLoginResult* result);
	//注册账户
	void registerUserAccount();

	void popUserSelectVersions(int step);
	virtual void editBoxReturn(ui::EditBox* editBox);
	void openYesOrNO(std::string title, std::string content, bool Changedcontent = false);//两个按钮，带标题
private:
	virtual void menuCall_func(cocos2d::Ref *pSender,cocos2d::ui::Widget::TouchEventType TouchType);
	int tryNumber;
	Vec2 m_inputOldPostion;
	Node*m_inputParent;
	void textEvent(Ref*obj, bool type);
	ProtobufCMessage * m_userInfo;
	int m_nBtnIndex;
	bool bIsEnter;
	Widget* m_pTempWidget;
	CheckDeleteCharactersNumResult* m_nunResult;
};

#endif
