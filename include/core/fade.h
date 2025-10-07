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
void RunFade();