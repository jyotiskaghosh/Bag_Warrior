#include "battle_main.h"

#include "data.h"
#include "main.h"
#include "constants/monsters.h"
#include "constants/moves.h"
#include "core/text.h"
#include "core/task.h"
#include "core/sprite.h"
#include "item.h"
#include "dungeon.h"
#include "end_screen.h"
#include <raylib.h>
#include <stdio.h>

#define BATTLE_TEXT_BOX (Rectangle){60, VIRTUAL_HEIGHT * 3/4, 200, VIRTUAL_HEIGHT * 1/4}
#define MONSTER_SPRITE_BOX (Rectangle){VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2, 64, 64}
#define state data[0]

void CB_HandleBattle();
static void DrawBattleInterface();
static void Task_EncounterText(int taskId);
static void Task_StartBattle(int taskId);
void Task_ActionSelection(int taskId);
static void Task_OpponentPlaysMove(int taskId);
void Task_PlayMove(int taskId);
static void Task_Victory(int taskId);
static void Task_VictoryReward(int taskId);
static void Task_Defeat(int taskId);
static void Task_Flee(int taskId);

bool gInBattle;
BattlePlayer gBattlePlayer;
BattleMonster gBattleOpponent;
int sMonsterSpriteId;

static const AnimCmd sMonsterAnim[] = {
    ANIMCMD_FRAME(0, 0, 8),
    ANIMCMD_JUMP(0),
};

static const AnimCmd *const sMonsterAnims[] = {
    sMonsterAnim,
};

static void sDummySpriteCallback(Sprite *s) {}

static void sMonsterDefeatSpriteCB(Sprite *s) {
    s->bounds.y += 0.1;

    if (s->bounds.y > VIRTUAL_HEIGHT)
        DestroySprite(s);
} 

static const SpriteTemplate sMonsterTemplate = {
    .width = 64,
    .height = 64,
    .anims = sMonsterAnims,
    .callback = sDummySpriteCallback,
};

typedef struct {
    int monster;
    int rarity;
} MonsterEncounter;

static MonsterEncounter sEncounterTable1[] = {
    {MONSTER_WOLF, 30},
    {MONSTER_BAT, 30},
    {MONSTER_AIR_MAGE, 10},
    {MONSTER_EARTH_MAGE, 10},
    {MONSTER_WATER_MAGE, 10},
    {MONSTER_FIRE_MAGE, 10},
    {MONSTER_DRAGON, 0}
};

static MonsterEncounter sEncounterTable2[] = {
    {MONSTER_WOLF, 30},
    {MONSTER_BAT, 30},
    {MONSTER_AIR_MAGE, 10},
    {MONSTER_EARTH_MAGE, 10},
    {MONSTER_WATER_MAGE, 10},
    {MONSTER_FIRE_MAGE, 10},
    {MONSTER_DRAGON, 5}
};

static int GetRandomMonster(void) {
    int totalWeight = 0;
    MonsterEncounter *table = gLevel < 3 ? sEncounterTable1 : sEncounterTable2;

#define MONSTER_COUNT 7

    for (int i = 0; i < MONSTER_COUNT; i++)
        totalWeight += table[i].rarity;

    int r = GetRandomValue(1, totalWeight);
    for (int i = 0; i < MONSTER_COUNT; i++) {
        r -= table[i].rarity;

        if (r <= 0)
            return table[i].monster;
    }

    return table[0].monster; // fallback
}


void CB_InitBattle() {
    gInBattle = true;

    int monster = GetRandomMonster();

    gBattleOpponent = (BattleMonster){
        .info = &gMonstersInfo[monster],
        .HP = gMonstersInfo[monster].HP
    };

    CreateTask(Task_EncounterText, 0);
    sMonsterSpriteId = CreateSprite(&sMonsterTemplate, gMonstersInfo[monster].imageFilename, MONSTER_SPRITE_BOX, 0);

    gMainCallback = CB_HandleBattle;
}

void CB_HandleBattle() {
    DrawBattleInterface();
    AnimateSprites();
    RunTextPrinters();
    RunTasks();
}

#define PLAYER_HEALTHBOX (Rectangle){0, 0, 100, 32}
#define OPPONENT_HEALTHBOX (Rectangle){VIRTUAL_WIDTH - 100, 0, 100, 32}

static void DrawBattleInterface() {
    // player healthbox
    int maxHP = gBattlePlayer.maxHP;
    int HP = gBattlePlayer.HP;

    char buffer[16];

    DrawRectangleRec(PLAYER_HEALTHBOX, BLACK);
    DrawRectangleLinesEx(PLAYER_HEALTHBOX, 1, WHITE);

    DrawText("PLAYER", PLAYER_HEALTHBOX.x + 4, PLAYER_HEALTHBOX.y + 4, 8, WHITE);
    sprintf(buffer, "HP: %d / %d", HP, maxHP);
    DrawText(buffer, PLAYER_HEALTHBOX.x + 4, PLAYER_HEALTHBOX.y + 16, 8, WHITE);

    // opponent health box 
    maxHP = gBattleOpponent.info->HP;
    HP = gBattleOpponent.HP;

    DrawRectangleRec(OPPONENT_HEALTHBOX, BLACK);
    DrawRectangleLinesEx(OPPONENT_HEALTHBOX, 1, WHITE);

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
                gMainCallback = CB_OpenBag;
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

        DrawText(">", 60 - 4, gTasks[taskId].cursor == 0 ? VIRTUAL_HEIGHT * 3/4 + 4 : VIRTUAL_HEIGHT * 3/4 + 19, 8, WHITE);
        break;
    }
}

