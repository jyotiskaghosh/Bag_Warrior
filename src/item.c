#include "item.h"

#include "main.h"
#include "battle_main.h"
#include "data.h"
#include "constants/item.h"
#include "core/task.h"
#include "core/text.h"
#include <raylib.h>

#define ITEMS_PER_PAGE 10
#define DESCRIPTION_BOX (Rectangle){VIRTUAL_WIDTH / 2, 28, VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2}

int ItemCount();
static void RemoveItem(int index);
static void CB_HandleBag();
static void DrawBag();
static void Task_ItemSelection(int taskId);

int gBag[BAG_CAPACITY] = {ITEM_THROWING_KNIFE, ITEM_THROWING_KNIFE, ITEM_THROWING_KNIFE};

static int sItemSelectionCursor;
static int sPage;

int ItemCount() {
    int count = 0;

    for (int i = 0; i < BAG_CAPACITY; i++)
        if (gBag[i] != ITEM_NONE)
            count++;

    return count;
}

static void RemoveItem(int index) {
    for (int i = index; i < BAG_CAPACITY; i++) {
        if (gBag[i] == ITEM_NONE)
            return;
        if (i == BAG_CAPACITY - 1) {
            gBag[i] = ITEM_NONE;
            return;
        }
        gBag[i] = gBag[i + 1];
    }
}

void Task_OpenBag(int taskId) {
    StopAllTextPrinters();
    sItemSelectionCursor = 0;
    gTasks[taskId].func = Task_ItemSelection;
    gMainCallback = CB_HandleBag;
}

static void CB_HandleBag() {
    DrawBag();
    RunTextPrinters();
    RunTasks();
}

static void DrawBag() {
    DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, BLACK);
    DrawText("BAG", 4, 4, 16, WHITE);
    DrawLine(0, 24, VIRTUAL_WIDTH, 24, WHITE);
    for (int i = 0; i < 10; i++)
        DrawText(gItemsInfo[gBag[i + sPage * ITEMS_PER_PAGE]].name, 4, 28 + i * 12, 8, WHITE);

    DrawRectangleLinesEx(DESCRIPTION_BOX, 1, WHITE);
    DrawText(gItemsInfo[gBag[sItemSelectionCursor + sPage * ITEMS_PER_PAGE]].description, DESCRIPTION_BOX.x + 4, DESCRIPTION_BOX.y + 4, 8, WHITE);
}

static void Task_ItemSelection(int taskId) {
    if (IsKeyPressed(KEY_X)) {
        gMainCallback = CB_HandleBattle;
        gTasks[taskId].func = Task_PlayMove;
        gTasks[taskId].data[1] = PLAYER;
        gTasks[taskId].data[2] = gItemsInfo[gBag[sItemSelectionCursor + sPage * ITEMS_PER_PAGE]].move;

        // remove the item
        RemoveItem(sItemSelectionCursor + sPage * ITEMS_PER_PAGE);
    }

    if (IsKeyPressed(KEY_Z)) {
        CreateTask(Task_ActionSelection, 0);
        gMainCallback = CB_HandleBattle;
        DestroyTask(taskId);
    }

    if (IsKeyPressed(KEY_UP)) {
        if (--sItemSelectionCursor < 0) {
            if (sPage == 0) {
                sItemSelectionCursor = 0;
            } else {
                sPage--;
                sItemSelectionCursor = ITEMS_PER_PAGE;
            }
        }
    }

    if (IsKeyPressed(KEY_DOWN)) {
        int temp = sItemSelectionCursor;

        if (++sItemSelectionCursor > ITEMS_PER_PAGE) {
            if (sPage == 3) {
                sItemSelectionCursor = ITEMS_PER_PAGE;
            } else {
                sPage++;
                sItemSelectionCursor = 0;
            }
        }

        if (gBag[sItemSelectionCursor + sPage * ITEMS_PER_PAGE] == ITEM_NONE)
            sItemSelectionCursor = temp;
    }

    DrawText(">", 0, 28 + sItemSelectionCursor * 12, 8, WHITE);
}