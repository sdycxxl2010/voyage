/*
*Header File:TVGlobalConfig.h
*@author emps
*
*/

#ifndef _H_TV_GLOBAL_CONFIG_
#define _H_TV_GLOBAL_CONFIG_
#include "TVBasic.h"
class TVGlobalConfig : public SingletonPtr<TVGlobalConfig>
{
	friend class SingletonPtr<TVGlobalConfig>;
public:
	TVGlobalConfig();
	virtual ~TVGlobalConfig();

public:
	int Enalbe_NewUIArch;


private:

};

#endif

