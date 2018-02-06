/*
*Header File:
*@author emps
*
*/

#ifndef _H_TV_EVENT_MGR_
#define _H_TV_EVENT_MGR_
#include "TVBasic.h"
#include "TVGameObject.h"
#include "TVEventDefine.h"

typedef std::function<void()> TVEventCallback0;
typedef std::function<void(const string&)> TVEventCallback1;

class TVEventer : public Ref
{
public:
	TVEventer();
	~TVEventer();

public:
	void SetEventTarget0(TVEventType eType, Ref *target, const TVEventCallback0& callback);

	void SetEventTarget1(TVEventType eType, Ref *target, const TVEventCallback1& callback);

	void DoEvent();

	void DoEvent(const string& param);

	TVEventType eventType()
	{
		return m_eType;
	}
private:
	TVEventType m_eType;
	Ref* m_pTarget;
	TVEventCallback0 m_pCallback0;
	TVEventCallback1 m_pCallback1;
};



class TVEventManager : public SingletonPtr<TVEventManager>
{
public:
	TVEventManager();
	virtual ~TVEventManager();

public:

	static void PushEvent(TVEventType eType);
	static void PushEvent(TVEventType eType, string& params);

	static void RegEvent0(TVEventType eType, const TVEventCallback0& callback);
	static void RegEvent1(TVEventType eType, const TVEventCallback1& callback);

	static void UnRegEvent(TVEventType eType);

protected:
	void PushEventImpl(TVEventType eType);

	void PushEventImpl(TVEventType eType, string& params);

	void RegEventImpl(TVEventType eType, const TVEventCallback0& callback);

	void RegEventImpl(TVEventType eType, const TVEventCallback1& callback);

	void UnRegEventImpl(TVEventType eType);

private:
	std::vector<TVEventer*>  m_vEventList;
	std::vector<TVGameObject*> m_obj;
};

#endif