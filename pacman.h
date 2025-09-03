#ifndef PACMAN_H
#define PACMAN_H

#include "common.h"

// 팩맨 구조체
typedef struct {
    int x, y, prev_x, prev_y;
    Direction direction;
    int lives;
} Pacman;

// 팩맨 관련 함수
void updatePacman(Pacman* pacman);
void processInput(Pacman* pacman);
void activatePowerMode();
void initializePacman(Pacman* pacman, int lives);

#endif // PACMAN_H