/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月29日
 *  Author:Sen
 *  The game center item implementation class
 *
 */

#ifndef __CENTER__ITEM__
#define __CENTER__ITEM__

#include "UIBasicLayer.h"
#include "UICommon.h"

class UICenterItem : public UIBasicLayer
{
public:
	UICenterItem();
	~UICenterItem();
	static UICenterItem* createItem(UIBasicLayer *parent);
	/*
	*打开物品的界面
	*/
	void openCreateItem();
	/*
	*按钮点击事件
	*/
	void buttonEvent(Widget *pSender,std::string name);
	/*
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage *message,int msgType);
private:
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	/*
	*更改按钮状态(物品界面左侧的按钮)
	*参数 target:当前点击的按钮
	*/
	void changeMinorButtonState(Widget *target);
	/*
	*更新船只界面
	*/
	void updateShipsView(); 
	/*
	*更新船装备和人装备界面
	*/
	void updateShipEquipView();
	/*
	*更新货物、道具、图纸和特殊物品界面
	*/
	void updateItemsView();
	/*
	*更新下方的背包和船舱容量
	*/
	void updateBagAndShipCapacity();
	/*
	*丢弃物品窗口
	*/
	void showDropItem();
	/*
	*丢弃物品窗口--滑动条实现显示
	*参数 num:个数
	*/
	void updataDropNum(int num);
	/*
	*丢弃确认
	*/
	void onDropItem(Ref *pSender, Widget::TouchEventType TouchType);

	/*
	*左侧滚动提示
	*/
	void scrollButtonEvent(Ref *pSender, cocos2d::ui::ScrollView::EventType type);
private:
	/*
	*根节点(CenterLayer)
	*/
	UIBasicLayer	*m_pParent;
	/*
	*记录物品界面左侧的按钮
	*/
	Widget      *m_pMinorButton;
	/*
	*个人物品的数据
	*/
	GetPersonalItemResult *m_pResult;
	/*
	*存储已用装备的index
	*/
	std::vector<int>EquipUsing;
	/*
	*存储背包中的index
	*/
	std::vector<int>EquipNotUsing;
	/*
	*记录个人物品的index索引值
	*/
	int			 m_nItemIndex;

	/*
	*技能图标的表达(每个界面显示时用到要清空)
	*/
	std::vector<SKILL_DEFINE> m_vSkillDefine;
};

#endif
