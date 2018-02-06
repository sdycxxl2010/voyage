#ifndef __SHIP_H__
#define __SHIP_H__
#include "cocos2d.h"
#include "SailRoad.h"
#include "TVSailDefineVar.h"
#include "Particle3D/CCParticleSystem3D.h"
#include "Particle3D/PU/CCPUParticleSystem3D.h"
#define SPEEDIATE 10
//#define SECONDS_PER_DAY 6000 //10 secs

USING_NS_CC;
class Ship3D :public Sprite3D
{
public:
	static Sprite3D* create(const std::string &modelPath);
	void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags);
};

struct _ShipRoutine;

class UIShip : public Node
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


	UIShip(int sid);
	~UIShip();

	static UIShip* create(int sid = 1);
	void onEnter();
	void onExit();

	bool init(int sid);
	void talk(int id);
	void MoveEnd();
	void autoSail();
	void setParent(Node* parent);
	void setIsCenterX(bool isX){ m_bIsCenterX = isX;};
	void setIsCenterY(bool isY){ m_bIsCenterY = isY;};
	void setGoldPosition(cocos2d::Vec2 pos);//船移动的位置保存
	void setAutoSailGoldPosition(Vec2 pos);//自动航行位置保存
	void changeShipDirect();//船的位置改变
	void setUpdatePos(Point pos);
	void setUpdateSize(Size wh);
	/*船只暂停航行*/
	void stopShip(bool isStop);
	void setSupplyDays(int days);
	void setCurrentCrew(int crewnum);
	int  getCurrentCrew() { return m_CurrentCrew; };
	void updateUseDays(float f,int64_t curTime);

	void setSpeed(int sp);
	float getSpeed();

	void isContinueAutoSailing(bool isBreak);
	void updateSmallMaps();
	void setAutoSailRoad(SailRoad* sr) { m_SailRoad = sr; };
	int64_t	getCurrentTimeUsev();
	/*true 海上事故发生 ,false 海上事件结束*/
	void setSeaAccident(bool isHave); 
	/*是否是自动航行*/
	bool m_IsAutoSailing;
	bool m_autoSailInterrupt;
	/*海上航海时间暂停*/
	void startPauseTime(){ pausedStartMillis = getCurrentTimeUsev(); }
	/*取消海上航海时间暂停*/
	void stopPauseTime(){
		if (!pausedStartMillis) return;
		pausedMillis += getCurrentTimeUsev() - pausedStartMillis;
		m_lastUpdateMilliSec = 0;
		pausedStartMillis = 0;
		resume();
	 }
	void setLastSailDays(int days){ m_lastSailingDays = days; }
	void setFalgVisible(bool isVisible) { ship_falg->setVisible(isVisible); }
	void setServerConfigForShip(int reportInterval,int secPerDay);
	int getSecondsPerSailDay(){return m_secondsPerSailDay;}
	int getReportInterval(){ return m_reportPosInterval;};
	int64_t m_PauseTime;

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

	//船的转向点0元素为起始位置
	std::vector<Vec2> m_dirChangePos;
	//船转向的事件0元素为船只起始事件
	std::vector<int64_t> m_timePoints;
	//水手死亡的浮动信息
	void sailorsDieAnimation(int diedNUm = 0);
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
	//构造位置数据结构发送给服务器，参数为当前时间，单位：毫秒
	struct _ShipRoutine*makeShipRoutine(int64_t curTime);
	//发送成功后，reset船的位置数据
	void cleanShipPostions(int64_t curTime);
	//获取当前毫秒数
	int64_t getCurrentMilliseconds();
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
	
	//int m_SupplyFlag;
	int m_CurrentCrew;
	int m_DieCrew;
	int m_CityPort[CITY_NUMBER][2];

	Sprite3D* _s3d;
	//船模型
	Sprite3D* m_sp;

	Node* _obj_x;

	SailRoad	*m_SailRoad;
	void setPath(std::vector<Point> path);
	void initCityPortPos();
	void update(float f);
	bool isCanLandCity();
	Sprite *m_dir;
	int64_t m_lastUpdateMilliSec;
	//这次登录航行的天数
	int m_sailingDays;
	//有多久没有增加航行天数了，毫秒
	int m_newUpdateDayCycleMillSec;
	PUParticleSystem3D*rootps;
	Sprite *ship_falg;
	int m_secondsPerSailDay;
	int m_reportPosInterval;//sec
	int64_t m_lastUpdatePosTime;
	int64_t m_lastChangeDirectTime;
};


#endif
