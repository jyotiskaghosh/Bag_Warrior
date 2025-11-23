#include "main.h"
#include "core/music.h"
#include "core/fade.h"
#include "start_screen.h"
#include "constants/textures.h"
#include "constants/audio.h"

void (*gMainCallback)(void);
Texture2D gTextures[TEX_COUNT];
Music gMusic[MUSIC_COUNT];

static void LoadResources(void);
static void UnloadResources(void);

int main(void) {
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Bag Warrior");

	InitAudioDevice();

	LoadResources();

	RenderTexture2D virtualScreen = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

	gMainCallback = CB_InitStartScreen;

	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginTextureMode(virtualScreen);
			ClearBackground(BLACK);
			gMainCallback();
			RunFade();
		EndTextureMode();

		UpdateMusicStream(gCurrentMusic);

		BeginDrawing();
			ClearBackground(BLACK);

			float scaleX = (float) WINDOW_WIDTH / VIRTUAL_WIDTH;
			float scaleY = (float) WINDOW_HEIGHT / VIRTUAL_HEIGHT;
			DrawTexturePro(
				virtualScreen.texture,
				(Rectangle){0, 0, VIRTUAL_WIDTH, -VIRTUAL_HEIGHT},
				(Rectangle){0, 0, VIRTUAL_WIDTH * scaleX, VIRTUAL_HEIGHT * scaleY},
				(Vector2){0, 0},
				0,
				WHITE	
			);
		EndDrawing();
	}

	UnloadResources();

	CloseAudioDevice();

	CloseWindow();
}

static void LoadResources(void) {
	// monster textures
	gTextures[TEX_WOLF] = LoadTexture("graphics/Wolf.png");
	gTextures[TEX_BAT] = LoadTexture("graphics/Bat.png");
	gTextures[TEX_EARTH_MAGE] = LoadTexture("graphics/EarthMage.png");
	gTextures[TEX_AIR_MAGE] = LoadTexture("graphics/AirMage.png");
	gTextures[TEX_WATER_MAGE] = LoadTexture("graphics/WaterMage.png");
	gTextures[TEX_FIRE_MAGE] = LoadTexture("graphics/FireMage.png");
	gTextures[TEX_DRAGON] = LoadTexture("graphics/Dragon.png");

	// dungeon tileset
	gTextures[TEX_DUNGEON_TILESET] = LoadTexture("graphics/DungeonTileset.png");

	// player
	gTextures[TEX_PLAYER] = LoadTexture("graphics/Player.png");

	//cursor
	gTextures[TEX_CURSOR] = LoadTexture("graphics/Cursor.png");

	// *************** Audio *****************
	gMusic[MUSIC_DUNGEON] = LoadMusicStream("audio/827900__expiredsoda__treasure-island-8-bit-adventure-loop-var1.wav");
	gMusic[MUSIC_BATTLE] = LoadMusicStream("audio/647908__sonically_sound__short-loop-made-in-a-few-minutes-with-qws-and-goldwave.wav");
}

static void UnloadResources(void) {
	for (int i = 0; i < TEX_COUNT; i++)
		UnloadTexture(gTextures[i]);

	for (int i = 0; i < MUSIC_COUNT; i++)
		UnloadMusicStream(gMusic[i]);
}