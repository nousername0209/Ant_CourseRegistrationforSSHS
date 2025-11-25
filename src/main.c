#include "main.h"

int main(void) {
    system("mode con cols=80 lines=30");

    int user_id;
    PageEnum page = LOGIN_PAGE;

    while (1) {
        int flag = 0;
        switch (page) {
        case LOGIN_PAGE:
            login(&user_id);
            page = HOME_PAGE;
            break;
        case INPUT_PAGE:
            input(user_id);
            page = OUTPUT_PAGE;
            break;
        case BOARD_PAGE:
            board_main(user_id);
            page = HOME_PAGE;
            break;
        case OUTPUT_PAGE:
            page = run_output(user_id);
            break;
        case HOME_PAGE:
            page = run_home_page();
            break;
        case ESC_PAGE:
            system("cls");
            flag = 1;
            break;
        }
        if (flag)
            break;
    }

    return 0;
}
