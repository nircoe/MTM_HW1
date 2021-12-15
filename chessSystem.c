#include <stdio.h>
#include <stdlib.h>

#include "./mtm_map/map.h"
#include "chess_utilities.h"
#include "chessSystem.h"
#include "tournament.h"
#include "player.h"
#include "game.h"

#define FAIL -1
#define REMOVED_PLAYER -1
#define WRITING_MODE "w"

static void swap_int(int *element1, int *element2);
static void swap_double(double *element1, double *element2);
static bool bubble(double levels[], int ids[], int size);
static void bubble_sort(double levels[], int ids[], int size);
static Map chessFullPlayerMapCreate(ChessSystem chess, ChessResult *result);
static ChessResult chessPrintPlayersLevels(Map full_players_data, FILE *file,
                                             int *ids_array, double *levels_array, int size);

struct chess_system_t
{
    Map tournaments;
};

ChessSystem chessCreate()
{
    ChessSystem chess = malloc(sizeof(*chess));
    if (chess == NULL)
    {
        return NULL;
    }
    chess->tournaments = mapCreate(copyDataTournament, copyKeyInt, freeTournament, freeInt, compareInts);
    return chess;
}

void chessDestroy(ChessSystem chess)
{
    if (chess != NULL)
    {
        mapDestroy(chess->tournaments);
        free(chess);
    }
}

ChessResult chessAddTournament(ChessSystem chess, int tournament_id,
                               int max_games_per_player, const char *tournament_location)
{
    if (tournament_location == NULL || chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (!(tournament_id > 0))
    {
        return CHESS_INVALID_ID;
    }
    if (mapContains(chess->tournaments, (MapKeyElement)(&tournament_id)))
    {
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    ChessResult result;
    Tournament temp_tournament = createTournament(max_games_per_player, tournament_location, &result);
    if (result == CHESS_SUCCESS)
    {
        MapResult map_result = mapPut(chess->tournaments, (MapKeyElement)(&tournament_id),
                                     (MapDataElement)temp_tournament);
        destroyTournament(temp_tournament);
        if (map_result == MAP_OUT_OF_MEMORY)
        {
            return CHESS_OUT_OF_MEMORY;
        }
    }
    return result;
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    if (tournament_id <= 0 || first_player <= 0 || second_player <= 0 || first_player == second_player)
    {
        return CHESS_INVALID_ID;
    }
    Tournament current_tournament = mapGet(chess->tournaments, (MapKeyElement)(&tournament_id));
    if (current_tournament == NULL)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    if (tournamentHasEnded(current_tournament))
    {
        return CHESS_TOURNAMENT_ENDED;
    }
    ChessResult result = tournamentAddGame(current_tournament, first_player, second_player, winner, play_time);
    return result;
}

ChessResult chessRemoveTournament(ChessSystem chess, int tournament_id)
{
    if (tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments, (MapKeyElement)(&tournament_id)))
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    MapResult result = mapRemove(chess->tournaments, (MapKeyElement)(&tournament_id));
    if (result != MAP_SUCCESS)
    {
        return checkMapRemove(result);
    }
    return CHESS_SUCCESS;
}

ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    bool player_exist = false;
    if (!chess)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    for (int *current = mapGetFirst(chess->tournaments); current; current = mapGetNext(chess->tournaments))
    {
        Tournament tournament = mapGet(chess->tournaments, (MapKeyElement)(current));
        if (!tournament)
        {
            free(current);
            return CHESS_TOURNAMENT_NOT_EXIST;
        }
        Map players = tournamentGetPlayersMap(tournament);
        if (mapContains(players, (MapKeyElement)(&player_id)))
        {
            Player player = mapGet(players, (MapKeyElement)(&player_id));
            if (playerGetGames(player) == 0)
            {
                free(current);
                continue;
            }
            player_exist = true;
            if (tournamentHasEnded(tournament))
            {   
                playerReset(player);
                free(current);
                continue;
            }
            ChessResult result = tournamentRemovePlayer(tournament,player, player_id);
            if (result != CHESS_SUCCESS)
            {
                free(current);
                if (result == CHESS_PLAYER_NOT_EXIST)
                {
                    continue;
                }
                return result;
            }
        }
        else
        {
            free(current);
            continue;
        }
        free(current);
    }
    if (player_exist == false)
    {
        return CHESS_PLAYER_NOT_EXIST;
    }
    return CHESS_SUCCESS;
}

