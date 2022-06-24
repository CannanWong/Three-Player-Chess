#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_X 8
#define MAX_Y 4
#define MSG_SIZE 3
#define MAX_MOVES 64
#define NUM_OF_PLAYERS 3
#define TOTAL_NUM_OF_PIECES 16
#define MAX_VEC 8
#define DIMENSION 2

//Enumerations
typedef enum {BLACK = 2, WHITE = 3, RED = 4, NO_COLOR = -1} color;
typedef enum {GAME = 0, CHECKMATE = 10, STALEMATE = 5, DRAW = 3} status;

//=======================================================================================================

//chess pieces

typedef struct piece_type {
  bool single_move;
  //x then y
  short move_vec[MAX_VEC][DIMENSION];
} piece_type_t;

//Type definiations
typedef struct chess_piece {  
  color piece_color;
  piece_type_t* type;
} piece_t;


//global variables
extern piece_type_t i_pawn_type;
extern piece_type_t o_pawn_type; 
extern piece_type_t rook_type;
extern piece_type_t knight_type;
extern piece_type_t king_type;
extern piece_type_t queen_type;
extern piece_type_t bishop_type;
extern piece_t default_piece;

//=======================================================================================================

//player 

//request draw trigger interrupt
typedef struct chess_player { 
  color player_col;
  char *name;
  unsigned int score;
  piece_t *i_pawn;
  piece_t *o_pawn;
  piece_t *bishop;
  piece_t *rook;
  piece_t *knight;
  piece_t *queen;
  piece_t *king;
  bool has_moved[TOTAL_NUM_OF_PIECES];
} player_t;

//global variables
extern player_t black_player;
extern player_t white_player;
extern player_t red_player;

//=======================================================================================================

typedef struct chess_board {
  piece_t *black_region[MAX_X][MAX_Y];
  piece_t *white_region[MAX_X][MAX_Y];
  piece_t *red_region[MAX_X][MAX_Y];
} board_t;

typedef struct coordinate {  
  short x;
  short y;
  player_t *belongs;
} coord_t;

//Global variables
extern board_t board;
extern unsigned short num_draw;
extern const coord_t DEFAULT_COORD;

//======================================================================================================
//Current state variables
extern status game_status;
extern player_t *current_player;
extern piece_t *current_piece;
extern coord_t *curr_avail_moves;
extern bool *moved_index;

//Functions
void init_players(char**);
void init_chess_boards();
player_t* adjacent(player_t*, bool);
piece_t* get_piece(coord_t);
player_t* get_player(color);
bool coord_equals(coord_t, coord_t);
coord_t move_vector(bool, coord_t, signed short, signed short);
coord_t *show_avail_move(coord_t);
bool movable(coord_t, coord_t*);
piece_t* move_piece(coord_t, coord_t, bool*, bool*);
piece_t* revert_move(coord_t, coord_t, bool, bool, piece_t*);

void castling(coord_t, bool);
char check_prom(coord_t);

void start_server();
bool receive_msg(char*, int);
bool send_msg(char*, int);

void terminate();
void next_player();
bool in_check();
bool has_legal_moves();
status draw();
status game_state();
bool displaced(coord_t);
void close_game();

/*
typedef struct piece_number_map {
  piece_t pc;
  unsigned short pc_num;
} pc_num_map;
pc_num_map attack(piece_t);
*/
