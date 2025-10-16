#include "battle_main.h"
#include "constants/monsters.h"
#include "constants/moves.h"

const Monster gMonstersInfo[] = {
    [MONSTER_WOLF] = {
        .name = "WOLF",
        .HP = 5,
        .speed = 6,
        .move = {MOVE_BITE},
    }
};