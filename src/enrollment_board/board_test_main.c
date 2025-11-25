#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "board.h"

#define MAX_POSTS 20
#define VISIBLE_ROWS 12

typedef enum {
    APPLY_NO_CHANGE,
    APPLY_APPLIED,
    APPLY_CANCELLED,
    APPLY_FILLED
} ApplyResult;

static int read_key(void) {
#ifdef _WIN32
    int ch = _getch();
    if (ch == 0 || ch == 224) ch = _getch();
#else
    int ch = getch();
    if (ch == 0 || ch == 224) ch = getch();
#endif
    return ch;
}

static void clear_board_post(BoardPost *post) {
    if (post != NULL && post->subject != NULL) {
        free(post->subject);
        post->subject = NULL;
    }
}

static void pause_message(const char *msg) {
    goto_ansi(START_X, START_Y + 18);
    printf("%s%s%s", UI_DIM, msg, UI_RESET);
    read_key();
}

static void render_buttons(int selected_index, int post_count) {
    int create_idx = post_count;
    int planned_idx = post_count + 1;
    int exit_idx = post_count + 2;

    goto_ansi(3, 22);
    if (selected_index == create_idx) {
        printf("%s[+] �Խñ� ����%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[+] �Խñ� ����");
    }

    int planned_x = (CONSOLE_WIDTH / 2) - (int)strlen("���� ���� ����") / 2 - 3;
    goto_ansi(planned_x, 22);
    if (selected_index == planned_idx) {
        printf("%s[���� ���� ����]%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[���� ���� ����]");
    }

    int exit_x = CONSOLE_WIDTH - (int)strlen("����") - 6;
    goto_ansi(exit_x, 22);
    if (selected_index == exit_idx) {
        printf("%s[����]%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[����]");
    }
}

static void render_post_row(const BoardPost *post, int row, int is_selected) {
    goto_ansi(START_X, START_Y + 5 + row);
    if (is_selected) printf("%s", UI_REVERSE);

    printf("#%02d | %s | %d/%d�� | %s",
           post->id,
           (post->subject && strlen(post->subject->name) > 0) ? post->subject->name : "(���� ����)",
           post->current_students,
           post->target_students,
           post->promo_message);

    if (is_selected) printf("%s", UI_RESET);
}

static void render_board(const BoardPost *posts, int post_count, int selected_index, int scroll_offset) {
    printf("\x1B[2J\x1B[H");
    print_center("[�Խ���]", 12, 2);

    goto_ansi(START_X, START_Y + 4);
    printf("%sID | ���� | ����/���� | ȫ������%s", UI_DIM, UI_RESET);

    if (post_count == 0) {
        goto_ansi(START_X, START_Y + 6);
        printf("�Խñ��� �����ϴ�. ���� �ϴܿ��� �� �Խñ��� �����ϼ���.");
    } else {
        int end = scroll_offset + VISIBLE_ROWS;
        if (end > post_count) end = post_count;
        for (int i = scroll_offset; i < end; i++) {
            render_post_row(&posts[i], i - scroll_offset, selected_index == i);
        }

        if (scroll_offset > 0) {
            goto_ansi(CONSOLE_WIDTH - 8, START_Y + 5);
            printf("%s��%s", UI_DIM, UI_RESET);
        }
        if (scroll_offset + VISIBLE_ROWS < post_count) {
            goto_ansi(CONSOLE_WIDTH - 8, START_Y + 5 + VISIBLE_ROWS - 1);
            printf("%s��%s", UI_DIM, UI_RESET);
        }
    }

    render_buttons(selected_index, post_count);
    goto_ansi(START_X, START_Y + 18);
    printf("%s��/�� ����, ��/�� ��ư �̵�, Enter Ȯ��%s", UI_DIM, UI_RESET);
}

