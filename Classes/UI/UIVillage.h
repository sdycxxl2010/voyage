/*
*  CopyRight (c)
*  Created on: 2015年7月21日
*  Author: xie
*  description: something about village.第一版暂不实现王宫
*/
#ifndef __VILLAGE_LAYER_H__
#define __VILLAGE_LAYER_H__
#include "UIBasicLayer.h"
class UIVillage  : public UIBasicLayer
{
public:
	enum VIEW_VILLAGE_TAG
	{
		VILLAGE_UI_PALACE = 1,//村庄王宫
		VILLAGE_UI_TARVEN,//村庄酒馆
		
	};
	enum SAILOR_HIRE_TAG
	{
		CONFIRM_INDEX_NORMAL_HIRE=500,//普通招募
		CONFIRM_INDEX_HOST_DRINK_HIRE,//喝酒招募

	};
public:
	UIVillage();
	~UIVillage();
	static UIVillage* createVillage(int tag);
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	bool init();
	void flushVillageOffice();//村庄第一阶段王宫数据
	void flushVillagePup();//村庄第一阶段酒馆数据
	void flushVillageRank();//排行
	void openSailorConfirm(const GetAvailCrewNumResult *result);   //雇佣水手界面
	void initSailorDialog();  //初始化购买水手的滑动条
	void sliderSailor(const int nNum);                     //雇佣水手时滑动条变动更新界面显示
	void updateMainCityCoin(const int64_t nCoin, const int64_t nGold);//更新主城银币
private:
	int				viilageUiFlag;//打开模块的标志
	int				hireSailorTag;//雇佣水手标志
	int				m_nCurSailorNum;//当前水手数
	bool				m_bIsBanquet;//雇水手宴请
	bool             m_bIsHint;      //雇水手时是否请喝酒了
	
	GetBarInfoResult	   *m_pBarInfoResult;//酒吧结果保存
	GetAvailCrewNumResult *m_pAvailCrewNumResult;//雇佣水手结果
};
#endif

