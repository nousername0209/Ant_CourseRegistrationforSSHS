#include "login.h"
#include "windows.h"

StatusCode draw_ui(Select select, const char* id_buffer) {
    system("cls");
    
    print_center("=== LOGIN ===", 13, 10);
    
    print_center("User ID: ", 33, 12);
    if (select == ID_FIELD) {
        printf("%s> [%-20s]%s", UI_REVERSE, id_buffer, UI_RESET);
        goto_ansi(START_X + (UI_WIDTH - 33) / 2 + (int)strlen(id_buffer), 12);
    } else {
        printf("  [%-20s]", id_buffer);
    }

    goto_ansi(START_X + (UI_WIDTH - 16)/2, 15);
    if (select == LOGIN_BUTTON) {
        printf("%s[ >> LOGIN << ]%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[    LOGIN    ]");
    }
    fflush(stdout);

    return SUCCESS;
}

StatusCode show_login_screen(char *id_buffer, int max_len) {
    Select current = ID_FIELD;
    int id_index = 0;
    id_buffer[0] = '\0';
    
    Key ch;
    while (1) {
        StatusCode draw_ui_s = draw_ui(current, id_buffer);

        ch = _getch(); 

        switch (ch) {
            case 224:
            case 0:
                ch = _getch();
                if (ch == UP_ARROW || ch == LEFT_ARROW) { // Up or Left
                    current = (current == ID_FIELD) ? LOGIN_BUTTON : ID_FIELD;
                } else if (ch == DOWN_ARROW || ch == RIGHT_ARROW) { // Down or Right
                    current = (current == ID_FIELD) ? LOGIN_BUTTON : ID_FIELD;
                }
                break;

            case ENTER:
                if (current == LOGIN_BUTTON) return SUCCESS;
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

int login() {
    char user_id[ID_LENGTH];

    // HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // SMALL_RECT windowSize = {0, 0, 80, 10};
    // SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
    system("mode con cols=80 lines=30");

    StatusCode s_show_login_screen = show_login_screen(user_id, ID_LENGTH);

    system("cls");

    return atoi(user_id);
}