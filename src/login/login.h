#ifndef LOGIN_H
#define LOGIN_H

#include "../struct.h"

#define ID_LENGTH 50

StatusCodeEnum draw_ui(SelectEnum select, const char *id_buffer);
StatusCodeEnum show_login_screen(char *id_buffer, int max_len);
StatusCodeEnum login(int *user_id);

#endif // LOGIN_H
