#include "output.h"
#include "../difficulty_calculator/calculator.h"

/**
 * @brief 난이도 입력 값이 유효한지 검사한다 (0~10 사이 정수)
 * 
 * @param buf 입력문자열(난이도가 적힌 문자열)을 입력받는다.
 * 
 * @return int 유효하면 1, 아니면 0 반환
 */
int is_valid_difficulty_input(const char* buf) {
    int val = atoi(buf);
    return (val >= 0 && val <= 10);
}

/**
 * @brief 난이도 입력 페이지를 그린다.
 * 
 * @param table 난이도 입력을 하는 시간표의 포인터
 * @param semester 학기 번호
 * @param total_difficulty_input 총 난이도 입력 값
 * @param subject_difficulties 과목별 난이도 입력 값 배열
 * @param current_idx 현재 선택된 입력 필드 인덱스 (-1: 총 난이도, 0~n-1: 과목 인덱스)
 * @param box_w 팝업 박스 너비
 * @param box_h 팝업 박스 높이
 * @param start_x 팝업 시작 X 좌표
 * @param start_y 팝업 시작 Y 좌표
 * 
 * @return void
 */
void draw_input_difficulty_popup(TimeTable* table, int semester, int total_difficulty_input, int subject_difficulties[], int current_idx, int box_w, int box_h, int start_x, int start_y) {
    system("cls");
    int cursor_y;

    goto_ansi(start_x + 2, start_y + 1);
    printf("%s[ %d학기 난이도 데이터 추가 ]%s (0~10점 입력, 숫자를 입력하세요)", UI_BOLD, semester, UI_RESET);
    
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
    cursor_y = start_y + 20;
    goto_ansi(start_x + 10, cursor_y);
    printf("%s[ ↑/↓: 이동 | 숫자: 입력 | ENTER: 저장 | ESC: 취소 ]%s", UI_BOLD, UI_RESET);
}

/**
 * @brief 입력된 title과 message를 cmd창에 출력해준다.
 * 
 * @param title 출력할 제목이다.
 * @param msg 출력할 메세지이다.
 */
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

/**
 * @brief 시간표의 난이도를 계산하고, 전체 난이도와 가장 힘든 과목& 과목쌍을 출력한다.
 * 
 * @param sem 선택한 학기(1~6중 하나의 값을 가진다.)
 * @param table 난이도를 계산할 시간표이다.
 */
void popup_show_difficulty_result(int sem, TimeTable *table) {
    int box_w = 50;
    int box_h = 12;
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 8;

    // 배경 지우기
    system("cls");

    goto_ansi(start_x + 2, start_y + 1);
    printf("%s %d학기 난이도 분석 결과 %s", UI_BOLD, sem, UI_RESET);

    goto_ansi(start_x + 2, start_y + 3);
    if (table->argmax_load == NULL) {
        printf("%s1. 최고 난이도 과목 (argmax_load)%s", UI_COLOR_YELLOW, UI_RESET);
        goto_ansi(start_x + 5, start_y + 4);
        printf("-> 과목이 없어 분석 불가");
    } else {
        goto_ansi(start_x + 2, start_y + 3);
        printf("%s1. 최고 난이도 과목 (argmax_load)%s", UI_COLOR_YELLOW, UI_RESET);
        goto_ansi(start_x + 5, start_y + 4);
        printf("-> %s (%d학점)", table->argmax_load->name, table->argmax_load->credit);
    }
    goto_ansi(start_x + 2, start_y + 6);
    printf("%s2. 최악의 조합 (argmax_synergy)%s", UI_COLOR_YELLOW, UI_RESET);
    goto_ansi(start_x + 5, start_y + 7);
    if (((*table->argmax_synergy)[0].id) == ((*table->argmax_synergy)[1].id)) {
        printf("-> 과목이 부족하여 분석 불가");
    } else {
        if(DEBUG_MODE){
            printf("%d %d\n", table->argmax_synergy[0]->id, (*table->argmax_synergy)[1].id);
        }
        printf("-> %s + %s", (*table->argmax_synergy)[0].name, (*table->argmax_synergy)[1].name);
    }

    goto_ansi(start_x + 2, start_y + 9);
    printf("%s3. 총 난이도 (total_difficulty)%s: %.1f / 10.0", UI_COLOR_YELLOW, UI_RESET, table->difficulty);

    goto_ansi(start_x + 12, start_y + 11);
    printf("%s[ 확인 ]%s", UI_BOLD, UI_RESET);

    while(1) {
        int ch = _getch();
        if (ch == ENTER || ch == ESC) break;
    }
}

/**
 * @brief 난이도 데이터 추가하는 창을 띄운다.
 * 
 * @param table 데이터에 추가할 시간표이다.
 * @param semester 추가할 학기를 선택한다.(홤녀에 출력 시 필요하다.)
 * 
 * @return 살행결과를 StatusCodeEnum으로 반환한다.
 */