static int choose_user_id(void) {
    int value = 0;

    while (1) {
        printf("\x1B[2J\x1B[H");
        print_center("����� ID�� �����ϼ���", 24, 4);
        goto_ansi(START_X, START_Y + 7);
        printf("%s<%s  %s%d%s  %s>%s", UI_COLOR_CYAN, UI_RESET, UI_BOLD, value, UI_RESET, UI_COLOR_CYAN, UI_RESET);
        goto_ansi(START_X, START_Y + 10);
        printf("%s��/�� ����, Enter Ȯ��%s", UI_DIM, UI_RESET);

        int ch = read_key();
        if (ch == LEFT_ARROW && value > 0) value--;
        else if (ch == RIGHT_ARROW && value < ID_NUM - 1) value++;
        else if (ch == ENTER) return value;
    }
}

static ApplyResult show_apply_screen(BoardPost *post, int user_id) {
    int selected = 0; // 0: Ȯ��, 1: ���
    int already_applied = is_user_applied(post, user_id);
    ApplyResult result = APPLY_NO_CHANGE;

    while (1) {
        printf("\x1B[2J\x1B[H");
        print_center("[��û Ȯ��]", 14, 2);

        goto_ansi(START_X, START_Y + 5);
        printf("����: %s", post->subject ? post->subject->name : "(�� �� ����)");
        goto_ansi(START_X, START_Y + 6);
        printf("���� %d�� / ���� %d��", post->target_students, post->current_students);
        goto_ansi(START_X, START_Y + 7);
        printf("��û�� ID: %d", user_id);
        goto_ansi(START_X, START_Y + 10);
        if (already_applied) {
            printf("��û�� ����Ͻðڽ��ϱ�?");
        } else {
            printf("��û�Ͻðڽ��ϱ�?");
        }

        goto_ansi(START_X, START_Y + 13);
        if (selected == 0) printf("%s[Ȯ��]%s  [���]", UI_REVERSE, UI_RESET);
        else printf("[Ȯ��]  %s[���]%s", UI_REVERSE, UI_RESET);

        goto_ansi(START_X, START_Y + 16);
        printf("%s��/�� �̵�, Enter ����%s", UI_DIM, UI_RESET);

        int ch = read_key();
        if (ch == LEFT_ARROW) selected = 0;
        else if (ch == RIGHT_ARROW) selected = 1;
        else if (ch == ESC) return result;
        else if (ch == ENTER) {
            if (selected == 0) {
                StatusCodeEnum code = already_applied ? cancel_post(post, user_id) : apply_post(post, user_id);
                printf("\x1B[2J\x1B[H");
                if (code == SUCCESS) {
                    if (already_applied) {
                        print_center("��û�� ��ҵǾ����ϴ�!", 24, 6);
                        result = APPLY_CANCELLED;
                    } else {
                        print_center("��û�� �Ϸ�Ǿ����ϴ�!", 24, 6);
                        result = APPLY_APPLIED;
                        if (post->current_students >= post->target_students) {
                            result = APPLY_FILLED;
                        }
                    }
                } else {
                    print_center("ó���� �����߽��ϴ�.", 22, 6);
                }
                goto_ansi(START_X, START_Y + 8);
                printf("��� �ڵ�: %d", code);
                pause_message("����Ϸ��� �ƹ� Ű�� ��������...");
            }
            return result;
        }
    }
}

static int add_post(BoardPost *posts, int count) {
    if (count >= MAX_POSTS) return count;

    StatusCodeEnum code = create_post(&posts[count]);
    if (code == SUCCESS) {
        return count + 1;
    }

    clear_board_post(&posts[count]);
    return count;
}

static void shift_left(BoardPost *posts, int *count, int start_index, int free_first) {
    if (free_first) {
        clear_board_post(&posts[start_index]);
    }

    for (int i = start_index; i < *count - 1; i++) {
        posts[i] = posts[i + 1];
    }

    if (*count > 0) {
        posts[*count - 1] = (BoardPost){0};
    }

    (*count)--;
}

static void move_to_planned(BoardPost *planned, int *planned_count, BoardPost *post) {
    if (*planned_count >= MAX_POSTS) return;
    planned[*planned_count] = *post;
    (*planned_count)++;
}

