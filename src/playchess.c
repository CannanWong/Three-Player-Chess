#include "chess.h"

#define NAME_SIZE 16

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

player_t* to_player(unsigned short code) {
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
    char msg[MSG_SIZE] = {3,0,0};
    if (send) {
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

    char names[MSG_SIZE*NAME_SIZE];
    receive_msg(names, MSG_SIZE*NAME_SIZE*sizeof(char));

    char *players[MSG_SIZE];
    for (int i = 0; i < MSG_SIZE; i++) {
        memcpy(players[i], &names[i*NAME_SIZE], NAME_SIZE); 
    }

    init_players(players);
    while (1) {
        init_chess_boards();
        current_player = &white_player;
        while (game_state() == GAME) {
            coord_t orig_grid;
            coord_t dest_grid;   

            while (1) {
                orig_grid = send_recv_coord(true);
                current_piece = get_piece(orig_grid);
                if (check_valid(current_piece)){
                    curr_avail_moves = show_avail_move(orig_grid);
                    send_avail_moves();
                    dest_grid = send_recv_coord(false);

                    if (movable(dest_grid, curr_avail_moves)) {
                        move_piece(orig_grid, dest_grid, NULL, NULL);
                        //check castling
                        if (current_piece->type == &king_type) {
                            signed short dx = dest_grid.x - orig_grid.x;
                            if (dx == -2) {
                                castling(orig_grid, true);
                            }
                            if (dx == 2) {
                                castling(orig_grid, false);
                            }
                        }
                        //check promotion
                        check_prom(dest_grid);
                        break;
                    }                   
                }
            }
            next_player();
        }
        char ctn = 0;
        receive_msg(&ctn, sizeof(char));
        if (ctn == 0) {
            //end game
            break;
        }
        assert (ctn == 1);
        //next round
    }
    
    terminate();
}
