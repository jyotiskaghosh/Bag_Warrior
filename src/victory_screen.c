#include "victory_screen.h"

#include "core/task.h"
#include "core/text.h"
#include "main.h"
#include "start_screen.h"
#include "dungeon.h"
#include <raylib.h>
#include <stdio.h>

#define VICTORY_TEXT "YOU ESCAPED THE DUNGEON!"

static void CB_HandleVictoryScreen(void);
static void Task_VictoryScreenSelection(int taskId);

static void DrawVictoryScreen(void) {
    DrawText(VICTORY_TEXT, VIRTUAL_WIDTH / 2 - MeasureText(VICTORY_TEXT, 16) / 2, VIRTUAL_HEIGHT / 2 - 16, 16, WHITE);
    
    char buffer[16];
    sprintf(buffer, "money: %d", gMoney);
    DrawText(buffer, VIRTUAL_WIDTH / 2 - MeasureText(buffer, 16) / 2,  VIRTUAL_HEIGHT / 2 + 16 ,16, WHITE);
} 

void CB_InitVictoryScreen(void) {
    StopAllTextPrinters();
    gMainCallback = CB_HandleVictoryScreen;
    CreateTask(Task_VictoryScreenSelection, 0);
}

static void Task_VictoryScreenSelection(int taskId) {
    if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_ENTER)) {
        DestroyTask(taskId);
        gMainCallback = CB_InitStartScreen;
    }
}

static void CB_HandleVictoryScreen(void) {
    DrawVictoryScreen();
    RunTasks();
}