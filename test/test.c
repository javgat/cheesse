#include <stdio.h>
#include "board.h"

void move_to(struct board *b, int from, int to){
    enum board_piece type = b->squares.piece[from];
    int white = b->squares.is_white[from];
    struct piece_id piece_id = {type, white, 0};
    move_piece(b, &piece_id, from, to);
}

int main(){
    struct board b;
    init_board(&b);
    move_to(&b, 60, 36);
    move_to(&b, 11, 27);
    enum board_piece type = b.squares.piece[27];
    int white = b.squares.is_white[27];
    struct piece_id piece_id = {type, white, 0};
    struct intarray ia = possible_pawn_moves(&b, &piece_id);
    for(int i = 0; i < ia.len; i++){
        printf("%d ", ia.arr[i]);
    }
    free(ia.arr);
    printf("\n");
    print_board(&b);
    return 0;
}