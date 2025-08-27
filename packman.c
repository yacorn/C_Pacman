#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include "colors.h"

// 맵 요소
#define EMPTY 0
#define WALL 1
#define COOKIE 2
#define POWER_COOKIE 3
#define GHOST_DOOR 4
#define GHOST_ZONE 5
#define WARP_ZONE 6

#define MAX_GHOSTS 4

#define ORANGE_GHOST_CHASE_DISTANCE 64

#define POWER_MODE_DURATION 300

// 맵 크기
#define MAP_WIDTH 28
#define MAP_HEIGHT 31

// 스크린 관련 변수
HANDLE screen[2];
int screen_index = 0;

// 게임 상태 관련 변수
int game_over = 0;
int power_mode = 0;
int power_mode_timer = 0;

// 디버그 관련 변수
int debug_mode = 0;

// 맵 관련 변수
// map[0~31][0~27]
// 맵의 GHOST_DOOR의 좌표는 14,13
int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,3,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,3,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,4,1,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,5,5,5,5,5,5,1,0,1,1,2,1,1,1,1,1,1},
    {6,0,0,0,0,0,2,0,0,0,1,5,5,5,5,5,5,1,0,0,0,2,0,0,0,0,0,6}, // 가운데 줄, 좌우는 워프 존
    {1,1,1,1,1,1,2,1,1,0,1,5,5,5,5,5,5,1,0,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,3,2,2,1,1,2,2,2,2,2,2,2,0,0,2,2,2,2,2,2,2,1,1,2,2,3,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int exit_path[][2] = {
    // {13, 13},
    {13, 12},
    {13, 11}
};

int exit_path_length = sizeof(exit_path) / sizeof(exit_path[0]);

// 방향 정의
typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

// 유령과 팩맨 충돌 관련 열거형 정의
typedef enum {
    COLLISION_NONE = 0,
    COLLISION_PACMAN_DIES,
    COLLISION_GHOST_EATEN,
    COLLISION_IGNORED
} CollisionResult;

// 팩맨 구조체 정의
typedef struct {
    int x;
    int y;
    Direction direction;
    int lives;
} Pacman;

// 고스트 구조체 정의
typedef enum { CHASING, FRIGHTENED, EATEN, EXITING, WAITING, RETURNING } GhostState;

typedef struct {
    int x;
    int y;
    int direction;
    GhostState state;
    char color;
    int path_index;
    int target_x;
    int target_y;
} Ghost;

Ghost ghosts[MAX_GHOSTS];

// 고스트 릴리즈 조건 구조체 정의
typedef struct {
    int cookies_required;
    int score_required;
    int time_required;
} GhostReleaseCondition;

GhostReleaseCondition release_conditions[] = {
    {0, 0, 0},
    {30, 200, 600},
    {80, 800, 1800},
    {150, 1500, 3600}
};

// 고스트 큐 관련 변수
Ghost* ghostQueue[MAX_GHOSTS];
int queue_front = 0, queue_rear = 0, queue_count = 0;

// 함수들 미리 선언
void initialize(); void initializeGhosts(); void hideCursor();
int isIntersection(int next_x, int next_y);
void enqueueGhost(Ghost* ghost); void dequeueGhost(); Ghost* frontGhost();
void render(const Pacman* pacman, int score); void drawEntity(int x, int y, const char* str, const char* color);
void drawScore(int score, int lives); void drawDebugInfo(const Pacman* pacman, int score); void drawGhostDebugInfo();
void clear(); void flip(); void releaseScreen();
void updateExitingGhost(Ghost* ghost); 
void updateRedGhost(Ghost* ghost, const Pacman* pacman); void updatePinkGhost(Ghost* ghost, const Pacman* pacman);
void updateGreenGhost(Ghost* ghost, const Pacman* pacman); void updateOrangeGhost(Ghost* ghost, const Pacman* pacman);
void updateAllGhost(const Pacman* pacman);
void decideGhostDirectionToTarget(Ghost* ghost, int target_x, int target_y);
void decideGhostDirection(Ghost* ghost, const Pacman* pacman);
int canGhostMoveTo(int x, int y, GhostState state);
void moveGhost(Ghost* ghost); void returnToGhostZone(Ghost* ghost); void killGhost(Ghost* ghost);
void findPacmanFuturePosition(const Pacman* pacman, int* future_x, int* future_y, int look_ahead);
void updatePacman(Pacman* pacman, int* score); void processInput(Pacman* pacman);

// 유틸리티 함수들
int isIntersection(int next_x, int next_y){
    int open_path = 0;

    if(map[next_y - 1][next_x] != WALL) open_path++; // 위
    if(map[next_y + 1][next_x] != WALL) open_path++; // 아래
    if(map[next_y][next_x - 1] != WALL) open_path++; // 왼쪽
    if(map[next_y][next_x + 1] != WALL) open_path++; // 오른쪽

    return open_path >= 3;
}

CollisionResult checkCollision(const Pacman* pacman, const Ghost* ghost){
    if(pacman->x != ghost->x || pacman->y != ghost->y) return COLLISION_NONE;

    switch(ghost->state){
        case CHASING:
            return COLLISION_PACMAN_DIES;
        case FRIGHTENED:
            return COLLISION_GHOST_EATEN;
        case EATEN:
        case RETURNING:
        case EXITING:
        case WAITING:
            return COLLISION_IGNORED;
        default:
            return COLLISION_NONE;
    }
}

// 큐 관련 함수들
void enqueueGhost(Ghost* ghost){
    if(queue_count < MAX_GHOSTS){

        for(int i = 0; i < queue_count; i++){
            int queue_index = (queue_front + i) % MAX_GHOSTS;
            if(ghostQueue[queue_index] == ghost){
                return;
            }
        }

        ghostQueue[queue_rear] = ghost;
        queue_rear = (queue_rear + 1) % MAX_GHOSTS;
        queue_count++;
        ghost->state = WAITING;
        ghost->path_index = 0;

        // 현재 위치에서 가장 가까운 exit_path 인덱스 찾기
        int best_index = 0;
        int min_distance = abs(ghost->x - exit_path[0][0]) + abs(ghost->y - exit_path[0][1]);
        
        for(int i = 1; i < exit_path_length; i++){
            int distance = abs(ghost->x - exit_path[i][0]) + abs(ghost->y - exit_path[i][1]);
            if(distance < min_distance || (distance == min_distance && i > best_index)){
                min_distance = distance;
                best_index = i;
            }
        }
        
        ghost->path_index = best_index;
    }
}

Ghost* frontGhost() {
    if(queue_count > 0) return ghostQueue[queue_front];
    return NULL;
}

void dequeueGhost(){
    if(queue_count > 0){
        Ghost* ghost = ghostQueue[queue_front];
        ghost->state = EXITING;
        queue_front = (queue_front + 1) % MAX_GHOSTS;
        queue_count--;
    }
}

// 초기화 함수들
void initialize() {
    // HANDLE std_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    // // 버퍼는 넉넉하게, 윈도우는 필요한 만큼만
    // COORD buffer_size = {MAP_WIDTH * 2 + 20, MAP_HEIGHT * 2}; // 76 x 36 (넉넉하게)
    // SMALL_RECT window_size = {0, 0, (MAP_WIDTH * 2 + 20) - 1, MAP_HEIGHT * 2}; // 75 x 32 (맵+여유공간)

    // // 먼저 윈도우를 작게 만들기
    // SMALL_RECT small_rect = {0, 0, 10, 10};
    // SetConsoleWindowInfo(std_handle, TRUE, &small_rect);

    // SetConsoleScreenBufferSize(std_handle, buffer_size);
    // SetConsoleWindowInfo(std_handle, TRUE, &window_size);

    // // 화면 버퍼를 2개 생성합니다.
    // screen[0] = CreateConsoleScreenBuffer(
    //     GENERIC_READ | GENERIC_WRITE,
    //     0,
    //     NULL,
    //     CONSOLE_TEXTMODE_BUFFER,
    //     NULL
    // );

    // screen[1] = CreateConsoleScreenBuffer(
    //     GENERIC_READ | GENERIC_WRITE,
    //     0,
    //     NULL,
    //     CONSOLE_TEXTMODE_BUFFER,
    //     NULL
    // );

    // // 버퍼 크기 설정
    // SetConsoleScreenBufferSize(screen[0], buffer_size);
    // SetConsoleScreenBufferSize(screen[1], buffer_size);

    // hideCursor();
    // SetConsoleOutputCP(65001);
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

void initializeGhosts() {
    // 빨간 GHOST
    ghosts[0] = (Ghost){13, 13, DIR_NONE, WAITING, 'R', 0, -1, -1};

    // 분홍 유령
    ghosts[1] = (Ghost){14, 13, DIR_NONE, WAITING, 'P', 0, -1, -1};

    // 청록 유령
    ghosts[2] = (Ghost){14, 14, DIR_NONE, WAITING, 'G', 0, -1, -1};

    // 주황 유령
    ghosts[3] = (Ghost){13, 14, DIR_NONE, WAITING, 'O', 0, -1, -1};

    for(int i = 0; i < MAX_GHOSTS; i++){
        enqueueGhost(&ghosts[i]);
    }
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursor_info;

    cursor_info.dwSize = 1; // 커서 크기 설정
    cursor_info.bVisible = FALSE; // 커서 숨기기

    SetConsoleCursorInfo(screen[0], &cursor_info);
    SetConsoleCursorInfo(screen[1], &cursor_info);
}

// 랜더 관련 함수들
void render(const Pacman* pacman, int score) {
    clear();

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
                drawEntity(x, y, "C", PACMAN_COLOR);
            } else if(is_ghost_here){
                switch(current_ghost->color){
                    case 'R': drawEntity(x, y, "R", GHOST_RED_COLOR); break;
                    case 'P': drawEntity(x, y, "P", GHOST_PINK_COLOR); break;
                    case 'G': drawEntity(x, y, "G", GHOST_GREEN_COLOR); break;
                    case 'O': drawEntity(x, y, "O", GHOST_ORANGE_COLOR); break;
                }
            } else if(is_target_here){
                // 목표 위치 표시
                switch(target_ghost_color){
                    case 'R': drawEntity(x, y, "T", GHOST_RED_COLOR ANSI_BLINK); break;
                    case 'P': drawEntity(x, y, "T", GHOST_PINK_COLOR ANSI_BLINK); break;
                    case 'G': drawEntity(x, y, "T", GHOST_GREEN_COLOR ANSI_BLINK); break;
                    case 'O': drawEntity(x, y, "T", GHOST_ORANGE_COLOR ANSI_BLINK); break;
                }
            } else {
                switch(map[y][x]){
                case EMPTY: drawEntity(x, y, " ", ANSI_RESET); break;
                case WALL: drawEntity(x, y, "#", WALL_COLOR); break;
                case COOKIE: drawEntity(x, y, "·", COOKIE_COLOR); break;
                case POWER_COOKIE: drawEntity(x, y, "●", POWER_COOKIE_COLOR); break;
                case GHOST_DOOR: drawEntity(x, y, "─", ANSI_WHITE); break;
                case GHOST_ZONE: drawEntity(x, y, " ", GHOST_ZONE_COLOR); break;
                case WARP_ZONE: drawEntity(x, y, "W", SUCCESS_COLOR); break;
                }
            }
            
        }
    }

    drawScore(score, pacman->lives);
    
    if(debug_mode){

        // 디버그 모드 상태 표시
        int score_x = MAP_WIDTH * 2 + 2;
        drawEntity(score_x / 2, 15, "DEBUG MODE", DEBUG_COLOR);
        drawEntity(score_x / 2, 16, "SPACE: Toggle", INFO_COLOR);

        drawGhostDebugInfo();
        
        // int score_x = MAP_WIDTH * 2 + 2;
        // int debug_y = 16;  // 시작 Y 위치
        
        // ANSI 이스케이프 시퀀스에 변수 사용
        // printAtPosition(score_x / 2, debug_y, "DEBUG MODE", DEBUG_COLOR);
        // printAtPosition(score_x / 2, debug_y + 1, "SPACE: Toggle", INFO_COLOR);
    }

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

