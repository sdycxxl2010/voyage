#include "LayerHudControl.h"

#include "LayerMapWalk.h"
#include "MapWalkVertex.h"
#include "MapWalkConfigManager.h"
#include "tinyxml2/tinyxml2.h"

#define isButton(_tt) (name.find(#_tt) == 0)
LayerHudControl::LayerHudControl( )
{
	m_pLayerMapWalk = 0 ; 
}


LayerHudControl::~LayerHudControl( )
{
}

bool LayerHudControl::init( )
{

	Widget * pUI = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ui/hud_control/hud_control.ExportJson");
	this->addChild(pUI);
	pUI->setAnchorPoint(Point(0, 1));
	/*auto pUI2 = cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ui/mainUI/mainUI.json");
	if (pUI2!=nullptr)
	{
	log("++++++++pui2 is not null");
	}
	this->addChild(pUI2);
	pUI2->setAnchorPoint(Point(0, 0));
	pUI2->setPosition(Point(0, 0));*/

	auto rect = Director::getInstance( )->getVisibleSize( ) ; 
	//float y = rect.height - pUI->getSize( ).height ; 
	pUI->setPositionY( rect.height ) ;

	m_pTxtInfo = ( Text* ) Helper::seekWidgetByName( pUI , "lblInfo" ) ;

	m_pTxtInfo2 = ( Text* ) Helper::seekWidgetByName( pUI , "txtInfo2" ) ;

	Button * pBtn = 0 ; 
	function< void( Ref* , Widget::TouchEventType ) >  fn ;

	// 读入字符串表
	tinyxml2::XMLDocument doc ; 
	doc.LoadFile( "config/strings.xml" ) ; 
	auto pRoot = doc.RootElement( ) ; 
	for ( auto pElem = pRoot->FirstChildElement( ) ; pElem != 0 ; pElem = pElem->NextSiblingElement() )
	{
		m_StringList[ pElem->Attribute( "Key" ) ] = pElem->Attribute( "Value" ) ;
	}

	// 默认是图编辑模式
	m_pTxtInfo->setString( m_StringList[ "btnEditGraph" ] ) ; 

	// 静态函数对象。这样对象才不会被销毁。
	static auto fnSetInfo = [ & ]( Widget* pWidget )
	{
		string name = pWidget->getName( ) ;
		auto it = m_StringList.find( name ) ; 
		if ( it != m_StringList.end() )
		{
			m_pTxtInfo->setString( it->second ) ;
		}
	} ; 

	// 拖曳按钮
	pBtn = (Button*)Helper::seekWidgetByName( pUI , "btnDrag" ) ;
	fn = [ & ]( Ref* pSender , Widget::TouchEventType e )
	{
		if ( e != Widget::TouchEventType::ENDED )	return ;

		m_pLayerMapWalk->m_Mode = LayerMapWalk::OperationMode::DragContent ;
		//m_pTxtInfo->setString( "当前模式:拖曳场景" ) ;
		fnSetInfo( ( Widget* ) pSender ) ; 

	} ; 
	pBtn->addTouchEventListener( fn ) ;

	// 图编辑按钮
	pBtn = ( Button* )Helper::seekWidgetByName( pUI , "btnEditGraph" ) ;
	fn = [ &]( Ref* pSender , Widget::TouchEventType e )
	{
		if ( e != Widget::TouchEventType::ENDED )	return ;

		m_pLayerMapWalk->m_Mode = LayerMapWalk::OperationMode::PutVertex ;
		
		fnSetInfo( ( Widget* ) pSender ) ;
	} ;
	pBtn->addTouchEventListener( fn ) ;

	// 显示 隐藏图
	pBtn = ( Button* ) Helper::seekWidgetByName( pUI , "btnShowHideGraph" ) ;
	fn = [ &]( Ref* , Widget::TouchEventType e )
	{
		if ( e != Widget::TouchEventType::ENDED )	return ;

		for ( auto& it : m_pLayerMapWalk->m_MapWalkVertexes )
		{
			it->setVisible( !it->isVisible( ) ) ; 
		}
		
		for ( auto& it : m_pLayerMapWalk->m_Lines )
		{
			it->setVisible( !it->isVisible( ) ) ;
		}
	} ;
	pBtn->addTouchEventListener( fn ) ;
	
	// 放置人物
	pBtn = ( Button* ) Helper::seekWidgetByName( pUI , "btnPositionRole" ) ;
	fn = [ &]( Ref* pSender , Widget::TouchEventType e )
	{
		if ( e != Widget::TouchEventType::ENDED )	return ;

		m_pLayerMapWalk->m_Mode = LayerMapWalk::OperationMode::PositionRole ;

		fnSetInfo( ( Widget* ) pSender ) ;
	} ;
	pBtn->addTouchEventListener( fn ) ;
	
	// 行走
	pBtn = ( Button* ) Helper::seekWidgetByName( pUI , "btnWalk" ) ;
	fn = [ &]( Ref* pSender , Widget::TouchEventType e )
	{
		if ( e != Widget::TouchEventType::ENDED )	return ;

		m_pLayerMapWalk->m_Mode = LayerMapWalk::OperationMode::RoleWalk ;

		fnSetInfo( ( Widget* ) pSender ) ;
	} ;
	pBtn->addTouchEventListener( fn ) ;


	pBtn = ( Button* ) Helper::seekWidgetByName( pUI , "btnSave" ) ;
	fn = [ &]( Ref* , Widget::TouchEventType e )
	{
		if ( e != Widget::TouchEventType::ENDED )	return ;

		MapWalkConfigManager::Save( "config/map_walk.xml" , m_pLayerMapWalk ) ; 
	} ;
	pBtn->addTouchEventListener( fn ) ;
	
	
	pBtn = ( Button* ) Helper::seekWidgetByName( pUI , "btnLoad" ) ;
	fn = [&](Ref*pSender, Widget::TouchEventType e)
	{
		if ( e != Widget::TouchEventType::ENDED )	return ;

		MapWalkConfigManager::Load( "config/map_walk.xml" , m_pLayerMapWalk ) ;
		fnSetInfo((Widget*)pSender);
	} ;
	pBtn->addTouchEventListener( fn ) ;
	//清除
	pBtn = (Button*)Helper::seekWidgetByName(pUI, "btnClearGraph");
	fn = [&](Ref*pSender, Widget::TouchEventType e)
	{
		if (e != Widget::TouchEventType::ENDED)	return;
		fnSetInfo((Widget*)pSender);
	};
	pBtn->addTouchEventListener(fn);

	return true ; 
}
