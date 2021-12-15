#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "chess_utilities.h"
#include "tournament.h"
#include "./mtm_map/map.h"

#define NO_TIME 0
#define EMPTY -1

#define FIRST_UPPER_LETTER 'A'
#define LAST_UPPER_LETTER 'Z'
#define FIRST_LOWER_LETTER 'a'
#define LAST_LOWER_LETTER 'z'

static bool tournamentCheckGameExists(Tournament tournament, int first_player, int second_player);
static ChessResult tournamentCheckForAddGame(Tournament tournament, int first_player,
 int second_player, Winner winner, int play_time);
static void tournamentDestroyForAddGame(Player player1, Player player2);
static void tournamentAddPlayerStats(Player player, int play_time, Winner player_id, Winner winner);
static ChessResult tournamentCheckIfUsedOrRemovedPlayers(Tournament tournament, Player player1, Player player2);
static ChessResult tournamentPutNewPlayersForAddGame(Tournament tournament, Player player1, Player player2,
                                                        int first_player, int second_player);
static ChessResult tournamentCreateNewPlayersForAddGame(Tournament tournament, Player* player1, Player* player2,
                                                        int first_player, int second_player);

struct tournament_t
{
    int max_games_per_player;
    char *tournament_location;
    Map games;
    Map players;
    int longest_game_time;
    double avg_game_time;
    int winner_id;
    bool ended;
    int number_of_players;
};

Tournament createTournament(int max_games_per_player, const char *tournament_location, ChessResult *result)
{
    Tournament tournament = malloc(sizeof(*tournament));
    if (tournament == NULL)
    {
        *result = CHESS_OUT_OF_MEMORY;
        return NULL;
    }
    if (!tournamentCheckLegalLocation(tournament_location))
    {
        *result = CHESS_INVALID_LOCATION;
        free(tournament);
        return NULL;
    }
    if (!(max_games_per_player > 0))
    {
        *result = CHESS_INVALID_MAX_GAMES;
        free(tournament);
        return NULL;
    }
    tournament->max_games_per_player = max_games_per_player;
    tournament->tournament_location = malloc(sizeof(char) * (strlen(tournament_location) + 1));
    if (!tournament->tournament_location)
    {
        *result = CHESS_OUT_OF_MEMORY;
        free(tournament);
        return NULL;
    }
    strcpy(tournament->tournament_location, tournament_location);
    tournament->games = mapCreate(copyDataGame, copyKeyInt, freeGame, freeInt, compareInts);
    tournament->players = mapCreate(copyDataPlayer, copyKeyInt, freePlayer, freeInt, compareInts);

    if (strcmp(tournament->tournament_location, tournament_location) != 0 
    || tournament->games == NULL || tournament->players == NULL)
    {
        *result = CHESS_OUT_OF_MEMORY;
        destroyTournament(tournament);
        return NULL;
    }

    tournament->longest_game_time = NO_TIME;
    tournament->avg_game_time = NO_TIME;
    tournament->winner_id = -1;
    tournament->ended = false;
    tournament->number_of_players = 0;
    *result = CHESS_SUCCESS;
    return tournament;
}

bool tournamentCheckGameExists(Tournament tournament, int first_player, int second_player)
{
    MapKeyElement *current;
    Game current_game;
    for (current = mapGetFirst(tournament->games);
         current; current = mapGetNext(tournament->games))
    {
        current_game = mapGet(tournament->games, current);
        if (
        (gameGetFirstPlayer(current_game) == first_player && 
        gameGetSecondPlayer(current_game) == second_player) ||
        (gameGetFirstPlayer(current_game) == second_player &&
         gameGetSecondPlayer(current_game) == first_player))
        {
            free(current);
            return true;
        }
        free(current);
    }
    return false;
}

ChessResult tournamentCheckForAddGame(Tournament tournament, int first_player, int second_player,
                                         Winner winner, int play_time)
{
    if (!tournament || !first_player || !second_player || !play_time)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (winner != FIRST_PLAYER && winner != SECOND_PLAYER && winner != DRAW)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament->ended)
    {
        return CHESS_TOURNAMENT_ENDED;
    }
    if (tournamentCheckGameExists(tournament, first_player, second_player))
    {
        return CHESS_GAME_ALREADY_EXISTS;
    }
    return CHESS_SUCCESS;
}

void tournamentDestroyForAddGame(Player player1, Player player2)
{
    if (player1 != NULL)
    {
        playerDestroy(player1);
    }
    if (player2 != NULL)
    {
        playerDestroy(player2);
    }
}

void tournamentAddPlayerStats(Player player, int play_time, Winner player_id, Winner winner)
{
    playerAddGamesPlayed(player, 1);
    playerAddTimePlayed(player, play_time);
    if (winner == DRAW)
    {
        playerAddDraws(player, 1);
    }
    else if (winner == player_id)
    {
        playerAddWins(player, 1);
    }
    else
    {
        playerAddLoses(player, 1);
    }
}

