#include "battle_main.h"
#include "constants/monsters.h"
#include "constants/moves.h"
#include "constants/item.h"

const Monster gMonstersInfo[] = {
    [MONSTER_WOLF] = {
        .name = "WOLF",
        .HP = 5,
        .speed = 5,
        .move = MOVE_BITE,
    },
    [MONSTER_BAT] = {
        .name = "BAT",
        .HP = 3,
        .speed = 7,
        .move = MOVE_BITE,
        .resistance = ELEMENT_EARTH,
    },
    [MONSTER_WIZARD_E] = {
        .name = "EARTH WIZARD",
        .HP = 6,
        .speed = 4,
        .move = MOVE_EARTH_SPELL,
        .resistance = ELEMENT_FIRE | ELEMENT_EARTH,
        .weakness = ELEMENT_WATER,
        .item = ITEM_EARTH_SCROLL,
    },
    [MONSTER_WIZARD_A] = {
        .name = "AIR WIZARD",
        .HP = 6,
        .speed = 4,
        .move = MOVE_AIR_SPELL,
        .resistance = ELEMENT_AIR | ELEMENT_EARTH,
        .weakness = ELEMENT_FIRE,
        .item = ITEM_AIR_SCROLL,
    },
    [MONSTER_WIZARD_W] = {
        .name = "WATER WIZARD",
        .HP = 6,
        .speed = 4,
        .move = MOVE_WATER_SPELL,
        .resistance = ELEMENT_WATER | ELEMENT_FIRE,
        .weakness = ELEMENT_AIR,
        .item = ITEM_WATER_SCROLL,
    },
    [MONSTER_WIZARD_F] = {
        .name = "FIRE WIZARD",
        .HP = 6,
        .speed = 4,
        .move = MOVE_FIRE_SPELL,
        .resistance = ELEMENT_FIRE | ELEMENT_AIR,
        .weakness = ELEMENT_EARTH,
        .item = ITEM_FIRE_SCROLL,
    },
    [MONSTER_DRAGON] = {
        .name = "DRAGON",
        .HP = 16,
        .speed = 4,
        .move = MOVE_DRAGON_FIRE,
        .resistance = ELEMENT_FIRE | ELEMENT_AIR,
        .weakness = ELEMENT_WATER,
        .item = ITEM_DRAGON_SCALE,
    },
};