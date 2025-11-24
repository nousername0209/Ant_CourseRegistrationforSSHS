#include "output.h"
#include "../difficulty_calculator/calculator.h"

int is_valid_difficulty_input(const char* buf) {
    int val = atoi(buf);
    return (val >= 0 && val <= 10);
}

void draw_input_difficulty_popup(TimeTable* table, int semester, int total_difficulty_input, int subject_difficulties[], int current_idx, int box_w, int box_h, int start_x, int start_y) {
    system("cls");
    int cursor_y;

    goto_ansi(start_x + 2, start_y + 1);
    printf("%s[ %d학기 난이도 데이터 추가 ]%s (0~10점 입력, 숫자를 입력하세요)", UI_BOLD, semester, UI_RESET);
    
    // 1. 총 난이도 입력 필드
    cursor_y = start_y + 3;
    goto_ansi(start_x + 2, cursor_y);
    if (current_idx == -1) printf("%s", UI_COLOR_CYAN); // 현재 선택 강조
    printf("총 시간표 난이도: ");
    
    goto_ansi(start_x + 30, cursor_y);
    if (total_difficulty_input != -1) {
        printf("%d", total_difficulty_input);
    } else {
         printf("___");
    }
    printf("%s", UI_RESET);

    // 2. 과목별 난이도 입력 필드
    for (int i = 0; i < table->n; i++) {
        cursor_y = start_y + 5 + i * 2;
        goto_ansi(start_x + 2, cursor_y);
        if (current_idx == i) printf("%s", UI_COLOR_CYAN); // 현재 선택 강조

        printf("과목 %s 난이도: ", table->subjects[i]->name);
        
        goto_ansi(start_x + 30, cursor_y);
        if (subject_difficulties[i] > 0) {
             printf("%d", subject_difficulties[i]);
        } else {
             printf("___");
        }
        printf("%s", UI_RESET);
    }

    // 3. 버튼 영역
    cursor_y = start_y + box_h - 2;
    goto_ansi(start_x + (box_w / 2) - 15, cursor_y);
    printf("%s[ ↑/↓: 이동 | 숫자: 입력 | ENTER: 저장 | ESC: 취소 ]%s", UI_BOLD, UI_RESET);
}

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
    if (strcmp((*synergy)[0].name, "N/A") == 0) {
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

// --- [신규 UI] 난이도 정보 입력 팝업 및 파일 저장 ---
// 요청하신 난이도 입력 로직과 파일 저장 로직을 통합합니다.
StatusCode popup_input_difficulty(TimeTable* table, int student_id, int semester) {
    if (table == NULL) return ERROR_INVALID_INPUT;

    // 팝업 설정
    int box_w = 70;
    int box_h = 7 + table->n * 2; // 총 난이도 입력 2줄 + 과목별 입력 2*n줄
    if (box_h > 25) box_h = 25; // 최대 높이 제한
    
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 3;

    
    char dir_path[PATH_LENGTH] = "./dataset/difficulty_calculator";
    char file_path[PATH_LENGTH + 40];
    int num_of_data;
    int total_difficulty = -1;
    int subject_difficulties[MAX_SUBJECT_NUM];
    char input_buffer[10];

    FILE *fp = NULL;
    sprintf(file_path, "%s/num_of_data.dat", dir_path);
    fp = fopen(file_path, "rb");
    if(fp == NULL){
        num_of_data = 0;
        fp = fopen(file_path, "wb");
        fwrite(&num_of_data, sizeof(int), 1, fp);
    }
    else{
        fread(&num_of_data, sizeof(int), 1, fp);
    }
    fclose(fp); 

    // 초기값 설정
    int current_input_idx = -1; // -1: 총 난이도, 0~n-1: 과목별 난이도
    for (int i = 0; i < table->n; i++) subject_difficulties[i] = 0;

    // 입력 필드 순회
    while (1) {
        // draw_popup 대신 외부 함수 호출
        draw_input_difficulty_popup(table, semester, total_difficulty, subject_difficulties, 
                                    current_input_idx, box_w, box_h, start_x, start_y);

        // 입력 대기
        int ch = _getch();

        if (ch == ESC) return SUCCESS; // 취소

        if (ch == 224 || ch == 0) {
            ch = _getch();
            if (ch == UP_ARROW) {
                if (current_input_idx > -1) current_input_idx--;
            } else if (ch == DOWN_ARROW) {
                if (current_input_idx < table->n - 1) current_input_idx++;
            }
        } else if (ch == ENTER) {
            // 모든 필드 입력 확인
            if (total_difficulty == -1) {
                 popup_show_message("오류", "총 난이도를 입력해 주세요.");
                 continue;
            }
            int all_subjects_entered = 1;
            for(int i=0; i<table->n; i++) {
                if (subject_difficulties[i] == 0) {
                    all_subjects_entered = 0;
                    break;
                }
            }
            if (!all_subjects_entered) {
                 popup_show_message("오류", "모든 과목의 난이도를 입력해 주세요.");
                 continue;
            }
            
            // 파일 저장 로직 실행
            sprintf(file_path, "%s/data/data%:03d.txt", dir_path, num_of_data);
            fp = fopen(file_path, "w");

            fprintf(fp, "%d %d\n", table->n, total_difficulty);
            for (int i = 0; i < table->n; i++) {
                fprintf(fp, "%d %d\n", table->subjects[i]->id, subject_difficulties[i]);
            }
            fclose(fp);

            // 성공 메시지 팝업 후 종료
            popup_show_message("완료", "난이도 정보가 성공적으로 저장되었습니다.");
            return SUCCESS;
        } else if (ch >= '0' && ch <= '9') {
            // 숫자 입력 처리 (단일 숫자 입력만 가능하다고 가정)
            input_buffer[0] = (char)ch;
            input_buffer[1] = '\0';

            // is_valid_input 대신 외부 함수 호출
            if (is_valid_difficulty_input(input_buffer)) {
                int input_val = atoi(input_buffer);
                if (current_input_idx == -1) {
                    total_difficulty = input_val;
                } else {
                    subject_difficulties[current_input_idx] = input_val;
                }
            } else {
                 // 0~10 범위 밖 입력에 대한 처리 (필요시)
            }
        }
    }
}


// 출력 화면 그리기
void draw_output_view(User* user, int btn_idx) {
    system("cls");

    // 1. 헤더
    printf("================================================================================\n");
    printf("  내 시간표 확인                                    %sID: %5d%s\n", 
            UI_BOLD, user->id, UI_RESET);
    printf("================================================================================\n");

    int start_table_row = 4;
    int box_w = 26; 

    // 2. 학기별 시간표 출력
    for (int sem = 0; sem < SEMESTER_NUM; sem++) {
        int col = sem % 3;
        int row = sem / 3;
        int box_x = 2 + (col * box_w);
        int box_y = start_table_row + (row * 8); 

        goto_ansi(box_x, box_y);
        if ((sem + 1) <= user->current_sem) 
            printf("%s[ %d 학기 (이수 완료) ]%s", UI_DIM, sem + 1, UI_RESET);
        else 
            printf("%s[ %d 학기 (수강 예정) ]%s", UI_COLOR_CYAN, sem + 1, UI_RESET);

        TimeTable* t = user->table[sem];
        int sem_credits = 0;

        if (t == NULL || t->n == 0) {
            goto_ansi(box_x, box_y + 1);
            printf("%s(비어있음)%s", UI_DIM, UI_RESET);
        } else {
            for (int i = 0; i < t->n; i++) {
                if (i >= 6) { 
                    goto_ansi(box_x, box_y + 1 + i);
                    printf("...외 %d건", t->n - i);
                    break;
                }
                goto_ansi(box_x, box_y + 1 + i);
                printf("- %s (%d)", t->subjects[i]->name, t->subjects[i]->credit);
                sem_credits += t->subjects[i]->credit;
            }
        }
        goto_ansi(box_x + 16, box_y);
        printf("(%d학점)", sem_credits);
    }

    // 3. 하단 버튼 영역 (4개 버튼)
    int btn_y = 23;
    int btn_spacing = 18;
    int btn_start_x = (CONSOLE_WIDTH - (btn_spacing * 4)) / 2 + 2;
    
    // 버튼 라벨
    const char* labels[4] = { "시간표 수정", "난이도 계산", "데이터 추가", "홈으로" };

    for (int i = 0; i < 4; i++) {
        goto_ansi(btn_start_x + (i * btn_spacing), btn_y);
        if (btn_idx == i) {
            printf("%s [ %s ] %s", UI_REVERSE, labels[i], UI_RESET);
        } else {
            printf(" [ %s ] ", labels[i]);
        }
    }
}

// Output 모드 실행 함수
int run_output(int student_id) {
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
                            
                            calculate_difficulty(t);
                            popup_show_difficulty_result(selected_sem, t->argmax_load, t->argmax_synergy, t->difficulty);
                        } 
                        else { 
                            // [데이터 추가] 로직 (신규 팝업 호출)
                            StatusCode status = popup_input_difficulty(t, student_id, selected_sem);
                            if (status == ERROR_FILE_NOT_FOUND) {
                                popup_show_message("오류", "파일 저장 경로에 접근할 수 없습니다.");
                            }
                            // Note: popup_input_difficulty 내부에서 성공 메시지를 띄움
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