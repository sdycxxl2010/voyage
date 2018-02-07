#pragma once
#include <vector>
#include <unordered_map>
#include <string>
using namespace std ;

class Edge ; 
class Vertex ; 
class Graph ;
class GraphPathfinding ;

/*
	ͼ����
*/
class Vertex
{
	friend class Graph ;
	
public:

	Vertex( const string& Name )
	{
		m_strId = Name ;

		m_pGraph = 0 ;
	}

	~Vertex( ) { };

public:

	// ��������
	unordered_map< string , void*> UserData ;

public : 

	const unordered_map< string , Edge* >& GetEdgesOut( ) const { return m_EdgesOut ; }//��ȡ�ڵ�ĳ��߼���

	const unordered_map< string , Edge* >& GetEdgesIn( ) const { return m_EdgesIn ; }//��ȡ�ڵ����߼���

	const string& GetId( ) const { return m_strId ; }//��ȡ�ڵ��id

	const string& GetText( ) const { return m_Text ; }//��ȡ�ڵ��ʶ���ַ���
	void SetText( const string& Text ) { m_Text = Text ; }//���ýڵ��ʶ���ַ���

	Graph * GetGraph( ) { return m_pGraph ; }//��ȡ�ڵ������ڵ�ͼ
	
protected: 

	// ���߼��� key�Ƕ����id
	unordered_map< string , Edge* > m_EdgesOut ; 

	// ��߼��� key�Ƕ����id
	unordered_map< string , Edge* > m_EdgesIn ;

	// �ڵ��ʾ���ַ���
	string m_Text ; 

	// �ڵ��ID
	string m_strId ; 

	// ������ͼ
	Graph * m_pGraph ; 

public : 

	// Ѱ·�㷨��Ҫ������
	struct Pathfinding
	{
		// �����ǰ�����㡣
		Vertex * pParent ;

		// ·�����۹���
		int Cost ; 

		// ��ʶ��
		int Flag ;

		// ����ʽ�����ļ��������ֵ
		int Heuristic ; 

		Pathfinding( )
		{
			pParent = 0 ;
			Cost = 0 ; 
			Flag = 0 ; 
			Heuristic = 0 ;
		}
	}
	PathfindingData ;

};




/*
	ͼ����ı�
	�����
	ÿ���߶��ᱻ��¼���������������1�ĳ�����ϣ�����2�������ϣ������еļ��ϡ����ӻ���ɾ��һ����ʱע����������
*/
class Edge
{
	friend class Graph ;

public:

	Edge( )
	{
		m_Weight = 0 ;

		m_pStartVertex = m_pEndVertex = 0 ;
	}

	Edge( Vertex* pStartVertex , Vertex* pEndVertex , int Weight = 0 )
	{
		m_Weight = Weight ;

		m_pStartVertex = pStartVertex ;
		m_pEndVertex = pEndVertex ;
	}

public:

	int GetWeight( ) const { return m_Weight ; }//��ȡ�ߵ�Ȩ��
	void SetWeight( int var ) { m_Weight = var ; }//���ñߵ�Ȩ��

	Vertex* GetStartVertex( ) const { return m_pStartVertex ; }//��ȡ�ߵ����

	Vertex* GetEndVertex( ) const { return m_pEndVertex ; }//��ȡ�ߵ��յ�

protected:

	// �ߵ�Ȩֵ
	int m_Weight  ;

	// ���Ķ���
	Vertex * m_pStartVertex ;

	// �յ�Ķ���
	Vertex * m_pEndVertex ;

};



/*
	ͼ.
	ͼ�Ḻ���ͷŶ���ͱߵ��ڴ�
*/
class Graph
{
public : 

	Graph( ) ; 
	~Graph( ) ;

public : 

	// ���һ������
	void AddVertex( Vertex* pV ) ;

	// ɾ��һ�����㣨��Ҫɾ�����ߺ���ߣ�
	void DeleleVertex( const string& VertexName ) ;


	// ���һ���ߡ����ر߶���
	Edge* AddEdge( const string& Vertex1Name , const string& Vertex2Name , int Weight = 0 ) ;

	// ɾ��һ����
	void DeleteEdge( const string& StartVertexName , const string& EndVertexName ) ;

public : 

	const unordered_map< string , Vertex* >& GetVertexes( ) const { return m_Vertexes ; }

protected: 

	// ����ļ��� key��id
	unordered_map< string , Vertex* > m_Vertexes ; //

	// �ߵļ��ϡ�Key�ĸ�ʽ������1name->����2name"
	unordered_map< string , Edge* > m_Edges ;

protected:

#define GetEdgeKey( pV1 , pV2 )(  pV1->m_strId + "->" + pV2->m_strId ) ; 

};