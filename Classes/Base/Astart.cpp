#include "Astart.h"
#include <iostream>  
#include <cstdio>  
#include <stdio.h>
#include <cmath>  
#include <string>  
#include <fstream>  
#include "TVSailDefineVar.h"
using namespace std;  

const int MaxDistance = 99999; 

AStarPathFinding::AStarPathFinding() 
	:m_steps(0)  
{  
	/*m_mapFileName = (char *)malloc((strlen(mapFileName) + 1) * sizeof(char));  
	strcpy(m_mapFileName,mapFileName); */ 
	m_rows = 300;
	m_cols = 219;
	m_path = new int *[m_rows];  
	for (int i = 0;i < m_rows;++i)  
	{  
		m_path[i] = new int[m_cols]; 
	} 
}  //

AStarPathFinding::~AStarPathFinding()  
{  
	//free(m_mapFileName);  

	//千万不能有这句代码，因为startNode已加入OPEN表，会在释放OPEN表  
	//的时候释放，否则会造成重复释放，出现bug  
	//delete startNode;  
	delete endNode;  

	////释放迷宫布局数组：注意多维数组空间释放  
	//for (int i = 0;i < m_rows;++i)  
	//{  
	//	//delete[] MAPINDEX[i];  
	//}  
	//delete[] MAPINDEX;  

	for (int i = 0;i < m_rows;++i)  
	{  
		delete[] m_path[i];  
	}  
	delete[] m_path; 

	//释放OPEN表以及CLOSED表内存空间
	vector<CNode*>::iterator iter;  
	for (iter = OPENTable.begin();iter != OPENTable.end();++iter)  
	{  
		delete (*iter);  
	}  
	OPENTable.clear();  

	vector<CNode*>::iterator iter2;  
	for (iter2 = CLOSEDTable.begin();iter2 != CLOSEDTable.end();++iter2)  
	{  
		delete (*iter2);  
	}  
	CLOSEDTable.clear();  
}  

void AStarPathFinding::init(cocos2d::Point A,cocos2d::Point B)
{  
	m_startX = A.x;
	m_startY = A.y;
	m_endX = B.x;
	m_endY = B.y;

	dx[0] = -1;  
	dx[1] =dx[3] = 0;  
	dx[2]  = 1;  

	dy[3] = -1;  
	dy[0] =dy[2] = 0;  
	dy[1]  = 1;  

	
	
	//readMap();  

	//分配空间  
	/*m_path = new int *[m_rows];  
	for (int i = 0;i < m_rows;++i)  
	{  
	m_path[i] = new int[m_cols]; 
	}  */

	startNode = new CNode;  
	startNode->x = m_startX;  
	startNode->y = m_startY;  
	startNode->g = 0;  
	startNode->h = judge(startNode->x,startNode->y);  
	startNode->f = startNode->g + startNode->h;  
	startNode->father = NULL;  

	endNode = new CNode;  
	endNode->x = m_endX;  
	endNode->y = m_endY;  
	endNode->father = NULL;  
}  

std::vector<cocos2d::Point> AStarPathFinding::getEnablePath(cocos2d::Point startPos,cocos2d::Point endPos)
{
	m_vPaths.clear();
	init(Vec2(startPos.x,m_rows - startPos.y - 1),Vec2(endPos.x,m_rows - endPos.y - 1));
	pathFinding();
	return m_vPaths;
}

