
#include "AStar.h"
#include <vector>
//#include <Windows.h>
#include <iostream>

using namespace std;

double mult(Point a, Point b, Point c)
{
	return (a.x-c.x)*(b.y-c.y)-(b.x-c.x)*(a.y-c.y);
}

CHeap::CHeap()
{

}

//判断二叉堆是否为空
bool CHeap::empty ()
{
	if (v.size () == 0)
		return true;
	else
		return false;
}

//清空二叉堆
void CHeap::clear()
{
	if (v.size () != 0)
		v.clear ();
}

//返回二叉堆中的第一个元素（即F值最低的元素）
Coordinate CHeap::begin()
{
	return v[0];
}

//删除二叉堆的堆首元素（即F值最低的元素）
void CHeap::removeFront ()
{
	if (v.size() == 0)
		return ;
	
	mMap[v[v.size() - 1].sx][v[v.size() - 1].sy].iHeapPosition=0;
	mMap[v[0].sx][v[0].sy].iHeapPosition=-1;

	v[0] = v[v.size() - 1];      //用最后一个元素把第一个元素覆盖掉，即为删除
	v.pop_back();                //删除容器尾巴元素

	

	int currentIndex = 0;
	while (currentIndex < v.size()) //把新的堆首元素放在堆中适当的位置
	{
		int leftChildIndex = 2 * currentIndex + 1;
		int rightChildIndex = 2 * currentIndex + 2;

		//已经到最底层，结束
		if (rightChildIndex > v.size())
			break;
		int minIndex = leftChildIndex;
		
		//有两个孩子，找出两个孩子节点中F值最低的元素
		if (rightChildIndex<v.size() && (mMap[v[minIndex].sx][v[minIndex].sy].getF()  > mMap[v[rightChildIndex].sx][v[rightChildIndex].sy].getF()))
		{
			minIndex = rightChildIndex;
		}
		
		//如果当前节点的F值 大于 他孩子节点的F值，则交换
		if (mMap[v[currentIndex].sx][v[currentIndex].sy].getF() > mMap[v[minIndex].sx][v[minIndex].sy].getF())
		{
			Coordinate temp = v[minIndex];
			v[minIndex] = v[currentIndex];
			v[currentIndex] = temp;

			//同步保存地图中该坐标在堆中的最新位置
			mMap[v[currentIndex].sx][v[currentIndex].sy].iHeapPosition = currentIndex; 
            mMap[v[minIndex].sx][v[minIndex].sy].iHeapPosition = minIndex;
			currentIndex = minIndex;
		}
		else
		{
			break;
		}
	}
}

//返回堆中的最小F值
int CHeap::getMinF() 
{
	if (v.size() > 0)
		return mMap[v[0].sx][v[0].sx].getF();
	else
		return 0;
}


//往堆中添加新的元素（节点）
void CHeap::push_back (Coordinate element)
{
	v.push_back(element);//把新节点添加到堆的末尾
	int currentIndex = v.size() - 1;
	mMap[v[currentIndex].sx][v[currentIndex].sy].iHeapPosition = currentIndex; //保存该坐标在堆中的位置

	while (currentIndex > 0) //不断的与他的父节点比较，直到该新节点的F值大于他的父节点的F值为止 或者 该新节点到了堆首
	{
		int parentIndex = (currentIndex - 1) / 2;
		if (mMap[v[currentIndex].sx][v[currentIndex].sy].getF() < mMap[v[parentIndex].sx][v[parentIndex].sy].getF())
		{
			Coordinate temp = v[currentIndex];
			v[currentIndex] = v[parentIndex];
			v[parentIndex] = temp;

			//同步保存地图中该坐标在堆中的最新位置
			mMap[v[currentIndex].sx][v[currentIndex].sy].iHeapPosition = currentIndex;
			mMap[v[parentIndex].sx][v[parentIndex].sy].iHeapPosition = parentIndex;
			currentIndex = parentIndex;
			continue;
		}
		else
		{
			break;
		}
	}
}

//当堆中某元素的F值发生改变时，更新该元素在堆中的位置
void CHeap::newHeap (int position)
{
	int currentIndex = position;
	int parentIndex;
	//while (currentIndex > 0) //如果该元素新的F值比他的父节点的F值小，交换
	//{
	//	parentIndex = (currentIndex - 1) / 2;
	//	if (g_Map[v[currentIndex].sx][v[currentIndex].sy].getF()  < g_Map[v[parentIndex].sx][v[parentIndex].sy].getF())
	//	{
	//		Coordinate temp = v[currentIndex];
	//		v[currentIndex] = v[parentIndex];
	//		v[parentIndex] = temp;
	//		g_Map[v[currentIndex].sx][v[currentIndex].sy].iHeapPosition = parentIndex;
	//		g_Map[v[parentIndex].sx][v[parentIndex].sy].iHeapPosition = currentIndex;
	//		currentIndex = parentIndex;
	//		continue;
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
	Coordinate temp;
	//如果上面的循环没有执行，则无法判断该节点的最新F值的相对大小，
	//所以，此时需要把该节点移动到堆首删除掉，然后再在堆末尾添加该节点，最后程序再把该节点移动到堆中适当的位置处
	if (currentIndex == position) 
	{
		while (currentIndex > 0) //把该节点移动到堆首
		{
			parentIndex = (currentIndex - 1) / 2;
			temp = v[currentIndex];
			v[currentIndex] = v[parentIndex];
			v[parentIndex] = temp;
			mMap[v[currentIndex].sx][v[currentIndex].sy].iHeapPosition = currentIndex;
			mMap[v[parentIndex].sx][v[parentIndex].sy].iHeapPosition = parentIndex;
			currentIndex = parentIndex;
		}
	}
	temp = v[0];
	removeFront (); //删除该节点
	push_back (temp);//重新在堆中插入该节点
}

//返回堆中元素的个数
int CHeap::getSize ()
{
	return v.size();
}

CMap::~CMap(){
	for(int i = 0; i < TOTAL_MAP_WIDTH; i++)
		delete []g_Map[i];
	delete []g_Map;
	if(!selfShip) delete selfShip;
}

//构造函数
CMap::CMap(int width,int height)
{
	m_isShow = false;
	duration = 0;
    m_Mode = MAP_RUN;
	m_StartX = m_StartY = 0;
	m_EndX = m_EndY = 0;
	m_i = -1;
	selfShip = 0;
	TOTAL_MAP_WIDTH = width/SCREEN_SCALE;
	TOTAL_MAP_HEIGHT = height /SCREEN_SCALE;

	g_Map = new MapNode*[TOTAL_MAP_WIDTH];
	for(int i = 0; i < TOTAL_MAP_WIDTH; i++)
		g_Map[i] = new MapNode[TOTAL_MAP_HEIGHT];
	m_iOpenList.init(g_Map);
}

