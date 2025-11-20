#ifndef STRUCT_H
#define STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


// UI 관련
#define UI_RESET "\x1B[0m"
#define UI_REVERSE "\x1B[7m"
#define UI_DIM "\x1B[2m"
#define UI_BOLD "\x1B[1m"
#define UI_COLOR_CYAN "\x1B[36m"
#define UI_COLOR_YELLOW "\x1B[33m"
#define UI_COLOR_GREEN "\x1B[32m"

typedef enum _Key {
    LEFT_ARROW = 75,
    RIGHT_ARROW = 77,
    UP_ARROW = 72,
    DOWN_ARROW = 80,
    ENTER = '\r',
    BACKSPACE = '\b',
    ESC = 27
} Key;

#define CONSOLE_WIDTH 80
#define UI_WIDTH 50 // UI 요소가 차지하는 대략적인 가로 길이 (제목 포함)
#define START_X ((CONSOLE_WIDTH - UI_WIDTH) / 2) // UI 시작점 X 좌표
#define START_Y 0

void goto_ansi(int x, int y) {
    printf("\x1B[%d;%dH", y, x);
}

void print_center(const char* title, int len, int y_pos) {
    goto_ansi(START_X + (UI_WIDTH - len) / 2, START_Y + y_pos);
    printf("%s", title);
}

// 프로그램 관련
#define PATH_LENGTH 100
#define NAME_LENGTH 100
#define SEMESTER_NUM 6
#define MAX_SUBJECT_NUM 50
#define ID_NUM 50
#define MESSAGE_LENGTH 210
#define PW_LENGTH 30
#define MAX_APPLY_NUM 50

// 구조체

typedef struct _subject{
    int isFile, n, id;
    int year;
    int semester;
    int credit;
    double mean_raw_score;
    double stdev_raw_score;
    char name[NAME_LENGTH];
    struct _subject *arr[MAX_SUBJECT_NUM];
} Subject;

typedef struct {
int n;
Subject *subjects[MAX_SUBJECT_NUM];
double difficulty;
Subject *argmax_load;
Subject (*argmax_synergy)[2];
} TimeTable;

typedef struct {
int id, n;
char name[NAME_LENGTH];
Subject *subject_arr[MAX_SUBJECT_NUM];
int weight[MAX_SUBJECT_NUM];
} TechTree;

typedef struct {
int id;
Subject *subject;
int current_students, target_students;
int students_id[ID_NUM];
char promo_message[MESSAGE_LENGTH];
} BoardPost;

typedef struct {
    Subject subject;
    double z_score;
} SubjectZScore;

typedef struct {
    int id;
    int current_sem;
    TimeTable *table[SEMESTER_NUM];
    BoardPost *apply[MAX_APPLY_NUM];
    SubjectZScore z_array[MAX_SUBJECT_NUM];
} User;

// 공용체

typedef union {
    Subject *subject;
    TimeTable *time_table;
    TechTree *tech_tree;
    BoardPost *board_post;
} DataPointer;

// 열거형
typedef enum {
    MENU_STATE_MAIN,
    MENU_STATE_CALCULATOR, 
    MENU_STATE_TECH_TREE,
    MENU_STATE_BOARD,
    MENU_STATE_EXIT
} MenuState;

typedef enum {
    SUCCESS,
    ERROR_FILE_NOT_FOUND,
    ERROR_INVALID_INPUT,
    ERROR_INDEX_OUT
} StatusCode;

#endif //STRUCT_H