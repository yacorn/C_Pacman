#ifndef GHOST_H
#define GHOST_H

#include "common.h"
#include "pacman.h"

#define ORANGE_GHOST_CHASE_DISTANCE 64
#define PINK_GHOST_LOOK_AHEAD 4

// 고스트 스폰 좌표
typedef enum {
    // Red Ghost
    RED_GHOST_SPAWN_X = 14,
    RED_GHOST_SPAWN_Y = 15,

    // Pink Ghost
    PINK_GHOST_SPAWN_X = 13,
    PINK_GHOST_SPAWN_Y = 14,

    // Orange Ghost
    ORANGE_GHOST_SPAWN_X = 15,
    ORANGE_GHOST_SPAWN_Y = 14,

    // Green Ghost
    GREEN_GHOST_SPAWN_X = 14,
    GREEN_GHOST_SPAWN_Y = 14

} GhostSpawnCoordinates;

// 고스트 상태
typedef enum { 
    CHASING, FRIGHTENED, EXITING, WAITING, RETURNING 
} GhostState;

// 고스트 구조체
typedef struct {
    int x, y, prev_x, prev_y;
    int direction;
    GhostState state;
    char color;
    int target_x, target_y;
} Ghost;

// 고스트 릴리즈 조건
typedef struct {
    int cookies_required, score_required, time_required;
} GhostReleaseCondition;

// 방향 탐색 결과
typedef struct {
    Direction directions[4];
    Direction best_direction;
    Direction opposite_direction;
    int count;
    int best_distance;
} DirectionResult;

// Flow Field
typedef struct {
    Direction direction;
    int distance;
    int valid;
} FlowCell;

// 고스트 관련 변수 (extern)
extern Ghost ghosts[MAX_GHOSTS];
extern Ghost* ghostQueue[MAX_GHOSTS];
extern GhostReleaseCondition release_conditions[];
extern int ghost_released;
extern double ghost_release_timer;
extern double ghost_release_interval;
extern int queue_front, queue_rear, queue_count;

// 고스트 관련 함수
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
void generateFlowField(int target_x, int target_y, GhostState ghost_state, FlowCell flow_field[MAP_HEIGHT][MAP_WIDTH]);
Direction getFlowFieldDirection(int x, int y, FlowCell flow_field[MAP_HEIGHT][MAP_WIDTH]);

#endif // GHOST_H