//
// Created by Antonie Gabriel Belu on 12.02.2025.
//

#ifndef ENGINE_INIT_H
#define ENGINE_INIT_H
#include "engine_types.h"

void init_pipe(struct Pipe *, int);
void init_pipes(struct Pipes *);
void init_player(struct Player *);
void init_game_info(struct GameInfo *);

#endif //ENGINE_INIT_H
