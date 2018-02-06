/*
*Header File:UIHelper.h
*@author emps
*
*/

#ifndef _H_UI_HELPER_
#define _H_UI_HELPER_
#include "TVBasic.h"
typedef enum E_WIDGET_TYPE
{
	_UIButton,
	_UIText,
}WidgetType;

using namespace cocos2d::ui;
class UIHelper
{
public:
	static void DetectAddButtonWidget(Node* n, WidgetType refType, Vector<Node*>& list)
	{
		switch (refType)
		{
		case WidgetType::_UIText:
		{
          Text *w = dynamic_cast<Text*>(n);
          if (w != NULL)
          {
			list.pushBack(w);
		  }
		}
			break;
		case WidgetType::_UIButton:
		{
          Button *w = dynamic_cast<Button*>(n);
          if (w != NULL)
          {
			 list.pushBack(w);
		  }
		}
			break;
		}
	}

	static void GetWigetsByType(ui::Widget* rootW, WidgetType refType,  Vector<Node*>& list)
	{
		DetectAddButtonWidget(rootW, refType, list);
		Vector<Node*>& children = rootW->getChildren();
		for(auto it = children.begin(); it != children.end(); it ++)
		{
			ui::Widget * n = dynamic_cast<ui::Widget*>(*it);
			if(n == NULL)
			{
				continue;
			}
			
			GetWigetsByType(n, refType, list);
		}
	}


	static void ForeachSetCallbackForButton(Vector<Node*>& buttons, const Widget::ccWidgetTouchCallback& callback)
	{
		for (auto it = buttons.begin(); it != buttons.end(); it ++)
		{
			Button* btn = dynamic_cast<Button*>(*it);
			if (btn != NULL)
			{
				btn->addTouchEventListener(callback);
			}
		}
	}

private:

};

#endif