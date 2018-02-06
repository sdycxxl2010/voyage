#pragma once

#include "cocos2d.h"
USING_NS_CC ;

#include "cocostudio/CocoStudio.h"
using namespace cocostudio;

#include "ui/CocosGUI.h"
using namespace cocos2d::ui;

#include <unordered_map>
using namespace std ;

class LayerMapWalk ; 

class LayerHudControl :
	public Layer
{
public:
	LayerHudControl( );
	~LayerHudControl( );

public:

	CREATE_FUNC( LayerHudControl ) ;

	bool init( ) ;


public:

	LayerMapWalk * m_pLayerMapWalk ;

	Text *m_pTxtInfo ;

	Text *m_pTxtInfo2 ;

	// ×Ö·û´®±í
	unordered_map< string , string> m_StringList ;

};

