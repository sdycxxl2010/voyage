#pragma once

#include "GraphPathfinding.h"
#include <functional>

class AStar : public GraphPathfinding
{
public:
	AStar( );
	~AStar( );


public : 

	// ���ƶ��㵽Ŀ�궥��Ĵ���
	std::function<int( const Vertex* pVCurrent , const Vertex* pVTarget ) > Estimate ; 

public:

	virtual void Execute( const Graph& Graph , const string& VetexId ) override ; 

private : 

	// �����С·����ֵ�Ķ���
	inline Vertex* ExtractMin( vector< Vertex* >& Q ) ;

	// �ɳ�
	inline bool Relax( Vertex* v1 , Vertex* v2 , int Weight ) ;

public:

	void SetTarget( Vertex* pVTarget ) { m_pVTarget = pVTarget ; }

private: 

	Vertex* m_pVTarget ;

};

