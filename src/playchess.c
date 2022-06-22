#include "chess.h"
#include <stdlib.h>
#include <stdio.h>

#define MSG_SIZE 3

unsigned short player_num(player_t *pl) {
    if (pl == &black_player) {
        return 0;
    }
    if (pl == &white_player) {
        return 1;
    }
    assert (pl == &red_player);
    return 2;
}

unsigned short to_player(unsigned short code) {
    switch (code) {
        case 0: return &black_player;
        case 1: return &white_player;
        default: {
            assert (code = 2);
            return &red_player;
        }
    }
    return &red_player;
}

coord_t send_recv_coord(bool send) {
    coord_t pos;
    if (send) {
        char msg[MSG_SIZE] = {3,0,0};
        send_msg(msg, MSG_SIZE*sizeof(char));
    }
    receive_msg(msg, MSG_SIZE*sizeof(char));
    pos.x = msg[0];
    pos.y = msg[1];
    pos.belongs = to_player(msg[2]);
    return pos;
}

bool send_avail_moves() {
    char msg[MAX_MOVES*MSG_SIZE];
    for (int i = 0; &curr_avail_moves[i]; i++) {
        coord_t pos = curr_avail_moves[i];
        unsigned short index = MSG_SIZE*i;
        msg[index] = pos.x;
        msg[index+1] = pos.y;
        msg[index+2] = player_num(pos.belongs);
    }
    return send_msg(msg, MAX_MOVES*MSG_SIZE*sizeof(char));
}

bool check_valid(piece_t *pc) {
    return (pc != &default_piece && pc->piece_color == current_player->player_col);
}

int main() {
    char *players[] = {"Andy", "Beth", "Chad"};
    init_players(players);
    while (1) {
        init_chess_boards();
        while (1) {
            coord_t orig_grid;
            coord_t dest_grid;
            
            while (1) {
                bool action = false;
                orig_grid = send_recv_coord(true);
                current_piece = get_piece(orig_grid);
                if (check_valid(current_piece)){
                    while (1) {
                        curr_avail_moves = show_avail_move(curr_grid);
                        send_avail_moves();
                        dest_grid = send_recv_coord(false);

                        if (move_piece(orig_grid, dest_grid, false)) {
                            action = true;
                            break;
                        }else {
                            current_piece = get_piece(dest_grid);
                            if (!check_valid(current_piece)) {
                                break;
                            }
                        }
                        orig_grid = dest_grid;
                    }
                    if (action) {
                        //check castling
                        if (current_piece->type == &king_type) {
                            signed short dx = dest_grid.x - orig_grid.x;
                            if (dx == -2) {
                                castling(true, orig_grid);
                            }
                            if (dx == 2) {
                                castling(false, orig_grid);
                            }
                        }
                        //check promotion
                        check_prom(dest_grid);
                        break;
                    }
                }
            }
            int game_status = game_state();
            switch (game_status)
                {
                case CHECKMATE:
                    current_player->score += CHECKMATE/10;
                    break;
                case STALEMATE:

                    break;
                }

            next_player();
        }
    }

}