//初始化寻路需要用到的STL容器
void CMap::initSTL ()
{
	if (!m_iOpenList.empty()) //清空开启列表
		m_iOpenList.clear();

	if (!m_iRightRoad.empty()) //清空储存的路径
		m_iRightRoad.clear();

	if(!m_road.empty())
		m_road.clear();
}

void CMap::removeAllWallShip(){
	for (int x = 0; x < TOTAL_MAP_WIDTH; ++x)
	{
		for (int y = 0; y < TOTAL_MAP_HEIGHT; ++y)
		{
			g_Map[x][y].iColor = -1;
		}
	}
	blockLines.clear();
}

//每次寻路前，清空地图中每个格子储存的相关数据
void CMap::initMap () 
{
	for (int x = 0; x < TOTAL_MAP_WIDTH; ++x)
	{
		for (int y = 0; y < TOTAL_MAP_HEIGHT; ++y)
		{
			g_Map[x][y].iHeapPosition = -1;
			g_Map[x][y].iH = -1;
			g_Map[x][y].iG = -1;
			g_Map[x][y].iF = -1;
			g_Map[x][y].isOpen = IS_UNKNOWN;
			g_Map[x][y].father.sx = -1;
            g_Map[x][y].father.sy = -1;
		}
	}
}

void CMap::setWallData(ShipPoints*p)
{
	drawLine1(p->mp1,p->mp2);
	drawLine1(p->mp3,p->mp2);
	drawLine1(p->mp3,p->mp4);
	drawLine1(p->mp1,p->mp4);
}

//随机初始化地图
void CMap::resetMap ()
{
	m_i = -1;
	m_StartX = m_StartY = -1;
	m_EndX = m_EndY = -1;
	m_isShow = false;
	initSTL ();

// 	int x = 150;
// 	for (int y = 10; y < 120; ++y)
// 	{
// 
// 		g_Map[x][y].iColor = MAP_WALL;
// 		
// 	}
// 
// 	x = 50;
// 	for (int y = 50; y < 90; ++y)
// 	{
// 
// 		g_Map[x][y].iColor = MAP_WALL;
// 
// 	}
// 
// 	int y = 20;
// 	for (int x = 50; x < 90; ++x)
// 	{
// 
// 		g_Map[x][y].iColor = MAP_WALL;
// 
// 	}
}
#define  ROTATE_NEW_X(oldx,oldy) (oldx*cos(delta) + oldy*sin(delta))
#define  ROTATE_NEW_Y(oldx,oldy) (oldy*cos(delta) - oldx*sin(delta))

//double newy = oldy*cos(delta) - oldx*sin(delta);
bool CMap::checkIfTargetInSelf(int xCurrent,int yCurrent,int xTarget,int yTarget){
	double delta = 0;//
	if(xTarget == xCurrent){
		delta = 3.1415926 / 2;
		if(yTarget < yCurrent){
			delta = 0-delta;
		}
	}else {
		delta = atan((yTarget-yCurrent )*1.0/(double)(xTarget-xCurrent));
	}

	double rx1 = rSelf*cos(theta+delta);
	double ry1 = rSelf*sin(theta+delta);

	double rx2 = rSelf*cos(theta-delta);
	double ry2 = 0 - rSelf*sin(theta-delta);

	double x1 = xCurrent + rx1;
	double y1 = yCurrent + ry1;

	double x2 = xCurrent + rx2;
	double y2 = yCurrent + ry2;

	double x3 = xCurrent - rx1;
	double y3 = yCurrent - ry1;

	double x4 = xCurrent - rx2;
	double y4 = yCurrent - ry2;

	double newx1 = ROTATE_NEW_X(x1,y1);
	double newy1 = ROTATE_NEW_Y(x1,y1);

	double newx2 = ROTATE_NEW_X(x2,y2);
	double newy2 = ROTATE_NEW_Y(x2,y2);

	double newx3 = ROTATE_NEW_X(x3,y3);
	double newy3 = ROTATE_NEW_Y(x3,y3);

	double newx4 = ROTATE_NEW_X(x4,y4);
	double newy4 = ROTATE_NEW_Y(x4,y4);

	double xmin = newx1;
	if(xmin > newx2) xmin = newx2;
	if(xmin > newx3) xmin = newx3;
	if(xmin > newx4) xmin = newx4;

	double ymin = newy1;
	if(ymin > newy2) ymin = newy2;
	if(ymin > newy3) ymin = newy3;
	if(ymin > newy4) ymin = newy4;

	double xmax = newx1;
	if(xmax < newx2) xmax = newx2;
	if(xmax < newx3) xmax = newx3;
	if(xmax < newx4) xmax = newx4;

	double ymax = newy1;
	if(ymax < newy2) ymax = newy2;
	if(ymax < newy3) ymax = newy3;
	if(ymax < newy4) ymax = newy4;


	double newTargetX =  ROTATE_NEW_X(xTarget,yTarget);
	double newTargetY =  ROTATE_NEW_Y(xTarget,yTarget);

	if(newTargetX>= xmin && newTargetX <= xmax && newTargetY<= ymax && newTargetY>=ymin){
		return true;
	}
	return false;
}

bool CMap::checkSelf(int xCurrent,int yCurrent,int xTarget,int yTarget){
	//return true;
	
	double delta = 0;//
	if(xTarget == xCurrent){
		delta = 3.1415926 / 2;
		if(yTarget < yCurrent){
			delta = 0-delta;
		}
	}else {
		delta = atan((yTarget-yCurrent )*1.0/(double)(xTarget-xCurrent));
	}
	
	int rx1 = rSelf*cos(theta+delta) + 0.5;
	int ry1 = rSelf*sin(theta+delta) + 0.5;

	int rx2 = rSelf*cos(theta-delta) + 0.5;
	int ry2 = 0 - (rSelf*sin(theta-delta) + 0.5);

	int x1 = xCurrent + rx1;
	int y1 = yCurrent + ry1;

	int x2 = xCurrent + rx2;
	int y2 = yCurrent + ry2;

	int x3 = xCurrent - rx1;
	int y3 = yCurrent - ry1;

	int x4 = xCurrent - rx2;
	int y4 = yCurrent - ry2;

	if(drawLine2(x1,y1,x2,y2) && drawLine2(x2,y2,x3,y3) && drawLine2(x3,y3,x4,y4) && drawLine2(x4,y4,x1,y1)){
		return true;
	}else{
// 		delta = delta + 3.1415926/2;
// 		int rx1 = rSelf*cos(theta+delta) + 0.5;
// 		int ry1 = rSelf*sin(theta+delta) + 0.5;
// 
// 		int rx2 = rSelf*cos(theta-delta) + 0.5;
// 		int ry2 = 0 - (rSelf*sin(theta-delta) + 0.5);
// 
// 		int x1 = xCurrent + rx1;
// 		int y1 = yCurrent + ry1;
// 
// 		int x2 = xCurrent + rx2;
// 		int y2 = yCurrent + ry2;
// 
// 		int x3 = xCurrent - rx1;
// 		int y3 = yCurrent - ry1;
// 
// 		int x4 = xCurrent - rx2;
// 		int y4 = yCurrent - ry2;
// 
// 		if(drawLine2(x1,y1,x2,y2) && drawLine2(x2,y2,x3,y3) && drawLine2(x3,y3,x4,y4) && drawLine2(x4,y4,x1,y1)){
// 			return true;
// 		}

		return false;
	}	
}

