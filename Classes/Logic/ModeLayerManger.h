#ifndef __MODELAYERMANGER_H
#define __MODELAYERMANGER_H
#include "cocos2d.h"
#include "UIModelLayer.h"
USING_NS_CC;
class ModeLayerManger
{
public:
	ModeLayerManger();
	~ModeLayerManger();
	static ModeLayerManger * getInstance();
	void openModelLayer();
	void addShip(Widget * pSender, int modelId, LAYER_TYPE type);
	void addCharacter(Widget * pSender, LAYER_TYPE type,CharacterEquiment info);
	void operaModels();
	void operaModels(LAYER_TYPE type);
	void removeLayer();
	void showModelLayer(bool isShow);
	Sprite3D * getCurModel();
	Sprite3D * getModelByType(LAYER_TYPE type);
	Layer * getModelLayer(){ return m_modellayer;};
	LAYER_TYPE getCurLayerType(){ return m_modellayer->getCurrentType(); };
	LAYER_TYPE getNeedShowLayerType();
private:
	UIModelLayer * m_modellayer;
	bool m_centerCharacterOpened;
	bool m_dockShipOpened;
};

#endif