#undef cursor

static void Task_OpponentPlaysMove(int taskId) {
    gTasks[taskId].func = Task_PlayMove;
    gTasks[taskId].data[1] = OPPONENT;
    gTasks[taskId].data[2] = gBattleOpponent.info->move;
}

#define user data[1]
#define move data[2]

void Task_PlayMove(int taskId) {
    char buffer[64];

    switch (gTasks[taskId].state)
    {
    case 0:
        switch (gMovesInfo[gTasks[taskId].move].effect) {
        case EFFECT_HIT:
            // accuracy check
            if (!(GetRandomValue(0, 100) < gMovesInfo[gTasks[taskId].move].accuracy)) {
                if (gTasks[taskId].user == PLAYER)
                    sprintf(buffer, "You used %s\nIt missed", gMovesInfo[gTasks[taskId].move].name);
                else 
                    sprintf(buffer, "%s used %s\nIt missed", gBattleOpponent.info->name, gMovesInfo[gTasks[taskId].move].name);
                
                AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
                gTasks[taskId].state = 2;
                break;
            }

            if (gTasks[taskId].user == PLAYER) {
                float mod = 1; // damage modifier

                if (gBattleOpponent.info->weakness & gMovesInfo[gTasks[taskId].move].flags)
                    mod = 2;

                if (gBattleOpponent.info->resistance & gMovesInfo[gTasks[taskId].move].flags)
                    mod = 0.5;

                gBattleOpponent.HP -= (int)(gMovesInfo[gTasks[taskId].move].damage * mod);
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
        case EFFECT_HEAL:
            if (gTasks[taskId].user == PLAYER) {
                gBattlePlayer.HP -= gMovesInfo[gTasks[taskId].move].damage;
                if (gBattlePlayer.HP > gBattlePlayer.maxHP)
                    gBattlePlayer.HP = gBattlePlayer.maxHP;
                sprintf(buffer, "You used %s\nHealed %d HP", gMovesInfo[gTasks[taskId].move].name, -gMovesInfo[gTasks[taskId].move].damage);
            } else {
                gBattleOpponent.HP -= gMovesInfo[gTasks[taskId].move].damage;
                if (gBattleOpponent.HP > gBattleOpponent.info->HP)
                    gBattleOpponent.HP = gBattleOpponent.info->HP; 
                sprintf(buffer, "%s used %s\nHealed %d HP", gBattleOpponent.info->name, gMovesInfo[gTasks[taskId].move].name, -gMovesInfo[gTasks[taskId].move].damage);
            }

            AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
            gTasks[taskId].state++;
            break;
        default:
            break;
        }
        break;
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
    case 2:
        if (IsKeyPressed(KEY_X) || IsKeyPressed(KEY_Z)) {
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

#undef user
#undef move

static void Task_Victory(int taskId) {
    char buffer[32];    

    switch (gTasks[taskId].state) {
    case 0:
        sprintf(buffer, "You defeated %s", gBattleOpponent.info->name);
        AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
        gSprites[sMonsterSpriteId].callback = sMonsterDefeatSpriteCB;
        gTasks[taskId].state++;
        gInBattle = false;
        break;
    case 1:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            DestroyTask(taskId);
            if (gBattleOpponent.info->item) 
                CreateTask(Task_VictoryReward, 0);
            else 
                gMainCallback = CB_LoadDungeon;
        }
        break;
    default:
        break;
    }
}

static void Task_VictoryReward(int taskId) {
    char buffer[32];    

    switch (gTasks[taskId].state) {
    case 0:
        AddItem(gBattleOpponent.info->item);
        sprintf(buffer, "You got a %s", gItemsInfo[gBattleOpponent.info->item].name);
        AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
        gTasks[taskId].state++;
        break;
    case 1:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            DestroyTask(taskId);
            gMainCallback = CB_LoadDungeon;
        }
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
        gInBattle = false;
        break;
    case 1:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            DestroyTask(taskId);
            gMainCallback = CB_InitEndScreen;
        }
        break;
    default:
        break;
    }
}

static void Task_Flee(int taskId) {
    switch (gTasks[taskId].state) {
    case 0:
        if (GetRandomValue(0, gBattlePlayer.speed + gBattleOpponent.info->speed) < gBattlePlayer.speed) {
            AddTextPrinterDefault("You ran away", BATTLE_TEXT_BOX, 4);
            gTasks[taskId].state++;
            gInBattle = false;
        } else {
            AddTextPrinterDefault("Failed to run away", BATTLE_TEXT_BOX, 4);
            gTasks[taskId].state = 2;
        }
        break;
    case 1:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            DestroyTask(taskId);
            gMainCallback = CB_LoadDungeon;
        }
        break;
    case 2:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            StopAllTextPrinters();
            CreateTask(Task_OpponentPlaysMove, 0);
            DestroyTask(taskId);
        }
        break;
    default:
        break;
    }
}

