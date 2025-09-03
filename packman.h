// packman.h

#ifndef PACKMAN_H
#define PACKMAN_H

#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "colors.h" // colors.h가 있다면 포함

// 맵 요소 정의
#define EMPTY 0
#define WALL 1
#define COOKIE 2
#define POWER_COOKIE 3
#define GHOST_DOOR 4
#define GHOST_ZONE 5
#define WARP_ZONE 6

#define MAX_GHOSTS 4

#define ORANGE_GHOST_CHASE_DISTANCE 64
#define POWER_MODE_DURATION 50
#define MAP_WIDTH 29
#define MAP_HEIGHT 31

// --- 자료 구조 정의 ---
typedef enum { STATE_TITLE, STATE_HELP, STATE_READY, STATE_PLAYING, STATE_PACMAN_DEATH, STATE_LEVEL_COMPLETE, STATE_GAME_OVER, STATE_CUTSCENE, STATE_ALL_CLEAR } GameState;
typedef enum { DIR_NONE = 0, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT } Direction;
typedef enum { COLLISION_NONE = 0, COLLISION_PACMAN_DIES, COLLISION_GHOST_EATEN, COLLISION_IGNORED } CollisionResult;

// 타이틀 메뉴 관련
typedef enum { MENU_START_GAME = 0, MENU_HOW_TO_PLAY, MENU_EXIT_GAME, MENU_COUNT } MenuOption;

typedef struct {
    int x, y, prev_x, prev_y;
    Direction direction;
    int lives;
} Pacman;

typedef enum { CHASING, FRIGHTENED, EXITING, WAITING, RETURNING } GhostState;
typedef struct {
    int x, y, prev_x, prev_y;
    int direction;
    GhostState state;
    char color;
    int target_x, target_y;
} Ghost;

typedef struct {
    int cookies_required, score_required, time_required;
} GhostReleaseCondition;

typedef struct {
    Direction directions[4];
    Direction best_direction;
    Direction opposite_direction;
    int count;
    int best_distance;
} DirectionResult;

typedef struct {
    int x, y;
    int active;
    int timer;
    int score;
} Fruit;

// --- 전역 변수 선언 (extern) ---
extern HANDLE screen[2];
extern int screen_index;
extern int game_time;
extern int score;
extern int power_mode;
extern int power_mode_timer;
extern int total_cookies;
extern int cookies_eaten;
extern int ghost_released;
extern int current_stage;
extern int debug_mode;
extern int is_bgm_playing;
extern int is_first_start;
extern int current_siren_level;
extern int ghost_release_timer;
extern int ghost_release_interval;
extern int ready_delay_timer;
extern int queue_front, queue_rear, queue_count;

extern int power_music_active;
extern int ghost_back_active;

// 타이틀 메뉴 관련 변수
extern MenuOption current_menu_selection;

extern Ghost ghosts[MAX_GHOSTS];
extern Ghost* ghostQueue[MAX_GHOSTS];
extern GameState current_state;
extern GhostReleaseCondition release_conditions[];
extern Fruit bonus_fruit;

// --- 함수 원형 선언 ---

// game.c
void initialize();
void resetGame(Pacman* pacman);
void nextStage(Pacman* pacman);
void adjustDifficulty();
void handleCollisions(Pacman* pacman, int* score);
void handlePacmanDeath(Pacman* pacman);
void handleGhostEaten(Ghost* ghost, int* score);
int isLevelComplete();
CollisionResult checkCollision(const Pacman* pacman, const Ghost* ghost);
int isLevelComplete();
void enqueueGhost(Ghost* ghost);
Ghost* frontGhost();
void dequeueGhost();
void clearGhostQueue();
void spawnBonusFruit(const Pacman* pacman);
void updateBonusFruit();
void updateBackGroundMusic();
void handleInput(Pacman* pacman);
void handleRender(Pacman* pacman);
void handleSound();
void stopAllGameSounds();
int getDelayTime();

// render.c
void hideCursor();
void clear();
void flip();
void releaseScreen();
void drawEntity(int x, int y, const char* str, const char* color);
void renderGameplayScreen(const Pacman* pacman, int score);
void renderGameComplete(const Pacman* pacman, int score);
void renderGameOver(const Pacman* pacman, int score);
void renderDebugInfo(const Pacman* pacman);
void renderGhost(const Ghost* ghost, int x, int y);
void renderTarget(char ghost_color, int x, int y);
void renderMapTile(int tile, int x, int y);
void renderAllClear(const Pacman* pacman, int score);
void drawEntity(int x, int y, const char* str, const char* color);
void drawScore(int score, int lives);
void drawGhostDebugInfo();
void drawPacmanDebugInfo(const Pacman* pacman);
void renderTitleScreen();
void renderHelpScreen();
void drawGameStateInfo();
void drawMapInfo();

// pacman.c
void updatePacman(Pacman* pacman);
void processInput(Pacman* pacman);
void activatePowerMode();

// ghost.c
void initializeGhosts();
void updateExitingGhost(Ghost* ghost);
void updateRedGhost(Ghost* ghost, const Pacman* pacman);
void updatePinkGhost(Ghost* ghost, const Pacman* pacman);
void updateGreenGhost(Ghost* ghost, const Pacman* pacman);
void updateOrangeGhost(Ghost* ghost, const Pacman* pacman);
void updateEatenGhost(Ghost* ghost);
void updateFrightenedGhost(Ghost* ghost);
void updateAllGhost(const Pacman* pacman);
void getNextPosition(int* x, int* y, Direction dir);
void decideGhostDirection(Ghost* ghost, const Pacman* pacman);
void decideGhostDirectionToTarget(Ghost* ghost, int target_x, int target_y);
int canGhostMoveTo(int x, int y, GhostState state);
void moveGhost(Ghost* ghost);
void findPacmanFuturePosition(const Pacman* pacman, int* future_x, int* future_y, int look_ahead);
DirectionResult findPossibleDirections(const Ghost* ghost, int target_x, int target_y, int exclude_opposite);

// Flow Field
typedef struct {
    Direction direction;
    int distance;
    int valid;
} FlowCell;

void generateFlowField(int target_x, int target_y, GhostState ghost_state, FlowCell flow_field[MAP_HEIGHT][MAP_WIDTH]);
Direction getFlowFieldDirection(int x, int y, FlowCell flow_field[MAP_HEIGHT][MAP_WIDTH]);

// sound.c
void playSoundMci(const char* filename, const char* alias, int loop);
void playSoundAndWait(const char* filename, const char* alias);
void stopSoundMci(const char* alias);
void debug_log(const char* format, ...);
int isSoundFinished(const char* alias);

#endif // PACKMAN_H