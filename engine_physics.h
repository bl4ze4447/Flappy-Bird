//
// Created by Antonie Gabriel Belu on 12.02.2025.
//

#ifndef ENGINE_PHYSICS_H
#define ENGINE_PHYSICS_H

#include <raylib.h>
#include "engine_types.h"

void update_player(struct Player *, struct GameInfo *);
void update_pipe(struct Pipe *, int, struct GameInfo *);
void update_pipes(struct Pipes *, struct GameInfo *);
void apply_gravity(struct Player *, const struct GameInfo *);
bool has_collided(const struct Player *, const struct Pipe *);
bool check_collisions(const struct Player *, const struct Pipes *);
void handle_smooth_jump(struct Player *);
void simulate_jump(struct Player *, bool);

#endif //ENGINE_PHYSICS_H
