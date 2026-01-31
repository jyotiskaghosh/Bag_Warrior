#include "battle_main.h"
#include "constants/moves.h"
#include "constants/textures.h"

const Move gMovesInfo[] = {
    [MOVE_BITE] = {
        .name = "BITE",
        .effect = EFFECT_HIT,
        .damage = 3,
        .accuracy = 90,
    },
    [MOVE_SUCK_BLOOD] = {
        .name = "SUCK BLOOD",
        .effect = EFFECT_ABSORB,
        .damage = 2,
        .accuracy = 90,
    },
    [MOVE_THROWING_KNIFE] = {
        .name = "THROW KNIFE",
        .effect = EFFECT_HIT,
        .damage = 3,
        .accuracy = 100,
        .textureId = TEX_THROW_KNIFE_MOVE
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
        .flags = ELEMENT_FIRE,
        .textureId = TEX_BOMB_MOVE
    },
    [MOVE_EARTH_SPELL] = {
        .name = "EARTH SPELL",
        .effect = EFFECT_HIT,
        .damage = 4,
        .accuracy = 100,
        .flags = ELEMENT_EARTH,
        .textureId = TEX_EARTH_SPELL_MOVE
    },
    [MOVE_AIR_SPELL] = {
        .name = "AIR SPELL",
        .effect = EFFECT_HIT,
        .damage = 4,
        .accuracy = 100,
        .flags = ELEMENT_AIR,
        .textureId = TEX_AIR_SPELL_MOVE
    },
    [MOVE_WATER_SPELL] = {
        .name = "WATER SPELL",
        .effect = EFFECT_HIT,
        .damage = 4,
        .accuracy = 100,
        .flags = ELEMENT_WATER,
        .textureId = TEX_WATER_SPELL_MOVE
    },
    [MOVE_FIRE_SPELL] = {
        .name = "FIRE SPELL",
        .effect = EFFECT_HIT,
        .damage = 4,
        .accuracy = 100,
        .flags = ELEMENT_FIRE,
        .textureId = TEX_FIRE_SPELL_MOVE
    },
    [MOVE_DRAGON_FIRE] = {
        .name = "DRAGON FIRE",
        .effect = EFFECT_HIT,
        .damage = 5,
        .accuracy = 90,
        .flags = ELEMENT_FIRE,
    },
};