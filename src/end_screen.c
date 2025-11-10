#include "end_screen.h"

#include "core/task.h"
#include "core/text.h"
#include "main.h"
#include "start_screen.h"
#include <raylib.h>

#define DIED_TEXT "YOU DIED!"

static void CB_HandleEndScreen(void);
static void Task_EndScreenSelection(int taskId);

static void DrawEndScreen(void) {
    DrawText(DIED_TEXT, VIRTUAL_WIDTH / 2 - MeasureText(DIED_TEXT, 32) / 2, VIRTUAL_HEIGHT / 2 - 16, 32, WHITE);
} 

void CB_InitEndScreen(void) {
    StopAllTextPrinters();
    gMainCallback = CB_HandleEndScreen;
    CreateTask(Task_EndScreenSelection, 0);
}

static void Task_EndScreenSelection(int taskId) {
    if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
        DestroyTask(taskId);
        gMainCallback = CB_InitStartScreen;
    }
}

static void CB_HandleEndScreen(void) {
    DrawEndScreen();
    RunTasks();
}