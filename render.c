// render.c
#include "packman.h"

void hideCursor() {
    CONSOLE_CURSOR_INFO cursor_info;

    cursor_info.dwSize = 1; // 커서 크기 설정
    cursor_info.bVisible = FALSE; // 커서 숨기기

    SetConsoleCursorInfo(screen[0], &cursor_info);
    SetConsoleCursorInfo(screen[1], &cursor_info);
}


void clear() {
    COORD coord = {0, 0};
    SetConsoleCursorPosition(screen[screen_index], coord);

    // 실제 사용하는 영역만 지우기
    // int buffer_size = (MAP_WIDTH * 2 + 20) * (MAP_HEIGHT * 2);

    DWORD written;
    // FillConsoleOutputCharacter(screen[screen_index], ' ', buffer_size, coord, &written);
    WriteConsoleA(screen[screen_index], ANSI_CLEAR ANSI_HOME, strlen(ANSI_CLEAR ANSI_HOME), &written, NULL);
}


void flip() {
    SetConsoleActiveScreenBuffer(screen[screen_index]);
    screen_index = !screen_index;
}


void releaseScreen() {
    stopSoundMci("siren");
    stopSoundMci("loop_sfx");
    printf(ANSI_SHOW_CURSOR);
    printf(ANSI_RESET);
    printf(ANSI_CLEAR);
    CloseHandle(screen[0]);
    CloseHandle(screen[1]);
}


void drawEntity(int x, int y, const char* str, const char* color){
    COORD pos = {x * 2, y};
    SetConsoleCursorPosition(screen[screen_index], pos);

    char buffer[512];
    sprintf(buffer, "%s%s" ANSI_RESET, color, str);
    DWORD written;
    // WriteConsoleOutputCharacterA(screen[screen_index], str, strlen(str), pos, &dword);
    // FillConsoleOutputAttribute(screen[screen_index], color, strlen(str), pos, &dword);
    WriteConsoleA(screen[screen_index], buffer, strlen(buffer), &written, NULL);
}


void drawMapInfo() {
    // 맵 관련 정보
    int score_x = MAP_WIDTH * 2 + 2;
    int map_x = score_x + 20;
    int base_y = 23;  // 게임 상태 정보 아래
    char buffer[60];
    
    // 맵 정보 헤더
    sprintf(buffer, "MAP INFO:");
    drawEntity(map_x / 2, base_y, buffer, ANSI_WHITE ANSI_BOLD);
    
    // 맵 크기
    sprintf(buffer, "SIZE: %dx%d", MAP_WIDTH, MAP_HEIGHT);
    drawEntity(map_x / 2, base_y + 1, buffer, INFO_COLOR);
    
    // 남은 쿠키 수 계산 (선택사항)
    
    int remaining_power = 0;
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            // if(map[y][x] == COOKIE) remaining_cookies++;
            if(current_map[y][x] == POWER_COOKIE) remaining_power++;
        }
    }
    
    // ✅ 총 쿠키 개수와 먹은 개수 표시 (더 효율적)
    sprintf(buffer, "COOKIES: %d/%d", total_cookies - cookies_eaten, total_cookies);
    drawEntity(map_x / 2, base_y + 2, buffer, COOKIE_COLOR);
    // sprintf(buffer, "COOKIES: %d", remaining_cookies);
    // drawEntity(map_x / 2, base_y + 2, buffer, COOKIE_COLOR);
    
    sprintf(buffer, "POWER: %d", remaining_power);
    drawEntity(map_x / 2, base_y + 3, buffer, POWER_COOKIE_COLOR);

    // 현재 스테이지
    sprintf(buffer, "STAGE: %d", current_stage);
    drawEntity(map_x / 2, base_y + 4, buffer, ANSI_BRIGHT_BLACK);
}


