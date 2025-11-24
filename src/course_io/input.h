//
// Created by ±Ë¡÷»Ø on 25. 11. 9.
//

#ifndef COURSE_INPUT_H
#define COURSE_INPUT_H

#include "../struct.h"

Subject* create_node(const char* name, int isFile, int credit);

Subject* copy_subject(Subject* original);

void add_child(Subject* parent, Subject* child);

void trim_newline(char* str);

Subject* load_data_from_file(const char* filename);

void save_user_data(User* user);

void load_user_data(User* user, int id, int *is_first);

int popup_select_semester(int start_sem, int end_sem, const char* title);

int popup_edit_menu();

void delete_subject(User* user, int sem_idx, int sub_idx);

void modify_subject(User* user, int old_sem_idx, int sub_idx, int new_sem);

void draw_screen(User* user, Subject* root, int col_idx, int row_indices[3], 
                 int focus_area, int btn_idx, int edit_mode, int timetable_select_idx);

void run_registration(int student_id);

// to main:
StatusCode input(int id);

#endif //COURSE_INPUT_H
