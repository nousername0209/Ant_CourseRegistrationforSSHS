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
<<<<<<< HEAD
        printf("ï¿½ï¿½ï¿½ï¿½: ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½ (%s).\n", filename);
=======
        printf("¿À·ù: ÆÄÀÏÀ» ÀúÀåÇÒ ¼ö ¾ø½À´Ï´Ù (%s).\n", filename);
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
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
<<<<<<< HEAD
                printf("%s%s > %d ï¿½Ð±ï¿½ %s", UI_REVERSE, UI_COLOR_YELLOW, target_sem, UI_RESET);
            } else {
                printf("%s   %d ï¿½Ð±ï¿½ %s", UI_REVERSE, target_sem, UI_RESET);
=======
                printf("%s%s > %d ÇÐ±â %s", UI_REVERSE, UI_COLOR_YELLOW, target_sem, UI_RESET);
            } else {
                printf("%s   %d ÇÐ±â %s", UI_REVERSE, target_sem, UI_RESET);
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
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
<<<<<<< HEAD
    const char* menus[] = { "1. ï¿½Ð±ï¿½ ï¿½ï¿½ï¿½ï¿½", "2. ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½", "3. ï¿½ï¿½ï¿½" };
=======
    const char* menus[] = { "1. ÇÐ±â ¼öÁ¤", "2. °ú¸ñ »èÁ¦", "3. Ãë¼Ò" };
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
    int menu_cnt = 3;
    
    int box_w = 20;
    int box_h = menu_cnt + 4;
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 10;

<<<<<<< HEAD
    // ï¿½ï¿½ï¿½
=======
    // ¹è°æ
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
    for(int i=0; i<box_h; i++) {
        goto_ansi(start_x, start_y + i);
        printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET); 
    }

    int ch;
    while(1) {
        goto_ansi(start_x + 2, start_y + 1);
<<<<<<< HEAD
        printf("%s ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ %s", UI_BOLD, UI_RESET); 
=======
        printf("%s °ú¸ñ °ü¸® %s", UI_BOLD, UI_RESET); 
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)

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
<<<<<<< HEAD
            // ï¿½ï¿½ï¿½ï¿½ï¿½
=======
            // Áö¿ì±â
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%*s", box_w, ""); 
            }
<<<<<<< HEAD
            return selected_idx + 1; // 1, 2, 3 ï¿½ï¿½È¯
=======
            return selected_idx + 1; // 1, 2, 3 ¹ÝÈ¯
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
        } else if (ch == ESC) {
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%*s", box_w, ""); 
            }
            return 0;
        }
    }
}

<<<<<<< HEAD
// [ï¿½ß°ï¿½] ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ô¼ï¿½
=======
// [Ãß°¡] °ú¸ñ »èÁ¦ ÇÔ¼ö
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
void delete_subject(User* user, int sem_idx, int sub_idx) {
    TimeTable* t = user->table[sem_idx];
    if (t == NULL || sub_idx >= t->n) return;

<<<<<<< HEAD
    // ï¿½Þ¸ï¿½ ï¿½ï¿½ï¿½ï¿½
    free(t->subjects[sub_idx]);
    
    // ï¿½è¿­ ï¿½ï¿½ï¿½ï¿½
    for (int i = sub_idx; i < t->n - 1; i++) {
        t->subjects[i] = t->subjects[i+1];
    }
    t->subjects[t->n - 1] = NULL; // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê±ï¿½È­
    t->n--;
}

// [ï¿½ß°ï¿½] ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ô¼ï¿½ (ï¿½Ð±ï¿½ ï¿½Ìµï¿½)
void modify_subject(User* user, int old_sem_idx, int sub_idx, int new_sem) {
    if (old_sem_idx == new_sem - 1) return; // ï¿½ï¿½ï¿½ï¿½ ï¿½Ð±ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
=======
    // ¸Þ¸ð¸® ÇØÁ¦
    free(t->subjects[sub_idx]);
    
    // ¹è¿­ ´ç±â±â
    for (int i = sub_idx; i < t->n - 1; i++) {
        t->subjects[i] = t->subjects[i+1];
    }
    t->subjects[t->n - 1] = NULL; // ¸¶Áö¸· Æ÷ÀÎÅÍ ÃÊ±âÈ­
    t->n--;
}

