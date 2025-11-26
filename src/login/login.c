#include "login.h"
#include "windows.h"

/**
 * @brief 기본적인 로그인 UI를 띄워준다.(매 틱마다 실행되어야 함)
 * @param select 현재 커서가 올라와 있는 항목이다.(로그인/ 이름 수정)
 * @param id_buffer 현재까지 입력한 id의 문자열이다.
 * @return 성공 여부를 StatusCodeEnum으로 반환한다. 항상 SUCCESS를 반환한다.
 */
StatusCodeEnum draw_ui(SelectEnum select, const char* id_buffer) {
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

/**
 * @brief 로그인 UI를 띄우고 로그인 정보를 입력받는 함수이다.
 * @param id_buffer 입력받은 id를 저장하고, 반환하는 문자열이다. 해당 포인터에 아이디가 저장된다.
 * @param max_len 입력받을 id의 최대 길이이다.
 * @return 성공 여부를 StatusCodeEnum으로 반환한다. 항상 SUCCESS를 반환한다.
 */
StatusCodeEnum show_login_screen(char *id_buffer, int max_len) {
    SelectEnum current = ID_FIELD;
    int id_index = 0;
    id_buffer[0] = '\0';
    
    KeyEnum ch;
    while (1) {
        StatusCodeEnum draw_ui_s = draw_ui(current, id_buffer);

        ch = _getch(); 

        switch (ch) {
            case PRE_INPUT1:
            case PRE_INPUT2:
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

/**
 * @brief 로그인 UI를 띄우고 로그인 정보를 입력받는 함수이다. show_login_screen에 필요한 기능들을 정리해놓은 함수이다.
 * @param user_id 입력받은 id를 저장하고, 반환하는 문자열이다. 해당 포인터에 아이디가 저장된다.
 * @return 성공 여부를 StatusCodeEnum으로 반환한다. 항상 SUCCESS를 반환한다.
 */
StatusCodeEnum login(int *user_id) {
    system("cls");
    char id_str[ID_LENGTH];
    StatusCodeEnum s_show_login_screen = show_login_screen(id_str, ID_LENGTH);
    *user_id = atoi(id_str);
    return SUCCESS;
}