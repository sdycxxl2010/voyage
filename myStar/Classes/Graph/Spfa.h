#pragma once

#include "Graph\GraphPathfinding.h"

class Spfa :
	public GraphPathfinding
{
public:
	Spfa( );
	~Spfa( );

public : 

	virtual void Execute( const Graph& Graph , const string& VetexId ) ; 

private:

	inline bool Relax( Vertex* pStartVertex , Vertex* pEndVertex , int Weight ) ;

};

