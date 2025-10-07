#pragma once

typedef struct {
    char description[128];
    char name[16];
    int effect;
    int move;
} Item;

extern int gBag[50];