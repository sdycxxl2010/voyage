#include "TVHero.h"
#include "ProtocolThread.h"

TVHero::TVHero()
{
	init();
}

TVHero::~TVHero()
{

}

bool TVHero::init()
{
	m_iDeleteNum = 0;
	 m_iGender = 0;
	 m_iCoin = 0;
	 m_iGold = 0;
	 m_iIconidx = 0;
	 m_iID = 0;
	 m_iLevel = 0;
	 m_iNation = 0;
	 m_iCityID = 0;
	 m_iCurCityNation = 0;
	 m_iGuildId = 0;
	 shippos=Vec2(0,0);
	 m_pBattelData = nullptr;
	 m_bShipStateIsAuto=false;
	 m_iTotalcontinuousdays=-1;
	 m_iNdailyrewarddata=-1;
	 m_iDailyrewarddata=nullptr;
	 m_iBornCity=-1;
	 m_escortCitySucces=false;
	 m_iNwantedforceids=-1;
	 m_iWantedforceids=nullptr;
	 for (int i=0;i<CITY_NUMBER;i++)
	 {
		 findCityStatues.push_back(false);
	 }
	 shipGuideEnemy = false;
	 m_iExp = 0;                             
	 m_iRexp = 0;                            
	 m_iStage = -1;
	 m_iSilver = 0;
	 m_iVp = 0;
	 m_bSeaToCenter = false;
	 m_heroIsOnsea = false;
	 m_iHave_licence_cityId = 0;
	 m_iMyEmailAccountVerified = false;
	 m_iAccountSafe = true;
	 m_forceNPCId = 0;
	 m_isShowOutLineResult = true;
	 m_londonWeather = 0;
	 m_nBossEventId = 0;
	 m_lastBattleNotComplete = false;
	 m_nTriggerId = 0;
	 m_InvitedToGuildNation = 0;
	 m_bClickTouched = false;
	 m_bInSafeArea = true;
	 m_bGoshipFixDirect = false;
	 m_bGameMaster = false;
	 m_bDialogToTarven = false;
	 m_Infoshowtime = 0;
	 m_waitTime = 0;
	 m_nAttackCityId = 0;
	 m_nSystemMessageType = 0;
	 m_vSystemInfo.clear();
	 m_centerTosea = false;
	 return true;
}

bool TVHero::clean(){
	//FIXME 先内存释放，再init
	init();
	return true;
}

bool TVHero::initHeroInfo(EmailLoginResult* result)
{
	m_iCoin = result->coin;
	m_iExp = result->exp;
	m_iRexp = result->prestige;
	m_iLevel = EXP_NUM_TO_LEVEL(result->exp);
	m_iPrestigeLv = FAME_NUM_TO_LEVEL(result->prestige);
	m_iGender = result->gender;
	m_iIconidx = result->iconidx;
	m_iGold = result->gold;
	if(result->heroname){
		m_sName = result->heroname;
	}else{
		m_sName = "Unknown";
	}
	m_iNation = result->nation;
	m_iCityID = result->lastcityid;
	m_iID = result->cid;
	m_iGuildId = result->guildid;
	m_iHaslisence = 0;
	m_iEffectivelisence = 1;
	m_escortCitySucces = 0;
	m_iTotalcontinuousdays=result->totalcontinuousdays;
	m_iNdailyrewarddata=result->n_dailyrewarddata;
	m_iDailyrewarddata=result->dailyrewarddata;

	getHeroBornCity(result->nation);

	if (result->isgamemaster)
	{
		m_bGameMaster = true;
	}
	else
	{
		m_bGameMaster = false;
	}
	return true;
}

bool TVHero::initHeroInfo(LoginFirstData* result)
{
	
	m_iCoin = result->coin;
	m_iLevel = EXP_NUM_TO_LEVEL(result->exp);
	m_iPrestigeLv = FAME_NUM_TO_LEVEL(result->prestige);
	m_iGender = result->gender;
	m_iIconidx = result->iconidx;
	m_iGold = result->gold;
	m_sName = result->heroname;
	m_iNation = result->nation;
	m_iCityID = result->lastcity->cityid;
	m_iID = result->cid;
	m_iGuildId = result->guildid;
	m_iHaslisence = 0;
	m_iEffectivelisence = 1;
	m_escortCitySucces = 0;

	m_iNwantedforceids=result->n_wantedforceids;
	m_iWantedforceids=result->wantedforceids;
	getHeroBornCity(result->nation);

	if (result->isgamemaster)
	{
		m_bGameMaster = true;
	}
	else
	{
		m_bGameMaster = false;
	}
	return true;
}

void TVHero::getHeroBornCity(int nationId)
{
	m_iBornCity=1;
	if (nationId==2)
	{
		m_iBornCity=2;
	}
	else if (nationId==3)
	{
		m_iBornCity=3;
	}
	else if (nationId==4)
	{
		m_iBornCity=4;
	}
	else if (nationId==5)
	{
		m_iBornCity=6;
	}
}
void TVHero::moveTo(int direct)
{
	
}
bool TVHero::getFindCityStatus(int cityIdIndex)
{
	if (cityIdIndex>findCityStatues.size())
	{
		return false;
	}
	else
	{
		return findCityStatues.at(cityIdIndex);
	}
}
void TVHero::setFindCityStatus(int cityIdIndex, bool haveFind)
{
	if (cityIdIndex <= findCityStatues.size())
	{
		findCityStatues.at(cityIdIndex) = haveFind;
	}
}
