#include "packman.h"

// 디버그 로깅 함수
void debug_log(const char* format, ...) {
    if (!debug_mode) return;
    
    FILE* debug_file = fopen("debug_log.txt", "a");
    if (debug_file) {
        // 시간 정보 추가
        time_t now = time(NULL);
        char* time_str = ctime(&now);
        time_str[strlen(time_str)-1] = '\0'; // 개행 제거
        
        fprintf(debug_file, "[%s] ", time_str);
        
        va_list args;
        va_start(args, format);
        vfprintf(debug_file, format, args);
        va_end(args);
        
        fclose(debug_file);
    }
}

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