bool CMap::drawLine2(int x1,int y1,int x2,int y2){
	Coordinate node;
	if(x1 == x2){
		int ymin = y1>y2?y2:y1;
		int ymax = y1>y2?y1:y2;
		while(ymin <= ymax){
			node.sx = x1;
			node.sy = ymin;
			if(node.sx < TOTAL_MAP_WIDTH && node.sy < TOTAL_MAP_HEIGHT && node.sx >=0 && node.sy >=0 &&g_Map[node.sx][node.sy].iColor == MAP_WALL){
				return false;
			}
			ymin++;
		}
	}else if(y1 == y2){
		int xmin = x1>x2?x2:x1;
		int xmax = x1>x2?x1:x2;
		while(xmin <= xmax){
			node.sy = y1;
			node.sx = xmin;
			if(node.sx < TOTAL_MAP_WIDTH && node.sy < TOTAL_MAP_HEIGHT && node.sx >=0 && node.sy >=0 &&g_Map[node.sx][node.sy].iColor == MAP_WALL){
				return false;
			}
			xmin++;
		}
	}else{
		int xstart = x1;
		int ystart = y1;
		int xend = x2;
		int yend = y2;
		if(x2 < x1){
			xstart = x2;
			ystart = y2;
			xend = x1;
			yend = y1;
		}
		int deltaY = yend - ystart;
		int deltaX = xend - xstart;

		node.sy = ystart;
		node.sx = xstart;
		if(node.sx < TOTAL_MAP_WIDTH && node.sy < TOTAL_MAP_HEIGHT && node.sx >=0 && node.sy >=0 &&g_Map[node.sx][node.sy].iColor == MAP_WALL){
			return false;
		}
		int sign = 1;
		if(deltaY < 0){
			deltaY = -deltaY;
			sign = -1;
		}
		if(deltaY <= deltaX){
			int i = 1;
			while(i<=deltaX){
				double desty = (double)deltaY/(double)deltaX*1.0*i;
				int tmpY = (int)(desty + 0.5);
				node.sy = ystart+tmpY*sign;
				node.sx = xstart+i;
				if(node.sx < TOTAL_MAP_WIDTH && node.sy < TOTAL_MAP_HEIGHT && node.sx >=0 && node.sy >=0 &&g_Map[node.sx][node.sy].iColor == MAP_WALL){
					return false;
				}
				i++;
			}
		}else{
			int i = 1;
			int sign = 1;
			if(yend < ystart){
				sign = -1;
			}
			while(i<=deltaY){
				double destx = (double)deltaX/(double)deltaY*1.0*i;
				int tmpY = (int)(destx + 0.5);
				node.sx = xstart+tmpY;
				node.sy = ystart+sign*i;
				if(node.sx < TOTAL_MAP_WIDTH && node.sy < TOTAL_MAP_HEIGHT && node.sx >=0 && node.sy >=0 &&g_Map[node.sx][node.sy].iColor == MAP_WALL){
					return false;
				}
				i++;
			}
		}
	}
	return true;
}
/*
m_iRightRoad.push_back(tempNode.father);
tempNode.father = g_Map[tempNode.father.sx][tempNode.father.sy].father;
*/
void CMap::drawLine1(cocos2d::Point start,cocos2d::Point end){
	int x1 = start.x;
	int y1 = start.y;
	int x2 = end.x;
	int y2 = end.y;

	Coordinate node;

	if(x1 == x2){
		int ymin = y1>y2?y2:y1;
		int ymax = y1>y2?y1:y2;
		while(ymin <= ymax){
			node.sx = x1;
			node.sy = ymin;
			//data.push_back(node);
			if(node.sy >= TOTAL_MAP_HEIGHT) node.sy = TOTAL_MAP_HEIGHT - 1;
			if(node.sx >= TOTAL_MAP_WIDTH) node.sx = TOTAL_MAP_WIDTH - 1;
			if(node.sy < 0) node.sy = 0;
			if(node.sx < 0) node.sx = 0;

			g_Map[node.sx][node.sy].iColor = MAP_WALL;
			ymin++;
		}
	}else if(y1 == y2){
		int xmin = x1>x2?x2:x1;
		int xmax = x1>x2?x1:x2;
		while(xmin <= xmax){
			node.sy = y1;
			node.sx = xmin;

			if(node.sy >= TOTAL_MAP_HEIGHT) node.sy = TOTAL_MAP_HEIGHT - 1;
			if(node.sx >= TOTAL_MAP_WIDTH) node.sx = TOTAL_MAP_WIDTH - 1;
			if(node.sy < 0) node.sy = 0;
			if(node.sx < 0) node.sx = 0;

			g_Map[node.sx][node.sy].iColor = MAP_WALL;
			xmin++;
		}
	}else{
		int xstart = x1;
		int ystart = y1;
		int xend = x2;
		int yend = y2;
		if(x2 < x1){
			xstart = x2;
			ystart = y2;
			xend = x1;
			yend = y1;
		}
		int deltaY = yend - ystart;
		int deltaX = xend - xstart;

		node.sy = ystart;
		node.sx = xstart;
		if(node.sy >= TOTAL_MAP_HEIGHT) node.sy = TOTAL_MAP_HEIGHT - 1;
		if(node.sx >= TOTAL_MAP_WIDTH) node.sx = TOTAL_MAP_WIDTH - 1;
		if(node.sy < 0) node.sy = 0;
		if(node.sx < 0) node.sx = 0;

		g_Map[node.sx][node.sy].iColor = MAP_WALL;
		int sign = 1;
		if(deltaY < 0){
			deltaY = -deltaY;
			sign = -1;
		}
		if(deltaY <= deltaX){
			int i = 1;
			while(i<deltaX){
				double desty = (double)deltaY/(double)deltaX*1.0*i;
				int tmpY = (int)(desty + 0.5);
				node.sy = ystart+tmpY*sign;
				node.sx = xstart+i;
				if(node.sy >= TOTAL_MAP_HEIGHT) node.sy = TOTAL_MAP_HEIGHT - 1;
				if(node.sx >= TOTAL_MAP_WIDTH) node.sx = TOTAL_MAP_WIDTH - 1;
				if(node.sy < 0) node.sy = 0;
				if(node.sx < 0) node.sx = 0;

				g_Map[node.sx][node.sy].iColor = MAP_WALL;
				i++;
			}
		}else{
			int i = 1;
			int sign = 1;
			if(yend < ystart){
				sign = -1;
			}
			while(i<deltaY){
				double destx = (double)deltaX/(double)deltaY*1.0*i;
				int tmpY = (int)(destx + 0.5);
				node.sx = xstart+tmpY;
				node.sy = ystart+sign*i;
				if(node.sy >= TOTAL_MAP_HEIGHT) node.sy = TOTAL_MAP_HEIGHT - 1;
				if(node.sx >= TOTAL_MAP_WIDTH) node.sx = TOTAL_MAP_WIDTH - 1;
				if(node.sy < 0) node.sy = 0;
				if(node.sx < 0) node.sx = 0;

				g_Map[node.sx][node.sy].iColor = MAP_WALL;
				i++;
			}
		}
	}
}

