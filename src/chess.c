#include "chess.h"

#define IS_CHECKED 0
#define LEGAL_MOVE 1
#define GET_KING 2

coord_t *old_king = NULL;

color is_checked(player_t *pl, coord_t curr, piece_t *pc, coord_t *king) {
    if (pc != &default_piece && pc->piece_color != pl->player_col) {
        coord_t *moves = show_avail_move(curr);
        for (int l = 0; moves[l].x != 0; l++) {
            //whenever any piece of the other two color has an available move
            //which has the same coord as the king, checked
            if (coord_equals(moves[l], *king)) {
                return (pc -> piece_color);
            }
        }
    }
    return NO_COLOR;
}

bool legal_move(player_t *pl, coord_t curr, piece_t *pc) {
    if (pc != &default_piece && pc->piece_color == pl->player_col) {
        coord_t *moves = show_avail_move(curr);
        for (int i = 0; moves[i].x != 0; i++) {
            //moves[i] is now dest of the pseudo-legal move
            //invoke in_check after move to check if it's legal
            bool alt_orig = false;
            bool alt_dest = false;
            piece_t* attacked = move_piece(curr, moves[i], &alt_orig, &alt_dest);
            bool still_check = in_check(pl);
            //reset board (not sure if this is safe)
            revert_move(curr, moves[i], alt_orig, alt_dest, attacked);
            if (!still_check) {
                return true;
            }
        }
    }
    return false;
}

bool get_pieces_info(player_t *pl, int mode, coord_t *coord, player_t *win1, player_t *win2) {
    player_t* pls[3] = {pl, adjacent(pl, true), adjacent(pl, false)};
    color rest = NO_COLOR;
    bool check;
    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < MAX_X; i++) {
            for (int j = 0; j < MAX_Y; j++) {
                coord_t curr = {.x = i, .y = j, .belongs = pls[k]};
                piece_t *pc = get_piece(curr);
                //Nested fors are just for iterating through the structure and getting the pieces
                //Following is the logic part
                switch (mode) {
                    case IS_CHECKED:
                        if (win1 != NULL) {
                            if (pc->piece_color == rest) {
                                color col = is_checked(pl, curr, pc, coord);
                                if (col != NO_COLOR) {
                                    win2 = get_player(col);
                                    return true;
                                }
                            }
                        } else {
                            color col = is_checked(pl, curr, pc, coord);
                            if (col != NO_COLOR) {
                                if (adjacent(pl, true) == win1) {
                                    rest = adjacent(pl, false)->player_col;
                                } else {
                                    rest = adjacent(pl, true)->player_col;
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
    return (win1 == NULL);
}

bool in_check(player_t *pl, player_t *win1, player_t *win2) {
    //Optimisation for multiple "check" checks during legal_moves
    //prevents searching for king everytime, search only if king itself moves
    coord_t *king = NULL;
    if (old_king != NULL && get_piece(*old_king)->type == &king_type) {
        //old_king points to coord_t of current king
        //so no pointer refs..?
        king = old_king;
    } else {
        bool check = get_pieces_info(pl, GET_KING, king, NULL, NULL);
        assert(check);
    }
    return get_pieces_info(pl, IS_CHECKED, king, win1, win2);
}

//check if king is in check
bool has_legal_moves(player_t *pl) {
    return get_pieces_info(pl, LEGAL_MOVE, NULL, NULL, NULL);
}
//check for every piece in players command if piece has legal move, on first legal move break, else proceed with 0 counter, if counter == 0 than no legal moves

status draw() {
    black_player.score += DRAW/10;
    white_player.score += DRAW/10;
    red_player.score += DRAW/10;
    return DRAW;
}

//check if every player has agreed to draw 
status game_state(player_t *win1, player_t *win2) {
    //if draw then game status draw
    //if !has_legal_moves {if check then status = checkmate else stalemate}
    //else continue as state = game
    if (!has_legal_moves(current_player)) {
        if (in_check(current_player, win1, win2)) {
            win1 -> score += CHECKMATE/10;
            if (win2 != NULL) {
                win2 -> score += CHECKMATE/10;
            } else {
                win2 -> score += STALEMATE/10;
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
