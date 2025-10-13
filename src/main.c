#include <raylib.h>
#include "main.h"
#include "core/music.h"
#include "battle_main.h"
#include "dungeon.h"

void (*gMainCallback)(void);

int main() {
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Bag Warrior");

	InitAudioDevice();

	RenderTexture2D virtualScreen = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

	gMainCallback = CB_LoadDungeon;

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

	CloseAudioDevice();

	CloseWindow();
}
