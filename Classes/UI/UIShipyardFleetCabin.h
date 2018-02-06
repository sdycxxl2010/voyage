/*
*
*  CopyRight (c) ....
*  Created on: 2016年01月25日
*  Author:Sen
*  The game Fleet Cabin implementation class
*
*/

#ifndef __FLEET_CABIN__
#define __FLEET_CABIN__

#include "UIBasicLayer.h"

class UIShipyardFleetCabin : public UIBasicLayer
{
public:
	UIShipyardFleetCabin();
	~UIShipyardFleetCabin();
	bool init(int sid);
	void onEnter()override;
	void onExit() override;
	static UIShipyardFleetCabin* createFleetCabin(int sid);

	//多点触控回调函数
	void onTouchesBegan(const std::vector<Touch*>& pTouches, cocos2d::Event *pEvent);
	void onTouchesMoved(const std::vector<Touch*>& pTouches, cocos2d::Event *pEvent);
	void onTouchesEnded(const std::vector<Touch*>& pTouches, cocos2d::Event *pEvent);
	void onTouchesCancelled(const std::vector<Touch*>&pTouches, cocos2d::Event *pEvent);
private:
	/*
	*按钮点击事件(默认)
	*/
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	/*
	*服务器返回数据
	*/
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	/*
	*船舱界面初始化
	*/
	void shipCabinView();
	/*
	*舱室伙伴的详情
	*/
	void companionDetails();
	/*
	*配置伙伴
	*参数 pResult:伙伴的数据
	*/
	void configCompanion();
	/*
	*舱室的数据处理(舱室是否废弃)
	*/
	void  treatShipCabinNo();
	/*
	*添加或者删除小伙伴图片
	*参数 parent:父节点 id:小伙伴的id isCaptain:是否是船长(id = -1时做删除 id = -10时舰长室特殊处理 )
		 shipPosition 船上的位置
	*/
	void addOrDelCompanion(Widget* parent, int id, bool isCaptain, int shipPosition);
	/*
	*刷新货币
	*/
	void updateCoinNum(const int64_t gold, const int64_t silver);
	/*
	*获取船舱位置
	*参数 shipPosition 船上的位置
	*/
	int getRoomType(int shipPosition);
private:
	/*
	*小伙伴的类型;服役的小伙伴,没有服役的小伙伴，没有服役的船长
	*/
	enum COMPANION_TYPE
	{
		INFLEET_COMPANION,
		IDLE_COMPANION,
		IDLE_CAPTAIN,
	};
	/*
	*船舱界面的数据
	*/
	GetShipCompanionsResult *m_pShipCResult;
	/*
	*选择的小伙伴
	*/
	Widget *m_pCompanionButton;
	/*
	*小伙伴列表数据
	*/
	GetCompaniesResult *m_pCompaniesResult;
	/*
	*舱室的数据(舱室是否废弃)
	*/
	std::vector<int> m_vShipCabinNo;
	/*
	*船只的id
	*/
	int m_nShipId;
	/*
	*是否是第一次进入
	*/
	bool m_bFirst;
	//两个触摸点之间的距离 
	double distance;     
	//目标
	cocos2d::ui::ScrollView* s_cabin;
	//初始地图缩放比例 
	double mscale;  
};
#endif
