#include <stdlib.h>
#include <string.h>

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

struct board copy_board(struct board* b){
    struct board nb = *b;
    return nb;
}

struct intarray{
    int* arr;
    int len;
};

// White is on top and first cell is 0
// Promotion should be added outside (queen and knight only imo)
struct intarray possible_pawn_moves(struct board* b, struct piece_id* id){
    int is_white = id->white;
    struct pieces_board* pb = &(b->black_pieces);
    struct previous_moves* enemy_pm = &(b->white_moves);
    if(is_white){
        pb = &(b->white_pieces);
        enemy_pm = &(b->black_moves);
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
        int on_passant_pawn = cell + 1;
        if(is_white){
            on_passant_pawn = cell - 1;
        }
        if((is_white && cell%8 == 0) || (!is_white && cell%8 == 7) || !(b->squares.piece[on_passant_pawn] == pawn && b->squares.is_white[on_passant_pawn] != is_white && enemy_pm->just_two_squared[on_passant_pawn%8])){
            possible_positions[2] = 0;
            num_positions--;
        }
    }
    if((is_white && cell%8 == 7) || (!is_white && cell%8 == 0) || !(b->squares.piece[cell + eat_2] != none && b->squares.is_white[cell + eat_2] != is_white)){
        int on_passant_pawn = cell - 1;
        if(is_white){
            on_passant_pawn = cell + 1;
        }
        if((is_white && cell%8 == 7) || (!is_white && cell%8 == 0) || !(b->squares.piece[on_passant_pawn] == pawn && b->squares.is_white[on_passant_pawn] != is_white && enemy_pm->just_two_squared[on_passant_pawn%8])){
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
    struct intarray returnable = {positions, num_positions};
    return returnable;
}

struct intarray possible_knight_moves(struct board* b, struct piece_id* id){
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
    struct intarray returnable = {positions, num_positions};
    return returnable;
}

struct intarray possible_bishop_moves(struct board* b, struct piece_id* id){
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
    struct intarray returnable = {positions, num_positions};
    return returnable;
}

struct intarray possible_rook_moves(struct board* b, struct piece_id* id){
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
    struct intarray returnable = {positions, num_positions};
    return returnable;
}

struct intarray possible_queen_moves(struct board* b, struct piece_id* id){
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
    struct intarray returnable = {positions, num_positions};
    return returnable;
}

// if castling the move should move the rook
struct intarray possible_king_moves(struct board* b, struct piece_id* id){
    int is_white = id->white;
    struct pieces_board* pb = &(b->black_pieces);
    struct previous_moves* pm = &(b->black_moves);
    if(is_white){
        pb = &(b->white_pieces);
        pm = &(b->white_moves);
    }
    int cell = get_piece_cell(pb, id);
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
                if(cell==3 && b->squares.piece[0]==rook && b->squares.is_white[0]==1 && b->squares.piece[1]==none && b->squares.piece[2]==none){
                    can_castle_near = 1;
                }
            }else if(!is_white && cell==59 && b->squares.piece[56]==rook && b->squares.is_white[56]==1 && b->squares.piece[57]==none && b->squares.piece[58]==none){
                can_castle_near = 1;
            }
        }
        if(!can_castle_near){
            possible_moves[8] = 0;
        }
        int can_castle_far = 0;
        if(!pm->moved_rook_far){
            if(is_white){
                if(cell==3 && b->squares.piece[7]==rook && b->squares.is_white[7]==1 && b->squares.piece[6]==none && b->squares.piece[5]==none && b->squares.piece[4]==none){
                    can_castle_far = 1;
                }
            }else if(!is_white && cell==59 && b->squares.piece[63]==rook && b->squares.is_white[63]==1 && b->squares.piece[62]==none && b->squares.piece[61]==none && b->squares.piece[60]==none){
                can_castle_far = 1;
            }
        }
        if(!can_castle_far){
            possible_moves[9] = 0;
        }
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
    struct intarray returnable = {positions, num_positions};
    return returnable;
}

struct intarray possible_piece_moves(struct board* b, struct piece_id* id){
    switch(id->type){
        case pawn:
            return possible_pawn_moves(b, id);
        case knight:
            return possible_knight_moves(b, id);
        case bishop:
            return possible_bishop_moves(b, id);
        case rook:
            return possible_rook_moves(b, id);
        case queen:
            return possible_queen_moves(b, id);
        case king:
            return possible_king_moves(b, id);
        default: {
            int* empty = (int*) malloc(0);
            struct intarray returnable = {empty, 0};
            return returnable;
        }
    }
}

void kill_piece(struct board* b, int new_position){
    int* pieces_arr;
    int is_white = b->squares.is_white[new_position];
    struct pieces_board* pb = &(b->black_pieces);
    struct previous_moves* pm = &(b->black_moves);
    if(is_white){
        pb = &(b->white_pieces);
        pm = &(b->white_moves);
    }
    int tam_arr = 0;
    switch(b->squares.piece[new_position]){
        case pawn:
            pieces_arr = pb->pawn;
            tam_arr = 8;
            break;
        case knight:
            pieces_arr = pb->knight;
            tam_arr = 10;
            break;
        case bishop:
            pieces_arr = pb->bishop;
            tam_arr = 10;
            break;
        case rook:
            pieces_arr = pb->rook;
            tam_arr = 10;
            if(is_white){
                if(new_position == 0){
                    pm->moved_rook_near = 1;
                }else if(new_position == 7){
                    pm->moved_rook_far = 1;
                }
            }else{
                if(new_position == 56){
                    pm->moved_rook_near = 1;
                }else if(new_position == 63){
                    pm->moved_rook_far = 1;
                }
            }
            break;
        case queen:
            pieces_arr = pb->queen;
            tam_arr = 9;
            break;
        case king:
            pb->king = PIECES_BOARD_DEAD;
            pm->moved_king = 1;
            return;
        default:
            return;
    }
    int index = 0;
    for(int i = 0; i < tam_arr; i++){
        if(pieces_arr[i] == new_position){
            index = i;
            break;
        }
    }
    for(int i = index; i < tam_arr-1; i++){
        pieces_arr[i] = pieces_arr[i+1];
    }
    pieces_arr[tam_arr-1] = PIECES_BOARD_DEAD;
    b->squares.piece[new_position] = none;
}

