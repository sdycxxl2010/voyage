/*
*  CopyRight (c) 
*  Created on: 2014年11月18日
*  Author:  
*  description: some ways about music
*/
#ifndef __AUDIO_SINGLE_H__
#define __AUDIO_SINGLE_H__
#include "TVBasic.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

class TVAudioManager
{
public:
	TVAudioManager();
	~TVAudioManager();

	int  iplayEffect(const char* fileName,bool isLoop = false);//判断音效是否在播放
	void vplayBGMusic(const char* fileName);//播放背景音乐
	bool isPlayingBGMusic();//背景音乐是否在播放
	
	void vplayExchangeBGMusic();//交易所
	void vplayBankBGMusic();//银行
	void vplayShipYardBGMusic();//船坞

	void vplayButtonEffect(int falg);//按钮音效

	void vstopBGMusic();//停止背景音乐
	void vpurgeEffect(const char* fileName);//卸载音效
	void vpreLoadEffect(const char* fileName);//预加载音效
	void vpreLoadBGMusic(const char* fileName);//预加载背景音乐
	void vplayMusicByType(int type);//根据场景播放音乐
	void vplayEffectByType(int type,bool bloop = false);//播放音效
	
	inline void vpauseBGMusic(){ if(m_isBackgroundMusicON) SimpleAudioEngine::getInstance()->pauseBackgroundMusic();};//根据状态值是否暂停播放背景音乐
	inline void vresumeBGMusic(){ if(m_isBackgroundMusicON) SimpleAudioEngine::getInstance()->resumeBackgroundMusic();};//恢复
	inline void vpasueAllEffect(){ if(m_isEffectON) SimpleAudioEngine::getInstance()->pauseAllEffects();};//不播放音效
	inline void vresumeAllEffect(){ if(m_isEffectON) SimpleAudioEngine::getInstance()->resumeAllEffects();};//可以播放音效
	inline void setEffectON(bool pRet){ m_isEffectON = pRet; };//设置是否可以播放音效
	inline bool getEffectON(){ return m_isEffectON;};//得到音效状态值
	inline void setBackgroundMusicON(bool pRet) {m_isBackgroundMusicON = pRet;} ;//设置是否可以播放背景音乐
	inline bool getBackgroundMusicON() { return m_isBackgroundMusicON ;} ;//得到背景音乐状态
	inline void setMusicType(int type = -1) { _music_type = type; };

private:
	void vplayPortBGMusic();//播放背景音乐,也是港口音乐
	void vplayPalaceBGMusic();//播放王宫音乐
	void vplayBarBGMusic();//播放酒吧音乐
	void vplayBattleBGMusic();//战斗时的音乐
	void vplaySailingBGMusic();//航海时的音乐
	//攻击海盗前音乐
	void vplayBeforeBattleBGMusic();
	void vplayAfterBattleBGMusic();//战斗胜利后缅怀烈士的音乐
	//开始界面的背景音效
	void vplayStartBGMusic();

	bool m_isEffectON;//音效状态，是否播放
	bool m_isBackgroundMusicON;//背景音乐状态
	int _music_type;//保存场景音乐的类型
};

#endif



