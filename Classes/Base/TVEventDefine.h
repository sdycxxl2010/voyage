/*
*Header File:
*@author emps
*
*/

#ifndef _H_TV_EVENT_DEFINE_
#define _H_TV_EVENT_DEFINE_
typedef enum ENUM_TYPE
{
	_NONE = - 1000,
	_START_0 = 0,
	_APP,
	
	_NET,
	_NET_RECONNECT,

	_NET_SELECT_SERVER,

	_DATA_,
	_EVT_FINISH_REQUEST_SERVER_LIST,

	_UI,

	_END,

}TVEventType;



#endif