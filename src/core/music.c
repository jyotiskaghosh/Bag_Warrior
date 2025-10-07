#include "core/music.h"

Music gCurrentMusic;

void PlayMusic(Music music) {
    StopMusicStream(gCurrentMusic);
    gCurrentMusic = music;
    PlayMusicStream(music);
}

void StopMusic() {
    StopMusicStream(gCurrentMusic);
}