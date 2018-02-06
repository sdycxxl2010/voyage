#include "Localization.h"
#include "TVSDHeaders.h"
#include "StrUtils.h"
#include "Utils.h"

Localization::Localization()
{
	Init();
}

Localization::~Localization()
{
}

void Localization::Init()
{
	TEXTRESOOL->RegLocalizer(this);
}

string GetLocalStr(string& idskey)
{
	return Localization::getInstance()->GetLocalStr(idskey);
}

string  Localization::GetLocalStr(string& idskey)
{
	if (GetLocalizationString(idskey, &m_pData))
	{
		return m_pData.text;
	}
	return idskey;
}


bool Localization::GetLocalizationString(std::string& idsKey, TextData* td)
{
	if (idsKey.size() <= 1)
	{
		return false;
	}
	string key = StrUtils::trim(idsKey);
	SDLangDict * loc = SDLangDict::GetElement(key);
	if (loc != NULL)
	{
		td->name = loc->getname();
		LanguageType lang = (LanguageType)Utils::getLanguage();
		switch (lang)
		{
		case cocos2d::LanguageType::ENGLISH:
			td->text = loc->gettext_en_US();
			break;
		case cocos2d::LanguageType::CHINESE:
			td->text = loc->gettext_zh_CN();
			break;

		case cocos2d::LanguageType::TRADITIONAL_CHINESE:
			td->text = loc->gettext_zh_HK();
			break;
			/*
			case cocos2d::LanguageType::FRENCH:
			break;
			case cocos2d::LanguageType::ITALIAN:
			break;
			case cocos2d::LanguageType::GERMAN:
			break;
			case cocos2d::LanguageType::SPANISH:
			break;
			case cocos2d::LanguageType::DUTCH:
			break;
			case cocos2d::LanguageType::RUSSIAN:
			break;
			case cocos2d::LanguageType::KOREAN:
			break;
			case cocos2d::LanguageType::JAPANESE:
			break;
			case cocos2d::LanguageType::HUNGARIAN:
			break;
			case cocos2d::LanguageType::PORTUGUESE:
			break;
			case cocos2d::LanguageType::ARABIC:
			break;
			case cocos2d::LanguageType::NORWEGIAN:
			break;
			case cocos2d::LanguageType::POLISH:
			break;
			case cocos2d::LanguageType::TURKISH:
			break;
			case cocos2d::LanguageType::UKRAINIAN:
			break;
			case cocos2d::LanguageType::ROMANIAN:
			break;
			case cocos2d::LanguageType::BULGARIAN:
			break;
			*/
		default:
			td->text = loc->gettext_en_US();
			break;
		}
		return true;
	}
	return false;
}