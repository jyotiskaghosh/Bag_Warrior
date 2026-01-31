#include "main.h"
#include "core/music.h"
#include "core/fade.h"
#include "start_screen.h"
#include "constants/textures.h"
#include "constants/audio.h"

void (*gMainCallback)(void);
Texture2D gTextures[TEX_COUNT];
Music gMusic[MUSIC_COUNT];
Sound gSounds[SOUND_COUNT];

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
	
	// move textures
	gTextures[TEX_THROW_KNIFE_MOVE] = LoadTexture("graphics/ThrowKnifeAttack.png");
	gTextures[TEX_BOMB_MOVE] = LoadTexture("graphics/BombAttack.png");
	gTextures[TEX_EARTH_SPELL_MOVE] = LoadTexture("graphics/EarthSpellAttack.png");
	gTextures[TEX_AIR_SPELL_MOVE] = LoadTexture("graphics/AirSpellAttack.png");
	gTextures[TEX_FIRE_SPELL_MOVE] = LoadTexture("graphics/FireSpellAttack.png");
	gTextures[TEX_WATER_SPELL_MOVE] = LoadTexture("graphics/WaterSpellAttack.png");

	// dungeon tileset
	gTextures[TEX_DUNGEON_TILESET] = LoadTexture("graphics/DungeonTileset.png");

	// player
	gTextures[TEX_PLAYER] = LoadTexture("graphics/Player.png");

	// cursor
	gTextures[TEX_CURSOR] = LoadTexture("graphics/Cursor.png");

	// battle background
	gTextures[TEX_BACKGROUND] = LoadTexture("graphics/DungeonBackground.png");

	// *************** Audio *****************

	// music
	gMusic[MUSIC_BATTLE] = LoadMusicStream("audio/WATCH OUT.mp3");
	gMusic[MUSIC_DUNGEON] = LoadMusicStream("audio/cave diving.mp3");

	// sounds
	gSounds[SOUND_CHEST_OPEN] = LoadSound("audio/771164__steprock__treasure-chest-open.mp3");
	SetSoundVolume(gSounds[SOUND_CHEST_OPEN], 5.0);

	gSounds[SOUND_COINS] = LoadSound("audio/drop-coin-384921.mp3");
	gSounds[SOUND_SELECT] = LoadSound("audio/select-sound-121244.mp3");
	
	gSounds[SOUND_FADE] = LoadSound("audio/swoosh-1-376872.mp3");
}

static void UnloadResources(void) {
	for (int i = 0; i < TEX_COUNT; i++)
		UnloadTexture(gTextures[i]);

	for (int i = 0; i < MUSIC_COUNT; i++)
		UnloadMusicStream(gMusic[i]);

	for (int i = 0; i < SOUND_COUNT; i++)
		UnloadSound(gSounds[i]);
}