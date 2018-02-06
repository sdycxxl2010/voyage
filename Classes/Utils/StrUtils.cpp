#include "StrUtils.h"
#include <locale>
#include <algorithm>
std::string StrUtils::ltrim(std::string &str)
{
	auto it2 = std::find_if(str.begin(), str.end(), [](char ch){ return !std::isspace<char>(ch, std::locale::classic()); });
	str.erase(str.begin(), it2);
	return str;
}

std::string StrUtils::rtrim(std::string &str)
{
	auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch){ return !std::isspace<char>(ch, std::locale::classic()); });
	str.erase(it1.base(), str.end());
	return str;
}

std::string StrUtils::trim(std::string &p)
{
	std::string s = rtrim(p);
	return  ltrim(s);
}