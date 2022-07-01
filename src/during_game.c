#include "chess.h"

coord_t *curr_avail_moves = NULL;

const coord_t end_of_list = {-1, -1, NULL};

coord_t* show_avail_move(coord_t piece_coord) {
    coord_t *return_corrds = malloc(64 * sizeof(coord_t));
    int num_of_moves = 0;

    if (current_piece == NULL) {
        printf("NUL\n");
    }   

    short dx;
    short dy; 

    for (int i = 0; i < 8; i++) {
        dx = current_piece->type->move_vec[i][0];
        dy = current_piece->type->move_vec[i][1];
        if (dx == 0 && dy == 0) {
            break;
        } 
        coord_t current_coord = piece_coord;
        coord_t alt_loc = move_vector(false, current_coord, dx, dy);
        coord_t buffer = move_vector(true, current_coord, dx, dy);
        bool changed_once = false;
        bool alt_select = false;

        if (!coord_equals(buffer, DEFAULT_COORD)) {    
            //if piece is not knight, king or pawn
            if (!current_piece->type->single_move) {
                while (!coord_equals(buffer, DEFAULT_COORD) && get_piece(buffer) == &default_piece) {                
                    return_corrds[num_of_moves] = buffer;
                    num_of_moves++;
                    if (!changed_once) {
                        if (buffer.belongs != current_coord.belongs) {
                            changed_once = true;
                            if (!alt_select) {
                                buffer = move_vector(false, current_coord, dx, dy);
                                if (!coord_equals(buffer, return_corrds[num_of_moves - 1])) {
                                    alt_loc = buffer;
                                    alt_select = true;
                                }
                            }
                            dx *= -1;
                            dy *= -1;
                        }
                    }
                    //update
                    current_coord = return_corrds[num_of_moves - 1];
                    buffer = move_vector(true, current_coord, dx, dy);
                }
            }   

            if (alt_select) {
                while (!coord_equals(alt_loc, DEFAULT_COORD) && get_piece(alt_loc) == &default_piece) {
                    return_corrds[num_of_moves] = alt_loc;
                    num_of_moves++;
                    alt_loc = move_vector(false, alt_loc, dx, dy);
                }
            }

            //both single and multiple moves
            piece_t *dest_pc = get_piece(buffer);
            piece_t *alt_pc = get_piece(alt_loc);
            if (!coord_equals(buffer, DEFAULT_COORD) && 
            (dest_pc == &default_piece ||
            dest_pc->piece_color != current_piece->piece_color)) {
                if (current_piece->type == &i_pawn_type || current_piece->type == &o_pawn_type){
                    if ((dx != 0 && dest_pc != &default_piece) 
                    || (dx == 0 && dest_pc == &default_piece)) {
                        return_corrds[num_of_moves] = buffer;
                        num_of_moves++;
                    }
                } else {
                    return_corrds[num_of_moves] = buffer;
                    num_of_moves++;
                }  
            }

            if (!coord_equals(alt_loc, DEFAULT_COORD) && !coord_equals(alt_loc, buffer) &&
            (alt_pc == &default_piece ||
            alt_pc->piece_color != current_piece->piece_color)) {
                if (current_piece->type == &i_pawn_type || current_piece->type == &o_pawn_type){
                    if ((dx != 0 && alt_pc != &default_piece) 
                    || (dx == 0 && alt_pc == &default_piece)) {
                        return_corrds[num_of_moves] = alt_loc;
                        num_of_moves++;
                    }
                }  else {
                    return_corrds[num_of_moves] = alt_loc;
                    num_of_moves++;
                }
            }
        }
    }

    if (!displaced(piece_coord)) {
        if (current_piece->type == &i_pawn_type || current_piece->type == &o_pawn_type) {
            printf("pawn double\n");
            coord_t buffer = move_vector(true, piece_coord, 0, current_piece->type->move_vec[0][1] * 2);
            if (!coord_equals(buffer, DEFAULT_COORD)) {
                return_corrds[num_of_moves] = buffer;
                num_of_moves++;   
            }
        } else if (current_piece->type == &king_type) {
            printf("check castling\n");
            coord_t rook_l = {0, 0, piece_coord.belongs};
            coord_t rook_r = {0, MAX_X-1, piece_coord.belongs};
            if (!displaced(rook_l)) {
                bool left = true;
                for (int i = 1; i < piece_coord.x; i++) {
                    coord_t pos = {i, 0, piece_coord.belongs};
                    if (get_piece(pos) != &default_piece) {
                        left = false;
                        break;
                    }
                }
                if (left) {
                    coord_t dest = {piece_coord.x-2, 0, piece_coord.belongs};
                    return_corrds[num_of_moves] = dest;
                    num_of_moves++;
                }
            }
            if (!displaced(rook_r)) {
                bool right = true;
                for (int j = piece_coord.x+1; j < MAX_X-1; j++) {
                    coord_t pos = {j,0,piece_coord.belongs};
                    if (get_piece(pos) != &default_piece) {
                        right = false;
                        break;
                    }
                }
                if (right) {
                    coord_t dest = {piece_coord.x+2, 0, piece_coord.belongs};
                    return_corrds[num_of_moves] = dest;
                    num_of_moves++;
                }
            }
        }
    }
    return_corrds[num_of_moves] = end_of_list;
    num_of_moves++;
    return return_corrds;
}
