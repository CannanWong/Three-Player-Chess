#include "chess.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    set_pieces();
    //place pieces on board
    while(1) {
        turn_player();
        //find player turn and turn board to face player
        if (is_game_finished(current_player))
        {
            //takes current player, sees if they are checkmated or have no valid moves if so game is over
            break;
        }
        while (!valid_move_made)
        {
            get_player_input();
            //click on piece
            get_player_input();
            //click away or on valid move
        }
        do_move();
    }
    end_game();

    return 0;
}


