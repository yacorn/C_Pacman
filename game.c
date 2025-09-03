// game.c
#include "game.h"
#include "render.h"
#include "sound.h"
#include "maps.h"
#include <time.h>
#include <stdlib.h>

// --- 전역 변수 정의 (실제 데이터) ---
// packman.h에서 extern으로 선언했던 변수들의 실체를 여기에 정의합니다.

// 시간 관리 변수들
static LARGE_INTEGER frequency;
static LARGE_INTEGER last_time;
static double delta_time = 0.0;
static double accumulator = 0.0;
static const double FIXED_TIMESTEP = 0.25; // 250ms를 초 단위로
static double fps_timer = 0.0;
static int fps_counter = 0;
static int current_fps = 0;

HANDLE screen[2];
int screen_index = 0;
int game_time = 0;
int score = 0;
int power_mode = 0;
double power_mode_timer = 0.0;
int total_cookies = 0;
int cookies_eaten = 0;
int ghost_released = 1;
int current_stage = 1;
int is_bgm_playing = 0;
int is_first_start = 1;
int current_siren_level = 1;
int power_music_active = 0;
int ghost_back_active = 0;
double ghost_release_timer = 0.0;
double ghost_release_interval = 0.0;
double ready_delay_timer = 0.0;
// ... (모든 전역 변수 정의)
int debug_mode = 0;
GameState current_state = STATE_TITLE;
MenuOption current_menu_selection = MENU_START_GAME;  // 메뉴 선택 초기화
// GhostReleaseCondition release_conditions[] = {
//     {0, 0, 0},
//     {0, 0, 300},
//     {0, 0, 600},
//     {0, 0, 900}
// };

Ghost ghosts[MAX_GHOSTS];
Ghost* ghostQueue[MAX_GHOSTS];
int queue_front = 0, queue_rear = 0, queue_count = 0;

Fruit bonus_fruit = {-1, -1, 0, 0.0, 0, 0};

