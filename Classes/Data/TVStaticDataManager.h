#ifndef _H_STATIC_DATA_MANAGER_H
#define _H_STATIC_DATA_MANAGER_H
#include "TVStaticDataBase.h"

class TVStaticDataManager:public SingletonPtr<TVStaticDataManager>
{
	friend class SingletonPtr<TVStaticDataManager>;
public:
	TVStaticDataManager();
	~TVStaticDataManager();
	
private:
	void LoadStrTableData(const char* fileName);
	void LoadIntTableData(const char* fileName);

	void InitLoadTableList();

public:

	TVStaticDataBase* GetElementByIntId(const char* eleName, int eId)
	{
		map<int, TVStaticDataBase*>* mapIns = intKeyTableDB[eleName];
		if (mapIns != 0 && mapIns->find(eId) != mapIns->end())
		{
			return mapIns->at(eId);
		}
		return 0;
	}

	TVStaticDataBase* GetElementByStrId(const char* eleName, const char* eId)
	{
		map<string, TVStaticDataBase*>* mapIns = strKeyTableDB[eleName];
		if (mapIns != 0 && mapIns->find(eId) != mapIns->end())
		{
			return mapIns->at(eId);
		}
		return 0;
	}

	map<string, TVStaticDataBase*>* GetStrTable(const char* tableName)
	{
		if (strKeyTableDB.find(tableName) != strKeyTableDB.end())
		{
			return strKeyTableDB[tableName];
		}
		return 0;
	}

	map<int, TVStaticDataBase*>* GetIntTable(const char* tableName)
	{
		if (intKeyTableDB.find(tableName) != intKeyTableDB.end())
		{
			return intKeyTableDB[tableName];
		}
		return 0;
	}

private:

	map<string, map<string, TVStaticDataBase*>*> strKeyTableDB;
	map<string, map<int, TVStaticDataBase*>*> intKeyTableDB;

};


#endif