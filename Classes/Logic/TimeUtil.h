/*
*
*  CopyRight (c) ....
*  Created on: 2016Äê06ÔÂ13ÈÕ
*  Author:Sen
*  Time Util
*
*/

#ifndef __TIME_UTIL__
#define __TIME_UTIL__

#include "TVBasic.h"

typedef enum _TIME_UTIL_FORMAT
{
	_TU_NONE,
	_TU_ALL,
	_TU_YEAR_MON_DAY,
	_TU_HOUR_MIN,
	_TU_HOUR_MIN_SEC,
	_TU_MIN_SEC
}TIME_UTIL;

typedef enum _TIME_TYPE
{
	_TT_NONE,
	_TT_GM,
	_TT_LOCAL,
	
}TIME_TYPE;


static std::string timeUtil(int64_t curTime, TIME_UTIL time_util_format, TIME_TYPE time_type)
{
	time_t t_sendTime = time_t(curTime);
	tm* formTime;
	switch (time_type)
	{
	case _TT_GM:
		formTime = gmtime(&t_sendTime);
		break;
	case _TT_LOCAL:
		formTime = localtime(&t_sendTime);
		break;
	default:
		log("timeUtil time_type error");
		break;
	}

	char buff[80];
	switch (time_util_format)
	{
	case _TU_ALL:
		sprintf(buff, "%4d/%02d/%02d %02d:%02d", formTime->tm_year + 1900, formTime->tm_mon + 1, formTime->tm_mday, formTime->tm_hour, formTime->tm_min);
		break;
	case _TU_YEAR_MON_DAY:
		sprintf(buff, "%4d/%02d/%02d", formTime->tm_year + 1900, formTime->tm_mon + 1, formTime->tm_mday);
		break;
	case _TU_HOUR_MIN:
		sprintf(buff, "%02d:%02d", formTime->tm_hour, formTime->tm_min);
		break;
	case _TU_HOUR_MIN_SEC:
		sprintf(buff, "%02d:%02d:%02d", formTime->tm_hour, formTime->tm_min, formTime->tm_sec);
		break;
	case _TU_MIN_SEC:
		sprintf(buff, "%02d:%02d", formTime->tm_min, formTime->tm_sec);
		break;
	default:
		log("timeUtil time_util_format error");
		break;
	}
	
	log("timeUtil time %s", buff);
	return buff;
}

#endif