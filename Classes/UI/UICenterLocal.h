/*
 *
 *  CopyRight (c) ....
 *  Created on: 2015年06月29日
 *  Author:Sen
 *  The game center local implementation class
 *
 */

#ifndef __CENTER__LOCAL__
#define __CENTER__LOCAL__

#include "UIBasicLayer.h"

class UICenterLocal : public UIBasicLayer
{
public:
	

public:
	UICenterLocal();
	~UICenterLocal();
	static UICenterLocal* createLocal(UIBasicLayer *parent);
	/*
	*打开本地仓库的界面
	*/
	void openCenterLocal();
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
	*刷新左侧城市button
	*/
	void flushCityButton();
	/*
	*更新城市的本地仓库物品
	*/
	void updateCityGoods();
	/*
	*本地仓库物品信息
	*参数 indxe:下标索引值
	*/
	void openGoodInfo(int index);
	/*
	*丢弃物品窗口
	*/
	void showDropItem();
	/*
	*滑动条实现显示
	*参数 num:个数
	*/
	void updataDropNum(int num);
	/*
	*费用提醒界面
	*/
	void showHelpInfomation();
	/*
	*更改左侧按键的状态
	*参数 target:当前点击的按钮
	*/
	void changeMinorButtonState(Widget *target);

	/*
	*限制物品的名字长度汉字10个，英文20个
	*/
	std::string changeItemName(std::string itemName);

	/*
	*左侧滚动提示
	*/
	void scrollButtonEvent(Ref *pSender, cocos2d::ui::ScrollView::EventType type);
private:
	/*
	*操作索引
	*/
	enum LOCAL_OPERATION_INDEX
	{
		LOCAL_INDEX_DROP, //丢弃
		LOCAL_INDEX_TAKE, //取回
		LOCAL_INDEX_TAKEALL,//全部取回
		NOT_LOCAL_INDEX_DROP,//丢弃的不是本地的物品
	};
	/*
	*根节点(CenterLayer)
	*/
	UIBasicLayer				*m_pParent;
	/*
	*记录本地仓库左侧的按钮
	*/
	Widget					*m_pMinorButton;
	/*
	*是不是刚打开进入本地仓库界面
	*/
	bool					 m_isFirst;
	/*
	*记录本地仓库的index索引值
	*/
	int						 m_nItemIndex;
	/*
	*本地仓库的物品的数据
	*/
	GetWarehouseItemsResult *m_Result;
	/*
	*点击按钮时，区分不同的操作
	*/
	LOCAL_OPERATION_INDEX    m_eOperationIndex;
};

#endif
