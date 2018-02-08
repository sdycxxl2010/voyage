#pragma once

#include "cocos2d.h"
USING_NS_CC ;

class Vertex ; 
//ͼ�ڵ��� �Ǹ�node�� Vertex��ֻ��ʾ���ǵ����Щ���ԣ�������id�����ߣ���� ��
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

