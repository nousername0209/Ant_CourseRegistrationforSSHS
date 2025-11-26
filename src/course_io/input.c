#include "input.h" 

/**
 * @brief 과목에 대한 정보를 담은 파일에서 과목 데이터를 로드하여 트리 구조로 반환합니다.
 *
 * @return Subject*
 *         - 파일에서 로드한 과목 트리의 루트 노드 포인터
 */
Subject* load_data_from_file() {
    FILE* fp = fopen("dataset/course_io/subjects.txt", "r");

    Subject* root = create_node("ROOT", 0, 0);
    Subject* parents[TREE_DEPTH];
    parents[0] = root;

    char line[LINE_LENGTH];
    int depth, isFile, credit, sub_id; 
    char name_buffer[STR_LENGTH];

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d|%d|%d|%d|%[^\n]", &depth, &isFile, &credit, &sub_id, name_buffer) == 5) {
            trim_newline(name_buffer);
            Subject* new_node = create_node(name_buffer, isFile, credit);
            new_node->id = sub_id;
            if (depth > 0 && depth <= TREE_DEPTH) {
                add_child(parents[depth - 1], new_node);
                parents[depth] = new_node;
            }
        }
    }
    fclose(fp);
    return root;
}

/**
 * @brief 사용자 데이터를 파일에 저장한다.
 *
 * @param user 저장하고자 하는 사용자의 포인터 
 * 
 * @return void
 */
void save_user_data(User* user) {
    char filename[PATH_LENGTH];
    sprintf(filename, "dataset/course_io/user/user_%d.txt", user->id);
    
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        system("cls");
        printf("오류: 파일을 저장할 수 없습니다 (%s).\n", filename);
        _getch();
        return;
    }

    fprintf(fp, "ID:%d\n", user->id);
    fprintf(fp, "CUR_SEM:%d\n", user->current_sem);
    
    for (int i = 0; i < SEMESTER_NUM; i++) {
        if (user->table[i] != NULL && user->table[i]->n > 0) {
            fprintf(fp, "SEM|%d|%d\n", i + 1, user->table[i]->n); 
            
            for (int j = 0; j < user->table[i]->n; j++) {
                Subject* s = user->table[i]->subjects[j];
                if (s == NULL) continue;
                fprintf(fp, "%s|%d|%d\n", s->name, s->credit, s->id);
            }
        }
    }
    
    fflush(fp);
    fclose(fp);
}

/**
 * @brief 학기 선택 팝업을 표시하고 사용자가 선택한 학기를 반환한다.
 * 
 * @param start_sem 시작 학기 번호
 * @param end_sem 종료 학기 번호
 * @param title 팝업 제목
 * 
 * @return int 선택된 학기 번호, 취소 시 -1 반환
 */
int popup_select_semester(int start_sem, int end_sem, const char* title) {
    if (start_sem > end_sem) return -1;

    int selected_idx = 0;
    int count = end_sem - start_sem + 1;
    int box_w = 35;
    int box_h = count + 4;
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 8;

    for(int i=0; i<box_h; i++) {
        goto_ansi(start_x, start_y + i);
        printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET); 
    }

    int ch;
    while(1) {
        goto_ansi(start_x + 2, start_y + 1);
        printf("%s %s %s", UI_BOLD, title, UI_RESET); 

        for(int i=0; i<count; i++) {
            int target_sem = start_sem + i;
            goto_ansi(start_x + 2, start_y + 3 + i);
            
            if (i == selected_idx) {
                printf("%s%s > %d 학기 %s", UI_REVERSE, UI_COLOR_YELLOW, target_sem, UI_RESET);
            } else {
                printf("%s   %d 학기 %s", UI_REVERSE, target_sem, UI_RESET);
            }
        }
        
        goto_ansi(1, 1);
        ch = _getch();

        if (ch == 224 || ch == 0) {
            ch = _getch();
            if (ch == UP_ARROW && selected_idx > 0) selected_idx--;
            if (ch == DOWN_ARROW && selected_idx < count - 1) selected_idx++;
        }
        else if (ch == ENTER) {
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%*s", box_w, ""); 
            }
            return start_sem + selected_idx;
        }
        else if (ch == ESC) {
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%*s", box_w, ""); 
            }
            return -1;
        }
    }
}

