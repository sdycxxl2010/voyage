/*
*  CopyRight (c) 
*  Created on: 2014年11月15日
*  Author: 
*  description:confirm MessageBox
*/
#ifndef __COMMON_INFORM_VIEW_H__
#define __COMMON_INFORM_VIEW_H__

#include "UIBasicLayer.h"
#define INFORMVIEWTAG 99
class UIInform : public UIBasicLayer
{
public:
	UIInform();
	~UIInform();
	static UIInform*  getInstance()
	{
		if (m_informView == nullptr)
		{
			m_informView = new UIInform;
		}
		return m_informView;
	}
	void openInformView(UIBasicLayer *parent);//获取当前场景
    void openGoldNotEnoughYesOrNo(int num); //金币不足，带数字。
	void openConfirmYesOrNO(std::string title, std::string content,bool Changedcontent = false);//两个按钮，带标题
	void openConfirmYes(std::string name);
	void openViewAutoClose(std::string name, float time = 2.0f,const int64_t addScore =-1);//使用自动提示框

	void closeAutoInform(float time);

	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	void buttonEventByName(Widget* target,std::string name);//点击事件
	void confirmButtonEvent(Ref *pSender,Widget::TouchEventType TouchType);


protected:
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
private:

	int m_index;//提示框的标识
	static UIInform     *m_informView;//单例对象
	UIBasicLayer* m_pParent;//父类对象
};


#endif
