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
    struct board* ib = new_board_default();
    struct board b;
    copy_board(ib, &b);
    while(true){
        struct board_result br = minimax(&b, true, rec);
        //printf("\nFinal board\n\n");
        //print_board(br.eb->b);
        printf("Evaluation: %d\n", br.eb->evaluation);
        //struct board old_b = b;
        if(rec > 0){
            copy_board(&br.previous[rec-1], &b);
        }else{
            copy_board(br.eb->b, &b);
        }
        //for(int i = 0; i < rec; i++){
        //    printf("\nPreboard %d\n\n", i);
        //    print_board(&br.previous[i], true);
        //}
        if(rec > 0){
            printf("\nNext move\n\n");
            print_board(&br.previous[rec-1], true);
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
        move_piece(&b, from, to);
        printf("Moved board:\n\n");
        print_board(&b, true);
        printf("\n");
    }
    return 0;
}