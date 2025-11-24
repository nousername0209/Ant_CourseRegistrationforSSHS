CC=gcc
CFLAGS=-I src -Wall -Wextra -std=c11

board_test: src/enrollment_board/board.c src/enrollment_board/board_test_main.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f board_test

.PHONY: board_test clean