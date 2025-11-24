#include "output.h"

void popup_show_message(const char* title, const char* msg) {
    int box_w = 40;
    int box_h = 7;
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 10;

    // 배경 지우기 및 박스
    for(int i=0; i<box_h; i++) {
        goto_ansi(start_x, start_y + i);
        printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET); 
    }

    goto_ansi(start_x + 2, start_y + 2);
    printf("%s[%s]%s", UI_BOLD, title, UI_RESET);
    
    goto_ansi(start_x + 2, start_y + 3);
    printf("%s%s%s", UI_REVERSE, msg, UI_RESET);

    goto_ansi(start_x + 2, start_y + 5);
    printf("%s[ 엔터 키를 눌러 확인 ]%s", UI_DIM, UI_RESET);

    while(1) {
        int ch = _getch();
        if (ch == ENTER || ch == ESC) break;
    }
    
    // 팝업 닫기 (화면 복구는 draw_screen에서 처리됨)
}

void popup_show_difficulty_result(int sem, Subject *load, Subject (*synergy)[2], double diff) {
    int box_w = 50;
    int box_h = 12;
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 8;

    // 배경
    for(int i=0; i<box_h; i++) {
        goto_ansi(start_x, start_y + i);
        printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET); 
    }

    goto_ansi(start_x + 2, start_y + 1);
    printf("%s %d학기 난이도 분석 결과 %s", UI_BOLD, sem, UI_RESET);

    goto_ansi(start_x + 2, start_y + 3);
    printf("%s1. 최고 난이도 과목 (argmax_load)%s", UI_COLOR_YELLOW, UI_RESET);
    goto_ansi(start_x + 5, start_y + 4);
    printf("-> %s (%d학점)", load->name, load->credit);

    goto_ansi(start_x + 2, start_y + 6);
    printf("%s2. 최악의 조합 (argmax_synergy)%s", UI_COLOR_YELLOW, UI_RESET);
    goto_ansi(start_x + 5, start_y + 7);
    if (strcmp(synergy[0]->name, "N/A") == 0) {
        printf("-> 과목이 부족하여 분석 불가");
    } else {
        printf("-> %s + %s", (*synergy)[0].name, (*synergy)[1].name);
    }

    goto_ansi(start_x + 2, start_y + 9);
    printf("%s3. 총 난이도 (total_difficulty)%s: %.1f / 10.0", UI_COLOR_YELLOW, UI_RESET, diff);

    goto_ansi(start_x + 12, start_y + 11);
    printf("%s[ 확인 ]%s", UI_BOLD, UI_RESET);

    while(1) {
        int ch = _getch();
        if (ch == ENTER || ch == ESC) break;
    }
}

void draw_output_view(User* user, int btn_idx) {
    system("cls");

    // 1. 헤더
    printf("================================================================================\n");
    printf("  내 시간표 확인                                    %sID: %5d%s\n", 
            UI_BOLD, user->id, UI_RESET);
    printf("================================================================================\n");

    int start_table_row = 4;
    int box_w = 26; // 컬럼 너비

    // 2. 학기별 시간표 출력 (2행 3열 그리드)
    for (int sem = 0; sem < SEMESTER_NUM; sem++) {
        int col = sem % 3;
        int row = sem / 3;
        int box_x = 2 + (col * box_w);
        int box_y = start_table_row + (row * 8); // 간격 넉넉하게

        goto_ansi(box_x, box_y);
        // 학기 타이틀
        if ((sem + 1) <= user->current_sem) 
            printf("%s[%d 학기 (이수)]%s", UI_DIM, sem + 1, UI_RESET);
        else 
            printf("%s[%d 학기 (수강)]%s", UI_COLOR_CYAN, sem + 1, UI_RESET);

        TimeTable* t = user->table[sem];
        int sem_credits = 0;

        if (t == NULL || t->n == 0) {
            goto_ansi(box_x, box_y + 1);
            printf("%s(비어있음)%s", UI_DIM, UI_RESET);
        } else {
            for (int i = 0; i < t->n; i++) {
                if (i >= 4) { // 6개까지만 표시
                    goto_ansi(box_x, box_y + 1 + i);
                    printf("...외 %d건", t->n - i);
                    break;
                }
                goto_ansi(box_x, box_y + 1 + i);
                printf("- %s (%d)", t->subjects[i]->name, t->subjects[i]->credit);
                sem_credits += t->subjects[i]->credit;
            }
        }
        
        // 학기별 학점 합계
        goto_ansi(box_x + 16, box_y);
        printf("(%d학점)", sem_credits);
    }

    // 3. 난이도 및 통계 영역 (하단)
    int stats_y = 21;
    goto_ansi(2, stats_y);
    printf("--------------------------------------------------------------------------------");
    
    double diff_score = 100;

    // 4. 하단 버튼 영역
    int btn_y = 26;
    int btn_spacing = 30;
    int btn_start_x = (CONSOLE_WIDTH - (btn_spacing * 2)) / 2 + 5;
    const char* labels[3] = { " 시간표 수정 ", " 난이도 분석 ", " 데이터 추가 ", "  홈으로   "};

    for (int i = 0; i < 3; i++) {
        goto_ansi(btn_start_x + (i * btn_spacing), btn_y);
        if (btn_idx == i) {
            printf("%s [ %s ] %s", UI_REVERSE, labels[i], UI_RESET);
        } else {
            printf(" [ %s ] ", labels[i]);
        }
    }
}

Page run_output(int student_id) {
    User user;
    int is_first;
    
    load_user_data(&user, student_id, &is_first);

    int btn_idx = 0; // 0:수정, 1:계산, 2:추가, 3:홈
    int ch;

    while (1) {
        draw_output_view(&user, btn_idx);
        
        goto_ansi(1, 1);
        ch = _getch();

        if (ch == 224 || ch == 0) {
            ch = _getch();
            if (ch == LEFT_ARROW) {
                if (btn_idx > 0) btn_idx--;
            }
            else if (ch == RIGHT_ARROW) {
                if (btn_idx < 3) btn_idx++;
            }
        }
        else if (ch == ENTER) {
            if (btn_idx == 0) {
                return INPUT_PAGE; // 시간표 수정
            } 
            else if (btn_idx == 1 || btn_idx == 2) { 
                // [난이도 계산] 또는 [데이터 추가]
                
                // 1. 학기 선택 (course_reg.c에 있는 함수 재사용)
                int selected_sem = popup_select_semester(1, SEMESTER_NUM, "작업할 학기 선택");
                
                if (selected_sem != -1) {
                    TimeTable* t = user.table[selected_sem - 1];
                    
                    // 데이터가 없는 경우 처리
                    if (t == NULL || t->n == 0) {
                        popup_show_message("오류", "선택한 학기에 등록된 과목이 없습니다.");
                    } 
                    else {
                        if (btn_idx == 1) { 
                            // [난이도 계산] 로직
                            Subject argmax_load;
                            Subject argmax_synergy[2];
                            double total_diff;
                            
                            // calculate_difficulty(t, &argmax_load, &argmax_synergy, &total_diff);
                            popup_show_difficulty_result(selected_sem, &argmax_load, &argmax_synergy, total_diff);
                        } 
                        else { 
                            // [데이터 추가] 로직
                            add_difficulty_db(t);
                            popup_show_message("완료", "데이터베이스에 성공적으로 추가되었습니다.");
                        }
                    }
                }
            } 
            else { // btn_idx == 3
                return HOME_PAGE;  // 홈으로
            }
        }
        else if (ch == ESC) {
            return HOME_PAGE;
        }
    }
}