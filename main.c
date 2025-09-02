// main.c
#include "packman.h"

int main() {
    srand((unsigned int)time(NULL));
    initialize();
    
    Pacman pacman;
    current_state = STATE_TITLE;

    while(1){
        // 1. 입력 처리
        handleInput(&pacman);
        
        // 2. 렌더링
        handleRender(&pacman);
        
        // 3. 사운드 처리
        handleSound();
        
        // 4. 대기
        Sleep(getDelayTime());
    }

    releaseScreen();
    return 0;
}