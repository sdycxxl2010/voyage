#ifndef __MY_DRAW_H__
#define __MY_DRAW_H__
#include "cocos2d.h"
#include "AStar.h"
class MyDrawLine : public cocos2d::Node
{
public:
	MyDrawLine();
	~MyDrawLine();
	static MyDrawLine* create();
	void drawLineByPoint(cocos2d::Point* points,int pNum,float f);
	void drawLineByVector(std::vector<cocos2d::Point>& points,float f = 0);
	void drawPathByVector(std::vector<RoadNode>& roadNode);
	void draw(cocos2d::Renderer *renderer, const cocos2d::kmMat4 &transform, uint32_t transformUpdated) override;
private:
	cocos2d::CustomCommand _customCommand;
	void onDraw(const cocos2d::kmMat4 &transform, uint32_t transformUpdated);
	int m_curTime;
	int m_curPointNum;
	int m_PNum;
	std::vector<cocos2d::Vec2> m_Circles;
	std::vector<int> m_radians;
	std::vector<cocos2d::Vec2> m_Points;
	
};



#endif