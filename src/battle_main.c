#include "battle_main.h"

#include "data.h"
#include "main.h"
#include "constants/monsters.h"
#include "constants/moves.h"
#include "constants/textures.h"
#include "constants/audio.h"
#include "core/text.h"
#include "core/task.h"
#include "core/sprite.h"
#include "core/fade.h"
#include "core/music.h"
#include "item.h"
#include "dungeon.h"
#include "end_screen.h"
#include "util.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>

#define BATTLE_TEXT_BOX (Rectangle){60, VIRTUAL_HEIGHT * 3/4, 200, VIRTUAL_HEIGHT * 1/4}
#define MONSTER_SPRITE_BOX (Rectangle){VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2, 64, 64}
#define state data[0]
#define PLAYER_TEXT "YOU"

static void DrawBattleInterface(void);
static void Task_EncounterText(int taskId);
static void Task_StartBattle(int taskId);
static void Task_OpponentPlaysMove(int taskId);
static void Task_Victory(int taskId);
static void Task_VictoryReward(int taskId);
static void Task_Defeat(int taskId);
static void Task_Flee(int taskId);
static void Task_FadeToDungeon(int taskId);
static void Task_FadeToEndScreen(int taskId);

bool gInBattle;
BattlePlayer gBattlePlayer;
BattleMonster gBattleOpponent;
int sMonsterSpriteId;

#define SPRITE_SIZE 64

static const AnimCmd sMonsterAnim[] = {
    ANIMCMD_FRAME(0, 0, 8),
    ANIMCMD_JUMP(0),
};

static const AnimCmd s4FrameAttackAnim[] = {
    ANIMCMD_FRAME(0, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 2, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 3, 0, 8),
    ANIMCMD_END
};

static const AnimCmd s5FrameAttackAnim[] = {
    ANIMCMD_FRAME(0, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 2, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 3, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 4, 0, 8),
    ANIMCMD_END
};

static const AnimCmd s11FrameAttackAnim[] = {
    ANIMCMD_FRAME(0, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 2, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 3, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 4, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 5, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 6, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 7, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 8, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 9, 0, 8),
    ANIMCMD_FRAME(SPRITE_SIZE * 10, 0, 8),
    ANIMCMD_END
};

static const AnimCmd *const sMonsterAnims[] = {
    sMonsterAnim,
    s4FrameAttackAnim,
    s5FrameAttackAnim,
    s11FrameAttackAnim,
};

static void MonsterAttackSpriteCB(Sprite *s) {
    if (s->animEnded) {
        StartSpriteAnim(s, 0);
        s->callback = DummySpriteCallback;
    }
} 

static void MonsterDefeatSpriteCB(Sprite *s) {
    s->bounds.y += 4;

    if (s->bounds.y > VIRTUAL_HEIGHT)
        DestroySprite(s);
} 