/**
 * @brief 과목 수정 메뉴 팝업을 표시하고 사용자가 선택한 옵션을 반환한다.
 * 
 * @return int 선택된 옵션 번호 (1: 학기 이동, 2: 과목 삭제, 3: 취소), 취소 시 0 반환
 */
int popup_edit_menu() {
    int selected_idx = 0;
    const char* menus[] = { "1. 학기 이동", "2. 과목 삭제", "3. 취소" };
    int menu_cnt = 3;
    
    int box_w = 20;
    int box_h = menu_cnt + 4;
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 10;

    // 배경
    for(int i=0; i<box_h; i++) {
        goto_ansi(start_x, start_y + i);
        printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET); 
    }

    int ch;
    while(1) {
        goto_ansi(start_x + 2, start_y + 1);
        printf("%s 수정 메뉴 %s", UI_BOLD, UI_RESET); 

        for(int i=0; i<menu_cnt; i++) {
            goto_ansi(start_x + 2, start_y + 3 + i);
            if (i == selected_idx) {
                printf("%s%s > %s %s", UI_REVERSE, UI_COLOR_YELLOW, menus[i], UI_RESET);
            } else {
                printf("%s   %s %s", UI_REVERSE, menus[i], UI_RESET);
            }
        }
        
        goto_ansi(1, 1);
        ch = _getch();
        if (ch == 224 || ch == 0) {
            ch = _getch();
            if (ch == UP_ARROW && selected_idx > 0) selected_idx--;
            if (ch == DOWN_ARROW && selected_idx < menu_cnt - 1) selected_idx++;
        } else if (ch == ENTER) {
            // 지우기
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%*s", box_w, ""); 
            }
            return selected_idx + 1;
        } else if (ch == ESC) {
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%*s", box_w, ""); 
            }
            return 0;
        }
    }
}

/**
 * @brief user의 특정 학기에서 특정 과목을 삭제한다.
 * 
 * @param user 과목을 삭제할 사용자의 포인터
 * @param sem_idx 삭제할 과목이 속한 학기 인덱스
 * @param sub_idx 삭제할 과목의 인덱스
 * 
 * @return void
 */
void delete_subject(User* user, int sem_idx, int sub_idx) {
    TimeTable* t = user->table[sem_idx];
    if (t == NULL || sub_idx >= t->n) return;

    free(t->subjects[sub_idx]);
    
    for (int i = sub_idx; i < t->n - 1; i++) {
        t->subjects[i] = t->subjects[i+1];
    }
    t->subjects[t->n - 1] = NULL;
    t->n--;
}

/**
 * @brief user의 특정 학기에서 특정 과목의 학기를 변경한다.
 * 
 * @param user 과목을 수정할 사용자의 포인터
 * @param old_sem_idx 변경할 과목이 속한 기존 학기 인덱스
 * @param sub_idx 변경할 과목의 인덱스
 * @param new_sem 변경할 새로운 학기 번호
 * 
 * @return void
 */
void modify_subject(User* user, int old_sem_idx, int sub_idx, int new_sem) {
    if (old_sem_idx == new_sem - 1) return;

    TimeTable* old_t = user->table[old_sem_idx];
    if (old_t == NULL || sub_idx >= old_t->n) return;

    Subject* target = old_t->subjects[sub_idx];
    
    int new_sem_idx = new_sem - 1;
    if (user->table[new_sem_idx] == NULL) {
        user->table[new_sem_idx] = (TimeTable*)calloc(1, sizeof(TimeTable));
    }
    TimeTable* new_t = user->table[new_sem_idx];

    if (new_t->n < MAX_SUBJECT_NUM) {
        target->semester = new_sem;
        new_t->subjects[new_t->n++] = target;
        
        for (int i = sub_idx; i < old_t->n - 1; i++) {
            old_t->subjects[i] = old_t->subjects[i+1];
        }
        old_t->subjects[old_t->n - 1] = NULL;
        old_t->n--;
    }
}

