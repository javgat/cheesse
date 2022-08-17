#include <stdio.h>
#include "board.h"

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
        }
#ifdef DEBUG
        printf("LAST MOVE: FROM %d TO %d\n", b.last_move[0], b.last_move[1]);
#endif
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