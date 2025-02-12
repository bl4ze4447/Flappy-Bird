//
// Created by Antonie Gabriel Belu on 12.02.2025.
//

#ifndef ENGINE_DRAW_H
#define ENGINE_DRAW_H

#include <raylib.h>
#include "engine_types.h"


void init_draw_engine(struct Player *);
void draw_background(void);
void draw_player(const struct Player *);
void draw_pipe(const struct Pipe *);
void draw_pipes(const struct Pipes *);
void draw_score(int);
void draw_frame(const struct Player *, const struct Pipes *, const struct GameInfo *);

void load_player_texture(struct Player *);
void unload_player_texture(const struct Player *);
void load_background_texture(void);
void unload_background_texture(void);
void load_pipe_textures(void);
void unload_pipe_textures(void);
void draw_engine_cleanup(const struct Player *);

#endif //ENGINE_DRAW_H
