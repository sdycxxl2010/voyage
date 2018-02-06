#include "MyDrawLine.h"

USING_NS_CC;
MyDrawLine::MyDrawLine():
	m_PNum(0),
	m_curTime(0),
	m_curPointNum(0)
{
}

MyDrawLine::~MyDrawLine()
{
}

MyDrawLine* MyDrawLine::create()
{
	MyDrawLine* mdl = new MyDrawLine();
	if (mdl)
	{
		mdl->autorelease();
		return mdl;
	}
	CC_SAFE_RELEASE(mdl);
	return nullptr;
}
void MyDrawLine::drawLineByPoint(cocos2d::Point* points,int pNum,float f)
{
	
}
void MyDrawLine::drawLineByVector(std::vector<cocos2d::Point>& points,float f /* = 0 */)
{
	m_curPointNum = points.size();
	m_PNum = 0;
	m_Points.clear();
	m_radians.clear();
	m_Circles.clear();
	if (points.size() < 2)
	{
		return;
	}
	for (int i = 0; i < points.size(); i++)
	{
		m_Points.push_back(points[i]);
	}
	m_PNum = points.size();
}

void MyDrawLine::drawPathByVector(std::vector<RoadNode>& roadNode)
{
	m_Points.clear();
	m_Circles.clear();
	m_radians.clear();

	for (auto iter : roadNode)
	{
		if ((iter).turnR == 0)
		{
			m_Points.push_back(Vec2((iter).pt1));
			m_Points.push_back(Vec2((iter).pt2));
		}else
		{
			m_Circles.push_back((iter).centerPoint);
			m_radians.push_back((iter).turnR);
		}
	}
	//m_Circles.push_back()
}
void MyDrawLine::draw(cocos2d::Renderer *renderer, const kmMat4 &transform, uint32_t transformUpdated)
{
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(MyDrawLine::onDraw, this, transform, transformUpdated);
	renderer->addCommand(&_customCommand);
}

void MyDrawLine::onDraw(const kmMat4 &transform, uint32_t transformUpdated)
{
	kmGLPushMatrix();
	kmGLLoadMatrix(&transform);
	//draw
	{
		glLineWidth( 1.0f );
		DrawPrimitives::setDrawColor4B(255,0,0,255);
		if (m_Points.size() > 1)
		{
			for (auto i =0; i < m_Points.size() - 1; i++)
			{
				DrawPrimitives::drawLine(m_Points[i],m_Points[i + 1]);
			}
		}
		
		for (auto i =0; i < m_Circles.size(); i++)
		{
			DrawPrimitives::drawCircle(m_Circles[i],m_radians[i],0, 180, false);
		}
		int nowTime = time(nullptr);
		if (nowTime > m_curTime)
		{
			m_curTime = nowTime;
			
			if (m_curPointNum < m_PNum)
			{
				//m_curPointNum++;
			}
		}
		DrawPrimitives::setDrawColor4B(255,0,0,255);
	}
	CHECK_GL_ERROR_DEBUG();
	kmGLPopMatrix();
}