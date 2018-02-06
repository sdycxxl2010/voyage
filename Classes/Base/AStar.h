#ifndef ___ASTAR_H__
#define ___ASTAR_H__
#include <vector>

#include "TVBasic.h"

const int IS_UNKNOWN = 0;
const int IS_OPEN = 1;
const int IS_CLOSE = 2;

const int MAP_ROAD = 0;
const int MAP_WALL = 1;
const int MAP_CANT = 2;

struct Coordinate
{
	int sx;
	int sy;
	Coordinate()
	{
		sx = 0;
		sy = 0;
	}
};

struct MapNode
{
	int iHeapPosition;  //此节点在堆中的位置
	double iG;
	double iH;
	double iF;
	int iColor;  //检测是否可以通过
	int isOpen; //检测是否在开启列表中
	Coordinate father;

	MapNode()
	{
		iHeapPosition = -1;
		iG = -1;
		iH = -1;
		iF = -1;
		iColor = -1;
		isOpen = IS_UNKNOWN;
	}
	double getF()
	{
		return iH + iG;
	}
};

class CHeap
{
public:
	CHeap();
	void init(MapNode**mapArray){mMap = mapArray;}
	void removeFront(); //移除堆中的第一个元素
	void push_back(Coordinate element); //往堆中添加元素
	void newHeap(int position);    //当F值改变时，更新堆
	int getSize();
	int getMinF();
	bool empty();
	void clear();
	Coordinate begin();
	MapNode** mMap;
private:
	vector<Coordinate> v;  
};

enum Mode {MAP_EDIT, MAP_RUN };
#define POSITIVE(x,y) ((((x) - (y))>0)? ((x)-(y)):((y)-(x)))
#define SCREEN_SCALE 10

class ShipPoints{
public:
	ShipPoints(cocos2d::Point p1,cocos2d::Point p2,cocos2d::Point p3,cocos2d::Point p4){
		mp1=getScalePoint(p1); mp2= getScalePoint(p2); mp3 = getScalePoint(p3);mp4 = getScalePoint(p4);
	}
	cocos2d::Point mp1;
	cocos2d::Point mp2;
	cocos2d::Point mp3;
	cocos2d::Point mp4;
	cocos2d::Point getScalePoint(cocos2d::Point p1){
		p1.x /= SCREEN_SCALE;
		p1.y /= SCREEN_SCALE;
		return p1;
	}
};

class RoadNode{
public:
	RoadNode(Point p1,Point p2,float a,float r,Point center){
		pt1 = p1;
		pt2 = p2;
		angle = a;
		turnR = r;
		centerPoint = center;
	}

	Point pt1; //起点
	Point pt2; //终点
	float angle; //目的点角度
	float turnR; //转弯半径
	Point centerPoint; //圆心
};

class BlockLine{
public:
	BlockLine(Point p1,Point p2)
	{
		pt1 = p1;
		pt2 = p2;
	}

	Point pt1;
	Point pt2;
};


/*
test code;

CMap*map = new CMap(800,480);
map->addWallShip(Vec2(200,30),Vec2(250,30),Vec2(250,480),Vec2(200,480));
//map->addWallShip(Vec2(400,400),Vec2(450,400),Vec2(450,350),Vec2(400,350));

map->addSelfPoints(Vec2(100,400),Vec2(150,400),Vec2(150,350),Vec2(100,350));

if(map->selectRoad(Vec2(750,380)))
{
auto road = map->getRoad();
}
delete map;

*/

#define ASTAR_PI 3.1415926535897932385f

class CMap
{
public:
	CMap(int width,int height);
	~CMap();
	void resetMap ();
	void setWallData(ShipPoints*p);
	void initSTL ();
	void initMap ();
	void setMode (Mode mode)
	{
		m_Mode = mode;
	}
	Mode getMode ()
	{
		return m_Mode;
	}


	void addWallShip(cocos2d::Point p1,cocos2d::Point p2,cocos2d::Point p3,cocos2d::Point p4 )
	{
		blockLines.push_back(BlockLine(p1,p2));
		blockLines.push_back(BlockLine(p2,p3));
		blockLines.push_back(BlockLine(p3,p4));
		blockLines.push_back(BlockLine(p4,p1));

		ShipPoints p (p1,p2,p3,p4);
		setWallData(&p);
		//wallShips.push_back(p);
	}

	void removeAllWallShip();

