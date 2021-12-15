 CC = gcc
 OBJS = chessSystem.o chess_utilities.o tournament.o player.o game.o
 EXEC = chess
 DEBUG = -g
 CFLAGS = -std=c99 -Wall -pedantic-errors -Werror -DNDEBUG

 $(EXEC): $(OBJS)
	$(CC) $(DEBUG) $(CFLAGS) $(OBJS) ./tests/chessSystemTestsExample.c -L. -lmap -o $(EXEC)

chessSystem.o: chessSystem.c chessSystem.h ./mtm_map/map.h chess_utilities.h tournament.h player.h game.h
	$(CC) -c $(CFLAGS) -o chessSystem.o chessSystem.c

chess_utilities.o: chess_utilities.c chess_utilities.h ./mtm_map/map.h chessSystem.h player.h game.h tournament.h
	$(CC) -c $(CFLAGS) chess_utilities.c

game.o: game.c game.h ./mtm_map/map.h chessSystem.h
	$(CC) -c $(CFLAGS) game.c

player.o: player.c player.h
	$(CC) -c $(CFLAGS) player.c

tournament.o: tournament.c tournament.h chess_utilities.h ./mtm_map/map.h chessSystem.h player.h game.h
	$(CC) -c $(CFLAGS) tournament.c

clean:
	rm -f $(OBJS) $(EXEC)