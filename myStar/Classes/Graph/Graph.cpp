#include "Graph.h"

Graph::Graph( )
{

}

Graph::~Graph( )
{
	// �ͷ����ж���
	for ( auto& kv : m_Vertexes )
	{
		delete kv.second ; 
	}

	// ʩ�����еı�
	for ( auto& kv : m_Edges )
	{
		delete kv.second ; 
	}
}

void Graph::AddVertex( Vertex* pV )
{
	if ( m_Vertexes.find( pV->GetId( ) ) != m_Vertexes.end( ) )
	{
		// �Ѿ�������ͬKey�Ķ���
	}

	m_Vertexes[ pV->GetId( ) ] = pV ; 
	pV->m_pGraph = this ;
}

void Graph::DeleleVertex( const string& VertexName )
{
	Vertex *pV = m_Vertexes.find( VertexName )->second ; 

	// ����Ҫɾ���Ľڵ�ĳ���
	for ( auto it = pV->m_EdgesOut.begin( ) , end = pV->m_EdgesOut.end( ) ; it != end ; ++it )
	{
		Edge *pEdgeOut = it->second ;

		// ɾ����߼�¼
		pEdgeOut->m_pEndVertex->m_EdgesIn.erase( VertexName ) ;

		// �ڱ߼����б���ɾ��
		string key = GetEdgeKey( pV , pEdgeOut->m_pEndVertex ) ; 
		m_Edges.erase( key ) ; 

		// ɾ���߶���
		delete pEdgeOut ; 
	}

	delete pV ; 

}



Edge * Graph::AddEdge( const string& Vertex1Name , const string& Vertex2Name , int Weight /*= 0 */ )
{
	Vertex *pV1 = m_Vertexes.find( Vertex1Name )->second ;
	Vertex *pV2 = m_Vertexes.find( Vertex2Name )->second ;

	// ����߼���
	Edge *pEdge = new Edge( pV1 , pV2 , Weight ) ;
	string key = GetEdgeKey( pV1 , pV2 ) ; 
	m_Edges[ key ] = pEdge ; 

	// ����V1�ĳ���
	pV1->m_EdgesOut[ Vertex2Name ] = pEdge ;

	// ����V2�����
	pV2->m_EdgesIn[ Vertex1Name ] = pEdge ;

	return pEdge ; 

}

void Graph::DeleteEdge( const string& StartVertexName , const string& EndVertexName )
{
	Vertex *pV1 = m_Vertexes.find( StartVertexName )->second ;
	Vertex *pV2 = m_Vertexes.find( EndVertexName )->second ;

	string key = GetEdgeKey( pV1 , pV2 ) ;

	Edge *pEdge = m_Edges.find( key )->second ; 

	// �ڶ���1�ĳ����б���ɾ��
	pV1->m_EdgesOut.erase( EndVertexName ) ; 
	
	// �ڶ���2������б���ɾ��
	pV2->m_EdgesIn.erase( StartVertexName ) ;

	// �ڱ߼����б���ɾ��
	m_Edges.erase( key ) ; 

	delete pEdge ; 

}
