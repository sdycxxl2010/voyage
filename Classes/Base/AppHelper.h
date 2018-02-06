#ifndef _H_APP_HELPER_
#define _H_APP_HELPER_
#include "TVBasic.h"

class AppHelper : CCObject
{
public:
	static int   frameCount;
	static float frameTime;
	static float time;

public: 
	static void InitHelper();

public:
	void update(float delta);

};

#endif // !_H_APP_HELPER_
