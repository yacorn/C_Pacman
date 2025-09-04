#ifndef SOUND_H
#define SOUND_H

#include "common.h"
#include "maps.h"

// 사운드 관련 변수 (extern)
// extern int is_bgm_playing;
// extern int power_music_active;
// extern int ghost_back_active;
// extern int current_siren_level;

// 사운드 함수
void playSoundMci(const char* filename, const char* alias, int loop);
void playSoundAndWait(const char* filename, const char* alias);
void stopSoundMci(const char* alias);
void stopAllGameSounds();
void debug_log(const char* format, ...);
int isSoundFinished(const char* alias);
void handleSound();
void updateBackGroundMusic();
void setBgmPlaying(int playing);
int isBgmPlaying();
void setCurrentSirenLevel(int level);
int getCurrentSirenLevel();
void setPowerMusicActive(int active);
int isPowerMusicActive();

#endif // SOUND_H