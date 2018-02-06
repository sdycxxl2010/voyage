#include "Spfa.h"
#include <queue>
using namespace std ;

Spfa::Spfa( )
{
}


Spfa::~Spfa( )
{
}

void Spfa::Execute( const Graph& Graph , const string& VetexId )
{
	// 取得图的顶点集合
	const auto& Vertexes = Graph.GetVertexes( ) ; 
	//  取得起始顶点对象
	Vertex *pVStart = Vertexes.find( VetexId )->second   ;

	// Spfa算法需要一个队列保存顶点
	queue< Vertex* > Q ; 

	// 初始化
	for ( auto& it : Vertexes )
	{
		Vertex *pV = it.second ; 

		pV->PathfindingData.Cost = 0x0FFFFFFF ;
		//IsInQueue[ pV ] = false ; 
		pV->PathfindingData.Flag = false ;
		pV->PathfindingData.pParent = 0 ; // 顶点的父路径都设置为空
	}
	pVStart->PathfindingData.Cost = 0 ;			// 起始顶点的路径代价为0
	pVStart->PathfindingData.Flag = true ;		// 起始顶点在队列中
	//m_Ret.PathTree[ pVStart ] = 0 ;				//  起始顶点的父路径为空
	Q.push( pVStart ) ;									// 起始顶点先入队
	

	// spfa算法
	for ( ; Q.size( ) ;  )
	{
		auto pStartVertex = Q.front( ) ; Q.pop( ) ;	// 队列弹出一个顶点v
		pStartVertex->PathfindingData.Flag = false ;

		// 松弛v的所有出边
		const auto& Eo = pStartVertex->GetEdgesOut( ) ;
		for ( auto& it : Eo )
		{
			auto pEdge = it.second ; 
			auto pEndVertex = pEdge->GetEndVertex( ) ;
			bool bRelaxRet = Relax( pStartVertex , pEndVertex , pEdge->GetWeight( ) ) ;
			if ( bRelaxRet )
			{
				// 如果对于出边松弛成功，且出边对应的终点顶点不在队列中的话，就插入队尾
				if ( pEndVertex->PathfindingData.Flag == false )
				{
					Q.push( pEndVertex ) ;
					pEndVertex->PathfindingData.Flag = false ;
				}

			}

		}
		// end for

	}
	// end for


}

bool Spfa::Relax( Vertex* pStartVertex , Vertex* pEndVertex , int Weight )
{
	int n = pStartVertex->PathfindingData.Cost + Weight ;
	if ( n < pEndVertex->PathfindingData.Cost )
	{
		// 更新路径代价
		pEndVertex->PathfindingData.Cost = n ;
		// 更新路径
		//m_Ret.PathTree[ pEndVertex ] = pStartVertex ; 
		pEndVertex->PathfindingData.pParent = pStartVertex ;

		return true ;
	}

	return false ; 
}
