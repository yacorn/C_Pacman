// main.c
#include "main.h"

int main() {
    initialize();
    
    Pacman pacman;

    while(1){
        // 1. Delta time 업데이트
        updateDeltaTime();
        double dt = getDeltaTime();
        
        // 2. 누적 시간에 delta time 추가
        static double accumulator = 0.0;
        accumulator += dt;
        
        // 3. 고정 시간 스텝으로 게임 로직 업데이트
        const double FIXED_TIMESTEP = 0.25; // 250ms
        while (accumulator >= FIXED_TIMESTEP) {
            // 입력 처리 (고정 스텝)
            handleInput(&pacman);

            // 사운드 처리 (고정 스텝)
            handleSound();
            
            // 게임 로직 업데이트 (고정 스텝) - handleLogic 직접 호출
            handleLogic(&pacman);

            accumulator -= FIXED_TIMESTEP;
        }
        
        // 4. 실시간 타이머 업데이트 (Delta Time 의존적)
        if(!isGamePaused()){
            updateTimersRealTime(dt);
        }
        
        // 6. 렌더링 (매 프레임)
        handleRender(&pacman);
        
        // 7. CPU 사용률을 위한 최소 대기
        Sleep(1); // 1ms만 대기하여 CPU 점유율 조절
    }

    releaseScreen();
    return 0;
}