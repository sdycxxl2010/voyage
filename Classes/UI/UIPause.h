/*
*  description: 暂停页面
*/
#ifndef __GAMEPAUSE__LAYER__
#define __GAMEPAUSE__LAYER__

#include "UIBasicLayer.h"
class UISailManage;
class TVBattleManager;

class UIPause : public UIBasicLayer
{
public:
	UIPause();
	~UIPause();
	void onExit();
	void onEnter();
	static UIPause* getInstance()
	{
		if (m_gamePauseLayer == nullptr)
		{
			m_gamePauseLayer = new UIPause;
		}
		return m_gamePauseLayer;
	}
	void onServerEvent(struct ProtobufCMessage* message, int msgType);
	void menuCall_func(Ref *pSender, Widget::TouchEventType TouchType);
	void openGamePauseLayer();//打开暂停页
private:
	static UIPause*		m_gamePauseLayer;//单例对象
	UISailManage*  m_seaLayer;//海上对象
	TVBattleManager* m_battleLayer;//战斗时的对象
	//海上和战斗特殊处理暂停(是否已经暂停了)
	bool m_bSeaPause;
	//界面摄像机
	Camera * m_viewCamera;
};
#endif

