#ifndef _H_UI_FIRST_START_
#define _H_UI_FIRST_START_
#include "IUIBase.h"

class UIFirstScreen : public IUIBase
{
public:
	UIFirstScreen();
	~UIFirstScreen();

public:
	void OnInit() override;

	void OnShow() override;

	void OnHide() override;

	void OnUpdate() override;
};


#endif