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
    bool is_white = true;
    enum prune_strat prune = alpha_beta;
    if(argc > 1){
        is_white = (argv[1][0] == 'w');
    }
    if(argc > 2){
        int pruneint = atoi(argv[2]);
        switch(pruneint){
            case 0:
                prune = no_pruning;
                break;
            case 1:
                prune = alpha_beta;
                break;
            default:
                prune = bns_pruning;
        }
    }
    if(argc > 3){
        rec = atoi(argv[3]);
    }
    char* color_st = "black";
    if(is_white){
        color_st = "white";
    }
    char* prune_st = "";
    if(prune == alpha_beta){
        prune_st = " Performing alphabeta pruning.";
    }else if(prune == bns_pruning){
        prune_st = " Performing BNS pruning.";
    }
    printf("Playing as %s, recursion level %d.%s\n\n", color_st, rec, prune_st);
    struct board* b = new_board_default();
    char move_from_st[3] = "";
    char move_to_st[3] = "";
    int from, to;
    char from_st[4], to_st[4];
    char buffer[40];
    if(!is_white){
        printf("Your move: ");
        fgets(buffer, 40, stdin);
        strncpy(from_st, buffer, 2);
        from_st[2] = '\0';
        strncpy(to_st, &buffer[2], 2);
        to_st[2] = '\0';
        from = get_cell_id(from_st);
        to = get_cell_id(to_st);
        struct board* old_b = new_board_copy(b);
        move_piece_save_history(old_b, b, from, to);
        printf("\n\tMoved board:\n\n");
        print_board(b, is_white);
        printf("\n");
    }
    int last_eval = 0;
    while(true){
        struct board_result br;
        if(prune == bns_pruning){
            br = bns(b, last_eval - 100, last_eval + 100, is_white, rec);
        }else{
            br = minimax(b, is_white, rec, prune==alpha_beta);
        }
        last_eval = br.eb->evaluation;
        printf("Evaluation: %d\n", br.eb->evaluation);
        if(rec > 0){
            b = new_board_copy(&br.previous.arr[rec-1]);
        }else{
            b = new_board_copy(br.eb->b);
        }
        if(rec > 0){
            printf("\nNext move\n\n");
            print_board(&br.previous.arr[rec-1], is_white);
        }
        if(br.eb->draw){
            printf("\nDRAW\n");
            break;
        }
        fill_cell_name(b->last_move[0], move_from_st);
        fill_cell_name(b->last_move[1], move_to_st);
        printf("My move: %s%s\n", move_from_st, move_to_st);fflush(stdout);
        destroy_boardarray(&br.previous);
        destroy_eval_board(br.eb);
        bool input_move = false;
        while(!input_move){
            printf("Your move: ");
            fgets(buffer, 40, stdin);
            if(buffer[0]=='r'){// rewind
                int times_back = ((buffer[1] - '0'));
                int same_pieces_size = b->prev_boards.same_pieces->size;
                struct board* to_copy;
                if(same_pieces_size < times_back){
                    times_back -= same_pieces_size;
                    int ignore_size = b->prev_boards.ignore->size;
                    to_copy = (struct board*) arraylist_get(b->prev_boards.ignore, ignore_size - times_back);
                }else{
                    to_copy = (struct board*) arraylist_get(b->prev_boards.same_pieces, same_pieces_size - times_back);
                }
                b = new_board_copy(to_copy);
                printf("\n\tReturned to:\n\n");
                print_board(b, is_white);
            }else if(buffer[0]=='x'){// expand
                for(int i = br.previous.len; i > 0; i--){
                    printf("\nStep %d\n\n", br.previous.len-i+1);
                    print_board(&br.previous.arr[i-1], is_white);
                }
            }else{
                if(strlen(buffer) < 5){
                    printf("Error in input, it should be like g2g3\n");
                    continue;
                }
                strncpy(from_st, buffer, 2);
                from_st[2] = '\0';
                strncpy(to_st, &buffer[2], 2);
                to_st[2] = '\0';
                from = get_cell_id(from_st);
                to = get_cell_id(to_st);
                if(from > 63 || from < 0 || to > 63 || from < 0){
                    printf("Error in input, it should be like g2g3\n");
                    continue;
                }
                input_move = true;
                bool promoting = (to < 8 || to > 55) && b->piece[from] == pawn;
                struct board* old_b = new_board_copy(b);
                move_piece_save_history(old_b, b, from, to);
                while(promoting){
                    printf("Pawn must promote. Which piece? Queen(q), Rook(r), Bishop(b) or Knight(n): ");
                    char buffer2[40];
                    fgets(buffer2, 40, stdin);
                    switch(buffer2[0]){
                        case 'q':
                            apply_promotion(b, !is_white, queen);
                            promoting = false;
                            break;
                        case 'r':
                            apply_promotion(b, !is_white, rook);
                            promoting = false;
                            break;
                        case 'b':
                            apply_promotion(b, !is_white, bishop);
                            promoting = false;
                            break;
                        case 'n':
                            apply_promotion(b, !is_white, knight);
                            promoting = false;
                            break;
                        default:
                            printf("Error inputing the promotion. Try again.\n");
                            break;
                    }
                }
                printf("\n\tMoved board:\n\n");
                print_board(b, is_white);
                printf("\n");
            }
        }
    }
    return 0;
}