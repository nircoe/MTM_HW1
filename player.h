#ifndef PLAYER_H_
#define PLAYER_H_

typedef struct player_t *Player;

/**
* playerCreate: Allocates a new player.
* @return 
* NULL if the allocation failed, a new player if suucceed.
*/
Player playerCreate();

/**
*
* playerDestroy: Frees player from memory.
* @param player - player to free.
*/
void playerDestroy(Player player);

/**
* playerAddWins: add function for wins.
* @param player - player to update.
* @param add - number of wins to add.
*/
void playerAddWins(Player player, int add);

/**
* playerAddDraws: add function for draws.
* @param player - player to update.
* @param add - number of draws to add.
*/
void playerAddDraws(Player player, int add);

/**
* playerAddLoses: add function for loses.
* @param player - player to update.
* @param add - number of loses to add.
*/
void playerAddLoses(Player player, int add);

/**
* playerAddGamesPlayed: add function for games played.
* @param player - player to update.
* @param add - number of games played to add.
*/
void playerAddGamesPlayed(Player player, int add);

/**
* playerAddTimePlayed: add function for time played.
* @param player - player to update.
* @param add - number of time played to add.
*/
void playerAddTimePlayed(Player player, int add);

/**
* playerGetWins: getter for wins
* @param player - player to get info from.
* @return 
* wins of the player.
*/
int playerGetWins(Player player);

/**
* playerGetLoses: getter for loses
* @param player - player to get info from.
* @return 
* loses of the player.
*/
int playerGetLoses(Player player);

/**
* playerGetDraws: getter for draws
* @param player - player to get info from.
* @return 
* draws of the player.
*/
int playerGetDraws(Player player);

/**
* playerGetGames: getter for games_played
* @param player - player to get info from.
* @return 
* games_played of the player.
*/
int playerGetGames(Player player);

/**
* playerGetPoints: get player total points.
* @param player - player to get info from.
* @return
* player points calculated by: (2 * WINS) + (1 * DRAWS) + (0 * LOSES).
*/
int playerGetPoints(Player player);

/**
* playerGetLevel: get player level (without dividing by total games played!).
* @param player - player to get info from.
* @return
* player level calculated by: (6 * WINS) - (10 * LOSES) + (2 * DRAWS).
*/
int playerGetLevel(Player player);

/**
* playerGetPlayTime: getter for time_played.
* @param player - player to get info from.
* @return
* player time_played.
*/
int playerGetPlayTime(Player player);

/**
* comparePlayers: compare function between 2 players.
* Compare logic: most points -> least loses -> most wins -> 0 if everything is the same
* @param player1 - first player to compare.
* @param player2 - second player to compare.
* @return
* Positive number if player1 is greater.
* Negative number if player2 is greater.
* 0 if players are equal.
* NO ASSUMPTIONS CAN BE MADE ABOUT THE VALUE RETURNED.
*/
int comparePlayers(Player player1, Player player2);

/**
* copyPlayer: returns a copy of the player.
* @param player - player to copy.
* @return
* player copy is success, NULL if failed.
*/
Player copyPlayer(Player player);

/**
* playerReset: resets playes stats to zero.
* @param player - player to reset.
*/
void playerReset(Player player);

/**
 * playerIfWasRemoved: check if the player was removed
 * @param player - player to check
 * */
bool playerIfWasRemoved(Player player);

#endif   