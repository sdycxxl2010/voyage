#ifndef _H_UI_NEW_START_
#define _H_UI_NEW_START_
#include "TVBasic.h"
#include "IUIBase.h"
#include "ProtocolDefine.h"

class UINewStart : public IUIBase
{
	enum WidgetTag
	{
		_BUTTON_START = 1,

	};


public:
	UINewStart();
	~UINewStart();

public:
	virtual void OnInit() override;

	virtual void OnShow() override;

	virtual void OnUpdate() override;

	virtual void OnHide() override;

public:
	void MenuCallback(Ref *pSender, Widget::TouchEventType TouchType);

private:
	int m_ConfigClientVersion;
	int m_DefaultServerIndex;
};


#endif