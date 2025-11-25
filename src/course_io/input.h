#ifndef COURSE_INPUT_H
#define COURSE_INPUT_H

#include "../struct.h"

#define COL_WIDTH 25
#define COL_Y 4
#define TREE_DEPTH 3
#define BUTTON_NUM 3

typedef enum {
    TREE, BUTTON, EDIT
} FocusEnum;
typedef enum {
    MODE, SEM, SAVE
} ButtonEnum;

Subject* load_data_from_file();

void save_user_data(User* user);

void load_user_data(User* user, int id, int *is_first);

int popup_select_semester(int start_sem, int end_sem, const char* title);

int popup_edit_menu();

void delete_subject(User* user, int sem_idx, int sub_idx);

void modify_subject(User* user, int old_sem_idx, int sub_idx, int new_sem);

void draw_screen(User* user, Subject* root, int col_idx, int row_indices[3], 
                 FocusEnum focus_area, ButtonEnum btn_idx, int edit_mode, int timetable_select_idx);

void run_registration(int student_id);

// to main:
StatusCodeEnum input(int id);

#endif //COURSE_INPUT_H
