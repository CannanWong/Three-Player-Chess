#include "chess.h"
#define MAX_MOVES 64

static bool is_valid(coord_t current_coord) {
    return (current_coord.x != MAX_X && current_coord.y != MAX_Y);
}

static bool coord_equals(coord_t coord1, coord_t coord2) {
    return coord1.belongs == coord2.belongs && coord1.x == coord2.x && coord1.y == coord2.y;
}

coord_t* show_avail_move(coord_t piece_coord) {
    piece_t *current_piece = get_piece(piece_coord);
    coord_t *return_corrds = calloc(64, sizeof(coord_t));
    int num_of_moves = 0;
    short *current_vec = NULL; 
    for (int i = 0; current_piece->type->move_vec[i]; i++) {
        coord_t current_coord = piece_coord;
        current_vec = &current_piece->type->move_vec[0];
        coord_t buffer = move_vector(true, current_coord, current_vec[0], current_vec[1]);
        bool change_boarder  = false;
        bool changed_once = false;
        if (is_valid(buffer)) {            
            //if piece is not knight, king or pawn
            if (!current_piece->type->single_move) {
                while (is_valid(buffer) && get_piece(buffer) == NULL) {                
                    return_corrds[num_of_moves] = buffer;
                    num_of_moves++;
                    if (!changed_once) {
                        change_boarder = buffer.belongs != current_coord.belongs;
                        if (change_boarder) {
                            changed_once = true;
                            if (current_piece->type == &i_pawn_type) {
                                current_piece->type = &o_pawn_type;
                            }
                            buffer = move_vector(false, current_coord, current_vec[0], current_vec[1]);
                            if (!coord_equals(buffer, return_corrds[num_of_moves - 1])) {
                                return_corrds[num_of_moves] = buffer;
                                num_of_moves++;   
                            }
                    }
                    }
                    current_coord = return_corrds[num_of_moves - 1];
                    buffer = move_vector(true, current_coord, current_vec[0], current_vec[1]);  
                }
                //attack opponent piece
            }                
            if (is_valid(buffer) && get_piece(buffer)->piece_color != current_piece->piece_color) {
                return_corrds[num_of_moves] = buffer;
                num_of_moves++;  
                change_boarder = buffer.belongs != current_coord.belongs;
                if (!changed_once && change_boarder) {
                    buffer = move_vector(false, current_coord, current_vec[0], current_vec[1]);
                    if (!coord_equals(buffer, return_corrds[num_of_moves - 1])) {
                        return_corrds[num_of_moves] = buffer;
                        num_of_moves++;   
                    }
                }
            }
        }
    }
    //first move from pawn can be 2 blocks
    if (displaced(piece_coord, current_piece)) {
        if (current_piece->type == &i_pawn_type || current_piece->type == &o_pawn_type) {
            coord_t buffer = move_vector(true, piece_coord, 0, current_piece->type->move_vec[0][1] * 2);
            if (is_valid(buffer)) {
                return_corrds[num_of_moves] = buffer;
                num_of_moves++;   
            }
        } else if (current_piece->type == &king_type) {
         
        }
    }
    return return_corrds;
}