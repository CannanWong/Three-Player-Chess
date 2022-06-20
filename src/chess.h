#include <stdio.h>
#include <stdbool.h>

#define MAX_X 8
#define MAX_Y 4

//Enumerations
typedef enum {BLACK = 0, WHITE = 1, RED = 2} color;
typedef enum {PAWN,BISHOP,KNIGHT,KING,QUEEN,ROOK} piece;
typedef enum {GAME, CHECKMATE = 3, STALEMATE = 2, DRAW = 1} status;

//Type definiations
typedef struct chess_piece {  
  bool single_move;
  color piece_color;
  piece piece_type;
  coord_t current_coord;
  int *(move_vecs[2]);
} piece_t;

typedef struct chess_board {
  piece_t black_region[MAX_X][MAX_Y];
  piece_t white_region[MAX_X][MAX_Y];
  piece_t red_region[MAX_X][MAX_Y];
} board_t;

typedef struct coordinate {
  color region;
  int x;
  int y
} coord_t;

//request draw trigger interrupt
typedef struct chess_player { 
  bool agree_draw;
  color player_col;
  char *name;
  unsigned short score;
  //pc_num_map possession[NUM_PIECE_TYPES];
} player_t;

typedef struct sequence {
 color curr_col;
 struct sequence *next_player;
} seq_t;

//Global variables
extern board_t board;
extern status game_status;
extern seq_t curr_player;
extern unsigned short num_draw;

//Functions
void initialize();
piece_t get_piece(coord_t);
coord_t move_vector(coord_t, signed short, signed short);
coord_t* show_avail_move(coord_t);
bool move_piece(coord_t, coord_t);
bool click_draw(color);
void terminate();

/*
typedef struct piece_number_map {
  piece_t pc;
  unsigned short pc_num;
} pc_num_map;
pc_num_map attack(piece_t);
*/
