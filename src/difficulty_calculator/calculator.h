//  2025-11-22 ±Ë¡÷»Ø: calculator.h ¿€º∫

#include "../struct.h" 

#ifndef DIFFICULTY_CALCULATOR_H
#define DIFFICULTY_CALCULATOR_H

StatusCodeEnum preprocess_load(double (*Load)[MAX_SUBJECT_NUM]);
StatusCodeEnum preprocess_synergy(double (*Synergy)[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM]);
StatusCodeEnum calculate_difficulty(TimeTable* table);

#endif //DIFFICULTY_CALCULATOR_H
