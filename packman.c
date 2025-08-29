#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <string.h>
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
#define GHOST_ZONE_X 13
#define GHOST_ZONE_Y 14

#define ORANGE_GHOST_CHASE_DISTANCE 64

#define POWER_MODE_DURATION 300

// 맵 크기
#define MAP_WIDTH 28
#define MAP_HEIGHT 31

// 스크린 관련 변수
HANDLE screen[2];
int screen_index = 0;

// 게임 상태 관련 변수
int game_time = 0;
int score = 0;
int power_mode = 0;
int power_mode_timer = 0;
int total_cookies = 0;
int cookies_eaten = 0;
int ghost_released = 0;
int current_stage = 1;

// 디버그 관련 변수
int debug_mode = 0;

// 맵 관련 변수
// map[0~31][0~27]
// 맵의 GHOST_DOOR의 좌표는 14,13
int map_stage1[MAP_HEIGHT][MAP_WIDTH] = {
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
    {6,0,0,0,0,0,2,0,0,0,1,5,5,5,5,5,5,1,0,0,0,2,0,0,0,0,0,6},
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
// Stage 2: 중앙 격전지
int map_stage2[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,1,1,2,2,2,1,1,1,2,2,2,2,1,1,1,2,2,2,1,1,2,2,2,1},
    {1,2,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,3,2,2,2,2,2,1,2,1,1,1,2,1,1,2,1,1,1,2,1,2,2,2,2,2,3,1},
    {1,2,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,1,1,1,2,2,2,2,1,1,1,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,2,1}, // exit_path (13,11) 확보
    {1,2,2,2,2,2,2,1,2,1,1,1,2,2,2,2,1,1,2,1,2,2,2,2,2,2,2,1}, // exit_path (13,12) 확보
    {1,0,0,0,0,1,2,2,2,2,1,1,1,4,1,1,1,1,2,2,2,1,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,2,1,5,5,5,5,5,5,1,2,1,1,1,1,1,1,1,1,1},
    {6,2,2,2,2,2,2,2,2,2,1,5,5,5,5,5,5,1,2,2,2,2,2,2,2,2,2,6},
    {1,1,1,1,1,1,1,1,1,2,1,5,5,5,5,5,5,1,2,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,1,1,1,1,1,2,2,2,2,1,1,1,1,1,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,1,1,2,0,0,2,1,1,1,2,2,2,2,2,2,2,2,1}, // 팩맨 시작 위치 (13,23) 확보
    {1,2,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,3,2,2,2,2,2,1,2,1,1,1,2,1,1,2,1,1,1,2,1,2,2,2,2,2,3,1},
    {1,2,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,2,2,2,1,1,2,2,2,1,1,1,2,2,2,2,1,1,1,2,2,2,1,1,2,2,2,1},
    {1,1,1,2,1,1,1,2,1,1,1,1,1,2,2,1,1,1,1,1,2,1,1,1,2,1,1,1},
    {1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};// Stage 3: 나선형 미로
// Stage 3: 고립된 방
int map_stage3[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,3,2,2,2,1,2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,2,1,2,2,2,3,1},
    {1,2,1,1,2,1,2,1,1,2,1,2,1,1,1,1,2,1,2,1,1,2,1,2,1,1,2,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,1,1,1,1,2,1},
    {1,2,2,2,2,1,2,1,1,1,2,2,2,2,2,2,2,2,1,1,1,2,1,2,2,2,2,1},
    {1,1,1,1,2,1,2,1,2,2,2,1,1,1,1,1,1,2,2,2,1,2,1,2,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,2,1,1,1,2,1},
    {1,2,2,2,2,1,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,2,2,2,1},
    {1,1,1,1,2,1,1,1,1,1,1,1,2,1,1,2,2,1,1,1,1,1,1,1,2,1,1,1}, // exit_path (13,11) 확보
    {6,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,6}, // exit_path (13,12) 확보
    {1,1,1,1,1,1,1,1,2,1,2,1,1,4,1,1,1,2,1,2,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1,2,2,2,1,5,5,5,5,1,2,2,2,1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,2,1,2,1,5,5,5,5,1,2,1,2,1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,2,2,2,1,5,5,5,5,1,2,2,2,1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,2,1,1,2,1,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,2,1,2,1,2,2,2,1,1,1,1,1,1,2,2,2,1,2,1,2,1,1,1,1},
    {1,2,2,2,2,1,2,1,1,1,2,2,2,2,2,2,2,2,1,1,1,2,1,2,2,2,2,1},
    {1,2,1,1,1,1,2,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,0,0,2,2,2,2,1,1,2,2,2,2,2,2,1}, // 팩맨 시작 위치 (13,23) 확보
    {1,2,1,1,2,1,2,1,1,2,1,2,1,1,1,1,2,1,2,1,1,2,1,2,1,1,2,1},
    {1,3,2,2,2,1,2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,2,1,2,2,2,3,1},
    {1,1,1,2,1,1,1,1,1,2,1,1,1,2,1,2,1,1,1,1,1,2,2,2,1,2,1,1},
    {1,1,2,2,2,1,1,1,1,2,2,2,2,2,1,2,2,2,1,1,1,2,1,1,1,2,1,1},
    {1,1,2,1,2,2,1,2,2,2,1,1,1,2,1,2,1,2,2,1,2,2,1,1,2,2,1,1},
    {1,1,2,2,1,2,2,2,1,2,1,1,1,2,2,2,1,1,2,2,2,2,2,2,2,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};// Stage 4: 듀얼 워프
// Stage 4: 분할된 미로 (최종 수정본)
int map_stage4[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,3,2,2,2,1,1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,1,2,2,2,3,1},
    {1,2,1,1,2,1,1,2,1,1,1,1,2,1,1,2,1,1,1,1,2,1,1,2,1,1,2,1},
    {1,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,1},
    {1,2,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,2,1,1,2,2,2,2,2,2,2,2,1,1,2,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,1}, // exit_path (13,11) 확보
    {1,2,2,2,2,2,2,2,2,2,2,1,1,2,1,1,1,2,2,2,2,2,2,2,2,2,2,1}, // exit_path (13,12) 확보
    {1,0,0,1,1,2,1,1,2,1,1,1,1,4,1,1,1,1,2,1,1,2,1,1,0,0,0,1},
    {1,1,1,1,1,2,1,1,2,1,1,5,5,5,5,5,1,1,2,1,1,2,1,1,1,1,1,1},
    {6,2,2,2,2,2,2,2,2,1,1,5,5,5,5,5,1,1,2,2,2,2,2,2,2,2,2,6},
    {1,1,1,1,1,2,1,1,2,1,1,5,5,5,5,5,1,1,2,1,1,2,1,1,1,1,1,1},
    {1,0,0,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,0,0,0,1},
    {1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,2,1,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,2,1,1,2,2,2,2,2,2,2,2,1,1,2,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,1,1,2,2,2,0,0,2,2,2,1,1,2,2,2,2,2,2,2,1}, // 팩맨 시작 위치 (13,23) 확보
    {1,2,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,2,1},
    {1,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,1},
    {1,2,1,1,2,1,1,2,1,1,1,1,2,1,1,2,1,1,1,1,2,1,1,2,1,1,2,1},
    {1,3,2,2,2,1,1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,1,2,2,2,3,1},
    {1,2,1,1,2,1,1,2,1,1,1,2,2,2,2,2,1,1,1,1,2,1,1,2,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int current_map[MAP_HEIGHT][MAP_WIDTH];

const int (*all_maps[])[MAP_WIDTH] = {
    map_stage1,
    map_stage2,
    map_stage3,
    map_stage4
};

int exit_path[][2] = {
    // {13, 13},
    {13, 12},
    {13, 11}
};

int exit_path_length = sizeof(exit_path) / sizeof(exit_path[0]);
int MAX_STAGES = sizeof(all_maps) / sizeof(all_maps[0]);

// 게임 상태 정의
typedef enum {
    STATE_TITLE,
    STATE_READY,
    STATE_PLAYING,
    STATE_PACMAN_DEATH,
    STATE_LEVEL_COMPLETE,
    STATE_GAME_OVER,
    STATE_CUTSCENE
} GameState;

GameState current_state;

// 방향 정의
typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct {
    Direction directions[4];
    Direction best_direction;
    Direction opposite_direction;
    int count;
    int best_distance;
} DirectionResult;

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
    int prev_x;
    int prev_y;
    Direction direction;
    int lives;
} Pacman;

// 고스트 구조체 정의
typedef enum { CHASING, FRIGHTENED, EXITING, WAITING, RETURNING } GhostState;

typedef struct {
    int x;
    int y;
    int prev_x;
    int prev_y;
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
void updatePacman(Pacman* pacman); void processInput(Pacman* pacman);
void updateFrightenedGhost(Ghost* ghost);
void handleGhostEaten(Ghost* ghost, int* score);
void handlePacmanDeath(Pacman* pacman);
void updateEatenGhost(Ghost* ghost);
void getNextPosition(int* x, int* y, Direction dir);
void activatePowerMode(); void drawPacmanDebugInfo(const Pacman* pacman); void drawGameStateInfo(); void drawMapInfo();
void restoreMap(const int map[MAP_HEIGHT][MAP_WIDTH]); int getTotalCookies(); int isGameComplete(); void resetGame(Pacman* pacman);
void renderGameComplete(const Pacman* pacman, int score);
void renderGameOver(const Pacman* pacman, int score); 
void renderGameplayScreen(const Pacman* pacman, int score);
void renderGhost(const Ghost* ghost, int x, int y);
void renderTarget(char ghost_color, int x, int y);
void renderMapTile(int tile, int x, int y);
void renderDebugInfo(const Pacman* pacman);
void nextStage(Pacman* pacman);

// 유틸리티 함수들
int isIntersection(int next_x, int next_y){
    int open_path = 0;

    if(current_map[next_y - 1][next_x] != WALL) open_path++; // 위
    if(current_map[next_y + 1][next_x] != WALL) open_path++; // 아래
    if(current_map[next_y][next_x - 1] != WALL) open_path++; // 왼쪽
    if(current_map[next_y][next_x + 1] != WALL) open_path++; // 오른쪽

    return open_path >= 3;
}

CollisionResult checkCollision(const Pacman* pacman, const Ghost* ghost){
    
    if(pacman->x == ghost->x && pacman->y == ghost->y)
    {
        // 충돌 발생
        switch(ghost->state){
        case CHASING:
            return COLLISION_PACMAN_DIES; // 팩맨이 죽음
        case FRIGHTENED:
            return COLLISION_GHOST_EATEN; // 고스트가 먹힘
        case RETURNING:
        case EXITING:
        case WAITING:
            return COLLISION_IGNORED;
        default:
            return COLLISION_NONE;
        }
    }

    if(pacman->x == ghost->prev_x && pacman->y == ghost->prev_y &&
       pacman->prev_x == ghost->x && pacman->prev_y == ghost->y)
    {
        // 서로 위치 바꿈
        switch(ghost->state){
        case CHASING:
            return COLLISION_PACMAN_DIES; // 팩맨이 죽음
        case FRIGHTENED:
            return COLLISION_GHOST_EATEN; // 고스트가 먹힘
        case RETURNING:
        case EXITING:
        case WAITING:
            return COLLISION_IGNORED;
        default:
            return COLLISION_NONE;
        }
    }
    
    return COLLISION_NONE;
    
}

DirectionResult findPossilbeDirections(const Ghost* ghost, int target_x, int target_y, int exclude_oppsite){
    DirectionResult result = {0};
    result.best_direction = DIR_NONE;
    result.best_distance = INT_MAX;
    result.opposite_direction = DIR_NONE;
    if(exclude_oppsite){
        switch(ghost->direction){
            case DIR_UP: result.opposite_direction = DIR_DOWN; break;
            case DIR_DOWN: result.opposite_direction = DIR_UP; break;
            case DIR_LEFT: result.opposite_direction = DIR_RIGHT; break;
            case DIR_RIGHT: result.opposite_direction = DIR_LEFT; break;
        }
    }

    Direction check_directions[] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};

    for(int i = 0; i < 4; i++){
        if(exclude_oppsite && check_directions[i] == result.opposite_direction) continue;

        int next_x = ghost->x , next_y = ghost->y;
        getNextPosition(&next_x, &next_y, check_directions[i]);

        if(canGhostMoveTo(next_x, next_y, ghost->state)) {
            result.directions[result.count++] = check_directions[i];
            
            // 목표가 있다면
            if(target_x != -1 && target_y != -1){
                int distance = abs(next_x - target_x) + abs(next_y - target_y);
                if(distance < result.best_distance){
                    result.best_distance = distance;
                    result.best_direction = check_directions[i];
                }
            }
        }
    }

    return result;
}

void handleCollisions(Pacman* pacman, int* score){
    for(int i = 0; i < MAX_GHOSTS; i++){
        CollisionResult result = checkCollision(pacman, &ghosts[i]);

        switch(result){
            case COLLISION_PACMAN_DIES:
                handlePacmanDeath(pacman);
                break;
            case COLLISION_GHOST_EATEN:
                handleGhostEaten(&ghosts[i], score);
                break;

            default: 
                break;
        }
    }
}

void handlePacmanDeath(Pacman* pacman){
    pacman->lives--;

    current_state = STATE_PACMAN_DEATH;

    // 팩맨 위치 초기화
    pacman->x = 13;
    pacman->y = 23;
    pacman->direction = DIR_NONE;

    power_mode = 0;
    power_mode_timer = 0;

    // 모든 유령들 대기 상태로 초기화
    for(int i = 0; i < MAX_GHOSTS; i++){
        if(ghosts[i].state == FRIGHTENED){
            ghosts[i].state = CHASING;
        }
    }
}

void handleGhostEaten(Ghost* ghost, int* score){
    static int ghost_combo = 0;
    int points = 200 * (1 << ghost_combo);
    *score += points;
    ghost_combo++;

    ghost->state = RETURNING;
    ghost->target_x = GHOST_ZONE_X;
    ghost->target_y = GHOST_ZONE_Y;

    if(power_mode_timer <= 0){
        ghost_combo = 0;
    }
}

void restoreMap(const int socurce_map[MAP_HEIGHT][MAP_WIDTH]){
    memcpy(current_map, socurce_map, sizeof(current_map));
}

int getTotalCookies(){
    int total = 0;

    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            if(current_map[y][x] == COOKIE || current_map[y][x] == POWER_COOKIE){
                total++;
            }
        }
    }
    return total;
}

