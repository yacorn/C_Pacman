// pacman.c
#include "packman.h"

void updatePacman(Pacman* pacman){

    static int waka_sound_toggle = 0;

    int next_x = pacman->x;
    int next_y = pacman->y;

    pacman->prev_x = pacman->x;
    pacman->prev_y = pacman->y;

    // 현재 방향에 따라 다음 위치 계산
    switch(pacman->direction){
        case DIR_UP: next_y--; break;
        case DIR_DOWN: next_y++; break;
        case DIR_LEFT: next_x--; break;
        case DIR_RIGHT: next_x++; break;
        default: break;
    }

    int ate_something = 0;
    int moved = 0;

    if(bonus_fruit.active && pacman->x == bonus_fruit.x && pacman->y == bonus_fruit.y){
        ate_something = 1;
        score += bonus_fruit.score;
        bonus_fruit.active = 0;
        PlaySound(TEXT("sounds/eating_fruit.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }

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
        } else if(current_map[next_y][next_x] == COOKIE){
            current_map[next_y][next_x] = EMPTY;
            cookies_eaten++;
            score += 10; // 점수 증가

            ate_something = 1;
            pacman->x = next_x;
            pacman->y = next_y;
            moved = 1;

        } else if(current_map[next_y][next_x] == POWER_COOKIE){
            current_map[next_y][next_x] = EMPTY;
            cookies_eaten++;
            score += 50; // 점수 증가

            ate_something = 1;
            activatePowerMode();
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

        if(ate_something){
            if(!power_mode && current_siren_level == 1 && cookies_eaten * 2 >= total_cookies ){
                current_siren_level = 2;
                stopSoundMci("siren");
                playSoundMci("sounds/ghost_siren_lv2.wav", "siren", 1);
            }
        }
    }

    
}

void processInput(Pacman* pacman){
    if(GetAsyncKeyState(VK_SPACE) & 0x0001){
        debug_mode = !debug_mode;
    }

    // 방향키 입력 처리 (계속 누르고 있으면 계속 이동?)
    if(GetAsyncKeyState(VK_UP) & 0x0001){
        if(pacman->y - 1 >= 0 && current_map[pacman->y - 1][pacman->x] != WALL){
            pacman->direction = DIR_UP;
        }
    }
    else if(GetAsyncKeyState(VK_DOWN) & 0x0001) {
        if(pacman->y + 1 < MAP_HEIGHT && current_map[pacman->y + 1][pacman->x] != WALL) {
            pacman->direction = DIR_DOWN;
        }
    }
    else if(GetAsyncKeyState(VK_LEFT) & 0x0001) {
        if(pacman->x - 1 >= 0 && current_map[pacman->y][pacman->x - 1] != WALL) {
            pacman->direction = DIR_LEFT;
        }
    }
    else if(GetAsyncKeyState(VK_RIGHT) & 0x0001) {
        if(pacman->x + 1 < MAP_WIDTH && current_map[pacman->y][pacman->x + 1] != WALL) {
            pacman->direction = DIR_RIGHT;
        }
    }

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
    
    power_mode = 1;
    power_mode_timer = POWER_MODE_DURATION;

    debug_log("Stopping siren...\n");
    stopSoundMci("siren");

    debug_log("Playing power_up.wav (single play)...\n");
    playSoundMci("sounds/power_up.wav", "power_up", 0); // loop = 0으로 변경
    power_music_active = 1; // 플래그 설정

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