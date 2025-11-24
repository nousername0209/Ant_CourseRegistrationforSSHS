//
// Created by 김주환 on 25. 11. 9.
//

#include "../struct.h" 

#ifndef DIFFICULTY_CALCULATOR_H
#define DIFFICULTY_CALCULATOR_H

StatusCode preprocess_load(double (*Load)[MAX_SUBJECT_NUM]);
StatusCode preprocess_synergy(double (*Synergy)[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM]);
StatusCode calculate_difficulty(TimeTable* table);
StatusCode add_difficulty_db(const TimeTable *table);

#endif //DIFFICULTY_CALCULATOR_H