void AStarPathFinding::setRowsAndCols(int rows,int cols)
{
	m_rows = rows;
	m_cols = cols;
}
bool AStarPathFinding::pathFinding()  
{  
	//判断起始点和目标点是否是同一点   
	if (m_startX == m_endX && m_startY == m_endY)  
	{  
		cout << "WARNNING : The start point is the same as th destination " << endl;  
		return true;  
	}  

	OPENTable.push_back(startNode);//起始点装入OPEN表  

	//对vector中元素进行排序：将最后一个元素加入原本已序的heap内  
	push_heap(OPENTable.begin(),OPENTable.end(),HeapCompare_f());  

	CNode *tempNode = new CNode;  

	//开始遍历  
	for (;;)  
	{  
		if (OPENTable.empty())//判断OPEN表是否为空 
		{  
			cout << "ERROR : unable to find the destination" << endl; 
			log("ERROR : unable to find the destination");
			return false;  
		}  

		tempNode = OPENTable.front();//注意：OPEN表为空会导致未定义行为  
		++m_steps;  
		//将第一个元素移到最后，并将剩余区间重新排序，组成新的heap  
		pop_heap(OPENTable.begin(),OPENTable.end(),HeapCompare_f());  
		OPENTable.pop_back();//删除最后一个元素  

		//判断是否已经搜寻到目标节点  
		if (tempNode->x == m_endX && tempNode->y == m_endY)  
		{  
			cout << "OK : success to find the destination" << endl;  
			endNode->g = tempNode->g;  
			endNode->h = tempNode->h;  
			endNode->f = tempNode->f;  
			endNode->father = tempNode->father;  
			log("OK : success to find the destination");
			generatePath();  

			return true;  
		}  

		for (int i = 0;i < 4;++i)//针对每个子节点   
		{  
			int nextX = tempNode->x + dx[i];  
			int nextY = tempNode->y + dy[i];  
			if (isIllegle(nextX,nextY))  
			{  
				//注意：障碍物角落不能直接通过  
				if (1 != *(*(MAPINDEX + nextY)+ nextX))//0 == *(*(MAPINDEX + tempNode->x) + nextY) ||  
					//0 == *(*(MAPINDEX + nextX) + tempNode->y))  
				{  
					continue;  
				}  
				//计算此子节点的g值 
				int newGVal;  
				if (!dx[i] && !dy[i])//位于对角线上  
				{  
					newGVal = tempNode->g + 14;  
				}  
				else  
					newGVal = tempNode->g + 10;  

				//搜索OPEN表，判断此点是否在OPEN表中  
				vector<CNode*>::iterator OPENTableResult;  
				for (OPENTableResult = OPENTable.begin();  
					OPENTableResult != OPENTable.end();++OPENTableResult)  
				{  
					if ((*OPENTableResult)->x == nextX &&  
						(*OPENTableResult)->y == nextY)  
					{  
						break;  
					}  
				}  

				//此子节点已经存在于OPEN表中  
				if (OPENTableResult != OPENTable.end())  
				{  
					//OPEN表中节点的g值已经是最优的，则跳过此节点 
					if ((*OPENTableResult)->g <= newGVal)  
					{  
						continue;  
					}  
				}  

				//搜索CLOSED表，判断此节点是否已经存在于其中  
				vector<CNode*>::iterator CLOSEDTableResult;  
				for (CLOSEDTableResult = CLOSEDTable.begin();  
					CLOSEDTableResult != CLOSEDTable.end();++CLOSEDTableResult)  
				{  
					if ((*CLOSEDTableResult)->x == nextX &&  
						(*CLOSEDTableResult)->y == nextY)  
					{  
						break;  
					}  
				}  

				//此节点已经存在于CLOSED表中  
				if (CLOSEDTableResult != CLOSEDTable.end())  
				{  
					//CLOSED表中的节点已经是最优的，则跳过  
					if ((*CLOSEDTableResult)->g <= newGVal)  
					{  
						continue;  
					}  
					
				}  

				//此节点是迄今为止的最优节点  
				CNode *bestNode = new CNode;  
				bestNode->x = nextX;  
				bestNode->y = nextY;  
				bestNode->father = tempNode;  
				bestNode->g = newGVal;  
				bestNode->h = judge(nextX,nextY);  
				bestNode->f = bestNode->g + bestNode->h;  

				//如果已经存在于CLOSED表中，将其移除  
				if (CLOSEDTableResult != CLOSEDTable.end())  
				{  
					delete (*CLOSEDTableResult);  
					CLOSEDTable.erase(CLOSEDTableResult);  
				}  

				//如果已经存在于OPEN表，更新  
				if (OPENTableResult != OPENTable.end())  
				{  
					delete (*OPENTableResult);  
					OPENTable.erase(OPENTableResult);  

					//重新建堆，实现排序。注意不能用sort_heap，因为如果容器为空的话会出现bug
					make_heap(OPENTable.begin(),OPENTable.end(),HeapCompare_f());  
				}  

				OPENTable.push_back(bestNode); //将最优节点放入OPEN表

				push_heap(OPENTable.begin(),OPENTable.end(),HeapCompare_f());//重新排序  
			}  
		}  

		CLOSEDTable.push_back(tempNode);  
	}  

	return false;  
}  