bool CMap::selectRoad (cocos2d::Point startPoint,cocos2d::Point end)
{
	int Xstart = startPoint.x/SCREEN_SCALE;
	int Ystart = startPoint.y/SCREEN_SCALE;
	int Xend = end.x/SCREEN_SCALE;
	int Yend = end.y/SCREEN_SCALE;

	initSTL ();
	initMap ();
    Coordinate coordinate;
	Coordinate minCoord;
	coordinate.sx = Xstart;
	coordinate.sy = Ystart; 
	g_Map[Xstart][Ystart].iH = (POSITIVE(Xend, Xstart)*10) + (POSITIVE(Yend, Ystart)*10);
	g_Map[Xstart][Ystart].iG = 0;
	g_Map[Xstart][Ystart].iF = g_Map[Xstart][Ystart].getF();

	// 1. 把起始格添加到开启列表
	m_iOpenList.push_back(coordinate); 
	g_Map[Xstart][Ystart].isOpen = IS_OPEN; 

	//2.重复如下工作
	double minF = g_Map[Xstart][Ystart].iF ; //该变量表示最小F值
    m_isFind = false; //该变量，用来检测目标位置是否被添加进了关闭列表

    while(!m_iOpenList.empty()) //当开启列表为空时， 循环停止
    {
	    //a).寻找开启列表 中F最低的格子，我们称它为当前格
		minCoord = m_iOpenList.begin();
		minF = g_Map[minCoord.sx][minCoord.sy].getF();
		if ((minCoord.sx == Xend) && (minCoord.sy == Yend)) //如果目标位置被添加到了关闭容器，直接跳出循环
		{
			m_isFind = true;
			break; 
		}

		//b).把遍历得到的当前格切换到关闭列表
		m_iOpenList.removeFront(); //删除开启列表中的F值最低的格子
		g_Map[minCoord.sx][minCoord.sy].isOpen = IS_CLOSE;//把该格子的标记设置为“在关闭列表中”


		int i = -1, j = -1;
		int x = 0, y = 0;
		for (i = -1; i < 2; ++i)  //遍历当前格子周围的其他8个格子
		{
			for (j = -1; j < 2; ++j)
			{
				x = minCoord.sx + i;
				y = minCoord.sy + j;

				if ((x < 0) || (y < 0) || (x > TOTAL_MAP_WIDTH - 1) || (y > TOTAL_MAP_HEIGHT - 1)) 
					continue;
				if (g_Map[x][y].iColor == MAP_WALL)
					continue;
				if( !checkSelf(x,y,Xend,Yend) ){
					if(checkIfTargetInSelf(x,y,Xend,Yend)){
						m_isFind = true;
						goto searchEnd;
					}else{
						continue;
					}
				}

				//
				if (g_Map[x][y].isOpen == IS_CLOSE) //* 格子已经在关闭列表中了，略过
					continue;
				if (g_Map[x][y].isOpen == IS_UNKNOWN) //* 如果该格子未加入到开启列表，
				{
					coordinate.sx = x;
					coordinate.sy = y;
					g_Map[x][y].father = minCoord;  //当前格作为这一格的父节点
					g_Map[x][y].iH = (POSITIVE(Xend, x)*10) + (POSITIVE(Yend, y)*10);
					if((i == 0) || (j == 0))
					{
						g_Map[x][y].iG = g_Map[minCoord.sx][minCoord.sy].iG + 10;  
					}
					else
					{
						g_Map[x][y].iG = g_Map[minCoord.sx][minCoord.sy].iG + 14; 
					}
					g_Map[x][y].iF = g_Map[x][y].getF();
					g_Map[x][y].isOpen = IS_OPEN;
					m_iOpenList.push_back(coordinate);  //把他加入到开启列表中
					continue;
				}
				if (g_Map[x][y].isOpen == IS_OPEN) //* 如果该格子已经在开启列表中
				{
					int newG;
					if((i == 0) || (j == 0))  //计算新的G值
					{
						newG = g_Map[minCoord.sx][minCoord.sy].iG + 10;   
					}
					else
					{
						newG = g_Map[minCoord.sx][minCoord.sy].iG + 14; 
					}
					if (newG < g_Map[x][y].iG) //如果新的G值比先前的更好
					{
						g_Map[x][y].father = minCoord; //把该格的父节点改为当前格
						int position = g_Map[x][y].iHeapPosition; //得到该节点在堆中的位置
						g_Map[x][y].iG = newG;
						g_Map[x][y].iF = g_Map[x][y].getF();    
						m_iOpenList.newHeap(position);             //由于F值发生了变化，更新此节点在堆中的位置
					}
				}
			}
		}	
	}
searchEnd:

	if (m_isFind)
	{
		MapNode tempNode;
		tempNode.father = minCoord;
		while (true)
		{
			if ((tempNode.father.sx == -1) && (tempNode.father.sy == -1)){
				break;
			}
			m_iRightRoad.push_back(tempNode.father);
            tempNode.father = g_Map[tempNode.father.sx][tempNode.father.sy].father;
		}
		//m_iRightRoad.reserve();
#if 0
		if(m_iRightRoad.size() > 1){
			cocos2d::Point result;
			//result.push_back(m_iRightRoad.());
			//drawLine(m_iRightRoad);
			Coordinate node;
			vector<Coordinate>::reverse_iterator ite = m_iRightRoad.rbegin();
			
			int x1 = ite->sx;
			int y1 = ite->sy;
			result.x = x1*SCREEN_SCALE + SCREEN_SCALE/ 2;
			result.y = y1*SCREEN_SCALE + SCREEN_SCALE/ 2;
			m_road.push_back(result);
			//m_road.push_back(mStart);
			ite++;
			for(;ite != m_iRightRoad.rend(); ++ite)
			{
				int x2 = ite->sx;
				int y2 = ite->sy;
				result.x = x2*SCREEN_SCALE + SCREEN_SCALE/ 2;
				result.y = y2*SCREEN_SCALE + SCREEN_SCALE/ 2;
				m_road.push_back(result);
			}
		}
#else
		if(m_iRightRoad.size() >= 1){
			cocos2d::Point result;
			Coordinate node;
			vector<Coordinate>::reverse_iterator ite = m_iRightRoad.rbegin();

			int x1 = ite->sx;
			int y1 = ite->sy;
			result.x = x1*SCREEN_SCALE + SCREEN_SCALE/ 2;
			result.y = y1*SCREEN_SCALE + SCREEN_SCALE/ 2;
			m_road.push_back(result);
			//m_road.push_back(mStart);
			int ny = rSelf*sin(theta)*sin(theta)+0.5;
			int nx = rSelf*sin(theta)*cos(theta)+0.5;

			ite++;
			for(;ite != m_iRightRoad.rend(); ++ite)
			{
				int x2 = ite->sx;
				int y2 = ite->sy;
				Point p1(x1*SCREEN_SCALE + SCREEN_SCALE/ 2,y1*SCREEN_SCALE + SCREEN_SCALE/ 2);
				Point p2(x2*SCREEN_SCALE + SCREEN_SCALE/ 2,y2*SCREEN_SCALE + SCREEN_SCALE/ 2);
				if(!checkLine(p1,p2)){
					vector<Coordinate>::reverse_iterator last = ite -1;
					x1 = last->sx;
					y1 = last->sy;
					result.x = x1*SCREEN_SCALE + SCREEN_SCALE/ 2;
					result.y = y1*SCREEN_SCALE + SCREEN_SCALE/ 2;
					m_road.push_back(result);
				}

// 				if(!drawLine2(x1-nx,y1+ny,x2-nx,y2+ny) && !drawLine2(x1+nx,y1-ny,x2+nx,y2-ny)){
// 					vector<Coordinate>::reverse_iterator last = ite -1;
// 					x1 = last->sx;
// 					y1 = last->sy;
// 					result.x = x1*SCREEN_SCALE + SCREEN_SCALE/ 2;
// 					result.y = y1*SCREEN_SCALE + SCREEN_SCALE/ 2;
// 					m_road.push_back(result);
// 				}
			}

// 			node.sx = m_iRightRoad[0].sx;
// 			node.sy = m_iRightRoad[0].sy;
			result.x = m_iRightRoad[0].sx*SCREEN_SCALE + SCREEN_SCALE/ 2;
			result.y = m_iRightRoad[0].sy*SCREEN_SCALE + SCREEN_SCALE/ 2;
			m_road.push_back(result);

// 			vector<Coordinate> result1 = result;
// 
// 			vector<Coordinate>::iterator ite1 = result1.begin();
// 			m_iRightRoad.clear();
// 
// 			x1 = ite1->sx;
// 			y1 = ite1->sy;
// 
// 			for( ;ite1 != result1.end(); ++ite1)
// 			{
// 				int x2 = ite1->sx;
// 				int y2 = ite1->sy;
// 				drawLine1(m_iRightRoad,x1,y1,x2,y2);
// 				x1 = ite1->sx;
// 				y1 = ite1->sy;
// 			}

		}
#endif
	}
	else
	{
		m_i = -1;
		m_StartX = m_StartY = -1;
		m_EndX = m_EndY = -1;
	}	

	if(m_isFind && m_road.empty()){
		m_road.push_back(end);
	}
	return m_isFind;
}

