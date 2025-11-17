#ifndef STRUCT_H
#define STRUCT_H

#include <stdio.h>
#include <stdlib.h>

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
    char pw[PW_LENGTH];
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