void AStarPathFinding::generatePath()  
{  
	CNode *nodeChild = endNode;  
	CNode *nodeParent = endNode->father;  
	do   
	{  
		*(*(m_path + nodeChild->y) + nodeChild->x) = NODE_PATH;//标记为路径上的点
		m_vPaths.push_back(Vec2(nodeChild->x,m_rows - nodeChild->y - 1));// save path point
		nodeChild = nodeParent;  
		nodeParent = nodeParent->father;  
	} while (nodeChild != startNode);
	m_vPaths.push_back(Vec2(nodeChild->x,m_rows - nodeChild->y - 1));// save path point
	std::reverse(m_vPaths.begin(),m_vPaths.end());
	*(*(m_path + startNode->y) + startNode->x) = NODE_PATH;//标记为路径上的点 
	/*  origin code

	CNode *nodeChild = endNode;  
	CNode *nodeParent = endNode->father;  
	do   
	{  
	*(*(m_path + nodeChild->y) + nodeChild->x) = NODE_PATH;//标记为路径上的点 
	m_vPaths.push_back(Vec2(nodeChild->x,nodeChild->y));// save path point
	nodeChild = nodeParent;  
	nodeParent = nodeParent->father;  
	} while (nodeChild != startNode);  

	*(*(m_path + startNode->y) + startNode->x) = NODE_PATH;//标记为路径上的点 
	m_vPaths.push_back(Vec2(nodeChild->x,nodeChild->y));// save path point
	*/
}  

// for test
void AStarPathFinding::printPath()  
{  
	//m_vPaths.clear();
	cout << "The path is " << endl;  
	ofstream fileread("wwww.txt");
	for (int i = 0;i < m_rows;++i)  
	{  
		for (int j = 0;j < m_cols;++j)  
		{  
			if (NODE_PATH == *(*(m_path + i) + j))  
			{  
				cout << "# ";  
				fileread <<"#";
				//m_vPaths.push_back(Vec2(j,m_rows - i -1));
				//fileread<<j<<":"<<(m_rows - i)<<endl;
			}  else { 
				cout << "0";//*(*(MAPINDEX + i) + j);  
				fileread<<"0";
			}
		}  
		cout << endl; 
		fileread<<endl;
	} 
	//std::reverse(m_vPaths.begin(),m_vPaths.end());
	//fileread.flush();
	//fileread.close();
	cout << "搜索总步数："    << m_steps << endl;  
}  

bool AStarPathFinding::readMap()  
{  
	//从文本文件读取迷宫布局信息 
	//ifstream mapFileStream(m_mapFileName);  
	//if (!mapFileStream)  
	//{  
	//	cerr << "ERROR : unable to open map file" << endl;  
	//	return false;  
	//}  

	//mapFileStream 
	
	m_rows >> m_cols;  
	//ofstream mapfile("wang.txt");
	//if (m_startX == 0)
	//{
	//	m_startX = 21;
	//	m_startY = 28;
	//	m_endX = 86;
	//	m_endY = 46;
	//}
	//m_rows = 64;
	//m_cols = 96;
	//// 生成地图文件
	//TMXTiledMap* map = TMXTiledMap::create("sailing.tmx"); 
	//TMXLayer* layer = map->getLayer("warter");
	//////多维数组空间分配
	//MAPINDEX = new int *[m_rows];  
	//for (int i = 0;i < m_rows;++i)  
	//{  
	//	MAPINDEX[i] = new int[m_cols];  
	//}  

	////mapFileStream >> m_startX >> m_startY;  
	////mapFileStream >> m_endX >> m_endY;  

	//for (int i = 0;i < m_rows;++i)  
	//{  
	//	for (int j = 0;j < m_cols;++j)  
	//	{  
	//		//mapFileStream >> *(*(MAPINDEX + i) + j);  
	//		int gid = layer->getTileGIDAt(Vec2(j, i));
	//		if (gid)
	//		{
	//			*(*(MAPINDEX + i) + j) = 1;
	//		}else
	//		{
	//			*(*(MAPINDEX + i) + j) = 0;
	//		}
	//		mapfile<<(*(*(MAPINDEX + i) + j));
	//		//printf("%d",(*(*(MAPINDEX + i) + j)));
	//	}  
	//	mapfile<<endl;
	//}  
	//mapfile.flush();
	//mapfile.close();
	return true;  
}  

int AStarPathFinding::judge(int x, int y)  
{  
	return (10 * (abs(m_endX - x) + abs(m_endY - y)));  
}  

bool AStarPathFinding::isIllegle(int x, int y)  
{  
	if (x >= 0 && x < m_cols &&  
		y >= 0 && y < m_rows &&  
		*(*(MAPINDEX + y) + x) == 1)  
		return true;  
	else  
		return false;  
}
