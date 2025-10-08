#include "battle_main.h"

#include "data.h"
#include "main.h"
#include "constants/monsters.h"
#include "constants/moves.h"
#include "core/text.h"
#include "core/task.h"
#include "core/sprite.h"
#include "item.h"
#include <raylib.h>
#include <stdio.h>

#define BATTLE_TEXT_BOX (Rectangle){60, VIRTUAL_HEIGHT * 3/4, 200, VIRTUAL_HEIGHT * 1/4}
#define state data[0]

void CB_HandleBattle();
static void DrawBattleInterface();
static void Task_EncounterText(int taskId);
static void Task_StartBattle(int taskId);
void Task_ActionSelection(int taskId);
static void Task_OpponentPlaysMove(int taskId);
void Task_PlayMove(int taskId);
static void Task_Victory(int taskId);
static void Task_Defeat(int taskId);
static void Task_Flee(int taskId);

bool gInBattle;
BattlePlayer gBattlePlayer;
BattleMonster gBattleOpponent;

void CB_InitBattle() {
    gBattlePlayer = (BattlePlayer){
        .HP = 10,
        .maxHP = 10,
        .speed = 5
    }; 

    gBattleOpponent = (BattleMonster){
        .info = &gMonstersInfo[MONSTER_WOLF],
        .HP = gMonstersInfo[MONSTER_WOLF].HP
    };

    CreateTask(Task_EncounterText, 0);

    gMainCallback = CB_HandleBattle;
}

void CB_HandleBattle() {
    DrawBattleInterface();
    AnimateSprites();
    RunTextPrinters();
    RunTasks();
}

#define PLAYER_HEALTHBOX (Rectangle){0, 0, 80, 32}
#define OPPONENT_HEALTHBOX (Rectangle){VIRTUAL_WIDTH - 80, 0, 80, 32}

static void DrawBattleInterface() {
    // player healthbox
    int maxHP = gBattlePlayer.maxHP;
    int HP = gBattlePlayer.HP;

    char buffer[16];

    DrawRectangleRec(PLAYER_HEALTHBOX, GRAY);
    DrawRectangleLinesEx(PLAYER_HEALTHBOX, 1, BLACK);

    DrawText("PLAYER", PLAYER_HEALTHBOX.x + 4, PLAYER_HEALTHBOX.y + 4, 8, WHITE);
    sprintf(buffer, "HP: %d / %d", HP, maxHP);
    DrawText(buffer, PLAYER_HEALTHBOX.x + 4, PLAYER_HEALTHBOX.y + 16, 8, WHITE);

    // opponent health box 
    maxHP = gBattleOpponent.info->HP;
    HP = gBattleOpponent.HP;

    DrawRectangleRec(OPPONENT_HEALTHBOX, GRAY);
    DrawRectangleLinesEx(OPPONENT_HEALTHBOX, 1, BLACK);

    DrawText(gBattleOpponent.info->name, OPPONENT_HEALTHBOX.x + 4, OPPONENT_HEALTHBOX.y + 4, 8, WHITE);
    sprintf(buffer, "HP: %d / %d", HP, maxHP);
    DrawText(buffer, OPPONENT_HEALTHBOX.x + 4, OPPONENT_HEALTHBOX.y + 16, 8, WHITE);
}

#undef PLAYER_HEALTHBOX
#undef OPPONENT_HEALTHBOX

static void Task_EncounterText(int taskId) {
    char buffer[32];

    switch (gTasks[taskId].state) {
    case 0:
        sprintf(buffer, "A %s appeared!", gBattleOpponent.info->name);
        AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
        gTasks[taskId].state++;
        break;
    case 1:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            CreateTask(Task_StartBattle, 0);
            DestroyTask(taskId);
        }
        break;
    default:
        break;
    }
}

static void Task_StartBattle(int taskId) {
    if (gBattlePlayer.speed > gBattleOpponent.info->speed) {
        gTasks[taskId].func = Task_ActionSelection;
    } else if (gBattlePlayer.speed < gBattleOpponent.info->speed) {
        gTasks[taskId].func = Task_OpponentPlaysMove;
    } else {
        if (GetRandomValue(0, 1)) 
            gTasks[taskId].func = Task_ActionSelection;
        else
            gTasks[taskId].func = Task_OpponentPlaysMove;
    }
}

