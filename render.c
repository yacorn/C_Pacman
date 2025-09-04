// render.c
#include "maps.h"
#include "game.h"
#include "render.h"
#include "sound.h"
#include "highscore.h"

static HANDLE screen[2];
static int screen_index = 0;

void initializeRenderSystem(){
    screen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    screen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    DWORD dwMode = 0;
    GetConsoleMode(screen[0], &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(screen[0], dwMode);

    GetConsoleMode(screen[1], &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(screen[1], dwMode);

    hideCursor();
}


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

// 렌더링 함수
void handleRender(Pacman* pacman) {
    clear();
    
    switch(current_state) {
        case STATE_TITLE:
            renderTitleScreen();
            break;

        case STATE_HELP:
            renderHelpScreen();
            break;

        case STATE_HIGHSCORE:
            renderHighScoreScreen();
            break;

        case STATE_READY:
            renderGameplayScreen(pacman);
            drawEntity(MAP_WIDTH/2 - 1, MAP_HEIGHT/2 + 2, "Ready!", ANSI_YELLOW);
            break;

        case STATE_PLAYING:
            renderGameplayScreen(pacman);
            break;

        case STATE_PACMAN_DEATH:
            renderGameplayScreen(pacman);
            break;
        
        case STATE_LEVEL_COMPLETE:
            renderGameComplete(pacman);
            break;

        case STATE_GAME_OVER:
            renderGameOver(pacman);
            break;

        case STATE_ALL_CLEAR:
            renderAllClear(pacman);
            break;
    }
    
    if (debug_mode) {
        renderDebugInfo(pacman);
    }

    flip();
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
    int current_stage = getCurrentStage();
    int cookies_eaten = getCookiesEaten();
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
    sprintf(buffer, "COOKIES: %d/%d", getTotalCookies() - cookies_eaten, getTotalCookies());
    drawEntity(map_x / 2, base_y + 2, buffer, COOKIE_COLOR);
    // sprintf(buffer, "COOKIES: %d", remaining_cookies);
    // drawEntity(map_x / 2, base_y + 2, buffer, COOKIE_COLOR);
    
    // 파워 쿠키 남은 개수
    sprintf(buffer, "POWER: %d", remaining_power);
    drawEntity(map_x / 2, base_y + 3, buffer, POWER_COOKIE_COLOR);

    // 현재 스테이지
    sprintf(buffer, "STAGE: %d", current_stage);
    drawEntity(map_x / 2, base_y + 4, buffer, ANSI_BRIGHT_BLACK);

    // 보너스 과일 정보
    const char* fruit_names[] = {
        "CHERRY", "STRAWBERRY", "ORANGE", "APPLE", "MELON", "NONE"
    };
    sprintf(buffer, "FRUIT: %s", bonus_fruit.active ? fruit_names[bonus_fruit.type] : "NONE");
    drawEntity(map_x / 2, base_y + 5, buffer, bonus_fruit.active ? SUCCESS_COLOR : ANSI_BRIGHT_BLACK);

    // 보너스 과일 타이머
    if(bonus_fruit.active) {
        sprintf(buffer, "FRUIT TIME: %.1f seconds", bonus_fruit.timer);
        drawEntity(map_x / 2, base_y + 6, buffer, SUCCESS_COLOR);
    } else {
        sprintf(buffer, "FRUIT TIME: N/A");
        drawEntity(map_x / 2, base_y + 6, buffer, ANSI_BRIGHT_BLACK);
    }

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
    if(isPowerModeActive()) {
        sprintf(buffer, "POWER: %.1f seconds", getPowerModeTimer());
        drawEntity(state_x / 2, base_y + 2, buffer, ANSI_RED ANSI_BLINK);
    } else {
        sprintf(buffer, "POWER: OFF");
        drawEntity(state_x / 2, base_y + 2, buffer, ANSI_BRIGHT_BLACK);
    }
    
    // 게임 상태
    switch(current_state) {
        case STATE_LEVEL_COMPLETE:
            sprintf(buffer, "STATUS: COMPLETE!");
            drawEntity(state_x / 2, base_y + 3, buffer, ANSI_GREEN ANSI_BLINK);
            break;
        case STATE_GAME_OVER:
            sprintf(buffer, "STATUS: GAME OVER");
            drawEntity(state_x / 2, base_y + 3, buffer, ANSI_RED ANSI_BLINK);
            break;
        case STATE_PACMAN_DEATH:
            sprintf(buffer, "STATUS: PACMAN DIES");
            drawEntity(state_x / 2, base_y + 3, buffer, ANSI_YELLOW ANSI_BLINK);
            break;
        case STATE_READY:
            sprintf(buffer, "STATUS: READY");
            drawEntity(state_x / 2, base_y + 3, buffer, ANSI_YELLOW);
            break;
        case STATE_PLAYING:
            sprintf(buffer, "STATUS: PLAYING");
            drawEntity(state_x / 2, base_y + 3, buffer, ANSI_GREEN);
            break;
        case STATE_TITLE:
            sprintf(buffer, "STATUS: TITLE");
            drawEntity(state_x / 2, base_y + 3, buffer, ANSI_YELLOW);
            break;
        default:
            sprintf(buffer, "STATUS: UNKNOWN");
            drawEntity(state_x / 2, base_y + 3, buffer, ANSI_YELLOW);
            break;
    }
    // 릴리즈된 고스트 수
    sprintf(buffer, "RELEASED: %d/%d", getGhostReleased(), MAX_GHOSTS);
    drawEntity(state_x / 2, base_y + 4, buffer, ANSI_YELLOW);

    // 고스트 릴리즈 타이머
    sprintf(buffer, "GHOST TIMER: %.1f sec", getGhostReleaseTimer());
    drawEntity(state_x / 2, base_y + 5, buffer, ANSI_YELLOW);
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
    const char* tile_names[] = {"EMPTY", "WALL", "COOKIE", "POWER", "DOOR", "ZONE", "WARP", "VOID"};
    int tile_type = (current_map[pacman->y][pacman->x] >= 0 && current_map[pacman->y][pacman->x] <= 7) ? 
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
        
    }
    
    // 큐와 파워 정보는 제거 (drawGameStateInfo로 이동)
}


void drawScore(int lives){
    int score = getScore();
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

    // FPS 표시 (debug 모드일 때만)
    if (debug_mode) {
        drawEntity(score_x / 2, 8, "FPS ", ANSI_WHITE ANSI_BOLD);
        
        char fps_str[20];
        sprintf(fps_str, "%d", getCurrentFPS());
        drawEntity(score_x / 2 + 2 , 8, fps_str, ANSI_WHITE ANSI_BOLD);
    }
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
        case VOID_ZONE: drawEntity(x, y, " ", ANSI_BLACK); break;
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
        if(getPowerModeTimer() > POWER_MODE_DURATION * 0.5){
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


void renderFruit(FruitTypes type, int x, int y){
    switch(type){
        case FRUIT_CHERRY: drawEntity(x, y, "@", FRUIT_CHERRY_COLOR); break;
        case FRUIT_STRAWBERRY: drawEntity(x, y, "@", FRUIT_STRAWBERRY_COLOR); break;
        case FRUIT_ORANGE: drawEntity(x, y, "@", FRUIT_ORANGE_COLOR); break;
        case FRUIT_APPLE: drawEntity(x, y, "@", FRUIT_APPLE_COLOR); break;
        case FRUIT_MELON: drawEntity(x, y, "@", FRUIT_MELON_COLOR); break;
    }
}


void renderGameplayScreen(const Pacman* pacman){
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
                renderFruit(bonus_fruit.type, x, y);
            } else if(is_ghost_here){
                renderGhost(current_ghost, x, y);
            } else if(is_target_here){
                renderTarget(target_ghost_color, x, y);
            } else {
                renderMapTile(current_map[y][x], x, y);
            }
            
        }
    }

    drawScore(pacman->lives);
}


