#pragma once

#include <raylib.h>

#define FRAMES_FOR_FADE 30

typedef enum {
    FADE_OUT,
    FADE_IN
} FadeType;

typedef struct {
    Color color;
    float alpha;
    FadeType type;
    int frameCounter;      
    bool active;
} FadeController;

extern FadeController gFade;

void StartFadeOut(Color color);
void StartFadeIn(Color color);
void RunFade(void);

#define Transition(func)                \
    switch (gTasks[taskId].data[0]) {     \
    case 0:                             \
        StartFadeOut(WHITE);            \
        gTasks[taskId].data[0]++;         \
        break;                          \
    case 1:                             \
        if (gFade.active)               \
            return;                     \
        gMainCallback = func;           \
        StartFadeIn(WHITE);             \
        DestroyTask(taskId);            \
        break;                          \
    }
