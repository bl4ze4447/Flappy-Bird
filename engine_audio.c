//
// Created by Antonie Gabriel Belu on 12.02.2025.
//

#include "engine_audio.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

static Sound jump_sound, score_sound, death_sound;

void init_audio_engine(const float volume) {
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        perror("Audio device not ready");
        exit(1);
    }
    SetMasterVolume(volume);
    load_game_sounds();
}
void load_game_sounds(void) {
    jump_sound = LoadSound("jump.mp3");
    score_sound = LoadSound("score.mp3");
    death_sound = LoadSound("death.mp3");
}
void unload_game_sounds(void) {
    UnloadSound(jump_sound);
    UnloadSound(score_sound);
    UnloadSound(death_sound);
}
void audio_engine_cleanup(void) {
    unload_game_sounds();
    CloseAudioDevice();
}

void play_sound(const enum Sounds sound) {
    switch (sound) {
        case JUMP:
            PlaySound(jump_sound);
            break;
        case SCORE:
            PlaySound(score_sound);
            break;
        case DEATH:
            PlaySound(death_sound);
            break;
    }
}
