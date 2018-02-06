#ifndef __SAIL_ROAD_H__
#define __SAIL_ROAD_H__

struct RoadPoint{
	int x;
	int y;
};

class SailRoadData{
public:
	SailRoadData();
	~SailRoadData();
	RoadPoint**points;
	int num;
	int startCity;
	int endCity;
};

class SailRoad
{
public:
	SailRoad();
	~SailRoad();


	SailRoadData**roadData;
	SailRoadData* resultData;
	bool isNormal;
	int roadNum;
	void initSailRoadData();
	SailRoadData*getSailRoad(int cityid1,int cityid2);
private:

};


#endif