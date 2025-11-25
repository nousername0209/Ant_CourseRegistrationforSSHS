#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "board.h"

#define MAX_POSTS 20
#define VISIBLE_ROWS 12

/**
 * @brief post_id를 관리하는 함수이다. 이전에 호출됐었다면 그 때 나왔던 값 +1을, 처음 호출됐다면 0을 반환한다.
 * @return 관리하고 있는 id를 반환한다.
 */
static int generate_post_id(void) {
    static int next_id = 0;
    return next_id++;
}

/**
 * @brief 현재 화면에 출력된 결과를 모두 지운다.
 */
static void clear_screen(void) {
    printf("\x1B[2J\x1B[H");
}

/**
 * @brief 재귀적 방법을 이용해 키를 입력받는다. 디폴트 값(0, 224)이라면 계속 입력을 받는다.
 * @return 키보드 입력값(화살표, 엔터 등)을 반환한다.
 */
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

/**
 * @brief 게시글을 작성할 때 목표 인원수를 지정하게 하는 함수이다.
 * @param title 작성할 게시글의 제목이다.
 * @param initial 작성할 게시글의 목표 인원수의 기본값이다. 기본적으로 6명으로 입력된다.
 * @param min 목표 인원수의 최솟값이다. 기본적으로 1명이다.
 * @param max 목표 인원수의 최댓값이다. 기본적으로 50명이다.
 * @return 목표 인원을 int로 반환한다.
 *       - 게시글 작성을 취소할 경우 -1을 반환한다.
 */
static int adjust_value_with_arrows(const char *title, int initial, int min, int max) {
    int value = initial;

    while (1) {
        clear_screen();
        print_center("[게시글 작성]", 18, 2);

        goto_ansi(START_X, START_Y + 5);
        printf("%s%s%s", UI_BOLD, title, UI_RESET);

        goto_ansi(START_X, START_Y + 7);
        printf("%s<%s  %s%d명%s  %s>%s", UI_COLOR_CYAN, UI_RESET, UI_BOLD, value, UI_RESET, UI_COLOR_CYAN, UI_RESET);

        goto_ansi(START_X, START_Y + 10);
        printf("%s좌/우 키로 조절, Enter로 확인 (ESC로 취소)%s", UI_DIM, UI_RESET);

        int ch = read_key();
        if (ch == LEFT_ARROW && value > min) value--;
        else if (ch == RIGHT_ARROW && value < max) value++;
        else if (ch == ENTER) return value;
        else if (ch == ESC) return -1;
    }
}

/**
 * @brief 입력받은 문자열을 출력한다.
 * @param msg 출력할 문자열이다.
 */
static void pause_message(const char *msg) {
    goto_ansi(START_X, START_Y + 16);
    printf("%s%s%s", UI_DIM, msg, UI_RESET);
    read_key();
}

/**
 * @brief 적절한 정보를 입력ㅂ다아 주어진 주소값에 게시글을 만들어 저장한다.
 * @param result 게시글을 저장할 주소값이다.
 * @return 결과를 StatusCodeEnum으로 반환한다.
 *       - SUCCESS: 정상 작동
 *       - ERROR_INVALID_INPUT: calloc 실패 혹은 각종 다양한 Invalid input
 */
