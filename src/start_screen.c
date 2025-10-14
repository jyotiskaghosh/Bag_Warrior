#include "start_screen.h"

#include "constants/item.h"
#include "main.h"
#include "core/task.h"
#include "dungeon.h"
#include "battle_main.h"
#include "item.h"
#include <raylib.h>

#define GAME_TITLE "BAG WARRIOR"
#define NEW_GAME_TEXT "NEW GAME"

static void CB_HandleStartScreen();
static void Task_StartScreenSelection(int taskId);
static void InitBag();

static void DrawStartScreen() {
    DrawText(GAME_TITLE, VIRTUAL_WIDTH / 2 - MeasureText(GAME_TITLE, 32) / 2, VIRTUAL_HEIGHT * 1 / 4 - 16, 32, WHITE);
    DrawText(NEW_GAME_TEXT, VIRTUAL_WIDTH / 2 - MeasureText(NEW_GAME_TEXT, 16) / 2, VIRTUAL_HEIGHT / 2 - 8, 16, WHITE);
} 

void CB_InitStartScreen() {
    gMainCallback = CB_HandleStartScreen;
    CreateTask(Task_StartScreenSelection, 0);
}

static void CB_HandleStartScreen() {
    DrawStartScreen();
    RunTasks();
}

static void Task_StartScreenSelection(int taskId) {
    if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_ENTER)) {
        DestroyTask(taskId);
        gMainCallback = CB_NewGame;
    }
}

void CB_NewGame() {
    gBattlePlayer = (BattlePlayer){10, 10, 5};
    InitBag();
    gMainCallback = CB_NewLevel;
}

static void InitBag() {
    gBag[0] = ITEM_THROWING_KNIFE;
    gBag[1] = ITEM_THROWING_KNIFE;
    gBag[2] = ITEM_THROWING_KNIFE;
}