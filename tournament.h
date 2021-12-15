#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <stdio.h>
#include "chessSystem.h"
#include "player.h"
#include "game.h"

typedef struct tournament_t *Tournament;

/** 
 * createTournament: create a new tournament.
 * @param max_games_per_player - The max number of games_played a single player can play in the tournament.
 * @param tournament_location - where the game happened ("London" ect...).
 * @param result - enum for the function status.
 * @return - A new tournament if successful, NULL if failed.
*/
Tournament createTournament(int max_games_per_player, const char *tournament_location, ChessResult *result);

/** 
 * tournamentAddGame: Adds a new game to the game map of the tournament.
 * @param tournament - the tournament we wish to add a game to.
 * @param first_player - player 1 id.
 * @param second_player - player 2 id.
 * @param winner - enum for the winner of the game (FIRST/SECOND/DRAW)
 * @param play_time - play time_played in seconds (bigger than zero)
 * @return 
 *      CHESS_SUCCESS if everything went OK.
 *      CHESS_OUT_OF_MEMORY if the memory allocation failed.
 *      CHESS_INVALID_MAX_GAMES if max_games_per_player is less than 0
 *      CHESS_INVALID_LOCATION tournament_location is illegal (see tournamentCheckLegalLocation())
 */
ChessResult tournamentAddGame(Tournament tournament, int first_player, int second_player,
                                 Winner winner, int play_time);

/** 
 * endTournament: Calculate the tournament winner and ends the tournament.
 * @param tournament - the tournament we wish to end.
 * @return 
 * CHESS_SUCCESS if everything wenk OK.
 * CHESS_OUT_OF_MEMORY if the memory allocation failed.
 * CHESS_NULL_ARGUMENT if NULL was sent as one of the arguments.
 * CHESS_EXCEEDED_GAMES if one of the players exceeded its max games to play.
 * CHESS_INVALID_ID if the id is not legal - less or equal to 0.
 * CHESS_INVALID_PLAY_TIME play time is not legal - less than 0.
 * CHESS_TOURNAMENT_ENDED if the tournament has ended.
 * CHESS_GAME_ALREADY_EXISTS if a game between the two players already exists.
 */
ChessResult endTournament(Tournament tournament);

/**
* destroyTournament: free tournament and its data.
* @param tournament - tournament to free.
*/
void destroyTournament(Tournament tournament);

/**
* tournamentCheckLegalLocation: input validation for location.
* validation logic: First capital letter + only non-capital english letters and spaces after.
* @param location - location char* to validate.
* @return true if legal, false if not.
*/
bool tournamentCheckLegalLocation(const char *location);

/**
* tournamentHasEnded: check if tournament has ended.
* @param tournament - tournament to check.
* @return true if ended, false if not.
*/
bool tournamentHasEnded(Tournament tournament);

/**
* copyTournament: copies a tournament and its data.
* @param tournament - tournament to copy.
* @return
* tournament copy if succeed, NULL if failed.
*/
Tournament copyTournament(Tournament tournament);

/**
* tournamentGetPlayersMap: returns players map of the tournament.
* @param tournament - tournament to get the map from.
* @return
* players map. NULL if tournament is NULL.
*/
Map tournamentGetPlayersMap(Tournament tournament);

/**
* tournamentGetPlayer: get player from players map.
* @param tournament - tournament to get info from.
* @param player_id - wanted player`s id.
* @return
* NULL if player or tournament don`t exist. Player object (not a copy) if success.
*/
Player tournamentGetPlayer(Tournament tournament, int player_id);

/**
* tournamentRemovePlayer: clears player from players map.
* @param tournament - tournament to remove info from.
* @param player_id - wanted player`s id.
* @return
* CHESS_NULL_ARGUMENT if one of the agruments are NULL
* CHESS_INVALID_ID if illegal id.
* CHESS_SUCCESS if ok.
* CHESS_PLAYER_NOT_EXIST if the player`s id doesnt exists in the players map.
*/
ChessResult tournamentRemovePlayer(Tournament tournament, Player player, int player_id);

/**
* printTournamentStatistics: print tournament statistics to a file.
* @param file - file to write to.
* @param tournament - tournament to print its statistics.
*/
ChessResult printTournamentStatistics(FILE *file, Tournament tournament);

#endif