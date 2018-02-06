/*
*  description: 主城等级礼包
*/
#ifndef __GIFTPACK_LAYER_H__
#define __GIFTPACK_LAYER_H__
#include "UIBasicLayer.h"
class UIPackageDeals : public UIBasicLayer
{
public:
	enum GODDS_TAG
	{
		ONSALE_SHIP_TAG=0,//船
		ONSALE_EQUIP_TAG = 1000,//装备
		ONSALE_ITEM_TAG = 10000,//物品
	};
public:
	UIPackageDeals();
	~UIPackageDeals();
	bool init();
	void initStaticData(float f);
	static UIPackageDeals* createGiftLayer();
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	//更新物品
	void updateGiftPage();
	//显示普通的物品(没有时间限制)
	void showCommonPageGoods(Layout*pageShow);
	//显示有时间限制的物品
	void showLimitPageGoods();
	//显示倒计时
	void showLimitTime(float f);
	//显示买的东西
	void showOnsaleCost();
	//更新主城V票
	void updateMaincityCoin();
	//点击物品查看详情
	void  imageOnsaleItem_callfunc(Ref *pSender, Widget::TouchEventType TouchType);

private:
	bool m_bBuyTimeLimitGood;//是否买的限时礼包（有限时礼包，普通礼包，第一版只做限时礼包）
	//Unreasonable_code_30;取消类控件成员化
	PageView*myPageView;  //翻页容器
	int pagesCount;//页数
	GetOnsaleItemsResult*m_pOnsaleItemResults;//打包的物品组合
	BuyOnsaleItemsResult*m_pBuyItemResults;//买东西结果存储
	//Unreasonable_code_31;取消类控件成员化
	Button*buyGoodsBtn;//买东西的按钮
	std::vector<OnsaleItemDefine*>shipSale;//存储船
	std::vector<OnsaleItemDefine*>equipSale;//存储设备
	std::vector<OnsaleItemDefine*>itemsale;//存储物品
	

};
#endif
