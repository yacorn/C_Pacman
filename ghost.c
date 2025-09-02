// ghost.c
#include "packman.h"
#include <time.h>
#include <limits.h>

void initializeGhosts() {
    // 빨간 GHOST
    ghosts[0] = (Ghost){13, 11, 12, 14, DIR_NONE, CHASING, 'R', 0, -1, -1};

    // 분홍 유령
    ghosts[1] = (Ghost){13, 14, 13, 14, DIR_NONE, WAITING, 'P', 0, -1, -1};

    // 청록 유령
    ghosts[2] = (Ghost){14, 14, 14, 14, DIR_NONE, WAITING, 'G', 0, -1, -1};

    // 주황 유령
    ghosts[3] = (Ghost){15, 14, 15, 14, DIR_NONE, WAITING, 'O', 0, -1, -1};

    // 큐를 한번 싹다 비우고 추가
    clearGhostQueue();

    for(int i = 1; i < MAX_GHOSTS; i++){
        enqueueGhost(&ghosts[i]);
    }
}

void updateAllGhost(const Pacman* pacman){

    if(power_mode && power_mode_timer > 0){
        power_mode_timer--;
        // 파워모드 시간이 감소해서 0초가 되면
        if(power_mode_timer <= 0){
            // 파워모드 종료
            power_mode = 0;

            stopSoundMci("power_up");

            is_bgm_playing = 0;

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

    // 경계 체크 및 보정 추가!
    if(ghost->target_x < 0) {
        ghost->target_x = 0;
    } else if(ghost->target_x >= MAP_WIDTH) {
        ghost->target_x = MAP_WIDTH - 1;
    }
    
    if(ghost->target_y < 0) {
        ghost->target_y = 0;
    } else if(ghost->target_y >= MAP_HEIGHT) {
        ghost->target_y = MAP_HEIGHT - 1;
    }

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

        // 유령이 돌아가는 소리를 멈춘다
        stopSoundMci("loop_sfx");

        if(power_mode){
            playSoundMci("sounds/power_up.wav", "power_up", 1);
        }
        enqueueGhost(ghost);
        ghost_released--;
    }
}

void updateFrightenedGhost(Ghost* ghost){
    DirectionResult result = findPossibleDirections(ghost, -1, -1, 1);
    
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

    DirectionResult result = findPossibleDirections(ghost, pacman->x, pacman->y, 1);

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
    // 플로우 필드 생성 (매 호출마다 새로 생성)
    static FlowCell flow_field[MAP_HEIGHT][MAP_WIDTH];
    generateFlowField(target_x, target_y, ghost->state, flow_field);
    
    // 현재 위치에서 플로우 필드 방향 가져오기
    Direction flow_direction = getFlowFieldDirection(ghost->x, ghost->y, flow_field);
    
    if(flow_direction != DIR_NONE) {
        // 플로우 필드 방향으로 갈 수 있는지 확인
        int next_x = ghost->x, next_y = ghost->y;
        getNextPosition(&next_x, &next_y, flow_direction);
        
        if(canGhostMoveTo(next_x, next_y, ghost->state)) {
            ghost->direction = flow_direction;
            
            if(debug_mode) {
                debug_log("Ghost %c at (%d,%d) -> target (%d,%d): direction %d\n", 
                         ghost->color, ghost->x, ghost->y, target_x, target_y, flow_direction);
            }
            return;
        }
    }
    
    // 플로우 필드가 실패하면 기존 방식 사용
    DirectionResult result = findPossibleDirections(ghost, target_x, target_y, 1);

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

DirectionResult findPossibleDirections(const Ghost* ghost, int target_x, int target_y, int exclude_opposite){
    DirectionResult result = {0};
    result.best_direction = DIR_NONE;
    result.best_distance = INT_MAX;
    result.opposite_direction = DIR_NONE;
    if(exclude_opposite){
        switch(ghost->direction){
            case DIR_UP: result.opposite_direction = DIR_DOWN; break;
            case DIR_DOWN: result.opposite_direction = DIR_UP; break;
            case DIR_LEFT: result.opposite_direction = DIR_RIGHT; break;
            case DIR_RIGHT: result.opposite_direction = DIR_LEFT; break;
        }
    }

    Direction check_directions[] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};

    for(int i = 0; i < 4; i++){
        if(exclude_opposite && check_directions[i] == result.opposite_direction) continue;

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

// 플로우 필드 생성 함수
void generateFlowField(int target_x, int target_y, GhostState ghost_state, FlowCell flow_field[MAP_HEIGHT][MAP_WIDTH]) {
    // 성능 측정용 (디버그 모드일 때만)
    clock_t start_time = 0;
    if(debug_mode) {
        start_time = clock();
    }
    
    // 초기화
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            flow_field[y][x].direction = DIR_NONE;
            flow_field[y][x].distance = INT_MAX;
            flow_field[y][x].valid = 0;
        }
    }
    
    // BFS 자료구조
    int queue_x[MAP_WIDTH * MAP_HEIGHT], queue_y[MAP_WIDTH * MAP_HEIGHT];
    int front = 0, rear = 0;
    
    // 목표점에서 시작 (역방향 BFS)
    queue_x[rear] = target_x;
    queue_y[rear] = target_y;
    rear++;
    
    flow_field[target_y][target_x].distance = 0;
    flow_field[target_y][target_x].valid = 1;
    flow_field[target_y][target_x].direction = DIR_NONE; // 목표점은 방향 없음
    
    Direction dirs[] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    Direction reverse_dirs[] = {DIR_DOWN, DIR_UP, DIR_RIGHT, DIR_LEFT};
    
    while(front < rear) {
        int curr_x = queue_x[front];
        int curr_y = queue_y[front];
        front++;
        
        for(int i = 0; i < 4; i++) {
            int new_x = curr_x, new_y = curr_y;
            getNextPosition(&new_x, &new_y, dirs[i]);
            
            // 맵 범위 체크
            if(new_x < 0 || new_x >= MAP_WIDTH || new_y < 0 || new_y >= MAP_HEIGHT) {
                continue;
            }
            
            // 갈 수 있는 곳이고 아직 방문하지 않았으면
            if(canGhostMoveTo(new_x, new_y, ghost_state) && !flow_field[new_y][new_x].valid) {
                flow_field[new_y][new_x].valid = 1;
                flow_field[new_y][new_x].distance = flow_field[curr_y][curr_x].distance + 1;
                flow_field[new_y][new_x].direction = reverse_dirs[i]; // 목표로 가는 방향
                
                queue_x[rear] = new_x;
                queue_y[rear] = new_y;
                rear++;
            }
        }
    }
    
    // 성능 측정 결과 출력
    if(debug_mode) {
        clock_t end_time = clock();
        double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        debug_log("Flow field generation for target (%d,%d): %.4f seconds, processed %d cells\n", 
                 target_x, target_y, time_taken, rear);
    }
}

// 플로우 필드에서 방향 가져오기
Direction getFlowFieldDirection(int x, int y, FlowCell flow_field[MAP_HEIGHT][MAP_WIDTH]) {
    if(x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return DIR_NONE;
    }
    
    if(!flow_field[y][x].valid) {
        return DIR_NONE;
    }
    
    return flow_field[y][x].direction;
}

// ... 이 외 ghost.c에 속하기로 한 모든 유령 관련 함수의 구현 ...