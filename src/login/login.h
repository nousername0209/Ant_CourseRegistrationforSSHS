#ifndef LOGIN_H
#define LOGIN_H

#define ID_LENGTH 50

#include "../struct.h"

typedef enum _Select {ID_FIELD, LOGIN_BUTTON} Select;

StatusCodeEnum draw_ui(Select select, const char* id_buffer);
StatusCodeEnum show_login_screen(char *id_buffer, int max_len);

// to main:
StatusCodeEnum login(int *user_id);


#endif //COURSE_INPUT_H