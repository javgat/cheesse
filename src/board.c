#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "tables.h"
#include "piece.h"
#include "board.h"

void copy_board(struct board* src, struct board* dst){
    *dst = *src;
}

void fill_default_board(struct board* b){
    // prev moves
    b->black_moves.moved_king = false;
    b->black_moves.moved_rook_far = false;
    b->black_moves.moved_rook_near = false;
    memset(b->black_moves.just_two_squared, 0, sizeof(b->black_moves.just_two_squared));
    b->white_moves.moved_king = false;
    b->white_moves.moved_rook_far = false;
    b->white_moves.moved_rook_near = false;
    memset(b->white_moves.just_two_squared, 0, sizeof(b->white_moves.just_two_squared));
    // squares
    memset(b->is_white, 0, sizeof(b->is_white));
    for(int i = 0; i < 16; i++){
        b->is_white[i] = 1;
    }
    for(int i = 16; i < 48; i++){
        b->piece[i] = none;
    }
    for(int i = 8; i < 16; i++){
        b->piece[i] = pawn;
    }
    for(int i = 48; i < 56; i++){
        b->piece[i] = pawn;
    }
    int rooks[] = {0, 7, 56, 63};
    int knights[] = {1, 6, 57, 62};
    int bishops[] = {2, 5, 58, 61};
    for(int i = 0; i < 4; i++){
        b->piece[rooks[i]] = rook;
        b->piece[knights[i]] = knight;
        b->piece[bishops[i]] = bishop;
    }
    b->piece[4] = queen;
    b->piece[60] = queen;
    b->piece[3] = king;
    b->piece[59] = king;
    b->last_move[0] = -1;
    b->last_move[1] = -1;
    return;
}

struct board* new_board_default(){
    struct board* b = (struct board*) malloc(sizeof(struct board));
    fill_default_board(b);
    return b;
}

struct board* new_board_copy(struct board* src){
    struct board* b = (struct board*) malloc(sizeof(struct board));
    copy_board(src, b);
    return b;
}

void print_board(struct board* b, bool white_on_top){
    for(int i = 0; i<64; i++){
        int index = i;
        if(white_on_top){
            index = 63 - i;
        }
        char trans = piece_to_char(b->piece[index], b->is_white[index]);
        printf("%c", trans);
        if(i%8 == 7){
            printf("\n");
        }
    }
}

struct boardarray get_empty_boardarray(){
    struct boardarray bar;
    bar.arr = (struct board*) malloc(0);
    bar.len = 0;
    return bar;
}

// White is on top and first cell is 0
// Promotion should be added outside (queen and knight only imo)
struct intarray possible_pawn_moves(struct board* b, int cell){
    int is_white = b->is_white[cell];
    struct previous_moves* enemy_pm = &(b->white_moves);
    if(is_white){
        enemy_pm = &(b->black_moves);
    }
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
    if(b->piece[cell + forward] == none){ // forward cell is free
        if(!(is_initial && b->piece[cell + initial_forward] == none)){
            possible_positions[1] = 0; // forward 2 is not free
            num_positions--;
        }
    } else {
        possible_positions[0] = 0;
        possible_positions[1] = 0;
        num_positions -= 2;
    }
    if((is_white && cell%8 == 0) || (!is_white && cell%8 == 7) || !(b->piece[cell + eat_1] != none && b->is_white[cell + eat_1] != is_white)){
        int on_passant_pawn = cell + 1;
        if(is_white){
            on_passant_pawn = cell - 1;
        }
        if((is_white && cell%8 == 0) || (!is_white && cell%8 == 7) || !(b->piece[on_passant_pawn] == pawn && b->is_white[on_passant_pawn] != is_white && enemy_pm->just_two_squared[on_passant_pawn%8])){
            possible_positions[2] = 0;
            num_positions--;
        }
    }
    if((is_white && cell%8 == 7) || (!is_white && cell%8 == 0) || !(b->piece[cell + eat_2] != none && b->is_white[cell + eat_2] != is_white)){
        int on_passant_pawn = cell - 1;
        if(is_white){
            on_passant_pawn = cell + 1;
        }
        if((is_white && cell%8 == 7) || (!is_white && cell%8 == 0) || !(b->piece[on_passant_pawn] == pawn && b->is_white[on_passant_pawn] != is_white && enemy_pm->just_two_squared[on_passant_pawn%8])){
            possible_positions[3] = 0;
            num_positions--;
        }
    }
    int *positions = (int*) malloc(sizeof(int) * num_positions);
    int cont = 0;
    for(int i = 0; i < num_possible_positions; i++){
        if(possible_positions[i] != 0){
            positions[cont] = possible_positions[i];
            cont++;
        }
    }
    struct intarray returnable = (struct intarray){positions, num_positions};
    return returnable;
}

