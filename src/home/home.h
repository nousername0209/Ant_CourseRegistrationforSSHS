// 2025-11-24 이연지: home.h 제작

#ifndef HOME_H
#define HOME_H

#include "../struct.h"

StatusCodeEnum draw_ui(SelectEnum select, const char* id_buffer);

StatusCodeEnum show_login_screen(char *id_buffer, int max_len);

StatusCodeEnum login(int *user_id);

#endif //HOME_H
