#pragma once

#include "cocos2d.h"
USING_NS_CC ;

class Vertex ; 
//图节点类 是个node。 Vertex（只表示的是点的那些属性，包括，id，出边，入边 ）
class MapWalkVertex :
	public Node
{
public:
	MapWalkVertex( );
	~MapWalkVertex( );

public:

	CREATE_FUNC( MapWalkVertex ) ;

	bool init( ) ;

public : 

	void SetGraphVertex( Vertex * Var ) { m_pGraphVertex = Var ; }
	Vertex* GetGraphVertex( ) { return m_pGraphVertex ; }

private:

	Vertex * m_pGraphVertex ; 

};

