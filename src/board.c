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
// Promotion should be added outside (queen and knight only imo)
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
        multiplier = -1;
    }
    int forward = 8 * multiplier;
    int initial_forward = 16 * multiplier;
    int eat_1 = 7 * multiplier;
    int eat_2 = 9 * multiplier;
    int possible_positions[] = {forward, initial_forward, eat_1, eat_2};
    int num_possible_positions = 4;
    int num_positions = num_possible_positions;
    if(b->squares.piece[cell + forward] == none){ // forward cell is free
        if(!(is_initial && b->squares.piece[cell + initial_forward] == none)){
            possible_positions[1] = 0; // forward 2 is not free
            num_positions--;
        }
    } else {
        possible_positions[0] = 0;
        possible_positions[1] = 0;
        num_positions -= 2;
    }
    if((is_white && cell%8 == 0) || (!is_white && cell%8 == 7) || !(b->squares.piece[cell + eat_1] != none && b->squares.is_white[cell + eat_1] != is_white)){
        possible_positions[2] = 0;
        num_positions--;
    }
    if((is_white && cell%8 == 7) || (!is_white && cell%8 == 0) || !(b->squares.piece[cell + eat_2] != none && b->squares.is_white[cell + eat_2] != is_white)){
        possible_positions[3] = 0;
        num_positions--;
    }
    int *positions = (int*) malloc(sizeof(int) * num_positions);
    int cont = 0;
    for(int i = 0; i < num_possible_positions; i++){
        if(possible_positions[i] != 0){
            positions[cont] = possible_positions[i];
            cont++;
        }
    }
    return positions;
}

int* possible_knight_moves(struct board* b, struct piece_id* id){
    int is_white = id->white;
    struct pieces_board* pb = &(b->black_pieces);
    if(is_white){
        pb = &(b->white_pieces);
    }
    int cell = get_piece_cell(pb, id);
    int possible_moves[] = {-17, -15, -10, -6, 6, 10, 15, 17};
    int num_possible_moves = 6;
    int num_positions = num_possible_moves;
    if(cell%8 <= 1){
        num_positions -= 2;
        possible_moves[2] = 0;
        possible_moves[4] = 0;
    }else{
        if(cell < 8){
            num_positions--;
            possible_moves[2] = 0;
        }
        if(cell > 55){
            num_positions--;
            possible_moves[4] = 0;
        }
    }
    if(cell%8 >= 6){
        num_positions -= 2;
        possible_moves[3] = 0;
        possible_moves[5] = 0;
    }else{
        if(cell < 8){
            num_positions--;
            possible_moves[3] = 0;
        }
        if(cell > 55){
            num_positions--;
            possible_moves[5] = 0;
        }
    }
    if(cell <= 15){
        num_positions -= 2;
        possible_moves[0] = 0;
        possible_moves[1] = 0;
    }else{
        if(cell%8 == 0){
            num_positions--;
            possible_moves[0] = 0;
        }
        if(cell%8 == 7){
            num_positions--;
            possible_moves[1] = 0;
        }
    }
    if(cell >= 48){
        num_positions -= 2;
        possible_moves[6] = 0;
        possible_moves[7] = 0;
    }else{
        if(cell%8 == 0){
            num_positions--;
            possible_moves[6] = 0;
        }
        if(cell%8 == 7){
            num_positions--;
            possible_moves[7] = 0;
        }
    }
    for(int i = 0; i < num_possible_moves; i++){
        if(possible_moves[i] != 0){
            int pos = cell + possible_moves[i];
            if(b->squares.piece[pos] != none && b->squares.is_white[pos] == is_white){
                possible_moves[i] = 0;
                num_positions--;
            }
        }
    }
    int *positions = (int*) malloc(sizeof(int) * num_positions);
    int cont = 0;
    for(int i = 0; i < num_possible_moves; i++){
        if(possible_moves[i] != 0){
            positions[cont] = possible_moves[i];
            cont++;
        }
    }
    return positions;
}

