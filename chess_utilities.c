#include <stdlib.h>
#include "chess_utilities.h"
#include "player.h"
#include "game.h"
#include "tournament.h"

/** Function to be used for copying an int as a key to the map */
MapKeyElement copyKeyInt(MapKeyElement key)
{
    if (!key)
    {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy)
    {
        return NULL;
    }
    *copy = *(int *)key;
    return (MapKeyElement)copy;
}

/** Function to be used by the map for freeing elements */
void freeInt(MapKeyElement key)
{
    free(key);
}

/** Function to be used by the map for comparing elements */
int compareInts(MapKeyElement number_1, MapKeyElement number_2)
{
    return (*(int *)number_1 - *(int *)number_2);
}

/** Function to be used for copying an Game as a data to the map */
MapDataElement copyDataGame(MapDataElement game)
{
    if (!game)
    {
        return NULL;
    }
    Game new_game = gameCopy((Game)game);
    return (MapDataElement)new_game;
}

/** Function to be used by the map for freeing elements */
void freeGame(MapDataElement game)
{
    if (game != NULL)
    {
        gameDestroy((Game)game);
    }
}

/** Function to be used for copying an Player as a data to the map */
MapDataElement copyDataPlayer(MapDataElement player)
{
    if (!player)
    {
        return NULL;
    }
    Player new_player = copyPlayer((Player)player);
    return (MapDataElement)new_player;
}

/** Function to be used by the map for freeing elements */
void freePlayer(MapDataElement player)
{
    if (player != NULL)
    {
        playerDestroy((Player)player);
    }
}

/** Function to be used for copying an tournament as a data to the map */
MapDataElement copyDataTournament(MapDataElement tournament)
{
    if (!tournament)
    {
        return NULL;
    }
    Tournament new_tournament = copyTournament((Tournament)tournament);
    return (MapDataElement)new_tournament;
}

/** Function to be used by the map for freeing elements */
void freeTournament(MapDataElement tournament)
{
    if (tournament != NULL)
    {
        destroyTournament((Tournament)tournament);
    }
}

ChessResult checkMapRemove(MapResult result)
{
    if (result == MAP_NULL_ARGUMENT)
    {
        return CHESS_NULL_ARGUMENT;
    }
    return CHESS_TOURNAMENT_NOT_EXIST;
}