#include "input.h" 

#define COL_WIDTH 25
#define COL_Y 4

Subject* load_data_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");

    Subject* root = create_node("ROOT", 0, 0);
    Subject* parents[10];
    parents[0] = root;

    char line[256];
    int depth, isFile, credit, sub_id; 
    char name_buffer[NAME_LENGTH];

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d|%d|%d|%d|%[^\n]", &depth, &isFile, &credit, &sub_id, name_buffer) == 5) {
            trim_newline(name_buffer);
            Subject* new_node = create_node(name_buffer, isFile, credit);
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

void save_user_data(User* user) {
    char filename[100];
    sprintf(filename, "dataset/course_io/user/user_%d.txt", user->id);
    
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        system("cls");
        printf("����: ������ ������ �� �����ϴ� (%s).\n", filename);
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
                printf("%s%s > %d �б� %s", UI_REVERSE, UI_COLOR_YELLOW, target_sem, UI_RESET);
            } else {
                printf("%s   %d �б� %s", UI_REVERSE, target_sem, UI_RESET);
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
            return -    1;
        }
    }
}

int popup_edit_menu() {
    int selected_idx = 0;
    const char* menus[] = { "1. �б� ����", "2. ���� ����", "3. ���" };
    int menu_cnt = 3;
    
    int box_w = 20;
    int box_h = menu_cnt + 4;
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 10;

    // ���
    for(int i=0; i<box_h; i++) {
        goto_ansi(start_x, start_y + i);
        printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET); 
    }

    int ch;
    while(1) {
        goto_ansi(start_x + 2, start_y + 1);
        printf("%s ���� ���� %s", UI_BOLD, UI_RESET); 

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
            // �����
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%*s", box_w, ""); 
            }
            return selected_idx + 1; // 1, 2, 3 ��ȯ
        } else if (ch == ESC) {
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%*s", box_w, ""); 
            }
            return 0;
        }
    }
}

// [�߰�] ���� ���� �Լ�
void delete_subject(User* user, int sem_idx, int sub_idx) {
    TimeTable* t = user->table[sem_idx];
    if (t == NULL || sub_idx >= t->n) return;

    // �޸� ����
    free(t->subjects[sub_idx]);
    
    // �迭 ����
    for (int i = sub_idx; i < t->n - 1; i++) {
        t->subjects[i] = t->subjects[i+1];
    }
    t->subjects[t->n - 1] = NULL; // ������ ������ �ʱ�ȭ
    t->n--;
}

// [�߰�] ���� ���� �Լ� (�б� �̵�)
void modify_subject(User* user, int old_sem_idx, int sub_idx, int new_sem) {
    if (old_sem_idx == new_sem - 1) return; // ���� �б�� ����

    TimeTable* old_t = user->table[old_sem_idx];
    if (old_t == NULL || sub_idx >= old_t->n) return;

    // ���� ���� ������ ��������
    Subject* target = old_t->subjects[sub_idx];
    
    // �� �б� ���̺� Ȯ�� �� ����
    int new_sem_idx = new_sem - 1;
    if (user->table[new_sem_idx] == NULL) {
        user->table[new_sem_idx] = (TimeTable*)calloc(1, sizeof(TimeTable));
    }
    TimeTable* new_t = user->table[new_sem_idx];

    if (new_t->n < MAX_SUBJECT_NUM) {
        // ���� ������Ʈ
        target->semester = new_sem;
        // �� ���̺��� �߰�
        new_t->subjects[new_t->n++] = target;
        
        // ���� ���̺����� ���� (�޸� ���� ���� �����͸� �̵��ϹǷ� �迭�� ���)
        for (int i = sub_idx; i < old_t->n - 1; i++) {
            old_t->subjects[i] = old_t->subjects[i+1];
        }
        old_t->subjects[old_t->n - 1] = NULL;
        old_t->n--;
    } else {
        // �� ���� �̵� �Ұ� �˸� �� (����)
    }
}

