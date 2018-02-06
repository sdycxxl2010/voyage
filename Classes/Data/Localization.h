/*
*Header File:
*@author emps
*
*/

#ifndef _H_LOCALIZATION_
#define _H_LOCALIZATION_
#include "TVBasic.h"
#include "cocostudio/TextResUtils.h"

string GetLocalStr(string& idskey);

class Localization : public SingletonPtr<Localization>, public TextResIdsProxy
{
	friend class SingletonPtr<Localization>;
public:
	Localization();
	virtual ~Localization();

public:
	virtual bool GetLocalizationString(std::string& idsKey, TextData* td) override;
	string GetLocalStr(string& idskey);
private:
	void Init();
	struct TextData m_pData;
	
};



#endif

