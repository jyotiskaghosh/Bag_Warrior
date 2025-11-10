#include "core/fade.h"
#include "main.h"

FadeController gFade;

void StartFadeOut(Color color) {
    gFade.color = color;
    gFade.type = FADE_OUT;
    gFade.alpha = 0.0f;
    gFade.frameCounter = 0;
    gFade.active = true;
}

void StartFadeIn(Color color) {
    gFade.color = color;
    gFade.type = FADE_IN;
    gFade.alpha = 1.0f;
    gFade.frameCounter = 0;
    gFade.active = true;
}

void RunFade(void) {
    if (!gFade.active) return;

    gFade.frameCounter++;

    if (gFade.type == FADE_OUT) {
        gFade.alpha = (float) gFade.frameCounter / FRAMES_FOR_FADE;
        if (gFade.alpha >= 1.0f) {
            gFade.alpha = 1.0f;
            gFade.active = false;
        }
    }
    else if (gFade.type == FADE_IN) {
        gFade.alpha = 1.0 - (float) gFade.frameCounter / FRAMES_FOR_FADE;
        if (gFade.alpha <= 0.0f) {
            gFade.alpha = 0.0f;
            gFade.active = false;
        }
    }

    if (gFade.active)
        DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Fade(gFade.color, gFade.alpha));
}

