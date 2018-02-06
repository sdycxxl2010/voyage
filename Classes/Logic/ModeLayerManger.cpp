#include "ModeLayerManger.h"
#include "UIModelLayer.h"
ModeLayerManger::ModeLayerManger()
{
	m_modellayer = nullptr;
	m_centerCharacterOpened = false;
	m_dockShipOpened = false;
}
ModeLayerManger::~ModeLayerManger()
{
	m_modellayer = nullptr;
	m_centerCharacterOpened = false;
	m_dockShipOpened = false;
}
ModeLayerManger * m_pInstance = 0;
ModeLayerManger* ModeLayerManger::getInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new ModeLayerManger;
	}
	return m_pInstance;
}
void ModeLayerManger::openModelLayer()
{
	if (m_modellayer == nullptr)
	{
		m_modellayer = UIModelLayer::create();
		m_modellayer->setName("modelLayer");
	}
	else
	{
		m_modellayer->setVisible(true);
		return;
	}
	auto scene = Director::getInstance()->getRunningScene();
	scene->addChild(m_modellayer);
}
void ModeLayerManger::addShip(Widget * pSender, int modelId, LAYER_TYPE type)
{
	m_modellayer->addShipModel(pSender, modelId, type);
	operaModels();
	if (type == DOCK_SHIP)
	{
		m_dockShipOpened = true;
	}
}
void ModeLayerManger::addCharacter(Widget * pSender, LAYER_TYPE type, CharacterEquiment info)
{
	m_modellayer->addCharacterModel(pSender, type, info);
	operaModels();
	if (type == CENTER_CHARACTER)
	{
		m_centerCharacterOpened = true;
	}
}

void ModeLayerManger::operaModels()
{
	auto models = m_modellayer->getModels();
	auto type = m_modellayer->getCurrentType();
	int i = 0;
	for (auto model : models)
	{		
		if (model != nullptr && i != (int)type)
		{
			model->setVisible(false);
		}
		else if(model != nullptr && i == (int)type)
		{
			model->setVisible(true);
		}
		i++;
	}
}
void ModeLayerManger::operaModels(LAYER_TYPE type)
{
	auto models = m_modellayer->getModels();
	int i = 0;
	for (auto model : models)
	{
		if (model != nullptr && i != (int)type)
		{
			model->setVisible(false);
		}
		else if (model != nullptr && i == (int)type)
		{
			model->setVisible(true);
		}
		i++;
	}
	m_modellayer->setLayerType(type);
}
void ModeLayerManger::removeLayer()
{
	if (m_modellayer != nullptr)
	{
		m_modellayer->removeFromParentAndCleanup(true);
		m_modellayer = nullptr;
		m_centerCharacterOpened = false;
		m_dockShipOpened = false;
	}
}
void ModeLayerManger::showModelLayer(bool isShow)
{
	if (m_modellayer != nullptr)
	{
		m_modellayer->setVisible(isShow);
		m_modellayer->setTouchEnabled(isShow);
	}
}
Sprite3D * ModeLayerManger::getCurModel()
{
	return m_modellayer->getCurModel();
}
Sprite3D * ModeLayerManger::getModelByType(LAYER_TYPE type)
{
	Sprite3D * sp = m_modellayer->getModels().at(type);
	return sp;
}
LAYER_TYPE ModeLayerManger::getNeedShowLayerType()
{
	if (m_dockShipOpened)
	{
		return DOCK_SHIP;
	}
	if (m_centerCharacterOpened)
	{
		return CENTER_CHARACTER;
	}
	return INITIAL_STATUS;
}