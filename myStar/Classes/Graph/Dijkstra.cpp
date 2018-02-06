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

	// 初始化顶点
	for ( auto& it : Vertexes )
	{
		it.second->PathfindingData.Cost = 0x0FFFFFFF ;
		pVertexStart->PathfindingData.pParent = 0 ;
	}
	// 初始化起始顶点
	pVertexStart->PathfindingData.Cost = 0 ;
	pVertexStart->PathfindingData.pParent = 0 ; 
	// 把起始顶点放入列表中
	Q.push_back( pVertexStart ) ;
	pVertexStart->PathfindingData.Flag = true ; 

	for ( ; Q.size() > 0 ; )
	{
		// 选出最小路径估计的顶点
		auto v = ExtractMin( Q ) ;
		v->PathfindingData.Flag = false ; 

		// 对所有的出边进行“松弛”
		const auto& EO = v->GetEdgesOut( ) ; 
		for (  auto& it : EO )
		{
			Edge* pEdge = it.second ; 
			Vertex* pVEnd = pEdge->GetEndVertex( ) ;

			bool bRet = Relax( v , pVEnd , pEdge->GetWeight( ) ) ;
			// 如果松弛成功，加入列表中。
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