#if 1

//rotate cord , x1 = x*cos + y*sin ,, y1 = -x*sin + y*cos
Point getRotated(Point p1,float angle ){
	Point res;
	res.x = p1.x*cos(angle) + p1.y*sin(angle);
	res.y = -p1.x*sin(angle) + p1.y*cos(angle);
	return res;
}

vector<RoadNode> CMap::getSmoothRoad(Vec2 vShip,Point startPoint,Point end,double minR, bool isMainShip)
{
	bool useFarSide = false;
	m_node.clear();
	Vec2 x_dir(1,0);
	float angle = Vec2::angle(vShip,x_dir);
	float cross = x_dir.cross(vShip);//vShip.cross(x_dir);
	if(cross < 0) angle = - angle;
	
	Point end1;
	end1.x = end.x - startPoint.x;
	end1.y = end.y - startPoint.y;

	Point end2 = getRotated(end1,angle);
	Point start2(0,0);

	float destAngle = Vec2::angle(end2,x_dir);
	float destCross = x_dir.cross(end2);
	int signy = 1;
	if(destCross < 0){
		signy = -1;
	}

//	if(end1.x*end1.x+end1.y*end1.y < minR*minR*4){
//		m_node.push_back(RoadNode(startPoint,end,Vec2::angle(vShip,end1)*signy,0,end));
//		return m_node;
//	}

	float cox = 0;
	float coy = signy*minR;

	Point ptCenter(cox,coy);
	Point ptOutside = end2;
	Point E,F,G,H,H1;
	double r=minR;
	//1. 坐标平移到圆心ptCenter处,求园外点的新坐标E
	E.x= ptOutside.x-ptCenter.x;
	E.y= ptOutside.y-ptCenter.y; //平移变换到E

	//2. 求园与OE的交点坐标F, 相当于E的缩放变换
	double t= r / sqrt (E.x * E.x + E.y * E.y);  //得到缩放比例
	F.x= E.x * t;   F.y= E.y * t;   //缩放变换到F

	if(t>1){
		signy = -signy;
		coy = signy*minR;

		ptCenter =  Point(cox,coy);
		E.x= ptOutside.x-ptCenter.x;
		E.y= ptOutside.y-ptCenter.y; //平移变换到E
		t= r / sqrt (E.x * E.x + E.y * E.y);  //得到缩放比例
		F.x= E.x * t;   F.y= E.y * t;   //缩放变换到F
		useFarSide  = true;
		if(t > 1){
			log("god save me.");
			//		m_node.push_back(RoadNode(startPoint,end,Vec2::angle(vShip,end1)*signy,0,end));
			return m_node;
		}

	}
	//3. 将E旋转变换角度a到切点G，其中cos(a)=r/OF=t, 所以a=arccos(t);
	double a=acos(t);   //得到旋转角度
	G.x=F.x*cos(a) -F.y*sin(a);
	G.y=F.x*sin(a) +F.y*cos(a);    //旋转变换到G

	//4. 将G平移到原来的坐标下得到新坐标H
	H.x=G.x+ptCenter.x;
	H.y=G.y+ptCenter.y;             //平移变换到H

	a=-acos(t);   //得到旋转角度
	G.x=F.x*cos(a) -F.y*sin(a);
	G.y=F.x*sin(a) +F.y*cos(a);    //旋转变换到G

	//4. 将G平移到原来的坐标下得到新坐标H
	H1.x=G.x+ptCenter.x;
	H1.y=G.y+ptCenter.y;             //平移变换到H

	Point co(cox,coy);
	Point co1 = getRotated(co,-angle);
//	Point co2 = getRotated(Point(cox, -coy), -angle);

	//Point t(H.x,ty);
	Point t1 = getRotated(H,-angle);
	Point t2 = getRotated(H1,-angle);

	co1.x += startPoint.x;
	co1.y += startPoint.y;

	t1.x += startPoint.x;
	t1.y += startPoint.y;

	t2.x += startPoint.x;
	t2.y += startPoint.y;

	float angle1 = Vec2::angle(vShip,Vec2(startPoint,t1));
	float angle2 = Vec2::angle(vShip,Vec2(startPoint,t2));

	//如果自身位置更靠近目标点，需要取角度更大的那个点。
	if(Vec2(end.x, end.y).distanceSquared(Vec2(startPoint.x, startPoint.y)) >= Vec2(end.x, end.y).distanceSquared(Vec2(t1.x, t1.y))){
		if(angle2 < angle1){
			t1 = t2;
		}
	}else{
		if(angle2 > angle1){
			t1 = t2;
		}
	}
	m_node.push_back(RoadNode(startPoint,t1,destAngle*signy,minR,co1));
	m_node.push_back(RoadNode(t1,end,0,0,end));

	return m_node;
}