// [Ãß°¡] °ú¸ñ ¼öÁ¤ ÇÔ¼ö (ÇÐ±â ÀÌµ¿)
void modify_subject(User* user, int old_sem_idx, int sub_idx, int new_sem) {
    if (old_sem_idx == new_sem - 1) return; // °°Àº ÇÐ±â¸é ¹«½Ã
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)

    TimeTable* old_t = user->table[old_sem_idx];
    if (old_t == NULL || sub_idx >= old_t->n) return;

<<<<<<< HEAD
    // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    Subject* target = old_t->subjects[sub_idx];
    
    // ï¿½ï¿½ ï¿½Ð±ï¿½ ï¿½ï¿½ï¿½Ìºï¿½ È®ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
=======
    // ±âÁ¸ °ú¸ñ Æ÷ÀÎÅÍ °¡Á®¿À±â
    Subject* target = old_t->subjects[sub_idx];
    
    // »õ ÇÐ±â Å×ÀÌºí È®ÀÎ ¹× »ý¼º
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
    int new_sem_idx = new_sem - 1;
    if (user->table[new_sem_idx] == NULL) {
        user->table[new_sem_idx] = (TimeTable*)calloc(1, sizeof(TimeTable));
    }
    TimeTable* new_t = user->table[new_sem_idx];

    if (new_t->n < MAX_SUBJECT_NUM) {
<<<<<<< HEAD
        // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®
        target->semester = new_sem;
        // ï¿½ï¿½ ï¿½ï¿½ï¿½Ìºï¿½ï¿½ï¿½ ï¿½ß°ï¿½
        new_t->subjects[new_t->n++] = target;
        
        // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ìºï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½Þ¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Í¸ï¿½ ï¿½Ìµï¿½ï¿½Ï¹Ç·ï¿½ ï¿½è¿­ï¿½ï¿½ ï¿½ï¿½ï¿½)
=======
        // Á¤º¸ ¾÷µ¥ÀÌÆ®
        target->semester = new_sem;
        // »õ Å×ÀÌºí¿¡ Ãß°¡
        new_t->subjects[new_t->n++] = target;
        
        // ±âÁ¸ Å×ÀÌºí¿¡¼­ Á¦°Å (¸Þ¸ð¸® ÇØÁ¦ ¾øÀÌ Æ÷ÀÎÅÍ¸¸ ÀÌµ¿ÇÏ¹Ç·Î ¹è¿­¸¸ ´ç±è)
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
        for (int i = sub_idx; i < old_t->n - 1; i++) {
            old_t->subjects[i] = old_t->subjects[i+1];
        }
        old_t->subjects[old_t->n - 1] = NULL;
        old_t->n--;
    } else {
<<<<<<< HEAD
        // ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ìµï¿½ ï¿½Ò°ï¿½ ï¿½Ë¸ï¿½ ï¿½ï¿½ (ï¿½ï¿½ï¿½ï¿½)
    }
}

// [ï¿½ï¿½ï¿½ï¿½] È­ï¿½ï¿½ ï¿½×¸ï¿½ï¿½ï¿½ (focus_area: 0=Tree, 1=Buttons, 2=Timetable)
=======
        // ²Ë Â÷¼­ ÀÌµ¿ ºÒ°¡ ¾Ë¸² µî (»ý·«)
    }
}

