#ifndef _H_UIMANAGER_
#define _H_UIMANAGER_
#include "TVBasic.h"
#include "UIEnums.h"
#include "IUIBase.h"

class UIManager :SingletonPtr<UIManager>
{
	friend class SingletonPtr<UIManager>;
public:
	UIManager();
	~UIManager();

public:
	static void PushUI(UI_WINDOWS winName);
	static void PopUI();
	static void RegUIByEnum(UI_WINDOWS winName, IUIBase* uiInstance);

private:
	void Init();
	void RegUIByEnumImpl(UI_WINDOWS winName, IUIBase* uiInstance);
	void PushUIImpl(UI_WINDOWS winName);
	void PopUIImpl();

	void HideLastUI(); 

private:
	map<UI_WINDOWS, IUIBase*> m_mUISet;

	IUIBase *curUIIns;
	vector<IUIBase*> m_vUIStack;
};

#endif
