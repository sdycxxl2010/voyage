/*
*Header File:
*@author emps
*
*/

#ifndef _TV_MACRO_H_
#define _TV_MACRO_H_

#include "assert.h"

#ifndef NULL
#define NULL 0
#endif

template <typename TYPE>
class  SingletonPtr
{
protected:
	static  TYPE*  m_pMySelf;

public:
	SingletonPtr()
	{
		assert(m_pMySelf == NULL);
		m_pMySelf = (TYPE*)this;
	}
	virtual ~SingletonPtr()
	{
		m_pMySelf = NULL;
	}

public:
	static  TYPE*  getInstance()
	{
		return m_pMySelf;
	}
};
template<typename TYPE> TYPE* SingletonPtr<TYPE>::m_pMySelf = NULL;



template <typename TYPE>
class  SingletonPtr2
{
protected:
	static  TYPE*  m_pMySelf;

public:
	SingletonPtr2()
	{
		assert(m_pMySelf == NULL);
		m_pMySelf = (TYPE*)this;
	}
	virtual ~SingletonPtr2()
	{
		m_pMySelf = NULL;
	}

public:
	static  TYPE*  getInstance()
	{
		if (m_pMySelf == NULL)
		{
			m_pMySelf = new TYPE();
		}
		return m_pMySelf;
	}
};
template<typename TYPE> TYPE* SingletonPtr2<TYPE>::m_pMySelf = NULL;

#endif