static void show_planned_courses(const BoardPost *planned, int planned_count) {
    while (1) {
        printf("\x1B[2J\x1B[H");
        print_center("[���� ���� ����]", 20, 2);

        if (planned_count == 0) {
            goto_ansi(START_X, START_Y + 5);
            printf("���� ���� ������ ������ �����ϴ�.");
        } else {
            goto_ansi(START_X, START_Y + 4);
            printf("%sID | ���� | ȫ������%s", UI_DIM, UI_RESET);
            for (int i = 0; i < planned_count && i < VISIBLE_ROWS; i++) {
                goto_ansi(START_X, START_Y + 6 + i);
                const BoardPost *p = &planned[i];
                printf("#%02d | %s | %s", p->id, p->subject ? p->subject->name : "(���� ����)", p->promo_message);
            }
        }

        goto_ansi(START_X, START_Y + 18);
        printf("%s[Ȯ��]%s", UI_REVERSE, UI_RESET);

        int ch = read_key();
        if (ch == ENTER || ch == ESC) return;
    }
}

int main(void) {
    BoardPost posts[MAX_POSTS] = {0};
    BoardPost planned[MAX_POSTS] = {0};
    int post_count = 0;
    int planned_count = 0;
    int selected_index = 0;
    int scroll_offset = 0;
    int user_id = choose_user_id();

    while (1) {
        int total_items = post_count + 3; // posts + create + planned + exit
        if (selected_index >= total_items) selected_index = total_items - 1;

        if (selected_index < scroll_offset) scroll_offset = selected_index;
        if (selected_index >= scroll_offset + VISIBLE_ROWS && selected_index < post_count) {
            scroll_offset = selected_index - VISIBLE_ROWS + 1;
        }
        if (scroll_offset < 0) scroll_offset = 0;

        render_board(posts, post_count, selected_index, scroll_offset);

        int ch = read_key();
        if (ch == UP_ARROW) {
            if (selected_index > 0) selected_index--;
        } else if (ch == DOWN_ARROW) {
            if (selected_index < total_items - 1) selected_index++;
        } else if (ch == LEFT_ARROW) {
            int create_idx = post_count;
            int planned_idx = post_count + 1;
            int exit_idx = post_count + 2;
            if (selected_index == planned_idx) selected_index = create_idx;
            else if (selected_index == exit_idx) selected_index = planned_idx;
        } else if (ch == RIGHT_ARROW) {
            int create_idx = post_count;
            int planned_idx = post_count + 1;
            int exit_idx = post_count + 2;
            if (selected_index == create_idx) selected_index = planned_idx;
            else if (selected_index == planned_idx) selected_index = exit_idx;
        } else if (ch == ENTER) {
            int create_idx = post_count;
            int planned_idx = post_count + 1;
            int exit_idx = post_count + 2;

            if (selected_index < post_count) {
                ApplyResult result = show_apply_screen(&posts[selected_index], user_id);
                if (result == APPLY_FILLED) {
                    move_to_planned(planned, &planned_count, &posts[selected_index]);
                    shift_left(posts, &post_count, selected_index, 0);
                    if (selected_index >= post_count && post_count > 0) selected_index = post_count - 1;
                }
            } else if (selected_index == create_idx) {
                post_count = add_post(posts, post_count);
                if (post_count > 0) selected_index = post_count - 1;
            } else if (selected_index == planned_idx) {
                show_planned_courses(planned, planned_count);
            } else if (selected_index == exit_idx) {
                for (int i = 0; i < post_count; i++) clear_board_post(&posts[i]);
                for (int i = 0; i < planned_count; i++) clear_board_post(&planned[i]);
                return 0;
            }
        }
    }
}

#ifdef _WIN32
// �Ϻ� MinGW �������� GUI ����ý����� �⺻������ ����� �� WinMain�� �ʿ��ϹǷ�
// �ܼ� ���α׷� ��Ʈ���� main�� �����Ѵ�.
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;
    return main();
}
#endif