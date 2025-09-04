#include "sound.h"
#include "game.h"

static int is_bgm_playing = 0;
static int current_siren_level = 1;
static int power_music_active = 0;

// 사운드 관련 함수들 작성
void playSoundAndWait(const char* filename, const char* alias) {
    char command[256];
    const char* type = (strstr(filename, ".wav") || strstr(filename, ".WAV")) ? "waveaudio" : "mpegvideo";

    // [수정] 이전에 열려있던 같은 별명의 사운드를 먼저 닫습니다.
    sprintf(command, "close %s", alias);
    mciSendString(command, NULL, 0, NULL);

    sprintf(command, "open \"%s\" type %s alias %s", filename, type, alias);
    mciSendString(command, NULL, 0, NULL);

    sprintf(command, "play %s wait", alias);
    mciSendString(command, NULL, 0, NULL);

    sprintf(command, "close %s", alias);
    mciSendString(command, NULL, 0, NULL);
}

void playSoundMci(const char* filename, const char* alias, int loop) {
    char command[256];
    char error_buffer[512];
    char status_buffer[256];
    MCIERROR error;
    const char* type = (strstr(filename, ".wav") || strstr(filename, ".WAV")) ? "waveaudio" : "mpegvideo";
    int is_wav = (strstr(filename, ".wav") || strstr(filename, ".WAV")) ? 1 : 0;
    
    debug_log("=== playSoundMci called ===\n");
    debug_log("Filename: %s, Alias: %s, Loop: %d, Type: %s\n", filename, alias, loop, type);
    
    // 기존 것 완전히 정리
    sprintf(command, "stop %s", alias);
    mciSendString(command, NULL, 0, NULL);
    
    sprintf(command, "close %s", alias);
    mciSendString(command, NULL, 0, NULL);

    // 파일 존재 여부 확인
    FILE* file = fopen(filename, "r");
    if (!file) {
        debug_log("ERROR: File not found: %s\n", filename);
        return;
    }
    fclose(file);
    debug_log("File exists: %s\n", filename);

    // 파일 열기
    sprintf(command, "open \"%s\" type %s alias %s", filename, type, alias);
    debug_log("Command: %s\n", command);
    error = mciSendString(command, error_buffer, sizeof(error_buffer), NULL);
    
    if (error) {
        debug_log("Failed to open %s: %s (Error: %d)\n", filename, error_buffer, error);
        return;
    }
    debug_log("Successfully opened %s\n", filename);

    // 재생 명령 (파일 타입에 따라 다르게)
    if (loop && !is_wav) {
        // MP3 파일이고 반복 재생이면 repeat 사용
        sprintf(command, "play %s repeat", alias);
        debug_log("Play command (MP3 with repeat): %s\n", command);
    } else {
        // WAV 파일이거나 단일 재생이면 repeat 없음
        sprintf(command, "play %s", alias);
        debug_log("Play command (WAV or single): %s\n", command);
    }
    
    error = mciSendString(command, error_buffer, sizeof(error_buffer), NULL);
    
    if (error) {
        debug_log("Failed to play %s: %s (Error: %d)\n", filename, error_buffer, error);
        
        // MP3에서 repeat 실패시 단일 재생으로 폴백
        if (loop && !is_wav) {
            debug_log("Trying fallback without repeat...\n");
            sprintf(command, "play %s", alias);
            error = mciSendString(command, error_buffer, sizeof(error_buffer), NULL);
            if (!error) {
                debug_log("Fallback play successful\n");
            }
        }
    } else {
        debug_log("Play command successful for %s\n", filename);
        
        // 재생 후 상태 확인
        sprintf(command, "status %s mode", alias);
        mciSendString(command, status_buffer, sizeof(status_buffer), NULL);
        debug_log("Status after play: %s\n", status_buffer);
    }
    debug_log("=== playSoundMci end ===\n\n");
}

// 사운드 완료 체크 함수 추가
int isSoundFinished(const char* alias) {
    char command[256];
    char status[256] = {0};
    
    sprintf(command, "status %s mode", alias);
    MCIERROR error = mciSendString(command, status, sizeof(status), NULL);
    
    // 오류가 있거나 stopped 상태면 끝난 것
    return (error != 0 || strcmp(status, "stopped") == 0);
}

void stopSoundMci(const char* alias) {
    char command[256];
    sprintf(command, "stop %s", alias);
    mciSendString(command, NULL, 0, NULL);
    sprintf(command, "close %s", alias);
    mciSendString(command, NULL, 0, NULL);
}

void stopAllGameSounds() {
    stopSoundMci("siren");
    stopSoundMci("loop_sfx");
    stopSoundMci("power_up");
    is_bgm_playing = 0;
}

