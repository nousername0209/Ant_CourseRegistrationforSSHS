//
// Created by 김주환 on 25. 11. 9.
//

#ifndef COURSE_INPUT_H
#define COURSE_INPUT_H

#include "../struct.h"

StatusCode input_subject(Subject *result);

StatusCode input_time_table(TimeTable *result);

StatusCode input_total_time_table(TimeTable *result[SEMESTER_NUM]);

#endif //COURSE_INPUT_H
