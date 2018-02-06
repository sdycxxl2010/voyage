/*
*  CopyRight (c) 
*  Created on: 2014年11月15日
*  Author: 
*  description: when request date,add this layer
*/
#ifndef __LOADING_LAYER_H__
#define __LOADING_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "editor-support/cocostudio/CCSGUIReader.h"
USING_NS_CC;
using namespace ui;
enum LORADING_VIEW
{
	VIEW_PANEL_LOAD_1,//一般用于场景之间的切换
	VIEW_PANEL_LOAD_2//普通的切换

};

class UILoadingIndicator : public cocos2d::Layer
{
public:
	UILoadingIndicator();
	~UILoadingIndicator();
	bool init();
	void onEnter();
	void onExit();
	void showSelf(int tag = 0);//显示loading层
	void showLoading_1(float loadDiffTag);//一般用于场景之间的切换
	void showLoading_2(int loadDiffTag);//普通的切换
	void removeSelf();//自动移除该层
	static UILoadingIndicator* create(cocos2d::Layer*parent);//一般的加载层创建
	static UILoadingIndicator* create(cocos2d::Layer*parent,int ui_Type);//用于不同场景的loading,暂时不用
	static UILoadingIndicator* createWithMask(cocos2d::Layer*parent,int cameraMask);//带摄像机的，一般用在与海上有关的部分
	static UILoadingIndicator* createWithMask(cocos2d::Layer*parent,int ui_Type,int cameraMask);//用于不同场景的，带摄像机的，一般用在与海上有关的部分，暂时不用

private:
	int Tipindex;//不同场景的标识
	cocos2d::Layer*mParent;//用到加载层的那个层
	Vector<Widget*> m_Loadviews;
};


#endif

