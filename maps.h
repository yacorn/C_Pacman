#ifndef MAPS_H
#define MAPS_H

#include "common.h"

// 맵 관련 상수
#define PACMAN_SPAWN_X 14
#define PACMAN_SPAWN_Y 23
#define GHOST_ZONE_X 14
#define GHOST_ZONE_Y 14
#define GHOST_DOOR_X 14
#define GHOST_DOOR_Y 12
#define BONUS_FRUIT_X 14
#define BONUS_FRUIT_Y 17

// 맵 데이터 선언
extern int map_stage1[MAP_HEIGHT][MAP_WIDTH];
extern int map_stage2[MAP_HEIGHT][MAP_WIDTH];
extern int map_stage3[MAP_HEIGHT][MAP_WIDTH];
extern int map_stage4[MAP_HEIGHT][MAP_WIDTH];
extern int current_map[MAP_HEIGHT][MAP_WIDTH];

// 맵 배열과 관련 변수
extern const int (*all_maps[])[MAP_WIDTH];
extern const int MAX_STAGES;

// 맵 관리 함수들
void initializeMaps(void);
void restoreMap(const int source_map[MAP_HEIGHT][MAP_WIDTH]);
int getCurrentMapRemainingCookies(void);
int getMapCookieCount(const int map[MAP_HEIGHT][MAP_WIDTH]);

int getCurrentMapTileAt(int x, int y);
void setCurrentMapTileAt(int x, int y, int new_tile);

// 쿠키 관련
int getCookiesEaten(void);
void eatCookie(void);
void resetCookiesEaten(void);
void resetTotalCookies(void);
int getTotalCookies(void);
void setTotalCookies(int count);

// 오렌지 고스트 도망 위치
Position getOrangeGhostEscapePosition(int stage);

#endif // MAPS_H