#include "Dijkstra.h"
#include <queue>

Dijkstra::Dijkstra( )
{
}

Dijkstra::~Dijkstra( )
{
}

void Dijkstra::Execute( const Graph& Graph , const string& VetexId  )
{
	const auto& Vertexes = Graph.GetVertexes( ) ; 
	Vertex* pVertexStart = Vertexes.find( VetexId )->second ; 
	vector< Vertex* > Q ; 

	// ��ʼ������
	for ( auto& it : Vertexes )
	{
		it.second->PathfindingData.Cost = 0x0FFFFFFF ;
		pVertexStart->PathfindingData.pParent = 0 ;
	}
	// ��ʼ����ʼ����
	pVertexStart->PathfindingData.Cost = 0 ;
	pVertexStart->PathfindingData.pParent = 0 ; 
	// ����ʼ��������б���
	Q.push_back( pVertexStart ) ;
	pVertexStart->PathfindingData.Flag = true ; 

	for ( ; Q.size() > 0 ; )
	{
		// ѡ����С·�����ƵĶ���
		auto v = ExtractMin( Q ) ;
		v->PathfindingData.Flag = false ; 

		// �����еĳ��߽��С��ɳڡ�
		const auto& EO = v->GetEdgesOut( ) ; 
		for (  auto& it : EO )
		{
			Edge* pEdge = it.second ; 
			Vertex* pVEnd = pEdge->GetEndVertex( ) ;

			bool bRet = Relax( v , pVEnd , pEdge->GetWeight( ) ) ;
			// ����ɳڳɹ��������б��С�
			if ( bRet && pVEnd->PathfindingData.Flag == false )
			{
				Q.push_back( pVEnd ) ;
				pVEnd->PathfindingData.Flag = true ;
			}
		}
		// end for
	}
	// end for

}

Vertex* Dijkstra::ExtractMin( vector< Vertex* >& Q )
{
	Vertex* Ret = 0 ; 

	Ret = Q[ 0 ] ;
	int pos = 0 ; 
	for ( int i = 1 , size = Q.size( ) ; i < size ; ++i )
	{
		if ( Ret->PathfindingData.Cost > Q[ i ]->PathfindingData.Cost )
		{
			Ret = Q[ i ] ; 
			pos = i ; 
		}
	}

	Q.erase( Q.begin() + pos ) ;

	return Ret ;

}



bool Dijkstra::Relax( Vertex* v1 , Vertex* v2 , int Weight )
{
	int n = v1->PathfindingData.Cost + Weight ;
	if ( n < v2->PathfindingData.Cost )
	{
		v2->PathfindingData.Cost = n ;

		//m_Ret.PathTree[ v2 ] = v1 ; 
		v2->PathfindingData.pParent = v1 ; 

		return true ; 
	}

	return false ; 

}
