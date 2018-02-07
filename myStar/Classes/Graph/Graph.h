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
	图顶点
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

	// 附加数据
	unordered_map< string , void*> UserData ;

public : 

	const unordered_map< string , Edge* >& GetEdgesOut( ) const { return m_EdgesOut ; }//获取节点的出边集合

	const unordered_map< string , Edge* >& GetEdgesIn( ) const { return m_EdgesIn ; }//获取节点的入边集合

	const string& GetId( ) const { return m_strId ; }//获取节点的id

	const string& GetText( ) const { return m_Text ; }//获取节点表识的字符串
	void SetText( const string& Text ) { m_Text = Text ; }//设置节点表识的字符串

	Graph * GetGraph( ) { return m_pGraph ; }//获取节点所属于的图
	
protected: 

	// 出边集合 key是顶点的id
	unordered_map< string , Edge* > m_EdgesOut ; 

	// 入边集合 key是顶点的id
	unordered_map< string , Edge* > m_EdgesIn ;

	// 节点表示的字符串
	string m_Text ; 

	// 节点的ID
	string m_strId ; 

	// 所属的图
	Graph * m_pGraph ; 

public : 

	// 寻路算法需要的数据
	struct Pathfinding
	{
		// 顶点的前驱顶点。
		Vertex * pParent ;

		// 路径代价估计
		int Cost ; 

		// 标识符
		int Flag ;

		// 启发式函数的计算出来的值
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
	图顶点的边
	有向边
	每条边都会被记录在三个集合里：顶点1的出边组合，顶点2的入边组合，边所有的集合。增加或者删除一条边时注意这三部分
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

	int GetWeight( ) const { return m_Weight ; }//获取边的权重
	void SetWeight( int var ) { m_Weight = var ; }//设置边的权重

	Vertex* GetStartVertex( ) const { return m_pStartVertex ; }//获取边的起点

	Vertex* GetEndVertex( ) const { return m_pEndVertex ; }//获取边的终点

protected:

	// 边的权值
	int m_Weight  ;

	// 起点的顶点
	Vertex * m_pStartVertex ;

	// 终点的顶点
	Vertex * m_pEndVertex ;

};



/*
	图.
	图会负责释放顶点和边的内存
*/
class Graph
{
public : 

	Graph( ) ; 
	~Graph( ) ;

public : 

	// 添加一个顶点
	void AddVertex( Vertex* pV ) ;

	// 删除一个顶点（还要删除出边和入边）
	void DeleleVertex( const string& VertexName ) ;


	// 添加一条边。返回边对象
	Edge* AddEdge( const string& Vertex1Name , const string& Vertex2Name , int Weight = 0 ) ;

	// 删除一条边
	void DeleteEdge( const string& StartVertexName , const string& EndVertexName ) ;

public : 

	const unordered_map< string , Vertex* >& GetVertexes( ) const { return m_Vertexes ; }

protected: 

	// 顶点的集合 key是id
	unordered_map< string , Vertex* > m_Vertexes ; //

	// 边的集合。Key的格式“顶点1name->顶点2name"
	unordered_map< string , Edge* > m_Edges ;

protected:

#define GetEdgeKey( pV1 , pV2 )(  pV1->m_strId + "->" + pV2->m_strId ) ; 

};