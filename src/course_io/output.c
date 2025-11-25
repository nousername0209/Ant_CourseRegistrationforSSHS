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
    printf("%s[ %d�б� ���̵� ������ �߰� ]%s (0~10�� �Է�, ���ڸ� �Է��ϼ���)", UI_BOLD, semester, UI_RESET);
    
    // 1. �� ���̵� �Է� �ʵ�
    cursor_y = start_y + 3;
    goto_ansi(start_x + 2, cursor_y);
    if (current_idx == -1) printf("%s", UI_COLOR_CYAN); // ���� ���� ����
    printf("�� �ð�ǥ ���̵�: ");
    
    goto_ansi(start_x + 30, cursor_y);
    if (total_difficulty_input != -1) {
        printf("%d", total_difficulty_input);
    } else {
         printf("___");
    }
    printf("%s", UI_RESET);

    // 2. ���� ���̵� �Է� �ʵ�
    for (int i = 0; i < table->n; i++) {
        cursor_y = start_y + 5 + i * 2;
        goto_ansi(start_x + 2, cursor_y);
        if (current_idx == i) printf("%s", UI_COLOR_CYAN); // ���� ���� ����

        printf("���� %s ���̵�: ", table->subjects[i]->name);
        
        goto_ansi(start_x + 30, cursor_y);
        if (subject_difficulties[i] > 0) {
             printf("%d", subject_difficulties[i]);
        } else {
             printf("___");
        }
        printf("%s", UI_RESET);
    }

    // 3. ��ư ����
    cursor_y = start_y + box_h - 2;
    goto_ansi(start_x + (box_w / 2) - 15, cursor_y);
    printf("%s[ ��/��: �̵� | ����: �Է� | ENTER: ���� | ESC: ��� ]%s", UI_BOLD, UI_RESET);
}

void popup_show_message(const char* title, const char* msg) {
    int box_w = 40;
    int box_h = 7;
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 10;

    // ��� ����� �� �ڽ�
    for(int i=0; i<box_h; i++) {
        goto_ansi(start_x, start_y + i);
        printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET); 
    }

    goto_ansi(start_x + 2, start_y + 2);
    printf("%s[%s]%s", UI_BOLD, title, UI_RESET);
    
    goto_ansi(start_x + 2, start_y + 3);
    printf("%s%s%s", UI_REVERSE, msg, UI_RESET);

    goto_ansi(start_x + 2, start_y + 5);
    printf("%s[ ���� Ű�� ���� Ȯ�� ]%s", UI_DIM, UI_RESET);

    while(1) {
        int ch = _getch();
        if (ch == ENTER || ch == ESC) break;
    }
    
    // �˾� �ݱ� (ȭ�� ������ draw_screen���� ó����)
}

void popup_show_difficulty_result(int sem, TimeTable *t) {
    int box_w = 50;
    int box_h = 12;
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 8;

    // ��� �����
    system("cls");

    goto_ansi(start_x + 2, start_y + 1);
    printf("%s %d�б� ���̵� �м� ��� %s", UI_BOLD, sem, UI_RESET);

    goto_ansi(start_x + 2, start_y + 3);
    if (t->argmax_load == NULL) {
        printf("%s1. �ְ� ���̵� ���� (argmax_load)%s", UI_COLOR_YELLOW, UI_RESET);
        goto_ansi(start_x + 5, start_y + 4);
        printf("-> ������ ���� �м� �Ұ�");
    } else {
        goto_ansi(start_x + 2, start_y + 3);
        printf("%s1. �ְ� ���̵� ���� (argmax_load)%s", UI_COLOR_YELLOW, UI_RESET);
        goto_ansi(start_x + 5, start_y + 4);
        printf("-> %s (%d����)", t->argmax_load->name, t->argmax_load->credit);
    }
    goto_ansi(start_x + 2, start_y + 6);
    printf("%s2. �־��� ���� (argmax_synergy)%s", UI_COLOR_YELLOW, UI_RESET);
    goto_ansi(start_x + 5, start_y + 7);
    if (((*t->argmax_synergy)[0].id) == ((*t->argmax_synergy)[1].id)) {
        printf("-> ������ �����Ͽ� �м� �Ұ�");
    } else {
        if(DEBUG_MODE){
            printf("%d %d\n", t->argmax_synergy[0]->id, (*t->argmax_synergy)[1].id);
        }
        printf("-> %s + %s", (*t->argmax_synergy)[0].name, (*t->argmax_synergy)[1].name);
    }

    goto_ansi(start_x + 2, start_y + 9);
    printf("%s3. �� ���̵� (total_difficulty)%s: %.1f / 10.0", UI_COLOR_YELLOW, UI_RESET, t->difficulty);

    goto_ansi(start_x + 12, start_y + 11);
    printf("%s[ Ȯ�� ]%s", UI_BOLD, UI_RESET);

    while(1) {
        int ch = _getch();
        if (ch == ENTER || ch == ESC) break;
    }
}

