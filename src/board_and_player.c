#include "chess.h"
#define MAX_PIECES 11

const coord_t DEFAULT_COORD = {MAX_X, MAX_Y, &black_player};
board_t board = {{{NULL}},{{NULL}},{{NULL}}};
piece_t *current_piece = NULL;
player_t *current_player = NULL;
bool *moved_index = NULL;
status game_status = GAME;

player_t* adjacent(player_t *self, bool prev) {
  if (self == &black_player) {
    if (prev) {
      return &red_player;
    }
    return &white_player;
  }
  if (self == &white_player) {
    if (prev) {
      return &black_player;
    }
    return &red_player;
  }

  assert (self == &red_player);
  if (prev) {
    return &white_player;      
  }
  return &black_player;
}

player_t* get_player(color col) {
  if (col == BLACK) {
    return &black_player;
  }
  if (col == WHITE) {
    return &white_player;
  }
  printf("next player color: %d\n", col);
  assert (col == RED);
  return &red_player;
}

bool coord_equals(coord_t c1, coord_t c2) {
  return (c1.belongs == c2.belongs && c1.x == c2.x && c1.y == c2.y);
}

piece_t* get_piece(coord_t grid) {
  if (grid.belongs == &black_player) {
    return board.black_region[grid.x][grid.y];
  }
  if (grid.belongs == &white_player) {
    return board.white_region[grid.x][grid.y];
  }
  
  assert (grid.belongs == &red_player);
  return board.red_region[grid.x][grid.y];
}

void set_piece(coord_t grid, piece_t* pc) {
  if (grid.belongs == &black_player) {
    board.black_region[grid.x][grid.y] = pc;
    return;
  }
  if (grid.belongs == &white_player) {
    board.white_region[grid.x][grid.y] = pc;
    return;
  }
  
  assert (grid.belongs == &red_player);
  board.red_region[grid.x][grid.y] = pc;
}


static coord_t move_x(coord_t orig, signed short dx) {
  unsigned short new_x = orig.x + dx;
  if(new_x < 0 || new_x >= MAX_X) { 
    return DEFAULT_COORD;
  }
  coord_t dest = {new_x, orig.y, orig.belongs};
  return dest;
}

static coord_t move_y(coord_t orig, signed short dy) {
  coord_t dest;
  unsigned short new_y = orig.y + dy;
  if (new_y < MAX_Y) {
    dest.y = new_y;
    dest.x = orig.x;
    dest.belongs = orig.belongs;
  } else {
    if (orig.x < MAX_X/2) {
      dest.belongs = adjacent(orig.belongs, true);
    } else {
      dest.belongs = adjacent(orig.belongs, false);
    }
    dest.y = 2 * MAX_Y - 1 - new_y;
    dest.x = MAX_X - 1 - orig.x;
  }
  if (new_y < 0 || new_y >= 2 * MAX_Y) {
    return DEFAULT_COORD;
  }
  return dest;
}

coord_t move_vector(bool x_first, coord_t orig, signed short dx, signed short dy) {
  coord_t dest;
  coord_t temp;
  
  if (x_first) {
    temp = move_x(orig, dx);
    if (coord_equals(temp, DEFAULT_COORD)) {
      return DEFAULT_COORD;
    }
    dest = move_y(temp, dy);
  } else {
    temp = move_y(orig, dy);
    if (coord_equals(temp, DEFAULT_COORD)) {
      return DEFAULT_COORD;
    }
    if (temp.belongs == orig.belongs) {
      dest = move_x(temp, dx);
    } else {
      dest = move_x(temp, -1 * dx);
    }
  }
  return dest;
}

bool* get_moved_index(coord_t grid, piece_t *pc) {
  unsigned short index = MAX_PIECES;
  player_t *pl = get_player(pc -> piece_color);
  if (pl == grid.belongs) {
    if (grid.y == 1) {
      //i-pawn
      index = grid.x;
    } 
    if (grid.y == 0) {
      switch (grid.x)
      {
      case 0: //Rook l
        index = MAX_X;
      case (MAX_X-1): //Rook r
        index = MAX_X+1;
      case 3: //King
        index = MAX_X+2;
      default:
        index = MAX_PIECES; 
      }
    }
  }
  printf(",crsp moved index: %d\n", index);
  if (index < MAX_PIECES) {
    if (pl->has_moved[index]){
      printf("has moved: true\n");
    } else {
      printf("has moved: false\n");
    }
    moved_index = &(pl -> has_moved[index]);
    return moved_index;
  }
  return NULL;
}

