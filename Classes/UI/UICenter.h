/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月29日
 *  Author:Sen
 *  The game center layer implementation class
 *
 */

#ifndef __CENTER__LAYER__
#define __CENTER__LAYER__

#include "UIBasicLayer.h"

class UICenterCharactor;
class UICenterItem;
class UICenterLocal;

class UICenter : public UIBasicLayer
{
public:


public:
	UICenter();
	~UICenter();
	static	UICenter* createCenter();
	bool	init();
	/**
	*获取熟练度信息
	*/
	UICenterCharactor	 * getCharacterLayer(){ return m_pCharaLayer; };
private:
	/*
	*按钮点击事件(默认)
	*/
	void	menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	/*
	*按钮点击事件
	*/
	void	buttonEvent(Widget *target, std::string name);
	/*
	*按钮点击事件(上方的角色，物品，本地仓库按钮)
	*/
	void	scrollToView(Widget *target);
	/*
	*按钮点击事件(向下传递相应事件)
	*/
	void	distributeEvent(Widget *target,std::string name);
	/*
	*服务器返回数据
	*/
	void	onServerEvent(struct ProtobufCMessage *message,int msgType);

private:
	/*
	*主界面索引(角色，物品，本地仓库)
	*/
	enum CENTER_PAGEINDEX
	{
		PAGE_CHARACTOR = 1,
		PAGE_ITEM,
		PAGE_LOCAL,
	};

	/*
	*角色类
	*/
	UICenterLocal         *m_pLocalLayer;
	/*
	*物品类
	*/
	UICenterItem			*m_pItemLayer;
	/*
	*本地仓库类
	*/
	UICenterCharactor		*m_pCharaLayer;
	/*
	*当前的按钮--主界面索引(角色，物品，本地仓库)
	*/
	Widget				*m_pCurMainButton;
};

#endif