void drawGameStateInfo() {
    // 게임 상태 정보를 팩맨 정보 아래에 표시
    int score_x = MAP_WIDTH * 2 + 2;
    int state_x = score_x + 20;
    int base_y = 17;  // 팩맨 정보 아래
    char buffer[60];
    
    // 게임 상태 헤더
    sprintf(buffer, "GAME STATE:");
    drawEntity(state_x / 2, base_y, buffer, ANSI_WHITE ANSI_BOLD);
    
    // 큐 정보
    sprintf(buffer, "QUEUE: %d/%d", queue_count, MAX_GHOSTS);
    drawEntity(state_x / 2, base_y + 1, buffer, ANSI_CYAN);
    
    // 파워 모드 정보
    if(power_mode) {
        sprintf(buffer, "POWER: %d", power_mode_timer);
        drawEntity(state_x / 2, base_y + 2, buffer, ANSI_RED ANSI_BLINK);
    } else {
        sprintf(buffer, "POWER: OFF");
        drawEntity(state_x / 2, base_y + 2, buffer, ANSI_BRIGHT_BLACK);
    }
    
    // 게임 상태
    if(current_state == STATE_LEVEL_COMPLETE) {
        sprintf(buffer, "STATUS: COMPLETE!");
        drawEntity(state_x / 2, base_y + 3, buffer, ANSI_GREEN ANSI_BLINK);
    } else if(current_state == STATE_GAME_OVER) {
        sprintf(buffer, "STATUS: GAME OVER");
        drawEntity(state_x / 2, base_y + 3, buffer, ANSI_RED ANSI_BLINK);
    } else {
        sprintf(buffer, "STATUS: PLAYING");
        drawEntity(state_x / 2, base_y + 3, buffer, ANSI_GREEN);
    }
    // 릴리즈된 고스트 수
    sprintf(buffer, "RELEASED: %d/%d", ghost_released, MAX_GHOSTS);
    drawEntity(state_x / 2, base_y + 4, buffer, ANSI_YELLOW);
}


void drawPacmanDebugInfo(const Pacman* pacman) {
    // 팩맨 디버그 정보를 고스트 정보 오른쪽에 표시
    int score_x = MAP_WIDTH * 2 + 2;
    int pacman_x = score_x + 20;  // 고스트 정보 오른쪽으로 20칸 이동
    int base_y = 9;
    char buffer[60];
    
    // 팩맨 헤더
    sprintf(buffer, "PACMAN INFO:");
    drawEntity(pacman_x / 2, base_y, buffer, PACMAN_COLOR);
    
    // 위치 정보
    sprintf(buffer, "POS: (%d,%d)", pacman->x, pacman->y);
    drawEntity(pacman_x / 2, base_y + 1, buffer, INFO_COLOR);
    
    // 방향 정보
    const char* direction_names[] = {"NONE", "UP", "DOWN", "LEFT", "RIGHT"};
    int dir_index = (pacman->direction >= 0 && pacman->direction <= 4) ? pacman->direction : 0;
    sprintf(buffer, "DIR: %s", direction_names[dir_index]);
    drawEntity(pacman_x / 2, base_y + 2, buffer, SUCCESS_COLOR);
    
    // 생명 정보
    sprintf(buffer, "LIVES: %d", pacman->lives);
    drawEntity(pacman_x / 2, base_y + 3, buffer, ANSI_RED ANSI_BOLD);
    
    // 현재 타일 정보
    const char* tile_names[] = {"EMPTY", "WALL", "COOKIE", "POWER", "DOOR", "ZONE", "WARP"};
    int tile_type = (current_map[pacman->y][pacman->x] >= 0 && current_map[pacman->y][pacman->x] <= 6) ? 
                    current_map[pacman->y][pacman->x] : 0;
    sprintf(buffer, "TILE: %s", tile_names[tile_type]);
    drawEntity(pacman_x / 2, base_y + 4, buffer, ANSI_YELLOW);
    
    // 다음 위치 정보 (방향키 눌렸을 때의 이동 예상 위치)
    int next_x = pacman->x, next_y = pacman->y;
    switch(pacman->direction) {
        case DIR_UP: next_y--; break;
        case DIR_DOWN: next_y++; break;
        case DIR_LEFT: next_x--; break;
        case DIR_RIGHT: next_x++; break;
    }
    
    if(next_x >= 0 && next_x < MAP_WIDTH && next_y >= 0 && next_y < MAP_HEIGHT) {
        sprintf(buffer, "NEXT: (%d,%d)", next_x, next_y);
        drawEntity(pacman_x / 2, base_y + 5, buffer, ANSI_CYAN);
        
        // 다음 타일 타입
        int next_tile = current_map[next_y][next_x];
        const char* next_tile_name = (next_tile >= 0 && next_tile <= 6) ? 
                                   tile_names[next_tile] : "INVALID";
        sprintf(buffer, "NEXT TILE: %s", next_tile_name);
        drawEntity(pacman_x / 2, base_y + 6, buffer, ANSI_MAGENTA);
    } else {
        sprintf(buffer, "NEXT: OUT");
        drawEntity(pacman_x / 2, base_y + 5, buffer, ANSI_RED);
        sprintf(buffer, "NEXT TILE: WALL");
        drawEntity(pacman_x / 2, base_y + 6, buffer, ANSI_RED);
    }
    
    // 가장 가까운 고스트와 충돌 가능성 표시
    int min_distance = INT_MAX;
    int closest_ghost = -1;
    
    for(int i = 0; i < MAX_GHOSTS; i++) {
        if(ghosts[i].state != WAITING) {
            int distance = abs(pacman->x - ghosts[i].x) + abs(pacman->y - ghosts[i].y);
            if(distance < min_distance) {
                min_distance = distance;
                closest_ghost = i;
            }
        }
    }
    
    if(closest_ghost != -1) {
        const char* ghost_names[] = {"RED", "PINK", "GREEN", "ORANGE"};
        sprintf(buffer, "CLOSEST: %s (%d)", ghost_names[closest_ghost], min_distance);
        
        // 충돌 위험도에 따른 색상
        const char* color = ANSI_WHITE;
        if(min_distance == 0) {
            // 같은 위치 = 충돌!
            sprintf(buffer, "COLLISION! %s", ghost_names[closest_ghost]);
            if(ghosts[closest_ghost].state == FRIGHTENED) {
                color = ANSI_BLUE ANSI_BOLD;  // 고스트 먹을 수 있음
            } else if(ghosts[closest_ghost].state == CHASING) {
                color = ANSI_RED ANSI_BLINK;  // 위험!
            }
        } else if(min_distance <= 2) {
            color = ANSI_RED ANSI_BOLD;  // 위험
        } else if(min_distance <= 5) {
            color = ANSI_YELLOW;  // 주의
        }
        
        drawEntity(pacman_x / 2, base_y + 7, buffer, color);
    }
}


