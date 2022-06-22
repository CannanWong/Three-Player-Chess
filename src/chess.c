#include "chess.h"
#include <stdio.h>

#define IS_CHECKED 0
#define LEGAL_MOVE 1
#define GET_KING 2

coord_t *old_king = NULL;

bool check_info(player_t *pl, coord_t curr, piece_t *pc, coord_t *king) {
    if (pc->type != &default_piece && pc->piece_color != pl->player_col) {
        coord_t *moves = show_avail_move(curr);
        for (int l = 0; moves[l].x != 0; l++) {
            //whenever any piece of the other two color has an available move
            //which has the same coord as the king, checked
            if (coord_equals(moves[l], *king)) {
                return true;
            }
        }
    }
    return false;
}

bool legal_move(player_t *pl, coord_t curr, piece_t *pc) {
    if (pc->type != &default_piece && pc->piece_color == pl->player_col) {
        coord_t *moves = show_avail_move(curr);
        for (int i = 0; moves[i].x != 0; i++) {
            //moves[i] is now dest of the pseudo-legal move
            //(determined by inspecting moves on tiles with own color pieces)
            //invoke in_check after move to check if it's legal
            move_piece(curr, moves[i], true);
            bool still_check = in_check(pl);
            //reset board (not sure if this is safe)
            move_piece(moves[i], curr, true);
            if (!still_check) {
                return true;
            }
        }
    }
    return false;
}

bool get_pieces_info(player_t *pl, int mode, coord_t *coord) {
    bool check = false;
    //check own player board first to optimise search speeds at early stage
    player_t* pls[3] = {pl, adjacent(pl, true), adjacent(pl, false)};
    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < MAX_X; i++) {
            for (int j = 0; j < MAX_Y; j++) {
                coord_t curr = {.x = i, .y = j, .belongs = pls[k]};
                piece_t *pc = get_piece(curr);
                //Nested fors are just for iterating through the structure and getting the pieces
                //Following is the logic part
                switch (mode) {
                    case IS_CHECKED:
                        check = is_checked(pl, curr, pc, coord);
                        if (check) {
                            return true;
                        }
                    case LEGAL_MOVE:
                        check = legal_move(pl, curr, pc);
                        if (check) {
                            return true;
                        }
                    case GET_KING:
                        if (pc == pl->king) {
                            *coord = curr;
                            return true;
                        }
                }
            }
        }
    }
    return false;
}

bool in_check(player_t *pl) {
    //Optimisation for multiple "check" checks during legal_moves
    //prevents searching for king everytime, search only if king itself moves
    coord_t *king;
    if (old_king == NULL || get_piece(*old_king)->type != &king_type) {
        bool check = get_pieces_info(pl, GET_KING, king);
        assert(check);
    } else {
        //old_king points to coord_t of current king
        //so no pointer refs..?
        king = old_king;
    }
    return get_pieces_info(pl, IS_CHECKED, king);
}

//check if king is in check
bool has_legal_moves(player_t *pl) {
    return get_pieces_info(pl, LEGAL_MOVE, NULL);
}
//check for every piece in players command if piece has legal move, on first legal move break, else proceed with 0 counter, if counter == 0 than no legal moves
bool draw() {
    return black_player.agree_draw && white_player.agree_draw && red_player.agree_draw;
}
//check if every player has agreed to draw 
int game_state(player_t *curr_player) {
    //if draw then game status draw
    //if !has_legal_moves {if check then status = checkmate else stalemate}
    //else continue as state = game
    if (draw()) {
        return DRAW;
    } else if (!has_legal_moves(curr_player)) {
        if (in_check(curr_player)) {
            return CHECKMATE;
        } else {
            return STALEMATE;
        }
    } else {
        return GAME;
    }
}
