// pacman.c
#include "game.h"
#include "maps.h"
#include "sound.h"

static int handleItemConsumption(Pacman* pacman, int next_x, int next_y){
    if(current_map[next_y][next_x] == COOKIE){
        current_map[next_y][next_x] = EMPTY;
        eatCookie();
        addScore(SCORE_COOKIE); // 점수 증가

        return 1;
    } else if(current_map[next_y][next_x] == POWER_COOKIE){
        current_map[next_y][next_x] = EMPTY;
        eatCookie();
        addScore(SCORE_POWER_COOKIE); // 점수 증가
        activatePowerMode();

        return 1;
    } else if(bonus_fruit.active && next_x == bonus_fruit.x && next_y == bonus_fruit.y){
        addScore(bonus_fruit.score);
        bonus_fruit.active = 0;
        PlaySound(TEXT("sounds/eating_fruit.wav"), NULL, SND_FILENAME | SND_ASYNC);

        return 1;
    }
    return 0;
}

void updatePacman(Pacman* pacman){

    static int waka_sound_toggle = 0;

    int next_x = pacman->x;
    int next_y = pacman->y;

    pacman->prev_x = pacman->x;
    pacman->prev_y = pacman->y;

    // 현재 방향에 따라 다음 위치 계산
    getNextPosition(&next_x, &next_y, pacman->direction);

    // 현재 좌표와 다음 좌표가 같으면 방향없이 멈춘상태
    if(next_x == pacman->x && next_y == pacman->y){
        // 이동하지 않음
        return;
    }

    int ate_something = 0;
    int moved = 0;

    if(current_map[next_y][next_x] != WALL){
        // 워프존 확인
        if(current_map[next_y][next_x] == WARP_ZONE){
            if(next_x == 0){ // 왼쪽 워프존
                pacman->x = MAP_WIDTH - 1;
                moved = 1;
            } else if(next_x == MAP_WIDTH - 1){ // 오른쪽 워프존
                pacman->x = 1;
                moved = 1;
            }
        // 아이템 처리
        } else if(handleItemConsumption(pacman, next_x, next_y)){
            pacman->x = next_x;
            pacman->y = next_y;
            moved = 1;

        } else if(current_map[next_y][next_x] != GHOST_DOOR && current_map[next_y][next_x] != GHOST_ZONE && current_map[next_y][next_x] != WALL){
            // 일반 이동
            pacman->x = next_x;
            pacman->y = next_y;
            moved = 1;
        }

        if(moved){
            if(waka_sound_toggle == 0){
                    PlaySound(TEXT("sounds/wakawaka1.wav"), NULL, SND_FILENAME | SND_ASYNC);
                    waka_sound_toggle = 1;
                } else {
                    PlaySound(TEXT("sounds/wakawaka2.wav"), NULL, SND_FILENAME | SND_ASYNC);
                    waka_sound_toggle = 0;
            }
        }

        // if(ate_something){
        //     if(!power_mode && current_siren_level == 1 && cookies_eaten * 2 >= total_cookies ){
        //         current_siren_level = 2;
        //         stopSoundMci("siren");
        //         playSoundMci("sounds/ghost_siren_lv2.wav", "siren", 1);
        //     }
        // }
    }

    
}

