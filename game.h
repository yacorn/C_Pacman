#ifndef GAME_H
#define GAME_H

#include "common.h"
#include "pacman.h"
#include "ghost.h"

// 보너스 과일 구조체
typedef struct {
    int x, y;
    int active;
    double timer;
    int score;
    int type;
} Fruit;

// 게임 관련 변수 (extern)
extern int game_time;
extern int score;
extern int power_mode;
extern double power_mode_timer;
extern int total_cookies;
extern int cookies_eaten;
extern int current_stage;
extern int debug_mode;
extern int is_first_start;
extern MenuOption current_menu_selection;
extern GameState current_state;
extern Fruit bonus_fruit;

// 게임 관련 함수
void initialize();
void resetGame(Pacman* pacman);
void nextStage(Pacman* pacman);
void adjustDifficulty();
void handleCollisions(Pacman* pacman, int* score);
void handlePacmanDeath(Pacman* pacman);
void handleGhostEaten(Ghost* ghost, int* score);
int isLevelComplete();
CollisionResult checkCollision(const Pacman* pacman, const Ghost* ghost);
void enqueueGhost(Ghost* ghost);
Ghost* frontGhost();
void dequeueGhost();
void clearGhostQueue();
void spawnBonusFruit(const Pacman* pacman);
void updateBonusFruit();
void updateBackGroundMusic();
void handleInput(Pacman* pacman);
void handleLogic(Pacman* pacman);  // 새로 추가된 함수
void handleRender(Pacman* pacman);
void handleSound();
void stopAllGameSounds();
// int getDelayTime();
int getFruitScore(FruitTypes type);

// Delta Time 관리 함수들
void updateDeltaTime();
// void updateGameLogicFixed(Pacman* pacman);
void updateTimersRealTime(double dt);
void updatePowerMode(double dt);
void updateReadyState(double dt);
void updateGhostReleaseState(double dt);
void updateBonusFruitState(double dt);
double getDeltaTime();
int getCurrentFPS();

#endif // GAME_H