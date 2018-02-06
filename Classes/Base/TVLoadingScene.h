#ifndef __LOADINGSCENE_H__
#define __LOADINGSCENE_H__

#include "UIBasicLayer.h"
class UILoadingIndicator;

class TVLoadingScene : public UIBasicLayer
{
public:
	TVLoadingScene();
	~TVLoadingScene();
	bool init();
	static Scene* createLoadingScene(bool isAccountSafe = false);
	void updateData(float f);
	void exitGame();
	void reConnect();
	void sendEmailLogin(float t=0);
	void onServerEvent(struct ProtobufCMessage *message,int msgType);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	void openChooseRoleView(EmailLoginResult* result);
	//异地登录验证
	void openAbnormalAccount();
	//玩家被打劫的等待界面
	void openWaitView();
	//登录等待
	void waitTime(float dt);
	//更新版本
	void popUserSelectVersions(int step);
	void openYesOrNO(std::string title, std::string content, bool Changedcontent = false);//两个按钮，带标题
	
private:
	UILoadingIndicator*m_loading;
	int m_pFlag;
	bool m_bIsFinsh;
	bool m_bSendEmailLogin;
	ProtobufCMessage * m_userInfo;
	int m_nBtnIndex;
	//由异地账户安全转入LoadingScene
	bool n_fromAccountSafe;
	//等待时间
	int m_waitTime;
	//
	EmailLoginResult* m_emailResult;
	CheckDeleteCharactersNumResult* m_nunResult;
	Widget* m_pTempWidget;

};

enum GUIDE_STATUS{
	GUIDE_START = 0,
	GUIDE_SEA = GUIDE_START + 1,
	GUIDE_FIRST_CITY = GUIDE_START + 2,
};

enum LOADING_CONFIRM_YES
{
	INDEX_DEL_ROLE = 1,//删除角色
	INDEX_LOGOUT,//登出
};
#endif
