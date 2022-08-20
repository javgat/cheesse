#include <stdbool.h>
#ifndef PIECE_H
#define PIECE_H

enum board_piece{
    none = 0,
    pawn = 1,
    knight = 2,
    bishop = 3,
    rook = 4,
    queen = 5,
    king = 6,
};

char piece_to_char(enum board_piece type, bool white);

char* piece_to_unicode(enum board_piece type, bool white);
#endif