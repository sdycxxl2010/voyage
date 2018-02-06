/*
*  CopyRight (c)
*  Created on: 2016年03月09日
*  Author: sen
*  description: 所有场景聊天按钮表达
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
	//初始化
	bool init();
	//默认点击事件
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	void update(float delta);
private:
	ImageView * i_unRead;
};
#endif
