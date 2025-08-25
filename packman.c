#include <stdio.h>
#include <windows.h>
#include <conio.h>

// 맵 요소
#define EMPTY 0
#define WALL 1
#define COOKIE 2
#define POWER_COOKIE 3
#define GHOST_DOOR 4
#define GHOST_ZONE 5
#define WARP_ZONE 6

// 맵 크기
#define MAP_WIDTH 28
#define MAP_HEIGHT 31

HANDLE screen[2];
int screen_index = 0;

int map[MAP_HEIGHT][MAP_WIDTH] = {
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
    {0,0,0,0,0,1,2,1,1,0,1,1,1,4,4,1,1,1,0,1,1,2,1,0,0,0,0,0},
    {1,1,1,1,1,1,2,1,1,0,1,5,5,5,5,5,5,1,0,1,1,2,1,1,1,1,1,1},
    {6,0,0,0,0,0,2,0,0,0,1,5,5,5,5,5,5,1,0,0,0,2,0,0,0,0,0,6}, // 가운데 줄, 좌우는 워프 존
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

void drawScore(int score, int lives);

typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct {
    int x;
    int y;
    Direction direction;
    int lives;
} Pacman;

typedef enum { CHASING, FRIGHTENED, EATEN } GhostState;

typedef struct {
    int x;
    int y;
    int direction;
    GhostState state;
    char color;
} Ghost;

void hideCursor() {
    CONSOLE_CURSOR_INFO cursor_info;

    cursor_info.dwSize = 1; // 커서 크기 설정
    cursor_info.bVisible = FALSE; // 커서 숨기기

    SetConsoleCursorInfo(screen[0], &cursor_info);
    SetConsoleCursorInfo(screen[1], &cursor_info);
}
void initialize() {
    HANDLE std_handle = GetStdHandle(STD_OUTPUT_HANDLE);


    COORD buffer_size = {(MAP_WIDTH * 2) + 20, MAP_HEIGHT};
    SMALL_RECT window_size = {0, 0, buffer_size.X - 1, buffer_size.Y - 1};
    

    // SMALL_RECT small_rect = {0, 0, 50, 20};
    // SetConsoleWindowInfo(std_handle, TRUE, &small_rect);
    SetConsoleScreenBufferSize(std_handle, buffer_size);
    SetConsoleWindowInfo(std_handle, TRUE, &window_size);

    // 화면 버퍼를 2개 생성합니다.
    screen[0] = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );

    screen[1] = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );

    // 버퍼 크기 설정 (맵 크기에 맞춤)
    SetConsoleScreenBufferSize(screen[0], buffer_size);
    SetConsoleScreenBufferSize(screen[1], buffer_size);

    hideCursor();
    SetConsoleOutputCP(65001); // UTF-8
}

void clear() {
    COORD coord = {0, 0};
    DWORD written;

    int buffer_size = ((MAP_WIDTH * 2) + 20) * (MAP_HEIGHT);

    FillConsoleOutputCharacter(screen[screen_index], ' ', buffer_size, coord, &written);
}

void flip() {
    SetConsoleActiveScreenBuffer(screen[screen_index]);
    screen_index = !screen_index;
}

