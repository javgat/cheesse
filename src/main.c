#include <stdio.h>
#include "board.h"

void init_board(struct board* b){
    // prev moves
    b->black_moves.moved_king = 0;
    b->black_moves.moved_rook_far = 0;
    b->black_moves.moved_rook_near = 0;
    memset(b->black_moves.just_two_squared, 0, sizeof(b->black_moves.just_two_squared));
    b->white_moves.moved_king = 0;
    b->white_moves.moved_rook_far = 0;
    b->white_moves.moved_rook_near = 0;
    memset(b->white_moves.just_two_squared, 0, sizeof(b->white_moves.just_two_squared));
    // white
    struct pieces_board* pb = &(b->white_pieces);
    int white_pawns[] = {8, 9, 10, 11, 12, 13, 14, 15};
    int white_knights[] = {1, 6, 64, 64, 64, 64, 64, 64, 64, 64};
    int white_bishops[] = {2, 5, 64, 64, 64, 64, 64, 64, 64, 64};
    int white_rooks[] = {0, 7, 64, 64, 64, 64, 64, 64, 64, 64};
    int white_queens[] = {4, 64, 64, 64, 64, 64, 64, 64, 64};
    pb->king = 3;
    for(int i = 0; i < 8; i++){
        pb->pawn[i] = white_pawns[i];
    }
    for(int i = 0; i < 10; i++){
        pb->knight[i] = white_knights[i];
    }
    for(int i = 0; i < 10; i++){
        pb->bishop[i] = white_bishops[i];
    }
    for(int i = 0; i < 10; i++){
        pb->rook[i] = white_rooks[i];
    }
    for(int i = 0; i < 9; i++){
        pb->queen[i] = white_queens[i];
    }
    // black
    pb = &(b->black_pieces);
    int black_pawns[] = {48, 49, 50, 51, 52, 53, 54, 55};
    int black_knights[] = {57, 62, 64, 64, 64, 64, 64, 64, 64, 64};
    int black_bishops[] = {58, 61, 64, 64, 64, 64, 64, 64, 64, 64};
    int black_rooks[] = {56, 63, 64, 64, 64, 64, 64, 64, 64, 64};
    int black_queens[] = {60, 64, 64, 64, 64, 64, 64, 64, 64};
    pb->king = 59;
    for(int i = 0; i < 8; i++){
        pb->pawn[i] = black_pawns[i];
    }
    for(int i = 0; i < 10; i++){
        pb->knight[i] = black_knights[i];
    }
    for(int i = 0; i < 10; i++){
        pb->bishop[i] = black_bishops[i];
    }
    for(int i = 0; i < 10; i++){
        pb->rook[i] = black_rooks[i];
    }
    for(int i = 0; i < 9; i++){
        pb->queen[i] = black_queens[i];
    }
    // squares
    memset(b->squares.is_white, 0, sizeof(b->squares.is_white));
    for(int i = 0; i < 16; i++){
        b->squares.is_white[i] = 1;
    }
    for(int i = 16; i < 48; i++){
        b->squares.piece[i] = none;
    }
    for(int i = 8; i < 16; i++){
        b->squares.piece[i] = pawn;
    }
    for(int i = 48; i < 56; i++){
        b->squares.piece[i] = pawn;
    }
    int rooks[] = {0, 7, 56, 63};
    int knights[] = {1, 6, 57, 62};
    int bishops[] = {2, 5, 58, 61};
    for(int i = 0; i < 4; i++){
        b->squares.piece[rooks[i]] = rook;
        b->squares.piece[knights[i]] = knight;
        b->squares.piece[bishops[i]] = bishop;
    }
    b->squares.piece[4] = queen;
    b->squares.piece[60] = queen;
    b->squares.piece[3] = king;
    b->squares.piece[59] = king;
    return;
}

void print_board(struct board* b){
    for(int j = 0; j<64; j++){
        switch(b->squares.piece[j]){
            case pawn:
                printf("p");
                break;
            case knight:
                printf("n");
                break;
            case bishop:
                printf("b");
                break;
            case rook:
                printf("r");
                break;
            case queen:
                printf("q");
                break;
            case king:
                printf("k");
                break;
            default:
                printf("_");
        }
        if(j%8 == 7){
            printf("\n");
        }
    }
}

int main(){
    struct board b;
    init_board(&b);
    struct eval_board_array bs = max_board(&b, 1, 3);
    return 0;
}