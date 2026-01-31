#include "dungeon.h"

#include "battle_main.h"
#include "item.h"
#include "core/task.h"
#include "core/text.h"
#include "core/sprite.h"
#include "core/fade.h"
#include "core/music.h"
#include "constants/items.h"
#include "constants/textures.h"
#include "constants/audio.h"
#include "data.h"
#include "util.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_ROOMS ROOM_ROWS * ROOM_COLS

#define ENCOUNTER_RATE 5

typedef struct {
    int x, y, w, h; // position and size
    bool exists;     // 1 if the room exists
} Room;

typedef struct {
    int layer1[MAP_H][MAP_W], layer2[MAP_H][MAP_W];
} Map;

static struct RoomNode {
    bool conn[MAX_ROOMS];
    bool isConn[MAX_ROOMS];
    bool inGraph;
} sRoomGraph[MAX_ROOMS] = {
    { { 0, 1, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 1, 0, 1, 0, 1, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 1, 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 1, 0, 0, 0, 1, 0, 1, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 1, 0, 1, 0, 1, 0, 1, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 0, 1, 0, 1, 0, 0, 0, 1 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 0, 0, 1, 0, 0, 0, 1, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 0, 0, 0, 1, 0, 1, 0, 1 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
	{ { 0, 0, 0, 0, 0, 1, 0, 1, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 },
};

/*
 * Coordinates data type
 */
typedef struct {
    int x;
    int y;
} Coordinates;

enum {
    TILE_EMPTY,
    TILE_FLOOR,
    TILE_STAIR,
    TILE_CHEST,
    TILE_CHEST_CLOSED,
    TILE_COINS,
    TILE_BONE,
    TILE_SKULL
};

static void CB_HandleDungeon(void);
static int RandomRoom(void);
static Coordinates RandomPos(Room *r);
static void CarveRoom(Room r);
static void ConnectRooms(int r1, int r2);
static void Task_FadeToBattle(int taskId);
static void Task_FadeToNewLevel(int taskId);
static void Task_OpenChest(int taskId);
static void Task_PickCoins(int taskId);

int gLevel;
int gGold;

static Map sMap;
static Room sRooms[MAX_ROOMS];
static Coordinates sPlayerPos;
static Camera2D sCamera;

static bool sIsFieldControlLocked;
static bool sInDungeon;

static const AnimCmd sPlayerAnim[] = {
    ANIMCMD_FRAME(0, 0, 8),
    ANIMCMD_FRAME(16, 0, 8),
    ANIMCMD_FRAME(32, 0, 8),
    ANIMCMD_FRAME(48, 0, 8),
    ANIMCMD_JUMP(0),
};

static const AnimCmd *const sPlayerAnims[] = {
    sPlayerAnim,
};

static const SpriteTemplate sPlayerTemplate = {
    .width = 16,
    .height = 16,
    .anims = sPlayerAnims,
    .callback = DummySpriteCallback,
};

static void ClearMap(void) {
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++) {
            sMap.layer1[y][x] = TILE_EMPTY;
            sMap.layer2[y][x] = TILE_EMPTY;
        }
}

static void GenerateRooms(void) {
    int cellW = MAP_W / ROOM_COLS;
    int cellH = MAP_H / ROOM_ROWS;

    // clear things for a new level
    for (int i = 0; i < MAX_ROOMS; i++)
        sRooms[i].exists = true;

    // put the gone rooms, if any, on the level
    int leftOut = GetRandomValue(0, 3);
    for (int i = 0; i < leftOut; i++)
        sRooms[RandomRoom()].exists = false;

    for (int i = 0; i < MAX_ROOMS; i++) {
        int cellX = (i % 3) * cellW;
        int cellY = (i / 3) * cellH;

        if (!sRooms[i].exists) {
            sRooms[i].x = cellX + GetRandomValue(0, cellW - 3) + 1;
            sRooms[i].y = cellY + GetRandomValue(0, cellH - 3) + 1;
            sRooms[i].w = 1;
            sRooms[i].h = 1;
        } else {
            sRooms[i].w = GetRandomValue(0, cellW - 5) + 4;
            sRooms[i].h = GetRandomValue(0, cellH - 5) + 4;
            sRooms[i].x = cellX + GetRandomValue(0, cellW - sRooms[i].w - 1);
            sRooms[i].y = cellY + GetRandomValue(0, cellH - sRooms[i].h - 1);
        }

        CarveRoom(sRooms[i]);
    }
}

static int RandomRoom(void) {
    int n;
    do {
        n = GetRandomValue(0, MAX_ROOMS - 1);
    } while (!sRooms[n].exists);
    return n;
} 

static Coordinates RandomPos(Room *r) {
    Coordinates c = {
        r->x + GetRandomValue(0, r->w - 3) + 1,
        r->y + GetRandomValue(0, r->h - 3) + 1
    };
    return c;
}

// Carve room floor
static void CarveRoom(Room r) {
    for (int y = r.y; y < r.y + r.h; y++)
        for (int x = r.x; x < r.x + r.w; x++)
            sMap.layer1[y][x] = TILE_FLOOR;
}

static void GeneratePassages(void) {
    struct RoomNode *r1, *r2 = NULL;
    int i, j;

    // reinitialize room graph
    for (r1 = sRoomGraph; r1 <= &sRoomGraph[MAX_ROOMS - 1]; r1++) {
        for (int j = 0; j < MAX_ROOMS; j++)
            r1->isConn[j] = false;
        r1->inGraph = false;
    }

    /*
     * starting with one room, connect it to a random adjacent room and
     * then pick a new room to start with.
     */
    int roomCount = 1;
    r1 = &sRoomGraph[GetRandomValue(0, MAX_ROOMS - 1)];
    r1->inGraph = true;

    do {
        // find a room to connect with
        j = 0;
        for (i = 0; i < MAX_ROOMS; i++)
            if (r1->conn[i] && !sRoomGraph[i].inGraph && GetRandomValue(0, ++j - 1) == 0)
                r2 = &sRoomGraph[i];

        /*
        * if no adjacent rooms are outside the graph, pick a new room
        * to look from
        */
        if (j == 0)
        {
            do
                r1 = &sRoomGraph[GetRandomValue(0, MAX_ROOMS - 1)];
            while (!r1->inGraph);
        } else {      
            /*
            * otherwise, connect new room to the graph, and draw a tunnel
            * to it
            */
            r2->inGraph = true;
            i = (int)(r1 - sRoomGraph);
            j = (int)(r2 - sRoomGraph);
            ConnectRooms(i, j);
            r1->isConn[j] = true;
            r2->isConn[i] = true;
            roomCount++;
        }
    } while (roomCount < MAX_ROOMS);

    /*
     * attempt to add passages to the graph a random number of times so
     * that there isn't always just one unique passage through it.
     */
    for (roomCount = GetRandomValue(0, 4); roomCount > 0; roomCount--)
    {
        r1 = &sRoomGraph[GetRandomValue(0, MAX_ROOMS - 1)];	/* a random room to look from */
        /*
        * find an adjacent room not already connected
        */
        j = 0;
        for (i = 0; i < MAX_ROOMS; i++)
            if (r1->conn[i] && !r1->isConn[i] && GetRandomValue(0, ++j - 1) == 0)
                r2 = &sRoomGraph[i];
        /*
        * if there is one, connect it and look for the next added
        * passage
        */
        if (j != 0)
        {
            i = (int)(r1 - sRoomGraph);
            j = (int)(r2 - sRoomGraph);
            ConnectRooms(i, j);
            r1->isConn[j] = true;
            r2->isConn[i] = true;
        }
    }
}

static void ConnectRooms(int r1, int r2) {
    Room *rpf, *rpt = NULL;
    int distance = 0, turn_spot, turn_distance = 0;
    int rm, rmt;
    char direc;
    static Coordinates del, curr, turn_delta, spos, epos;
    
    if (r1 < r2) {
        rm = r1;
        if (r1 + 1 == r2)
            direc = 'r';
        else 
            direc = 'd';
    } else {
        rm = r2;
        if (r2 + 1 == r1)
            direc = 'r';
        else
            direc = 'd';
    }

    rpf = &sRooms[rm];

    /*
     * Set up the movement variables, in two cases:
     * first drawing one down.
     */
    if (direc == 'd') {
        rmt = rm + 3;				/* room # of dest */
        rpt = &sRooms[rmt];			/* room pointer of dest */
        del.x = 0;				/* direction of move */
        del.y = 1;
        spos.x = rpf->x;			/* start of move */
        spos.y = rpf->y;
        epos.x = rpt->x;			/* end of move */
        epos.y = rpt->y;
        
        if (rpf->exists) {
            spos.x = rpf->x + GetRandomValue(0, rpf->w - 3) + 1;
		    spos.y = rpf->y + rpf->h - 1;
        }
        if (rpt->exists) {
            epos.x = rpt->x + GetRandomValue(0, rpt->w - 3) + 1;
        }

        distance = abs(spos.y - epos.y); // distance to move
        turn_delta.y = 0; // direction to turn
        turn_delta.x = (spos.x < epos.x ? 1 : -1);
        turn_distance = abs(spos.x - epos.x); // how far to turn

    } else if (direc == 'r') { // setup for moving right 
        rmt = rm + 1;
        rpt = &sRooms[rmt];
        del.x = 1;
        del.y = 0;
        spos.x = rpf->x;
        spos.y = rpf->y;
        epos.x = rpt->x;
        epos.y = rpt->y;
        
        if (rpf->exists) {
            spos.x = rpf->x + rpf->w - 1;
            spos.y = rpf->y + GetRandomValue(0, rpf->h - 3) + 1;
        } 
        if (rpt->exists) {
            epos.y = rpt->y + GetRandomValue(0, rpt->h - 3) + 1;
        }

        distance = abs(spos.x - epos.x);
        turn_delta.y = (spos.y < epos.y ? 1 : -1);
        turn_delta.x = 0;
        turn_distance = abs(spos.y - epos.y);
    }

    turn_spot = GetRandomValue(0, distance - 2) + 1; // where turn starts

    // get ready to move...
    curr.x = spos.x;
    curr.y = spos.y;

    while (distance > 0) {
        // move to new position
        curr.x += del.x;
        curr.y += del.y;

        // check if we are at the turn place, if so we the turn
        if (distance == turn_spot) {
            while (turn_distance--) {
                sMap.layer1[curr.y][curr.x] = TILE_FLOOR;
                curr.x += turn_delta.x;
	        	curr.y += turn_delta.y;
            }
        }
        sMap.layer1[curr.y][curr.x] = TILE_FLOOR;
        distance--;
    }
}

static Coordinates FindFloor(void) {
    Room *r = &sRooms[RandomRoom()];
    return RandomPos(r);
}

static void PutStair(void) {
    Coordinates c = FindFloor();
    sMap.layer2[c.y][c.x] = TILE_STAIR;
}

#define MAX_CHESTS 9
#define CHEST_CHANCE 80

static void PutChests(void) {
    for (int i = 0; i < MAX_CHESTS; i++) 
        if (GetRandomValue(1, 100) <= CHEST_CHANCE) {
            Coordinates c = FindFloor();
            sMap.layer2[c.y][c.x] = TILE_CHEST;
        }
}

#define MAX_COINS 9
#define COIN_CHANCE 50

static void PutCoins(void) {
    for (int i = 0; i < MAX_COINS; i++) 
        if (GetRandomValue(1, 100) <= COIN_CHANCE) {
            Coordinates c = FindFloor();
            sMap.layer2[c.y][c.x] = TILE_COINS;
        }
}

#define MAX_BONES 9
#define BONES_CHANCE 50

static void PutBones(void) {
    for (int i = 0; i < MAX_BONES; i++) 
        if (GetRandomValue(1, 100) <= BONES_CHANCE) {
            Coordinates c = FindFloor();
            sMap.layer2[c.y][c.x] = TILE_BONE;
        }
}

#define MAX_SKULL 9
#define SKULL_CHANCE 30

static void PutSkull(void) {
    for (int i = 0; i < MAX_SKULL; i++) 
        if (GetRandomValue(1, 100) <= SKULL_CHANCE) {
            Coordinates c = FindFloor();
            sMap.layer2[c.y][c.x] = TILE_SKULL;
        }
}

#define FLOOR (Rectangle){TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE}
#define STAIR (Rectangle){0, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE}
#define CHEST (Rectangle){3 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE}
#define CHEST_CLOSED (Rectangle){3 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE}
#define COINS (Rectangle){4 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE}
#define BONES (Rectangle){4 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, TILE_SIZE}
#define SKULL (Rectangle){2 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE}

static void DrawTile(int tile, int y, int x) { 
    Rectangle dest = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};    

    switch (tile) {                                                                    
    case TILE_EMPTY: 
        break; 
    case TILE_FLOOR: 
        DrawTexturePro(gTextures[TEX_DUNGEON_TILESET], FLOOR, dest, (Vector2){0, 0}, 0, WHITE); 
        break;
    case TILE_STAIR: 
        DrawTexturePro(gTextures[TEX_DUNGEON_TILESET], STAIR, dest, (Vector2){0, 0}, 0, WHITE); 
        break;
    case TILE_CHEST: 
        DrawTexturePro(gTextures[TEX_DUNGEON_TILESET], CHEST, dest, (Vector2){0, 0}, 0, WHITE); 
        break;
    case TILE_CHEST_CLOSED: 
        DrawTexturePro(gTextures[TEX_DUNGEON_TILESET], CHEST_CLOSED, dest, (Vector2){0, 0}, 0, WHITE); 
        break;
    case TILE_COINS: 
        DrawTexturePro(gTextures[TEX_DUNGEON_TILESET], COINS, dest, (Vector2){0, 0}, 0, WHITE); 
        break;
    case TILE_BONE: 
        DrawTexturePro(gTextures[TEX_DUNGEON_TILESET], BONES, dest, (Vector2){0, 0}, 0, WHITE); 
        break;
    case TILE_SKULL: 
        DrawTexturePro(gTextures[TEX_DUNGEON_TILESET], SKULL, dest, (Vector2){0, 0}, 0, WHITE); 
        break;
    }  
}

static void DrawMap(void) {
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++) {
            DrawTile(sMap.layer1[y][x], y, x); 
            DrawTile(sMap.layer2[y][x], y, x);
        }
}

