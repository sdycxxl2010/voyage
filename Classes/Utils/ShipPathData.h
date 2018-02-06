#ifndef __SHIP_PATH_H__
#define __SHIP_PATH_H__
#include "SystemVar.h"
#include "cocos2d.h"


static int strIcmp(const char* p1, const char* p2)
{ 
	while (*p1) 
	{ 
		char c1 = *p1; 
		char c2 = *p2; 

		if ((c1 | 32) != (c2 |32))
		{ 
			return c1 - c2; 
		} 
		p1++;
		p2++; 
	} 
	return *p1 - *p2; 
}

static int getCityIdFromName(const char*name){
	int id = 0;
	if(!name)
		return -1;
	int num = sizeof(cites_name_table)/sizeof(char*);
	if(id >= num) return -1;
	while(id < num ){
		if((strIcmp(name,cites_name_table[id])) == 0){
			break;
		}
		id++;
	}
	
	return id;
}

// int** getPointsForCities(int cityid1,int cityid2){
// 	return city_path_1_2;
// }



#endif