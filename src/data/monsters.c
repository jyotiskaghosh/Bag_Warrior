#include "battle_main.h"
#include "constants/monsters.h"
#include "constants/moves.h"
#include "constants/item.h"

const Monster gMonstersInfo[] = {
    [MONSTER_WOLF] = {
        .name = "WOLF",
        .imageFilename = "graphics/Wolf.png",
        .HP = 5,
        .speed = 5,
        .move = MOVE_BITE,
    },
    [MONSTER_BAT] = {
        .name = "BAT",
        .imageFilename = "graphics/Bat.png",
        .HP = 3,
        .speed = 7,
        .move = MOVE_BITE,
        .resistance = ELEMENT_EARTH | ELEMENT_AIR,
    },
    [MONSTER_EARTH_MAGE] = {
        .name = "EARTH MAGE",
        .imageFilename = "graphics/EarthMage.png",
        .HP = 6,
        .speed = 4,
        .move = MOVE_EARTH_SPELL,
        .resistance = ELEMENT_EARTH | ELEMENT_WATER,
        .weakness = ELEMENT_AIR,
        .item = ITEM_EARTH_SCROLL,
    },
    [MONSTER_AIR_MAGE] = {
        .name = "AIR MAGE",
        .imageFilename = "graphics/AirMage.png",
        .HP = 6,
        .speed = 4,
        .move = MOVE_AIR_SPELL,
        .resistance = ELEMENT_AIR | ELEMENT_EARTH,
        .weakness = ELEMENT_FIRE,
        .item = ITEM_AIR_SCROLL,
    },
    [MONSTER_WATER_MAGE] = {
        .name = "WATER MAGE",
        .imageFilename = "graphics/WaterMage.png",
        .HP = 6,
        .speed = 4,
        .move = MOVE_WATER_SPELL,
        .resistance = ELEMENT_WATER | ELEMENT_FIRE,
        .weakness = ELEMENT_EARTH,
        .item = ITEM_WATER_SCROLL,
    },
    [MONSTER_FIRE_MAGE] = {
        .name = "FIRE MAGE",
        .imageFilename = "graphics/FireMage.png",
        .HP = 6,
        .speed = 4,
        .move = MOVE_FIRE_SPELL,
        .resistance = ELEMENT_FIRE | ELEMENT_AIR,
        .weakness = ELEMENT_WATER,
        .item = ITEM_FIRE_SCROLL,
    },
    [MONSTER_DRAGON] = {
        .name = "DRAGON",
        .imageFilename = "graphics/Dragon.png",
        .HP = 16,
        .speed = 4,
        .move = MOVE_DRAGON_FIRE,
        .resistance = ELEMENT_FIRE,
        .weakness = ELEMENT_WATER,
    },
};