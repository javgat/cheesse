#ifndef BOARD_H
#define BOARD_H
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "piece.h"
#ifndef IGNORE_PREV_BOARDS
#include "arraylist.h"
#endif

struct previous_moves{
    bool moved_king;
    bool moved_rook_near;
    bool moved_rook_far;
    bool just_two_squared[8];
};

#ifndef IGNORE_PREV_BOARDS
struct previous_boards{
    arraylist* same_pieces;
    arraylist* ignore;
};
#endif

struct board{
    enum board_piece piece[64];
    bool is_white[64]; // if piece[i] == none, whatever is ok
    struct previous_moves white_moves;
    struct previous_moves black_moves;
    int last_move[2];
#ifndef IGNORE_PREV_BOARDS
    struct previous_boards prev_boards;
#endif
#ifdef DEBUG
    int last_debug_move[2];
#endif
};

void copy_board(struct board* src, struct board* dst);

void fill_default_board(struct board* b);

struct board* new_board_default();

struct board* new_board_copy(struct board* src);

void print_board(struct board* b, bool white_on_top);

bool board_are_equal(struct board* b, struct board* o);

struct intarray{
    int* arr;
    int len;
};

struct boardarray{
    struct board* arr;
    int len;
};

void destroy_board(struct board* b);

void destroy_boardarray(struct boardarray* b);

struct boardarray get_empty_boardarray();

// White is on top and first cell is 0
// Promotion should be added outside (queen and knight only imo)
struct intarray possible_pawn_moves(struct board* b, int cell);

struct intarray possible_knight_moves(struct board* b, int cell);

struct intarray possible_bishop_moves(struct board* b, int cell);

struct intarray possible_rook_moves(struct board* b, int cell);

struct intarray possible_queen_moves(struct board* b, int cell);

struct intarray possible_king_moves(struct board* b, int cell);

struct intarray possible_piece_moves(struct board* b, int cell);

void kill_piece(struct board* b, int cell);

void move_piece(struct board* b, int prev_position, int new_position);

void move_piece_save_history(struct board* old_b, struct board* b, int prev_position, int new_position);

void apply_promotion(struct board* b, bool is_white, enum board_piece piece_type);

#ifndef IGNORE_PREV_BOARDS
int count_repetitions(struct board* b);
#endif

struct boardarray get_potential_boards_moving_piece(struct board* b, int cell);

struct boardarray get_potential_boards_board(struct board* b, bool white);

struct eval_board{
    struct board* b;
    int evaluation;
    bool draw;
};

void destroy_eval_board(struct eval_board* eb);

void copy_eval_board(struct eval_board* src, struct eval_board* dst);

struct eval_board* new_eval_board_copy(struct eval_board* src);

struct eval_board_array{
    struct eval_board* evs;
    int len;
};

struct board_result{
    struct eval_board* eb;
    struct boardarray previous;
};

void destroy_eval_board_array(struct eval_board_array* eba);

void fill_eval_board(struct eval_board* eb, struct board* b, int evaluation, bool draw);

struct eval_board* new_eval_board_values(struct board* b, int evaluation, bool draw);

int evaluate(struct board* b);

bool is_king_dead(struct board* b, bool white);

struct eval_board_array get_evaluated_potential_boards(struct board* b, bool white);

struct board_result minmax_board(struct board* b, bool white, int depth, int orig_depth);

struct board_result minimax(struct board* b, bool white, int depth);
#endif