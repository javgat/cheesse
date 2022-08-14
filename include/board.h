#include <stdlib.h>
#include <string.h>
#ifndef BOARD_H
#define BOARD_H
#endif

enum board_piece{
    none = 0,
    pawn = 1,
    knight = 2,
    bishop = 3,
    rook = 4,
    queen = 5,
    king = 6,
};

struct piece_id{
    enum board_piece type;
    int white; // 1 white, 0 black
    int index;
};

// PIECES_BOARD

#define PIECES_BOARD_DEAD 64
struct pieces_board{
    // 0-63 in board, 64 dead
    // all alive pieces must be at the begining of the array
    int pawn[8];
    int knight[10]; // 2 + 8 possible promoted pawns
    int bishop[10];
    int rook[10];
    int queen[9];
    int king;
};

int get_piece_cell(struct pieces_board* pb, struct piece_id* id);

// SQUARES_BOARD
struct squares_board{
    enum board_piece piece[64];
    int is_white[64]; // if piece[i] == none, whatever is ok
};

struct previous_moves{
    int moved_king;
    int moved_rook_near;
    int moved_rook_far;
    int just_two_squared[8];
};

struct board{
    struct pieces_board white_pieces;
    struct pieces_board black_pieces;
    struct squares_board squares;
    struct previous_moves white_moves;
    struct previous_moves black_moves;
};

struct intarray{
    int* arr;
    int len;
};

struct boardarray{
    struct board* arr;
    int len;
};

struct board copy_board(struct board* b);

// White is on top and first cell is 0
// Promotion should be added outside (queen and knight only imo)
struct intarray possible_pawn_moves(struct board* b, struct piece_id* id);

struct intarray possible_knight_moves(struct board* b, struct piece_id* id);

struct intarray possible_bishop_moves(struct board* b, struct piece_id* id);

struct intarray possible_rook_moves(struct board* b, struct piece_id* id);

struct intarray possible_queen_moves(struct board* b, struct piece_id* id);

struct intarray possible_king_moves(struct board* b, struct piece_id* id);

struct intarray possible_piece_moves(struct board* b, struct piece_id* id);

void kill_piece(struct board* b, int new_position);

void move_piece(struct board* b, struct piece_id* id, int prev_position, int new_position);

void apply_promotion(struct board* b, int is_white, enum board_piece piece_type);

struct boardarray get_potential_boards_moving_piece(struct board* b, struct piece_id* piece_id);

struct boardarray get_potential_boards_moving_type(struct board* b, int white, enum board_piece type);

struct boardarray get_potential_boards_board(struct board* b, int white);

struct eval_board{
    struct board* b;
    int evaluation;
};

struct eval_board_array{
    struct eval_board* evs;
    int len;
};

int evaluate(struct board* b);

struct eval_board_array get_evaluated_potential_boards(struct board* b, int white);

struct eval_board_array min_board(struct board* b, int white, int depth);

struct eval_board_array max_board(struct board* b, int white, int depth);