#define INFO_BAR (Rectangle){0, 0, VIRTUAL_WIDTH, 16}

static void DrawCameraView(void) {
    char buffer[16];

    sCamera.target = (Vector2){sPlayerPos.x * TILE_SIZE, sPlayerPos.y * TILE_SIZE};
    sCamera.offset = (Vector2){ VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2};  // screen center (half screen width/height)
    sCamera.rotation = 0.0f;
    sCamera.zoom = 1.0f;

    BeginMode2D(sCamera);
        DrawMap();
    EndMode2D();

    DrawRectangleRec(INFO_BAR, BLACK);
    DrawLine(0, 16, VIRTUAL_WIDTH, 16, WHITE);

    sprintf(buffer, "HP: %d / %d", gBattlePlayer.HP, gBattlePlayer.maxHP);
    DrawText(buffer, 4, 4, 8, WHITE);

    sprintf(buffer, "GOLD: %d", gGold);
    DrawText(buffer, VIRTUAL_WIDTH * 1 / 3, 4, 8, WHITE);

    sprintf(buffer, "LEVEL: %d", gLevel);
    DrawText(buffer, VIRTUAL_WIDTH * 2 / 3, 4, 8, WHITE);
}

#undef INFO_BAR

static void Task_HandleOverworld(int taskId) {
    if (sIsFieldControlLocked) return;

    if (IsKeyPressed(KEY_UP)) {
        if (sPlayerPos.y - 1 >= 0 && sMap.layer1[sPlayerPos.y - 1][sPlayerPos.x] != TILE_EMPTY) {
            sPlayerPos.y--;

            goto doMovementActions;
        }
    }

    if (IsKeyPressed(KEY_DOWN)) {
        if (sPlayerPos.y + 1 < MAP_H && sMap.layer1[sPlayerPos.y + 1][sPlayerPos.x] != TILE_EMPTY) {
            sPlayerPos.y++;

            goto doMovementActions;
        }
    } 

    if (IsKeyPressed(KEY_LEFT)) {
        if (sPlayerPos.x - 1 >= 0 && sMap.layer1[sPlayerPos.y][sPlayerPos.x - 1] != TILE_EMPTY) {
            sPlayerPos.x--;

            goto doMovementActions;
        }
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        if (sPlayerPos.x + 1 < MAP_W && sMap.layer1[sPlayerPos.y][sPlayerPos.x + 1] != TILE_EMPTY) {
            sPlayerPos.x++;

            goto doMovementActions;
        }
    }

    if (IsKeyPressed(KEY_S)) {
        DestroyTask(taskId);
        gMainCallback = CB_OpenBag;
    }

    return;

    doMovementActions:
        switch (sMap.layer2[sPlayerPos.y][sPlayerPos.x]) {
        case TILE_STAIR:
            DestroyTask(taskId);
            CreateTask(Task_FadeToNewLevel, 0);
            return;
        case TILE_CHEST:
            CreateTask(Task_OpenChest, 0);
            sMap.layer2[sPlayerPos.y][sPlayerPos.x] = TILE_CHEST_CLOSED;
            return;
        case TILE_COINS:
            CreateTask(Task_PickCoins, 0);
            sMap.layer2[sPlayerPos.y][sPlayerPos.x] = TILE_EMPTY;
            return;
        }

        if (GetRandomValue(1, 100) <= ENCOUNTER_RATE) {
            sInDungeon = false;        

            DestroyTask(taskId);
            CreateTask(Task_FadeToBattle, 0);
        }
}