struct intarray possible_knight_moves(struct board* b, int cell){
    int is_white = b->is_white[cell];
    int possible_moves[] = {-17, -15, -10, -6, 6, 10, 15, 17};
    int num_possible_moves = 8;
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
            if(b->piece[pos] != none && b->is_white[pos] == is_white){
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
    struct intarray returnable = (struct intarray){positions, num_positions};
    return returnable;
}

struct intarray possible_bishop_moves(struct board* b, int cell){
    int is_white = b->is_white[cell];
    int num_possible_moves = 13;
    int possible_moves[13] = {0};
    int current_pos_move = 0;
    int pointing_to = cell;
    while(pointing_to > 7 && pointing_to%8 > 0){
        pointing_to = pointing_to - 9;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to > 7 && pointing_to%8 < 7){
        pointing_to = pointing_to - 7;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56 && pointing_to%8 > 0){
        pointing_to = pointing_to + 7;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56 && pointing_to%8 < 7){
        pointing_to = pointing_to + 9;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
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
    struct intarray returnable = (struct intarray){positions, num_positions};
    return returnable;
}

struct intarray possible_rook_moves(struct board* b, int cell){
    int is_white = b->is_white[cell];
    int num_possible_moves = 14;
    int possible_moves[14] = {0};
    int current_pos_move = 0;
    int pointing_to = cell;
    while(pointing_to > 7){
        pointing_to = pointing_to - 8;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to%8 > 0){
        pointing_to = pointing_to - 1;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to%8 < 7){
        pointing_to = pointing_to + 1;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56){
        pointing_to = pointing_to + 8;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
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
    struct intarray returnable = (struct intarray){positions, num_positions};
    return returnable;
}

struct intarray possible_queen_moves(struct board* b, int cell){
    int is_white = b->is_white[cell];
    int num_possible_moves = 27;
    int possible_moves[27] = {0};
    int current_pos_move = 0;
    int pointing_to = cell;
    while(pointing_to > 7){
        pointing_to = pointing_to - 8;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to%8 > 0){
        pointing_to = pointing_to - 1;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to%8 < 7){
        pointing_to = pointing_to + 1;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56){
        pointing_to = pointing_to + 8;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    // Bishop like moves
    pointing_to = cell;
    while(pointing_to > 7 && pointing_to%8 > 0){
        pointing_to = pointing_to - 9;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to > 7 && pointing_to%8 < 7){
        pointing_to = pointing_to - 7;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56 && pointing_to%8 > 0){
        pointing_to = pointing_to + 7;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
            break;
        }
    }
    pointing_to = cell;
    while(pointing_to < 56 && pointing_to%8 < 7){
        pointing_to = pointing_to + 9;
        if(b->piece[pointing_to] != none && b->is_white[pointing_to] == is_white){
            break;
        }
        possible_moves[current_pos_move] = pointing_to - cell;
        current_pos_move++;
        if(b->piece[pointing_to] != none){
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
    struct intarray returnable = (struct intarray){positions, num_positions};
    return returnable;
}

// if castling the move should move the rook
struct intarray possible_king_moves(struct board* b, int cell){
    int is_white = b->is_white[cell];
    struct previous_moves* pm = &(b->black_moves);
    if(is_white){
        pm = &(b->white_moves);
    }
    int num_possible_moves = 10;
    int possible_moves[] = {-9, -8, -7, -1, 1, 7, 8, 9, -2, 2}; // -2 castling close
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
    if(!pm->moved_king){
        int can_castle_near = 0;
        if(!pm->moved_rook_near){
            if(is_white){
                if(cell==3 && b->piece[0]==rook && b->is_white[0]==1 && b->piece[1]==none && b->piece[2]==none){
                    can_castle_near = 1;
                }
            }else if(!is_white && cell==59 && b->piece[56]==rook && b->is_white[56]==1 && b->piece[57]==none && b->piece[58]==none){
                can_castle_near = 1;
            }
        }
        if(!can_castle_near){
            possible_moves[8] = 0;
        }
        int can_castle_far = 0;
        if(!pm->moved_rook_far){
            if(is_white){
                if(cell==3 && b->piece[7]==rook && b->is_white[7]==1 && b->piece[6]==none && b->piece[5]==none && b->piece[4]==none){
                    can_castle_far = 1;
                }
            }else if(!is_white && cell==59 && b->piece[63]==rook && b->is_white[63]==1 && b->piece[62]==none && b->piece[61]==none && b->piece[60]==none){
                can_castle_far = 1;
            }
        }
        if(!can_castle_far){
            possible_moves[9] = 0;
        }
    }else{
        possible_moves[8] = 0;
        possible_moves[9] = 0;
    }
    int num_positions = 0;
    for(int i = 0; i < num_possible_moves; i++){
        if(possible_moves[i] != 0){
            num_positions++;
            int pos = cell + possible_moves[i];
            if(b->piece[pos] != none && b->is_white[pos] == is_white){
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
    struct intarray returnable = (struct intarray){positions, num_positions};
    return returnable;
}

struct intarray possible_piece_moves(struct board* b, int cell){
    switch(b->piece[cell]){
        case pawn:
            return possible_pawn_moves(b, cell);
        case knight:
            return possible_knight_moves(b, cell);
        case bishop:
            return possible_bishop_moves(b, cell);
        case rook:
            return possible_rook_moves(b, cell);
        case queen:
            return possible_queen_moves(b, cell);
        case king:
            return possible_king_moves(b, cell);
        default: {
#ifdef EMPTY_MALLOC
            printf("EMPTY MALLOC");fflush(stdout);
#endif
            int* empty = (int*) malloc(0);
            struct intarray returnable = (struct intarray){empty, 0};
            return returnable;
        }
    }
}

void kill_piece(struct board* b, int cell){
    int is_white = b->is_white[cell];
    struct previous_moves* pm = &(b->black_moves);
    if(is_white){
        pm = &(b->white_moves);
    }
    switch(b->piece[cell]){
        case rook:
            if(is_white){
                if(cell == 0){
                    pm->moved_rook_near = true;
                }else if(cell == 7){
                    pm->moved_rook_far = true;
                }
            }else{
                if(cell == 56){
                    pm->moved_rook_near = true;
                }else if(cell == 63){
                    pm->moved_rook_far = true;
                }
            }
            break;
        case king:
            pm->moved_king = true;
            break;
        default:
            break;
    }
    b->piece[cell] = none;
}

void move_piece(struct board* b, int prev_position, int new_position){
    // assert: the move is valid
#ifdef DEBUG
    if(new_position > 63){
        fprintf(stderr, "Invalid new position of %d type from %d to %d\n", b->piece[prev_position], prev_position, new_position);fflush(stderr);
    }
    b->last_debug_move[0] = prev_position;
    b->last_debug_move[1] = new_position;
#endif
    b->last_move[0] = prev_position;
    b->last_move[1] = new_position;
    int is_white = b->is_white[prev_position];
    struct previous_moves* pm = &(b->black_moves);
    struct previous_moves* enemy_pm = &(b->white_moves);
    int on_passant_pawn = new_position + 8;
    if(is_white){
        pm = &(b->white_moves);
        enemy_pm = &(b->black_moves);
        on_passant_pawn = new_position - 8;
    }
    bool safe_check_passant = true;
    if(on_passant_pawn < 0 || on_passant_pawn > 63){
        safe_check_passant = false;
    }
    if(b->piece[new_position]!=none){
        kill_piece(b, new_position);
    }else if(safe_check_passant && b->piece[on_passant_pawn] == pawn &&\
            b->is_white[on_passant_pawn] != is_white && enemy_pm->just_two_squared[on_passant_pawn%8]){ // on passant
        kill_piece(b, on_passant_pawn);
    }
    b->piece[new_position] = b->piece[prev_position];
    b->piece[prev_position] = none;
    b->is_white[new_position] = is_white;
    int move = new_position - prev_position;
    if(b->piece[new_position] == king){ // check castling and if it is, move rook
        pm->moved_king = true;
        if(move == -2 || move == 2){// castling
            int rook_prev = new_position - 1; // initialized to castling near values
            int rook_new = new_position + 1;
            if(move == -2){ // castling near
                pm->moved_rook_near = 1;
            }else{ // castling far
                pm->moved_rook_far = 1;
                rook_prev = new_position + 2;
                rook_new = new_position - 1;
            }
            move_piece(b, rook_prev, rook_new);
            b->last_move[0] = prev_position; // last move should still be the king
            b->last_move[1] = new_position;
        }
    }
    memset(pm->just_two_squared, 0, sizeof(pm->just_two_squared));
    if(b->piece[new_position] == pawn && (move == 16 || move == -16)){ // just moved two (could be on passanted)
        int pawn_index = prev_position%8;
        pm->just_two_squared[pawn_index] = 1;
    }
}

void apply_promotion(struct board* b, bool is_white, enum board_piece piece_type){
    int base = 0;
    if(is_white){
        base = 56;
    }
    for(int i = base; i < base+8; i++){
        if(b->piece[i] == pawn && b->is_white[i] == is_white){
            b->piece[i] = piece_type;
            b->is_white[i] = is_white;
            break;
        }
    }
}

struct boardarray get_potential_boards_moving_piece(struct board* b, int cell){
    int is_white = b->is_white[cell];
    enum board_piece ptype = b->piece[cell];
    struct intarray movesarray = possible_piece_moves(b, cell);
    int* moves = movesarray.arr;
    int cant_moves = movesarray.len;
    int cant_boards = cant_moves;
    if(ptype == pawn){
        for(int i = 0; i < cant_moves; i++){
            int new_pos = moves[i] + cell;
            if(new_pos <= 7 || new_pos >= 56){
                cant_boards++;
            }
        }
    }
    struct board* boards = (struct board*) malloc(cant_boards* sizeof(struct board));
    int board_pos = 0;
    for(int i = 0; i < cant_boards; i++){
        copy_board(b, &boards[i]);
    }
    for(int i = 0; i < cant_moves; i++){
        struct board* nb = &(boards[board_pos]);
        board_pos++;
        struct previous_moves* pm = &(nb->black_moves);
        if(is_white){
            pm = &(nb->white_moves);
        }
        if(ptype == king){
            pm->moved_king = true;
        }else if(ptype == rook){
            if(cell == 0 || cell == 56){
                pm->moved_rook_near = true;
            }else if(cell == 7 || cell == 63){
                pm->moved_rook_far = true;
            }
        }
        int new_position = cell + moves[i];
        move_piece(nb, cell, new_position);
        if(ptype == pawn && (new_position <= 7 || new_position >= 56)){ // promotion
            copy_board(nb, &boards[board_pos]);
            struct board* nb2 = &(boards[board_pos]);
            board_pos++;
            apply_promotion(nb, is_white, queen);
            apply_promotion(nb2, is_white, knight);
        }
    }
    struct boardarray returnable;
    returnable.arr = boards;
    returnable.len = cant_boards;
    free(movesarray.arr);
    return returnable;
}

struct boardarray get_potential_boards_board(struct board* b, bool white){
    struct boardarray bas[64];
    int cant_boards = 0;
    for(int i = 0; i < 64; i++){
        if(b->piece[i] != none && b->is_white[i] == white){
            bas[i] = get_potential_boards_moving_piece(b, i);   
        }else{
            bas[i] = get_empty_boardarray();
        }
        cant_boards += bas[i].len;
    }
    struct board* boards = (struct board*) malloc(sizeof(struct board) * cant_boards);
    int b_index = 0;
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < bas[i].len; j++){
            copy_board(&bas[i].arr[j], &boards[b_index]);
            b_index++;
        }
    }
    struct boardarray returnable = (struct boardarray){boards, cant_boards};
    for(int i = 0; i < 64; i++){
        free(bas[i].arr);
    }
    return returnable;
}

void copy_eval_board(struct eval_board* src, struct eval_board* dst){
    copy_board(src->b, dst->b);
    dst->evaluation = src->evaluation;
    dst->draw = src->draw;
}

struct eval_board* new_eval_board_copy(struct eval_board* src){
    struct eval_board* dst = (struct eval_board*) malloc(sizeof(struct eval_board));
    dst->b = (struct board*) malloc(sizeof(struct board));
    copy_eval_board(src, dst);
    return dst;
}

void fill_eval_board(struct eval_board* eb, struct board* b, int evaluation, bool draw){
    eb->b = (struct board*) malloc(sizeof(struct board));
    copy_board(b, eb->b);
    eb->draw = draw;
    eb->evaluation = evaluation;
}

struct eval_board* new_eval_board_values(struct board* b, int evaluation, bool draw){
    struct eval_board* eb = (struct eval_board*) malloc(sizeof(struct eval_board));
    fill_eval_board(eb, b, evaluation, draw);
    return eb;
}

int evaluate(struct board* b){
    int points = 0;
    int piece_points[] = PIECE_VALS;
    int psqn[] = KNIGHT_SQ;
    int psqb[] = BISHOP_SQ;
    int psqr[] = ROOK_SQ;
    int psqq[] = QUEEN_SQ;
    int psqk[] = KING_SQ;
    int psqp[] = PAWN_SQ;
    bool is_king_white = false;
    bool is_king_black = false;
    for(int i = 0; i < 64; i++){
        enum board_piece ptype = b->piece[i];
        if(ptype == none){
            continue;
        }
        int iptype = (int)ptype;
        int subpoints = piece_points[iptype-1];
        int *piece_sq;
        switch(iptype){
            case 1:
                piece_sq = psqp;
                break;
            case 2:
                piece_sq = psqn;
                break;
            case 3:
                piece_sq = psqb;
                break;
            case 4:
                piece_sq = psqr;
                break;
            case 5:
                piece_sq = psqq;
                break;
            case 6:
                piece_sq = psqk;
                if(b->is_white[i]){
                    is_king_white = true;
                }else{
                    is_king_black = true;
                }
                break;
            default:
                fprintf(stderr, "Evaluation type piece incorrect.\n");
                fflush(stdout);
                piece_sq = psqp;
        }
        if(b->is_white[i]){
            subpoints = subpoints + piece_sq[63-i];
        }else{
            subpoints = -subpoints - piece_sq[i];
        }
        points += subpoints;
    }
    if(!is_king_black && !is_king_white){
        points = 0; // shouldnt happen
    }else if(!is_king_black){
        points = KING_DEAD;
    }else if(!is_king_white){
        points = -KING_DEAD;
    }
    return points; // 1 win white, -1 win black
}

bool is_king_dead(struct board* b, bool white){
    for(int i = 0; i < 64; i++){
        if(b->piece[i] == king && b->is_white[i] == white){
            return false;
        }
    }
    return true;
}

struct eval_board* get_dead_king_eval_board(struct board* b, bool white){
    struct eval_board* eb = (struct eval_board*) malloc(sizeof(struct eval_board));
    int eval = KING_DEAD;
    if(white){
        eval = -KING_DEAD;
    }
    fill_eval_board(eb, b, eval, false);
    return eb;
}

struct eval_board_array get_evaluated_potential_boards(struct board* b, bool white){
    if(is_king_dead(b, white)){
        struct eval_board* eb = get_dead_king_eval_board(b, white);
        struct eval_board_array reteba = (struct eval_board_array) {eb, 1};
        return reteba;
    }
    struct boardarray ba = get_potential_boards_board(b, white);
    struct eval_board* evs = (struct eval_board*) malloc(ba.len * sizeof(struct eval_board));
    for(int i = 0; i<ba.len; i++){
        int eval = evaluate(&ba.arr[i]);
        fill_eval_board(&evs[i], &(ba.arr[i]), eval, false);
    }
    struct eval_board_array ret = (struct eval_board_array) {evs, ba.len};
    free(ba.arr);
    if(ret.len == 0){
        struct eval_board* draw_evs = new_eval_board_values(b, 0, true);
        ret = (struct eval_board_array) {draw_evs, 1};
    }
    return ret;
}

struct board_result minmax_board(struct board* b, bool white, int depth, int orig_depth){
    if(is_king_dead(b, white)){
        struct eval_board* eb = get_dead_king_eval_board(b, white);
        struct board *prev = (struct board*) malloc(sizeof(struct board) * orig_depth);
        for(int i = 0; i<depth; i++){
            copy_board(b, &prev[i]);
        }
        struct board_result ret = (struct board_result){eb, prev};
        return ret;
    }
    if(depth == 0){
        struct eval_board_array eba = get_evaluated_potential_boards(b, white);
        if(eba.len == 0){
            struct board_result empty = (struct board_result){NULL, NULL};
            return empty;
        }
        struct eval_board *eb = &(eba.evs[0]);
        for(int i = 0; i < eba.len; i++){
            int eval_should_bigger = eb->evaluation;
            int eval_smaller = eba.evs[i].evaluation;
            if(!white){
                int temp = eval_smaller;
                eval_smaller = eval_should_bigger;
                eval_should_bigger = temp;
            }
            if(eval_smaller > eval_should_bigger){
                eb = &(eba.evs[i]);
            }
        }
        struct eval_board *neb = new_eval_board_copy(eb);
        struct board *prev = (struct board*) malloc(sizeof(struct board) * orig_depth);
        struct board_result ret = {neb, prev};
        for(int i = 0; i < eba.len; i++){
            free(eba.evs[i].b);
        }
        free(eba.evs);
        return ret;
    }
    struct boardarray ba = get_potential_boards_board(b, white);
    struct board_result max_br = (struct board_result) {NULL, NULL};
    int inited = 0;
    //printf("Evaluations: ");fflush(stdout);
    for(int i = 0; i < ba.len; i++){
        struct board_result br = minmax_board(&(ba.arr[i]), !white, depth-1, orig_depth);
        //printf("%d ", br.eb->evaluation);fflush(stdout);
        if(br.eb != NULL){
            int multiplier = -1;
            if(!white){
                multiplier = 1;
            }
            if(br.eb->evaluation == KING_DEAD * multiplier){ //as later the death, the better
                int death_late_priority = -1*(orig_depth-depth);
                br.eb->evaluation += multiplier*death_late_priority;
            }
            int eval_should_bigger, eval_smaller;
            if(inited){
                eval_should_bigger = max_br.eb->evaluation;
                eval_smaller = br.eb->evaluation;
                if(!white){
                    int temp = eval_smaller;
                    eval_smaller = eval_should_bigger;
                    eval_should_bigger = temp;
                }
            }
            if(!inited || eval_should_bigger < eval_smaller){
                if(inited){
                    free(max_br.previous);
                    free(max_br.eb->b);
                    free(max_br.eb);
                }
                inited = 1;
                max_br = br;
                copy_board(&ba.arr[i], &max_br.previous[depth-1]);
            }else{
                free(br.previous);
                free(br.eb->b);
                free(br.eb);
            }
        }
    }
    if(ba.len == 0){
        struct eval_board* draw_evs = (struct eval_board*) malloc(sizeof(struct eval_board));
        draw_evs->evaluation = 0;
        copy_board(b, draw_evs->b);
        draw_evs->draw = 1;
        struct board *prev = (struct board*) malloc(sizeof(struct board) * orig_depth);
        max_br.eb = draw_evs;
        max_br.previous = prev;
        for(int i = 0; i<depth; i++){
            copy_board(b, &max_br.previous[i]);
        }
    }
    //printf("Chosen %d\n", max_br.eb->evaluation);fflush(stdout);
    free(ba.arr);
    return max_br;
}

struct board_result minimax(struct board* b, bool white, int depth){
    return minmax_board(b, white, depth, depth);
}