#include "credits.h"

#include "core/task.h"
#include "main.h"
#include "start_screen.h"
#include "constants/audio.h"
#include <raylib.h>

#define CREDIT_TEXT "CREDITS"

#define PROGRAMMER_TEXT "Programmer:"
#define ARTIST_TEXT "Artist:"

#define PROGRAMMER_NAME "Jyotiska Ghosh"
#define ARTIST_NAME "Adam Missi (Estilik04)"

static void CB_HandleCreditScreen(void);
static void Task_CreditScreenSelection(int taskId);

static void DrawEndScreen(void) {
    DrawText(CREDIT_TEXT, VIRTUAL_WIDTH / 2 - MeasureText(CREDIT_TEXT, 32) / 2, VIRTUAL_HEIGHT * 1 / 4 - 16, 32, WHITE);
    
    DrawText(PROGRAMMER_TEXT, VIRTUAL_WIDTH / 16, VIRTUAL_HEIGHT / 2 - 8, 16, WHITE);
    DrawText(PROGRAMMER_NAME, 15 * VIRTUAL_WIDTH / 16 - MeasureText(PROGRAMMER_NAME, 16), VIRTUAL_HEIGHT / 2 - 8, 16, WHITE);

    DrawText(ARTIST_TEXT, VIRTUAL_WIDTH / 16, VIRTUAL_HEIGHT / 2 + 8, 16, WHITE);
    DrawText(ARTIST_NAME, 15 * VIRTUAL_WIDTH / 16 - MeasureText(ARTIST_NAME, 16), VIRTUAL_HEIGHT / 2 + 8, 16, WHITE);
} 

void CB_InitCreditScreen(void) {
    gMainCallback = CB_HandleCreditScreen;
    CreateTask(Task_CreditScreenSelection, 0);
}

static void Task_CreditScreenSelection(int taskId) {
    if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
        PlaySound(gSounds[SOUND_SELECT]);
    
        DestroyTask(taskId);
        gMainCallback = CB_InitStartScreen;
    }
}

static void CB_HandleCreditScreen(void) {
    DrawEndScreen();
    RunTasks();
}