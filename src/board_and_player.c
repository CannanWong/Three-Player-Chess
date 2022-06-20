#include "chess.h"

color adjacent(color self, bool prev) {
  switch (self) {
    case (BLACK) {
      if (prev) {
        return RED;
      }
      return WHITE;
    }
    case (WHITE) {
      if (prev) {
        return BLACK;
      }
      return RED;
    }
    default {
      assert (self == RED);
      if (prev) {
        return WHITE;
      }
      return BLACK;
    }
  }
}

bool coord_equals(coord_t c1, coord_t c2) {
  return (c1.region == c2.region && c1.x = c2.x && c1.y == c2.y);
}

piece_t* get_piece(coord_t grid) {
  switch (grid.region) {
    case (WHITE){
      return board.white_region[grid.x][grid.y];
    }
    case (BLACK){
      return = board.white_region[grid.x][grid.y];;
    }
    default {
      assert(grid.region = RED);
      return = board.white_region[grid.x][grid.y];;
    } 
  }
}

coord_t move_x(coord_t orig, signed short dx) {
  coord_t dest;
  unsigned short new_x = orig.x + dx;
  assert(0 <= new_x && new_x < MAX_X);
  dest.x = new_x;
  dest.y = orig.y;
  dest.region = orig.region;
  return dest;    
}

coord_t move_y(coord_t orig, signed short dy) {
  coord_t dest;
  unsigned short new_y = orig.y + dy;
  assert(0 <= new_y && new_y < 2 * MAX_Y);
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
  return dest;
}

coord_t move_vector(bool x_first, coord_t orig, signed short dx, signed short dy) {
  coord_t dest;
  if (x-first) {
    dest = move_y(move_x(orig, dx), dy);
  } else {
    coord_t temp = move_y(orig, dy);
    dest = move_x(temp, -1 * dx);
  }
  return dest;
}

bool click_draw(player_t *pl) {
  pl->agree_draw = true;
  num_draw++;
  return true;
}

bool move_piece(coord_t orig, coord_t dest) {

  for(int i = 0; show_avail_move(coord_t orig)[i]; i++) {
    
  }
}