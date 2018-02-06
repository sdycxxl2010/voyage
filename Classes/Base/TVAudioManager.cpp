#include "TVAudioManager.h"
#include "EnumList.h"
#include "TVSingle.h"


TVAudioManager::TVAudioManager()
	:m_isEffectON(true),
	m_isBackgroundMusicON(true),
	_music_type(-1)
{
}

TVAudioManager::~TVAudioManager()
{
}

void TVAudioManager::vplayBGMusic(const char* fileName)
{
	if (m_isBackgroundMusicON)
	{
		SimpleAudioEngine::getInstance()->playBackgroundMusic(fileName,true);
	}
}
bool TVAudioManager::isPlayingBGMusic()
{
		return SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}
int TVAudioManager::iplayEffect(const char* fileName,bool isLoop)
{
	if (m_isEffectON)
	{
		int id = SimpleAudioEngine::getInstance()->playEffect(fileName,isLoop);
		return id;
	}
	return -1;
}

void TVAudioManager::vstopBGMusic()
{
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	_music_type = -1;
	m_isBackgroundMusicON = false;
}

void TVAudioManager::vplayMusicByType(int type)
{
	log("play music : %d, last: %d", type, _music_type);
	if(_music_type != type){
		switch(type){
			case MUSIC_CITY:
				vplayPortBGMusic();
				break;
			case MUSIC_PALACE:
				vplayPalaceBGMusic();
				break;
			case MUSIC_TAVERN:
				vplayBarBGMusic();
				break;
			case MUSIC_BATTLE:
				vplayBattleBGMusic();
				break;
			case MUSIC_SAILING:
				vplaySailingBGMusic();
				break;
			case MUSIC_AFTER_BATTLE:
				vplayAfterBattleBGMusic();
				break;
			case MUSIC_START:
			    vplayStartBGMusic();
				break;
			case MUSIC_BEFORE_BATTLE:
				vplayBeforeBattleBGMusic();
				break;
			default:
				break;
			}
		_music_type = type;
	}

}

void TVAudioManager::vplayEffectByType(int type,bool bloop)
{
	SimpleAudioEngine::getInstance()->playEffect(audio_effect[type],bloop);
}

void TVAudioManager::vplayPortBGMusic()
{
	int cityId	= SINGLE_HERO->m_iCityID;
	log("AudioSingleMG city id:%d", cityId);
	int nationId = 0;
	if (SINGLE_HERO->m_iCurCityNation)
	{
		nationId = SINGLE_HERO->m_iCurCityNation;
	}
	else
	{
		nationId = SINGLE_SHOP->getCitiesInfo()[cityId].nation;
	}

	//vplayBGMusic(audio_port[nationId-1]);
	
	//change
	if (cityId==1||cityId==2||cityId==3)
	{
		vplayBGMusic(audio_port[nationId-1]);
	}
	else if (nationId==4||nationId==5||nationId==6||nationId==8)
	{
		vplayBGMusic(audio_port[nationId-1]);
	}
	else
	{
		vplayBGMusic(audio_port[12]);
	}
}
void TVAudioManager::vpreLoadBGMusic(const char* fileName)
{
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic(fileName);
}

void TVAudioManager::vpreLoadEffect(const char* fileName)
{
	SimpleAudioEngine::getInstance()->preloadEffect(fileName);
}

void TVAudioManager::vpurgeEffect(const char* fileName)
{
	SimpleAudioEngine::getInstance()->unloadEffect(fileName);
}
//normal button
void TVAudioManager::vplayButtonEffect(int flag)
{
	if (m_isEffectON)
	{
		SimpleAudioEngine::getInstance()->playEffect(audio_effect[flag]);
	}
}
void TVAudioManager::vplayBankBGMusic()
{
	int cityId	= SINGLE_HERO->m_iCityID;
	int nationId = 0;
	if (SINGLE_HERO->m_iCurCityNation)
	{
		nationId = SINGLE_HERO->m_iCurCityNation;
	}
	else
	{
		nationId = SINGLE_SHOP->getCitiesInfo()[cityId].nation;
	}
	vplayBGMusic(audio_bar[nationId-1]);
}
void TVAudioManager::vplayBarBGMusic()
{
	int cityId	= SINGLE_HERO->m_iCityID;
	int nationId = 0;
	if (SINGLE_HERO->m_iCurCityNation)
	{
		nationId = SINGLE_HERO->m_iCurCityNation;
	}
	else
	{
		nationId = SINGLE_SHOP->getCitiesInfo()[cityId].nation;
	}
	if (nationId == 8)
	{
		nationId = 1;
	}else
	{
		nationId = 0;
	}
	vplayBGMusic(audio_bar[nationId]);
}
void TVAudioManager::vplayShipYardBGMusic()
{
	int cityId	= SINGLE_HERO->m_iCityID;
	int nationId = 0;
	if (SINGLE_HERO->m_iCurCityNation)
	{
		nationId = SINGLE_HERO->m_iCurCityNation;
	}
	else
	{
		nationId = SINGLE_SHOP->getCitiesInfo()[cityId].nation;
	}
	vplayBGMusic(audio_port[nationId-1]);
}
void TVAudioManager::vplayExchangeBGMusic()
{
	int cityId	= SINGLE_HERO->m_iCityID;
	int nationId = 0;
	if (SINGLE_HERO->m_iCurCityNation)
	{
		nationId = SINGLE_HERO->m_iCurCityNation;
	}
	else
	{
		nationId = SINGLE_SHOP->getCitiesInfo()[cityId].nation;
	}
	vplayBGMusic(audio_port[nationId-1]);
}
void TVAudioManager::vplayPalaceBGMusic()
{
	int cityId	= SINGLE_HERO->m_iCityID;
	int nationId = 0;
	if (SINGLE_HERO->m_iCurCityNation)
	{
		nationId = SINGLE_HERO->m_iCurCityNation;
	}
	else
	{
		nationId = SINGLE_SHOP->getCitiesInfo()[cityId].nation;
	}
	if (nationId == 8)
	{
		nationId = 1;
	}else
	{
		nationId = 0;
	}
	vplayBGMusic(audio_palace[nationId]);
}

void TVAudioManager::vplaySailingBGMusic()
{
	vplayBGMusic(audio_sail[0]);
}

void TVAudioManager::vplayBattleBGMusic()
{
	vplayBGMusic(audio_sail[2]);
}

void TVAudioManager::vplayBeforeBattleBGMusic()
{
	vplayBGMusic(audio_sail[4]);
}

void TVAudioManager::vplayAfterBattleBGMusic()
{
	vplayBGMusic(audio_sail[3]);
}
void TVAudioManager::vplayStartBGMusic()
{
	vplayBGMusic(audio_start[0]);
}

