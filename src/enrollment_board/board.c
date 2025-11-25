#include "board.h"

static int generate_post_id(void) {
    static int next_id = 0;
    return next_id++;
}

static void clear_screen(void) {
    printf("\x1B[2J\x1B[H");
}

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
        printf("%s←/→%s 로 조절, Enter로 확정 (ESC로 취소)", UI_DIM, UI_RESET);

        int ch = read_key();
        if (ch == LEFT_ARROW && value > min) value--;
        else if (ch == RIGHT_ARROW && value < max) value++;
        else if (ch == ENTER) return value;
        else if (ch == ESC) return -1;
    }
}

static void pause_message(const char *msg) {
    goto_ansi(START_X, START_Y + 16);
    printf("%s%s%s", UI_DIM, msg, UI_RESET);
    read_key();
}

StatusCode create_post(BoardPost *result) {
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

    int target = adjust_value_with_arrows("모집 인원을 정하세요", 5, 1, ID_NUM);
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
    print_center("게시글이 저장되었습니다!", 24, 8);
    goto_ansi(START_X, START_Y + 10);
    printf("과목: %s\n", subject->name);
    goto_ansi(START_X, START_Y + 11);
    printf("모집 인원: %d명\n", target);
    goto_ansi(START_X, START_Y + 12);
    printf("홍보 문구: %s\n", result->promo_message);
    pause_message("계속하려면 아무 키나 누르세요...");

    return SUCCESS;
}

StatusCode apply_post(BoardPost *post, int id) {
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

static int find_student_index(const BoardPost *post, int id) {
    if (post == NULL || id < 0 || id >= ID_NUM) return -1;

    for (int i = 0; i < post->current_students; i++) {
        if (post->students_id[i] == id) return i;
    }

    return -1;
}

int is_user_applied(const BoardPost *post, int id) {
    return find_student_index(post, id) >= 0;
}

StatusCode cancel_post(BoardPost *post, int id) {
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