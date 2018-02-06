#include "SailRoad.h"

#include "cocos2d.h"
USING_NS_CC;

SailRoadData::SailRoadData(){
	
}

SailRoadData::~SailRoadData(){

}


SailRoad::SailRoad()
{
	roadData = 0;
	roadNum = 0;
	resultData = nullptr;
}

SailRoad::~SailRoad()
{
	for(int i =0;i<roadNum;i++){
		SailRoadData*data = roadData[i];
		if(data){
			for(int j =0;j<data->num;j++){
				if(data->points[j]){
					delete data->points[j];
				}
			}
			delete data;
			roadData[i] = 0;
		}
	}
	delete [] roadData;
}

SailRoadData* SailRoad::getSailRoad(int cityid1,int cityid2){
	SailRoadData*resData = 0;
	for(int i =0;i<roadNum;i++){
		SailRoadData*data = roadData[i];
		if(data){
			if(data->startCity == cityid1 && data->endCity == cityid2){
				resData = data;
				isNormal = true;
				break;
			}
// 			else if(data->startCity == cityid2 && data->endCity == cityid1){
// 				resData = data;
// 				isNormal = false;
// 				break;
// 			}
		}
	}
	resultData = resData;
	return resData;
}

void SailRoad::initSailRoadData(){
	std::string path = CCFileUtils::getInstance()->fullPathForFilename("res/map_cell/road");
	ssize_t size = 0;
	unsigned char*charData = FileUtils::getInstance()->getFileData(path,"r",&size);
	if(charData){
		unsigned char*start = charData;
		unsigned char*end = start + size;
		int lineNum = 0;
		while(start < end && *start != '\n' && *start != '\r'){
			lineNum = *start-0x30 + lineNum*10;
			start++;
		}
		roadNum = lineNum;
		roadData = new SailRoadData*[lineNum];
		memset(roadData,0,sizeof(SailRoadData*)*lineNum);

		while(start < end && (*start == '\n' || *start == '\r'))
			start++;

		int line = 0;
		while(start < end){			
			int startId = 0;
			int endId = 0;
			int pointNum = 0;
			while(start < end && *start != '-'){
				startId = *start-0x30 + startId*10;
				start++;
			}
			//while((*start == '\n' || *start == '\r') && start < end)
			start++;

			while(start < end && *start != ':'){
				endId = *start-0x30 + endId*10;
				start++;
			}
			start++;

			while(start < end && *start != ':'){
				pointNum = *start-0x30 + pointNum*10;
				start++;
			}
			start++;

			if(line<lineNum){
				int value = 0;
				int valueNum = 0;
				SailRoadData*newRoad = new SailRoadData;
				newRoad->startCity = startId;
				newRoad->endCity = endId;
				newRoad->num = pointNum;
				newRoad->points = 0;
				if(pointNum > 0)
					newRoad->points = new RoadPoint*[pointNum];
				RoadPoint point; 
				while(start<end && *start !='\n' && *start != '\r'){
					if(*start == ','){						
						point.y = value;
						if(valueNum < pointNum){
							newRoad->points[valueNum] = new RoadPoint;
							*newRoad->points[valueNum] = point;
							valueNum ++;
						}
						value = 0;
						start ++;

						if(start>= end || *start =='\n' || *start == '\r'){
							break;
						}
					}else if(*start == '-'){
						point.x = value;
						if(start>= end || *start =='\n' || *start == '\r'){
							break;
						}
						value = 0;
						start ++;
					}
					value = *start-0x30 + value*10;
					start ++;
				}
				point.y = value;

				newRoad->points[valueNum] = new RoadPoint;
				*newRoad->points[valueNum] = point;

				roadData[line] = newRoad;
			}			

			while(start < end && (*start == '\n' || *start == '\r'))
				start++;

			line ++;
		}
		free(charData);
	}

	// 	Data data = FileUtils::getInstance()->getDataFromFile(path);
	// 	unsigned char*charData = data->getBytes();
	// 	int size = data->getContentSize();
}