ChessResult chessEndTournament(ChessSystem chess, int tournament_id)
{
    if (!chess)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    Tournament tournament = mapGet(chess->tournaments, (MapKeyElement)(&tournament_id));
    if (!tournament)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    return endTournament(tournament);
}

double chessCalculateAveragePlayTime(ChessSystem chess, int player_id, ChessResult *chess_result)
{
    if(chess == NULL)
    {
        *chess_result = CHESS_NULL_ARGUMENT;
        return FAIL;
    }
    if (player_id <= 0)
    {
        *chess_result = CHESS_INVALID_ID;
        return FAIL;
    }
    int sum_time = 0, sum_games = 0;
    for (int *current = mapGetFirst(chess->tournaments); current; current = mapGetNext(chess->tournaments))
    {
        Tournament tournament = mapGet(chess->tournaments, (MapKeyElement)(current));
        if (!tournament)
        {
            *chess_result = CHESS_TOURNAMENT_NOT_EXIST;
            free(current);
            return FAIL;
        }
        Player player = tournamentGetPlayer(tournament, player_id);
        if (!player)
        {
            free(current);
            continue;
        }
        sum_time += playerGetPlayTime(player);
        sum_games += playerGetGames(player);
        free(current);
    }
    if (sum_games == 0)
    {
        *chess_result = CHESS_PLAYER_NOT_EXIST;
        return REMOVED_PLAYER;
    }
    *chess_result = CHESS_SUCCESS;
    return (double)sum_time / (double)sum_games;
}

void swap_int(int *element1, int *element2)
{
    int temp = *element1;
    *element1 = *element2;
    *element2 = temp;
}

void swap_double(double *element1, double *element2)
{
    double temp = *element1;
    *element1 = *element2;
    *element2 = temp;
}

bool bubble(double levels[], int ids[], int size)
{
    bool swapped = false;
    for (int i = 1; i < size; i++)
        if ((levels[i - 1] < levels[i]) || ((levels[i - 1] == levels[i]) && (ids[i - 1] > ids[i])))
        {
            swap_double(&levels[i], &levels[i - 1]);
            swap_int(&ids[i], &ids[i - 1]);
            swapped = true;
        }
    return swapped;
}

void bubble_sort(double levels[], int ids[], int size)
{
    bool not_sorted = true;
    while (size > 1 && not_sorted)
    {
        not_sorted = bubble(levels, ids, size);
        size--;
    }
}

Map chessFullPlayerMapCreate(ChessSystem chess, ChessResult* result)
{
    Map full_players_data = mapCreate(copyDataPlayer, copyKeyInt, freePlayer, freeInt, compareInts);
    for (MapKeyElement current_tournament_key = mapGetFirst(chess->tournaments);
         current_tournament_key; current_tournament_key = mapGetNext(chess->tournaments))
    {
        Tournament current_tournament = mapGet(chess->tournaments, current_tournament_key);
        if (current_tournament != NULL)
        {
            Map current_players_map = tournamentGetPlayersMap(current_tournament);
            for (MapKeyElement current_player_key = mapGetFirst(current_players_map);
                 current_player_key; current_player_key = mapGetNext(current_players_map))
            {
                Player current_player = mapGet(current_players_map, current_player_key);
                if (!mapContains(full_players_data, current_player_key))
                {
                    if (mapPut(full_players_data, current_player_key, (MapDataElement)(current_player)) !=
                            MAP_SUCCESS)
                    {
                        free(current_tournament_key);
                        free(current_player_key);
                        mapDestroy(full_players_data);
                        *result = CHESS_SAVE_FAILURE;
                        return NULL;
                    }
                }
                else
                {
                    Player current_full_player = mapGet(full_players_data, current_player_key);
                    playerAddWins(current_full_player, playerGetWins(current_player));
                    playerAddLoses(current_full_player, playerGetLoses(current_player));
                    playerAddDraws(current_full_player, playerGetDraws(current_player));
                    playerAddGamesPlayed(current_full_player, playerGetGames(current_player));
                    playerAddTimePlayed(current_full_player, playerGetPlayTime(current_player));
                }
                free(current_player_key);
            }
        }
        free(current_tournament_key);
    }
    return full_players_data;
}

