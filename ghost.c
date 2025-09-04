// ghost.c
#include "ghost.h"
#include "game.h"
#include "maps.h"
#include "sound.h"
#include <time.h>
#include <limits.h>

static int ghost_released = 1;
static double ghost_release_timer = 0.0;
static double ghost_release_interval = 0.0;
static int ghost_back_active = 0;

Ghost ghosts[MAX_GHOSTS];
Ghost* ghostQueue[MAX_GHOSTS];
int queue_front = 0, queue_rear = 0, queue_count = 0;

void initializeGhosts() {
    // 빨간 GHOST
    ghosts[0] = (Ghost){RED_GHOST_SPAWN_X, RED_GHOST_SPAWN_Y - 4, RED_GHOST_SPAWN_X, RED_GHOST_SPAWN_Y, DIR_NONE, CHASING, 'R', -1, -1};

    // 분홍 유령
    ghosts[1] = (Ghost){PINK_GHOST_SPAWN_X, PINK_GHOST_SPAWN_Y, PINK_GHOST_SPAWN_X, PINK_GHOST_SPAWN_Y, DIR_NONE, WAITING, 'P', -1, -1};

    // 청록 유령
    ghosts[2] = (Ghost){GREEN_GHOST_SPAWN_X, GREEN_GHOST_SPAWN_Y, GREEN_GHOST_SPAWN_X, GREEN_GHOST_SPAWN_Y, DIR_NONE, WAITING, 'G', -1, -1};

    // 주황 유령
    ghosts[3] = (Ghost){ORANGE_GHOST_SPAWN_X, ORANGE_GHOST_SPAWN_Y, ORANGE_GHOST_SPAWN_X, ORANGE_GHOST_SPAWN_Y, DIR_NONE, WAITING, 'O', -1, -1};

    setGhostReleased(1);
    setGhostReleaseTimer(0.0);

    // 큐를 한번 싹다 비우기
    clearGhostQueue();

    for(int i = 1; i < MAX_GHOSTS; i++){
        enqueueGhost(&ghosts[i]);
    }
}

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
        ghost->direction = DIR_NONE;
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

void clearGhostQueue(){
    queue_front = 0;
    queue_rear = 0;
    queue_count = 0;

    for(int i = 0; i < MAX_GHOSTS; i++){
        ghostQueue[i] = NULL;
    }
}

void updateAllGhost(const Pacman* pacman){
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
                    // case 'R':
                    //     updateRedGhost(&ghosts[i], pacman);
                    //     break;
                    // case 'P':
                    //     updatePinkGhost(&ghosts[i], pacman);
                    //     break;
                    // case 'G':
                    //     updateGreenGhost(&ghosts[i], pacman);
                    //     break;
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

    findPacmanFuturePosition(pacman, &target_x, &target_y, PINK_GHOST_LOOK_AHEAD);

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
    static int chase_mode = 1;
    
    // 먼저 팩맨과 고스트의 거리를 구해보자
    int dx = pacman->x - ghost->x;
    int dy = pacman->y - ghost->y;

    int distance = dx * dx + dy * dy;

    // 거리가 8타일(64) 이상이면 추격 모드, 이하면 구석으로 이동
    if(distance >= ORANGE_GHOST_CHASE_DISTANCE && chase_mode == 1){
        ghost->target_x = pacman->x;
        ghost->target_y = pacman->y;

        decideGhostDirectionToTarget(ghost, ghost->target_x, ghost->target_y);
        moveGhost(ghost);

    } else {

        chase_mode = 0;

        if(ghost->x == ghost->target_x && ghost->y == ghost->target_y){

            chase_mode = 1; // 구석에 도달하면 다시 추격 모드로 전환

        } else {
            Position escape_pos = getOrangeGhostEscapePosition(getCurrentStage());
            ghost->target_x = escape_pos.x;
            ghost->target_y = escape_pos.y;
            decideGhostDirectionToTarget(ghost, ghost->target_x, ghost->target_y);
            moveGhost(ghost);
        }
        
    }
}

