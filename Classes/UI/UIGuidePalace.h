/*
*  CopyRight (c)
*  Created on: 2014年10月12日
*  Author: xie
*  description: palace or city hall
*/
#ifndef __PALACEGUIDE_SCENE_H__
#define __PALACEGUIDE_SCENE_H__

#include "UIVoyageSlider.h"
#include "UIBasicLayer.h"

class UIGuidePalace : public UIBasicLayer
{
public:
	enum PALACEGUIDE_STAGE
	{
		//选择城市信息按钮
		SELECT_CITYINFO = 1,
		//购买交易许可证 
		BUY_LIENCE,
		//购买交易许可证确认
		BUY_CONFIRM,
		//返回主城界面
		BACKTO_CITY,
	};


public:
	UIGuidePalace();
	~UIGuidePalace();
	bool init();
	void onEnter();
	void onExit();
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType) override;
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	static UIGuidePalace* createPalace();

	/**
	*设置需要开启点击事件的按钮
	*@param node 按钮父节点
	*@param btn_name 按钮名
	*/
	void setButtonsDisable(Node * node, std::string btn_name = "");

	/**
	*引导玩家下一步需要点击的按钮
	*@param node 下一步需要点击的按钮
	*/
	void focusOnButton(Node * psender);
private:
	//初始化宫殿界面
	void initStatic(float f);
	/*
	*城市基本信息
	*result :城市信息数据
	*/
	void showPalaceInfo(const GetPalaceInfoResult* result);
	/*
	*按钮点击事件
	*target: 当前点击的按钮
	*name: 当前点击的按钮名字
	*/
	void buttonEventByName(Widget* target, std::string name);

	//卫兵动画及城市基本信息
	void initCityView();

	//箭头动画
	void anchorAction(float time);

	//更新银币和V票
	void showMainCoin();

	//动画退出
	void palaceoutofscreen();

	//商业许可证
	void showBussinessliceView();

	//流行品信息
	void showPalacePopularGood();

	/**
	*引导
	*/
	void guide();

	/**
	*宫殿右侧按钮动画进场
	*/
	void buttonAction(float dt);
	/**
	*宫殿按钮向右移动动画
	*/
	void buttonMoveRight();
	/**
	*打开的界面动画
	*pSender : 需要移动的界面
	*viewPath : 当前运动的界面路径
	*/
	void viewAction(Node * pSender, std::string viewPath);

	/**
	*界面关闭动画
	*/
	void viewCloseAction();

	/**
	*改变当前选中按钮状态
	*target : 当前点击按钮
	*/
	void changeMainButtonState(Widget*target);
	/**
	*获取港口类型或者港口信息
	*int port 获取的内容 0港口类型  1港口信息 2港口类型对应json文件的key值
	*/
	std::string getPortTypeOrInfo(int port = 0);
private:

	//当前剧情进度
	int                 m_guideStage;
	//是否拥有许可证
	int                 m_Haslisence;
	//新手引导模式下许可证价格
	int                 m_lisenceprice;
	//存储王宫信息
	GetPalaceInfoResult*				m_pPalaceResult;
	//NPC是否退出
	bool m_bDialogOut;
	//引导的小手
	Sprite * sprite_hand;

	//按钮是否做完动画
	bool                m_btnMoved;
	//当前显示的View;
	Node *              m_curView;
	//界面动画时候做完
	bool                m_viewruancomplete;
	//主城信息界面时候打开
	bool                m_InfoViewalreadyOpen;
	//当前打开的view的路径
	std::string         m_viewPath;
	//当前选中的按钮
	Widget* m_preMainButton;
	//是否显示了流行品
	bool  isShowPopularGoods;
};


#endif
