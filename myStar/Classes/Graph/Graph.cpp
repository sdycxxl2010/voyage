#include "Graph.h"

Graph::Graph( )
{

}

Graph::~Graph( )
{
	// 释放所有顶点
	for ( auto& kv : m_Vertexes )
	{
		delete kv.second ; 
	}

	// 施放所有的边
	for ( auto& kv : m_Edges )
	{
		delete kv.second ; 
	}
}

void Graph::AddVertex( Vertex* pV )
{
	if ( m_Vertexes.find( pV->GetId( ) ) != m_Vertexes.end( ) )
	{
		// 已经存在相同Key的顶点
	}

	m_Vertexes[ pV->GetId( ) ] = pV ; 
	pV->m_pGraph = this ;
}

void Graph::DeleleVertex( const string& VertexName )
{
	Vertex *pV = m_Vertexes.find( VertexName )->second ; 

	// 遍历要删除的节点的出边
	for ( auto it = pV->m_EdgesOut.begin( ) , end = pV->m_EdgesOut.end( ) ; it != end ; ++it )
	{
		Edge *pEdgeOut = it->second ;

		// 删除入边记录
		pEdgeOut->m_pEndVertex->m_EdgesIn.erase( VertexName ) ;

		// 在边集合列表中删除
		string key = GetEdgeKey( pV , pEdgeOut->m_pEndVertex ) ; 
		m_Edges.erase( key ) ; 

		// 删除边对象
		delete pEdgeOut ; 
	}

	delete pV ; 

}



Edge * Graph::AddEdge( const string& Vertex1Name , const string& Vertex2Name , int Weight /*= 0 */ )
{
	Vertex *pV1 = m_Vertexes.find( Vertex1Name )->second ;
	Vertex *pV2 = m_Vertexes.find( Vertex2Name )->second ;

	// 加入边集合
	Edge *pEdge = new Edge( pV1 , pV2 , Weight ) ;
	string key = GetEdgeKey( pV1 , pV2 ) ; 
	m_Edges[ key ] = pEdge ; 

	// 加入V1的出边
	pV1->m_EdgesOut[ Vertex2Name ] = pEdge ;

	// 加入V2的入边
	pV2->m_EdgesIn[ Vertex1Name ] = pEdge ;

	return pEdge ; 

}

void Graph::DeleteEdge( const string& StartVertexName , const string& EndVertexName )
{
	Vertex *pV1 = m_Vertexes.find( StartVertexName )->second ;
	Vertex *pV2 = m_Vertexes.find( EndVertexName )->second ;

	string key = GetEdgeKey( pV1 , pV2 ) ;

	Edge *pEdge = m_Edges.find( key )->second ; 

	// 在顶点1的出边列表中删除
	pV1->m_EdgesOut.erase( EndVertexName ) ; 
	
	// 在顶点2的入边列表中删除
	pV2->m_EdgesIn.erase( StartVertexName ) ;

	// 在边集合列表中删除
	m_Edges.erase( key ) ; 

	delete pEdge ; 

}
