#include <stdio.h>

#define L_AXIS_LEN 8
#define N_AXIS_LEN 4 
#define NUM_PIECE_TYPES 6

//Enumerations
extern enum {BLACK = 0, WHITE = 1, RED = 2} color;
extern enum {PAWN,BISHOP,KNIGHT,KING,QUEEN,ROOK} piece;
extern enum {GAME, CHECKMATE = 1, STALEMATE = 0.5, DRAW = 0.3} status;

//Type definiations
typedef struct chess_piece {
  color piece_col;
  piece piece_type;
} piece_t;

typedef struct piece_number_map {
  piece_t pc;
  unsigned short pc_num;
} pc_num_map;

typedef struct chess_board {
  piece_t black_region[L_AXIS_LEN][N_AXIS_LEN];
  piece_t white_region[L_AXIS_LEN][N_AXIS_LEN];
  piece_t red_region[L_AXIS_LEN][N_AXIS_LEN];
} board_t;

typedef struct coordinate {
  color region_col;
  char letter;
  unsigned short number;
} coord_t;

typedef struct chess_player {
  color player_col;
  char *name;
  unsigned short score;
  pc_num_map possession[NUM_PIECE_TYPES];
  bool agree_draw = false;
} player_t;

typedef struct sequence {
 color curr_col;
 struct sequence *next_player;
} seq_t;

//Global variables
extern status game_status;
extern sequence curr_player;
extern unsigned short num_draw;

//Functions
void initialize();
piece_t get_piece(coord_t);
coord_t move_vector(coord_t, signed short, signed short);
coord_t* show_avail_move(coord_t)
bool move_piece(coord_t, coord_t);
pc_num_map attack(piece_t);
bool click_draw(color);
void terminate();
