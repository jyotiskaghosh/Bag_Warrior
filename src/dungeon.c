#include "dungeon.h"

#include "core/task.h"
#include <raylib.h>
#include <stdlib.h>

#define MAX_ROOMS ROOM_ROWS * ROOM_COLS

typedef struct {
    int x, y, w, h; // position and size
    bool exists;     // 1 if the room exists
} Room;

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
    TILE_FLOOR
};

static void CB_HandleDungeon();
static int RandomRoom();
static Coordinates RandomPos(Room *r);
static void CarveRoom(Room r);
static void ConnectRooms(int r1, int r2);

static int sMap[MAP_H][MAP_W];
static Room sRooms[MAX_ROOMS];
static Coordinates sPlayerPos;
static Camera2D sCamera;

// Fill map with walls
static void ClearMap(void) {
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            sMap[y][x] = TILE_EMPTY;
}

static void GenerateRooms() {
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
        int cellX = (i % 3) * cellW + 1;
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

static int RandomRoom() {
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
    for (int y = r.y; y < r.y + r.h; y++) {
        for (int x = r.x; x < r.x + r.w; x++) {
            sMap[y][x] = TILE_FLOOR;
        }
    }
}

static void GeneratePassages() {
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

        distance = abs(spos.y - epos.y) - 1; // distance to move
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

        distance = abs(spos.x - epos.x) - 1;
        turn_delta.y = (spos.y < epos.y ? 1 : -1);
        turn_delta.x = 0;
        turn_distance = abs(spos.y - epos.y);
    }

    turn_spot = GetRandomValue(0, distance - 2) + 1; // where turn starts

    sMap[spos.y][spos.x] = TILE_FLOOR;
    sMap[epos.y][epos.x] = TILE_FLOOR;

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
                sMap[curr.y][curr.x] = TILE_FLOOR;
                curr.x += turn_delta.x;
	        	curr.y += turn_delta.y;
            }
        }
        sMap[curr.y][curr.x] = TILE_FLOOR;
        distance--;
    }
}

static Coordinates FindFloor() {
    Room *r = &sRooms[RandomRoom()];
    return RandomPos(r);
}

static void DrawMap() {
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++) {
            switch (sMap[y][x])
            {
            case TILE_EMPTY:
                DrawText(" ", x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, WHITE);
                break;
            case TILE_FLOOR:
                DrawText(".", x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, WHITE);
                break;
            default:
                break;
            }
        }
}

static void DrawCameraView() {
    sCamera.target = (Vector2){sPlayerPos.x * TILE_SIZE, sPlayerPos.y * TILE_SIZE};
    sCamera.offset = (Vector2){ VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2};  // screen center (half screen width/height)
    sCamera.rotation = 0.0f;
    sCamera.zoom = 1.0f;

    BeginMode2D(sCamera);
        DrawMap();
        DrawText("@", sPlayerPos.x * TILE_SIZE, sPlayerPos.y * TILE_SIZE, TILE_SIZE, WHITE);
    EndMode2D();
}

static void Task_HandleOverworld(int taskId) {
    if (IsKeyPressed(KEY_UP)) {
        if (sPlayerPos.y - 1 >= 0 && sMap[sPlayerPos.y - 1][sPlayerPos.x] == TILE_FLOOR) {
            sPlayerPos.y--;
        }
    }

    if (IsKeyPressed(KEY_DOWN)) {
        if (sPlayerPos.y + 1 < MAP_H && sMap[sPlayerPos.y + 1][sPlayerPos.x] == TILE_FLOOR) {
            sPlayerPos.y++;
        }
    } 

    if (IsKeyPressed(KEY_LEFT)) {
        if (sPlayerPos.x - 1 >= 0 && sMap[sPlayerPos.y][sPlayerPos.x - 1] == TILE_FLOOR) {
            sPlayerPos.x--;
        }
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        if (sPlayerPos.x + 1 < MAP_W && sMap[sPlayerPos.y][sPlayerPos.x + 1] == TILE_FLOOR) {
            sPlayerPos.x++;
        }
    }
}

void CB_LoadDungeon() {
    ClearMap();
    GenerateRooms();
    GeneratePassages();

    sPlayerPos = FindFloor();
    
    gMainCallback = CB_HandleDungeon;
    CreateTask(Task_HandleOverworld, 0);
}

static void CB_HandleDungeon() {
    DrawCameraView();
    RunTasks();
}