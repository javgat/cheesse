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
    int rec = 3;
    if(argc > 1){
        rec = atoi(argv[1]);
    }
    struct board* ib = new_board_default();
    struct board b;
    copy_board(ib, &b);
    while(true){
        struct board_result br = minimax(&b, true, rec);
        printf("Evaluation: %d\n", br.eb->evaluation);
        if(rec > 0){
            copy_board(&br.previous.arr[rec-1], &b);
        }else{
            copy_board(br.eb->b, &b);
        }
        if(rec > 0){
            printf("\nNext move\n\n");
            print_board(&br.previous.arr[rec-1], true);
        }
        if(br.eb->draw){
            printf("\nDRAW\n");
            break;
        }
        char move_from_st[3] = "";
        char move_to_st[3] = "";
        fill_cell_name(b.last_move[0], move_from_st);
        fill_cell_name(b.last_move[1], move_to_st);
        printf("%s %s\n", move_from_st, move_to_st);fflush(stdout);
        destroy_boardarray(&br.previous);
        destroy_eval_board(br.eb);
        if(rec > 0){
            //printf("\nNext move:\n\n");
            //print_board(&b);
        }
        int from, to;
        char from_st[4], to_st[4];
        scanf("%s %s", from_st, to_st);
        if(from_st[0]=='r'){
            int times_back = from_st[1] - '0';
            int same_pieces_size = b.prev_boards.same_pieces->size;
            struct board* to_copy;
            if(same_pieces_size < times_back){
                times_back -= same_pieces_size;
                int ignore_size = b.prev_boards.ignore->size;
                to_copy = (struct board*) arraylist_get(b.prev_boards.ignore, ignore_size - times_back);
            }else{
                to_copy = (struct board*) arraylist_get(b.prev_boards.same_pieces, same_pieces_size - times_back);
            }
            copy_board(to_copy, &b);
            print_board(to_copy, true);
            printf("\nReturned to:\n\n");
            print_board(&b, true);
        }else{
            from = get_cell_id(from_st);
            to = get_cell_id(to_st);
            printf("moving from %d to %d\n", from, to);
            move_piece(&b, from, to);
            printf("Moved board:\n\n");
            print_board(&b, true);
            printf("\n");
        }
    }
    return 0;
}