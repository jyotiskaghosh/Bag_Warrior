#include "core/sprite.h"

typedef void (*AnimFunc)(Sprite *);
typedef void (*AnimCmdFunc)(Sprite *);

static void BeginAnim(Sprite *sprite);
static void ContinueAnim(Sprite *sprite);

static void AnimCmd_frame(Sprite *sprite);
static void AnimCmd_end(Sprite *sprite);
static void AnimCmd_jump(Sprite *sprite);
static void AnimCmd_loop(Sprite *sprite);

static void BeginAnimLoop(Sprite *sprite);
static void ContinueAnimLoop(Sprite *sprite);
static void JumpToTopOfAnimLoop(Sprite *sprite);

Sprite gSprites[MAX_SPRITES];
static int sSpriteOrder[MAX_SPRITES];

static const AnimFunc sAnimFuncs[] = {
    ContinueAnim,
    BeginAnim,
};

static const AnimCmdFunc sAnimCmdFuncs[] = {
    AnimCmd_loop,
    AnimCmd_jump,
    AnimCmd_end,
    AnimCmd_frame,
};

void ResetAllSprites(void) {
    for (int i = 0; i < MAX_SPRITES; i++) {
        gSprites[i] = (Sprite){};
        sSpriteOrder[i] = i;
    }
}

static void AnimateSprite(Sprite *sprite) {
    sAnimFuncs[sprite->animBeginning](sprite);
}

void AnimateSprites(void) {
    for (int i = 0; i < MAX_SPRITES; i++) {
        Sprite *sprite = &gSprites[sSpriteOrder[i]];

        if (sprite->inUse) {
            if (sprite->callback)
                sprite->callback(sprite);

            if (sprite->inUse)
                AnimateSprite(sprite);
        }
    }
}

static void SortSprites(void) {
    // insertion sort
    for (int i = 1; i < MAX_SPRITES; i++) {
        int key = sSpriteOrder[i];

        int j = i - 1;
        while (j >= 0 && gSprites[sSpriteOrder[j]].priority > gSprites[key].priority) {
            sSpriteOrder[j + 1] = sSpriteOrder[j];
            j--;
        }

        sSpriteOrder[j + 1] = key;
    }
}

int CreateSprite(const SpriteTemplate *template, const char *filename, Rectangle bounds, int priority) {
    for (int i = 0; i < MAX_SPRITES; i++)
        if (!gSprites[i].inUse) {
            Sprite *sprite = &gSprites[i];

            *sprite = (Sprite){};

            sprite->inUse = true;
            sprite->animBeginning = true;
            sprite->priority = priority;

            sprite->bounds = bounds;

            sprite->texture = LoadTexture(filename);
            sprite->anims = template->anims;
            sprite->callback = template->callback;
            sprite->width = template->width;
            sprite->height = template->height;

            SortSprites();

            return i;
        }

    return -1;
}

void DestroySprite(Sprite *sprite) {
    if (sprite->inUse){
        UnloadTexture(sprite->texture);
        *sprite = (Sprite){};
    }
}

static void DrawSprite(Sprite *sprite) {
    int x = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.x;
    int y = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.y;
    bool hFlip = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.hFlip;
    bool vFlip = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.vFlip;

    int width = sprite->width;
    int height = sprite->height;

    Rectangle source = {x, y, width, height};

    if (hFlip) 
        source.width *= -1;

    if (vFlip) 
        source.height *= -1;

    DrawTexturePro(
        sprite->texture, 
        source, 
        sprite->bounds,
        (Vector2){sprite->bounds.width / 2, sprite->bounds.height / 2},
        0,
        WHITE
    );
}

static void BeginAnim(Sprite *sprite) {
    sprite->animCmdIndex = 0;
    sprite->animEnded = false;
    sprite->animLoopCounter = 0;

    int type = sprite->anims[sprite->animNum][sprite->animCmdIndex].type;

    if (type >= 0) {
        sprite->animBeginning = false;
        
        int duration = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.duration;
        
        if (duration)
            duration--;

        sprite->animDelayCounter = duration;

        DrawSprite(sprite);
    }
}

static void DecrementAnimDelayCounter(Sprite *sprite) {
    if (!sprite->animPaused)
        sprite->animDelayCounter--;
}

static void ContinueAnim(Sprite *sprite) {
    if (sprite->animDelayCounter) {
        DecrementAnimDelayCounter(sprite);
        DrawSprite(sprite);
    } else if (!sprite->animPaused) {
        sprite->animCmdIndex++;
        int type = sprite->anims[sprite->animNum][sprite->animCmdIndex].type;
        int funcIndex = 3;
        if (type < 0)
            funcIndex = type + 3;
        sAnimCmdFuncs[funcIndex](sprite);
    }
}

static void AnimCmd_frame(Sprite *sprite) {
    int duration = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.duration;
    
    if (duration)
        duration--;

    sprite->animDelayCounter = duration;
 
    DrawSprite(sprite);
}

static void AnimCmd_end(Sprite *sprite) {
    sprite->animCmdIndex--;
    sprite->animEnded = true;
}

static void AnimCmd_jump(Sprite *sprite) {
    sprite->animCmdIndex = sprite->anims[sprite->animNum][sprite->animCmdIndex].jump.target;

    int duration = sprite->anims[sprite->animNum][sprite->animCmdIndex].frame.duration;

    if (duration)
        duration--;

    sprite->animDelayCounter = duration;

    DrawSprite(sprite);
}

static void AnimCmd_loop(Sprite *sprite) {
    if (sprite->animLoopCounter)
        ContinueAnimLoop(sprite);
    else
        BeginAnimLoop(sprite);
}

static void BeginAnimLoop(Sprite *sprite) {
    sprite->animLoopCounter = sprite->anims[sprite->animNum][sprite->animCmdIndex].loop.count;
    JumpToTopOfAnimLoop(sprite);
    ContinueAnim(sprite);
}

static void ContinueAnimLoop(Sprite *sprite) {
    sprite->animLoopCounter--;
    JumpToTopOfAnimLoop(sprite);
    ContinueAnim(sprite);
}

static void JumpToTopOfAnimLoop(Sprite *sprite) {
    if (sprite->animLoopCounter) {
        sprite->animCmdIndex--;

        while (sprite->anims[sprite->animNum][sprite->animCmdIndex - 1].type != -3) {
            if (sprite->animCmdIndex == 0)
                break;
            sprite->animCmdIndex--;
        }

        sprite->animCmdIndex--;
    }
}