#else
vector<RoadNode> CMap::getSmoothRoad(Vec2 vShip,Point startPoint,Point end,double minR)
{
	m_node.clear();
	Vec2 dest = Vec2(Vec2(startPoint.x,startPoint.y),Vec2(end.x,end.y));
	float angle = Vec2::angle(vShip,dest);
	float cross = vShip.cross(dest);
	if(abs(angle) > ASTAR_PI/2){
		//float theta1 = Vec2::angle(vShip,Vec2(Vec2(startPoint.x,startPoint.y),Vec2(0,0)));
#if 1
		float theta1 = Vec2::angle(vShip,Vec2(1,0));

		if(theta1 > ASTAR_PI) theta1 -= ASTAR_PI;
		int signy = 1;
		if(end.y<startPoint.y)
			signy = -1;

		int signx = 1;
		float dir = Vec2::angle(vShip,Vec2(1,0));
		if(dir > ASTAR_PI/2)
			signx = -1;

		float cox = startPoint.x + signx*minR*cos(ASTAR_PI/2-theta1);
		float coy = startPoint.y + signy*minR*sin(ASTAR_PI/2-theta1);

		float h = sqrt((cox-end.x)*(cox-end.x) + (coy-end.y)*(coy-end.y));
		if(h > minR){
			float alpha3 = angle - ASTAR_PI/2;
			float alpha1 = asin(minR/h);
			float alpha2 = asin(minR*sin(alpha3)/h);

			float alpha = alpha1 + alpha2 + alpha3;
			float phi = alpha + ASTAR_PI/2;

			float myTheta = alpha + theta1;

			float d2 = sqrt(h*h-minR*minR);
			float tx = end.x + signx*d2*sin(myTheta);
			float ty = end.y - signy*d2*cos(myTheta);
			if(cross < 0) phi = -phi;
			m_node.push_back(RoadNode(startPoint,Point(tx,ty),phi,minR,Point(cox,coy)));
			m_node.push_back(RoadNode(Point(tx,ty),end,0,0,end));
		}else{
			m_node.push_back(RoadNode(startPoint,end,0,0,end));
		}
#else
		float theta1 = Vec2::angle(vShip,Vec2(1,0));

		int signy = 1;
		if(end.y<startPoint.y)
			signy = -1;

		int signx = 1;
		float dir = Vec2::angle(vShip,Vec2(1,0));
		if(dir > ASTAR_PI/2)
			signx = -1;

		float cox = startPoint.x + signx*minR*cos(ASTAR_PI/2-theta1);
		float coy = startPoint.y + signy*minR*sin(ASTAR_PI/2-theta1);

		float h = sqrt((cox-end.x)*(cox-end.x) + (coy-end.y)*(coy-end.y));
		float phi1 = acos(minR/h);
		float phi2 = ASTAR_PI - angle;
		float d1 = minR*sin(angle - ASTAR_PI/2);
		float phi3 = acos(d1/h);

		float phi = 2*ASTAR_PI - phi1 - phi2 - phi3;

		float theta2 = ASTAR_PI - phi;
		float myTheta = (ASTAR_PI - theta2)/2 + theta1;
		float d2 = minR*sin(phi/2)*2;

		float tx = startPoint.x - signx*d2*cos(myTheta);
		float ty = startPoint.y + signy*d2*sin(myTheta);

		m_node.push_back(RoadNode(startPoint,Point(tx,ty),phi,minR,Point(cox,coy)));
		m_node.push_back(RoadNode(Point(tx,ty),end,0,0,end));
#endif

	}else{
		//float theta1 = Vec2::angle(vShip,Vec2(Vec2(0,0),Vec2(startPoint.x,startPoint.y)));
#if 1
		float theta1 = Vec2::angle(vShip,Vec2(1,0));
		if(theta1 > ASTAR_PI/2){
			if(cross < 0){
				float cox = startPoint.x - minR*cos(theta1 - ASTAR_PI/2);  
				float coy = startPoint.y + minR*sin(theta1 - ASTAR_PI/2);  //
				float d = sqrt((cox-end.x)*(cox-end.x) + (coy-end.y)*(coy-end.y));
				if(d > minR){
					float h = sqrt(d*d - minR*minR);
					float alpha1 = asin(minR/d);
					float alpha2 = Vec2::angle(dest,Vec2(1,0)) - ASTAR_PI/2;
				
					float d1 = sqrt((startPoint.x-end.x)*(startPoint.x-end.x)+(startPoint.y-end.y)*(startPoint.y-end.y));

					float alpha3 = acos((d*d-minR*minR+d1*d1)/(2*d*d1));//cosA=(b*c+c*c-a*a)/(2bc)

					float alpha = alpha1 + alpha2 - alpha3;

					float phi1 = acos((d*d+minR*minR-d1*d1)/(2*d*minR));
					float phi = phi1 - acos(minR/d);
					float tx = end.x + h*sin(alpha);
					float ty = end.y + h*cos(alpha);


					// 			float error = (tx-cox)*(tx-cox) + (ty-coy)*(ty-coy) - minR*minR;
					// 			Vec2 c(cox,coy);
					// 			Vec2 t(tx,ty);
					// 			Vec2 v1(c,t);
					// 			Vec2 v2(t,end);
					// 
					// 			float test = Vec2::angle(v1,v2)/ASTAR_PI;

					if(cross < 0) phi = -phi;

					m_node.push_back(RoadNode(startPoint,Point(tx,ty),phi,minR,Point(cox,coy)));
					m_node.push_back(RoadNode(Point(tx,ty),end,0,0,end));
				}else{
					m_node.push_back(RoadNode(startPoint,end,0,0,end));
				}
			}else{
				float cox = startPoint.x + minR*sin(ASTAR_PI - theta1);  
				float coy = startPoint.y - minR*cos(ASTAR_PI - theta1);  //
				float d = sqrt((cox-end.x)*(cox-end.x) + (coy-end.y)*(coy-end.y));
				Vec2 c(cox,coy);
				Vec2 v1(end,c);
				Vec2 v2(0,1);
				float theta = Vec2::angle(v1,v2);
				float theta2 = asin(minR/d);
				float thetaNeed = theta - theta2;

				if(d > minR){
					float h = sqrt(d*d - minR*minR);
					float phi = ASTAR_PI/2-thetaNeed;

					float tx = end.x + h*sin(thetaNeed);
					float ty = end.y + h*cos(thetaNeed);
					m_node.push_back(RoadNode(startPoint,Point(tx,ty),phi,minR,Point(cox,coy)));
					m_node.push_back(RoadNode(Point(tx,ty),end,0,0,end));
				}else{
					m_node.push_back(RoadNode(startPoint,end,0,0,end));
				}
			}
		}else{

			float targetTheta = ASTAR_PI/2 - angle + theta1;
			int signy = 1;
			if(end.y<startPoint.y)
				signy = -1;

			int signx = 1;
			float dir = Vec2::angle(vShip,Vec2(1,0));
			if(dir > ASTAR_PI/2)
				signx = -1;

			float cox = startPoint.x - signx*minR*cos(ASTAR_PI/2-theta1);  
			float coy = startPoint.y + signy*minR*sin(ASTAR_PI/2-theta1);  //

			float d = sqrt((cox-end.x)*(cox-end.x) + (coy-end.y)*(coy-end.y));
			if(d > minR){
				float h = sqrt(d*d - minR*minR);
				float alpha1 = acos(minR/d);

				float d1 = sqrt((startPoint.x-end.x)*(startPoint.x-end.x)+(startPoint.y-end.y)*(startPoint.y-end.y));
				float alpha = acos((d*d+minR*minR-d1*d1)/(2*d*minR));    //cosA=(b*c+c*c-a*a)/(2bc)
				float phi = alpha - alpha1;
	// 			float tx = end.x - signx*h*sin(targetTheta);
	// 			float ty = end.y - signy*h*cos(targetTheta);

				float tx = cox + signx*minR*sin(phi+theta1);
				float ty = coy - signy*minR*cos(phi+theta1);

	// 			float error = (tx-cox)*(tx-cox) + (ty-coy)*(ty-coy) - minR*minR;
	// 			Vec2 c(cox,coy);
	// 			Vec2 t(tx,ty);
	// 			Vec2 v1(c,t);
	// 			Vec2 v2(t,end);
	// 
	// 			float test = Vec2::angle(v1,v2)/ASTAR_PI;

				if(cross < 0) phi = -phi;

				m_node.push_back(RoadNode(startPoint,Point(tx,ty),phi,minR,Point(cox,coy)));
				m_node.push_back(RoadNode(Point(tx,ty),end,0,0,end));
			}else{
				m_node.push_back(RoadNode(startPoint,end,0,0,end));
			}
		}
#else
		float theta1 = Vec2::angle(vShip,Vec2(1,0));

		int signy = 1;
		if(end.y<startPoint.y)
			signy = -1;

		int signx = 1;
		float dir = Vec2::angle(vShip,Vec2(1,0));
		if(dir > ASTAR_PI/2)
			signx = -1;

		float cox = startPoint.x - signx*minR*cos(ASTAR_PI/2-theta1);  
		float coy = startPoint.y + signy*minR*sin(ASTAR_PI/2-theta1);  //

		float h = sqrt((cox-end.x)*(cox-end.x) + (coy-end.y)*(coy-end.y));
		float phi1 = asin(minR/h);
		Vec2 tc = Vec2(Vec2(cox,coy),Vec2(end.x,end.y));
		tc.rotate(Vec2(end.x,end.y),phi1);

		float phi = Vec2::angle(tc,vShip);
		float d2 = minR*sin(phi/2)*2;

		float myTheta = phi/2 + theta1;

		float tx = startPoint.x + signx*d2*cos(myTheta);
		float ty = startPoint.y + signy*d2*sin(myTheta);

		m_node.push_back(RoadNode(startPoint,Point(tx,ty),phi,minR,Point(cox,coy)));
		m_node.push_back(RoadNode(Point(tx,ty),end,0,0,end));
#endif
	}
	{
		if(m_node.size() > 1){
			RoadNode n1 = m_node.at(0);
			RoadNode n2 = m_node.at(1);

			float cox = n1.centerPoint.x;
			float coy = n1.centerPoint.y;

			float tx = n1.pt2.x;
			float ty = n1.pt2.y;

			float error = (tx-cox)*(tx-cox) + (ty-coy)*(ty-coy) - minR*minR;
			Vec2 c(cox,coy);
			Vec2 t(tx,ty);
			Vec2 v1(c,t);
			Vec2 v2(t,end);
			// 
			float test = Vec2::angle(v1,v2)/ASTAR_PI;
			if(error > 0.01 || test >0.51 || test < 0.49 || error < -0.01){
				__asm nop;
			}
		}
	}
	return m_node;
}
#endif