void drawGhostDebugInfo(){
    int score_x = MAP_WIDTH * 2 + 2;

    // drawEntity 사용 (더블버퍼 호환)
    drawEntity(score_x / 2, 9, "PINK GHOST:", GHOST_PINK_COLOR);
    
    char buffer[50];
    sprintf(buffer, "X: %d", ghosts[1].x);
    drawEntity(score_x / 2, 10, buffer, INFO_COLOR);
    
    sprintf(buffer, "Y: %d", ghosts[1].y);
    drawEntity(score_x / 2, 11, buffer, INFO_COLOR);
    
    const char* state_names[] = {"CHASING", "FRIGHTENED", "EATEN", "EXITING", "WAITING", "RETURNING"};
    sprintf(buffer, "STATE: %s", state_names[ghosts[1].state]);
    drawEntity(score_x / 2, 12, buffer, SUCCESS_COLOR);
    
    sprintf(buffer, "PATH: %d", ghosts[1].path_index);
    drawEntity(score_x / 2, 13, buffer, INFO_COLOR);
    
    if(ghosts[1].target_x != -1 && ghosts[1].target_y != -1) {
        sprintf(buffer, "TARGET: (%d,%d)", ghosts[1].target_x, ghosts[1].target_y);
        drawEntity(score_x / 2, 14, buffer, ANSI_YELLOW);
    }
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
    printf(ANSI_SHOW_CURSOR);
    printf(ANSI_RESET);
    printf(ANSI_CLEAR);
    CloseHandle(screen[0]);
    CloseHandle(screen[1]);
}

