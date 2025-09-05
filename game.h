#ifndef GAME_H
#define GAME_H

#include "common.h"
#include "pacman.h"
#include "ghost.h"
#include "highscore.h"

// 보너스 과일 구조체
typedef struct {
    int x, y;
    int active;
    double timer;
    int score;
    int type;
} Fruit;

// 게임 관련 변수 (extern)
// extern int game_time;
// extern int score;
// extern int power_mode;
// extern double power_mode_timer;
// extern int total_cookies;
// extern int cookies_eaten;
// extern int current_stage;
extern int debug_mode;
// extern int is_first_start;
extern MenuOption current_menu_selection;
extern PauseMenuOption current_pause_menu_selection;
extern GameState current_state;
extern Fruit bonus_fruit;
extern HighScoreEntry new_highscore_entry;

// ========================================
// 📊 CORE GAME STATISTICS (캡슐화)
// ========================================

// 현재 점수 반환 함수
int getScore();
void addScore(int points);
int getCurrentStage();
GameState getCurrentGameState();
void setGameState(GameState state);
int isPowerModeActive();
void setPowerMode(int mode);
double getPowerModeTimer();
void setPowerModeTimer(double timer);
int getHighScoreEntryActive();
const char* getHighScoreEntryNickname();
void setDebugMode(int mode);
void resetDebugMode();
void setGamePaused(int paused);
int isGamePaused();


// 게임 관련 함수
void initialize();
void initializeBonusFruit();
void initializeNewHighScoreEntry();
void initializePacman(Pacman* pacman, int lives);
void resetGame(Pacman* pacman);
void nextStage(Pacman* pacman);
void adjustDifficulty();
void handleCollisions(Pacman* pacman, int* score);
int isLevelComplete();
CollisionResult checkCollision(const Pacman* pacman, const Ghost* ghost);
void spawnBonusFruit(const Pacman* pacman);
void updateBonusFruit();
void handleInput(Pacman* pacman);
void handleGameOverInput(Pacman* pacman);
void handleNormalGameOverInput(Pacman* pacman);
void handleGamePauseInput(Pacman* pacman);
void handleGameTitleInput(Pacman* pacman);
void handleHighScoreInput();
void startHighScoreEntry();
void finishHighScoreEntry();
void handleLogic(Pacman* pacman);
// int getDelayTime();
int getFruitScore(FruitTypes type);
int isFirstStart();

// Delta Time 관리 함수들
void updateDeltaTime();
// void updateGameLogicFixed(Pacman* pacman);
void updateTimersRealTime(double dt);
void updatePowerMode(double dt);
void updateReadyState(double dt);
void resetReadyTimer();
void updateGhostReleaseState(double dt);
void updateBonusFruitState(double dt);
double getDeltaTime();
int getCurrentFPS();

#endif // GAME_H