#ifndef _H_UI_EULA_
#define _H_UI_EULA_
#include "IUIBase.h"
class UIEula : public IUIBase
{
public:
	UIEula();
	~UIEula();

public:
	virtual void OnInit() override;

	virtual void OnShow() override;

	virtual void OnUpdate() override;

	virtual void OnHide() override;


private:
	
};


#endif