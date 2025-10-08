#pragma onec

#define ELEMENT_EARTH (1 << 0)
#define ELEMENT_WATER (1 << 1)
#define ELEMENT_WIND (1 << 2)
#define ELEMENT_FIRE (1 << 3)

enum {
    EFFECT_HIT,
    MOVE_EFFECT_COUNT
};

enum {
    MOVE_NONE,
    MOVE_BITE,
    MOVE_THROWING_KNIFE,
    MOVE_COUNT
};