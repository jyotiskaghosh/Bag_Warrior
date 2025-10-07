#include "item.h"
#include "constants/item.h"
#include "constants/moves.h"

const Item gItemsInfo[] = {
    [ITEM_THROWING_KNIFE] = {
        .name = "THROWING KNIFE",
        .effect = ITEM_EFFECT_MOVE,
        .move = MOVE_THROWING_KNIFE,
        .description = "Throw knife to do damage."
    }
};