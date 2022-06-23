#include "chess.h"
#include <stdio.h>

#define IS_CHECKED 0
#define LEGAL_MOVE 1
#define GET_KING 2

coord_t *old_king = NULL;

color is_checked(player_t *pl, coord_t curr, piece_t *pc, coord_t *king) {
    if (pc->type != &default_piece && pc->piece_color != pl->player_col) {
        coord_t *moves = show_avail_move(curr);
        for (int l = 0; moves[l].x != 0; l++) {
            //whenever any piece of the other two color has an available move
            //which has the same coord as the king, checked
            if (coord_equals(moves[l], *king)) {
                pc -> piece_color;
            }
        }
    }
    return NO_COLOR;
}

bool legal_move(player_t *pl, coord_t curr, piece_t *pc) {
    if (pc->type != &default_piece && pc->piece_color == pl->player_col) {
        coord_t *moves = show_avail_move(curr);
        for (int i = 0; moves[i].x != 0; i++) {
            //moves[i] is now dest of the pseudo-legal move
            //invoke in_check after move to check if it's legal
            bool alt_orig = false;
            bool alt_dest = false;
            piece_t* attacked = move_piece(curr, moves[i], &alt_orig, &alt_dest);
            bool still_check = in_check(pl);
            //reset board (not sure if this is safe)
            revert_piece((curr, move[i], alt_orig, alt_dest, attacked));
            if (!still_check) {
                return true;
            }
        }
    }
    return false;
}

bool get_pieces_info(player_t *pl, int mode, coord_t *coord, color *win1, color *win2) {
    player_t* pls[3] = {pl, adjacent(pl, true), adjacent(pl, false)};
    color rest = NO_COLOR;
    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < MAX_X; i++) {
            for (int j = 0; j < MAX_Y; j++) {
                coord_t curr = {.x = i, .y = j, .belongs = pls[k]};
                piece_t *pc = get_piece(curr);
                //Nested fors are just for iterating through the structure and getting the pieces
                //Following is the logic part
                switch (mode) {
                    case IS_CHECKED:
                        if (*win1 != NO_COLOR) {
                            if (pc->piece_color == rest) {
                                *win2 = is_checked(pl, curr, pc, coord);
                                if (*win2 != NO_COLOR) {
                                    return true;
                                }
                            }
                        } else {
                            *win1 = is_checked(pl, curr, pc, coord);
                            if (*win1 != NO_COLOR) {
                                if (adjacent(pl, true) == get_player(check)) {
                                    rest = adjacent(p1, false);
                                } else {
                                    rest = adjacent(p1, true;);
                                }
                            }
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
    return (win1 == NULL || *win1 != NO_COLOR);
}

bool in_check(player_t *pl, color *win1, color *win1) {
    //Optimisation for multiple "check" checks during legal_moves
    //prevents searching for king everytime, search only if king itself moves
    coord_t *king;
    if (old_king == NULL || get_piece(*old_king)->type != &king_type) {
        bool check = get_pieces_info(pl, GET_KING, king, NULL, NULL);
        assert(check);
    } else {
        //old_king points to coord_t of current king
        //so no pointer refs..?
        king = old_king;
    }
    return get_pieces_info(pl, IS_CHECKED, king, win1, win2);
}

//check if king is in check
bool has_legal_moves(player_t *pl) {
    return get_pieces_info(pl, LEGAL_MOVE, NULL, NULL, NULL);
}
//check for every piece in players command if piece has legal move, on first legal move break, else proceed with 0 counter, if counter == 0 than no legal moves
bool draw() {
    return black_player.agree_draw && white_player.agree_draw && red_player.agree_draw;
}
//check if every player has agreed to draw 
int game_state() {
    //if draw then game status draw
    //if !has_legal_moves {if check then status = checkmate else stalemate}
    //else continue as state = game
    if (draw()) {
        black_player.score += DRAW/10;
        white_player.score += DRAW/10;
        red_player.score += DRAW/10;
        return DRAW;
    } else if (!has_legal_moves(current_player)) {
        color win1 = NO_COLOR;
        color win2 = NO_COLOR;
        if (in_check(current_player, &win1, &win2)) {
            win -> score += CHECKMATE/10;
            if (win2 != NO_COLOR) {
                get_player(win2) -> score += CHECKMATE/10;
            } else {
                get_player(win2) -> score += STALEMATE/10;
            }
            return CHECKMATE;
        } else {
            black_player.score += STALEMATE/10;
            white_player.score += STALEMATE/10;
            red_player.score += STALEMATE/10;
            return STALEMATE;
        }
    } else {
        return GAME;
    }
}
