#include "IUIBase.h"
#include "TVScene.h"
#include "TVSDHeaders.h"


IUIBase::IUIBase():
uiNameType(UI_WINDOWS::_UI_BEGIN),
uiPOPType(UI_POPTYE::_FULLSCREEN)
{

}

IUIBase::~IUIBase()
{

}

bool IUIBase::InitUI()
{
	if (!isInited)
	{
		isInited = true;
		TVScene::curSceneRootNode()->addChild(this);
		LoadCsb();
	}
	return isInited;
}

void IUIBase::OnInit()
{
	
}

void IUIBase::OnShow()
{
}

void IUIBase::OnUpdate()
{

}

void IUIBase::OnHide()
{
	assert(m_pRootNode != NULL);
	removeChild(m_pRootNode);
	m_pRootWidget = NULL;
}

void IUIBase::OnClean()
{
	if (m_pRootNode != NULL)
	{
		removeChild(m_pRootNode, true);
	}
	TVScene::curSceneRootNode()->removeChild(this, true);
}


void IUIBase::LoadCsb()
{
	/*SDUIList *_ui = SDUIList::GetElement((int)uiNameType);
	assert(_ui != nullptr);
	uiPOPType = (UI_POPTYE)_ui->getIsPop();

	string fullPath("cocosstudio/");
	fullPath.append(_ui->getUIResPath());

	Node* wgRoot = CSLoader::createNode(fullPath);

	Widget* pWidgetRoot = dynamic_cast<Widget*>(wgRoot->getChildren().at(0));
	m_pRootNode = wgRoot;
	m_pRootWidget = pWidgetRoot;

	addChild(m_pRootNode);*/

}
void IUIBase::addNewCsb()
{

}