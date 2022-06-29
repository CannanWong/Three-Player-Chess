#include "chess.h"

coord_t *curr_avail_moves = NULL;

const coord_t end_of_list = {-1, -1, NULL};

static bool is_valid(coord_t current_coord) {
    return (current_coord.x < MAX_X && current_coord.y < MAX_Y) && (current_coord.x >= 0 && current_coord.y >= 0);
}

coord_t* show_avail_move(coord_t piece_coord) {
    coord_t *return_corrds = malloc(64 * sizeof(coord_t));
    int num_of_moves = 0;
    short *current_vec = NULL; 

    if (current_piece == NULL) {
        printf("NUL\n");
    }    

    for (int i = 0; (current_piece->type->move_vec[i][0] != 0 || current_piece->type->move_vec[i][1] != 0) && i < 8; i++) {    
        printf("%i %i\n", current_piece->type->move_vec[i][0], current_piece->type->move_vec[i][1]);
        printf("%i\n", i);
        coord_t current_coord = piece_coord;
        current_vec = current_piece->type->move_vec[i];
        coord_t buffer = move_vector(true, current_coord, current_vec[0], current_vec[1]);
        printf("%i %i %i\n", buffer.belongs->player_col, buffer.x, buffer.y);
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
                    if (changed_once) {
                        buffer = move_vector(true, current_coord, current_vec[0] * -1, current_vec[1] * -1);  
                    }
                }
                //attack opponent piece
            }                
            if (is_valid(buffer) && get_piece(buffer)->piece_color != current_piece->piece_color) {
                return_corrds[num_of_moves] = buffer;
                num_of_moves++;  
                if (!changed_once) {
                    change_boarder = buffer.belongs != current_coord.belongs;
                    if (change_boarder) {
                        buffer = move_vector(false, current_coord, current_vec[0], current_vec[1]);
                        if (!coord_equals(buffer, return_corrds[num_of_moves - 1])) {
                            return_corrds[num_of_moves] = buffer;
                            num_of_moves++;   
                        }
                    }
                }
            }
        }
    }
    printf("exit for loop\n");
    //first move from pawn can be 2 blocks

    if (!displaced(piece_coord)) {
        printf("not displaced\n");
        if (current_piece->type == &i_pawn_type || current_piece->type == &o_pawn_type) {
            printf("pawn double\n");
            coord_t buffer = move_vector(true, piece_coord, 0, current_piece->type->move_vec[0][1] * 2);
            if (is_valid(buffer)) {
                printf("%d %d %d\n", buffer.belongs->player_col, buffer.x, buffer.y);
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
    printf("after not displaced\n");
    return return_corrds;
}
