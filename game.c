// game.c
#include "packman.h"

// --- 전역 변수 정의 (실제 데이터) ---
// packman.h에서 extern으로 선언했던 변수들의 실체를 여기에 정의합니다.
HANDLE screen[2];
int screen_index = 0;
int game_time = 0;
int score = 0;
int power_mode = 0;
int power_mode_timer = 0;
int total_cookies = 0;
int cookies_eaten = 0;
int ghost_released = 1;
int current_stage = 1;
int is_bgm_playing = 0;
int is_first_start = 1;
int current_siren_level = 1;
int power_music_active = 0;
int ghost_back_active = 0;
int ghost_release_timer = 0;
int ghost_release_interval = 20;
// ... (모든 전역 변수 정의)
int debug_mode = 0;
GameState current_state = STATE_TITLE;
GhostReleaseCondition release_conditions[] = {
    {0, 0, 0},
    {0, 0, 300},
    {0, 0, 600},
    {0, 0, 900}
};

// 맵 데이터
int map_stage1[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,3,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,3,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,4,1,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,5,5,5,5,5,5,1,0,1,1,2,1,1,1,1,1,1},
    {6,0,0,0,0,0,2,0,0,0,1,5,5,5,5,5,5,1,0,0,0,2,0,0,0,0,0,6},
    {1,1,1,1,1,1,2,1,1,0,1,5,5,5,5,5,5,1,0,1,1,2,1,1,1,1,1,1},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
    {1,3,2,2,1,1,2,2,2,2,2,2,2,0,0,2,2,2,2,2,2,2,1,1,2,2,3,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,1,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,1,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
int map_stage2[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,1,1,2,2,2,1,1,1,2,2,2,2,1,1,1,2,2,2,1,1,2,2,2,1},
    {1,2,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,3,2,2,2,2,2,1,2,1,1,1,2,1,1,2,1,1,1,2,1,2,2,2,2,2,3,1},
    {1,2,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,1,1,1,2,2,2,2,1,1,1,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1}, // exit_path (13,11) 확보
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1}, // exit_path (13,12) 확보
    {1,1,1,1,1,1,1,1,1,2,1,1,1,4,1,1,1,1,2,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,2,1,5,5,5,5,5,5,1,2,1,1,1,1,1,1,1,1,1},
    {6,2,2,2,2,2,2,2,2,2,1,5,5,5,5,5,5,1,2,2,2,2,2,2,2,2,2,6},
    {1,1,1,1,1,1,1,1,1,2,1,5,5,5,5,5,5,1,2,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,1,1,1,1,2,2,2,2,2,2,1,1,1,1,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,2,2,2,2,1,1,1,1,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,1,1,1,1,0,1,1,0,1,1,1,1,1,2,1,1,1,1,1,1},
    {1,1,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,1,1,2,0,0,2,1,1,1,2,2,2,2,2,2,2,2,1}, // 팩맨 시작 위치 (13,23) 확보
    {1,2,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,3,2,2,2,2,2,1,2,1,1,1,2,1,1,2,1,1,1,2,1,2,2,2,2,2,3,1},
    {1,2,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,2,2,2,1,1,2,2,2,1,1,1,2,2,2,2,1,1,1,2,2,2,1,1,2,2,2,1},
    {1,1,1,2,1,1,1,2,1,1,1,1,1,2,2,1,1,1,1,1,2,1,1,1,2,1,1,1},
    {1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};// Stage 3: 나선형 미로
