#include "UIEnums.h"
#include "IUIAllFile.h"
#include "UIManager.h"


void RegUI(UI_WINDOWS t, IUIBase* ins)
{
	UIManager::RegUIByEnum(t, ins);
}

void UIEnums::RegAllUI()
{
	RegUI(UI_WINDOWS::_UI_SPLASH, new UISplash());
	RegUI(UI_WINDOWS::_UI_EULA, new UIEula());
	RegUI(UI_WINDOWS::_UI_START, new UINewStart());
}