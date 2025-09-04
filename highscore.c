#include "highscore.h"
#include "common.h"
#include <string.h>
#include <stdio.h>

static HighScore high_scores[MAX_HIGHSCORES];
static int highscore_loaded = 0;

// 초기화
void initializeHighScoresSystem() {
    // 기본값으로 초기화
    for(int i = 0; i < MAX_HIGHSCORES; i++){
        strcpy(high_scores[i].name, "EMPTY");
        high_scores[i].score = 0;
        high_scores[i].stage = 0;
        strcpy(high_scores[i].date, "1999-01-01");
    }

    if(!loadHighScores()){
        debug_log("High score file not found, using default values.\n");

        // 기본 하이스코어 설정
        strcpy(high_scores[0].name, "PACMAN");
        high_scores[0].score = 10000;
        high_scores[0].stage = 1;
        strcpy(high_scores[0].date, "1999-01-01");

        saveHighScores(); // 기본값 저장
    }

    highscore_loaded = 1;
    debug_log("High score system initialized.\n");
}

int loadHighScores() {
    FILE* file = fopen(HIGHSCORE_FILE, "r");
    if(!file){
        return 0;
    }

    char line[256];
    int count = 0;

    while(fgets(line, sizeof(line), file) && count < MAX_HIGHSCORES){
        
        if(line[0] == '#' || line[0] == '\0' || line[0] == '\n'){
            continue; // 주석이나 빈 라인 무시
        }

        char name[MAX_NAME_LENGTH];
        int score, stage;
        char date[12];

        if(sscanf(line, "%19[^,],%d,%d,%11s", name, &score, &stage, date) == 4){
            strncpy(high_scores[count].name, name, MAX_NAME_LENGTH - 1);
            high_scores[count].name[MAX_NAME_LENGTH - 1] = '\0'; // null 종료
            high_scores[count].score = score;
            high_scores[count].stage = stage;
            strncpy(high_scores[count].date, date, 11);
            high_scores[count].date[11] = '\0'; // null 종료
            count++;
        }        
    }
    // 첫 번째 데이터 라인 처리
    
    fclose(file);
    debug_log("High scores loaded from %s\n", HIGHSCORE_FILE);
    debug_log("Total high scores loaded: %d\n", count);
    return 1;
}

int saveHighScores() {
    FILE* file = fopen(HIGHSCORE_FILE, "w");
    if(!file){
        debug_log("Failed to open high score file for writing: %s\n", HIGHSCORE_FILE);
        return 0;
    }

    // 헤더 작성
    fprintf(file, "# Pacman High Scores (Top %d)\n", MAX_HIGHSCORES);
    fprintf(file, "# Format: Name, Score, Stage, Date (YYYY-MM-DD)\n");

    // 데이터 작성
    for(int i = 0; i < MAX_HIGHSCORES; i++){
        if(high_scores[i].score > 0){
            fprintf(file, "%s,%d,%d,%s\n",
                    high_scores[i].name,
                    high_scores[i].score,
                    high_scores[i].stage,
                    high_scores[i].date);
        }
    }

    fclose(file);
    debug_log("High scores saved to %s\n", HIGHSCORE_FILE);
    return 1;
}

int isHighScore(int score) {
    if(!highscore_loaded) return 0;

    // 현재 점수가 최하위 점수보다 높으면 하이스코어
    return score > high_scores[MAX_HIGHSCORES - 1].score;
}

int addHighScore(const char* name, int score, int stage, const char* date){
    if(!highscore_loaded || !name) return -1;

    // 하이스코어인지 확인
    if(!isHighScore(score)){
        return 0; // 하이스코어 아님
    }

    // 삽입할 위치 찾기
    int insert_pos = MAX_HIGHSCORES;
    for(int i = 0; i < MAX_HIGHSCORES; i++){
        if(score > high_scores[i].score){
            insert_pos = i;
            break;
        }
    }

    if(insert_pos >= MAX_HIGHSCORES){
        return 0; // 삽입할 위치 없음
    }

    // 배열을 한 칸씩 뒤로 밀기
    for(int i = MAX_HIGHSCORES - 1; i > insert_pos; i--){
        high_scores[i] = high_scores[i - 1];
    }

    strncpy(high_scores[insert_pos].name, name, MAX_NAME_LENGTH - 1);
    high_scores[insert_pos].name[MAX_NAME_LENGTH - 1] = '\0'; // null 종료
    high_scores[insert_pos].score = score;
    high_scores[insert_pos].stage = stage;
    strncpy(high_scores[insert_pos].date, date, 11);

    // 파일 저장
    saveHighScores();

    debug_log("New high score added: %s - %d points on stage %d\n", name, score, stage);

    return insert_pos + 1; // 순위 반환
}

char* getCurrentDate(){
    static char date_str[12];
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);

    strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_info);
    return date_str;
}

/*
    하이스코어 목록 복사 함수
    scores 배열에 최대 MAX_HIGHSCORES 개수만큼 복사
    반환값: 성공 시 1, 실패 시 -1
*/
int getHighScores(HighScore* scores) {
    if(!highscore_loaded || !scores) return -1;

    for(int i = 0; i < MAX_HIGHSCORES; i++){
        scores[i] = high_scores[i];
    }
    return 1;
}
