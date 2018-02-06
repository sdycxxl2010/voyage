/*
*  CopyRight (c) 
*  Created on: 2015年4月21日
*  Author: xie 
*  description: something about set,include music, help info and so on
*/
#ifndef __SETTING_LAYER_H__
#define __SETTING_LAYER_H__
#include "UIBasicLayer.h"


class UISetting:public UIBasicLayer
{
public:
	enum ACCOUNT_CONFIRM_YES
	{
		INDEX_CHANG_PASWORD=1,//修改密码
		INDEX_QUIT_GAME,//注销
		INDEX_DELE_ACCOUNT//删除账户
	};

public:
	UISetting();
	~UISetting();
	bool init();
	void initSetting(float f);
	void onExit();
	void onEnter();
	static UISetting* getInstance()
	{
		if (sm_settingLayer==nullptr)
		{
			sm_settingLayer=new UISetting;
		}
		return sm_settingLayer;
	}
	void onServerEvent(struct ProtobufCMessage* message,int msgType);
	void menuCall_func(Ref *pSender,Widget::TouchEventType TouchType);
	void WebSiteTouch(Ref *pSender,Widget::TouchEventType TouchType);//点击网址
	void openSettingLayer(int cameraTag =1);
	void firstSettingView();//初始化开始界面
	void showLanguage();
	void showInfoPanel();//信息界面
	void legendItemDetail();//图鉴信息
	void openCustomService();//打开客服界面
	void customServiceButton(Ref *pSender, Widget::TouchEventType TouchType);//客服点击事件
	void customServiceFeedBack(bool closeMainSet = false);//客户提交反馈页面
	void useLicenseAgreement();//用户协议
	void openPassword();//修改密码
	void checkboxPassword(Ref* target, CheckBoxEventType type);
	//账户显示
	void viewMyAccount();
	//设置里的认证账户相关按钮
	void verifyAccountBtnEvent(Ref *pSender, Widget::TouchEventType TouchType);
	//打开账户输入框
	void openResetAccount();
	//更新日志
	void openChangLogView(GetChangeLogResult*logResult);
	//获取更新日志信息
	void getChangeLogInfo(bool closeMainSet = false);
private:
	int								m_nBtnIndex;//按钮的标识
	UILoadingIndicator*				m_pLoadLayer;
	static UISetting*		sm_settingLayer;//单例对象
	TextField* m_pTextfiled;//客服的输入框
	int m_ncustormBtnTag;//问题反馈的按钮tag
	TextField* m_pNewPassword;
	TextField* m_pReaptPassword;
	TextField* m_pResetAccountContent;
};
#endif
