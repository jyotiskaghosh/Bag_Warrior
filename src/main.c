#include "main.h"
#include "core/music.h"
#include "start_screen.h"
#include "constants/textures.h"

void (*gMainCallback)(void);
Texture2D gTextures[TEXTURE_COUNT];

static void LoadTextures(void);
static void UnloadTextures(void);

int main(void) {
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Bag Warrior");

	InitAudioDevice();

	LoadTextures();

	RenderTexture2D virtualScreen = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

	gMainCallback = CB_InitStartScreen;

	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginTextureMode(virtualScreen);
			ClearBackground(BLACK);
			gMainCallback();
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

	UnloadTextures();

	CloseAudioDevice();

	CloseWindow();
}

static void LoadTextures(void) {
	gTextures[TEXTURE_WOLF] = LoadTexture("graphics/Wolf.png");
	gTextures[TEXTURE_BAT] = LoadTexture("graphics/Bat.png");
	gTextures[TEXTURE_EARTH_MAGE] = LoadTexture("graphics/EarthMage.png");
	gTextures[TEXTURE_AIR_MAGE] = LoadTexture("graphics/AirMage.png");
	gTextures[TEXTURE_WATER_MAGE] = LoadTexture("graphics/WaterMage.png");
	gTextures[TEXTURE_FIRE_MAGE] = LoadTexture("graphics/FireMage.png");
	gTextures[TEXTURE_DRAGON] = LoadTexture("graphics/Dragon.png");
}

static void UnloadTextures(void) {
	for (int i = 0; i < TEXTURE_COUNT; i++)
		UnloadTexture(gTextures[i]);
}