#include "input.h"

#define COL_WIDTH 25
#define COL_Y 4

Subject *load_data_from_file(const char *filename) {
    FILE *fp = fopen(filename, "r");

    Subject *root = create_node("ROOT", 0, 0);
    Subject *parents[10];
    parents[0] = root;

    char line[256];
    int depth, isFile, credit, sub_id;
    char name_buffer[NAME_LENGTH];

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d|%d|%d|%d|%[^\n]", &depth, &isFile, &credit, &sub_id, name_buffer) ==
            5) {
            trim_newline(name_buffer);
            Subject *new_node = create_node(name_buffer, isFile, credit);
            new_node->id = sub_id;
            if (depth > 0 && depth < 10) {
                add_child(parents[depth - 1], new_node);
                parents[depth] = new_node;
            }
        }
    }
    fclose(fp);
    return root;
}

void save_user_data(User *user) {
    char filename[100];
    sprintf(filename, "dataset/course_io/user/user_%d.txt", user->id);

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        system("cls");

            fprintf(fp, "SEM|%d|%d\n", i + 1, user->table[i]->n);

                Subject *s = user->table[i]->subjects[j];
                if (s == NULL)
                    continue;

int popup_select_semester(int start_sem, int end_sem, const char *title) {
    if (start_sem > end_sem)
        return -1;
    for (int i = 0; i < box_h; i++) {
        printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET);
    while (1) {
        printf("%s %s %s", UI_BOLD, title, UI_RESET);
        for (int i = 0; i < count; i++) {


            if (ch == UP_ARROW && selected_idx > 0)
                selected_idx--;
            if (ch == DOWN_ARROW && selected_idx < count - 1)
                selected_idx++;
        } else if (ch == ENTER) {
            for (int i = 0; i < box_h; i++) {
                printf("%*s", box_w, "");
        } else if (ch == ESC) {
            for (int i = 0; i < box_h; i++) {
                printf("%*s", box_w, "");
            return -1;

    int selected_idx = 0;
    const char *menus[] = {"1. б ", "2.  ", "3. "};

    for (int i = 0; i < box_h; i++) {
        printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET);
    while (1) {
        printf("%s   %s", UI_BOLD, UI_RESET);
        for (int i = 0; i < menu_cnt; i++) {

            if (ch == UP_ARROW && selected_idx > 0)
                selected_idx--;
            if (ch == DOWN_ARROW && selected_idx < menu_cnt - 1)
                selected_idx++;
            for (int i = 0; i < box_h; i++) {
                printf("%*s", box_w, "");
            for (int i = 0; i < box_h; i++) {
                printf("%*s", box_w, "");
void delete_subject(User *user, int sem_idx, int sub_idx) {
    TimeTable *t = user->table[sem_idx];
    if (t == NULL || sub_idx >= t->n)
        return;

        t->subjects[i] = t->subjects[i + 1];
void modify_subject(User *user, int old_sem_idx, int sub_idx, int new_sem) {
    if (old_sem_idx == new_sem - 1)
        return; //  б 
    TimeTable *old_t = user->table[old_sem_idx];
    if (old_t == NULL || sub_idx >= old_t->n)
        return;
    Subject *target = old_t->subjects[sub_idx];

        user->table[new_sem_idx] = (TimeTable *)calloc(1, sizeof(TimeTable));
    TimeTable *new_t = user->table[new_sem_idx];

            old_t->subjects[i] = old_t->subjects[i + 1];

void draw_screen(User *user, Subject *root, int col_idx, int row_indices[3], int focus_area,
                 int btn_idx, int edit_mode, int timetable_select_idx) {
    printf("  û | %s%s%s |  ̼ б: %dб                     %sID: %5d%s\n",
           color_code, mode_str, UI_RESET, user->current_sem, UI_BOLD, user->id, UI_RESET);
    Subject *cols[3] = {root, NULL, NULL};
    if (cols[0]->n > 0 && row_indices[0] < cols[0]->n)
    if (cols[1] != NULL && cols[1]->isFile == 0 && cols[1]->n > 0 && row_indices[1] < cols[1]->n)

        if (c == col_idx && focus_area == 0)
        else
        if (c < 2)
            printf("|");
        if (cols[c] == NULL || cols[c]->isFile == 1)
            continue;
            Subject *item = cols[c]->arr[i];

            if (item->isFile == 1)
                sprintf(display_name, "%s(%d)", item->name, item->credit);
            else
                strcpy(display_name, item->name);
                if (item->isFile == 0)
                    printf(" >");
            } else if (c < col_idx && i == row_indices[c]) {
            } else {

    const char *labels[3] = {"     ", "̼ б ", "    Ȯ     "};
    int table_y = btn_y + 3;


        if ((sem + 1) <= user->current_sem)
            printf("%s[%d б (̼)]%s", UI_DIM, sem + 1, UI_RESET);
        else
            printf("%s[%d б ()]%s", UI_COLOR_YELLOW, sem + 1, UI_RESET);
        TimeTable *t = user->table[sem];
                    current_item_idx += (t->n - i);
                    break;



    Subject *root = load_data_from_file("dataset/course_io/subjects.txt");


    int focus_area = 0;
    int btn_idx = 0;
    int edit_mode = 0;
        draw_screen(&user, root, col_idx, row_indices, focus_area, btn_idx, edit_mode,
                    timetable_select_idx);


        for (int i = 0; i < SEMESTER_NUM; i++)
            if (user.table[i])
                total_subjects += user.table[i]->n;
            ch = _getch();
                if (ch == UP_ARROW)
                    focus_area = 0;
                } else if (ch == LEFT_ARROW) {
                    if (btn_idx > 0)
                        btn_idx--;
                } else if (ch == RIGHT_ARROW) {
                    if (btn_idx < 2)
                        btn_idx++;
            } else if (focus_area == 2) { // ðǥ Ŀ
                if (ch == UP_ARROW)
                    focus_area = 1;
                else if (ch == LEFT_ARROW) {
                    if (timetable_select_idx > 0)
                        timetable_select_idx--;
                } else if (ch == RIGHT_ARROW) {
                    if (timetable_select_idx < total_subjects - 1)
                        timetable_select_idx++;
                }
            } else { // Ʈ Ŀ
                Subject *current_parent = root;
                if (col_idx == 1)
                    current_parent = root->arr[row_indices[0]];
                if (col_idx == 2)
                    current_parent = root->arr[row_indices[0]]->arr[row_indices[1]];
                int list_count =
                    (current_parent && current_parent->isFile == 0) ? current_parent->n : 0;

                case UP_ARROW:
                    if (row_indices[col_idx] > 0)
                        row_indices[col_idx]--;
                    break;
                case DOWN_ARROW:
                    if (list_count > 0 && row_indices[col_idx] >= list_count - 1)
                        focus_area = 1;
                    else if (list_count > 0 && row_indices[col_idx] < list_count - 1)
                        row_indices[col_idx]++;
                    break;
                case LEFT_ARROW:
                    if (col_idx > 0)
                        col_idx--;
                    break;
                case RIGHT_ARROW:
                    if (col_idx < 2 && current_parent && current_parent->n > 0) {
                        Subject *selected_item = current_parent->arr[row_indices[col_idx]];
                        if (selected_item->isFile == 0) {
                            col_idx++;
                            row_indices[col_idx] = 0;
                    }
                    break;
        } else if (ch == ENTER) {
                if (btn_idx == 0) {
                    edit_mode = !edit_mode;
                } else if (btn_idx == 1) {
                    int new_sem =
                        popup_select_semester(0, SEMESTER_NUM - 1, " ̼ б⸦ ϼ");
                    if (new_sem != -1)
                        user.current_sem = new_sem;
                } else if (btn_idx == 2) {
                    save_user_data(&user);
            } else if (focus_area == 2) { // ðǥ ׸  -> / ˾

                    for (int s = 0; s < SEMESTER_NUM; s++) {
                        if (user.table[s] == NULL)
                            continue;
                        if (current_idx + user.table[s]->n > timetable_select_idx) {
                            if (timetable_select_idx >= total_subjects - 1)
                                timetable_select_idx--;
                            if (timetable_select_idx < 0)
                                timetable_select_idx = 0;
                            if (edit_mode == 0) {
                                start_s = user.current_sem + 1;
                                end_s = SEMESTER_NUM;
                            } else {
                                start_s = 1;
                                end_s = user.current_sem;
            } else if (focus_area == 0 && col_idx == 2) { //  ߰
                Subject *parent = root->arr[row_indices[0]]->arr[row_indices[1]];
                Subject *target = parent->arr[row_indices[2]];
                    if (edit_mode == 0) {
                        start_s = user.current_sem + 1;
                        end_s = SEMESTER_NUM;
                    } else {
                        start_s = 1;
                        end_s = user.current_sem;
                        if (sem != -1) {
                                user.table[sem_idx] = (TimeTable *)calloc(1, sizeof(TimeTable));
                            TimeTable *t = user.table[sem_idx];
                                Subject *new_subj = copy_subject(target);
            old_t->subjects[i] = old_t->subjects[i+1];
        }
        old_t->subjects[old_t->n - 1] = NULL;
        old_t->n--;
    } else {
        // 꽉 차서 이동 불가 알림 등 (생략)
    }
}

// [수정] 화면 그리기 (focus_area: 0=Tree, 1=Buttons, 2=Timetable)
void draw_screen(User* user, Subject* root, int col_idx, int row_indices[3], 
                 int focus_area, int btn_idx, int edit_mode, int timetable_select_idx) {
    system("cls");

    // 1. 상단바
    char mode_str[50];
    char color_code[10];
    if (edit_mode == 0) {
        strcpy(mode_str, "수강 예정");
        strcpy(color_code, UI_COLOR_GREEN);
    } else {
        strcpy(mode_str, "수강 완료");
        strcpy(color_code, UI_COLOR_YELLOW);
    }

    printf("================================================================================\n");
    printf("  수강신청 | %s%s%s | 현재 이수 학기: %d학기                     %sID: %5d%s\n", 
            color_code, mode_str, UI_RESET, user->current_sem, UI_BOLD, user->id, UI_RESET);
    printf("================================================================================\n");

    // 2. 트리 데이터 준비
    Subject* cols[3] = {root, NULL, NULL};
    if (cols[0]->n > 0 && row_indices[0] < cols[0]->n) 
        cols[1] = cols[0]->arr[row_indices[0]];
    if (cols[1] != NULL && cols[1]->isFile == 0 && cols[1]->n > 0 && row_indices[1] < cols[1]->n) 
        cols[2] = cols[1]->arr[row_indices[1]];

    // 3. 컬럼 그리기
    for (int c = 0; c < 3; c++) {
        int x_pos = 2 + (c * COL_WIDTH);
        goto_ansi(x_pos, COL_Y);
        
        if (c == col_idx && focus_area == 0) 
            printf("%s v Step %d%s", UI_COLOR_CYAN, c + 1, UI_RESET);
        else 
            printf("  Step %d", c + 1);

        goto_ansi(x_pos + COL_WIDTH - 2, COL_Y);
        if(c < 2) printf("|");

        if (cols[c] == NULL || cols[c]->isFile == 1) continue;

        for (int i = 0; i < cols[c]->n; i++) {
            goto_ansi(x_pos, COL_Y + 1 + i);
            Subject* item = cols[c]->arr[i];
            
            char display_name[50];
            if (item->isFile == 1) sprintf(display_name, "%s(%d)", item->name, item->credit);
            else strcpy(display_name, item->name);

            if (c == col_idx && i == row_indices[c] && focus_area == 0) {
                printf("%s %-15s %s", UI_REVERSE, display_name, UI_RESET);
                if (item->isFile == 0) printf(" >");
            } 
            else if (c < col_idx && i == row_indices[c]) {
                printf("%s %-15s %s >", UI_BOLD, display_name, UI_RESET);
            }
            else {
                printf("  %-15s", display_name);
            }
        }
    }

    // 4. 하단 버튼 영역
    int btn_y = START_Y + 12;
    int btn_spacing = 22;
    int btn_start_x = (CONSOLE_WIDTH - (btn_spacing * 3)) / 2 + 2;
    const char* labels[3] = { 
        "  모드 변경  ", 
        "이수 학기 변경", 
        "    확정     " 
    };
    for (int i = 0; i < 3; i++) {
        goto_ansi(btn_start_x + (i * btn_spacing), btn_y);
        if (focus_area == 1 && btn_idx == i) {
            printf("%s [ %s ] %s", UI_REVERSE, labels[i], UI_RESET);
        } else {
            printf(" [ %s ] ", labels[i]);
        }
    }

    // 5. 시간표 요약 및 그리드
    int table_y = btn_y + 3;   
    int total_credits = 0;
    int total_subjects = 0;
    // 전체 과목 수 계산
    for (int i = 0; i < SEMESTER_NUM; i++) {
        if (user->table[i] != NULL) {
            total_subjects += user->table[i]->n;
            for (int j = 0; j < user->table[i]->n; j++) {
                if (user->table[i]->subjects[j] != NULL)
                    total_credits += user->table[i]->subjects[j]->credit;
            }
        }
    }

    goto_ansi(2, table_y);
    printf("--------------------------------------------------------------------------------");
    goto_ansi(2, table_y + 1);
    printf("[ 전체 시간표 요약 | 총 %d과목 | 총 %d학점 ]", total_subjects, total_credits);
    
    int box_w = 26;
    int start_table_row = table_y + 3;
    
    // 시간표 아이템 인덱싱용 카운터
    int current_item_idx = 0;

    for (int sem = 0; sem < SEMESTER_NUM; sem++) {
        int col = sem % 3;
        int row = sem / 3;
        int box_x = 2 + (col * box_w);
        int box_y = start_table_row + (row * 6);

        goto_ansi(box_x, box_y);
        if ((sem + 1) <= user->current_sem) printf("%s[%d 학기 (이수)]%s", UI_DIM, sem + 1, UI_RESET);
        else printf("%s[%d 학기 (예정)]%s", UI_COLOR_YELLOW, sem + 1, UI_RESET);

        TimeTable* t = user->table[sem];
        if (t == NULL || t->n == 0) {
            goto_ansi(box_x, box_y + 1);
            printf("%s(비어있음)%s", UI_DIM, UI_RESET);
        } else {
            for (int i = 0; i < t->n; i++) {
                if (i >= 4) {
                    goto_ansi(box_x, box_y + 1 + i);
                    printf("...외 %d건", t->n - i);
                    current_item_idx += (t->n - i); 
                    break; 
                }
                
                goto_ansi(box_x, box_y + 1 + i);
                
                // 시간표 영역 포커스 시 하이라이트
                if (focus_area == 2 && current_item_idx == timetable_select_idx) {
                    printf("%s- %s%s", UI_REVERSE, t->subjects[i]->name, UI_RESET);
                } else {
                    printf("- %s", t->subjects[i]->name);
                }
                
                current_item_idx++;
            }
        }
    }
}

void run_registration(int student_id) {
    Subject* root = load_data_from_file("dataset/course_io/subjects.txt");
    
    User user;
    int is_first;

    load_user_data(&user, student_id, &is_first);

    int col_idx = 0;
    int row_indices[3] = {0, 0, 0};
    
    // 0:Tree, 1:Buttons, 2:Timetable
    int focus_area = 0; 
    int btn_idx = 0; 
    int timetable_select_idx = 0;

    int edit_mode = 0; 

    if (is_first) {
        user.current_sem = popup_select_semester(1, SEMESTER_NUM, "현재 이수 학기를 선택하세요");
        edit_mode = 1;
    }

    KeyEnum ch;
    while (1) {
        draw_screen(&user, root, col_idx, row_indices, focus_area, btn_idx, edit_mode, timetable_select_idx);
        
        goto_ansi(1, 1);
        ch = _getch();
        
        // 총 등록 과목 수 계산 (시간표 네비게이션용)
        int total_subjects = 0;
        for(int i=0; i<SEMESTER_NUM; i++) 
            if(user.table[i]) total_subjects += user.table[i]->n;

        if (ch == 224 || ch == 0) {
            ch = _getch(); 

            if (focus_area == 1) { // 버튼 포커스
                if (ch == UP_ARROW) focus_area = 0; 
                else if (ch == DOWN_ARROW) {
                    if (total_subjects > 0) {
                        focus_area = 2;
                        timetable_select_idx = 0;
                    }
                }
                else if (ch == LEFT_ARROW) { if (btn_idx > 0) btn_idx--; }
                else if (ch == RIGHT_ARROW) { if (btn_idx < 2) btn_idx++; }
            } 
            else if (focus_area == 2) { // 시간표 포커스
                if (ch == UP_ARROW) focus_area = 1;
                else if (ch == LEFT_ARROW) { if (timetable_select_idx > 0) timetable_select_idx--; }
                else if (ch == RIGHT_ARROW) { if (timetable_select_idx < total_subjects - 1) timetable_select_idx++; }
            }
            else { // 트리 포커스
                Subject* current_parent = root;
                if (col_idx == 1) current_parent = root->arr[row_indices[0]];
                if (col_idx == 2) current_parent = root->arr[row_indices[0]]->arr[row_indices[1]];
                int list_count = (current_parent && current_parent->isFile == 0) ? current_parent->n : 0;
            
                switch (ch) {
                    case UP_ARROW:
                        if (row_indices[col_idx] > 0) row_indices[col_idx]--;
                        break;
                    case DOWN_ARROW:
                        if (list_count > 0 && row_indices[col_idx] >= list_count - 1) focus_area = 1; 
                        else if (list_count > 0 && row_indices[col_idx] < list_count - 1) row_indices[col_idx]++;
                        break;
                    case LEFT_ARROW:
                        if (col_idx > 0) col_idx--;
                        break;
                    case RIGHT_ARROW:
                        if (col_idx < 2 && current_parent && current_parent->n > 0) {
                            Subject* selected_item = current_parent->arr[row_indices[col_idx]];
                            if (selected_item->isFile == 0) {
                                col_idx++;
                                row_indices[col_idx] = 0;
                            }
                        }
                        break;
                }
            }
        }
        else if (ch == ENTER) {
            if (focus_area == 1) { // 버튼 클릭
                if (btn_idx == 0) { edit_mode = !edit_mode; }
                else if (btn_idx == 1) { 
                    int new_sem = popup_select_semester(0, SEMESTER_NUM - 1, "현재 이수 학기를 선택하세요");
                    if (new_sem != -1) user.current_sem = new_sem;
                }
                else if (btn_idx == 2) { 
                    save_user_data(&user); 
                    system("cls");
                    printf("\n저장 완료 (ID: %d). 프로그램을 종료합니다.\n", user.id);
                    break;
                }
            }
            else if (focus_area == 2) { // 시간표 항목 선택 -> 수정/삭제 팝업
                int action = popup_edit_menu();
                if (action == 1 || action == 2) { // 수정 or 삭제
                    // 선택된 인덱스(linear)를 (학기, 과목) 인덱스로 변환
                    int current_idx = 0;
                    int target_sem = -1, target_sub = -1;
                    
                    for(int s=0; s<SEMESTER_NUM; s++) {
                        if(user.table[s] == NULL) continue;
                        if(current_idx + user.table[s]->n > timetable_select_idx) {
                            target_sem = s;
                            target_sub = timetable_select_idx - current_idx;
                            break;
                        }
                        current_idx += user.table[s]->n;
                    }

                    if (target_sem != -1) {
                        if (action == 2) { // 삭제
                            delete_subject(&user, target_sem, target_sub);
                            // 인덱스 조정 (삭제 후 범위 초과 방지)
                            if (timetable_select_idx >= total_subjects - 1) timetable_select_idx--;
                            if (timetable_select_idx < 0) timetable_select_idx = 0;
                        } else { // 수정
                            int start_s, end_s;
                            char title[50];
                            if (edit_mode == 0) { 
                                start_s = user.current_sem + 1; end_s = SEMESTER_NUM;
                                strcpy(title, "이동할 학기 선택 (예정)");
                            } else { 
                                start_s = 1; end_s = user.current_sem;
                                strcpy(title, "이동할 학기 선택 (기이수)");
                            }

                            if (start_s <= end_s) {
                                int new_sem = popup_select_semester(start_s, end_s, title);
                                if (new_sem != -1) {
                                    modify_subject(&user, target_sem, target_sub, new_sem);
                                }
                            }
                        }
                    }
                }
            }
            else if (focus_area == 0 && col_idx == 2) { // 과목 추가
                Subject* parent = root->arr[row_indices[0]]->arr[row_indices[1]];
                Subject* target = parent->arr[row_indices[2]];

                if (target->isFile == 1) {
                    int start_s, end_s;
                    char title[50];
                    if (edit_mode == 0) { 
                        start_s = user.current_sem + 1; end_s = SEMESTER_NUM;
                        strcpy(title, "수강할 학기 선택 (예정)");
                    } else { 
                        start_s = 1; end_s = user.current_sem;
                        strcpy(title, "수강했던 학기 선택 (기이수)");
                    }

                    if (start_s <= end_s) {
                        int sem = popup_select_semester(start_s, end_s, title);
                        if (sem != -1) { 
                            int sem_idx = sem - 1;
                            if (user.table[sem_idx] == NULL) {
                                user.table[sem_idx] = (TimeTable*)calloc(1, sizeof(TimeTable));
                            }
                            TimeTable* t = user.table[sem_idx];
                            if (t->n < MAX_SUBJECT_NUM) {
                                Subject* new_subj = copy_subject(target);
                                if (new_subj != NULL) {
                                    new_subj->semester = sem;
                                    t->subjects[t->n++] = new_subj;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

StatusCodeEnum input(int id) {
    run_registration(id);
    return SUCCESS;
}