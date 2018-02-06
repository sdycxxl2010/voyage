#include "UISplash.h"
#include "Utils.h"
#include "UIManager.h"

UISplash::UISplash()
{
}

UISplash::~UISplash()
{
}



void UISplash::OnInit()
{

}

void UISplash::OnShow()
{
	IUIBase::OnShow();
	auto view = m_pRootWidget;
	view->setOpacity(0);
	view->runAction(Sequence::create(FadeIn::create(0.8), DelayTime::create(1.7), FadeOut::create(0.5), CallFunc::create([=]{
		if (UserDefault::getInstance()->getBoolForKey(ProtocolThread::GetInstance()->getFullKeyName(HAVE_ACCPET_EULA).c_str()))
		{
			openStartPanel();
		}
		else
		{
			openEula();
		}

	}), nullptr));
}

void UISplash::OnUpdate()
{

}

void UISplash::OnHide()
{
	IUIBase::OnHide();

}


void UISplash::openEula()
{
	UIManager::PushUI(UI_WINDOWS::_UI_EULA);
}

void  UISplash::openStartPanel()
{
	UIManager::PushUI(UI_WINDOWS::_UI_START);
}