// [¼öÁ¤] È­¸é ±×¸®±â (focus_area: 0=Tree, 1=Buttons, 2=Timetable)
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
void draw_screen(User* user, Subject* root, int col_idx, int row_indices[3], 
                 int focus_area, int btn_idx, int edit_mode, int timetable_select_idx) {
    system("cls");

<<<<<<< HEAD
    // 1. ï¿½ï¿½Ü¹ï¿½
    char mode_str[50];
    char color_code[10];
    if (edit_mode == 0) {
        strcpy(mode_str, "ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½");
        strcpy(color_code, UI_COLOR_GREEN);
    } else {
        strcpy(mode_str, "ï¿½ï¿½ï¿½ï¿½ ï¿½Ï·ï¿½");
=======
    // 1. »ó´Ü¹Ù
    char mode_str[50];
    char color_code[10];
    if (edit_mode == 0) {
        strcpy(mode_str, "¼ö°­ ¿¹Á¤");
        strcpy(color_code, UI_COLOR_GREEN);
    } else {
        strcpy(mode_str, "¼ö°­ ¿Ï·á");
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
        strcpy(color_code, UI_COLOR_YELLOW);
    }

    printf("================================================================================\n");
<<<<<<< HEAD
    printf("  ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ã» | %s%s%s | ï¿½ï¿½ï¿½ï¿½ ï¿½Ì¼ï¿½ ï¿½Ð±ï¿½: %dï¿½Ð±ï¿½                     %sID: %5d%s\n", 
            color_code, mode_str, UI_RESET, user->current_sem, UI_BOLD, user->id, UI_RESET);
    printf("================================================================================\n");

    // 2. Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Øºï¿½
=======
    printf("  ¼ö°­½ÅÃ» | %s%s%s | ÇöÀç ÀÌ¼ö ÇÐ±â: %dÇÐ±â                     %sID: %5d%s\n", 
            color_code, mode_str, UI_RESET, user->current_sem, UI_BOLD, user->id, UI_RESET);
    printf("================================================================================\n");

    // 2. Æ®¸® µ¥ÀÌÅÍ ÁØºñ
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
    Subject* cols[3] = {root, NULL, NULL};
    if (cols[0]->n > 0 && row_indices[0] < cols[0]->n) 
        cols[1] = cols[0]->arr[row_indices[0]];
    if (cols[1] != NULL && cols[1]->isFile == 0 && cols[1]->n > 0 && row_indices[1] < cols[1]->n) 
        cols[2] = cols[1]->arr[row_indices[1]];

<<<<<<< HEAD
    // 3. ï¿½Ã·ï¿½ ï¿½×¸ï¿½ï¿½ï¿½
=======
    // 3. ÄÃ·³ ±×¸®±â
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
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

<<<<<<< HEAD
    // 4. ï¿½Ï´ï¿½ ï¿½ï¿½Æ° ï¿½ï¿½ï¿½ï¿½
=======
    // 4. ÇÏ´Ü ¹öÆ° ¿µ¿ª
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
    int btn_y = START_Y + 12;
    int btn_spacing = 22;
    int btn_start_x = (CONSOLE_WIDTH - (btn_spacing * 3)) / 2 + 2;
    const char* labels[3] = { 
<<<<<<< HEAD
        "  ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½  ", 
        "ï¿½Ì¼ï¿½ ï¿½Ð±ï¿½ ï¿½ï¿½ï¿½ï¿½", 
        "    È®ï¿½ï¿½     " 
=======
        "  ¸ðµå º¯°æ  ", 
        "ÀÌ¼ö ÇÐ±â º¯°æ", 
        "    È®Á¤     " 
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
    };
    for (int i = 0; i < 3; i++) {
        goto_ansi(btn_start_x + (i * btn_spacing), btn_y);
        if (focus_area == 1 && btn_idx == i) {
            printf("%s [ %s ] %s", UI_REVERSE, labels[i], UI_RESET);
        } else {
            printf(" [ %s ] ", labels[i]);
        }
    }

<<<<<<< HEAD
    // 5. ï¿½Ã°ï¿½Ç¥ ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½×¸ï¿½ï¿½ï¿½
    int table_y = btn_y + 3;   
    int total_credits = 0;
    int total_subjects = 0;
    // ï¿½ï¿½Ã¼ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½
=======
    // 5. ½Ã°£Ç¥ ¿ä¾à ¹× ±×¸®µå
    int table_y = btn_y + 3;   
    int total_credits = 0;
    int total_subjects = 0;
    // ÀüÃ¼ °ú¸ñ ¼ö °è»ê
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
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
<<<<<<< HEAD
    printf("[ ï¿½ï¿½Ã¼ ï¿½Ã°ï¿½Ç¥ ï¿½ï¿½ï¿½ | ï¿½ï¿½ %dï¿½ï¿½ï¿½ï¿½ | ï¿½ï¿½ %dï¿½ï¿½ï¿½ï¿½ ]", total_subjects, total_credits);
=======
    printf("[ ÀüÃ¼ ½Ã°£Ç¥ ¿ä¾à | ÃÑ %d°ú¸ñ | ÃÑ %dÇÐÁ¡ ]", total_subjects, total_credits);
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
    
    int box_w = 26;
    int start_table_row = table_y + 3;
    
<<<<<<< HEAD
    // ï¿½Ã°ï¿½Ç¥ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Îµï¿½ï¿½Ì¿ï¿½ Ä«ï¿½ï¿½ï¿½ï¿½
=======
    // ½Ã°£Ç¥ ¾ÆÀÌÅÛ ÀÎµ¦½Ì¿ë Ä«¿îÅÍ
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
    int current_item_idx = 0;

    for (int sem = 0; sem < SEMESTER_NUM; sem++) {
        int col = sem % 3;
        int row = sem / 3;
        int box_x = 2 + (col * box_w);
        int box_y = start_table_row + (row * 6);

        goto_ansi(box_x, box_y);
<<<<<<< HEAD
        if ((sem + 1) <= user->current_sem) printf("%s[%d ï¿½Ð±ï¿½ (ï¿½Ì¼ï¿½)]%s", UI_DIM, sem + 1, UI_RESET);
        else printf("%s[%d ï¿½Ð±ï¿½ (ï¿½ï¿½ï¿½ï¿½)]%s", UI_COLOR_YELLOW, sem + 1, UI_RESET);
=======
        if ((sem + 1) <= user->current_sem) printf("%s[%d ÇÐ±â (ÀÌ¼ö)]%s", UI_DIM, sem + 1, UI_RESET);
        else printf("%s[%d ÇÐ±â (¿¹Á¤)]%s", UI_COLOR_YELLOW, sem + 1, UI_RESET);
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)

        TimeTable* t = user->table[sem];
        if (t == NULL || t->n == 0) {
            goto_ansi(box_x, box_y + 1);
<<<<<<< HEAD
            printf("%s(ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½)%s", UI_DIM, UI_RESET);
=======
            printf("%s(ºñ¾îÀÖÀ½)%s", UI_DIM, UI_RESET);
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
        } else {
            for (int i = 0; i < t->n; i++) {
                if (i >= 4) {
                    goto_ansi(box_x, box_y + 1 + i);
<<<<<<< HEAD
                    printf("...ï¿½ï¿½ %dï¿½ï¿½", t->n - i);
=======
                    printf("...¿Ü %d°Ç", t->n - i);
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
                    current_item_idx += (t->n - i); 
                    break; 
                }
                
                goto_ansi(box_x, box_y + 1 + i);
                
<<<<<<< HEAD
                // ï¿½Ã°ï¿½Ç¥ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ä¿ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½Ì¶ï¿½ï¿½ï¿½Æ®
=======
                // ½Ã°£Ç¥ ¿µ¿ª Æ÷Ä¿½º ½Ã ÇÏÀÌ¶óÀÌÆ®
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
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
<<<<<<< HEAD
        user.current_sem = popup_select_semester(1, SEMESTER_NUM, "ï¿½ï¿½ï¿½ï¿½ ï¿½Ì¼ï¿½ ï¿½Ð±â¸¦ ï¿½ï¿½ï¿½ï¿½ï¿½Ï¼ï¿½ï¿½ï¿½");
=======
        user.current_sem = popup_select_semester(1, SEMESTER_NUM, "ÇöÀç ÀÌ¼ö ÇÐ±â¸¦ ¼±ÅÃÇÏ¼¼¿ä");
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
        edit_mode = 1;
    }

    KeyEnum ch;
    while (1) {
        draw_screen(&user, root, col_idx, row_indices, focus_area, btn_idx, edit_mode, timetable_select_idx);
        
        goto_ansi(1, 1);
        ch = _getch();
        
<<<<<<< HEAD
        // ï¿½ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ (ï¿½Ã°ï¿½Ç¥ ï¿½×ºï¿½ï¿½ï¿½Ì¼Ç¿ï¿½)
=======
        // ÃÑ µî·Ï °ú¸ñ ¼ö °è»ê (½Ã°£Ç¥ ³×ºñ°ÔÀÌ¼Ç¿ë)
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
        int total_subjects = 0;
        for(int i=0; i<SEMESTER_NUM; i++) 
            if(user.table[i]) total_subjects += user.table[i]->n;

        if (ch == 224 || ch == 0) {
            ch = _getch(); 

<<<<<<< HEAD
            if (focus_area == 1) { // ï¿½ï¿½Æ° ï¿½ï¿½Ä¿ï¿½ï¿½
=======
            if (focus_area == 1) { // ¹öÆ° Æ÷Ä¿½º
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
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
<<<<<<< HEAD
            else if (focus_area == 2) { // ï¿½Ã°ï¿½Ç¥ ï¿½ï¿½Ä¿ï¿½ï¿½
=======
            else if (focus_area == 2) { // ½Ã°£Ç¥ Æ÷Ä¿½º
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
                if (ch == UP_ARROW) focus_area = 1;
                else if (ch == LEFT_ARROW) { if (timetable_select_idx > 0) timetable_select_idx--; }
                else if (ch == RIGHT_ARROW) { if (timetable_select_idx < total_subjects - 1) timetable_select_idx++; }
            }
<<<<<<< HEAD
            else { // Æ®ï¿½ï¿½ ï¿½ï¿½Ä¿ï¿½ï¿½
=======
            else { // Æ®¸® Æ÷Ä¿½º
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
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
<<<<<<< HEAD
            if (focus_area == 1) { // ï¿½ï¿½Æ° Å¬ï¿½ï¿½
                if (btn_idx == 0) { edit_mode = !edit_mode; }
                else if (btn_idx == 1) { 
                    int new_sem = popup_select_semester(0, SEMESTER_NUM - 1, "ï¿½ï¿½ï¿½ï¿½ ï¿½Ì¼ï¿½ ï¿½Ð±â¸¦ ï¿½ï¿½ï¿½ï¿½ï¿½Ï¼ï¿½ï¿½ï¿½");
=======
            if (focus_area == 1) { // ¹öÆ° Å¬¸¯
                if (btn_idx == 0) { edit_mode = !edit_mode; }
                else if (btn_idx == 1) { 
                    int new_sem = popup_select_semester(0, SEMESTER_NUM - 1, "ÇöÀç ÀÌ¼ö ÇÐ±â¸¦ ¼±ÅÃÇÏ¼¼¿ä");
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
                    if (new_sem != -1) user.current_sem = new_sem;
                }
                else if (btn_idx == 2) { 
                    save_user_data(&user); 
                    system("cls");
<<<<<<< HEAD
                    printf("\nï¿½ï¿½ï¿½ï¿½ ï¿½Ï·ï¿½ (ID: %d). ï¿½ï¿½ï¿½Î±×·ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Õ´Ï´ï¿½.\n", user.id);
                    break;
                }
            }
            else if (focus_area == 2) { // ï¿½Ã°ï¿½Ç¥ ï¿½×¸ï¿½ ï¿½ï¿½ï¿½ï¿½ -> ï¿½ï¿½ï¿½ï¿½/ï¿½ï¿½ï¿½ï¿½ ï¿½Ë¾ï¿½
                int action = popup_edit_menu();
                if (action == 1 || action == 2) { // ï¿½ï¿½ï¿½ï¿½ or ï¿½ï¿½ï¿½ï¿½
                    // ï¿½ï¿½ï¿½Ãµï¿½ ï¿½Îµï¿½ï¿½ï¿½(linear)ï¿½ï¿½ (ï¿½Ð±ï¿½, ï¿½ï¿½ï¿½ï¿½) ï¿½Îµï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½È¯
=======
                    printf("\nÀúÀå ¿Ï·á (ID: %d). ÇÁ·Î±×·¥À» Á¾·áÇÕ´Ï´Ù.\n", user.id);
                    break;
                }
            }
            else if (focus_area == 2) { // ½Ã°£Ç¥ Ç×¸ñ ¼±ÅÃ -> ¼öÁ¤/»èÁ¦ ÆË¾÷
                int action = popup_edit_menu();
                if (action == 1 || action == 2) { // ¼öÁ¤ or »èÁ¦
                    // ¼±ÅÃµÈ ÀÎµ¦½º(linear)¸¦ (ÇÐ±â, °ú¸ñ) ÀÎµ¦½º·Î º¯È¯
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
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
<<<<<<< HEAD
                        if (action == 2) { // ï¿½ï¿½ï¿½ï¿½
                            delete_subject(&user, target_sem, target_sub);
                            // ï¿½Îµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ê°ï¿½ ï¿½ï¿½ï¿½ï¿½)
                            if (timetable_select_idx >= total_subjects - 1) timetable_select_idx--;
                            if (timetable_select_idx < 0) timetable_select_idx = 0;
                        } else { // ï¿½ï¿½ï¿½ï¿½
=======
                        if (action == 2) { // »èÁ¦
                            delete_subject(&user, target_sem, target_sub);
                            // ÀÎµ¦½º Á¶Á¤ (»èÁ¦ ÈÄ ¹üÀ§ ÃÊ°ú ¹æÁö)
                            if (timetable_select_idx >= total_subjects - 1) timetable_select_idx--;
                            if (timetable_select_idx < 0) timetable_select_idx = 0;
                        } else { // ¼öÁ¤
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
                            int start_s, end_s;
                            char title[50];
                            if (edit_mode == 0) { 
                                start_s = user.current_sem + 1; end_s = SEMESTER_NUM;
<<<<<<< HEAD
                                strcpy(title, "ï¿½Ìµï¿½ï¿½ï¿½ ï¿½Ð±ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½ï¿½ï¿½)");
                            } else { 
                                start_s = 1; end_s = user.current_sem;
                                strcpy(title, "ï¿½Ìµï¿½ï¿½ï¿½ ï¿½Ð±ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½ï¿½Ì¼ï¿½)");
=======
                                strcpy(title, "ÀÌµ¿ÇÒ ÇÐ±â ¼±ÅÃ (¿¹Á¤)");
                            } else { 
                                start_s = 1; end_s = user.current_sem;
                                strcpy(title, "ÀÌµ¿ÇÒ ÇÐ±â ¼±ÅÃ (±âÀÌ¼ö)");
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
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
<<<<<<< HEAD
            else if (focus_area == 0 && col_idx == 2) { // ï¿½ï¿½ï¿½ï¿½ ï¿½ß°ï¿½
=======
            else if (focus_area == 0 && col_idx == 2) { // °ú¸ñ Ãß°¡
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
                Subject* parent = root->arr[row_indices[0]]->arr[row_indices[1]];
                Subject* target = parent->arr[row_indices[2]];

                if (target->isFile == 1) {
                    int start_s, end_s;
                    char title[50];
                    if (edit_mode == 0) { 
                        start_s = user.current_sem + 1; end_s = SEMESTER_NUM;
<<<<<<< HEAD
                        strcpy(title, "ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ð±ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½ï¿½ï¿½)");
                    } else { 
                        start_s = 1; end_s = user.current_sem;
                        strcpy(title, "ï¿½ï¿½ï¿½ï¿½ï¿½ß´ï¿½ ï¿½Ð±ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½ï¿½Ì¼ï¿½)");
=======
                        strcpy(title, "¼ö°­ÇÒ ÇÐ±â ¼±ÅÃ (¿¹Á¤)");
                    } else { 
                        start_s = 1; end_s = user.current_sem;
                        strcpy(title, "¼ö°­Çß´ø ÇÐ±â ¼±ÅÃ (±âÀÌ¼ö)");
>>>>>>> parent of 98462c4 (Merge pull request #1 from nousername0209/codex/refactor-code-to-follow-coding-standards)
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
