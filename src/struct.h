// modified by ?ž¥ë¯¼ì?? on 2025.11.23. SubjectStats, SubjectInfo êµ¬ì¡°ì²? ì¶”ê??


#ifndef STRUCT_H
#define STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>

static inline int getch(void) {
    struct termios oldattr, newattr;

    if (tcgetattr(STDIN_FILENO, &oldattr) != 0) return -1;

    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newattr) != 0) return -1;

    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

    return ch;
}
#endif

// UI ê´?? ¨
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

typedef enum _Page {
    HOME_PAGE,
    LOGIN_PAGE,
    INPUT_PAGE,
    OUTPUT_PAGE,
    ESC_PAGE
} Page;

#define CONSOLE_WIDTH 80
#define UI_WIDTH 50 // UI ?š”?†Œê°? ì°¨ì???•˜?Š” ????žµ? ?¸ ê°?ë¡? ê¸¸ì´ (? œëª? ?¬?•¨)
#define START_X ((CONSOLE_WIDTH - UI_WIDTH) / 2) // UI ?‹œ?ž‘?  X ì¢Œí‘œ
#define START_Y 0

static inline void goto_ansi(int x, int y) {
    printf("\x1B[%d;%dH", y, x);
}

static inline void print_center(const char* title, int len, int y_pos) {
    goto_ansi(START_X + (UI_WIDTH - len) / 2, START_Y + y_pos);
    printf("%s", title);
}

// ?”„ë¡œê·¸?ž¨ ê´?? ¨
#define PATH_LENGTH 300
#define NAME_LENGTH 100
#define SEMESTER_NUM 6
#define MAX_SUBJECT_NUM 50
#define ID_NUM 50
#define MESSAGE_LENGTH 210
#define PW_LENGTH 30
#define MAX_APPLY_NUM 50

//?„ ?ˆ˜ê³¼ëª©?˜ ìµœë?? ê°œìˆ˜
#define MAX_PREREQ 10

//ê³¼ëª© ?†µê³? ? •ë³´ì˜ ìµœë?? ê°œìˆ˜
#define MAX_SUBJECT_STATS 100

// êµ¬ì¡°ì²?

// ?Š¹? • ê³¼ëª©?˜ ê°? ?…„?„, ?•™ê¸? ë³? ?›? ?ˆ˜?˜ ?†µê³? ? •ë³´ë?? ?‹´??? êµ¬ì¡°ì²?
typedef struct {
    int year;
    int semester;
    double mean_raw_score;
    double stdev_raw_score;
} SubjectStats;

typedef struct {
    int id;
    char name[NAME_LENGTH];

    //ê³¼ëª© ?•™? (?¼ì£¼ì¼ ?ˆ˜?—… ?šŸ?ˆ˜)
    int credit; 

    //?„ ?ˆ˜ê³¼ëª© ê°œìˆ˜ ë°? ID ëª©ë¡
    int prereq_count;
    int prereq_ids[MAX_PREREQ];

    //ê³¼ëª©?˜ ?—°?„ë³?, ?•™ê¸°ë³„ ?„±?  ?†µê³? ? •ë³?
    int stats_count;
    SubjectStats stats[MAX_SUBJECT_STATS];
} SubjectInfo;

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

// ê³µìš©ì²?

typedef union {
    Subject *subject;
    TimeTable *time_table;
    TechTree *tech_tree;
    BoardPost *board_post;
} DataPointer;

// ?—´ê±°í˜•
typedef enum {
    MENU_STATE_MAIN,
    MENU_STATE_CALCULATOR,
    MENU_STATE_TECH_TREE,
    MENU_STATE_BOARD,
    MENU_STATE_EXIT
} MenuState;

#ifdef ERROR_FILE_NOT_FOUND
#undef ERROR_FILE_NOT_FOUND
#endif

typedef enum {
    SUCCESS,
    ERROR_FILE_NOT_FOUND,
    ERROR_INVALID_INPUT,
    ERROR_INDEX_OUT,
    ERROR_MEMORY_ALLOC
} StatusCode;

