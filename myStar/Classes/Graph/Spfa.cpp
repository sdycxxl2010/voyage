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
	// ȡ��ͼ�Ķ��㼯��
	const auto& Vertexes = Graph.GetVertexes( ) ; 
	//  ȡ����ʼ�������
	Vertex *pVStart = Vertexes.find( VetexId )->second   ;

	// Spfa�㷨��Ҫһ�����б��涥��
	queue< Vertex* > Q ; 

	// ��ʼ��
	for ( auto& it : Vertexes )
	{
		Vertex *pV = it.second ; 

		pV->PathfindingData.Cost = 0x0FFFFFFF ;
		//IsInQueue[ pV ] = false ; 
		pV->PathfindingData.Flag = false ;
		pV->PathfindingData.pParent = 0 ; // ����ĸ�·��������Ϊ��
	}
	pVStart->PathfindingData.Cost = 0 ;			// ��ʼ�����·������Ϊ0
	pVStart->PathfindingData.Flag = true ;		// ��ʼ�����ڶ�����
	//m_Ret.PathTree[ pVStart ] = 0 ;				//  ��ʼ����ĸ�·��Ϊ��
	Q.push( pVStart ) ;									// ��ʼ���������
	

	// spfa�㷨
	for ( ; Q.size( ) ;  )
	{
		auto pStartVertex = Q.front( ) ; Q.pop( ) ;	// ���е���һ������v
		pStartVertex->PathfindingData.Flag = false ;

		// �ɳ�v�����г���
		const auto& Eo = pStartVertex->GetEdgesOut( ) ;
		for ( auto& it : Eo )
		{
			auto pEdge = it.second ; 
			auto pEndVertex = pEdge->GetEndVertex( ) ;
			bool bRelaxRet = Relax( pStartVertex , pEndVertex , pEdge->GetWeight( ) ) ;
			if ( bRelaxRet )
			{
				// ������ڳ����ɳڳɹ����ҳ��߶�Ӧ���յ㶥�㲻�ڶ����еĻ����Ͳ����β
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
		// ����·������
		pEndVertex->PathfindingData.Cost = n ;
		// ����·��
		//m_Ret.PathTree[ pEndVertex ] = pStartVertex ; 
		pEndVertex->PathfindingData.pParent = pStartVertex ;

		return true ;
	}

	return false ; 
}
