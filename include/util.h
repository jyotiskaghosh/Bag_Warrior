#pragma once

#include "core/music.h"
#include "core/fade.h"

#define Transition(func)                \
    switch (gTasks[taskId].data[0]) {   \
    case 0:                             \
        PlaySound(gSounds[SOUND_FADE]); \
        StartFadeOut(WHITE);            \
        gTasks[taskId].data[0]++;       \
        break;                          \
    case 1:                             \
        if (gFade.active)               \
            return;                     \
        gMainCallback = func;           \
        StartFadeIn(WHITE);             \
        DestroyTask(taskId);            \
        break;                          \
    }
