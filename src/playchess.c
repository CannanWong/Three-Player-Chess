#include "chess.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    char *players[] = {"Andy", "Beth", "Chad"};
    initialize(players);

    while(game_status == GAME) {
        do {
            coord_t curr_grid = get_player_input();
            get_piece(curr_grid);
        } while (current_piece->piece_color != current_player->player_col);
            
        show_avail_move(curr_grid);

        bool ctn = false;
        do {
            coord_t dest = get_player_input();
            if (current_piece->type == &king_type) {
                signed short dx = dest.x - curr_grid.x;
                if (dx == -2) {
                    castling(curr_grid, true);
                    ctn = true;
                }
                if (dx == 2) {
                    castling(curr_grid, false);
                    ctn = true;
                }
            } else {
                ctn = move_piece(curr_grid, dest, false);
            }
        } while (!ctn);

        check_prom(curr_grid);

        update_status();
        next_player();
    }

    end_game();
    return 0;
}