void move_piece(struct board* b, struct piece_id* id, int prev_position, int new_position){
    // assert: the move is valid
    int is_white = id->white;
    struct previous_moves* pm = &(b->black_moves);
    struct previous_moves* enemy_pm = &(b->white_moves);
    struct pieces_board* pb = &(b->black_pieces);
    int on_passant_pawn = new_position + 8;
    if(is_white){
        pb = &(b->white_pieces);
        pm = &(b->white_moves);
        enemy_pm = &(b->black_moves);
        on_passant_pawn = new_position - 8;
    }
    if(b->squares.piece[new_position]!=none){
        kill_piece(b, new_position);
    }else if(b->squares.piece[on_passant_pawn] == pawn && b->squares.is_white[on_passant_pawn] != is_white && enemy_pm->just_two_squared[on_passant_pawn%8]){ // on passant
        kill_piece(b, on_passant_pawn);
    }
    b->squares.piece[new_position] = b->squares.piece[prev_position];
    b->squares.piece[prev_position] = none;
    b->squares.is_white[new_position] = is_white;
    int move = new_position - prev_position;
    if(id->type == king){ // check castling and if it is, move rook
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
            int size_rook_arr = sizeof(pb->rook)/sizeof(int);
            int rook_index = 0;
            for(int i = 0; i < size_rook_arr; i++){
                if(pb->rook[i] == rook_prev){
                    rook_index = i;
                    break;
                }
            }
            struct piece_id rook_id = (struct piece_id) {rook, is_white, rook_index};
            move_piece(b, &rook_id, rook_prev, rook_new);
        }
    }
    if(id->type == pawn && (move == 16 || move == -16)){ // just moved two (could be on passanted)
        int pawn_index = prev_position%8;
        pm->just_two_squared[pawn_index] = 1;
    }else{
        memset(pm->just_two_squared, 0, sizeof(pm->just_two_squared));
    }
}

void apply_promotion(struct board* b, int is_white, enum board_piece piece_type){
    int base = 0;
    struct pieces_board* pb = &(b->black_pieces);
    if(is_white){
        base = 56;
        pb = &(b->white_pieces);
    }
    for(int i = base; i < base+8; i++){
        if(b->squares.piece[i] == pawn){
            int cant_pawns = sizeof(pb->pawn)/sizeof(int);
            for(int j = 0; j < cant_pawns; j++){
                if(pb->pawn[j] == i){
                    kill_piece(b, i);
                    b->squares.piece[i] = piece_type;
                    int cant_pieces_arr = 9;
                    int* piece_array = pb->queen;
                    if(piece_type == knight){
                        cant_pieces_arr = 10;
                        piece_array = pb->knight;
                    }
                    for(int k = 0; k < cant_pieces_arr; k++){
                        if(piece_array[k] == PIECES_BOARD_DEAD){
                            piece_array[k] = i;
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
}

struct board* get_potential_boards_moving_piece(struct board* b, struct piece_id* piece_id){
    int is_white = piece_id->white;
    struct pieces_board* opb = &(b->black_pieces);
    if(is_white){
        opb = &(b->white_pieces);
    }
    int prev_cell = get_piece_cell(opb, piece_id);
    struct intarray movesarray = possible_piece_moves(b, piece_id);
    int* moves = movesarray.arr;
    int cant_moves = movesarray.len;
    int cant_boards = cant_moves;
    if(piece_id->type == pawn){
        for(int i = 0; i < cant_moves; i++){
            int new_pos = moves[i] + prev_cell;
            if(new_pos <= 7 || new_pos >= 56){
                cant_boards++;
            }
        }
    }
    struct board* boards = (struct board*) malloc(cant_boards* sizeof(struct board));
    int board_pos = 0;
    for(int i = 0; i < cant_moves; i++){
        boards[board_pos] = copy_board(b);
        struct board* nb = &(boards[board_pos]);
        board_pos++;
        struct previous_moves* pm = &(nb->black_moves);
        if(is_white){
            pm = &(nb->white_moves);
        }
        if(piece_id->type == king){
            pm->moved_king = 1;
        }else if(piece_id->type == rook){
            if(prev_cell == 0 || prev_cell == 56){
                pm->moved_rook_near = 1;
            }else if(prev_cell == 7 || prev_cell == 63){
                pm->moved_rook_far = 1;
            }
        }
        int new_position = prev_cell + moves[i];
        move_piece(nb, piece_id, prev_cell, new_position);
        if(piece_id->type == pawn && (new_position <= 7 || new_position >= 56)){ // promotion
            boards[board_pos] = copy_board(nb);
            struct board* nb2 = &(boards[board_pos]);
            board_pos++;
            apply_promotion(nb, is_white, queen);
            apply_promotion(nb2, is_white, knight);
        }
    }
    return boards;
}

int main(){
    return 0;
}