void renderGameOver(const Pacman* pacman) {
    // 현재 맵 상태 그대로 렌더링 (어둡게)
    int current_score = getScore();
    renderGameplayScreen(pacman);

    if(isHighScore(current_score)){
        renderHighScoreAchieved(pacman, current_score);
    } else {
        renderNormalGameOver(pacman, current_score);
    }
}


void renderHighScoreAchieved(const Pacman* pacman, int score) {
    int center_x = MAP_WIDTH / 2;
    int center_y = MAP_HEIGHT / 2;
    
    // 하이스코어 달성 축하 박스
    drawEntity(center_x - 11, center_y - 6, "+=========================================+", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 11, center_y - 5, "|                                         |", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 11, center_y - 4, "|          *** NEW HIGH SCORE! ***        |", ANSI_GREEN ANSI_BOLD ANSI_BLINK);
    drawEntity(center_x - 11, center_y - 3, "|                                         |", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 11, center_y - 2, "+=========================================+", ANSI_YELLOW ANSI_BOLD);
    
    // 달성 점수와 순위 정보
    char score_text[50];
    sprintf(score_text, "Your Score: %d", score);
    drawEntity(center_x - 4, center_y, score_text, ANSI_CYAN ANSI_BOLD);
    
    char stage_text[50];
    sprintf(stage_text, "Stage Reached: %d", getCurrentStage());
    drawEntity(center_x - 4, center_y + 1, stage_text, ANSI_CYAN);
    
    // 예상 순위 계산 및 표시
    HighScore scores[MAX_HIGHSCORES];
    getHighScores(scores);
    int predicted_rank = 1;
    for(int i = 0; i < MAX_HIGHSCORES; i++) {
        if(scores[i].score > score) {
            predicted_rank++;
        } else {
            break;
        }
    }
    
    char rank_text[50];
    sprintf(rank_text, "Predicted Rank: %d", predicted_rank);
    drawEntity(center_x - 4, center_y + 2, rank_text, ANSI_YELLOW ANSI_BOLD);
    
    // 닉네임 입력 안내
    if(!getHighScoreEntryActive()) {
        drawEntity(center_x - 6, center_y + 4, "Press ENTER to save score", ANSI_WHITE ANSI_BOLD);
        drawEntity(center_x - 4, center_y + 5, "Press ESC to skip", ANSI_BRIGHT_BLACK);
    } else {
        // 닉네임 입력 모드
        renderNicknameInput(center_x, center_y + 4);
    }
    

    int offset = (getHighScoreEntryActive()) ? + 1 : 0;
    // 축하 메시지
    static int congratulation_timer = 0;
    const char* messages[] = {
        "CONGRATULATIONS!",
        "AMAZING SCORE!",
        "INCREDIBLE!",
        "YOU'RE AWESOME!"
    };
    congratulation_timer = (congratulation_timer + 1) % 120; // 2초마다 변경
    int msg_index = (congratulation_timer / 30) % 4;
    
    drawEntity(center_x - 4, center_y + 7 + offset, messages[msg_index], ANSI_RAINBOW ANSI_BOLD);
}