#define state data[0]

static void Task_FadeToBattle(int taskId) {
    Transition(CB_InitBattle)
}

static void Task_FadeToNewLevel(int taskId) {
    Transition(CB_NewLevel)    
}

#define MAX_CHEST_GOLD 1000
#define TEXT_BOX (Rectangle){60, VIRTUAL_HEIGHT * 3/4, 200, VIRTUAL_HEIGHT * 1/4}

typedef struct {
    int item;
    int rarity;
} ChestDrop;

static ChestDrop sChestDrops[] = {
    {ITEM_THROWING_KNIFE, 10},
    {ITEM_BOMB, 10},
    {ITEM_POTION, 10},
    {ITEM_EARTH_SCROLL, 10},
    {ITEM_WATER_SCROLL, 10},
    {ITEM_AIR_SCROLL, 10},
    {ITEM_FIRE_SCROLL, 10},
    {ITEM_ESCAPE_SCROLL, 5},
};

static int GetRandomItem(void) {
    int totalWeight = 0;

    for (int i = 0; i < ITEM_COUNT - 1; i++)
        totalWeight += sChestDrops[i].rarity;

    int r = GetRandomValue(1, totalWeight);
    for (int i = 0; i < ITEM_COUNT - 1; i++) {
        r -= sChestDrops[i].rarity;

        if (r <= 0)
            return sChestDrops[i].item;
    }
    return sChestDrops[0].item; // fallback
}

