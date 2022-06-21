#include "chess.h"
#include <stdio.h>

bool in_check();
//check if king is in check
bool has_legal_moves();
//check for every piece in players command if piece has legal move, on first legal move break, else proceed with 0 counter, if counter == 0 than no legal moves
bool draw();
//check if every player has agreed to draw 
int game_state(player_t curr_player) {
    //if draw then game status draw
    //if !has_legal_moves {if check then status = checkmate else stalemate}\
    //else continue as state = game
}