void renderNormalGameOver(const Pacman* pacman, const int current_score){
    // 게임 오버 메시지 오버레이
    int center_x = MAP_WIDTH / 2;
    int center_y = MAP_HEIGHT / 2;

    int offset = (getHighScoreEntryActive() == -1) ? -1 : 0;

    // ASCII 문자만 사용
    drawEntity(center_x - 7, center_y - 2 + offset, "+==========================+", ANSI_BLACK);
    drawEntity(center_x - 7, center_y - 1 + offset, "|        GAME OVER         |", ANSI_RED ANSI_BOLD ANSI_BLINK);
    drawEntity(center_x - 7, center_y + offset,     "+==========================+", ANSI_BLACK);

    // ✅ 하이스코어 저장 완료 메시지
    if(getHighScoreEntryActive() == -1) {
        drawEntity(center_x - 8, center_y - 4, "*** HIGH SCORE SAVED! ***", ANSI_GREEN ANSI_BOLD);
    }

    drawEntity(center_x - 4, center_y + 2, "Press R to Restart", ANSI_WHITE);
    drawEntity(center_x - 2, center_y + 3, "ESC to Quit", ANSI_WHITE);
    
    // 최종 스코어
    char final_score[50];
    sprintf(final_score, "Final Score: %d", current_score);
    drawEntity(center_x - 4, center_y + 5, final_score, ANSI_YELLOW ANSI_BOLD);
}

// 닉네임 입력 UI
void renderNicknameInput(int base_x, int base_y) {
    // 닉네임 입력 안내
    drawEntity(base_x - 4, base_y, "Enter your name:", ANSI_WHITE ANSI_BOLD);
    
    // 입력 박스
    drawEntity(base_x - 5, base_y + 1, "+================+", ANSI_WHITE);
    drawEntity(base_x - 5, base_y + 2, "|                |", ANSI_WHITE);
    drawEntity(base_x - 5, base_y + 3, "+================+", ANSI_WHITE);
    
    // 현재 입력된 닉네임 + 커서
    char display_name[MAX_NAME_LENGTH + 2];
    strcpy(display_name, getHighScoreEntryNickname());
    
    // 커서 깜빡임 효과
    static int cursor_blink = 0;
    cursor_blink = (cursor_blink + 1) % 30;
    
    if(cursor_blink < 15) {
        strcat(display_name, "_");
    } else {
        strcat(display_name, " ");
    }
    
    // 닉네임을 박스 중앙에 표시
    drawEntity(base_x - 3, base_y + 2, display_name, ANSI_GREEN ANSI_BOLD);
    
    // 입력 안내
    char length_info[30];
    sprintf(length_info, "%d/%d chars", (int)strlen(getHighScoreEntryNickname()), MAX_NAME_LENGTH - 1);
    drawEntity(base_x - 3, base_y + 5, length_info, ANSI_BRIGHT_BLACK);
    
    drawEntity(base_x - 6, base_y + 6, "A-Z, 0-9, SPACE, BACKSPACE", ANSI_WHITE);
    drawEntity(base_x - 6, base_y + 7, "ENTER: Confirm  ESC: Cancel", ANSI_WHITE);
}


