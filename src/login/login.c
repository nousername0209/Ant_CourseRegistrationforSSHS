#include "login.h"

#ifdef _WIN32
#include <windows.h>
#endif

static int read_login_key(void) {
#ifdef _WIN32
    int ch = _getch();
    if (ch == 0 || ch == 224)
        ch = _getch();
#else
    int ch = getch();
    if (ch == 0 || ch == 224)
        ch = getch();
#endif
    return ch;
}

StatusCodeEnum draw_ui(SelectEnum select, const char *id_buffer) {
    system("cls");

    print_center("=== LOGIN ===", 13, 10);

    print_center("User ID: ", 33, 12);
    if (select == ID_FIELD) {
        printf("%s> [%-20s]%s", UI_REVERSE, id_buffer, UI_RESET);
        goto_ansi(START_X + (UI_WIDTH - 33) / 2 + (int)strlen(id_buffer), 12);
    } else {
        printf("  [%-20s]", id_buffer);
    }

    goto_ansi(START_X + (UI_WIDTH - 16) / 2, 15);
    if (select == LOGIN_BUTTON) {
        printf("%s[ >> LOGIN << ]%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[    LOGIN    ]");
    }
    fflush(stdout);

    return SUCCESS;
}

StatusCodeEnum show_login_screen(char *id_buffer, int max_len) {
    SelectEnum current = ID_FIELD;
    int id_index = 0;
    id_buffer[0] = '\0';

    while (1) {
        StatusCodeEnum draw_status = draw_ui(current, id_buffer);
        (void)draw_status;

        int ch = read_login_key();

        switch (ch) {
        case 224:
        case 0:
            ch = read_login_key();
            if (ch == UP_ARROW || ch == LEFT_ARROW) {
                current = (current == ID_FIELD) ? LOGIN_BUTTON : ID_FIELD;
            } else if (ch == DOWN_ARROW || ch == RIGHT_ARROW) {
                current = (current == ID_FIELD) ? LOGIN_BUTTON : ID_FIELD;
            }
            break;

        case ENTER:
            if (current == LOGIN_BUTTON)
                return SUCCESS;
            break;

        case BACKSPACE:
            if (current == ID_FIELD && id_index > 0) {
                id_index--;
                id_buffer[id_index] = '\0';
            }
            break;

        default:
            if (current == ID_FIELD && id_index < max_len - 1 && ch >= '0' && ch <= '9') {
                id_buffer[id_index] = (char)ch;
                id_index++;
                id_buffer[id_index] = '\0';
            }
            break;
        }
    }

    return SUCCESS;
}

StatusCodeEnum login(int *user_id) {
    system("cls");
    char id_str[ID_LENGTH];
    StatusCodeEnum status = show_login_screen(id_str, ID_LENGTH);
    (void)status;
    *user_id = atoi(id_str);
    return SUCCESS;
}
