#include "item.h"
#include "constants/items.h"
#include "constants/moves.h"

const Item gItemsInfo[] = {
    [ITEM_THROWING_KNIFE] = {
        .name = "THROWING KNIFE",
        .effect = ITEM_EFFECT_MOVE,
        .move = MOVE_THROWING_KNIFE,
        .description = "Throw knife to do damage."
    },
    [ITEM_POTION] = {
        .name = "POTION",
        .effect = ITEM_EFFECT_HEAL,
        .move = MOVE_HEAL,
        .description = "Recover 10 HP."
    },
    [ITEM_BOMB] = {
        .name = "BOMB",
        .effect = ITEM_EFFECT_MOVE,
        .move = MOVE_BOMB,
        .description = "Throw bomb to do damage."
    },
    [ITEM_EARTH_SCROLL] = {
        .name = "EARTH SCROLL",
        .effect = ITEM_EFFECT_MOVE,
        .move = MOVE_EARTH_SPELL,
        .description = "Cast earth spell."
    },
    [ITEM_WATER_SCROLL] = {
        .name = "WATER SCROLL",
        .effect = ITEM_EFFECT_MOVE,
        .move = MOVE_WATER_SPELL,
        .description = "Cast water spell."
    },
    [ITEM_AIR_SCROLL] = {
        .name = "AIR SCROLL",
        .effect = ITEM_EFFECT_MOVE,
        .move = MOVE_AIR_SPELL,
        .description = "Cast air spell."
    },
    [ITEM_FIRE_SCROLL] = {
        .name = "FIRE SCROLL",
        .effect = ITEM_EFFECT_MOVE,
        .move = MOVE_FIRE_SPELL,
        .description = "Cast fire spell."
    },
    [ITEM_ESCAPE_SCROLL] = {
        .name = "ESCAPE SCROLL",
        .effect = ITEM_EFFECT_ESCAPE,
        .description = "Exit the dungeon."
    }
};