bool CMap::checkLine(Point pt1,Point pt2)
{
	if(!checkSigleLine(pt1,pt2))
		return false;

	double theta = 0;
	if(pt1.x == pt2.x){
		if(pt1.y < pt2.y){
			theta = CC_DEGREES_TO_RADIANS(90);
		}else{
			theta = -CC_DEGREES_TO_RADIANS(90);
		}
	}else{
		theta = atan((pt2.y-pt1.y)/(pt2.x-pt1.x));
	}

	int x1 = pt1.x - mSelfHeight*0.5*cos(theta);
	int y1 = pt1.y + mSelfHeight*0.5*sin(theta);

	int x2 = pt2.x - mSelfHeight*0.5*cos(theta);
	int y2 = pt2.y + mSelfHeight*0.5*sin(theta);

	Point newP1(x1,y1);
	Point newP2(x2,y2);
	if(!checkSigleLine(newP1,newP2))
		return false;

	{
		int x1 = pt1.x + mSelfHeight*0.5*sin(theta);
		int y1 = pt1.y - mSelfHeight*0.5*cos(theta);

		int x2 = pt2.x + mSelfHeight*0.5*sin(theta);
		int y2 = pt2.y - mSelfHeight*0.5*cos(theta);

		Point newP1(x1,y1);
		Point newP2(x2,y2);
		if(!checkSigleLine(newP1,newP2))
			return false;
	}
	return true;
}


bool CMap::intersectLine(Point aa, Point bb, Point cc, Point dd)
{
	if ( max(aa.x, bb.x)<min(cc.x, dd.x) )
	{
		return false;
	}
	if ( max(aa.y, bb.y)<min(cc.y, dd.y) )
	{
		return false;
	}
	if ( max(cc.x, dd.x)<min(aa.x, bb.x) )
	{
		return false;
	}
	if ( max(cc.y, dd.y)<min(aa.y, bb.y) )
	{
		return false;
	}
	if ( mult(cc, bb, aa)*mult(bb, dd, aa)<0 )
	{
		return false;
	}
	if ( mult(aa, dd, cc)*mult(dd, bb, cc)<0 )
	{
		return false;
	}
	return true;
}

