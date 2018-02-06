/*
*Header File:
*@author emps
*
*/
#ifndef _H_STRUTILS_
#define _H_STRUTILS_
#include <string>
#include <iostream>

class StrUtils
{
public:
	static std::string ltrim(std::string &p);
	static std::string rtrim(std::string &p);
	static std::string trim(std::string &p);
private:

};
#endif