void release() {
    CloseHandle(screen[0]);
    CloseHandle(screen[1]);
}
void drawEntity(int x, int y, const char* str, WORD color){
    COORD pos = {x * 2, y};
    DWORD dword;
    WriteConsoleOutputCharacterA(screen[screen_index], str, strlen(str), pos, &dword);
    FillConsoleOutputAttribute(screen[screen_index], color, strlen(str), pos, &dword);
}
void render(const Pacman* pacman, int score) {
    clear();

    for (int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            if(x == pacman->x && y == pacman->y){
                drawEntity(x, y, "C", FOREGROUND_RED | FOREGROUND_GREEN);
            } else {
                switch(map[y][x]){
                case EMPTY: drawEntity(x, y, " ", FOREGROUND_BLUE); break;
                case WALL: drawEntity(x, y, "#", FOREGROUND_INTENSITY); break;
                case COOKIE: drawEntity(x, y, "*", FOREGROUND_GREEN | FOREGROUND_GREEN); break;
                case POWER_COOKIE: drawEntity(x, y, "O", FOREGROUND_BLUE); break;
                case GHOST_DOOR: drawEntity(x, y, " ", FOREGROUND_INTENSITY); break;
                case GHOST_ZONE: drawEntity(x, y, " ", FOREGROUND_INTENSITY); break;
                case WARP_ZONE: drawEntity(x, y, "W", FOREGROUND_INTENSITY); break;
                }
            }
            
        }
    }

    drawScore(score, pacman->lives);
}
void debug_console_info() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(screen[screen_index], &csbi);
    
    printf("Buffer size: %d x %d\n", csbi.dwSize.X, csbi.dwSize.Y);
    printf("Window size: %d x %d\n", 
           csbi.srWindow.Right - csbi.srWindow.Left + 1,
           csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    printf("Window coords: Left=%d, Top=%d, Right=%d, Bottom=%d\n",
           csbi.srWindow.Left, csbi.srWindow.Top, 
           csbi.srWindow.Right, csbi.srWindow.Bottom);
}
void processInput(Pacman* pacman){
    if(_kbhit()){
        int key = _getch();

        if (key == 224){
            key = _getch();
            switch(key){
                case 72: {
                    // 위 방향으로 변경
                    if(pacman->y - 1 < 0 || map[pacman->y - 1][pacman->x] == WALL){
                        return;
                    }
                    pacman->direction = DIR_UP; 
                    break;
                }
                case 80: {
                    if(pacman->y + 1 >= MAP_HEIGHT || map[pacman->y + 1][pacman->x] == WALL){
                        return;
                    }
                    pacman->direction = DIR_DOWN;
                    break;
                }
                case 75: {
                    if(pacman->x - 1 < 0 || map[pacman->y][pacman->x - 1] == WALL){
                        return;
                    }
                    pacman->direction = DIR_LEFT;
                    break;
                }
                case 77: {
                    if(map[pacman->y][pacman->x + 1] == WALL){
                        return;
                    }
                    pacman->direction = DIR_RIGHT;
                    break;
                }
            }
        }
    }
}
void update(Pacman* pacman, int* score){
    int next_x = pacman->x;
    int next_y = pacman->y;

    // 현재 방향에 따라 다음 위치 계산
    switch(pacman->direction){
        case DIR_UP: next_y--; break;
        case DIR_DOWN: next_y++; break;
        case DIR_LEFT: next_x--; break;
        case DIR_RIGHT: next_x++; break;
        default: break;
    }

    if(map[next_y][next_x] != WALL){
        
        // 워프존 확인
        if(map[next_y][next_x] == WARP_ZONE){
            if(next_x == 0){ // 왼쪽 워프존
                pacman->x = MAP_WIDTH - 1;
            } else if(next_x == MAP_WIDTH - 1){ // 오른쪽 워프존
                pacman->x = 1;
            }
            return;
        }

        // 쿠키인지확인
        if(map[next_y][next_x] == COOKIE){
            // 쿠키 먹기
            map[next_y][next_x] = EMPTY;
            (*score) += 10; // 점수 증가
        } else if(map[next_y][next_x] == POWER_COOKIE){
            map[next_y][next_x] = EMPTY;
            (*score) += 50; // 점수 증가
        }

        pacman->x = next_x;
        pacman->y = next_y;
    }
}
void drawScore(int score, int lives){
    int score_x = MAP_WIDTH * 2 + 2; // 맵 오른쪽에 점수 표시

    drawEntity(score_x / 2, 2, "SCORE:", FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    char score_str[20];
    sprintf(score_str, "%d", score);
    drawEntity(score_x / 2, 3, score_str, FOREGROUND_GREEN | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    // lives
    drawEntity(score_x / 2, 5, "LIVES:", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);

    // lives 값
    char lives_str[10];
    sprintf(lives_str, "%d", lives);
    drawEntity(score_x / 2, 6, lives_str, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
}

int main() {
    int score = 0;
    initialize();
    
    Pacman pacman = {13, 23, DIR_NONE, 3};
    while(1){
        processInput(&pacman);
        update(&pacman, &score);
        render(&pacman, score);
        flip();
        Sleep(150); // 60FPS
    }

    release();
    return 0;
}