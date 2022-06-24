#include "chess.h"

#define NAME_SIZE 16

char *device_ip = NULL;

unsigned short player_num(player_t *pl) {
    if (pl == &black_player) {
        return BLACK;
    }
    if (pl == &white_player) {
        return WHITE;
    }
    if(pl == &red_player) {
        return RED;
    }
    assert (pl == NULL);
    return 0;
}

player_t* to_player(unsigned short code) {
    switch (code) {
        case BLACK: return &black_player;
        case WHITE: return &white_player;
        default: {
            assert (code = RED);
            return &red_player;
        }
    }
    return &red_player;
}

bool send_avail_moves() {
    char msg[MAX_MOVES*MSG_SIZE];
    for (int i = 0; &curr_avail_moves[i]; i++) {
        coord_t pos = curr_avail_moves[i];
        unsigned short index = MSG_SIZE*i;
        msg[index+1] = pos.x;
        msg[index+2] = pos.y;
        msg[index] = player_num(pos.belongs);
    }
    return send_msg(msg, MAX_MOVES*MSG_SIZE*sizeof(char));
}

bool check_valid(piece_t *pc) {
    return (pc != &default_piece && pc->piece_color == current_player->player_col);
}

int main() {
    //int status = system("\"path\" width=640 height=480 isWindowedMode=true");
    device_ip = get_local_ip();
    if (device_ip == NULL) {
        return 0;
    }
    start_server();
    receive_msg(NULL, NAME_SIZE*sizeof(char));
    char *names[MSG_SIZE] = {"Andy", "Jesh", "Jiaju"};
    init_players(names);
    while (1) {
        init_chess_boards();
        current_player = &white_player;
        player_t *win1 = NULL;
        player_t *win2 = NULL;
        while (1) {
            game_status = game_state(win1, win2);
            //check if game has ended
            if (game_status != GAME) {
                break;
            }

            coord_t orig_grid;
            coord_t dest_grid;   

            while (1) {
                char msg_orig[MSG_SIZE] = {7,0,0};
                send_msg(msg_orig, MSG_SIZE);
                receive_msg(msg_orig, MSG_SIZE);

                if (msg_orig[0] == 5) {
                    game_status = DRAW;
                    break;
                }

                orig_grid.belongs = to_player(msg_orig[0]);
                orig_grid.x = msg_orig[1];
                orig_grid.y = msg_orig[2];

                current_piece = get_piece(orig_grid);
                if (check_valid(current_piece)){
                    curr_avail_moves = show_avail_move(orig_grid);
                    send_avail_moves();

                    char msg_dest[MSG_SIZE] = {7,0,0};
                    receive_msg(msg_dest, MSG_SIZE);

                    if (msg_dest[0] == 5) {
                        game_status = DRAW;
                        break;
                    }

                    dest_grid.belongs = to_player(msg_dest[0]);
                    dest_grid.x = msg_dest[1];
                    dest_grid.y = msg_dest[2];

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
            if (game_status == DRAW) {
                draw();
                break;
            }
            next_player();
        }
        //display game result
        char msg_result[MSG_SIZE];
        if (game_status == CHECKMATE) {
            msg_result[0] = 6;
            msg_result[1] = player_num(win1);
            msg_result[2] = player_num(win2);
        } else {
            msg_result[0] = 5;
            if (game_status == STALEMATE) {
                msg_result[1] = 0;
            } else {
                assert(game_status == DRAW);
                msg_result[1] = 1;
            }
        }
        send_msg(msg_result, MSG_SIZE);

        //display scores
        char bs = black_player.score;
        char ws = white_player.score;
        char rs = red_player.score;
        char msg_score[MSG_SIZE+1] ={1,bs,ws,rs};
        send_msg(msg_score, MSG_SIZE+1);

        //continue next round or quit
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