StatusCodeEnum create_post(BoardPost *result) {
    if (result == NULL) return ERROR_INVALID_INPUT;

    memset(result, 0, sizeof(BoardPost));
    result->id = generate_post_id();
    for (int i = 0; i < ID_NUM; i++) {
        result->students_id[i] = -1;
    }

    char buffer[256];

    clear_screen();
    print_center("[게시글 작성]", 18, 2);
    goto_ansi(START_X, START_Y + 4);
    printf("과목 이름을 입력하세요: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return ERROR_INVALID_INPUT;
    trim_newline(buffer);

    Subject *subject = (Subject *)calloc(1, sizeof(Subject));
    if (subject == NULL) return ERROR_INVALID_INPUT;

    subject->isFile = 1;
    subject->n = 0;
    subject->id = result->id;
    subject->year = 0;
    subject->semester = 0;
    subject->credit = 0;
    subject->mean_raw_score = 0.0;
    subject->stdev_raw_score = 0.0;
    strncpy(subject->name, buffer, NAME_LENGTH - 1);
    subject->name[NAME_LENGTH - 1] = '\0';

    int target = adjust_value_with_arrows("수강 인원을 설정하세요", 5, 1, ID_NUM);
    if (target < 0) {
        free(subject);
        return ERROR_INVALID_INPUT;
    }

    clear_screen();
    print_center("[게시글 작성]", 18, 2);
    goto_ansi(START_X, START_Y + 4);
    printf("홍보 메시지를 입력하세요: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        free(subject);
        return ERROR_INVALID_INPUT;
    }
    trim_newline(buffer);

    strncpy(result->promo_message, buffer, MESSAGE_LENGTH - 1);
    result->promo_message[MESSAGE_LENGTH - 1] = '\0';

    result->subject = subject;
    result->target_students = target;
    result->current_students = 0;

    clear_screen();
    print_center("게시글이 생성되었습니다!", 24, 8);
    goto_ansi(START_X, START_Y + 10);
    printf("과목명: %s\n", subject->name);
    goto_ansi(START_X, START_Y + 11);
    printf("수강 정원: %d명\n", target);
    goto_ansi(START_X, START_Y + 12);
    printf("홍보 메시지: %s\n", result->promo_message);
    pause_message("돌아가려면 아무 키나 누르세요...");

    return SUCCESS;
}

/**
 * @brief 게시글에 지원하는(게시글의 과목에 참가 신청을 하는) 함수이다.
 * @param post 지원할 게시글이다.
 * @param id 지원하는 사람의 id이다. 중복을 확인해서 한 명이 여러번 신청하는 것을 방지한다.
 * @return 실행 결과를 StatusCodeEnum으로 반환한다.
 */
StatusCodeEnum apply_post(BoardPost *post, int id) {
    if (post == NULL) return ERROR_INVALID_INPUT;
    if (id < 0 || id >= ID_NUM) return ERROR_INVALID_INPUT;
    if (post->target_students <= 0) return ERROR_INVALID_INPUT;
    if (post->current_students >= post->target_students) return ERROR_INDEX_OUT;

    for (int i = 0; i < post->current_students; i++) {
        if (post->students_id[i] == id) {
            return ERROR_INVALID_INPUT;
        }
    }

    if (post->current_students >= ID_NUM) return ERROR_INDEX_OUT;

    post->students_id[post->current_students] = id;
    post->current_students++;

    return SUCCESS;
}

/**
 * @brief 각 게시글에서 사람이 지원했는지, 지원했다면 몇번째로 지원했는지 확인한다.
 * @param post 확인할 게시글이다.
 * @param id 확인할 사람의 id이다.
 * @return 해당 사람이 지원했다면 몇번째로 지원했었는지를 반환한다.(0-based index)
 *       - 만약 해당 사람이 지원하지 않았거나, post가 잘못 입력되었다면 -1을 반환한다.
 */
static int find_student_index(const BoardPost *post, int id) {
    if (post == NULL || id < 0) return -1;

    for (int i = 0; i < post->current_students; i++) {
        if (post->students_id[i] == id) return i;
    }

    return -1;
}

/**
 * @brief 각 게시글에서 사람이 지원했는지 여부를 확인한다.
 * @param post 확인할 계시글이다.
 * @param id 확인할 사람의 id이다.
 * @return 지원했다면 1, 지원하지 않았다면 0을 반환한다.
 */
int is_user_applied(const BoardPost *post, int id) {
    return find_student_index(post, id) >= 0;
}

/**
 * @brief 각 게시글에서 특정 사람의 지원을 취소하는 함수이다.
 * @param post 확인할 계시글이다.
 * @param id 확인할 사람의 id이다.
 * @return 함수의 처리 여부를 StatusCodeEnum으로 반환한다.
 */
StatusCodeEnum cancel_post(BoardPost *post, int id) {
    if (post == NULL) return ERROR_INVALID_INPUT;
    if (id < 0 || id >= ID_NUM) return ERROR_INVALID_INPUT;
    if (post->target_students <= 0) return ERROR_INVALID_INPUT;

    int index = find_student_index(post, id);
    if (index < 0) return ERROR_INVALID_INPUT;

    for (int i = index; i < post->current_students - 1; i++) {
        post->students_id[i] = post->students_id[i + 1];
    }

    post->students_id[post->current_students - 1] = -1;
    post->current_students--;

    return SUCCESS;
}


#ifdef _WIN32
    /**
     * @brief cmd에서 정상적인 출력이 가능케 하도록 하는 함수이다.(화면 전체 지우기, 인코딩 지정 등)
     */
    void enable_virtual_terminal_processing() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        
        // 1. 콘솔을 UTF-8(또는 949)로 설정하여 한글 깨짐 방지
        SetConsoleOutputCP(949);

        // 2. Virtual Terminal Processing 모드 활성화 (ANSI 코드 해석)
        if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

/**
 * @brief 게시글을 삭제하는 함수이다.
 * @param post 삭제할 게시글이다.
 */
static void clear_board_post(BoardPost *post) {
    if (post != NULL && post->subject != NULL) {
        free(post->subject);
        post->subject = NULL;
    }
}


static void render_buttons(int selected_index, int post_count) {
    int create_idx = post_count;
    int planned_idx = post_count + 1;
    int exit_idx = post_count + 2;

    goto_ansi(3, 22);
    if (selected_index == create_idx) {
        printf("%s[+] 게시글 작성%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[+] 게시글 작성");
    }

    int planned_x = (CONSOLE_WIDTH / 2) - (int)strlen("마감된 강의 목록") / 2 - 3;
    goto_ansi(planned_x, 22);
    if (selected_index == planned_idx) {
        printf("%s[마감된 강의 목록]%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[마감된 강의 목록]");
    }

    int exit_x = CONSOLE_WIDTH - (int)strlen("종료") - 6;
    goto_ansi(exit_x, 22);
    if (selected_index == exit_idx) {
        printf("%s[종료]%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[종료]");
    }
}

// 문자열을 max_len 길이(바이트 기준)에 맞춰 자르고 '...'을 붙이는 함수
static void get_truncated_text(char *dest, const char *src, int max_len) {
    int src_len = (int)strlen(src);
    
    // 길이가 충분히 짧으면 그대로 복사
    if (src_len <= max_len) {
        strcpy(dest, src);
        return;
    }

    // '...'이 들어갈 공간(3바이트)을 뺀 길이만큼만 복사 시도
    int limit = max_len - 3;
    int i = 0;
    
    while (i < limit && src[i] != '\0') {
        // 한글/특수문자 여부 확인 (최상위 비트가 1이면 멀티바이트)
        if ((unsigned char)src[i] & 0x80) {
            // 멀티바이트인데 공간이 부족하면 여기서 멈춤
            // (CP949는 2바이트, UTF-8은 3바이트... 안전하게 2바이트 이상 체크)
            if (i + 2 > limit) break; 
            
            // 일단 단순히 바이트를 증가시킵니다. 
            // (정확한 인코딩 체크보다는 깨짐 방지용으로 안전하게 건너뜀)
            i++; 
        }
        i++;
    }

    // 문자열 복사 후 ... 붙이기
    strncpy(dest, src, i);
    dest[i] = '\0';
    strcat(dest, "...");
}

static void render_post_row(const BoardPost *post, int row, int is_selected) {
    goto_ansi(START_X, START_Y + 5 + row);
    if (is_selected) printf("%s", UI_REVERSE);


    char count_buf[20];
    sprintf(count_buf, "%d/%d", post->current_students, post->target_students);
    
    char name_buf[50];
    const char* raw_name = (post->subject && strlen(post->subject->name) > 0) ? post->subject->name : "(제목 없음)";
    get_truncated_text(name_buf, raw_name, 15);

    char promo_buf[60];
    get_truncated_text(promo_buf, post->promo_message, 20);

    printf(" #%-02d | %-15s | %-9s | %-20s",
           post->id,
           name_buf,    // 잘린 제목
           count_buf,
           promo_buf);  // 잘린 메시지

    if (is_selected) printf("%s", UI_RESET);
}

static void render_board(const BoardPost *posts, int post_count, int selected_index, int scroll_offset) {
    printf("\x1B[2J\x1B[H");
    print_center("[수강신청 게시판]", 10, 2);

    goto_ansi(START_X, START_Y + 4);
    printf("%s %-3s | %-15s | %-9s | %s%s", 
           UI_DIM, "ID", "과목명", "현황", "홍보메시지", UI_RESET);

    if (post_count == 0) {
        goto_ansi(START_X, START_Y + 6);
        printf("게시글이 없습니다. 아래 메뉴에서 새 게시글을 작성하세요.");
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
    printf("%s상/하 이동, 좌/우 메뉴 이동, Enter 확인%s", UI_DIM, UI_RESET);
}

static char* choose_user_id(void) {
    // int value = 0;

    // while (1) {
    //     printf("\x1B[2J\x1B[H");
    //     print_center("사용자 ID를 선택하세요", 24, 4);
    //     goto_ansi(START_X, START_Y + 7);
    //     printf("%s<%s  %s%d%s  %s>%s", UI_COLOR_CYAN, UI_RESET, UI_BOLD, value, UI_RESET, UI_COLOR_CYAN, UI_RESET);
    //     goto_ansi(START_X, START_Y + 10);
    //     printf("%s좌/우 이동, Enter 확인%s", UI_DIM, UI_RESET);

    //     int ch = read_key();
    //     if (ch == LEFT_ARROW && value > 0) value--;
    //     else if (ch == RIGHT_ARROW && value < ID_NUM - 1) value++;
    //     else if (ch == ENTER) return value;
    // }
    char *id_buffer = (char *)malloc(21);
    SelectEnum select = ID_FIELD; // 초기화 추가 (원문에는 없었으나 로직상 필요)
    
    // (이 함수 내부의 UI 로직은 원본 코드에서 일부 누락된 것으로 보이나,
    //  깨진 텍스트 복구 요청에 집중하여 영문 텍스트는 그대로 둡니다)
    system("cls");
    
    print_center("=== LOGIN ===", 13, 10);
    
    print_center("User ID: ", 33, 12);
    if (select == ID_FIELD) {
        printf("%s> [%-20s]%s", UI_REVERSE, id_buffer, UI_RESET);
        goto_ansi(START_X + (UI_WIDTH - 33) / 2 + (int)strlen(id_buffer), 12);
    } else {
        printf("  [%-20s]", id_buffer);
    }

    goto_ansi(START_X + (UI_WIDTH - 16)/2, 15);
    if (select == LOGIN_BUTTON) {
        printf("%s[ >> LOGIN << ]%s", UI_REVERSE, UI_RESET);
    } else {
        printf("[    LOGIN    ]");
    }
    fflush(stdout);

    return id_buffer;
}

static ApplyResultEnum show_apply_screen(BoardPost *post, int user_id) {
    int selected = 0; // 0: 확인, 1: 취소
    int already_applied = is_user_applied(post, user_id);
    ApplyResultEnum result = APPLY_NO_CHANGE;

    while (1) {
        printf("\x1B[2J\x1B[H");
        print_center("[신청 확인]", 14, 2);

        goto_ansi(START_X, START_Y + 5);
        printf("과목명: %s", post->subject ? post->subject->name : "(제목 없음)");
        goto_ansi(START_X, START_Y + 6);
        printf("정원 %d명 / 현재 %d명", post->target_students, post->current_students);
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
        printf("%s좌/우 이동, Enter 선택%s", UI_DIM, UI_RESET);

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
                    print_center("처리 중 오류가 발생했습니다.", 22, 6);
                }
                goto_ansi(START_X, START_Y + 8);
                printf("에러 코드: %d", code);
                pause_message("돌아가려면 아무 키나 누르세요...");
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
        print_center("[마감된 강의 목록]", 20, 2);

        if (planned_count == 0) {
            goto_ansi(START_X, START_Y + 5);
            printf("마감된 강의가 없습니다.");
        } else {
            goto_ansi(START_X, START_Y + 4);
            printf("%sID | 과목명 | 홍보메시지%s", UI_DIM, UI_RESET);
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

int board_main(int user_id) {
#ifdef _WIN32
    enable_virtual_terminal_processing();
#endif

    int post_count = 0;
    int planned_count = 0;
    int selected_index = 0;
    int scroll_offset = 0;
    int state[3] = {post_count,
                    planned_count,
                    selected_index};
    BoardPost posts[MAX_POSTS] = {0};
    BoardPost planned[MAX_POSTS] = {0};

    char dir_path[PATH_LENGTH] = "./dataset/enrollment_board";
    char file_path[PATH_LENGTH + 20];
    FILE *fp = NULL;

    sprintf(file_path, "%s/state.dat", dir_path);
    fp = fopen(file_path, "rb");
    if(fp == NULL){
        fp = fopen(file_path, "wb");
        fwrite(state, sizeof(int), 3, fp);
    }
    else{
        fread(state, sizeof(int), 3, fp);
    }
    fclose(fp);
    
    sprintf(file_path, "%s/posts.dat", dir_path);
    fp = fopen(file_path, "rb");
    if(fp == NULL){
        fp = fopen(file_path, "wb");
        fwrite(posts, sizeof(BoardPost), MAX_POSTS, fp);
    }
    else{
        fread(posts, sizeof(BoardPost), MAX_POSTS, fp);
    }
    fclose(fp);

    sprintf(file_path, "%s/planned.dat", dir_path);
    fp = fopen(file_path, "rb");
    if(fp == NULL){
        fp = fopen(file_path, "wb");
        fwrite(planned, sizeof(BoardPost), MAX_POSTS, fp);
    }
    else{
        fread(planned, sizeof(BoardPost), MAX_POSTS, fp);
    }
    fclose(fp);

    
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
                ApplyResultEnum result = show_apply_screen(&posts[selected_index], user_id);
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
                
                sprintf(file_path, "%s/state.dat", dir_path);
                fp = fopen(file_path, "wb");
                fwrite(state, sizeof(int), 3, fp);
                fclose(fp);
                
                sprintf(file_path, "%s/posts.dat", dir_path);
                fp = fopen(file_path, "wb");
                fwrite(posts, sizeof(BoardPost), MAX_POSTS, fp);
                fclose(fp);

                sprintf(file_path, "%s/planned.dat", dir_path);
                fp = fopen(file_path, "wb");
                fwrite(planned, sizeof(BoardPost), MAX_POSTS, fp);
                fclose(fp);
                return 0;
            }
        }
    }
}

#ifdef _WIN32
// 일부 MinGW 환경에서 GUI 서브시스템을 사용하는 경우, 기본 진입점이 
// WinMain이 필요하므로 단일 프로그램 엔트리에서 main을 호출합니다.
// int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
//     (void)hInstance;
//     (void)hPrevInstance;
//     (void)lpCmdLine;
//     (void)nCmdShow;
//     return main();
// }
#endif