#ifndef MAP_UTILITIES_
#define MAP_UTILITIES_

#include "./mtm_map/map.h"
#include "chessSystem.h"

/** Function to be used for copying an int as a key to the map */
MapKeyElement copyKeyInt(MapKeyElement key);

/** Function to be used by the map for freeing elements */
void freeInt(MapKeyElement key);

/** Function to be used by the map for comparing elements */
int compareInts(MapKeyElement number_1, MapKeyElement number_2);

/** Function to be used for copying an Game as a data to the map */
MapDataElement copyDataGame(MapDataElement game);

/** Function to be used by the map for freeing elements */
void freeGame(MapDataElement game);

/** Function to be used for copying an Player as a data to the map */
MapDataElement copyDataPlayer(MapDataElement player);

/** Function to be used by the map for freeing elements */
void freePlayer(MapDataElement player);

/** Function to be used for copying an tournament as a data to the map */
MapDataElement copyDataTournament(MapDataElement tournament);

/** Function to be used by the map for freeing elements */
void freeTournament(MapDataElement tournament);

/** coverts map enum errors to chess enum errors */
ChessResult checkMapRemove(MapResult result);

#endif /* MAP_UTILITIES_ */