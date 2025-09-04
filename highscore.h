#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#define MAX_HIGHSCORES 10
#define MAX_NAME_LENGTH 20
#define HIGHSCORE_FILE "data/highscores.txt"

typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
    int stage;
    char date[12]; // YYYY-MM-DD
} HighScore;

typedef struct {
    char nickname[MAX_NAME_LENGTH];
    int cursor_pos;
    int is_active;
    int achieved_score;
    int achieved_stage;
} HighScoreEntry;

void initializeHighScoresSystem();
int loadHighScores();
int saveHighScores();
int isHighScore(int score);
int addHighScore(const char* name, int score, int stage, const char* date);
char* getCurrentDate();
int getHighScores(HighScore* scores);

#endif // HIGHSCORE_H