static const SpriteTemplate sMonsterTemplate = {
    .width = 64,
    .height = 64,
    .anims = sMonsterAnims,
    .callback = DummySpriteCallback,
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

#define MONSTER_COUNT 7

static int GetRandomMonster(void) {
    int totalWeight = 0;
    MonsterEncounter *table = gLevel < 3 ? sEncounterTable1 : sEncounterTable2;

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

void CB_InitBattle(void) {
    // reset
    ResetAllSprites();

    gInBattle = true;

    int monster = GetRandomMonster();

    gBattleOpponent = (BattleMonster){
        .info = &gMonstersInfo[monster],
        .HP = gMonstersInfo[monster].HP
    };

    CreateTask(Task_EncounterText, 0);
    sMonsterSpriteId = CreateSprite(&sMonsterTemplate, gTextures[gMonstersInfo[monster].textureId], MONSTER_SPRITE_BOX, 0);

    gMainCallback = CB_HandleBattle;

    PlayMusic(gMusic[MUSIC_BATTLE]);
}

void CB_HandleBattle(void) {
    DrawBattleInterface();
    AnimateSprites();
    RunTextPrinters();
    RunTasks();
}

#define PLAYER_HEALTHBOX (Rectangle){0, 0, 100, 32}
#define OPPONENT_HEALTHBOX (Rectangle){VIRTUAL_WIDTH - 100, 0, 100, 32}

static void DrawBattleInterface(void) {
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
            PlaySound(gSounds[SOUND_SELECT]);

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

        DrawTexture(gTextures[TEX_CURSOR], 60 - 8, gTasks[taskId].cursor == 0 ? VIRTUAL_HEIGHT * 3/4 + 4 : VIRTUAL_HEIGHT * 3/4 + 19, WHITE);
        break;
    }
}

#undef cursor

static void Task_OpponentPlaysMove(int taskId) {
    gTasks[taskId].func = Task_PlayMove;
    gTasks[taskId].data[1] = OPPONENT;
    gTasks[taskId].data[2] = gBattleOpponent.info->move;
    
    int anim = 0;
    switch (gBattleOpponent.info->textureId) {
    case TEX_AIR_MAGE:
    case TEX_EARTH_MAGE:
    case TEX_FIRE_MAGE:
    case TEX_WATER_MAGE:
        anim = 1;
        break;
    case TEX_BAT:
    case TEX_WOLF:
        anim = 2;
        break;
    case TEX_DRAGON:
        anim = 3;
        break;
    }

    StartSpriteAnim(&gSprites[sMonsterSpriteId], anim);
    
    gSprites[sMonsterSpriteId].callback = MonsterAttackSpriteCB;
}

static void Recover(int user, int HP) {
    if (user == PLAYER) {
        gBattlePlayer.HP += HP;
        if (gBattlePlayer.HP > gBattlePlayer.maxHP)
            gBattlePlayer.HP = gBattlePlayer.maxHP;
    }else {
        gBattleOpponent.HP += HP;
        if (gBattleOpponent.HP > gBattleOpponent.info->HP)
            gBattleOpponent.HP = gBattleOpponent.info->HP;
    }
}

static void Damage(int user, int HP) {
    if (user == PLAYER) {
        gBattlePlayer.HP -= HP;
        if (gBattlePlayer.HP < 0)
            gBattlePlayer.HP = 0;
    }else {
        gBattleOpponent.HP -= HP;
        if (gBattleOpponent.HP < 0)
            gBattleOpponent.HP = 0;
    }
}

#define user data[1]
#define move data[2]

void Task_PlayMove(int taskId) {
#define AccuracyCheck()                                                                             \
    if (!(GetRandomValue(1, 100) <= gMovesInfo[gTasks[taskId].move].accuracy)) {                    \
        if (gTasks[taskId].user == PLAYER)                                                          \
            strcpy(userText, PLAYER_TEXT);                                                          \
        else                                                                                        \
            strcpy(userText, gBattleOpponent.info->name);                                           \
        sprintf(buffer, "%s used %s\nIt missed", userText, gMovesInfo[gTasks[taskId].move].name);   \
        AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);                                          \
        gTasks[taskId].state = 2;                                                                   \
        break;                                                                                      \
    }                

#define DamageModifier()                                                            \
    if (gBattleOpponent.info->weakness & gMovesInfo[gTasks[taskId].move].flags)     \
        mod = 2;                                                                    \
    if (gBattleOpponent.info->resistance & gMovesInfo[gTasks[taskId].move].flags)   \
        mod = 0.5;

    char buffer[64];
    char userText[16];

    switch (gTasks[taskId].state)
    {
    case 0:
        switch (gMovesInfo[gTasks[taskId].move].effect) {
        case EFFECT_HIT:
            AccuracyCheck()

            if (gTasks[taskId].user == PLAYER) {
                float mod = 1; // damage modifier

                DamageModifier()

                Damage(OPPONENT, (int)(gMovesInfo[gTasks[taskId].move].damage * mod));
                strcpy(userText, PLAYER_TEXT);
            } else {
                Damage(PLAYER, gMovesInfo[gTasks[taskId].move].damage);
                strcpy(userText, gBattleOpponent.info->name);
            }

            sprintf(buffer, "%s used %s", userText, gMovesInfo[gTasks[taskId].move].name);
            AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
            gTasks[taskId].state++;
            break;
        case EFFECT_HEAL:
            Recover(gTasks[taskId].user, -gMovesInfo[gTasks[taskId].move].damage);
            if (gTasks[taskId].user == PLAYER)
                strcpy(userText, PLAYER_TEXT);
            else
                strcpy(userText, gBattleOpponent.info->name);

            sprintf(buffer, "%s used %s\nRecovered health", userText, gMovesInfo[gTasks[taskId].move].name);
            AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
            gTasks[taskId].state++;
            break;
        case EFFECT_ABSORB:
            AccuracyCheck()

            if (gTasks[taskId].user == PLAYER) {
                float mod = 1; // damage modifier

                DamageModifier()

                int damage = (int)(gMovesInfo[gTasks[taskId].move].damage * mod);
                Damage(OPPONENT, damage);
                Recover(PLAYER, damage);
                strcpy(userText, PLAYER_TEXT);
            } else {
                int damage = gMovesInfo[gTasks[taskId].move].damage;
                Damage(PLAYER, damage);
                Recover(OPPONENT, damage);
                strcpy(userText, gBattleOpponent.info->name);
            }

            sprintf(buffer, "%s used %s\nAbsorbed health", userText, gMovesInfo[gTasks[taskId].move].name);
            AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
            gTasks[taskId].state++;
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
    }
}

#undef user
#undef move

static void Task_Victory(int taskId) {
    char buffer[32];    

    switch (gTasks[taskId].state) {
    case 0:
        sprintf(buffer, "%s defeated %s", PLAYER_TEXT, gBattleOpponent.info->name);
        AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
        gSprites[sMonsterSpriteId].callback = MonsterDefeatSpriteCB;
        gTasks[taskId].state++;
        gInBattle = false;
        break;
    case 1:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            DestroyTask(taskId);
            if (gBattleOpponent.info->item) 
                CreateTask(Task_VictoryReward, 0);
            else 
                CreateTask(Task_FadeToDungeon, 0);
        }
        break;
    }
}

