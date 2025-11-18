#include "start_screen.h"

#include "constants/items.h"
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

static void CB_HandleStartScreen(void);
static void Task_StartScreenSelection(int taskId);
static void InitBag(void);

static void DrawStartScreen(void) {
    DrawText(GAME_TITLE, VIRTUAL_WIDTH / 2 - MeasureText(GAME_TITLE, 32) / 2, VIRTUAL_HEIGHT * 1 / 4 - 16, 32, WHITE);
} 

void CB_InitStartScreen(void) {
    // reset
    StopAllTextPrinters();

    gMainCallback = CB_HandleStartScreen;
    CreateTask(Task_StartScreenSelection, 0);
    AddTextPrinterDefault(
        NEW_GAME_TEXT,
        (Rectangle){VIRTUAL_WIDTH / 2 - BUTTON_W / 2, VIRTUAL_HEIGHT / 2 - BUTTON_H / 2, BUTTON_W, BUTTON_H},
        0
    );
}

static void CB_HandleStartScreen(void) {
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

void CB_NewGame(void) {
    gBattlePlayer = (BattlePlayer){10, 10, 5};
    gLevel = 0;
    gGold = 0;
    InitBag();

    gMainCallback = CB_NewLevel;
}

static void InitBag(void) {
    EmptyBag();

    gBag[0] = ITEM_THROWING_KNIFE;
    gBag[1] = ITEM_THROWING_KNIFE;
    gBag[2] = ITEM_THROWING_KNIFE;
    gBag[3] = ITEM_THROWING_KNIFE;
    gBag[4] = ITEM_BOMB;
    gBag[5] = ITEM_BOMB;
    gBag[6] = ITEM_BOMB;
    gBag[7] = ITEM_POTION;
    gBag[8] = ITEM_POTION;
    gBag[9] = ITEM_POTION;
}