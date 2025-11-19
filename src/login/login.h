#ifndef LOGIN_H
#define LOGIN_H

#define ID_LENGTH 50

#include "../struct.h"

StatusCode find_user(const char path[PATH_LENGTH], User *user);

typedef enum _Select {ID_FIELD, LOGIN_BUTTON} Select;

#endif //COURSE_INPUT_H