void drawGhostDebugInfo(){
    int score_x = MAP_WIDTH * 2 + 2;
    const char* ghost_names[] = {"RED", "PINK", "GREEN", "ORANGE"};
    const char* ghost_colors[] = {GHOST_RED_COLOR, GHOST_PINK_COLOR, GHOST_GREEN_COLOR, GHOST_ORANGE_COLOR};
    const char* state_names[] = {"CHASING", "FRIGHTENED", "EXITING", "WAITING", "RETURNING", "UNKNOWN"};
    
    for(int i = 0; i < MAX_GHOSTS; i++){
        int base_y = 9 + (i * 6);  // 각 고스트마다 6줄씩 사용
        char buffer[60];
        
        // 고스트 이름 헤더
        sprintf(buffer, "%s GHOST:", ghost_names[i]);
        drawEntity(score_x / 2, base_y, buffer, ghost_colors[i]);
        
        // 위치 정보
        sprintf(buffer, "POS: (%d,%d)", ghosts[i].x, ghosts[i].y);
        drawEntity(score_x / 2, base_y + 1, buffer, INFO_COLOR);
        
        // 상태 정보 (state 값 범위 체크)
        int state_index = (ghosts[i].state >= 0 && ghosts[i].state < 5) ? ghosts[i].state : 5;
        sprintf(buffer, "STATE: %s", state_names[state_index]);
        drawEntity(score_x / 2, base_y + 2, buffer, SUCCESS_COLOR);
        
        // 방향 정보
        const char* direction_names[] = {"NONE", "UP", "DOWN", "LEFT", "RIGHT"};
        int dir_index = (ghosts[i].direction >= 0 && ghosts[i].direction <= 4) ? ghosts[i].direction : 0;
        sprintf(buffer, "DIR: %s", direction_names[dir_index]);
        drawEntity(score_x / 2, base_y + 3, buffer, INFO_COLOR);
        
        // 목표 좌표 (CHASING이나 EXITING 상태일 때만)
        if((ghosts[i].state == CHASING || ghosts[i].state == EXITING || ghosts[i].state == RETURNING) &&
           ghosts[i].target_x != -1 && ghosts[i].target_y != -1) {
            sprintf(buffer, "TARGET: (%d,%d)", ghosts[i].target_x, ghosts[i].target_y);
            drawEntity(score_x / 2, base_y + 4, buffer, ANSI_YELLOW);
        } else {
            sprintf(buffer, "TARGET: NONE");
            drawEntity(score_x / 2, base_y + 4, buffer, ANSI_BRIGHT_BLACK);
        }
        
        // PATH INDEX (EXITING이나 RETURNING 상태일 때만)
        if(ghosts[i].state == EXITING || ghosts[i].state == RETURNING) {
            sprintf(buffer, "PATH: %d/%d", ghosts[i].path_index, exit_path_length);
            drawEntity(score_x / 2, base_y + 5, buffer, ANSI_CYAN);
        } else {
            sprintf(buffer, "PATH: -");
            drawEntity(score_x / 2, base_y + 5, buffer, ANSI_BRIGHT_BLACK);
        }
    }
    
    // 큐와 파워 정보는 제거 (drawGameStateInfo로 이동)
}


