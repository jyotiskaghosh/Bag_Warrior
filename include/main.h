#pragma once

#include <raylib.h>

#define VIRTUAL_WIDTH 320
#define VIRTUAL_HEIGHT 180

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define TILE_SIZE 16

extern void (*gMainCallback)(void);
extern Texture2D gTextures[];