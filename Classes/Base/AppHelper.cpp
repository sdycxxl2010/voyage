#include "AppHelper.h"
#include "TVBasic.h"

int   AppHelper::frameCount = 0;
float AppHelper::frameTime = 0.0f;
float AppHelper::time = 0.0f;

void AppHelper::InitHelper()
{
	AppHelper *helper = new AppHelper();

	Director::getInstance()->getScheduler()->scheduleUpdate(helper, -127, false);
}

void AppHelper::update(float delta)
{
	frameCount++;
	frameTime = delta;
	time += delta;
}