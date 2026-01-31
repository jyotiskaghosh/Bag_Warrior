#pragma once

#include <stdbool.h>

typedef struct {
    char name[16];
    int effect;
    int damage;
    int accuracy;
    int flags;
    int textureId;
} Move;

typedef struct {
    char name[16];
    int textureId;
    int move;
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

enum {
    PLAYER,
    OPPONENT
};

extern bool gInBattle;
extern BattleMonster gBattleOpponent;
extern BattlePlayer gBattlePlayer;

void CB_InitBattle(void);
void CB_HandleBattle(void);
void Task_ActionSelection(int taskId);
void Task_PlayMove(int taskId);