void renderGameComplete(const Pacman* pacman) {
    int current_stage = getCurrentStage();
    // 현재 맵 상태 그대로 렌더링 (AI와 팩맨은 정지된 상태)
    renderGameplayScreen(pacman);
    
    // 게임 완료 메시지 오버레이
    int center_x = MAP_WIDTH / 2;
    int center_y = MAP_HEIGHT / 2;
    
    // ASCII 문자만 사용한 배경 박스
    drawEntity(center_x - 7, center_y - 2, "+============================+", ANSI_WHITE ANSI_BOLD);
    drawEntity(center_x - 7, center_y - 1, "|                            |", ANSI_WHITE ANSI_BOLD);
    drawEntity(center_x - 7, center_y,     "|                            |", ANSI_WHITE ANSI_BOLD);
    drawEntity(center_x - 7, center_y + 1, "|                            |", ANSI_WHITE ANSI_BOLD);
    drawEntity(center_x - 7, center_y + 2, "+============================+", ANSI_WHITE ANSI_BOLD);

    // ASCII 문자만 사용한 완료 메시지
    char stage_msg[50];
    sprintf(stage_msg, "*** STAGE %d COMPLETE! ***", current_stage);
    drawEntity(center_x - 6, center_y - 1, stage_msg, ANSI_GREEN ANSI_BOLD ANSI_BLINK);
    drawEntity(center_x - 6, center_y, "Press \"N\" for Next Stage", ANSI_YELLOW ANSI_BOLD);
    // drawEntity(center_x - 5, center_y, "Press R to Restart", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 3, center_y + 1, "ESC to Quit", ANSI_WHITE);
}


void renderAllClear(const Pacman* pacman){
    int current_stage = getCurrentStage();
    int score = getScore();
    // 배경을 어둡게 표시
    for (int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            drawEntity(x, y, "#", ANSI_BRIGHT_BLACK);
        }
    }
    
    // 올클리어 메시지 박스
    int center_x = MAP_WIDTH / 2;
    int center_y = MAP_HEIGHT / 2;
    
    // 큰 축하 박스 (ASCII 아트 스타일)
    drawEntity(center_x - 12, center_y - 6, "+===========================================+", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 12, center_y - 5, "|                                           |", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 12, center_y - 4, "|    ★★★  ALL STAGES CLEARED!  ★★★          |", ANSI_GREEN ANSI_BOLD ANSI_BLINK);
    drawEntity(center_x - 12, center_y - 3, "|                                           |", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 12, center_y - 2, "|             CONGRATULATIONS!              |", ANSI_CYAN ANSI_BOLD);
    drawEntity(center_x - 12, center_y - 1, "|                                           |", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 12, center_y,     "|         You are the PACMAN MASTER!        |", ANSI_MAGENTA ANSI_BOLD);
    drawEntity(center_x - 12, center_y + 1, "|                                           |", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 12, center_y + 2, "+===========================================+", ANSI_YELLOW ANSI_BOLD);
    
    // 최종 스코어와 통계
    char final_score[50];
    sprintf(final_score, "Final Score: %d", score);
    drawEntity(center_x - 5, center_y + 4, final_score, ANSI_WHITE ANSI_BOLD);
    
    char stages_completed[50];
    sprintf(stages_completed, "Stages Completed: %d", current_stage - 1);
    drawEntity(center_x - 6, center_y + 5, stages_completed, ANSI_GREEN);
    
    // 보너스 점수 계산 (예시)
    int bonus_score = pacman->lives * SCORE_LEFT_LIFE_BONUS;
    char bonus_text[50];
    sprintf(bonus_text, "Lives Bonus: %d x 1000 = %d", pacman->lives, bonus_score);
    drawEntity(center_x - 8, center_y + 6, bonus_text, ANSI_YELLOW);
    
    // 총합 점수
    char total_score[50];
    sprintf(total_score, "Total Score: %d", score + bonus_score);
    drawEntity(center_x - 5, center_y + 7, total_score, ANSI_RED ANSI_BOLD);
    
    // 조작 안내
    drawEntity(center_x - 7, center_y + 9, "Press \"R\" to Play Again", ANSI_WHITE ANSI_BOLD);
    drawEntity(center_x - 5, center_y + 10, "Press ESC to Exit", ANSI_WHITE);
    
    // 추가 축하 메시지들 (깜빡이는 효과)
    drawEntity(center_x - 7, center_y + 12, "★ PERFECT GAME CLEAR! ★", ANSI_RAINBOW ANSI_BLINK);
    
    // 사이드에 스코어 정보도 표시
    drawScore(pacman->lives);
}