static void Task_OpenChest(int taskId) {
    char buffer[52];
    int gold = 0;
    int item = 0;

    int *data = gTasks[taskId].data;
    switch (state) {
    case 0:
        sIsFieldControlLocked = true;

        gold = GetRandomValue(0, MAX_CHEST_GOLD);
        gGold += gold; 

        item = GetRandomItem();

        AddItem(item);

        sprintf(buffer, "You got %d gold\nYou got a %s", gold, gItemsInfo[item].name);
        AddTextPrinterDefault(buffer, TEXT_BOX, 4);

        PlaySound(gSounds[SOUND_CHEST_OPEN]);

        state++;
        break;
    case 1:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            sIsFieldControlLocked = false;
            StopAllTextPrinters();
            DestroyTask(taskId);
        }
        break;
    }
}

#define MIN_COINS_GOLD 4
#define MAX_COINS_GOLD 100

static void Task_PickCoins(int taskId) {
    char buffer[32];
    int gold = 0;

    int *data = gTasks[taskId].data;
    switch (state) {
    case 0:
        sIsFieldControlLocked = true;

        gold = GetRandomValue(MIN_COINS_GOLD, MAX_COINS_GOLD);
        gGold += gold; 

        sprintf(buffer, "You got %d gold\n", gold);
        AddTextPrinterDefault(buffer, TEXT_BOX, 4);

        PlaySound(gSounds[SOUND_COINS]);

        state++;
        break;
    case 1:
        if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_X)) {
            sIsFieldControlLocked = false;
            StopAllTextPrinters();
            DestroyTask(taskId);
        }
        break;
    }
}