// --- [�ű� UI] ���̵� ���� �Է� �˾� �� ���� ���� ---
// ��û�Ͻ� ���̵� �Է� ������ ���� ���� ������ �����մϴ�.
StatusCodeEnum popup_input_difficulty(TimeTable* table, int student_id, int semester) {
    if (table == NULL) return ERROR_INVALID_INPUT;

    // �˾� ����
    int box_w = 70;
    int box_h = 7 + table->n * 2; // �� ���̵� �Է� 2�� + ���� �Է� 2*n��
    if (box_h > 25) box_h = 25; // �ִ� ���� ����
    
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

    // �ʱⰪ ����
    int current_input_idx = -1; // -1: �� ���̵�, 0~n-1: ���� ���̵�
    for (int i = 0; i < table->n; i++) subject_difficulties[i] = 0;

    // �Է� �ʵ� ��ȸ
    while (1) {
        // draw_popup ��� �ܺ� �Լ� ȣ��
        draw_input_difficulty_popup(table, semester, total_difficulty, subject_difficulties, 
                                    current_input_idx, box_w, box_h, start_x, start_y);

        // �Է� ���
        int ch = _getch();

        if (ch == ESC) return SUCCESS; // ���

        if (ch == 224 || ch == 0) {
            ch = _getch();
            if (ch == UP_ARROW) {
                if (current_input_idx > -1) current_input_idx--;
            } else if (ch == DOWN_ARROW) {
                if (current_input_idx < table->n - 1) current_input_idx++;
            }
        } else if (ch == ENTER) {
            // ��� �ʵ� �Է� Ȯ��
            if (total_difficulty == -1) {
                 popup_show_message("����", "�� ���̵��� �Է��� �ּ���.");
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
                 popup_show_message("����", "��� ������ ���̵��� �Է��� �ּ���.");
                 continue;
            }
            
            // ���� ���� ���� ����  
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

            // ���� �޽��� �˾� �� ����
            popup_show_message("�Ϸ�", "���̵� ������ ���������� ����Ǿ����ϴ�.");
            return SUCCESS;
        } else if (ch >= '0' && ch <= '9') {
            // ���� �Է� ó�� (���� ���� �Է¸� �����ϴٰ� ����)
            input_buffer[0] = (char)ch;
            input_buffer[1] = '\0';

            // is_valid_input ��� �ܺ� �Լ� ȣ��
            if (is_valid_difficulty_input(input_buffer)) {
                int input_val = atoi(input_buffer);
                if (current_input_idx == -1) {
                    total_difficulty = input_val;
                } else {
                    subject_difficulties[current_input_idx] = input_val;
                }
            } else {
                 // 0~10 ���� �� �Է¿� ���� ó�� (�ʿ��)
            }
        }
    }
}


