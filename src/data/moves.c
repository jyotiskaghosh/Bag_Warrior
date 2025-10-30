#include "battle_main.h"
#include "constants/moves.h"

const Move gMovesInfo[] = {
    [MOVE_BITE] = {
        .name = "BITE",
        .effect = EFFECT_HIT,
        .damage = 3,
        .accuracy = 90,
    },
    [MOVE_THROWING_KNIFE] = {
        .name = "THROW KNIFE",
        .effect = EFFECT_HIT,
        .damage = 3,
        .accuracy = 100,
    },
    [MOVE_HEAL] = {
        .name = "HEAL",
        .effect = EFFECT_HEAL,
        .damage = -10,
    },
    [MOVE_BOMB] = {
        .name = "BOMB",
        .effect = EFFECT_HIT,
        .damage = 5,
        .accuracy = 80,
        .flags = ELEMENT_FIRE
    },
    [MOVE_EARTH_SPELL] = {
        .name = "EARTH SPELL",
        .effect = EFFECT_HIT,
        .damage = 4,
        .accuracy = 100,
        .flags = ELEMENT_EARTH,
    },
    [MOVE_AIR_SPELL] = {
        .name = "AIR SPELL",
        .effect = EFFECT_HIT,
        .damage = 4,
        .accuracy = 100,
        .flags = ELEMENT_AIR,
    },
    [MOVE_WATER_SPELL] = {
        .name = "WATER SPELL",
        .effect = EFFECT_HIT,
        .damage = 4,
        .accuracy = 100,
        .flags = ELEMENT_WATER,
    },
    [MOVE_FIRE_SPELL] = {
        .name = "FIRE SPELL",
        .effect = EFFECT_HIT,
        .damage = 4,
        .accuracy = 100,
        .flags = ELEMENT_FIRE,
    },
    [MOVE_DRAGON_FIRE] = {
        .name = "DRAGON FIRE",
        .effect = EFFECT_HIT,
        .damage = 5,
        .accuracy = 90,
        .flags = ELEMENT_FIRE,
    },
};