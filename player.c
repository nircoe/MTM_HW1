#include <stdlib.h>
#include <stdbool.h>
#include "player.h"

#define NULL_PLAYER -1

struct player_t
{
    int wins;
    int draws;
    int loses;
    int games_played;
    int time_played;
    bool removed;
};

Player playerCreate()
{
    Player player = malloc(sizeof(*player));
    if (!player)
    {
        return NULL;
    }
    player->wins = 0;
    player->draws = 0;
    player->loses = 0;
    player->games_played = 0;
    player->time_played = 0;
    player->removed = false;
    return player;
}

void playerDestroy(Player player)
{
    if (player != NULL)
    {
        free(player);
    }
}

void playerAddWins(Player player, int add)
{
    if (!player)
    {
        return;
    }
    player->wins += add;
}

void playerAddDraws(Player player, int add)
{
    if (!player)
    {
        return;
    }
    player->draws += add;
}

void playerAddLoses(Player player, int add)
{
    if (!player)
    {
        return;
    }
    player->loses += add;
}

void playerAddGamesPlayed(Player player, int add)
{
    if (!player)
    {
        return;
    }
    player->games_played += add;
}

void playerAddTimePlayed(Player player, int time_played)
{
    if (!player)
    {
        return;
    }
    player->time_played += time_played;
}

int playerGetGames(Player player)
{
    if (!player)
    {
        return NULL_PLAYER;
    }
    return player->games_played;
}

int playerGetPoints(Player player)
{
    if (!player)
    {
        return NULL_PLAYER;
    }
    return (2 * player->wins) + player->draws;
}

int playerGetLevel(Player player)
{
    if (!player)
    {
        return NULL_PLAYER;
    }
    return ((6 * player->wins) - (10 * player->loses) + (2 * player->draws));
}

int playerGetPlayTime(Player player)
{
    if (!player)
    {
        return NULL_PLAYER;
    }
    return player->time_played;
}

int comparePlayers(Player player1, Player player2)
{
    int player1_points = playerGetPoints(player1);
    int player2_points = playerGetPoints(player2);
    if (player1_points != player2_points)
    {
        return player1_points - player2_points;
    }
    if (player1->loses != player2->loses)
    {
        return player2->loses - player1->loses;
    }
    if (player1->wins != player2->wins)
    {
        return player1->wins - player2->wins;
    }
    return 0;
}

int playerGetWins(Player player)
{
    if (player == NULL)
    {
        return NULL_PLAYER;
    }
    return player->wins;
}
int playerGetLoses(Player player)
{
    if (player == NULL)
    {
        return NULL_PLAYER;
    }
    return player->loses;
}
int playerGetDraws(Player player)
{
    if (player == NULL)
    {
        return NULL_PLAYER;
    }
    return player->draws;
}

Player copyPlayer(Player player)
{
    if (!player)
    {
        return NULL;
    }
    Player new_player = playerCreate();
    if (!new_player)
    {
        return NULL;
    }
    new_player->wins = player->wins;
    new_player->draws = player->draws;
    new_player->loses = player->loses;
    new_player->games_played = player->games_played;
    new_player->time_played = player->time_played;
    return new_player;
}

void playerReset(Player player)
{
    if(!player)
    {
        return;
    }
    player->wins = 0;
    player->draws = 0;
    player->loses = 0;
    player->games_played = 0;
    player->time_played = 0;
    player->removed = true;
}

bool playerIfWasRemoved(Player player)
{
    if(player != NULL)
    {
        return player->removed;
    }
    return true;
}