ChessResult tournamentCheckIfUsedOrRemovedPlayers(Tournament tournament, Player player1, Player player2)
{
    if ((player1 != NULL && playerGetGames(player1) >= tournament->max_games_per_player) ||
        (player2 != NULL && playerGetGames(player2) >= tournament->max_games_per_player))
    {
        return CHESS_EXCEEDED_GAMES;
    }
    if (player1 != NULL)
    {
        if (playerGetGames(player1) == 0)
        {
            tournament->number_of_players++;
        }
    }
    if (player2 != NULL)
    {
        if (playerGetGames(player2) == 0)
        {
            tournament->number_of_players++;
        }
    }
    return CHESS_SUCCESS;
}

ChessResult tournamentPutNewPlayersForAddGame(Tournament tournament, Player player1, Player player2,
 int first_player, int second_player)
{
    MapResult map_result;
    if (playerGetGames(player1) == 1 && playerIfWasRemoved(player1) != true)
    {
        map_result = mapPut(tournament->players,
         (MapKeyElement)(&first_player), (MapDataElement)(player1));
        playerDestroy(player1);
        if (map_result != MAP_SUCCESS)
        {
            tournamentDestroyForAddGame(player1, player2);
            return CHESS_OUT_OF_MEMORY;
        }
    }
    if (playerGetGames(player2) == 1 && playerIfWasRemoved(player2) != true)
    {
        map_result = mapPut(tournament->players,
         (MapKeyElement)(&second_player), (MapDataElement)(player2));
        playerDestroy(player2);
        if (map_result != MAP_SUCCESS)
        {
            tournamentDestroyForAddGame(player1, player2);
            return CHESS_OUT_OF_MEMORY;
        }
    }
    return CHESS_SUCCESS;
}

ChessResult tournamentCreateNewPlayersForAddGame(Tournament tournament, Player* player1,
                                                    Player* player2, int first_player, int second_player)
{
    if (!mapContains(tournament->players, (MapKeyElement)(&first_player)))
    {
        *player1 = playerCreate();
        if (!(*player1))
        {
            return CHESS_OUT_OF_MEMORY;
        }
        tournament->number_of_players++;
    }
    if (!mapContains(tournament->players, (MapKeyElement)(&second_player)))
    {
        *player2 = playerCreate();
        if (!(*player2))
        {
            tournamentDestroyForAddGame(*player1, NULL);
            return CHESS_OUT_OF_MEMORY;
        }
        tournament->number_of_players++;
    }
    return CHESS_SUCCESS;
}

