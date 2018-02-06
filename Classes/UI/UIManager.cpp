#include "UIManager.h"

UIManager::UIManager()
{
	m_mUISet.clear();
	curUIIns = NULL;
	m_vUIStack.clear();

	Init();
}

UIManager::~UIManager()
{
	auto it = m_mUISet.begin();
	while (it != m_mUISet.end())
	{
		delete it->second;
		it++;
	}
	m_mUISet.clear();
	curUIIns = NULL;
	m_vUIStack.clear();
}

void UIManager::Init()
{
	UIEnums::RegAllUI();
}

void UIManager::PushUI(UI_WINDOWS winName)
{
	log("ui name idx£º %s", UI_WINDOWS_STR[winName].c_str());
	UIManager::getInstance()->PushUIImpl(winName);
}


void UIManager::PopUI()
{
	UIManager::getInstance()->PopUIImpl();
}

void UIManager::RegUIByEnum(UI_WINDOWS winType, IUIBase* uiInstance)
{
	UIManager::getInstance()->RegUIByEnumImpl(winType, uiInstance);
}

void UIManager::RegUIByEnumImpl(UI_WINDOWS winType, IUIBase* uiInstance)
{
	uiInstance->uiNameType = winType;
	m_mUISet.insert(make_pair(winType, uiInstance));
}

void UIManager::PushUIImpl(UI_WINDOWS winType)
{
	if (m_mUISet.find(winType) != m_mUISet.end())
	{
		IUIBase* ins = m_mUISet[winType];


		curUIIns = ins;
		if (curUIIns->uiPOPType != UI_POPTYE::_POPUP)
		{
			HideLastUI();
		}

		if (curUIIns->InitUI())
		{
			curUIIns->OnInit();
		}

		if (!ins->isVisible())
		{
			ins->setVisible(true);
		}
		ins->OnShow();

		m_vUIStack.push_back(curUIIns);
	}
	else
	{
		assert(0);//you forget reg it after you impl it.
	}
}

void UIManager::PopUIImpl()
{
	if (m_vUIStack.size() > 0)
	{
		int idx = m_vUIStack.size() - 1;
		IUIBase *ins = m_vUIStack[idx];
		ins->setVisible(false);
		m_vUIStack.erase(m_vUIStack.begin() + idx);
	}
}

void UIManager::HideLastUI()
{
	if (m_vUIStack.size() > 0)
	{
		int idx = m_vUIStack.size() - 1;
		IUIBase *ins = m_vUIStack[idx];
		ins->setVisible(false);
		ins->OnHide();
	}
}
