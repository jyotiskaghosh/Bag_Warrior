#pragma once

#include "main.h"
#include "stdbool.h"

#define MAP_W VIRTUAL_WIDTH / 16 * 3
#define MAP_H VIRTUAL_HEIGHT / 16 * 3
#define ROOM_ROWS 3
#define ROOM_COLS 3

enum {
    TILE_EMPTY,
    TILE_FLOOR
};

extern int gMap[MAP_H][MAP_W];

void CB_LoadDungeon();