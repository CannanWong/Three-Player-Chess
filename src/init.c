#include "chess.h"

#define NUM_OF_PAWNS 8
#define MAX_NAME_SIZE 32

piece_t default_piece = {NO_COLOR, NULL};
piece_type_t i_pawn_type = {true, {{0, 1}, {-1, 1}, {1, 1}, {0, 0}}};
piece_type_t o_pawn_type = {true, {{0, -1}, {-1, -1}, {1, -1}, {0, 0}}};
piece_type_t knight_type = {true, {{1, 2}, {1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {-1, 2}, {-1, -2}}};
piece_type_t king_type = {true, {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
piece_type_t queen_type = {false, {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
piece_type_t rook_type = {false, {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {0, 0}}}; 
piece_type_t bishop_type = {false, {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {0, 0}}};

//agree_draw, player_color, name, score, i_pawn, o_pawn, bishop, rook, knight, queen, king
player_t black_player = {BLACK, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
player_t white_player = {WHITE, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
player_t red_player = {RED, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static void init_player(player_t *current_player, char *name) {
    current_player->name = malloc(MAX_NAME_SIZE);
    current_player->name = name;
    current_player->i_pawn = malloc(sizeof(piece_t));
    current_player->i_pawn->piece_color = current_player->player_col;
    current_player->i_pawn->type = &i_pawn_type;
    current_player->o_pawn = malloc(sizeof(piece_t));
    current_player->o_pawn->piece_color = current_player->player_col;
    current_player->o_pawn->type = &o_pawn_type; 
    current_player->bishop = malloc(sizeof(piece_t));
    current_player->bishop->piece_color = current_player->player_col;  
    current_player->bishop->type = &bishop_type;
    current_player->rook = malloc(sizeof(piece_t)); 
    current_player->rook->piece_color = current_player->player_col; 
    current_player->rook->type = &rook_type;
    current_player->knight = malloc(sizeof(piece_t)); 
    current_player->knight->piece_color = current_player->player_col; 
    current_player->knight->type = &knight_type;
    current_player->queen = malloc(sizeof(piece_t)); 
    current_player->queen->piece_color = current_player->player_col; 
    current_player->queen->type = &queen_type;
    current_player->king = malloc(sizeof(piece_t));  
    current_player->king->piece_color = current_player->player_col; 
    current_player->king->type = &king_type;
    for (int i = 0; i != 11; i++) {
        current_player->has_moved[i] = false;
    }
}

void init_players(char *player_names[NUM_OF_PLAYERS]) {
    init_player(&black_player, player_names[0]);
    init_player(&white_player, player_names[1]);
    init_player(&red_player, player_names[2]);
}

static void init_player_board(piece_t *current_board[MAX_X][MAX_Y], player_t *player) {

    for (int x = 0; x != MAX_X; x++) {
        current_board[x][1] = player->i_pawn;
        for (int y = 2; y < 4; y++) {
            current_board[x][y] = &default_piece;
        }
    } 
    current_board[0][0] = player->rook;
    current_board[7][0] = player->rook;
    current_board[1][0] = player->knight;
    current_board[6][0] = player->knight;
    current_board[2][0] = player->bishop;
    current_board[5][0] = player->bishop;
    current_board[3][0] = player->king;
    current_board[4][0] = player->queen;
}

void init_chess_boards() {
    init_player_board(board.black_region, &black_player);
    init_player_board(board.white_region, &white_player);
    init_player_board(board.red_region, &red_player);
}

/*
void initialize(char *player_names[NUM_OF_PLAYERS]) {
    init_players(player_names);
    init_chess_boards();
}
*/
