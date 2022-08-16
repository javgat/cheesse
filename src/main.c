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

int get_cell_id(char* cell_st){
    int cell_id = 0;
    int number = cell_st[1] - '0';
    cell_id = (number-1)*8;
    char letter = cell_st[0];
    int letter_add = -((int)(letter - 'h'));
    return cell_id + letter_add;
}

void fill_cell_name(int cell_id, char* cell_st){
    int letter_add = cell_id%8;
    cell_st[0] = (char)(-letter_add + (int)'h');
    int number = ((cell_id-letter_add)/8) + 1;
    cell_st[1] = (char)(number + (int)'0');
}

int main(int argc, char *argv[]){
    int rec = 2;
    if(argc > 1){
        rec = atoi(argv[1]);
    }
    struct board b;
    init_board(&b);
    while(1){
        struct board_result br = minimax(&b, 1, rec);
        //printf("\nFinal board\n\n");
        //print_board(br.eb->b);
        printf("Evaluation: %d\n", br.eb->evaluation);
        struct board old_b = b;
        if(rec > 0){
            b = copy_board(&br.previous[rec-1]);
        }else{
            b = copy_board(br.eb->b);
        }
        for(int i = 0; i < rec; i++){
            printf("\nPreboard %d\n\n", i);
            print_board(&br.previous[i]);
        }
        enum board_piece bps[] = {pawn, knight, bishop, rook, queen};
        int cant_pieces[] = {8, 10, 10, 10, 9};
        int no_change = 1;
        for(int i = 0; i < 5; i++){
            int *oparr = get_piece_array(&old_b.white_pieces, bps[i]);
            int *parr = get_piece_array(&b.white_pieces, bps[i]);
            for(int j = 0; j < cant_pieces[i]; j++){
                if(oparr[j] != parr[j]){
                    no_change = 0;
                    char move_from_st[3] = "";
                    char move_to_st[3] = "";
                    fill_cell_name(oparr[j], move_from_st);
                    fill_cell_name(parr[j], move_to_st);
                    printf("%s %s\n", move_from_st, move_to_st);fflush(stdout);
                }
            }
        }
        if(old_b.white_pieces.king != b.white_pieces.king){
            no_change = 0;
            char move_from_stkg[3] = "";
            char move_to_stkg[3] = "";
            fill_cell_name(old_b.white_pieces.king, move_from_stkg);
            fill_cell_name(b.white_pieces.king, move_to_stkg);
            printf("%s %s\n", move_from_stkg, move_to_stkg);fflush(stdout);
        }
        if(no_change && br.eb->stalemate){
            printf("\nSTALEMATE\n");
            return 0;
        }
        if(no_change){
            printf("ERROR NO CHANGE\n");
#ifdef DEBUG
            printf("LAST MOVE: FROM %d TO %d\n", b.last_move[0], b.last_move[1]);
#endif
        }
        free(br.previous);
        free(br.eb->b);
        free(br.eb);
        if(rec > 0){
            //printf("\nNext move:\n\n");
            //print_board(&b);
        }
        int from, to;
        char from_st[4], to_st[4];
        scanf("%s %s", from_st, to_st);
        from = get_cell_id(from_st);
        to = get_cell_id(to_st);
        printf("moving from %d to %d\n", from, to);
        enum board_piece type = b.squares.piece[from];
        int white = b.squares.is_white[from];
        struct piece_id piece_id = {type, white, 0};
        move_piece(&b, &piece_id, from, to);
        printf("Moved board:\n\n");
        print_board(&b);
        printf("\n");
    }
    return 0;
}