// --- 함수 구현 ---
void initialize() {
    // HANDLE std_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    // // 버퍼는 넉넉하게, 윈도우는 필요한 만큼만
    // COORD buffer_size = {MAP_WIDTH * 2 + 20, MAP_HEIGHT * 2}; // 76 x 36 (넉넉하게)
    // SMALL_RECT window_size = {0, 0, (MAP_WIDTH * 2 + 20) - 1, MAP_HEIGHT * 2}; // 75 x 32 (맵+여유공간)

    // // 먼저 윈도우를 작게 만들기
    // SMALL_RECT small_rect = {0, 0, 10, 10};
    // SetConsoleWindowInfo(std_handle, TRUE, &small_rect);

    // SetConsoleScreenBufferSize(std_handle, buffer_size);
    // SetConsoleWindowInfo(std_handle, TRUE, &window_size);

    // // 화면 버퍼를 2개 생성합니다.
    // screen[0] = CreateConsoleScreenBuffer(
    //     GENERIC_READ | GENERIC_WRITE,
    //     0,
    //     NULL,
    //     CONSOLE_TEXTMODE_BUFFER,
    //     NULL
    // );

    // screen[1] = CreateConsoleScreenBuffer(
    //     GENERIC_READ | GENERIC_WRITE,
    //     0,
    //     NULL,
    //     CONSOLE_TEXTMODE_BUFFER,
    //     NULL
    // );

    // // 버퍼 크기 설정
    // SetConsoleScreenBufferSize(screen[0], buffer_size);
    // SetConsoleScreenBufferSize(screen[1], buffer_size);

    // hideCursor();
    // SetConsoleOutputCP(65001);
    // SetConsoleCP(65001);
    screen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    screen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    DWORD dwMode = 0;
    GetConsoleMode(screen[0], &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(screen[0], dwMode);

    GetConsoleMode(screen[1], &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(screen[1], dwMode);

    hideCursor();
    restoreMap(map_stage1);
    
    // 랜덤 시드 초기화
    srand((unsigned int)time(NULL));

    // 고성능 타이머 초기화
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&last_time);
}

void resetGame(Pacman* pacman){
    current_stage = 1;
    
    score = 0;
    
    game_time = 0;
    
    cookies_eaten = 0;
    
    ghost_released = 1;
    ghost_release_timer = 0;
    ghost_release_interval = 5.0 - (current_stage - 1) * 0.5; // 5초에서 점점 빨라짐
    if (ghost_release_interval < 2.0) ghost_release_interval = 2.0; // 최소 2.0 초 유지

    power_mode = 0;
    power_mode_timer = 0;
    
    // 보너스 과일 초기화
    bonus_fruit.active = 0;
    bonus_fruit.timer = 0;
    
    initializePacman(pacman, 3);

    restoreMap(map_stage1);

    queue_count = 0;
    queue_front = 0;
    queue_rear = 0;

    initializeGhosts();

    is_bgm_playing = 0;
    is_first_start = 1;
    current_siren_level = 1;
    ready_delay_timer = 0;
    total_cookies = getTotalCookies();
}

void nextStage(Pacman* pacman){
    // 스테이지 클리어 보너스
    score += current_stage * SCORE_STAGE_CLEAR;
    score += pacman->lives * SCORE_BONUS_LIFE;

    current_stage++; // 스테이지 증가

    // 스테이지 난이도 조절(더 어렵게)
    // for(int i = 0; i < MAX_GHOSTS; i++){
    //     release_conditions[i].cookies_required = max(0, release_conditions[i].cookies_required - (stage * 5));
    // }
    ghost_release_interval = 5.0 - (current_stage - 1) * 0.5; // 5초에서 점점 빨라짐
    if (ghost_release_interval < 2.0) ghost_release_interval = 2.0; // 최소 2.0 초 유지

    int next_stage_index = (current_stage - 1 ) % MAX_STAGES;
    restoreMap(all_maps[next_stage_index]);

    total_cookies = getTotalCookies();

    cookies_eaten = 0;
    ghost_released = 1;
    ghost_release_timer = 0;
    power_mode = 0;
    power_mode_timer = 0;
    current_siren_level = 1;
    is_first_start = 1;
    ready_delay_timer = 0;

    // 보너스 과일 초기화
    bonus_fruit.active = 0;
    bonus_fruit.timer = 0;

    current_state = STATE_READY;

    // Pacman 초기화
    initializePacman(pacman, pacman->lives);

    queue_count = 0;
    queue_front = 0;
    queue_rear = 0;
    initializeGhosts();

    
}

void initializePacman(Pacman* pacman, int lives){
    pacman->x = PACMAN_SPAWN_X;
    pacman->y = PACMAN_SPAWN_Y;
    pacman->prev_x = PACMAN_SPAWN_X;
    pacman->prev_y = PACMAN_SPAWN_Y;
    pacman->direction = DIR_NONE;
    pacman->lives = lives;
}

void handleCollisions(Pacman* pacman, int* score){
    for(int i = 0; i < MAX_GHOSTS; i++){
        CollisionResult result = checkCollision(pacman, &ghosts[i]);

        switch(result){
            case COLLISION_PACMAN_DIES:
                handlePacmanDeath(pacman);
                break;
            case COLLISION_GHOST_EATEN:
                handleGhostEaten(&ghosts[i], score);
                break;

            default: 
                break;
        }
    }
}

void handlePacmanDeath(Pacman* pacman){
    pacman->lives--;
    current_state = STATE_PACMAN_DEATH;

    // 팩맨 방향 초기화
    pacman->direction = DIR_NONE;

    power_mode = 0;
    power_mode_timer = 0;

    // 모든 유령들 대기 상태로 초기화
    for(int i = 0; i < MAX_GHOSTS; i++){
        if(ghosts[i].state == FRIGHTENED){
            ghosts[i].state = CHASING;
        }
    }
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

    if(power_mode_timer <= 0){
        ghost_combo = 0;
    }
}

int isLevelComplete(){
    return cookies_eaten >= total_cookies;
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

void getNextPosition(int* x, int* y, Direction dir){
    switch(dir){
        case DIR_UP: (*y)--; break;
        case DIR_DOWN: (*y)++; break;
        case DIR_LEFT: (*x)--; break;
        case DIR_RIGHT: (*x)++; break;
        case DIR_NONE: break;
        default: break;
    }
}

static CollisionResult getCollisionType(const Ghost* ghost){
    switch(ghost->state){
        case CHASING:    return COLLISION_PACMAN_DIES;
        case FRIGHTENED: return COLLISION_GHOST_EATEN;
        default:         return COLLISION_NONE;
    }
}

CollisionResult checkCollision(const Pacman* pacman, const Ghost* ghost){
    
    if(pacman->x == ghost->x && pacman->y == ghost->y)
    {
        return getCollisionType(ghost);
    }

    if(pacman->x == ghost->prev_x && pacman->y == ghost->prev_y &&
       pacman->prev_x == ghost->x && pacman->prev_y == ghost->y)
    {
        return getCollisionType(ghost);
    }
    
    return COLLISION_NONE;
    
}

// 배경 음악 및 효과음 업데이트 함수
void updateBackGroundMusic() {
    if (power_mode && power_mode_timer > 0) {
        if (power_music_active) {
            char command[256];
            char position[256] = {0};
            char length[256] = {0};
            
            // 현재 위치와 길이 확인
            sprintf(command, "status power_up position");
            mciSendString(command, position, sizeof(position), NULL);
            
            sprintf(command, "status power_up length");
            mciSendString(command, length, sizeof(length), NULL);
            
            int pos = atoi(position);
            int len = atoi(length);
            
            // 90% 지점에서 다시 처음부터 재생
            if (len > 0 && pos >= (len * 0.9)) {
                sprintf(command, "seek power_up to start");
                mciSendString(command, NULL, 0, NULL);
                
                sprintf(command, "play power_up");
                mciSendString(command, NULL, 0, NULL);
                
                debug_log("Power sound looped at 90%% position\n");
            }
        }
    } else if (power_music_active) {
        // 파워모드가 끝났으면 파워 음악 정지
        stopSoundMci("power_up");
        power_music_active = 0;
        debug_log("Power mode ended, music stopped\n");
        
        is_bgm_playing = 0;
    }

    if (ghost_back_active) {
        int has_returning_ghost = 0;
        for(int i = 0; i < MAX_GHOSTS; i++){
            if(ghosts[i].state == RETURNING){
                has_returning_ghost = 1;
                break;
            }
        }
        if (has_returning_ghost) {
            // 고스트가 돌아가고 있는 중
            if(isSoundFinished("loop_sfx")){
                playSoundMci("sounds/ghost_back_to_base.wav", "loop_sfx", 1);
                debug_log("Ghost back sound restarted\n");
            }
        } else {
            // 돌아가는 고스트가 없으면 정지
            stopSoundMci("loop_sfx");
            ghost_back_active = 0;
            debug_log("All ghosts returned, back sound stopped\n");

            // 파워 모드가 아니면 siren 재시작
            if(!power_music_active) {
                is_bgm_playing = 0; // siren 재생 플래그 초기화
            }
        }
    }
}

// 게임 로직 처리 함수 (상태별 로직 분리)
void handleLogic(Pacman* pacman) {
    switch(current_state) {
        case STATE_TITLE:
            // 타이틀 화면에서는 로직 없음
            break;

        case STATE_HELP:
            // 도움말 화면에서는 로직 없음
            break;

        case STATE_READY:
            // Ready 상태 로직은 updateTimersRealTime에서 실시간 처리
            
            // 첫 시작이고 게임 시작 사운드가 끝났으면 게임 시작
            if(is_first_start && isSoundFinished("start_game")){
                stopSoundMci("start_game");
                is_first_start = 0;
                is_bgm_playing = 0;
                current_state = STATE_PLAYING;
            }
            break;
            
        case STATE_PLAYING:
            // 팩맨 업데이트
            updatePacman(pacman);
            
            // 충돌 검사
            handleCollisions(pacman, &score);

            if(current_state != STATE_PLAYING){
                break;
            }

            // 레벨 완료 체크
            if (isLevelComplete()) {
                stopAllGameSounds();
                
                // 마지막 스테이지 클리어 체크
                if(current_stage >= MAX_STAGES) {
                    // 모든 스테이지 완료 - 바로 올클리어!
                    int bonus_score = pacman->lives * SCORE_LEFT_LIFE_BONUS;
                    score += bonus_score;
                    current_state = STATE_ALL_CLEAR;
                } else {
                    // 일반 스테이지 클리어
                    current_state = STATE_LEVEL_COMPLETE;
                }
                break;
            }
            
            // 보너스 과일 생성 조건 (실시간 타이머에서 처리됨)
            if (!bonus_fruit.active) {
                if (cookies_eaten == BONUS_FRUIT_TRIGGER_COOKIES || 
                    (cookies_eaten == total_cookies / 2)) {
                    spawnBonusFruit(pacman);
                }
            }

            // // 보너스 과일 업데이트
            // updateBonusFruit();
            
            // 고스트 AI 업데이트
            updateAllGhost(pacman);

            // 2차 충돌 검사
            handleCollisions(pacman, &score);
                            
            // 게임 시간 증가
            game_time++;
            break;

        case STATE_PACMAN_DEATH:
            if(isSoundFinished("pacman_dying")){
                initializePacman(pacman, pacman->lives); // 팩맨 위치 초기화
                initializeGhosts(); // 모든 유령을 초기 상태로
                ghost_released = 1;
                ghost_release_timer = 0;
                ready_delay_timer = 0;
                
                if (pacman->lives <= 0) {
                    current_state = STATE_GAME_OVER;
                } else {
                    current_state = STATE_READY;
                }
            }
            break;

        case STATE_LEVEL_COMPLETE:
            {
                static int game_complete_delay = 0;
                game_complete_delay++;
                
                if(game_complete_delay > 60) { // 자동으로 다음 스테이지 (1.5초 후)
                    nextStage(pacman);
                    game_complete_delay = 0;
                }
            }
            break;
            
        case STATE_GAME_OVER:
            // 게임 오버 상태에서는 로직 없음 (입력만 처리)
            break;

        case STATE_ALL_CLEAR:
            // 올클리어 상태에서는 로직 없음 (입력만 처리)
            break;
    }
}

// 입력 처리 함수 (입력만 담당)
void handleInput(Pacman* pacman) {
    switch(current_state) {
        case STATE_TITLE:
            // 메뉴 내비게이션
            if(GetAsyncKeyState(VK_UP) & 0x0001) {
                current_menu_selection = (current_menu_selection - 1 + MENU_COUNT) % MENU_COUNT;
            }
            if(GetAsyncKeyState(VK_DOWN) & 0x0001) {
                current_menu_selection = (current_menu_selection + 1) % MENU_COUNT;
            }
            
            // 엔터 키로 선택 확정
            if(GetAsyncKeyState(VK_RETURN) & 0x0001) {
                switch(current_menu_selection) {
                    case MENU_START_GAME:
                        current_state = STATE_READY;
                        resetGame(pacman);
                        break;
                    case MENU_HOW_TO_PLAY:
                        current_state = STATE_HELP;
                        break;
                    case MENU_EXIT_GAME:
                        exit(0);
                        break;
                }
            }
            
            // ESC 키로 종료
            if(GetAsyncKeyState(VK_ESCAPE) & 0x0001) {
                exit(0);
            }
            break;

        case STATE_HELP:
            // ESC나 스페이스바로 타이틀로 돌아가기
            {
                static int enter_released = 0;

                if(!(GetAsyncKeyState(VK_RETURN) & 0x8000)){
                    enter_released = 1;
                }

                if(enter_released){
                    if((GetAsyncKeyState(VK_ESCAPE) & 0x0001) || (GetAsyncKeyState(VK_SPACE) & 0x0001)) {
                        current_state = STATE_TITLE;
                        enter_released = 0;
                    }
                }
            }
            break;

        case STATE_READY:
            // Ready 상태에서는 입력 없음 (자동 진행)
            break;
            
        case STATE_PLAYING:
            // 팩맨 입력 처리만
            processInput(pacman);
            break;

        case STATE_PACMAN_DEATH:
            // 죽는 애니메이션 중에는 입력 없음
            break;

        case STATE_LEVEL_COMPLETE:
            {
                static int game_complete_delay = 0;
                game_complete_delay++;
                
                if(game_complete_delay > 1){
                    if((GetAsyncKeyState('N') & 0x0001) || (GetAsyncKeyState('n') & 0x0001)){
                        // 다음 스테이지로 진행
                        nextStage(pacman);
                        game_complete_delay = 0;
                    } else if(GetAsyncKeyState(27) & 0x0001){ // ESC 키
                        current_state = STATE_TITLE;
                        stopAllGameSounds();
                        game_complete_delay = 0;
                    }
                }
            }
            break;
            
        case STATE_GAME_OVER:
            {
                static int game_over_delay = 0;
                game_over_delay++;

                if(game_over_delay > 1){
                    if((GetAsyncKeyState('R') & 0x0001) || (GetAsyncKeyState('r') & 0x0001)){
                        current_state = STATE_READY;
                        resetGame(pacman);
                        game_over_delay = 0;
                    }
                    else if(GetAsyncKeyState(27) & 0x0001){ // ESC 키
                        current_state = STATE_TITLE;
                        game_over_delay = 0;
                    }
                }
            }
            break;

        case STATE_ALL_CLEAR:
            if((GetAsyncKeyState('R') & 0x0001) || (GetAsyncKeyState('r') & 0x0001)){
                current_state = STATE_READY;
                resetGame(pacman);
            }
            else if(GetAsyncKeyState(27) & 0x0001){ // ESC 키
                current_state = STATE_TITLE;
            }
            break;
    }
}

// 렌더링 함수
void handleRender(Pacman* pacman) {
    clear();
    
    switch(current_state) {
        case STATE_TITLE:
            renderTitleScreen();
            break;

        case STATE_HELP:
            renderHelpScreen();
            break;

        case STATE_READY:
            renderGameplayScreen(pacman, score);
            drawEntity(MAP_WIDTH/2 - 1, MAP_HEIGHT/2 + 2, "Ready!", ANSI_YELLOW);
            break;

        case STATE_PLAYING:
            renderGameplayScreen(pacman, score);
            break;

        case STATE_PACMAN_DEATH:
            renderGameplayScreen(pacman, score);
            break;
        
        case STATE_LEVEL_COMPLETE:
            renderGameComplete(pacman, score);
            break;

        case STATE_GAME_OVER:
            renderGameOver(pacman, score);
            break;

        case STATE_ALL_CLEAR:
            renderAllClear(pacman, score);
            break;
    }
    
    if (debug_mode) {
        renderDebugInfo(pacman);
    }

    flip();
}

// 사운드 처리 함수
void handleSound() {
    switch(current_state) {
        case STATE_TITLE:
            // 타이틀 화면 BGM (필요시)
            break;

        case STATE_READY:
            // 첫 시작일 때만 게임 시작 사운드 재생
            if(is_first_start && is_bgm_playing == 0){
                playSoundMci("sounds/game_start.wav", "start_game", 0);
                is_bgm_playing = 1;
            }
            break;
            
        case STATE_PLAYING:
            // In the handleSound() function, replace the selection with:
            if(!power_mode && current_siren_level == 1 && cookies_eaten > (total_cookies * SIREN_LEVEL_CHANGE_THRESHOLD / 100)){
                current_siren_level = 2;
                stopSoundMci("siren");
                is_bgm_playing = 0;
            }
            // siren 사운드 시작
            if(is_bgm_playing == 0){
                if(current_siren_level == 1){
                    playSoundMci("sounds/ghost_siren_lv1.mp3", "siren", 1);
                } else if(current_siren_level >= 2){
                    playSoundMci("sounds/ghost_siren_lv2.mp3", "siren", 1);
                }
                is_bgm_playing = 1;
            }
            break;

        case STATE_PACMAN_DEATH:
            // 사운드 모두 정지
            if(is_bgm_playing){
                stopAllGameSounds();
            }
            // 팩맨 죽는 사운드 재생
            playSoundAndWait("sounds/pacman_dying.mp3", "pacman_dying");
            break;

        case STATE_LEVEL_COMPLETE:
            // 레벨 완료 사운드
            if(is_bgm_playing){
                stopAllGameSounds();
            }
            break;
            
        case STATE_GAME_OVER:
            // 게임 오버 사운드 (필요시)
            break;

        case STATE_ALL_CLEAR:
            // 올클리어 사운드
            if(is_bgm_playing){
                stopAllGameSounds();
            }
            // 올클리어 축하 사운드 재생
            playSoundAndWait("sounds/pacman_gets_high_score.mp3", "all_clear");
            break;
    }
}

// 대기 시간 계산 함수
// int getDelayTime() {
//     switch(current_state) {
//         case STATE_READY:
//             return 500;  // 사운드 체크를 위해 짧은 대기
//         case STATE_PACMAN_DEATH:
//         case STATE_LEVEL_COMPLETE:
//         case STATE_ALL_CLEAR:
//             return 1500; // 1.5초 대기
//         default:
//             return 250;  // 기본 대기 시간
//     }
// }


void stopAllGameSounds() {
    stopSoundMci("siren");
    stopSoundMci("loop_sfx");
    stopSoundMci("power_up");
    is_bgm_playing = 0;
}

int getFruitScore(FruitTypes type) {
    switch (type) {
        case FRUIT_CHERRY:
            return SCORE_CHERRY;
        case FRUIT_STRAWBERRY:
            return SCORE_STRAWBERRY;
        case FRUIT_ORANGE:
            return SCORE_ORANGE;
        case FRUIT_APPLE:
            return SCORE_APPLE;
        case FRUIT_MELON:
            return SCORE_MELON;
        default:
            return 0;
    }
}

// 보너스 과일 생성 함수
void spawnBonusFruit(const Pacman* pacman) {
    if (bonus_fruit.active) {
        return; // 이미 활성화된 과일이 있으면 생성하지 않음
    }
    
    // 빈 공간을 찾기 위한 시도 횟수 제한
    int max_attempts = MAX_SPAWN_ATTEMPTS;
    int attempts = 0;
    
    while (attempts < max_attempts) {
        // 맵 범위 내에서 랜덤 위치 생성
        int random_x = (rand() % (MAP_WIDTH - 2)) + 1;   // 벽을 피하기 위해 1~MAP_WIDTH-2
        int random_y = (rand() % (MAP_HEIGHT - 2)) + 1;  // 벽을 피하기 위해 1~MAP_HEIGHT-2
        
        // 해당 위치가 빈 공간이거나 쿠키가 있는 곳인지 확인
        int tile = current_map[random_y][random_x];
        if (tile == EMPTY || tile == COOKIE || tile == POWER_COOKIE) {
            // 팩맨이나 유령이 있는 위치는 피하기
            int position_occupied = 0;
            
            // 팩맨 위치 체크
            if (pacman && pacman->x == random_x && pacman->y == random_y) {
                position_occupied = 1;
            }
            
            // 유령 위치 체크
            if (!position_occupied) {
                for (int i = 0; i < MAX_GHOSTS; i++) {
                    if (ghosts[i].x == random_x && ghosts[i].y == random_y) {
                        position_occupied = 1;
                        break;
                    }
                }
            }
            
            if (!position_occupied) {
                // 과일 생성 (랜덤 지속시간: 8~12초)
                bonus_fruit.active = 1;
                bonus_fruit.x = random_x;
                bonus_fruit.y = random_y;
                bonus_fruit.type = rand() % FRUIT_COUNT; // 랜덤 과일 타입
                bonus_fruit.timer = FRUIT_MIN_DURATION + (rand() % (FRUIT_MAX_DURATION - FRUIT_MIN_DURATION + 1)); // 32~48 프레임 (8~12초, 프레임 단위)
                bonus_fruit.score = getFruitScore(bonus_fruit.type);

                // 만약 쿠키가 있던 자리에 과일이 생성되었다면 쿠키를 제거하고 총 쿠키 수 재계산
                if (tile == COOKIE || tile == POWER_COOKIE) {
                    current_map[random_y][random_x] = EMPTY;
                    total_cookies = getTotalCookies(); // 쿠키 개수 재계산
                }
                
                debug_log("Bonus fruit spawned at (%d, %d), total_cookies: %d\n", 
                         random_x, random_y, total_cookies);
                break;
            }
        }
        attempts++;
    }
    
    // 적절한 위치를 찾지 못했을 경우 기본 위치에 생성
    if (attempts >= max_attempts) {
        bonus_fruit.active = 1;
        bonus_fruit.x = 13;  // 기본 위치
        bonus_fruit.y = 17;
        bonus_fruit.timer = 32 + (rand() % 17); // 8~12초 (32~48 프레임)
        
        // 기본 위치에 쿠키가 있다면 제거하고 재계산
        int tile = current_map[17][13];
        if (tile == COOKIE || tile == POWER_COOKIE) {
            current_map[17][13] = EMPTY;
            total_cookies = getTotalCookies();
        }
        
        debug_log("Bonus fruit spawned at default position (13, 17)\n");
    }
}

// 보너스 과일 업데이트 함수
void updateBonusFruitState(double dt) {
    // === 보너스 과일 실시간 관리 ===
    if (bonus_fruit.active && bonus_fruit.timer <= 0) {
        // 8~15초 사이 랜덤 지속시간
        bonus_fruit.timer = 8.0 + (rand() % 8); // 8~15초
        debug_log("Bonus fruit timer started - %.1f seconds\n", bonus_fruit.timer);
    }

    if (bonus_fruit.active) {
        bonus_fruit.timer -= dt;

        if (bonus_fruit.timer <= 0) {
            bonus_fruit.active = 0;
            debug_log("Bonus fruit disappeared (real-time)\n");
        }
    }
}

// === Delta Time 관리 함수들 ===

// Delta time 업데이트 (매 프레임 호출)
void updateDeltaTime() {
    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    
    delta_time = (double)(current_time.QuadPart - last_time.QuadPart) / frequency.QuadPart;
    last_time = current_time;
    
    // 최대 delta time 제한 (프레임 드롭 방지)
    if (delta_time > 0.05) { // 50ms 이상이면 제한
        delta_time = 0.05;
    }
    
    // FPS 계산
    fps_counter++;
    fps_timer += delta_time;
    if (fps_timer >= 1.0) { // 1초마다 FPS 갱신
        current_fps = fps_counter;
        fps_counter = 0;
        fps_timer = 0.0;
        
        if (debug_mode) {
            debug_log("FPS: %d, Delta: %.4f\n", current_fps, delta_time);
        }
    }
}

// 고정 시간 스텝 게임 로직 업데이트 (Delta Time 독립적)
// void updateGameLogicFixed(Pacman* pacman) {
//     // handleLogic 함수를 호출하여 중복 제거
//     handleLogic(pacman);
// }

// 파워모드 업데이트
void updatePowerMode(double dt){
    // === 파워모드 실시간 관리 ===

    // 파워모드 활성 중일 때
    if (power_mode && power_mode_timer > 0) {
        power_mode_timer -= dt;

        if (power_mode_timer <= 0) {
            power_mode = 0;
            power_mode_timer = 0;
            
            // 모든 유령을 정상 상태로
            for(int i = 0; i < MAX_GHOSTS; i++){
                if(ghosts[i].state == FRIGHTENED){
                    ghosts[i].state = CHASING;
                }
            }
            debug_log("Power mode ended (exactly 10.0 seconds)\n");
        }
    }
    
}

// Ready 상태 업데이트
void updateReadyState(double dt){

    // === Ready 상태 실시간 관리 ===
    if (current_state == STATE_READY) {
        ready_delay_timer -= dt;

        if (!is_first_start && ready_delay_timer <= 0) {
            current_state = STATE_PLAYING;
            debug_log("Ready period ended - Game started\n");
        }
    }

    if(current_state != STATE_READY){
        ready_delay_timer = READY_DELAY_TIME;
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

// 타이머 기반 값들을 실시간으로 업데이트 (Delta Time 의존적)
void updateTimersRealTime(double dt) {
    
    // 파워모드 업데이트
    updatePowerMode(dt);

    // 배경음악 업데이트
    updateBackGroundMusic();

    // Ready 상태 업데이트
    updateReadyState(dt);

    // 고스트 릴리즈 상태 업데이트
    updateGhostReleaseState(dt);

    // 보너스 과일 상태 업데이트
    updateBonusFruitState(dt);
}

// Delta time 값 반환
double getDeltaTime() {
    return delta_time;
}

// 현재 FPS 반환
int getCurrentFPS() {
    return current_fps;
}