// ��� ȭ�� �׸���
void draw_output_view(User* user, int btn_idx) {
    system("cls");

    // 1. ���
    printf("================================================================================\n");
    printf("  �� �ð�ǥ Ȯ��                                                 %sID: %5d%s\n", 
            UI_BOLD, user->id, UI_RESET);
    printf("================================================================================\n");

    int start_table_row = 4;
    int box_w = 26; 

    // 2. �б⺰ �ð�ǥ ���
    for (int sem = 0; sem < SEMESTER_NUM; sem++) {
        int col = sem % 3;
        int row = sem / 3;
        int box_x = 2 + (col * box_w);
        int box_y = start_table_row + (row * 8); 

        goto_ansi(box_x, box_y);
        if ((sem + 1) <= user->current_sem) 
            printf("%s[%d �б� (�Ϸ�)]%s", UI_DIM, sem + 1, UI_RESET);
        else 
            printf("%s[%d �б� (����)]%s", UI_COLOR_CYAN, sem + 1, UI_RESET);

        TimeTable* t = user->table[sem];
        int sem_credits = 0;

        if (t == NULL || t->n == 0) {
            goto_ansi(box_x, box_y + 1);
            printf("%s(�������)%s", UI_DIM, UI_RESET);
        } else {
            for (int i = 0; i < t->n; i++) {
                if (i >= 6) { 
                    goto_ansi(box_x, box_y + 1 + i);
                    printf("...�� %d��", t->n - i);
                    break;
                }
                goto_ansi(box_x, box_y + 1 + i);
                printf("- %s (%d)", t->subjects[i]->name, t->subjects[i]->credit);
                sem_credits += t->subjects[i]->credit;
            }
        }
        goto_ansi(box_x + 16, box_y);
        printf("(%d����)", sem_credits);
    }

    // 3. �ϴ� ��ư ���� (4�� ��ư)
    int btn_y = 23;
    int btn_spacing = 18;
    int btn_start_x = (CONSOLE_WIDTH - (btn_spacing * 4)) / 2 + 2;
    
    // ��ư ��
    const char* labels[4] = { "�ð�ǥ ����", "���̵� ���", "������ �߰�", "  Ȩ����  " };

    for (int i = 0; i < 4; i++) {
        goto_ansi(btn_start_x + (i * btn_spacing), btn_y);
        if (btn_idx == i) {
            printf("%s [ %s ] %s", UI_REVERSE, labels[i], UI_RESET);
        } else {
            printf(" [ %s ] ", labels[i]);
        }
    }
}

// Output ��� ���� �Լ�
int run_output(int student_id) {
    User user;
    int is_first;
    
    load_user_data(&user, student_id, &is_first);

    int btn_idx = 0; // 0:����, 1:���, 2:�߰�, 3:Ȩ
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
                return INPUT_PAGE; // �ð�ǥ ����
            } 
            else if (btn_idx == 1 || btn_idx == 2) { 
                // [���̵� ���] �Ǵ� [������ �߰�]
                
                // 1. �б� ���� (course_reg.c�� �ִ� �Լ� ����)
                int selected_sem = popup_select_semester(1, SEMESTER_NUM, "�۾��� �б� ����");
                
                if (selected_sem != -1) {
                    TimeTable* t = user.table[selected_sem - 1];
                    
                    // �����Ͱ� ���� ��� ó��
                    if (t == NULL || t->n == 0) {
                        popup_show_message("����", "������ �б⿡ ��ϵ� ������ �����ϴ�.");
                    } 
                    else {
                        if (btn_idx == 1) {
                            StatusCodeEnum status = calculate_difficulty(t);
                            if (status != SUCCESS) {
                                popup_show_message("����", "���̵� ��� �� ������ �߻��߽��ϴ�.");
                            }
                            else popup_show_difficulty_result(selected_sem, t);
                        } 
                        else { 
                            // [������ �߰�] ���� (�ű� �˾� ȣ��)
                            StatusCodeEnum status = popup_input_difficulty(t, student_id, selected_sem);
                            if (status == ERROR_FILE_NOT_FOUND) {
                                popup_show_message("����", "���� ���� ��ο� ������ �� �����ϴ�.");
                            }
                            // Note: popup_input_difficulty ���ο��� ���� �޽����� ���
                        }
                    }
                }
            } 
            else { // btn_idx == 3
                return HOME_PAGE;  // Ȩ����
            }
        }
        else if (ch == ESC) {
            return HOME_PAGE;
        }
    }
}
