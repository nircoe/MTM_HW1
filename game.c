#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "./mtm_map/map.h"
#include "game.h"
#include "player.h"

#define NULL_PLAYER -1
#define REDUCE -1
#define ADD 1

static void setNewStatsForPlayerRemove(Game game, Player player, Winner this_player, Winner other_player);

struct Game_t
{
    int first_player;
    int second_player;
    Winner winner;
    int play_time;
};

Game gameCreate(int first_player, int second_player, Winner winner, int play_time, ChessResult *result)
{
    if (first_player <= 0 || second_player <= 0 || first_player == second_player)
    {
        *result = CHESS_INVALID_ID;
        return NULL;
    }
    if (play_time <= 0)
    {
        *result = CHESS_INVALID_PLAY_TIME;
        return NULL;
    }
    Game new_game = malloc(sizeof(*new_game));
    if (new_game == NULL)
    {
        *result = CHESS_OUT_OF_MEMORY;
        return NULL;
    }
    new_game->first_player = first_player;
    new_game->second_player = second_player;
    new_game->winner = winner;
    new_game->play_time = play_time;
    *result = CHESS_SUCCESS;
    return new_game;
}

void gameDestroy(Game game)
{
    free(game);
}

int gameGetWinner(Game game)
{
    return game->winner;
}
int gameGetPlaytime(Game game)
{
    return game->play_time;
}
int gameGetFirstPlayer(Game game)
{
    return game->first_player;
}
int gameGetSecondPlayer(Game game)
{
    return game->second_player;
}

Game gameCopy(Game game)
{
    assert(game != NULL);
    if (game == NULL)
    {
        return NULL;
    }
    ChessResult result;
    Game game_copy = gameCreate(game->first_player, game->second_player,
                                game->winner, game->play_time, &result);
    if (game_copy == NULL)
    {
        return NULL;
    }
    return game_copy;
}

void setNewStatsForPlayerRemove(Game game, Player player, Winner this_player, Winner other_player)
{
    if(playerGetGames(player)!=0)
        {
            if(game->winner==DRAW)
            {
                playerAddDraws(player,REDUCE);
            }   
            else if(game->winner==other_player)
            {
                playerAddLoses(player,REDUCE);
            }
            if(game->winner!=this_player)
            {
                playerAddWins(player,ADD);
            }
        }
        if(other_player == FIRST_PLAYER)
        {
            game->first_player = NULL_PLAYER;
        }
        else if(other_player == SECOND_PLAYER)
        {
            game->second_player = NULL_PLAYER;
        }
        game->winner = this_player;
}

ChessResult gameRemovePlayer(Map players, Game game, int *game_id, int player_id)
{
    if (!game)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if (player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    if (player_id == game->first_player)
    {
        Player player = mapGet(players,(MapKeyElement)(&(game->second_player)));
        if(!player)
        {
            return CHESS_OUT_OF_MEMORY;
        }
        setNewStatsForPlayerRemove(game,player,SECOND_PLAYER,FIRST_PLAYER);
        
    }
    else if (player_id == game->second_player)
    {
        Player player = mapGet(players,(MapKeyElement)(&(game->first_player)));
        if(!player)
        {
            return CHESS_OUT_OF_MEMORY;
        }
        setNewStatsForPlayerRemove(game,player,FIRST_PLAYER,SECOND_PLAYER);
    }
    return CHESS_SUCCESS;
}