// 사운드 처리 함수
void handleSound() {
    switch(current_state) {
        case STATE_TITLE:
            // 타이틀 화면 BGM (필요시)
            break;

        case STATE_READY:
            // 첫 시작일 때만 게임 시작 사운드 재생
            if(isFirstStart() && is_bgm_playing == 0){
                playSoundMci("sounds/game_start.wav", "start_game", 0);
                is_bgm_playing = 1;
            }
            break;
            
        case STATE_PLAYING:
            // In the handleSound() function, replace the selection with:
            if(!isPowerModeActive() && current_siren_level == 1 && getCookiesEaten() > (getTotalCookies() * SIREN_LEVEL_CHANGE_THRESHOLD / 100)){
                current_siren_level = 2;
                stopSoundMci("siren");
                is_bgm_playing = 0;
            }
            // siren 사운드 시작
            if(is_bgm_playing == 0){
                if(current_siren_level == 1){
                    playSoundMci("sounds/ghost_siren_lv1.mp3", "siren", 1);
                } else if(current_siren_level >= 2){
                    playSoundMci("sounds/ghost_siren_lv2.mp3", "siren", 1);
                }
                is_bgm_playing = 1;
            }
            break;

        case STATE_PACMAN_DEATH:
            // 사운드 모두 정지
            if(is_bgm_playing){
                stopAllGameSounds();
            }
            // 팩맨 죽는 사운드 재생
            playSoundAndWait("sounds/pacman_dying.mp3", "pacman_dying");
            break;

        case STATE_LEVEL_COMPLETE:
            // 레벨 완료 사운드
            if(is_bgm_playing){
                stopAllGameSounds();
            }
            break;
            
        case STATE_GAME_OVER:
            // 게임 오버 사운드 (필요시)
            break;

        case STATE_ALL_CLEAR:
            // 올클리어 사운드
            if(is_bgm_playing){
                stopAllGameSounds();
            }
            // 올클리어 축하 사운드 재생
            playSoundAndWait("sounds/pacman_gets_high_score.mp3", "all_clear");
            break;
    }
}

// 배경 음악 및 효과음 업데이트 함수
void updateBackGroundMusic() {
    if (isPowerModeActive() && getPowerModeTimer() > 0) {
        if (power_music_active) {
            char command[256];
            char position[256] = {0};
            char length[256] = {0};
            
            // 현재 위치와 길이 확인
            sprintf(command, "status power_up position");
            mciSendString(command, position, sizeof(position), NULL);
            
            sprintf(command, "status power_up length");
            mciSendString(command, length, sizeof(length), NULL);
            
            int pos = atoi(position);
            int len = atoi(length);
            
            // 90% 지점에서 다시 처음부터 재생
            if (len > 0 && pos >= (len * 0.9)) {
                sprintf(command, "seek power_up to start");
                mciSendString(command, NULL, 0, NULL);
                
                sprintf(command, "play power_up");
                mciSendString(command, NULL, 0, NULL);
                
                debug_log("Power sound looped at 90%% position\n");
            }
        }
    } else if (power_music_active) {
        // 파워모드가 끝났으면 파워 음악 정지
        stopSoundMci("power_up");
        power_music_active = 0;
        debug_log("Power mode ended, music stopped\n");
        
        is_bgm_playing = 0;
    }

    if (isGhostBackActive()) {
        int has_returning_ghost = 0;
        for(int i = 0; i < MAX_GHOSTS; i++){
            if(ghosts[i].state == RETURNING){
                has_returning_ghost = 1;
                break;
            }
        }
        if (has_returning_ghost) {
            // 고스트가 돌아가고 있는 중
            if(isSoundFinished("loop_sfx")){
                playSoundMci("sounds/ghost_back_to_base.wav", "loop_sfx", 1);
                debug_log("Ghost back sound restarted\n");
            }
        } else {
            // 돌아가는 고스트가 없으면 정지
            stopSoundMci("loop_sfx");
            setGhostBackActive(0);
            debug_log("All ghosts returned, back sound stopped\n");

            // 파워 모드가 아니면 siren 재시작
            if(!power_music_active) {
                is_bgm_playing = 0; // siren 재생 플래그 초기화
            }
        }
    }
}

void setBgmPlaying(int playing) {
    is_bgm_playing = playing;
}

int isBgmPlaying() {
    return is_bgm_playing;
}

void setCurrentSirenLevel(int level) {
    current_siren_level = level;
}

int getCurrentSirenLevel() {
    return current_siren_level;
}

void setPowerMusicActive(int active) {
    power_music_active = active;
}
int isPowerMusicActive() {
    return power_music_active;
}