bool CMap::checkSigleLine(Point pt1,Point pt2){
	for (auto iter : blockLines)
	{
		if(intersectLine(pt1,pt2,iter.pt1,iter.pt2))
			return false;
	}
	return true;
}


FindPath::~FindPath()
{

}

FindPath::FindPath(int w,int h)
{
	TOTAL_MAP_WIDTH = w;
	TOTAL_MAP_HEIGHT = h;
}




//aa, bb为一条线段两端点 cc, dd为另一条线段的两端点 相交返回true, 不相交返回false
bool FindPath::intersectLine(Point aa, Point bb, Point cc, Point dd)
{
	if ( max(aa.x, bb.x)<min(cc.x, dd.x) )
	{
		return false;
	}
	if ( max(aa.y, bb.y)<min(cc.y, dd.y) )
	{
		return false;
	}
	if ( max(cc.x, dd.x)<min(aa.x, bb.x) )
	{
		return false;
	}
	if ( max(cc.y, dd.y)<min(aa.y, bb.y) )
	{
		return false;
	}
	if ( mult(cc, bb, aa)*mult(bb, dd, aa)<0 )
	{
		return false;
	}
	if ( mult(aa, dd, cc)*mult(dd, bb, cc)<0 )
	{
		return false;
	}
	return true;
}

///------------alg 3------------
double FindPath::determinant(double v1, double v2, double v3, double v4) 
{
	return (v1*v3-v2*v4);
}
// 
// bool FindPath::intersectLine(Point aa, Point bb, Point cc, Point dd)
// {
// 	double delta = determinant(bb.x-aa.x, cc.x-dd.x, bb.y-aa.y, cc.y-dd.y);
// 	if ( delta<=(1e-6) && delta>=-(1e-6) )  // delta=0，表示两线段重合或平行
// 	{
// 		return false;
// 	}
// 	double namenda = determinant(cc.x-aa.x, cc.x-dd.x, cc.y-aa.y, cc.y-dd.y) / delta;
// 	if ( namenda>1 || namenda<0 )
// 	{
// 		return false;
// 	}
// 	double miu = determinant(bb.x-aa.x, cc.x-aa.x, bb.y-aa.y, cc.y-aa.y) / delta;
// 	if ( miu>1 || miu<0 )
// 	{
// 		return false;
// 	}
// 	bool res = intersect(aa,bb,cc,dd);
// 
// 	double x = aa.x+namenda*(bb.x-aa.x);
// 	double y = aa.y+namenda*(bb.y-aa.y);
// 
// 	double x2 = cc.x+miu*(dd.x-cc.x);
// 	double y2 = cc.y+miu*(dd.y-cc.y);
// 
// 
// 	m_road.push_back(cc);
// 	m_road.push_back(dd);
// 	return true;
// }

bool FindPath::RectIntersect(Point r1[4],Point r2[4]){
	for(int i=0;i<4;i++){
		Point p1 = r1[i];
		Point p2;
		if(i == 3)
			p2 = r1[0];
		else
			p2 = r1[i+1];
		for(int j =0;j<4;j++){
			Point p3 = r2[j];
			Point p4;
			if(j == 3)
				p4 = r2[0];
			else
				p4 = r2[j+1];
			if(intersectLine(p1,p2,p3,p4))
				return true;
		}
	}
	return false;
}


bool FindPath::checkSigleLine(Point pt1,Point pt2){
	for (auto iter : blockLines)
	{
		if(intersectLine(pt1,pt2,iter.pt1,iter.pt2))
			return false;
	}
	return true;
}


bool FindPath::checkLine(Point pt1,Point pt2)
{
	if(!checkSigleLine(pt1,pt2))
		return false;

	double theta = 0;
	if(pt1.x == pt2.x){
		if(pt1.y < pt2.y){
			theta = CC_DEGREES_TO_RADIANS(90);
		}else{
			theta = -CC_DEGREES_TO_RADIANS(90);
		}
	}else{
		theta = atan((pt2.y-pt1.y)/(pt2.x-pt1.x));
	}

	int x1 = pt1.x - mSelfHeight*0.5*cos(theta);
	int y1 = pt1.y + mSelfHeight*0.5*sin(theta);

	int x2 = pt2.x - mSelfHeight*0.5*cos(theta);
	int y2 = pt2.y + mSelfHeight*0.5*sin(theta);

	Point newP1(x1,y1);
	Point newP2(x2,y2);
	if(!checkSigleLine(newP1,newP2))
		return false;

	{
		int x1 = pt1.x + mSelfHeight*0.5*sin(theta);
		int y1 = pt1.y - mSelfHeight*0.5*cos(theta);

		int x2 = pt2.x + mSelfHeight*0.5*sin(theta);
		int y2 = pt2.y - mSelfHeight*0.5*cos(theta);

		Point newP1(x1,y1);
		Point newP2(x2,y2);
		if(!checkSigleLine(newP1,newP2))
			return false;
	}
	return true;
}


bool FindPath::selectRoad (cocos2d::Point startPoint,cocos2d::Point end)
{
	m_road.clear();
	m_road.push_back(startPoint);
	if(checkLine(startPoint,end))
	{		
		m_road.push_back(end);
		return true;
	}else{
		Point inter1,inter2;
		double theta = 0;
		if(end.x == startPoint.x){
			if(startPoint.y < end.y){
				theta = CC_DEGREES_TO_RADIANS(90);			
			}else{
				theta = -CC_DEGREES_TO_RADIANS(90);	
			}
		}else{
			theta = atan((end.y-startPoint.y)/(end.x-startPoint.x));
		}
		Point high = startPoint;
		Point low = end;
		if(high.y < low.y){
			high = end;
			low = startPoint;
		}
		double distance = (end.y-startPoint.y)*(end.y-startPoint.y) + (end.x-startPoint.x)*(end.x-startPoint.x);
		distance = sqrt(distance);
		double addTheta = atan(1/((cos(theta) + sin(theta))*distance))*2;
		double newTheta = 0;
		int lastx = low.x;
		int lasty = low.y;
		bool found = false;
		Point now;
		while(newTheta < CC_DEGREES_TO_RADIANS(90)){
			double r1 = tan(newTheta/2)*distance;
			double x1 = low.x - r1*(sin(theta)+cos(theta));
			double y1 = low.y + r1*(sin(theta)+cos(theta));
			int i_x1 = (int)x1;
			int i_y1 = (int)y1;
			if(lastx != i_x1 || lasty != i_y1){
				now.x = i_x1;
				now.y = i_y1;
				if(checkLine(high,now)){
// 					found = true;
 					m_road.push_back(now);
 					return true;
// 					//break;
					if(checkLine(low,now)){
						m_road.push_back(now);
						m_road.push_back(end);
						return true;
					}
				}
			}
			newTheta += addTheta;
		}
	}

	return false;
}
