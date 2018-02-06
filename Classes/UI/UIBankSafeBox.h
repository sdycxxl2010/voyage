#ifndef __WARE_BANK_H__
#define __WARE_BANK_H__

#include "UIBasicLayer.h"

class UIBankSafeBox : public UIBasicLayer
{
public:
	UIBankSafeBox();
	~UIBankSafeBox();

	static	UIBankSafeBox* createWarehouseLayer(Widget *vRoot, bool isPersonal);
	
	bool	init(Widget *vRoot);
	int	getGoodsType();//获得物品的类型
	void	onEnter()override;
	void	onExit() override;
	void	getFirstResult();//默认点击的按钮
	void	updateView(GetItemsToSellResult*,bool isFail = false);//更新物品
	void	setCallBack(std::function<void ()> callback);//银行里的回调函数
	void	setRootWidget(Widget *root);//设置上一级
	void	updateViewSell(const GetItemsToSellResult*,bool isFail = false);//更新卖界面
	void	onServerEvent(struct ProtobufCMessage* message,int msgType);
	void	updateCoin();//刷新银币V票
	void	buttonClikEvent(Ref *pSender,Widget::TouchEventType touchType);//点击事件
	void	menuCall_func(Ref *pSender,Widget::TouchEventType touchType) override;
	void	callEventByName(std::string& name,Widget* node);
	void	getItemNamePath(int itemtype,int itemid,std::string& name,std::string& path);//获取物品路径
	//Unreasonable_code_27;使用公用的关闭方法
	void closeSelf();
	ToSellItemsDefine* getSeletedItemInfo();//获取已选择物品
private:
	bool					m_bIsVisible;//物品面板的显示
	int					m_iGoodsId;//物品Id
	GetItemsToSellResult	*m_pSellResult;//保存返回结果
	std::function<void ()> m_Callback;
	//个人银行还是公会银行
	int m_nPersonalBank;
	
};


#endif

