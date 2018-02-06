/*
*
*  CopyRight (c) ....
*  Created on: 2016年04月07日
*  Author:Sen
*  国战海上背景层
*
*/

#ifndef __WAR__MAP__LAYER__
#define __WAR__MAP__LAYER__

#include "UIBasicLayer.h"

class UINationWarMap : public UIBasicLayer
{
public:
	UINationWarMap();
	~UINationWarMap();
	bool init();
	static UINationWarMap* createWarMapLayer(UIBasicLayer* parent);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	std::string getCellName(int x, int y);
	void getRandomAnimations(int randomCount, Animation* ani);

	/*
	*初始化主城图标
	*/
	void initCityData();
	/*
	*获取攻打主城的icon
	*/
	Button* getCityImage();
	/*
	*城市30秒发射一个炮弹
	*/
	void updateBySecond(const float fTime);
	/*
	*开炮
	*/
	void fired();
	/*
	*伤害效果
	*参数 tag:那个舰队(0-4)
	*/
	void hurt(const int tag);
	/*
	*国战结束
	*/
	void countryWarStop();

	/*
	*更新堡垒的状态
	*参数  duable:当前城市的血量 max_duable:城市的最大血量
	*/
	void updateCityButton(int duable, int max_duable);
private:
	/*
	*城市的图标
	*/
	Button  *m_pCityButton;
	UIBasicLayer *m_pParent;
};

#endif
