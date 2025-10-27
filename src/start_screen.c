#include "start_screen.h"

#include "constants/item.h"
#include "main.h"
#include "core/task.h"
#include "core/text.h"
#include "dungeon.h"
#include "battle_main.h"
#include "item.h"
#include <raylib.h>

#define GAME_TITLE "BAG WARRIOR"
#define NEW_GAME_TEXT "NEW GAME"

#define BUTTON_W 80
#define BUTTON_H 24

static void CB_HandleStartScreen();
static void Task_StartScreenSelection(int taskId);
static void InitBag();

static void DrawStartScreen() {
    DrawText(GAME_TITLE, VIRTUAL_WIDTH / 2 - MeasureText(GAME_TITLE, 32) / 2, VIRTUAL_HEIGHT * 1 / 4 - 16, 32, WHITE);
} 

void CB_InitStartScreen() {
    gMainCallback = CB_HandleStartScreen;
    CreateTask(Task_StartScreenSelection, 0);
    AddTextPrinterDefault(
        NEW_GAME_TEXT,
        (Rectangle){VIRTUAL_WIDTH / 2 - BUTTON_W / 2, VIRTUAL_HEIGHT / 2 - BUTTON_H / 2, BUTTON_W, BUTTON_H},
        0
    );
}

static void CB_HandleStartScreen() {
    DrawStartScreen();
    RunTextPrinters();
    RunTasks();
}

#define cursor data[0]

static void Task_StartScreenSelection(int taskId) {
    if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_ENTER)) {
        DestroyTask(taskId);
        gMainCallback = CB_NewGame;
    }

    DrawText(">", VIRTUAL_WIDTH / 2 - BUTTON_W / 2 - 8, VIRTUAL_HEIGHT / 2 - BUTTON_H / 2, 16, WHITE);
}

#undef cursor

void CB_NewGame() {
    StopAllTextPrinters();
    gBattlePlayer = (BattlePlayer){10, 10, 5};
    InitBag();
    gMainCallback = CB_NewLevel;
    gLevel = 0;
}

static void InitBag() {
    gBag[0] = ITEM_THROWING_KNIFE;
    gBag[1] = ITEM_THROWING_KNIFE;
    gBag[2] = ITEM_THROWING_KNIFE;
    gBag[3] = ITEM_POTION;
    gBag[4] = ITEM_POTION;
}