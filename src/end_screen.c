#include "end_screen.h"

#include "core/task.h"
#include "core/fade.h"
#include "main.h"
#include "start_screen.h"
#include "constants/audio.h"
#include "util.h"
#include <raylib.h>

#define DIED_TEXT "YOU DIED!"

static void CB_HandleEndScreen(void);
static void Task_EndScreenSelection(int taskId);
static void Task_FadeToStartScreen(int taskId);

static void DrawEndScreen(void) {
    DrawText(DIED_TEXT, VIRTUAL_WIDTH / 2 - MeasureText(DIED_TEXT, 32) / 2, VIRTUAL_HEIGHT / 2 - 16, 32, WHITE);
} 

void CB_InitEndScreen(void) {
    PlayMusic(gMusic[MUSIC_END]);

    gMainCallback = CB_HandleEndScreen;
    CreateTask(Task_EndScreenSelection, 0);
}

static void Task_EndScreenSelection(int taskId) {
    if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
        PlaySound(gSounds[SOUND_SELECT]);
    
        DestroyTask(taskId);
        CreateTask(Task_FadeToStartScreen, 0);
    }
}

static void CB_HandleEndScreen(void) {
    DrawEndScreen();
    RunTasks();
}

static void Task_FadeToStartScreen(int taskId) {
    Transition(CB_InitStartScreen)
}