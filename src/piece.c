#include "piece.h"
#include <stdbool.h>

char piece_to_char(enum board_piece type, bool white){
    char translation = '_';
    switch(type){
        case pawn:
            translation = 'p';
            break;
        case knight:
            translation = 'n';
            break;
        case bishop:
            translation = 'b';
            break;
        case rook:
            translation = 'r';
            break;
        case queen:
            translation = 'q';
            break;
        case king:
            translation = 'k';
            break;
        default:
            translation = '_';
    }
    if(white && translation != '_'){
        translation = translation + ('A' - 'a');
    }
    return translation;
}

char* piece_to_unicode(enum board_piece type, bool white){
    char* piece_unicodes[] = {"·", "♟", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"};
    int index = (int)type;
    if(!white && index != 0){
        index += 6;
    }
    return piece_unicodes[index];
}