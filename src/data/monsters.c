#include "battle_main.h"
#include "constants/monsters.h"
#include "constants/moves.h"

const Monster gMonstersInfo[] = {
    [MONSTER_VRKA] = {
        .name = "VRKA",
        .HP = 5,
        .speed = 6,
        .move = {MOVE_BITE},
        .weakness = 0,
        .resistance = 0
    }
};