void handlePlayerInput(Pacman* pacman){
    if(GetAsyncKeyState(VK_SPACE) & 0x0001){
        debug_mode = !debug_mode;
    }

    Direction desired_direction = DIR_NONE;

    if(GetAsyncKeyState(VK_UP) & 0x0001){
        desired_direction = DIR_UP;
    } else if(GetAsyncKeyState(VK_DOWN) & 0x0001) {
        desired_direction = DIR_DOWN;
    } else if(GetAsyncKeyState(VK_LEFT) & 0x0001) {
        desired_direction = DIR_LEFT;
    } else if(GetAsyncKeyState(VK_RIGHT) & 0x0001) {
        desired_direction = DIR_RIGHT;
    } else if(GetAsyncKeyState('P') & 0x0001){
        if(getCurrentGameState() == STATE_PLAYING){
            setGamePaused(1);
            stopAllGameSounds();
            setGameState(STATE_GAME_PAUSE);
        }
    }

    if(desired_direction != DIR_NONE){
        int next_x = pacman->x;
        int next_y = pacman->y;

        getNextPosition(&next_x, &next_y, desired_direction);

        if(next_x >= 0 && next_x < MAP_WIDTH && next_y >= 0 && next_y < MAP_HEIGHT
            && current_map[next_y][next_x] != WALL){
                pacman->direction = desired_direction;
        }
    }


    // // 방향키 입력 처리 (계속 누르고 있으면 계속 이동?)
    // if(GetAsyncKeyState(VK_UP) & 0x0001){
    //     if(pacman->y - 1 >= 0 && current_map[pacman->y - 1][pacman->x] != WALL){
    //         pacman->direction = DIR_UP;
    //     }
    // }
    // else if(GetAsyncKeyState(VK_DOWN) & 0x0001) {
    //     if(pacman->y + 1 < MAP_HEIGHT && current_map[pacman->y + 1][pacman->x] != WALL) {
    //         pacman->direction = DIR_DOWN;
    //     }
    // }
    // else if(GetAsyncKeyState(VK_LEFT) & 0x0001) {
    //     if(pacman->x - 1 >= 0 && current_map[pacman->y][pacman->x - 1] != WALL) {
    //         pacman->direction = DIR_LEFT;
    //     }
    // }
    // else if(GetAsyncKeyState(VK_RIGHT) & 0x0001) {
    //     if(pacman->x + 1 < MAP_WIDTH && current_map[pacman->y][pacman->x + 1] != WALL) {
    //         pacman->direction = DIR_RIGHT;
    //     }
    // }

    // if(_kbhit()){
    //     int key = _getch();

    //     if (key == 32){
    //         debug_mode = !debug_mode;
    //         return;
    //     }

    //     if (key == 224){
    //         key = _getch();
    //         switch(key){
    //             case 72: {
    //                 // 위 방향으로 변경
    //                 if(pacman->y - 1 < 0 || current_map[pacman->y - 1][pacman->x] == WALL){
    //                     return;
    //                 }
    //                 pacman->direction = DIR_UP; 
    //                 break;
    //             }
    //             case 80: {
    //                 if(pacman->y + 1 >= MAP_HEIGHT || current_map[pacman->y + 1][pacman->x] == WALL){
    //                     return;
    //                 }
    //                 pacman->direction = DIR_DOWN;
    //                 break;
    //             }
    //             case 75: {
    //                 if(pacman->x - 1 < 0 || current_map[pacman->y][pacman->x - 1] == WALL){
    //                     return;
    //                 }
    //                 pacman->direction = DIR_LEFT;
    //                 break;
    //             }
    //             case 77: {
    //                 if(current_map[pacman->y][pacman->x + 1] == WALL){
    //                     return;
    //                 }
    //                 pacman->direction = DIR_RIGHT;
    //                 break;
    //             }
    //         }
    //     }
    // }

}

void activatePowerMode() {
    debug_log("=== activatePowerMode called ===\n");
    
    setPowerMode(1);
    setPowerModeTimer(POWER_MODE_DURATION);

    playPowerModeSound();

    for(int i = 0; i < MAX_GHOSTS; i++){
        if(ghosts[i].state == CHASING){
            ghosts[i].state = FRIGHTENED;

            switch(ghosts[i].direction) {
                case DIR_UP: ghosts[i].direction = DIR_DOWN; break;
                case DIR_DOWN: ghosts[i].direction = DIR_UP; break;
                case DIR_LEFT: ghosts[i].direction = DIR_RIGHT; break;
                case DIR_RIGHT: ghosts[i].direction = DIR_LEFT; break;
                default: break;
            }
        }
    }
    
    debug_log("=== activatePowerMode end ===\n");
}

void handlePacmanDeath(Pacman* pacman){
    pacman->lives--;
    setGameState(STATE_PACMAN_DEATH);

    // 팩맨 방향 초기화
    pacman->direction = DIR_NONE;

    setPowerMode(0);
    setPowerModeTimer(0);

    // 모든 유령들 대기 상태로 초기화
    for(int i = 0; i < MAX_GHOSTS; i++){
        if(ghosts[i].state == FRIGHTENED){
            ghosts[i].state = CHASING;
        }
    }
}

void initializePacman(Pacman* pacman, int lives){
    pacman->x = PACMAN_SPAWN_X;
    pacman->y = PACMAN_SPAWN_Y;
    pacman->prev_x = PACMAN_SPAWN_X;
    pacman->prev_y = PACMAN_SPAWN_Y;
    pacman->direction = DIR_NONE;
    pacman->lives = lives;
}