void drawScore(int score, int lives){
    int score_x = MAP_WIDTH * 2 + 2;

    drawEntity(score_x / 2, 2, "SCORE", INFO_COLOR);
    
    char score_str[20];
    sprintf(score_str, "%d", score);
    drawEntity(score_x / 2, 3, score_str, INFO_COLOR);
    
    // lives
    drawEntity(score_x / 2, 5, "LIVES", INFO_COLOR);

    // lives 값
    char lives_str[20];
    sprintf(lives_str, "%d", lives);
    drawEntity(score_x / 2, 6, lives_str, INFO_COLOR);

}


void renderDebugInfo(const Pacman* pacman) {
    // 디버그 정보 렌더링
    int score_x = MAP_WIDTH * 2 + 2;
    int debug_start_y = 9 + (MAX_GHOSTS * 6) + 4;
    drawEntity(score_x / 2, debug_start_y, "DEBUG MODE", DEBUG_COLOR);
    drawEntity(score_x / 2, debug_start_y + 1, "SPACE: Toggle", INFO_COLOR);

    // 고스트 디버그 정보 (왼쪽)
    drawGhostDebugInfo();

    // 팩맨 디버그 정보 (오른쪽)
    drawPacmanDebugInfo(pacman);

    // 게임 상태 정보 (오른쪽 중간)
    drawGameStateInfo();

    // 맵 정보(오른쪽 아래)
    drawMapInfo();
}


void renderMapTile(int tile, int x, int y) {
    switch(tile){
        case EMPTY: drawEntity(x, y, " ", ANSI_RESET); break;
        case WALL: drawEntity(x, y, "#", WALL_COLOR); break;
        case COOKIE: drawEntity(x, y, "·", COOKIE_COLOR); break;
        case POWER_COOKIE: drawEntity(x, y, "●", POWER_COOKIE_COLOR); break;
        case GHOST_DOOR: drawEntity(x, y, "─", ANSI_WHITE); break;
        case GHOST_ZONE: drawEntity(x, y, " ", GHOST_ZONE_COLOR); break;
        case WARP_ZONE: drawEntity(x, y, "W", SUCCESS_COLOR); break;
    }
}


void renderTarget(char ghost_color, int x, int y) {
    // 목표 위치 표시
    switch(ghost_color){
        case 'R': drawEntity(x, y, "T", GHOST_RED_COLOR ANSI_BLINK); break;
        case 'P': drawEntity(x, y, "T", GHOST_PINK_COLOR ANSI_BLINK); break;
        case 'G': drawEntity(x, y, "T", GHOST_GREEN_COLOR ANSI_BLINK); break;
        case 'O': drawEntity(x, y, "T", GHOST_ORANGE_COLOR ANSI_BLINK); break;
    }
}


void renderGhost(const Ghost* ghost, int x, int y) {
    char ghost_str[2] = {ghost->color, '\0'};
    
    if(ghost->state == FRIGHTENED){
        if(power_mode_timer > 20){
            drawEntity(x, y, ghost_str, ANSI_BLUE ANSI_BOLD); // 파란색
        } else {
            drawEntity(x, y, ghost_str, ANSI_WHITE ANSI_BLINK); // 흰색 깜빡임
        }
    } else if(ghost->state == RETURNING){
        drawEntity(x, y, "◇", ANSI_WHITE);  // 눈만 남은 상태
    } else {
		const char* colors[] = { GHOST_RED_COLOR, GHOST_PINK_COLOR, GHOST_GREEN_COLOR, GHOST_ORANGE_COLOR };
        int color_index = (ghost->color == 'R') ? 0 : (ghost->color == 'P') ? 1 : (ghost->color == 'G') ? 2 : 3;
        drawEntity(x, y, ghost_str, colors[color_index]);
    }
            
}


