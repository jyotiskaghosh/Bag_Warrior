#include "item.h"

#include "main.h"
#include "battle_main.h"
#include "dungeon.h"
#include "data.h"
#include "victory_screen.h"
#include "constants/items.h"
#include "core/task.h"
#include "core/text.h"
#include <raylib.h>

#define ITEMS_PER_PAGE 10
#define DESCRIPTION_BOX (Rectangle){VIRTUAL_WIDTH / 2, 28, VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2}
#define CONFIRMATION_BOX (Rectangle){VIRTUAL_WIDTH - 80, VIRTUAL_HEIGHT - 48, 80, 48}

static void RemoveItem(int index);
static void CB_HandleBag(void);
static void DrawBag(void);
static void Task_ItemSelection(int taskId);
static void Task_InitItemCofirmation(int taskId);
static void Task_ItemConfirmation(int taskId);

int gBag[BAG_CAPACITY];

static int sItemSelectionCursor;
static int sPage;

int ItemCount(void) {
    int count = 0;

    for (int i = 0; i < BAG_CAPACITY; i++)
        if (gBag[i] != ITEM_NONE)
            count++;

    return count;
}

void EmptyBag(void) {
    for (int i = 0; i < BAG_CAPACITY; i++)
        gBag[i] = ITEM_NONE;
}

void AddItem(int item) {
    for (int i = 0; i < BAG_CAPACITY; i++)
        if (gBag[i] == ITEM_NONE) {
            gBag[i] = item;
            return;
        }
}

static void RemoveItem(int index) {
    for (int i = index; i < BAG_CAPACITY; i++) {
        if (gBag[i] == ITEM_NONE)
            goto shiftCursorUp;
        if (i == BAG_CAPACITY - 1) {
            gBag[i] = ITEM_NONE;
            goto shiftCursorUp;
        }
        gBag[i] = gBag[i + 1];
    }
    shiftCursorUp:
        // shift cursor up
        if (--sItemSelectionCursor < 0) {
            if (sPage == 0) {
                sItemSelectionCursor = 0;
            } else {
                sPage--;
                sItemSelectionCursor = ITEMS_PER_PAGE - 1;
            }
        }
}

void CB_OpenBag(void) {
    StopAllTextPrinters();
    gMainCallback = CB_HandleBag;
    CreateTask(Task_ItemSelection, 0);
}

static void CB_HandleBag(void) {
    DrawBag();
    RunTextPrinters();
    RunTasks();
}

static void DrawBag(void) {
    DrawText("BAG", 4, 4, 16, WHITE);
    DrawLine(0, 24, VIRTUAL_WIDTH, 24, WHITE);
    for (int i = 0; i < 10; i++)
        DrawText(gItemsInfo[gBag[i + sPage * ITEMS_PER_PAGE]].name, 4, 28 + i * 12, 8, WHITE);

    DrawRectangleLinesEx(DESCRIPTION_BOX, 1, WHITE);
    DrawText(gItemsInfo[gBag[sItemSelectionCursor + sPage * ITEMS_PER_PAGE]].description, DESCRIPTION_BOX.x + 4, DESCRIPTION_BOX.y + 4, 8, WHITE);
}

static void Task_ItemSelection(int taskId) {
    if (IsKeyPressed(KEY_X)) {
        if (gInBattle) {
            gMainCallback = CB_HandleBattle;
            gTasks[taskId].func = Task_PlayMove;
            gTasks[taskId].data[1] = PLAYER;
            gTasks[taskId].data[2] = gItemsInfo[gBag[sItemSelectionCursor + sPage * ITEMS_PER_PAGE]].move;

            RemoveItem(sItemSelectionCursor + sPage * ITEMS_PER_PAGE);
        } else {
            // if no item skip item conirmation
            if (gBag[sItemSelectionCursor + sPage * ITEMS_PER_PAGE] == ITEM_NONE)
                return;

            CreateTask(Task_InitItemCofirmation, 0);
            DestroyTask(taskId);
        }
    }

    if (IsKeyPressed(KEY_Z)) {
        if (gInBattle) {
            CreateTask(Task_ActionSelection, 0);
            gMainCallback = CB_HandleBattle;
        } else {
            gMainCallback = CB_LoadDungeon;
        }
        
        DestroyTask(taskId);
    }

    if (IsKeyPressed(KEY_UP)) {
        if (--sItemSelectionCursor < 0) {
            if (sPage == 0) {
                sItemSelectionCursor = 0;
            } else {
                sPage--;
                sItemSelectionCursor = ITEMS_PER_PAGE - 1;
            }
        }
    }

    if (IsKeyPressed(KEY_DOWN)) {
        int prevCursor = sItemSelectionCursor;
        int prevPage = sPage;

        if (++sItemSelectionCursor >= ITEMS_PER_PAGE) {
            if (sPage == 3) {
                sItemSelectionCursor = ITEMS_PER_PAGE - 1;
            } else {
                sPage++;
                sItemSelectionCursor = 0;
            }
        }

        if (gBag[sItemSelectionCursor + sPage * ITEMS_PER_PAGE] == ITEM_NONE) {
            sItemSelectionCursor = prevCursor;
            sPage = prevPage;
        }
    }

    DrawText(">", 0, 28 + sItemSelectionCursor * 12, 8, WHITE);
}

static void Task_InitItemCofirmation(int taskId) {
    AddTextPrinterDefault("USE\nTOSS", CONFIRMATION_BOX, 0);
    gTasks[taskId].func = Task_ItemConfirmation;
}

#define cursor data[0]

static void Task_ItemConfirmation(int taskId) {
    if (IsKeyPressed(KEY_X)) {
        if (gTasks[taskId].cursor == 0) {
            Item item = gItemsInfo[gBag[sItemSelectionCursor + sPage * ITEMS_PER_PAGE]];

            switch (item.effect) {
            case ITEM_EFFECT_HEAL:
                gBattlePlayer.HP -= gMovesInfo[item.move].damage;
                if (gBattlePlayer.HP > gBattlePlayer.maxHP)
                    gBattlePlayer.HP = gBattlePlayer.maxHP;
                break;
            case ITEM_EFFECT_ESCAPE:
                DestroyTask(taskId);
                gMainCallback = CB_InitVictoryScreen;
                break;
            default:
                return;
            }

            // remove the item
            RemoveItem(sItemSelectionCursor + sPage * ITEMS_PER_PAGE);
            goto itemSelection;
        } else {
            RemoveItem(sItemSelectionCursor + sPage * ITEMS_PER_PAGE);
            goto itemSelection;
        }
    }

    if (IsKeyPressed(KEY_Z)) {
        goto itemSelection;
    }

    if (IsKeyPressed(KEY_UP))
        if (--gTasks[taskId].cursor < 0)
            gTasks[taskId].cursor = 0;

    if (IsKeyPressed(KEY_DOWN)) 
        if (++gTasks[taskId].cursor > 1)
            gTasks[taskId].cursor = 1;

    DrawText(">", CONFIRMATION_BOX.x - 8, gTasks[taskId].cursor == 0 ? CONFIRMATION_BOX.y + 4 : CONFIRMATION_BOX.y + 19, 8, WHITE);

    return; 

    itemSelection:
        StopAllTextPrinters();
        gTasks[taskId].func = Task_ItemSelection;
}

#undef cursor