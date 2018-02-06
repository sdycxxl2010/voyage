/*
*
*  CopyRight (c) ....
*  Created on: 2016年04月07日
*  Author:Sen
*  国战船只
*
*/

#ifndef __WAR__SHIP__
#define __WAR__SHIP__

#include "UIBasicLayer.h"
#include "WarManage.h"

class UINationWarShip : public UIBasicLayer
{
public:
	UINationWarShip();
	~UINationWarShip();
	void onEnter();
	void onExit();
	bool init();
	/*
	*参数：position:在地图上的位置(0-4), ship_type:舰队的类型  fleetIndex;舰队索引值
	*/
	static UINationWarShip* createWarShip(UIBasicLayer* parent, int position, SHIP_FLEET_TYPE ship_type, int fleetIndex);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);

	/*
	*5秒发射一个炮弹
	*/
	void updateBySecond(const float fTime);
	/*
	*开炮
	*/
	void fired();
	/*
	*伤害效果
	*/
	void hurt(const float fTime);
	/*
	*获取船只的icon
	*/
	ImageView* getShipIcon();
	/*
	*设置船只的icon
	*参数: shipId:旗舰的id, rotation:旋转角度
	*/
	void setShipIcon(int shipId, int rotation);
	/*
	*获取船只的角度
	*/
	int getShipRotation();

	/*
	*获取舰队的类型
	*/
	SHIP_FLEET_TYPE getShipFleetType();
	/*
	*获取舰队的位置
	*/
	int getShipPosition();
	/*
	*设置舰队中船只的数据
	参数: shipFleetInfo;舰队中船只的信息
	*/
	void setShipFleetInfo(NationWarCharacterStatus *shipFleetInfo);
	/*
	*获取舰队中船只的信息
	*/
	NationWarCharacterStatus *getShipFleetInfo();
	/*
	*国战结束
	*/
	void countryWarStop();
	/*
	*舰队死亡
	*/
	void shipDied();
	/*
	*设置舰队的索引值（死亡处理是用到）
	*/
	void setShipFleetIndex(int index);
	/*
	*获取舰队的索引值
	*/
	int getShipFleetIndex();
	/*
	*设置cd显示和隐藏
	*/
	void setShipConnonCD(const bool isVisible);
private:	
	UIBasicLayer *m_pParent;
	/*
	*船只的icon
	*/
	ImageView *m_pShipIcon;
	/*
	*火炮开火的角度
	*/
	int   m_nRotation;
	/*
	*记录八项图记录方向
	*/
	int   m_nDirection;
	/*
	*船只的中心坐标
	*/
	Node * m_pShipNode;
	/*
	*舰队的类型
	*/
	SHIP_FLEET_TYPE m_eShipType;
	/*
	*舰队在那个位置(0-4)
	*/
	int m_nPositon;
	/*
	*舰队中船只的数据
	*/
	NationWarCharacterStatus *m_pShipFleetInfo;

	/*
	*舰队索引值
	*/
	int m_nFleetIndex;

	/*
	*火炮cd层
	*/
	Widget		*m_pConnonCD;
	/*
	*国家图标
	*/
	ImageView   *m_pNation;
	/*
	*玩家名称
	*/
	Text        *m_pName;
	/*
	*是否显示了伤害
	*/
	bool		m_bIsShowHurt;
};

#endif