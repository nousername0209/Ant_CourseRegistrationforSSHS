#include "home.h"

static int read_home_key(void) {
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

static void render_home_page(int selected_index) {
    system("cls");

    const char *menu[] = {"[1] ðǥ Է/", "[2] ̵ м  ߰", "[3]  û Խ", "[4]  ID  (α)", "[ESC] "};
    const int menu_count = 5;
    const int start_y = 10;

    print_center("=== HOME PAGE ===", 17, 8);

    for (int i = 0; i < menu_count; i++) {
        if (i == 4) {
            print_center("", 0, start_y + 6);
            if (i == selected_index) {
                printf("%s", UI_REVERSE);
                print_center(menu[i], strlen(menu[i]), start_y + 6);
                printf("%s", UI_RESET);
            } else {
                print_center(menu[i], strlen(menu[i]), start_y + 6);
            }
        } else {
            print_center("", 0, start_y + i);

            if (i == selected_index) {
                printf("%s", UI_REVERSE);
                print_center(menu[i], strlen(menu[i]), start_y + i);
                printf("%s", UI_RESET);
            } else {
                print_center(menu[i], strlen(menu[i]), start_y + i);
            }
        }
    }
}

PageEnum run_home_page(void) {
    int home_selected_index = 0;
    const int menu_count = 5;

    while (1) {
        int ch;

        render_home_page(home_selected_index);
        ch = read_home_key();

        if (ch == UP_ARROW) {
            if (home_selected_index > 0) {
                home_selected_index--;
            } else {
                home_selected_index = menu_count - 1;
            }
        } else if (ch == DOWN_ARROW) {
            if (home_selected_index < menu_count - 1) {
                home_selected_index++;
            } else {
                home_selected_index = 0;
            }
        } else if (ch == ENTER) {
            if (home_selected_index == 0)
                return INPUT_PAGE;
            if (home_selected_index == 1)
                return OUTPUT_PAGE;
            if (home_selected_index == 2)
                return BOARD_PAGE;
            if (home_selected_index == 3)
                return LOGIN_PAGE;
            if (home_selected_index == 4)
                return ESC_PAGE;
        } else if (ch == ESC) {
            return ESC_PAGE;
        }
    }
}
