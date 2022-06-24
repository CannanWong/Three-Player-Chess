CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
PROGS   = all

.SUFFIXES: .c .o

.PHONY: all clean

all: tpchess

tpchess: playchess.o chess.o board_and_player.o during_game.o init.o terminate_game.o network.o 

playchess.o: chess.h

chess.o: chess.h

board_and_player.o: chess.h

during_game.o: chess.h

init.o: chess.h

terminate_game.o: chess.h

network.o: chess.h

clean:
	rm -f $(wildcard *.o)
	rm -f tpchess