	void addSelfPoints(cocos2d::Point p1,cocos2d::Point p2,cocos2d::Point p3,cocos2d::Point p4 )
	{
		ShipPoints tmp(p1,p2,p3,p4);
		if(!selfShip){
			selfShip = new ShipPoints(p1,p2,p3,p4);
		}else{
			ShipPoints tmp(p1,p2,p3,p4);
			*selfShip = tmp;
		}
		mStart.x = (p1.x + p2.x + p3.x + p4.x )/4;
		mStart.y = (p1.y + p2.y + p3.y + p4.y )/4;

		mScaleStart.x = mStart.x/SCREEN_SCALE;
		mScaleStart.y = mStart.y/SCREEN_SCALE;
		double d1 = (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
		double d2 = (p3.x - p2.x)*(p3.x - p2.x) + (p3.y - p2.y)*(p3.y - p2.y);
		if(d1 <= d2){
			theta = atan(sqrt(d1/d2));
			mSelfHeight = sqrt(d1)+16;
			mSelfWidth = sqrt(d2)+16;
		}else{
			theta = atan(sqrt(d2/d1));
			mSelfHeight = sqrt(d2)+16;
			mSelfWidth = sqrt(d1)+16;
		}
		rSelf = sqrt((p3.x - p1.x)*(p3.x - p1.x) + (p3.y - p1.y)*(p3.y - p1.y))/(2*SCREEN_SCALE);
	}

	//bool selectRoad (cocos2d::Point end);
	bool selectRoad (cocos2d::Point startPoint,cocos2d::Point end);
	void drawLine1(cocos2d::Point start,cocos2d::Point end);
	bool drawLine2(int x1,int y1,int x2,int y2);
	bool checkSelf(int xCurrent,int yCurrent,int xTarget,int yTarget);
	bool checkIfTargetInSelf(int xCurrent,int yCurrent,int xTarget,int yTarget);
	vector<cocos2d::Point> getRoad(){return m_road;}
	bool checkLine(Point pt1,Point pt2);
	bool checkSigleLine(Point pt1,Point pt2);
	vector<RoadNode> getSmoothRoad(Vec2 vShip,Point startPoint,Point end,double minR, bool isMainShip=false);
private:
	vector<BlockLine> blockLines;
	bool intersectLine(Point aa, Point bb, Point cc, Point dd);
	int m_StartX;
	int m_StartY;
	int m_EndX;
	int m_EndY;
	double duration; //计算寻路所花的时间
	int m_i;
	bool m_isShow;
	bool m_isFind; // 检测是否发现了正确的路径
	enum Mode m_Mode;

	CHeap m_iOpenList; //开启列表
	vector<Coordinate> m_iRightRoad; //储存找到的路径
	vector<cocos2d::Point> m_road;
	vector<RoadNode>m_node;
	//vector<ShipPoints*> wallShips;
	ShipPoints*selfShip;
	cocos2d::Point mStart;
	cocos2d::Point mScaleStart;
	double theta;
	double rSelf;
	int TOTAL_MAP_WIDTH;
	int TOTAL_MAP_HEIGHT;
	MapNode** g_Map;
	double mSelfWidth;
	double mSelfHeight;
};


class FindPath
{
public:
	FindPath(int width,int height);
	~FindPath();



	void addWallShip(cocos2d::Point p1,cocos2d::Point p2,cocos2d::Point p3,cocos2d::Point p4 )
	{
		blockLines.push_back(BlockLine(p1,p2));
		blockLines.push_back(BlockLine(p2,p3));
		blockLines.push_back(BlockLine(p3,p4));
		blockLines.push_back(BlockLine(p4,p1));
	}

	void removeAllWallShip()
	{
		blockLines.clear();
	}

	void addSelfPoints(double w, double h )
	{
		mSelfWidth = w;
		mSelfHeight = h;
	}

	bool selectRoad (cocos2d::Point startPoint,cocos2d::Point end);
	vector<cocos2d::Point> getRoad(){return m_road;}
private:

	///------------alg 3------------
	double determinant(double v1, double v2, double v3, double v4);

	bool intersectLine(Point aa, Point bb, Point cc, Point dd);
	bool RectIntersect(Point r1[4],Point r2[4]);
	bool checkLine(Point pt1,Point pt2);
	bool checkSigleLine(Point pt1,Point pt2);
	bool m_isFind;

	vector<cocos2d::Point> m_road;
	vector<BlockLine> blockLines;
	//double theta;
	double rSelf;
	int TOTAL_MAP_WIDTH;
	int TOTAL_MAP_HEIGHT;
	double mSelfWidth;
	double mSelfHeight;
};

#endif
