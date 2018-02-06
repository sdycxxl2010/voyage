/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年07月03日
 *  Author:Sen
 *  注册用户
 *
 */

#ifndef __REGISTER__USER__SCENE__
#define __REGISTER__USER__SCENE__

#include "UIBasicLayer.h"

class UIRegister : public UIBasicLayer
{
public:
	UIRegister();
	~UIRegister();
	static UIRegister* create();
	static Scene* createScene();

	bool init();
	void exitGame();
	void reConnect();
	void update(float dt);   //判断输入框是否为输入状态并修改输入框透明度;
	void textEvent(Ref*obj, TextField::EventType type);
private:
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);
	Vec2 m_inputOldPostion;
	Node*m_inputParent;
};


#endif