#define cursor data[1]

void Task_ActionSelection(int taskId) {
    switch (gTasks[taskId].state) {
    case 0:
        AddTextPrinterDefault("ITEM\nFLEE", BATTLE_TEXT_BOX, 0);
        gTasks[taskId].state++;
        break;
    case 1:
        if (IsKeyPressed(KEY_X)) {
            if (gTasks[taskId].cursor == 0) {
                if (!ItemCount()) return;

                CreateTask(Task_OpenBag, 0);
                DestroyTask(taskId);
            } else {
                CreateTask(Task_Flee, 0);
                DestroyTask(taskId);
            }
        }

        if (IsKeyPressed(KEY_UP))
            if (--gTasks[taskId].cursor < 0)
                gTasks[taskId].cursor = 0;

        if (IsKeyPressed(KEY_DOWN))
            if (++gTasks[taskId].cursor > 1)
                gTasks[taskId].cursor = 1;

        DrawText(">", 60 - 4, gTasks[taskId].cursor == 0 ? VIRTUAL_HEIGHT * 3/4 + 4 : VIRTUAL_HEIGHT * 3/4 + 19, 8, BLACK);
        break;
    }
}

#undef cursor

static void Task_OpponentPlaysMove(int taskId) {
    int count = 0; // find valid moves

    for (int i = 0; i < MAX_MOVES; i++)
        if (gBattleOpponent.info->move[i] != MOVE_NONE)
            count++;

    gTasks[taskId].func = Task_PlayMove;
    gTasks[taskId].data[1] = OPPONENT;
    gTasks[taskId].data[2] = gBattleOpponent.info->move[GetRandomValue(0, count - 1)];
}

#define user data[1]
#define move data[2]

void Task_PlayMove(int taskId) {
    char buffer[32];

    switch (gTasks[taskId].state)
    {
    case 0:
        switch (gMovesInfo[gTasks[taskId].move].effect)
        {
        case EFFECT_HIT:
            if (gTasks[taskId].user == PLAYER) {
                gBattleOpponent.HP -= gMovesInfo[gTasks[taskId].move].damage;
                if (gBattleOpponent.HP < 0)
                    gBattleOpponent.HP = 0;
                sprintf(buffer, "You used %s", gMovesInfo[gTasks[taskId].move].name);
            } else {
                gBattlePlayer.HP -= gMovesInfo[gTasks[taskId].move].damage;
                if (gBattlePlayer.HP < 0)
                    gBattlePlayer.HP = 0;
                sprintf(buffer, "%s used %s", gBattleOpponent.info->name, gMovesInfo[gTasks[taskId].move].name);
            }

            AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
            gTasks[taskId].state++;
            break;
        default:
            break;
        }
    case 1:
        if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_Z)) {
            if (gBattleOpponent.HP <= 0) {
                CreateTask(Task_Victory, 0);
                DestroyTask(taskId);
                break;
            }

            if (gBattlePlayer.HP <= 0) {
                CreateTask(Task_Defeat, 0);
                DestroyTask(taskId);
                break;
            }

            if (gTasks[taskId].user == PLAYER)
                CreateTask(Task_OpponentPlaysMove, 0);
            else 
                CreateTask(Task_ActionSelection, 0);
            DestroyTask(taskId);
        }
        break;
    default:
        break;
    }
}

#define user
#define move

static void Task_Victory(int taskId) {
    char buffer[32];    

    switch (gTasks[taskId].state) {
    case 0:
        sprintf(buffer, "You defeated %s", gBattleOpponent.info->name);
        AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
        gTasks[taskId].state++;
        break;
    default:
        break;
    }
}

static void Task_Defeat(int taskId) {
    switch (gTasks[taskId].state) {
    case 0:
        AddTextPrinterDefault("You were defeated", BATTLE_TEXT_BOX, 4);
        gTasks[taskId].state++;
        break;
    default:
        break;
    }
}

static void Task_Flee(int taskId) {
    switch (gTasks[taskId].state) {
    case 0:
        AddTextPrinterDefault("You ran away", BATTLE_TEXT_BOX, 4);
        gTasks[taskId].state++;
        break;
    default:
        break;
    }
}