int* possible_bishop_moves(struct board* b, struct piece_id* id){
    int is_white = id->white;
    struct pieces_board* pb = &(b->black_pieces);
    if(is_white){
        pb = &(b->white_pieces);
    }
    int cell = get_piece_cell(pb, id);
    int num_possible_moves = 13;
    int possible_moves[13] = {0};
    int current_pos_move = 0;
    int pointing_to = cell;
    while(pointing_to > 7 && pointing_to%8 > 0){
        pointing_to = pointing_to - 9;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to > 7 && pointing_to%8 < 7){
        pointing_to = pointing_to - 7;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56 && pointing_to%8 > 0){
        pointing_to = pointing_to + 7;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56 && pointing_to%8 < 7){
        pointing_to = pointing_to + 9;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    int num_positions = current_pos_move;
    int *positions = (int*) malloc(num_positions * sizeof(int));
    int cont = 0;
    for(int i = 0; i < num_possible_moves; i++){
        if(possible_moves[i] != 0){
            positions[cont] = possible_moves[i];
            cont++;
        }
    }
    return positions;
}

int* possible_rook_moves(struct board* b, struct piece_id* id){
    int is_white = id->white;
    struct pieces_board* pb = &(b->black_pieces);
    if(is_white){
        pb = &(b->white_pieces);
    }
    int cell = get_piece_cell(pb, id);
    int num_possible_moves = 14;
    int possible_moves[14] = {0};
    int current_pos_move = 0;
    int pointing_to = cell;
    while(pointing_to > 7){
        pointing_to = pointing_to - 8;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to%8 > 0){
        pointing_to = pointing_to - 1;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to%8 < 7){
        pointing_to = pointing_to + 1;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56){
        pointing_to = pointing_to + 8;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    int num_positions = current_pos_move;
    int *positions = (int*) malloc(num_positions * sizeof(int));
    int cont = 0;
    for(int i = 0; i < num_possible_moves; i++){
        if(possible_moves[i] != 0){
            positions[cont] = possible_moves[i];
            cont++;
        }
    }
    return positions;
}

int* possible_queen_moves(struct board* b, struct piece_id* id){
    int is_white = id->white;
    struct pieces_board* pb = &(b->black_pieces);
    if(is_white){
        pb = &(b->white_pieces);
    }
    int cell = get_piece_cell(pb, id);
    int num_possible_moves = 27;
    int possible_moves[27] = {0};
    int current_pos_move = 0;
    int pointing_to = cell;
    while(pointing_to > 7){
        pointing_to = pointing_to - 8;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to%8 > 0){
        pointing_to = pointing_to - 1;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to%8 < 7){
        pointing_to = pointing_to + 1;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56){
        pointing_to = pointing_to + 8;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    // Bishop like moves
    while(pointing_to > 7 && pointing_to%8 > 0){
        pointing_to = pointing_to - 9;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to > 7 && pointing_to%8 < 7){
        pointing_to = pointing_to - 7;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56 && pointing_to%8 > 0){
        pointing_to = pointing_to + 7;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56 && pointing_to%8 < 7){
        pointing_to = pointing_to + 9;
        if(b->squares.piece[pointing_to] != none && b->squares.is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to;
        current_pos_move++;
        if(b->squares.piece[pointing_to] != none){
            break;
        }
    }
    int num_positions = current_pos_move;
    int *positions = (int*) malloc(num_positions * sizeof(int));
    int cont = 0;
    for(int i = 0; i < num_possible_moves; i++){
        if(possible_moves[i] != 0){
            positions[cont] = possible_moves[i];
            cont++;
        }
    }
    return positions;
}

// if castling the move should move the rook
int* possible_king_moves(struct board* b, struct piece_id* id){
    int is_white = id->white;
    struct pieces_board* pb = &(b->black_pieces);
    if(is_white){
        pb = &(b->white_pieces);
    }
    int cell = get_piece_cell(pb, id);
    int num_possible_moves = 9;
    int possible_moves[] = {-9, -8, -7, -1, 1, 7, 8, 9, -2};
    if(cell%8 == 0){
        possible_moves[0] = 0;
        possible_moves[3] = 0;
        possible_moves[5] = 0;
    }else if(cell%8 == 7){
        possible_moves[2] = 0;
        possible_moves[4] = 0;
        possible_moves[7] = 0;
    }
    if(cell <= 7){
        possible_moves[0] = 0;
        possible_moves[1] = 0;
        possible_moves[2] = 0;
    }else if(cell >= 56){
        possible_moves[5] = 0;
        possible_moves[6] = 0;
        possible_moves[7] = 0;
    }
    if(!can_enroque){
        possible_moves[8] = 0;
    }
    int num_positions = 0;
    for(int i = 0; i < num_possible_moves; i++){
        if(possible_moves != 0){
            num_positions++;
        }
    }
    for(int i = 0; i < num_possible_moves; i++){
        if(possible_moves[i] != 0){
            int pos = cell + possible_moves[i];
            if(b->squares.piece[pos] != none && b->squares.is_white[pos] == is_white){
                possible_moves[i] = 0;
                num_positions--;
            }
        }
    }
    int *positions = (int*) malloc(sizeof(int) * num_positions);
    int cont = 0;
    for(int i = 0; i < num_possible_moves; i++){
        if(possible_moves[i] != 0){
            positions[cont] = possible_moves[i];
            cont++;
        }
    }
    return positions;
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