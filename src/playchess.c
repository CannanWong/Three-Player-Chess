#include "chess.h"

#define NAME_SIZE 16

char device_ip[16] = {0};

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
            assert (code == RED);
            return &red_player;
        }
    }
    return &red_player;
}

bool send_avail_moves() {
    char msg[MAX_MOVES*MSG_SIZE+1] = "";
    unsigned int index = 0;
    for (int i = 0; !coord_equals(curr_avail_moves[i], end_of_list); i++) {
        coord_t pos = curr_avail_moves[i];
        printf(" avail %i, %i %i %i\n", i, pos.belongs->player_col, pos.x, pos.y);
        msg[index+1] = pos.x;
        msg[index+2] = pos.y;
        msg[index] = player_num(pos.belongs);
        index += MSG_SIZE;
    }
    msg[index] = 5;
    printf("index:%d\nmessage sent:%s\n",index, msg);
    return send_msg(msg, MAX_MOVES*MSG_SIZE*sizeof(char)+1);
}

bool check_valid(piece_t *pc) {
    bool val = (pc != &default_piece && pc->piece_color == current_player->player_col);
    if (!val) {
        printf("invalid location, please reselect\n");
    }
    return val;
}

int main() {
    //int status = system("\"path\" width=640 height=480 isWindowedMode=true");
    strcpy(device_ip, get_local_ip());
    /*
    if (device_ip[0] == 'o') {
        return 0;
    }
    */
        printf("%s\n", device_ip);
    start_server();
        printf("server started\n");
        char c[20];
    receive_msg(c, 20);
        printf("start signal: %c, %c\n", c[0], c[3]);
    char *names[MSG_SIZE] = {"Andy", "Jesh", "Jiaju"};
    init_players(names);
    while (1) {
        init_chess_boards();
        current_player = &white_player;
        game_status = GAME;
        /*
        player_t *win1 = NULL;
        player_t *win2 = NULL;
        */
        player_t *winner = NULL;
        player_t *checked = NULL;
        while (1) {
            /*game_status = game_state(win1, win2);
            //check if game has ended
            if (game_status != GAME) {
                break;
            }
            */
            coord_t orig_grid;
            coord_t dest_grid;

            while (1) {
                char msg_orig[MSG_SIZE] = {7,0,0};
                send_msg(msg_orig, MSG_SIZE);
                receive_msg(msg_orig, MSG_SIZE);

                if (msg_orig[0] == 5) {
                    game_status = DRAW;
                    printf("--draw--\n");
                    break;
                }
                printf("--origin--: %d,%d,%d\n",msg_orig[0],msg_orig[1],msg_orig[2]);
                orig_grid.belongs = to_player(msg_orig[0]);
                orig_grid.x = msg_orig[1];
                orig_grid.y = msg_orig[2];

                current_piece = get_piece(orig_grid);
                if (check_valid(current_piece)){
                    printf("--valid loc--\n");
                    curr_avail_moves = show_avail_move(orig_grid);
                    send_avail_moves();
                    char msg_dest[MSG_SIZE] = {7,0,0};
                    receive_msg(msg_dest, MSG_SIZE);
                    printf("--user selected move: %s,%d,%d,%d\n", msg_dest, msg_dest[0],msg_dest[1],msg_dest[2]);

                    if (msg_dest[0] == 5) {
                        game_status = DRAW;
                        printf("draw\n");
                        break;
                    }

                    dest_grid.belongs = to_player(msg_dest[0]);
                    dest_grid.x = msg_dest[1];
                    dest_grid.y = msg_dest[2];

                    if (movable(dest_grid)) {
                        printf("--movable--\n");
                        piece_t *attcked = move_piece(orig_grid, dest_grid);
                        if (attcked != &default_piece) {
                            if (attcked->type == &king_type) {
                                game_status = CHECKMATE;
                                printf("Checkmate\n");
                                winner = get_player(current_piece->piece_color);
                                winner->score += CHECKMATE/10;
                                checked = get_player(attcked->piece_color);
                                if (adjacent(winner, true) == checked) {
                                    adjacent(winner, false)->score += STALEMATE/10;
                                } else {
                                    adjacent(winner, true)->score += STALEMATE/10;
                                }
                                break;
                            }
                        }
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
                    printf("--not movable--\n");                   
                }
            }
            if (game_status == DRAW) {
                draw();
                break;
            } 
            if (game_status == CHECKMATE) {
                break;
            }
            next_player();
            printf("===next player===\n");
        }
        //display game result
        char msg_result[MSG_SIZE];
        if (game_status == CHECKMATE) {
            msg_result[0] = 6;
            msg_result[1] = player_num(winner);
            msg_result[2] = player_num(checked);
        } else {
            assert(game_status == DRAW);
            msg_result[0] = 5;
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