void renderGameplayScreen(const Pacman* pacman, int score){
    for (int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            int is_ghost_here = 0;
            int is_target_here = 0;
            char target_ghost_color = ' ';
            Ghost* current_ghost = NULL;

            // 고스트 위치 체크
            for(int i = 0; i < MAX_GHOSTS; i++){
                if(ghosts[i].x == x && ghosts[i].y == y){
                    is_ghost_here = 1;
                    current_ghost = &ghosts[i];
                }
            }

            // 디버그 모드에서 타겟 표시
            if(debug_mode && !is_ghost_here){
                for(int i = 0; i < MAX_GHOSTS; i++){
                    if((ghosts[i].state == CHASING || ghosts[i].state == EXITING) &&
                        ghosts[i].target_x == x && ghosts[i].target_y == y &&
                        ghosts[i].target_x != -1 && ghosts[i].target_y != -1){
                        is_target_here = 1;
                        target_ghost_color = ghosts[i].color;
                        break;
                    }
                }
            }

            if(x == pacman->x && y == pacman->y){
                if(current_state == STATE_LEVEL_COMPLETE){
                    drawEntity(x, y, "C", PACMAN_COLOR ANSI_BLINK);
                } else {
                    drawEntity(x, y, "C", PACMAN_COLOR);
                }
            } else if(bonus_fruit.active && x == bonus_fruit.x && y == bonus_fruit.y){
                drawEntity(x, y, "@", FRUIT_COLOR);
            } else if(is_ghost_here){
                renderGhost(current_ghost, x, y);
            } else if(is_target_here){
                renderTarget(target_ghost_color, x, y);
            } else {
                renderMapTile(current_map[y][x], x, y);
            }
            
        }
    }

    drawScore(score, pacman->lives);
}


void renderGameOver(const Pacman* pacman, int score) {
    // 현재 맵 상태 그대로 렌더링 (어둡게)
    renderGameplayScreen(pacman, score);
    
    // 게임 오버 메시지 오버레이
    int center_x = MAP_WIDTH / 2;
    int center_y = MAP_HEIGHT / 2;
    
    // ASCII 문자만 사용
    drawEntity(center_x - 6, center_y - 2, "############", ANSI_BLACK);
    drawEntity(center_x - 6, center_y - 1, "# GAME OVER #", ANSI_RED ANSI_BOLD ANSI_BLINK);
    drawEntity(center_x - 6, center_y,     "############", ANSI_BLACK);
    
    drawEntity(center_x - 5, center_y + 2, "Press R to Restart", ANSI_WHITE);
    drawEntity(center_x - 3, center_y + 3, "ESC to Quit", ANSI_WHITE);
    
    // 최종 스코어
    char final_score[50];
    sprintf(final_score, "Final Score: %d", score);
    drawEntity(center_x - 6, center_y + 5, final_score, ANSI_YELLOW ANSI_BOLD);
}


void renderGameComplete(const Pacman* pacman, int score) {
    // 현재 맵 상태 그대로 렌더링 (AI와 팩맨은 정지된 상태)
    renderGameplayScreen(pacman, score);
    
    // 게임 완료 메시지 오버레이
    int center_x = MAP_WIDTH / 2;
    int center_y = MAP_HEIGHT / 2;
    
    // ASCII 문자만 사용한 배경 박스
    drawEntity(center_x - 8, center_y - 2, "+=============================+", ANSI_WHITE ANSI_BOLD);
    drawEntity(center_x - 8, center_y - 1, "|                             |", ANSI_WHITE ANSI_BOLD);
    drawEntity(center_x - 8, center_y,     "|                             |", ANSI_WHITE ANSI_BOLD);
    drawEntity(center_x - 8, center_y + 1, "|                             |", ANSI_WHITE ANSI_BOLD);
    drawEntity(center_x - 8, center_y + 2, "+=============================+", ANSI_WHITE ANSI_BOLD);

    // ASCII 문자만 사용한 완료 메시지
    char stage_msg[50];
    sprintf(stage_msg, "*** STAGE %d COMPLETE! ***", current_stage);
    drawEntity(center_x - 8, center_y - 1, stage_msg, ANSI_GREEN ANSI_BOLD ANSI_BLINK);
    drawEntity(center_x - 6, center_y, "Press \"N\" for Next Stage", ANSI_YELLOW ANSI_BOLD);
    // drawEntity(center_x - 5, center_y, "Press R to Restart", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 3, center_y + 1, "ESC to Quit", ANSI_WHITE);
}

// ... 이 외 render.c에 속하기로 한 모든 함수의 구현 ...
// renderGameplayScreen, renderGameOver, renderDebugInfo 등