// 이연지 작업

#ifndef COURSE_OUTPUT_H
#define COURSE_OUTPUT_H

#include "../struct.h"
#include "../difficulty_calculator/calculator.h"

int is_valid_difficulty_input(const char* buf);

void draw_input_difficulty_popup(TimeTable* table, int semester, int total_difficulty_input, int subject_difficulties[], int current_idx, int box_w, int box_h, int start_x, int start_y);

void popup_show_message(const char* title, const char* msg);

void popup_show_difficulty_result(int sem, TimeTable *table);

StatusCodeEnum popup_input_difficulty(TimeTable* table, int semester);

void draw_output_view(User* user, int btn_idx);

int run_output(int student_id);

#endif //COURSE_OUTPUT_H