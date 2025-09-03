#include "common.h"
#include "game.h"

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