//
// Created by 김주환 on 25. 11. 9.
//

#ifndef ENROLLMENT_BOARD_H
#define ENROLLMENT_BOARD_H

#include "../struct.h"

static int generate_post_id(void);

static void clear_screen(void);

static int read_key(void);

static int adjust_value_with_arrows(const char *title, int initial, int min, int max);

static void pause_message(const char *msg);

StatusCodeEnum create_post(BoardPost *result);

StatusCodeEnum apply_post(BoardPost *post, int id);

static int find_student_index(const BoardPost *post, int id);

int is_user_applied(const BoardPost *post, int id);

StatusCodeEnum cancel_post(BoardPost *post, int id);

void enable_virtual_terminal_processing();

static void clear_board_post(BoardPost *post);

static void render_buttons(int selected_index, int post_count);

static void get_truncated_text(char *dest, const char *src, int max_len);

static void render_post_row(const BoardPost *post, int row, int is_selected);

static void render_board(const BoardPost *posts, int post_count, int selected_index, int scroll_offset);

static ApplyResultEnum show_apply_screen(BoardPost *post, int user_id);

static int add_post(BoardPost *posts, int count);

static void shift_left(BoardPost *posts, int *count, int start_index, int free_first);

static void move_to_planned(BoardPost *planned, int *planned_count, BoardPost *post);

static void show_planned_courses(const BoardPost *planned, int planned_count);

int board_main(int user_id);

#endif //ENROLLMENT_BOARD_H