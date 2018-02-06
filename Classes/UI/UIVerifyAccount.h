/*
*  description: 主城账户验证
*/
#ifndef __VERTIFY_LAYER_H__
#define __VERTIFY_LAYER_H__
#include "UIBasicLayer.h"
class UIVerifyAccount : public UIBasicLayer
{
public:
	UIVerifyAccount();
	~UIVerifyAccount();
	bool init();
	void initStaticData(float f);

	static UIVerifyAccount* createVertifyLayer();
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	//打开更该账户界面
	void openResetEmail();
private:
	//账户输入框
	//Unreasonable_code_15;类似的控件指针可以不设置成成员变量
	TextField*m_pEmailContent;
	//验证信息
	GetEmailVerifiedInfoResult*m_pGetEmailVerifyInfo;
};
#endif