bool displaced(coord_t grid) {
  get_moved_index(grid, current_piece);
  return (moved_index == NULL || *moved_index);
}

bool movable(coord_t dest) {
  for(int i = 0; !coord_equals(curr_avail_moves[i], end_of_list); i++) {
    if (coord_equals(dest, curr_avail_moves[i])) {
      return true;
    }
  }
  return false;
}

/*
piece_t* move_piece(coord_t orig, coord_t dest, bool *alt_orig, bool *alt_dest) {
  if (alt_orig != NULL) {
    get_moved_index(orig, current_piece);
    if (moved_index != NULL && !*moved_index) {
      *moved_index = true; //orig
      *alt_orig = true;
    }
  }
  piece_t* attacked = get_piece(dest);
  if (alt_dest != NULL) {
    get_moved_index(dest, attacked);
    if (moved_index != NULL && !*moved_index) {
      *moved_index = true; //dest
      *alt_dest = true;
    }
  }
  set_piece(dest, current_piece);
  set_piece(orig, &default_piece);
  return attacked;
}
*/

piece_t* move_piece(coord_t orig, coord_t dest) {
  get_moved_index(orig, current_piece);
  if (moved_index != NULL && !*moved_index) {
      *moved_index = true;
  }
  piece_t* attacked = get_piece(dest);
  if (attacked != &default_piece) {
    get_moved_index(dest, attacked);
    if (moved_index != NULL && !*moved_index) {
      *moved_index = true;
    }
  }
  set_piece(dest, current_piece);
  if (current_piece->type == &i_pawn_type && dest.belongs != orig.belongs) {
    set_piece(dest, current_player->o_pawn);
  }
  set_piece(orig, &default_piece);
  
  return attacked;
}


/*
piece_t* revert_move(coord_t orig, coord_t dest, bool alt1, bool alt2, piece_t* attacked) {
  set_piece(orig, current_piece);
  set_piece(dest, attacked);
  if (alt1) {
    *get_moved_index(orig, current_piece) = false;
  }
  if (alt2) {
    *get_moved_index(dest, attacked) = false;
  }
  return current_piece;
}
*/

static piece_t* ask_prom() {
  char msg[MSG_SIZE] = {8,0,0};
  send_msg(msg, MSG_SIZE*sizeof(char));
  receive_msg(msg, MSG_SIZE*sizeof(char));

  if (msg[0] == 5) {
    return &default_piece;
  }
  
  switch (msg[2]) {
    case 0: return current_player->o_pawn;
    case 1: return current_player->bishop;
    case 2: return current_player->rook;
    case 3: return current_player->knight;
    default: {
      assert(msg[2] == 4);
      return current_player->queen;
    }
  }
}

char check_prom(coord_t grid) {
  if (grid.y == 0) {
    if (current_piece->piece_color != grid.belongs->player_col) {

      piece_t *replace = ask_prom();
      if (replace != &default_piece) {
        set_piece(grid, replace);
        current_piece = replace;
        return 'p';
      }
      return 5;
    }
  }
  return 'n';
}

void castling(coord_t king_orig, bool left) {
  if (left) {
    coord_t rook_orig = {0, 0, king_orig.belongs};
    coord_t rook_dest = {king_orig.x-1, 0,king_orig.belongs};
    move_piece(rook_orig, rook_dest);
  } else  {
    coord_t rook_orig = {MAX_X-1, 0, king_orig.belongs};
    coord_t rook_dest = {king_orig.x+1,0,king_orig.belongs,};
    move_piece(rook_orig, rook_dest);
  }
}

void next_player() {
  current_player = adjacent(current_player, false);
  printf("next color: %d\n", current_player->player_col);
  char msg[1] = {5};
  send_msg(msg, sizeof(char));
}

