/*
*  CopyRight (c) 
*  Created on: 2014年11月15日
*  Author: xie 
*  description: something about map,include cities
*/
#ifndef __MAP_LAYER_H__
#define __MAP_LAYER_H__
#include "UIBasicLayer.h"

class UIMap : public UIBasicLayer
{
public:
	enum VIEW_MAP_TAG
	{
		MAP_TAG_MAINCITY=1,//主城
		MAP_TAG_SAILING,//海上雷达
		MAP_TAG_UNKNOWN//未知海域
	};
	enum MAP_DIRCT//地图按钮
	{
		D_UP,
		D_LEFT,
		D_DOWN,
		D_RIGHT,
	};

public:
	UIMap();
	~UIMap();
	bool init();
	void initStaticData(float f);
	static UIMap* createMap(int tag);
	
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);
	void changeMapByIndex(MAP_DIRCT direct);//更改地图显示
	void updateDirectButton();//更新方向按钮
	void showMyVisitedCities(const GetVisitedCitiesResult*result);//已发现城市
	void updateMapsInfo();//更新地图信息
	bool addCitiesForMap(ImageView* view, Rect& fieldSize, int index);//添加城市
	void updateShipPos(float f);//根据船的位置更新地图
	void hideButton(const bool hide);//自动航行时隐藏方向按钮
	void shipPositionChange();//更新船图标位置
	Vec2 getMapOffset(const ImageView* view);//放大地图
private:
	
	int		m_nMapIndex;//地图的标识
	int		m_nLastMapIndex;//更新下一张地图
	int		m_nMapTag;//不同场景下的地图显示
	
	Layout*			m_pCities;//地图上添加城市的层
	ImageView*		m_pImageMap;//全局地图
	ImageView*		m_pImageMapBg;//全局地图背景
	ImageView*		m_pShipIcon;//船图标
	Text*				m_pTextTitle;//海域名
	Button*			m_pButtonLook;//无海域地图按钮
	Widget*			m_pFindCityNum;//已发现城市数目
	Vec2				m_shipPosition;//船在海域中的位置
	GetVisitedCitiesResult* m_pResult;//保存已发现城市
	Vector<Widget*>	m_vDirectWidgets;//保存方向按钮
};
#endif