int map_stage3[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,3,2,2,2,1,2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,2,1,2,2,2,3,1},
    {1,2,1,1,2,1,2,1,1,2,1,2,1,1,1,1,2,1,2,1,1,2,1,2,1,1,2,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,1,1,1,1,2,1},
    {1,2,2,2,2,1,2,1,1,1,2,2,2,2,2,2,2,2,1,1,1,2,1,2,2,2,2,1},
    {1,1,1,1,2,1,2,1,2,2,2,1,1,1,1,1,1,2,2,2,1,2,1,2,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,2,1,1,1,2,1},
    {1,2,2,2,2,1,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,2,2,2,1},
    {1,1,1,1,2,1,1,1,1,1,1,1,2,1,1,2,2,1,1,1,1,1,1,1,2,1,1,1}, // exit_path (13,11) 확보
    {6,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,6}, // exit_path (13,12) 확보
    {1,1,1,1,1,1,1,1,2,1,2,1,1,4,1,1,1,2,1,2,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1,2,2,2,1,5,5,5,5,1,2,2,2,1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,2,1,2,1,5,5,5,5,1,2,1,2,1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,2,2,2,1,5,5,5,5,1,2,2,2,1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,2,1,1,2,1,1},
    {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,2,1,2,1,2,2,2,1,1,1,1,1,1,2,2,2,1,2,1,2,1,1,1,1},
    {1,2,2,2,2,1,2,1,1,1,2,2,2,2,2,2,2,2,1,1,1,2,1,2,2,2,2,1},
    {1,2,1,1,1,1,2,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,1,1,2,2,2,2,0,0,2,2,2,2,1,1,2,2,2,2,2,2,1}, // 팩맨 시작 위치 (13,23) 확보
    {1,2,1,1,2,1,2,1,1,2,1,2,1,1,1,1,2,1,2,1,1,2,1,2,1,1,2,1},
    {1,3,2,2,2,1,2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,2,1,2,2,2,3,1},
    {1,2,1,1,1,1,2,1,2,2,2,2,2,1,1,2,2,2,2,2,1,2,1,1,1,1,2,1},
    {1,2,2,2,1,1,2,2,2,1,1,1,2,2,2,2,1,1,1,2,2,2,1,1,2,2,2,1},
    {1,1,1,2,1,1,1,2,1,1,1,1,1,2,2,1,1,1,1,1,2,1,1,1,2,1,1,1},
    {1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
int map_stage4[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,3,2,2,2,1,1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,1,2,2,2,3,1},
    {1,2,1,1,2,1,1,2,1,1,1,1,2,1,1,2,1,1,1,1,2,1,1,2,1,1,2,1},
    {1,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,1},
    {1,2,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,2,1,1,2,2,2,2,2,2,2,2,1,1,2,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,2,1}, // exit_path (13,11) 확보
    {1,2,2,2,2,2,2,2,2,2,2,1,1,2,1,1,1,2,2,2,2,2,2,2,2,2,2,1}, // exit_path (13,12) 확보
    {1,0,0,1,1,2,1,1,2,1,1,1,1,4,1,1,1,1,2,1,1,2,1,1,0,0,0,1},
    {1,1,1,1,1,2,1,1,2,1,1,5,5,5,5,5,1,1,2,1,1,2,1,1,1,1,1,1},
    {6,2,2,2,2,2,2,2,2,1,1,5,5,5,5,5,1,1,2,2,2,2,2,2,2,2,2,6},
    {1,1,1,1,1,1,1,1,1,2,1,5,5,5,5,5,1,1,2,1,1,2,1,1,1,1,1,1},
    {1,0,0,1,1,2,1,1,2,1,1,1,1,1,1,1,1,1,2,1,1,2,1,1,0,0,0,1},
    {1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,1,1,1,2,1,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,2,1,1,2,2,2,2,2,2,2,2,1,1,2,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,1,1,2,2,2,0,0,2,2,2,1,1,2,2,2,2,2,2,2,1}, // 팩맨 시작 위치 (13,23) 확보
    {1,2,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,2,1},
    {1,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,1},
    {1,2,1,1,2,1,1,2,1,1,1,1,2,1,1,2,1,1,1,1,2,1,1,2,1,1,2,1},
    {1,3,2,2,2,1,1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,1,2,2,2,3,1},
    {1,2,1,1,2,1,1,2,1,1,1,2,2,2,2,2,1,1,1,1,2,1,1,2,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
int current_map[MAP_HEIGHT][MAP_WIDTH];
// ... (나머지 맵 관련 변수들)
const int (*all_maps[])[MAP_WIDTH] = {
    map_stage1,
    map_stage2,
    map_stage3,
    map_stage4
};
int exit_path[][2] = {
    // {13, 13},
    {13, 12},
    {13, 11}
};
int exit_path_length = sizeof(exit_path) / sizeof(exit_path[0]);
int MAX_STAGES = sizeof(all_maps) / sizeof(all_maps[0]);

Ghost ghosts[MAX_GHOSTS];
Ghost* ghostQueue[MAX_GHOSTS];
int queue_front = 0, queue_rear = 0, queue_count = 0;

Fruit bonus_fruit = {0, 0, 0, 0, 100};
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
}
void resetGame(Pacman* pacman){
    current_stage = 1;
    score = 0;
    game_time = 0;
    cookies_eaten = 0;
    ghost_released = 1;
    ghost_release_timer = 0;
    power_mode = 0;
    power_mode_timer = 0;
    *pacman = (Pacman){13, 23, 13, 23, DIR_NONE, 3};

    current_stage = 2;
    restoreMap(map_stage1);

    queue_count = 0;
    queue_front = 0;
    queue_rear = 0;
    initializeGhosts();

    is_bgm_playing = 0;
    is_first_start = 1;
    current_siren_level = 1;
    total_cookies = getTotalCookies();
}
void nextStage(Pacman* pacman){
    current_stage++; // 스테이지 증가

    // 스테이지 클리어 보너스
    score += current_stage * 1000;
    score += pacman->lives * 300;

    // 스테이지 난이도 조절(더 어렵게)
    // for(int i = 0; i < MAX_GHOSTS; i++){
    //     release_conditions[i].cookies_required = max(0, release_conditions[i].cookies_required - (stage * 5));
    // }
    ghost_release_interval = max(6, 12 - (current_stage - 1 ) * 1);

    int next_stage_index = (current_stage - 1 ) % MAX_STAGES;
    restoreMap(all_maps[next_stage_index]);

    total_cookies = getTotalCookies();

    cookies_eaten = 0;
    ghost_released = 1;
    ghost_release_timer = 0;
    power_mode = 0;
    power_mode_timer = 0;

    current_state = STATE_READY;

    // Pacman 초기화
    *pacman = (Pacman){13, 23, 13, 23, DIR_NONE, pacman->lives};

    queue_count = 0;
    queue_front = 0;
    queue_rear = 0;
    initializeGhosts();
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
    int points = 200 * (1 << ghost_combo);
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
void restoreMap(const int socurce_map[MAP_HEIGHT][MAP_WIDTH]){
    memcpy(current_map, socurce_map, sizeof(current_map));
}
int getTotalCookies(){
    int total = 0;

    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            if(current_map[y][x] == COOKIE || current_map[y][x] == POWER_COOKIE){
                total++;
            }
        }
    }
    return total;
}
int isGameComplete(){
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
        ghost->path_index = 0;

        // 현재 위치에서 가장 가까운 exit_path 인덱스 찾기
        int best_index = 0;
        int min_distance = abs(ghost->x - exit_path[0][0]) + abs(ghost->y - exit_path[0][1]);
        
        for(int i = 1; i < exit_path_length; i++){
            int distance = abs(ghost->x - exit_path[i][0]) + abs(ghost->y - exit_path[i][1]);
            if(distance < min_distance || (distance == min_distance && i > best_index)){
                min_distance = distance;
                best_index = i;
            }
        }
        
        ghost->path_index = best_index;
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

CollisionResult checkCollision(const Pacman* pacman, const Ghost* ghost){
    
    if(pacman->x == ghost->x && pacman->y == ghost->y)
    {
        // 충돌 발생
        switch(ghost->state){
        case CHASING:
            return COLLISION_PACMAN_DIES; // 팩맨이 죽음
        case FRIGHTENED:
            return COLLISION_GHOST_EATEN; // 고스트가 먹힘
        case RETURNING:
        case EXITING:
        case WAITING:
            return COLLISION_IGNORED;
        default:
            return COLLISION_NONE;
        }
    }

    if(pacman->x == ghost->prev_x && pacman->y == ghost->prev_y &&
       pacman->prev_x == ghost->x && pacman->prev_y == ghost->y)
    {
        // 서로 위치 바꿈
        switch(ghost->state){
        case CHASING:
            return COLLISION_PACMAN_DIES; // 팩맨이 죽음
        case FRIGHTENED:
            return COLLISION_GHOST_EATEN; // 고스트가 먹힘
        case RETURNING:
        case EXITING:
        case WAITING:
            return COLLISION_IGNORED;
        default:
            return COLLISION_NONE;
        }
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

            // 파워 모드가 아니면 siren 재시작?
            if(!power_music_active) {
                is_bgm_playing = 0; // siren 재생 플래그 초기화
            }
        }
    }
}

