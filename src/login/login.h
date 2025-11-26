// 2025-11-20 이연지: 로그인 페이지 제작

#ifndef LOGIN_H
#define LOGIN_H

#define ID_LENGTH 50

#include "../struct.h"

StatusCodeEnum draw_ui(SelectEnum select, const char* id_buffer);
StatusCodeEnum show_login_screen(char *id_buffer, int max_len);
StatusCodeEnum login(int *user_id);


#endif //COURSE_INPUT_H