ChessResult tournamentAddGame(Tournament tournament, int first_player,
                                int second_player, Winner winner, int play_time)
{
    ChessResult result = tournamentCheckForAddGame(tournament,
                                                    first_player, second_player, winner, play_time);
    if (result != CHESS_SUCCESS)
    {
        return result;
    }

    Player player1 = mapGet(tournament->players, (MapKeyElement)(&first_player));
    Player player2 = mapGet(tournament->players, (MapKeyElement)(&second_player));
    if(tournamentCheckIfUsedOrRemovedPlayers(tournament,player1,player2) != CHESS_SUCCESS)
    {
        return CHESS_EXCEEDED_GAMES;
    }
    Game game = gameCreate(first_player, second_player, winner, play_time, &result);
    if (!game)
    {
        return result;
    }
    int next_id = mapGetSize(tournament->games) + 1;
    MapResult map_result = mapPut(tournament->games, (MapKeyElement)(&next_id), (MapDataElement)(game));
    gameDestroy(game);
    if (map_result != MAP_SUCCESS)
    {
        return CHESS_OUT_OF_MEMORY;
    }
    tournament->longest_game_time = tournament->longest_game_time > play_time ?
                                     tournament->longest_game_time : play_time;
    int map_size = mapGetSize(tournament->games);
    tournament->avg_game_time = ((tournament->avg_game_time) * (map_size - 1) + play_time) / map_size;
    if(tournamentCreateNewPlayersForAddGame(tournament,&player1,&player2,first_player,second_player) !=
                                            CHESS_SUCCESS)
    {
        return CHESS_OUT_OF_MEMORY;
    }
    tournamentAddPlayerStats(player1, play_time, FIRST_PLAYER, winner);
    tournamentAddPlayerStats(player2, play_time, SECOND_PLAYER, winner);
    if(tournamentPutNewPlayersForAddGame(tournament,player1,player2,first_player,second_player) != 
                                        CHESS_SUCCESS)
    {
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
}

ChessResult endTournament(Tournament tournament)
{
    if (!tournament || tournament->players == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament->ended)
    {
        return CHESS_TOURNAMENT_ENDED;
    }
    int *current = mapGetFirst(tournament->players);
    if(current == NULL)
    {
        return CHESS_NO_GAMES;
    }
    int winner_key = *current;
    Player winner = mapGet(tournament->players, (MapKeyElement)(current));
    if (!winner)
    {
        return CHESS_NULL_ARGUMENT;
    }
    free(current);
    for (current = mapGetNext(tournament->players); current; current = mapGetNext(tournament->players))
    {
        Player temp_player = mapGet(tournament->players, (MapKeyElement)(current));
        int players_compare = comparePlayers(winner, temp_player);
        if (players_compare < 0)
        {
            winner = temp_player;
            winner_key = *current;
        }
        else if (players_compare == 0)
        {
            winner = winner_key < *current ? winner : temp_player;
            winner_key = winner_key < *current ? winner_key : *current;
        }
        free(current);
    }
    if(playerGetGames(winner) == 0)
    {
        return CHESS_NO_GAMES;
    }
    tournament->winner_id = winner_key;
    tournament->ended = true;
    return CHESS_SUCCESS;
}

void destroyTournament(Tournament tournament)
{
    if (tournament != NULL)
    {
        if (tournament->tournament_location != NULL)
        {
            free(tournament->tournament_location);
        }
        if (tournament->games != NULL)
        {
            mapDestroy(tournament->games);
        }
        if (tournament->players != NULL)
        {
            mapDestroy(tournament->players);
        }
        free(tournament);
    }
}

bool tournamentCheckLegalLocation(const char *location)
{
    if(location == NULL)
    {
        return false;
    }
    if (*location < FIRST_UPPER_LETTER || *location > LAST_UPPER_LETTER)
    {
        return false;
    }
    for (location++; *location; location++)
    {
        if (*location != ' ' && (*location < FIRST_LOWER_LETTER || *location > LAST_LOWER_LETTER))
        {
            return false;
        }
    }
    return true;
}

bool tournamentHasEnded(Tournament tournament)
{

    if (!tournament)
    {
        return true;
    }
    return tournament->ended;
}

Tournament copyTournament(Tournament tournament)
{
    if (!tournament)
    {
        return NULL;
    }
    if (!tournament->games || !tournament->players)
    {
        return NULL;
    }
    ChessResult result;
    Tournament new_tournament = createTournament(tournament->max_games_per_player,
                                                tournament->tournament_location, &result);
    if (!new_tournament)
    {
        return NULL;
    }
    new_tournament->longest_game_time = tournament->longest_game_time;
    new_tournament->avg_game_time = tournament->avg_game_time;
    new_tournament->winner_id = tournament->winner_id;
    new_tournament->ended = tournament->ended;
    new_tournament->number_of_players = tournament->number_of_players;
    mapDestroy(new_tournament->games);
    new_tournament->games = mapCopy(tournament->games);
    if (!new_tournament->games)
    {
        destroyTournament(new_tournament);
        return NULL;
    }
    mapDestroy(new_tournament->players);
    new_tournament->players = mapCopy(tournament->players);
    if (!new_tournament->players)
    {
        destroyTournament(new_tournament);
        return NULL;
    }
    return new_tournament;
}

Map tournamentGetPlayersMap(Tournament tournament)
{
    if (tournament == NULL)
    {
        return NULL;
    }
    return tournament->players;
}

Player tournamentGetPlayer(Tournament tournament, int player_id)
{
    if (!tournament)
    {
        return NULL;
    }
    Player player = mapGet(tournament->players, (MapKeyElement)(&player_id));
    return player;
}

ChessResult tournamentRemovePlayer(Tournament tournament, Player player, int player_id)
{
    if (!tournament)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    if (!player)
    {
        return CHESS_NULL_ARGUMENT;
    }
    for (int *current = mapGetFirst(tournament->games); current; current = mapGetNext(tournament->games))
    {
        Game game = mapGet(tournament->games, (MapKeyElement)(current));
        if (!game)
        {
            free(current);
            return CHESS_NULL_ARGUMENT;
        }
        ChessResult result = gameRemovePlayer(tournament->players,game, current, player_id);
        if (result != CHESS_SUCCESS)
        {
            free(current);
            return result;
        }
        free(current);
    }
    playerReset(player);
    return CHESS_SUCCESS;
}

ChessResult printTournamentStatistics(FILE *file, Tournament tournament)
{
    int map_size = mapGetSize(tournament->games);
    int result = fprintf(file, "%d\n%d\n%.2lf\n%s\n%d\n%d\n",
            tournament->winner_id, tournament->longest_game_time,
            tournament->avg_game_time, tournament->tournament_location,
            map_size, tournament->number_of_players);
    if(result < 0)
    {
        return CHESS_SAVE_FAILURE;
    }
    return CHESS_SUCCESS;
}
