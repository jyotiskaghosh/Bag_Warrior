#pragma once

#include <stdbool.h>

#define MAX_MOVES 4

typedef struct {
    char name[16];
    int effect;
    int damage;
    int accuracy;
    int flags;
} Move;

typedef struct {
    char name[16];
    int move[MAX_MOVES];
    int HP;
    int speed;
    int weakness;
    int resistance;
    int item;
} Monster;

typedef struct {
    Monster *info;
    int HP;
} BattleMonster;

typedef struct {
    int HP, maxHP;
    int speed;
} BattlePlayer;

enum{
    PLAYER,
    OPPONENT
};

extern bool gInBattle;
extern BattleMonster gBattleOpponent;
extern BattlePlayer gBattlePlayer;

void CB_InitBattle();
void CB_HandleBattle();
void Task_ActionSelection(int taskId);
void Task_PlayMove(int taskId);