// 입력 처리 함수
void handleInput(Pacman* pacman) {
    switch(current_state) {
        case STATE_TITLE:
            if (_kbhit()) {
                _getch();
                current_state = STATE_READY;
                resetGame(pacman);
            }
            break;

        case STATE_READY:
            // 첫 시작이 아니면 바로 게임 시작 (팩맨 사망 후)
            if(!is_first_start){
                current_state = STATE_PLAYING;
            }
            // 첫 시작이고 게임 시작 사운드가 끝났으면 게임 시작
            else if(is_first_start && isSoundFinished("start_game")){
                stopSoundMci("start_game");
                is_first_start = 0;
                is_bgm_playing = 0;
                current_state = STATE_PLAYING;
            }
            break;
            
        case STATE_PLAYING:
            processInput(pacman);
            updatePacman(pacman);

            if (isGameComplete()) {
                stopSoundMci("siren");
                stopSoundMci("loop_sfx");
                current_state = STATE_LEVEL_COMPLETE;
                break;
            }
            
            if (!bonus_fruit.active && cookies_eaten == 70){
                bonus_fruit.active = 1;
                bonus_fruit.x = 13;
                bonus_fruit.y = 17;
                bonus_fruit.timer = 300;
            }

            if (bonus_fruit.active){
                bonus_fruit.timer--;
                if (bonus_fruit.timer <= 0){
                    bonus_fruit.active = 0;
                }
            }
            
            if(ghost_released < MAX_GHOSTS){
                ghost_release_timer++;
                if(ghost_release_timer >= ghost_release_interval){
                    dequeueGhost();
                    ghost_released++;
                    ghost_release_timer = 0;
                    debug_log("Ghost released, total released: %d\n", ghost_released);
                }
            }
            
            updateAllGhost(pacman);
            updateBackGroundMusic();
            handleCollisions(pacman, &score);
            game_time++;
            break;

        case STATE_PACMAN_DEATH:
            if(isSoundFinished("pacman_dying")){
                pacman->x = 13;
                pacman->y = 23;
                pacman->direction = DIR_NONE;

                initializeGhosts(); // 모든 유령을 초기 상태로
                ghost_released = 1;

                if (pacman->lives <= 0) {
                    current_state = STATE_GAME_OVER;
                } else {
                    current_state = STATE_READY;
                }
            }
            break;

        case STATE_LEVEL_COMPLETE:
            if(_kbhit()){
                int key = _getch();
                if(key == 'n' || key == 'N'){
                    nextStage(pacman);
                } else if(key == 27){
                    current_state = STATE_TITLE;
                }
            }
            break;
            
        case STATE_GAME_OVER:
            if (_kbhit()) {
                int key = _getch();
                if (key == 'r' || key == 'R') {
                    current_state = STATE_READY;
                    resetGame(pacman);
                }
                else if (key == 27) {
                    current_state = STATE_TITLE;
                }
            }
            break;
    }
}

