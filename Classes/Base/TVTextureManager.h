#ifndef __TEXTURE_MANAGE_H__
#define __TEXTURE_MANAGE_H__

#include "cocos2d.h"

class TVTextureManager
{
public:
	TVTextureManager();
	~TVTextureManager();

	inline static TVTextureManager*  getInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new TVTextureManager;
		}
		return m_Instance;
	}

	void startTextureManage();
	void stopTextureManage();
	inline void release(){ delete m_Instance;};
	void check(float dt);
	void printTextureLog();
	// dt uint is sec
	inline void setLifeTime(float dt) { m_LifeTime = int(dt * 60); };
	inline int getLifeTime() { return m_LifeTime; };
	inline void setPrintLog(bool isPrint) { m_IsPrintLog = isPrint; };
private:
	static TVTextureManager* m_Instance;
	int m_LifeTime;
	int m_LifeFlag;
	bool m_IsPrintLog;
};

#endif



