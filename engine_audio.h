//
// Created by Antonie Gabriel Belu on 12.02.2025.
//

#ifndef ENGINE_AUDIO_H
#define ENGINE_AUDIO_H

enum Sounds {
    JUMP,
    SCORE,
    DEATH,
};

void init_audio_engine(float);
void load_game_sounds(void);
void unload_game_sounds(void);
void audio_engine_cleanup(void);
void play_sound(enum Sounds);

#endif //ENGINE_AUDIO_H
