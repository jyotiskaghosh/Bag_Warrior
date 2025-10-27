#include "battle_main.h"
#include "constants/monsters.h"
#include "constants/moves.h"
#include "constants/item.h"

const Monster gMonstersInfo[] = {
    [MONSTER_WOLF] = {
        .name = "WOLF",
        .HP = 5,
        .speed = 5,
        .move = {MOVE_BITE},
    },
    [MONSTER_BAT] = {
        .name = "BAT",
        .HP = 3,
        .speed = 7,
        .move = {MOVE_BITE},
    }
};