#define PLAYER_BOUNDS (Rectangle){VIRTUAL_WIDTH/2 + TILE_SIZE/2, VIRTUAL_HEIGHT/2 + TILE_SIZE/2, TILE_SIZE, TILE_SIZE}

void CB_NewLevel(void) {
    // reset
    ResetAllSprites();
    StopAllTextPrinters();

    sInDungeon = true;

    gLevel++;

    ClearMap();

    GenerateRooms();
    GeneratePassages();

    PutSkull();
    PutBones();
    PutCoins();
    PutChests();
    PutStair();
    sPlayerPos = FindFloor();  
    
    gMainCallback = CB_HandleDungeon;
    CreateTask(Task_HandleOverworld, 0);

    CreateSprite(&sPlayerTemplate, gTextures[TEX_PLAYER], PLAYER_BOUNDS, 0);

    PlayMusic(gMusic[MUSIC_DUNGEON]);
}

void CB_LoadDungeon(void) {
    // reset
    ResetAllSprites();
    StopAllTextPrinters();
    
    gMainCallback = CB_HandleDungeon;
    CreateTask(Task_HandleOverworld, 0);

    CreateSprite(&sPlayerTemplate, gTextures[TEX_PLAYER], PLAYER_BOUNDS, 0);

    if (!sInDungeon)
        PlayMusic(gMusic[MUSIC_DUNGEON]);

    sInDungeon = true;
}

static void CB_HandleDungeon(void) {
    DrawCameraView();
    AnimateSprites();
    RunTextPrinters();
    RunTasks();
}