// 렌더링 함수
void handleRender(Pacman* pacman) {
    clear();
    
    switch(current_state) {
        case STATE_TITLE:
            drawEntity(MAP_WIDTH/2 - 5, MAP_HEIGHT/2, "PACKMAN", PACMAN_COLOR);
            drawEntity(MAP_WIDTH/2 - 8, MAP_HEIGHT/2 + 2, "Press Any Key...", ANSI_WHITE);
            break;

        case STATE_READY:
            renderGameplayScreen(pacman, score);
            drawEntity(MAP_WIDTH/2 - 2, MAP_HEIGHT/2 + 2, "Ready!", ANSI_YELLOW);
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
                stopSoundMci("siren");
                stopSoundMci("loop_sfx");
                stopSoundMci("power_up");
                is_bgm_playing = 0;
            }
            // 팩맨 죽는 사운드 재생
            playSoundAndWait("sounds/pacman_dying.mp3", "pacman_dying");
            break;

        case STATE_LEVEL_COMPLETE:
            // 레벨 완료 사운드
            if(is_bgm_playing){
                stopSoundMci("siren");
                stopSoundMci("loop_sfx");
                stopSoundMci("power_up");
                is_bgm_playing = 0;
            }
            break;
            
        case STATE_GAME_OVER:
            // 게임 오버 사운드 (필요시)
            break;
    }
}

// 대기 시간 계산 함수
int getDelayTime() {
    switch(current_state) {
        case STATE_READY:
            return 100;  // 사운드 체크를 위해 짧은 대기
        case STATE_PACMAN_DEATH:
        case STATE_LEVEL_COMPLETE:
            return 1500; // 1.5초 대기
        default:
            return 250;  // 기본 대기 시간
    }
}

