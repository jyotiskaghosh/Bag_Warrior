#pragma once

#define BAG_CAPACITY 40

typedef struct {
    char description[128];
    char name[16];
    int effect;
    int move;
} Item;

extern int gBag[BAG_CAPACITY];

void CB_OpenBag();
int ItemCount();