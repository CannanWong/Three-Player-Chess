#include "chess.h"
#define MAX_PIECES 11

color adjacent(player_t *self, bool prev) {
  switch (self) {
    case (&black_player): {
      if (prev) {
        return &red_player;
      }
      return &white_player;
    }
    case (&white_player): {
      if (prev) {
        return &black_player;
      }
      return &red_player;
    }
    default: {
      assert (self == &red_player);
      if (prev) {
        return &white_player;
      }
      return &black_player;
    }
  }
}

bool coord_equals(coord_t c1, coord_t c2) {
  return (c1.region == c2.region && c1.x == c2.x && c1.y == c2.y);
}

piece_t*** get_region(color cl) {
   switch (cl){
    case BLACK:
      return board.black_region;
    case WHITE:
      return board.white_region;
    default: {
      assert (cl == RED);
      return board.red_region;
    }
  }
}

piece_t* get_piece(coord_t grid) {
  return (get_region(grid.region))[grid.x][grid.y];
}

void set_piece(coord_t grid, piece_t* pc) {
  (get_region(grid.region))[grid.x][grid.y] = pc;
}

coord_t move_x(coord_t orig, signed short dx, bool* in_boundary) {
  coord_t dest;
  unsigned short new_x = orig.x + dx;
  dest.x = new_x;
  dest.y = orig.y;
  dest.region = orig.region;
  if(new_x < 0 || new_x >= MAX_X) { 
    in_boundary = false;
  }   
  return dest;
}

coord_t move_y(coord_t orig, signed short dy, bool* in_boundary) {
  coord_t dest;
  unsigned short new_y = orig.y + dy;
  if (new_y < MAX_Y) {
    dest.y = new_y;
    dest.x = orig.x;
    dest.region = orig.region;
  } else {
    if (orig.x < MAX_X/2) {
      dest.region = adjacent(orig.region, true);
    } else {
      dest.region = adjacent(orig.region, false);
    }
    dest.y = 2 * MAX_Y - 1 - new_y;
    dest.x = MAX_X - 1 - orig.x;
  }
  if (new_y < 0 || new_y >= 2 * MAX_Y) {
    in_boundary = false;
  }
  return dest;
}

coord_t calc_vector(bool x_first, coord_t orig, signed short dx, signed short dy) {
  coord_t dest;
  bool in_boundary = true;
  if (x_first) {
    dest = move_y(move_x(orig, dx, &in_boundary), dy, &in_boundary);
  } else {
    coord_t temp = move_y(orig, dy, &in_boundary);
    dest = move_x(temp, -1 * dx, &in_boundary);
  }
  if (!in_boundary) {
    dest.x = MAX_X;
    dest.y = MAX_Y;
  }
  return dest;
}

bool click_draw(player_t *pl) {
  pl->agree_draw = true;
  num_draw++;
  return true;
}

bool* get_moved_index(coord_t grid, piece_t *pc) {
  unsigned short index = MAX_PIECES;
  if (pc->owner == grid.belongs) {
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
      case 4: //King
        index = MAX_X+2;
      default:
        index = MAX_PIECES; 
      }
    }
  }
  if (index < MAX_PIECES) {
    return &(pc -> owner -> has_moved[index]);
  }
  return NULL;
}

bool displaced(coord_t grid, piece_t *pc) {
  bool *result = get_moved_index(grid, pc);
  return (result != NULL || *result);
}

bool move_piece(coord_t orig, coord_t dest) {
  coord_t *avail_moves = show_avail_move(coord_t orig);
  piece_t *orig_pc = get_piece(orig);
  for(int i = 0; avail_moves[i]; i++) {
    if (coord_equals(dest, avail_moves[i])) {
      bool *orig_moved = get_moved_index(orig, orig_pc);
      bool *dest_moved = get_moved_index(dest, orig_pc);
      if (orig_moved != NULL) {
        orig_moved = true;
      }
      if (dest_moved != NULL) {
        dest_moved = true;
      }
      set_piece(dest, orig_pc);
      set_piece(orig, &default_piece);
      break;
    }
  }
}

void turn_board() {
  current_player = adjacent(current_player, false); 
}