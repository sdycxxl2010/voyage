#ifndef __SHIPGUIDE_H__
#define __SHIPGUIDE_H__
#include "cocos2d.h"
#include "SailRoad.h"
#include "TVSailDefineVar.h"
#include "Particle3D/CCParticleSystem3D.h"
#include "Particle3D/PU/CCPUParticleSystem3D.h"
#define SPEEDIATE 10
#define SECONDS_PER_DAY 6000

USING_NS_CC;
class Ship3DGuide :public Sprite3D
{
public:
	static Sprite3D* create(const std::string &modelPath);
	void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
};

struct _ShipRoutine;

class UIGuideShip : public Node
{
public:

	enum DIRECT_LOAD
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		L_UP,
		L_DOWN,
		R_UP,
		R_DOWN,
	};
	UIGuideShip(int sid);
	~UIGuideShip();

	static UIGuideShip* create(int sid = 1);
	void onEnter();
	void onExit();

	bool init(int sid);
	//自动航行
	void MoveEnd();
	void autoSail();
	//船的父节点
	void setParent(Node* parent);
	void setIsCenterX(bool isX){ m_bIsCenterX = isX; };
	void setIsCenterY(bool isY){ m_bIsCenterY = isY; };
	//船移动的位置保存
	void setGoldPosition(cocos2d::Vec2 pos);
	//自动航行位置保存
	void setAutoSailGoldPosition(Vec2 pos);
	//船的位置改变
	void changeShipDirect();
	//船的位置
	void setUpdatePos(Point pos);
	//船的大小
	void setUpdateSize(Size wh);
	//停止船
	void stopShip(bool isStop);
	//航行天数
	void setSupplyDays(int days);
	//水手
	void setCurrentCrew(int crewnum);
	int  getCurrentCrew() { return m_CurrentCrew; };
	void updateUseDays(int64_t curTime);
	//速度
	void setSpeed(int sp);
	float getSpeed();
	//是否停止自动航行
	void isContinueAutoSailing(bool isBreak);
	void setAutoSailRoad(SailRoad* sr) { m_SailRoad = sr; };
	int64_t	getCurrentTimeUsev();

	void setSeaAccident(bool isHave); // true 海上事故发生 ,false 海上事件结束
	bool m_IsAutoSailing;
	bool m_autoSailInterrupt;
	void startPauseTime(){ pausedStartMillis = getCurrentTimeUsev(); }
	void stopPauseTime(){
		if (!pausedStartMillis) return;
		pausedMillis = getCurrentTimeUsev() - pausedStartMillis;
		m_lastUpdateMilliSec = 0;
		pausedStartMillis = 0;
	}
	void setLastSailDays(int days){ m_lastSailingDays = days; }
	/*
	* 读取骨点名中信息
	* @param name 骨骼名字
	*/
	std::vector<std::string> analysisName(std::string name);

	/**
	*船的待机动画点
	*/
	void  addStandByPoint();


	/**
	*添加船部件
	*@param p_sender 当前加载的船只
	*/
	void addSail(Sprite3D * p_Sender);

	/**
	*船部件确认
	*@parm bone 需要添加部件的骨骼
	*/
	Sprite3D * confirmSail(Bone3D * bone);

	struct _ShipRoutine*makeShipRoutine();
   protected:
	/**
	*船桨编号
	*/
	int m_oars_index;
	/**
	*旗帜Mesh编号
	*/
	int m_flag_index;

	/**
	*船的运动点
	*/
	Sprite3D * m_standPoint;
private:
	int m_lastSailingDays;
	int64_t pausedStartMillis;
	int64_t pausedMillis;
	Sprite* m_vImage;
	Node* m_pParent;
	Point m_TargetPos;
	Node* m_CityLayer;
	float m_ShipDirect;
	Point m_PreUpdatePos;
	Size m_UpdateSize;
	Size m_WinSize;
	bool m_IsShipMoving;
	bool m_MoveEnd;
	bool m_bIsCenterX;
	bool m_bIsCenterY;
	bool m_IsHaveCheckCity;
	bool m_IsPause; //遇到海上事件判断

	Camera* camera;
	cocos2d::Point startPosition;
	cocos2d::Point endPosition;

	int m_ShipId;
	int m_CurrentIndex;  //自动航行的节点索引
	int m_SupplyDays;
	int m_useDay;
	int m_speed;

	float m_ptime;
	float m_atime;

	int64_t m_GoSeaTime;
	int64_t m_PauseTime;
	int m_SupplyFlag;
	int m_CurrentCrew;
	int m_DieCrew;
	int m_CityPort[CITY_NUMBER][2];
	Sprite3D* _s3d;
	Node* _obj_x;
	//船模型
	Sprite3D* m_sp;
	SailRoad	*m_SailRoad;
	void initCityPortPos();
	void update(float f);
	bool isCanLandCity();
	Sprite *m_dir;
	int64_t m_lastUpdateMilliSec;
};

#endif
