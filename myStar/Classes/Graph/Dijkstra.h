#pragma once
#include "GraphPathfinding.h"

class Dijkstra : public GraphPathfinding 
{
public:
	Dijkstra( );
	~Dijkstra( );

public:

public : 

	void Execute( const Graph& Graph , const string& VetexId ) override ;

private:

	// �����С·����ֵ�Ķ���
	inline Vertex* ExtractMin( vector< Vertex* >& Q ) ;

	// �ɳ�
	inline bool Relax( Vertex* v1 , Vertex* v2 , int Weight ) ;

public : 

	Result& GetResult( ) { return m_Ret  ; }

private : 

	Result m_Ret ; 

};

