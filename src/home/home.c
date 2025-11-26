#include "home.h"

// HOME_PAGE 화면을 그리는 헬퍼 함수
static void render_home_page(int selected_index) {
    system("cls");
    
    // 메뉴 항목 정의
    const char *menu[] = {
        "[1] 시간표 입력/수정",
        "[2] 난이도 분석 및 추가",
        "[3] 수강 신청 게시판",
        "[4] 성적 입력 및 테크 트리 추천",
        "[5] 사용자 ID 변경 (로그인)",
        "[ESC] 종료"
    };
    const int menu_count = 6;
    const int start_y = 10;

    print_center("=== HOME PAGE ===", 17, 8);
    
    // 메뉴 항목 출력
    for (int i = 0; i < menu_count; i++) {
        // 종료 항목은 다른 위치에 출력
        if (i == 5) {
             print_center("", 0, start_y + 6); // 커서 위치 초기화
             if (i == selected_index) {
                printf("%s", UI_REVERSE);
                print_center(menu[i], strlen(menu[i]), start_y + 6);
                printf("%s", UI_RESET);
            } else {
                print_center(menu[i], strlen(menu[i]), start_y + 6);
            }
        } else {
            print_center("", 0, start_y + i); // 커서 위치 초기화
            
            // 선택된 항목에 역상 적용
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

// HOME_PAGE 컨트롤러 (다음 페이지 상태를 반환)
PageEnum run_home_page() {
    int home_selected_index = 0;
    const int menu_count = 6;
    int ch;
    
    while (1) {
        render_home_page(home_selected_index);
        
        ch = read_key();

        if (ch == UP_ARROW) {
            if (home_selected_index > 0) home_selected_index--;
            else home_selected_index = menu_count - 1; // 순환
        } else if (ch == DOWN_ARROW) {
            if (home_selected_index < menu_count - 1) home_selected_index++;
            else home_selected_index = 0; // 순환
        } else if (ch == ENTER) {
            if (home_selected_index == 0) return INPUT_PAGE;
            if (home_selected_index == 1) return OUTPUT_PAGE;
            if (home_selected_index == 2) return BOARD_PAGE;
            if (home_selected_index == 3) return TECHTREE_PAGE;
            if (home_selected_index == 4) return LOGIN_PAGE;
            if (home_selected_index == 5) return ESC_PAGE;
        } else if (ch == ESC) {
            return ESC_PAGE;
        }
    }
}