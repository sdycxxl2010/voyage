#ifndef _H_UI_EUNUMS_
#define _H_UI_EUNUMS_

#include "TVBasic.h"


typedef enum _E_WINDOWS
{
	_UI_BEGIN = 100, 
	_UI_SPLASH,
	_UI_EULA,
	_UI_START,
	_UI_LOGIN,
	_UI_CREATEROLE,
	_UI_MANAGEROLE,
}UI_WINDOWS;

const string UI_WINDOWS_STR[] = 
{
	"_UI_SPLASH",
	"_UI_LOGIN",
	"_UI_CREATEROLE",
	"_UI_MANAGEROLE",
};

class UIEnums
{
public:
	static void RegAllUI();
};

#endif