#include "UICenter.h"
#include "UICenterCharactor.h"
#include "UICenterLocal.h"
#include "UICenterItem.h"
#include "UICenterCharactorEquip.h"
#include "UISocial.h"
#include "UIChatHint.h"
#include "UISailManage.h"
#include "TVSceneLoader.h"
#include "ModeLayerManger.h"
UICenter::UICenter()
	:m_pLocalLayer(nullptr)
	,m_pItemLayer(nullptr)
	,m_pCurMainButton(nullptr)
	,m_pCharaLayer(nullptr)
{
	m_eUIType = UI_CENTER;
}

UICenter::~UICenter()
{
	UISocial::getInstance()->showChat(1);
	CC_SAFE_RELEASE(m_pLocalLayer);
	CC_SAFE_RELEASE(m_pCharaLayer);
	CC_SAFE_RELEASE(m_pItemLayer);
	unregisterCallBack();
}

UICenter* UICenter::createCenter()
{
	UICenter* cl = new UICenter;
	if (cl && cl->init())
	{
		cl->autorelease();
		return cl;
	}
	CC_SAFE_DELETE(cl);
	return nullptr;
}

bool UICenter::init()
{
	bool pRet = false;
	do 
	{
		CC_BREAK_IF(!UIBasicLayer::init());
		registerCallBack();
	
		SINGLE_HERO->m_heroIsOnsea = false;
		openView(PLAYER_COCOS_RES[PLAYER_CSB]);
		auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_CSB]);

		auto t_charactor = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_charactor"));
		auto t_item = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_items"));
		auto t_warehouse = dynamic_cast<Text*>(Helper::seekWidgetByName(view,"label_warehouse"));
        auto btn_warehouse = dynamic_cast<Button*>(Helper::seekWidgetByName(view, "button_localitems"));
        if (SINGLE_HERO->m_bSeaToCenter)
        {
            t_warehouse->setTextColor(Color4B::GRAY);
            btn_warehouse->setTouchEnabled(false);
        }

		m_pCharaLayer = UICenterCharactor::createCharactor(this);
		m_pLocalLayer = UICenterLocal::createLocal(this);
		m_pItemLayer = UICenterItem::createItem(this);
		CC_SAFE_RETAIN(m_pCharaLayer);
		CC_SAFE_RETAIN(m_pLocalLayer);
		CC_SAFE_RETAIN(m_pItemLayer);
		scrollToView(nullptr);
		//聊天
		auto ch = UIChatHint::createHint();
		this->addChild(ch, 10);
		pRet = true;
	} while (0);
	return pRet;
}

void UICenter::menuCall_func(Ref *pSender,Widget::TouchEventType TouchType)
{
	if (TouchType == Widget::TouchEventType::ENDED)
	{
		Widget* button = dynamic_cast<Widget*>(pSender);
		std::string name = button->getName();
		buttonEvent(button,name);
		return;
	}
}

void UICenter::buttonEvent(Widget* button,std::string name)
{

	//退出个人中心按钮
	if (isButton(button_back))
	{	
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		if (m_pCharaLayer->getCenterEquip()->getFinishData() == false)
		{
			m_pCharaLayer->getCenterEquip()->setFinishData(true);
			m_pCharaLayer->getCenterEquip()->saveEquipHero();
		}
		//进入海上---个人中心在海上打开
		if (SINGLE_HERO->m_bSeaToCenter)
		{
			SINGLE_HERO->m_bSeaToCenter = false;
			SINGLE_HERO->m_centerTosea = true;
			unregisterCallBack();
			Scene* sc = Scene::create();
			auto mps = UISailManage::create(SINGLE_HERO->shippos, 1);
			mps->setTag(MAP_TAG + 100);
			sc->addChild(mps);
			Director::getInstance()->replaceScene(sc);
		}
		//进入主城
		else
		{
			button_callBack();
		}
		return;
	}
	//角色信息 背包 本地仓库按钮点击事件
	if (name.compare("button_role") == 0 || name.compare("button_items") == 0 || name.compare("button_localitems") == 0)
	{	
		SINGLE_AUDIO->vplayButtonEffect(AUDIO_EFFECT_BUTTON_01);
		auto instance = ModeLayerManger::getInstance();
		auto layer = instance->getModelLayer();
		if (layer != nullptr)
		{
			instance->removeLayer();
		}
		if (m_pCharaLayer->getCenterEquip()->getFinishData() == false)
		{
			m_pCharaLayer->getCenterEquip()->setFinishData(true);
			m_pCharaLayer->getCenterEquip()->saveEquipHero();
		}
		scrollToView(button);
		UISocial::getInstance()->showChat(1);
		return;
	}

	distributeEvent(button,name);
}

void UICenter::distributeEvent(Widget* target,std::string name)
{
	int curPageIndex = m_pCurMainButton->getTag();
	switch (curPageIndex)
	{
	case PAGE_CHARACTOR://角色界面
		{
			m_pCharaLayer->buttonEvent(target,name);
			break;
		}
	case PAGE_ITEM://背包界面
		{
			if (m_pCharaLayer->getCenterEquip()->getFinishData() == false)
			{
				m_pCharaLayer->getCenterEquip()->setFinishData(true);
				m_pCharaLayer->getCenterEquip()->saveEquipHero();
			}
			m_pItemLayer->buttonEvent(target,name);
			break;
		}
	case PAGE_LOCAL: //本地仓库界面
		{
			if (m_pCharaLayer->getCenterEquip()->getFinishData() == false)
			{
				m_pCharaLayer->getCenterEquip()->setFinishData(true);
				m_pCharaLayer->getCenterEquip()->saveEquipHero();
			}
			m_pLocalLayer->buttonEvent(target,name);
			break;
		}
	default:
		break;
	}
}

void UICenter::onServerEvent(struct ProtobufCMessage* message,int msgType)
{
	UIBasicLayer::onServerEvent(message,msgType);
	switch (m_pCurMainButton->getTag())
	{
	case PAGE_CHARACTOR:
		m_pCharaLayer->onServerEvent(message,msgType);
		break;
	case PAGE_ITEM:
		m_pItemLayer->onServerEvent(message,msgType);
		break;
	case PAGE_LOCAL:
		m_pLocalLayer->onServerEvent(message,msgType);
		break;
	default:
		break;
	}
}

void UICenter::scrollToView(Widget* target)
{
	if (!target)
	{
		auto view = getViewRoot(PLAYER_COCOS_RES[PLAYER_CSB]);
		target = dynamic_cast<Button*>(Helper::seekWidgetByName(view,"button_role"));
	}

	if (m_pCurMainButton)
	{
		m_pCurMainButton->setBright(true);
		dynamic_cast<Text*>(m_pCurMainButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_NORMAL);
		m_pCurMainButton->setTouchEnabled(true);
	}
	m_pCurMainButton = target;
	m_pCurMainButton->setBright(false);
	dynamic_cast<Text*>(m_pCurMainButton->getChildren().at(0))->setTextColor(TOP_BUTTON_TEXT_COLOR_PASSED);
	m_pCurMainButton->setTouchEnabled(false);
	
	//保留最底层的csb
	int num = m_vFilePath.size();
	for (int i = 1; i < num; i++)
	{
		closeView(); 
	}

	switch (m_pCurMainButton->getTag())
	{
	case PAGE_CHARACTOR:
		m_pCharaLayer->openCenterCharactor();
		break;
	case PAGE_ITEM:
		m_pItemLayer->openCreateItem();
		break;
	case PAGE_LOCAL:
		m_pLocalLayer->openCenterLocal();
		break;
	default:
		break;
	}
}