// [����] ȭ�� �׸��� (focus_area: 0=Tree, 1=Buttons, 2=Timetable)
void draw_screen(User* user, Subject* root, int col_idx, int row_indices[3], 
                 int focus_area, int btn_idx, int edit_mode, int timetable_select_idx) {
    system("cls");

    // 1. ��ܹ�
    char mode_str[50];
    char color_code[10];
    if (edit_mode == 0) {
        strcpy(mode_str, "���� ����");
        strcpy(color_code, UI_COLOR_GREEN);
    } else {
        strcpy(mode_str, "���� �Ϸ�");
        strcpy(color_code, UI_COLOR_YELLOW);
    }

    printf("================================================================================\n");
    printf("  ������û | %s%s%s | ���� �̼� �б�: %d�б�                     %sID: %5d%s\n", 
            color_code, mode_str, UI_RESET, user->current_sem, UI_BOLD, user->id, UI_RESET);
    printf("================================================================================\n");

    // 2. Ʈ�� ������ �غ�
    Subject* cols[3] = {root, NULL, NULL};
    if (cols[0]->n > 0 && row_indices[0] < cols[0]->n) 
        cols[1] = cols[0]->arr[row_indices[0]];
    if (cols[1] != NULL && cols[1]->isFile == 0 && cols[1]->n > 0 && row_indices[1] < cols[1]->n) 
        cols[2] = cols[1]->arr[row_indices[1]];

    // 3. �÷� �׸���
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

    // 4. �ϴ� ��ư ����
    int btn_y = START_Y + 12;
    int btn_spacing = 22;
    int btn_start_x = (CONSOLE_WIDTH - (btn_spacing * 3)) / 2 + 2;
    const char* labels[3] = { 
        "  ��� ����  ", 
        "�̼� �б� ����", 
        "    Ȯ��     " 
    };
    for (int i = 0; i < 3; i++) {
        goto_ansi(btn_start_x + (i * btn_spacing), btn_y);
        if (focus_area == 1 && btn_idx == i) {
            printf("%s [ %s ] %s", UI_REVERSE, labels[i], UI_RESET);
        } else {
            printf(" [ %s ] ", labels[i]);
        }
    }

    // 5. �ð�ǥ ��� �� �׸���
    int table_y = btn_y + 3;   
    int total_credits = 0;
    int total_subjects = 0;
    // ��ü ���� �� ���
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
    printf("[ ��ü �ð�ǥ ��� | �� %d���� | �� %d���� ]", total_subjects, total_credits);
    
    int box_w = 26;
    int start_table_row = table_y + 3;
    
    // �ð�ǥ ������ �ε��̿� ī����
    int current_item_idx = 0;

    for (int sem = 0; sem < SEMESTER_NUM; sem++) {
        int col = sem % 3;
        int row = sem / 3;
        int box_x = 2 + (col * box_w);
        int box_y = start_table_row + (row * 6);

        goto_ansi(box_x, box_y);
        if ((sem + 1) <= user->current_sem) printf("%s[%d �б� (�̼�)]%s", UI_DIM, sem + 1, UI_RESET);
        else printf("%s[%d �б� (����)]%s", UI_COLOR_YELLOW, sem + 1, UI_RESET);

        TimeTable* t = user->table[sem];
        if (t == NULL || t->n == 0) {
            goto_ansi(box_x, box_y + 1);
            printf("%s(�������)%s", UI_DIM, UI_RESET);
        } else {
            for (int i = 0; i < t->n; i++) {
                if (i >= 4) {
                    goto_ansi(box_x, box_y + 1 + i);
                    printf("...�� %d��", t->n - i);
                    current_item_idx += (t->n - i); 
                    break; 
                }
                
                goto_ansi(box_x, box_y + 1 + i);
                
                // �ð�ǥ ���� ��Ŀ�� �� ���̶���Ʈ
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
        user.current_sem = popup_select_semester(1, SEMESTER_NUM, "���� �̼� �б⸦ �����ϼ���");
        edit_mode = 1;
    }

    KeyEnum ch;
    while (1) {
        draw_screen(&user, root, col_idx, row_indices, focus_area, btn_idx, edit_mode, timetable_select_idx);
        
        goto_ansi(1, 1);
        ch = _getch();
        
        // �� ��� ���� �� ��� (�ð�ǥ �׺���̼ǿ�)
        int total_subjects = 0;
        for(int i=0; i<SEMESTER_NUM; i++) 
            if(user.table[i]) total_subjects += user.table[i]->n;

        if (ch == 224 || ch == 0) {
            ch = _getch(); 

            if (focus_area == 1) { // ��ư ��Ŀ��
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
            else if (focus_area == 2) { // �ð�ǥ ��Ŀ��
                if (ch == UP_ARROW) focus_area = 1;
                else if (ch == LEFT_ARROW) { if (timetable_select_idx > 0) timetable_select_idx--; }
                else if (ch == RIGHT_ARROW) { if (timetable_select_idx < total_subjects - 1) timetable_select_idx++; }
            }
            else { // Ʈ�� ��Ŀ��
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
            if (focus_area == 1) { // ��ư Ŭ��
                if (btn_idx == 0) { edit_mode = !edit_mode; }
                else if (btn_idx == 1) { 
                    int new_sem = popup_select_semester(0, SEMESTER_NUM - 1, "���� �̼� �б⸦ �����ϼ���");
                    if (new_sem != -1) user.current_sem = new_sem;
                }
                else if (btn_idx == 2) { 
                    save_user_data(&user); 
                    system("cls");
                    printf("\n���� �Ϸ� (ID: %d). ���α׷��� �����մϴ�.\n", user.id);
                    break;
                }
            }
            else if (focus_area == 2) { // �ð�ǥ �׸� ���� -> ����/���� �˾�
                int action = popup_edit_menu();
                if (action == 1 || action == 2) { // ���� or ����
                    // ���õ� �ε���(linear)�� (�б�, ����) �ε����� ��ȯ
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
                        if (action == 2) { // ����
                            delete_subject(&user, target_sem, target_sub);
                            // �ε��� ���� (���� �� ���� �ʰ� ����)
                            if (timetable_select_idx >= total_subjects - 1) timetable_select_idx--;
                            if (timetable_select_idx < 0) timetable_select_idx = 0;
                        } else { // ����
                            int start_s, end_s;
                            char title[50];
                            if (edit_mode == 0) { 
                                start_s = user.current_sem + 1; end_s = SEMESTER_NUM;
                                strcpy(title, "�̵��� �б� ���� (����)");
                            } else { 
                                start_s = 1; end_s = user.current_sem;
                                strcpy(title, "�̵��� �б� ���� (���̼�)");
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
            else if (focus_area == 0 && col_idx == 2) { // ���� �߰�
                Subject* parent = root->arr[row_indices[0]]->arr[row_indices[1]];
                Subject* target = parent->arr[row_indices[2]];

                if (target->isFile == 1) {
                    int start_s, end_s;
                    char title[50];
                    if (edit_mode == 0) { 
                        start_s = user.current_sem + 1; end_s = SEMESTER_NUM;
                        strcpy(title, "������ �б� ���� (����)");
                    } else { 
                        start_s = 1; end_s = user.current_sem;
                        strcpy(title, "�����ߴ� �б� ���� (���̼�)");
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