// 타이틀 화면 렌더링
void renderTitleScreen() {
    int center_x = MAP_WIDTH / 2;
    int center_y = MAP_HEIGHT / 2;
    
    // 게임 제목 (ASCII 아트 스타일)
    drawEntity(center_x - 8, center_y - 8, "########    #######     #######   #######    ####     ####", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 8, center_y - 7, "##     ##   ##    ##   ##     ##  ##    ##   ## ##   ##  ##", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 8, center_y - 6, "########    #######    ##         #######    ##  ## ##   ##", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 8, center_y - 5, "##          ##    ##   ##     ##  ##    ##   ##   ###    ##", ANSI_YELLOW ANSI_BOLD);
    drawEntity(center_x - 8, center_y - 4, "##          ##     ##   #######   ##     ##  ##    #     ##", ANSI_YELLOW ANSI_BOLD);
    
    // 메뉴 옵션들
    char* menu_texts[MENU_COUNT] = {"Game Start", "How to Play", "High Scores", "Exit Game"};
    char* menu_colors[MENU_COUNT] = {ANSI_WHITE, ANSI_WHITE, ANSI_WHITE, ANSI_WHITE};
    
    for(int i = 0; i < MENU_COUNT; i++) {
        if(i == current_menu_selection) {
            // 선택된 메뉴는 하이라이트
            drawEntity(center_x - 2, center_y + 1 + i, "> ", ANSI_GREEN ANSI_BOLD);
            drawEntity(center_x, center_y + 1 + i, menu_texts[i], ANSI_GREEN ANSI_BOLD);
        } else {
            drawEntity(center_x - 2, center_y + 1 + i, "  ", ANSI_WHITE);
            drawEntity(center_x, center_y + 1 + i, menu_texts[i], ANSI_WHITE);
        }
    }
    
    // 조작 안내
    drawEntity(center_x - 8, center_y + 6, "UP/DOWN : Navigate  Enter : Select  ESC : Exit", ANSI_CYAN);
    
    // 팩맨 캐릭터 표시 (장식용)
    drawEntity(center_x - 12, center_y - 2, "O", PACMAN_COLOR);
    drawEntity(center_x + 10, center_y - 2, "O", GHOST_RED_COLOR);
    drawEntity(center_x + 11, center_y - 2, "O", GHOST_PINK_COLOR);
    drawEntity(center_x + 12, center_y - 2, "O", GHOST_GREEN_COLOR);
    drawEntity(center_x + 13, center_y - 2, "O", GHOST_ORANGE_COLOR);
}

