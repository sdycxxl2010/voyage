/*
*  CopyRight (c)
*  Created on: 2016��03��09��
*  Author: sen
*  description: ���г������찴ť���
*/
#ifndef __CHAT_HINT__
#define __CHAT_HINT__
#include "UIBasicLayer.h"

class UIChatHint : public UIBasicLayer
{
public:
	UIChatHint();
	~UIChatHint();
	static UIChatHint* createHint();
	//��ʼ��
	bool init();
	//Ĭ�ϵ���¼�
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	void update(float delta);
private:
	ImageView * i_unRead;
};
#endif