StatusCodeEnum popup_input_difficulty(TimeTable* table, int semester) {
    if (table == NULL) return ERROR_INVALID_INPUT;

    // 팝업 설정
    int box_w = 70;
    int box_h = 7 + table->n * 2; // 총 난이도 입력 2줄 + 과목별 입력 2*n줄
    if (box_h > 25) box_h = 25; // 최대 높이 제한
    
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 3;

    
    char dir_path[PATH_LENGTH] = "./dataset/difficulty_calculator";
    char file_path[PATH_LENGTH];
    int num_of_data;
    int total_difficulty = -1;
    int subject_difficulties[MAX_SUBJECT_NUM];
    char input_buffer[STR_LENGTH];

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

        if (ch == PRE_INPUT1 || ch == PRE_INPUT2) {
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
            sprintf(file_path, "%s/data/data%03d.txt", dir_path, num_of_data);
            fp = fopen(file_path, "w");

            fprintf(fp, "%d %d\n", table->n, total_difficulty);
            for (int i = 0; i < table->n; i++) {
                fprintf(fp, "%d %d\n", table->subjects[i]-> id, subject_difficulties[i]);
            }
            fclose(fp);

            num_of_data++;

            sprintf(file_path, "%s/num_of_data.dat", dir_path);
            fp = fopen(file_path, "wb");
            fwrite(&num_of_data, sizeof(int), 1, fp);
            fclose(fp);
            
            double Load[MAX_SUBJECT_NUM], Synergy[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM];
            preprocess_load(&Load);
            preprocess_synergy(&Synergy);
            if (DEBUG_MODE){
                printf("Load result: \n");
                for(int i=0;i<MAX_SUBJECT_NUM;i++){
                    printf("%lf ", Load[i]);
                }
                printf("\nSynergy result: \n");
                for(int i=0;i<MAX_SUBJECT_NUM;i++){
                    for(int j=0;j<MAX_SUBJECT_NUM;j++){
                        printf("%lf ", Synergy[i][j]);
                    }
                    printf("\n");
                }
            }

            // 성공 메시지 팝업 후 종료
            popup_show_message("완료", "난이도 정보가 성공적으로 저장되었\n습니다.");
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
            }
        }
    }
}



/**
 * @brief 시간표 최종확인 후 시간표를 cmd창에 출력해준다.
 * 
 * @param user 시간표를 확인하는 사용자에 대응되는 User의 포인터이다.
 * @param semester 추가할 학기를 선택한다.(홤녀에 출력 시 필)
 * 
 * @return 살행결과를 StatusCodeEnum으로 반환한다.
 */
void draw_output_view(User* user, int btn_idx) {
    system("cls");

    // 1. 헤더
    printf("================================================================================\n");
    printf("  내 시간표 확인                                                 %sID: %5d%s\n", 
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
            printf("%s[%d 학기 (완료)]%s", UI_DIM, sem + 1, UI_RESET);
        else 
            printf("%s[%d 학기 (예정)]%s", UI_COLOR_CYAN, sem + 1, UI_RESET);

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
    const char* labels[4] = { "시간표 수정", "난이도 계산", "데이터 추가", "  홈으로  " };

    for (int i = 0; i < 4; i++) {
        goto_ansi(btn_start_x + (i * btn_spacing), btn_y);
        if (btn_idx == i) {
            printf("%s [ %s ] %s", UI_REVERSE, labels[i], UI_RESET);
        } else {
            printf(" [ %s ] ", labels[i]);
        }
    }
}

/**
 * @brief ouput을 실행하고 main 파일에 사용된다.
 * 
 * @param student_id 학생의 id이다.
 * 
 * @return 살행결과를 StatusCodeEnum으로 반환한다.
 */
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
                int selected_sem = popup_select_semester(1, SEMESTER_NUM, "작업할 학기 선택");
                
                if (selected_sem != -1) {
                    TimeTable* t = user.table[selected_sem - 1];
                    
                    // 데이터가 없는 경우 처리
                    if (t == NULL || t->n == 0) {
                        popup_show_message("오류", "선택한 학기에 등록된 과목이 없습니다.");
                    } 
                    else {
                        if (btn_idx == 1) {
                            StatusCodeEnum status = calculate_difficulty(t);
                            if (status != SUCCESS) {
                                popup_show_message("오류", "난이도 계산 중 오류가 발생했습니다.");
                            }
                            else popup_show_difficulty_result(selected_sem, t);
                        } 
                        else { 
                            // [데이터 추가] 로직 (신규 팝업 호출)
                            StatusCodeEnum status = popup_input_difficulty(t, selected_sem);
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