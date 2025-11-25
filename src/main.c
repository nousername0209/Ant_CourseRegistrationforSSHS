#include "main.h"

int main() {
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
                system("cls");
                print_center("=== HOME PAGE ===", 17, 10);
                print_center("[1] 시간표 입력/수정", 20, 12);
                print_center("[2] 난이도 분석 및 추가", 22, 13);
                print_center("[3] 수강 신청 게시판", 20, 14);
                print_center("[ESC] 종료", 10, 16);

                int ch = _getch();
                if (ch == '1') {
                    page = INPUT_PAGE;
                } else if (ch == '2') {
                    page = OUTPUT_PAGE;
                } else if (ch == '3') {
                    page = BOARD_PAGE;
                } else if (ch == ESC) {
                    flag = 1;
                }
                break;

            case ESC_PAGE:
                system("cls");
                flag = 1;
                break;
        }
        if (flag) break;
    }
}