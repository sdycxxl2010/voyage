/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月29日
 *  Author:Sen
 *  The game center role implementation class
 *
 */

#ifndef __CENTER__ROLE__
#define __CENTER__ROLE__

#include "UIBasicLayer.h"

class UICenterCharactorRole : public UIBasicLayer,ui::EditBoxDelegate
{
public:
	UICenterCharactorRole();
	~UICenterCharactorRole();
	static UICenterCharactorRole* createRole(UIBasicLayer *parent);
	bool init();
	/*
	*打开个人信息界面
	*/
	void openCenterRole();
	/*
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage *message, int msgType);
	/*
	*按钮点击事件
	*/
	void buttonEvent(Widget *pSender, std::string name);
private:
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	/*
	*个人签名点击事件
	*/
	void textFiledEvent(Ref *target, bool type);
	/*
	*刷新个人的信息
	*参数：pResult:个人数据
	*/
	void flushRoleInfomation(const GetPersonalInfoResult *pResult);
	/*
	*用户点下return调用
	*/
	void editBoxReturn(ui::EditBox * editBox);
private:
	/*
	*根节点(CenterLayer)
	*/
	UIBasicLayer				*m_pParent;
	/*
	*记录个人信息
	*/
	const GetPersonalInfoResult * m_pResult;
	/*
	*打开许可证详情界面
	*/
	void openBussinessView(const GetPersonalInfoResult *pResult);
	/*
	*个人签名
	*/
	ui::EditBox    *m_pInputText;
	/**
	*个性签名信息
	*/
	std::string m_stringbio;

	/*
	*记录用户是否输入完成
	*/
	bool                bIsEnter;
};

#endif
