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

#ifdef _WIN32
    void enable_virtual_terminal_processing() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        
        // 1. 인코딩을 UTF-8로 설정하여 한글 깨짐 방지
        SetConsoleOutputCP(949);

        // 2. Virtual Terminal Processing 모드 활성화 (ANSI 코드 해석)
        if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
    }
}
#endif

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
        printf("%s[+] 게시글 생성%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[+] 게시글 생성");
    }

    int planned_x = (CONSOLE_WIDTH / 2) - (int)strlen("개설 예정 과목") / 2 - 3;
    goto_ansi(planned_x, 22);
    if (selected_index == planned_idx) {
        printf("%s[개설 예정 과목]%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[개설 예정 과목]");
    }

    int exit_x = CONSOLE_WIDTH - (int)strlen("종료") - 6;
    goto_ansi(exit_x, 22);
    if (selected_index == exit_idx) {
        printf("%s[종료]%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[종료]");
    }
}

static void render_post_row(const BoardPost *post, int row, int is_selected) {
    goto_ansi(START_X, START_Y + 5 + row);
    if (is_selected) printf("%s", UI_REVERSE);

    printf("#%02d | %s | %d/%d명 | %s",
           post->id,
           (post->subject && strlen(post->subject->name) > 0) ? post->subject->name : "(제목 없음)",
           post->current_students,
           post->target_students,
           post->promo_message);

    if (is_selected) printf("%s", UI_RESET);
}

static void render_board(const BoardPost *posts, int post_count, int selected_index, int scroll_offset) {
    printf("\x1B[2J\x1B[H");
    print_center("[게시판]", 12, 2);

    goto_ansi(START_X, START_Y + 4);
    printf("%sID | 과목 | 현재/모집 | 홍보문구%s", UI_DIM, UI_RESET);

    if (post_count == 0) {
        goto_ansi(START_X, START_Y + 6);
        printf("게시글이 없습니다. 왼쪽 하단에서 새 게시글을 생성하세요.");
    } else {
        int end = scroll_offset + VISIBLE_ROWS;
        if (end > post_count) end = post_count;
        for (int i = scroll_offset; i < end; i++) {
            render_post_row(&posts[i], i - scroll_offset, selected_index == i);
        }

        if (scroll_offset > 0) {
            goto_ansi(CONSOLE_WIDTH - 8, START_Y + 5);
            printf("%s▲%s", UI_DIM, UI_RESET);
        }
        if (scroll_offset + VISIBLE_ROWS < post_count) {
            goto_ansi(CONSOLE_WIDTH - 8, START_Y + 5 + VISIBLE_ROWS - 1);
            printf("%s▼%s", UI_DIM, UI_RESET);
        }
    }

    render_buttons(selected_index, post_count);
    goto_ansi(START_X, START_Y + 18);
    printf("%s↑/↓ 선택, ←/→ 버튼 이동, Enter 확정%s", UI_DIM, UI_RESET);
}

static int choose_user_id(void) {
    int value = 0;

    while (1) {
        printf("\x1B[2J\x1B[H");
        print_center("사용자 ID를 선택하세요", 24, 4);
        goto_ansi(START_X, START_Y + 7);
        printf("%s<%s  %s%d%s  %s>%s", UI_COLOR_CYAN, UI_RESET, UI_BOLD, value, UI_RESET, UI_COLOR_CYAN, UI_RESET);
        goto_ansi(START_X, START_Y + 10);
        printf("%s←/→ 조절, Enter 확정%s", UI_DIM, UI_RESET);

        int ch = read_key();
        if (ch == LEFT_ARROW && value > 0) value--;
        else if (ch == RIGHT_ARROW && value < ID_NUM - 1) value++;
        else if (ch == ENTER) return value;
    }
}

static ApplyResult show_apply_screen(BoardPost *post, int user_id) {
    int selected = 0; // 0: 확인, 1: 취소
    int already_applied = is_user_applied(post, user_id);
    ApplyResult result = APPLY_NO_CHANGE;

    while (1) {
        printf("\x1B[2J\x1B[H");
        print_center("[신청 확인]", 14, 2);

        goto_ansi(START_X, START_Y + 5);
        printf("과목: %s", post->subject ? post->subject->name : "(알 수 없음)");
        goto_ansi(START_X, START_Y + 6);
        printf("모집 %d명 / 현재 %d명", post->target_students, post->current_students);
        goto_ansi(START_X, START_Y + 7);
        printf("신청자 ID: %d", user_id);
        goto_ansi(START_X, START_Y + 10);
        if (already_applied) {
            printf("신청을 취소하시겠습니까?");
        } else {
            printf("신청하시겠습니까?");
        }

        goto_ansi(START_X, START_Y + 13);
        if (selected == 0) printf("%s[확인]%s  [취소]", UI_REVERSE, UI_RESET);
        else printf("[확인]  %s[취소]%s", UI_REVERSE, UI_RESET);

        goto_ansi(START_X, START_Y + 16);
        printf("%s←/→ 이동, Enter 선택%s", UI_DIM, UI_RESET);

        int ch = read_key();
        if (ch == LEFT_ARROW) selected = 0;
        else if (ch == RIGHT_ARROW) selected = 1;
        else if (ch == ESC) return result;
        else if (ch == ENTER) {
            if (selected == 0) {
                StatusCode code = already_applied ? cancel_post(post, user_id) : apply_post(post, user_id);
                printf("\x1B[2J\x1B[H");
                if (code == SUCCESS) {
                    if (already_applied) {
                        print_center("신청이 취소되었습니다!", 24, 6);
                        result = APPLY_CANCELLED;
                    } else {
                        print_center("신청이 완료되었습니다!", 24, 6);
                        result = APPLY_APPLIED;
                        if (post->current_students >= post->target_students) {
                            result = APPLY_FILLED;
                        }
                    }
                } else {
                    print_center("처리에 실패했습니다.", 22, 6);
                }
                goto_ansi(START_X, START_Y + 8);
                printf("결과 코드: %d", code);
                pause_message("계속하려면 아무 키나 누르세요...");
            }
            return result;
        }
    }
}

static int add_post(BoardPost *posts, int count) {
    if (count >= MAX_POSTS) return count;

    StatusCode code = create_post(&posts[count]);
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
        print_center("[개설 예정 과목]", 20, 2);

        if (planned_count == 0) {
            goto_ansi(START_X, START_Y + 5);
            printf("아직 개설 예정인 과목이 없습니다.");
        } else {
            goto_ansi(START_X, START_Y + 4);
            printf("%sID | 과목 | 홍보문구%s", UI_DIM, UI_RESET);
            for (int i = 0; i < planned_count && i < VISIBLE_ROWS; i++) {
                goto_ansi(START_X, START_Y + 6 + i);
                const BoardPost *p = &planned[i];
                printf("#%02d | %s | %s", p->id, p->subject ? p->subject->name : "(제목 없음)", p->promo_message);
            }
        }

        goto_ansi(START_X, START_Y + 18);
        printf("%s[확인]%s", UI_REVERSE, UI_RESET);

        int ch = read_key();
        if (ch == ENTER || ch == ESC) return;
    }
}

int main(void) {
#ifdef _WIN32
    enable_virtual_terminal_processing();
#endif

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
// 일부 MinGW 설정에서 GUI 서브시스템을 기본값으로 사용할 때 WinMain이 필요하므로
// 콘솔 프로그램 엔트리를 main에 위임한다.
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;
    return main();
}
#endif