/**
 * @brief UI를 그린다.
 * 
 * @param user 현재 사용자 포인터
 * @param root 과목 트리의 루트 포인터
 * @param col_idx 현재 선택된 열 인덱스
 * @param row_indices 각 열 별 선택된 행 인덱스 배열
 * @param focus_area 현재 선택한 영역 (0: 과목 트리, 1: 버튼, 2: 과목 수정)
 * @param btn_idx 현재 선택된 버튼 인덱스
 * @param edit_mode 현재 모드 (0: 수강 예정, 1: 수강 완료)
 * @param timetable_select_idx 시간표 요약에서 선택된 과목 인덱스
 * 
 * @return void
 */
void draw_screen(User* user, Subject* root, int col_idx, int row_indices[3], 
                 FocusEnum focus_area, ButtonEnum btn_idx, int edit_mode, int timetable_select_idx) {
    system("cls");

    char mode_str[LINE_LENGTH];
    char color_code[STR_LENGTH];
    if (edit_mode == 0) {
        strcpy(mode_str, "수강 신청");
        strcpy(color_code, UI_COLOR_GREEN);
    } else {
        strcpy(mode_str, "기수강 입력");
        strcpy(color_code, UI_COLOR_YELLOW);
    }

    printf("================================================================================\n");
    printf("  수강신청 | %s%s%s | 현재 이수 학기: %d학기                      %sID: %5d%s\n", 
            color_code, mode_str, UI_RESET, user->current_sem, UI_BOLD, user->id, UI_RESET);
    printf("================================================================================\n");

    Subject* cols[3] = {root, NULL, NULL};
    if (cols[0]->n > 0 && row_indices[0] < cols[0]->n) 
        cols[1] = cols[0]->arr[row_indices[0]];
    if (cols[1] != NULL && cols[1]->isFile == 0 && cols[1]->n > 0 && row_indices[1] < cols[1]->n) 
        cols[2] = cols[1]->arr[row_indices[1]];

    for (int c = 0; c < 3; c++) {
        int x_pos = 2 + (c * COL_WIDTH);
        goto_ansi(x_pos, COL_Y);
        
        if (c == col_idx && focus_area == TREE) 
            printf("%s v Step %d%s", UI_COLOR_CYAN, c + 1, UI_RESET);
        else 
            printf("  Step %d", c + 1);

        goto_ansi(x_pos + COL_WIDTH - 2, COL_Y);
        if(c < 2) printf("|");

        if (cols[c] == NULL || cols[c]->isFile == 1) continue;

        for (int i = 0; i < cols[c]->n; i++) {
            goto_ansi(x_pos, COL_Y + 1 + i);
            Subject* item = cols[c]->arr[i];
            
            char display_name[STR_LENGTH];
            if (item->isFile == 1) sprintf(display_name, "%s(%d)", item->name, item->credit);
            else strcpy(display_name, item->name);

            if (c == col_idx && i == row_indices[c] && focus_area == TREE) {
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

    int btn_y = START_Y + 12;
    int btn_spacing = 22;
    int btn_start_x = (CONSOLE_WIDTH - (btn_spacing * 3)) / 2 + 2;
    const char* labels[3] = { 
        "  모드 전환  ", 
        "이수 학기 설정", 
        "     저장     " 
    };
    for (int i = 0; i < 3; i++) {
        goto_ansi(btn_start_x + (i * btn_spacing), btn_y);
        if (focus_area == BUTTON && btn_idx == i) {
            printf("%s [ %s ] %s", UI_REVERSE, labels[i], UI_RESET);
        } else {
            printf(" [ %s ] ", labels[i]);
        }
    }

    int table_y = btn_y + 3;   
    int total_credits = 0;
    int total_subjects = 0;

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
    printf("[ 전체 시간표 현황 | 총 %d과목 | 총 %d학점 ]", total_subjects, total_credits);
    
    int box_w = 26;
    int start_table_row = table_y + 3;
    
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
            printf("%s(과목없음)%s", UI_DIM, UI_RESET);
        } else {
            for (int i = 0; i < t->n; i++) {
                if (i >= 4) {
                    goto_ansi(box_x, box_y + 1 + i);
                    printf("...외 %d개", t->n - i);
                    current_item_idx += (t->n - i); 
                    break; 
                }
                
                goto_ansi(box_x, box_y + 1 + i);
                
                if (focus_area == EDIT && current_item_idx == timetable_select_idx) {
                    printf("%s- %s%s", UI_REVERSE, t->subjects[i]->name, UI_RESET);
                } else {
                    printf("- %s", t->subjects[i]->name);
                }
                
                current_item_idx++;
            }
        }
    }
}

/**
 * @brief 수강신청 프로그램을 실행한다.
 * 
 * @param student_id 현재 사용자의 학번
 * 
 * @return void
 */
void run_registration(int student_id) {
    Subject* root = load_data_from_file();
    
    User user;
    int is_first;

    load_user_data(&user, student_id, &is_first);

    int col_idx = 0;
    int row_indices[TREE_DEPTH] = {0, 0, 0};
    
    FocusEnum focus_area = TREE; 
    ButtonEnum btn_idx = MODE; 
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
        
        int total_subjects = 0;
        for(int i = 0; i<SEMESTER_NUM; i++) 
            if(user.table[i]) total_subjects += user.table[i]->n;

        if (ch == PRE_INPUT1 || ch == PRE_INPUT2) {
            ch = _getch(); 

            if (focus_area == BUTTON) {
                if (ch == UP_ARROW) focus_area = TREE; 
                else if (ch == DOWN_ARROW) {
                    if (total_subjects > 0) {
                        focus_area = EDIT;
                        timetable_select_idx = 0;
                    }
                }
                else if (ch == LEFT_ARROW) { if (btn_idx > 0) btn_idx--; }
                else if (ch == RIGHT_ARROW) { if (btn_idx < BUTTON_NUM - 1) btn_idx++; }
            } 
            else if (focus_area == EDIT) {
                if (ch == UP_ARROW) focus_area = BUTTON;
                else if (ch == LEFT_ARROW) {
                    if (timetable_select_idx > 0) timetable_select_idx--;
                } else if (ch == RIGHT_ARROW) {
                    if (timetable_select_idx < total_subjects - 1) timetable_select_idx++;
                }
            }
            else {
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
            if (focus_area == BUTTON) {
                if (btn_idx == MODE) { edit_mode = !edit_mode; }
                else if (btn_idx == SEM) { 
                    int new_sem = popup_select_semester(0, SEMESTER_NUM - 1, "현재 이수 학기를 선택하세요");
                    if (new_sem != -1) user.current_sem = new_sem;
                }
                else if (btn_idx == SAVE) { 
                    save_user_data(&user); 
                    system("cls");
                    printf("\n저장 완료 (ID: %d). 프로그램을 종료합니다.\n", user.id);
                    break;
                }
            }
            else if (focus_area == EDIT) {
                int action = popup_edit_menu();
                if (action == 1 || action == 2) {
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
                        if (action == 2) {
                            delete_subject(&user, target_sem, target_sub);
                            if (timetable_select_idx >= total_subjects - 1) timetable_select_idx--;
                            if (timetable_select_idx < 0) timetable_select_idx = 0;
                        } else {
                            int start_s, end_s;
                            char title[STR_LENGTH];
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
            else if (focus_area == TREE && col_idx == 2) {
                Subject* parent = root->arr[row_indices[0]]->arr[row_indices[1]];
                Subject* target = parent->arr[row_indices[2]];

                if (target->isFile == 1) {
                    int start_s, end_s;
                    char title[STR_LENGTH];
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

/**
 * @brief 수강신청 프로그램을 실행하는 메인에 들어가는 함수이다.
 * 
 * @param id 현재 사용자의 학번
 * 
 * @return StatusCodeEnum
 */
StatusCodeEnum input(int id) {
    run_registration(id);
    return SUCCESS;
}