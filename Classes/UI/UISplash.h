#ifndef _H_UI_SPLASH_
#define _H_UI_SPLASH_
#include "IUIBase.h"

class UISplash : public IUIBase
{
public:
	UISplash();
	~UISplash();

public:
	virtual void OnInit() override;

	virtual void OnShow() override;

	virtual void OnUpdate() override;

	virtual void OnHide() override;


private:
	void openEula();
	void openStartPanel();
};


#endif