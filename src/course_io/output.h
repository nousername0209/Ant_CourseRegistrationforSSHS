//
// Created by 김주환 on 25. 11. 9.
//

#ifndef COURSE_OUTPUT_H
#define COURSE_OUTPUT_H

#include "../struct.h"

StatusCode save_time_table(const TimeTable *table, const char path[PATH_LENGTH]);

StatusCode save_total_time_table(const TimeTable *table[SEMESTER_NUM], const char path[PATH_LENGTH]);

StatusCode print_time_table(const TimeTable *table);

StatusCode print_total_time_table(const TimeTable *table[SEMESTER_NUM]);

StatusCode preprocess_load(double (*Load)[MAX_SUBJECT_NUM], const char dir_path[PATH_LENGTH]);

StatusCode preprocess_synergy(double (*Synergy)[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM], const char dir_path[PATH_LENGTH]);

StatusCode calculate_difficulty(const TimeTable* table, const double (*Load)[MAX_SUBJECT_NUM], const double (*Synergy)[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM], Subject *argmax_load, Subject (*argmax_synergy)[2]);

StatusCode add_difficulty_db(const char path[PATH_LENGTH]);

#endif //COURSE_OUTPUT_H
