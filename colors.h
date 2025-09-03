#ifndef COLORS_H
#define COLORS_H

// ANSI 기본 제어 코드
#define ANSI_RESET     "\033[0m"
#define ANSI_CLEAR     "\033[2J"
#define ANSI_HOME      "\033[H"
#define ANSI_HIDE_CURSOR "\033[?25l"
#define ANSI_SHOW_CURSOR "\033[?25h"

// ANSI 텍스트 스타일
#define ANSI_BOLD      "\033[1m"
#define ANSI_DIM       "\033[2m"
#define ANSI_ITALIC    "\033[3m"
#define ANSI_UNDERLINE "\033[4m"
#define ANSI_BLINK     "\033[5m"
#define ANSI_REVERSE   "\033[7m"
#define ANSI_STRIKETHROUGH "\033[9m"

// ANSI 기본 텍스트 색상
#define ANSI_BLACK     "\033[30m"
#define ANSI_RED       "\033[31m"
#define ANSI_GREEN     "\033[32m"
#define ANSI_YELLOW    "\033[33m"
#define ANSI_BLUE      "\033[34m"
#define ANSI_MAGENTA   "\033[35m"
#define ANSI_CYAN      "\033[36m"
#define ANSI_WHITE     "\033[37m"
#define ANSI_RAINBOW   "\033[38;5;196m"

// ANSI 밝은 텍스트 색상
#define ANSI_BRIGHT_BLACK   "\033[90m"
#define ANSI_BRIGHT_RED     "\033[91m"
#define ANSI_BRIGHT_GREEN   "\033[92m"
#define ANSI_BRIGHT_YELLOW  "\033[93m"
#define ANSI_BRIGHT_BLUE    "\033[94m"
#define ANSI_BRIGHT_MAGENTA "\033[95m"
#define ANSI_BRIGHT_CYAN    "\033[96m"
#define ANSI_BRIGHT_WHITE   "\033[97m"

// ANSI 배경 색상
#define ANSI_BG_BLACK     "\033[40m"
#define ANSI_BG_RED       "\033[41m"
#define ANSI_BG_GREEN     "\033[42m"
#define ANSI_BG_YELLOW    "\033[43m"
#define ANSI_BG_BLUE      "\033[44m"
#define ANSI_BG_MAGENTA   "\033[45m"
#define ANSI_BG_CYAN      "\033[46m"
#define ANSI_BG_WHITE     "\033[47m"

// ANSI 밝은 배경 색상
#define ANSI_BG_BRIGHT_BLACK   "\033[100m"
#define ANSI_BG_BRIGHT_RED     "\033[101m"
#define ANSI_BG_BRIGHT_GREEN   "\033[102m"
#define ANSI_BG_BRIGHT_YELLOW  "\033[103m"
#define ANSI_BG_BRIGHT_BLUE    "\033[104m"
#define ANSI_BG_BRIGHT_MAGENTA "\033[105m"
#define ANSI_BG_BRIGHT_CYAN    "\033[106m"
#define ANSI_BG_BRIGHT_WHITE   "\033[107m"

// 256색 지원 (예시)
#define ANSI_256_FG(n)    "\033[38;5;" #n "m"
#define ANSI_256_BG(n)    "\033[48;5;" #n "m"

// RGB 색상 지원 (True Color)
#define ANSI_RGB_FG(r,g,b) "\033[38;2;" #r ";" #g ";" #b "m"
#define ANSI_RGB_BG(r,g,b) "\033[48;2;" #r ";" #g ";" #b "m"

// 게임 전용 색상 조합
#define PACMAN_COLOR        ANSI_BRIGHT_YELLOW ANSI_BOLD
#define GHOST_RED_COLOR     ANSI_BRIGHT_RED ANSI_BOLD
#define GHOST_PINK_COLOR    ANSI_BRIGHT_MAGENTA ANSI_BOLD
#define GHOST_CYAN_COLOR    ANSI_BRIGHT_CYAN ANSI_BOLD
#define GHOST_GREEN_COLOR   ANSI_BRIGHT_GREEN ANSI_BOLD
#define GHOST_ORANGE_COLOR  ANSI_YELLOW ANSI_BOLD
#define WALL_COLOR          ANSI_BLUE ANSI_BOLD
#define COOKIE_COLOR        ANSI_YELLOW
#define POWER_COOKIE_COLOR  ANSI_WHITE ANSI_BOLD ANSI_BLINK
#define TARGET_COLOR        ANSI_WHITE ANSI_BLINK
#define DEBUG_COLOR         ANSI_GREEN ANSI_BOLD
#define SCORE_COLOR         ANSI_WHITE ANSI_BOLD
#define GHOST_ZONE_COLOR    ANSI_BG_BLUE
#define FRUIT_DEFAULT_COLOR ANSI_BRIGHT_MAGENTA ANSI_BOLD
#define FRUIT_CHERRY_COLOR  ANSI_BRIGHT_RED ANSI_BOLD
#define FRUIT_STRAWBERRY_COLOR ANSI_RED ANSI_BOLD
#define FRUIT_ORANGE_COLOR  ANSI_YELLOW ANSI_BOLD
#define FRUIT_APPLE_COLOR   ANSI_BRIGHT_GREEN ANSI_BOLD
#define FRUIT_MELON_COLOR   ANSI_GREEN ANSI_BOLD

// 특별한 조합들
#define ERROR_COLOR         ANSI_RED ANSI_BOLD ANSI_BLINK
#define SUCCESS_COLOR       ANSI_GREEN ANSI_BOLD
#define WARNING_COLOR       ANSI_YELLOW ANSI_BOLD
#define INFO_COLOR          ANSI_CYAN

// 커서 위치 이동 매크로
#define GOTO_XY(x, y)       "\033[" #y ";" #x "H"

// 게임별 특수 색상
#define FRIGHTENED_GHOST    ANSI_BLUE ANSI_BLINK
#define EATEN_GHOST         ANSI_WHITE
#define EXITING_GHOST       ANSI_CYAN

#endif // COLORS_H