static void Task_VictoryReward(int taskId) {
    char buffer[32];    

    switch (gTasks[taskId].state) {
    case 0:
        AddItem(gBattleOpponent.info->item);
        sprintf(buffer, "%s got a %s", PLAYER_TEXT, gItemsInfo[gBattleOpponent.info->item].name);
        AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
        gTasks[taskId].state++;
        break;
    case 1:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            DestroyTask(taskId);
            CreateTask(Task_FadeToDungeon, 0);
        }
        break;
    }
}

static void Task_Defeat(int taskId) {
    char buffer[24];

    switch (gTasks[taskId].state) {
    case 0:
        sprintf(buffer, "%s were defeated", PLAYER_TEXT);
        AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
        gTasks[taskId].state++;
        gInBattle = false;
        break;
    case 1:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            DestroyTask(taskId);
            CreateTask(Task_FadeToEndScreen, 0);
        }
        break;
    }
}

static void Task_Flee(int taskId) {
    char buffer[16];

    switch (gTasks[taskId].state) {
    case 0:
        if (GetRandomValue(1, gBattlePlayer.speed + gBattleOpponent.info->speed) <= gBattlePlayer.speed) {
            sprintf(buffer, "%s ran away", PLAYER_TEXT);
            AddTextPrinterDefault(buffer, BATTLE_TEXT_BOX, 4);
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
            CreateTask(Task_FadeToDungeon, 0);
        }
        break;
    case 2:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            StopAllTextPrinters();
            CreateTask(Task_OpponentPlaysMove, 0);
            DestroyTask(taskId);
        }
        break;
    }
}

static void Task_FadeToDungeon(int taskId) {
    Transition(CB_LoadDungeon)
}

static void Task_FadeToEndScreen(int taskId) {
    Transition(CB_InitEndScreen)
}