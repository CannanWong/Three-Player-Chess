#include "chess.h"

static void free_player(player_t *player) {
    free(player->i_pawn);
    free(player->o_pawn);
    free(player->bishop);
    free(player->rook);
    free(player->knight);
    free(player->queen);
    free(player->king);
}

static void free_players() {
    free_player(&black_player);
    free_player(&white_player);
    free_player(&red_player);
}

void close_game() {
    
}

void terminate() {
    free_players();
    close_game();
}
