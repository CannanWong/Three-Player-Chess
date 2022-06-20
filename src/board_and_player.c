#include "chess.h"

piece_t get_piece(coord_t grid) {
  piece_t *region;
  switch (grid.region_col) {
    case (WHITE){
      region = board.white_region;
    }
    case (BLACK){
      region = board.white_region;
    }
    default {
      assert(grid.region_col = RED);
      region = board.white_region;
    } 
  }
  return board[grid.x][grid.y];
}

coord_t move_vector(bool l_first, coord_t orig, signed short dx, signed short dy) {
  if (l-first) {
    
  }
}