void updateEatenGhost(Ghost* ghost){
    // 각 고스트의 색깔에 따라 스폰 위치를 타겟으로 설정
    switch(ghost->color){
        case 'R':
            ghost->target_x = RED_GHOST_SPAWN_X;
            ghost->target_y = RED_GHOST_SPAWN_Y;
            break;
        case 'P':
            ghost->target_x = PINK_GHOST_SPAWN_X;
            ghost->target_y = PINK_GHOST_SPAWN_Y;
            break;
        case 'G':
            ghost->target_x = GREEN_GHOST_SPAWN_X;
            ghost->target_y = GREEN_GHOST_SPAWN_Y;
            break;
        case 'O':
            ghost->target_x = ORANGE_GHOST_SPAWN_X;
            ghost->target_y = ORANGE_GHOST_SPAWN_Y;
            break;
        default:
            ghost->target_x = GHOST_ZONE_X;
            ghost->target_y = GHOST_ZONE_Y;
            break;
    }

    decideGhostDirectionToTarget(ghost, ghost->target_x, ghost->target_y);
    moveGhost(ghost);

    // 각 고스트가 자신의 스폰 위치에 도달했는지 확인
    if(ghost->x == ghost->target_x && ghost->y == ghost->target_y){

        // 유령이 돌아가는 소리를 멈춘다
        stopSoundMci("loop_sfx");

        if(isPowerModeActive()){
            playSoundMci("sounds/power_up.wav", "power_up", 1);
        }
        enqueueGhost(ghost);
        addGhostReleased(-1);
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
    // 고스트 도어 위치를 직접 타겟으로 설정
    ghost->target_x = GHOST_DOOR_X;  // 14
    ghost->target_y = GHOST_DOOR_Y - 1;  // 11
    
    // 고스트 도어 밖 도달 체크
    if(ghost->x == GHOST_DOOR_X && ghost->y == GHOST_DOOR_Y - 1) {
        ghost->state = CHASING;
        debug_log("Ghost %c exited to door, now CHASING\n", ghost->color);
        return;
    }
    
    // 기존 플로우 필드 시스템 사용
    decideGhostDirectionToTarget(ghost, ghost->target_x, ghost->target_y);
    moveGhost(ghost);
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

void handleGhostEaten(Ghost* ghost, int* score){
    static int ghost_combo = 0;
    int points = GHOST_COMBO_BASE_SCORE * (1 << ghost_combo);
    *score += points;
    ghost_combo++;

    // 파워 업 사운드를  멈춘다
    stopSoundMci("power_up");

    // 유령 먹는 소리를 재생
    playSoundAndWait("sounds/pacman_eats_ghost.wav", "ghost_eaten");

    ghost->state = RETURNING;
    ghost->target_x = GHOST_ZONE_X;
    ghost->target_y = GHOST_ZONE_Y;

    // 고스트가 돌아가는 소리 재생
    playSoundMci("sounds/ghost_back_to_base.wav", "loop_sfx", 1);
    ghost_back_active = 1;

    if(getPowerModeTimer() <= 0){
        ghost_combo = 0;
    }
}

// 고스트 릴리즈 상태 업데이트
void updateGhostReleaseState(double dt){
    // === 고스트 릴리즈 실시간 관리 ===
    if (ghost_released < MAX_GHOSTS && current_state == STATE_PLAYING) {
        ghost_release_timer += dt;

        if (ghost_release_timer >= ghost_release_interval) {
            dequeueGhost();
            ghost_released++;
            ghost_release_timer = 0;
            debug_log("Ghost released! Total: %d\n", ghost_released);
        }
    }
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
        // debug_log("Flow field generation for target (%d,%d): %.4f seconds, processed %d cells\n", 
        //          target_x, target_y, time_taken, rear);
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

int getGhostReleased() {
    return ghost_released;
}

void setGhostReleased(int value) {
    ghost_released = value;
}

void addGhostReleased(int value) {
    ghost_released += value;
}

double getGhostReleaseTimer() {
    return ghost_release_timer;
}

void setGhostReleaseTimer(double timer) {
    ghost_release_timer = timer;
}

double getGhostReleaseInterval() {
    return ghost_release_interval;
}

void setGhostReleaseInterval(double interval) {
    ghost_release_interval = interval;
}

int isGhostBackActive() {
    return ghost_back_active;
}

void setGhostBackActive(int active) {
    ghost_back_active = active;
}