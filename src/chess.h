#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_X 8
#define MAX_Y 4
#define NUM_OF_PLAYERS 3
#define TOTAL_NUM_OF_PIECES 16

//Enumerations
typedef enum {BLACK = 0, WHITE = 1, RED = 2} color;
typedef enum {GAME = 0, CHECKMATE = 10, STALEMATE = 5, DRAW = 3} status;

//=======================================================================================================

//chess pieces

typedef struct piece_type {
  bool single_move;
  //x then y
  short *(move_vec[2]);
} piece_type_t;

//Type definiations
typedef struct chess_piece {  
  color piece_color;  
  piece_type_t* type;
} piece_t;


//global variables
extern const piece_type_t i_pawn_type;
extern const piece_type_t o_pawn_type; 
extern const piece_type_t rook_type;
extern const piece_type_t knight_type;
extern const piece_type_t king_type;
extern const piece_type_t queen_type;
extern const piece_type_t bishop_type;
extern const piece_t default_piece;

//=======================================================================================================

//player 

//request draw trigger interrupt
typedef struct chess_player { 
  bool agree_draw;
  color player_col;
  char *name;
  unsigned short score;
  piece_t *i_pawn;
  piece_t *o_pawn;
  piece_t *bishop;
  piece_t *rook;
  piece_t *knight;
  piece_t *queen;
  piece_t *king;
  bool has_moved[TOTAL_NUM_OF_PIECES];
} player_t;

typedef struct sequence {
 player_t *current_player;
 struct sequence *next_player;
} seq_t;

//global variables
extern player_t black_player;
extern player_t white_player;
extern player_t red_player;
extern seq_t *curr_player;

//=======================================================================================================

typedef struct chess_board {
  piece_t *black_region[MAX_X][MAX_Y];
  piece_t *white_region[MAX_X][MAX_Y];
  piece_t *red_region[MAX_X][MAX_Y];
} board_t;

typedef struct coordinate {
  color region;
  short x;
  short y;
} coord_t;

//Global variables
extern board_t board;
extern status game_status;
extern unsigned short num_draw;



//Functions
void initialize(char*[NUM_OF_PLAYERS]);
piece_t *get_piece(coord_t);
bool coord_equals(coord_t, coord_t);
coord_t move_vector(bool, coord_t, signed short, signed short);
coord_t *show_avail_move(coord_t);
bool move_piece(coord_t, coord_t);
bool click_draw(player_t*);
void terminate();
void free_board();

/*
typedef struct piece_number_map {
  piece_t pc;
  unsigned short pc_num;
} pc_num_map;
pc_num_map attack(piece_t);
*/
