//
// Created by 김주환 on 25. 11. 9.
//

#ifndef ENROLLMENT_BOARD_H
#define ENROLLMENT_BOARD_H

#include "../struct.h"

StatusCodeEnum create_post(BoardPost *result);
StatusCodeEnum apply_post(BoardPost *post, int id);
StatusCodeEnum cancel_post(BoardPost *post, int id);
int is_user_applied(const BoardPost *post, int id);
int board_main(int user_id);

#endif // ENROLLMENT_BOARD_H
