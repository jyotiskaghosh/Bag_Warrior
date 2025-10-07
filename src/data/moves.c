#include "battle_main.h"
#include "constants/moves.h"

const Move gMovesInfo[] = {
    [MOVE_BITE] = {
        .name = "BITE",
        .effect = EFFECT_HIT,
        .damage = 3,
        .accuracy = 90,
        .flags = 0
    },
    [MOVE_THROWING_KNIFE] = {
        .name = "THROW KNIFE",
        .effect = EFFECT_HIT,
        .damage = 3,
        .accuracy = 90,
        .flags = 0
    }
};