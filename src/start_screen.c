#include "start_screen.h"

#include "constants/items.h"
#include "constants/textures.h"
#include "main.h"
#include "core/task.h"
#include "core/text.h"
#include "core/fade.h"
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
static void Task_FadeToNewGame(int taskId);

static void DrawButton(char *text, Rectangle rec, int fontSize) {
    DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, WHITE);
    DrawText(text, (rec.x + rec.width/2) - MeasureText(text, fontSize)/2, rec.y + rec.height/2 - fontSize/2, fontSize, WHITE);
}

static void DrawStartScreen(void) {
    DrawText(GAME_TITLE, VIRTUAL_WIDTH / 2 - MeasureText(GAME_TITLE, 32) / 2, VIRTUAL_HEIGHT * 1 / 4 - 16, 32, WHITE);
    DrawButton(
        NEW_GAME_TEXT, 
        (Rectangle){VIRTUAL_WIDTH / 2 - BUTTON_W / 2, VIRTUAL_HEIGHT / 2 - BUTTON_H / 2, BUTTON_W, BUTTON_H},
        8
    );
} 

void CB_InitStartScreen(void) {
    // reset
    StopAllTextPrinters();

    gMainCallback = CB_HandleStartScreen;
    CreateTask(Task_StartScreenSelection, 0);
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
        CreateTask(Task_NewGame, 0);
    }

    DrawTexture(gTextures[TEX_CURSOR],  VIRTUAL_WIDTH / 2 - BUTTON_W / 2 - 8, VIRTUAL_HEIGHT / 2 - 4, WHITE);
}

#undef cursor

#define state data[0]

void Task_NewGame(int taskId) {
    gBattlePlayer = (BattlePlayer){10, 10, 5};
    gLevel = 0;
    gGold = 0;
    InitBag();

    DestroyTask(taskId);
    CreateTask(Task_FadeToNewGame, 0);
}

static void Task_FadeToNewGame(int taskId) {
    Transition(CB_NewLevel)
}

static void InitBag(void) {
    EmptyBag();

    AddItem(ITEM_THROWING_KNIFE);
    AddItem(ITEM_THROWING_KNIFE);
    AddItem(ITEM_THROWING_KNIFE);
    AddItem(ITEM_THROWING_KNIFE);
    AddItem(ITEM_BOMB);
    AddItem(ITEM_BOMB);
    AddItem(ITEM_BOMB);
    AddItem(ITEM_POTION);  
    AddItem(ITEM_POTION);
    AddItem(ITEM_POTION);
}
