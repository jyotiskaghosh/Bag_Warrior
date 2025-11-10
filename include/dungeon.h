#pragma once

#include "main.h"
#include "stdbool.h"

#define MAP_W VIRTUAL_WIDTH / 16 * 3
#define MAP_H VIRTUAL_HEIGHT / 16 * 3
#define ROOM_ROWS 3
#define ROOM_COLS 3

extern int gLevel;
extern int gMoney;

void CB_NewLevel(void);
void CB_LoadDungeon(void);