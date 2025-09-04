#ifndef RENDER_H
#define RENDER_H

#include "common.h"
#include "pacman.h"
#include "ghost.h"

// 렌더링 관련 변수 (extern)
// extern HANDLE screen[2];
// extern int screen_index;

// 렌더링 함수
void initializeRenderSystem();
void hideCursor();
void clear();
void flip();
void releaseScreen();
void handleRender(Pacman* pacman);
void drawEntity(int x, int y, const char* str, const char* color);
void renderGameplayScreen(const Pacman* pacman);
void renderGameComplete(const Pacman* pacman);
void renderGameOver(const Pacman* pacman);
void renderDebugInfo(const Pacman* pacman);
void renderGhost(const Ghost* ghost, int x, int y);
void renderTarget(char ghost_color, int x, int y);
void renderMapTile(int tile, int x, int y);
void renderAllClear(const Pacman* pacman);
void renderFruit(FruitTypes type, int x, int y);
void drawScore(int lives);
void drawGhostDebugInfo();
void drawPacmanDebugInfo(const Pacman* pacman);
void renderTitleScreen();
void renderHelpScreen();
void drawGameStateInfo();
void drawMapInfo();

#endif // RENDER_H