// AI 관련 함수들
void updateExitingGhost(Ghost* ghost) {
    // 이미 출구 도달
    if (ghost->path_index >= exit_path_length) {
        ghost->state = CHASING;
        ghost->target_x = -1;
        ghost->target_y = -1;
        return;
    }

    ghost->target_x = exit_path[ghost->path_index][0];
    ghost->target_y = exit_path[ghost->path_index][1];

    int target_x = exit_path[ghost->path_index][0];
    int target_y = exit_path[ghost->path_index][1];

    // 현재 목표 좌표 도달 시 path_index 증가
    if (ghost->x == target_x && ghost->y == target_y) {
        ghost->path_index++;
        return;
    }

    // 다음 좌표를 향해 방향 결정
    int next_x = ghost->x;
    int next_y = ghost->y;

    if (ghost->x < target_x) next_x++;
    else if (ghost->x > target_x) next_x--;
    else if (ghost->y < target_y) next_y++;
    else if (ghost->y > target_y) next_y--;

    // 이동 가능하면 방향 설정
    if (canGhostMoveTo(next_x, next_y, ghost->state)) {
        if (next_x > ghost->x) ghost->direction = DIR_RIGHT;
        else if (next_x < ghost->x) ghost->direction = DIR_LEFT;
        else if (next_y > ghost->y) ghost->direction = DIR_DOWN;
        else if (next_y < ghost->y) ghost->direction = DIR_UP;
    } else {
        // 막혔을 때는 다른 방향 탐색
        Direction directions[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
        for (int i = 0; i < 4; i++) {
            next_x = ghost->x;
            next_y = ghost->y;
            switch(directions[i]) {
                case DIR_UP: next_y--; break;
                case DIR_DOWN: next_y++; break;
                case DIR_LEFT: next_x--; break;
                case DIR_RIGHT: next_x++; break;
            }
            if (canGhostMoveTo(next_x, next_y, ghost->state)) {
                ghost->direction = directions[i];
                break;
            }
        }
    }
}

void updateRedGhost(Ghost* ghost, const Pacman* pacman){
    // 타겟의 x,y좌표
    ghost->target_x = pacman->x;
    ghost->target_y = pacman->y;

    decideGhostDirectionToTarget(ghost, ghost->target_x, ghost->target_y);
    moveGhost(ghost);
}

void updatePinkGhost(Ghost* ghost, const Pacman* pacman){
    int target_x = pacman->x;
    int target_y = pacman->y;

    findPacmanFuturePosition(pacman, &target_x, &target_y, 4);

    ghost->target_x = target_x;
    ghost->target_y = target_y;

    decideGhostDirectionToTarget(ghost, target_x, target_y);
    moveGhost(ghost);
}

void updateGreenGhost(Ghost* ghost, const Pacman* pacman){
    // 청록 유령은 빨간 유령과 팩맨의 위치를 기반으로 목표 좌표를 설정
    Ghost* red_ghost = &ghosts[0]; // 빨간 유령

    int pacman_future_x = pacman->x;
    int pacman_future_y = pacman->y;

    findPacmanFuturePosition(pacman, &pacman_future_x, &pacman_future_y, 2);

    // 빨간 유령과 팩맨의 미래 위치를 기반으로 목표 좌표 계산
    int vector_x = pacman_future_x - red_ghost->x;
    int vector_y = pacman_future_y - red_ghost->y;

    ghost->target_x = pacman_future_x + ( vector_x * 2 );
    ghost->target_y = pacman_future_y + ( vector_y * 2 );

    decideGhostDirectionToTarget(ghost, ghost->target_x, ghost->target_y);
    moveGhost(ghost);
}

void updateOrangeGhost(Ghost* ghost, const Pacman* pacman){
    // 먼저 팩맨과 고스트의 거리를 구해보자
    int dx = pacman->x - ghost->x;
    int dy = pacman->y - ghost->y;

    int distance = dx * dx + dy * dy;
    if(distance >= ORANGE_GHOST_CHASE_DISTANCE){ // 8*8 = 64
        ghost->target_x = pacman->x;
        ghost->target_y = pacman->y;

        decideGhostDirectionToTarget(ghost, ghost->target_x, ghost->target_y);
        moveGhost(ghost);
    } else {
        // 맵의 왼쪽 아래 코너 좌표를 하드코딩 (1, 29)
        ghost->target_x = 1;
        ghost->target_y = 29;

        decideGhostDirectionToTarget(ghost, ghost->target_x, ghost->target_y);
        moveGhost(ghost);
    }
}

void updateAllGhost(const Pacman* pacman){
    for(int i = 0; i < MAX_GHOSTS; i++){
        switch(ghosts[i].state){
            case WAITING:
                // 대기중인 유령은 아무것도 안함
                break;
            case RETURNING:
                returnToGhostZone(&ghosts[i]);
                break;
            case EXITING:
                updateExitingGhost(&ghosts[i]);
                moveGhost(&ghosts[i]);
                break;

            case CHASING:
                switch(ghosts[i].color){
                    case 'R':
                        updateRedGhost(&ghosts[i], pacman);
                        break;
                    case 'P':
                        updatePinkGhost(&ghosts[i], pacman);
                        break;
                    case 'G':
                        updateGreenGhost(&ghosts[i], pacman);
                        break;
                    case 'O':
                        updateOrangeGhost(&ghosts[i], pacman);
                        break;
                }
                break;

            case FRIGHTENED:
                break;

            case EATEN:
                break;
        }
    }
}

void getNextPosition(int* x, int* y, Direction dir){
    switch(dir){
        case DIR_UP: (*y)--; break;
        case DIR_DOWN: (*y)++; break;
        case DIR_LEFT: (*x)--; break;
        case DIR_RIGHT: (*x)++; break;
    }
}

void decideGhostDirection(Ghost* ghost, const Pacman* pacman){

    // 1. 현재 좌표를 기준으로 갈 수 있는 방향을 탐색
    // 2. 진행 반대 방향을 제외하고 갈 수 있는 모든 방향이 막혔을 경우에 진행 반대 방향으로 변경
    // 3. 가능한 방향중 팩맨과 가장 가까워지는 최적의 방향으로 변경

    int possible_directions[4] = {0, 0, 0, 0}; // UP, DOWN, LEFT, RIGHT
    int direction_count = 0;

    int direction_to_check[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    int opposite_direction = 0;

    int best_direction = 0;
    int min_distance = INT_MAX;

    switch(ghost->direction){
        case DIR_UP: opposite_direction = DIR_DOWN; break;
        case DIR_DOWN: opposite_direction = DIR_UP; break;
        case DIR_LEFT: opposite_direction = DIR_RIGHT; break;
        case DIR_RIGHT: opposite_direction = DIR_LEFT; break;
        default: opposite_direction = DIR_NONE; break;
    }

    // 4방향 탐색
    for(int i = 0; i < 4; i++){
        int dir = direction_to_check[i];
        int next_x = ghost->x;
        int next_y = ghost->y;
        switch(dir){
            case DIR_UP: next_y--; break;
            case DIR_DOWN: next_y++; break;
            case DIR_LEFT: next_x--; break;
            case DIR_RIGHT: next_x++; break;
        }

        if (map[next_y][next_x] != WALL && map[next_y][next_x] != GHOST_DOOR && dir != opposite_direction){
            // possible_directions[direction_count++] = dir;
            // 여기서 바로 pacman과의 멘해튼 거리를 계산해서 불필요한 반복 계산을 줄이자
            // 여기서 만약 하나라도 있으면 그 방향이 최적의 방향이기 때문에 direction_count는 증가 시킬 필요가없다.
            // 만약 여기서 하나라도 없으면 best_direction이 0이므로
            // 이를 기반으로 반대 방향으로 갈지 판단하면 된다.
            int distance = abs(next_x - pacman->x) + abs(next_y - pacman->y);
            if(distance < min_distance){
                min_distance = distance;
                best_direction = dir;
            }
        }
    }

    if(best_direction != 0){
        ghost->direction = best_direction;
    } else {
        ghost->direction = opposite_direction;
    }

    // if (direction_count == 0){
    //     ghost->direction = opposite_direction;
    //     return;
    // }

    // 가능한 방향중 팩맨과 가장 가까워지는 최적의 방향을 찾자
    

    // for(int i = 0; i < direction_count; i++){
    //     int dir = possible_directions[i];
    //     int next_x = ghost->x;
    //     int next_y = ghost->y;
    //     // 예상 위치 계산
    //     switch(dir){
    //         case DIR_UP: next_y--; break;
    //         case DIR_DOWN: next_y++; break;
    //         case DIR_LEFT: next_x--; break;
    //         case DIR_RIGHT: next_x++; break;
    //     }

    //     int distance = abs(next_x - pacman->x) + abs(next_y - pacman->y);
    //     if(distance < min_distance){
    //         min_distance = distance;
    //         best_direction = dir;
    //     }
    // }

    // if(best_direction != -1){
    //     ghost->direction = best_direction;
    // }
}

void decideGhostDirectionToTarget(Ghost* ghost, int target_x, int target_y){
    // 1. 현재 좌표를 기준으로 갈 수 있는 방향을 탐색
    // 2. 진행 반대 방향을 제외하고 갈 수 있는 모든 방향이 막혔을 경우에 진행 반대 방향으로 변경
    // 3. 가능한 방향중 팩맨 앞 4칸을 목표로해서 가장 가까워지는 최적의 방향으로 변경

    int possible_directions[4] = {0, 0, 0, 0}; // UP, DOWN, LEFT, RIGHT
    int direction_count = 0;

    int direction_to_check[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    int opposite_direction = 0;

    int best_direction = 0;
    int min_distance = INT_MAX;

    switch(ghost->direction){
        case DIR_UP: opposite_direction = DIR_DOWN; break;
        case DIR_DOWN: opposite_direction = DIR_UP; break;
        case DIR_LEFT: opposite_direction = DIR_RIGHT; break;
        case DIR_RIGHT: opposite_direction = DIR_LEFT; break;
        default: opposite_direction = DIR_NONE; break;
    }

    // 4방향 탐색
    for(int i = 0; i < 4; i++){
        int next_x = ghost->x;
        int next_y = ghost->y;
        getNextPosition(&next_x, &next_y, direction_to_check[i]);

        if (map[next_y][next_x] != WALL && map[next_y][next_x] != GHOST_DOOR && direction_to_check[i] != opposite_direction){
            int distance = abs(next_x - target_x) + abs(next_y - target_y);
            if(distance < min_distance){
                min_distance = distance;
                best_direction = direction_to_check[i];
            }
        }
    }

    if(best_direction == 0){
        ghost->direction = opposite_direction;
    } else {
        ghost->direction = best_direction;
    }

    // if (direction_count == 0){
    //     ghost->direction = opposite_direction;
    //     return;
    // }

    // 가능한 방향중 팩맨 앞 4칸을 목표로해서 가장 가까워지는 최적의 방향으로 변경
    

    // for(int i = 0; i < direction_count; i++){
    //     int dir = possible_directions[i];
    //     int next_x = ghost->x;
    //     int next_y = ghost->y;
    //     // 예상 위치 계산
    //     getNextPosition(&next_x, &next_y, dir);

    //     int distance = abs(next_x - target_x) + abs(next_y - target_y);
    //     if(distance < min_distance){
    //         min_distance = distance;
    //         best_direction = dir;
    //     }
    // }

    // if(best_direction != -1){
    //     ghost->direction = best_direction;
    // }
}

int canGhostMoveTo(int x, int y, GhostState state){
    if(x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return 0;

    int tile = map[y][x];

    switch(state){
        case EXITING:
            return (tile != WALL);
        case CHASING:
        case FRIGHTENED:
            return (tile != WALL && tile != GHOST_DOOR);
        case EATEN:
            // 먹힌 상태면 모든 곳 통과
            return 1;
        default:
            return (tile != WALL);
    }
}

void moveGhost(Ghost* ghost){
    int next_x = ghost->x;
    int next_y = ghost->y;

    getNextPosition(&next_x, &next_y, ghost->direction);

    if(canGhostMoveTo(next_x, next_y, ghost->state)){
        ghost->x = next_x;
        ghost->y = next_y;
    }
}

void findPacmanFuturePosition(const Pacman* pacman, int* future_x, int* future_y, int look_ahead){
    *future_x = pacman->x;
    *future_y = pacman->y;

    if(pacman->direction == DIR_NONE) return;

    int dx = 0, dy = 0;
    switch(pacman->direction){
        case DIR_UP: dy = -1; break;
        case DIR_DOWN: dy = 1; break;
        case DIR_LEFT: dx = -1; break;
        case DIR_RIGHT: dx = 1; break;
    }

    for(int step = 0; step < look_ahead; step++){
        int test_x = pacman->x + (dx * step);
        int test_y = pacman->y + (dy * step);

        if(test_x >= 0 && test_x < MAP_WIDTH 
            && test_y >= 0 && test_y < MAP_HEIGHT 
            // && map[test_y][test_x] != WALL 
            && map[test_y][test_x] != GHOST_DOOR
        ){
                *future_x = test_x;
                *future_y = test_y;
        } else {
            break;
        }
    }
}

void killGhost(Ghost* ghost){
    ghost->state = RETURNING;
    ghost->direction = DIR_NONE;
}

void returnToGhostZone(Ghost* ghost){
    int center_x = 13;
    int center_y = 13;
}

// 플레이어 관련 함수들
void updatePacman(Pacman* pacman, int* score){
    int next_x = pacman->x;
    int next_y = pacman->y;

    // 현재 방향에 따라 다음 위치 계산
    switch(pacman->direction){
        case DIR_UP: next_y--; break;
        case DIR_DOWN: next_y++; break;
        case DIR_LEFT: next_x--; break;
        case DIR_RIGHT: next_x++; break;
        default: break;
    }

    if(map[next_y][next_x] != WALL){
        
        // 워프존 확인
        if(map[next_y][next_x] == WARP_ZONE){
            if(next_x == 0){ // 왼쪽 워프존
                pacman->x = MAP_WIDTH - 1;
            } else if(next_x == MAP_WIDTH - 1){ // 오른쪽 워프존
                pacman->x = 1;
            }
            return;
        }

        // 쿠키인지확인
        if(map[next_y][next_x] == COOKIE){
            // 쿠키 먹기
            map[next_y][next_x] = EMPTY;
            (*score) += 10; // 점수 증가
        } else if(map[next_y][next_x] == POWER_COOKIE){
            map[next_y][next_x] = EMPTY;
            (*score) += 50; // 점수 증가
        }

        if(map[next_y][next_x] == GHOST_DOOR || map[next_y][next_x] == GHOST_ZONE){
            // 유령문이나 유령구역은 들어갈 수 없음
            return;
        }

        pacman->x = next_x;
        pacman->y = next_y;
    }
}

void processInput(Pacman* pacman){
    if(_kbhit()){
        int key = _getch();

        if (key == 32){
            debug_mode = !debug_mode;
            return;
        }

        if (key == 224){
            key = _getch();
            switch(key){
                case 72: {
                    // 위 방향으로 변경
                    if(pacman->y - 1 < 0 || map[pacman->y - 1][pacman->x] == WALL){
                        return;
                    }
                    pacman->direction = DIR_UP; 
                    break;
                }
                case 80: {
                    if(pacman->y + 1 >= MAP_HEIGHT || map[pacman->y + 1][pacman->x] == WALL){
                        return;
                    }
                    pacman->direction = DIR_DOWN;
                    break;
                }
                case 75: {
                    if(pacman->x - 1 < 0 || map[pacman->y][pacman->x - 1] == WALL){
                        return;
                    }
                    pacman->direction = DIR_LEFT;
                    break;
                }
                case 77: {
                    if(map[pacman->y][pacman->x + 1] == WALL){
                        return;
                    }
                    pacman->direction = DIR_RIGHT;
                    break;
                }
            }
        }
    }
}

// 메인 함수
int main() {
    int score = 1500;
    int cookies_eaten = 0;
    int game_time = 0;
    int ghost_released = 0;

    initialize();
    initializeGhosts();
    
    Pacman pacman = {13, 23, DIR_NONE, 3};

    while(1){
        processInput(&pacman);
        updatePacman(&pacman, &score);

        if(ghost_released < MAX_GHOSTS && queue_count > 0){
            GhostReleaseCondition conditions = release_conditions[ghost_released];

            if(cookies_eaten >= conditions.cookies_required ||
               score >= conditions.score_required ||
               game_time >= conditions.time_required) {
                dequeueGhost();
                ghost_released++;
            }
        }

        updateAllGhost(&pacman);
        render(&pacman, score);
        flip();
        Sleep(150); // 60FPS
    }

    releaseScreen();
    return 0;
}