void load_user_data(User* user, int id, int *is_first) {
    char filename[100];
    sprintf(filename, "dataset/course_io/user/user_%d.txt", id);
    
    user->id = id;
    user->current_sem = 0; 
    for(int i=0; i<SEMESTER_NUM; i++) user->table[i] = NULL;

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        *is_first = 1; return;
    }

    *is_first = 0;
    
    char line[256];
    int current_sem_idx = -1;
    int subjects_to_read = 0;

    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        
        if (strncmp(line, "ID:", 3) == 0) {
            continue;
        }
        else if (strncmp(line, "CUR_SEM:", 8) == 0) {
            sscanf(line, "CUR_SEM:%d", &user->current_sem);
        }
        else if (strncmp(line, "SEM|", 4) == 0) {
            int sem_num, count;
            sscanf(line, "SEM|%d|%d", &sem_num, &count);
            
            current_sem_idx = sem_num - 1; 
            subjects_to_read = count;

            if (user->table[current_sem_idx] == NULL) {
                user->table[current_sem_idx] = (TimeTable*)calloc(1, sizeof(TimeTable));
            }
        }
        else if (current_sem_idx != -1 && subjects_to_read > 0) {
            char name[NAME_LENGTH];
            int credit, sub_id;
            
            if (sscanf(line, "%[^|]|%d|%d", name, &credit, &sub_id) == 3) {
                Subject* s = create_node(name, 1, credit);
                s->id = sub_id;
                s->semester = current_sem_idx + 1;
                
                TimeTable* t = user->table[current_sem_idx];
                if (t != NULL && t->n < MAX_SUBJECT_NUM) {
                    t->subjects[t->n++] = s;
                }
                subjects_to_read--;
            }
        }
    }
    fclose(fp);
}

void load_user_data(User* user, int id, int *is_first) {
    char filename[100];
    sprintf(filename, "dataset/course_io/user/user_%d.txt", id);
    
    user->id = id;
    user->current_sem = 0; 
    for(int i=0; i<SEMESTER_NUM; i++) user->table[i] = NULL;

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        *is_first = 1; return;
    }

    *is_first = 0;
    
    char line[256];
    int current_sem_idx = -1;
    int subjects_to_read = 0;

    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        
        if (strncmp(line, "ID:", 3) == 0) {
            continue;
        }
        else if (strncmp(line, "CUR_SEM:", 8) == 0) {
            sscanf(line, "CUR_SEM:%d", &user->current_sem);
        }
        else if (strncmp(line, "SEM|", 4) == 0) {
            int sem_num, count;
            sscanf(line, "SEM|%d|%d", &sem_num, &count);
            
            current_sem_idx = sem_num - 1; 
            subjects_to_read = count;

            if (user->table[current_sem_idx] == NULL) {
                user->table[current_sem_idx] = (TimeTable*)calloc(1, sizeof(TimeTable));
            }
        }
        else if (current_sem_idx != -1 && subjects_to_read > 0) {
            char name[NAME_LENGTH];
            int credit, sub_id;
            
            if (sscanf(line, "%[^|]|%d|%d", name, &credit, &sub_id) == 3) {
                Subject* s = create_node(name, 1, credit);
                s->id = sub_id;
                s->semester = current_sem_idx + 1;
                
                TimeTable* t = user->table[current_sem_idx];
                if (t != NULL && t->n < MAX_SUBJECT_NUM) {
                    t->subjects[t->n++] = s;
                }
                subjects_to_read--;
            }
        }
    }
    fclose(fp);
}

Subject* copy_subject(Subject* original) {
    Subject* copy = (Subject*)malloc(sizeof(Subject));
    if (copy == NULL) return NULL;
    *copy = *original; 
    copy->n = 0; 
    for(int i=0; i<MAX_SUBJECT_NUM; i++) copy->arr[i] = NULL;
    return copy;
}

// dummy data


#endif //STRUCT_H