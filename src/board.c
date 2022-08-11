#include <stdlib.h>

enum board_piece{
    none = 0,
    pawn = 1,
    knight = 2,
    bishop = 3,
    rook = 4,
    queen = 5,
    king = 6,
};

enum piece{
    pawn = 1,
    knight = 2,
    bishop = 3,
    rook = 4,
    queen = 5,
    king = 6,
};

struct piece_id{
    enum piece type;
    int white; // 1 white, 0 black
    int index;
};

// PIECES_BOARD

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

int get_piece_cell(struct pieces_board* pb, struct piece_id* id){
    /**
     * @brief Get the position [0, 63] of piece <id> on the board <pb>
     * 
     */
    switch(id->type){
        case pawn:
            return pb->pawn[id->index];
        case knight:
            return pb->knight[id->index];
        case bishop:
            return pb->bishop[id->index];
        case rook:
            return pb->rook[id->index];
        case queen:
            return pb->queen[id->index];
        case king:
            return pb->king;
        default:
            return -1;
    }
}

// SQUARES_BOARD
struct squares_board{
    enum board_piece piece[64];
    int is_white[64]; // if piece[i] == none, whatever is ok
};

struct board{
    struct pieces_board white_pieces;
    struct pieces_board black_pieces;
    struct squares_board squares;
};

// White is on top and first cell is 0
int* possible_pawn_moves(struct board* b, struct piece_id* id){
    int is_white = id->white;
    struct pieces_board* pb = &(b->black_pieces);
    if(is_white){
        pb = &(b->white_pieces);
    }
    int cell = get_piece_cell(pb, id);
    int is_initial = 0;
    if(is_white && cell >= 8 && cell <= 15){
        is_initial = 1;
    }else if(!is_white && cell >= 48 && cell <= 55){
        is_initial = 1;
    }
    int multiplier = 1;
    if(!is_white){
        multiplier = -1
    }
    int forward = 8 * multiplier;
    int initial_forward = 16 * multiplier;
    int eat_1 = 7 * multiplier;
    int eat_2 = 9 * multiplier;
    int possible_positions[] = {forward, initial_forward, eat_1, eat_2};
    if(b->squares.piece[cell + forward] == none){
        if(!(is_initial && b->squares.piece[cell + initial_forward] == none)){
            possible_positions[1] = 0;
        }
    } else {
        possible_positions[0] = 0;
        possible_positions[1] = 0;
    }

}

struct board* get_potential_boards_moving_piece(struct board* b, struct piece_id* piece_id){
    int is_white = piece_id->white;
    struct pieces_board* pb = &(b->black_pieces);
    if(is_white){
        pb = &(b->white_pieces);
    }
    struct board* boards;
    switch(piece_id->type){
        case pawn:
            break;
        case knight:
            break;
        case bishop:
            break;
        case rook:
            break;
        case queen:
            break;
        case king:
            break;
        default:
            boards = (struct board*) malloc(0);
    }
    return boards;
}