//
// Created by 김주환 on 25. 11. 9.
//

#ifndef ENROLLMENT_BOARD_H
#define ENROLLMENT_BOARD_H

#include "../struct.h"

StatusCode create_post(BoardPost *result);

StatusCode apply_post(BoardPost *post, int id);

StatusCode cancel_post(BoardPost *post, int id);

int is_user_applied(const BoardPost *post, int id);

#endif //ENROLLMENT_BOARD_H