int isGameComplete(){
    return cookies_eaten >= total_cookies;
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
    restoreMap(map_stage1);

    total_cookies = getTotalCookies();
}

void initializeGhosts() {
    // 빨간 GHOST
    ghosts[0] = (Ghost){12, 14, 12, 14, DIR_NONE, WAITING, 'R', 0, -1, -1};

    // 분홍 유령
    ghosts[1] = (Ghost){13, 14, 13, 14, DIR_NONE, WAITING, 'P', 0, -1, -1};

    // 청록 유령
    ghosts[2] = (Ghost){14, 14, 14, 14, DIR_NONE, WAITING, 'G', 0, -1, -1};

    // 주황 유령
    ghosts[3] = (Ghost){15, 14, 15, 14, DIR_NONE, WAITING, 'O', 0, -1, -1};

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

void renderGhost(const Ghost* ghost, int x, int y) {
    char ghost_str[2] = {ghost->color, '\0'};
    
    if(ghost->state == FRIGHTENED){
        if(power_mode_timer > 60){
            drawEntity(x, y, ghost_str, ANSI_BLUE ANSI_BOLD); // 파란색
        } else {
            drawEntity(x, y, ghost_str, ANSI_WHITE ANSI_BLINK); // 흰색 깜박임
        }
    } else if(ghost->state == RETURNING){
        drawEntity(x, y, "◇", ANSI_WHITE);  // 눈만 남은 상태
    } else {
        const char* colors[] = {GHOST_RED_COLOR, GHOST_PINK_COLOR, GHOST_GREEN_COLOR, GHOST_ORANGE_COLOR};
        int color_index = (ghost->color == 'R') ? 0 : (ghost->color == 'P') ? 1 : (ghost->color == 'G') ? 2 : 3;
        drawEntity(x, y, ghost_str, colors[color_index]);
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

// void render(const Pacman* pacman, int score) {
//     clear();
//    
//     if(game_complete){
//         renderGameComplete(pacman, score);
//     } else if (game_over){
//         renderGameOver(pacman, score);
//     } else {
//         renderGameplayScreen(pacman, score);
//     }
//     if(debug_mode){
//         // 디버그 모드 상태 표시
//         renderDebugInfo(pacman);
//     }
//
// }

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
    const char* ghost_names[] = {"RED", "PINK", "GREEN", "ORANGE"};
    const char* ghost_colors[] = {GHOST_RED_COLOR, GHOST_PINK_COLOR, GHOST_GREEN_COLOR, GHOST_ORANGE_COLOR};
    const char* state_names[] = {"CHASING", "FRIGHTENED", "RETURNING", "EXITING", "WAITING", "UNKNOWN"};
    
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
    sprintf(buffer, "RELEASED: %d/%d", MAX_GHOSTS - queue_count, MAX_GHOSTS);
    drawEntity(state_x / 2, base_y + 4, buffer, ANSI_YELLOW);
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

    moveGhost(ghost);
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

void updateEatenGhost(Ghost* ghost){
    ghost->target_x = GHOST_ZONE_X;
    ghost->target_y = GHOST_ZONE_Y;

    decideGhostDirectionToTarget(ghost, ghost->target_x, ghost->target_y);
    moveGhost(ghost);

    if(ghost->x == GHOST_ZONE_X && ghost->y == GHOST_ZONE_Y){
        ghost->state = WAITING;
        ghost->direction = DIR_NONE;
        enqueueGhost(ghost);
    }
}

void updateFrightenedGhost(Ghost* ghost){
    DirectionResult result = findPossilbeDirections(ghost, -1, -1, 1);
    
    ghost->target_x = -1;
    ghost->target_y = -1;
    
    // 고스트 방향 결정
    if(result.count > 0){
        if(result.count >= 2) {  // T자형 이상 교차로
            // 70% 확률로 직진, 30% 확률로 랜덤
            if(rand() % 10 < 7) {
                // 현재 방향이 가능한지 확인
                int can_continue = 0;
                for(int i = 0; i < result.count; i++) {
                    if(result.directions[i] == ghost->direction) {
                        can_continue = 1;
                        break;
                    }
                }
                
                if(can_continue) {
                    // 직진 유지
                    // ghost->direction 그대로
                } else {
                    // 직진 불가능하면 랜덤 선택
                    int rand_index = rand() % result.count;
                    ghost->direction = result.directions[rand_index];
                }
            } else {
                // 30% 확률로 랜덤 선택
                int rand_index = rand() % result.count;
                ghost->direction = result.directions[rand_index];
            }
        } else {  // count == 1 (직선 또는 코너)
            ghost->direction = result.directions[0];
        }
    } else if(result.opposite_direction != DIR_NONE){
        ghost->direction = result.opposite_direction;
    }

    // 디버그용 타겟 설정
    int next_x = ghost->x, next_y = ghost->y;
    getNextPosition(&next_x, &next_y, ghost->direction);
    ghost->target_x = next_x;
    ghost->target_y = next_y;

    moveGhost(ghost);
}

void updateAllGhost(const Pacman* pacman){

    if(power_mode && power_mode_timer > 0){
        power_mode_timer--;
        // 파워모드 시간이 감소해서 0초가 되면
        if(power_mode_timer <= 0){
            // 파워모드 종료
            power_mode = 0;
            // 겁먹은 유령을 전부 CHASING으로 다시 변경
            for(int i = 0; i < MAX_GHOSTS; i++){
                if(ghosts[i].state == FRIGHTENED){
                    ghosts[i].state = CHASING;
                }
            }
        }
    }

    for(int i = 0; i < MAX_GHOSTS; i++){
        switch(ghosts[i].state){
            case WAITING:
                // 대기중인 유령은 움직이지 않음
                break;
            case RETURNING:
                updateEatenGhost(&ghosts[i]);
                break;
            case EXITING:
                updateExitingGhost(&ghosts[i]);
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
                updateFrightenedGhost(&ghosts[i]);
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

    DirectionResult result = findPossilbeDirections(ghost, pacman->x, pacman->y, 1);

    if(result.best_direction != DIR_NONE){
        ghost->direction = result.best_direction;
    } else if(result.count > 0){
        ghost->direction = result.directions[0];
    } else {
        ghost->direction = result.opposite_direction;
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

    DirectionResult result = findPossilbeDirections(ghost, target_x, target_y, 1);

    if(result.best_direction != DIR_NONE){
        ghost->direction = result.best_direction;
    } else if(result.count > 0){
        ghost->direction = result.directions[0];
    } else {
        ghost->direction = result.opposite_direction;
    }

    // int possible_directions[4] = {0, 0, 0, 0}; // UP, DOWN, LEFT, RIGHT
    // int direction_count = 0;

    // int direction_to_check[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    // int opposite_direction = 0;

    // int best_direction = 0;
    // int min_distance = INT_MAX;

    // switch(ghost->direction){
    //     case DIR_UP: opposite_direction = DIR_DOWN; break;
    //     case DIR_DOWN: opposite_direction = DIR_UP; break;
    //     case DIR_LEFT: opposite_direction = DIR_RIGHT; break;
    //     case DIR_RIGHT: opposite_direction = DIR_LEFT; break;
    //     default: opposite_direction = DIR_NONE; break;
    // }

    // // 4방향 탐색
    // for(int i = 0; i < 4; i++){
    //     int next_x = ghost->x;
    //     int next_y = ghost->y;
    //     getNextPosition(&next_x, &next_y, direction_to_check[i]);

    //     if (map[next_y][next_x] != WALL && map[next_y][next_x] != GHOST_DOOR && direction_to_check[i] != opposite_direction){
    //         int distance = abs(next_x - target_x) + abs(next_y - target_y);
    //         if(distance < min_distance){
    //             min_distance = distance;
    //             best_direction = direction_to_check[i];
    //         }
    //     }
    // }

    // if(best_direction == 0){
    //     ghost->direction = opposite_direction;
    // } else {
    //     ghost->direction = best_direction;
    // }

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

    int tile = current_map[y][x];

    switch(state){
        case RETURNING:
        case EXITING:
            return (tile != WALL && tile != WARP_ZONE);
        case CHASING:
        case FRIGHTENED:
            return (tile != WALL && tile != GHOST_DOOR);
        default:
            return (tile != WALL);
    }
}

void moveGhost(Ghost* ghost){
    int next_x = ghost->x;
    int next_y = ghost->y;

    getNextPosition(&next_x, &next_y, ghost->direction);

    if(canGhostMoveTo(next_x, next_y, ghost->state)){
        ghost->prev_x = ghost->x;
        ghost->prev_y = ghost->y;
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
            && current_map[test_y][test_x] != GHOST_DOOR
        ){
                *future_x = test_x;
                *future_y = test_y;
        } else {
            break;
        }
    }
}

// 플레이어 관련 함수들
void updatePacman(Pacman* pacman){
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

    if(current_map[next_y][next_x] != WALL){
        
        // 워프존 확인
        if(current_map[next_y][next_x] == WARP_ZONE){
            if(next_x == 0){ // 왼쪽 워프존
                pacman->x = MAP_WIDTH - 1;
            } else if(next_x == MAP_WIDTH - 1){ // 오른쪽 워프존
                pacman->x = 1;
            }

        } else if(current_map[next_y][next_x] == COOKIE){
            current_map[next_y][next_x] = EMPTY;
            cookies_eaten++;
            score += 10; // 점수 증가
            pacman->x = next_x;
            pacman->y = next_y;

        } else if(current_map[next_y][next_x] == POWER_COOKIE){
            current_map[next_y][next_x] = EMPTY;
            cookies_eaten++;
            score += 50; // 점수 증가
            activatePowerMode();
            pacman->x = next_x;
            pacman->y = next_y;

        } else if(current_map[next_y][next_x] != GHOST_DOOR && current_map[next_y][next_x] != WALL){
            // 일반 이동
            pacman->x = next_x;
            pacman->y = next_y;
        }
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
                    if(pacman->y - 1 < 0 || current_map[pacman->y - 1][pacman->x] == WALL){
                        return;
                    }
                    pacman->direction = DIR_UP; 
                    break;
                }
                case 80: {
                    if(pacman->y + 1 >= MAP_HEIGHT || current_map[pacman->y + 1][pacman->x] == WALL){
                        return;
                    }
                    pacman->direction = DIR_DOWN;
                    break;
                }
                case 75: {
                    if(pacman->x - 1 < 0 || current_map[pacman->y][pacman->x - 1] == WALL){
                        return;
                    }
                    pacman->direction = DIR_LEFT;
                    break;
                }
                case 77: {
                    if(current_map[pacman->y][pacman->x + 1] == WALL){
                        return;
                    }
                    pacman->direction = DIR_RIGHT;
                    break;
                }
            }
        }
    }
}

void activatePowerMode() {
    power_mode = 1;
    power_mode_timer = POWER_MODE_DURATION;

    for(int i = 0; i < MAX_GHOSTS; i++){
        if(ghosts[i].state == CHASING){
            ghosts[i].state = FRIGHTENED;

            // 즉시 방향 반전 (실제 팩맨 게임처럼)
            switch(ghosts[i].direction) {
                case DIR_UP: ghosts[i].direction = DIR_DOWN; break;
                case DIR_DOWN: ghosts[i].direction = DIR_UP; break;
                case DIR_LEFT: ghosts[i].direction = DIR_RIGHT; break;
                case DIR_RIGHT: ghosts[i].direction = DIR_LEFT; break;
                default: break;
            }
        }
    }
}

// 게임 상태 관련 함수들
void resetGame(Pacman* pacman){
    current_stage = 1;
    score = 0;
    game_time = 0;
    cookies_eaten = 0;
    ghost_released = 0;
    power_mode = 0;
    power_mode_timer = 0;
    *pacman = (Pacman){13, 23, 13, 23, DIR_NONE, 3};

    current_stage = 2;
    restoreMap(map_stage2);
    
    queue_count = 0;
    queue_front = 0;
    queue_rear = 0;

    initializeGhosts();
}

void nextStage(Pacman* pacman){
    current_stage++; // 스테이지 증가

    // 스테이지 클리어 보너스
    score += current_stage * 1000;
    score += pacman->lives * 300;

    // 스테이지 난이도 조절(더 어렵게)
    // for(int i = 0; i < MAX_GHOSTS; i++){
    //     release_conditions[i].cookies_required = max(0, release_conditions[i].cookies_required - (stage * 5));
    // }

    int next_stage_index = (current_stage - 1 ) % MAX_STAGES;

    restoreMap(all_maps[next_stage_index]);

    total_cookies = getTotalCookies();

    cookies_eaten = 0;
    ghost_released = 0;
    power_mode = 0;
    power_mode_timer = 0;

    current_state = STATE_READY;

    // Pacman 초기화
    *pacman = (Pacman){13, 23, 13, 23, DIR_NONE, pacman->lives};

    queue_count = 0;
    queue_front = 0;
    queue_rear = 0;
    initializeGhosts();
}

void adjustDifficulty() {
    // 난이도 조절
}
// 메인 함수
int main() {
    srand((unsigned int)time(NULL));
    
    initialize();
    
    Pacman pacman; // 선언만 하고 초기화는 resetGame에서

    current_state = STATE_TITLE; // 게임 시작 상태를 TITLE로 설정

    while(1){
        // 1. 상태에 따른 입력 처리 (Update)
        switch(current_state) {
            case STATE_TITLE:
                // 아무 키나 누르면 게임 시작 (READY 상태로 변경)
                if (_kbhit()) {
                    _getch(); // 키 버퍼 비우기
                    current_state = STATE_READY;
                    resetGame(&pacman); // 새 게임 시작
                }
                break;

            case STATE_READY:
                // "Ready!" 메시지를 보여주고 잠시 대기 후 PLAYING으로 전환
                // 여기서는 간단하게 바로 전환. Sleep()을 render 후에 주므로 괜찮음.
                current_state = STATE_PLAYING;
                break;
                
            case STATE_PLAYING:
                // 기존의 게임 로직을 여기에 배치
                processInput(&pacman);
                updatePacman(&pacman);

                if (isGameComplete()) {
                    current_state = STATE_LEVEL_COMPLETE;
                    break; // 상태가 바뀌었으므로 즉시 루프 탈출
                }

                // 고스트 릴리즈 로직
                if (ghost_released < MAX_GHOSTS && queue_count > 0) {
                    GhostReleaseCondition conditions = release_conditions[ghost_released];
                    if (cookies_eaten >= conditions.cookies_required ||
                        score >= conditions.score_required ||
                        game_time >= conditions.time_required) {
                        dequeueGhost();
                        ghost_released++;
                    }
                }
                
                updateAllGhost(&pacman);
                handleCollisions(&pacman, &score);
                game_time++;
                break;

            case STATE_PACMAN_DEATH:
                // 팩맨 사망 후 잠시 대기 상태 (애니메이션 등을 위해)
                // 현재는 바로 다음 상태로 가지만, 타이머를 추가할 수 있음
                if (pacman.lives > 0) {
                    current_state = STATE_READY; // 목숨 남았으면 다시 준비
                } else {
                    current_state = STATE_GAME_OVER; // 목숨 없으면 게임 오버
                }
                break;

            case STATE_LEVEL_COMPLETE:
                // 레벨 클리어 후 잠시 대기 상태
                // 현재는 바로 다음 스테이지로 넘어가지만, 타이머를 추가할 수 있음
                if(_kbhit()){
                    int key = _getch();
                    if(key == 'n' || key == 'N'){
                        nextStage(&pacman);
                    } else if(key == 27){
                        current_state = STATE_TITLE;
                    }
                }
                break;
            case STATE_GAME_OVER:
                // 재시작 또는 종료 입력 처리
                if (_kbhit()) {
                    int key = _getch();
                    if (key == 'r' || key == 'R') {
                        current_state = STATE_READY;
                        resetGame(&pacman);
                    }
                    else if (key == 27) { // ESC 누르면 TITLE로
                        current_state = STATE_TITLE;
                    }
                }
                break;
        }

        // 2. 상태에 따른 화면 그리기 (Render)
        clear();
        switch(current_state) {
            case STATE_TITLE:
                // renderTitleScreen(); // 타이틀 화면 렌더링 함수 (새로 만들어야 함)
                drawEntity(MAP_WIDTH/2 - 5, MAP_HEIGHT/2, "PACKMAN", PACMAN_COLOR);
                drawEntity(MAP_WIDTH/2 - 8, MAP_HEIGHT/2 + 2, "Press Any Key...", ANSI_WHITE);
                break;

            case STATE_READY:
                renderGameplayScreen(&pacman, score);
                drawEntity(MAP_WIDTH/2 - 2, MAP_HEIGHT/2 + 2, "Ready!", ANSI_YELLOW);
                break;

            case STATE_PLAYING:
                renderGameplayScreen(&pacman, score);
                break;

            case STATE_PACMAN_DEATH:
                 renderGameplayScreen(&pacman, score); // 팩맨이 사라진 화면
                 // 여기에 사망 애니메이션 렌더링 가능
                break;
            
            case STATE_LEVEL_COMPLETE:
                renderGameComplete(&pacman, score);
                break;

            case STATE_GAME_OVER:
                renderGameOver(&pacman, score);
                break;
        }
        
        if (debug_mode) {
            renderDebugInfo(&pacman);
        }

        flip();
        
        // 상태별로 다른 대기 시간 적용 가능
        if (current_state == STATE_READY || current_state == STATE_PACMAN_DEATH || current_state == STATE_LEVEL_COMPLETE) {
            Sleep(1500); // 준비 또는 사망 후 1.5초 대기
        } else {
            Sleep(300);
        }
    }

exit_game_loop:
    releaseScreen();
    return 0;
}