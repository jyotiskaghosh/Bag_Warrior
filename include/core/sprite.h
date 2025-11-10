#pragma once

#include <raylib.h>
#include <stdbool.h>

#define MAX_SPRITES 64

typedef struct {
    int x, y;
    int duration;
    bool hFlip;
    bool vFlip;
} AnimFrameCmd;

typedef struct {
    int type;
    int count;
} AnimLoopCmd;

typedef struct {
    int type;
    int target;
} AnimJumpCmd;

// The first value of this union specifies the type of command.
// If it -2, then it is a jump command. If it is -1, then it is the end of the script.
// Otherwise, it is the x value for a frame command.
typedef union {
    int type;
    AnimFrameCmd frame;
    AnimLoopCmd loop;
    AnimJumpCmd jump;
} AnimCmd;

#define ANIMCMD_FRAME(...) {.frame = {__VA_ARGS__}}
#define ANIMCMD_LOOP(_count) {.loop = {.type = -3, .count = _count}}
#define ANIMCMD_JUMP(_target) {.jump = {.type = -2, .target = _target}}
#define ANIMCMD_END {.type = -1}

struct Sprite;

typedef void (*SpriteCallback)(struct Sprite *);

typedef struct {
    const AnimCmd *const *anims;
    SpriteCallback callback;
    int width, height;
} SpriteTemplate;

typedef struct Sprite {
    Texture2D texture;

    const AnimCmd *const *anims;
    SpriteCallback callback;

    int width, height;

    Rectangle bounds;

    int animNum;
    int animCmdIndex;
    int animDelayCounter;
    bool animPaused;
    int animLoopCounter;

    bool inUse;
    bool animBeginning;
    bool animEnded;

    int priority;
} Sprite;

extern Sprite gSprites[];

void ResetAllSprites(void);
void AnimateSprites(void);
int CreateSprite(const SpriteTemplate *template, const char *filename, Rectangle bounds, int priority);
void DestroySprite(Sprite *sprite);