// 도움말 화면 렌더링
void renderHelpScreen() {
    int center_x = MAP_WIDTH / 2;
    int start_y = 3;
    
    // 제목
    drawEntity(center_x - 4, start_y, "HOW TO PLAY", ANSI_YELLOW ANSI_BOLD);
    
    // 게임 설명
    drawEntity(2, start_y + 3, "OBJECTIVE:", ANSI_GREEN ANSI_BOLD);
    drawEntity(4, start_y + 4, "- Eat all cookies to proceed to next stage", ANSI_WHITE);
    drawEntity(4, start_y + 5, "- Avoid ghosts or eat them with power cookies", ANSI_WHITE);
    
    drawEntity(2, start_y + 7, "CONTROLS:", ANSI_GREEN ANSI_BOLD);
    drawEntity(4, start_y + 8, "- Arrow Keys: Move Pacman", ANSI_WHITE);
    drawEntity(4, start_y + 9, "- ESC: Exit Game", ANSI_WHITE);
    
    drawEntity(2, start_y + 11, "GAME ELEMENTS:", ANSI_GREEN ANSI_BOLD);
    drawEntity(4, start_y + 12, "O Pacman (You)", PACMAN_COLOR);
    drawEntity(4, start_y + 13, ". Cookie (10 points)", ANSI_YELLOW);
    drawEntity(4, start_y + 14, "O Power Cookie (50 points, weakens ghosts)", ANSI_YELLOW ANSI_BOLD);
    drawEntity(4, start_y + 15, "O Red Ghost (Chaser)", GHOST_RED_COLOR);
    drawEntity(4, start_y + 16, "O Pink Ghost (Ambusher)", GHOST_PINK_COLOR);
    drawEntity(4, start_y + 17, "O Green Ghost (Random)", GHOST_GREEN_COLOR);
    drawEntity(4, start_y + 18, "O Orange Ghost (Patrol)", GHOST_ORANGE_COLOR);
    
    drawEntity(2, start_y + 20, "SCORING:", ANSI_GREEN ANSI_BOLD);
    drawEntity(4, start_y + 21, "- Cookie: 10 points", ANSI_WHITE);
    drawEntity(4, start_y + 22, "- Power Cookie: 50 points", ANSI_WHITE);
    drawEntity(4, start_y + 23, "- Ghost: 200 points", ANSI_WHITE);
    drawEntity(4, start_y + 24, "- Bonus Fruit: 100 points", ANSI_WHITE);
    
    // 돌아가기 안내
    drawEntity(center_x - 6, start_y + 27, "Press any ESC or SPACE to go back", ANSI_CYAN ANSI_BOLD);
}

// 하이스코어 화면 렌더링
void renderHighScoreScreen() {
    HighScore scores[MAX_HIGHSCORES];
    int start_x = 5;  // 왼쪽부터 시작
    int start_y = 3;
    
    // 타이틀
    drawEntity(start_x + 10, start_y, "HIGH SCORES", ANSI_YELLOW ANSI_BOLD);
    drawEntity(start_x + 8, start_y + 1, "==============", ANSI_YELLOW);
    
    // 테이블 헤더
    drawEntity(start_x,      start_y + 3, "RANK", ANSI_WHITE ANSI_BOLD);
    drawEntity(start_x + 6,  start_y + 3, "NAME", ANSI_WHITE ANSI_BOLD);
    drawEntity(start_x + 18, start_y + 3, "SCORE", ANSI_WHITE ANSI_BOLD);
    drawEntity(start_x + 26, start_y + 3, "STAGE", ANSI_WHITE ANSI_BOLD);
    drawEntity(start_x + 32, start_y + 3, "DATE", ANSI_WHITE ANSI_BOLD);
    
    // 구분선
    drawEntity(start_x, start_y + 4, "========================================", ANSI_CYAN);
    
    int load_result = getHighScores(scores);
    
    if(load_result == -1) {
        drawEntity(start_x + 8, start_y + 8, "No high scores yet!", ANSI_RED);
        drawEntity(start_x + 6, start_y + 10, "Play the game to set records!", ANSI_WHITE);
    } else {
        for(int i = 0; i < MAX_HIGHSCORES && i < 10; i++) {
            if(scores[i].score > 0) {
                char rank_str[4];
                char score_str[10];
                char stage_str[4];
                
                sprintf(rank_str, "%2d.", i + 1);
                sprintf(score_str, "%7d", scores[i].score);
                sprintf(stage_str, "%2d", scores[i].stage);
                
                const char* color = (i < 3) ? ANSI_YELLOW ANSI_BOLD : ANSI_WHITE;
                
                drawEntity(start_x,      start_y + 6 + i, rank_str, color);
                drawEntity(start_x + 6,  start_y + 6 + i, scores[i].name, color);
                drawEntity(start_x + 18, start_y + 6 + i, score_str, color);
                drawEntity(start_x + 26, start_y + 6 + i, stage_str, color);
                drawEntity(start_x + 32, start_y + 6 + i, scores[i].date, color);
                
                if(i == 0) {
                    drawEntity(start_x - 2, start_y + 6 + i, ">>", ANSI_YELLOW ANSI_BOLD);
                }
            }
        }
    }
    
    // 하단 메시지
    drawEntity(start_x + 8, start_y + 20, "Press ESC or SPACE to go back", ANSI_WHITE ANSI_BOLD);
}