ChessResult chessPrintPlayersLevels(Map full_players_data, FILE *file, int* ids_array, double* levels_array, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (levels_array[i] != REMOVED_PLAYER)
        {
            int result = fprintf(file, "%d %.2lf\n", ids_array[i], levels_array[i]);
            if (result < 0)
            {
                free(levels_array);
                free(ids_array);
                mapDestroy(full_players_data);
                return CHESS_SAVE_FAILURE;
            }
        }
    }
    return CHESS_SUCCESS;
}

ChessResult chessSavePlayersLevels(ChessSystem chess, FILE *file)
{
    if (chess == NULL || file == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }
    ChessResult result;
    Map full_players_data = chessFullPlayerMapCreate(chess,&result);
    if(!full_players_data)
    {
        return result;
    }
    int size = mapGetSize(full_players_data), i = 0;
    double *levels_array = malloc(sizeof(*levels_array) * size);
    int *ids_array = malloc(sizeof(*ids_array) * size);
    if (!levels_array)
    {
        mapDestroy(full_players_data);
        return CHESS_SAVE_FAILURE;
    }
    if (!ids_array)
    {
        free(levels_array);
        mapDestroy(full_players_data);
        return CHESS_SAVE_FAILURE;
    }
    for (MapKeyElement current_player_key = mapGetFirst(full_players_data);
         current_player_key; current_player_key = mapGetNext(full_players_data))
    {
        Player current_player = mapGet(full_players_data, current_player_key);
        if (current_player == NULL)
        {
            free(levels_array);
            free(ids_array);
            free(current_player_key);
            mapDestroy(full_players_data);
            return CHESS_SAVE_FAILURE;
        }
        int player_games = playerGetGames(current_player);
        if (player_games == 0)
        {
            levels_array[i] = REMOVED_PLAYER;
        }
        else
        {
            levels_array[i] = (double)playerGetLevel(current_player) / (double)player_games;
        }
        ids_array[i] = *((int *)current_player_key);
        i++;
        free(current_player_key);
    }
    bubble_sort(levels_array, ids_array, size);
    result = chessPrintPlayersLevels(full_players_data,file,ids_array,levels_array,size);
    if(result != CHESS_SUCCESS)
    {
        return result;
    }
    free(levels_array);
    free(ids_array);
    mapDestroy(full_players_data);
    return CHESS_SUCCESS;
}

ChessResult chessSaveTournamentStatistics(ChessSystem chess, char *path_file)
{
    bool no_tournaments_ended = true;
    if (!chess || !path_file)
    {
        return CHESS_NULL_ARGUMENT;
    }
    FILE *file = fopen(path_file, WRITING_MODE);
    if (!file)
    {
        return CHESS_SAVE_FAILURE;
    }
    for (int *current = mapGetFirst(chess->tournaments); current; current = mapGetNext(chess->tournaments))
    {
        Tournament tournament = mapGet(chess->tournaments, (MapKeyElement)(current));
        if (!tournament)
        {
            free(current);
            fclose(file);
            return CHESS_SAVE_FAILURE;
        }
        if (!tournamentHasEnded(tournament))
        {
            free(current);
            continue;
        }
        no_tournaments_ended = false;
        ChessResult result = printTournamentStatistics(file, tournament);
        if (result != CHESS_SUCCESS)
        {
            free(current);
            fclose(file);
            return CHESS_SAVE_FAILURE;
        }
        free(current);
    }
    fclose(file);
    if (no_tournaments_ended)
    {
        return CHESS_NO_TOURNAMENTS_ENDED;
    }
    return CHESS_SUCCESS;
}
