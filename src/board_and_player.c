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
      region = board.white_region;
    } 
  }
  return board[][]

}