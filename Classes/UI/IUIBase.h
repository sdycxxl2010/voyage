#ifndef _H_I_UIBASE_
#define _H_I_UIBASE_
#include "TVBasic.h"
#include "TVSingle.h"
#include "UIEnums.h"
#include "ProtocolThread.h"

enum UI_POPTYE
{
	_FULLSCREEN,
	_POPUP,
};

class IUIBase : public Layer
{

public:
	IUIBase();
	~IUIBase();

public:
	virtual void OnInit();

	virtual void OnShow();

	virtual void OnUpdate();

	virtual void OnHide();

	virtual void OnClean();

public:
	bool InitUI();

	void LoadCsb();

public:
	UI_WINDOWS uiNameType;
	UI_POPTYE uiPOPType;
public:
	bool isInited = false;

protected:
	Node*   m_pRootNode;
	Widget* m_pRootWidget;

};


#endif // !_H_I_UIBASE_
