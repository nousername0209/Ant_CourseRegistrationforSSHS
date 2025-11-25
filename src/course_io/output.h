#ifndef COURSE_OUTPUT_H
#define COURSE_OUTPUT_H

#include "../difficulty_calculator/calculator.h"
#include "../struct.h"

StatusCodeEnum popup_input_difficulty(TimeTable *table, int student_id, int semester);
int run_output(int student_id);

#endif // COURSE_OUTPUT_H
