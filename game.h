#ifndef GAME_H_
#define GAME_H_

#include <stdbool.h>
#include "chessSystem.h"

/**
 * Game object for storing single game data.
 * 
 * Functions:
 * gameCreate: Allocates a new game.
 * gameDestroy: Frees game from memory.
 * gameGetWinner: returns game`s winner`s id.
 * gameGetPlaytime: returns game`s length in seconds.
 * gameGetFirstPlayer: returns game`s first player`s id.
 * gameGetSecondPlayer: returns game`s second player`s id.
 * gameCopy: returns game copy for map purposes.
 * gameRemovePlayer: remove player from game and updates stats.
 */

typedef struct Game_t *Game;

/**
 * gameCreate: Allocates a new game.
 * @param first_player - player 1 id.
 * @param second_player - player 2 id.
 * @param winner - enum for the winner of the game (FIRST/SECOND/DRAW)
 * @param play_time - play time_played in seconds (bigger than zero)
 * @param result - enum for the function result
 * CHESS_INVALID_ID if the id is not legal - less or equal to 0.
 * CHESS_INVALID_PLAY_TIME play time is not legal - less than 0.
 * CHESS_OUT_OF_MEMORY - allocatione error occured.
 * CHESS_SUCCESS - everything is ok.
 * @return
 * A new game in case of success, NULL if error occured.
 */
Game gameCreate(int first_player, int second_player, Winner winner, int play_time, ChessResult *result);

/**
 * gameDestroy: Frees game from memory.
 * @param game - game to free.
 */
void gameDestroy(Game game);

/**
 * gameGetWinner: returns game`s winner`s id.
 * @param game - The game we wish to get the winner from.
 * @return - Winning player`s id.
 */
int gameGetWinner(Game game);

/**
 * gameGetPlaytime: returns game`s length in seconds.
 * @param game - The game we wish to get the length of.
 * @return - Game`s length in seconds.
 */
int gameGetPlaytime(Game game);

/**
 * gameGetFirstPlayer: returns game`s first player`s id.
 * @param game - The game we wish to get the first player`s id from.
 * @return - first player`s id.
 */
int gameGetFirstPlayer(Game game);

/**
 * gameGetSecondPlayer: returns game`s second player`s id.
 * @param game - The game we wish to get the second player`s id from.
 * @return - second player`s id.
 */
int gameGetSecondPlayer(Game game);

/**
 * gameCopy: returns a copy of the game`s data.
 * @param game - The game we wish to copy.
 * @return - a copy of the game, NULL if failed.
 */
Game gameCopy(Game game);

/**
* gameRemovePlayer: remove player from the game and updates stats for the second player.
* @param game - The game we remove the player from.
* @param player_id - The id of the player we wish to remove.
* @return
* CHESS_NULL_ARGUMENT - one of the arguments is NULL.
* CHESS_INVALID_ID - The id is illegal.
* CHESS_SUCCESS - if function succeed.
*/
ChessResult gameRemovePlayer(Map players, Game game, int* game_id, int player_id);

#endif