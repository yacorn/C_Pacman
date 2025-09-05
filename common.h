#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "colors.h"

// 맵 요소 정의
#define EMPTY 0
#define WALL 1
#define COOKIE 2
#define POWER_COOKIE 3
#define GHOST_DOOR 4
#define GHOST_ZONE 5
#define WARP_ZONE 6
#define VOID_ZONE 9

#define MAX_GHOSTS 4
#define MAP_WIDTH 29
#define MAP_HEIGHT 31

// 게임 상수들
#define BONUS_FRUIT_TRIGGER_COOKIES 70
#define SIREN_LEVEL_CHANGE_THRESHOLD 50
#define MAX_SPAWN_ATTEMPTS 100
#define FRUIT_MIN_DURATION 8
#define FRUIT_MAX_DURATION 15
#define POWER_MODE_DURATION 15
#define READY_DELAY_TIME 3
#define GHOST_RELEASE_INTERVAL 10

// 공통 열거형
typedef enum { 
    STATE_TITLE, STATE_HELP, STATE_HIGHSCORE, STATE_READY, STATE_PLAYING, 
    STATE_GAME_PAUSE, STATE_PACMAN_DEATH, STATE_LEVEL_COMPLETE, STATE_GAME_OVER, 
    STATE_CUTSCENE, STATE_ALL_CLEAR 
} GameState;

typedef struct {
    int x, y;
} Position;

typedef enum { 
    DIR_NONE = 0, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT 
} Direction;

typedef enum { 
    COLLISION_NONE = 0, COLLISION_PACMAN_DIES, 
    COLLISION_GHOST_EATEN, COLLISION_IGNORED 
} CollisionResult;

typedef enum { 
    MENU_START_GAME = 0, MENU_HOW_TO_PLAY, MENU_HIGH_SCORE,
    MENU_EXIT_GAME, MENU_COUNT 
} MenuOption;

typedef enum {
    PAUSE_MENU_RESUME = 0, PAUSE_MENU_QUIT, PAUSE_MENU_COUNT
} PauseMenuOption;

typedef enum {
    SCORE_COOKIE = 10,
    SCORE_POWER_COOKIE = 50,
    GHOST_COMBO_BASE_SCORE = 200,
    SCORE_CHERRY = 100,
    SCORE_STRAWBERRY = 300,
    SCORE_ORANGE = 500,
    SCORE_APPLE = 700,
    SCORE_MELON = 1000,
    SCORE_BONUS_LIFE = 300,
    SCORE_STAGE_CLEAR = 1000,
    SCORE_LEFT_LIFE_BONUS = 1000,
} ScoreValues;

typedef enum {
    FRUIT_CHERRY = 0,
    FRUIT_STRAWBERRY,
    FRUIT_ORANGE,
    FRUIT_APPLE,
    FRUIT_MELON,
    FRUIT_COUNT
} FruitTypes;

// 디버그 로깅 함수
void debug_log(const char* format, ...);

#endif // COMMON_H